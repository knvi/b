#ifndef THREADING_H
#define THREADING_H
#ifdef _WIN32
#include <windows.h>
typedef DWORD WINAPI ThreadHandler(LPVOID);
#define THREAD_HANDLER(name) DWORD WINAPI name(LPVOID data)
LPHANDLE start_thread(ThreadHandler fn, LPVOID data);
void close_threads();
#else
#include <pthread.h>
typedef void *ThreadHandler(void *);
#define THREAD_HANDLER(name) void *name(void *data)
void *start_thread(ThreadHandler fn, void *data);
void close_threads();
#endif
#endif