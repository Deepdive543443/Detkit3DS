#include "sections.h"

static void cleanup()
{
    destroy_detector(&g_det);
    HAL_cleanup();
    ui_cleanup();
    lv_deinit();
    camExit();
    gfxExit();
    acExit();
}

int main(int argc, char **argv)
{
    if (setjmp(g_exitJmp))
    {
        cleanup();
        return 0;
    }

    u32 kHeld;

    // IVGL init
    lv_init();
    HALinit();
    res_init();
    widgets_init();

    // Cam init
    camSetup();

    while (aptMainLoop())
    {
        if (camUpdate()) continue;
        lv_timer_handler();

        // User input
        hidScanInput();
        kHeld = hidKeysHeld();

        // Quit App
        if (kHeld & KEY_START) break;

        main_loop_locker();
    }

    cleanup();
    return 0;
}