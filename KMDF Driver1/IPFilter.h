//
// 
//

#ifndef SELF_PROTECTING_DRIVER_IPFILTER_H
#define SELF_PROTECTING_DRIVER_IPFILTER_H
#define NDIS630
#include <ndis.h>
#include <Pfhook.h>

//struct to define filter rules

#pragma pack(push, 1)
typedef struct filter
{
	USHORT protocol;		//protocol used

	ULONG sourceIp;			//source ip address
	ULONG destinationIp;	//destination ip address

	ULONG sourceMask;		//source mask
	ULONG destinationMask;	//destination mask

	USHORT sourcePort;		//source port
	USHORT destinationPort; //destination port

	BOOLEAN drop;			//if true, the packet will be drop, otherwise the packet pass
}IPFilter;
#pragma pack(pop)



//struct to build a linked list 
struct filterList
{
	IPFilter ipf;

	struct filterList* next;
};


//Ip Header
typedef struct IPHeader
{
	UCHAR     iphVerLen;      // Version and length 
	UCHAR     ipTOS;          // Type of service 
	USHORT    ipLength;       // Total datagram length 
	USHORT    ipID;		      // Identification 
	USHORT    ipFlags;	      // Flags
	UCHAR     ipTTL;	      // Time to live 
	UCHAR     ipProtocol;	  // Protocol 
	USHORT    ipChecksum;     // Header checksum 
	ULONG     ipSource;       // Source address 
	ULONG     ipDestination;  // Destination address 
} IPPacket;


//TCP Header
typedef struct _TCPHeader
{
	USHORT			sourcePort;			// Source Port
	USHORT			destinationPort;	// Destination Port
	ULONG			sequenceNumber;		// Number of Sequence
	ULONG			acknowledgeNumber;	// Number of aknowledge
	UCHAR			dataoffset;			// Pointer to data
	UCHAR			flags;				// Flags
	USHORT			windows;			// Size of window
	USHORT			checksum;			// Total checksum
	USHORT			urgentPointer;		// Urgent pointer
} TCPHeader;


//UDP Header
typedef struct _UDPHeader
{
	USHORT			sourcePort;			// Source Port
	USHORT			destinationPort;	// Destination Port
	USHORT			len;				// Total length
	USHORT			checksum;			// Total checksum
} UDPHeader;



#pragma pack(push, 1)
typedef struct _IPV4_HEADER {
	UINT8 VersionAndHeaderLength;
	UINT8 TypeOfService;
	UINT16 TotalLength;
	UINT16 Identification;
	UINT16 FlagsAndFragmentOffset;
	UINT8 TimeToLive;
	UINT8 Protocol;
	UINT16 Checksum;
	UINT32 SourceAddress;
	UINT32 DestinationAddress;
} IPV4_HEADER, * PIPV4_HEADER;

typedef struct _TCP_HEADER {
	UINT16 src_port;
	UINT16 dest_port;
	UINT32 seqnum;
	UINT32 acknum;
	UINT8 offset_ns;
	UINT8 flags;
	UINT16 window_size;
	UINT16 checksum;
	UINT16 urgent_pointer;
} TCP_HEADER, * PTCP_HEADER;

typedef struct _UDP_HEADER {
	UINT16 src_port;
	UINT16 dest_port;
	UINT16 length;
	UINT16 checksum;
} UDP_HEADER, * PUDP_HEADER;

typedef struct _ICMP_HEADER {
	UINT8 type;
	UINT8 code;
	UINT16 checksum;
	UINT16 identifier;
	UINT16 sequence;
} ICMP_HEADER, * PICMP_HEADER;
#pragma pack(pop)


typedef struct {
	UINT32 srcAddr;
	UINT32 destAddr;
	SINGLE_LIST_ENTRY SingleListEntry;
} IP_FILTER_ENTRY, * PIP_FILTER_ENTRY;

NTSTATUS SetFilterFunction(PacketFilterExtensionPtr filterFunction);

//PF_FORWARD_ACTION MyPacketFilterExtension(
//    IN unsigned char* PacketHeader,
//    IN unsigned char* Packet,
//    IN unsigned int PacketLength,
//    IN unsigned int RecvInterfaceIndex,
//    IN unsigned int SendInterfaceIndex,
//    IN IPAddr RecvLinkNextHop,
//    IN IPAddr SendLinkNextHop
//);

PF_FORWARD_ACTION cbFilterFunction(
	IN unsigned char* PacketHeader, 
	IN unsigned char* Packet, 
	IN unsigned int PacketLength, 
	IN unsigned int RecvInterfaceIndex,
	IN unsigned int SendInterfaceIndex,
	IN unsigned long RecvLinkNextHop,
	IN unsigned long SendLinkNextHop
);
NTSTATUS AddFilterToList(IPFilter* pf);

VOID ClearFilterList();

//PIP_FILTER_ENTRY PopIpFilterEntry(PSINGLE_LIST_ENTRY ListHead);

//void PushIpFilterEntry(PSINGLE_LIST_ENTRY ListHead, PIP_FILTER_ENTRY Entry);

#endif //SELF_PROTECTING_DRIVER_IPFILTER_H
