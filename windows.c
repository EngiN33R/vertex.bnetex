#include <windows.h>
#include <iptypes.h>
#include <ipifcons.h>
#include <iphlpapi.h>
#include <string.h>
#include <ws2tcpip.h>

int GetNetworkAdapter(char *Device, char MAC[6], int *Address, int *Netmask, int *Broadcast)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter;
	ULONG            BufferSize;

	pAdapterInfo = (PIP_ADAPTER_INFO) malloc(sizeof(IP_ADAPTER_INFO));
	pAdapter = NULL;
	BufferSize = sizeof(IP_ADAPTER_INFO);

	if(GetAdaptersInfo(pAdapterInfo, &BufferSize) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO) malloc(BufferSize);
		if(!pAdapterInfo) return FALSE;
	}

	if(GetAdaptersInfo(pAdapterInfo, &BufferSize) != NO_ERROR)
	{
		free(pAdapterInfo);
		return FALSE;
	}

	pAdapter = pAdapterInfo;
	while(pAdapter)
	{
		if(pAdapter->Type == MIB_IF_TYPE_ETHERNET) break;
		pAdapter = pAdapter->Next;
	}
	if(!pAdapter)
	{
		free(pAdapterInfo);
		return FALSE;
	}

	strcpy(Device, pAdapter->Description);
	if(pAdapter->AddressLength != 6) {
		ZeroMemory(MAC, 6);
	} else {
		memcpy(MAC, pAdapter->Address, 6);
	}

	*Address   = ntohl(inet_addr(pAdapter->IpAddressList.IpAddress.String));
	*Netmask   = ntohl(inet_addr(pAdapter->IpAddressList.IpMask.String));
	*Broadcast = *Address | ~(*Netmask);

	free(pAdapterInfo);
	return TRUE;
}

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