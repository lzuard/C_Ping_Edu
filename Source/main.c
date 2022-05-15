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
int packet_size = 32;       // ICMP packet size
int bytes_sent=0;           // Bytes have been sent
int ttl = 128;              // ICMP packet TTL
int result=0;               // ICMP packet decode result
int timeout = 1000;         // Timout for echo-reply waiting in ms
int program_error_code = 0; // Program execution error code
int log_error_code = 0;    // Log work error code
char *params_address;       // User input destination address
char *params_log_path;      // User input log file path
char *res_buf;

//Structures
struct ICMPHeader send_buf;     // Buffer for send packet
struct IPHeader *recv_buf;      // Buffer for receive packet
struct WSAData wsaData;         // WSA data struct for socket initialization
struct sockaddr_in dest_addr;   // Destination address
struct sockaddr_in source_addr; // Local device address

FILE* log_file;                 // Pointer to the log file


void stop_program()
{
    fclose(log_file);
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
            switch(log_open_file(&log_file,params_log_path,params_address, &program_error_code,&log_error_code))      //Open log file
            {
                case 1:
                    log_diagnostics(log_error_code);
                    stop_program();
                    break;
                case 0:
                    switch(nw_check_host(params_address,ttl,&dest_addr,&wsaData,&ping_socket, &program_error_code))      // check host
                    {
                        case 2:                                                                                                         //error on host check
                            if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                            {
                                log_diagnostics(log_error_code);
                            }
                            stop_program();
                            break;
                        case 1:                                                                                                 // host is domain
                            switch(nw_get_ip(params_address,&dest_addr,&program_error_code))        // get ip
                            {
                                case 1:
                                    if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                                    {
                                        log_diagnostics(log_error_code);
                                    }
                                    stop_program();
                                    break;
                                case 0:
                                    while (packets_sent<max_packets_sent)
                                    {
                                        start_time_ms=u_get_cur_time_ms();
                                        switch (nw_send_request(ping_socket, dest_addr, send_buf, packet_size,&program_error_code, &bytes_sent)) //send
                                        {
                                            case 1:
                                                if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                                                {
                                                    log_diagnostics(log_error_code);
                                                }
                                                stop_program();
                                                break;
                                            case 0:
                                                packets_sent++;
                                                switch(nw_get_reply(ping_socket,dest_addr,recv_buf,packet_size,&program_error_code, &result))
                                                {
                                                    case 0:
                                                        u_show_result(result, inet_ntoa(dest_addr.sin_addr),u_get_cur_time_ms()-start_time_ms,packet_size,recv_buf->ttl);
                                                        if(log_write_result(log_file,result, inet_ntoa(dest_addr.sin_addr),u_get_cur_time_ms()-start_time_ms,packet_size,recv_buf->ttl)!=0)
                                                        {
                                                            log_diagnostics(log_error_code);
                                                            stop_program();
                                                        }
                                                        break;
                                                    default:
                                                        if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                                                        {
                                                            log_diagnostics(log_error_code);
                                                        }
                                                        stop_program();
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
                                switch (nw_send_request(ping_socket, dest_addr, send_buf, packet_size,&program_error_code, &bytes_sent)) //send
                                {
                                    case 1:
                                        if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                                        {
                                            log_diagnostics(log_error_code);
                                        }
                                        stop_program();
                                        break;
                                    case 0:

                                        packets_sent++;
                                        switch(nw_get_reply(ping_socket,dest_addr,recv_buf,packet_size,&program_error_code, &result))
                                        {
                                            case 0:
                                                u_show_result(result, inet_ntoa(dest_addr.sin_addr),u_get_cur_time_ms()-start_time_ms,packet_size,recv_buf->ttl);
                                                log_write_result(log_file,result, inet_ntoa(dest_addr.sin_addr),u_get_cur_time_ms()-start_time_ms,packet_size,recv_buf->ttl);
                                                break;
                                            default:
                                                if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                                                {
                                                    log_diagnostics(log_error_code);
                                                }
                                                stop_program();
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
