//IO validation part
//Headers in validation.h

#include "../Headers/utils.h"

ULONG u_get_cur_time_ms()
{
    SYSTEMTIME now;
    GetSystemTime(&now);
    return now.wMilliseconds;
}

int u_check_params(char input[])
{
    return 0; //TODO: add params_address check
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
