#include <sys/select.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <errno.h>

int pselect_(int ReadCount,   int *ReadSockets,
             int WriteCount,  int *WriteSockets,
             int ExceptCount, int *ExceptSockets,
             int Milliseconds)
{
	int      Index, High, Result;
	fd_set   ReadSet, WriteSet, ExceptSet;
	struct   timespec Timeout;
	sigset_t SignalMask;

	High = -1;

	FD_ZERO(&ReadSet);
	for(Index = 0; Index < ReadCount; Index++)
	{
		FD_SET(ReadSockets[Index], &ReadSet);
		if(ReadSockets[Index] > High) High = ReadSockets[Index];
	}

	FD_ZERO(&WriteSet);
	for(Index = 0; Index < WriteCount; Index++)
	{
		FD_SET(WriteSockets[Index], &WriteSet);
		if(WriteSockets[Index] > High) High = WriteSockets[Index];
	}

	FD_ZERO(&ExceptSet);
	for(Index = 0; Index < ExceptCount; Index++)
	{
		FD_SET(ExceptSockets[Index], &ExceptSet);
		if(ExceptSockets[Index] > High) High = ExceptSockets[Index];
	}

	Timeout.tv_sec  = Milliseconds/1000;
	Timeout.tv_nsec = (Milliseconds%1000)*1000000;

	sigfillset(&SignalMask);
	
	Result = pselect(High + 1, &ReadSet, &WriteSet, &ExceptSet, &Timeout, &SignalMask);
	if(Result == -1) return Result;

	for(Index = 0; Index < ReadCount; Index++)
	{
		if(!FD_ISSET(ReadSockets[Index], &ReadSet)) ReadSockets[Index] = 0;
	}

	for(Index = 0; Index < WriteCount; Index++)
	{
		if(!FD_ISSET(WriteSockets[Index], &WriteSet)) WriteSockets[Index] = 0;
	}

	for(Index = 0; Index < ExceptCount; Index++)
	{
		if(!FD_ISSET(ExceptSockets[Index], &ExceptSet)) ExceptSockets[Index] = 0;
	}

	return Result;
}

extern int errno;

int sendto2(int socket, void* restrict buf, size_t length, int flags, int address, int address_port)
{
	struct sockaddr_in toaddr;
	socklen_t tolen = sizeof(toaddr);
	memset(&toaddr, 0, tolen);
	toaddr.sin_family = AF_INET;
	toaddr.sin_addr.s_addr = ntohl(address);
	toaddr.sin_port = ntohs(address_port);
	int result = sendto(socket, buf, length, flags, &toaddr, tolen);
	if (result < 0)
	{
		printf("sendto2 errno: %d\n", errno);
	}
	return result;
}
