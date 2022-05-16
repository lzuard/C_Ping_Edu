//Декларация вспомогательных фунций

#include <windows.h> //Библиотека функций win32 api
#include <stdio.h>   //Стандартные функции ввода/вывода


#ifndef PING_UTILS_H
#define PING_UTILS_H

#endif //PING_UTILS_H

// Функции
// Функция получения текущего времени
ULONG u_get_cur_time_ms();
// Функция расчета контрольной суммы
USHORT u_ip_checksum(USHORT *buffer, int size);
// Функция проверки параметров
int u_check_params(int argc, char *argv[], char* *params_address, char* *params_log_path);
// Функция вывода результата в консоль
void u_show_result(int result, char* host, unsigned long time_ms, int packet_size, byte ttl);

