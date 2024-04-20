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

    // IVGL init
    lv_init();
    HALinit();
    widgets_init();

#ifndef BUILD_CIA
    u32 kHeld;
#endif  // BUILD_CIA

    while (aptMainLoop()) {
        if (camUpdate()) continue;
        lv_timer_handler();
        hidScanInput();

#ifndef BUILD_CIA
        kHeld = hidKeysHeld();
        if (kHeld & KEY_START) break;
#endif  // BUILD_CIA

        main_loop_locker();
    }

    cleanup();
    return 0;
}