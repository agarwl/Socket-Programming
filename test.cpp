#define _XOPEN_SOURCE
#include "utilfuncs.h"
#include <map>
#include <pthread.h>
#include <string.h>
char alphabet[] = {'a' ,'b'  ,'c'  ,'d'  ,'e'  ,'f'  ,'g'  ,'h'  ,'i'  ,'j'  ,'k'  ,'l'  ,'m'  ,'n'  ,'o'  ,'p'  ,'q'  ,'r'  ,'s'  
 ,'t'  ,'u'  ,'v'  ,'w'  ,'x'  ,'y'  ,'z'  ,'A'  ,'B'  ,'C'  ,'D'  ,'E'  ,'F'  ,'G'  ,'H'  ,'I'  ,'J'  ,'K'  ,'L' 
 ,'M'  ,'N'  ,'O'  ,'P'  ,'Q'  ,'R'  ,'S'  ,'T'  ,'U'  ,'V'  ,'W'  ,'X'  ,'Y'  ,'Z'  ,'0'  ,'1'  ,'2'  ,'3'  ,'4'  
 ,'5'  ,'6' ,'7'  ,'8'  ,'9'};


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

#define sch 26
#define ch 52  
#define sz 62
#define PWDLEN 8
#define MAXLEN 20
#define HASHLEN 13

map<char,int> ctoi;

char hash[HASHLEN+1],gen[PWDLEN+1];
int pwd_len=4;
char pwd[PWDLEN+1],bin_str[3],salts[2];
bool found = false;
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
	memcpy(salts,hash,2);

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
	
	if(strcmp(crypt(gen,salts),hash) == 0){
		memcpy(pwd,gen,pwd_len);
		found  = true;
	}
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
		cout << gen << endl;
		generate_next();
	}
	return NULL;
}

// compile as g++ test.cpp -lcrypt
int main()
{
    // printf("%s\n",crypt("123456","ab"));
	 int cnt = 0;
	 // gen[pwd_len-1] = '\0';
	 // gen[2] = '\0';
	 // cout << crypt("1234","bc");
	 // ab1iBa.N.U2C6
	 strcpy(hash ,"bc9kg/L7Fd0Nw");
	 for (int i = 0; i < sz; ++i)
		ctoi[alphabet[i]] = i;
	 strcpy(bin_str,"001");
	 int x = 1;
	 // pthread_t thread1;
	 gen[pwd_len] = '\0';
	 testall(&x);
	 // int iret1 = pthread_create( &thread1, NULL, testall, (void*)&x);
	 // cout << strcmp();
	 // testall(0,gen);
	 // cout << strcmp(gen,"a")"ab") << endl;
	 // generate_hash(0,gen);
	 // gen_salts(0,cnt,str);
	 // for (int i = 0; i < 100; ++i)
	 // {
	 // 	std::cout << salts[i] << std::endl;
	 // 	/* code */
	 // }
	 // pthread_join(thread1, NULL);
	 
	 // while(1)
	 // {
	 // 	y++;
	 // 	if(y > 1000)
	 // 		break;
	 // }
	 // x = 2;
	 // while(1){}
	 pwd[pwd_len] = '\0';
	 if(found)
	 	cout << pwd;
	 return 0;
}

/*void generate_hash(int curr,char* gen)
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
		 cout << gen << endl;
		if(strcmp(crypt(gen,salt),hash) == 0){
			memcpy(pwd,gen,pwd_len);
			found  = true;
			cout << "yeah";
		}
	}
}*/