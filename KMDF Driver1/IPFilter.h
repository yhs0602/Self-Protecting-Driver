//
// Created by 양현서 on 2021/04/13.
//

#ifndef SELF_PROTECTING_DRIVER_IPFILTER_H
#define SELF_PROTECTING_DRIVER_IPFILTER_H
NTSTATUS SetFilterFunction(PacketFilterExtensionPtr filterFunction);
extern PacketFilterExtensionPtr MyPacketFilterExtension;
NTSTATUS AddFilterToList();
VOID ClearFilters();
#endif //SELF_PROTECTING_DRIVER_IPFILTER_H
