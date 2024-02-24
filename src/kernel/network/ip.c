#include "../include/ip.h"


uint8_t my_ip[]              = { 10, 0, 2, 14 };
uint8_t test_target_ip[]     = { 10, 0, 2, 15 };
uint8_t zero_hardware_addr[] = { 0, 0, 0, 0, 0, 0 };


void get_ip_str(char* ip_str, uint8_t* ip) {
    kprintf(ip_str, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

uint16_t ip_calculate_checksum(ip_packet_t* packet) {
    int array_size  = sizeof(ip_packet_t) / 2;
    uint16_t* array = (uint16_t*)packet;
    uint8_t* array2 = (uint8_t*)packet;
    uint32_t sum    = 0;
    for (int i = 0; i < array_size; i++) 
        sum += flipEndian16(array[i]);

    uint32_t carry = sum >> 16;

    sum = sum & 0x0000FFFF;
    sum = sum + carry;

    uint16_t ret = ~sum;
    return ret;
}

void ip_send_packet(uint8_t* dst_ip, void* data, int len) {
    int arp_sent = 3;
    ip_packet_t* packet = calloc(sizeof(ip_packet_t) + len, 1);

    packet->version              = IP_IPV4;
    packet->ihl                  = 5; // Internet Header Length
    packet->tos                  = 0; // Type of Service
    packet->length               = sizeof(ip_packet_t) + len;
    packet->id                   = 0;
    packet->flags                = 0;
    packet->fragment_offset_high = 0;
    packet->fragment_offset_low  = 0;
    packet->ttl                  = 64;
    packet->protocol             = PROTOCOL_UDP;

    get_host_addr(my_ip);
    memcpy(packet->src_ip, my_ip, 4);
    memcpy(packet->dst_ip, dst_ip, 4);

    void* packet_data = (void*)packet + packet->ihl * 4;
    memcpy(packet_data, data, len);

    *((uint8_t*)(&packet->version_ihl_ptr))   = hostToNetByte(*((uint8_t*)(&packet->version_ihl_ptr)), 4);
    *((uint8_t*)(packet->flags_fragment_ptr)) = hostToNetByte(*((uint8_t*)(packet->flags_fragment_ptr)), 3);

    packet->length          = hostToNet16(sizeof(ip_packet_t) + len);
    packet->header_checksum = 0;
    packet->header_checksum = hostToNet16(ip_calculate_checksum(packet));

    uint8_t dst_hardware_addr[6];
    while(!arp_lookup(dst_hardware_addr, dst_ip)) 
        if(arp_sent != 0) {
            arp_sent--;
            arp_send_packet(zero_hardware_addr, dst_ip);
        }

    ethernet_send_packet(dst_hardware_addr, packet, hostToNet16(packet->length), ETHERNET_TYPE_IP);
    kprintf("IP Packet Sent...(checksum: %x)\n", packet->header_checksum);
}


void ip_handle_packet(ip_packet_t* packet) {
    *((uint8_t*)(&packet->version_ihl_ptr))   = netToHostByte(*((uint8_t*)(&packet->version_ihl_ptr)), 4);
    *((uint8_t*)(packet->flags_fragment_ptr)) = netToHostByte(*((uint8_t*)(packet->flags_fragment_ptr)), 3);

    kprintf("Receive: the whole ip packet \n");
    
    uint8_t src_ip[20];
    if(packet->version == IP_IPV4) {
        get_ip_str(src_ip, packet->src_ip);

        void* data_ptr = (void*)packet + packet->ihl * 4;
        int data_len   = netToHost16(packet->length) - sizeof(ip_packet_t);

        kprintf("src: %s, data dump: \n", src_ip);
        if(packet->protocol == PROTOCOL_UDP) 
            udp_handle_packet(data_ptr);
    }
}