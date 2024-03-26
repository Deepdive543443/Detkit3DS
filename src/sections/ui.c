#include "sections.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

UI_activity        g_ui_stack[LAYER_NUM];
int                g_num_ui_layer = -1;
static UI_activity s_activity_table[LAYER_NUM];

static void UI_activities_init()
{
    s_activity_table[LAYER_BACKGROUND] = ui_background_activities();
    s_activity_table[LAYER_STREAMING]  = ui_stream_activities();
    s_activity_table[LAYER_TABVIEW]    = ui_tabview_activities();
    s_activity_table[LAYER_RESULT]     = ui_result_activities();
}

void ui_layer_join(UI_layer layer)
{
    if (g_num_ui_layer != -1)
    {
        g_ui_stack[g_num_ui_layer].onLeave();
    }

    if (g_num_ui_layer < LAYER_NUM)
    {
        g_num_ui_layer++;
        g_ui_stack[g_num_ui_layer] = s_activity_table[layer];
        g_ui_stack[g_num_ui_layer].onCreate();
        g_ui_stack[g_num_ui_layer].onEnter();
    } else
    {
        char msg_err[40];
        sprintf(msg_err, "Fail to add layer: %d\n", layer);
        hang_err(msg_err);
    }
}

void ui_layer_pop()
{
    if (g_num_ui_layer >= 0)
    {
        g_ui_stack[g_num_ui_layer].onLeave();
        g_ui_stack[g_num_ui_layer].onDestroy();
        g_num_ui_layer--;
        g_ui_stack[g_num_ui_layer].onEnter();
    } else
    {
        hang_err("No UI to on stack\n");
    }
}

int add_res_depth16(const char *path, lv_img_dsc_t *res_buffer)
{
    int      width, height, n;
    uint8_t *pixels = (uint8_t *)stbi_load(path, &width, &height, &n, 0);
    if (pixels == NULL)
    {
        return 0;
    }

    uint8_t *lvgl_datas = malloc(sizeof(uint8_t) * width * height * 3);
    if (lvgl_datas == NULL)
    {
        return 0;
    }

    uint8_t *pixels_ptr    = pixels;
    uint8_t *lvgl_data_ptr = lvgl_datas;

    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            uint8_t r = pixels_ptr[0];
            uint8_t g = pixels_ptr[1];
            uint8_t b = pixels_ptr[2];
            uint8_t a = pixels_ptr[3];

            lvgl_data_ptr[0] = ((g & 0x1c) << 3) | ((b & 0xF8) >> 3);  // Lower 3 bit of green, 5 bit of Blue
            lvgl_data_ptr[1] = (r & 0xF8) | ((g & 0xE0) >> 5);         // Red 5 bit, Green 3 higher bit
            lvgl_data_ptr[2] = a;                                      // Alpha channels

            pixels_ptr += 4;
            lvgl_data_ptr += 3;
        }
    }

    if (lvgl_datas != NULL)
    {
        stbi_image_free(pixels);
    }

    *res_buffer = (lv_img_dsc_t){
        .header.cf          = LV_IMG_CF_TRUE_COLOR_ALPHA,
        .header.always_zero = 0,
        .header.reserved    = 0,
        .header.w           = width,
        .header.h           = height,
        .data_size          = width * height * n,
        .data               = lvgl_datas,
    };

    return 1;
}

void dealloc_res(lv_img_dsc_t *res_buffer) { free((void *)res_buffer->data); }

void widgets_init()
{
    encoder_driver_init();
    UI_activities_init();
    ui_layer_join(LAYER_BACKGROUND);

    button_init();
    ui_layer_join(LAYER_STREAMING);

    // Detector, Detector objects and group of enconder containers
    g_det = create_nanodet(320, "romfs:nanodet-plus-m_416_int8.param", "romfs:nanodet-plus-m_416_int8.bin");
}

void ui_cleanup()
{
    while (g_num_ui_layer > 0)
    {
        ui_layer_pop(g_ui_stack[g_num_ui_layer].idx);
        g_num_ui_layer--;
    }
}