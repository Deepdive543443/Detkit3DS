#include "detector.h"


const char* class_names[] = {
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

const int color_list[80][3] =
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

void create_box_vector(BoxVec *box_vector, size_t capacity)
{
    // BoxVec box_vector;
    box_vector->self = &box_vector;
    box_vector->capacity = capacity;
    box_vector->num_item = 0;
    box_vector->data = (BoxInfo *) malloc(sizeof(BoxInfo) * capacity);
}

BoxInfo BoxVec_getItem(size_t index, void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    if (index < boxVec->num_item && index >= 0)
    {
        return boxVec->data[index];
    }
    else
    {
        printf("Index:%d out of range\n", index);
        return boxVec->data[boxVec->num_item - 1];
    }

}

BoxInfo BoxVec_pop(void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    BoxInfo empty_box;
    
    if(boxVec->num_item > 0)
    {
        empty_box = boxVec->data[boxVec->num_item - 1];
        boxVec->num_item--;
        return empty_box;
    }
    else
    {
        printf("No box in vector\n");
        return empty_box;
    }
}

BoxInfo BoxVec_remove(size_t index, void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    BoxInfo empty = {-1, -1, -1, -1, -1, -1};

    if (index < boxVec->num_item - 1 && index >= 0)
    {
        int num_to_copy = boxVec->num_item - index + 1;
        empty = boxVec->data[index];
        BoxInfo temp[num_to_copy];

        memcpy(&temp, &boxVec->data[index + 1], sizeof(BoxInfo) * num_to_copy);
        memset(&boxVec->data[index], 0.0, sizeof(BoxInfo) * num_to_copy + 1);
        memcpy(&boxVec->data[index], &temp, sizeof(BoxInfo) * num_to_copy);

        boxVec->num_item--;
        return empty;
    }
    else if (index == boxVec->num_item - 1)
    {
        return BoxVec_pop(self_ptr);
    }
    printf("Index:%d out of range\n", index);
    return empty;
}

void BoxVec_push_back(BoxInfo item, void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    if (boxVec->capacity == boxVec->num_item)
    {
        void *data_ptr = realloc(boxVec->data, sizeof(BoxInfo) * (boxVec->capacity + 20));
        if (data_ptr == NULL)
        {
            printf("Ran out of mem\n");
        }
        else
        {
            boxVec->data = (BoxInfo *) data_ptr;
            boxVec->data[boxVec->num_item] = item;
            boxVec->capacity += 20;
            boxVec->num_item++;
        }
    }
    else if (boxVec->capacity > boxVec->num_item)
    {
        boxVec->data[boxVec->num_item] = item;
        boxVec->num_item++;
    }
}

void BoxVec_insert(BoxInfo item, size_t index, void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    if (index == boxVec->num_item)
    {
        BoxVec_push_back(item, self_ptr);
        return;
    }

    if (boxVec->capacity == boxVec->num_item)
    {
        void *data_ptr = realloc(boxVec->data, sizeof(BoxInfo) * (boxVec->capacity + 20));
        if (data_ptr == NULL)
        {
            printf("Ran out of mem\n");
            return;
        }
        else
        {
            boxVec->data = (BoxInfo *) data_ptr;
            boxVec->capacity += 20;
        }
    }

    int num_to_copy = boxVec->num_item - index;
    BoxInfo temp[num_to_copy];

    memcpy(&temp, &boxVec->data[index], sizeof(BoxInfo) * num_to_copy);
    boxVec->data[index] = item;
    memcpy(&boxVec->data[index+1], &temp, sizeof(BoxInfo) * num_to_copy);
    boxVec->num_item++;
}

void BoxVec_fit_size(void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    void *data_ptr = realloc(boxVec->data, sizeof(BoxInfo) * (boxVec->num_item));
    if (data_ptr == NULL)
    {
        printf("Ran out of mem\n");
        return;
    }
    else
    {
        boxVec->data = (BoxInfo *) data_ptr;
        boxVec->capacity = boxVec->num_item;
    }
}

void BoxVec_free(void *self_ptr)
{
    BoxVec *boxVec = (BoxVec *) self_ptr;
    free(boxVec->data);
}


float fast_exp(float x)
{
    union {
        __uint32_t i;
        float f;
    } v;
    v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
    return v.f;
}

float fast_sigmoid(float x)
{
    return 1.0f / (1.0f + fast_exp(-x));
}

float fast_tanh(float x)
{
    return 2.f / (1.f + fast_exp(-2 * x)) - 1.f;
}

int activation_function_softmax_inplace(float *src, int length)
{
    float alpha = -FLT_MAX;
    for (int i = 0; i < length; ++i)
    {
        if (alpha < src[i]) alpha = src[i];
    }

    float denominator = 0.f;

    for (int i = 0; i < length; ++i)
    {
        src[i] = fast_exp(src[i] - alpha);
        denominator += src[i];
    }

    for (int i = 0; i < length; ++i)
    {
        src[i] /= denominator;
    }
    return 0;
}

void qsort_descent_inplace(BoxVec *objects, int left, int right)
{
    int i = left;
    int j = right;

    float p = BoxVec_getItem((int) (left + right) / 2, objects).prob;

    while (i <= j)
    {
        while(BoxVec_getItem(i, objects).prob > p)
            i++;

        while(BoxVec_getItem(j, objects).prob < p)
            j--;

        if (i <= j)
        {
            // swap
            BoxInfo temp = BoxVec_getItem(i, objects);
            memcpy(&objects->data[i], &objects->data[j], sizeof(BoxInfo));
            objects->data[j] = temp;

            i++;
            j--;
        }
    }

    // #pragma omp parallel sections
    {
        // #pragma omp section
        {
            if (left < j) qsort_descent_inplace(objects, left, j);
        }
        // #pragma omp section
        {
            if (i < right) qsort_descent_inplace(objects, i, right);
        }
    }
}

float intersection(BoxInfo *box1, BoxInfo *box2)
{
    float xA = fmaxf(box1->x1, box2->x1);
    float yA = fmaxf(box1->y1, box2->y1);
    float xB = fminf(box1->x2, box2->x2);
    float yB = fminf(box1->y2, box2->y2);

    return fmaxf(0, xB - xA) * fmaxf(0, yB - yA);
}

int nms(BoxVec *objects, int *picked_box_idx, float thresh)
{
    int num_picked = 0;
    float areas[objects->num_item];

    for (int i = 0; i < objects->num_item; i++)
    {
        BoxInfo box = BoxVec_getItem(i, objects);
        areas[i] = (box.x2 - box.x1) * (box.y2 - box.y1);
    }

    for (int i = 0; i < objects->num_item; i++)
    {
        BoxInfo *boxA = &objects->data[i];
        int keep = 1;

        for (int j=0; j < num_picked; j++)
        {
            BoxInfo *boxB = &objects->data[picked_box_idx[j]];

            float inter_area = intersection(boxA, boxB);
            float union_area = areas[i] + areas[picked_box_idx[j]] - inter_area;

            if (inter_area / union_area > thresh) keep = 0;
        }

        if (keep == 1)
        {
            picked_box_idx[num_picked] = i;
            num_picked++;
        }
    }

    return num_picked;
}

void draw_boxxes(unsigned char *pixels, int pixel_w, int pixel_h, BoxVec *objects)
{
    union 
    {
        struct
        {
            __uint8_t r;
            __uint8_t g;
            __uint8_t b;
            __uint8_t a;
        };
        __uint32_t rgba;
    } color;
    for (size_t i =0; i < objects->num_item; i++)
    {
        BoxInfo box = BoxVec_getItem(i, objects);
        color.r = color_list[i][0];
        color.g = color_list[i][1];
        color.b = color_list[i][2];
        color.a = 255;
        ncnn_draw_rectangle_c3(
            pixels, 
            pixel_w, 
            pixel_h, 
            box.x1, 
            box.y1, 
            box.x2 - box.x1, 
            box.y2 - box.y1, 
            color.rgba, 3
        );
        ncnn_draw_text_c3(
            pixels,
            pixel_w, 
            pixel_h,
            class_names[box.label],
            (int) box.x1 + 1, 
            (int) box.y1 + 1,
            7,
            (int) color.rgba
        );
    }
}

void destroy_detector(Detector *det)
{
    ncnn_net_destroy(det->net);
}