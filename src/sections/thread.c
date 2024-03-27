#include <stdio.h>
#include <time.h>
#include "sections/thread.h"

// Glob
jmp_buf  g_exitJmp;  // Debug
Detector g_det;

static bool   s_thread_ticking;
static Thread s_tick_thread;  // Thread

static void lvgl_tick_thread()
{
    while (s_thread_ticking) {
        svcSleepThread((s64)TICK_NS);
        lv_tick_inc(TICK_MS);
    }
}

void main_loop_locker() { svcSleepThread((s64)TICK_NS); }

void hang_err(const char *message)
{
    gfxExit();
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);
    printf("%s\nPress START to exit\n", message);
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) longjmp(g_exitJmp, 1);
    }
}

void HALinit()
{
    // Rom file system
    Result rc = romfsInit();
    if (rc) {
        char err[40];
        sprintf(err, "romfs init failed: %08lX\n", rc);
        hang_err(err);
    }

    // Screen init
    gfxInitDefault();
    gfxSetDoubleBuffering(GFX_BOTTOM, true);

#if USE_SYS_CORE
    APT_SetAppCpuTimeLimit(80);
#endif

    // Display init
    lv_disp_t *disp_btm = display_init(GFX_BOTTOM);
    lv_disp_set_default(disp_btm);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);  // We don't want the screen to be scrollable

    // Tick thread init
    s_thread_ticking = true;
    s32 prio         = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);

#if USE_SYS_CORE
    s_tick_thread = threadCreate(lvgl_tick_thread, NULL, STACKSIZE, prio - 1, 1, false);
#else
    s_tick_thread = threadCreate(lvgl_tick_thread, NULL, STACKSIZE, prio - 1, -2, false);
#endif  // USE_SYS_CORE

    // Detector init
    g_det = create_nanodet(320, "romfs:nanodet-plus-m_416_int8.param", "romfs:nanodet-plus-m_416_int8.bin");
}

void HAL_cleanup()
{
    destroy_detector(&g_det);
    s_thread_ticking = false;
    threadJoin(s_tick_thread, U64_MAX);
    threadFree(s_tick_thread);
}
