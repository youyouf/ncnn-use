#include <opencv2/opencv.hpp>
#include "net.h"
#include <algorithm>
 
int main() {
    ncnn::Net model;
    // 创建VideoCapture对象
    cv::VideoCapture capture(0); // 0代表电脑的默认摄像头
 
    // 检查摄像头是否成功打开
    if (!capture.isOpened()) {
        std::cerr << "ERROR: 摄像头无法打开" << std::endl;
        return -1;
    }
 
    cv::Mat frame; // 用于存储每一帧图像的变量
    while (true) {
        // 从摄像头读取新的帧
        capture >> frame;
        if (frame.empty()) {
            std::cerr << "ERROR: 没有捕捉到帧" << std::endl;
            break;
        }
 
        // 显示帧
        cv::imshow("摄像头", frame);

        //按‘q’ 退出循环
        if(cv::waitKey(1) == 'q')
            break;
    }
    //释放摄像头
    capture.release();
    // 按 'q' 退出循环s
    cv::destroyAllWindows();
    return 0;
}