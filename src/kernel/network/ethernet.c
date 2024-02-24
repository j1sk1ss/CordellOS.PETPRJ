#include "../include/ethernet.h"


int ethernet_send_packet(uint8_t* dst_mac_addr, uint8_t* data, int len, uint16_t protocol) {
    uint8_t src_mac_addr[6];
    ethernet_frame_t* frame = calloc(sizeof(ethernet_frame_t) + len, 1);
    void* frame_data = (void*)frame + sizeof(ethernet_frame_t);

    get_mac_addr(src_mac_addr);
    memcpy(frame->src_mac_addr, src_mac_addr, 6);
    memcpy(frame->dst_mac_addr, dst_mac_addr, 6);
    memcpy(frame_data, data, len);

    frame->type = hostToNet16(protocol);

    rtl8139_send_packet(frame, sizeof(ethernet_frame_t) + len);
    free(frame);

    return len;
}

void ethernet_handle_packet(ethernet_frame_t* packet, int len) {
    void* data = (void*) packet + sizeof(ethernet_frame_t);
    int data_len = len - sizeof(ethernet_frame_t);
    // ARP packet
    if(netToHost16(packet->type) == ETHERNET_TYPE_ARP) {
        kprintf("(ARP Packet)\n");
        arp_handle_packet(data, data_len);
    }
    // IP packets(could be TCP, UDP or others)
    if(netToHost16(packet->type) == ETHERNET_TYPE_IP) {
        kprintf("(IP Packet)\n");
        ip_handle_packet(data);
    }
}

/*
 * Initialize the ethernet layer
 * */
void ethernet_init() {
    return;
}