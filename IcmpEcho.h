#pragma once
#include <winsock.h>
#include <QHostInfo>
#include "mytrigger.h"

typedef std::function<void(char*)> Functional;

class IcmpEcho
{  
#define ICMP_ECHOREPLY 0
#define ICMP_ECHOREQ 8
#define REQ_DATASIZE 32
#define REPLY_DATASIZE 256

	//IP header
	typedef struct tagIPHDR
	{
		u_char VIHL;			// version and IHL
		u_char TOS;				// Type of service
		short TotLen;			// Total Length
		short ID;				// Identification
		short Flagoff;			// Flags and Fragment Offset
		u_char TTL;				// Time To Live
		u_char Protocol;		// Protocol
		u_short Checksum;		// Checksum
		struct in_addr iaSrc;	// Internet Address - Source
		struct in_addr iaDst;	// Internet Address - Destination
	}IPHDR, * PIPHDR;

	// ICMP header
	typedef struct tagICMPHDR
	{
		u_char Type;			// Type
		u_char Code;			// code
		u_short Checksum;		// checksum
		u_short ID;				// Identification
		u_short Seq;			// Sequence
		char Data;				// Data
	}ICMPHDR, * PICMPHDR;

	// ICMP Echo Request
	typedef struct tagECHOREQUEST
	{
		ICMPHDR icmpHdr;
		DWORD dwTime;
		char cData[REQ_DATASIZE];
	}ECHOREQUEST, * PECHOREQUEST;

	// ICMP Echo Reply
	typedef struct tagECHOREPLY
	{
		IPHDR ipHdr;
		ECHOREQUEST echoRequest;
		char cFiller[REPLY_DATASIZE];
	}ECHOREPLY, * PECHOREPLY;

private:
	bool bInitWinSockOK;
	u_short in_cksum(u_short* addr, int len);
	int SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr);
	DWORD RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpstFrom, u_char* pTTL);
	int waitForEchoReply(SOCKET s);
	void Report(const char* format, ...);

public:
    explicit IcmpEcho();
    virtual ~IcmpEcho();
    MyTrigger *tri;
	void ping(LPCSTR pstrHost);
};

