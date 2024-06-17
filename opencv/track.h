#pragma once
#include "analysis.h"

bool missing_frame(std::string prev_img_path, std::string current_img_path, int cmxBwFrames);
int diff_cmx(cv::Mat prev_img, cv::Mat current_img);