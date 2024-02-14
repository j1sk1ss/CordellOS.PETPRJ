#include "../include/bitmap.h"

bitmap_t* BMP_create(char* file_path) {
    bitmap_t* ret = malloc(sizeof(bitmap_t));
    struct FATContent* content = FAT_get_content(file_path);
    if (content->file == NULL) {
        kprintf("File not found\n");
        free(ret);
        FAT_unload_content_system(content);
        return NULL;
    }

    char* file_data    = FAT_read_content(content);
    char* data_pointer = file_data;
    FAT_unload_content_system(content);

    bmp_fileheader_t* h = data_pointer;
    unsigned int offset = h->bfOffBits;

    bmp_infoheader_t* info = data_pointer + sizeof(bmp_fileheader_t);
    ret->width        = info->biWidth;
    ret->height       = info->biHeight;
    ret->image_bytes  = (void*)((unsigned int)data_pointer + offset);
    ret->buf          = data_pointer;
    ret->total_size   = content->file->file_meta.file_size;
    ret->bpp          = info->biBitCount;
    ret->data_pointer = file_data;

    return ret;
}

void BMP_display(bitmap_t* bmp) {
    if (bmp == NULL) {
        kprintf("Invalid bitmap\n");
        return;
    }

    if (bmp->width > gfx_mode.x_resolution || bmp->height > gfx_mode.y_resolution) {
        kprintf("Invalid resolution of bitmap\n");
        return;
    }
    
    for (int y = bmp->height - 1; y >= 0; y--) {
        for (int x = 0; x < bmp->width; x++) {
            unsigned int pixel_offset = y * bmp->width + x;
            unsigned int color_index = pixel_offset * (bmp->bpp / 8);
            uint32_t color = *(uint32_t*)(bmp->image_bytes + color_index);
            GFX_draw_pixel(x, bmp->height - 1 - y, color);
        }
    }
}

void BMP_display_at(bitmap_t* bmp, int screen_x, int screen_y) {
    if (bmp == NULL) {
        kprintf("Invalid bitmap\n");
        return;
    }

    if (bmp->width > gfx_mode.x_resolution || bmp->height > gfx_mode.y_resolution) {
        kprintf("Invalid resolution of bitmap\n");
        return;
    }

    for (int y = bmp->height - 1; y >= 0; y--) {
        for (int x = 0; x < bmp->width; x++) {
            unsigned int pixel_offset = y * bmp->width + x;
            unsigned int color_index = pixel_offset * (bmp->bpp / 8);
            uint32_t color = *(uint32_t*)(bmp->image_bytes + color_index);
            GFX_draw_pixel(x + screen_x, (bmp->height - 1 - y) + screen_y, color);
        }
    }
}

void BMP_unload(bitmap_t* bitmap) {
    if (bitmap == NULL) return;
    free(bitmap->data_pointer);
    free(bitmap);
}