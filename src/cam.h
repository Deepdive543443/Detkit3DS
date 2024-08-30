#ifndef CAM_H
#define CAM_H

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

#endif  // CAM_H