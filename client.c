// Single Author info: 
// vsharma5 Vipin Sharma 
// Group info: 
// skukret Sarthak Kukreti 
// vsharma5 Vipin Sharma 
// djzager David J Zager 

#include <unistd.h>
#include "client.h"
#include <stdlib.h>

/**
 * Debug function to dump a single nodes information to stdout
 */
void dump_node_info(neighbor_t node)
{
  debug("#############################");
  debug("INDEX: %d",node.index);
  debug("HEARTBEAT: %d",node.heartbeat);
  debug("LOCALTIME: %d",node.localtime);
  debug("#############################");
}

/**
 * Debug function to dump the info of all nodes in system to stdout
 */
void dump_my_info()
{
  int i;
  debug("My id %d",me.id);
  for (i = 0; i < args.gossip_b; i++) {
     debug("Neighbor %d",send_to[i]);
  }

  for (i = 0; i < args.num_nodes; i++) {
    dump_node_info(me.neighbors[i]);
  }
}

/**
 * Dump my node info and neighbor list to listX where X is my id.
 */
void dump_nodes()
{
  debug("Start dumping nodes to file");
  int i, j;
  char filename[FILENAME_SIZE];

  sprintf(filename, "%s%d", NODE_DUMP, me.id);
  FILE *list_fp = fopen(filename , "w");

  char ok_fail[] = "FAIL ";
  if (me.alive) {
    strcpy(ok_fail, "OK ");
  }
  
  fprintf(list_fp, "%s\n", ok_fail);
  j = 2;
  for (i = 0; i < args.num_nodes; i++) {
    if (i != me.id) {
      fprintf(list_fp, "%d %d\n", i, (int) me.neighbors[i].heartbeat);
    }
  }
  debug("Finished dumping nodes");
}

/**
 * Send message (message) to a specified neighbor
 */
void send_socket_msg(int socket_id, int neighbor_index, const char *msg, int msg_len)
{
  sendto(socket_id, msg, msg_len, 0, &endpoints[neighbor_index].addrInfo, sizeof(struct sockaddr_in));
}

/**
 * Randomly get neighbors and store the information in an integer array
 */
int choose_random_neighbors(int *neighbors)
{
  int i;
  int found_neighbors = 0;
  int temp = 0;
  int dead_nodes = 0;
  int *prev_seen = (int *) malloc(sizeof(int) * args.num_nodes);
  memset(prev_seen, 0, args.num_nodes * sizeof(int));

  // don't maintain too much history
  memset(neighbors, -1, args.gossip_b * sizeof(int));

  while (found_neighbors < args.gossip_b && found_neighbors + dead_nodes < args.num_nodes - 1) {

    temp = rand_r(&me.neighbor_seed) % args.num_nodes;
    if(prev_seen[temp] == 1) continue;

    prev_seen[temp] = 1;

    if (temp == me.id) continue;

    pthread_mutex_lock(&me.lock);
    if(me.neighbors[temp].index == STATE_DEAD){
      dead_nodes++;
      pthread_mutex_unlock(&me.lock);
      continue;
    }
    pthread_mutex_unlock(&me.lock);

    neighbors[found_neighbors++] = temp;
  }
  return found_neighbors;
}

void send_heartbeats(int *send_to, int send_to_count)
{
  int i;
  char *msg = encode(&me.id, 1);
  debug("encoded message : %s", msg);
  int len = strlen(msg);
  for (i = 0; i < send_to_count; i++) {
    send_socket_msg(me.self->socket,send_to[i],msg,len);
  }
  free(msg);
}

/**
 * Sends whole known live neighbor list entries to predetermined me.neighbors in send_to
 */
void send_nl(int *send_to, int send_to_count)
{
  int *neighbor_list = (int*) malloc(sizeof(int)*args.num_nodes);
  int i ;
  int selected_count = 0;
  char *msg;

  pthread_mutex_lock(&me.lock);
  for(i = 0; i < args.num_nodes; i++){
    //select neighbors which are not dead or not unknown.
    if(me.neighbors[i].index >= STATE_LIVE)
      neighbor_list[selected_count++] = me.neighbors[i].index;
  }
  pthread_mutex_unlock(&me.lock);

  msg = encode(neighbor_list, selected_count);
  for (i = 0; i < send_to_count; i++) {
    // Encode neighbor list into a string and send
    debug("Neighbor List: Encoded Message: %s", msg);
    send_socket_msg(me.self->socket, send_to[i], msg, strlen(msg));
  }
  free(msg);
  free(neighbor_list);
  return;
}

/**
 * Initialize client with all the information we'll ever need
 */
void client_init()
{
  //allocate space for storing index of me.neighbors to send heartbeats
  send_to = (int *)malloc(args.gossip_b * sizeof(int));

  // allocate memory for our endpoints
  endpoints = (nodeInfo *)malloc(args.num_nodes * sizeof(nodeInfo));
  int i;
  int numNodes = 0;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  // get all nodes from the file
  me.endpoints_fp = fopen(ENDPOINTS, "r");
  if (me.endpoints_fp == NULL) {
    log_err("Couldn't open endpoints file for reading");
  }

  while ((read = getline(&line, &len, me.endpoints_fp)) != -1) {
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
  fclose(me.endpoints_fp);
  if (me.last_process) {
    debug("This is the last process");
    // send message to all
    for (i = 0; i < args.num_nodes - 1; i++) {
      debug("Sending \"OK\" message to %s", getNodeInfo(&endpoints[i]));
      send_socket_msg(me.self->socket,i,"OK",3);
      //sendto(me.self->socket, "OK", 3, 0, &endpoints[i].addrInfo, sizeof(struct sockaddr_in));
    }
  } else {
    debug("I'm not the last process");
  }
  free(line);
  me.neighbor_seed = args.random_seed + me.id;
  me.neighbors[me.id].index = me.id;
  choose_random_neighbors(send_to);
}

/**
 * Cleanup after we have finished executing, don't forget to dump the neighbor list
 * before we exit
 */
void client_cleanup()
{
  debug("B(P+1)= %d time over",me.life_time);
  dump_my_info();
  dump_nodes();
  free(endpoints);
  free(send_to);
}

/**
 * This is the heart of the client, where control of when messages are sent lives.
 */
void client()
{
  debug("Client function started...");

  client_init();
  long time_counter = 0;
  int *send_nl_list = (int *)malloc(sizeof(int) * args.gossip_b);
  int b = args.gossip_b;

  while (time_counter < args.time_to_run) {

    sleep(1);
    time_counter++;

    if (!me.alive) {
      debug("I am dead");
      continue;
    }
    // sendheartbeats to me.neighbors
    send_heartbeats(send_to,args.gossip_b);
    me.neighbors[me.id].heartbeat++;
    // send randomly chosen b neighbour list entries to b neighbours every c iterations
    if (time_counter < args.gossip_c) {
      b = choose_random_neighbors(send_nl_list);
      send_nl(send_nl_list,b);
    }

    check_dead_neighbors();

    if(time_counter%args.time_bw_failures == 0 && me.current_dead_count < args.num_failure_nodes)
      check_me_dead();
  }
  free(send_nl_list);
  client_cleanup();
}

/**
 * Go through our neighbor list and see if anyone has died while we were doing other things
 */
void check_dead_neighbors(){
  int i = 0;
  time_t current_time;
  time(&current_time);

  pthread_mutex_lock(&me.lock);
  for(i = 0; i < args.num_nodes; i++){

    if(me.neighbors[i].index >= STATE_LIVE && me.neighbors[i].index != me.id){
      if(current_time - me.neighbors[i].localtime >= args.time_to_failure){
        me.neighbors[i].index = STATE_DEAD;
      }
    }
  }
  pthread_mutex_unlock(&me.lock);
}

/**
 * See if I am the node to die.
 */
void check_me_dead(){
  int temp = 0;

  while(TRUE){
    temp = rand_r(&me.killer_seed)%args.num_nodes;

    if(me.killed_history[temp] == 0) break;
  }

  me.killed_history[temp] = 1;
  me.current_dead_count++;

  if(temp == me.id){
    me.alive = FALSE;
    me.neighbors[me.id].index = STATE_DEAD;
  }
}

/**
 * Encode the neighborlist as a string
 */
char *encode(int *nodes, int numNodes)
{
  int i;
  char *message = (char *)malloc(BUFFER_LENGTH);
  char num_str[15] = "";
  message[0]='\0';
  memset(message,'\0',BUFFER_LENGTH);
  sprintf(num_str,"%d",numNodes);
  strcat(message,num_str);
  debug("******************* %s",message);
  pthread_mutex_lock(&me.lock);
  for (i = 0; i < numNodes; i++) {
    strcat(message," ");

    sprintf(num_str,"%d",nodes[i]);
    strcat(message,num_str);

    strcat(message," ");

    sprintf(num_str,"%d",me.neighbors[nodes[i]].heartbeat);
    strcat(message,num_str);
  }
  pthread_mutex_unlock(&me.lock);

  debug("******************* %s",message);
  return message;
}

