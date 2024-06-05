// Ball_rolling.cpp : Defines the entry point for the application.
// 
//currently it takes 18 milliseconds to process one image


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

namespace fs = std::filesystem;
using namespace cv;
using namespace std;

using namespace std::chrono;

//function reads the image and returns the relevant contours and bounding boxes to find area
int reading_image(String path) {
	cv::ocl::setUseOpenCL(false);
	cout << path << endl;
	Mat image = imread(path ,0);
	auto start = high_resolution_clock::now();

	//particle_analysis(path);
	//return 0;
	//Mat image = imread(path, 0);

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	//cout << "reading time " << duration.count() << endl;

	// Check for failure
	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	//String windowName = "ball rolling";

	//Mat imgOriginal = image;

	start = high_resolution_clock::now();
	
	Mat filtered = particle_filtering(image, 3000, 30000, 55, 255);
	particle_analysis(filtered);

	stop = high_resolution_clock::now();
	duration = duration_cast<milliseconds>(stop - start);
	cout << "analysis+filtering time " << duration.count() << "\n" << endl;
	//waitKey(0);
	return 0;

}



int ball()
{
	// Read the image file
	//String windowName = "ball rolling"; //Name of the window

	//namedWindow(windowName); // Create a window

	String path_root = "E:\\ImageSave With Apple Diff. Size\\IRLight\\TopUMCam1\\Lane1";
	//reading the image and making the corresponding contours and boxes for each file in the directory
	//Mat image;
	String files[100]; int i = 0, flag = 0, size = 0;
	cout << "next iter" << endl;
	vector<float>areas(100);
	//goes through all the files in the root directory and stores their names in files[]
	for (const auto& entry : fs::directory_iterator(path_root)) {
		//cout << "reading" << endl;
		files[i] = entry.path().string();
		//reading_image(files[i]);
		areas[i] = flag;
		if (flag == -1) {
			cout << "Image find failed." << endl;
		}
		i = i + 1;
	}
	size = i;
	i = 0;
	//calls reading_image for the next image, and goes back to previous image on clicking 'a'
	while (i<size && i>=0) {
		//auto start = high_resolution_clock::now();
		reading_image(files[i]);
		//auto stop = high_resolution_clock::now();
		//auto duration = duration_cast<milliseconds>(stop - start);
		//cout << "analysis time " << duration.count() << endl;
		
		waitKey(0);
		if (GetAsyncKeyState(65))
			i--;
		else if (GetAsyncKeyState(68))
			i++;
		else
		{
			i++;
		}
	}

	//destroyWindow(windowName); //destroy the created window
	return 0;
}

int main(int argc, char** argv) {
	
	String path = ("E:\\ImageSave With Apple Diff. Size\\IRLight\\TopUMCam1\\Lane1\\05-09-2024_09-51-46.835_42.png");
	//path = "C:\\Users\\rojard\\Documents\\Ranjani Suraj\\ball rolling\\ImageSave with single ball\\WhiteLight\\LeftUCCam1\\Lane1\\01-16-2024_14-03-20.503_27.png";
	Mat img = imread(path, 0); // since this is the first opencv function call, it takes ~10ms

	//cout << img.size() << endl;

	//imshow("analysis", gray[2]);
	//waitKey(0);
	//  return ball();
	// 
	//gets values for thresholding and area from JSON based on camera type
	cout << "reading" << endl;
	vector<float> value = parsing_values("LeftUCCam1");

	//Mat filtered = particle_filtering(img, value[2], value[3], value[0], value[1]);  // even thoguh this doesnt do anything, running this before running analysis(path) redces runtime by half

	Mat filtered = particle_filtering(img, value[2], value[3], value[0], value[1]); // ~12-13
	analyses img_details = particle_analysis(filtered);

	auto start = high_resolution_clock::now();

	vector<vector<float>> results = particle_analysis(path);
	//ball();
	//Mat im = imread(path);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "duration = " << duration.count() << " microseconds" << endl;
	///*
	vector<string> names = {"num", "cmx", "cmy", "perimeter", "area", "circularity", "top y", "bottom y", "left x", "right x"};
	for (int i = 0; i < results.size(); i++) {
		cout << endl << names[i] << endl;
		for (int j = 0; j < results[i].size(); j++) {
			//c{out << "angles" << angle[i] << endl;
			cout << results[i][j] <<", ";
		}
	}
	//return 0;
	//*/

	path = ("E:\\ImageSave With Apple Diff. Size\\IRLight\\TopUMCam1\\Lane1\\05-09-2024_09-51-46.935_56.png");
	img = imread(path, 0);
	start = high_resolution_clock::now();
	filtered = particle_filtering(img, value[2], value[3], value[0], value[1]); // ~12-13
	img_details = particle_analysis(filtered);

	//analyses img_details = particle_analysis(path);
	//ball();
	//Mat im = imread(path);
	stop = high_resolution_clock::now();
	duration = duration_cast<microseconds>(stop - start);
	cout << "duration = " << duration.count() << " microseconds" << endl;

	//prints output values from the struct
	//analyses img_details = particle_analysis(filtered);

	/*
	vector<float> angle = img_details.angles;

	vector<float> areas = img_details.area;
	for (int i = 0; i < img_details.num_particles; i++) {
		
		//cout << "angles" << angle[i] << endl;
 		cout << "area--------------->" << areas[i] << endl;
		cout << img_details.perimeter[i] << endl;
		cout << img_details.num_particles << endl;
		cout << img_details.comx[i] << endl;
		cout << img_details.comy[i] << endl;
		cout << img_details.top_line[i] << endl;
	}
	*/
	return 0;
	
}
