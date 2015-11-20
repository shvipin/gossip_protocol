#include<string.h>
#include "util.h"



char *encode(int *nodes,int numNodes)
{
  int i;
  char *message = (char *)malloc(sizeof(neighbor_t) * numNodes + ENCODE_PADDING);

  strcpy(message, ENCODE_HEADER);
  for (i = 0; i < numNodes; i++) {
    strncat(message, (char *)&neighbors[nodes[i]],sizeof(neighbor_t));
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

void dump_node_info(neighbor_t node)
{
  debug("#############################");
  debug("INDEX: %d",node.index);
  debug("HEARTBEAT: %d",node.heartbeat);
  debug("LOCALTIME: %d",node.localtime);
  debug("#############################");
}
void dump_my_info()
{
  int i;
  debug("My id %d",self_id);
  
  for(i=0;i<num_nodes;i++)
  {
    dump_node_info(neighbors[i]); 
  }
}

