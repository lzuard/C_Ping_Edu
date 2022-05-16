// Файл функций работы с файлом логов

#include <time.h> // Функции получения времени

#include "../Headers/logs.h" // Функции лога


// Декларация функций и процедур

// Декларация функции открытия файла лога
/* Принимает параметры:
   * FILE* *log_file - ссылка на файл лога;
   * char* params_log_path - ссылка на буфер для вывода пути к логу;
   * char* params_address - ссылка на буфер для вывода адреса;
   * int *program_error_code - код ошибки программы;
   * int *log_error_code - код ошибки лога.
   Возвращает: 0 - при успехе; 1 - при ошибке.
*/
int log_open_file(FILE* *log_file, char* params_log_path,char* params_address, int *program_error_code, int *log_error_code)
{
    time_t rawtime;         // Время
    struct tm* timeinfo;   // Подробное время

    time(&rawtime);                 // Привязка к системному времени
    timeinfo = localtime(&rawtime); // Получение времени


    // Открытие файла логов
    *log_file=fopen(params_log_path,"a");
    if(*log_file==NULL) // Ошибка открытия файла логов
    {
        *program_error_code=108;
        *log_error_code=errno;
        return 1;
    }
    else //Файл открыт успешно
    {
        if(fprintf(*log_file,"\n=========================================================\n[%d.%d.%d %d:%d:%d]",timeinfo->tm_mday,
                timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec)<0)
        {
            //  Ошибка записи в файл логов
            *program_error_code=109;
            *log_error_code=errno;
            return 1;
        }
        //  Запись в файл логов параметров программы
        if(fprintf(*log_file," New program start with parameters: host: %s, log: %s\n",params_address,params_log_path)<0)
        {
            // Ошибка записи в файл логов
            *program_error_code=109;
            *log_error_code=errno;
            return 1;
        }
        else
        {
            // Запись успешна
            return 0;
        }
    }
}

// Декларация функции записи ошибки в файл логов
/* Принимает параметры:
   * FILE* log_file - ссылка на файл лога;
   * int program_error_code - код ошибки программы;
   * int* log_error_code - код ошибки лога.
   Возвращает: 0 - при успехе; 1 - при ошибке.
*/
int log_write_error(FILE* log_file, int program_error_code, int* log_error_code)
{
    int wrote=0; // Код возврата записи
    switch(program_error_code){
        case 101:   // Ошибка открытия сокета
            wrote=fprintf(log_file, "[] An error has occurred while opening a socket. WSA error code %d\n", WSAGetLastError());
            break;
        case 102:   // Ошибка распознования хоста
            wrote=fprintf(log_file, "[] An error has occurred while trying to recognize the host\n");
            break;
        case 103:   // Ошибка отправки пакета
            wrote=fprintf(log_file, "[] An error has occurred while trying to send packet. WSA error code %d\n", WSAGetLastError());
            break;
        case 105:   // Ошибка превышения времени ожидания
            wrote=fprintf(log_file, "[] An error has occurred. There was a reply waiting timout\n");
            break;
        case 106:   // Ошибка полученя ICMP-пакета
            wrote=fprintf(log_file, "[] An error has occurred while trying to receive packet. WSA error code %d\n", WSAGetLastError());
            break;
        case 107:   // Ошибка ввода некорректных параметров
            wrote=fprintf(log_file, "[] Program stopped because user entered wrong parameters. Usage: ping [host] [log file full path]\n");
            break;
        default:    // Неизвестная причина ошибки
            wrote=fprintf(log_file, "[] Program stopped because of unknown error has occurred\n");
            break;
    }
    if(wrote==0)    // Ошибка записи в лог
    {
      *log_error_code=errno;
      return 1;
    }
    else
    {   //Запись успешна
        return 0;
    }
}

// Декларация функции записи результата работы программы в файл логов
/* Принимает параметры:
   * FILE* logfile - указатель на файл лога;
   * int result - результат расшифровки полученного ICMP пакета;
   * char* host - адрес хоста;
   * ULONG time_ms - время отправки эхо-запроса;
   * int packet_size - размер ICMP пакета в байтах;
   * byte ttl - TTL отправляемых пакетов.
   Возвращает: 0 - при успехе; 1 - при ошибке.
*/
int log_write_result(FILE* logfile,int result, char* host, ULONG time_ms, int packet_size, byte ttl)
{
    int wrote=0;    // Код возврата записи
    switch(result)
    {
        case 0: //Получен эхо-ответ
            wrote=fprintf(logfile,"Sent %d bytes to %s received %d bytes in %d ms TTL: %d\n",packet_size, host, packet_size, time_ms,ttl);
            break;
        case 3: //Получен адрес назначения недостижим
            wrote=fprintf(logfile,"Sent %d bytes to %s Destination unreachable \n",packet_size, host);
            break;
        case 11: //Получен ответ, но TTL истек
            wrote=fprintf(logfile,"Sent %d bytes to %s TTL expired\n",packet_size, host);
            break;
        default: // Получен неизвестный ICMP пакет
            wrote=fprintf(logfile,"Sent %d bytes to %s Got unknown ICMP packet\n",packet_size, host);
            break;
    }
    if(wrote==0) // Ошибка записи в лог
    {
        return 1;
    }
    else    //Запись успешна
    {
        return 0;
    }
}

// Декларация процедуры диагностики файла логов
/* Принимает параметры:
   * int log_error_code - код ошибки лога.
*/
void log_diagnostics(int log_error_code){
    switch(log_error_code){ //Обработка типа ошибки
        case -1: // Файл логов не был открыт
            printf("Log file wasn't opened\n");
            break;
        default: // Неизвестная ошибка
            printf("Unknown issue caused error on writing logs\n");
            break;
    }
}