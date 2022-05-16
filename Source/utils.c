// Файл с вспомогательными функциями

#include <unistd.h>             // Cтандартные символические константы и типы
#include "../Headers/utils.h"   //Декларация вспомогательных функций

// Декларация функций и процедур

// Декларация процедуры завершения программы
/* Принимает параметры:
   * int program_error_code - код ошибки программы;
   * int log_error_code - код ошибки лога;
   * FILE* *log_file - ссылка на файл лога.
*/
void u_stop_program(int program_error_code, int log_error_code, FILE* *log_file)
{
    if(program_error_code==-1) //В случае, если введены неверные параметры, выводим в консоль
    {
        printf("Program stopped because user entered wrong parameters. Usage: ping [host] [log file full path]\n");
    }
    else if (program_error_code!=0 && log_error_code==0)
    {
        printf("Program stopped because an error has occurred. Check log to see more details.\n");
    }
    if(*log_file==NULL)
    {
        fclose(*log_file);   //Закрыть файл лога
    }
    exit(0);           //Завершить выполнение
}

// Декларация функции получения текущего времени
/* Возвращает: текущее время в миллисекундах.
*/
ULONG u_get_cur_time_ms()
{
    SYSTEMTIME now;                     // Системное время
    GetSystemTime(&now);     // Получение текущего времени
    return now.wMilliseconds;           // Возврат текущего времени в миллисекундах
}

// Декларация функции проверки параметров
/* Принимает параметры:
   * int argc - колличество входных аргументов;
   * char *argv[] - массив входных аргументов;
   * char* *params_address - ссылка на буфер для вывода адреса;
   * char* *params_log_path - ссылка на буфер для вывода пути к логу.
   Возвращает: 0 - при успехе; 1 - при ошибке.
*/
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

// Декларация функции расчета контрольной суммы
/* Принимает параметры:
   * USHORT *buffer - буфер для отправляемого заголовка ICMP;
   * int size - размер ICMP пакета в байтах.
   Возвращает: контрольную сумму ICMP-пакета.
*/
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

// Декларация функции вывода результата в консоль
/* Принимает параметры:
   * int result - результат расшифровки полученного ICMP пакета;
   * char* host - адрес хоста;
   * ULONG time_ms - время отправки эхо-запроса;
   * int packet_size - размер ICMP пакета в байтах;
   * byte ttl - TTL отправляемых пакетов.
   Возвращает: 0 - при успехе; 1 - при ошибке.
*/
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
        case 11: //  Время ожидания превышено
            printf("TTL expired\n");
            break;
        default: // Неизвестный тип ICMP-пакета
            printf("Got unknown ICMP packet\n");
            break;
    }
}
