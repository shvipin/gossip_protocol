#include "netif.h"

serverInfo *setupServer(void)
{
  // 1. Create UDP socket
  int s =  socket(AF_INET, SOCK_DGRAM, 0), sa_len;
  serverInfo *server = (serverInfo*) malloc(sizeof(serverInfo));
  struct ifaddrs *if_addr, *tmp;
  struct sockaddr_in *sa;
  if (s < 0) {
    log_err("ERROR opening socket");
    exit(-1);
  }
  server->socket = s;
  
  // 2. Bind socket with port 0; this allows the OS to use an unused port
  memset((char *)&(server->addrInfo), 0, sizeof(struct sockaddr_in));
  server->addrInfo.sin_family = AF_INET;
  getifaddrs(&if_addr);
  for(tmp = if_addr; tmp; tmp = tmp->ifa_next)
  {
    printf("IP Address: %s\n", inet_ntoa(((struct sockaddr_in *)(tmp->ifa_addr))->sin_addr));
    if(strcmp(tmp->ifa_name,"eth0")) // Get ip addres for eth0
    {
      sa = (struct sockaddr_in *) tmp->ifa_addr;
      //server->addrInfo.sin_addr = sa->sin_addr;
      break;
    }
  }
  freeifaddrs(if_addr);
  if(bind(server->socket, (struct sockaddr *) &server->addrInfo, sizeof(struct sockaddr_in)) < 0)
  {
    log_err("ERROR binding socket \n");
    printf("IP Address: %s Port Number: %d\n", inet_ntoa(server->addrInfo.sin_addr), server->addrInfo.sin_port);
    exit(-1);
  }

  // 3. getSockName() to get back the socket name
  sa_len = sizeof(struct sockaddr_in);
  if(getsockname(server->socket, (struct sockaddr *)&server->addrInfo, &sa_len) < 0)
  {
    log_err("ERROR getting socket information\n");
    exit(-1);
  }
  printf("IP Address: %s Port Number: %d\n", inet_ntoa(server->addrInfo.sin_addr), server->addrInfo.sin_port);
  // 4. Return socket for main server thread to listen into
  return server;
}

main()
{
  setupServer();
}


