// Файл функций работы с файлом логов


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
    //printf("Debug-entered log_open_file");
    time_t rawtime;         // Время
    struct tm* timeinfo;   // Подробное время

    time(&rawtime);                 // Привязка к системному времени
    timeinfo = localtime(&rawtime); // Получение времени

    //printf("Debug-log_open_file-trying to open");
    // Открытие файла логов
    *log_file=fopen(params_log_path,"a");
    if(*log_file==NULL) // Ошибка открытия файла логов
    {
        //printf("Debug-log_open_file-open error, exit code 1");
        *program_error_code=108;
        *log_error_code=(int)GetLastError();
        return 1;
    }
    else //Файл открыт успешно
    {
        //printf("Debug-log_open_file-open success");
        if(fprintf(*log_file,"\n=========================================================\n[%d.%d.%d %d:%d:%d]",timeinfo->tm_mday,
                timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec)<0)
        {
            //  Ошибка записи в файл логов
            //printf("Debug-log_open_file-first write error, exit code 1");
            *program_error_code=109;
            *log_error_code=(int)GetLastError();
            return 1;
        }
        //  Запись в файл логов параметров программы
        if(fprintf(*log_file," New program start with parameters: host: %s, log: %s\n",params_address,params_log_path)<0)
        {
            // Ошибка записи в файл логов
            //printf("Debug-log_open_file-second write error, exit code 1");
            *program_error_code=109;
            *log_error_code=(int)GetLastError();
            return 1;
        }
        else
        {
            // Запись успешна
            //printf("Debug-exiting log_open_file with code 0");
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
    //printf("Debug-entered log_write_error with program error %d",program_error_code);
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
      *log_error_code=(int)GetLastError();
      //printf("Debug-log_write_error- write error, exit code 1");
      return 1;
    }
    else
    {   //Запись успешна
        //printf("Debug-log_write_error- write success, exit code 0");
        return 0;
    }
}

// Декларация функции записи результата работы программы в файл логов
/* Принимает параметры:
   * FILE* logfile - указатель на файл лога;
   * int *log_error_code - указатель на код ошибки лога
   * int result - результат расшифровки полученного ICMP пакета;
   * char* host - адрес хоста;
   * ULONG time_ms - время отправки эхо-запроса;
   * int packet_size - размер ICMP пакета в байтах;
   * byte ttl - TTL отправляемых пакетов.
   Возвращает: 0 - при успехе; 1 - при ошибке.
*/
int log_write_result(FILE* logfile,int *log_error_code, int result, char* host, ULONG time_ms, int packet_size, byte ttl)
{
    //printf("Debug-entered log_write_result");
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
        *log_error_code=(int)GetLastError();
        //printf("Debug-log_write_error- write error, exit code 1");
        return 1;
    }
    else    //Запись успешна
    {
        //printf("Debug-log_write_error- write success, exit code 0");
        return 0;
    }
}

// Декларация процедуры диагностики файла логов
/* Принимает параметры:
   * int log_error_code - код ошибки лога.
*/
void log_diagnostics(int log_error_code) //TODO: implement all windows codes
{
    //printf("Debug-entered log_diagnostics with code %d", log_error_code);
    printf("Log error. Windows error code %d: ",log_error_code);
    switch(log_error_code){ //Обработка типа ошибки
        case 2:
            printf("Can't find the file\n");
            break;
        case 4:
            printf("The system can't open the file\n");
            break;
        case 15:
            printf("The system can't find the drive specified\n");
            break;
        case 25:
            printf("Cannot find the specified area or track on disk\n");
            break;
        case 26:
            printf("No access to disk\n");
            break;
        case 32:
            printf("Current process can't open the file because it's occupied by another process\n");
            break;
        case 33:
            printf("Current process can't open the file because it's part is locked by another process\n");
            break;
        case 38:
            printf("End of the file reached\n");
            break;
        case 39:
            printf("No disk space left\n");
            break;
        case 108:
            printf("Disk is busy or locked by another process\n");
            break;
        case 110:
            printf("The system can't open the device or file\n");
            break;
        case 183:
            printf("Can't create a file, it already exists\n");
            break;
        case 303:
            printf("Can't open the file because it's int the process being delete\n");
            break;
        case 2229:
            printf("Disk I/o error\n");
            break;
        case 3064:
            printf("Problems with the file\n");
            break;
        default: // Неизвестная ошибка
            printf(". Unknown issue\n");
            break;
    }
}