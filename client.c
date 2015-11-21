#include<unistd.h>
#include<stdlib.h>
#include "client.h"
#include "netif.h"
#include "debug.h"
#include "util.h"

void send_socket_msg(int socket_id, int neighbor_index, const char *msg, int msg_len)
{
  
  sendto(socket_id, msg, msg_len, 0, &endpoints[neighbor_index].addrInfo, sizeof(struct sockaddr_in));

}

void choose_random_neighbors(unsigned int seed)
{
  int found_neighbors=0;
  int temp=0;
  srandom(seed);
        
  while(found_neighbors < gossip_b)
  {
    temp = random()%num_nodes;
    if(temp == self_id)
      continue;
    if(neighbors[temp].index<0)
    {
      neighbors[temp].index = temp;
      send_to[found_neighbors]=temp;
      found_neighbors++;
    } 
  }
  
}

void send_heartbeats(int *send_to, int send_to_count)
{
  int i;
  char *msg = encode(&self_id, 1); 
  debug("encoded message : %s",msg);
  int len = strlen(msg);
  for(i=0;i<send_to_count;i++)
  {
    send_socket_msg(self->socket,send_to[i],msg,len);
  }
  free(msg);
}

// Sends randomly chosen b neighbor entries to predetermined neighbors in send_to
void send_nl(int *send_to, int send_to_count)
{
  int *neighbor_list = (int*) malloc(sizeof(int)*gossip_b);
  int i, j, flag = 0;;
  int tmp;
  int selected_count = 0;
  char *msg;
  for(i=0;i<send_to_count;i++)
  {
    while(selected_count < gossip_b)
    {
      flag = 0;
      tmp = rand() % num_nodes;
      // Neighbor should exclude self and target node (and nodes previously selected)
      for(j=0; j<selected_count; j++)
      {
        if(tmp == neighbor_list[j])
        {
          flag = 1;
          break;
        }
      }
      if(flag == 1)
        continue;
      if(tmp != send_to[i] && tmp != self_id)
      {
        neighbor_list[selected_count] = tmp;
        selected_count++;
      }
    }
    // Encode neighbor list into a string and send
    msg = encode(neighbor_list, selected_count);
    debug("Neighbor List: Encoded Message: %s", msg);
    send_socket_msg(self->socket, send_to[i], msg, strlen(msg));
    selected_count = 0;
    free(msg);
  }
  free(neighbor_list);
  return;
}

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
      send_socket_msg(self->socket,i,"OK",3);
      //sendto(self->socket, "OK", 3, 0, &endpoints[i].addrInfo, sizeof(struct sockaddr_in));
    }
  } else {
    debug("I'm not the last process");
  }
  free(line);
  choose_random_neighbors(random_seed + self_id);
  neighbors[self_id].index = self_id;

}

void client_cleanup()
{
  debug("B(P+1)= %d time over",life_time);  
  dump_my_info();
}

void client()
{
  debug("Client function started...");
  client_init();
  long time_counter = 0;
  while (time_counter < life_time) {

    // sendheartbeats to neighbors
    send_heartbeats(send_to,gossip_b);
    neighbors[self_id].heartbeat++;
    // send randomly chosen b neighbour list entries to b neighbours every c iterations 
    if (time_counter % gossip_c == 0)
      send_nl(send_to, gossip_b);
    sleep(1);
    time_counter++;
  }
  client_cleanup();
}
