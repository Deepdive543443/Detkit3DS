#include <stdlib.h>
#include "sections.h"

static void cleanup()
{
    destroy_detector(&det);
    BoxVec_free(&objects);
    HAL_cleanup();
    res_cleanup();
    lv_deinit();
    camExit();
    gfxExit();
    acExit();
}

int main(int argc, char **argv)
{
    if (setjmp(exitJmp))
    {
        cleanup();
        return 0;
    }

    // Rom file system
    Result rc = romfsInit();
    if (rc)
    {
        char err[40];
        sprintf(err, "romfs init failed: %08lX\n", rc);
        hang_err(err);
    }

    // Camera framebuffer init
    cam_buf = malloc(SCRSIZE_TOP * 2);  // RBG565 frame buffer
    if (!cam_buf)
    {
        hang_err("Failed to allocate memory for Camera!");
    }

    detecting = false;
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
        lv_timer_handler();
        if (!detecting)
        {
            if (camUpdate()) continue;
        }

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