// Ball_rolling.cpp : Defines the entry point for the application.
//


//#include <stdafx.h>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace fs = std::filesystem;
using namespace cv;
using namespace std;



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

	int iLowH = 0;
	int iHighH = 255;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 200;
	int iHighV = 255;
	String windowName = "ball rolling";

	int iLastX = -1;
	int iLastY = -1;

	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(image.size(), CV_8UC3);;

	//bool object = false;
	Mat imgOriginal = image;
	//imshow(windowName, imgOriginal);
	//waitKey(100);

	Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	//imshow(windowName, imgHSV);
	Mat imgThresholded;
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image based on the given ranges

	//morphological opening (removes small objects from the foreground)
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	//morphological closing (removes small holes from the foreground)
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	waitKey(100);
	imshow(windowName, imgThresholded);
	//Calculate the moments of the thresholded image
	Moments oMoments = moments(imgThresholded);

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
	if (dArea > 10000)
	{
		cout << "ball" << endl;
		waitKey(0); // Wait for any keystroke in the window
		imshow(windowName, imgOriginal); //show the thresholded image

	}
	else {
		cout << "no ball in image" << endl;
	}

	return 0;

}

int main(int argc, char** argv)
{
	// Read the image file
	String windowName = "ball rolling"; //Name of the window

	namedWindow(windowName); // Create a window

	String path_root = "C:\\Users\\rojard\\Documents\\Ranjani Suraj\\ball rolling\\ImageSave with single ball\\IRLight\\LeftUMCam1\\Lane1\\";

	Mat image;
	String files[100]; int i = 0, flag = 0;
	//String path;

	//flag = reading_image(path_root + "01-16-2024_14-03-19.541_19.png");
	cout << "next iter" << endl;
	//path = path_root + "01-16-2024_14-03-19.541_19.png";

	for (const auto& entry : fs::directory_iterator(path_root)) {
		cout << entry.path() << endl;
		flag = reading_image(entry.path().string());
		if (flag == -1) {
			cout << "Image find failed." << endl;
		}
		i = i + 1;
	}

	destroyWindow(windowName); //destroy the created window

	return 0;
}