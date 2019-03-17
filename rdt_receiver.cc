/*
 * FILE: rdt_receiver.cc
 * DESCRIPTION: Reliable data transfer receiver.
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
#include <algorithm>
#include <vector>

#include "rdt_struct.h"
#include "rdt_receiver.h"
#include "rdt_protocol.h"

/* Declaration added by me */
std::vector<PktItem> pkt_buff;
uint32_t seq;

/* receiver initialization, called once at the very beginning */
void Receiver_Init()
{
    fprintf(stdout, "At %.2fs: receiver initializing ...\n", GetSimulationTime());
    seq = 0;
    pkt_buff.clear();
}

/* receiver finalization, called once at the very end.
   you may find that you don't need it, in which case you can leave it blank.
   in certain cases, you might want to use this opportunity to release some 
   memory you allocated in Receiver_init(). */
void Receiver_Final()
{
    fprintf(stdout, "At %.2fs: receiver finalizing ...\n", GetSimulationTime());
}

/* event handler, called when a packet is passed from the lower layer at the 
   receiver */
void Receiver_FromLowerLayer(struct packet *pkt)
{
    printf("At %.2fs: i Receiver_FromLowerLayer\n", GetSimulationTime());
    PktItem *p = (PktItem *)pkt;
    if (varifyChecksum(p))
    {
        printf("seq %u p->seq %u\n", seq, p->seq);
        PktItem temp;
        temp.seq = 0xFFFFFFFF;
        while (pkt_buff.size() <= p->seq)
        {
            pkt_buff.push_back(temp);
        }
        printf("pkt buff size %u p->seq %u\n", pkt_buff.size(), p->seq);
        pkt_buff[p->seq] = *p;
        if (seq == p->seq)
        {
            // printf("recc %u %.*s\n", p->payload_size, p->payload_size, p->payload);
            while (seq < pkt_buff.size() && pkt_buff[seq].seq != 0xFFFFFFFF)
            {
                /* code */
                struct message msg;
                msg.data = (char *)(pkt_buff[seq].payload);
                msg.size = pkt_buff[seq].payload_size;
                Receiver_ToUpperLayer(&msg);
                seq++;
            }
        }
        PktItem answer;
        answer.seq = seq;
        setChecksum(&answer);
        Receiver_ToLowerLayer((struct packet *)&answer);
    }
    printf("o Receiver_FromLowerLayer\n");
}
