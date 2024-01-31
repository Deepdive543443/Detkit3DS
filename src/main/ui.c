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

void color_flip_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);
    lv_obj_t *target =lv_event_get_current_target(e);
    // lv_obj_t *label = lv_obj_get_child(target, 0);

    switch(code) 
    {
        case LV_EVENT_PRESSED:
            lv_obj_set_style_text_color(label, lv_color_hex(0x2986cc), NULL);
            // lv_label_set_text(label, "PRESSED");
            // lv_obj_set_style_bg_color(target, lv_color_hex(0xffffff), NULL);
            break;
        case LV_EVENT_CLICKED:
            lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), NULL);
            break;
        case LV_EVENT_LONG_PRESSED:
            lv_obj_set_style_text_color(label, lv_color_hex(0x2986cc), NULL);
            // lv_obj_set_style_bg_color(target, lv_color_hex(0xffffff), NULL);
            break;
        case LV_EVENT_LONG_PRESSED_REPEAT:
            lv_obj_set_style_text_color(label, lv_color_hex(0x2986cc), NULL);
            // lv_obj_set_style_bg_color(target, lv_color_hex(0xffffff), NULL);
            break;
        default:
            lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), NULL);
            break;
    }
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
    lv_obj_set_size(boxxes, 150, 160);
    lv_obj_align(boxxes, LV_ALIGN_TOP_MID, 0, 0);

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

lv_obj_t *create_joystick()
{
    // Create the container 
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, CANVAS_WIDTH, CANVAS_HEIGHT);
    lv_obj_align(cont, LV_ALIGN_LEFT_MID, 10, -40);
    lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, NULL);
    lv_obj_set_style_clip_corner(cont, true, NULL);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    // Create the Joy stick
    lv_obj_t *js = lv_obj_create(cont);
    lv_obj_set_size(js, 50, 50);
    lv_obj_center(js);
    lv_obj_set_style_radius(js, LV_RADIUS_CIRCLE, NULL);
    lv_obj_set_style_clip_corner(cont, true, NULL);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(js, lv_color_hex(0x777777), NULL);
    lv_obj_set_style_border_width(js, 5, NULL);
    lv_obj_set_style_border_color(js, lv_color_hex(0xbcbcbc), NULL);

    return js;
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

ui_ABXY_t create_ABXY()
{
    // Create container
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(cont, 130, 130);
    lv_obj_align(cont, LV_ALIGN_RIGHT_MID, 0, -20);

    lv_obj_set_style_border_opa(cont, LV_OPA_TRANSP, NULL);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, NULL);;

    // Create button
    lv_obj_t *btn_ptr[4];
    lv_point_t *points_array_ptr[4];
    const char *btn_labels[] = {"A", "B", "Y", "X"};
    lv_color_t colors[] = 
    {
        lv_color_hex(0xe06666), lv_color_hex(0xffd966),
        lv_color_hex(0x8fce00), lv_color_hex(0x6fa8dc)
    };
    u32 offset = 35;
    const u32 btn_position[] = {offset, 0, 0, offset, -offset, 0, 0, -offset};
    static lv_indev_drv_t drv_list_ABXY[4];
    void (*press_callbacks[4])() = {virtual_A_cb, virtual_B_cb, virtual_Y_cb, virtual_X_cb};

    
    for (int i=0; i < 4;i++)
    {
        btn_ptr[i] = lv_btn_create(cont);
        lv_obj_t *label = lv_label_create(btn_ptr[i]);
        lv_label_set_text(label, btn_labels[i]);
        lv_label_set_recolor(label, true);
        lv_obj_center(label);

        // Shape circle
        lv_obj_set_size(btn_ptr[i], 30, 30);
        lv_obj_set_style_radius(btn_ptr[i], LV_RADIUS_CIRCLE, NULL);
        lv_obj_set_style_clip_corner(btn_ptr[i], true, NULL);
        lv_obj_set_scrollbar_mode(btn_ptr[i], LV_SCROLLBAR_MODE_OFF);

        // Position
        lv_obj_center(btn_ptr[i]);
        lv_obj_set_pos(btn_ptr[i], btn_position[i * 2], btn_position[i * 2 + 1]);

        // Color
        lv_obj_set_style_bg_color(btn_ptr[i], colors[i], NULL);
        lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), NULL);
        lv_label_set_recolor(label, true);

        // Callback
        lv_obj_update_layout(btn_ptr[i]);
        lv_obj_add_event_cb(btn_ptr[i], color_flip_cb, LV_EVENT_ALL, label);
        // lv_obj_add_event_cb(btn_ptr[i], display_event_cb, LV_EVENT_ALL, label); // Color flip callback

        points_array_ptr[i] = (lv_point_t *) malloc(sizeof(lv_point_t) * 2);
        points_array_ptr[i][0] = (lv_point_t) {-1, -1};
        points_array_ptr[i][1] = (lv_point_t)
        {
            (btn_ptr[i]->coords.x1 + btn_ptr[i]->coords.x2) / 2,
            (btn_ptr[i]->coords.y1 + btn_ptr[i]->coords.y2) / 2
        };

        drv_list_ABXY[i].type = LV_INDEV_TYPE_BUTTON;
        drv_list_ABXY[i].read_cb = press_callbacks[i];
        lv_indev_t *indev = lv_indev_drv_register(&drv_list_ABXY[i]);
        lv_indev_set_button_points(indev, points_array_ptr[i]);
    }
    

    ui_ABXY_t output;
    output.container = cont;
    output.A = btn_ptr[0];
    output.B = btn_ptr[1];
    output.X = btn_ptr[2];
    output.Y = btn_ptr[3];
    output.point_array_A = points_array_ptr[0];
    output.point_array_B = points_array_ptr[1];
    output.point_array_X = points_array_ptr[2];
    output.point_array_Y = points_array_ptr[3];
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

void update_joy_stick(lv_obj_t *js, circlePosition *js_read)
{
    lv_obj_set_pos(
        js,
        js_read->dx * 0.07792f,
        -js_read->dy * 0.07792f
    );
}
