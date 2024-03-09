#include <stdio.h>
#include <time.h>
#include "sections.h"

// Glob
jmp_buf exitJmp; // Debug
Detector det;
BoxVec objects; // Containers
bool detecting; // Stage marker
void *cam_buf;
bool g_thread_ticking;

static Thread tick_thread; // Thread
static struct timespec start, end;

static void lvgl_tick_thread()
{
    while (g_thread_ticking)
    {
        svcSleepThread((s64)TICK_NS);
        lv_tick_inc(TICK_MS);
    }
}

bool main_loop_locker()
{
    /* Hands the main loop until it reach the tick time*/
    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    return delta_us < TICK_US;
}

bool time_stamp_update()
{
    clock_gettime(CLOCK_MONOTONIC, &start); 
    return true;
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

        if (kDown & KEY_START)
            longjmp(exitJmp, 1);
    }
}

void HALinit()
{
    // Screen init
    gfxInitDefault();
    gfxSetDoubleBuffering(GFX_BOTTOM, true);

#if USE_SYS_CORE
    APT_SetAppCpuTimeLimit(80);
#endif

    // Display init
    static lv_disp_draw_buf_t draw_buf_btm;
    static lv_color_t buf1_btm[WIDTH_BTM * HEIGHT_BTM];
    static lv_disp_drv_t disp_drv_btm; /*Descriptor of a display driver*/
    lv_disp_t *disp_btm = display_init(GFX_BOTTOM, &draw_buf_btm, &*buf1_btm, &disp_drv_btm);

    // Initial touch screen's display, ui, and control
    lv_disp_set_default(disp_btm);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE); // We don't want the screen to be scrollable

    // Touchpad init
    static lv_indev_drv_t indev_drv_touch;
    lv_indev_drv_init(&indev_drv_touch); /*Basic initialization*/
    indev_drv_touch.type = LV_INDEV_TYPE_POINTER;
    indev_drv_touch.read_cb = touch_cb_3ds;
    lv_indev_drv_register(&indev_drv_touch); // lv_indev_t *touch_indev

    // Tick thread init
    g_thread_ticking = true;
    s32 prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);

#if USE_SYS_CORE
    tick_thread = threadCreate(lvgl_tick_thread, NULL, STACKSIZE, prio - 1, 1, false);
#else
    tick_thread = threadCreate(lvgl_tick_thread, NULL, STACKSIZE, prio - 1, -2, false);
#endif // USE_SYS_CORE
}

void HAL_cleanup()
{
    g_thread_ticking = false;
    threadJoin(tick_thread, U64_MAX);
    threadFree(tick_thread);
}
