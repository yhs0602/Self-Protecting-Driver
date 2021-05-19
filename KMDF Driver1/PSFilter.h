#pragma once

typedef struct {
	SINGLE_LIST_ENTRY list_node;
	UNICODE_STRING imagePath;
} BLOCKED_PROCESS, *PBLOCKED_PROCESS;

extern SINGLE_LIST_ENTRY BlockProcessHead;


void AddToBlockProcessList(const wchar_t* buffer, size_t len);

void RemoveFromBlockProcessList(const wchar_t* buffer);