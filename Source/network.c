//Network connection part
//Headers in network.h
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include "../Headers/network.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER

#pragma pack(1)
#endif


#define MAX_PING_DATA_SIZE 1024
#define MAX_PING_PACKET_SIZE (MAX_PING_DATA_SIZE + sizeof(struct IPHeader))

// ICMP packet types
#define ICMP_ECHO_REPLY 0
#define ICMP_DEST_UNREACH 3
#define ICMP_TTL_EXPIRE 11
#define ICMP_ECHO_REQUEST 8

// Minimum ICMP packet size, in bytes
#define ICMP_MIN 8

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

// ICMP header
struct ICMPHeader
{
    BYTE type; // ICMP packet type (0=ans, 8=req)
    BYTE code; // Type sub code
    USHORT checksum;
    USHORT id;
    USHORT seq;
    ULONG timestamp; // not part of ICMP, but we need it
};

//int network_try_connect(){
//    WSADATA wsadata; //initializing using Windows sockets
//
//    int iResult = WSAStartup(MAKEWORD(1,1), &wsadata); //linking WS2_32.DLL
//    if (iResult!=0) return 1;
//
//
//}

ULONG get_cur_time_ms()
{
    SYSTEMTIME now;
    GetSystemTime(&now);
    return now.wMilliseconds;
}

USHORT ip_checksum(USHORT *buffer, int size) {
    unsigned long cksum = 0;

    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }
    if (size){
        cksum += *(UCHAR *)buffer;
    }

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);

    return (USHORT)(~cksum);
}

int setup_for_ping(char *host, int ttl, SOCKET *sd, struct sockaddr_in *dest)
{
    *sd = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0, 0, 0);

    if (*sd == INVALID_SOCKET){
        printf("Failed to create raw socket: %d\n", WSAGetLastError());
        return -1;
    }

    if (setsockopt(*sd, IPPROTO_IP, IP_TTL, (const char *)&ttl, sizeof(ttl)) == SOCKET_ERROR){
        printf("TTL setsockopt failed, %d\n", WSAGetLastError());
        return -1;
    }

    // Initialize the destination host info block

    // Turn first passed parameter into an IP address to ping

    unsigned int addr = inet_addr(host);

    if (addr != INADDR_NONE){
        // It was a dotted quad number, so save result
        dest->sin_addr.s_addr = (u_long)addr;

        dest->sin_family = AF_INET;
    }
    else{
        // Not in dotted quad form, so try and look it up
        struct hostent *hp = gethostbyname(host);
        if (hp != 0){
            // Found an address for that host, so save it
            memcpy(&(dest->sin_addr), hp->h_addr, hp->h_length);
            dest->sin_family = hp->h_addrtype;
        }
        else{
            // Not a recognized hostname either!
            printf("Not a recognized hostname");
            return -1;
        }
    }

    return 0;
}

int decode_reply(struct IPHeader *reply, int bytes, struct sockaddr_in *from)
{

    // Skip ahead to the ICMP header within the IP packet
    unsigned short header_len = reply->h_len * 4;
    struct ICMPHeader *icmphdr = (struct ICMPHeader *)((char *)reply + header_len);

    // Make sure the reply is sane
    if (bytes < header_len + ICMP_MIN)
    {
        printf("Error\n");
        return -1;
    }
    else if (icmphdr->type != ICMP_ECHO_REPLY)
    {
        if (icmphdr->type != ICMP_TTL_EXPIRE)
        {
            if (icmphdr->type == ICMP_DEST_UNREACH)
            {
                printf("Dest unreachable");
            }
            else
            {
                printf("Unknown ICMP packet type received");
            }
            return -1;
        }
        // If "TTL expired", fall through.  Next test will fail if we
        // try it, so we need a way past it.
    }
    else if (icmphdr->id != (USHORT)GetCurrentProcessId())
    {
        // Must be a reply for another pinger running locally, so just
        // ignore it.
        return -2;
    }

    printf("\n%d Bytes from %s, ", bytes, inet_ntoa(from->sin_addr));

    if (icmphdr->type == ICMP_TTL_EXPIRE)
    {
        printf("TTL expired.\n");
    }
    else
    {

        printf("TTL %d, time %u ms.", reply->ttl, (GetTickCount() - icmphdr->timestamp));
    }

    return 0;
}

int recv_ping(SOCKET sd, struct sockaddr_in *source, struct IPHeader *recv_buf, int packet_size)
{
    // Wait for the ping reply

    int fromlen = sizeof(*source);

    int bread = recvfrom(sd, (char *)recv_buf,
                         packet_size + sizeof(struct IPHeader), 0,
                         (struct sockaddr *)source, &fromlen);
    if (bread == SOCKET_ERROR)
    {
        printf("Read failed\n");
        if (WSAGetLastError() == WSAEMSGSIZE)
        {

            printf("buffer too small\n");
        }
        else
        {

            printf("Error #%d\n", WSAGetLastError());
        }
        return -1;
    }

    return 0;
}

int send_ping(SOCKET sd, const struct sockaddr_in *dest, struct ICMPHeader *send_buf, int packet_size)
{
    // Send the ping packet in send_buf as-is

    int bwrote = sendto(sd, (char *)send_buf, packet_size, 0,
                        (struct sockaddr *)dest, sizeof(*dest));
    if (bwrote == SOCKET_ERROR)
    {
        printf("Send failed: %d\n", WSAGetLastError());
        return -1;
    }
    else if (bwrote < packet_size)
    {
        printf("Sent %d bytes...", bwrote);
    }

    return 0;
}

void init_ping_packet(struct ICMPHeader *icmp_hdr, int packet_size, int seq_no)
{
    // Set up the packet's fields

    icmp_hdr->code = 0;
    icmp_hdr->type = ICMP_ECHO_REQUEST;
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0;
    icmp_hdr->id = (USHORT)GetCurrentProcessId();
    icmp_hdr->seq = seq_no;
    icmp_hdr->timestamp = GetTickCount();

    icmp_hdr->checksum = ip_checksum((USHORT *)icmp_hdr, packet_size);
}


int network_ping(int packet_size, int ttl, char *host, int timeout){

    int seq_no = 0; //?
    struct ICMPHeader send_buf;
    struct IPHeader *recv_buf = malloc(sizeof(struct IPHeader));
    struct WSAData wsaData; //WSAData initialization

    if (WSAStartup(MAKEWORD(2, 1), &wsaData)!= 0){
        return 1;
    }

    packet_size = max(sizeof(struct ICMPHeader), min(MAX_PING_DATA_SIZE, (unsigned int)packet_size));

    SOCKET sd;
    struct sockaddr_in dest, source;

    if (setup_for_ping(host, ttl, &sd, &dest) < 0)
    {
        // goto cleanup;

        printf("Setup error");
        WSACleanup();
    }

    // Set up the packet's fields
    printf("\nSending %d bytes to %s ...", packet_size, inet_ntoa(dest.sin_addr));

    int result = -1;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = (1000 % 1000) * 1000;
    const ULONG start_time_ms = get_cur_time_ms();

    send_buf.code = 0;

    send_buf.type = ICMP_ECHO_REQUEST;
    send_buf.code = 0;
    send_buf.checksum = 0;
    send_buf.id = (USHORT)GetCurrentProcessId();
    send_buf.seq = seq_no;
    send_buf.timestamp = GetTickCount();

    send_buf.checksum = ip_checksum((USHORT *)&send_buf, packet_size);

    if (send_ping(sd, &dest, &send_buf, packet_size) >= 0)
    {
        while (1)
        {

            fd_set rfd;
            FD_ZERO(&rfd);
            FD_SET(sd, &rfd);

            int n = select(sd + 1, &rfd, 0, 0, &tv);
            if (n == 0)
            {
                result = -1;
                break;
            }
            if (n < 0)
            {
                break;
            }
            const ULONG elapsed_time = (get_cur_time_ms() - start_time_ms);
            if (elapsed_time > timeout)
            {
                result = -1;
                break;
            }
            else
            {
                const int new_timeout = timeout - elapsed_time;
                tv.tv_sec = new_timeout / 1000;
                tv.tv_usec = (new_timeout % 1000) * 1000;
            }
            if (FD_ISSET(sd, &rfd))
            {
                if (recv_ping(sd, &source, recv_buf, MAX_PING_DATA_SIZE) < 0)
                {
                    unsigned short header_len = recv_buf->h_len * 4;
                    struct ICMPHeader *icmphdr = (struct ICMPHeader *)((char *)recv_buf + header_len);
                    if (icmphdr->seq != seq_no)
                    {
                        printf("bad sequence number!\n");
                        continue;
                    }
                    else
                    {
                        result = 2;
                        break;
                    }
                }
                if (decode_reply(recv_buf, packet_size, &source) != -2)
                {
                    // Success or fatal error (as opposed to a minor error)
                    // so take off.
                    result = 0;
                    break;
                }
                result = 0;
                break;
            }
        }
    }

    WSACleanup();
    free(recv_buf);
    return result;
}

