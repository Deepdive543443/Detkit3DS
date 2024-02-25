#include "3ds.h"
#include "stdio.h"

#include "sections.h"

#include "lvgl/lvgl.h"


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

	// Top screen scroll init 
	lv_group_t *g = lv_group_create();
	static lv_indev_drv_t indev_drv_cross;
	lv_indev_drv_init(&indev_drv_cross);
	indev_drv_cross.type = LV_INDEV_TYPE_ENCODER;
	indev_drv_cross.read_cb = encoder_cb_3ds;
	lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_cross);
	lv_indev_set_group(enc_indev, g);
	lv_group_add_obj(g, lv_disp_get_scr_act(disp_top));


	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xf4cccc), 0);
	lv_obj_t *label = lv_label_create(lv_scr_act());
	lv_obj_center(label);
	lv_label_set_text(label, "Hello LVGL!");

	lv_disp_set_default(disp_top);
	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xf7e4c6), 0);

	lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_COLUMN);
	for(int i =0; i < 5; i++)
	{
		label = lv_label_create(lv_scr_act());
		lv_label_set_text(label, "Hello LVGL!");		
	}
}

static void load_img()
{
	int width, height, n;
	const char *file = "romfs:/button/cam_icon.png";
	uint8_t *pixels = (uint8_t *) stbi_load(file, &width, &height, &n, 0);


	lv_obj_t *label = lv_label_create(lv_scr_act());
	lv_label_set_text_fmt(label, "Width: %d Height: %d Channels: %d", width, height, n);

	lv_obj_t *img = lv_img_create(lv_scr_act());
	lv_img_set_src(img, LV_SYMBOL_IMAGE);

	uint8_t *lvgl_datas = malloc(sizeof(uint8_t) * width * height * 3); 
	uint8_t *pixels_ptr = pixels;
	uint8_t *lvgl_data_ptr = lvgl_datas;


	for(int h=0; h < height; h++)
	{
		for(int w=0; w < width; w++)
		{
			uint8_t r = pixels_ptr[0];
			uint8_t g = pixels_ptr[1];
			uint8_t b = pixels_ptr[2];
			uint8_t a = pixels_ptr[3];

			lvgl_data_ptr[0] = ((g & 0x1c) << 3) | ((b & 0xF8) >> 3); // Lower 3 bit of green, 5 bit of Blue
			lvgl_data_ptr[1] = (r & 0xF8) | ((g & 0xE0) >> 5); // Red 5 bit, Green 3 higher bit
			lvgl_data_ptr[2] = a; // Alpha channels

			pixels_ptr+=4;
			lvgl_data_ptr+=3;
		}
	}

	const lv_img_dsc_t loaded_img = {
		.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
		.header.always_zero = 0,
		.header.reserved = 0,
		.header.w = width,
		.header.h = height,
		.data_size = width * height * n,
		.data = lvgl_datas,
	};

	img = lv_img_create(lv_scr_act());
	lv_img_set_src(img, &loaded_img);

	label = lv_label_create(lv_scr_act());
	lv_label_set_text(label, "Top compare: ");

	// free(lvgl_datas);
	stbi_image_free(pixels);
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
	load_img();

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