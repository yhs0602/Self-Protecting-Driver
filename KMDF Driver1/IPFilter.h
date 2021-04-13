//
// Created by 양현서 on 2021/04/13.
//

#ifndef SELF_PROTECTING_DRIVER_IPFILTER_H
#define SELF_PROTECTING_DRIVER_IPFILTER_H

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
} IPV4_HEADER, *PIPV4_HEADER;

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
} TCP_HEADER, *PTCP_HEADER;

typedef struct _UDP_HEADER {
    UINT16 src_port;
    UINT16 dest_port;
    UINT16 length;
    UINT16 checksum;
} UDP_HEADER, *PUDP_HEADER;

typedef struct _ICMP_HEADER {
    UINT8 type;
    UINT8 code;
    UINT16 checksum;
    UINT16 identifier;
    UINT16 sequence;
} ICMP_HEADER, *PICMP_HEADER;
#pragma pack(pop)


typedef struct {
    UINT32 srcAddr;
    UINT32 destAddr;
    SINGLE_LIST_ENTRY SingleListEntry;
} IP_FILTER_ENTRY, *PIP_FILTER_ENTRY;

NTSTATUS SetFilterFunction(PacketFilterExtensionPtr filterFunction);

extern PacketFilterExtensionPtr MyPacketFilterExtension;

NTSTATUS AddFilterToList(UINT32 srcAddr, UINT32 destAddr);

VOID ClearFilters();

PIP_FILTER_ENTRY PopIpFilterEntry(PSINGLE_LIST_ENTRY ListHead);

void PushIpFilterEntry(PSINGLE_LIST_ENTRY ListHead, PIP_FILTER_ENTRY Entry);

#endif //SELF_PROTECTING_DRIVER_IPFILTER_H
