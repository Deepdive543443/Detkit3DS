#include "sections.h"

static void cleanup()
{
    HAL_cleanup();
    ui_cleanup();
    lv_deinit();
    camExit();
    gfxExit();
    acExit();
}

int main(int argc, char **argv)
{
    if (setjmp(g_exitJmp)) {
        cleanup();
        return 0;
    }

    u32 kHeld;

    // IVGL init
    lv_init();
    HALinit();
    widgets_init();

    // Cam init
    camSetup();

    while (aptMainLoop()) {
        if (camUpdate()) continue;

        lv_timer_handler();

        // User input
        hidScanInput();
        kHeld = hidKeysHeld();
        if (kHeld & KEY_START) break;

        main_loop_locker();
    }

    cleanup();
    return 0;
}