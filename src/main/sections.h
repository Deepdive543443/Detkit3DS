#ifndef SECTIONS_H
#define SECTIONS_H

#include "stdio.h"
#include "stdlib.h"
#include "setjmp.h"

#include "3ds.h"
#include "lvgl/lvgl.h"

#include "detector/detector.h"

#define WIDTH_BTM 320
#define HEIGHT_BTM 240

#define WIDTH_TOP 400
#define HEIGHT_TOP 240
#define SCRSIZE_TOP WIDTH_TOP * HEIGHT_TOP

//Debug
void hang_err(const char *message);

//cam.c
#define WAIT_TIMEOUT 1000000000ULL

void pause_cam_capture(void *cam_buf);
void writeCamToFramebufferRGB565_filter(void *fb, void *img, u16 x, u16 y, u16 width, u16 height, float weight);
void writeCamToFramebufferRGB565(void *fb, void *img, u16 x, u16 y, u16 width, u16 height);
void writeCamToPixels(unsigned char *pixels, void *img, u16 x0, u16 y0, u16 width, u16 height);
void writePixelsToFrameBuffer(void *fb, unsigned char *pixels, u16 x0, u16 y0, u16 width, u16 height);

// display.c
void writePic2FrameBuf565(void *fb, lv_color_t * color, u16 x, u16 y, u16 w, u16 h);
void flush_cb_3ds_btm(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void flush_cb_3ds_top(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void flush_cb_3ds(gfxScreen_t gfx_scr, lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
lv_disp_t *display_init(gfxScreen_t gfx_scr, lv_disp_draw_buf_t *draw_buf, lv_color_t *buf1, lv_disp_drv_t *disp_drv);

// ui.c
void button_style_init(lv_style_t *btn);
void pop_up_tabview_cb(lv_event_t *e);
void close_tabview_cb(lv_event_t *e);
void quit_detect_cb(lv_event_t *e);
void object_display_cb(lv_event_t *e);
void detect_cb(lv_event_t *e);
lv_obj_t *create_box_list();
void model_list_hanlder(lv_event_t *e);
void create_model_list(Detector *det);
void create_LR();
void create_btm_btn_container();
void add_btm_btn(lv_obj_t *cont ,u32 key, void *callback, lv_coord_t width, const char *label);
void tab_add_icon_description(lv_obj_t *parent, lv_img_dsc_t *img, const char *description);
void tab_Ac_Li();

// input.c
void virtual_A_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_B_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_X_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_Y_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_L_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_R_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void virtual_press_cb(u32 key, lv_indev_drv_t *drv, lv_indev_data_t *data);
void touch_cb_3ds(lv_indev_drv_t *drv, lv_indev_data_t *data);
void encoder_cb_3ds(lv_indev_drv_t *drv, lv_indev_data_t *data);

#endif // SECTIONS_H