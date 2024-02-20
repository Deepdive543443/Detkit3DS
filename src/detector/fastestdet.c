#include "detector.h"


static BoxVec fastestdet_detect(unsigned char *pixels, int pixel_w, int pixel_h, void *self_ptr)
{
    Detector *self = (Detector *) self_ptr;
    
    // Resize
    ncnn_mat_t mat = ncnn_mat_from_pixels_resize(pixels, NCNN_MAT_PIXEL_BGR, pixel_w, pixel_h, pixel_w * 3, self->input_size, self->input_size, NULL);

    // Create extractor
    ncnn_mat_substract_mean_normalize(mat, self->mean_vals, self->norm_vals);
    ncnn_extractor_t ex = ncnn_extractor_create(self->net);
    ncnn_extractor_input(ex, "data", mat);

    ncnn_mat_t out_mat;
    ncnn_extractor_extract(ex, "output", &out_mat);
    ncnn_extractor_destroy(ex);

    ncnn_mat_destroy(mat);

    int c_step = ncnn_mat_get_cstep(out_mat);
    int out_h = ncnn_mat_get_h(out_mat);
    int out_w = ncnn_mat_get_w(out_mat);
    float *data_ptr = (float *) ncnn_mat_get_data(out_mat);

    BoxVec proposals;
    create_box_vector(&proposals, 50);

    for(int h = 0; h < out_h; h++)
    {
        float *w_ptr = &data_ptr[h * out_w];
        for(int w = 0; w < out_w; w++)
        {
            float obj_score = w_ptr[0];
            float max_cls_score = 0.0;
            int max_cls_idx = -1;

            for (int c = 0; c < 80; c++)
            {
                float cls_score = w_ptr[(c + 5) * c_step];
                if (cls_score > max_cls_score)
                {
                    max_cls_score = cls_score;
                    max_cls_idx = c;
                }
            }

            if (pow(max_cls_score, 0.4) * pow(obj_score, 0.6) > 0.65)
            {
                float x_offset = fast_tanh(w_ptr[c_step]);
                float y_offset = fast_tanh(w_ptr[c_step * 2]);
                float box_width = fast_sigmoid(w_ptr[c_step * 3]);
                float box_height = fast_sigmoid(w_ptr[c_step * 4]);
                float x_center = (w + x_offset) / out_w;
                float y_center = (h + y_offset) / out_h;

                BoxInfo info;
                info.x1 = (x_center - 0.5 * box_width)  * pixel_w;
                info.y1 = (y_center - 0.5 * box_height) * pixel_h;
                info.x2 = (x_center + 0.5 * box_width) * pixel_w;
                info.y2 = (y_center + 0.5 * box_height) * pixel_h;
                info.label = max_cls_idx;
                info.prob = obj_score;

                BoxVec_push_back(info, &proposals);  
            }
            w_ptr++;
        }
    }

    BoxVec_fit_size(&proposals);
    if (proposals.num_item > 2)
    {
        qsort_descent_inplace(&proposals, 0, proposals.num_item - 1);
    }

    int picked_box_idx[proposals.num_item];
    int num_picked = nms(&proposals, picked_box_idx, 0.65f);

    BoxVec objects;
    create_box_vector(&objects, num_picked);

    for (int i=0; i < num_picked; i++)
    {
        BoxInfo box = BoxVec_getItem(picked_box_idx[i], &proposals);

        box.x1 = fmaxf(fminf(box.x1, (float)(pixel_w - 1)), 0.f);
        box.y1 = fmaxf(fminf(box.y1, (float)(pixel_h - 1)), 0.f);
        box.x2 = fmaxf(fminf(box.x2, (float)(pixel_w - 1)), 0.f);
        box.y2 = fmaxf(fminf(box.y2, (float)(pixel_h - 1)), 0.f);

        BoxVec_push_back(box, &objects); 
    }


    // Clean up
    BoxVec_free(&proposals);
    ncnn_mat_destroy(out_mat);

    return objects;
}

Detector create_fastestdet(int input_size, const char* param, const char* bin)
{
    Detector fastestdet;

    fastestdet.net = ncnn_net_create();
    ncnn_net_load_param(fastestdet.net, param);
    ncnn_net_load_model(fastestdet.net, bin);

    fastestdet.input_size = input_size;
    fastestdet.mean_vals[0] = 0.0f;
    fastestdet.mean_vals[1] = 0.0f;
    fastestdet.mean_vals[2] = 0.0f;
    fastestdet.norm_vals[0] = 0.00392157f;
    fastestdet.norm_vals[1] = 0.00392157f;
    fastestdet.norm_vals[2] = 0.00392157f;

    fastestdet.detect = &fastestdet_detect;
    return fastestdet;
}