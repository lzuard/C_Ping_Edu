//Файл с сетевыми функциями

//Подключение заголовочных файлов проекта
#include "../Headers/network.h" //Объявление сетевых функций
#include "../Headers/utils.h"   //Вспомогательные функции

//Функции
//Функция получения Ip-адреса из доменного имени
int nw_get_ip(char *host,struct sockaddr_in *dest_addr, int *program_error_code){
    struct hostent *ip_buf; //Вспомогательный буфер для временной записи адреса

    //Получаем ip
    ip_buf = gethostbyname(host);
    if(ip_buf != 0) //Если удалось
    {
        //Копируем адрес в основной буфер
        memcpy(&(dest_addr->sin_addr), ip_buf->h_addr_list[0], ip_buf->h_length);
        dest_addr->sin_family = ip_buf->h_addrtype;
        return 0;
    }
    else //В случае ошибки
    {
        *program_error_code=102;
        return 1;
    }
}
int nw_check_host(char *host, int ttl, struct sockaddr_in *dest_addr,struct WSAData *wsaData,SOCKET *ping_socket, int *program_error_code){
    unsigned long ip_address;
    //printf("[Debug info] Host name is: %s\n",host);///////////////////////////////////////////////////////////////////

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

                    //printf("[Debug info] Host recognized as an IPv4 address: %s\n", inet_ntoa(dest_addr->sin_addr));//////////////////////////////////////////////////////////////////////////////////
                    return 0;
                } else return 1; //not an ip, will check if it's a domain later
            }
        }
    }
    *program_error_code=101;
    return 2;
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
    send_buf.checksum = u_ip_checksum((USHORT *) &send_buf, packet_size);

    *bytes_sent = sendto(ping_socket, (char *)&send_buf, packet_size, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    //printf("[Debug info] %d bytes sent to %s\n",*bytes_sent, inet_ntoa(dest_addr.sin_addr)); //////////////////////////////////////////////////////////

    if (*bytes_sent == SOCKET_ERROR){
        printf("sent errno #%d, wsa #%d\n",errno,WSAGetLastError());
        *program_error_code=103;
        return 1;
    }
    else return 0;
}

int nw_get_reply(SOCKET ping_socket,struct sockaddr_in source_addr, struct IPHeader *recv_buf,int packet_size, int *program_error_code, int *result)
{
    //Local variables definitions
    struct timeval time_for_timout;     //Max waiting time for reply
    struct ICMPHeader *header;          //Header of received ICMP packet
    fd_set socket_descriptor;           //Socket descriptor for select
    int source_addr_len;                //Source address length
    unsigned short header_len;          //Received packet header length


    //Local variables initialization
    time_for_timout.tv_sec = 1;
    time_for_timout.tv_usec = (1000 % 1000) * 100000;
    source_addr_len = sizeof(source_addr);

    //printf("[Debug info] Waiting a reply from  %s\n", inet_ntoa(source_addr.sin_addr));

    //Receiving
    while(1)
    {
        FD_ZERO(&socket_descriptor);
        FD_SET(ping_socket, &socket_descriptor);

        switch(select(ping_socket+1, &socket_descriptor, 0, 0, &time_for_timout))
        {
            case 0:
                *program_error_code=-1;
                return 1;
            case -1:
                *program_error_code=105;
                return 1;
            default:
                //printf("[Debug info] Select passed\n");//////////////////////////////////////////////////////
                if(recvfrom(ping_socket, (char*) recv_buf,packet_size+sizeof(struct IPHeader),0,(struct sockaddr*) &source_addr, &source_addr_len) != SOCKET_ERROR)
                {
                    //printf("[Debug info] Socket received packet\n");/////////////////////////////////////////////////
                    //TODO: maybe check sq_no

                    header_len = recv_buf->h_len * 4;
                    header = (struct ICMPHeader *)((char *)recv_buf + header_len);
                    *result=header->type;
                    return 0;
                }
                else //recv=socket_error
                {
                    *program_error_code=106;
                    return 1;
                }
        }
    }
}



