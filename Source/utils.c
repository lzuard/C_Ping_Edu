// Файл с вспомогательными функциями

#include <unistd.h>             // Cтандартные символические константы и типы
#include "../Headers/utils.h"   //Декларация вспомогательных функций

// Функции
// Функция получения текущего времени
ULONG u_get_cur_time_ms()
{
    SYSTEMTIME now;                     // Системное время
    GetSystemTime(&now);     // Получение текущего времени
    return now.wMilliseconds;           // Возврат текущего времени в миллисекундах
}

// Функция проверки параметров
int u_check_params(int argc, char *argv[], char* *params_address, char* *params_log_path)
{
    char* curr_path;                    // Путь к текущему файлу
    char* filename="\\ping_log.txt";    // Имя файла в системе

    if(argc<2 || argc>3) // Если было получено неверное количество параметров
    {   //Выход с ошибкой
        return 1;
    }
    else    // Если передано правильное количество параметров
    {
        *params_address=argv[1];    // Получаем адрес хоста
        if (argc==3)    //Если передан путь к логу
        {
            *params_log_path=argv[2];  // Получаем путь к логу
        }
        else    //Если путь к логу не указан
        {
            curr_path=getcwd(NULL,128);                     //Получаем путь к текущей дериктории
            *params_log_path=curr_path;                     //Копируем путь к текущей дериктории
            strcat(*params_log_path,filename);  //Добавляем стандартное имя файла
        }
        return 0;
    }
}
// Функция расчета контрольной суммы
USHORT u_ip_checksum(USHORT *buffer, int size)
{
    unsigned long cksum = 0;

    while (size > 1)
    {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }
    if (size)
    {
        cksum += *(UCHAR *)buffer;
    }

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);

    return (USHORT)(~cksum);
}
// Функция вывода результата в консоль
void u_show_result(int result, char* host, ULONG time_ms, int packet_size, byte ttl)
{
    printf("Sent %d bytes to %s ",packet_size, host);
    switch(result) // Обработка результата
    {
        case 0: //  Ответ получен
            printf("received %d bytes in %d ms TTL: %d\n", packet_size, time_ms,ttl);
            break;
        case 3: //  Пункт назначения недоступен
            printf("Destination unreachable \n");
            break;
        case 1: //  Время ожидания превышено
            printf("TTL expired\n");
            break;
        default: // Неизвестный тип ICMP-пакета
            printf("Got unknown ICMP packet\n");
            break;
    }
}
