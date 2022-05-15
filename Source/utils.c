//IO validation part
//Headers in validation.h
#include <unistd.h>
#include "../Headers/utils.h"

ULONG u_get_cur_time_ms()
{
    SYSTEMTIME now;
    GetSystemTime(&now);
    return now.wMilliseconds;
}

int u_check_params(int argc, char *argv[], char* *params_address, char* *params_log_path)
{
    char* curr_path;
    char* filename="\\ping_log.txt";

    if(argc<2 || argc>3) //argv[0] - exe file path
    {
        return 1;
    }
    else
    {
        *params_address=argv[1];
        if (argc==3)
        {
            *params_log_path=argv[2];
        }
        else
        {

            curr_path=getcwd(NULL,128);
            *params_log_path=curr_path;
            strcat(*params_log_path,filename);
        }
        return 0;
    }
}

USHORT u_ip_checksum(USHORT *buffer, int size)
{
    unsigned long buffer_size= sizeof(buffer);
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
