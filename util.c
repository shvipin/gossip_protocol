#include "util.h"


char *encode(int nodes[])
{
  int i;
  int numNodes = sizeof(nodes) / sizeof(int);
  char *message = (char *)malloc(sizeof(neighbor_t) * numNodes + ENCODE_PADDING);

  strcpy(message, ENCODE_HEADER);
  for (i = 0; i < numNodes; i++) {
    strcat(message, (char *)neighbors[nodes[i]];
  }

  return message;
}

void decode(char *message)
{
  debug("Start decoding message %s", message);
  int i;
  neighbor_t *r_neighbors = (neighbor_t *) message;
  int numNeighbors = strlen(message) / sizeof(neighbor_t);

  for (i = 0; i < numNeighbors; i++) {
    int index = r_neighbors[i].index;
    if (r_neighbors[i].heartbeat > neighbors[index].heartbeat) {
      debug("r_neighbors %d is more recent", index);
      neighbors[index].heartbeat = r_neighbors[i].heartbeat;
      time(&neighbors[index].localtime); 
    }
  } 
}
