//Декларация вспомогательных фунций

#include <windows.h> //Библиотека функций win32 api
#include <stdio.h>   //Стандартные функции ввода/вывода
#include <unistd.h>  // Cтандартные символические константы и типы


#ifndef PING_UTILS_H
#define PING_UTILS_H

#endif //PING_UTILS_H

// Функции и процедуры
// Функция получения текущего времени
ULONG u_get_cur_time_ms();
// Функция расчета контрольной суммы
USHORT u_ip_checksum(USHORT *buffer, int size);
// Функция проверки параметров
int u_check_params(int argc, char *argv[], char* *params_address, char* *params_log_path);
// Процедура вывода результата в консоль
void u_show_result(int result, char* host, unsigned long time_ms, int packet_size, byte ttl);
// Процедура завершения программы
void u_stop_program(int program_error_code, int log_error_code, FILE* *log_file);
