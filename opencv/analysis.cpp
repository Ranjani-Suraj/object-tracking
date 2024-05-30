//circulatiry, bounding rect, 
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "analysis.h"
#include <cmath>
#include "particle_filter.h"

using namespace cv;
using namespace std;

int num = 0;

analyses img_details;

int centre_mass(Mat img, vector<vector<Point>>contours, char choice) {
    
    if (img.empty()) {
        //return {-1};
    }
    vector<float> centreX(contours.size());
    vector<float> centreY(contours.size());
    vector<Moments> mu(contours.size());
    Moments m;
    try {
        
        for (size_t i = 0; i < contours.size(); i++)
        {
            
            mu[i] = moments(contours[i]);
            
            //std::cout << contourArea(contours[i]) << " " << mu[i].m00 << endl;
            try{
                centreX[i] = mu[i].m10 / mu[i].m00;
                centreY[i] = mu[i].m01 / mu[i].m00;
                
                //circle(img, Point(centreX[i], centreY[i]), 5, Scalar(0,0,255));
                /*
                String text = to_string(i);
                putText(img, //target image
                    text, //text
                    Point(centreX[i], centreY[i]), //top-left position
                    cv::FONT_HERSHEY_DUPLEX,
                    1.0,
                    CV_RGB(100, 185, 100), //font color
                    2);
                    
                std::cout << "Center of mass: (" << centreX[i] << ", " << centreY[i] << ")" << endl;
                */
            }
            catch (Exception& e) {
                //std::cout << mu[i].m00 << endl;
                return -1;
            }
            
        }
        //cv::imshow("analysis", img);
        //cv::waitKey(0);
    }
    catch (Exception& e)
    {
        std::cout << e.msg << endl; // output exception message
        return -1;
    }
    
    // Display the result
    img_details.comx = centreX;
    img_details.comy = centreY;
    /*
    if (choice == 'x')
    {
        return centreX;
    }
    else if (choice == 'y')
    {
        return centreY;
    }
    */
    return 0;
}

vector<vector<float>> perimeter_area_circularity(vector<vector<Point>> contours) {
    vector<float> perimeter(contours.size());
    vector<float> area(contours.size());
    vector<float> circularity(contours.size());
    vector<float> angles(contours.size());
    int x = 0;
    for (size_t i = 0; i < contours.size(); i++)
    {

        Point pt1, pt2;
        float c_area = contourArea(contours[i]);

        perimeter[i] = arcLength(contours[i], true);
        area[i] = c_area;
        circularity[i] = 4 * CV_PI * area[i] / (perimeter[i] * perimeter[i]);
        //std::cout << "---------------------------" << perimeter[x] << " " << area[x] << " " << circularity[x] << endl;
        //cv::drawContours(img, contours_poly, (int)i, Scalar(255, 100, 100));
        angles[i] = 0;

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
    img_details.angles = angles;
    img_details.area = area;
    img_details.circularity = circularity;
    img_details.perimeter = perimeter;
    
    return { perimeter, area, circularity, angles };
}

vector<vector<Point>> bounding_rect_details(String path, vector<vector<Point>> contours, vector<vector<Point>> contours_poly, int side, int buffer=5) {
    //Mat img = imread(path);
    vector<Rect> boundRect(contours.size());
    vector<vector<Point>> line_points(contours.size());

    for (size_t i = 0; i < contours.size(); i++)
    {
        try
        {
            boundRect[i] = boundingRect(contours_poly[i]);
            Point tl = boundRect[i].tl(), br = boundRect[i].br();
            Point start, end;
            int height = boundRect[i].height, width = boundRect[i].width;
            if (side == 0)
            {
                //left line
                start = tl - Point(buffer, buffer);
                end = start + Point(0, height+buffer*2);
            }
            //right line
            else if (side == 2) {
                start = tl + Point(width+ buffer, -1*buffer);
                end = start + Point(0, height+ buffer*2);
                
            }  
            else if (side == 3) {
                //bottom
                start = br + Point(buffer, buffer);
                end = start - Point(width+ buffer*2,0);
                
            }
            else if (side == 1)
            {
                start = br - Point(-buffer, abs(height) + buffer);
                end = start - Point(width + buffer*2, 0);
                
            }
            else
            {
                start = tl - Point(buffer, buffer);
                end = br + Point(buffer, buffer);
                
            }
            
            line_points[i] = { start, end };

            //line(img, line_points[i][0], line_points[i][1], Scalar(0, 255, 0), 2);;
            //drawContours(img, contours_poly, (int)i, Scalar(120, 0, 120), 1);
            //rectangle(img, boundRect[i].tl() - Point(5,5), boundRect[i].br() + Point(5, 5), Scalar(120, 120, 120), 1);

            //line(img, start, end, Scalar(0, 255, 0), 2);
            
            /*
            String text = to_string((count));
            putText(img, //target image
                text, //text
                start, //top-left position
                cv::FONT_HERSHEY_DUPLEX,
                1.0,
                CV_RGB(100, 185, 100), //font color
                2);
                /*/
            //std::cout << "    =============================" << (count) << endl;

        }
        catch (Exception& e)
        {
            std::cout << "either approxpoly, contour area, or min circle, have failed" << endl; // output exception message
        }
        
        //std::cout << "rect size " << side << boundRect[i].size() << " " << line_points[count-1][0] << " " << count << endl;
        
    }
    if (side == 0) {
        img_details.left_line = line_points;
        
    }
    else if (side == 3) {
        img_details.bottom_line = line_points;
    }
    else if (side == 2) {
        img_details.right_line = line_points;
    }
    else if (side == 1){
        img_details.top_line = line_points;
    }
    //cv::imshow("analysis", img);
    //cv::waitKey(0);
    return line_points;
}




analyses particle_analysis(String path, int low_thresh, int up_thresh, int low_area, int up_area, 
    bool num, bool cmx, bool cmy , bool perimeter, bool area , bool circularity, bool angles, bool rect_t , bool rect_b , bool rect_l , bool rect_r )
//analyses particle_analysis(Mat imgThresholded, bool num, bool cmx, bool cmy, bool perimeter, bool area, bool circularity, bool angles, bool rect_t, bool rect_b, bool rect_l, bool rect_r)

    {
    //cout << "analysis" << endl;
    analyses image_details;
    //namedWindow("analysis");
    Mat img = imread(path);
    //Mat imgThresholded;
    //auto start = std::chrono::high_resolution_clock::now();
    Mat imgThresholded = particle_filtering(img, low_area, up_area, low_thresh, up_thresh, true);
    //imshow("filtered", imgThresholded);
    //waitKey(0);
    //auto stop = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    //cout << "filtering time = " << duration.count() << " microseconds" << endl;
    Moments m;
    vector<vector<Point>> contours;
    
    try {
        //cvtColor(imgThresholded, imgThresholded, COLOR_BGR2GRAY);
        findContours(imgThresholded, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<vector<Point>> contours_poly(contours.size());
        img_details.num_particles = contours.size();
        size_t i = 0;
        for (i = 0; i < contours.size(); i++)
        {
            approxPolyDP(contours[i], contours_poly[i], 1, true);
            //drawContours(img, contours_poly, (int)i, Scalar(0, 0, 255), 1);
        }
    
    //vector<vector<vector<float, Point>>> returns(8);
        if(cmx || cmy)
            centre_mass(Mat(), contours, 'x');
        if(perimeter || area || circularity || angles)
            perimeter_area_circularity(contours);
        if(rect_l)
            image_details.left_line = bounding_rect_details(path, contours, contours_poly, LEFT);
        if (rect_t)
            image_details.top_line = bounding_rect_details(path, contours, contours_poly, TOP);
        if (rect_r)
            image_details.right_line = bounding_rect_details(path, contours, contours_poly, RIGHT);
        if (rect_b)
            image_details.bottom_line = bounding_rect_details(path, contours, contours_poly, BOTTOM);
        
    }
    catch (Exception& e)
    {
        std::cout << e.msg << endl; // output exception message
        return image_details;
    }
    //cv::destroyWindow("analysis");
    return img_details;
}

analyses particle_analysis(Mat imgFiltered, bool num, bool cmx, bool cmy, bool perimeter, bool area, 
    bool circularity, bool angles, bool rect_t, bool rect_b, bool rect_l, bool rect_r)
//analyses particle_analysis(Mat imgThresholded, bool num, bool cmx, bool cmy, bool perimeter, bool area, bool circularity, bool angles, bool rect_t, bool rect_b, bool rect_l, bool rect_r)

{
    //cout << "analysis" << endl;
    analyses image_details;
    //namedWindow("analysis");
    
    try {
        vector<vector<Point>> contours;
        //cvtColor(imgFiltered, imgFiltered, COLOR_BGR2GRAY);
        findContours(imgFiltered, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<vector<Point>> contours_poly(contours.size());
        img_details.num_particles = contours.size();
        size_t i = 0;
        for (i = 0; i < contours.size(); i++)
        {
            approxPolyDP(contours[i], contours_poly[i], 1, true);
            //drawContours(img, contours_poly, (int)i, Scalar(0, 0, 255), 1);
        }

        //vector<vector<vector<float, Point>>> returns(8);
        if (cmx || cmy)
            centre_mass(Mat(), contours, 'x');
       
        if (perimeter || area || circularity)
            perimeter_area_circularity( contours);
        if (rect_l)
            image_details.left_line = bounding_rect_details("path", contours, contours_poly, LEFT);
        if (rect_t)
            image_details.top_line = bounding_rect_details("path", contours, contours_poly, TOP);
        if (rect_r)
            image_details.right_line = bounding_rect_details("path", contours, contours_poly, RIGHT);
        if (rect_b)
            image_details.bottom_line = bounding_rect_details("path", contours, contours_poly, BOTTOM);

    }
    catch (Exception& e)
    {
        std::cout << e.msg << endl; // output exception message
        return image_details;
    }
    //cv::destroyWindow("analysis");
    return img_details;
}
