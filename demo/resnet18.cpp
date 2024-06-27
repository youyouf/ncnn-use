#include "net.h"
#include <algorithm>
#if defined(USE_NCNN_SIMPLEOCV)
#include "simpleocv.h"
#else
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif
#include <stdio.h>
#include <vector>


static int detect_resnet18(const cv::Mat& bgr, std::vector<float>& cls_scores)
{
    ncnn::Net resnet18;

    resnet18.opt.use_vulkan_compute = true; //使用vulkan计算加速

    //分别加载模型的参数和数据
    if (resnet18.load_param("models/resnet18.param"))
        exit(-1);
    if (resnet18.load_model("models/resnet18.bin"))
        exit(-1);
	//opencv读取图片是BGR格式，我们需要转换为RGB格式
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR2RGB, bgr.cols, bgr.rows, 224, 224);
    
    //图像归一标准化，以R通道为例（x/225-0.485）/0.229
    const float mean_vals[3] = {0.485f*255.f, 0.456f*255.f, 0.406f*255.f};
    const float norm_vals[3] = {1/0.229f/255.f, 1/0.224f/255.f, 1/0.225f/255.f};
    in.substract_mean_normalize(mean_vals, norm_vals);

    ncnn::Extractor ex = resnet18.create_extractor();
	
    //把图像数据放入input.1这个blob里
    ex.input("input.1", in);

    ncnn::Mat out;
    //提取出推理结果，推理结果存放在191这个blob里
    ex.extract("191", out);

    // manually call softmax on the fc output
    // convert result into probability
    // skip if your model already has softmax operation
    {
        ncnn::Layer* softmax = ncnn::create_layer("Softmax");

        ncnn::ParamDict pd;
        softmax->load_param(pd);

        softmax->forward_inplace(out, resnet18.opt);

        delete softmax;
    }

    out = out.reshape(out.w * out.h * out.c);

    cls_scores.resize(out.w);
    for (int j = 0; j < out.w; j++)
    {
        cls_scores[j] = out[j];
    }

    return 0;
}

static int print_topk(const std::vector<float>& cls_scores, int topk)
{
    // partial sort topk with index
    int size = cls_scores.size();
    std::vector<std::pair<float, int> > vec;
    vec.resize(size);
    for (int i = 0; i < size; i++)
    {
        vec[i] = std::make_pair(cls_scores[i], i);
    }

    std::partial_sort(vec.begin(), vec.begin() + topk, vec.end(),
                      std::greater<std::pair<float, int> >());

    // print topk and score
    for (int i = 0; i < topk; i++)
    {
        float score = vec[i].first;
        int index = vec[i].second;
        fprintf(stderr, "%d = %f\n", index, score);
    }

    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [imagepath]\n", argv[0]);
        return -1;
    }

    const char* imagepath = argv[1];
	
    //使用opencv读取图片
    cv::Mat m = cv::imread(imagepath, 1);
    if (m.empty())
    {
        fprintf(stderr, "cv::imread %s failed\n", imagepath);
        return -1;
    }

    std::vector<float> cls_scores;
    detect_resnet18(m, cls_scores);
	
    //打印得分前三的类别
    print_topk(cls_scores, 3);

    return 0;
}
