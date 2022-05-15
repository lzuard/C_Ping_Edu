//Главный файл проекта, содержит функцию main

//Подключение стандартных библиотек
#include <winsock2.h>   // Сокетов Windows
#include <windows.h>    // Функции Win32 api, необходимы для использования сокетов
#include <stdio.h>      // Стандартные функции ввода/вывода
#include <stdlib.h>     // Стандартные функции С

//Подключение заголовочных файлов проекта
#include "../Headers/network.h"     // Сетевые функции
#include "../Headers/logs.h"        // Функции лога
#include "../Headers/utils.h"       // Вспомогательные функции

//Объявление переменных
ULONG start_time_ms = 0;    // Время отправки эхо-запроса
SOCKET ping_socket=0;       // Cокет для отправки и приема пакетов
FILE* log_file=NULL;        // Указатель на файл лога
int packets_sent = 0;       // Количество отправленных пакетов
int max_packets_sent = 4;   // Максимальное количество пакетов для отправки
int packet_size = 32;       // Стандартный размер ICMP пакета в байтах
int bytes_sent=0;           // Количество отправленных байт
int ttl = 128;              // TTL отправляемых пакетов
int receive_result=0;       // Результат расшифровки полученного ICMP пакета
int receive_timeout = 1000; // Максимальное время ожидания ICMP эхо-ответа в миллисекундах
int program_error_code = 0; // Код ошибки программы
int log_error_code = 0;     // Код ошибки лога
char* params_address=NULL;  // Адресс назначения, полученный от пользователя
char* params_log_path=NULL; // Полный путь к файлу лога, полученный от пользователя

//Объявление структуры
struct ICMPHeader send_buf;     // Буффер для отправляемого заголовка ICMP
struct IPHeader *recv_buf;      // Буффер для поулчаемого заголовка IP
struct WSAData wsaData;         // Структура для инициализации сокета
struct sockaddr_in dest_addr;   // Структура для хранения адреса назначения хоста

//Функции и процедуры

//Процедура завершения программы
void stop_program()
{
    if(program_error_code==-1) //В случае, если введены неверные параметры, выводим в консоль
    {
        printf("Program stopped because user entered wrong parameters. Usage: ping [host] [log file full path]\n");
    }
    fclose(log_file);   //Закрыть файл лога
    exit(0);           //Завершить выполнение
}

//Процедура main
void main(int argc, char *argv[])
{
    recv_buf = malloc(sizeof(struct IPHeader));

    //Проверка входных параметров и запись в переменные
    switch(u_check_params(argc, argv, &params_address, &params_log_path))
    {
        case 1: //Введены неверные параметры
            program_error_code=-1;
            stop_program();
            break;
        case 0:
            //Открываем файл лога
            switch(log_open_file(&log_file,params_log_path,params_address, &program_error_code,&log_error_code))
            {
                case 1: //Ошибка открытия файла
                    log_diagnostics(log_error_code);
                    stop_program();
                    break;
                case 0:
                    //Проверяем тип адреса хоста
                    switch(nw_check_host(params_address,ttl,&dest_addr,&wsaData,&ping_socket, &program_error_code))
                    {
                        case 2: //Введен неверный хост
                            if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                            {
                                log_diagnostics(log_error_code);
                            }
                            stop_program();
                            break;
                        case 1: //Адрес хоста - доменное имя
                            //Пытаемся получить Ip адрес из доменного имени
                            switch(nw_get_ip(params_address,&dest_addr,&program_error_code))
                            {
                                case 1: //Произошла ошибка
                                    if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                                    {
                                        log_diagnostics(log_error_code);
                                    }
                                    stop_program();
                                    break;
                                case 0: //Ip Получен
                                    //Цикл для отправки нескольких пакетов
                                    while (packets_sent<max_packets_sent)
                                    {
                                        //Фиксируем время отправки пакета
                                        start_time_ms=u_get_cur_time_ms();
                                        //Отправляем пакет
                                        switch (nw_send_request(ping_socket, dest_addr, send_buf, packet_size,&program_error_code, &bytes_sent))
                                        {
                                            case 1: //Ошибка отправки
                                                if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                                                {
                                                    log_diagnostics(log_error_code);
                                                }
                                                stop_program();
                                                break;
                                            case 0: //Пакет отправлен
                                                //Увеличиваем счетчик отправленных пакетов
                                                packets_sent++;
                                                //Пытаемся получить ответ
                                                switch(nw_get_reply(ping_socket,dest_addr,recv_buf,packet_size,&program_error_code, &receive_result))
                                                {
                                                    case 0: //Ответ получен
                                                        //Выводим результат получения в консоль
                                                        u_show_result(receive_result, inet_ntoa(dest_addr.sin_addr), u_get_cur_time_ms() - start_time_ms, packet_size, recv_buf->ttl);
                                                        //Пытаемся вывести результат получения в лог
                                                        if(log_write_result(log_file, receive_result, inet_ntoa(dest_addr.sin_addr), u_get_cur_time_ms() - start_time_ms, packet_size, recv_buf->ttl) != 0)
                                                        {
                                                            log_diagnostics(log_error_code);
                                                            stop_program();
                                                        }
                                                        break;
                                                    default://Ошибка получения пакета
                                                        if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                                                        {
                                                            log_diagnostics(log_error_code);
                                                        }
                                                        stop_program();
                                                        break;
                                                }
                                                break;
                                        }
                                    }
                            }
                            break;
                        case 0: //Адрес хоста - Ip адрес
                            //Цикл для отправки нескольких пакетов
                            while (packets_sent<max_packets_sent)
                            {
                                //Фиксируем время отправки
                                start_time_ms=u_get_cur_time_ms();
                                //Отправляем пакет
                                switch (nw_send_request(ping_socket, dest_addr, send_buf, packet_size,&program_error_code, &bytes_sent)) //send
                                {
                                    case 1: //Ошибка отправки
                                        if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                                        {
                                            log_diagnostics(log_error_code);
                                        }
                                        stop_program();
                                        break;
                                    case 0: //Пакет отправлен
                                        //Увеличиваем счетчик отправленных пакетов
                                        packets_sent++;
                                        //Пытаемся получить ответ
                                        switch(nw_get_reply(ping_socket,dest_addr,recv_buf,packet_size,&program_error_code, &receive_result))
                                        {
                                            case 0: //Ответ получен
                                                u_show_result(receive_result, inet_ntoa(dest_addr.sin_addr), u_get_cur_time_ms() - start_time_ms, packet_size, recv_buf->ttl);
                                                if(log_write_result(log_file, receive_result, inet_ntoa(dest_addr.sin_addr), u_get_cur_time_ms() - start_time_ms, packet_size, recv_buf->ttl) != 0)
                                                {
                                                    log_diagnostics(log_error_code);
                                                    stop_program();
                                                }
                                                break;
                                            default: //Ошибка получения пакета
                                                if(log_write_error(log_file,program_error_code,&log_error_code)!=0)
                                                {
                                                    log_diagnostics(log_error_code);
                                                }
                                                stop_program();
                                                break;
                                        }
                                        break;
                                }
                            }
                            break;
                    }
            }
    }
    //Завершение работы
    stop_program();
}
