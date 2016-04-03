#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
using namespace std;

int main(int argc, char const *argv[])
{
	if(argc != 3){
		printf("Usage: ./generateHash password salt\n");
		return 1;
	}
	printf("%s",crypt(argv[1],argv[2]));
	return 0;
}