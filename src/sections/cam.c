#include <stdlib.h>
#include "sections.h"

CamState      g_camState           = CAM_CLOSE;
static Handle s_camReceiveEvent[2] = {0};
static void  *s_cam_buf            = NULL;
static s32    s_index_handler      = 0;
static bool   s_captureInterrupted;
static u32    s_bufSize;

static void writeCamToFramebufferRGB565(void *fb, u16 x, u16 y, u16 width, u16 height)
{
    u8  *fb_8   = (u8 *)fb;
    u16 *img_16 = (u16 *)s_cam_buf;
    int  i, j, draw_x, draw_y;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            draw_y      = y + height - j - 1;
            draw_x      = x + i;
            u32 v       = (draw_y + draw_x * height) * 3;
            u16 data    = img_16[j * width + i];
            u8  b       = ((data >> 11) & 0x1F) << 3;
            u8  g       = ((data >> 5) & 0x3F) << 2;
            u8  r       = (data & 0x1F) << 3;
            fb_8[v]     = r;
            fb_8[v + 1] = g;
            fb_8[v + 2] = b;
        }
    }
}

static void writeCamToFramebufferRGB565_filter(void *fb, u16 x, u16 y, u16 width, u16 height, float weight)
{
    writeCamToFramebufferRGB565(fb, x, y, width, height);
    u8 *fb_8 = (u8 *)fb;
    int i, j, k, draw_x, draw_y;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            draw_y = y + height - j - 1;
            draw_x = x + i;
            u32 v  = (draw_y + draw_x * height) * 3;
            for (k = 0; k < 3; k++)
            {
                fb_8[v + k] *= (1 - weight);
                fb_8[v + k] += 255 * weight;
            }
        }
    }
}

void writeCamToPixels(unsigned char *pixels, u16 x0, u16 y0, u16 width, u16 height)
{
    u16           *img_16  = (u16 *)s_cam_buf;
    unsigned char *mat_ptr = pixels;
    u16            data;

    for (int j = y0; j < height; j++)
    {
        for (int i = x0; i < width; i++)
        {
            data = img_16[j * width + i];

            mat_ptr[0] = (unsigned char)((data & 0x1F) << 3);
            mat_ptr[1] = (unsigned char)(((data >> 5) & 0x3F) << 2);
            mat_ptr[2] = (unsigned char)(((data >> 11) & 0x1F) << 3);

            mat_ptr += 3;
        }
    }
}

void writePixelsToFrameBuffer(void *fb, unsigned char *pixels, u16 x, u16 y, u16 width, u16 height)
{
    u8            *fb_8       = (u8 *)fb;
    unsigned char *pixels_ptr = pixels;

    int draw_x, draw_y;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            draw_y = y + height - j - 1;
            draw_x = x + i;
            u32 v  = (draw_y + draw_x * height) * 3;

            fb_8[v]     = pixels_ptr[0];
            fb_8[v + 1] = pixels_ptr[1];
            fb_8[v + 2] = pixels_ptr[2];

            pixels_ptr += 3;
        }
    }
}

void pause_cam_capture()
{
    writeCamToFramebufferRGB565_filter(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 0, 0, WIDTH_TOP, HEIGHT_TOP,
                                       0.5);

    // Flush and swap framebuffers
    gfxFlushBuffers();
    gfxScreenSwapBuffers(GFX_TOP, true);
    gspWaitForVBlank();
}

void camSetup()
{
    // Camera framebuffer init
    s_cam_buf = malloc(SCRSIZE_TOP * 2);  // RBG565 frame buffer
    if (!s_cam_buf)
    {
        hang_err("Failed to allocate memory for Camera!");
    }

    camInit();
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSet3D(false);

    CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A);
    CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_15);
    CAMU_SetNoiseFilter(SELECT_OUT1, true);
    CAMU_SetAutoExposure(SELECT_OUT1, true);
    CAMU_SetAutoWhiteBalance(SELECT_OUT1, true);
    CAMU_SetTrimming(PORT_CAM1, false);

    CAMU_GetMaxBytes(&s_bufSize, WIDTH_TOP, HEIGHT_TOP);
    CAMU_SetTransferBytes(PORT_CAM1, s_bufSize, WIDTH_TOP, HEIGHT_TOP);
    CAMU_Activate(SELECT_OUT1);

    CAMU_GetBufferErrorInterruptEvent(&s_camReceiveEvent[0], PORT_CAM1);
    CAMU_ClearBuffer(PORT_CAM1);
    CAMU_StartCapture(PORT_CAM1);
    CAMU_PlayShutterSound(SHUTTER_SOUND_TYPE_MOVIE);
    g_camState = CAM_STREAM;
}

bool camUpdate()
{
    if (g_camState == CAM_HANG) return 0;

    if (s_camReceiveEvent[1] == 0)
    {
        CAMU_SetReceiving(&s_camReceiveEvent[1], s_cam_buf, PORT_CAM1, SCRSIZE_TOP * 2, (s16)s_bufSize);
    }

    if (s_captureInterrupted)
    {
        CAMU_StartCapture(PORT_CAM1);
        s_captureInterrupted = false;
    }

    svcWaitSynchronizationN(&s_index_handler, s_camReceiveEvent, 2, false, WAIT_TIMEOUT);
    switch (s_index_handler)
    {
        case 0:
            svcCloseHandle(s_camReceiveEvent[1]);
            s_camReceiveEvent[1] = 0;
            s_captureInterrupted = true;

            return s_captureInterrupted;
            break;

        case 1:
            svcCloseHandle(s_camReceiveEvent[1]);
            s_camReceiveEvent[1] = 0;

            break;

        default:
            break;
    }

    writeCamToFramebufferRGB565(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 0, 0, WIDTH_TOP, HEIGHT_TOP);

    // Flush and swap framebuffers
    gfxFlushBuffers();
    gfxScreenSwapBuffers(GFX_TOP, true);
    gspWaitForVBlank();

    return s_captureInterrupted;
}