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
#include <opencv2\core\ocl.hpp >

using namespace cv;
using namespace std;


Mat particle_filtering(Mat img, float lower_range, float upper_range, int low_thresh, int up_thresh, bool keep, int buffer) {
    cv::ocl::setUseOpenCL(false);
    //auto start = std::chrono::high_resolution_clock::now();
	vector<vector<Point>> contours;
    Mat imgThresholded;

    //converting colour itself is taking about 13ms
    //cv::cvtColor(img, img, COLOR_BGR2GRAY, 1);
    //threshold(Mat(), Mat(), 0, 0, THRESH_BINARY);
    //thresholding is about 700microseconds
    auto start = std::chrono::high_resolution_clock::now();

    cv::threshold(img, imgThresholded, low_thresh, up_thresh, THRESH_BINARY);
    
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    //cout << "duration = " << duration.count() << " milliseconds to compile opencl and threshold" << endl;

    start = std::chrono::high_resolution_clock::now();

    //this takes about 1-2ms
    cv::findContours(imgThresholded, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    //cout << "duration = " << duration.count() << " milliseconds to find contours" << endl;

    //vector<vector<Point>> contours_poly(contours.size());
    int areaContour;
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < contours.size(); i++)
    { 
        //approxPolyDP(contours[i], contours_poly[i], 1, true);
        areaContour = cv::contourArea(contours[i]);
              
        if (areaContour >= lower_range && areaContour <= upper_range )
        {
            if (!keep)
            {
                fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
                continue;
            }

            for (int j = 0; j < contours[i].size(); j++) {
                //if a point on the contour is 100 pixels away from the edge, the contour will not touch the edge, since the contours are all less than 100 pixels large
                if (contours[i][j].x >= 120 && contours[i][j].y >= 120 && contours[i][j].y <= imgThresholded.rows - 120 && contours[i][j].x <= imgThresholded.cols - 120)
                {
                    //drawContours(imgThresholded, contours, i, cv::Scalar(128, 128, 0), 2);
                    //cout << "not edge" << contours[i][j] << endl;
                    break;
                }
                if (contours[i][j].x <= buffer
                    || contours[i][j].x >= imgThresholded.cols - buffer || contours[i][j].y <= buffer || 
                    contours[i][j].y >= imgThresholded.rows - buffer) {
                    fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
                
                    //cout << "edge" << contours[i][j] << endl;
                    break;
                }
                //cout << contours[i][j] << endl;
            }
            
            /*
            if (contours[i][0].x > 100 && contours[i][0].y > 100 && contours[i][0].y < img.rows - 100 && contours[i][0].x < img.cols - 100)
                continue;
            Rect boundRect = boundingRect(contours_poly[i]);
            Point br = boundRect.br(), tl = boundRect.tl();
            if (br.x <= 5 || br.x >= img.cols - 6 || tl.y <= 5 || tl.y >= img.rows - 6
                || br.y <= 5 || br.y >= img.rows - 6 || tl.x <= 5 || tl.x >= img.cols - 6) {
                //drawContours(result, contours_poly, i, cv::Scalar(128, 128, 0), 2);
                fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
                continue;
            }
            //drawContours(result, contours_poly, i, cv::Scalar(255, 255, 255), 1);
            //fillPoly(result, contours[i], Scalar(255, 255, 255));
            */
        }
        else{// if (areaContour < lower_range) {
            //drawContours(imgThresholded, contours_poly[i], i, cv::Scalar(0, 0, 0), 1);
            if (keep)
            {
                fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
            }
            else {
                for (int j = 0; j < contours[i].size(); j++) {
                    //if a point on the contour is 100 pixels away from the edge, the contour will not touch the edge, since the contours are all less than 100 pixels large
                    if (contours[i][j].x >= 120 && contours[i][j].y >= 120 && contours[i][j].y <= imgThresholded.rows - 120 && contours[i][j].x <= imgThresholded.cols - 120)
                    {
                        //drawContours(imgThresholded, contours, i, cv::Scalar(128, 128, 0), 2);
                        //cout << "not edge" << contours[i][j] << endl;
                        break;
                    }
                    if (contours[i][j].x <= buffer
                        || contours[i][j].x >= imgThresholded.cols - buffer || contours[i][j].y <= buffer || contours[i][j].y >= imgThresholded.rows - buffer) {
                        fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));

                        //cout << "edge" << contours[i][j] << endl;
                        break;
                    }
                }
            }
        }
    }

    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    //cout << "duration = " << duration.count() << " milliseconds to remove small particles" << endl;


    //cv::imshow("filter", imgThresholded);
    //cv::waitKey(0);
    return imgThresholded;
    //extra 4ms for print statements and calls and all
}