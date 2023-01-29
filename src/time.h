#ifdef _WIN32
#ifndef WIN32TIME_H
#define WIN32TIME_H
#include <sys/utime.h>
void usleep(unsigned useconds);
void gettimeofday(struct timeval* tp, struct timezone* tzp);
#endif
#else
#include <sys/time.h>
#include <unistd.h>
#endif