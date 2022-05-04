//Main project file, start and execution point

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

//Local project headers
#include "../Headers/settings.h"
#include "../Headers/console.h"
#include "../Headers/network.h"



//Variables

ULONG start_time_ms;




ULONG get_cur_time_ms_1()
{
    SYSTEMTIME now;
    GetSystemTime(&now);
    return now.wMilliseconds;
}

int main(int argc, char *argv[]){
    int packets_sent=0;
    int result =-1;
    io_welcome();
    char input[]="64.233.161.139"; //TODO:debug
    //io_get_input(input); //TODO:debug

    int packet_size = DEFAULT_PACKET_SIZE;
    int ttl = DEFAULT_TTL;
    int timeout = DEFAULT_TIMEOUT;

    struct ICMPHeader send_buf;
    struct IPHeader *recv_buf = malloc(sizeof(struct IPHeader));

    //init socket
    struct WSAData wsaData;
    SOCKET socket;

    struct sockaddr_in dest_addr;
    struct sockaddr_in source_addr;
    switch(nw_setup(&send_buf,&wsaData,&socket)){ // setup socket and things
        case 1:
            printf("error on network set up\n"); //TODO: error handle
            break;
        case 0:


            switch(nw_get_host(input,&dest_addr)){ //get host address
                case 1:
                    printf("error on host recognition\n"); //TODO: error handle
                    break;
                case 0:
                    packet_size = max(sizeof(struct ICMPHeader), min(MAX_PING_DATA_SIZE, (unsigned int)packet_size));
                    while(packets_sent<MAX_PACKETS_NUM){
                        start_time_ms = get_cur_time_ms_1();
                        printf("\nSending %d bytes to %s ...", packet_size, inet_ntoa(dest_addr.sin_addr)); //TODO: to IO


                        switch(nw_send_request(&socket,&dest_addr,&send_buf, packet_size)){ //send request
                            case 1:
                                printf("error on sending\n"); //TODO: error handle
                                packets_sent++;
                                break;
                            case 0:
                                packets_sent++;
                                switch(nw_get_reply(&socket,&source_addr,recv_buf,packet_size, start_time_ms)){
                                    case 1:
                                        printf("error on getting reply\n"); //TODO: error handle
                                        break;
                                    case 0:
                                        printf(" all good\n");
                                        break;

                                }
                        }
                    }
                    break;
            }
    }
    WSACleanup();
    free(recv_buf);
    printf("memory clear\n");
    return 0;
}
