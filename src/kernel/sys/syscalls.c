#include "../include/syscalls.h"

void syscall(SYSCall* regs) {
    switch (regs->eax) {
        case SYS_PRINT:
            const char* data = (const char*)regs->ecx;
            kprintf(data);
        break;

        case SYS_PUTC:
            char cdata = (char)regs->ecx;
            kputc(cdata);
        break;

        case SYS_COLOR_PUTC:
            char ccdata = (char)regs->ebx;
            uint8_t color = (uint8_t)regs->ecx;
            kcputc(ccdata, color);
        break;

        case SYS_CLEAR:
            if (!is_vesa) VGA_clrscr();
            else VESA_clrscr();
        break;

        case SYS_SCREEN_COLOR:
            char screen_color = (char)regs->ecx;
            kset_color(screen_color);
        break;

        case SYS_SLEEP:
            const int sleep = (const int)regs->edx;
            TIME_sleep_milliseconds(sleep);
        break;

        case SYS_READ_KEYBOARD:
            char* wait_buffer = (char*)regs->ecx;
            wait_buffer[0] = keyboard_navigation();
        break;

        case SYS_GET_KEY_KEYBOARD:
            char* key_buffer = (char*)regs->ecx;
            char key = '\0';
            if (i386_inb(0x64) & 0x1) {
                key = i386_inb(0x60);
                key = get_character(key);
            }

            key_buffer[0] = key;
        break;

        case SYS_AREAD_KEYBOARD:
            uint8_t keyborad_color = (uint8_t)regs->edx;
            int keyboard_mode = (int)regs->ebx;

            regs->eax = keyboard_read(keyboard_mode, keyborad_color);
        break;

        case SYS_TIME:
            datetime_read_rtc();

            short* date_buffer = (short*)regs->ecx;
            date_buffer[0] = datetime_second;
            date_buffer[1] = datetime_minute;
            date_buffer[2] = datetime_hour;
            date_buffer[3] = datetime_day;
            date_buffer[4] = datetime_month;
            date_buffer[5] = datetime_year;
        break;

        case SYS_MALLOC:
            uint32_t size = regs->ebx;
            if (!malloc_list_head)
                mm_init(size);

            void* allocated_memory = kmalloc(size);
            merge_free_blocks();
            
            regs->eax = allocated_memory;
        break;

        case SYS_FREE:
            void* ptr_to_free = (void*)regs->ebx;
            if (ptr_to_free != NULL)
                kfree(ptr_to_free);
        break;

        case SYS_READ_FILE:
            char* rfile_path = (char*)regs->ebx;
            struct FATContent* content = FAT_get_content(rfile_path);
            regs->eax = (uint32_t)FAT_read_content(content);
            
            FAT_unload_content_system(content);
        break;

        case SYS_WRITE_FILE:
            char* wfile_path = (char*)regs->ebx;
            char* fdata = (char*)regs->ecx;

            FAT_edit_content(wfile_path, fdata);
        break;

        case SYS_OPENDIR:
            char* path = (char*)regs->ebx;
            regs->eax = FAT_directory_list(GET_CLUSTER_FROM_ENTRY(FAT_get_content(path)->directory->directory_meta), NULL, FALSE);
        break;

        case SYS_EXECUTE_FILE:
            char* exec_path = (char*)regs->ebx;
            char** argv = (char**)regs->edx;
            int args = (int)regs->ecx;
            int eresult = (int)regs->eax;

            eresult = FAT_ELF_execute_content(exec_path, args, argv);
        break;

        case SYS_CEXISTS:
            int* cresult = (int*)regs->ecx;
            char* epath = (char *)regs->ebx;
            cresult[0] = FAT_content_exists(epath);
        break;

        case SYS_FCREATE:
            char* mkfile_path = (char*)regs->ebx;
            char* mkfile_name = (char*)regs->ecx;

            char* fname = strtok(mkfile_name, ".");
            char* fexec = strtok(NULL, "."); 

            struct FATContent* mkfile_content = FAT_create_content(fname, FALSE, fexec);
            FAT_put_content(mkfile_path, mkfile_content);
            FAT_unload_files_system(mkfile_content);
        break;

        case SYS_DIRCREATE:
            char* mkdir_path = (char*)regs->ebx;
            char* mkdir_name = (char*)regs->ecx;

            struct FATContent* mkdir_content = FAT_create_content(mkdir_name, TRUE, "");
            FAT_put_content(mkdir_path, mkdir_content);
            FAT_unload_files_system(mkdir_content);
        break;

        case SYS_CDELETE:
            char* delete_path = (char*)regs->ebx;
            char* delete_name = (char*)regs->ecx;
            FAT_delete_content(delete_path, delete_name);
        break;

        case SYS_GET_SCRCHAR:
            char* scrresult = (char*)regs->ecx;
            int scr_x = (int)regs->ebx;
            int scr_y = (int)regs->ecx;

            if (!is_vesa) scrresult[0] = VGA_getchr(scr_x, scr_y);
            else scrresult[0] = GFX_get_char(scr_x, scr_y);
        break;

        case SYS_GET_CURSOR:
            int* cursor_cords = (int*)regs->ecx;

            cursor_cords[0] = VGA_cursor_get_x();
            cursor_cords[1] = VGA_cursor_get_y();
        break;

        case SYS_SET_CURSOR:
            int cur_x = (int)regs->ebx;
            int cur_y = (int)regs->ecx;

            if (!is_vesa) VGA_setcursor(cur_x, cur_y);
            else VESA_set_cursor(cur_x, cur_y);
        break;

        case SYS_SET_SCRCHAR:
            int pos_x = (int)regs->ebx;
            int pos_y = (int)regs->ecx;
            char new_char = (char)regs->edx;

            if (!is_vesa) VGA_putchr(pos_x, pos_y, new_char);
            else GFX_put_char(pos_x, pos_y, new_char, WHITE, BLACK);
        break;

        case SYS_SET_SCRCOLOR:
            int cpos_x = (int)regs->ebx;
            int cpos_y = (int)regs->ecx;
            uint8_t new_color = (uint8_t)regs->edx;

            VGA_putcolor(cpos_x, cpos_y, new_color);
        break;

        case SYS_CHANGE_META:
            char* meta_path = (char*)regs->ebx;
            directory_entry_t* meta = (char*)regs->ecx;

            FAT_change_meta(meta_path, meta);
        break;

        case SYS_START_PROCESS:
            char* process_name = (char*)regs->ebx;
            uint32_t address = (uint32_t)regs->ecx;

            START_PROCESS(process_name, address);
        break;

        case SYS_KILL_PROCESS:
            __kill();
        break;

        case SYS_PUT_PIXEL:
            uint16_t npix_x = (uint16_t)regs->ebx;
            uint16_t npix_y = (uint16_t)regs->ecx;
            uint32_t pixel  = (uint32_t)regs->edx;

            GFX_draw_pixel(npix_x, npix_y, pixel);
        break;

        case SYS_GET_PIXEL:
            uint32_t* pixelresult = (uint32_t*)regs->ecx;
            uint16_t pix_x        = (uint16_t)regs->ebx;
            uint16_t pix_y        = (uint16_t)regs->ecx;

            pixelresult[0] = GFX_get_pixel(pix_x, pix_y);
        break;
    }
}