#ifndef UI_H
#define UI_H

#include "sections.h"

typedef enum {
    LAYER_BACKGROUND,
    LAYER_STREAMING,
    LAYER_RESULT,
    LAYER_TABVIEW,
    LAYER_NUM
} UI_LAYER;

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

typedef struct {
    void (*onCreate)(void);
    void (*onEnter)(void);
    void (*update)(void);
    void (*onLeave)(void);
    void (*onDestroy)(void);
    UI_LAYER idx;
} UI_CALLBACKS;

extern UI_CALLBACKS g_ui_stack[LAYER_NUM];
extern int          g_num_ui_layer;

extern UI_CALLBACKS g_ui_background;
extern UI_CALLBACKS g_ui_stream;
extern UI_CALLBACKS g_ui_result;
extern UI_CALLBACKS g_ui_tabview;

void ui_layer_join(UI_LAYER layer); /*Add a layer to stack and run the init function*/
void ui_layer_pop();
void ui_cleanup();
int  add_res_depth16(const char *path, lv_img_dsc_t *res_buffer);
void dealloc_res(lv_img_dsc_t *res_buffer);

void create_btm_btn_container();
void add_btm_btn(Button key, void *callback, lv_coord_t width, const char *label);
void remove_virtual_btn(Button key);
void create_LR();
void remove_LR();
void button_init();
void ui_init();

#endif  // UI_H