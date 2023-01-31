#pragma once

#ifdef _WIN32
#include <windows.h>
typedef DWORD WINAPI ThreadHandler(LPVOID)
#define THREAD_HANDLER(name) DWORD WINAPI name(LPVOID data)
LPHANDLE start_thread(ThreadHandler fn, LPVOID data);
void close_threads();
#else
#include <pthread.h>
#endif