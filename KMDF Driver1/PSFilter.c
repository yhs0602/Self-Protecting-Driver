#include <ntifs.h>
#include <ntstrsafe.h>
#include "PSFilter.h"

SINGLE_LIST_ENTRY BlockProcessHead = { NULL };

void AddToBlockProcessList(const wchar_t * buffer, size_t len) {
    PBLOCKED_PROCESS node = ExAllocatePoolZero(NonPagedPoolNx, sizeof(BLOCKED_PROCESS), '1gag');
    PWSTR path = ExAllocatePoolZero(NonPagedPoolNx, len+2, 'path');
    RtlStringCbCopyW(path, len+2, buffer);
    DbgPrintEx(0, DPFLTR_ERROR_LEVEL, "Copied path: %S <- %S\n", path, buffer);
    RtlInitUnicodeString(&(node->imagePath), path);
    DbgPrintEx(0, DPFLTR_ERROR_LEVEL, "Path: %S\n", node->imagePath.Buffer);
    PushEntryList(&BlockProcessHead, &node->list_node); // insert the new node in our list
}

void RemoveFromBlockProcessList(const wchar_t* buffer) {
    PSINGLE_LIST_ENTRY pSig = BlockProcessHead.Next;
    PBLOCKED_PROCESS blocked_process;

    while (pSig != NULL)
    {
        blocked_process = CONTAINING_RECORD(pSig, BLOCKED_PROCESS, list_node);

        DbgPrintEx(0, DPFLTR_ERROR_LEVEL, "(%wZ) - %S\n", blocked_process->imagePath, buffer);
        UNICODE_STRING pathUnicode;
        RtlInitUnicodeString(&pathUnicode, buffer);
        if (blocked_process->imagePath.Buffer == NULL) {
            pSig = pSig->Next;
            continue;
        }
        if (!RtlCompareUnicodeString(&blocked_process->imagePath, &pathUnicode, TRUE)) {
            DbgPrintEx(0, DPFLTR_ERROR_LEVEL, "Removing (%wZ)\n", blocked_process->imagePath);
            ExFreePool(blocked_process->imagePath.Buffer);
            blocked_process->imagePath.Buffer = NULL;
            blocked_process->imagePath.Length = 0;

            // FIXME: SYNC removing entries
            // RemoveEntryList((PLIST_ENTRY)pSig);
        }
        pSig = pSig->Next;
    }
}

BOOLEAN IsBlockedProcess() {
    return FALSE;
}