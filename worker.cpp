#define _XOPEN_SOURCE
// #include <unistd.h>
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

char alphabet[] = {'a' ,'b'  ,'c'  ,'d'  ,'e'  ,'f'  ,'g'  ,'h'  ,'i'  ,'j'  ,'k'  ,'l'  ,'m'  ,'n'  ,'o'  ,'p'  ,'q'  ,'r'  ,'s'  
 ,'t'  ,'u'  ,'v'  ,'w'  ,'x'  ,'y'  ,'z'  ,'A'  ,'B'  ,'C'  ,'D'  ,'E'  ,'F'  ,'G'  ,'H'  ,'I'  ,'J'  ,'K'  ,'L' 
 ,'M'  ,'N'  ,'O'  ,'P'  ,'Q'  ,'R'  ,'S'  ,'T'  ,'U'  ,'V'  ,'W'  ,'X'  ,'Y'  ,'Z'  ,'0'  ,'1'  ,'2'  ,'3'  ,'4'  
 ,'5'  ,'6' ,'7'  ,'8'  ,'9'};

#define sch 26
#define ch 52  
#define sz 62
#define PWDLEN 8
#define MAXLEN 20
#define HASHLEN 13

int l,h; // indices indicating the range of salts to be used
char hash[HASHLEN+1];
int pwd_len;
char pwd[PWDLEN+1],bin_str[4],salt[2];
bool found = 0;

void generate_hash(int curr,char* gen)
{
	if(curr!=pwd_len && !found){
		if(bin_str[0] == '1'){
			for (int i = 0; i < sch; ++i)
			{
				gen[curr] = alphabet[i];
				generate_hash(curr+1,gen);
			}
		}
		if(bin_str[1] == '1'){
			for (int i = sch; i < ch; ++i)
			{
				gen[curr] = alphabet[i];
				generate_hash(curr+1,gen);
			}
		}
		if(bin_str[2] == '1'){
			for (int i = ch; i < sz; ++i)
			{
				gen[curr] = alphabet[i];
				generate_hash(curr+1,gen);
			}
		}
	}
	else if(!found){
		if(strcmp(crypt(gen,salt),hash) == 0){
			strcpy(pwd,gen);
			found  = true;
		}
	}
}

int main(int argc, char const *argv[])
{
	if(argc != 3){
		cout << "Usage: ./worker <server ip/host-name> <server-port>" << endl;
		return 1;
	}
	int sock_fd, numbytes;
	char recvbuf[MAXLEN];
	struct hostent *he;
	struct sockaddr_in server_addr; // connector’s address information

	he = gethostbyname(argv[1]);
	if (he == NULL) { // get the host info
		perror("gethostbyname");
		return 1;
	}
	
	sock_fd = socket(AF_INET,SOCK_STREAM,0); //initialising the TCP socket
	if (sock_fd == -1) {
		perror("socket");
		return 1;
	}

	server_addr.sin_family = AF_INET; // host byte order
	server_addr.sin_port = htons(atoi(argv[2])); // short, network byte order
	server_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(server_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		return 1;
	}

	char recbuf[MAXLEN];
	char mssg[PWDLEN+2],gen[PWDLEN];
	while(true)
	{
		numbytes = recv_all(sock_fd, recbuf, MAXLEN-1, 0);
		if(numbytes <= 0){
			perror("receive");
			return 1;
		}
		// receive the hash, pwd-len, binary-string
		memcpy(hash,recbuf+1,HASHLEN);
		hash[HASHLEN] = '\0';
		memcpy(salt,hash,2);
		char c = recbuf[HASHLEN];
		int pwd_len = atoi(&c);
		memcpy(bin_str,recbuf+HASHLEN+1,sizeof(bin_str));	

		generate_hash(0,gen);
		
		mssg[0] = 'w';
		if(found){
			mssg[1] = 'y';
			memcpy(mssg+2,pwd,pwd_len);
			found = false;
		}
		else{
			mssg[1] = 'n';
		}
		send_all(sock_fd,mssg,sizeof(mssg),0);
	}
}