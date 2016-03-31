/*
Compile as g++ server.cpp utilfuncs.cpp -o server
http://www.lowtek.com/sockets/select.html
*/

#include <signal.h>
#include <queue>
#include <map>
#include "utilfuncs.h"
// using namespace std;

typedef map<int,int>::iterator it_type;

#define BACKLOG 8 // how many pending connections queue will hold
#define MAXLEN 20 
#define HASHLEN 13
#define PWDLEN 8
#define CLIENTS 3

struct client{
	int sock;
	int pwd_len;
	char hash[HASHLEN+1];
	char bin_str[3];

	client(int s, int len, char* h,char *b)
	{
		sock = s;
		pwd_len = len;
		memcpy(hash,h,HASHLEN);
		h[HASHLEN] = '\0';
		memcpy(bin_str,b,3);
	}
};

queue<client> clients;
// deque<worker> workers;
map<int,int> worker;
int curr = -1;
char pwd[PWDLEN+1];

// void error(const char *msg)
// {
//     perror(msg);
//     exit(1);
// }

void dostuff(int); /* function prototype */

int main(int argc, char const *argv[])
{
	if(argc != 2){
		cout << "Usage: ./server <server-port> ";
		return 1;
	}
	struct sockaddr_in myaddr; // server address
	struct sockaddr_in remoteaddr; // client address
	int sock_fd; // listening socket descriptor
	int newsockfd; // newly accept()ed socket descriptor
	int pid;

	// get the listener
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		perror("socket");
		return 1;
	}
	
	int yes=1;
	// lose the pesky "address already in use" error message 
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		error("setsockopt");
	}

	// bind
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY; // get my own IP address
	myaddr.sin_port = htons(atoi(argv[1])); // short, network byte order; arv[1] is the server port
	// memset(&(myaddr.sin_zero), ’\0’, 8);
		memset(&(myaddr.sin_zero), '\0', 8); // zero the rest of the struct
	
	if (bind(sock_fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
		perror("bind");
		return 1;
	}

	listen(sock_fd,BACKLOG);
	socklen_t remote_size = sizeof(remoteaddr);

	signal(SIGCHLD,SIG_IGN);
	while (true) 
	{
		newsockfd = accept(sock_fd,(struct sockaddr *) &remoteaddr, &remote_size);
		if (newsockfd < 0)
		 error("ERROR on accept");
		pid = fork();
		if (pid < 0)
		 error("ERROR on fork");
		if (pid == 0)  {
		 close(sock_fd);
		 dostuff(newsockfd);
		 return 0;
		}
		else close(newsockfd);
	} /* end of while */
	 close(sock_fd);
     return 0; /* we never get here */
}

void dostuff(int sock)
{
   int n,sock_fd,pwd_len;
   char buffer[MAXLEN],msg[MAXLEN];
   bzero(buffer,MAXLEN);
   n = recv_all(sock,buffer,MAXLEN,0);
   if (n < 0) error("ERROR reading from socket");
   printf("Here is the message: %s\n",buffer);
   // strcpy(msg,"fuckyeah");
   // send_all(sock,msg,PWDLEN+1,0);
   if(buffer[0] == 'c'){

   		if(clients.size() <= 3){
   			pwd_len = atoi(&buffer[HASHLEN+1]);
			clients.push( client(sock,pwd_len,buffer+1,buffer+HASHLEN+2));			
		}
		else{
			strcpy(msg,"Connection aborted");
			n = send_all(sock,msg,sizeof(msg),0);
			if (n < 0) error("ERROR writing to socket");
			close(sock);
			return;
		}

		cout << "worker size: " << worker.size() << '\n';
		for (it_type it = worker.begin();it!= worker.end(); it++)
		{
			client c = clients.front();
			memcpy(msg,c.hash,HASHLEN);
			msg[HASHLEN] = c.pwd_len + '0';
			memcpy(msg+HASHLEN+1,c.bin_str,3);
			
			if(it->second == 0)
			{
				cout << "sending: " << msg << endl;
				sock_fd =  it->first;
				send_all(sock_fd,msg,MAXLEN,0);
				worker[sock_fd] = 1;
				break;
			}
		}

	}
	else if(buffer[0] == 'w'){
		
		if(buffer[1] == 's'){
			if(worker.size() < 5){
				// workers.push(worker(sock,0));
				worker[sock] = 0;
				cout << "worker size: " << worker.size() << '\n';
			}
			else{
			  	strcpy(msg,"wConnection aborted");
			  	n = send_all(sock,msg,sizeof(msg),0);
				if (n < 0) error("ERROR writing to worker socket");
				close(sock);
				return;
			}
		}	
		else if(buffer[1] == 'y'){
			client c = clients.front();
			memcpy(pwd,buffer+2,c.pwd_len);
			pwd[c.pwd_len] = '\0';
			worker[sock] = 0;
			sock_fd = (clients.front()).sock;
			n  = send_all(sock_fd,pwd,sizeof(pwd),0);
			if (n < 0) error("ERROR writing to socket");		
			close(sock_fd);
			clients.pop();
		}
	}
}