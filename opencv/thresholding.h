#pragma once
#include "opencv2/opencv.hpp"

void threshold_img(cv::Mat imgscr, cv::Mat imgdst, std::vector<float> range, int replace_value);