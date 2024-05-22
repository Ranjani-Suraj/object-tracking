// Ball_rolling.cpp : Defines the entry point for the application.
//


//#include <stdafx.h>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "testing.h"
#include "analysis.h"

namespace fs = std::filesystem;
using namespace cv;
using namespace std;

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
	
	Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	Mat imgThresholded;
	GaussianBlur(imgHSV, imgHSV, Size(7, 7), 0, 0);

	threshold(imgHSV, imgThresholded, 60, 255, THRESH_BINARY);
	
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1));
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1));
	/*
	//morphological closing (removes small holes from the foreground)
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	*/
	//inRange(imgThresholded, Scalar(0, 0, 60), Scalar(100, 255, 255), imgThresholded); //Threshold the image based on the given ranges

	//Calculate the moments of the thresholded image
	//Moments oMoments = moments(imgThresholded);
	//double dArea = oMoments.m00;

	// if the area <= 10000, I consider that the there are no objects in the image and it's because of the noise, the area is not zero 
	//if (dArea > 1000000)
	//{
	cout << "ball?" << endl;
	imshow(windowName, imgOriginal); //show the thresholded image
	waitKey(0); // Wait for any keystroke in the window
	cout << "boxes" << endl;
	//particle_analysis(path);
	//return 0;
	return boxes(imgThresholded, imgOriginal);



	/* }
	else {
		cout << "no ball in image" << endl;
		imshow(windowName, imgOriginal);
		waitKey(0);
	}
	
	return 0; */

}



int ball()
{
	// Read the image file
	String windowName = "ball rolling"; //Name of the window

	namedWindow(windowName); // Create a window

	//String path_root = "C:\\Users\\rojard\\Documents\\Ranjani Suraj\\ball rolling\\ImageSave with single ball\\IRLight\\LeftUMCam1\\Lane1\\";
	String path_root = "E:\\ImageSave With Apple Diff. Size\\IRLight\\TopUMCam1\\Lane1";
	//reading the image and making the corresponding contours and boxes for each file in the directory
	Mat image;
	String files[100]; int i = 0, flag = 0;
	cout << "next iter" << endl;
	vector<float>areas(100);
	for (const auto& entry : fs::directory_iterator(path_root)) {
		cout << "reading" << endl;
		flag = reading_image(entry.path().string());
		areas[i] = flag;
		if (flag == -1) {
			cout << "Image find failed." << endl;
		}
		i = i + 1;
	}

	//finding average area of the region enclosed by the contours
	sort(areas.begin(), areas.end());
	i = 1;
	float avg = 0.0;
	while (areas[areas.size() - i] >= 5000) {
		cout << "areas:" << areas[areas.size() - i] << endl;
		avg += areas[areas.size() - i];
		i += 1;
	}	
	cout << avg / (i-1) << endl;
	cout << i - 1 << endl;
	destroyWindow(windowName); //destroy the created window
	return 0;
}

int main(int argc, char** argv) {
	analyses img_details = particle_analysis("E:\\ImageSave With Apple Diff. Size\\IRLight\\TopUMCam1\\Lane1\\05-09-2024_09-51-47.920_65.png");
	//return 0;
	return ball();
}
