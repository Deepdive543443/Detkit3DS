#ifndef WIDGET_H
#define WIDGET_H

#include "sections.h"

// Virtual Button
typedef enum {
    BTN_A,
    BTN_B,
    BTN_X,
    BTN_Y,
    BTN_L,
    BTN_R,
    NUM_BTN
} Button;

void create_btm_btn_container();
void add_btm_btn(Button key, void *callback, lv_coord_t width, const char *label);
void remove_virtual_btn(Button key);
void create_LR();
void remove_LR();
void button_init();
void widgets_init();

#endif  // WIDGET_H