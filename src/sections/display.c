#include "sections/display.h"

static void flush_cb_3ds(gfxScreen_t gfx_scr, lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    /* Update and swap frame buffer
     * TODO -- figure out a more efficient solution.
     * */
    writePic2FrameBuf565(gfxGetFramebuffer(gfx_scr, GFX_LEFT, NULL, NULL), color_p, 0, 0, disp->hor_res, disp->ver_res);

    // These three lines swap and display new buffer on screen
    gfxFlushBuffers();
    gfxScreenSwapBuffers(gfx_scr, true);
    gspWaitForVBlank();

    lv_disp_flush_ready(disp);
}

static void flush_cb_3ds_btm(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    flush_cb_3ds(GFX_BOTTOM, disp, area, color_p);
}

static void flush_cb_3ds_top(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    flush_cb_3ds(GFX_TOP, disp, area, color_p);
}

void writePic2FrameBuf565(void *fb, lv_color_t *color, u16 x, u16 y, u16 w, u16 h)
{
    /* Display the LVGL buffer on 3DS screen
     * void *fb: 3DS SDK's screen framebuffer input
     * void *img: input 16 bit framebuffer in 16bit per pixel
     * u16 x, y, w, h: Input area
     * */

    u8 *fb_8 = (u8 *)fb;

    u16 i, j, draw_x, draw_y;
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            draw_y = y + h - j - 1;
            draw_x = x + i;

            u32 v = (draw_y + draw_x * h) * 3;

            fb_8[v + 2] = color[j * w + i].ch.red << 3;    //((data >> 11) & 0x1F) << 3;
            fb_8[v + 1] = color[j * w + i].ch.green << 2;  //((data >> 5) & 0x3F) << 2;
            fb_8[v]     = color[j * w + i].ch.blue << 3;   //(data & 0x1F) << 3;
        }
    }
}

lv_disp_t *display_init(gfxScreen_t gfx_scr)
{
    lv_disp_t *display = NULL;
    switch (gfx_scr) {
        case GFX_TOP:

            static lv_disp_drv_t      disp_drv_top;
            static lv_disp_draw_buf_t draw_buf_struct_top;
            static lv_color_t         buf1_top[WIDTH_TOP * HEIGHT_TOP];
            lv_disp_drv_init(&disp_drv_top);
            lv_disp_draw_buf_init(&draw_buf_struct_top, buf1_top, NULL, WIDTH_TOP * HEIGHT_TOP);
            disp_drv_top.flush_cb    = flush_cb_3ds_top; /*Set your driver function*/
            disp_drv_top.hor_res     = WIDTH_TOP;        /*Set the horizontal resolution of the display*/
            disp_drv_top.ver_res     = HEIGHT_TOP;       /*Set the vertical resolution of the display*/
            disp_drv_top.draw_buf    = &draw_buf_struct_top;
            disp_drv_top.direct_mode = 1;
            display                  = lv_disp_drv_register(&disp_drv_top);
            break;

        case GFX_BOTTOM:

            static lv_disp_drv_t      disp_drv_btm;
            static lv_disp_draw_buf_t draw_buf_struct_btm;
            static lv_color_t         buf1_btm[WIDTH_BTM * HEIGHT_BTM];
            lv_disp_drv_init(&disp_drv_btm);
            lv_disp_draw_buf_init(&draw_buf_struct_btm, buf1_btm, NULL, WIDTH_BTM * HEIGHT_BTM);
            disp_drv_btm.flush_cb    = flush_cb_3ds_btm; /*Set your driver function*/
            disp_drv_btm.hor_res     = WIDTH_BTM;        /*Set the horizontal resolution of the display*/
            disp_drv_btm.ver_res     = HEIGHT_BTM;       /*Set the vertical resolution of the display*/
            disp_drv_btm.draw_buf    = &draw_buf_struct_btm;
            disp_drv_btm.direct_mode = 1;
            display                  = lv_disp_drv_register(&disp_drv_btm);
            break;

        default:
            break;
    }

    lv_disp_set_default(display);
    lv_obj_clear_flag(lv_disp_get_scr_act(display), LV_OBJ_FLAG_SCROLLABLE);
    return display;
}