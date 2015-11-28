#include "netif.h"

/**
 * Set up a node for client/server communication using UDP
 */
nodeInfo *setupNode(void)
{
  debug("Someone wants to setup a node");

  // 1. Create UDP socket
  int s =  socket(AF_INET, SOCK_DGRAM, 0), sa_len;
  nodeInfo *node = (nodeInfo*) malloc(sizeof(nodeInfo));
  struct ifaddrs *if_addr, *tmp;
  struct sockaddr_in *sa;

  if (s < 0) {
    log_err("ERROR opening socket");
    exit(-1);
  }
  node->socket = s;

  // 2. Bind socket with port 0; this allows the OS to use an unused port
  debug("Binding socket");
  memset((char *)&(node->addrInfo), 0, sizeof(struct sockaddr_in));
  node->addrInfo.sin_family = AF_INET;

  if(bind(node->socket, (struct sockaddr *) &node->addrInfo, sizeof(struct sockaddr_in)) < 0)
  {
    log_err("ERROR binding socket");
    debug("IP Address: %s Port Number: %d", inet_ntoa(node->addrInfo.sin_addr), node->addrInfo.sin_port);
    exit(-1);
  }

  // 3. getSockName() to get back the socket name
  debug("Getting our port number");
  sa_len = sizeof(struct sockaddr_in);
  if(getsockname(node->socket, (struct sockaddr *)&node->addrInfo, &sa_len) < 0)
  {
    log_err("ERROR getting socket information");
    exit(-1);
  }
  debug("Our port number is %d", node->addrInfo.sin_port);

  getifaddrs(&if_addr);
  for(tmp = if_addr; tmp != NULL; tmp = tmp->ifa_next)
  {
    if(tmp->ifa_addr->sa_family == AF_INET)
    {
      if(strcmp(tmp->ifa_name,"eth0") == 0) // Get ip addres for eth0
      {
        debug("IP Address: %s Interface:%s", inet_ntoa(((struct sockaddr_in *)(tmp->ifa_addr))->sin_addr), tmp->ifa_name);
        sa = (struct sockaddr_in *) tmp->ifa_addr;
        node->addrInfo.sin_addr = sa->sin_addr;
        break;
      }
    }
  }
  freeifaddrs(if_addr);

  debug("We're done -> IP Address: %s Port Number: %d", inet_ntoa(node->addrInfo.sin_addr), node->addrInfo.sin_port);
  // 4. Return socket for main node thread to listen into
  return node;
}

/**
 * Cleanup a node by closing the specified socket
 */
int cleanupNode(int socket)
{
  close(socket);
}

/**
 * Convenience function to convert a node to a string (ip:port)
 */
char *getNodeInfo(nodeInfo *node)
{
  char *buf = (char *) malloc(NODE_INFO_SIZE * sizeof(char));
  sprintf(buf, "%s:%d", inet_ntoa(node->addrInfo.sin_addr), node->addrInfo.sin_port);
  debug("Node Info: %s", buf);
  return buf;
}

/**
 * Convenience function to split ip:port into its pieces
 */
char *splitAddr(char *nodeAddr)
{
  debug("Asked to split %s", nodeAddr);
  char *loc = strchr(nodeAddr, ':');
  char *portAddr;
  if(loc == NULL) {
    return NULL;
  }
  *loc = '\0';
  portAddr = loc + 1;
  debug("IP Address: %s Port: %s", nodeAddr, portAddr);
  return portAddr;
}

/***Stubbed out: Only for testing
main()
{
  nodeInfo *x = setupNode();
  char *tmp = getNodeInfo(x);
  char *port = splitAddr(tmp);
  printf("We have %s as an IP address and %s as port\n", tmp, port);
  cleanupNode(x->socket);
}
**/
