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
    std::vector<std::vector<cv::Point>> left_line ;
    std::vector<std::vector<cv::Point>> right_line ;
    std::vector<std::vector<cv::Point>> top_line ;
    std::vector<std::vector<cv::Point>> bottom_line;
    int num_particles;
};

enum sides{
    LEFT,
    TOP,
    RIGHT,
    BOTTOM
};

//std::vector<float> centre_mass(std::string, std::vector<std::vector<cv::Point>>);
//int particle_analysis(std::string);
analyses particle_analysis(std::string, int low_thresh = 60, int up_thresh = 255 , int low_Area = 600 , int up_area = 15000, bool num = true, bool cmx = true, bool cmy = true, bool perimeter = true,
    bool area = true, bool circularity = true, bool angles = true, bool rect_t = true, bool rect_b = true, bool rect_l = true, bool rect_r = true);
//std::vector<std::vector<float>> perimeter_area(std::vector<std::vector<cv::Point>>);
analyses particle_analysis(cv::Mat, bool num = true, bool cmx = true, bool cmy = true, bool perimeter = true,
    bool area = true, bool circularity = true, bool angles = true, bool rect_t = true, bool rect_b = true, bool rect_l = true, 
    bool rect_r = true);



