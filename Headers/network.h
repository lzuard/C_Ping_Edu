// Декларация сетевых структур и функций

#include <winsock2.h>   / Сокеты Windows
#include <windows.h>    // Функции Win32 api, необходимы для использования сокетов
#include <ws2tcpip.h>   // Функции работы с Windows socket
#include <iphlpapi.h>   // Функции работы с ip
#include <stdio.h>      // Стандартные функции ввода/вывода
#include <stdlib.h>     // Стандартные функции С

#ifndef PING_NETWORK_H
#define PING_NETWORK_H

#endif //PING_NETWORK_H

//Декларация структур
//Структура заголовков IP адреса
struct IPHeader
{
    BYTE h_len : 4;   // Длинна заголовка
    BYTE version : 4; // Версия IP
    BYTE tos;         // Тип услуги
    USHORT total_len; // Длинна пакета
    USHORT ident;     // Уникальный индификатор
    USHORT flags;     // Флаги
    BYTE ttl;         // Time to live
    BYTE proto;       // Тип протокола
    USHORT checksum;  // Контрольная сумма
    ULONG source_ip;  // Исходный IP адресс
    ULONG dest_ip;    // IP адрес получателя
};
// Структура заголовков ICMP-пакета
struct ICMPHeader
{
    BYTE type;          // Тип ICMP-пакета
    BYTE code;          // Код ICMP-пакета
    USHORT checksum;    // Контрольная ссума ICMP-пакета
    USHORT id;          // Индификатор ICMP-пакета
    USHORT seq;         // Номер последовательности ICMP-пакета
    ULONG timestamp;    // Временная метка
};
//Декларация фунций
//Функция получения Ip-адреса из доменного имени
int nw_get_ip(char *host,struct sockaddr_in *dest_addr, int *program_error_code);
// Функция открытия сокета
int nw_check_host(char *host, int ttl, struct sockaddr_in *dest_addr,struct WSAData *wsaData,SOCKET *ping_socket, int *program_error_code);
// Функция отправки ICMP пакета
int nw_send_request(SOCKET socket, struct sockaddr_in dest_addr, struct ICMPHeader send_buf, int packet_size, int *program_error_code, int *bytes_sent);
// Функция получения ICMP-пакета
int nw_get_reply(SOCKET ping_socket,struct sockaddr_in source_addr, struct IPHeader *recv_buf,int packet_size, int *program_error_code, int *result);

