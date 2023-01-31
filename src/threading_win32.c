#include "threading.h"

#define HEAP_FLAGS (HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS)

static int nThreads = 0;
static HANDLE* hThreadArray = NULL;
static DWORD* dwThreadIdArray = NULL;

LPHANDLE start_thread(ThreadHandler fn, LPVOID data)
{
	nThreads++;
	if(!hThreadArray)
	{
		hThreadArray = HeapAlloc(GetProcessHeap(), HEAP_FLAGS, nThreads * sizeof(void*));
	} else
	{
		hThreadArray = HeapReAlloc(GetProcessHeap(), HEAP_FLAGS, hThreadArray, nThreads * sizeof(void*));
	}
	if(!dwThreadIdArray)
	{
		dwThreadIdArray = HeapAlloc(GetProcessHeap(), HEAP_FLAGS, nThreads * sizeof(DWORD));
	} else
	{
		dwThreadIdArray = HeapReAlloc(GetProcessHeap(), HEAP_FLAGS, dwThreadIdArray, nThreads * sizeof(DWORD));
	}

	LPHANDLE lpThread = hThreadArray + (nThreads - 1);
	LPDWORD lpThreadId = dwThreadIdArray + (nThreads - 1);
	*lpThread = CreateThread(
		NULL,
		0,
		fn,
		data,
		0,
		lpThreadId
	);
	if (*lpThread == NULL)
	{
		RaiseException(1, EXCEPTION_NONCONTINUABLE, 0, NULL);
	}
	return lpThread;
}

void close_threads()
{
		
}