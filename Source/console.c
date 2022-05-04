//User interaction point.
//Responsible for all console IO
//Headers in console.h

//C headers
#include <stdio.h>

//Local project headers
#include "../Headers/console.h"



//Functions implementations
void io_welcome(){
    printf("============================================================\n");
    printf("Welcome!\n");
    printf("\n\n\n\n");
    printf("============================================================\n");
}

int io_get_input(char input[STR_SIZE]){
    printf("Enter the line below:\n");
    scanf("%s",input);
    //TODO: better implementation?
    return 0;
}