
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#pragma once

cv::Mat particle_filtering(cv::Mat, float , float , int low_thresh= 60, int up_thresh = 255, bool keep = true);
