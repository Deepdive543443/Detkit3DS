#include "sys/time.h"
#include "stdio.h"

#include "3ds.h" 
#include "lvgl/lvgl.h"

#include "sections.h"



static struct timespec start, end;

// Glob
jmp_buf exitJmp; //Debug

lv_group_t *g;
lv_obj_t *box_list;  // LVGL Objects

Detector det;
BoxVec objects; // Containers 
void *cam_buf;

bool detecting; // Stage marker


lv_obj_t *btn_A;
lv_obj_t *btn_B;
lv_obj_t *btn_X;
lv_obj_t *btn_Y;
lv_obj_t *btn_L;
lv_obj_t *btn_R;
lv_obj_t *btm_btn_container;

lv_indev_t *indev_A;
lv_indev_t *indev_B;
lv_indev_t *indev_X;
lv_indev_t *indev_Y; 
lv_indev_t *indev_L; 
lv_indev_t *indev_R; 
lv_point_t point_array_A[2];
lv_point_t point_array_B[2];
lv_point_t point_array_X[2];
lv_point_t point_array_Y[2];
lv_point_t point_array_L[2];
lv_point_t point_array_R[2];

lv_indev_drv_t drv_virbtn[4];//Encoder 

lv_style_t btn_btm;
lv_style_t btn_press;
lv_style_t btn_shoulder_press;
lv_style_t btn_tabview;


lv_obj_t *tab_bg;
lv_obj_t *tab_view;// pop up tab view

bool ticker()
{
    /* Hands the main loop until it reach the tick time*/
    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    return delta_us < TICK_MS;
}

void cleanup() 
{
    camExit();
    gfxExit();
    acExit();
}

void hang_err(const char *message)
{
    gfxExit();
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);
    printf("%s\nPress START to exit\n", message);
    while (aptMainLoop())
    {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) longjmp(exitJmp, 1);
    }
}

// User input place holder
static u32 kDown;
static u32 kHeld;

int main(int argc, char** argv)
{
    if(setjmp(exitJmp))
    {
        cleanup();
        return 0;
    }

    // Rom file system
    Result rc = romfsInit();
    if (rc)
        printf("romfsInit: %08lX\n", rc);

    else
    {
        printf("romfs Init Successful!\n");
    }

    gfxInitDefault();
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetDoubleBuffering(GFX_BOTTOM, true);
    gfxSet3D(false);

    // Camera framebuffer, state, hanlder init
    camInit();
    CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A);
    CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_15);
    CAMU_SetNoiseFilter(SELECT_OUT1, true);
    CAMU_SetAutoExposure(SELECT_OUT1, true);
    CAMU_SetAutoWhiteBalance(SELECT_OUT1, true);
    CAMU_SetTrimming(PORT_CAM1, false);

    cam_buf = malloc(SCRSIZE_TOP * 2); // RBG565 frame buffer
    if(!cam_buf)
    {
        hang_err("Failed to allocate memory!");
    }

    u32 bufSize;
    CAMU_GetMaxBytes(&bufSize, WIDTH_TOP, HEIGHT_TOP);
    CAMU_SetTransferBytes(PORT_CAM1, bufSize, WIDTH_TOP, HEIGHT_TOP);
    CAMU_Activate(SELECT_OUT1);

    Handle camReceiveEvent[2] = {0};
    CAMU_GetBufferErrorInterruptEvent(&camReceiveEvent[0], PORT_CAM1);
    CAMU_ClearBuffer(PORT_CAM1);
    CAMU_StartCapture(PORT_CAM1);
    CAMU_PlayShutterSound(SHUTTER_SOUND_TYPE_MOVIE);

    bool captureInterrupted = false;
    s32 index = 0;
    detecting = false;

    // IVGL init
    lv_init();

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

    // Display init
    static lv_disp_draw_buf_t draw_buf_btm;
    static lv_color_t buf1_btm[WIDTH_BTM * HEIGHT_BTM];
    static lv_disp_drv_t disp_drv_btm;        /*Descriptor of a display driver*/
    lv_disp_t *disp_btm = display_init(GFX_BOTTOM, &draw_buf_btm, &buf1_btm, &disp_drv_btm);

    // Initial touch screen's display, ui, and control 
    lv_disp_set_default(disp_btm);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE); // We don't want the screen to be scrollable

    // BG
    LV_IMG_DECLARE(ncnn_bg_transprant);
    lv_obj_t *bg = lv_img_create(lv_scr_act());
    lv_img_set_src(bg, &ncnn_bg_transprant);

    lv_obj_t *hint_msg = lv_label_create(lv_scr_act());
    lv_label_set_text(hint_msg, "Press L, R, or A to detect");
    lv_obj_center(hint_msg);

    lv_obj_t *tab_btn = lv_btn_create(lv_scr_act());
    lv_obj_add_style(tab_btn, &btn_tabview, NULL);
    lv_obj_t *label = lv_label_create(tab_btn);
    lv_label_set_text(label, LV_SYMBOL_LEFT);
    lv_obj_center(label);
    lv_obj_add_event_cb(tab_btn, pop_up_tabview_cb, LV_EVENT_CLICKED, NULL);

    // Detector, Detector objects and group of enconder containers
    det = create_nanodet(320, "romfs:nanodet-plus-m_416_int8.param", "romfs:nanodet-plus-m_416_int8.bin");    
    g = lv_group_create();

    // Other UI widget
    create_model_list(&det);
    create_LR();

    btm_btn_container = lv_obj_create(lv_scr_act());
    create_btm_btn_container();
    add_btm_btn(btm_btn_container, KEY_A, detect_cb, lv_pct(100), " Detect");


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
        if (!detecting)
        {
            if (camReceiveEvent[1] == 0) 
            {
                CAMU_SetReceiving(&camReceiveEvent[1], cam_buf, PORT_CAM1, SCRSIZE_TOP * 2, (s16)bufSize);
            }

            if (captureInterrupted) 
            {
                CAMU_StartCapture(PORT_CAM1);
                captureInterrupted = false;
            }

            svcWaitSynchronizationN(&index, camReceiveEvent, 2, false, WAIT_TIMEOUT);
            switch (index)
            {
                case 0:
                    svcCloseHandle(camReceiveEvent[1]);
                    camReceiveEvent[1] = 0;
                    captureInterrupted = true;

                    continue;
                    break;

                case 1:
                    svcCloseHandle(camReceiveEvent[1]);
                    camReceiveEvent[1] = 0;

                    break;

                default:
                    break;

            }

            writeCamToFramebufferRGB565(
                gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 
                cam_buf,
                0,
                0,
                WIDTH_TOP,
                HEIGHT_TOP
            );

            // Flush and swap framebuffers
            gfxFlushBuffers();
            gfxScreenSwapBuffers(GFX_TOP, true);
            gspWaitForVBlank();   
        }

        if (!captureInterrupted) 
        {
            // User input
            hidScanInput();
            kDown = hidKeysDown();
            kHeld = hidKeysHeld();

            // Quit App
            if(kHeld & KEY_START)   hang_err("Testing hand error, just hand it by any means!");

            lv_timer_handler();
            while (ticker());

            // Display joystick    
            lv_tick_inc(TICK_S);   
        }
    }

    lv_deinit();
    cleanup();
    return 0;
}