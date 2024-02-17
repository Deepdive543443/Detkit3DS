#include "3ds.h"
#include "stdio.h"
#include "sys/time.h"

#include "sections.h"

#include "lvgl/lvgl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


static struct timespec start, end;

uint64_t ticker()
{
    /* Hands the main loop until it reach the tick time*/
    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    return delta_us;
}

int main(int argc, char **argv)
{
	gfxInitDefault();

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_TOP, NULL);

    APT_SetAppCpuTimeLimit(80);

    // Rom file system
    Result rc = romfsInit();
    if (rc)
        printf("romfsInit: %08lX\n", rc);

    else
    {
        printf("romfs Init Successful!\n");
    }

    Detector det;

    int test_times = 100;
    while(test_times > 0)
    {
        printf("\ntest_times: %d\n", test_times);
        test_times--;
        {
            clock_gettime(CLOCK_MONOTONIC, &start);
            unsigned char *pixels_cpy = (unsigned char *) malloc(sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
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
            unsigned char *pixels_cpy = (unsigned char *) malloc(sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
            memset(pixels_cpy, 0.5f, sizeof(unsigned char) * WIDTH_TOP * HEIGHT_TOP * 3);
            det = create_fastestdet(352, "romfs:FastestDet.param", "romfs:FastestDet.bin");
            BoxVec objects = det.detect(pixels_cpy, WIDTH_TOP, HEIGHT_TOP, &det);
            printf("(FastestDet) Detected %d items, (%lldms): \n", objects.num_item, ticker());

            free(pixels_cpy);   

            destroy_detector(&det);
            BoxVec_free(&objects);
        }
    }
    printf("Pass all testing successfully!\n");

	printf("\x1b[30;16HPress Start to exit.");

	// Main loop
	while (aptMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}