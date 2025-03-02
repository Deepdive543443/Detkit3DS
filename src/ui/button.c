#include "sections.h"

static void (*s_virbtn_callbacks[NUM_BTN])() = {
    virtual_A_cb, virtual_B_cb, virtual_X_cb, virtual_Y_cb, virtual_L_cb, virtual_R_cb,
};
static lv_point_t     s_btn_corr_list[NUM_BTN][1];
static lv_indev_t    *s_btn_indev_list[NUM_BTN];
static lv_obj_t      *s_btn_obj_list[NUM_BTN];
static lv_indev_drv_t s_drv_virbtn[NUM_BTN];  // Encoder A, B, X, Y

static lv_style_t btn_btm;
static lv_style_t btn_press;
static lv_style_t btn_shoulder_press;

static lv_obj_t *btm_btn_container;

static lv_img_dsc_t cam_icon;
static lv_img_dsc_t cam_icon_flip;
static lv_img_dsc_t iconL;
static lv_img_dsc_t iconR;
static lv_img_dsc_t Mid_fill;

static void button_style_init(lv_style_t *btn)
{
    lv_style_init(btn);
    lv_style_set_border_width(btn, 1);
    lv_style_set_border_color(btn, lv_color_hex(0x999999));
    lv_style_set_radius(btn, 0);
    lv_style_set_pad_all(btn, 0);
    lv_style_set_bg_opa(btn, LV_OPA_COVER);
    lv_style_set_bg_grad_color(btn, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_dir(btn, LV_GRAD_DIR_VER);
}

static void virtual_button_driver_init()
{
    for (int i = 0; i < 4; i++) {
        s_drv_virbtn[i].type    = LV_INDEV_TYPE_BUTTON;
        s_drv_virbtn[i].read_cb = s_virbtn_callbacks[i];
        s_btn_indev_list[i]     = lv_indev_drv_register(&s_drv_virbtn[i]);
        s_btn_corr_list[i][0]   = (lv_point_t){-1, -1};
        lv_indev_set_button_points(s_btn_indev_list[i], s_btn_corr_list[i]);
    }
}

void create_btm_btn_container()
{
    btm_btn_container = lv_obj_create(lv_scr_act());

    // lv_obj_clean(btm_btn_container);
    lv_obj_remove_style_all(btm_btn_container);
    lv_obj_set_size(btm_btn_container, lv_pct(100), 30);

    lv_obj_set_style_radius(btm_btn_container, 0, 0);
    lv_obj_set_style_pad_all(btm_btn_container, 0, 0);

    lv_obj_align(btm_btn_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(btm_btn_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btm_btn_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
}

void add_btm_btn(Button key, void *callback, lv_coord_t width, const char *label)
{
    lv_obj_t *btn_ptr    = NULL;
    lv_obj_t *icon       = NULL;
    lv_obj_t *label_btn  = NULL;
    lv_obj_t *label_icon = NULL;

    char icon_label[2] = {'\0'};

    switch (key) {
        case BTN_A:

            s_btn_obj_list[BTN_A] = lv_btn_create(btm_btn_container);
            btn_ptr               = s_btn_obj_list[BTN_A];
            icon_label[0]         = 'A';
            break;

        case BTN_B:

            s_btn_obj_list[BTN_B] = lv_btn_create(btm_btn_container);
            btn_ptr               = s_btn_obj_list[BTN_B];
            icon_label[0]         = 'B';
            break;

        case BTN_X:

            s_btn_obj_list[BTN_X] = lv_btn_create(btm_btn_container);
            btn_ptr               = s_btn_obj_list[BTN_X];
            icon_label[0]         = 'X';
            break;

        case BTN_Y:

            s_btn_obj_list[BTN_Y] = lv_btn_create(btm_btn_container);
            btn_ptr               = s_btn_obj_list[BTN_Y];
            icon_label[0]         = 'Y';
            break;

        default:
            hang_err("Fail to create button");
            break;
    }

    // Style
    lv_obj_remove_style_all(btn_ptr);
    lv_obj_set_size(btn_ptr, width, 30);
    lv_obj_add_style(btn_ptr, &btn_btm, 0);
    lv_obj_add_style(btn_ptr, &btn_press, LV_STATE_PRESSED);

    // Icon
    icon       = lv_obj_create(btn_ptr);
    label_icon = lv_label_create(icon);
    lv_label_set_text(label_icon, icon_label);
    lv_obj_set_style_text_color(label_icon, lv_color_hex(0x353535), 0);
    lv_obj_set_style_text_font(label_icon, &lv_font_montserrat_12, 0);
    lv_obj_center(label_icon);

    lv_obj_set_size(icon, 19, 19);
    lv_obj_set_style_radius(icon, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(icon, true, 0);
    lv_obj_set_scrollbar_mode(icon, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(icon, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(icon, 1, 0);
    lv_obj_set_style_border_color(icon, lv_color_hex(0x353535), 0);
    lv_obj_set_style_pad_all(icon, 0, 0);

    // Label
    label_btn = lv_label_create(btn_ptr);
    lv_label_set_text(label_btn, label);
    lv_obj_set_style_text_font(label_btn, &lv_font_montserrat_16, 0);

    // Layout: Flex
    lv_obj_set_flex_flow(btn_ptr, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_ptr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Callback
    lv_obj_add_event_cb(btn_ptr, callback, LV_EVENT_ALL, 0);

    lv_obj_update_layout(btn_ptr);
    lv_point_t initial_point = {(btn_ptr->coords.x1 + btn_ptr->coords.x2) / 2,
                                (btn_ptr->coords.y1 + btn_ptr->coords.y2) / 2};

    s_btn_corr_list[key][0] = initial_point;
    lv_indev_set_button_points(s_btn_indev_list[key], s_btn_corr_list[key]);
}

void create_LR(void *callback)
{
    add_res_depth16("romfs:widgets/button/cam_icon.png", &cam_icon);
    add_res_depth16("romfs:widgets/button/cam_icon_flip.png", &cam_icon_flip);
    add_res_depth16("romfs:widgets/button/iconL.png", &iconL);
    add_res_depth16("romfs:widgets/button/iconR.png", &iconR);
    add_res_depth16("romfs:widgets/button/Mid_fill.png", &Mid_fill);

    s_btn_obj_list[BTN_L] = lv_imgbtn_create(lv_scr_act());
    lv_imgbtn_set_src(s_btn_obj_list[BTN_L], LV_IMGBTN_STATE_RELEASED, &iconL, &Mid_fill, &cam_icon);
    lv_obj_align(s_btn_obj_list[BTN_L], LV_ALIGN_TOP_LEFT, 0, -10);
    lv_obj_set_size(s_btn_obj_list[BTN_L], 70, 35);

    s_btn_obj_list[BTN_R] = lv_imgbtn_create(lv_scr_act());
    lv_imgbtn_set_src(s_btn_obj_list[BTN_R], LV_IMGBTN_STATE_RELEASED, &cam_icon_flip, &Mid_fill, &iconR);
    lv_obj_align(s_btn_obj_list[BTN_R], LV_ALIGN_TOP_RIGHT, 0, -10);
    lv_obj_set_size(s_btn_obj_list[BTN_R], 70, 35);

    lv_obj_add_style(s_btn_obj_list[BTN_L], &btn_shoulder_press, LV_STATE_PRESSED);
    lv_obj_add_style(s_btn_obj_list[BTN_R], &btn_shoulder_press, LV_STATE_PRESSED);

    lv_obj_add_event_cb(s_btn_obj_list[BTN_L], callback, LV_EVENT_ALL, 0);
    lv_obj_add_event_cb(s_btn_obj_list[BTN_R], callback, LV_EVENT_ALL, 0);

    lv_obj_update_layout(s_btn_obj_list[BTN_L]);
    s_btn_corr_list[BTN_L][0] = (lv_point_t){(s_btn_obj_list[BTN_L]->coords.x1 + s_btn_obj_list[BTN_L]->coords.x2) / 2,
                                             (s_btn_obj_list[BTN_L]->coords.y1 + s_btn_obj_list[BTN_L]->coords.y2) / 2};

    s_drv_virbtn[BTN_L].type    = LV_INDEV_TYPE_BUTTON;
    s_drv_virbtn[BTN_L].read_cb = s_virbtn_callbacks[BTN_L];
    s_btn_indev_list[BTN_L]     = lv_indev_drv_register(&s_drv_virbtn[BTN_L]);
    lv_indev_set_button_points(s_btn_indev_list[BTN_L], s_btn_corr_list[BTN_L]);

    lv_obj_update_layout(s_btn_obj_list[BTN_R]);
    s_btn_corr_list[BTN_R][0] = (lv_point_t){(s_btn_obj_list[BTN_R]->coords.x1 + s_btn_obj_list[BTN_R]->coords.x2) / 2,
                                             (s_btn_obj_list[BTN_R]->coords.y1 + s_btn_obj_list[BTN_R]->coords.y2) / 2};

    s_drv_virbtn[BTN_R].type    = LV_INDEV_TYPE_BUTTON;
    s_drv_virbtn[BTN_R].read_cb = s_virbtn_callbacks[BTN_R];
    s_btn_indev_list[BTN_R]     = lv_indev_drv_register(&s_drv_virbtn[BTN_R]);
    lv_indev_set_button_points(s_btn_indev_list[BTN_R], s_btn_corr_list[BTN_R]);
}

void remove_LR()
{
    dealloc_res(&cam_icon);
    dealloc_res(&cam_icon_flip);
    dealloc_res(&iconL);
    dealloc_res(&iconR);
    dealloc_res(&Mid_fill);
}

void remove_virtual_btn(Button key)
{
    if (btm_btn_container != NULL) {
        lv_obj_del_async(btm_btn_container);
        btm_btn_container = NULL;
    }

    switch (key) {
        case BTN_A:
            s_btn_corr_list[BTN_A][0] = (lv_point_t){-1, -1};
            lv_indev_set_button_points(s_btn_indev_list[BTN_A], s_btn_corr_list[BTN_A]);
            break;

        case BTN_B:
            s_btn_corr_list[BTN_B][0] = (lv_point_t){-1, -1};
            lv_indev_set_button_points(s_btn_indev_list[BTN_B], s_btn_corr_list[BTN_B]);
            break;

        case BTN_X:
            s_btn_corr_list[BTN_X][0] = (lv_point_t){-1, -1};
            lv_indev_set_button_points(s_btn_indev_list[BTN_X], s_btn_corr_list[BTN_X]);
            break;

        case BTN_Y:
            s_btn_corr_list[BTN_Y][0] = (lv_point_t){-1, -1};
            lv_indev_set_button_points(s_btn_indev_list[BTN_Y], s_btn_corr_list[BTN_Y]);
            break;

        default:
            hang_err("Invalid virtual btn input");
            break;
    }
}

void button_init()
{
    button_style_init(&btn_btm);
    button_style_init(&btn_press);
    lv_style_set_bg_color(&btn_btm, lv_palette_lighten(LV_PALETTE_GREY, 2));
    lv_style_set_bg_color(&btn_press, lv_palette_darken(LV_PALETTE_GREY, 2));

    lv_style_init(&btn_shoulder_press);
    lv_style_set_img_recolor_opa(&btn_shoulder_press, LV_OPA_30);
    lv_style_set_img_recolor(&btn_shoulder_press, lv_color_black());
    lv_style_set_translate_y(&btn_shoulder_press, 2);

    virtual_button_driver_init();
}
