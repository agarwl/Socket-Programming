/*
Compile as g++ worker.cpp utilfuncs.cpp -lcrypt -o worker 
*/

#define _XOPEN_SOURCE
#include <map>
#include <pthread.h>
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
#define TASKINDEX 17

map<char,int> ctoi;

char hash[HASHLEN+1];
int pwd_len;
char pwd[PWDLEN+1],bin_str[3],salt[3];
static char gen[PWDLEN+1];
static bool found = false;
static int task;
bool is_cont = 0;
int low,high,last;

void initialise()
{
	low = high = -1;
	is_cont = (strcmp(bin_str,"101") != 0);
	if(bin_str[1] == '1'){
		low = sch;
		high = ch;
	}
	if(bin_str[0] == '1'){
		low = 0;
		if(high ==-1) high = sch;
	}
	if(bin_str[2] == '1'){
		if(low == -1) low = ch;
		high = sz;
	}
	//the first two characters of the hash is salt
	memcpy(salt,hash,2);
	salt[2] = '\0';
}

void generate_next()
{	
	last = pwd_len-1;
	while(gen[last] == alphabet[high-1] && last>0)
		gen[last--] = alphabet[low];
	
	if(!is_cont && gen[last] == 'z'){
		gen[last] = '0';
	}
	else
		gen[last] = alphabet[(ctoi[gen[last]] + 1)%sz];
	
	// cout << salt << endl;
	if(strcmp(crypt(gen,salt),hash) == 0){
		memcpy(pwd,gen,pwd_len);
		// cout << "yeah";
		found  = true;
	}
	// if(gen[0] == '1')
	// 	cout << gen << crypt(gen,salt);
}

void *testall(void* y)
{
	// while(!found){cout <<"...";}
	// return NULL;
	int x = *((int*) y);
	initialise();
	char c;
	if(!is_cont && x>=sch){
		c = alphabet[ch+x-sch];
	}	
	else
		c = alphabet[low + x];
	gen[0] = c;
	for (int i = 1; i < pwd_len; ++i)
		gen[i] = alphabet[low];

	while(gen[0] == c && !found){
		// cout << gen << endl;
		generate_next();
	}
	return NULL;
}

int main(int argc, char const *argv[])
{
	if(argc != 3){
		cout << "Usage: ./worker <server ip/host-name> <server-port>" << endl;
		return 1;
	}

	for (int i = 0; i < sz; ++i)
		ctoi[alphabet[i]] = i;

	int sock_fd,iret1,numbytes;
	pthread_t thread1,thread2;
	struct hostent *he;
	struct sockaddr_in server_addr; // connector’s address information

	he = gethostbyname(argv[1]);
	if (he == NULL) { // get the host info
		perror("gethostbyname");
		return 1;
	}
	
	//initialising the TCP socket
	sock_fd = socket(AF_INET,SOCK_STREAM,0); 
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


	char recbuf[MAXLEN],mssg[MAXLEN];
	mssg[0] = 'w';
	mssg[1] = 's';
	send_all(sock_fd,mssg,sizeof(mssg),0);
	
	while(true)
	{
		while( recv_all(sock_fd, recbuf, MAXLEN, 0) != 1){}
		cout << recbuf << endl;
		if(recbuf[0] == '$'){
			found = true;
		}
		else{

			// receive the hash, pwd-len, binary-string
			memcpy(hash,recbuf,HASHLEN);
			hash[HASHLEN] = '\0';
			
			pwd_len = (recbuf[HASHLEN] - '0');
			
			// cout << " oh" <<  pwd_len << "fuck" <<  endl;
			memcpy(bin_str,recbuf+HASHLEN+1,sizeof(bin_str));	

			gen[pwd_len]  = '\0';

			task = stoi(recbuf+TASKINDEX);
			cout << "task: " << task << endl;
			found = false;
			
			iret1 = pthread_create( &thread1, NULL, testall, (void*)&task);
			pthread_join(thread1,NULL);
			
			if(found){
				mssg[1] = 'y';
				memcpy(mssg+2,pwd,pwd_len);
				found = false;
			}
			else{
				mssg[1] = 'n';
				mssg[2] = '\0';
			}
			if (send_all(sock_fd,mssg,sizeof(mssg),0) < 0)
				error("send_all");
		}
	}
	return 0;
}