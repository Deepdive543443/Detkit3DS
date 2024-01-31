#pragma once
#include "float.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#include "c_api.h"


static const char* class_names[] = {
    "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
    "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
    "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
    "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
    "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
    "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
    "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
    "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
    "hair drier", "toothbrush"
};

static const int color_list[80][3] =
{
    //{255 ,255 ,255}, //bg
    {216 , 82 , 24},
    {236 ,176 , 31},
    {125 , 46 ,141},
    {118 ,171 , 47},
    { 76 ,189 ,237},
    {238 , 19 , 46},
    { 76 , 76 , 76},
    {153 ,153 ,153},
    {255 ,  0 ,  0},
    {255 ,127 ,  0},
    {190 ,190 ,  0},
    {  0 ,255 ,  0},
    {  0 ,  0 ,255},
    {170 ,  0 ,255},
    { 84 , 84 ,  0},
    { 84 ,170 ,  0},
    { 84 ,255 ,  0},
    {170 , 84 ,  0},
    {170 ,170 ,  0},
    {170 ,255 ,  0},
    {255 , 84 ,  0},
    {255 ,170 ,  0},
    {255 ,255 ,  0},
    {  0 , 84 ,127},
    {  0 ,170 ,127},
    {  0 ,255 ,127},
    { 84 ,  0 ,127},
    { 84 , 84 ,127},
    { 84 ,170 ,127},
    { 84 ,255 ,127},
    {170 ,  0 ,127},
    {170 , 84 ,127},
    {170 ,170 ,127},
    {170 ,255 ,127},
    {255 ,  0 ,127},
    {255 , 84 ,127},
    {255 ,170 ,127},
    {255 ,255 ,127},
    {  0 , 84 ,255},
    {  0 ,170 ,255},
    {  0 ,255 ,255},
    { 84 ,  0 ,255},
    { 84 , 84 ,255},
    { 84 ,170 ,255},
    { 84 ,255 ,255},
    {170 ,  0 ,255},
    {170 , 84 ,255},
    {170 ,170 ,255},
    {170 ,255 ,255},
    {255 ,  0 ,255},
    {255 , 84 ,255},
    {255 ,170 ,255},
    { 42 ,  0 ,  0},
    { 84 ,  0 ,  0},
    {127 ,  0 ,  0},
    {170 ,  0 ,  0},
    {212 ,  0 ,  0},
    {255 ,  0 ,  0},
    {  0 , 42 ,  0},
    {  0 , 84 ,  0},
    {  0 ,127 ,  0},
    {  0 ,170 ,  0},
    {  0 ,212 ,  0},
    {  0 ,255 ,  0},
    {  0 ,  0 , 42},
    {  0 ,  0 , 84},
    {  0 ,  0 ,127},
    {  0 ,  0 ,170},
    {  0 ,  0 ,212},
    {  0 ,  0 ,255},
    {  0 ,  0 ,  0},
    { 36 , 36 , 36},
    { 72 , 72 , 72},
    {109 ,109 ,109},
    {145 ,145 ,145},
    {182 ,182 ,182},
    {218 ,218 ,218},
    {  0 ,113 ,188},
    { 80 ,182 ,188},
    {127 ,127 ,  0},
};


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
 * Todo -- FastestDet's modules
 */

Detector create_fastestdet(int input_size, const char* param, const char* bin);
BoxVec fastestdet_detect(unsigned char *pixels, int pixel_w, int pixel_h, void *self_ptr);