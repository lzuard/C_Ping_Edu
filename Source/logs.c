//Place for all logs jobs
//Headers in logs.h
#include <stdio.h>
#include <errno.h>
#include "../Headers/logs.h"

int log_open_file(FILE* *log_file, char* log_path, int *program_error_code, int *log_error_code)
{
    *log_file=fopen(log_path,"w");
    if(*log_file==NULL)
    {
        *program_error_code=108;
        *log_error_code=errno;
        return 1;
    }
    else
    {
        return 0;
    }
}
int log_write(){
    return 0;
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