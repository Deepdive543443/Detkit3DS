#include "sections.h"

#define LV_DRAW_TEXT(SCR, TEXT) (lv_label_set_text(lv_label_create(lv_disp_get_scr_act(SCR)), TEXT))

static lv_disp_t *disp_top = NULL;
static lv_disp_t *disp_btm = NULL;

int main(int argc, char **argv)
{
    lv_init();
    disp_top = display_init(GFX_TOP);
    lv_obj_set_style_bg_color(lv_disp_get_scr_act(disp_top), lv_color_hex(0xf4cccc), 0);
    lv_obj_set_flex_flow(lv_disp_get_scr_act(disp_top), LV_FLEX_ALIGN_CENTER);
    LV_DRAW_TEXT(disp_top, "Hello LVGL!");

    disp_btm = display_init(GFX_BOTTOM);
    lv_obj_set_flex_flow(lv_disp_get_scr_act(disp_btm), LV_FLEX_FLOW_COLUMN);

    LV_DRAW_TEXT(disp_btm, LV_SYMBOL_BATTERY_EMPTY);
    LV_DRAW_TEXT(disp_btm, LV_SYMBOL_BATTERY_1);
    LV_DRAW_TEXT(disp_btm, LV_SYMBOL_BATTERY_2);
    LV_DRAW_TEXT(disp_btm, LV_SYMBOL_BATTERY_3);
    LV_DRAW_TEXT(disp_btm, LV_SYMBOL_BATTERY_FULL);

    dev_init();

    touchPosition touch;

    // Main loop
    while (aptMainLoop()) {
        lv_timer_handler();
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();
        hidTouchRead(&touch);

        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;  // break in order to return to hbmenu

        if (kDown & KEY_A) LV_DRAW_TEXT(disp_top, "Hello LVGL!");

        if (touch.px >= 5 && touch.py >= 5)
            lv_obj_set_style_bg_color(lv_disp_get_scr_act(disp_btm), lv_color_hex(0xffdbac), 0);

        else
            lv_obj_set_style_bg_color(lv_disp_get_scr_act(disp_btm), lv_color_hex(0xffffff), 0);

        frame_ctl();
    }

    dev_cleanup();
    gfxExit();
    return 0;
}