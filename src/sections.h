#ifndef SECTIONS_H
#define SECTIONS_H
#include <setjmp.h>
#include "3ds.h"
#include "lvgl/lvgl.h"
#include "detector_c/detector.h"

#define WIDTH_BTM 320
#define HEIGHT_BTM 240

#define WIDTH_TOP 400
#define HEIGHT_TOP 240
#define SCRSIZE_TOP WIDTH_TOP *HEIGHT_TOP

#define STACKSIZE (4 * 1024)
#define TICK_MS 1              // Millisec a tick
#define TICK_US TICK_MS * 1000 // Microsec a tick
#define TICK_NS TICK_US * 1000 // Nanosec a tick

// Glob
extern jmp_buf exitJmp;

extern Detector det;
extern BoxVec objects;
extern bool detecting;
extern void *cam_buf;

#define USE_SYS_CORE 0 /*Sys code leads to slower loading speed and it's not available in CIA*/
extern bool g_thread_ticking;

// cam.c
#define WAIT_TIMEOUT 1000000000ULL

void pause_cam_capture(void *cam_buf);
void writeCamToFramebufferRGB565_filter(void *fb, void *img, u16 x, u16 y, u16 width, u16 height, float weight);
void writeCamToFramebufferRGB565(void *fb, void *img, u16 x, u16 y, u16 width, u16 height);
void writeCamToPixels(unsigned char *pixels, void *img, u16 x0, u16 y0, u16 width, u16 height);
void writePixelsToFrameBuffer(void *fb, unsigned char *pixels, u16 x0, u16 y0, u16 width, u16 height);
void camSetup();
bool camUpdate();

// display.c
void writePic2FrameBuf565(void *fb, lv_color_t *color, u16 x, u16 y, u16 w, u16 h);
void flush_cb_3ds_btm(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void flush_cb_3ds_top(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
lv_disp_t *display_init(gfxScreen_t gfx_scr, lv_disp_draw_buf_t *draw_buf, lv_color_t *buf1, lv_disp_drv_t *disp_drv);

// ui.c
void pop_up_tabview_cb(lv_event_t *e);
void close_tabview_cb(lv_event_t *e);
void quit_detect_cb(lv_event_t *e);
void object_display_cb(lv_event_t *e);
void detect_cb(lv_event_t *e);
void res_init();
void widgets_init();
void res_cleanup();

// input.c
void virtual_A_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_B_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_X_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_Y_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_L_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_R_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void touch_cb_3ds(lv_indev_drv_t *drv, lv_indev_data_t *data);
void encoder_cb_3ds(lv_indev_drv_t *drv, lv_indev_data_t *data);

// thread.c
void main_loop_locker();
void hang_err(const char *message);
void HALinit();
void HAL_cleanup();

#endif // SECTIONS_H