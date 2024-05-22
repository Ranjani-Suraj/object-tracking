//circulatiry, bounding rect, 
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "analysis.h"

using namespace cv;
using namespace std;

int num = 0;


vector<float> centre_mass(String path, vector<vector<Point>>contours, char choice, int* num_particles){
    
    //namedWindow("CoM");
    Mat img = imread(path);
    if (img.empty()) {
        return {-1};
    }
    vector<float> centreX(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<float> centreY(contours.size());
    Moments m;
    try {
        vector<Moments> mu(contours.size());

        for (size_t i = 0; i < contours.size(); i++)
        {
            mu[i] = moments(contours[i]);
            vector<vector<Point>> contours_poly(contours.size());
            approxPolyDP(contours[i], contours_poly[i], 1, true);
            
            cout << contourArea(contours[i]) << " " << mu[i].m00 << endl;
            try{
                boundRect[i] = boundingRect(contours_poly[i]);
                if (boundRect[i].height < 80 || boundRect[i].width < 30 || boundRect[i].height > 300 || boundRect[i].width > 200) {
                    cout << "small" << contours.size() << i << endl;
                    continue;
                }
                
                centreX[i] = mu[i].m10 / mu[i].m00;
                centreY[i] = mu[i].m01 / mu[i].m00;
                circle(img, Point(centreX[i], centreY[i]), 5, Scalar(0,0,255));
                cout << "Center of mass: (" << centreX[i] << ", " << centreY[i] << ")" << endl;
                
                cout <<"-------------------!------------------"<< (*num_particles + 1 ) / 2 << endl;
            }
            catch (Exception& e) {
                cout << mu[i].m00 << endl;
                return { 0 };
            }
            drawContours(img, contours_poly, (int)i, Scalar(0, 255, 0), 1);
           
        }
        cout << *num_particles << endl;
        cv::imshow("analysis", img);
        cv::waitKey(0);
    }
    catch (Exception& e)
    {
        cout << e.msg << endl; // output exception message
        return { -1 };
    }
    
    // Display the result
    cv::imshow("analysis", img);
    cv::waitKey(0);
    //cv::destroyWindow("CoM");
    if (choice == 'x')
        return centreX;
    else if (choice == 'y')
        return centreY;
    return {0};
}

vector<vector<float>> perimeter_area_circularity(vector<vector<Point>> contours) {
    //namedWindow("perimeter and area");
    vector<float> perimeter(contours.size());
    vector<float> area(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<float> circularity(contours.size());

    for (size_t i = 0; i < contours.size(); i++)
    {   
        vector<vector<Point>> contours_poly(contours.size());
        approxPolyDP(contours[i], contours_poly[i], 1, true);
        boundRect[i] = boundingRect(contours_poly[i]);
        if (boundRect[i].height < 80 || boundRect[i].width < 30 || boundRect[i].height > 300 || boundRect[i].width > 200) {
            perimeter[i] = arcLength(contours[i], true);
            area[i] = contourArea(contours[i]);
            circularity[i] = 4 * CV_PI * area[i] / (perimeter[i] * perimeter[i]);
            cout << "---------------------------" << perimeter[i] << " " << area[i] << " " << circularity[i] << endl;
        }
        else {
            cout << "empty" << contourArea(contours[i]) << endl;
        }
        
    }
    return { perimeter, area, circularity };
}

vector<vector<Point>> bounding_rect_details(String path, vector<vector<Point>> contours, char side) {
    Mat img = imread(path);
    //namedWindow("bounding rect");
    vector<vector<Point>> contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<vector<Point>> line_points(contours.size());
    int count = 0;
    for (size_t i = 0; i < contours.size(); i++)
    {
        try
        {
            //approximates a bounding polynomial based on contours
            
            approxPolyDP(contours[i], contours_poly[i], 1, true);
            //estimates a bounding rectangle based on the result of the approximation above
            boundRect[i] = boundingRect(contours_poly[i]);
            if (boundRect[i].height < 80 || boundRect[i].width < 30 || boundRect[i].height > 300 || boundRect[i].width > 200) {
                cout << "small" << contours.size() << i << endl;
                continue;
            }
            Point start, end;
            if (side == 'l')
            {
                //left line
                start = boundRect[i].tl();
                end = boundRect[i].tl() + Point(0, boundRect[i].height);
                drawContours(img, contours_poly, (int)i, Scalar(120, 0, 120), 1);
                rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(120, 120, 120), 1);
                line(img, start, end, Scalar(0, 255, 0), 2);
                count++;

            }
            //right line
            else if (side == 'r') {
                start = boundRect[i].tl() + Point(boundRect[i].width, 0);
                end = start + Point(0, boundRect[i].height);
                drawContours(img, contours_poly, (int)i, Scalar(120, 0, 120), 1);
                rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(120, 120, 120), 1);
                count++;
                line(img, start, end, Scalar(0, 255, 0));
            }
            else if (side == 'b') {
                //bottom
                start = boundRect[i].br();
                end = start - Point(boundRect[i].width, 0);
                drawContours(img, contours_poly, (int)i, Scalar(120, 0, 120), 1);
                rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(120, 120, 120), 1);
                count++;
                line(img, start, end, Scalar(0, 255, 0));
            }
            else if (side == 't')
            {
                start = boundRect[i].br() - Point(0, abs(boundRect[i].height));
                end = start - Point(boundRect[i].width, 0);
                drawContours(img, contours_poly, (int)i, Scalar(120, 0, 120), 1);
                rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(120, 120, 120), 1);
                count++;
                line(img, start, end, Scalar(0, 255, 0));
            }
            else
                continue;
            cout << "    =============================" << count << endl;
            line_points[(int)((count + 1) / 2)] = { start, end };
            num = (int)((count + 1) / 2);
            
        }
        catch (Exception& e)
        {
            cout << "either approxpoly, contour area, or min circle, have failed" << endl; // output exception message
        }
        
        cout << "rect size " << side << boundRect[i].size() << " " << contourArea(contours[i]) << " " << count << endl;
        
    }
    cv::imshow("analysis", img);
    cv::waitKey(0);
    return line_points;
}


analyses particle_analysis(String path, bool num, bool cmx, bool cmy , bool perimeter, bool area , bool rect_t , bool rect_b , bool rect_l , bool rect_r ) 
    {
    analyses image_details;
    Mat img = imread(path);
    if (img.empty()) {
        return image_details;
    }
    //Mat imgThresholded;
    
    Mat imgHSV;
    cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
    Mat imgThresholded;
    GaussianBlur(imgHSV, imgHSV, Size(7, 7), 0, 0);

    threshold(imgHSV, imgThresholded, 60, 255, THRESH_BINARY);

    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1));
    dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1));

    //erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 4);
    //dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 4);
    
    //threshold(img, imgThresholded, 58, 255, THRESH_BINARY);
    cv::imshow("analysis", imgThresholded);
    cv::waitKey(0);
    Moments m;
    vector<vector<Point> > contours;
    try {
        Mat canny_output;
        Canny(imgThresholded, canny_output, 60, 255, 3);
        
        findContours(canny_output, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
        vector<vector<Point>> contours_poly(contours.size());

        size_t i = 0;
        for (i = 0; i < contours.size(); i++)
        {
            approxPolyDP(contours[i], contours_poly[i], 1, true);
            drawContours(img, contours_poly, (int)i, Scalar(0, 0, 255), 1);
        }
        

    }
    catch (Exception& e)
    {
        cout << e.msg << endl; // output exception message
        return image_details;
    }
    //vector<vector<vector<float, Point>>> returns(8);
    int num_particles = 0;
    int* ptr = &num_particles;
    if(cmx)
        image_details.comx = centre_mass(path, contours, 'x', ptr);
    if (cmy)
        image_details.comy = centre_mass(path, contours, 'y', ptr);
    if(perimeter)
        image_details.perimeter = perimeter_area_circularity(contours)[0];
    if(area)
        image_details.area = perimeter_area_circularity(contours)[1];
    if(rect_b)
        image_details.bottom_line = bounding_rect_details(path, contours, 'b');
    if (rect_l)
        image_details.left_line = bounding_rect_details(path, contours, 'l');
    if (rect_r)
        image_details.right_line = bounding_rect_details(path, contours, 'r');
    if (rect_t)
        image_details.top_line = bounding_rect_details(path, contours, 't');
    if (num)
        image_details.num_particles = num;
    // Display the result
    cv::imshow("analysis", img);
    cv::waitKey(0);
    cv::destroyWindow("analysis");
    return image_details;
}
