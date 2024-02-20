#include "3ds.h"
#include "stdio.h"

#include "sections.h"

#include "lvgl/lvgl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


static struct timespec start, end;
static lv_disp_t *disp_top;

static bool main_loop_locker()
{
    /* Hands the main loop until it reach the tick time*/
    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    return delta_us < TICK_US;
}

static void scr_init()
{
	lv_init();
	HALinit(); // btn screen, driver, and thread init

	// Top screen Display init
    static lv_disp_draw_buf_t draw_buf_top;
    static lv_color_t buf1_top[WIDTH_TOP * HEIGHT_TOP];
    static lv_disp_drv_t disp_drv_top;        /*Descriptor of a display driver*/
	disp_top = display_init(GFX_TOP, &draw_buf_top, &*buf1_top, &disp_drv_top);


	lv_obj_set_style_bg_color(lv_disp_get_scr_act(disp_top), lv_color_hex(0xf4cccc), 0);
	lv_obj_t *label = lv_label_create(lv_disp_get_scr_act(disp_top));
	lv_obj_center(label);
	lv_label_set_text(label, "Hello LVGL!");

	lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_COLUMN);
	for(int i =0; i < 3; i++)
	{
		label = lv_label_create(lv_scr_act());
		lv_label_set_text(label, "Hello LVGL!");		
	}


}


int main(int argc, char **argv)
{
	gfxInitDefault();
	// Rom file system
    Result rc = romfsInit();
    if (rc)
    {
        printf("romfs init failed: %08lX\n", rc);
    }
	
	scr_init();

	// Main loop
	while (aptMainLoop())
	{
		lv_timer_handler();
        clock_gettime(CLOCK_MONOTONIC, &start);
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu
        while (main_loop_locker());
	}

	gfxExit();
	return 0;
}