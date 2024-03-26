#include "sections/input.h"

static lv_indev_drv_t indev_drv_cross;
static lv_indev_t    *enc_indev = NULL;

static void virtual_press_cb(u32 key, lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    u32 kHeld = hidKeysHeld();

    if (kHeld & key)
    {
        data->btn_id = 0;
        data->state  = LV_INDEV_STATE_PRESSED;
    } else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void virtual_A_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) { virtual_press_cb(KEY_A, drv, data); }

void virtual_B_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) { virtual_press_cb(KEY_B, drv, data); }

void virtual_X_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) { virtual_press_cb(KEY_X, drv, data); }

void virtual_Y_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) { virtual_press_cb(KEY_Y, drv, data); }

void virtual_L_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) { virtual_press_cb(KEY_L, drv, data); }

void virtual_R_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) { virtual_press_cb(KEY_R, drv, data); }

void touch_cb_3ds(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    touchPosition touch;
    hidTouchRead(&touch);
    if (touch.px >= 5 && touch.py >= 5)
    {
        data->point.x = touch.px;
        data->point.y = touch.py;
        data->state   = LV_INDEV_STATE_PRESSED;
    } else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void encoder_cb_3ds(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    u32 kHeld = hidKeysHeld();

    data->state = LV_INDEV_STATE_PRESSED;
    if (kHeld & KEY_A)
    {
        data->key = LV_KEY_ENTER;
    } else if (kHeld & KEY_LEFT || kHeld & KEY_UP)
    {
        data->key = LV_KEY_LEFT;
    } else if (kHeld & KEY_RIGHT || kHeld & KEY_DOWN)
    {
        data->key = LV_KEY_RIGHT;
    } else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void encoder_driver_init()
{
    lv_indev_drv_init(&indev_drv_cross);
    indev_drv_cross.type    = LV_INDEV_TYPE_ENCODER;
    indev_drv_cross.read_cb = encoder_cb_3ds;
    enc_indev               = lv_indev_drv_register(&indev_drv_cross);
}

void encoder_driver_set_group(lv_group_t *group) { lv_indev_set_group(enc_indev, group); }