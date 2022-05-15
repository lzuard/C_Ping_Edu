//Headers for logs.c

#include <stdio.h>
#include <errno.h>
#include <windows.h>

#ifndef PING_LOGS_H
#define PING_LOGS_H

#endif //PING_LOGS_H



int log_open_file(FILE* *log_file, char* params_log_path,char* params_address, int *program_error_code, int *log_error_code);
int log_write_error(FILE* logfile, int program_error_code, int* log_error_code);
int log_write_result(FILE* logfile, int result, char* host, ULONG time_ms, int packet_size, byte ttl);
int log_diagnostics(int log_error_code);
