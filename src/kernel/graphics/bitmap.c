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

    uint8_t* header = calloc(sizeof(bmp_fileheader_t), 1);
    FAT_read_content2buffer(content, header, 0, sizeof(bmp_fileheader_t));

    bmp_fileheader_t* h = header;
    uint32_t offset     = h->bfOffBits;

    free(header);

    uint8_t* info = calloc(sizeof(bmp_infoheader_t), 1);
    FAT_read_content2buffer(content, info, sizeof(bmp_fileheader_t), sizeof(bmp_infoheader_t));

    bmp_infoheader_t* inf = info;
    ret->width            = inf->biWidth;
    ret->height           = inf->biHeight;
    ret->header_offset    = offset;
    ret->file             = content;
    ret->bpp              = inf->biBitCount;

    free(info);
    return ret;
}

void BMP_display_at(bitmap_t* bmp, int screen_x, int screen_y) {
    if (bmp == NULL) {
        kprintf("Invalid bitmap\n");
        return;
    }

    if (bmp->width > gfx_mode.x_resolution || bmp->height > gfx_mode.y_resolution) {
        kprintf("Invalid resolution of bitmap W%i H%i\n", bmp->width, bmp->height);
        return;
    }
    
    for (int y = bmp->height - 1; y >= 0; y--) {
        uint32_t bytes_per_pixel = bmp->bpp / 8;
        uint32_t line_size       = bmp->width * bytes_per_pixel;

        uint32_t offset      = bmp->header_offset + y * line_size;
        uint8_t* line_bytes = calloc(line_size * sizeof(uint8_t), 1);

        FAT_read_content2buffer(bmp->file, line_bytes, offset, line_size);

        for (int x = 0; x < bmp->width; x++) {
            uint32_t color_index = x * bytes_per_pixel;
            uint8_t blue  = line_bytes[color_index];
            uint8_t green = line_bytes[color_index + 1];
            uint8_t red   = line_bytes[color_index + 2];
            
            // Combine the individual color components into a 32-bit color value
            uint32_t color = (red << 16) | (green << 8) | blue;
            
            GFX_draw_pixel(x + screen_x, (bmp->height - 1 - y) + screen_y, color);
        }

        free(line_bytes);
    }
}

void BMP_unload(bitmap_t* bitmap) {
    if (bitmap == NULL) return;
    FAT_unload_content_system(bitmap->file);
    free(bitmap);
}