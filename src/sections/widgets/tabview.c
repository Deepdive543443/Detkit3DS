#include "sections.h"

static lv_img_dsc_t ncnn_logo;
static lv_img_dsc_t logo_lvgl;
static lv_img_dsc_t devkitpro;
static lv_img_dsc_t ftpd_icon;
static lv_img_dsc_t citra_logo;

static lv_obj_t *tab_bg   = NULL;
static lv_obj_t *tab_view = NULL;

static void close_tabview_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        g_camState = CAM_STREAM;
        ui_layer_pop();
    }
}

static void tab_add_icon_description(lv_obj_t *parent, const lv_img_dsc_t *img, const char *description)
{
    lv_obj_t *icon_slider = lv_obj_create(parent);
    lv_obj_set_size(icon_slider, LV_SIZE_CONTENT, 50);
    lv_obj_set_style_pad_all(icon_slider, 0, 0);
    lv_obj_set_style_bg_opa(icon_slider, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(icon_slider, LV_OPA_TRANSP, 0);

    lv_obj_clear_flag(icon_slider, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(icon_slider, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(icon_slider, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t *icon = lv_img_create(icon_slider);
    lv_img_set_src(icon, img);
    lv_obj_set_style_max_height(icon, 48, 0);
    lv_obj_set_style_max_width(icon, 48, 0);

    lv_obj_t *label = lv_label_create(icon_slider);
    lv_label_set_text(label, description);
}

static void tab_Ac_Li()
{
    // Create the background
    tab_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(tab_bg, WIDTH_BTM, HEIGHT_BTM);
    lv_obj_center(tab_bg);
    lv_obj_set_style_bg_color(tab_bg, lv_color_hex(0x999999), 0);
    lv_obj_set_style_bg_opa(tab_bg, LV_OPA_70, 0);
    lv_obj_set_style_border_opa(tab_bg, LV_OPA_TRANSP, 0);

    // Create the container
    lv_obj_t *cont = lv_obj_create(tab_bg);
    lv_obj_set_size(cont, 290, 160);
    lv_obj_center(cont);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_radius(cont, 0, 0);
    lv_obj_set_style_border_color(cont, lv_color_hex(0xeeeeee), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    tab_view = lv_tabview_create(cont, LV_DIR_TOP, 40);
    lv_obj_set_size(tab_view, lv_pct(100), 160);

    lv_obj_t *tab1 = lv_tabview_add_tab(tab_view, "Acknowledgement");
    lv_obj_set_width(tab1, 280);
    lv_obj_set_style_radius(tab1, 8, 0);
    lv_obj_set_flex_flow(tab1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(tab1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t *description = lv_label_create(tab1);
    lv_obj_set_width(description, 250);
    lv_label_set_text(description, "This project is powered and inspired by:");
    tab_add_icon_description(tab1, &ncnn_logo, " NCNN");
    tab_add_icon_description(tab1, &logo_lvgl, " LVGL");
    tab_add_icon_description(tab1, &devkitpro, " DevkitPRO");
    tab_add_icon_description(tab1, &ftpd_icon, " FTPD PRO");
    tab_add_icon_description(tab1, &citra_logo, " Citra Emulator");

    // Licences
    lv_obj_t *tab2 = lv_tabview_add_tab(tab_view, "About");
    lv_obj_set_width(tab2, 280);
    lv_obj_t *licences = lv_label_create(tab2);
    lv_label_set_text(
        licences,
        "This software is provided 'as-is', without any express or implied warranty."
        "In no event will the authors be held liable for any damages arising from the use of this software. "
        "Permission is granted to anyone to use this software for any purpose, including commercial applications, "
        "and to alter it and redistribute it freely, subject to the following restrictions:\n\n"
        "1.   The origin of this software must not be misrepresented; you must not claim that you wrote the original"
        " software.If you use this software in a           product,"
        "an acknowledgment in the product documentation would be"

        "appreciated but is not required.\n"
        "2.   Altered source versions must be plainly marked as such, and must not be misrepresented as being the "
        "original software.\n"
        "3.   This notice may not be removed or altered from any source distribution.");
    lv_label_set_long_mode(licences, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(licences, 250);
}

static void ui_tabview_onCreate()
{
    add_res_depth16("romfs:widgets/logos/ncnn_48.png", &ncnn_logo);
    add_res_depth16("romfs:widgets/logos/lvgl_logo.png", &logo_lvgl);
    add_res_depth16("romfs:widgets/logos/devkitpro_logo.png", &devkitpro);
    add_res_depth16("romfs:widgets/logos/ftpd_logo.png", &ftpd_icon);
    add_res_depth16("romfs:widgets/logos/citra_logo.png", &citra_logo);
    g_camState = CAM_HANG;
    pause_cam_capture();
    tab_Ac_Li();
}

static void ui_tabview_onEnter()
{
    create_btm_btn_container();
    add_btm_btn(BTN_B, close_tabview_cb, lv_pct(100), " Quit");
}

static void ui_tabview_update() {}

static void ui_tabview_onLeave()
{
    remove_virtual_btn(BTN_B);
    g_camState = CAM_STREAM;
}

static void ui_tabview_onDestroy()
{
    lv_obj_del_async(tab_bg);
    dealloc_res(&ncnn_logo);
    dealloc_res(&logo_lvgl);
    dealloc_res(&devkitpro);
    dealloc_res(&ftpd_icon);
    dealloc_res(&citra_logo);
}

UI_activity ui_tabview_activities()
{
    return (UI_activity){.onCreate  = ui_tabview_onCreate,
                         .onEnter   = ui_tabview_onEnter,
                         .update    = ui_tabview_update,
                         .onLeave   = ui_tabview_onLeave,
                         .onDestroy = ui_tabview_onDestroy,
                         .idx       = LAYER_TABVIEW};
}