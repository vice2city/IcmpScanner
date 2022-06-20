#include "IcmpEcho.h"
#include <stdio.h>

IcmpEcho::IcmpEcho()
{
	bInitWinSockOK = false;

	//Init Winsock
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData))
	{
        Report("Error initializing winsock\n");
	}
	else if (wsaData.wVersion != wVersionRequested)
	{
        Report("Winsock version not supported\n");
	}
	else
	{
		bInitWinSockOK = true;
	}
}

IcmpEcho::~IcmpEcho()
{
	if (bInitWinSockOK)
	{
		WSACleanup();
	}
}

void IcmpEcho::ping(LPCSTR pstrHost)
{
	SOCKET rawsocket;
	LPHOSTENT lpHost;
	struct sockaddr_in saDest;
	struct sockaddr_in saSrc;
	DWORD dwTimesent;
	DWORD dwElapsed;
	u_char cTTL;
	int nLoop;
	int nRet;
    bool isOnline = false;

	if (!bInitWinSockOK)
	{
        Report("Winsock must be initialized\n");
		return;
	}

	//Create ICMP socket
	rawsocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (rawsocket == SOCKET_ERROR)
	{
        Report("Socket obtain error:%d\n", WSAGetLastError());
		return;
	}

	//Get host address
	lpHost = gethostbyname(pstrHost);
	if (lpHost == NULL)
	{
        Report("Host not found:%s\n", pstrHost);
		return;
	}

	saDest.sin_addr.s_addr = *((u_long FAR*) (lpHost->h_addr));
	saDest.sin_family = AF_INET;
	saDest.sin_port = 0;

    Report("Pinging %s [%s] with %d bytes of data:\n",
		pstrHost,
		inet_ntoa(saDest.sin_addr),
		REQ_DATASIZE);

	for (nLoop = 0; nLoop < 4; nLoop++)
	{
		SendEchoRequest(rawsocket, &saDest);

		nRet = waitForEchoReply(rawsocket);
		if (nRet == SOCKET_ERROR)
		{
            Report("Function select() error: %d\n", WSAGetLastError());
			break;
		}
		if (!nRet)
		{
            Report("Time out\n");
			break;
		}

		//Receive reply
		dwTimesent = RecvEchoReply(rawsocket, &saSrc, &cTTL);
        isOnline = true;
		//Caculate time
		dwElapsed = GetTickCount64() - dwTimesent;
        Report("Reply from: %s: bytes=%d time=%ldms TTL=%d",
			inet_ntoa(saSrc.sin_addr),
			REQ_DATASIZE,
			dwElapsed,
			cTTL);
	}

    if(isOnline) emit tri->newIP(inet_ntoa(saSrc.sin_addr));
    Report("\n");
	nRet = closesocket(rawsocket);
	if (nRet == SOCKET_ERROR)
	{
		Report("\nFunction closesocket() error: %d\n", WSAGetLastError());
    }
}

void IcmpEcho::Report(const char* format, ...)
{
	char _Buff[8192];
	memset(_Buff, 0, sizeof(_Buff));

	va_list arg;
	va_start(arg, format);
    int charSize = _vsnprintf_s(_Buff, sizeof(_Buff), format, arg);
	va_end(arg);
    emit tri->newLog(_Buff);
}

int IcmpEcho::SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr)
{
	static ECHOREQUEST echoReq;
	static u_short nId = 1;
	static u_short nSeq = 1;
	int nRet;

	echoReq.icmpHdr.Type = ICMP_ECHOREQ;
	echoReq.icmpHdr.Code = 0;
	echoReq.icmpHdr.Checksum = 0;
	echoReq.icmpHdr.ID = nId++;
	echoReq.icmpHdr.Seq = nSeq++;

	for (nRet = 0; nRet < REQ_DATASIZE; nRet++)
	{
		echoReq.cData[nRet] = ' ' + nRet;
	}

	echoReq.dwTime = GetTickCount64();
	echoReq.icmpHdr.Checksum = in_cksum((u_short*)&echoReq, sizeof(ECHOREQUEST));
	nRet = sendto(s,
		(LPSTR)&echoReq,
		sizeof(ECHOREQUEST),
		0,
		(LPSOCKADDR)lpstToAddr,
		sizeof(SOCKADDR_IN));

	if (nRet == SOCKET_ERROR)
	{
		Report("\nFunction sendto() error:%d\n", WSAGetLastError());
	}

	return(nRet);
}

DWORD IcmpEcho::RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpstFrom, u_char* pTTL)
{
	ECHOREPLY echoReply;
	int nRet;
	int nAddrLen = sizeof(struct sockaddr_in);

	nRet = recvfrom(s,			// socket
		(LPSTR)&echoReply,		// buffer
		sizeof(ECHOREPLY),		// size of buffer
		0,						// flags
		(LPSOCKADDR)lpstFrom,	//From address 
		&nAddrLen);				// pointer to address len

		// check return value
	if (nRet == SOCKET_ERROR) {
		Report("\nFunction recvfrom() error: %d\n", WSAGetLastError());
	}

	*pTTL = echoReply.ipHdr.TTL;
	return(echoReply.echoRequest.dwTime);
}

int IcmpEcho::waitForEchoReply(SOCKET s)
{
	struct timeval Timeout;
	fd_set readfds;

	readfds.fd_count = 1;
	readfds.fd_array[0] = s;
	Timeout.tv_sec = 5;
	Timeout.tv_usec = 0;

	return(select(1, &readfds, NULL, NULL, &Timeout));
}

u_short IcmpEcho::in_cksum(u_short* addr, int len)
{
    int nleft = len;
    u_short* w = addr;
    u_short answer;
    int sum = 0;

	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		u_short	u = 0;

		*(u_char*)(&u) = *(u_char*)w;
		sum += u;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}
