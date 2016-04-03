/*
Compile as g++ worker.cpp utilfuncs.cpp -lcrypt -o worker 
*/

#define _XOPEN_SOURCE
#include <unordered_map>
#include <pthread.h>
#include "utilfuncs.h"

char alphabet[] = {'a' ,'b'  ,'c'  ,'d'  ,'e'  ,'f'  ,'g'  ,'h'  ,'i'  ,'j'  ,'k'  ,'l'  ,'m'  ,'n'  ,'o'  ,'p'  ,'q'  ,'r'  ,'s'  
 ,'t'  ,'u'  ,'v'  ,'w'  ,'x'  ,'y'  ,'z'  ,'A'  ,'B'  ,'C'  ,'D'  ,'E'  ,'F'  ,'G'  ,'H'  ,'I'  ,'J'  ,'K'  ,'L' 
 ,'M'  ,'N'  ,'O'  ,'P'  ,'Q'  ,'R'  ,'S'  ,'T'  ,'U'  ,'V'  ,'W'  ,'X'  ,'Y'  ,'Z'  ,'0'  ,'1'  ,'2'  ,'3'  ,'4'  
 ,'5'  ,'6' ,'7'  ,'8'  ,'9'};

#define sch 26
#define ch 52  
#define sz 62

unordered_map<char,int> ctoi;

char hashval[HASHLEN+1], pwd[PWDLEN+1],bin_str[3],salt[3]; // data passed by the server
static char mssg[MAXLEN];// string for sending message to the server
static char gen[PWDLEN+1];// a string to store all the possible password 
static bool found = false; // variable to indicate whether the password is found during given task or not
static bool toStop = false; // variable to indicate whether to stop the cracking 
static int task;	//indicate the starting character from which all the passwords are to be generated
int sock_fd,pwd_len; // socket file descriptor for worker and the length of the password to be cracked
bool is_cont = 0; // denotes whether bin_str is "101" or not
int low,high,last; // helper variables for testall

/*function prototypes*/

/* helper function for testall*/
void initialise();

/* generates the next pemutation of the string "gen" using characters according to bin_str*/
void generate_next();

/* function to generate all permutations and test whether they match with a hash or not
and inform the sender accordingly*/
void* testall(void* y);

int main(int argc, char const *argv[])
{
	if(argc != 3){
		cout << "Usage: ./worker <server ip/host-name> <server-port>" << endl;
		return 1;
	}

	//constructing the map from character to int
	for (int i = 0; i < sz; ++i)
		ctoi[alphabet[i]] = i;

	int n;
	pthread_t thread1;
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

	//setting up the connection
	if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		return 1;
	}


	char recbuf[MAXLEN];
	mssg[0] = 'w';
	mssg[1] = 's';
	if(send_all(sock_fd,mssg,sizeof(mssg),0) < 0)
		error("send");
	
	while(true)
	{
		while( (n = recv_all(sock_fd, recbuf, MAXLEN, 0))!= 1)
		{
			if(n == 0){
				cout << "Server closed connection" << endl;
				close(sock_fd);
				return 1;
			}
		}
		if( strcmp(recbuf,"Password found") == 0 || strcmp(recbuf,"Client hung up") == 0){
			cout << recbuf << endl;
			// assign this to indicate to stop and not send the server any message
			toStop = true;
			found = true;
		}
		// else if(strcmp(recbuf,))
		else{

			// receive the hash, pwd-len, binary-string
			memcpy(hashval,recbuf,HASHLEN);
			hashval[HASHLEN] = '\0';
			cout << "Trying to decrypt the hash " << hashval << endl;
			pwd_len = (recbuf[HASHLEN] - '0');
			memcpy(bin_str,recbuf+HASHLEN+1,sizeof(bin_str));	

			gen[pwd_len]  = '\0';
			task = stoi(recbuf+TASKINDEX);
			cout << "Doing task: " << task << endl;
			found = false;
			toStop = false;
			
			//initialsing thread for current task
			pthread_create( &thread1, NULL, testall, (void*)&task);
		}
	}
	return 0;
}

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
	memcpy(salt,hashval,2);
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
		gen[last] = alphabet[(ctoi[gen[last]] + 1)%sz]; // %sz to wrap around the last character to indicate change
	
	// check whether the hash of the current string matches with the given hash
	if(strcmp(crypt(gen,salt),hashval) == 0){
		memcpy(pwd,gen,pwd_len);
		found  = true;
	}
}

void *testall(void* y)
{
	// integer cast
	int x = *((int*) y);
	//initialise low,high and salt 
	initialise();
	char c;
	//find the first character of "gen"
	if(!is_cont && x>=sch){
		c = alphabet[ch+x-sch];
	}	
	else
		c = alphabet[low + x];
	
	//calculate the first permutation starting with character c
	gen[0] = c;
	for (int i = 1; i < pwd_len; ++i)
		gen[i] = alphabet[low];

	//the first character remains constant across all the generated passwords
	while(gen[0] == c && !found){
		generate_next();
	}

	// inform the server accordingly
	if(found){
	  if(!toStop){
		  mssg[1] = 'y';
		  memcpy(mssg+2,pwd,pwd_len);
		  found = false;
	  }
	}
	else{
		strcpy(mssg+1,"Not found");
		cout << "Password doesn't start with character " << c << endl; 
	}
	if(!toStop){
		if (send_all(sock_fd,mssg,sizeof(mssg),0) < 0)
		error("send_all");	
	}
	else{
		toStop = false;
	}
	return NULL;
}