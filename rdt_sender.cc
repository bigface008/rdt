/*
 * FILE: rdt_sender.cc
 * DESCRIPTION: Reliable data transfer sender.
 * NOTE: This implementation assumes there is no packet loss, corruption, or 
 *       reordering.  You will need to enhance it to deal with all these 
 *       situations.  In this implementation, the packet format is laid out as 
 *       the following:
 *       
 *       |<-  1 byte  ->|<-             the rest            ->|
 *       | payload size |<-             payload             ->|
 *
 *       The first byte of each packet indicates the size of the payload
 *       (excluding this single-byte header)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>

#include "rdt_struct.h"
#include "rdt_sender.h"
#include "rdt_protocol.h"

/* Declaration added by me. */
static std::vector<PktItem> pkt_buff;
static uint32_t pkt_base;
static uint32_t pkt_nextseqnumber;

/* Send packages and store them into buff. */
void Sender_StoreMessages(struct message *msg);

void Sender_SendPackets();

/* sender initialization, called once at the very beginning */
void Sender_Init()
{
    fprintf(stdout, "At %.2fs: sender initializing ...\n", GetSimulationTime());
    pkt_base = 0;
    pkt_nextseqnumber = 0;
}

/* sender finalization, called once at the very end.
   you may find that you don't need it, in which case you can leave it blank.
   in certain cases, you might want to take this opportunity to release some 
   memory you allocated in Sender_init(). */
void Sender_Final()
{
    fprintf(stdout, "At %.2fs: sender finalizing ...\n", GetSimulationTime());
}

/* event handler, called when a message is passed from the upper layer at the 
   sender */
void Sender_FromUpperLayer(struct message *msg)
{
    Sender_StoreMessages(msg);
    Sender_SendPackets();
}

/* event handler, called when a packet is passed from the lower layer at the 
   sender */
void Sender_FromLowerLayer(struct packet *pkt)
{
    printf("i Sender_FromLowerLayer\n");
    PktItem *p = (PktItem *)pkt;
    if (varifyChecksum(p))
    {
        if (p->seq > pkt_base)
        {
            pkt_base = p->seq;
            Sender_SendPackets();
            if (pkt_base == pkt_nextseqnumber)
                Sender_StopTimer();
        }
    }
    printf("o Sender_FromLowerLayer\n");
}

/* event handler, called when the timer expires */
void Sender_Timeout()
{
    printf("i Sender_Timeout\n");
    for (uint32_t i = pkt_base; i < pkt_nextseqnumber; i++)
    {
        // printf("%u %u\n", pkt_base, pkt_nextseqnumber);
        Sender_ToLowerLayer((struct packet *)&pkt_buff[i]);
        Sender_StartTimer(TIMEOUT);
    }
    printf("o Sender_Timeout\n");
}

void Sender_StoreMessages(struct message *msg)
{
    assert(msg);
    printf("i Sender_StoreMessage\n");
    int msg_size = msg->size;
    char *data = msg->data;
    printf("msg size %d\n", msg->size);
    while (msg_size)
    {
        PktItem p;
        p.seq = pkt_buff.size();
        p.payload_size = msg_size > MAX_PAYLOAD_SIZE ? MAX_PAYLOAD_SIZE : msg_size;
        memcpy(p.payload, data, p.payload_size);
        setChecksum(&p);
        pkt_buff.push_back(p);
        msg_size -= p.payload_size;
        data += p.payload_size;
    }
    // printf("pkt buff len %d\n", pkt_buff.size());
    printf("o Sender_StoreMessage\n");
}

void Sender_SendPackets()
{
    printf("i Sender_SendPackets\n");
    // printf("pkt size %d\n", sizeof(PktItem));
    for (; pkt_nextseqnumber < std::min((std::size_t)(pkt_base + WINDOW_SIZE), pkt_buff.size()); pkt_nextseqnumber++)
    {
        // printf("nextseqnumber %d\n", pkt_nextseqnumber);
        Sender_ToLowerLayer((struct packet *)(&pkt_buff[pkt_nextseqnumber]));
        // printf("send %u %.*s\n", pkt_buff[pkt_nextseqnumber].payload_size, pkt_buff[pkt_nextseqnumber].payload_size, pkt_buff[pkt_nextseqnumber].payload);
        Sender_StartTimer(TIMEOUT);
    }
    
    printf("o Sender_SendPackets\n");
}