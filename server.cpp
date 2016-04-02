/*
Compile as g++ server.cpp utilfuncs.cpp -o server
http://www.lowtek.com/sockets/select.html
*/

#include <signal.h>
#include <queue>
#include <map>
#include <fcntl.h>
#include "utilfuncs.h"
// using namespace std;

typedef map<int,int>::iterator it_type;

#define BACKLOG 8 // how many pending connections queue will hold
#define MAXLEN 20 
#define HASHLEN 13
#define PWDLEN 8
#define CLIENTS 3
#define TASKINDEX 17

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

queue<client> clients; // queue for the clients
map<int,int> worker; //a map to store whether a worker is busy or not
char pwd[PWDLEN+1];

const int MAX_WORKERS = 5;
const int MAX_CLIENTS = 3;
const int MAX_CONNECTIONS = 8;

int sock_fd; // listening socket descriptor
int connections[MAX_CONNECTIONS]; // Array of connected workers and clients
fd_set socket_set;// Socket file descriptors we want to wake up for, using select() 
int max_sock;// Highest  file descriptor, needed for select() 
struct sockaddr_in remoteaddr; // client address
socklen_t remote_size = sizeof(remoteaddr);

int curr_char = 0;
int task_len=0;

/* function prototypes */
void set_tasklen();
void updateConnections(const int &i);
void construct_select_list();
void deal_with_socket(int i);
void read_sockets();
void connection_handler();
void assign_task(const int &sock_fd,char* msg);
void assign_workers(char* msg,int sock=0);
void send_password(char* buf);
void stop_workers(char*msg);

int main(int argc, char const *argv[])
{
	if(argc != 2){
		cout << "Usage: ./server <server-port> ";
		return 1;
	}
	struct sockaddr_in myaddr; // server address
	
	struct timeval timeout;  // Timeout for select
	int read_num;	     // Number of sockets ready for reading 

	
	// Get a file descriptor for the "listening" socket
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		perror("socket");
		return 1;
	}
	
	// Used so we can re-bind to our port while a previous connection is still in TIME_WAIT state.
	int yes = 1;
	
	// For re-binding to it without TIME_WAIT problems 
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		error("setsockopt");
	}

	// Set socket to non-blocking with our setnonblocking routine
	if(fcntl(sock_fd,F_SETFL,O_NONBLOCK) < 0)
		error("fcntl(F_SETFL)");

	// Get the address information, and bind it to the socket
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY; // get my own IP address
	myaddr.sin_port = htons(atoi(argv[1])); // short, network byte order; arv[1] is the server port
	memset(&(myaddr.sin_zero), '\0', 8); // zero the rest of the struct

	if (bind(sock_fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
		close(sock_fd);
		error("bind");
	}

	// Set up queue-limit of BACKLOG for incoming connections.
	listen(sock_fd,BACKLOG);
	
	// Since we start with only one socket, the listening socket, it is the socket with highest id so far.
	max_sock = sock_fd;

	for (int i = 0; i < MAX_CONNECTIONS; ++i)
		connections[i] = 0;

	while (true)
	{
		construct_select_list();
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		//select returns the number of sockets ready to be read
		read_num = select(max_sock+1, &socket_set, NULL,NULL, &timeout);
		if (read_num < 0)
			error("select");
		if(read_num > 0)
			read_sockets();
	}

	return 0;
}

//Construct the fd_set for select()
void construct_select_list()
{
	// Clear the socket_set, so that it doesn't contain any file descriptors.
	FD_ZERO(&socket_set);
	
	// Add the file descriptor "sock_fd" to the socket_set
	FD_SET(sock_fd,&socket_set);
	
	// Loops through all the possible connections and adds those sockets to the fd_set 
	for (int i = 0; i < MAX_CONNECTIONS; ++i)
	{
		if(connections[i] != 0){
			FD_SET(connections[i],&socket_set);
			max_sock = max(max_sock,connections[i]);
		}
	}
}

void connection_handler()
{
	// newly accept()ed socket descriptor
	int newsockfd = accept(sock_fd,(struct sockaddr *) &remoteaddr, &remote_size);
	if(newsockfd < 0)
		error("accept");
	if(fcntl(newsockfd,F_SETFL,O_NONBLOCK) < 0)
		error("fcntl(F_SETFL)");

	for (int i = 0; i < MAX_CONNECTIONS; ++i)
	{
		if(connections[i] == 0){
		 	printf("\nConnection accepted:  FD=%d; Slot=%d\n",newsockfd,i);
		 	connections[i] = newsockfd;
		 	newsockfd = -1;
		 	break;
		}
	}

	if (newsockfd != -1) {
		printf("\nNo room left for new connection.\n");
		close(newsockfd);
	}
}

void read_sockets()
{
	// if the listening socket is part of the fd_set, we need to accept a new connection.
	if (FD_ISSET(sock_fd,&socket_set))
		connection_handler();

	// Run through all the connected sockets and check to see if anything happened with them, if so "process" them.
	for (int i = 0; i < MAX_CONNECTIONS; ++i)
		if(FD_ISSET(connections[i],&socket_set))
			deal_with_socket(i);
}

void deal_with_socket(int i)
{
   int sock = connections[i];
   if(!sock)
   	return;
   int n,sock_fd;
   // array to send data to client/worker
   char msg[MAXLEN];
   // array to receive data
   char buffer[MAXLEN];
   //empty the buffer
   bzero(buffer,MAXLEN);

   n = recv_all(sock,buffer,MAXLEN,0);
   if (n < 0) error("ERROR reading intial mssg from socket");
   else if(n==0){
   	updateConnections(i);
   	return;
   }
   printf("Here is the message: %s\n",buffer);

   if(buffer[0] == 'c'){

   		if(clients.size() <= MAX_CLIENTS){
   			char c = buffer[HASHLEN+1];
   			int pwd_len = atoi(&c);
   			cout << "pwd_len: " << pwd_len << endl;
			clients.push(client(sock,pwd_len,buffer+1,buffer+HASHLEN+2));
			set_tasklen();	
		}
		else{
			strcpy(msg,"Connection aborted");
			n = send_all(sock,msg,sizeof(msg),0);
			if (n < 0) 
				error("ERROR writing to client socket");
			updateConnections(i); 
			return;
		}
		assign_workers(msg);
	}
	else if(buffer[0] == 'w'){
		
		if(buffer[1] == 's'){
			if(worker.size() < MAX_WORKERS){
				//initialise this newly connected worker as free
				worker[sock] = 0;
				//assign this worker some task
				assign_workers(msg,sock);
				// cout << "worker size: " << worker.size() << '\n';
			}
			else{
			  	strcpy(msg,"wConnection aborted");
			  	if (send_all(sock,msg,sizeof(msg),0) < 0);
					error("ERROR writing to worker socket, the client message");
				updateConnections(i);
				printf("\n");
				return;
			}
		}	
		else if(buffer[1] == 'y'){
			send_password(buffer+2);
			stop_workers(msg);
			assign_workers(msg);
		}
		else if(buffer[1] == 'n')
		{
			assign_task(sock,msg);
		}
	}
}

// close the socket pointed by connections[i] and update max_sock
void updateConnections(const int &i)
{
	close(connections[i]);
	if(max_sock == connections[i]){
		max_sock = sock_fd;
		for (int j = 0; j < MAX_CONNECTIONS; ++j)
		{
			if(j!=i)
				max_sock = max(max_sock,connections[j]);
		}
	}
	connections[i] = 0;
}

void set_tasklen()
{
	if(task_len == 0 && !clients.empty()){
		client c = clients.front();
		task_len = (c.bin_str[0] - '0')*26 + (c.bin_str[1] - '0')*26 + (c.bin_str[2]-'0')*10; 
	}
}

void generate_msg(char* msg)
{
	if(!clients.empty()){
		
		//get the client at the top of the queue
		client c = clients.front();
		
		//copy the hash into the message
		memcpy(msg,c.hash,HASHLEN);
		cout << c.pwd_len << endl;
		msg[HASHLEN] = (c.pwd_len + '0');
		
		//append the binary-string into the message
		memcpy(msg+HASHLEN+1,c.bin_str,3);
	}	
}

void assign_workers(char* msg,int sock)
{
	generate_msg(msg);
	if(!clients.empty()){
		if(!sock)
		{
			for (it_type it = worker.begin();it!= worker.end(); it++)
			{
				if(it->second == 0)
				{
					assign_task(it->first,msg);
				    // cout << "sending: " << msg << endl;
				}
			}
		}
		else
			assign_task(sock,msg);
	}
}

void assign_task(const int & sock_fd,char* msg)
{
	if(curr_char < task_len)
	{
		itoc(curr_char,msg+TASKINDEX);
		msg[MAXLEN-1] = '\0';
		cout << "msg:" << msg <<endl;
		if (send_all(sock_fd,msg,MAXLEN,0) < 0)
			error("ERROR writing to worker socket");
		else
		{
			worker[sock_fd] = 1;
			curr_char++;
		}
	}
}

void send_password(char* buf)
{
	if(!clients.empty()){
		client c = clients.front();
		memcpy(pwd,buf,c.pwd_len);
		pwd[c.pwd_len] = '\0';
		int sock_fd = (clients.front()).sock;
		if(send_all(sock_fd,pwd,sizeof(pwd),0) < 0) 
			 error("ERROR writing the psswd to client socket");
		for (int i = 0; i < MAX_CONNECTIONS; ++i)
		{
			if(connections[i] == sock_fd){
				updateConnections(i);
				break;
			}
		}
		clients.pop();
		task_len = 0;
		curr_char = 0;
		set_tasklen();
	}
}

void stop_workers(char*msg)
{
	strcpy(msg,"$found");
	for (it_type it = worker.begin();it!= worker.end(); it++)
	{
		if(it->second == 1)
		{	
			if (send_all(it->first,msg,MAXLEN,0) < 0)
				error("ERROR sending the stop message to worker socket");
			else{
				it->second = 0;
			}
		}
	}
}