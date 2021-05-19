#include <ntifs.h>
#include "PSFilter.h"

UNICODE_STRING ExecutableBlocked[] = {
	RTL_CONSTANT_STRING(L"*OLLYDBG*.EXE"),
	RTL_CONSTANT_STRING(L"*MSPAINT*.EXE"),
	RTL_CONSTANT_STRING(L"*CALC*.EXE")
};

VOID ProcessNotifyCallbackEx(
	PEPROCESS  Process,
	HANDLE  ProcessId,
	PPS_CREATE_NOTIFY_INFO  CreateInfo)
{

	// ULONG ExecutableCount = sizeof(ExecutableBlocked) / sizeof(UNICODE_STRING);

	BOOLEAN Matched = FALSE;
	// ULONG Idx = 0;

	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(Process);

	if (CreateInfo) {
		PSINGLE_LIST_ENTRY pSig = BlockProcessHead.Next;
		PBLOCKED_PROCESS blocked_process;
		while (pSig != NULL)
		{
			PUNICODE_STRING detectedProcess = (PUNICODE_STRING)CreateInfo->ImageFileName;
			blocked_process = CONTAINING_RECORD(pSig, BLOCKED_PROCESS, list_node);

			if (blocked_process->imagePath.Buffer == NULL) {
				DbgPrintEx(0, DPFLTR_ERROR_LEVEL, "Filter null\n");
				pSig = pSig->Next;
				continue;
			}
			DbgPrintEx(0, DPFLTR_ERROR_LEVEL, "(%wZ) - (%wZ)\n", blocked_process->imagePath, detectedProcess);
			
			if (FsRtlIsNameInExpression(&blocked_process->imagePath, detectedProcess, TRUE, NULL)) {
				Matched = TRUE;
				break;
			}
			pSig = pSig->Next;
		}
		//for (Idx = 0; Idx < ExecutableCount; Idx++) {
		//	if (FsRtlIsNameInExpression(&ExecutableBlocked[Idx], (PUNICODE_STRING)CreateInfo->ImageFileName, TRUE, NULL)) {
		//		Matched = TRUE;
		//		break;
		//	}
		//}

		if (Matched) {
			DbgPrint("[ PsProtect ] Preventing Process (%wZ) Execution\n", CreateInfo->ImageFileName);
			CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
		}
		else {
			DbgPrint("[ PsProtect ] Starting Process: %wZ\n", CreateInfo->ImageFileName);
		}
	}

	return;
}


NTSTATUS InstallProcessProtect() {
	NTSTATUS Status = STATUS_SUCCESS;

	if (!NT_SUCCESS(Status = PsSetCreateProcessNotifyRoutineEx(ProcessNotifyCallbackEx, FALSE)))
	{
		DbgPrint("[ PsProtect ] [ ERROR ] PsSetCreateProcessNotifyRoutineEx Resistering Failed : (%x)\n", Status);
		return Status;
	}
	else {
		DbgPrint("[ PsProtect ] [ SUCCESS ] PsSetCreateProcessNotifyRoutineEx Resistering Success\n");
	}

	return STATUS_SUCCESS;

}

VOID UnInstallProcessProtect() {
	NTSTATUS Status = STATUS_SUCCESS;
	if (!NT_SUCCESS(Status = PsSetCreateProcessNotifyRoutineEx(ProcessNotifyCallbackEx, TRUE))) {
		DbgPrint("[ PsProtect ] [ ERROR ] PsSetCreateProcessNotifyRoutineEx Unresistering Failed : (%x)\n", Status);
	}
	else {
		DbgPrint("[ PsProtect ] [ SUCCESS ] PsSetCreateProcessNotifyRoutineEx Unresistering Success\n");
	}

}