#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include "debug.h"

/* struct sockaddr_in {
 * sin_port: PORT (ushort)
 * sin_family: AF_INET (short)
 * sin_addr: IP Address (struct in_addr)
 * sin_zero[8] : char; must be set to zero
 */

typedef struct serverInfo {
  int socket;
  struct sockaddr_in addrInfo;
} serverInfo;

// Returns pointer to sockaddr_in which has ipaddress and port + socket to listen on
serverInfo *setupServer(void);

