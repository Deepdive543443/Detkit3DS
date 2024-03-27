#ifndef DISPALY_H
#define DISPALY_H

#include "sections.h"

void       writePic2FrameBuf565(void *fb, lv_color_t *color, u16 x, u16 y, u16 w, u16 h);
void       flush_cb_3ds_btm(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void       flush_cb_3ds_top(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
lv_disp_t *display_init(gfxScreen_t gfx_scr);

#endif  // DISPALY_H