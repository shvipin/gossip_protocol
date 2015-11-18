#ifndef __NETIF_H__
#define __NETIF_H__
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include "debug.h"

#define NODE_INFO_SIZE 20

/* struct sockaddr_in {
 * sin_port: PORT (ushort)
 * sin_family: AF_INET (short)
 * sin_addr: IP Address (struct in_addr)
 * sin_zero[8] : char; must be set to zero
 */

typedef struct nodeInfo {
  int socket;
  struct sockaddr_in addrInfo;
} nodeInfo;

// Returns pointer to sockaddr_in which has ipaddress and port + socket to listen on
nodeInfo *setupNode(void);
int cleanupNode(int socket);
char *getNodeInfo(nodeInfo *node);
char *splitAddr(char *nodeAddr);

#endif //__NETIF_H__
