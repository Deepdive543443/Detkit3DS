#include "sections.h"

static void cleanup()
{
    dev_cleanup();
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
    dev_init();
    ui_init();

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

        frame_ctl();
    }

    cleanup();
    return 0;
}