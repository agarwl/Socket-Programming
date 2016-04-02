#include "utilfuncs.h"

// to send the complete packet of size length
int send_all(int sock_fd, char *buffer, size_t length, int flags)
{
	int ret;
	size_t bytes = 0;
	while (bytes < length) 	//send all the bytes of the mssg unless an error is received 
	{
    	ret = send(sock_fd, buffer+bytes, length-bytes, 0); // ret equals the no of bytes sent successfully to the receiver
    	if(ret<=0) break;
    	bytes+=ret; // to increment the buf pointer
	}
	return (ret<=0) ? -1 : 0;
}

// to print error message on stream
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// to convert a character array of maximum length 2 into a integer
int stoi(char*c)
{
	int x = c[0] - '0';
	if(c[1] != '\0'){
		x *= 10;
		x += (c[1] - '0');
	}
	return x;
}

// convert a integer of atmost 2 digits into a char array
void itoc(const int & num,char *c)
{
	if (num < 10){
		c[0] = num + '0';
		c[1] ='\0';
	}
	else{
		c[1] = (num%10) + '0';
		c[0] = num/10 + '0';
	}
	return;
}

// to receive the complete packet of size length
int recv_all(int sock_fd, char *buffer, size_t length, int flags)
{
	int cnt;
	size_t bytes = 0, total = 0;
	while(total < length)
	{
		// cnt equals the partial no. of bytes received sucessfully
		cnt = recv(sock_fd,buffer+bytes,length-bytes,0);
		if(cnt <= 0) break;
		total += cnt;
	}
	// error on receive
	if(cnt < 0)
		return -1;
	else if(cnt == 0)	// the other side of the connection hung up
		return 0;
	else
		return 1;  // mssg received succesfully
}