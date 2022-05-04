//Headers for console.c file

#include <stdio.h>

//Local project headers
#include "../Headers/settings.h"


#ifndef PING_CONSOLE_H
#define PING_CONSOLE_H


void io_welcome(); //First console output on program start

int io_get_input(char input[STR_SIZE]); //Get console input from user


#endif //PING_CONSOLE_H
