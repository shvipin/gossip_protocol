#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <pthread.h>
#include "debug.h"
#include "netif.h"

#define ENDPOINTS "endpoints"
#define NUM_THREADS 2
#define TRUE 1
#define FALSE 0
#define BUFFER_LENGTH 256

void *server(void *);
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
nodeInfo *endpoints;
nodeInfo *self;

int self_id;
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
  endpoints = (nodeInfo *)malloc(num_nodes * sizeof(nodeInfo));

  // dispatch server thread
  pthread_barrier_init(&barrier, NULL, NUM_THREADS);
  pthread_create(&server_thread, NULL, &server, NULL);

  // wait
  pthread_barrier_wait(&barrier);
  client();

  free(endpoints);
  return 0;
}


void client()
{
  int i;
  int numNodes = 0;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  debug("Client function started...");
  
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
      sendto(self->socket, "OK", 3, 0, &endpoints[i].addrInfo, sizeof(struct sockaddr_in));
    }
  } else {
    debug("I'm not the last process");
  }
  free(line);
}


void *server(void *token)
{
  self = setupNode();
  char message[BUFFER_LENGTH];
  int numNodes = 0;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  do {
    //sleep(3);
    ep = fopen(ENDPOINTS, "a+");
  } while (ep == NULL);
  
  while ((read = getline(&line, &len, ep)) != -1) {
    line[read - 1] = '\0';
    read--;
    numNodes++;
    debug("Read %s from endpoints file", line);
  }
  self_id = numNodes;
  debug("Done reading endpoints file, we have id: %d", self_id);

  if (num_nodes - 1 == numNodes) {
    debug("We are the last process");
    last_process = TRUE;
  }
  fprintf(ep, "%s\n", getNodeInfo(self));
  debug("Node info written to endpoints file");
  fclose(ep);

  if (!last_process) {
    // wait for OK message
    debug("Waiting for \"OK\" message");
    if (recvfrom(self->socket, message, BUFFER_LENGTH, 0, NULL, NULL) == -1) {
      log_err("Failed to receive message");
    }
    debug("Received message %s", message);
    if (strcmp(message, "OK") == 0) {
      debug("Got the \"OK\"");
    }
  } 

  pthread_barrier_wait(&barrier);
  free(line);
}
