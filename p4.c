#include <stdio.h>
#include <stdlib.h>

int num_nodes;
int gossip_b;
int gossip_c;
int time_to_failure;
int num_failure_nodes;
int time_bw_failures;
int random_seed;

int main(int argc, const char *argv[])
{
  if (argc != 8) {
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

  num_nodes = atoi(argv[1]);
  gossip_b = atoi(argv[2]);
  gossip_c = atoi(argv[3]);
  time_to_failure = atoi(argv[4]);
  num_failure_nodes = atoi(argv[5]);
  time_bw_failures = atoi(argv[6]);
  random_seed = atoi(argv[7]);


  return 0;
}
