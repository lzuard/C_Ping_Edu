//Main project file, start and execution point

//C headers
#include <stdio.h>

//Local project headers
#include "../Headers/settings.h"
#include "../Headers/console.h"
#include "../Headers/network.h"


#define DEFAULT_PACKET_SIZE 32
#define DEFAULT_TTL 30
#define DEFAULT_TIMEOUT 1000

//Variables
char input[STR_SIZE];




int main(int argc, char *argv[]){
    io_welcome();
    io_get_input(input);

    // Figure out how big to make the ping packet
    int packet_size = DEFAULT_PACKET_SIZE;
    int ttl = DEFAULT_TTL;
    int timeout = DEFAULT_TIMEOUT;

    switch (network_ping(packet_size, ttl, input, timeout)){
        case 0:
            break;
        case -1:
            printf("\nTimeout\n");
            break;
        case -2:
            break;
    }
    return 0;
}
