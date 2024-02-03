#include "sections.h"

void writeCamToFramebufferRGB565(void *fb, void *img, u16 x, u16 y, u16 width, u16 height)
{
    u8 *fb_8 = (u8*) fb;
    u16 *img_16 = (u16*) img;
    int i, j, draw_x, draw_y;
    for(j = 0; j < height; j++) 
    {
        for(i = 0; i < width; i++) 
        {
            draw_y = y + height - j - 1;
            draw_x = x + i;
            u32 v = (draw_y + draw_x * height) * 3;
            u16 data = img_16[j * width + i];
            u8 b = ((data >> 11) & 0x1F) << 3;
            u8 g = ((data >> 5) & 0x3F) << 2;
            u8 r = (data & 0x1F) << 3;
            fb_8[v] = r;
            fb_8[v+1] = g;
            fb_8[v+2] = b;
        }
    }
}

void writeCamToPixels(unsigned char *pixels, void *img, u16 x0, u16 y0, u16 width, u16 height)
{
    u16 *img_16 = (u16 *) img;
    unsigned char *mat_ptr = pixels;
    u16 data;

    for (int j = y0; j < height; j++)
    {
        for (int i = x0; i < width; i++)
        {
            data = img_16[j * width + i];
            
            mat_ptr[0] = (unsigned char) ((data & 0x1F) << 3);
            mat_ptr[1] = (unsigned char) (((data >> 5) & 0x3F) << 2);
            mat_ptr[2] = (unsigned char) (((data >> 11) & 0x1F) << 3);

            mat_ptr+=3;            
        }
    }
}