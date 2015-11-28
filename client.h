// Single Author info: 
// vsharma5 Vipin Sharma 
// Group info: 
// skukret Sarthak Kukreti 
// vsharma5 Vipin Sharma 
// djzager David J Zager 

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "p4.h"

nodeInfo *endpoints;
int *send_to;

void dump_node_info(neighbor_t node);
void dump_my_info();
void dump_nodes();
void send_socket_msg(int socket_id, int neighbor_index, const char *msg, int msg_len);
int choose_random_neighbors(int *neighbors);
void send_heartbeats(int *send_to, int send_to_count);
void send_nl(int *send_to, int send_to_count);
void client_init();
void client_cleanup();
void client(void);
void check_dead_neighbors();
void check_me_dead();
char *encode(int *nodes,int length);

#endif //__CLIENT_H__
