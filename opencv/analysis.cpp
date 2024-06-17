//circulatiry, bounding rect, 
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "analysis.h"
#include <cmath>
#include "particle_filter.h"
#include <opencv2/core/ocl.hpp>
#include "thresholding.h"
//#include <opencv2/core/ocl.hpp>

using namespace cv;
using namespace std;


analyses img_details;


//gets cmx and cmy
int centre_mass(vector<vector<Point>>contours) {
    
    vector<float> centreX(contours.size());
    vector<float> centreY(contours.size());
    vector<Moments> mu(contours.size());
    Moments m;
    try {
        
        for (size_t i = 0; i < contours.size(); i++)
        {
            
                mu[i] = moments(contours[i]);
                centreX[i] = mu[i].m10 / mu[i].m00;
                centreY[i] = mu[i].m01 / mu[i].m00;

        }
        //cv::imshow("analysis", img);
        //cv::waitKey(0);
    }
    catch (Exception& e)
    {
        std::cout << e.msg << endl; // output exception message
        return -1;
    }

    img_details.comx = centreX;
    img_details.comy = centreY;

    return 0;
}
//gets perimeter, area, anc circularity
vector<vector<float>> perimeter_area_circularity(vector<vector<Point>> contours) {
    vector<float> perimeter(contours.size());
    vector<float> area(contours.size());
    vector<float> circularity(contours.size());
    //vector<float> angles(contours.size());
    int x = 0;
    for (size_t i = 0; i < contours.size(); i++)
    {

        //Point pt1, pt2;
        perimeter[i] = arcLength(contours[i], true);
        area[i] = contourArea(contours[i]);
        circularity[i] = 4 * CV_PI * area[i] / (perimeter[i] * perimeter[i]);
        //std::cout << "---------------------------" << perimeter[i] << " " << area[i] << " " << circularity[i] << endl;
        //cv::drawContours(img, contours_poly, (int)i, Scalar(255, 100, 100));
        //angles[i] = 0;

        /*
        float angle = rect.angle;

        std::cout << "ANGLE OF ROTATION --------------" << angle << endl;
        angles[x] = angle;

        String text = to_string(x);
        putText(img, //target image
            text, //text
            vertices[0], //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            1.0,
            CV_RGB(100, 185, 100), //font color
            2);*/
        //x++;

    }
    //img_details.angles = angles;
    img_details.area = area;
    img_details.circularity = circularity;
    img_details.perimeter = perimeter;
    
    return { perimeter, area, circularity };
}
//gets bounding box left, right, top, or bottom
int bounding_rect_details(vector<vector<Point>> contours, vector<vector<Point>> contours_poly, bool rect_l, bool rect_t, bool rect_r, bool rect_b, int buffer=5) {
    //cout << rect_r << rect_t << rect_l << rect_b << endl;
    int size = contours.size();
    Rect boundRect;
    //vector<vector<Point>> line_points(size);

    //vector<vector<Point>> left(size);
    //vector<vector<Point>> right(size);
    //vector<vector<Point>> top(size);
    //vector<vector<Point>> bottom(size);
    vector<float> left(size);
    vector<float> right(size);
    vector<float> top(size);
    vector<float> bottom(size);
    for (size_t i = 0; i < size; i++)
    {
        try
        {
            boundRect = boundingRect(contours_poly[i]);
            Point tl = boundRect.tl(), br = boundRect.br();
            Point start, end;
            int height = boundRect.height, width = boundRect.width;
            if (rect_l)
            {
                //left line
                start = tl - Point(buffer, buffer);
                end = start + Point(0, height+buffer*2);
                //left[i] = { start, end };
                left[i] = start.x;
            }
            //right line
            if (rect_r) {
                start = tl + Point(width+ buffer, -1*buffer);
                end = start + Point(0, height+ buffer*2);
                right[i] = start.x;
            }  
            if (rect_b) {
                //bottom
                start = br + Point(buffer, buffer);
                end = start - Point(width+ buffer*2,0);
                bottom[i] = start.y;
            }
            if (rect_t)
            {
                start = br - Point(-buffer, abs(height) + buffer);
                end = start - Point(width + buffer*2, 0);
                top[i] = start.y;
            }
            
            //Mat img = imread(path);
            //line(img, img_details.top_line[i][0], img_details.top_line[i][1], Scalar(0, 255, 0), 2);;
            //drawContours(img, contours_poly, (int)i, Scalar(120, 0, 120), 1);
            //rectangle(img, boundRect[i].tl() - Point(5,5), boundRect[i].br() + Point(5, 5), Scalar(120, 120, 120), 1);

            //line(img, start, end, Scalar(0, 255, 0), 2);
           

        }
        catch (Exception& e)
        {
            std::cout << "either approxpoly, contour area, or min circle, have failed" << endl; // output exception message
        }
        
        //std::cout << "rect size " << side << boundRect[i].size() << " " << i << endl;
        
    }
    if (rect_l) {
        img_details.left_line = left;
        
    }
    if (rect_b) {
        img_details.bottom_line = bottom;
    }
    if (rect_r) {
        img_details.right_line = right;
    }
    if (rect_t){
        img_details.top_line = top;
    }
    
    //cv::imshow("analysis", img);
    //cv::waitKey(0);
    //cout << img_details.left_line[0][0] << endl;

    return 0;
}


vector<vector<float>> particle_analysis(String path, int low_area, int up_area, int low_thresh, int up_thresh,
    bool num, bool cmx, bool cmy , bool perimeter, bool area , bool circularity, bool rect_t , bool rect_b , bool rect_l , bool rect_r, int buffer)
//analyses particle_analysis(Mat imgThresholded, bool num, bool cmx, bool cmy, bool perimeter, bool area, bool circularity, bool angles, bool rect_t, bool rect_b, bool rect_l, bool rect_r)

    {
    //filtering since only path was passed. 
    //img = Mat();
    //Mat imgFiltered = particle_filtering(img, low_area, up_area, low_thresh, up_thresh, true);
    
    auto start = std::chrono::high_resolution_clock::now();
    cv::Mat img = cv::imread(path, 0);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "duration = " << duration.count() << " milliseconds reading" << endl;
    
    start = std::chrono::high_resolution_clock::now();
    threshold_img(img, img, { (float)low_thresh, (float)up_thresh }, 255);
    cv::Mat imgFiltered = particle_filtering(img, low_area, up_area, low_thresh, up_thresh);

    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "duration = " << duration.count() << " milliseconds filtering" << endl;
    vector<vector<Point>> contours;
    vector<vector<float>> results(10);
    start = std::chrono::high_resolution_clock::now();
    try {

        //cvtColor(imgThresholded, imgThresholded, COLOR_BGR2GRAY); //--------------------------------------------------------------------
        findContours(imgFiltered, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        //cout << "num " << contours.size() << endl;

        img_details.num_particles = contours.size();
        
        
        
        int index = 0;
        //calling required functions
        if (num) {
            results[index] = { (float)contours.size() };
            index++;
        }
        if(true)
        {
            //cout << "cm" << endl;
            centre_mass(contours);
            if (cmx) {
                results[index] = img_details.comx;
                index++;
            }
            if (cmy) {
                results[index] = img_details.comy;
                index++;
            }
        }
        if(perimeter || area || circularity)
        {
            //cout << "p, a, c" << endl;
            perimeter_area_circularity(contours);
            if (perimeter) {
                results[index] = img_details.perimeter;
                index++;
            }
            if (area) {
                results[index] = img_details.area;
                index++;
            }
            if (circularity) {
                results[index] = img_details.circularity;
                index++;
            }
        }

        if(rect_l || rect_t || rect_r || rect_b)
        {
            vector<vector<Point>> contours_poly(contours.size());

            for (size_t x = 0; x < contours.size(); x++)
            {
                approxPolyDP(contours[x], contours_poly[x], 1, true);
            }
            //cout << "rect" << endl;
            bounding_rect_details(contours, contours_poly, rect_l, rect_t, rect_r, rect_b, buffer);
            if (rect_t) {
                results[index] = img_details.top_line;
                index++;
            }
            if (rect_b) {
                results[index] = img_details.bottom_line;
                index++;
            }
            if (rect_l) {
                results[index] = img_details.left_line;
                index++;
            }
            if (rect_r) {
                results[index] = img_details.right_line;
                index++;
            }
        }

        //arranging them in terms of their cmx
        ///*
        float j = 0; vector<float> key(index);
        for (int l = 1; l < img_details.comx.size(); l++) {
            for (int k = 1; k < index; k++)
            {
                //cout << k << endl;
                key[k] = results[k][l];
            }
            j = l - 1;
            // Move elements of arr[0..i-1],
            // that are greater than key, 
            // to one position ahead of their
            // current position
            while (j >= 0 && results[1][j] > key[1]) {
                for (int k = 1; k < index; k++)
                    results[k][j + 1] = results[k][j];
                j = j - 1;
            }
            for (int k = 1; k < index; k++)
                results[k][j + 1] = key[k];
        }
        //*/ having them in form of one row per object
        vector<vector<float>> return_vals(contours.size());

        vector<float> temp(index);
        int ind = 0;
        for (int i = 0; i < img_details.num_particles; i++) {
            for (int j = 0; j < index; j++) {
                if (j == 0) {
                    temp[j] = i;
                }
                else
                    temp[j] = results[j][i];
            }
            return_vals[i] = temp;
        }

        //drawing onto the image to show 
        /*
        for (int i = 0; i < img_details.num_particles; i++) {
            circle(img, Point(results[1][i], results[2][i]), 5, Scalar(0, 0, 255));
        
            String text = to_string(i);
            putText(img, //target image
                text, //text
                Point(results[1][i], results[2][i]), //top-left position
                cv::FONT_HERSHEY_DUPLEX,
                1.0,
                CV_RGB(100, 185, 100), //font color
                2);
            
            //std::cout << "Center of mass: (" << centreX[i] << ", " << centreY[i] << ")" << endl;
            //line(img, Point(img_details.left_line[i], img_details.top_line[i]), Point(img_details.left_line[i], img_details.bottom_line[i]), Scalar(255, 255, 255), 2);;
            //rectangle(img, boundRect[i].tl() - Point(5,5), boundRect[i].br() + Point(5, 5), Scalar(120, 120, 120), 1);
            //drawContours(img, contours, -1, Scalar(128, 128, 128), 1);
        }
        imshow("analysis", img);
        waitKey(0);
        //*/

        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "duration = " << duration.count() << " milliseconds analysis" << "\n" << endl;
        return return_vals;

    }
    catch (Exception& e)
    {
        std::cout << e.msg << endl; // output exception message
        return {};
    }
}

vector<vector<float>> particle_analysis(Mat img, bool num, bool cmx, bool cmy, bool perimeter, bool area, 
    bool circularity, bool rect_t, bool rect_b, bool rect_l, bool rect_r, int buffer)
//analyses particle_analysis(Mat imgThresholded, bool num, bool cmx, bool cmy, bool perimeter, bool area, bool circularity, bool angles, bool rect_t, bool rect_b, bool rect_l, bool rect_r)
{

    //img must be grayscale or findcontours will fail
    vector<vector<Point>> contours;
    //cvtColor(imgFiltered, img, COLOR_BGR2GRAY);  
    vector<vector<float>> results(10);
    //cv::ocl::setUseOpenCL(false);
    if (img.empty()) {
        return {};
    }

    try {
        
        findContours(img, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

        int size = contours.size();
        vector<vector<Point>> contours_poly(size);
        img_details.num_particles = size;
        size_t i = 0;
        

        //calling required functions based on boolean values
        int index = 0;
        //calling required functions
        if (num) {
            results[index] = { (float)contours.size() };
            index++;
        }
        if (true)
        {
            //cout << "cm" << endl;
            //this is is a true block since we always need the cmx so that the objects can be arranged in order of left ot right
            centre_mass(contours);
            if (cmx) {
                results[index] = img_details.comx;
                index++;
            }
            if (cmy) {
                results[index] = img_details.comy;
                index++;
            }
        }
        if (perimeter || area || circularity)
        {
            //cout << "p, a, c" << endl;
            perimeter_area_circularity(contours);
            if (perimeter) {
                results[index] = img_details.perimeter;
                index++;
            }
            if (area) {
                results[index] = img_details.area;
                index++;
            }
            if (circularity) {
                results[index] = img_details.circularity;
                index++;
            }
        }

        if (rect_l || rect_t || rect_r || rect_b)
        {
            for (i = 0; i < size; i++)
            {
                approxPolyDP(contours[i], contours_poly[i], 1, true);
            }
            //cout << "rect" << endl;
            bounding_rect_details(contours, contours_poly, rect_l, rect_t, rect_r, rect_b, buffer);
            if (rect_t) {
                results[index] = img_details.top_line;
                index++;
            }
            if (rect_b) {
                results[index] = img_details.bottom_line;
                index++;
            }
            if (rect_l) {
                results[index] = img_details.left_line;
                index++;
            }
            if (rect_r) {
                results[index] = img_details.right_line;
                index++;
            }
        }

        //arranging them in terms of their cmx
        ///*
        float j = 0; vector<float> key(index);
        for (int l = 1; l < img_details.comx.size(); l++) {
            for (int k = 1; k < index; k++)
            {
                //cout << k << endl;
                key[k] = results[k][l];
            }
            j = l - 1;
            // Move elements of arr[0..i-1],
            // that are greater than key, 
            // to one position ahead of their
            // current position
            while (j >= 0 && results[1][j] > key[1]) {
                for (int k = 1; k < index; k++)
                    results[k][j + 1] = results[k][j];
                j = j - 1;
            }
            for (int k = 1; k < index; k++)
                results[k][j + 1] = key[k];
        }
        //*/ 
        // having them in form of one row per object
        vector<float> temp(index);
        vector<vector<float>> return_vals(contours.size());

        for (int i = 0; i < img_details.num_particles; i++) {
            for (int j = 0; j < index; j++) {
                if (j == 0) {
                    temp[j] = i;
                }
                else
                    temp[j] = results[j][i];
            }
            return_vals[i] = temp;
            //cout << "" << endl;
        }
        //drawing the centre of mass and labels for the objects on the image
        ///*
        for (int i = 0; i < img_details.num_particles; i++) {
            circle(img, Point(results[1][i], results[2][i]), 5, Scalar(0, 0, 255));

            String text = to_string(i); //+ " " + to_string(results[1][i]);
            putText(img, //target image
                text, //text
                Point(results[1][i], results[2][i]), //top-left position
                cv::FONT_HERSHEY_DUPLEX,
                1.0,
                CV_RGB(100, 185, 100), //font color
                2);

            //std::cout << "Center of mass: (" << centreX[i] << ", " << centreY[i] << ")" << endl;
            //line(img, Point(img_details.left_line[i], img_details.top_line[i]), Point(img_details.left_line[i], img_details.bottom_line[i]), Scalar(255, 255, 255), 2);;
            //rectangle(img, boundRect[i].tl() - Point(5,5), boundRect[i].br() + Point(5, 5), Scalar(120, 120, 120), 1);
            //drawContours(img, contours, -1, Scalar(128, 128, 128), 1);
        }
        imshow("analysis", img);
        waitKey(0);
        //*/
        return return_vals;
    }
    catch (Exception& e)
    {
        std::cout << e.msg << endl; // output exception message
        return {};
    }
}