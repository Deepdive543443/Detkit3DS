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

typedef struct
{
    BoxInfo *data;
    size_t capacity;
    size_t num_item;
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

/**
 * -- FastestDet's modules
 */

Detector create_fastestdet(int input_size, const char* param, const char* bin);