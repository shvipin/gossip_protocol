#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <string.h>
#include <time.h>

#define ENDPOINTS "endpoints"
#define NUM_THREADS 2
#define TRUE 1
#define FALSE 0
#define BUFFER_LENGTH 256
#define ENCODE_HEADER "NL_"
#define ENCODE_PADDING 10

// user provided arguments
extern int num_nodes;
extern int gossip_b;
extern int gossip_c;
extern int time_to_failure;
extern int num_failure_nodes; // B
extern int time_bw_failures;  // P
extern int random_seed;

extern nodeInfo *endpoints;
extern nodeInfo *self;

extern FILE *ep;
extern pthread_barrier_t barrier;
extern int last_process;
extern int self_id;
extern long life_time;

typedef struct neighbor {
  int index;
  long heartbeat;
  time_t localtime;
} neighbor_t;

char *encode(int nodes[]);
void decode(char *message);
#endif // __UTIL_H__
