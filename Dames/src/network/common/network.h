#ifndef __NETWORK_COMMON_NETWORK_H__
#define __NETWORK_COMMON_NETWORK_H__
#include <sys/socket.h>

#ifdef _DEBUG
#include <stdio.h>
#define write_buffer(buffer, s) printf("send : 0x");for(int q=0;q<s;q++){printf("%02x",buffer[q]);}printf("\n")
#define write_socket(buffer, s, n, sock) send(sock, buffer, n * s, MSG_NOSIGNAL);write_buffer(buffer, s*n)
#else
#define write_socket(buffer, s, n, sock) send(sock, buffer, n * s, MSG_NOSIGNAL)
#endif // _DEBUG

#define read_socket(buffer, s, n, sock) recv(sock, buffer, n * s, MSG_WAITALL)



#endif

