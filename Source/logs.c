//Place for all logs jobs
//Headers in logs.h

#include <time.h>

#include "../Headers/logs.h"


int log_open_file(FILE* *log_file, char* params_log_path,char* params_address, int *program_error_code, int *log_error_code)
{
    int write_result=0;
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);


    *log_file=fopen(params_log_path,"a");
    if(*log_file==NULL)
    {
        *program_error_code=108;
        *log_error_code=errno;
        return 1;
    }
    else
    {
        if(fprintf(*log_file,"\n=========================================================\n[%d.%d.%d %d:%d:%d]",timeinfo->tm_mday,
                timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec)<0)
        {
            *program_error_code=109;
            *log_error_code=errno;
            return 1;
        }
        if(fprintf(*log_file," New program start with parameters: host: %s, log: %s\n",params_address,params_log_path)<0)
        {
            *program_error_code=109;
            *log_error_code=errno;
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
int log_write_error(FILE* logfile, int program_error_code, int* log_error_code)
{
    int wrote=0;
    switch(program_error_code){
        case 101:
            wrote=fprintf(logfile,"[] An error has occurred while opening a socket. WSA error code %d\n", WSAGetLastError());
            break;
        case 102:
            wrote=fprintf(logfile,"[] An error has occurred while trying to recognize the host\n");
            break;
        case 103:
            wrote=fprintf(logfile,"[] An error has occurred while trying to send packet. WSA error code %d\n",WSAGetLastError());
            break;
        case 105:
            wrote=fprintf(logfile,"[] An error has occurred. There was a reply waiting timout\n");
            break;
        case 106:
            wrote=fprintf(logfile,"[] An error has occurred while trying to receive packet. WSA error code %d\n",WSAGetLastError());
            break;
        case 107:
            wrote=fprintf(logfile,"[] Program stopped because user entered wrong parameters. Usage: ping [host] [log file full path]\n");
            break;
        default:
            wrote=fprintf(logfile,"[] Program stopped because of unknown error has occurred\n");
            break;
    }
    if(wrote==0)
    {
      *log_error_code=errno;
      return 1;
    }
    else
    {
        return 0;
    }
}
int log_write_result(FILE* logfile,int result, char* host, ULONG time_ms, int packet_size, byte ttl)
{
    int wrote=0;
    switch(result)
    {
        case 0: //echo reply
            wrote=fprintf(logfile,"Sent %d bytes to %s received %d bytes in %d ms TTL: %d\n",packet_size, host, packet_size, time_ms,ttl);
            break;
        case 3: //unreach
            wrote=fprintf(logfile,"Sent %d bytes to %s Destination unreachable \n",packet_size, host);
            break;
        case 1: //ttl exp
            wrote=fprintf(logfile,"Sent %d bytes to %s TTL expired\n",packet_size, host);
            break;
        default: // Unknown ICMP packet
            wrote=fprintf(logfile,"Sent %d bytes to %s Got unknown ICMP packet\n",packet_size, host);
            break;
    }
    if(wrote==0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int log_diagnostics(int log_error_code){
    switch(log_error_code){
        case -1:
            printf("Log file wasn't opened\n");
            break;
        case 0:
            //printf("All logs have been saved in %s\n",params_log_path);
            break;
        default:
            printf("Unknown issue caused error on writing logs\n");
    }
    return 0;
}