#include "sections.h" 


static void flush_cb_3ds(gfxScreen_t gfx_scr, lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    /* Update and swap frame buffer
     * TODO -- figure out a more efficient solution.
     * */
    writePic2FrameBuf565(
        gfxGetFramebuffer(gfx_scr, GFX_LEFT, NULL, NULL),
        color_p,
        0,
        0,
        disp->hor_res,
        disp->ver_res
    );

    // These three lines swap and display new buffer on screen
    gfxFlushBuffers();
    gfxScreenSwapBuffers(gfx_scr, true);
    gspWaitForVBlank();

    lv_disp_flush_ready(disp);
}

void writePic2FrameBuf565(void *fb, lv_color_t * color, u16 x, u16 y, u16 w, u16 h)
{
    /* Display the LVGL buffer on 3DS screen 
     * void *fb: 3DS SDK's screen framebuffer input
     * void *img: input 16 bit framebuffer in 16bit per pixel
     * u16 x, y, w, h: Input area
     * */

    u8 *fb_8 = (u8*) fb;

    u16 i, j, draw_x, draw_y;
    for(j = 0; j < h; j++)
    {
        for(i = 0; i < w; i++)
        {
            draw_y = y + h - j - 1;
            draw_x = x + i;

            u32 v = (draw_y + draw_x * h) * 3;

            fb_8[v + 2] = color[j * w + i].ch.red << 3;    //((data >> 11) & 0x1F) << 3;
            fb_8[v + 1] = color[j * w + i].ch.green << 2;  //((data >> 5) & 0x3F) << 2;
            fb_8[v] = color[j * w + i].ch.blue << 3;   //(data & 0x1F) << 3;
        }
    }

}

void flush_cb_3ds_btm(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    flush_cb_3ds(GFX_BOTTOM, disp, area, color_p);
}

void flush_cb_3ds_top(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    flush_cb_3ds(GFX_TOP, disp, area, color_p);
}

lv_disp_t *display_init(gfxScreen_t gfx_scr, lv_disp_draw_buf_t *draw_buf, lv_color_t *buf1, lv_disp_drv_t *disp_drv)
{
    lv_disp_drv_init(disp_drv);
    
    switch (gfx_scr)
    {
        case GFX_TOP:
            lv_disp_draw_buf_init(draw_buf, buf1, NULL, WIDTH_TOP * HEIGHT_TOP);
            disp_drv->flush_cb = flush_cb_3ds_top;    /*Set your driver function*/
            disp_drv->hor_res = WIDTH_TOP;   /*Set the horizontal resolution of the display*/
            disp_drv->ver_res = HEIGHT_TOP;   /*Set the vertical resolution of the display*/
            break;

        case GFX_BOTTOM:
            lv_disp_draw_buf_init(draw_buf, buf1, NULL, WIDTH_BTM * HEIGHT_BTM);
            disp_drv->flush_cb = flush_cb_3ds_btm;    /*Set your driver function*/
            disp_drv->hor_res = WIDTH_BTM;   /*Set the horizontal resolution of the display*/
            disp_drv->ver_res = HEIGHT_BTM;   /*Set the vertical resolution of the display*/
            break;
        
        default:
            break;
    }

    disp_drv->draw_buf = draw_buf;
    disp_drv->direct_mode = 1;
    return lv_disp_drv_register(disp_drv);;
}