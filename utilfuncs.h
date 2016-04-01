#ifndef UTILFUNCS_H
#define UTILFUNCS_H
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
using namespace std;

int send_all(int socket, char *buffer, size_t length, int flags);
int recv_all(int socket,char *buffer, size_t length, int flags);
void error(const char *msg);
void itoc(const int & num,char *c);
// void initialise(int sock_fd,sockaddr_in server_addr,char* server_name,char* port);

#endif