
#ifndef PING_DECLARATIONS_H
#define PING_DECLARATIONS_H

//Подключение стандартных библиотек

#include <winsock2.h>   // Сокеты Windows
#include <windows.h>    // Функции Win32 api, необходимы для использования сокетов
#include <ws2tcpip.h>   // Функции работы с Windows socket
#include <stdio.h>      // Стандартные функции ввода/вывода
#include <stdlib.h>     // Стандартные функции С

//Подключение заголовочных файлов проекта
#include "../Headers/network.h"     // Сетевые функции
#include "../Headers/logs.h"        // Функции лога
#include "../Headers/utils.h"       // Вспомогательные функции

// Декларация переменных
ULONG start_time_ms;        // Время отправки эхо-запроса
SOCKET ping_socket;         // Cокет для отправки и приема пакетов
FILE* log_file;             // Указатель на файл лога
int packets_sent;           // Количество отправленных пакетов
int max_packets_sent;       // Максимальное количество пакетов для отправки
int packet_size;            // Стандартный размер ICMP пакета в байтах
int bytes_sent;             // Колличество отправленных байт
int ttl;                    // TTL отправляемых пакетов
int receive_result;         // Результат расшифровки полученного ICMP пакета
int program_error_code;     // Код ошибки программы
int log_error_code;         // Код ошибки лога
char* params_address;       // Адрес назначения, полученный от пользователя
char* params_log_path;      // Полный путь к файлу лога, полученный от пользователя

// Декларация структур
struct ICMPHeader send_buf;     // Буфер для отправляемого заголовка ICMP
struct IPHeader *recv_buf;      // Буфер для поулчаемого заголовка IP
struct WSAData wsaData;         // Структура для инициализации сокета
struct sockaddr_in dest_addr;   // Структура для хранения адреса назначения хоста
#endif //PING_DECLARATIONS_H