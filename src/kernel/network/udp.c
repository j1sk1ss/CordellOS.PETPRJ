#include "../include/udp.h"


udp_packet_body packets[PACKET_BUFFER_SIZE];
int current_packet = 0;


uint16_t UDP_calculate_checksum(udp_packet_t* packet) {
    return 0;
}

void UDP_send_packet(uint8_t* dst_ip, uint16_t src_port, uint16_t dst_port, void* data, int len) {
    int length = sizeof(udp_packet_t) + len;
    udp_packet_t* packet = (udp_packet_t*)kmalloc(length);

    packet->src_port = host2net16(src_port);
    packet->dst_port = host2net16(dst_port);
    packet->length   = host2net16(length);
    packet->checksum = UDP_calculate_checksum(packet);

    memcpy((void*)packet + sizeof(udp_packet_t), data, len);
    IP_send_packet(dst_ip, packet, length, PROTOCOL_UDP);
}

void UDP_handle_packet(udp_packet_t* packet) {
    uint16_t dst_port = net2host16(packet->dst_port);
    uint16_t length   = net2host16(packet->length);
    void* data_ptr    = (void*)packet + sizeof(udp_packet_t);
    uint16_t data_len = length;

    if (current_packet >= PACKET_BUFFER_SIZE) {
        current_packet = PACKET_BUFFER_SIZE - 1;

        if (packets[current_packet].data_size != -1) {
            free(packets[current_packet].data);
            packets[current_packet].data_size = -1;            
        }
    }

    packets[current_packet].data = (uint8_t*)clralloc(data_len);
    memcpy(packets[current_packet++].data, data_ptr, data_len);
    packets[current_packet].data_size = data_len;
    
    if (NETWORK_DEBUG) kprintf("\nReceived UDP packet, dst_port %d", dst_port);
    if (net2host16(packet->dst_port) == 68) DHCP_handle_packet(data_ptr);
}

udp_packet_body* UDP_pop_packet() {
    if (current_packet >= 0) {
        if (packets[current_packet].data_size == -1) return NULL;
        
        udp_packet_body* packet = (udp_packet_body*)kmalloc(sizeof(udp_packet_body));
        packet->data = (uint8_t*)kmalloc(packets[current_packet].data_size);
        memcpy(packet->data, packets[current_packet].data, packets[current_packet].data_size);

        packet->data_size = packets[current_packet].data_size;

        kfree(packets[current_packet].data);
        packets[current_packet].data_size = -1;

        if (current_packet > 1) current_packet--;
        return packet;
    }

    return NULL;
}

void UDP_init() {
    for (int i = 0; i < PACKET_BUFFER_SIZE; i++) {
        packets[i].data      = NULL;
        packets[i].data_size = -1;
    }
}