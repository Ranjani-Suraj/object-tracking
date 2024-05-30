// Ball_rolling.cpp : Defines the entry point for the application.
// 
//currently it takes 220 milliseconds to process one image


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

namespace fs = std::filesystem;
using namespace cv;
using namespace std;

using namespace std::chrono;

//function reads the image and returns the relevant contours and bounding boxes to find area
int reading_image(String path) {
	cout << path << endl;
	Mat image = imread(path);
	// Check for failure
	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	String windowName = "ball rolling";

	Mat imgOriginal = image;
	//imshow(windowName, imgOriginal);
	//waitKey(100);
	/*
	Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	Mat imgThresholded;
	GaussianBlur(imgHSV, imgHSV, Size(7, 7), 0, 0);

	threshold(imgHSV, imgThresholded, 60, 255, THRESH_BINARY);

	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 2);
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 2);
	*/
	cout << "ball?" << endl;
	//imshow(windowName, imgOriginal); //show the thresholded image
	//waitKey(0); // Wait for any keystroke in the window
	cout << "boxes" << endl;
	//particle_filering(path, 600, 15000);
	particle_analysis(path);
	//particle_analysis(imgThresholded);
	return 0;
	//return boxes(imgThresholded, imgOriginal);

}



int ball()
{
	// Read the image file
	String windowName = "ball rolling"; //Name of the window

	namedWindow(windowName); // Create a window

	String path_root = "E:\\ImageSave With Apple Diff. Size\\IRLight\\TopUMCam1\\Lane1";
	//reading the image and making the corresponding contours and boxes for each file in the directory
	Mat image;
	String files[100]; int i = 0, flag = 0, size = 0;
	cout << "next iter" << endl;
	vector<float>areas(100);
	for (const auto& entry : fs::directory_iterator(path_root)) {
		cout << "reading" << endl;
		files[i] = entry.path().string();
		areas[i] = flag;
		if (flag == -1) {
			cout << "Image find failed." << endl;
		}
		i = i + 1;
	}
	size = i;
	i = 0;
	while (i<size && i>=0) {
		reading_image(files[i]);
		if (GetKeyState('D'))
			i++;
		else if (GetKeyState('A'))
			i--;
	}

	destroyWindow(windowName); //destroy the created window
	return 0;
}

int main(int argc, char** argv) {
	
	String path = ("E:\\ImageSave With Apple Diff. Size\\IRLight\\TopUMCam1\\Lane1\\05-09-2024_09-51-47.386_60.png");
	//path = "C:\\Users\\rojard\\Documents\\Ranjani Suraj\\ball rolling\\ImageSave with single ball\\WhiteLight\\LeftUCCam1\\Lane1\\01-16-2024_14-03-20.503_27.png";
	Mat img = imread(path);
	
	
	//return ball();
	
	vector<float> value = parsing_values("LeftUCCam1");
	for (int i = 0; i < 4; i++) {
		cout << value[i] << endl;
	}
	//return 0;
	//Mat filtered = particle_filtering(img, value[2], value[3], value[0], value[1]);

	auto start = high_resolution_clock::now();
	Mat filtered = particle_filtering(img, 600, 15000, 60, 255);
	analyses img_details = particle_analysis(filtered, true, true, true, true, true, true, true, true, false, false, false);
	//analyses img_details = particle_analysis(path, value[0], value[1], value[2], value[3], true, true, true, true, true, true, true, true, false, false, false);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	imshow("hhh", filtered);
	waitKey(0);
	cout << "duration = " << duration.count() << " milliseconds" << endl;
	//analyses img_details = particle_analysis(filtered, true, true, true, true, true, true, true, true, false, false, false);
	//vector<float> angle = img_details.angles;
	vector<float> areas = img_details.area;
	for (int i = 0; i < img_details.num_particles; i++) {
		
		//cout << "angles!!!!!!!!!!!!!!!!" << angle[i] << endl;
 		cout << "area!!!!!!!!!!!!!!!!" << areas[i] << endl;
		cout << img_details.perimeter[i] << endl;
		cout << img_details.num_particles << endl;
		cout << img_details.comx[i] << endl;
		cout << img_details.comy[i] << endl;
		cout << img_details.top_line[i] << endl;
	}
	return 0;
}
