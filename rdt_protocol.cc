#include "rdt_protocol.h"
#include "rdt_struct.h"

#define CHECKSUM_BLOCK_NUM (RDT_PKTSIZE / 2)

uint16_t calcChecksum(char *data, int len)
{
    uint32_t temp = 0;
    uint16_t *p = (uint16_t *)data;
    uint16_t *end = p + len / 2;
    for (; p < end; p++)
    {
        temp += *p;
    }
    return ~(uint16_t)((temp & 0xFFFF) + (temp >> 16));
}

void setChecksum(PktItem *ptr)
{
    ptr->checksum = 0;
    ptr->checksum = calcChecksum((char *)ptr, RDT_PKTSIZE);
}

int varifyChecksum(PktItem *ptr)
{
    return !calcChecksum((char *)ptr, RDT_PKTSIZE);
}

// #include <cstdio>

// int main()
// {
//     PktItem ptr;
//     ptr.seq = 123;
//     ptr.payload_size = 12;
//     ptr.payload[2] = 11;
//     ptr.payload[5] = 11;
//     ptr.payload[8] = 91;
//     ptr.payload[12] = 221;
//     ptr.payload[21] = 1;
//     setChecksum(&ptr);
//     printf("checksum %x\n varify %x", ptr.checksum, varifyChecksum(&ptr));
// }