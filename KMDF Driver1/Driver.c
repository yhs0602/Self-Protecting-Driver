#include <ntddk.h>
#include <ntstrsafe.h>
#include "IPFilter.h"   
#include "PSFilter.h"

NTSTATUS InstallProcessProtect();
VOID UnInstallProcessProtect();

#define MAX_PATH 260

#define LINK_NAME    L"\\DosDevices\\ZoomPlus"
#define DEVICE_NAME    L"\\Device\\test"
#define IOCTL_TEST    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1000,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_START_IP_HOOK    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1001,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_STOP_IP_HOOK    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1002,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ADD_FILTER    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1003,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_CLEAR_FILTER    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1004,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_BLOCK_PROCESS    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1005,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_UNBLOCK_PROCESS    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1006,METHOD_BUFFERED,FILE_ANY_ACCESS)

PDEVICE_OBJECT MyDevice;
UNICODE_STRING DeviceLink;
UNICODE_STRING DeviceName;


NTSTATUS MyIOControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION pStack;
    NTSTATUS returnStatus = STATUS_SUCCESS;
    ULONG ControlCode;

    pStack = IoGetCurrentIrpStackLocation(Irp);
    ControlCode = pStack->Parameters.DeviceIoControl.IoControlCode;
    DbgPrint("\n IOCTL Call~~ \n");
    Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;

    switch (ControlCode) {
        case IOCTL_TEST:
            DbgPrint("\n IOCTL_TEST Call~~ \n");
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;
        case IOCTL_START_IP_HOOK:
            DbgPrint("\n IOCTL_START Call~~! \n");
            SetFilterFunction(cbFilterFunction); // Register filter function
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;
        case IOCTL_STOP_IP_HOOK:
            DbgPrint("\n IOCTL_STOP Call~~ \n");
            SetFilterFunction(NULL); // Unregister
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;
        case IOCTL_ADD_FILTER:
            if (pStack->Parameters.DeviceIoControl.InputBufferLength == sizeof(IPFilter))
            {
                IPFilter* nf;

                nf = (IPFilter*)Irp->AssociatedIrp.SystemBuffer;

                AddFilterToList(nf);
                DbgPrint("\n IOCTL_ADD_FILTER Call~~ %u %u \n", nf->destinationIp, nf->sourceIp);
            }
            else {
                Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
                DbgPrint("\n IOCTL ERROR ROR ROR OR  Call~~ req: %u real: %u \n", sizeof(IPFilter), pStack->Parameters.DeviceIoControl.InputBufferLength);

            }
            //    AddFilterToList(addrs[0], addrs[1]);
            
            break;
        case IOCTL_CLEAR_FILTER:
            ClearFilterList();
            DbgPrint("\n IOCTL CLEAR   Call~~ \n");

            break;
        case IOCTL_BLOCK_PROCESS:
            DbgPrint("\n IOCTL PROCESS   Call~~ \n");

            if (pStack->Parameters.DeviceIoControl.InputBufferLength <= MAX_PATH) {
                DbgPrint("\n IOCTL PROCESS len OK~~ \n");
                size_t len;
                
                RtlStringCbLengthW((const wchar_t*)Irp->AssociatedIrp.SystemBuffer, MAX_PATH, &len);
                if (len >= MAX_PATH) {
                    DbgPrint("\n IOCTL Add Len too long~~ \n");

                    Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
                    break;
                }
                const char* buf_char = (const char*)Irp->AssociatedIrp.SystemBuffer;
                DbgPrintEx(0, DPFLTR_ERROR_LEVEL, "size is : %d\n", pStack->Parameters.DeviceIoControl.InputBufferLength);
                DbgPrintEx(0, DPFLTR_ERROR_LEVEL, "Interpreted len: %zu, buf: %S\n", len, (const wchar_t*)Irp->AssociatedIrp.SystemBuffer);
                for (unsigned int i = 0; i < pStack->Parameters.DeviceIoControl.InputBufferLength; i++) {
                    DbgPrintEx(0, DPFLTR_ERROR_LEVEL, "Path %u: %c\n", i ,buf_char[i]);
                }
                //DbgPrintEx(0, DPFLTR_ERROR_LEVEL, "Path: %c %c %c %c %c %c\n", buf_char[0], buf_char[1], buf_char[2], buf_char[3], buf_char[4], buf_char[5]);
                AddToBlockProcessList((const wchar_t*)Irp->AssociatedIrp.SystemBuffer, len);
                DbgPrint("\n IOCTL Add Success~~ \n");
            }
            break;
        case IOCTL_UNBLOCK_PROCESS:
            DbgPrint("\n IOCTL UNPROCESS Call~~ \n");

            if (pStack->Parameters.DeviceIoControl.InputBufferLength <= MAX_PATH) {
                DbgPrint("\n IOCTL UNPROCESS len OK~~ \n");
                size_t len;
                RtlStringCbLengthW((const wchar_t*)Irp->AssociatedIrp.SystemBuffer, MAX_PATH, &len);
                if (len >= MAX_PATH) {
                    DbgPrint("\n IOCTL UNblock Len too long~~ \n");

                    Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
                    break;
                }
                RemoveFromBlockProcessList((const wchar_t*)Irp->AssociatedIrp.SystemBuffer);
                DbgPrint("\n IOCTL UNBLock Success~~ \n");
            }
            break;
        default:
            DbgPrint("\n IOCTL ELSE~~ \n");

            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            break;
    }
    DbgPrint("\n IOCTL DONE~~ \n");

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return returnStatus;
}

NTSTATUS Create_Handler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

VOID OnUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    SetFilterFunction(NULL);
    ClearFilterList();
    UnInstallProcessProtect();
    IoDeleteDevice(MyDevice);
    IoDeleteSymbolicLink(&DeviceLink);

    //DbgPrint("OnUnload Call! \n");
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS returnStatus = STATUS_SUCCESS;

    RtlInitUnicodeString(&DeviceLink, LINK_NAME);
    RtlInitUnicodeString(&DeviceName, DEVICE_NAME);

    returnStatus = IoCreateDevice(
            DriverObject,
            0,
            &DeviceName,
            FILE_DEVICE_UNKNOWN,
            FILE_DEVICE_SECURE_OPEN,
            FALSE,
            &MyDevice
    );
    if (!NT_SUCCESS(returnStatus)) {
        //DbgPrint("IoCreateDevice Fail! \n");
        return returnStatus;
    }
    // DbgPrint("Success IoCreateDevice \n");

    returnStatus = IoCreateSymbolicLink(&DeviceLink, &DeviceName);
    if (!NT_SUCCESS(returnStatus)) {
        //DbgPrint("IoCreateSymbolicLink Fail! \n");
        return returnStatus;
    }
    // DbgPrint("Success IoCreateSymbolicLink \n");

    DriverObject->DriverUnload = OnUnload;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyIOControl;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = Create_Handler;
    returnStatus = InstallProcessProtect();
    return returnStatus;
}
