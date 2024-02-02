#include "sections.h"

#define CANVAS_WIDTH 80
#define CANVAS_HEIGHT 80

void list_item_add_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_group_list_t *glt = lv_event_get_user_data(e);

    lv_obj_t *btn;  
    btn = lv_list_add_btn(glt->list,  LV_SYMBOL_LIST, "x1:    x2:");
    lv_obj_add_event_cb(btn, list_item_delete_cb, LV_EVENT_PRESSED, NULL);
    lv_group_add_obj(glt->g, btn);
}

void list_item_delete_cb(lv_event_t *e)
{
    lv_obj_t *item = lv_event_get_target(e);
    lv_obj_del_async(item);
}

void display_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    switch(code) {
        case LV_EVENT_PRESSED:
            lv_label_set_text(label, "PRESSED");
            break;
        case LV_EVENT_CLICKED:
            lv_label_set_text(label, "CLICKED");
            break;
        case LV_EVENT_LONG_PRESSED:
            lv_label_set_text(label, "LONG_PRE");
            break;
        case LV_EVENT_LONG_PRESSED_REPEAT:
            lv_label_set_text(label, "LONG_PREAT");
            break;
        default:
            break;
    }
}

lv_obj_t *create_box_list(lv_group_t *g)
{

    lv_obj_t *boxxes = lv_list_create(lv_scr_act());
    lv_obj_set_size(boxxes, WIDTH_BTM, 160);
    lv_obj_align(boxxes, LV_ALIGN_TOP_MID, 0, 40);

    lv_group_list_t *glt = (lv_group_list_t *) malloc(sizeof(lv_group_list_t));
    glt->g = g;
    glt->list = boxxes;

    lv_obj_t *btn;
    lv_list_add_text(boxxes, "Boxxes");

    btn = lv_list_add_btn(boxxes, LV_SYMBOL_FILE, "New");
    lv_obj_add_event_cb(btn, list_item_add_cb, LV_EVENT_PRESSED, glt);
    lv_group_add_obj(g, btn);
    lv_group_add_obj(g, lv_list_add_btn(boxxes, LV_SYMBOL_GPS, "Navigate"));

    free(glt);
    return boxxes;
}

ui_LR_t create_shoulder_button()
{
    /** Create L, R button that aligned with top left and top right of screen
     * Width: 90,  Height: 30
     * */

    lv_obj_t *btn_L = lv_btn_create(lv_scr_act());    /*Add a button the current screen*/                            /*Set its position*/
    lv_obj_set_size(btn_L, 90, 30);
    lv_obj_align(btn_L, LV_ALIGN_TOP_LEFT, -10, -5);
    lv_obj_t *label_L = lv_label_create(btn_L);          /*Add a label to the button*/
    lv_label_set_text(label_L, "L  " LV_SYMBOL_VIDEO);                     /*Set the labels text*/
    lv_obj_align(label_L, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_t *btn_R = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/                            /*Set its position*/
    lv_obj_set_size(btn_R, 90, 30);
    lv_obj_align(btn_R, LV_ALIGN_TOP_RIGHT, 10, -5);
    lv_obj_t *label_R = lv_label_create(btn_R);          /*Add a label to the button*/
    lv_label_set_text(label_R, LV_SYMBOL_IMAGE "  R");                     /*Set the labels text*/
    lv_obj_align(label_R, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_add_event_cb(btn_L, display_event_cb, LV_EVENT_ALL, label_L);
    lv_obj_add_event_cb(btn_R, display_event_cb, LV_EVENT_ALL, label_R); /*Display the press stage of two button*/

    lv_obj_update_layout(btn_L);
    lv_point_t *points_array_L = (lv_point_t *) malloc(sizeof(lv_point_t) * 2);
    points_array_L[0] = (lv_point_t) {-1, -1};
    points_array_L[1] = (lv_point_t) {(btn_L->coords.x1 + btn_L->coords.x2) / 2, (btn_L->coords.y1 + btn_L->coords.y2) / 2};

    void (*functions[2])() = {virtual_L_cb, virtual_R_cb};
    static lv_indev_drv_t drv_list_LR[2];
    
    drv_list_LR[0].type = LV_INDEV_TYPE_BUTTON;
    drv_list_LR[0].read_cb = functions[0];
    lv_indev_t *l_indev = lv_indev_drv_register(&drv_list_LR[0]);
    lv_indev_set_button_points(l_indev, points_array_L);

    lv_obj_update_layout(btn_R);
    lv_point_t *points_array_R = (lv_point_t *) malloc(sizeof(lv_point_t) * 2);
    points_array_R[0] = (lv_point_t) {-1, -1};
    points_array_R[1] = (lv_point_t) {(btn_R->coords.x1 + btn_R->coords.x2) / 2, (btn_R->coords.y1 + btn_R->coords.y2) / 2};

    drv_list_LR[1].type = LV_INDEV_TYPE_BUTTON;
    drv_list_LR[1].read_cb = functions[1];
    lv_indev_t *r_indev = lv_indev_drv_register(&drv_list_LR[1]);
    lv_indev_set_button_points(r_indev, points_array_R);


    ui_LR_t output;
    output.L = btn_L;
    output.R = btn_R;
    output.point_array_L = points_array_L;
    output.point_array_R = points_array_R;
    return output;
}

lv_obj_t *put_text_example(const char *string)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 5);

    /*Make a gradient*/
    lv_style_set_width(&style, 150);
    lv_style_set_height(&style, LV_SIZE_CONTENT); /* This enable a flexible width and height that change 
                                                   * during inputing*/
    lv_style_set_pad_ver(&style, 20);
    lv_style_set_pad_left(&style, 5);

    /*Create an object with the new style*/
    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj, &style, 0);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 90);

    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, string);

    return label;
}

void model_list_hanlder(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) 
    {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Option: %s", buf);
    }
}


lv_obj_t *create_model_list()
{
    lv_obj_t *models = lv_dropdown_create(lv_scr_act());
    lv_dropdown_set_options(models, "Nanodet\n"
                            "FastestDet\n"
                            "Nuts");

    lv_obj_align(models, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_event_cb(models, model_list_hanlder, LV_EVENT_ALL, NULL);
}

ui_LR_t create_bottom_btn()
{
    ui_LR_t btn_BA;

    lv_obj_t *btns[2] = {btn_BA.L, btn_BA.R};
    lv_point_t *pts_arrays[2] = {btn_BA.point_array_L, btn_BA.point_array_R};
    
    const *labels[] = {"B quit", "A continue"};
    void (*functions[2])() = {virtual_B_cb, virtual_A_cb};
    static lv_indev_drv_t drv_list_BA[2];


    int align[2] = {LV_ALIGN_BOTTOM_LEFT, LV_ALIGN_BOTTOM_RIGHT};
    for(int i=0; i < 2; i++)
    {
        btns[i] = lv_btn_create(lv_scr_act());
        lv_obj_set_size(btns[i], lv_pct(50), 30);

        // Positions
        lv_obj_t *label = lv_label_create(btns[i]);
        lv_label_set_text(label, labels[i]);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_obj_align(btns[i], align[i], 0, 0);

        //Styles
        // lv_obj_set_style_border_opa(btns[i], LV_OPA_TRANSP, NULL);
        lv_obj_set_style_border_width(btns[i], 1, NULL);
        lv_obj_set_style_border_color(btns[i], lv_color_hex(0x999999), NULL);
        lv_obj_set_style_radius(btns[i], 0, NULL);
        lv_obj_set_style_pad_all(btns[i], 0, NULL);
        lv_obj_set_style_bg_color(btns[i], lv_palette_lighten(LV_PALETTE_GREY, 2), NULL);
        lv_obj_set_style_bg_grad_color(btns[i], lv_palette_main(LV_PALETTE_GREY), NULL);
        lv_obj_set_style_bg_grad_dir(btns[i], LV_GRAD_DIR_VER, NULL);
        // lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);

        // Assign click button activities
        lv_obj_update_layout(btns[i]);
        drv_list_BA[i].type = LV_INDEV_TYPE_BUTTON;
        drv_list_BA[i].read_cb = functions[i];
        lv_indev_t *BA_indev = lv_indev_drv_register(&drv_list_BA[i]);
        pts_arrays[i] = (lv_point_t *) malloc(sizeof(lv_point_t) * 2);
        pts_arrays[i][0] = (lv_point_t){-1, -1};
        pts_arrays[i][1] = (lv_point_t) {(btns[i]->coords.x1 + btns[i]->coords.x2) / 2, (btns[i]->coords.y1 + btns[i]->coords.y2) / 2};


        lv_indev_set_button_points(BA_indev, pts_arrays[i]);
    }
}
