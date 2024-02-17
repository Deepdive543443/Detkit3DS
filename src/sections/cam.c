#include "sections.h"

static u32 bufSize;
static Handle camReceiveEvent[2] = {0};
static bool captureInterrupted;
static s32 index_handler = 0;

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

void writePixelsToFrameBuffer(void *fb, unsigned char *pixels, u16 x, u16 y, u16 width, u16 height)
{
    u8 *fb_8 = (u8 *) fb;
    unsigned char *pixels_ptr = pixels;

    int draw_x, draw_y;
    for(int j = 0; j < height; j++) 
    {
        for(int i = 0; i < width; i++) 
        {
            draw_y = y + height - j - 1;
            draw_x = x + i;
            u32 v = (draw_y + draw_x * height) * 3;

            fb_8[v] = pixels_ptr[0];
            fb_8[v+1] = pixels_ptr[1];
            fb_8[v+2] = pixels_ptr[2];

            pixels_ptr += 3;
        }
    }
}

void camSetup()
{
    camInit();
    CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A);
    CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_15);
    CAMU_SetNoiseFilter(SELECT_OUT1, true);
    CAMU_SetAutoExposure(SELECT_OUT1, true);
    CAMU_SetAutoWhiteBalance(SELECT_OUT1, true);
    CAMU_SetTrimming(PORT_CAM1, false);

    CAMU_GetMaxBytes(&bufSize, WIDTH_TOP, HEIGHT_TOP);
    CAMU_SetTransferBytes(PORT_CAM1, bufSize, WIDTH_TOP, HEIGHT_TOP);
    CAMU_Activate(SELECT_OUT1);

    CAMU_GetBufferErrorInterruptEvent(&camReceiveEvent[0], PORT_CAM1);
    CAMU_ClearBuffer(PORT_CAM1);
    CAMU_StartCapture(PORT_CAM1);
    CAMU_PlayShutterSound(SHUTTER_SOUND_TYPE_MOVIE);

}

bool camUpdate()
{
    if (camReceiveEvent[1] == 0) 
    {
        CAMU_SetReceiving(&camReceiveEvent[1], cam_buf, PORT_CAM1, SCRSIZE_TOP * 2, (s16) bufSize);
    }

    if (captureInterrupted) 
    {
        CAMU_StartCapture(PORT_CAM1);
        captureInterrupted = false;
    }

    svcWaitSynchronizationN(&index_handler, camReceiveEvent, 2, false, WAIT_TIMEOUT);
    switch (index_handler)
    {
        case 0:
            svcCloseHandle(camReceiveEvent[1]);
            camReceiveEvent[1] = 0;
            captureInterrupted = true;

            return captureInterrupted;
            break;

        case 1:
            svcCloseHandle(camReceiveEvent[1]);
            camReceiveEvent[1] = 0;

            break;

        default:
            break;

    }

    writeCamToFramebufferRGB565(
        gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 
        cam_buf,
        0,
        0,
        WIDTH_TOP,
        HEIGHT_TOP
    );

    // Flush and swap framebuffers
    gfxFlushBuffers();
    gfxScreenSwapBuffers(GFX_TOP, true);
    gspWaitForVBlank();   

    return captureInterrupted;
}