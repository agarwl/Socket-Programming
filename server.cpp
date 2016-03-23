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
using namespace std;

#define BACKLOG 8 // how many pending connections queue will hold

void error(const char *msg)
{
    perror(msg);
    return 1;
}

void dostuff(int); /* function prototype */

int main(int argc, char const *argv[])
{
	if(argc != 2){
		cout << "Usage: ./server <server-port> ";
		return 1;
	}
	struct sockaddr_in myaddr; // server address
	struct sockaddr_in remoteaddr; // client address
	int listener; // listening socket descriptor
	int newsockfd; // newly accept()ed socket descriptor
	int pid;
	char buf[256]; // buffer for client data
	int nbytes;
	int num_workers=0,num_clients=0;
	short workers[5];

	// get the listener
	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener == -1) {
		perror("socket");
		return 1;
	}
	
	int yes=1;
	// lose the pesky "address already in use" error message 
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		return 1;
	}

	// bind
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY; // get my own IP address
	myaddr.sin_port = htons(atoi(argv[1])); // short, network byte order; arv[1] is the server port
 	memset(&(myaddr.sin_zero), ’\0’, 8);
	
	if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
		perror("bind");
		return 1;
	}

	listen(listener,BACKLOG);
	remote_size = sizeof(remoteaddr);

	signal(SIGCHLD,SIG_IGN);
	while (true) 
	{
		newsockfd = accept(sockfd,(struct sockaddr *) &remoteaddr, &remote_size);
		if (newsockfd < 0) 
		 error("ERROR on accept");
		pid = fork();
		if (pid < 0)
		 error("ERROR on fork");
		if (pid == 0)  {
		 close(sockfd);
		 dostuff(newsockfd);
		 return 0;
		}
		else close(newsockfd);
	} /* end of while */
	 close(sockfd);
     return 0; /* we never get here */
}

void dostuff (int sock)
{
   int n,len;
   char buffer[256],msg[256],hash[14],bin_str[4];
      
   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   
	if(buffer[0] == 'c'){
		num_clients++;
		strncpy(hash,buffer+1,13);
		hash[13] = '\0';
		strncpy(bin_str,buffer+14,3);
		bin_str[3] = '\0';
		msg = "Connection aborted,No of clients exceeded 3!";
	}
	else if(buffer[0] == 'w'){
		num_workers++;
		msg = "Connection aborted,No of workers exceeded 5!";
	}

	if(num_workers > 5 || num_clients > 3)
	{
		num_clients = (num_clients > 3)?3:num_clients;
		num_workers = (num_workers > 5)?5:num_workers;
		n = send_all(sock,msg,sizeof(msg));
		if (n < 0) error("ERROR writing to socket");
		return;
	}

   }
   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18);
   if (n < 0) error("ERROR writing to socket");
}