#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <pthread.h>

#define ENDPOINTS "endpoints"
#define NUM_THREADS 2
#define TRUE 1
#define FALSE 0

void server();
void client();

// user provided arguments
int num_nodes;
int gossip_b;
int gossip_c;
int time_to_failure;
int num_failure_nodes;
int time_bw_failures;
int random_seed;

// our globals
typedef struct node {
} node_t;
node *endpoints; 
int last_process = FALSE;
FILE *ep;
pthread_t server_thread;
pthread_barrier_t barrier;

int main(int argc, const char *argv[])
{
  if (argc != 8) {
    // user messed up. Print usage info and exit
    printf("Usage ./p4 N b c F B P S\n \
      number of peer nodes N\n \
      gossip parameter b\n \
      gossip parameter c\n \
      number of seconds after which a node is considered dead F\n \
      number of bad nodes that should fail B\n \
      number of seconds to wait between failures P\n \
      the see of the random number generator S\n");
    exit(1);
  }

  // take in user's arguments
  num_nodes = atoi(argv[1]);
  gossip_b = atoi(argv[2]);
  gossip_c = atoi(argv[3]);
  time_to_failure = atoi(argv[4]);
  num_failure_nodes = atoi(argv[5]);
  time_bw_failures = atoi(argv[6]);
  random_seed = atoi(argv[7]);

  // allocate memory for our endpoints
  endpoints = (node *)malloc(num_nodes * sizeof(node_t));
  ep = fopen(ENDPOINTS, "w+"):  

  // dispatch server thread
  pthread_barrier_init(&barrier, NULL, NUM_THREADS);
  pthread_create(&server_thread, 0, NULL, &server, NULL);

  // wait
  pthread_barrier_wait(&barrier);
  client();

  fclose(fp);
  return 0;
}


void client()
{
  if (last_process) {

  } else {

  }
}


void server()
{

}
