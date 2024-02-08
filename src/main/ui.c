#include "sections.h"

#define CANVAS_WIDTH 80
#define CANVAS_HEIGHT 80

extern jmp_buf exitJmp;

extern lv_group_t *g;
extern lv_obj_t *box_list; 
extern Detector det;
extern BoxVec objects; 
extern bool detecting;
extern void *cam_buf;

extern lv_obj_t *btn_A;
extern lv_obj_t *btn_B;
extern lv_obj_t *btn_X;
extern lv_obj_t *btn_Y;
extern lv_obj_t *btn_L;
extern lv_obj_t *btn_R;
extern lv_obj_t *btm_btn_container;

extern lv_indev_t *indev_A;
extern lv_indev_t *indev_B;
extern lv_indev_t *indev_X;
extern lv_indev_t *indev_Y;
extern lv_indev_t *indev_L; 
extern lv_indev_t *indev_R; 
extern lv_point_t point_array_A[2];
extern lv_point_t point_array_B[2];
extern lv_point_t point_array_X[2];
extern lv_point_t point_array_Y[2];
extern lv_point_t point_array_L[2];
extern lv_point_t point_array_R[2]; //Encoder 

extern lv_indev_drv_t drv_virbtn[4];

extern lv_style_t btn_btm;
extern lv_style_t btn_press;
extern lv_style_t btn_shoulder_press;

extern lv_obj_t *tab_bg;
extern lv_obj_t *tab_view;// pop up tab view

void button_style_init(lv_style_t *btn)
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

void pop_up_tabview_cb(lv_event_t *e)
{

}

void close_tabview_cb(lv_event_t *e)
{
    
}

void quit_detect_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        if(detecting)
        {
            lv_obj_clean(btm_btn_container);
            lv_indev_enable(indev_B, false);
            lv_indev_enable(indev_A, false);
            add_btm_btn(btm_btn_container, KEY_A, detect_cb, lv_pct(100), " Detect");
            lv_obj_del(box_list);
            detecting = false;        
        }
    }
}


void object_display_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *item = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        unsigned char *pixels = malloc(sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
        writeCamToPixels(pixels, cam_buf, 0, 0, WIDTH_TOP, HEIGHT_TOP);

        int idx = lv_obj_get_child_id(item) - 2;
        if (idx < 0)
        {
            draw_boxxes(pixels, WIDTH_TOP, HEIGHT_TOP, &objects);
        }
        else
        {
            BoxInfo obj = BoxVec_getItem(idx, &objects);//objects.getItem(idx);

            BoxVec box_Vec_temp;
            create_box_vector(&box_Vec_temp, 1);
            BoxVec_push_back(obj, &box_Vec_temp);
            draw_boxxes(pixels, WIDTH_TOP, HEIGHT_TOP, &box_Vec_temp);
            BoxVec_free(&box_Vec_temp);
        }


        writePixelsToFrameBuffer(
            gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 
            pixels, 
            0, 
            0, 
            WIDTH_TOP, 
            HEIGHT_TOP
        );

        gfxFlushBuffers();
        gfxScreenSwapBuffers(GFX_TOP, true);
        gspWaitForVBlank();

        free(pixels);    
    }
}

void detect_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        if(detecting)
        {
            lv_obj_del(box_list); 
        }
        detecting = true;
        lv_obj_clean(btm_btn_container);
        lv_indev_enable(indev_A, false);
        BoxVec_free(&objects);
        pause_cam_capture(cam_buf);

        // Inference 
        unsigned char *pixels = malloc(sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
        writeCamToPixels(pixels, cam_buf, 0, 0, WIDTH_TOP, HEIGHT_TOP);
        objects = det.detect(pixels, WIDTH_TOP, HEIGHT_TOP, &det);

        // Print inference outputs
        draw_boxxes(pixels, WIDTH_TOP, HEIGHT_TOP, &objects);
        writePixelsToFrameBuffer(
            gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 
            pixels, 
            0, 
            0, 
            WIDTH_TOP, 
            HEIGHT_TOP);

        gfxFlushBuffers();
        gfxScreenSwapBuffers(GFX_TOP, true);
        gspWaitForVBlank();

        free(pixels);

        // Create box list and botton list for detecting page
        box_list = create_box_list();

        add_btm_btn(btm_btn_container, KEY_B, quit_detect_cb, lv_pct(50), " Continue");
        add_btm_btn(btm_btn_container, KEY_A, NULL, lv_pct(50), " Select");

    }
}

lv_obj_t *create_box_list()
{

    lv_obj_t *boxxes = lv_list_create(lv_scr_act());
    lv_obj_set_size(boxxes, WIDTH_BTM, 160);
    lv_obj_align(boxxes, LV_ALIGN_TOP_MID, 0, 40);

    lv_obj_t *btn;
    lv_list_add_text(boxxes, "Press B to continue");
    char detected[40];
    sprintf(detected, "Founded %ld items", objects.num_item);
    btn = lv_list_add_btn(boxxes, LV_SYMBOL_FILE, detected);
    lv_obj_add_event_cb(btn, object_display_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(g, btn);

    for(size_t i=0; i < objects.num_item; i++)
    {
        BoxInfo obj = objects.getItem(i, &objects);
        char list_item[40];
        int label = obj.label;
        int x1 = obj.x1;
        int x2 = obj.x2;
        int y1 = obj.y1;
        int y2 = obj.y2;
        float prob = obj.prob * 100;

        sprintf(list_item, "%15s %03.1f [%3d,%3d,%3d,%3d]", class_names[label], prob, x1, x2, y1, y2);
        btn = lv_list_add_btn(boxxes, LV_SYMBOL_GPS, list_item);

        lv_obj_add_event_cb(btn, object_display_cb, LV_EVENT_ALL, NULL);
        lv_group_add_obj(g, btn);
    }
    return boxxes;
}

void model_list_hanlder(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    Detector *det = (Detector *) lv_event_get_user_data(e);
    if(code == LV_EVENT_VALUE_CHANGED) 
    {
        destroy_detector(det);
        uint16_t model_idx = lv_dropdown_get_selected(obj);
        switch(model_idx)
        {
            case 0:
            *det = create_nanodet(320, "romfs:nanodet-plus-m_416_int8.param", "romfs:nanodet-plus-m_416_int8.bin");
            break;

            case 1:
            *det = create_fastestdet(352, "romfs:FastestDet.param", "romfs:FastestDet.bin");
            break;

            default:
            break;
        } ;
    }
}


lv_obj_t *create_model_list(Detector *det)
{
    lv_obj_t *models = lv_dropdown_create(lv_scr_act());
    lv_dropdown_set_options(models, "NanoDet-plus\n"
                            "FastestDet");

    lv_obj_align(models, LV_ALIGN_TOP_MID, 0, -10);
    lv_obj_set_width(models, 150);
    lv_obj_add_event_cb(models, model_list_hanlder, LV_EVENT_ALL, det);
}

void create_LR()
{
    LV_IMG_DECLARE(cam_icon);
    LV_IMG_DECLARE(cam_icon_flip);
    LV_IMG_DECLARE(iconL);
    LV_IMG_DECLARE(iconR);
    LV_IMG_DECLARE(Mid_fill);

    lv_obj_t *btn_L = lv_imgbtn_create(lv_scr_act());
    lv_imgbtn_set_src(btn_L, LV_IMGBTN_STATE_RELEASED, &iconL, &Mid_fill, &cam_icon);
    lv_obj_align(btn_L, LV_ALIGN_TOP_LEFT, 0, -10);
    lv_obj_set_size(btn_L, 70, 35);

    lv_obj_t *btn_R = lv_imgbtn_create(lv_scr_act());
    lv_imgbtn_set_src(btn_R, LV_IMGBTN_STATE_RELEASED, &cam_icon_flip, &Mid_fill, &iconR);
    lv_obj_align(btn_R, LV_ALIGN_TOP_RIGHT, 0, -10);
    lv_obj_set_size(btn_R, 70, 35);

    lv_obj_add_style(btn_L, &btn_shoulder_press, LV_STATE_PRESSED);
    lv_obj_add_style(btn_R, &btn_shoulder_press, LV_STATE_PRESSED);

    lv_obj_add_event_cb(btn_L, detect_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btn_R, detect_cb, LV_EVENT_ALL, NULL); /*Display the press stage of two button*/

    lv_obj_update_layout(btn_L);
    point_array_L[0] = (lv_point_t) {-1, -1};
    point_array_L[1] = (lv_point_t) {(btn_L->coords.x1 + btn_L->coords.x2) / 2, (btn_L->coords.y1 + btn_L->coords.y2) / 2};

    void (*functions[2])() = {virtual_L_cb, virtual_R_cb};
    static lv_indev_drv_t drv_list_LR[2];
    
    drv_list_LR[0].type = LV_INDEV_TYPE_BUTTON;
    drv_list_LR[0].read_cb = functions[0];
    lv_indev_t *indev_L = lv_indev_drv_register(&drv_list_LR[0]);
    lv_indev_set_button_points(indev_L, point_array_L);

    lv_obj_update_layout(btn_R);
    point_array_R[0] = (lv_point_t) {-1, -1};
    point_array_R[1] = (lv_point_t) {(btn_R->coords.x1 + btn_R->coords.x2) / 2, (btn_R->coords.y1 + btn_R->coords.y2) / 2};

    drv_list_LR[1].type = LV_INDEV_TYPE_BUTTON;
    drv_list_LR[1].read_cb = functions[1];
    lv_indev_t *indev_R = lv_indev_drv_register(&drv_list_LR[1]);
    lv_indev_set_button_points(indev_R, point_array_R);

}

void create_btm_btn_container()
{
    lv_obj_clean(btm_btn_container);
    lv_obj_remove_style_all(btm_btn_container);
    lv_obj_set_size(btm_btn_container, lv_pct(100), 30);

    lv_obj_set_style_radius(btm_btn_container, 0, NULL);
    lv_obj_set_style_pad_all(btm_btn_container, 0, NULL);

    lv_obj_align(btm_btn_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(btm_btn_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btm_btn_container,  LV_FLEX_ALIGN_START,  LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
}

void add_btm_btn(lv_obj_t *cont ,u32 key, void *callback, lv_coord_t width, const char *label)
{
    lv_obj_t *btn_ptr;
    lv_obj_t *icon;
    lv_obj_t *label_btn;
    lv_obj_t *label_icon;

    char icon_label[2] = {'\0'};

    switch(key)
    {
        case KEY_A:

            btn_A = lv_btn_create(cont);
            btn_ptr = btn_A;
            icon_label[0] = 'A';
            break;

        case KEY_B:

            btn_B = lv_btn_create(cont);
            btn_ptr = btn_B;
            icon_label[0] = 'B';
            break;

        case KEY_X:

            btn_X = lv_btn_create(cont);
            btn_ptr = btn_X;
            icon_label[0] = 'X';
            break;

        case KEY_Y:

            btn_Y = lv_btn_create(cont);
            btn_ptr = btn_Y;
            icon_label[0] = 'Y';
            break;

        default:
            hang_err("Fail to create button");
            break;
    }

    // Style
    lv_obj_remove_style_all(btn_ptr);
    lv_obj_set_size(btn_ptr, width, 30);
    lv_obj_add_style(btn_ptr, &btn_btm, NULL);
    lv_obj_add_style(btn_ptr, &btn_press, LV_STATE_PRESSED);

    //Icon
    icon = lv_obj_create(btn_ptr);
    label_icon = lv_label_create(icon);
    lv_label_set_text(label_icon, icon_label);
    lv_obj_set_style_text_color(label_icon, lv_color_hex(0x353535), NULL);
    lv_obj_set_style_text_font(label_icon, &lv_font_montserrat_12, NULL);
    lv_obj_center(label_icon);

    lv_obj_set_size(icon, 19, 19);
    lv_obj_set_style_radius(icon, LV_RADIUS_CIRCLE, NULL);
    lv_obj_set_style_clip_corner(icon, true, NULL);
    lv_obj_set_scrollbar_mode(icon, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(icon, LV_OPA_TRANSP, NULL);
    lv_obj_set_style_border_width(icon, 1, NULL);
    lv_obj_set_style_border_color(icon, lv_color_hex(0x353535), NULL);
    lv_obj_set_style_pad_all(icon, 0, NULL);

    //Label
    label_btn = lv_label_create(btn_ptr);
    lv_label_set_text(label_btn, label);
    lv_obj_set_style_text_font(label_btn, &lv_font_montserrat_16, NULL);

    // Layout: Flex
    lv_obj_set_flex_flow(btn_ptr, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_ptr,  LV_FLEX_ALIGN_CENTER,  LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Callback
    lv_obj_add_event_cb(btn_ptr, callback, LV_EVENT_ALL, NULL);

    lv_obj_update_layout(btn_ptr);
    switch(key)
    {
        case KEY_A:
            drv_virbtn[0].type = LV_INDEV_TYPE_BUTTON;
            drv_virbtn[0].read_cb = virtual_A_cb;
            indev_A = lv_indev_drv_register(&drv_virbtn[0]);
            point_array_A[0] = (lv_point_t){-1, -1};
            point_array_A[1] = (lv_point_t) {(btn_ptr->coords.x1 + btn_ptr->coords.x2) / 2, (btn_ptr->coords.y1 + btn_ptr->coords.y2) / 2};
            lv_indev_set_button_points(indev_A, point_array_A);

            break;

        case KEY_B:
            drv_virbtn[1].type = LV_INDEV_TYPE_BUTTON;
            drv_virbtn[1].read_cb = virtual_B_cb;
            indev_B = lv_indev_drv_register(&drv_virbtn[1]);
            point_array_B[0] = (lv_point_t){-1, -1};
            point_array_B[1] = (lv_point_t) {(btn_ptr->coords.x1 + btn_ptr->coords.x2) / 2, (btn_ptr->coords.y1 + btn_ptr->coords.y2) / 2};
            lv_indev_set_button_points(indev_B, point_array_B);

            break;

        case KEY_X:
            drv_virbtn[2].type = LV_INDEV_TYPE_BUTTON;
            drv_virbtn[2].read_cb = virtual_X_cb;
            indev_X = lv_indev_drv_register(&drv_virbtn[2]);
            point_array_X[0] = (lv_point_t){-1, -1};
            point_array_X[1] = (lv_point_t) {(btn_ptr->coords.x1 + btn_ptr->coords.x2) / 2, (btn_ptr->coords.y1 + btn_ptr->coords.y2) / 2};
            lv_indev_set_button_points(indev_X, point_array_X);

            break;

        case KEY_Y:
            drv_virbtn[3].type = LV_INDEV_TYPE_BUTTON;
            drv_virbtn[3].read_cb = virtual_Y_cb;
            indev_Y = lv_indev_drv_register(&drv_virbtn[3]);
            point_array_Y[0] = (lv_point_t){-1, -1};
            point_array_Y[1] = (lv_point_t) {(btn_ptr->coords.x1 + btn_ptr->coords.x2) / 2, (btn_ptr->coords.y1 + btn_ptr->coords.y2) / 2};
            lv_indev_set_button_points(indev_Y, point_array_Y);
            break;

        default:
            hang_err("Fail to register virtual button");
            break;
    }
}

void tab_add_icon_description(lv_obj_t *parent, lv_img_dsc_t *img, const char *description)
{

}

void tab_Ac_Li()
{

}