#ifndef SECTIONS_H
#define SECTIONS_H

#include "stdlib.h"

#include "3ds.h"
#include "lvgl-8.3.11/lvgl.h"

#include "detector/detector.h"

#define WIDTH_BTM 320
#define HEIGHT_BTM 240

#define WIDTH_TOP 400
#define HEIGHT_TOP 240
#define SCRSIZE_TOP WIDTH_TOP * HEIGHT_TOP

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
typedef struct
{
    lv_obj_t *L;
    lv_obj_t *R;
    lv_point_t *point_array_L;
    lv_point_t *point_array_R;
} ui_LR_t;

void list_item_add_cb(lv_event_t *e);
void list_item_delete_cb(lv_event_t *e);
void display_event_cb(lv_event_t *e);
lv_obj_t *create_box_list();
ui_LR_t create_shoulder_button(Detector *det, void *cam_buf, bool *detecting);
lv_obj_t *put_text_example(const char *string);
void model_list_hanlder(lv_event_t *e);
lv_obj_t *create_model_list(Detector *det);
ui_LR_t create_bottom_btn();

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