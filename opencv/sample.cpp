// Ball_rolling.cpp : Defines the entry point for the application.
// 
//currently it takes 18 milliseconds to process one image

// find the difference between cmx between frames 
// add a trackbar for threshold and area bounding values
// doxygen to do code documentation
// document code
// 
//#include <stdafx.h>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "testing.h"
#include "analysis.h"
#include "particle_filter.h"
#include <Windows.h>
#include "Reading_JSON.h"
#include <opencv2/core/ocl.hpp>
#include "track.h"
#include "thresholding.h"

namespace fs = std::filesystem;
using namespace cv;
using namespace std;

using namespace std::chrono;

//function reads the image and returns the relevant contours and bounding boxes to find area
int reading_image(String path) {
	cv::ocl::setUseOpenCL(false);
	cout << path << endl;
	auto start = high_resolution_clock::now();
	Mat image = imread(path, 0);

	//particle_analysis(path);
	//return 0;
	//Mat image = imread(path, 0);

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	cout << "reading time " << duration.count() << endl;

	// Check for failure
	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	//String windowName = "ball rolling";

	//Mat imgOriginal = image;
	// threshold_img(image, image, { 50, 255 }, 255);
 
	start = high_resolution_clock::now();
	threshold_img(image, image, { 50, 255 }, 255);
	imshow("thresh", image);
	Mat filtered = image.clone();// = particle_filtering(image, 800, 10000);
	//imshow("filter", filtered);
	//waitKey(0);
	area_filter(image, filtered, 800, 10000);
	imshow("filter", filtered);
	waitKey(0);
	vector<vector<float>> results = particle_analysis(filtered);

	stop = high_resolution_clock::now();
	duration = duration_cast<milliseconds>(stop - start);
	std::cout << "analysis+filtering time " << duration.count() << "\n" << endl;
	//waitKey(0);
	///*
	cout << "num " << "cmx " << "cmy " << "perimeter " << "area " << "circularity " << "top y " << "bottom y " << "left x " << "right x " << endl;
	for (int i = 0; i < results.size(); i++) {
		for (int j = 0; j < results[i].size(); j++) {
			cout << results[i][j] << ", ";
		}
		cout << endl;
	}
	//*/
	return 0;

}



int ball()
{
	// Read the image file
	//String windowName = "ball rolling"; //Name of the window

	//namedWindow(windowName); // Create a window

	String path_root = "E:\\ImageSave With Apple Diff. Size\\IRLight\\TopUMCam1\\Lane1";
	path_root = "E:\\ImageSave\\ImageSave\\IRLight\\LeftUMCam1\\Lane1";
	//reading the image and making the corresponding contours and boxes for each file in the directory
	//Mat image;
	String files[100], path, pic_number = "", prev_pic_number; int i = 0, flag = 0, size = 0;
	std::cout << "next iter" << endl;
	//vector<float>areas(100);
	//goes through all the files in the root directory and stores their names in files[]
	for (const auto& entry : fs::directory_iterator(path_root)) {
		//cout << "reading" << endl;
		files[i] = entry.path().string();
		path = files[i];
		for (int j = path.length() - 5; j >= 0; j--) {
			if (path.at(j) == '_') {
				break;
			}
			pic_number = path.at(j) + pic_number;
		}

		if (i >= 1 && to_string(stoi(prev_pic_number) + 1).compare(pic_number) != 0) {
			std::cout << "JUMP - MISSING IMAGE " << pic_number << ", " << prev_pic_number << endl;
		}
		//reading_image(files[i]);
		//areas[i] = flag;
		if (flag == -1) {
			std::cout << "Image find failed." << endl;
		}
		i = i + 1;
		prev_pic_number = pic_number;
		pic_number = "";
	}
	size = i;
	i = 0;
	//calls reading_image for the next image, and goes back to previous image on clicking 'a'
	while (i < size - 1 && i >= 0) {
		//auto start = high_resolution_clock::now();
		reading_image(files[i]);
		//cout << files[i] << "\n" << files[i + 1] << endl;
		/*
		* Mat img = imread(files[i+1]).clone();
		threshold_img(imread(files[i+1], img, {50, 255}, 255), 
		Mat filtered1 = particle_filtering(imread(files[i+1], 0), 600, 15000);
		Mat filtered2 = particle_filtering(imread(files[i], 0), 600, 15000);
		int delCMX = diff_cmx(filtered1, filtered2);
		cout << delCMX << "---------------------" << endl;
		//*/

		//auto stop = high_resolution_clock::now();
		//auto duration = duration_cast<milliseconds>(stop - start);
		//cout << "analysis time " << duration.count() << endl;

		//waitKey(0);
		if (GetAsyncKeyState(65))
			i--;
		else if (GetAsyncKeyState(68))
			i++;
		else
		{
			i++;
		}
	}

	return 0;
}


int main(int argc, char** argv) {
	
	String path = ("E:\\ImageSave With Apple Diff. Size\\IRLight\\TopUMCam1\\Lane1\\05-09-2024_09-51-47.164_58.png");
	String path3 = "C:\\Users\\rojard\\Documents\\Ranjani Suraj\\ball rolling\\ImageSave with single ball\\WhiteLight\\LeftUCCam1\\Lane1\\01-16-2024_14-03-20.503_27.png";
	path = "E:\\ImageSave\\ImageSave\\IRLight\\LeftUMCam1\\Lane1\\05-31-2024_17-33-17.946_15.png";
	path3 = "E:\\ImageSave\\ImageSave\\IRLight\\LeftUMCam1\\Lane1\\05-31-2024_17-33-17.427_14.png";
	Mat img = imread(path, 0); // since this is the first opencv function call, it takes ~10ms
	Mat result = img.clone();
	imshow("analysis", img);
	waitKey(0);
	//cout << img.size() << endl;
	threshold_img(img, result, { 50, 255 }, 255);
	Mat filtered1 = particle_filtering(result, 600, 15000);
	vector<vector<float>> results = particle_analysis(filtered1);

    imshow("analysis", img);
	imshow("no", result);
	imshow("filtered", filtered1);
	waitKey(0);
	///*
   //vector<vector<float>> particle_values(results.size());

   cout << "num " << "cmx " << "cmy " << "perimeter " << "area " << "circularity " << "top y " << "bottom y " << "left x " << "right x " << endl;
   for (int i = 0; i < results.size(); i++) {
	   for (int j = 0; j < results[i].size(); j++) {
		   cout << results[i][j] <<", ";

	   }
	   cout << endl;
   }

   //*/

	//return 0;
	//gets values for thresholding and area from JSON based on camera type
	cout << "reading" << endl;
	//vector<float> value = parsing_values("LeftUCCam1");
	//cout << value[0] << endl;
	//Mat filtered = particle_filtering(img, value[2], value[3]); // ~12-13
	//analyses img_details = particle_analysis(filtered);
	//Mat filtered = particle_filtering(img, value[2], value[3]);  // even thoguh this doesnt do anything, running this before running analysis(path) redces runtime by half
	// return 0;
	
	auto start = high_resolution_clock::now();
	ball();
	//vector<vector<float>> results = particle_analysis(path, 600, 15000, 50, 255);
	//Mat filtered = particle_filtering(img, value[2], value[3]); // ~12-13
	
	//analyses img_details = particle_analysis(filtered);
	//filter them before sending them 
	Mat thresh1 = imread(path, 0).clone();
	threshold_img(imread(path, 0), thresh1, { 50, 255 }, 255);
	Mat thresh2 = imread(path3, 0).clone();
	threshold_img(imread(path3, 0), thresh2, { 50, 255 }, 255);
	filtered1 = particle_filtering(thresh1, 600, 15000);
	Mat filtered2 = particle_filtering(thresh2, 600, 15000);
	int delCMX = diff_cmx(filtered1, filtered2);
	cout << delCMX << "---------------------" << endl;
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	cout << "duration = " << duration.count() << " millisecs" << endl;
	
	
	
	return 0;
	/*

	path = ("E:\\ImageSave With Apple Diff. Size\\IRLight\\TopUMCam1\\Lane1\\05-09-2024_09-51-46.935_56.png");
	img = imread(path, 0);
	start = high_resolution_clock::now();
	filtered = particle_filtering(img, value[2], value[3], value[0], value[1]); // ~12-13
	//img_details = particle_analysis(filtered);

	//ball();
	//Mat im = imread(path);
	stop = high_resolution_clock::now();
	duration = duration_cast<milliseconds>(stop - start);
	cout << "duration = " << duration.count() << " milliseconds" << endl;
	//*/
	return 0;
	
	
}
