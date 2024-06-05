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
    bool num, bool cmx, bool cmy , bool perimeter, bool area , bool circularity, bool angles, bool rect_t , bool rect_b , bool rect_l , bool rect_r )
//analyses particle_analysis(Mat imgThresholded, bool num, bool cmx, bool cmy, bool perimeter, bool area, bool circularity, bool angles, bool rect_t, bool rect_b, bool rect_l, bool rect_r)

    {
    //cv::ocl::setUseOpenCL(false);
    //filtering since only path was passed. This takes about 25ms totally for both reading and filtering. the rest takes 1ms
    
    cv::Mat img;
    //img = Mat();
    //Mat imgFiltered = particle_filtering(img, low_area, up_area, low_thresh, up_thresh, true);
    auto start = std::chrono::high_resolution_clock::now();
    img = cv::imread(path, 0);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "duration = " << duration.count() << " milliseconds filtering" << endl;
    Mat imgFiltered = particle_filtering(img, low_area, up_area, low_thresh, up_thresh, true);

    //Moments m;
    vector<vector<Point>> contours;
    vector<vector<float>> results(10);
    try {

        //cvtColor(imgThresholded, imgThresholded, COLOR_BGR2GRAY); //--------------------------------------------------------------------
        findContours(imgFiltered, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<vector<Point>> contours_poly(contours.size());
        //cout << contours.size() << endl;
        img_details.num_particles = contours.size();
        
        size_t i = 0;
        for (i = 0; i < contours.size(); i++)
        {
            approxPolyDP(contours[i], contours_poly[i], 1, true);
        }
        int index = 0;
        //calling required functions
        if (num) {
            results[index] = { (float)contours.size() };
            index++;
        }
        if(cmx || cmy)
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
        if(perimeter || area || circularity || angles)
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
            //cout << "rect" << endl;
            bounding_rect_details(contours, contours_poly, rect_l, rect_t, rect_r, rect_b);
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

        /*        
        if (rect_t)
            image_details.top_line = bounding_rect_details(path, contours, contours_poly, TOP);
        if (rect_r)
            image_details.right_line = bounding_rect_details(path, contours, contours_poly, RIGHT);
        if (rect_b)
            image_details.bottom_line = bounding_rect_details(path, contours, contours_poly, BOTTOM);
        */
    }
    catch (Exception& e)
    {
        std::cout << e.msg << endl; // output exception message
        return results;
    }
    //cv::destroyWindow("analysis");
    /*
    for (int i = 0; i < img_details.num_particles; i++) {
        circle(img, Point(img_details.comx[i], img_details.comy[i]), 5, Scalar(0, 0, 255));
        
        String text = to_string(i);
        putText(img, //target image
            text, //text
            Point(img_details.comx[i], img_details.comy[i]), //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            1.0,
            CV_RGB(100, 185, 100), //font color
            2);
            
        //std::cout << "Center of mass: (" << centreX[i] << ", " << centreY[i] << ")" << endl;
        line(img, Point(img_details.left_line[i], img_details.top_line[i]), Point(img_details.left_line[i], img_details.bottom_line[i]), Scalar(255, 255, 255), 2);;
        //rectangle(img, boundRect[i].tl() - Point(5,5), boundRect[i].br() + Point(5, 5), Scalar(120, 120, 120), 1);
    }
    imshow("analisys", img);
    waitKey(0);
    //*/
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << "duration = " << duration.count() << " milliseconds analysis" << "\n" << endl;
    return results;
}

analyses particle_analysis(Mat imgFiltered, bool num, bool cmx, bool cmy, bool perimeter, bool area, 
    bool circularity, bool angles, bool rect_t, bool rect_b, bool rect_l, bool rect_r)
//analyses particle_analysis(Mat imgThresholded, bool num, bool cmx, bool cmy, bool perimeter, bool area, bool circularity, bool angles, bool rect_t, bool rect_b, bool rect_l, bool rect_r)
{
    auto start = std::chrono::high_resolution_clock::now();

    
    //cv::ocl::setUseOpenCL(false);
    try {
        vector<vector<Point>> contours;
        //cvtColor(imgFiltered, imgFiltered, COLOR_BGR2GRAY);  
        findContours(imgFiltered, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

        int size = contours.size();
        vector<vector<Point>> contours_poly(size);
        img_details.num_particles = size;
        size_t i = 0;
        for (i = 0; i < size; i++)
        {
            approxPolyDP(contours[i], contours_poly[i], 1, true);
            //drawContours(img, contours_poly, (int)i, Scalar(0, 0, 255), 1);
        }

        //calling required functions based on boolean values
        if (cmx || cmy)
            centre_mass(contours);
        if (perimeter || area || circularity)
            perimeter_area_circularity( contours);
        if (rect_l || rect_t || rect_r || rect_b)
            bounding_rect_details(contours, contours_poly, rect_l, rect_t, rect_r, rect_b);

        /*        if (rect_t)
                    image_details.top_line = bounding_rect_details(path, contours, contours_poly, TOP);
                if (rect_r)
                    image_details.right_line = bounding_rect_details(path, contours, contours_poly, RIGHT);
                if (rect_b)
                    image_details.bottom_line = bounding_rect_details(path, contours, contours_poly, BOTTOM);
                */
        cv::Point(0, 0);
    }
    catch (Exception& e)
    {
        std::cout << e.msg << endl; // output exception message
        return img_details;
    }
     /*
    for (int i = 0; i < img_details.num_particles; i++) {
        circle(imgFiltered, Point(img_details.comx[i], img_details.comy[i]), 5, Scalar(0, 0, 255));
        std::cout << "Center of mass: (" << img_details.comx[i] << ", " << img_details.comy[i] << ")" << endl;
        line(imgFiltered, img_details.right_line[i][0], img_details.right_line[i][1], Scalar(255, 255, 255), 2);
        //drawContours(img, contours_poly, (int)i, Scalar(120, 0, 120), 1);
        //rectangle(img, boundRect[i].tl() - Point(5,5), boundRect[i].br() + Point(5, 5), Scalar(120, 120, 120), 1);
    }
    imshow("analisys", imgFiltered);
    waitKey(0);
    */
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    //cout << "duration = " << duration.count() << " milliseconds analysis" << "\n" << endl;
    return img_details;
}
