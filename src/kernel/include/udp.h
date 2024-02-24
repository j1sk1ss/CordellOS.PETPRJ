#ifndef UDP_H
#define UDP_H


#include <memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <netutils.h>

#include "ip.h"


typedef struct udp_packet {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
    uint8_t data[];
} __attribute__((packed)) udp_packet_t;


uint16_t udp_calculate_checksum(udp_packet_t* packet);
void udp_send_packet(uint8_t* dst_ip, uint16_t src_port, uint16_t dst_port, void* data, int len);
void udp_handle_packet(udp_packet_t* packet);

#endif