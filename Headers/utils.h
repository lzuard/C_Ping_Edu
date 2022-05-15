//Headers for validation.c
#include <windows.h>
#include <stdio.h>


#ifndef PING_UTILS_H
#define PING_UTILS_H

#endif //PING_UTILS_H


ULONG u_get_cur_time_ms();
USHORT u_ip_checksum(USHORT *buffer, int size);
int u_check_params(int argc, char *argv[], char* *params_address, char* *params_log_path);
void u_show_result(int result, char* host, unsigned long time_ms, int packet_size, byte ttl);

