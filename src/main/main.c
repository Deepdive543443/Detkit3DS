#include "sys/time.h"
#include "stdio.h"

#include "3ds.h" 
#include "lvgl-8.3.11/lvgl.h"

#include "sections.h"


static struct timespec start, end;

bool ticker()
{
    /* Hands the main loop until it reach the tick time*/
    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    return delta_us < TICK_MS;
}

// User input place holder
static u32 kDown;
static u32 kHeld;
static circlePosition joy_stick;

int main(int argc, char** argv)
{

    // Rom file system
    Result rc = romfsInit();
    if (rc)
        printf("romfsInit: %08lX\n", rc);

    else
    {
        printf("romfs Init Successful!\n");
    }

    gfxInitDefault();
    // PrintConsole topScreen;
    // consoleInit(GFX_TOP, &topScreen);
    // consoleSelect(&topScreen);

    // IVGL init
    lv_init();

    // Display init
    static lv_disp_draw_buf_t draw_buf_btm;
    static lv_color_t buf1_btm[WIDTH_TOP * HEIGHT_TOP];
    static lv_disp_drv_t disp_drv_btm;        /*Descriptor of a display driver*/
    lv_disp_t *disp_btm = display_init(GFX_BOTTOM, &draw_buf_btm, &buf1_btm, &disp_drv_btm);


    // Initial top screen's display, ui, and control 
    lv_disp_set_default(disp_btm);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE); // We don't want the screen to be scrollable
    lv_group_t *g = lv_group_create();
    
    // lv_obj_t *btm_text = put_text_example("Hello\nLVGL 3DS");
    lv_obj_t *model_list = create_model_list();
    ui_LR_t ui_LR = create_shoulder_button();
    lv_obj_t *boxxes = create_box_list(g); // Dummy boxxes



    // Input init
    static lv_indev_drv_t indev_drv_cross;
    lv_indev_drv_init(&indev_drv_cross);
    indev_drv_cross.type = LV_INDEV_TYPE_ENCODER;
    indev_drv_cross.read_cb = encoder_cb_3ds;
    lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_cross);
    lv_indev_set_group(enc_indev, g);

    // Touchpad init
    static lv_indev_drv_t indev_drv_touch;
    lv_indev_drv_init(&indev_drv_touch);      /*Basic initialization*/
    indev_drv_touch.type = LV_INDEV_TYPE_POINTER;
    indev_drv_touch.read_cb = touch_cb_3ds;
    lv_indev_t *touch_indev = lv_indev_drv_register(&indev_drv_touch);


    while(aptMainLoop())
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        // User input
        hidScanInput();
        kDown = hidKeysDown();
        kHeld = hidKeysHeld();
        hidCircleRead(&joy_stick);

        // Quit App
        if(kHeld & KEY_START) break;

        lv_timer_handler();
        while (ticker());

        // Display joystick    
        lv_tick_inc(TICK_S);
    }

    free(ui_LR.point_array_L);
    free(ui_LR.point_array_R);
    return 0;
}