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
ULONG get_cur_time_ms()
{
    SYSTEMTIME now;
    GetSystemTime(&now);
    return now.wMilliseconds;
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

int nw_send_request(SOCKET socket, const struct sockaddr_in *dest_addr, struct ICMPHeader *send_buf, int packet_size){
    int bwrote = sendto(socket, (char *)send_buf, packet_size, 0,(struct sockaddr *)dest_addr, sizeof(*dest_addr));
    if (bwrote == SOCKET_ERROR){
        //printf("Send failed: %d\n", WSAGetLastError()); //TODO: io
        return 1;
    }
    return 0;
}

int nw_get_reply(SOCKET socket, struct sockaddr_in *source_addr, struct IPHeader *recv_buf, int packet_size, ULONG start_time_ms){
    struct timeval timeval;
    timeval.tv_sec = 1;
    timeval.tv_usec = (1000 % 1000) * 1000;
    int fromlen=sizeof(*source_addr);
    int recv_result;
    unsigned short header_len;

    while(1) {
        //socket descriptor for select
        fd_set socket_fd;
        FD_ZERO(&socket_fd);
        FD_SET(socket, &socket_fd);

        //wait for reply
        int wait_result = select(socket + 1, &socket_fd, 0, 0, &timeval);
        switch (wait_result) {
            case 0:
                return 1; //timeout
            case -1:
                return 2; //select error
            default:
                switch (recvfrom(socket, (char *) recv_buf, packet_size + sizeof(struct IPHeader), 0,
                                 (struct sockaddr *) source_addr, &fromlen)) { // read reply
                    case SOCKET_ERROR:
                        if (WSAGetLastError() == WSAEMSGSIZE) {
                            //printf("buffer too small\n"); //TODO: error handle
                        } else {
                            //printf("Error #%d\n", WSAGetLastError());
                        }
                        return 3;
                    default:
                        // Skip ahead to the ICMP header within the IP packet
                        header_len = recv_buf->h_len * 4;
                        struct ICMPHeader *icmphdr = (struct ICMPHeader *) ((char *) recv_buf + header_len);

                        // Make sure the reply is sane
                        if (packet_size < header_len + ICMP_MIN) {
                            printf("Error\n");
                            return -1;
                        } else if (icmphdr->type != ICMP_ECHO_REPLY) {
                            if (icmphdr->type != ICMP_TTL_EXPIRE) {
                                if (icmphdr->type == ICMP_DEST_UNREACH) {
                                    printf("Dest unreachable");
                                } else {
                                    printf("Unknown ICMP packet type received");
                                }
                                return -1;
                            }
                            // If "TTL expired", fall through.  Next test will fail if we
                            // try it, so we need a way past it.
                        } else if (icmphdr->id != (USHORT) GetCurrentProcessId()) {
                            // Must be a reply for another pinger running locally, so just
                            // ignore it.
                            return -2;
                        }
                        printf("\n%d Bytes from %s, ", packet_size, inet_ntoa(source_addr->sin_addr));
                        if (icmphdr->type == ICMP_TTL_EXPIRE) {
                            printf("TTL expired.\n");
                            return 0;
                        } else {
                            printf("TTL %d, time %u ms.", recv_buf->ttl, (GetTickCount() - icmphdr->timestamp));
                            return 0;
                        }

                }
        }
    }
}

