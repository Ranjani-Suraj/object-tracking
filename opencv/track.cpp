#include "analysis.h"
#include"particle_filter.h"
#include <opencv2/core/ocl.hpp>

//you can get fruit width from subtracting the y on the left and right bounding rect
//img, prev_img
bool missing_frame(std::string prev_img_path, std::string current_img_path, int cmxBwFrames) {
	//cv::ocl::setUseOpenCL(false);
	//you alse need whether its going left->righgt or left<-right
	
	auto start1 = std::chrono::high_resolution_clock::now();
	std::vector<std::vector<float>> previous_analysis = particle_analysis(prev_img_path, 5500, 15000, 60, 255, true, true, true, false, false, false, false, false, false, true, true);
	std::vector<std::vector<float>> current_analysis = particle_analysis(current_img_path, 5500, 15000, 60, 255, true, true, true, false, false, false, false, false, false, true, true);

	auto stop1 = std::chrono::high_resolution_clock::now();
	auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1);
	
	std::cout << "time to analyse " << duration2.count() << std::endl;
	std::vector<float> prev_cmx(previous_analysis.size());
	std::vector<float> current_cmx(current_analysis.size());
	///*
	//int i = 0;
	std::cout << "size" << prev_cmx.size() << current_cmx.size() << std::endl;
	
 	for (int i = 0; i < previous_analysis.size(); i++) {
		//std::cout << previous_analysis[i].size()  << std::endl;
		for (int j = 0; j < previous_analysis[i].size() ; j++) {
			prev_cmx[i] = previous_analysis[i][1];
			std::cout << "prev, " << previous_analysis[i][j] << std::endl;

		}
		std::cout << "----------------------------" << std::endl;
	}
	for (int i = 0; i < current_analysis.size(); i++) {
		//std::cout << previous_analysis[i].size()  << std::endl;
		for (int j = 0; j < current_analysis[i].size(); j++) {
 			current_cmx[i] = current_analysis[i][1];
			std::cout << "curr, " << current_analysis[i][j] << std::endl;

		}
		std::cout << "----------------------------" << std::endl;
	}
	//*/
	if (previous_analysis.size() == 0) {
		if (current_analysis.size() - previous_analysis.size() > 1) {
			return 1;
		}
		else
			prev_cmx = {};
	}
	else if(current_analysis.size() == 0)
		if (previous_analysis.size() - current_analysis.size() > 1) {
			return 1;
		}
		else
			current_cmx = {};
	else {
		if (previous_analysis.size() == current_analysis.size()) {
             std::cout  << "--------------------------------------------------__" << previous_analysis[0][1];
  			return current_analysis[0][1] - previous_analysis[0][1] > cmxBwFrames;
		}
		else if (previous_analysis.size() > current_analysis.size()) {
			//its leaving the screen 
			return previous_analysis[1][previous_analysis.size() - 1] > 1440 - cmxBwFrames - 50;
		}
		else if (previous_analysis.size() < current_analysis.size()) {
			//its entering the screen 
			return current_analysis[1][0] < cmxBwFrames + 50;
		}
	}
	return 0;
}

int diff_cmx(cv::Mat prev_img, cv::Mat current_img) {
	//all of this assumes that we are going LEFT TO RIGHT
	std::vector<std::vector<float>> prev_analysis = particle_analysis(prev_img, true, true, false, false, false, false, false, false, true, true);
	std::vector<std::vector<float>> curr_analysis = particle_analysis(current_img, true, true, false, false, false, false, false, false, true, true);
	int prev_size = prev_analysis.size();
	int curr_size = curr_analysis.size();
	int diff_in_cmx = 0;
	std::cout << prev_size << curr_size << std::endl;
	//if either prev or curr is empty,
	


	if (prev_size == 0 && curr_size == 0) {
		return 0;
	}
	//if prev is empty but curr is not, it will return the distance from the left border to the first element 
	else if (prev_size == 0) {
		int width = curr_analysis[curr_size - 1][1] - curr_analysis[curr_size - 1][2]; //find the size from cmx to the left boundary
		return curr_analysis[curr_size-1][2]; //since partial objects are removed at borders, we calculate the distance of the left border form the left edge
	}
	//if prev is not empty but curr is, it will return the distance of the first element from the right border
	else if (curr_size == 0) {
		return prev_img.size().width - prev_analysis[0][3];
	}
	//if they are NOT zero, then it happens normally. 
	//if they have the same size, it will directly compare the first elemtents' cmx
	if (prev_size == curr_size) {
		diff_in_cmx = abs(prev_analysis[0][1] - curr_analysis[0][1]);
	}
	//if prev has less objects, it will find the difference between the first prev and the second curr, since we are assuming that a new curr has been added
	else if (prev_size < curr_size) {
		diff_in_cmx = abs(prev_analysis[prev_size-1][1] - curr_analysis[prev_size][1]);
	}
	//same logic as before
	else {
		diff_in_cmx = abs(prev_analysis[1][1] - curr_analysis[0][1]);
	}
	return diff_in_cmx;
}