#include "../include/syscalls.h"


void i386_syscalls_init() {
    i386_isr_registerHandler(0x80, syscall);
}
 
void syscall(Registers* regs) {

    //=======================
    //  PRINT SYSCALLS
    //=======================

        if (regs->eax == SYS_PRINT) {
            const char* data = (const char*)regs->ecx;
            kprintf(data);
        } 
        
        else if (regs->eax == SYS_PUTC) {
            char data = (char)regs->ecx;
            kputc(data);
        } 
        
        else if (regs->eax == SYS_COLOR_PUTC) {
            char data     = (char)regs->ebx;
            uint8_t color = (uint8_t)regs->ecx;
            kcputc(data, color);
        } 
        
        else if (regs->eax == SYS_CLEAR) {
            if (!is_vesa) VGA_clrscr();
            else VESA_clrscr();
        } 
        
        else if (regs->eax == SYS_SCREEN_COLOR) {
            char screen_color = (char)regs->ecx;
            kset_color(screen_color);
        } 
        
        else if (regs->eax == SYS_GET_SCRCHAR) {
            char* result = (char*)regs->ecx;
            int x = (int)regs->ebx;
            int y = (int)regs->ecx;
            if (!is_vesa) result[0] = VGA_getchr(x, y);
            else result[0] = GFX_get_char(x, y);
        } 

        else if (regs->eax == SYS_GET_CURSOR) {
            int* cursor_cords = (int*)regs->ecx;

            if (!is_vesa) {
                cursor_cords[0] = VGA_cursor_get_x();
                cursor_cords[1] = VGA_cursor_get_y();
            }
            else {
                cursor_cords[0] = VESA_get_cursor_x();
                cursor_cords[1] = VESA_get_cursor_y();
            }
        } 
        
        else if (regs->eax == SYS_SET_CURSOR) {
            int x = (int)regs->ebx;
            int y = (int)regs->ecx;
            
            if (!is_vesa) VGA_setcursor(x, y);
            else VESA_set_cursor(x, y);
        } 
        
        else if (regs->eax == SYS_SET_SCRCHAR) {
            int x = (int)regs->ebx;
            int y = (int)regs->ecx;
            char new_char = (char)regs->edx;

            if (!is_vesa) VGA_putchr(x, y, new_char);
            else GFX_put_char(x, y, new_char, WHITE, BLACK);
        } 
        
        else if (regs->eax == SYS_SET_SCRCOLOR) {
            int x = (int)regs->ebx;
            int y = (int)regs->ecx;
            uint8_t new_color = (uint8_t)regs->edx;
            VGA_putcolor(x, y, new_color);
        } 

        //=======================
        //  KEYBOARD SYSCALLS
        //=======================

            else if (regs->eax == SYS_READ_KEYBOARD) {
                char* wait_buffer = (char*)regs->ecx;
                wait_buffer[0] = keyboard_navigation();
            } 
            
            else if (regs->eax == SYS_GET_KEY_KEYBOARD) {
                char* key_buffer = (char*)regs->ecx;
                char key = '\0';

                if (i386_inb(0x64) & 0x1) {
                    key = i386_inb(0x60);
                    key = get_character(key);
                }

                key_buffer[0] = key;
            } 
            
            else if (regs->eax == SYS_AREAD_KEYBOARD) {
                uint8_t keyborad_color = (uint8_t)regs->edx;
                int keyboard_mode      = (int)regs->ebx;
                regs->eax = keyboard_read(keyboard_mode, keyborad_color);
            } 
            
            else if (regs->eax == SYS_AREAD_KEYBOARD_STP) {
                uint8_t keyborad_color = (uint8_t)regs->edx;
                int keyboard_mode      = (int)regs->ebx;
                char* stop_list        = (char*)regs->ecx;
                regs->eax = keyboard_read_stop(keyboard_mode, keyborad_color, stop_list);
            } 

        //=======================
        //  KEYBOARD SYSCALLS
        //=======================

    //=======================
    //  PRINT SYSCALLS
    //=======================
    //  SYSTEM SYSCALLS
    //=======================

        else if (regs->eax == SYS_SLEEP) {
            const int sleep = (const int)regs->edx;
            TIME_sleep_milliseconds(sleep);
        } 

        else if (regs->eax == SYS_TIME) {
            datetime_read_rtc();
            short* date_buffer = (short*)regs->ecx;
            date_buffer[0] = datetime_second;
            date_buffer[1] = datetime_minute;
            date_buffer[2] = datetime_hour;
            date_buffer[3] = datetime_day;
            date_buffer[4] = datetime_month;
            date_buffer[5] = datetime_year;
        } 

        //=======================
        //  SYSTEM TASKING SYSCALLS
        //=======================

            else if (regs->eax == SYS_KILL_PROCESS) __kill();
            
            else if (regs->eax == SYS_START_PROCESS) {
                char* process_name = (char*)regs->ebx;
                uint32_t address   = (uint32_t)regs->ecx;
                START_PROCESS(process_name, address);
            } 

        //=======================
        //  SYSTEM TASKING SYSCALLS
        //=======================
        //  SYSTEM MEMMANAGER SYSCALLS
        //=======================

            else if (regs->eax == SYS_MALLOC) {
                uint32_t size = regs->ebx;
                if (!malloc_list_head)
                    mm_init(size);

                void* allocated_memory = kmalloc(size);
                merge_free_blocks();
                regs->eax = allocated_memory;
            } 
            
            else if (regs->eax == SYS_PAGE_MALLOC) {
                uint32_t address = regs->ebx;
                kmallocp(address);
                regs->eax = address;
            } 

            else if (regs->eax == SYS_FREE) {
                void* ptr_to_free = (void*)regs->ebx;
                if (ptr_to_free != NULL)
                    kfree(ptr_to_free);
            }

            else if (regs->eax == SYS_PAGE_FREE) {
                void* ptr_to_free = (void*)regs->ebx;
                if (ptr_to_free != NULL)
                    kfreep(ptr_to_free);
            }

        //=======================
        //  SYSTEM MEMMANAGER SYSCALLS
        //=======================

    //=======================
    //  SYSTEM SYSCALLS
    //=======================
    //  FILE SYSTEMS SYSCALLS
    //=======================

        else if (regs->eax == SYS_READ_FILE) {
            char* rfile_path           = (char*)regs->ebx;
            struct FATContent* content = current_vfs->getobj(rfile_path);
            regs->eax                  = (uint32_t)current_vfs->read(content);
            FATLIB_unload_content_system(content);
        } 
        
        else if (regs->eax == SYS_WRITE_FILE) {
            char* wfile_path = (char*)regs->ebx;
            char* data       = (char*)regs->ecx;
            current_vfs->write(wfile_path, data);
        } 
        
        else if (regs->eax == SYS_OPENDIR) {
            char* path = (char*)regs->ebx;
            regs->eax  = current_vfs->dir(GET_CLUSTER_FROM_ENTRY(current_vfs->getobj(path)->directory->directory_meta), NULL, FALSE);
        } 
        
        else if (regs->eax == SYS_GET_CONTENT) {
            char* content_path = (char*)regs->ebx;
            regs->eax = current_vfs->getobj(content_path);
        } 
        
        else if (regs->eax == SYS_EXECUTE_FILE) {
            char* exec_path = (char*)regs->ebx;
            char** argv     = (char**)regs->edx;
            int args        = (int)regs->ecx;
            int result      = (int)regs->eax;
            result = current_vfs->objexec(exec_path, args, argv);
        } 
        
        else if (regs->eax == SYS_CEXISTS) {
            int* result = (int*)regs->ecx;
            char* path  = (char *)regs->ebx;
            result[0]   = current_vfs->objexist(path);
        } 
        
        else if (regs->eax == SYS_FCREATE) {
            char* mkfile_path = (char*)regs->ebx;
            char* mkfile_name = (char*)regs->ecx;

            char* fname = strtok(mkfile_name, ".");
            char* fexec = strtok(NULL, "."); 

            struct FATContent* mkfile_content = current_vfs->crobj(fname, FALSE, fexec);

            current_vfs->putobj(mkfile_path, mkfile_content);
            FATLIB_unload_files_system(mkfile_content);
        } 
        
        else if (regs->eax == SYS_DIRCREATE) {
            char* mkdir_path = (char*)regs->ebx;
            char* mkdir_name = (char*)regs->ecx;

            struct FATContent* mkdir_content = current_vfs->crobj(mkdir_name, TRUE, "");

            current_vfs->putobj(mkdir_path, mkdir_content);
            FATLIB_unload_files_system(mkdir_content);
        } 
        
        else if (regs->eax == SYS_CDELETE) {
            char* delete_path = (char*)regs->ebx;
            char* delete_name = (char*)regs->ecx;
            current_vfs->delobj(delete_path, delete_name);
        } 
        
        else if (regs->eax == SYS_CHANGE_META) {
            char* meta_path = (char*)regs->ebx;
            directory_entry_t* meta = (char*)regs->ecx;
            current_vfs->objmetachg(meta_path, meta);
        } 

        else if (regs->eax == SYS_READ_FILE_OFF) {
            struct FATContent* read_off_file = (struct FATContent*)regs->ebx;
            int offset                       = (int)regs->ecx;
            uint8_t* offset_buffer           = (uint8_t*)regs->edx;
            int offset_len                   = (int)regs->esi;
            
            current_vfs->readoff(read_off_file, offset_buffer, offset, offset_len);
        }

    //=======================
    //  FILE SYSTEMS SYSCALLS
    //=======================
    //  GRAPHICS SYSCALLS
    //=======================
    
        else if (regs->eax == SYS_PUT_PIXEL) {
            uint16_t x      = (uint16_t)regs->ebx;
            uint16_t y      = (uint16_t)regs->ecx;
            uint32_t pixel  = (uint32_t)regs->edx;
            GFX_draw_pixel(x, y, pixel);
        } 
        
        else if (regs->eax == SYS_VPUT_PIXEL) {
            uint16_t x      = (uint16_t)regs->ebx;
            uint16_t y      = (uint16_t)regs->ecx;
            uint32_t pixel  = (uint32_t)regs->edx;
            GFX_vdraw_pixel(x, y, pixel);
        } 

        else if (regs->eax == SYS_GET_PIXEL) {
            uint32_t* pixel = (uint32_t*)regs->edx;
            uint16_t x      = (uint16_t)regs->ebx;
            uint16_t y      = (uint16_t)regs->ecx;
            pixel[0] = GFX_get_pixel(x, y);
        } 

        else if (regs->eax == SYS_FBUFFER_SWIPE) {
            GFX_buffer2buffer();
        }

        else if (regs->eax == SYS_GET_RESOLUTION_X) {
            int* resolution = (int*)regs->edx;
            resolution[0] = gfx_mode.x_resolution;
        }

        else if (regs->eax == SYS_GET_RESOLUTION_Y) {
            int* resolution = (int*)regs->edx;
            resolution[0] = gfx_mode.y_resolution;
        }

    //=======================
    //  GRAPHICS SYSCALLS
    //=======================
    //  NETWORKING SYSCALLS
    //=======================

        else if (regs->eax == SYS_SET_IP) {
            uint8_t* ip = (uint8_t*)regs->ebx;
            IP_set(ip);
        }

        else if (regs->eax == SYS_GET_IP) {
            uint8_t* buffer = (uint8_t*)regs->ebx;
            IP_get(buffer);
        }

        else if (regs->eax == SYS_GET_MAC) {
            uint8_t* buffer = (uint8_t*)regs->ebx;
            get_mac_addr(buffer);
        }

        else if (regs->eax == SYS_SEND_UDP_PACKET) {
            uint8_t* dst_ip   = (uint8_t*)regs->ebx;
            uint16_t src_port = (uint16_t)regs->ecx;
            uint16_t dst_port = (uint16_t)regs->edx;
            void* data        = (void*)regs->esi;
            int len           = (int)regs->edi;

            UDP_send_packet(dst_ip, src_port, dst_port, data, len);
        }

        else if (regs->eax == SYS_GET_UDP_PACKETS) {
            uint8_t* data = (uint8_t*)regs->ebx;
            struct UDPpacket* packet = UDP_pop_packet();
            if (packet == NULL) return;

            memcpy(data, packet->data, packet->data_size);

            free(packet->data);
            free(packet);
        }

        else if (regs->eax == SYS_RESTART) {
            i386_reboot();
        }

        else if (regs->eax == SYS_GET_FS_INFO) {
            uint32_t* buffer = (uint32_t*)regs->ebx;

            buffer[0] = (uint32_t)current_vfs->device->mountpoint;
            buffer[1] = (uint32_t)current_vfs->name;
            buffer[2] = (uint32_t)fat_type;
            buffer[3] = (uint32_t)total_clusters;
            buffer[4] = (uint32_t)total_sectors;
            buffer[5] = (uint32_t)bytes_per_sector;
            buffer[6] = (uint32_t)sectors_per_cluster;
            buffer[7] = (uint32_t)fat_size;
        }

    //=======================
    //  NETWORKING SYSCALLS
    //=======================

}