#include <stdio.h>

void server_init(server *s, unsigned short port) {    
    #ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
    #endif

    FD_ZERO
}