#include "../include/udp.h"


uint16_t udp_calculate_checksum(udp_packet_t* packet) {
    return 0;
}

void udp_send_packet(uint8_t* dst_ip, uint16_t src_port, uint16_t dst_port, void* data, int len) {
    int length = sizeof(udp_packet_t) + len;
    udp_packet_t* packet = calloc(length, 1);

    packet->src_port = hostToNet16(src_port);
    packet->dst_port = hostToNet16(dst_port);
    packet->length   = hostToNet16(length);
    packet->checksum = udp_calculate_checksum(packet);

    memcpy((void*)packet + sizeof(udp_packet_t), data, len);
    ip_send_packet(dst_ip, packet, length);
}

void udp_handle_packet(udp_packet_t* packet) {
    uint16_t dst_port = netToHost16(packet->dst_port);
    uint16_t length   = netToHost16(packet->length);

    void* data_ptr    = (void*)packet + sizeof(udp_packet_t);
    uint32_t data_len = length;
    kprintf("Received UDP packet, dst_port %d, data dump:\n", dst_port);

    if (netToHost16(packet->dst_port) == 68) dhcp_handle_packet(data_ptr);
    return;
}