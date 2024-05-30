// you can find the size per pixel and put that as a constant so reference point is alr set
// you need to find what the shape is, so shape detection
// then you need, what? bounding box? hm



//accuracy is still off slightly
//for some reason it takes a million key clicks to mke it switch pictures

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat src_gray;
int ithreshold = 200;

float thresh_callback(int, int, Mat);

int boxes(Mat src, Mat imgOriginal)
{
	
	if (src.empty())
	{
		cout << "Could not find the image!\n" << endl;
		return -1;
	}
	src_gray = src.clone();
	
	const char* source_window = "Source";
	namedWindow(source_window);
	
	try
	{
		imshow(source_window, src_gray);
	}
	catch (Exception& e)
	{
		cout << "could not show, line 51" << endl; // output exception message
	}

	
	//calls callback function to draw contours
	float area = thresh_callback(0, 0, imgOriginal);

	waitKey(0);
	destroyWindow(source_window);

	return area;
}

float thresh_callback(int, int, Mat imgOriginal)
{

	Mat canny_output;
	try
	{
		//edge detection on thres
		std::cout << "edge detect" << endl;
		dilate(src_gray, src_gray, getStructuringElement(MORPH_ELLIPSE, Size(1, 1)));

		Canny(src_gray, canny_output, 60, 255);
	}
	catch (Exception& e)
	{
		std::cout << "canny didnt work line 76" << endl; // output exception message
	}
	//based on edges, draw contours and store as vector 
	vector<vector<Point> > contours;
	namedWindow("canny");
	cv::imshow("canny", canny_output);
	findContours(canny_output, contours, RETR_LIST, CHAIN_APPROX_NONE); 
	
	vector<vector<Point>> contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f>centers(contours.size());
	vector<float>radius(contours.size());
	Moments oMoments;
	double dArea;
	int num_particles = 0;

	for (size_t i = 0; i < contours.size(); i++)
	{
		try
		{
			//approximates a bounding polynomial based on contours
			if (contourArea(contours[i]) < 500 || contourArea(contours[i]) > 50000) {
				std::cout << "small/big" << contours.size() << i << endl;
				//continue;
			}
			approxPolyDP(contours[i], contours_poly[i], 1, true);
			//estimates a bounding rectangle based on the result of the approximation above
			boundRect[i] = boundingRect(contours_poly[i]);
			oMoments = moments(contours[i]);
			dArea = oMoments.m00;
			std::cout << "rect size" << boundRect[i].size() << endl;
			std::cout << contourArea(contours[i]) << " " << dArea << endl;
			//minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
		}
		catch (Exception& e)
		{
			std::cout << "either approxpoly, contour area, or min circle, have failed" << endl; // output exception message
		}

	}
	

	Mat drawing = imgOriginal.clone();
	float max_area = 0.0;
	RotatedRect rect;
	//goes through all the found objects in the image
	for (size_t i = 0; i < contours.size(); i++)
	{
		//randomly chooses a colour for the drawn contours
		Scalar colour = Scalar(255, 0, 255);
		//draws the contours, bounding rectangle, and minimum enclosing circle onto the image
		if(contourArea(contours[i]) < 600 || contourArea(contours[i]) > 50000){
			std::cout << "no. contours " << contours.size() << " " << contourArea(contours[i]) << endl;
			continue;
		}
		cv::drawContours(drawing, contours_poly, (int)i, colour, 1);
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), colour, 1);
		num_particles++;
		rect = minAreaRect(contours_poly[i]);
		std::cout << "rotated rect sizes" << rect.size << endl;
		Point2f vertices[4];
		rect.points(vertices);

		if (contourArea(contours[i]) > max_area) {
			max_area = contourArea(contours[i]);
		}
	}

	cv::imshow("Contours", drawing);
	cv::drawContours(drawing, contours_poly, -1, Scalar(120,100,120), 1);
	num_particles = (num_particles+1)/2;
	std::cout << num_particles << "PARTICLES -----------------------------------------" << endl;
	
	
	return max_area;
}
