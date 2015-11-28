// Single Author info: 
// skukret Sarthak Kukreti 
// Group info: 
// skukret Sarthak Kukreti 
// vsharma5 Vipin Sharma 
// djzager David J Zager 

#include "p4.h"
#include "server.h"
#include "client.h"

gossip_args_t args;
self_t me;

/**
 * Start the execution of the program, set important global
 * values and dispatch server thread and client.
 */
int main(int argc, const char *argv[])
{
  pthread_t server_thread;
  int i;

  if (argc != 9) {
    // user messed up. Print usage info and exit
    printf("Usage ./p4 N b c F B P S\n \
      number of peer nodes N\n \
      gossip parameter b\n \
      gossip parameter c\n \
      number of seconds after which a node is considered dead F\n \
      number of bad nodes that should fail B\n \
      number of seconds to wait between failures P\n \
      the see of the random number generator S\n \
      the total number of seconds to run T\n");
    exit(1);
  }
  // take in user's arguments
  args.num_nodes = atoi(argv[1]);
  args.gossip_b = atoi(argv[2]);
  args.gossip_c = atoi(argv[3]);
  args.time_to_failure = atoi(argv[4]);
  args.num_failure_nodes = atoi(argv[5]);
  args.time_bw_failures = atoi(argv[6]);
  args.random_seed = atoi(argv[7]);
  args.time_to_run = atoi(argv[8]);

  //storing life of this program
  me.life_time = args.num_failure_nodes * ( args.time_bw_failures +1 );

  //allocate space for me.neighbors
  me.neighbors = (neighbor_t *)malloc(args.num_nodes * sizeof(neighbor_t));
  for (i = 0; i < args.num_nodes; i++) {
    me.neighbors[i].index = -1;
    me.neighbors[i].heartbeat = 0;
    me.neighbors[i].localtime = 0;
  }
  me.current_dead_count = 0;

  pthread_mutex_init(&me.lock, NULL);
  me.last_process = FALSE;
  me.alive = TRUE;
  me.neighbor_seed = 0;
  me.killer_seed = args.random_seed;
  me.killed_history = (int *)malloc(sizeof(int) * args.num_nodes);
  memset(me.killed_history,0,sizeof(int) * args.num_nodes);

  // dispatch server thread
  pthread_barrier_init(&me.barrier, NULL, NUM_THREADS);
  pthread_create(&server_thread, NULL, &server, NULL);

  // wait
  pthread_barrier_wait(&me.barrier);
  client();

  pthread_mutex_destroy(&me.lock);
  return 0;
}
