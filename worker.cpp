#define _XOPEN_SOURCE
#include <unistd.h>
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

char alphabet[] = {'a' ,'b'  ,'c'  ,'d'  ,'e'  ,'f'  ,'g'  ,'h'  ,'i'  ,'j'  ,'k'  ,'l'  ,'m'  ,'n'  ,'o'  ,'p'  ,'q'  ,'r'  ,'s'  
 ,'t'  ,'u'  ,'v'  ,'w'  ,'x'  ,'y'  ,'z'  ,'A'  ,'B'  ,'C'  ,'D'  ,'E'  ,'F'  ,'G'  ,'H'  ,'I'  ,'J'  ,'K'  ,'L' 
 ,'M'  ,'N'  ,'O'  ,'P'  ,'Q'  ,'R'  ,'S'  ,'T'  ,'U'  ,'V'  ,'W'  ,'X'  ,'Y'  ,'Z'  ,'0'  ,'1'  ,'2'  ,'3'  ,'4'  
 ,'5'  ,'6' ,'7'  ,'8'  ,'9' ,'.' ,'/'};

#define sz 64

int l,h; // indices indicating the range of salts to be used
char* hash;
int pswd_len;

char salts[sz*sz][3];

void gen_salts(int curr,int& cnt)
{
	if(curr != 2)
	{
		for (int i = 0; i < sz; ++i)
		{
			str[curr] =alphabet[i];
			gen_salts(curr+1,cnt); 
			/* code */
		}
	}
	else{
		strcpy(salts[cnt],str);
		cnt++;
	}
}


void generate_hash(int curr,char* gen)
{
	if(curr!=pswd_len){
		if(bin_str[0] == '1'){
			for (int i = 0; i < 26; ++i)
			{
				gen[curr] = alphabet[i];
				generate_hash(curr+1,gen);
			}
		}
		if(bin_str[1] == '1'){
			for (int i = 26; i < 52; ++i)
			{
				gen[curr] = alphabet[i];
				generate_hash(curr+1,gen);
			}
		}
		if(bin_str[2] == '1'){
			for (int i = 52; i < 62; ++i)
			{
				gen[curr] = alphabet[i];
				generate_hash(curr+1,gen);
			}
		}
	}
	else{
		for (int i = l; i < h ; ++i)
		{
			if(strcmp(crypt(gen,salts[i]),hash)){
				found  = true;
				return;
			}
		}
		found = false;
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

	int cnt = 0;
	str[2] = '\0';
	gen_salts(0,cnt);

	// received the hash, pwd-len, binary-string and range of salts
	char gen[10]; 
	gen[strlen(hash)] = '\0';
	generate_hash(0,gen);
	if(found){
		
	}

	else{

	}
}