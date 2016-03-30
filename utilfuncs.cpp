
int send_all(int sock_fd, const void *buffer, size_t length, int flags)
{
	int ret, bytes = 0;
	while (bytes < length) 	//send all the bytes of the mssg unless an error is received 
	{
    	ret = send(sock_fd, buffer+bytes, length-bytes, 0); // ret equals the no of bytes sent successfully to the receiver
    	if(ret<=0) break;
    	bytes+=ret; // to increment the buf pointer
	}
	return (ret<=0) ? -1 : 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int recv_all(int sock_fd, const void *buffer, size_t length, int flags)
{
	int ret, bytes = 0, total = 0;
	while(total < length)
	{
		cnt = recv(sock_fd,buffer+bytes,length-bytes,0);
		if(cnt <= 0) break;
		total += cnt;
	}
	if(cnt < 0)
		return -1;
	else if(cnt == 0)
		return 0;
	else
		return 1;
}