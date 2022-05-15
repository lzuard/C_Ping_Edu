//Headers for logs.c

#ifndef PING_LOGS_H
#define PING_LOGS_H

#endif //PING_LOGS_H

int log_open_file(FILE* *log_file, char* log_path, int *program_error_code, int *log_error_code);
int log_write();
int log_diagnostics(int log_error_code);
