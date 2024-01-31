#ifndef SECTIONS_H
#define SECTIONS_H

#include "stdlib.h"

#include "3ds.h"
#include "lvgl-8.3.11/lvgl.h"

#define WIDTH_BTM 320
#define HEIGHT_BTM 240

#define WIDTH_TOP 400
#define HEIGHT_TOP 240

//Debug
static char debug_string[30] = {"Bubug"};


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

typedef struct
{
    lv_obj_t *container;
    lv_obj_t *A;
    lv_obj_t *B;
    lv_obj_t *X;
    lv_obj_t *Y;
    lv_point_t *point_array_A;
    lv_point_t *point_array_B;
    lv_point_t *point_array_X;
    lv_point_t *point_array_Y;
} ui_ABXY_t;

typedef struct
{
    lv_group_t *g;
    lv_obj_t *list;
} lv_group_list_t;

void list_item_add_cb(lv_event_t *e);
void list_item_delete_cb(lv_event_t *e);
void color_flip_cb(lv_event_t *e);
void display_event_cb(lv_event_t *e);
lv_obj_t *create_box_list();
lv_obj_t *create_joystick();
ui_LR_t create_shoulder_button();
ui_ABXY_t create_ABXY();
lv_obj_t *create_bottom_container();
lv_obj_t *put_text_example(const char *string);
void update_joy_stick(lv_obj_t *js, circlePosition *js_read);

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