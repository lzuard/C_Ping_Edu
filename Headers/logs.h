//Декларация фунций работы с логом

#include <stdio.h>      // Стандартные функции ввода/вывода
#include <errno.h>      // Стандартные функции работы с ошибками
#include <windows.h>    // Функции Win32 api

#ifndef PING_LOGS_H
#define PING_LOGS_H

#endif //PING_LOGS_H



//Декларация фунций
// Функция открытия файла лога
int log_open_file(FILE* *log_file, char* params_log_path,char* params_address, int *program_error_code, int *log_error_code);
// Функция записи ошибки в файл логов
int log_write_error(FILE* logfile, int program_error_code, int* log_error_code);
// Функция записи результата работы программы в файл логов
int log_write_result(FILE* logfile, int result, char* host, ULONG time_ms, int packet_size, byte ttl);
// Функция диагностики файла логов
int log_diagnostics(int log_error_code);
