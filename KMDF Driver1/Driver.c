#include <ntddk.h>
#include "IPFilter.h"

#define LINK_NAME    L"\\DosDevices\\ZoomPlus"
#define DEVICE_NAME    L"\\Device\\test"
#define IOCTL_TEST    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1000,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_START_IP_HOOK    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1001,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_STOP_IP_HOOK    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1002,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ADD_FILTER    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1003,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_CLEAR_FILTER    CTL_CODE(FILE_DEVICE_UNKNOWN,0x1004,METHOD_BUFFERED,FILE_ANY_ACCESS)

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

    return returnStatus;
}
