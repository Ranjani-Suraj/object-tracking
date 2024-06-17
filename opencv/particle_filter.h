
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#pragma once

cv::Mat particle_filtering(cv::Mat, float = 3000 , float = 15000, bool keep = true, 
	int buffer = 5);

int area_filter(cv::Mat img, cv::Mat imgdst, float lower_range, float upper_range, bool keep = true, int buffer = 5);
