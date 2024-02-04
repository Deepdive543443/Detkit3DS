#include "sections.h"


void pause_cam_capture(void *cam_buf)
{
    writeCamToFramebufferRGB565_filter(
        gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 
        cam_buf,
        0,
        0,
        WIDTH_TOP,
        HEIGHT_TOP,
        0.5
    );

    // Flush and swap framebuffers
    gfxFlushBuffers();
    gfxScreenSwapBuffers(GFX_TOP, true);
    gspWaitForVBlank(); 
}


void writeCamToFramebufferRGB565_filter(void *fb, void *img, u16 x, u16 y, u16 width, u16 height, float weight)
{
    writeCamToFramebufferRGB565(fb, img, x, y, width, height);
    u8 *fb_8 = (u8*) fb;
    int i, j, k, draw_x, draw_y;
    for(j = 0; j < height; j++) 
    {
        for(i = 0; i < width; i++) 
        {
            draw_y = y + height - j - 1;
            draw_x = x + i;
            u32 v = (draw_y + draw_x * height) * 3;
            for (k = 0; k < 3; k++)
            {
                fb_8[v + k] *= (1 - weight);
                fb_8[v + k] += 255 * weight;
            }
        }
    }
}

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