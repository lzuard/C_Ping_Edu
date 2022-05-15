//Place for all logs jobs
//Headers in logs.h
#include <stdio.h>
#include "../Headers/logs.h"

int log_open_file(char path[]){
    return 0;
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