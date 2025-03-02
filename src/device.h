#ifndef DEVICE_H
#define DEVICE_H

#include "sections.h"

typedef enum {
    CAM_CLOSE,
    CAM_STREAM,
    CAM_HANG
} CamState;

extern CamState g_camState;
#define WAIT_TIMEOUT 1000000000ULL
void writeCamToPixels(unsigned char *pixels, u16 x0, u16 y0, u16 width, u16 height);
void writePixelsToFrameBuffer(void *fb, unsigned char *pixels, u16 x0, u16 y0, u16 width, u16 height);
void pause_cam_capture();
void camSetup();
bool camUpdate();

void       writePic2FrameBuf565(void *fb, lv_color_t *color, u16 x, u16 y, u16 w, u16 h);
lv_disp_t *display_init(gfxScreen_t gfx_scr);

void frame_ctl();
void hang_err(const char *message);
void dev_init();
void dev_cleanup();

#endif  // DEVICE_H