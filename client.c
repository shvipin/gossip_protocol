#include<unistd.h>
#include "client.h"
#include "netif.h"
#include "debug.h"
#include "util.h"

void client_init()
{
  int i;
  int numNodes = 0;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  
  // get all nodes from the file
  ep = fopen(ENDPOINTS, "r");
  if (ep == NULL) {
    log_err("Couldn't open endpoints file for reading");
  }

  while ((read = getline(&line, &len, ep)) != -1) {
    line[read - 1] = '\0';
    read--;
    char *port = splitAddr(line);
    debug("We have %s as IP and %s as port", line, port);
    memset((char *) &endpoints[numNodes].addrInfo, 0, sizeof(struct sockaddr_in));
    endpoints[numNodes].addrInfo.sin_family = AF_INET;
    endpoints[numNodes].addrInfo.sin_port = atoi(port);
    if (inet_aton(line, &endpoints[numNodes].addrInfo.sin_addr)==0) {
      log_err("inet_aton() failed");
      exit(1);
    }
    numNodes++; 
  }
  debug("Done reading endpoints file");
  fclose(ep);
  if (last_process) {
    debug("This is the last process");
    // send message to all
    for (i = 0; i < num_nodes - 1; i++) {
      debug("Sending \"OK\" message to %s", getNodeInfo(&endpoints[i]));
      sendto(self->socket, "OK", 3, 0, &endpoints[i].addrInfo, sizeof(struct sockaddr_in));
    }
  } else {
    debug("I'm not the last process");
  }
  free(line);
}

void client_cleanup()
{
  debug("B(P+1)= %d time over",life_time);  
}

void client()
{
  debug("Client function started...");
  client_init();
  long time_counter = 0;
  while (time_counter < life_time) {
    sleep(1);
    time_counter++;
  }
  client_cleanup();
}
