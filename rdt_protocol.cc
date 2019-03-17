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
    assert(ptr);
    ptr->checksum = 0;
    ptr->checksum = calcChecksum((char *)ptr, RDT_PKTSIZE);
}

int varifyChecksum(PktItem *ptr)
{
    assert(ptr);
    uint16_t temp = ptr->checksum;
    setChecksum(ptr);
    return temp == ptr->checksum;
}

static const uint16_t crc_table[16] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

uint16_t calcCRC(uint8_t *data, int len)
{
    uint16_t crc = 0, h, l;
    while (len--)
    {
        h = (crc >> 12), l = (crc << 4);
        crc = l ^ crc_table[h ^ (*data >> 4)];
        h = (crc >> 12), l = (crc << 4);
        crc = l ^ crc_table[h ^ (*data & 0x0f)];
        data++;
    }
    return crc;
}

void setCRC(PktItem *ptr)
{
    assert(ptr);
    ptr->checksum = 0;
    ptr->checksum = calcCRC((uint8_t *)ptr, RDT_PKTSIZE);
}

int varifyCRC(PktItem *ptr)
{
    assert(ptr);
    uint16_t temp = ptr->checksum;
    setCRC(ptr);
    return temp == ptr->checksum;
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
//     setCRC(&ptr);
//     printf("checksum %x\n varify %x", ptr.checksum, varifyCRC(&ptr));
// }