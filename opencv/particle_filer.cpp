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


Mat particle_filtering(Mat img, float lower_range, float upper_range, bool keep, int buffer) {

	vector<vector<Point>> contours;
    //img must be grayscale
    Mat imgThresholded = img.clone();
     
    ///*
    //start = std::chrono::high_resolution_clock::now();

    //cv::threshold(img, imgThresholded, low_thresh, up_thresh, THRESH_BINARY);
    
    //stop = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    //std::cout << "duration = " << duration.count() << " milliseconds to compile opencl and threshold" << endl;
    //*/
    auto start = std::chrono::high_resolution_clock::now();
    vector<Vec4i> hierarchy;
    cv::findContours(imgThresholded, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    
    //cout << contours.size() << endl;
    //stop = std::chrono::high_resolution_clock::now();
    //duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    //cout << "duration = " << duration.count() << " milliseconds to find contours" << endl;
    
    //Mat result(img.size().height, img.size().width, CV_8UC3, Scalar(0,0,0));

    int areaContour, flag = 0;;
    vector<int> contour_numbers(contours.size());
    //start = std::chrono::high_resolution_clock::now();
    int x = 0, width = 0;
    //assuming it is semi-circular, we can take 
    vector<int> corners(contours.size());
    cv::imshow("filter", imgThresholded);
    cv::waitKey(0);
    for (int i = 0; i < contours.size(); i++)
    { 
        
        ///*
        flag = 0;
        //checking if there is a hole within an object contour
        x = hierarchy[i][3];
        //if the current contour is inside another
        //*/
        areaContour = cv::contourArea(contours[i]);
        width = sqrt(areaContour / CV_PI)*4 + buffer;
        //cout << "width" << width << endl;

        if(x!=-1 && keep)
        {
            //if it is not a hole in a corner object, fill the hole
            if (corners[hierarchy[i][3]] == 0) {
                flag = 0;
            }
            else {
                flag = 1;
            }
            if (flag == 0 && x != -1) { //it has no children so it must be a hole
                if ((contourArea(contours[hierarchy[i][3]]) <= upper_range && contourArea(contours[hierarchy[i][3]]) >= lower_range))
                {

                    fillPoly(imgThresholded, contours[i], Scalar(255, 255, 255));

                    continue;
                }

            }
        }
        flag = 0;
        //*/
       
        
        if (areaContour >= lower_range && areaContour <= upper_range )
        {
            
            if (!keep)
            {
                fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
                continue;
            }

            for (int j = 0; j < contours[i].size(); j++) {
                
                //if a point on the contour is 100 pixels away from the edge, the contour will not touch the edge, since the contours are all less than 100 pixels large
                if (contours[i][j].x >= width && contours[i][j].y >= width && contours[i][j].y <= imgThresholded.rows - width - 1 && contours[i][j].x <= imgThresholded.cols - width - 1)
                {
                    break;
                }
                if (contours[i][j].x <= buffer
                    || contours[i][j].x >= imgThresholded.cols - buffer || contours[i][j].y <= buffer || 
                    contours[i][j].y >= imgThresholded.rows - buffer) {
                    //cout << "corner" << endl;
                    corners[i] = i;
                    fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
                    flag = 1;
                    //cout << "edge" << contours[i][j] << endl;
                    break;
                }
                
                //cout << contours[i][j] << endl;
            }

            ///*
            if (flag == 1) {
                flag = 0;
                continue;
            }
            //fillPoly(result, contours[i], Scalar(255, 255, 255));
            //*/

            //

        }
        else{// if (areaContour < lower_range) {
            if (keep)
            {

                fillPoly(imgThresholded, contours[i], Scalar(0, 0, 0));
            }
            else {
                for (int j = 0; j < contours[i].size(); j++) {
                    //if a point on the contour is 100 pixels away from the edge, the contour will not touch the edge, since the contours are all less than 100 pixels large
                    if (contours[i][j].x >= width && contours[i][j].y >= width && contours[i][j].y <= imgThresholded.rows - width && contours[i][j].x <= imgThresholded.cols - width
                        )
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
        corners[i] = 0;
        cv::imshow("filter", imgThresholded);
        //std::cout << i << endl;
        //cv::imshow("result", result);
        cv::waitKey(0);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "duration = " << duration.count() << " milliseconds to remove small particles" << endl;

    return imgThresholded;
}


int area_filter(cv::Mat img, cv::Mat imgdst, float lower_range, float upper_range, bool keep, int buffer) {

    cv::Mat imgThresholded;
    if (imgdst.empty()) {
        imgThresholded = img;
    }
    else {
        imgThresholded = imgdst;
    }
    //defining a list of hierarchies to store the parent contours of every contour. -1 if no parent

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    //finds contours and stores them in contours, along with their hierarchies
    try {
        cv::findContours(imgThresholded, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

        int areaContour, corner_flag = 0, parent_contour = 0, width = 0;
        std::vector<int> corners(contours.size());
        for (int i = 0; i < contours.size(); i++)
        {

            corner_flag = 0;
            parent_contour = hierarchy[i][3];
            areaContour = cv::contourArea(contours[i]);
            //assuming a level of circularity: the object is not larger than 4x the radius of a circle of same area as this contour
            width = sqrt(areaContour / CV_PI) * 4 + buffer;
            // if the parent contour exists, we check if the parent contour is a corner object, in which case we will not fill the hole
            if (parent_contour != -1 && keep)
            {
                //if it is not a hole in a corner object, fill the hole
                if (corners[hierarchy[i][3]] == 0) {
                    corner_flag = 0;
                }
                else {
                    corner_flag = 1;
                }
                if (corner_flag == 0 && parent_contour != -1) { //it has no children so it must be a hole
                    if ((cv::contourArea(contours[hierarchy[i][3]]) <= upper_range && cv::contourArea(contours[hierarchy[i][3]]) >= lower_range))
                    {

                        fillPoly(imgThresholded, contours[i], cv::Scalar(255, 255, 255));

                        continue;
                    }

                }
            }
            corner_flag = 0;
            //if it is in the specified range
            if (areaContour >= lower_range && areaContour <= upper_range)
            {

                if (!keep)
                {
                    fillPoly(imgThresholded, contours[i], cv::Scalar(0, 0, 0));
                    continue;
                }
                //iterating through the points of the contour and checking if any of them are (buffer) pixels away from an edge
                for (int j = 0; j < contours[i].size(); j++) {

                    //if a point on the contour is width pixels away from the edge, the contour will not touch the edge, since the contours are all less than 100 pixels large
                    if (contours[i][j].x >= width && contours[i][j].y >= width && contours[i][j].y <= imgThresholded.rows - width - 1 && contours[i][j].x <= imgThresholded.cols - width - 1)
                    {
                        break;
                    }
                    if (contours[i][j].x <= buffer
                        || contours[i][j].x >= imgThresholded.cols - buffer || contours[i][j].y <= buffer ||
                        contours[i][j].y >= imgThresholded.rows - buffer) {
                        corners[i] = i;
                        fillPoly(imgThresholded, contours[i], cv::Scalar(0, 0, 0));
                        corner_flag = 1;
                        break;
                    }

                }
                //resetting flag variable for next iteration
                if (corner_flag == 1) {
                    corner_flag = 0;
                    continue;
                }

            }
            else {// if (areaContour < lower_range) {
                if (keep)
                {
                    fillPoly(imgThresholded, contours[i], cv::Scalar(0, 0, 0));
                }
                else {
                    for (int j = 0; j < contours[i].size(); j++) {
                        //if a point on the contour is width pixels away from the edge, the contour will not touch the edge, since the contours are all less than 100 pixels large
                        if (contours[i][j].x >= width && contours[i][j].y >= width && contours[i][j].y <= imgThresholded.rows - width && contours[i][j].x <= imgThresholded.cols - width
                            )
                        {
                            break;
                        }
                        if (contours[i][j].x <= buffer
                            || contours[i][j].x >= imgThresholded.cols - buffer || contours[i][j].y <= buffer || contours[i][j].y >= imgThresholded.rows - buffer) {
                            fillPoly(imgThresholded, contours[i], cv::Scalar(0, 0, 0));
                            break;
                        }
                    }
                }
            }
            corners[i] = 0;
        }
        return 0;
    }
    catch (cv::Exception& e) {
        std::cout << e.msg << std::endl;
        return -1;
    }
    return -1;
}