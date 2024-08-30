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

typedef struct {
    void (*onCreate)(void);
    void (*onEnter)(void);
    void (*update)(void);
    void (*onLeave)(void);
    void (*onDestroy)(void);
    UI_LAYER idx;
} UI_CALLBACKS;

extern UI_CALLBACKS g_ui_stack[LAYER_NUM];
extern int         g_num_ui_layer;

extern UI_CALLBACKS g_ui_background;
extern UI_CALLBACKS g_ui_stream;
extern UI_CALLBACKS g_ui_result;
extern UI_CALLBACKS g_ui_tabview;


void ui_layer_join(UI_LAYER layer); /*Add a layer to stack and run the init function*/
void ui_layer_pop();
void ui_cleanup();
int  add_res_depth16(const char *path, lv_img_dsc_t *res_buffer);
void dealloc_res(lv_img_dsc_t *res_buffer);

#endif  // UI_H