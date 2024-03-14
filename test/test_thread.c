#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "3ds.h"
#include "sections.h"

#define TEST_THREAD    1
#define TEST_INFERENCE 1

#define DURATION_MILLION_SEC 1000000
#define MILLION_SEC          1000
#define STACKSIZE            (4 * 1024)

static int    timer   = 0;
static bool   ticking = false;
static Thread timer_thread;

void timer_thread_func()
{
    while (ticking)
    {
        printf("Timer: %d\n", timer);
        timer++;
        svcSleepThread((u64)MILLION_SEC * DURATION_MILLION_SEC);
    }
}

static struct timespec start, end;

uint64_t ticker()
{
    /* Hands the main loop until it reach the tick time*/
    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    return delta_us / 1000;
}

int main(int argc, char **argv)
{
    gfxInitDefault();

    // Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal
    // console structure as current one
    consoleInit(GFX_TOP, NULL);

#if USE_SYS_CORE
    APT_SetAppCpuTimeLimit(80);
#endif

    s32 prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    printf("Main thread prio: 0x%lx\n", prio);

    ticking = true;

#if USE_SYS_CORE
    timer_thread = threadCreate(timer_thread_func, NULL, STACKSIZE, prio - 1, 1, false);
#else
    timer_thread = threadCreate(timer_thread_func, NULL, STACKSIZE, prio - 1, -2, false);
#endif

    // Rom file system
    Result rc = romfsInit();
    if (rc)
        printf("romfsInit: %08lX\n", rc);

    else
    {
        printf("romfs Init Successful!\n");
    }

#if TEST_INFERENCE

    Detector det;

    int test_times = 2;
    while (test_times > 0)
    {
        printf("\ntest_times: %d\n", test_times);
        test_times--;
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
            unsigned char *pixels_cpy = (unsigned char *)malloc(sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
            memset(pixels_cpy, 0.5f, sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
            det = create_nanodet(320, "romfs:nanodet-plus-m_416_int8.param", "romfs:nanodet-plus-m_416_int8.bin");
            BoxVec objects = det.detect(pixels_cpy, WIDTH_TOP, HEIGHT_TOP, &det);
            printf("(Nanodet) Detected %d items, (%lldms): \n", objects.num_item, ticker());

            free(pixels_cpy);

            destroy_detector(&det);
            BoxVec_free(&objects);
        }

        /**
         * Create FastestDet
         *
         */
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
            unsigned char *pixels_cpy = (unsigned char *)malloc(sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
            memset(pixels_cpy, 0.5f, sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
            det            = create_fastestdet(352, "romfs:FastestDet.param", "romfs:FastestDet.bin");
            BoxVec objects = det.detect(pixels_cpy, WIDTH_TOP, HEIGHT_TOP, &det);
            printf("(FastestDet) Detected %d items, (%lldms): \n", objects.num_item, ticker());

            free(pixels_cpy);

            destroy_detector(&det);
            BoxVec_free(&objects);
        }
    }
    printf("Pass all testing successfully!\n");

#endif

    consoleSelect(consoleInit(GFX_BOTTOM, NULL));
    printf("\x1b[10;10HPress Start to exit.\n");
    // Main loop
    while (aptMainLoop())
    {
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous
        // frame)
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;  // break in order to return to hbmenu

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();

        // Wait for VBlank
        gspWaitForVBlank();
    }
    printf("Wating for all thread to terminated...\n");
    ticking = false;
    threadJoin(timer_thread, U64_MAX);
    threadFree(timer_thread);

    gfxExit();
    return 0;
}