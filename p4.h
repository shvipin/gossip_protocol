#ifndef __P4_H__
#define __P4_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "debug.h"
#include "netif.h"

#define ENDPOINTS "endpoints"
#define NUM_THREADS 2
#define TRUE 1
#define FALSE 0
#define BUFFER_LENGTH 65536
//#define ENCODE_HEADER "NL_"
//#define ENCODE_PADDING 10

// user provided arguments
typedef struct gossip_args {
  int num_nodes;
  int gossip_b;
  int gossip_c;
  int time_to_failure;
  int num_failure_nodes;
  int time_bw_failures;
  int random_seed;
  int time_to_run;
} gossip_args_t;

typedef struct neighbor {
  int index;
  long heartbeat;
  time_t localtime;
} neighbor_t;

typedef struct self {
  FILE *endpoints_fp;
  int id;
  int last_process;
  int life_time;
  int alive;
  unsigned int neighbor_seed;
  unsigned int killer_seed;
  neighbor_t *neighbors;
  nodeInfo *self;
  pthread_barrier_t barrier;
  pthread_mutex_t lock;
} self_t;

extern gossip_args_t args;
extern self_t me;

#endif
