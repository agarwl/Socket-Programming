#define _XOPEN_SOURCE
#include "utilfuncs.h"
#define sz 64

char alphabet[] = {'a' ,'b'  ,'c'  ,'d'  ,'e'  ,'f'  ,'g'  ,'h'  ,'i'  ,'j'  ,'k'  ,'l'  ,'m'  ,'n'  ,'o'  ,'p'  ,'q'  ,'r'  ,'s'  
 ,'t'  ,'u'  ,'v'  ,'w'  ,'x'  ,'y'  ,'z'  ,'A'  ,'B'  ,'C'  ,'D'  ,'E'  ,'F'  ,'G'  ,'H'  ,'I'  ,'J'  ,'K'  ,'L' 
 ,'M'  ,'N'  ,'O'  ,'P'  ,'Q'  ,'R'  ,'S'  ,'T'  ,'U'  ,'V'  ,'W'  ,'X'  ,'Y'  ,'Z'  ,'0'  ,'1'  ,'2'  ,'3'  ,'4'  
 ,'5'  ,'6' ,'7'  ,'8'  ,'9'};

int low,high; // indices indicating the range of salts to be generated
const char* hash;
int pwd_len;

char salts[sz*sz][3];

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void generate_hash(int curr,char* gen)
{
	if(curr!=pwd_len){
		// if(bin_str[0] == '1'){
			for (int i = 52; i < 62; ++i)
			{
				gen[curr] = alphabet[i];
				generate_hash(curr+1,gen);
			}
		}
	else
		cout << crypt(gen,"ac") << "," <<"ac" << endl;
}

// compile as g++ test.cpp -lcrypt
int main()
{
    // printf("%s\n",crypt("123456","ab"));
	 int cnt = 0;
	 pswd_len = 1;
	 char* gen = new char[pswd_len+1];
	 gen[0] = 'a';
	 gen[1] = 'b';
	 // gen[2] = '\0';
	 cout << crypt("1234",gen);
	 // abWMpd9uBwR.g

	 // cout << strcmp(gen,"a")"ab") << endl;
	 // generate_hash(0,gen);
	 // gen_salts(0,cnt,str);
	 // for (int i = 0; i < 100; ++i)
	 // {
	 // 	std::cout << salts[i] << std::endl;
	 // 	/* code */
	 // }
	 return 0;
}