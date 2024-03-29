#include "sections.h"

static lv_disp_t *disp_top = NULL;
static lv_disp_t *disp_btm = NULL;

int main(int argc, char **argv)
{
    lv_init();
    disp_top = display_init(GFX_TOP);
    lv_obj_set_style_bg_color(lv_disp_get_scr_act(disp_top), lv_color_hex(0xf4cccc), 0);
    lv_obj_t *label = lv_label_create(lv_disp_get_scr_act(disp_top));
    lv_obj_center(label);
    lv_label_set_text(label, "Hello LVGL!");

    disp_btm = display_init(GFX_BOTTOM);
    lv_obj_set_flex_flow(lv_disp_get_scr_act(disp_btm), LV_FLEX_FLOW_COLUMN);
    for (int i = 0; i < 3; i++) {
        label = lv_label_create(lv_disp_get_scr_act(disp_btm));
        lv_label_set_text(label, "Hello LVGL!");
    }
    HALinit();

    // Main loop
    while (aptMainLoop()) {
        lv_timer_handler();
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;  // break in order to return to hbmenu
        main_loop_locker();
    }

    HAL_cleanup();
    gfxExit();
    return 0;
}