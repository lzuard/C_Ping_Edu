//Place for all logs jobs
//Headers in logs.h

#include <time.h>

#include "../Headers/logs.h"

int log_open_file(FILE* *log_file, char* params_log_path,char* params_address, int *program_error_code, int *log_error_code)
{
    int write_result=0;
    *log_file=fopen(params_log_path,"a");
    if(*log_file==NULL)
    {
        *program_error_code=108;
        *log_error_code=errno;
        return 1;
    }
    else
    {
        write_result=fprintf(*log_file,"\n=========================================================\n[] New program start with parameters: host: %s, log: %s\n",params_address,params_log_path);
        if(write_result<0)
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