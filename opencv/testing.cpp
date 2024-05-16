// you can find the size per pixel and put that as a constant so reference point is alr set
// you need to find what the shape is, so shape detection
// then you need, what? bounding box? hm


#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat src_gray;
int ithreshold = 100;
RNG rng(12345);

float thresh_callback(int, int);

int boxes(String path)
{
	//String path_root = "C:\\Users\\rojard\\Documents\\Ranjani Suraj\\ball rolling\\ImageSave with single ball\\IRLight\\LeftUMCam1\\Lane1\\";
	//String path = path_root + "01-16-2024_14-03-19.541_19.png";

	Mat src = imread(path);
	if (src.empty())
	{
		cout << "Could not open or find the image!\n" << endl;
		return -1;
	}

	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));

	const char* source_window = "Source";
	namedWindow(source_window);
	imshow(source_window, src);

	const int max_thresh = 255;
	/*
	createTrackbar("Canny thresh:", source_window, nullptr, max_thresh, thresh_callback);
	setTrackbarPos("Canny thresh:", source_window, ithreshold);
	*/
	float area = thresh_callback(0, 0);

	waitKey(0);
	destroyWindow(source_window);
	return area;
}

float thresh_callback(int, int)
{

	Mat canny_output;
	try
	{
		Canny(src_gray, canny_output, ithreshold, ithreshold * 2);
	}
	catch (Exception& e)
	{
		cout << e.msg << endl; // output exception message
	}

	vector<vector<Point> > contours;
	findContours(canny_output, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f>centers(contours.size());
	vector<float>radius(contours.size());
	
	for (size_t i = 0; i < contours.size(); i++)
	{
		try
		{
			approxPolyDP(contours[i], contours_poly[i], 3, true);
			boundRect[i] = boundingRect(contours_poly[i]);
			cout << "rect size" << boundRect[i].size() << endl;
			cout << contourArea(contours[i]) << endl;
			cout << radius[i] << 3.14*radius[i]*radius[i] <<endl;
			
			vector<Vec4i> hierarchy;
			Mat image_copy = src_gray.clone();
			drawContours(image_copy, contours, -1, Scalar(125, 255, 0), 2);
			imshow("Contours", image_copy);
			waitKey(0);
			minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
		}
		catch (Exception& e)
		{
			cout << e.msg << endl; // output exception message
		}

	}
	

	//Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	Mat drawing = src_gray;
	float rad = 0.0;
	for (size_t i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		drawContours(drawing, contours_poly, (int)i, color);
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2);
		circle(drawing, centers[i], (int)radius[i], color, 2);
		if (radius[i] > rad) {
			rad = radius[i];
		}
	}

	imshow("Contours", drawing);
	return 3.14 * rad*rad;
}