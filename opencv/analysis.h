#pragma once

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

struct analyses {
    std::vector<float> comx;
    std::vector<float> comy;
    std::vector<float> perimeter;
    std::vector<float> area;
    std::vector<float> circularity ;
    std::vector<float> angles;
    //std::vector<std::vector<cv::Point>> left_line ;
    //std::vector<std::vector<cv::Point>> right_line ;
    //std::vector<std::vector<cv::Point>> top_line ;
    //std::vector<std::vector<cv::Point>> bottom_line;
    std::vector<float> left_line;
    std::vector<float> right_line;
    std::vector<float> top_line;
    std::vector<float> bottom_line;
    int num_particles = 0;
};

enum sides{
    LEFT,
    TOP,
    RIGHT,
    BOTTOM
};

//std::vector<float> centre_mass(std::string, std::vector<std::vector<cv::Point>>);
//int particle_analysis(std::string);
std::vector<std::vector<float>> particle_analysis(std::string, int low_Area = 3000,
    int up_area = 15000, int low_thresh = 60, int up_thresh = 255 , bool num = true, bool cmx = true, bool cmy = true, 
    bool perimeter = true, bool area = true, bool circularity = true, bool rect_t = true, 
    bool rect_b = true, bool rect_l = true, bool rect_r = true, int buffer = 5);
//std::vector<std::vector<float>> perimeter_area(std::vector<std::vector<cv::Point>>);
std::vector<std::vector<float>> particle_analysis(cv::Mat, bool num = true, bool cmx = true, bool cmy = true, bool perimeter = true,
    bool area = true, bool circularity = true, bool rect_t = true, bool rect_b = true, bool rect_l = true, 
    bool rect_r = true, int buffer = 5);


