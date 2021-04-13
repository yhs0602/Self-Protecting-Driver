//
// Created by 양현서 on 2021/04/13.
// https://www.hackerschool.org/HS_Boards/data/Lib_kernel/1160630745/050104.pdf
// 작성자 김기홍 와우해커 세인트 시큐리티 대표이사 :( & )
// www.wowhacker.com
// www.stsc.co.kr
// https://docs.microsoft.com/en-us/previous-versions/windows/hardware/network/ff562312(v%3Dvs.85)
// https://docs.microsoft.com/en-us/previous-versions/windows/hardware/network/ff548976(v=vs.85)

#include "IPFilter.h"
#include <Pfhook.h>
#include <wdm.h>
#include <Ntddk.h>

NTSTATUS SetFilterFunction(PacketFilterExtensionPtr filterFunction) {
    NTSTATUS status = STATUS_SUCCESS, waitStatus = STATUS_SUCCESS;
    UNICODE_STRING filterName;
    PDEVICE_OBJECT ipDeviceObject = NULL;
    PFILE_OBJECT ipFileObject = NULL;
    PF_SET_EXTENSION_HOOK_INFO filterData;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIRP irp;
    // IpFilterDriver . 장치의 포인터를 가져온다
    RtlInitUnicodeString(&filterName, DD_IPFLTRDRVR_DEVICE_NAME);
    status = IoGetDeviceObjectPointer(&filterName, STANDARD_RIGHTS_ALL, &ipFileObject, &ipDeviceObject);
    if (NT_SUCCESS(status)) {
        // 함수의 인자들과 함께 구조체를 초기화 시켜준다
        filterData.ExtensionPointer = filterFunction;
        // 이벤트를 초기화 시켜준다
        // IpFilterDriver 그러면 로부터 이벤트가 세팅되면
        // 우리는 작업 완료
        KeInitializeEvent(&event, NotificationEvent, FALSE);
        // irp를 만들어서 필터 함수를 내보낼 수 있도록 한다
        irp = IoBuildDeviceIoControlRequest(IOCTL_PF_SET_EXTENSION_POINTER, ipDeviceObject);
        if (irp != NULL) {
            // irp 보냄
            status = IoCallDriver(ipDeviceObject, irp);
            // IpFilterDriver 로부터 이벤트 셋 기다림
            if (status == STATUS_PENDING) {
                waitStatus = KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
                if (waitStatus != STATUS_SUCCESS)
                // 에러
            }
            status = ioStatus.Status;
            if (!NT_SUCCESS(status))
            // 에러
        } else {
            // 실패할 경우 에러를 리턴한다
            status = STATUS_INSUFFICIENT_RESOURCES;
            // IRP 생성 오류
        }
        if (ipFileObject != NULL)
            ObDereferenceObject(ipFileObject);
        ipFileObject = NULL;
        ipDeviceObject = NULL;
    } else
        // 포인터 가져오는 동안 오류
        return status;
}


NTSTATUS MyPacketFilterExtension(
// IP 패킷 헤더
        IN unsigned char *PacketHeader,
// 헤더를 포함하지 않는 패킷
        IN unsigned char *Packet,
// IP 패킷 헤더의 길이를 제외한 패킷 길이
        IN unsigned int PacketLength,
// ( ) 장치 인덱스 몇 번째 장치 인지
// 받은 패킷에 대해서
        IN unsigned int RecvInterfaceIndex,
// ( ) 장치 인덱스 몇 번째 장치 인지
// 보내는 패킷에 대해서
        IN unsigned int SendInterfaceIndex,
// IP 주소 형태
// 장치가 받은 주소
        IN IPAddr RecvLinkNextHop,
// IP 주소 형태
// 장치가 보낼 주소
        IN IPAddr SendLinkNextHop
) {
    return PF_FORWARD;
}

// typedef struct IPHeader { UCHAR iph_verlen; UCHAR iph_tos; USHORT iph_length;
//        USHORT iph_id; USHORT iph_offset; UCHAR iph_ttl; UCHAR iph_protocol; USHORT iph_xsum; ULONG iph_src;
//        ULONG iph_dest; } IPHeader;

// typedef PF_FORWARD_ACTION (*PacketFilterExtensionPtr)(
//// IP 패킷 헤더
//IN unsigned char *PacketHeader,
//// 헤더를 포함하지 않는 패킷
//IN unsigned char *Packet,
//// IP 패킷 헤더의 길이를 제외한 패킷 길이
//IN unsigned int PacketLength,
//// ( ) 장치 인덱스 몇 번째 장치 인지
//// 받은 패킷에 대해서
//IN unsigned int RecvInterfaceIndex,
//// ( ) 장치 인덱스 몇 번째 장치 인지
//// 보내는 패킷에 대해서
//IN unsigned int SendInterfaceIndex,
//// IP 주소 형태
//// 장치가 받은 주소
//IN IPAddr RecvLinkNextHop,
//// IP 주소 형태
//// 장치가 보낼 주소
//IN IPAddr SendLinkNextHop
//);

// PF_FORWARD_ACTION . 은 아래와 같은 결과 값을 가질 수 있게 된다
//• PF_FORWARD
//패킷을 정상적으로 처리 하기 위해 시스템 상의 에 값을 넣는다 넣게 되면 해 IP Stack .
//당 패킷은 처리를 하기 위한 어플리케이션으로 넘어가게 되며 해당 어플리케이션에서는 받
//은 정보를 가지고 적절한 처리를 하게 된다.
//• PF_DROP
//패킷을 드롭 하게 된다 시스템 상의 에 해당 포인터를 넘겨 주지 않고 폐기를
//함으로써 어플리케이션은 해당 패킷을 받지 못하게 된다.
//• PF_PASS
//패킷을 그냥 통과 시킨다 에 넣지는 않지만 시스템 드라이버 내부는 통과 하게 . IP Stack
//된다 하지만 에 값을 넣지 않기 때문에 어플리케이션에서는 정상적인 패킷 데이 . IP Stack
//터를 받지 못하는 것으로 나온다.