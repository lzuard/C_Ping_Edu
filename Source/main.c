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
char input[STR_SIZE]="192.168.0.1"; //TODO:debug


int main(int argc, char *argv[]){
    io_welcome();
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

    switch(nw_get_host(input,&dest_addr)){ //get host address
        case 1:
            printf("error on host recognition\n"); //TODO: error handle
            break;
        case 0:
            switch(nw_setup(&send_buf,&wsaData,&socket)){ // setup socket and things
                case 1:
                    printf("error on network set up\n"); //TODO: error handle
                    break;
                case 0:
                    printf("all good\n");
                    break;
            }
    }
    return 0;
}
