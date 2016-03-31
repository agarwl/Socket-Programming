/*
Compile as g++ user.cpp -o user 
*/

#include "utilfuncs.h"
// using namespace std;

#define MAXLEN 20
#define PWDLEN 8
#define HASHLEN 13

int main(int argc, char const *argv[])
{
	if(argc != 6){
		cout << "Usage: ./user <server ip/host-name> <server-port> <hash> <passwd-length> <binary-string>" << endl;
		return 1;
	}
	int sock_fd, server_port;
	char recvbuf[PWDLEN+1];
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

	char buf[MAXLEN];
	buf[0] = 'c';
	memcpy(buf+1,argv[3],HASHLEN);
	memcpy(buf+HASHLEN+1,argv[4],1);
	memcpy(buf+HASHLEN+2,argv[5],3);
	buf[HASHLEN+5] = '\0';
	if(send_all(sock_fd,buf,MAXLEN,0) == -1)
		error("send");

	listen(sock_fd,1);
	while(recv_all(sock_fd, recvbuf,PWDLEN+1,0) != 1){}
		
	// if(recv_all(sock_fd, recvbuf,PWDLEN+1, 0) < 0) // receiving the password and time taken from server
	// 	error("recv");
	
	cout << "Password and time taken:" << endl << recvbuf << endl;
	close(sock_fd);
	return 0; 
}