//Main project file, start and execution point

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "user32.lib")
//Standard C libraries
#include <winsock2.h>   // Sockets library
#include <windows.h>    // Win32 api functions
#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Standard functions TODO: change desc

//Local project headers

#include "../Headers/network.h"     // Network functions
#include "../Headers/utils.h"       // Helpful utils
#include "../Headers/logs.h"        // Log functions


//Variables
ULONG start_time_ms;        // Ping send time in ms
SOCKET ping_socket;         // Socket for network communication
int packets_sent = 0;       // Number of sent packets
int max_packets_sent = 4;   // Maximum number of packets to be sent
int packet_size = 32;     // ICMP packet size
int bytes_sent=0;           // Bytes have been sent
int ttl = 128;              // ICMP packet TTL
int result=0;               // ICMP packet decode result
int timeout = 1000;         // Timout for echo-reply waiting in ms
int program_error_code = 0; // Program execution error code
int log_error_code = -1;    // Log work error code

char *params_address;    // User input destination address
char *params_log_path;       // User input log file path

//Structures
struct ICMPHeader send_buf;     // Buffer for send packet
struct IPHeader *recv_buf;      // Buffer for receive packet
struct WSAData wsaData;         // WSA data struct for socket initialization
struct sockaddr_in dest_addr;   // Destination address
struct sockaddr_in source_addr; // Local device address


void stop_program()
{
    printf("Program stopped ");
    switch(program_error_code){
        case 0:
            printf("");
            break;
        case 101:
            printf("due to socket open error #%d ", WSAGetLastError());
            break;
        case 102:
            printf("due to error on host recognition ");
            break;
        case 103:
            printf("due to sending WSA error %d ", WSAGetLastError());
            break;
        case 104:
            printf("due to wrong ICMP packet checksum\n");
            break;
        case 105:
            printf("due to reply waiting timeout\n");
            break;
        case 106:
            printf("due to receiving reply, WSA error %d\n",WSAGetLastError());
            break;
        case 107:
            printf("due to invalid parameters. Usage: ping [host] [log file full path]\n");
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
    exit(0);
}


int main(int argc, char *argv[])
{
    recv_buf = malloc(sizeof(struct IPHeader));

    switch(u_check_params(argc, argv, &params_address, &params_log_path))  //check params
    {
        case 1:
            program_error_code=107;
            stop_program();
            break;
        case 0:
            printf("adr: %s, log:%s\n",params_address,params_log_path);//////////////////////////////////////////////////////
            switch(log_open_file(params_log_path))      //Open log file
            {
                case 1:
                    log_diagnostics(log_error_code);
                    stop_program();
                    break;
                case 0:
                    switch(nw_check_host(params_address,ttl,&dest_addr,&wsaData,&ping_socket, &program_error_code))      // check host
                    {
                        case 2:                                                                                                         //error on host check
//                            if(log_write()!=0){
//                                log_diagnostics();
//                            }
//                            stop_program();
                            break;
                        case 1:                                                                                                 // host is domain
                            switch(nw_get_ip(params_address,&dest_addr,&program_error_code))        // get ip
                            {
                                case 1:
//                                    printf("wrong host");
//                                    if(log_write()!=0){
//                                        log_diagnostics();
//                                    }
//                                    stop_program();
                                    break;
                                case 0:
                                    while (packets_sent<max_packets_sent)
                                    {
                                        start_time_ms=u_get_cur_time_ms();
                                        //printf("[Log] Sending packet %d of %d\n",packets_sent+1,max_packets_sent);////////////////////////////////////////////
                                        switch (nw_send_request(ping_socket, dest_addr, send_buf, packet_size,&program_error_code, &bytes_sent)) //send
                                        {
                                            case 1:
//                                                printf("error on sent\n");
//                                                if (log_write() != 0) {
//                                                    log_diagnostics();
//                                                }
//                                                stop_program();
                                                break;
                                            case 0:
                                                packets_sent++;
                                                //printf("sent %d bytes\n", bytes_sent);
                                                switch(nw_get_reply(ping_socket,dest_addr,recv_buf,packet_size,&program_error_code, &result))
                                                {
                                                    case 0:
                                                        nw_show_result(recv_buf,params_address,start_time_ms,result,packet_size);
                                                        //printf("[Log] Got packet %d of %d in %d ms\n",packets_sent,max_packets_sent,u_get_cur_time_ms()-start_time_ms);////////////////////////////////////////////
                                                        break;
                                                    default:
                                                        printf("error on getting reply\n");
                                                        break;
                                                }
                                                break;
                                        }
                                    }
                            }
                            break;
                        case 0:                                                                                                                                         // host is ip
                            while (packets_sent<max_packets_sent)
                            {
                                start_time_ms=u_get_cur_time_ms();
                                //printf("[Log] Sending packet %d of %d\n",packets_sent+1,max_packets_sent);////////////////////////////////////////////
                                switch (nw_send_request(ping_socket, dest_addr, send_buf, packet_size,&program_error_code, &bytes_sent)) //send
                                {
                                    case 1:
//                                        printf("error on sent\n");
//                                        if (log_write() != 0) {
//                                            log_diagnostics();
//                                        }
//                                        stop_program();
                                        break;
                                    case 0:

                                        packets_sent++;
                                        //printf("sent %d bytes\n", bytes_sent);
                                        switch(nw_get_reply(ping_socket,dest_addr,recv_buf,packet_size,&program_error_code, &result))
                                        {
                                            case 0:
                                                nw_show_result(recv_buf,params_address,start_time_ms,result,packet_size);
                                                //printf("[Log] Got packet %d of %d in %d ms\n",packets_sent,max_packets_sent,u_get_cur_time_ms()-start_time_ms);////////////////////////////////////////////
                                                break;
                                            default:
                                                printf("error on getting reply\n");
                                                break;
                                        }
                                        break;
                                }
                            }
                            break;
                    }
            }
    }
    stop_program();
    return 0;
}
