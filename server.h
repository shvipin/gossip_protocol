#ifndef __SERVER_H__
#define __SERVER_H__

#include "p4.h"

void *server(void *token);
void decode(char *message);
void server_listen(char *message);

#endif //__SERVER_H__
