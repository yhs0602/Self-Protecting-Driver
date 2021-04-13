//
// Created by 양현서 on 2021/04/13.
//

#ifndef SELF_PROTECTING_DRIVER_IPFILTER_H
#define SELF_PROTECTING_DRIVER_IPFILTER_H

#pragma pack(push, 1)
typedef struct _IPV4_HEADER
{
    UINT8  VersionAndHeaderLength;
    UINT8  TypeOfService;
    UINT16 TotalLength;
    UINT16 Identification;
    UINT16 FlagsAndFragmentOffset;
    UINT8  TimeToLive;
    UINT8  Protocol;
    UINT16 Checksum;
    UINT32 SourceAddress;
    UINT32 DestinationAddress;
} IPV4_HEADER, *PIPV4_HEADER;
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
