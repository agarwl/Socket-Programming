// #include <iostream>
// #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <errno.h>
// #include <netdb.h>
// #include <sys/types.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <sys/types.h>
#include "utilfuncs.h"
// using namespace std;

#define MAXLEN 80

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char const *argv[])
{
	if(argc != 6){
		cout << "Usage: ./user <server ip/host-name> <server-port> <hash> <passwd-length> <binary-string>" << endl;
		return 1;
	}
	int sock_fd, server_port,numbytes;
	char recvbuf[MAXLEN];
	struct hostent *server;
	struct sockaddr_in server_addr; // connector’s address information
	

	server_port = atoi(argv[2]);

	server = gethostbyname(argv[1]);
	if (server == NULL) // get the host info
		error("gethostbyname");
	
	sock_fd = socket(AF_INET,SOCK_STREAM,0); //initialising the TCP socket
	if (sock_fd == -1)
		error("socket");

	server_addr.sin_family = AF_INET; // host byte order
	server_addr.sin_port = htons(server_port); // short, network byte order
	server_addr.sin_addr = *((struct in_addr *)server->h_addr);
	memset(&(server_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
		error("connect");

	string mssg = "";
	mssg =  (((mssg + argv[3] + " ")  + argv[4]) + " ")+ argv[5]; // mssg contains the hash, length and the binary-string
	const char* buf = mssg.c_str();// converting the mssg string to char array 
	
	int ret, bytes = 0, buflen = sizeof(buf); //bytes indicates the position of pointer of the byte to be sent
	while (bytes < buflen) 	//send all the bytes of the mssg unless an error is received 
	{
    	ret = send(sock_fd, buf+bytes, buflen-bytes, 0); // ret equals the no of bytes sent successfully to the receiver
    	
    	if (ret == -1) 	//check for errors
        	error("send");
    	bytes+=ret; // to increment the buf pointer
	}
	
	numbytes = recv(sock_fd, recvbuf, MAXLEN-1, 0); // receiving the password and time taken from server
	if(numbytes == -1) perror("recv");
	
	recvbuf[numbytes] = '\0';
	cout << "Password and time taken:" << endl << recvbuf << endl;
	close(sock_fd);
	return 0; 
}