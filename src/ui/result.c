#include <stdlib.h>
#include "ui.h"

static lv_group_t *group    = NULL;
static lv_obj_t   *box_list = NULL;  // LVGL Objects
static BoxVec      s_objects;

static void object_display_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t       *item = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        unsigned char *pixels = malloc(sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
        writeCamToPixels(pixels, 0, 0, WIDTH_TOP, HEIGHT_TOP);

        int idx = lv_obj_get_child_id(item) - 2;
        if (idx < 0) {
            draw_boxxes(pixels, WIDTH_TOP, HEIGHT_TOP, &s_objects);
        } else {
            BoxInfo obj = BoxVec_getItem(idx, &s_objects);

            BoxVec box_Vec_temp;
            create_box_vector(&box_Vec_temp, 1);
            BoxVec_push_back(obj, &box_Vec_temp);
            draw_boxxes(pixels, WIDTH_TOP, HEIGHT_TOP, &box_Vec_temp);
            BoxVec_free(&box_Vec_temp);
        }

        writePixelsToFrameBuffer(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), pixels, 0, 0, WIDTH_TOP, HEIGHT_TOP);
        gfxFlushBuffers();
        gfxScreenSwapBuffers(GFX_TOP, true);
        gspWaitForVBlank();

        free(pixels);
    }
}

static void quit_detect_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        g_camState = CAM_STREAM;
        ui_layer_pop();
    }
}

static void create_box_list()
{
    box_list = lv_list_create(lv_scr_act());
    lv_obj_set_size(box_list, WIDTH_BTM, 160);
    lv_obj_align(box_list, LV_ALIGN_TOP_MID, 0, 48);

    lv_obj_t *btn;
    lv_list_add_text(box_list, "Press B to continue");
    char detected[40];
    sprintf(detected, "Found %d items", s_objects.num_item);
    btn = lv_list_add_btn(box_list, LV_SYMBOL_FILE, detected);
    lv_obj_add_event_cb(btn, object_display_cb, LV_EVENT_ALL, 0);
    lv_group_add_obj(group, btn);

    for (size_t i = 0; i < s_objects.num_item; i++) {
        BoxInfo obj = BoxVec_getItem(i, &s_objects);
        char    list_item[40];
        int     label = obj.label;
        int     x1    = obj.x1;
        int     x2    = obj.x2;
        int     y1    = obj.y1;
        int     y2    = obj.y2;
        float   prob  = obj.prob * 100;

        sprintf(list_item, "%15s %03.1f [%3d,%3d,%3d,%3d]", class_names[label], prob, x1, x2, y1, y2);
        btn = lv_list_add_btn(box_list, LV_SYMBOL_GPS, list_item);

        lv_obj_add_event_cb(btn, object_display_cb, LV_EVENT_ALL, 0);
        lv_group_add_obj(group, btn);
    }
}

static void ui_result_onCreate()
{
    group = lv_group_create();
    encoder_driver_set_group(group);

    // Inference
    unsigned char *pixels = malloc(sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
    writeCamToPixels(pixels, 0, 0, WIDTH_TOP, HEIGHT_TOP);
    s_objects = g_det.detect(pixels, WIDTH_TOP, HEIGHT_TOP, &g_det);

    // Print inference outputs
    draw_boxxes(pixels, WIDTH_TOP, HEIGHT_TOP, &s_objects);
    writePixelsToFrameBuffer(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), pixels, 0, 0, WIDTH_TOP, HEIGHT_TOP);
    gfxFlushBuffers();
    gfxScreenSwapBuffers(GFX_TOP, true);
    gspWaitForVBlank();

    free(pixels);
    create_box_list();
}

static void ui_result_onEnter()
{
    create_btm_btn_container();
    add_btm_btn(BTN_B, quit_detect_cb, lv_pct(50), " Continue");
    add_btm_btn(BTN_A, NULL, lv_pct(50), " Select");
}

static void ui_result_update() {}

static void ui_result_onLeave()
{
    remove_virtual_btn(BTN_B);
    remove_virtual_btn(BTN_A);
}

static void ui_result_onDestroy()
{
    BoxVec_free(&s_objects);
    lv_obj_del_async(box_list);
    box_list = NULL;
}

UI_CALLBACKS g_ui_result = {.onCreate  = ui_result_onCreate,
                            .onEnter   = ui_result_onEnter,
                            .update    = ui_result_update,
                            .onLeave   = ui_result_onLeave,
                            .onDestroy = ui_result_onDestroy,
                            .idx       = LAYER_RESULT};
