#include "sys/time.h"
#include "stdio.h"

#include "3ds.h" 
#include "lvgl/lvgl.h"

#include "sections.h"


static struct timespec start, end;

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
        lv_deinit();
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

    // Screen init
    gfxInitDefault();
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetDoubleBuffering(GFX_BOTTOM, true);
    gfxSet3D(false);

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
    camSetup();
    
    while(aptMainLoop())
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        if (!detecting)
        {
            if (camUpdate())    continue;  
        }

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

    cleanup();
    return 0;
}