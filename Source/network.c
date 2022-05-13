//Network connection part
//Headers in network.h



#include "../Headers/network.h"
#include "../Headers/utils.h"




#ifdef _MSC_VER
#pragma pack(1)
#endif

int nw_get_ip(char *host,struct sockaddr_in *dest_addr, int *program_error_code){
    struct hostent *domain_address;
    domain_address = gethostbyname(host);
    if(domain_address!=0){
        memcpy(&(dest_addr->sin_addr),domain_address->h_addr_list[0],domain_address->h_length);
        dest_addr->sin_family = domain_address->h_addrtype;
        printf("[Debug info] Host recognized as a domain name, it's IP address: %s\n", inet_ntoa(dest_addr->sin_addr));//////////////////////////////////////////////////////////////////////////////////
        return 0;
    }
    else
    {
        *program_error_code=102;
        return 1;
    }
}
int nw_check_host(char *host, int ttl, struct sockaddr_in *dest_addr,struct WSAData *wsaData,SOCKET *ping_socket, int *program_error_code){
    unsigned long ip_address;
    printf("[Debug info] Host name is: %s\n",host);///////////////////////////////////////////////////////////////////

    if(WSAStartup(MAKEWORD(2,1), wsaData)==0)   //Init wsa to open socket
    {
        *ping_socket=WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0, 0, 0);   //init socket using wsa
        if(*ping_socket!=INVALID_SOCKET)
        {
            if (setsockopt(*ping_socket, IPPROTO_IP, IP_TTL, (const char *) &ttl, sizeof(ttl)) != SOCKET_ERROR)     //open socket
            {
                ip_address = inet_addr(host);
                if (ip_address != INADDR_NONE) // case ipV4
                {
                    dest_addr->sin_family = AF_INET;
                    dest_addr->sin_addr.s_addr = ip_address;

                    printf("[Debug info] Host recognized as an IPv4 address: %s\n", inet_ntoa(dest_addr->sin_addr));//////////////////////////////////////////////////////////////////////////////////
                    return 0;
                } else return 1; //not an ip, will check if it's a domain later
            }
        }
    }
    *program_error_code=101;
    return 2;
}
//int nw_get_host(char *host, struct sockaddr_in *dest_addr){
//
//    unsigned int addr = inet_addr(host);
//
//    if (addr != INADDR_NONE){ //IPv4
//        dest_addr->sin_addr.s_addr = (u_long)addr;
//
//        dest_addr->sin_family = AF_INET;
//    }
//    else{
//        struct hostent *hp = gethostbyname(host);
//        if (hp != 0){ //Domain name
//            memcpy(&(dest_addr->sin_addr), hp->h_addr, hp->h_length);
//            dest_addr->sin_family = hp->h_addrtype;
//        }
//        else{ //Wrong name
//            return -1; //TODO: host name error;
//        }
//    }
//    return 0;
//}

int nw_setup(struct ICMPHeader *icmpHeader,  struct WSAData *wsaData, SOCKET* socket){
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

int nw_send_request(SOCKET ping_socket, struct sockaddr_in dest_addr, struct ICMPHeader send_buf, int packet_size, int *program_error_code, int *bytes_sent){

    packet_size = max(sizeof(struct ICMPHeader),min(MAX_PING_DATA_SIZE, (unsigned int)packet_size));

    send_buf.code = 0;
    send_buf.type = ICMP_ECHO_REQUEST;
    send_buf.code = 0;
    send_buf.checksum = 0;
    send_buf.id = (USHORT)GetCurrentProcessId();
    send_buf.seq = 0;
    send_buf.timestamp = GetTickCount();
    send_buf.checksum = ip_checksum((USHORT *)&send_buf, packet_size);

    *bytes_sent = sendto(ping_socket, (char *)&send_buf, packet_size, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    printf("[Debug info] %d bytes sent to %s\n",*bytes_sent, inet_ntoa(dest_addr.sin_addr)); //////////////////////////////////////////////////////////

    if (*bytes_sent == SOCKET_ERROR){
        printf("sent errno #%d, wsa #%d\n",errno,WSAGetLastError());
        *program_error_code=103;
        return 1;
    }
    else return 0;
}

int nw_get_reply(SOCKET ping_socket,struct sockaddr_in source_addr, struct IPHeader *recv_buf,int packet_size, int *program_error_code)
{
    struct timeval time_for_timout;
    fd_set socket_descriptor;
    int receive_result;
    int fromlen;

    time_for_timout.tv_sec = 5;
    time_for_timout.tv_usec = (1000 % 1000) * 100000;
    fromlen = sizeof(source_addr);

    printf("[Debug info] Waiting a reply from  %s\n", inet_ntoa(source_addr.sin_addr));
    while(1)
    {
        FD_ZERO(&socket_descriptor);
        FD_SET(ping_socket, &socket_descriptor);

        switch(select(ping_socket+1, &socket_descriptor, 0, 0, &time_for_timout))
        {
            case 0:
                printf("select error\n");
                return 1;
            case -1:
                printf("timeout, errno: %d\n",errno);
                return 1;
            default:
                printf("[Debug info] Select passed\n");//////////////////////////////////////////////////////
                if(recvfrom(ping_socket, (char*) recv_buf,packet_size+sizeof(struct IPHeader),0,(struct sockaddr*) &source_addr, &fromlen)!=SOCKET_ERROR){
                    printf("[Debug info] Socket received packet\n");/////////////////////////////////////////////////
                    return 0;
                }
                else
                {
                    printf("error on recv\n");
                    return 10;
                }


        }
    }
}

int nw_get_repl(SOCKET* socket, struct sockaddr_in *source_addr, struct IPHeader *recv_buf, int packet_size, ULONG start_time_ms){
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
        FD_SET(*socket, &socket_fd);

        //wait for reply
        int wait_result = select(*socket + 1, &socket_fd, 0, 0, &timeval);
        switch (wait_result) {
            case 0:
                return 1; //timeout
            case -1:
                return 2; //select error
            default:
                switch (recvfrom(*socket, (char *) recv_buf, packet_size + sizeof(struct IPHeader), 0,(struct sockaddr *) source_addr, &fromlen)) { // read reply
                    case SOCKET_ERROR:
                        if (WSAGetLastError() == WSAEMSGSIZE) {
                            printf("buffer too small\n"); //TODO: error handle
                        } else {
                            printf("Error #%d\n", WSAGetLastError());
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


