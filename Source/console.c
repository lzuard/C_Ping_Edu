#include <stdio.h>
#include "../Headers/console.h"


void io_welcome(){
    printf("hello\n");
}

int io_get_input(char input[100]){
    scanf("%s",input);
    //TODO: better implementation?
    return 0;
}