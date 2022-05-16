//Файл с сетевыми функциями

//Подключение заголовочных файлов проекта
#include "../Headers/network.h" //Объявление сетевых функций
#include "../Headers/utils.h"   //Вспомогательные функции

// Декларация функций

// Декларация функция получения Ip-адреса из доменного имени
/* Принимает параметры:
   * char *host - адрес хоста;
   * struct sockaddr_in *dest_addr - структура для хранения адреса назначения хоста;
   * int *program_error_code - код ошибки лога.
   Возвращает: 0 - при успехе; 1 - при ошибке.
*/
int nw_get_ip(char *host,struct sockaddr_in *dest_addr, int *program_error_code){
    struct hostent *ip_buf; //Вспомогательный буфер для временной записи адреса

    //Получаем ip
    ip_buf = gethostbyname(host);
    if(ip_buf != 0) //Если удалось
    {
        //Копируем адрес в основной буфер
        memcpy(&(dest_addr->sin_addr), ip_buf->h_addr_list[0], ip_buf->h_length);
        dest_addr->sin_family = ip_buf->h_addrtype;
        return 0;
    }
    else //В случае ошибки
    {
        *program_error_code=102;
        return 1;
    }
}

// Декларация функции настройки сокета и получения ip-адреса
/* Принимает параметры:
   * char *host - адрес хоста;
   * int ttl - TTL отправляемых пакетов;
   * struct sockaddr_in *dest_addr - структура для хранения адреса назначения хоста;
   * struct WSAData *wsaData - структура для инициализации сокета;
   * SOCKET *ping_socket - сокет для отправки и приема пакетов;
   * int *program_error_code - код ошибки лога.
   Возвращает: 0 - если хост IP-адрес; 1 - если хост не IP-адрес; 2 - при ошибке.
*/
int nw_check_host(char *host, int ttl, struct sockaddr_in *dest_addr,struct WSAData *wsaData,SOCKET *ping_socket, int *program_error_code){
    unsigned long ip_address;   //Ip адрес

    //Инициализация Wsa для открытия сокетов
    if(WSAStartup(MAKEWORD(2,1), wsaData)==0)   //Init wsa to open socket
    {
        //Инициализация сокета
        *ping_socket=WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0, 0, 0);
        if(*ping_socket!=INVALID_SOCKET)    //Если сокет инициализирован
        {
            //Открытие сокета
            if (setsockopt(*ping_socket, IPPROTO_IP, IP_TTL, (const char *) &ttl, sizeof(ttl)) != SOCKET_ERROR)
            {
                ip_address = inet_addr(host);   // Получение адреса
                if (ip_address != INADDR_NONE)      //Если адрес IPv4
                {
                    dest_addr->sin_family = AF_INET;    //Устанавливаем тип адреса Ip
                    dest_addr->sin_addr.s_addr = ip_address;   //Устанавливаем адрес в структуру
                    return 0;
                } else return 1; //Адрес не IP
            }
        }
    }
    *program_error_code=101;
    return 2;   //Если сокет не открылся
}

// Декларация функции отправки ICMP-пакета
/* Принимает параметры:
   * SOCKET ping_socket - сокет для отправки и приема пакетов;
   * struct sockaddr_in dest_addr - структура для хранения адреса назначения хоста;
   * struct ICMPHeader send_buf - буфер для отправляемого заголовка ICMP;
   * int packet_size - размер ICMP пакета в байтах;
   * int *program_error_code - код ошибки лога;
   * int *bytes_sent - колличество отправленных байт.
   Возвращает: 0 - при успехе; 1 - при ошибке.
*/
int nw_send_request(SOCKET ping_socket, struct sockaddr_in dest_addr, struct ICMPHeader send_buf, int packet_size, int *program_error_code, int *bytes_sent){

    // Размер пакета
    packet_size = max(sizeof(struct ICMPHeader),min(1024, (unsigned int)packet_size));

    //Заполняем структуру заголовка ICMP
    send_buf.code = 0;
    send_buf.type = 8;  //ICMP эхо запрос
    send_buf.checksum = 0;
    send_buf.id = (USHORT)GetCurrentProcessId();
    send_buf.seq = 0;
    send_buf.timestamp = GetTickCount();
    send_buf.checksum = u_ip_checksum((USHORT *) &send_buf, packet_size);

    // Отправка ICMP-пакета
    *bytes_sent = sendto(ping_socket, (char *)&send_buf, packet_size, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));

    if (*bytes_sent == SOCKET_ERROR) //Ошибка отправки
    {
        *program_error_code=103;
        return 1;
    }
    else return 0;  //Пакет успешно отправлен
}

// Декларация функции получения ICMP-пакета
/* Принимает параметры:
   * SOCKET ping_socket - сокет для отправки и приема пакетов;
   * struct sockaddr_in source_addr - структура для хранения адреса хоста отправителя;
   * struct IPHeader *recv_buf - буфер для поулчаемого заголовка IP;
   * int packet_size - размер ICMP пакета в байтах;
   * int *program_error_code - код ошибки лога;
   * int *result - результат расшифровки полученного ICMP пакета.
   Возвращает: 0 - при успехе; 1 - при ошибке. TODO
*/
int nw_get_reply(SOCKET ping_socket,struct sockaddr_in source_addr, struct IPHeader *recv_buf,int packet_size, int *program_error_code, int *result)
{
    //Декларация локальных переменных
    struct timeval time_for_timout;     //Максимальное время ответа
    struct ICMPHeader *header;          //Заголовок полученного ICMP-пакета
    fd_set socket_descriptor;           //Дескриптор сокета
    int source_addr_len;                //Длина изначального IP-адреса
    unsigned short header_len;          //Полученная длинна заголовка пакета


    //Инициализация локальных переменных
    time_for_timout.tv_sec = 1;                         // Время таймаута в секундах
    time_for_timout.tv_usec = (1000 % 1000) * 100000;   // Время таймаута в миллисекундах
    source_addr_len = sizeof(source_addr);              //  Размер адреса источника

    //Получение ICMP-пакета
    while(1)
    {
        FD_ZERO(&socket_descriptor);             //Инициализация набора файловых дескрипторов fd_set
        FD_SET(ping_socket, &socket_descriptor); //Устанавка бита для файлового дескриптора fd в наборе файловых дескрипторов fd_set.

        //Ожидание изменения данных на сокете
        switch(select(ping_socket+1, &socket_descriptor, 0, 0, &time_for_timout))
        {
            case 0:     //Таймаут ожидания
                *program_error_code=104;
                return 1;
            case -1:    //Ошибка ожидания
                *program_error_code=105;
                return 1;
            default:    //Изменения произошли
                //Извлечение данных из сокета
                if(recvfrom(ping_socket, (char*) recv_buf,packet_size+sizeof(struct IPHeader),0,(struct sockaddr*) &source_addr, &source_addr_len) != SOCKET_ERROR)
                {
                    header_len = recv_buf->h_len * 4;   // Длинна заголовка
                    header = (struct ICMPHeader *)((char *)recv_buf + header_len);  // Заголовок ICMP-пакета
                    *result=header->type;   // Тип ICMP-ответа
                    return 0;
                }
                else // Произошла ошибка извлечения
                {
                    *program_error_code=106;
                    return 1;
                }
        }
    }
}



