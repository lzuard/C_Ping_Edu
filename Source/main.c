//Main project file, start and execution point

//Standard C libraries
#include <winsock2.h>   // Sockets library
#include <windows.h>    // Win32 api functions
#include <ws2tcpip.h>   // TODO: delete
#include <iphlpapi.h>   // TODO: delete
#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard functions TODO: change desc

//Local project headers
#include "../Headers/settings.h"    // TODO: delete
#include "../Headers/console.h"     // TODO: delete
#include "../Headers/network.h"     // Network functions
#include "../Headers/utils.h"       // Helpful utils
#include "../Headers/logs.h"        // Log functions


//Variables
ULONG start_time_ms;        // Ping send time in ms
SOCKET ping_socket;         // Socket for network communication
int packets_sent = 0;       // Number of sent packets
int max_packets_sent = 5;   // Maximum number of packets to be sent
int packet_size = 1024;     // ICMP packet size
int bytes_sent=0;           // Bytes have been sent
int ttl = 30;               // ICMP packet TTL in ms
int timeout = 1000;         // Timout for echo-reply waiting in ms
int program_error_code = 0; // Program execution error code
int log_error_code = -1;    // Log work error code

char params_address[]="google.com";    // User input destination address TODO:debug delete arrays
char params_log_path[]="nothing";       // User input log file path TODO:debug delete arrays

//Structures
struct ICMPHeader send_buf;     // Buffer for send packet
struct IPHeader *recv_buf;      // Buffer for receive packet
struct WSAData wsaData;         // WSA data struct for socket initialization
struct sockaddr_in dest_addr;   // Destination address
struct sockaddr_in source_addr; // Local device address


void stop_program()
{
    WSACleanup();
    free(recv_buf);
    printf("Program stopped ");
    switch(program_error_code){
        case 0:
            printf("\n");
            break;
        case 101:
            printf("due to socket open error #%d\n", WSAGetLastError());
            break;
        case 102:
            printf("due to error on host recognition\n");
            break;
        case 103:
            printf("due to sending error\n");
            break;
        default:
            printf("due to unknown issue\n");
            break;
    }
    switch(log_error_code){
        case -1:
            printf("Log file wasn't opened\n");
            break;
        case 0:
            printf("All logs have been saved in %s\n",params_log_path);
            break;
        default:
            printf("Unknown issue caused error on writing logs\n");
    }
    exit(program_error_code);
}


void main(int argc, char *argv[])
{

    recv_buf = malloc(sizeof(struct IPHeader));


    switch(u_check_params(params_address))  //check params
    {
        case 1:
            printf("wrong params\n");
            stop_program();
            break;
        case 0:
            switch(log_open_file(params_log_path))      //Open log file
            {
                case 1:
                    log_diagnostics();
                    stop_program();
                    break;
                case 0:
                    switch(nw_check_host(params_address,ttl,&dest_addr,&wsaData,&ping_socket, &program_error_code))      // check host
                    {
                        case 2:                                                                                                         //error on host check
                            if(log_write()!=0){
                                log_diagnostics();
                            }
                            stop_program();
                            break;
                        case 1:                                                                                                 // host is domain
                            switch(nw_get_ip(params_address,&dest_addr,&program_error_code))        // get ip
                            {
                                case 1:
                                    printf("wrong host");
                                    if(log_write()!=0){
                                        log_diagnostics();
                                    }
                                    stop_program();
                                case 0:
                                    while (packets_sent<max_packets_sent) {
                                        switch (nw_send_request(&ping_socket, &dest_addr, &send_buf, packet_size,
                                                                &program_error_code, &bytes_sent)) //send
                                        {
                                            case 1:
                                                printf("error on sent\n");
                                                if (log_write() != 0) {
                                                    log_diagnostics();
                                                }
                                                stop_program();
                                            case 0:
                                                printf("sent %d bytes\n", bytes_sent);
                                                break;
                                        }
                                    }
                            }
                            break;
                        case 0: // ipv4
                            printf("all good, %d\n", program_error_code);
                            break;
                    }
            }
    }
    stop_program();

//    switch(nw_setup(&send_buf,&wsaData,&ping_socket)){ // setup ping_socket and things
//        case 1:
//            printf("error on network set up\n"); //TODO: error handle
//            break;
//        case 0:
//            switch(nw_get_host(params_address, &dest_addr))
//            { //get host address
//                case 1:
//                    printf("error on host recognition\n"); //TODO: error handle
//                    break;
//                case 0:
//                    packet_size = max(sizeof(struct ICMPHeader), min(MAX_PING_DATA_SIZE, (unsigned int)packet_size));
//                    while(packets_sent<MAX_PACKETS_NUM)
//                    {
//                        start_time_ms = u_get_cur_time_ms();
//                        printf("\nSending %d bytes to %s ...", packet_size, inet_ntoa(dest_addr.sin_addr)); //TODO: to IO
//
//
//                        switch(nw_send_request(&ping_socket, &dest_addr, &send_buf, packet_size))
//                        { //send request
//                            case 1:
//                                printf("error on sending\n"); //TODO: error handle
//                                packets_sent++;
//                                break;
//                            case 0:
//                                packets_sent++;
//                                switch(nw_get_reply(&ping_socket, &source_addr, recv_buf, packet_size, start_time_ms)){
//                                    case 1:
//                                        printf("error on getting reply\n"); //TODO: error handle
//                                        break;
//                                    case 0:
//                                        printf(" all good\n");
//                                        break;
//
//                                }
//                        }
//                    }
//                    break;
//            }
//    }

}
