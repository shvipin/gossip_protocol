#include <pthread.h>
#include "server.h"
#include "netif.h"
#include "debug.h"
#include "util.h"

void *server(void *token)
{
  self = setupNode();
  char message[BUFFER_LENGTH];
  int numNodes = 0;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  do {
    //sleep(3);
    ep = fopen(ENDPOINTS, "a+");
  } while (ep == NULL);
  
  while ((read = getline(&line, &len, ep)) != -1) {
    line[read - 1] = '\0';
    read--;
    numNodes++;
    debug("Read %s from endpoints file", line);
  }
  self_id = numNodes;
  debug("Done reading endpoints file, we have id: %d", self_id);

  if (num_nodes - 1 == numNodes) {
    debug("We are the last process");
    last_process = TRUE;
  }
  fprintf(ep, "%s\n", getNodeInfo(self));
  debug("Node info written to endpoints file");
  fclose(ep);

  if (!last_process) {
    // wait for OK message
    debug("Waiting for \"OK\" message");
    if (recvfrom(self->socket, message, BUFFER_LENGTH, 0, NULL, NULL) == -1) {
      log_err("Failed to receive message");
    }
    debug("Received message %s", message);
    if (strcmp(message, "OK") == 0) {
      debug("Got the \"OK\"");
    }
  } 

  pthread_barrier_wait(&barrier);
  free(line);
}
