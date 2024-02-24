#include "../../include/rtl8139.h"


pci_dev_t pci_rtl8139_device;
rtl8139_dev_t rtl8139_device;

uint32_t current_packet_ptr;

// Four TXAD register, you must use a different one to send packet each time(for example, use the first one, second... fourth and back to the first)
uint8_t TSAD_array[4] = {0x20, 0x24, 0x28, 0x2C};
uint8_t TSD_array[4]  = {0x10, 0x14, 0x18, 0x1C};


void receive_packet() {
    uint16_t* t = (uint16_t*)(rtl8139_device.rx_buffer + current_packet_ptr);
    uint16_t packet_length = *(t + 1);

    // Skip, packet header and packet length, now t points to the packet data
    t = t + 2;

    // Now, ethernet layer starts to handle the packet(be sure to make a copy of the packet, insteading of using the buffer)
    // and probabbly this should be done in a separate thread...
    void* packet = calloc(packet_length, 1);
    memcpy(packet, t, packet_length);
    ethernet_handle_packet(packet, packet_length);

    current_packet_ptr = (current_packet_ptr + packet_length + 4 + 3) & RX_READ_POINTER_MASK;
    if(current_packet_ptr > RX_BUFFER_SIZE) current_packet_ptr -= RX_BUFFER_SIZE;

    i386_outw(rtl8139_device.io_base + CAPR, current_packet_ptr - 0x10);
}

void rtl8139_handler(Registers* reg) {
    uint16_t status = i386_inw(rtl8139_device.io_base + 0x3E);
    i386_outw(rtl8139_device.io_base + 0x3E, 0x05);

    if (status & TOK) kprintf("Packet sent\n");
    if (status & ROK) {
        kprintf("Packet recieved\n");
        receive_packet();
    }
}

void read_mac_addr() {
    uint32_t mac_part1 = i386_inl(rtl8139_device.io_base + 0x00);
    uint16_t mac_part2 = i386_inw(rtl8139_device.io_base + 0x04);
    
    rtl8139_device.mac_addr[0] = mac_part1 >> 0;
    rtl8139_device.mac_addr[1] = mac_part1 >> 8;
    rtl8139_device.mac_addr[2] = mac_part1 >> 16;
    rtl8139_device.mac_addr[3] = mac_part1 >> 24;
    rtl8139_device.mac_addr[4] = mac_part2 >> 0;
    rtl8139_device.mac_addr[5] = mac_part2 >> 8;

    kprintf("\nMAC Address: %x:%x:%x:%x:%x:%x\n", 
                rtl8139_device.mac_addr[0], 
                rtl8139_device.mac_addr[1], 
                rtl8139_device.mac_addr[2], 
                rtl8139_device.mac_addr[3], 
                rtl8139_device.mac_addr[4], 
                rtl8139_device.mac_addr[5]
    );
}

void get_mac_addr(uint8_t* src_mac_addr) {
    memcpy(src_mac_addr, rtl8139_device.mac_addr, 6);
}

void rtl8139_send_packet(void* data, uint32_t len) {
    void* transfer_data = calloc(len, 1);
    void* phys_addr     = virtual2physical(transfer_data);
    memcpy(transfer_data, data, len);

    i386_outl(rtl8139_device.io_base + TSAD_array[rtl8139_device.tx_cur], (uint32_t)phys_addr);
    i386_outl(rtl8139_device.io_base + TSD_array[rtl8139_device.tx_cur++], len);
    if (rtl8139_device.tx_cur > 3) rtl8139_device.tx_cur = 0;

    free(transfer_data);
}

/*
 * Initialize the rtl8139 card driver
 * */
void i386_init_rtl8139() {
    pci_rtl8139_device = pci_get_device(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID, -1);
    uint32_t ret = pci_read(pci_rtl8139_device, PCI_BAR0);
    rtl8139_device.bar_type = ret & 0x1;
    rtl8139_device.io_base  = ret & (~0x3);
    rtl8139_device.mem_base = ret & (~0xf);
    kprintf("\nRTL_8139 USE %s ACCESS (BASE: %x)", 
                (rtl8139_device.bar_type == 0) ? "MEM BASED" : "PORT BASED", 
                (rtl8139_device.bar_type != 0) ? rtl8139_device.io_base : rtl8139_device.mem_base
    );
    
    rtl8139_device.tx_cur = 0;
    uint32_t pci_command_reg = pci_read(pci_rtl8139_device, PCI_COMMAND);
    if (!(pci_command_reg & (1 << 2))) {
        pci_command_reg |= (1 << 2);
        pci_write(pci_rtl8139_device, PCI_COMMAND, pci_command_reg);
    }

    kprintf("\nBASE PORT: %i", rtl8139_device.io_base);
    kprintf("\nMEM BASE:  %i", rtl8139_device.mem_base);

    i386_outb(rtl8139_device.io_base + 0x52, 0x0);
    i386_outb(rtl8139_device.io_base + 0x37, 0x10);
    while((i386_inb(rtl8139_device.io_base + 0x37) & 0x10) != 0) continue;


    // TODO: return to static allocation
    rtl8139_device.rx_buffer = calloc(RX_BUFFER_SIZE_EX, 1);
    i386_outl(rtl8139_device.io_base + 0x30, (uint32_t)virtual2physical(rtl8139_device.rx_buffer));

    i386_outw(rtl8139_device.io_base + 0x3C, 0x0005);
    i386_outb(rtl8139_device.io_base + 0x37, 0x0C);
    i386_outl(rtl8139_device.io_base + 0x44, 0xF);

    uint32_t irq_num = pci_read(pci_rtl8139_device, PCI_INTERRUPT_LINE);
    kprintf("\nRTL8139 IRQ REGISTERED [%i]", irq_num);
    i386_irq_registerHandler(irq_num, rtl8139_handler);
    read_mac_addr();
}