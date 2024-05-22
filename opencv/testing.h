#pragma once

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>


float thresh_callback(int, int, cv::Mat);

int boxes(cv::Mat, cv::Mat);
