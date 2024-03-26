#include "sections.h"

static lv_img_dsc_t ncnn_bg_transprant;
static lv_obj_t    *bg = NULL;

static void ui_background_onCreate()
{
    add_res_depth16("romfs:widgets/bg/ncnn_bg_transparent.png", &ncnn_bg_transprant);
}

static void ui_background_onEnter()
{
    bg = lv_img_create(lv_scr_act());
    lv_img_set_src(bg, &ncnn_bg_transprant);
}

static void ui_background_update() {}

static void ui_background_onLeave() {}

static void ui_background_onDestroy()
{
    lv_obj_del_async(bg);
    bg = NULL;

    dealloc_res(&ncnn_bg_transprant);
}

UI_activity ui_background_activities()
{
    return (UI_activity){.onCreate  = ui_background_onCreate,
                         .onEnter   = ui_background_onEnter,
                         .update    = ui_background_update,
                         .onLeave   = ui_background_onLeave,
                         .onDestroy = ui_background_onDestroy,
                         .idx       = LAYER_BACKGROUND};
}