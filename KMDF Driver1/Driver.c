#include <ntddk.h>

#define LINK_NAME	L"\\DosDevices\\ZoomPlus"
#define DEVICE_NAME	L"\\Device\\test"
#define IOCTL_TEST	CTL_CODE(FILE_DEVICE_UNKNOWN,0x4000,METHOD_BUFFERED,FILE_ANY_ACCESS)

PDEVICE_OBJECT MyDevice;
UNICODE_STRING DeviceLink;
UNICODE_STRING DeviceName;

NTSTATUS MyIOControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION pStack;
	NTSTATUS returnStatus = STATUS_SUCCESS;
	ULONG ControlCode;

	pStack = IoGetCurrentIrpStackLocation(Irp);
	ControlCode = pStack->Parameters.DeviceIoControl.IoControlCode;
	DbgPrint("\n IOCTL Call~~ \n");

	switch (ControlCode)
	{
	case IOCTL_TEST:
		DbgPrint("\n IOCTL_TEST Call~~ \n");
		break;
	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return returnStatus;
}

NTSTATUS Create_Handler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

VOID OnUnload(IN PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	IoDeleteDevice(MyDevice);
	IoDeleteSymbolicLink(&DeviceLink);

	//DbgPrint("OnUnload Call! \n");
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
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
	if (!NT_SUCCESS(returnStatus))
	{
		//DbgPrint("IoCreateDevice Fail! \n");
		return returnStatus;
	}
	//DbgPrint("Success IoCreateDevice \n");

	returnStatus = IoCreateSymbolicLink(&DeviceLink, &DeviceName);
	if (!NT_SUCCESS(returnStatus))
	{
		//DbgPrint("IoCreateSymbolicLink Fail! \n");
		return returnStatus;
	}
	//DbgPrint("Success IoCreateSymbolicLink \n");

	DriverObject->DriverUnload = OnUnload;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyIOControl;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = Create_Handler;

	return returnStatus;
}
