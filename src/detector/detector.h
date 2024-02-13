#pragma once
#include "float.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#include "c_api.h"


extern const char* class_names[];
extern const int color_list[80][3];


/**
 * @ https://stackoverflow.com/questions/4694401/how-to-replicate-vector-in-c
 *  -- Dynamic array of boxxes
 */

typedef struct
{
    float x1;
    float y1;
    float x2;
    float y2;
    
    float prob;
    int label;
} BoxInfo;

typedef BoxInfo (*getItem_func_ptr) (size_t index, void *self_ptr);
typedef BoxInfo (*pop_func_ptr) (void *self_ptr);
typedef BoxInfo (*remove_func_ptr) (size_t index, void *self_ptr);
typedef void (*push_back_func_ptr) (BoxInfo item, void *self_ptr);
typedef void (*insert_func_ptr) (BoxInfo item, size_t index, void *self_ptr);
typedef void (*fit_func_ptr) (void *self_ptr);
typedef void (*free_func_ptr) (void *self_ptr);

typedef struct
{
    void *self;
    BoxInfo *data;
    size_t capacity;
    size_t num_item;
    getItem_func_ptr getItem;
    pop_func_ptr pop;
    remove_func_ptr remove;
    push_back_func_ptr push_back;
    insert_func_ptr insert;
    fit_func_ptr fit;
    free_func_ptr free;
} BoxVec;

void create_box_vector(BoxVec *box_vector, size_t capacity);
BoxInfo BoxVec_getItem(size_t index, void *self_ptr);
BoxInfo BoxVec_pop(void *self_ptr);
BoxInfo BoxVec_remove(size_t index, void *self_ptr);
void BoxVec_push_back(BoxInfo item, void *self_ptr);
void BoxVec_insert(BoxInfo item, size_t index, void *self_ptr);
void BoxVec_free(void *self_ptr);
void BoxVec_fit_size(void *self_ptr);

/**
 * Detector modules
 */

typedef BoxVec (*detect_func_ptr) (unsigned char *pixels, int pixel_w, int pixel_h, void *self_ptr);

typedef struct
{
    // void *self;
    ncnn_net_t net;
    int input_size;
    float mean_vals[3];
    float norm_vals[3];
    detect_func_ptr detect;
} Detector;



/**
 * -- General function that share with all detector
 */

float fast_exp(float x);
float fast_sigmoid(float x);
float fast_tanh(float x);
int activation_function_softmax_inplace(float *src, int length);

void qsort_descent_inplace(BoxVec *objects, int left, int right);
float intersection(BoxInfo *box1, BoxInfo *box2);
int nms(BoxVec *objects, int *idx, float thresh);

void draw_boxxes(unsigned char *pixels, int pixel_w, int pixel_h, BoxVec *objects);
void destroy_detector(Detector *det);

/**
 * -- Nanodet's modules
 */

Detector create_nanodet(int input_size, const char* param, const char* bin);
BoxVec nanodet_detect(unsigned char *pixels, int pixel_w, int pixel_h, void *self_ptr);

/**
 * -- FastestDet's modules
 */

Detector create_fastestdet(int input_size, const char* param, const char* bin);
BoxVec fastestdet_detect(unsigned char *pixels, int pixel_w, int pixel_h, void *self_ptr);