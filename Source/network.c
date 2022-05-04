//Network connection part
//Headers in network.h
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include "../Headers/network.h"
#include "../Headers/settings.h"




#ifdef _MSC_VER
#pragma pack(1)
#endif


USHORT ip_checksum(USHORT *buffer, int size){
    unsigned long cksum = 0;
    while (size > 1){
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

int nw_get_host(char *host, struct sockaddr_in *dest_addr){

    unsigned int addr = inet_addr(host);

    if (addr != INADDR_NONE){ //IPv4
        dest_addr->sin_addr.s_addr = (u_long)addr;

        dest_addr->sin_family = AF_INET;
    }
    else{
        struct hostent *hp = gethostbyname(host);
        if (hp != 0){ //Domain name
            memcpy(&(dest_addr->sin_addr), hp->h_addr, hp->h_length);
            dest_addr->sin_family = hp->h_addrtype;
        }
        else{ //Wrong name
            return -1; //TODO: host name error;
        }
    }
    return 0;
}
int nw_setup(struct ICMPHeader *icmpHeader,  struct WSAData *wsaData, SOCKET *socket){
    icmpHeader->code = 0;
    icmpHeader->type = ICMP_ECHO_REQUEST;
    icmpHeader->code = 0;
    icmpHeader->checksum = 0;
    icmpHeader->id = (USHORT)GetCurrentProcessId();
    icmpHeader->seq = 0;
    icmpHeader->timestamp = GetTickCount();
    icmpHeader->checksum = ip_checksum((USHORT *)&icmpHeader, DEFAULT_PACKET_SIZE);

    int ttl=DEFAULT_TTL;
    switch(WSAStartup(MAKEWORD(2, 1), wsaData)){
        case 0:
            *socket=WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0, 0, 0);
            switch(*socket){
                case INVALID_SOCKET:
                    return 1; //TODO: open socket error
                default:
                    switch(setsockopt(*socket, IPPROTO_IP, IP_TTL, (const char *) &ttl, sizeof(ttl))){
                        case SOCKET_ERROR:
                            return 1; //TODO: open socket error
                        default:
                            return 0;
                    }
            }
        default:
            return 1; //TODO: wsa data error
    }
}





