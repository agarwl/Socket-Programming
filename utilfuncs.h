#ifndef UTILFUNCS_H
#define UTILFUNCS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int send_all(int socket, const void *buffer, size_t length, int flags);
int recv_all(int socket,const void *buffer, size_t length, int flags);
void initialise(int sock_fd,sockaddr_in server_addr,char* server_name,char* port);
void error(const char *msg);

// {
// ssize_t n;
// const char *p = buffer;
// while (length > 0)
// {
// n = send(socket, p, length, flags);
// if (n <= 0) break;
// p += n;
// length ­= n;
// }
// return (n <= 0) ? ­1 : 0;
// }

#endif