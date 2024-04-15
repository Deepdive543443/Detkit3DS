#include "sections.h"

static lv_obj_t *models         = NULL;
static lv_obj_t *hint_msg       = NULL;
static lv_obj_t *battery_symbol = NULL;

static void model_list_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t       *obj  = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        destroy_detector(&g_det);
        uint16_t model_idx = lv_dropdown_get_selected(obj);
        switch (model_idx) {
            case 0:
                g_det = create_nanodet_plus(320, "romfs:nanodet-plus-m_416_int8.param", "romfs:nanodet-plus-m_416_int8.bin");
                break;

            case 1:
                g_det = create_fastestdet(352, "romfs:FastestDet.param", "romfs:FastestDet.bin");
                break;

            default:
                break;
        };
    }
}

static void detect_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        g_camState = CAM_HANG;
        pause_cam_capture();

        if (g_ui_stack[g_num_ui_layer].idx == LAYER_RESULT) ui_layer_pop();

        ui_layer_join(LAYER_RESULT);
    }
}

static void tabview_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        g_camState = CAM_HANG;
        pause_cam_capture();

        ui_layer_join(LAYER_TABVIEW);
    }
}

static void create_model_list()
{
    models = lv_dropdown_create(lv_scr_act());
    lv_dropdown_set_options(models,
                            "NanoDet-plus\n"
                            "FastestDet");

    lv_obj_align(models, LV_ALIGN_TOP_MID, 0, -10);
    lv_obj_set_width(models, 150);
    lv_obj_add_event_cb(models, model_list_cb, LV_EVENT_ALL, 0);  // TODO
}

static void ui_stream_onCreate()
{
    create_LR(detect_cb);
    create_model_list();

    hint_msg = lv_label_create(lv_scr_act());
    lv_label_set_text(hint_msg, "Press L, R, or A to detect");
    lv_obj_center(hint_msg);

    battery_symbol = lv_label_create(lv_scr_act());
    lv_label_set_text(battery_symbol, LV_SYMBOL_BATTERY_3);
    lv_obj_align(battery_symbol, LV_ALIGN_TOP_RIGHT, -8, 30);
}

static void ui_stream_onEnter()
{
    create_btm_btn_container();
    add_btm_btn(BTN_Y, tabview_cb, lv_pct(50), " About");
    add_btm_btn(BTN_A, detect_cb, lv_pct(50), " Detect");
}

static void ui_stream_update() {}

static void ui_stream_onLeave()
{
    remove_virtual_btn(BTN_Y);
    remove_virtual_btn(BTN_A);
}

static void ui_stream_onDestroy()
{
    remove_LR();
    lv_obj_del_async(hint_msg);
    lv_obj_del_async(battery_symbol);

    hint_msg       = NULL;
    battery_symbol = NULL;
}

UI_activity ui_stream_activities()
{
    return (UI_activity){.onCreate  = ui_stream_onCreate,
                         .onEnter   = ui_stream_onEnter,
                         .update    = ui_stream_update,
                         .onLeave   = ui_stream_onLeave,
                         .onDestroy = ui_stream_onDestroy,
                         .idx       = LAYER_STREAMING};
}