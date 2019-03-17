#ifndef _RDT_PROTOCOL_H
#define _RDT_PROTOCOL_H

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "rdt_struct.h"

#define WINDOW_SIZE 10
#define TIMEOUT 1
#define HEADER_SIZE 8
#define MAX_PAYLOAD_SIZE (RDT_PKTSIZE - HEADER_SIZE)

struct PktItem
{
    /* data */
    uint32_t seq;
    uint16_t checksum;
    uint16_t payload_size;
    uint8_t payload[MAX_PAYLOAD_SIZE];
};

void setChecksum(PktItem *ptr);

int varifyChecksum(PktItem *ptr);

#endif