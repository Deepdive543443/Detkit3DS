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

    gfxInitDefault();
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetDoubleBuffering(GFX_BOTTOM, true);
    gfxSet3D(false);

    // Camera framebuffer, state, hanlder init
    camInit();
    CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A);
    CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_15);
    CAMU_SetNoiseFilter(SELECT_OUT1, true);
    CAMU_SetAutoExposure(SELECT_OUT1, true);
    CAMU_SetAutoWhiteBalance(SELECT_OUT1, true);
    CAMU_SetTrimming(PORT_CAM1, false);

    cam_buf = malloc(SCRSIZE_TOP * 2); // RBG565 frame buffer
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
    detecting = false;

    // IVGL init
    lv_init();
    HALinit();
    
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

            // Quit App
            if(kHeld & KEY_START)   hang_err("Testing hand error, just hand it by any means!");

            lv_timer_handler();
            while (ticker());

            // Display joystick    
            lv_tick_inc(TICK_S);   
        }
    }

    cleanup();
    return 0;
}