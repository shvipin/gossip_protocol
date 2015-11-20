#include<string.h>
#include "util.h"



char *encode(int *nodes,int numNodes)
{
  int i;
  char *message = (char *)malloc(BUFFER_LENGTH);
  char num_str[15] = "";
  message[0]='\0';
  sprintf(num_str,"%d",numNodes);
  strcat(message,num_str);
  debug("******************* %s",message);
  for (i = 0; i < numNodes; i++) {
    strcat(message," ");
    
    sprintf(num_str,"%d",nodes[i]);
    strcat(message,num_str);

    strcat(message," ");
    
    sprintf(num_str,"%d",neighbors[nodes[i]].heartbeat);
    strcat(message,num_str);
  }

  debug("******************* %s",message);
  return message;
}

void decode(char *message)
{
  debug("Start decoding message %s", message);
  int i;
  char *decoded_msg = strtok(message," ");

  int numNodes = atoi(decoded_msg);
  
  for (i = 0; i < numNodes; i++) {

    int index = atoi(strtok(NULL," "));
    int heartbeat = atoi(strtok(NULL," "));

    if (heartbeat > neighbors[index].heartbeat) {
      debug("r_neighbors %d is more recent", index);
      neighbors[index].index = index;
      neighbors[index].heartbeat = heartbeat;
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
  for(i=0;i<gossip_b;i++)
     debug("Neighbor %d",send_to[i]);

  for(i=0;i<num_nodes;i++)
  {
    dump_node_info(neighbors[i]); 
  }
}

