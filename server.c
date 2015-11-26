#include "p4.h"
#include "server.h"

void decode(char *message);
void server_listen(char *message);

void *server(void *token)
{
  me.self = setupNode();
  char message[BUFFER_LENGTH];
  int numNodes = 0;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  do {
    //sleep(3);
    me.endpoints_fp = fopen(ENDPOINTS, "a+");
  } while (me.endpoints_fp == NULL);
  
  while ((read = getline(&line, &len, me.endpoints_fp)) != -1) {
    line[read - 1] = '\0';
    read--;
    numNodes++;
    debug("Read %s from endpoints file", line);
  }
  me.id = numNodes;
  debug("Done reading endpoints file, we have id: %d", me.id);

  if (args.num_nodes - 1 == numNodes) {
    debug("We are the last process");
    me.last_process = TRUE;
  }
  fprintf(me.endpoints_fp, "%s\n", getNodeInfo(me.self));
  debug("Node info written to endpoints file");
  fclose(me.endpoints_fp);

  if (!me.last_process) {
    // wait for OK message
    debug("Waiting for \"OK\" message");
    if (recvfrom(me.self->socket, message, BUFFER_LENGTH, 0, NULL, NULL) == -1) {
      log_err("Failed to receive message");
    }
    debug("Received message %s", message);
    if (strcmp(message, "OK") == 0) {
      debug("Got the \"OK\"");
    }
  } 

  pthread_barrier_wait(&me.barrier);
  free(line);
  server_listen(message);
}

void decode(char *message)
{
  debug("Start decoding message %s", message);
  int i;
  char *decoded_msg = strtok(message," ");

  int numNodes = atoi(decoded_msg);
  
  pthread_mutex_lock(&me.lock);
  for (i = 0; i < numNodes; i++) {

    int index = atoi(strtok(NULL," "));
    int heartbeat = atoi(strtok(NULL," "));
    if (me.neighbors[index].index != STATE_DEAD && me.neighbors[index].index != me.id && heartbeat > me.neighbors[index].heartbeat) {
      debug("r_neighbors %d is more recent", index);
      me.neighbors[index].index = index;
      me.neighbors[index].heartbeat = heartbeat;
      time(&me.neighbors[index].localtime); 
    }
  } 
  pthread_mutex_unlock(&me.lock);
}

void server_listen(char *message)
{
  while (TRUE) {
    memset(message,'\0',BUFFER_LENGTH);
    if (recvfrom(me.self->socket, message, BUFFER_LENGTH, 0, NULL, NULL) == -1) {
      log_err("Failed to receive message");
    }

    debug("Received message %s", message);

    if (!me.alive) {
      debug("I am dead");
      continue;
    }
    decode(message); 
  }
}
