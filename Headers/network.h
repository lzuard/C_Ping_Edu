//Headers for network.c
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>


#include "../Headers/settings.h"

#ifndef PING_NETWORK_H
#define PING_NETWORK_H

#endif //PING_NETWORK_H

struct IPHeader
{
    BYTE h_len : 4;   // Length of the header in dwords
    BYTE version : 4; // Version of IP
    BYTE tos;         // Type of service
    USHORT total_len; // Length of the packet in dwords
    USHORT ident;     // unique identifier
    USHORT flags;     // Flags
    BYTE ttl;         // Time to live
    BYTE proto;       // Protocol number (TCP, UDP etc)
    USHORT checksum;  // IP checksum
    ULONG source_ip;
    ULONG dest_ip;
};
struct ICMPHeader
{
    BYTE type; // ICMP packet type (0=ans, 8=req)
    BYTE code; // Type sub code
    USHORT checksum;
    USHORT id;
    USHORT seq;
    ULONG timestamp; // not part of ICMP, but we need it
};

int nw_get_ip(char *host,struct sockaddr_in *dest_addr, int *program_error_code);
int nw_check_host(char *host, int ttl, struct sockaddr_in *dest_addr,struct WSAData *wsaData,SOCKET *ping_socket, int *program_error_code);
int nw_send_request(SOCKET socket, struct sockaddr_in dest_addr, struct ICMPHeader send_buf, int packet_size, int *program_error_code, int *bytes_sent);
int nw_get_reply(SOCKET ping_socket,struct sockaddr_in source_addr, struct IPHeader *recv_buf,int packet_size, int *program_error_code, int *result);

