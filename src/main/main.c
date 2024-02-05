#include "sys/time.h"
#include "stdio.h"

#include "3ds.h" 
#include "lvgl-8.3.11/lvgl.h"

#include "sections.h"


static struct timespec start, end;

lv_group_t *g;
lv_obj_t *box_list; 

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
    printf("%s\nPress START to exit\n", message);
    while (aptMainLoop())
    {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;
    }
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
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetDoubleBuffering(GFX_BOTTOM, true);

    // Camera framebuffer, state, hanlder init
    camInit();
    CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A);
    CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_15);
    CAMU_SetNoiseFilter(SELECT_OUT1, true);
    CAMU_SetAutoExposure(SELECT_OUT1, true);
    CAMU_SetAutoWhiteBalance(SELECT_OUT1, true);
    CAMU_SetTrimming(PORT_CAM1, false);

    void *cam_buf = malloc(SCRSIZE_TOP * 2); // RBG565 frame buffer
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
    bool detecting = false;


    // IVGL init
    lv_init();

    // Display init
    static lv_disp_draw_buf_t draw_buf_btm;
    static lv_color_t buf1_btm[WIDTH_BTM * HEIGHT_BTM];
    static lv_disp_drv_t disp_drv_btm;        /*Descriptor of a display driver*/
    lv_disp_t *disp_btm = display_init(GFX_BOTTOM, &draw_buf_btm, &buf1_btm, &disp_drv_btm);


    // Initial touch screen's display, ui, and control 
    lv_disp_set_default(disp_btm);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE); // We don't want the screen to be scrollable
    
    
    // lv_obj_t *btm_text = put_text_example("Hello\nLVGL 3DS");

    // BG
    LV_IMG_DECLARE(ncnn_bg_transprant);
    lv_obj_t *bg = lv_img_create(lv_scr_act());
    lv_img_set_src(bg, &ncnn_bg_transprant);

    // Detector, Detector objects and group of enconder containers
    Detector det = create_nanodet(320, "romfs:nanodet-plus-m_416_int8.param", "romfs:nanodet-plus-m_416_int8.bin");
    BoxVec objects;    
    // lv_group_t *g;
    // lv_obj_t *box_list;    // lv_obj_t *boxxes = create_box_list(g); // Dummy boxxes

    // Other UI widget
    lv_obj_t *model_list = create_model_list(&det);
    ui_LR_t ui_LR = create_shoulder_button(&det, cam_buf, &detecting, &objects, g, box_list);
    // ui_LR_t btm_btn = create_bottom_btn();




    // Input init
    static lv_indev_drv_t indev_drv_cross;
    lv_indev_drv_init(&indev_drv_cross);
    indev_drv_cross.type = LV_INDEV_TYPE_ENCODER;
    indev_drv_cross.read_cb = encoder_cb_3ds;
    lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_cross);
    lv_indev_set_group(enc_indev, &g);

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

            gfxSet3D(false);
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
            hidCircleRead(&joy_stick);

            // Quit App
            if(kHeld & KEY_START) break;

            if(kDown & KEY_SELECT) 
            {
                detecting = !detecting;
                pause_cam_capture(cam_buf); 
            }

            if(kDown & KEY_X)
            {
                if(box_list != NULL)
                {
                    lv_obj_del(box_list);
                    detecting = false;        
                }

            }

            lv_timer_handler();
            while (ticker());

            // Display joystick    
            lv_tick_inc(TICK_S);   
        }
    }

    free(ui_LR.point_array_L);
    free(ui_LR.point_array_R);
    cleanup();
    return 0;
}