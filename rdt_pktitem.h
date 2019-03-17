#ifndef _RDT_PKTITEM_H
#define _RDT_PKTITEM_H

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "rdt_struct.h"

#define WINDOW_SIZE 10
#define TIMEOUT 0.5
#define HEADER_SIZE 7
#define MAX_PAYLOAD_SIZE (RDT_PKTSIZE - HEADER_SIZE)

struct PktItem
{
    /* data */
    uint16_t checksum;
    uint32_t seq;
    uint8_t payload_size;
    uint8_t payload[MAX_PAYLOAD_SIZE];
};

void setChecksum(PktItem *ptr);

int varifyChecksum(PktItem *ptr);

#endif