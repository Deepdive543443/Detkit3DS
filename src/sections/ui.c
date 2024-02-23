#include "sections.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static lv_group_t *g;
static lv_obj_t *box_list;  // LVGL Objects

static lv_obj_t *btn_A;
static lv_obj_t *btn_B;
static lv_obj_t *btn_X;
static lv_obj_t *btn_Y;
static lv_obj_t *btn_L;
static lv_obj_t *btn_R;
static lv_obj_t *btm_btn_container;

static lv_indev_t *indev_A;
static lv_indev_t *indev_B;
static lv_indev_t *indev_X;
static lv_indev_t *indev_Y; 
static lv_indev_t *indev_L; 
static lv_indev_t *indev_R; 
static lv_point_t point_array_A[1];
static lv_point_t point_array_B[1];
static lv_point_t point_array_X[1];
static lv_point_t point_array_Y[1];
static lv_point_t point_array_L[1];
static lv_point_t point_array_R[1];

static lv_indev_drv_t drv_virbtn[4]; //Encoder A, B, X, Y

static lv_style_t btn_btm;
static lv_style_t btn_press;
static lv_style_t btn_shoulder_press;
static lv_style_t btn_tabview;


static lv_obj_t *tab_bg;
static lv_obj_t *tab_view;// pop up tab view

static Thread tick_thread; //Thread

static lv_img_dsc_t cam_icon;
static lv_img_dsc_t cam_icon_flip;
static lv_img_dsc_t iconL;
static lv_img_dsc_t iconR;
static lv_img_dsc_t Mid_fill;
LV_IMG_DECLARE(_ncnn);
LV_IMG_DECLARE(logo_lvgl);
LV_IMG_DECLARE(devkitpro);
LV_IMG_DECLARE(ftpd_icon);
LV_IMG_DECLARE(citra_logo);

lv_img_dsc_t res[8];

// Glob
jmp_buf exitJmp; //Debug

Detector det;
BoxVec objects; // Containers 
bool detecting; // Stage marker
void *cam_buf;
bool thread_ticking;

static int add_res_depth16(const char *path, lv_img_dsc_t *res_buffer)
{
    int width, height, n;
    uint8_t *pixels = (uint8_t *) stbi_load(path, &width, &height, &n, 0);
    if (pixels == NULL)
    {
        return 0;
    }

    uint8_t *lvgl_datas = malloc(sizeof(uint8_t) * width * height * 3);
    if (lvgl_datas == NULL)
    {
        return 0;
    }

	uint8_t *pixels_ptr = pixels;
	uint8_t *lvgl_data_ptr = lvgl_datas;

    for(int h=0; h < height; h++)
	{
		for(int w=0; w < width; w++)
		{
			uint8_t r = pixels_ptr[0];
			uint8_t g = pixels_ptr[1];
			uint8_t b = pixels_ptr[2];
			uint8_t a = pixels_ptr[3];

			lvgl_data_ptr[0] = ((g & 0x1c) << 3) | ((b & 0xF8) >> 3); // Lower 3 bit of green, 5 bit of Blue
			lvgl_data_ptr[1] = (r & 0xF8) | ((g & 0xE0) >> 5); // Red 5 bit, Green 3 higher bit
			lvgl_data_ptr[2] = a; // Alpha channels

			pixels_ptr+=4;
			lvgl_data_ptr+=3;
		}
	}

	// lv_img_dsc_t loaded_img = {
	// 	.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
	// 	.header.always_zero = 0,
	// 	.header.reserved = 0,
	// 	.header.w = width,
	// 	.header.h = height,
	// 	.data_size = width * height * n,
	// 	.data = lvgl_datas,
	// };

    // *res_buffer = loaded_img;

	*res_buffer = (lv_img_dsc_t){
		.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
		.header.always_zero = 0,
		.header.reserved = 0,
		.header.w = width,
		.header.h = height,
		.data_size = width * height * n,
		.data = lvgl_datas,
	};
    
    return 1;
}

static void lvgl_tick_thread()
{
    while(thread_ticking)
    {
        svcSleepThread((u64) TICK_US * DURATION_MICRO_SEC);
        lv_tick_inc(TICK_MS);
    }
}


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
    drv_virbtn[0].type = LV_INDEV_TYPE_BUTTON;
    drv_virbtn[0].read_cb = virtual_A_cb;
    indev_A = lv_indev_drv_register(&drv_virbtn[0]);
    point_array_A[0] = (lv_point_t){-1, -1};
    lv_indev_set_button_points(indev_A, point_array_A);

    drv_virbtn[1].type = LV_INDEV_TYPE_BUTTON;
    drv_virbtn[1].read_cb = virtual_B_cb;
    indev_B = lv_indev_drv_register(&drv_virbtn[1]);
    point_array_B[0] = (lv_point_t){-1, -1};
    lv_indev_set_button_points(indev_B, point_array_B);

    drv_virbtn[2].type = LV_INDEV_TYPE_BUTTON;
    drv_virbtn[2].read_cb = virtual_X_cb;
    indev_X = lv_indev_drv_register(&drv_virbtn[2]);
    point_array_X[0] = (lv_point_t){-1, -1};
    lv_indev_set_button_points(indev_X, point_array_Y);

    drv_virbtn[3].type = LV_INDEV_TYPE_BUTTON;
    drv_virbtn[3].read_cb = virtual_Y_cb;
    indev_Y = lv_indev_drv_register(&drv_virbtn[3]);
    point_array_Y[0] = (lv_point_t){-1, -1};
    lv_indev_set_button_points(indev_Y, point_array_Y);
}

static lv_obj_t *create_box_list()
{

    lv_obj_t *boxxes = lv_list_create(lv_scr_act());
    lv_obj_set_size(boxxes, WIDTH_BTM, 160);
    lv_obj_align(boxxes, LV_ALIGN_TOP_MID, 0, 40);

    lv_obj_t *btn;
    lv_list_add_text(boxxes, "Press B to continue");
    char detected[40];
    sprintf(detected, "Found %d items", objects.num_item);
    btn = lv_list_add_btn(boxxes, LV_SYMBOL_FILE, detected);
    lv_obj_add_event_cb(btn, object_display_cb, LV_EVENT_ALL, 0);
    lv_group_add_obj(g, btn);

    for(size_t i=0; i < objects.num_item; i++)
    {
        BoxInfo obj = BoxVec_getItem(i, &objects);
        char list_item[40];
        int label = obj.label;
        int x1 = obj.x1;
        int x2 = obj.x2;
        int y1 = obj.y1;
        int y2 = obj.y2;
        float prob = obj.prob * 100;

        sprintf(list_item, "%15s %03.1f [%3d,%3d,%3d,%3d]", class_names[label], prob, x1, x2, y1, y2);
        btn = lv_list_add_btn(boxxes, LV_SYMBOL_GPS, list_item);

        lv_obj_add_event_cb(btn, object_display_cb, LV_EVENT_ALL, 0);
        lv_group_add_obj(g, btn);
    }
    return boxxes;
}

static void model_list_hanlder(lv_event_t *e)
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


static void create_model_list(Detector *det)
{
    lv_obj_t *models = lv_dropdown_create(lv_scr_act());
    lv_dropdown_set_options(models, "NanoDet-plus\n"
                            "FastestDet");

    lv_obj_align(models, LV_ALIGN_TOP_MID, 0, -10);
    lv_obj_set_width(models, 150);
    lv_obj_add_event_cb(models, model_list_hanlder, LV_EVENT_ALL, det);
}

static void create_LR()
{
    btn_L = lv_imgbtn_create(lv_scr_act());
    lv_imgbtn_set_src(btn_L, LV_IMGBTN_STATE_RELEASED, &iconL, &Mid_fill, &cam_icon);
    lv_obj_align(btn_L, LV_ALIGN_TOP_LEFT, 0, -10);
    lv_obj_set_size(btn_L, 70, 35);

    btn_R = lv_imgbtn_create(lv_scr_act());
    lv_imgbtn_set_src(btn_R, LV_IMGBTN_STATE_RELEASED, &cam_icon_flip, &Mid_fill, &iconR);
    lv_obj_align(btn_R, LV_ALIGN_TOP_RIGHT, 0, -10);
    lv_obj_set_size(btn_R, 70, 35);

    lv_obj_add_style(btn_L, &btn_shoulder_press, LV_STATE_PRESSED);
    lv_obj_add_style(btn_R, &btn_shoulder_press, LV_STATE_PRESSED);

    lv_obj_add_event_cb(btn_L, detect_cb, LV_EVENT_ALL, 0);
    lv_obj_add_event_cb(btn_R, detect_cb, LV_EVENT_ALL, 0); /*Display the press stage of two button*/

    lv_obj_update_layout(btn_L);
    point_array_L[0] = (lv_point_t) {(btn_L->coords.x1 + btn_L->coords.x2) / 2, (btn_L->coords.y1 + btn_L->coords.y2) / 2};

    void (*functions[2])() = {virtual_L_cb, virtual_R_cb};
    static lv_indev_drv_t drv_list_LR[2];
    
    drv_list_LR[0].type = LV_INDEV_TYPE_BUTTON;
    drv_list_LR[0].read_cb = functions[0];
    indev_L = lv_indev_drv_register(&drv_list_LR[0]);
    lv_indev_set_button_points(indev_L, point_array_L);

    lv_obj_update_layout(btn_R);
    point_array_R[0] = (lv_point_t) {(btn_R->coords.x1 + btn_R->coords.x2) / 2, (btn_R->coords.y1 + btn_R->coords.y2) / 2};

    drv_list_LR[1].type = LV_INDEV_TYPE_BUTTON;
    drv_list_LR[1].read_cb = functions[1];
    indev_R = lv_indev_drv_register(&drv_list_LR[1]);
    lv_indev_set_button_points(indev_R, point_array_R);

}

static void create_btm_btn_container()
{
    lv_obj_clean(btm_btn_container);
    lv_obj_remove_style_all(btm_btn_container);
    lv_obj_set_size(btm_btn_container, lv_pct(100), 30);

    lv_obj_set_style_radius(btm_btn_container, 0, 0);
    lv_obj_set_style_pad_all(btm_btn_container, 0, 0);

    lv_obj_align(btm_btn_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(btm_btn_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btm_btn_container,  LV_FLEX_ALIGN_START,  LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
}

static void add_btm_btn(lv_obj_t *cont ,u32 key , void *callback, lv_coord_t width, const char *label)
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
    lv_obj_add_style(btn_ptr, &btn_btm, 0);
    lv_obj_add_style(btn_ptr, &btn_press, LV_STATE_PRESSED);

    //Icon
    icon = lv_obj_create(btn_ptr);
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

    //Label
    label_btn = lv_label_create(btn_ptr);
    lv_label_set_text(label_btn, label);
    lv_obj_set_style_text_font(label_btn, &lv_font_montserrat_16, 0);

    // Layout: Flex
    lv_obj_set_flex_flow(btn_ptr, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_ptr,  LV_FLEX_ALIGN_CENTER,  LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Callback
    lv_obj_add_event_cb(btn_ptr, callback, LV_EVENT_ALL, 0);

    lv_obj_update_layout(btn_ptr);
    switch(key)
    {
        case KEY_A:
            point_array_A[0] = (lv_point_t) {(btn_ptr->coords.x1 + btn_ptr->coords.x2) / 2, (btn_ptr->coords.y1 + btn_ptr->coords.y2) / 2};
            lv_indev_set_button_points(indev_A, point_array_A);

            break;

        case KEY_B:
            point_array_B[0] = (lv_point_t) {(btn_ptr->coords.x1 + btn_ptr->coords.x2) / 2, (btn_ptr->coords.y1 + btn_ptr->coords.y2) / 2};
            lv_indev_set_button_points(indev_B, point_array_B);

            break;

        case KEY_X:
            point_array_X[0] = (lv_point_t) {(btn_ptr->coords.x1 + btn_ptr->coords.x2) / 2, (btn_ptr->coords.y1 + btn_ptr->coords.y2) / 2};
            lv_indev_set_button_points(indev_X, point_array_X);

            break;

        case KEY_Y:
            point_array_Y[0] = (lv_point_t) {(btn_ptr->coords.x1 + btn_ptr->coords.x2) / 2, (btn_ptr->coords.y1 + btn_ptr->coords.y2) / 2};
            lv_indev_set_button_points(indev_Y, point_array_Y);
            break;

        default:
            hang_err("Fail to register virtual button");
            break;
    }
}

static void remove_virtual_btn(u32 key)
{
    switch(key)
    {
        case KEY_A:
            point_array_A[0] = (lv_point_t) {-1, -1};
            lv_indev_set_button_points(indev_A, point_array_A);

            break;

        case KEY_B:
            point_array_B[0] = (lv_point_t) {-1, -1};
            lv_indev_set_button_points(indev_B, point_array_B);

            break;

        case KEY_X:
            point_array_X[0] = (lv_point_t) {-1, -1};
            lv_indev_set_button_points(indev_X, point_array_X);

            break;

        case KEY_Y:
            point_array_Y[0] = (lv_point_t) {-1, -1};
            lv_indev_set_button_points(indev_Y, point_array_Y);
            break;

        default:
            hang_err("Invalid virtual btn input");
            break;
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
    lv_obj_set_flex_align(icon_slider,LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t *icon = lv_img_create(icon_slider);
    lv_img_set_src(icon, img);
    lv_obj_set_style_max_height(icon, 48, 0);

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
    tab_add_icon_description(tab1, &_ncnn, " NCNN");
    tab_add_icon_description(tab1, &logo_lvgl, " LVGL");
    tab_add_icon_description(tab1, &devkitpro, " DevkitPRO");
    tab_add_icon_description(tab1, &ftpd_icon, " FTPD PRO");
    tab_add_icon_description(tab1, &citra_logo, " Citra Emulator");

    // Licences
    lv_obj_t *tab2 = lv_tabview_add_tab(tab_view, "About");
    lv_obj_set_width(tab2, 280);
    lv_obj_t *licences = lv_label_create(tab2);
    lv_label_set_text(licences, "This software is provided 'as-is', without any express or implied warranty."
    "In no event will the authors be held liable for any damages arising from the use of this software. "
    "Permission is granted to anyone to use this software for any purpose, including commercial applications, "
    "and to alter it and redistribute it freely, subject to the following restrictions:\n\n"
    "1.   The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.\n"
    "2.   Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n"
    "3.   This notice may not be removed or altered from any source distribution."
    );
    lv_label_set_long_mode(licences, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(licences, 250);

    lv_obj_t *tab_btn = lv_btn_create(tab_bg);
    lv_obj_add_style(tab_btn, &btn_tabview, 0);
    lv_obj_t *label = lv_label_create(tab_btn);
    lv_label_set_text(label, LV_SYMBOL_RIGHT);
    lv_obj_center(label);

    lv_obj_add_event_cb(tab_btn, close_tabview_cb, LV_EVENT_ALL, NULL);
}

void pop_up_tabview_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        if(!detecting)
        {
            detecting = true;
            pause_cam_capture(cam_buf);
        }

        tab_Ac_Li();
    }
}

void close_tabview_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        detecting = false;
        pause_cam_capture(cam_buf);
        lv_obj_del_async(tab_bg);
    }
}

void quit_detect_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        if(detecting)
        {
            lv_obj_clean(btm_btn_container);
            remove_virtual_btn(KEY_B);
            remove_virtual_btn(KEY_A);
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
            BoxInfo obj = BoxVec_getItem(idx, &objects);

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
        remove_virtual_btn(KEY_A);
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


void HALinit()
{ 
    // Screen init
    gfxInitDefault();
    gfxSetDoubleBuffering(GFX_BOTTOM, true);

    // Display init
    static lv_disp_draw_buf_t draw_buf_btm;
    static lv_color_t buf1_btm[WIDTH_BTM * HEIGHT_BTM];
    static lv_disp_drv_t disp_drv_btm;        /*Descriptor of a display driver*/
    lv_disp_t *disp_btm = display_init(GFX_BOTTOM, &draw_buf_btm, &*buf1_btm, &disp_drv_btm);

    // Initial touch screen's display, ui, and control 
    lv_disp_set_default(disp_btm);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE); // We don't want the screen to be scrollable

    // Touchpad init
    static lv_indev_drv_t indev_drv_touch;
    lv_indev_drv_init(&indev_drv_touch);      /*Basic initialization*/
    indev_drv_touch.type = LV_INDEV_TYPE_POINTER;
    indev_drv_touch.read_cb = touch_cb_3ds;
    lv_indev_drv_register(&indev_drv_touch); // lv_indev_t *touch_indev
    
    // Tick thread init
    thread_ticking = true;
    s32 prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    tick_thread = threadCreate(lvgl_tick_thread, NULL, STACKSIZE, prio-1, -2, false);
}

void res_init()
{
    add_res_depth16("romfs:button/cam_icon.png", &cam_icon);
    add_res_depth16("romfs:button/cam_icon_flip.png", &cam_icon_flip);
    add_res_depth16("romfs:button/iconL.png", &iconL);
    add_res_depth16("romfs:button/iconR.png", &iconR);
    add_res_depth16("romfs:button/Mid_fill.png", &Mid_fill);
}

void widgets_init()
{
    // Virtual ABXY driver initial
    virtual_button_driver_init();

    // Input init
    g = lv_group_create();
    static lv_indev_drv_t indev_drv_cross;
    lv_indev_drv_init(&indev_drv_cross);
    indev_drv_cross.type = LV_INDEV_TYPE_ENCODER;
    indev_drv_cross.read_cb = encoder_cb_3ds;
    lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_cross);
    lv_indev_set_group(enc_indev, g);

    // Style init
    button_style_init(&btn_btm);
    button_style_init(&btn_press);
    lv_style_set_bg_color(&btn_btm, lv_palette_lighten(LV_PALETTE_GREY, 2));
    lv_style_set_bg_color(&btn_press, lv_palette_darken(LV_PALETTE_GREY, 2));
    lv_style_init(&btn_shoulder_press);
    lv_style_set_img_recolor_opa(&btn_shoulder_press, LV_OPA_30);
    lv_style_set_img_recolor(&btn_shoulder_press, lv_color_black());
    lv_style_set_translate_y(&btn_shoulder_press, 2);

    lv_style_init(&btn_tabview);
    lv_style_set_align(&btn_tabview, LV_ALIGN_RIGHT_MID);
    lv_style_set_translate_x(&btn_tabview, 4);
    lv_style_set_height(&btn_tabview, lv_pct(30));
    lv_style_set_text_color(&btn_tabview, lv_color_hex(0x000000));
    lv_style_set_bg_color(&btn_tabview, lv_color_hex(0x5b5b5b));

    // BG
    LV_IMG_DECLARE(ncnn_bg_transprant);
    lv_obj_t *bg = lv_img_create(lv_scr_act());
    lv_img_set_src(bg, &ncnn_bg_transprant);

    lv_obj_t *hint_msg = lv_label_create(lv_scr_act());
    lv_label_set_text(hint_msg, "Press L, R, or A to detect");
    lv_obj_center(hint_msg);

    lv_obj_t *tab_btn = lv_btn_create(lv_scr_act());
    lv_obj_add_style(tab_btn, &btn_tabview, 0);
    lv_obj_t *label = lv_label_create(tab_btn);
    lv_label_set_text(label, LV_SYMBOL_LEFT);
    lv_obj_center(label);
    lv_obj_add_event_cb(tab_btn, pop_up_tabview_cb, LV_EVENT_CLICKED, NULL);

    // Other UI widget
    create_model_list(&det);
    create_LR();

    btm_btn_container = lv_obj_create(lv_scr_act());
    create_btm_btn_container();
    add_btm_btn(btm_btn_container, KEY_A, detect_cb, lv_pct(100), " Detect");

    // Detector, Detector objects and group of enconder containers
    det = create_nanodet(320, "romfs:nanodet-plus-m_416_int8.param", "romfs:nanodet-plus-m_416_int8.bin");   
}

void HAL_cleanup()
{
    thread_ticking = false;
    threadJoin(tick_thread, U64_MAX);
    threadFree(tick_thread);
}