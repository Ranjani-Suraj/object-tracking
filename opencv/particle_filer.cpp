//based on passed area, remove from an image. Also, makes ure that if its touching the edges it's not counted - edges of the image and edges of the conveyer  bely
//also, for boundingboxes, add a 5 pixel boundary
//also, make it so you can go between photos


//using JSON file take details and use them as inputs
//optimize?

#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "analysis.h"
#include <cmath>

using namespace cv;
using namespace std;

cv::Mat particle_filtering(Mat img, float lower_range, float upper_range, int low_thresh, int up_thresh, bool keep) {
	vector<vector<Point>> contours;
    //Mat img = imread(path);
    //namedWindow("filter");
    Mat imgThresholded;
    cv::cvtColor(img, img, COLOR_BGR2GRAY);
    threshold(img, imgThresholded, low_thresh , up_thresh, THRESH_BINARY);
  	cv::findContours(imgThresholded, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    //vector<vector<Point>> borders(contours.size());
    vector<vector<Point>> contours_poly(contours.size());
    //Mat result = Mat::zeros(img.size(), CV_8UC3);
    //result = imgThresholded.clone();
    int areaContour, flag = 0;

    for (int i = 0; i < contours.size(); i++)
    { 
        //approxPolyDP(contours[i], contours_poly[i], 1, true);
        areaContour = cv::contourArea(contours[i]);

        if (areaContour >= lower_range && areaContour <= upper_range )
        {
            if (!keep)
            {
                continue;
            }
            
            for (int j = 0; j < contours[i].size(); j++) {
                if (contours[i][j].x >= 100 && contours[i][j].y >= 100 && contours[i][j].y <= img.rows - 100 && contours[i][j].x <= img.cols - 100)
                {
                    //fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
                    break;
                }
                if (contours[i][j].x <= 0
                    || contours[i][j].x >= img.cols - 1 || contours[i][j].y <= 0 || contours[i][j].y >= img.rows - 1) {
                    fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
                    //drawContours(result, contours_poly, i, cv::Scalar(128, 128, 0), 2);
                    break;
                }
            }
            
            /*/
            if (contours[i][0].x > 50 && contours[i][0].y > 50 && contours[i][0].y < img.rows - 50 && contours[i][0].x < img.cols - 50)
                continue;
            Rect boundRect = boundingRect(contours_poly[i]);
            Point br = boundRect.br(), tl = boundRect.tl();
            if (br.x <= 0 || br.x >= img.cols - 1 || tl.y <= 0 || tl.y >= img.rows - 1
                || br.y <= 0 || br.y >= img.rows - 1 || tl.x <= 0 || tl.x >= img.cols - 1) {
                //drawContours(result, contours_poly, i, cv::Scalar(128, 128, 0), 2);
                fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
                continue;
            }*/
            //drawContours(result, contours_poly, i, cv::Scalar(255, 255, 255), 1);
            //fillPoly(result, contours[i], Scalar(255, 255, 255));
            
        }
        else{// if (areaContour < lower_range) {
            //drawContours(result, contours_poly, i, cv::Scalar(0, 0, 0), 1);

            fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
        }

    }

    
    //cv::cvtColor(result, result, COLOR_BGR2GRAY);
    //cv::imshow("filter", imgThresholded);
    //cv::imshow("asdb", result);
    //cv::waitKey(0);
    //cv::destroyAllWindows();
    //vector<vector<Point>> contours2;
    //cv::findContours(result, contours2, RETR_LIST, CHAIN_APPROX_SIMPLE);
    //std::cout << "current contours->" << contours2.size() << endl;
    return imgThresholded;
}