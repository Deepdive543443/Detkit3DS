#include "sys/time.h"
#include "stdio.h"

#include "3ds.h" 
#include "lvgl/lvgl.h"

#include "sections.h"


static struct timespec start, end;

static bool main_loop_locker()
{
    /* Hands the main loop until it reach the tick time*/
    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    return delta_us < TICK_US;
}

static void cleanup() 
{
    destroy_detector(&det);
    BoxVec_free(&objects);
    HAL_cleanup();
    lv_deinit();
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
    {
        char err[40];
        sprintf(err, "romfs init failed: %08lX\n", rc);
        hang_err(err);
    }

    // Camera framebuffer init
    cam_buf = malloc(SCRSIZE_TOP * 2); // RBG565 frame buffer
    if(!cam_buf)
    {
        hang_err("Failed to allocate memory for Camera!");
    }

    detecting = false;

    // IVGL init
    lv_init();
    HALinit();

    // Cam init
    camSetup();
    
    while(aptMainLoop())
    {
        lv_timer_handler();
        clock_gettime(CLOCK_MONOTONIC, &start);
        if (!detecting)
        {
            if (camUpdate())    continue;  
        }

        // User input
        hidScanInput();
        kHeld = hidKeysHeld();

        // Quit App
        if(kHeld & KEY_START)   break;

        while (main_loop_locker());
    }

    cleanup();
    return 0;
}