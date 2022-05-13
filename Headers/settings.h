//Project settings

#ifndef PING_SETTINGS_H
#define PING_SETTINGS_H

#define STR_SIZE 100 //Size for all char arrays in project


#define MAX_PACKETS_NUM 5

// ICMP packet types
#define ICMP_ECHO_REPLY 0
#define ICMP_DEST_UNREACH 3
#define ICMP_TTL_EXPIRE 11
#define ICMP_ECHO_REQUEST 8


// Minimum ICMP packet size, in bytes
#define ICMP_MIN 8

#define MAX_PING_DATA_SIZE 1024
#define MAX_PING_PACKET_SIZE (MAX_PING_DATA_SIZE + sizeof(struct IPHeader))

#define DEFAULT_PACKET_SIZE 32
#define DEFAULT_TTL 30
#define DEFAULT_TIMEOUT 1000



//error codes
//101 - WSA startup error
//102 - Wrong user address

#endif //PING_SETTINGS_H
