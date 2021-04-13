//
// Created by 양현서 on 2021/04/13.
// https://www.hackerschool.org/HS_Boards/data/Lib_kernel/1160630745/050104.pdf
// 작성자 김기홍 와우해커 세인트 시큐리티 대표이사 :( & )
// www.wowhacker.com
// www.stsc.co.kr
// https://docs.microsoft.com/en-us/previous-versions/windows/hardware/network/ff562312(v%3Dvs.85)

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