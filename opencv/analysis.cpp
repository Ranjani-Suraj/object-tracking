//circulatiry, bounding rect, 
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "analysis.h"
#include <cmath>

using namespace cv;
using namespace std;

int num = 0;

analyses img_details;

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
        int x = 0;
        for (size_t i = 0; i < contours.size(); i++)
        {
            
            mu[i] = moments(contours[i]);
            vector<vector<Point>> contours_poly(contours.size());
            approxPolyDP(contours[i], contours_poly[i], 1, true);
             
            std::cout << contourArea(contours[i]) << " " << mu[i].m00 << endl;
            try{
                boundRect[i] = boundingRect(contours_poly[i]);
                //if (boundRect[i].height < 80 || boundRect[i].width < 30 || boundRect[i].height > 300 || boundRect[i].width > 200) {
                if(contourArea(contours[i]) < 600 || contourArea(contours[i]) > 50000){
                    std::cout << "small" << contours.size() << i << endl;
                    continue;
                }
                if (contourArea(contours[i]) == 0 || (i != 0 && (int)(contourArea(contours[i]) / contourArea(contours[i - 1])) == 1 || (float)(contourArea(contours[i - 1]) / contourArea(contours[i])) == 0)) {
                    continue;
                }
                if (x!=0 && abs(centreX[x-1]-(mu[i].m10 / mu[i].m00))<10 && abs(centreY[x-1] - (mu[i].m01 / mu[i].m00) < 10)) {
                    cout << "same  line 50" << endl;
                    continue;
                }
                centreX[x] = mu[i].m10 / mu[i].m00;
                centreY[x] = mu[i].m01 / mu[i].m00;
                
                circle(img, Point(centreX[x], centreY[x]), 5, Scalar(0,0,255));
                
                String text = to_string(x);
                putText(img, //target image
                    text, //text
                    Point(centreX[x], centreY[x]), //top-left position
                    cv::FONT_HERSHEY_DUPLEX,
                    1.0,
                    CV_RGB(100, 185, 100), //font color
                    2);
                    
                std::cout << "Center of mass: (" << centreX[x] << ", " << centreY[x] << ")" << endl;
                
                x++;
            }
            catch (Exception& e) {
                std::cout << mu[i].m00 << endl;
                return { 0 };
            }
            drawContours(img, contours_poly, (int)i, Scalar(0, 255, 0), 1);
           
        }
        std::cout << *num_particles << endl;
        cv::imshow("analysis", img);
        cv::waitKey(0);
    }
    catch (Exception& e)
    {
        std::cout << e.msg << endl; // output exception message
        return { -1 };
    }
    
    // Display the result
    cv::imshow("analysis", img);
    cv::waitKey(0);
    //cv::destroyWindow("CoM");
    if (choice == 'x')
    {
        img_details.comx = centreX;
        return centreX;
    }
    else if (choice == 'y')
    {
        img_details.comy = centreY;
        return centreY;
    }
    return {0};
}

vector<vector<float>> perimeter_area_circularity(Mat img, vector<vector<Point>> contours) {
    //namedWindow("perimeter and area");
    vector<float> perimeter(contours.size());
    vector<float> area(contours.size());
    vector<float> centreX(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<float> centreY(contours.size());
    vector<float> circularity(contours.size());
    vector<float> angles(contours.size());
    int x = 0;
    for (size_t i = 0; i < contours.size(); i++)
    {   
        vector<vector<Point>> contours_poly(contours.size());
        approxPolyDP(contours[i], contours_poly[i], 1, true);
        boundRect[i] = boundingRect(contours_poly[i]);
        RotatedRect rect = minAreaRect(contours_poly[i]);
        Point2f vertices[4];
        rect.points(vertices);
        Point pt1, pt2;
        //if (boundRect[i].height > 80 && boundRect[i].width > 30 && boundRect[i].height < 300 && boundRect[i].width < 200) {
        if (contourArea(contours[i]) < 600 || contourArea(contours[i]) > 50000) {
            continue;
        }
        else{
            if (contourArea(contours[i]) == 0 || (i != 0 && (int)(contourArea(contours[i]) / contourArea(contours[i-1])) == 1 || (float)(contourArea(contours[i-1]) / contourArea(contours[i])) == 0)) {
                continue;
            }
            vector<Moments> mu(contours.size());
           
            mu[i] = moments(contours[i]);
            if (x != 0 && abs(centreX[x - 1] - (mu[i].m10 / mu[i].m00)) < 10 && abs(centreY[x - 1] - (mu[i].m01 / mu[i].m00) < 20)) {
                cout << "same  line 136" << endl;
                continue;
            }
            centreX[x] = mu[i].m10 / mu[i].m00;
            centreY[x] = mu[i].m01 / mu[i].m00;
            circle(img, Point(centreX[x], centreY[x]), 10, Scalar(0, 0, 255));
            for (int k = 0; k < 4; k++) {
                line(img, vertices[k], vertices[(k + 1) % 4], Scalar(120, 50, 100));
            }
            perimeter[x] = arcLength(contours[i], true);
            area[x] = contourArea(contours[i]);
            circularity[x] = 4 * CV_PI * area[x] / (perimeter[x] * perimeter[x]);
            std::cout << "---------------------------" << perimeter[x] << " " << area[x] << " " << circularity[x] << endl;
            
            float angle = rect.angle;

            /*if (rect.size.height < rect.size.width) {
                int t = rect.size.width;
                rect.size.width = rect.size.height;
                rect.size.height = t;
                angle = rect.angle+90;   
            }
            if (abs(rect.angle-90) <= 10) {
                swap(rect.size.width, rect.size.height);
                angle = rect.angle;
            }
            else if (rect.angle > 90) {
                swap(rect.size.width, rect.size.height);
                angle = rect.angle;
            }*/

            std::cout << "ANGLE OF ROTATION --------------" << angle << endl;
            angles[x] = angle;
            namedWindow("eek");
            drawContours(img, contours_poly, (int)i, Scalar(255, 100, 100));
            
            
            String text = to_string(x);
            putText(img, //target image
                text, //text
                vertices[0], //top-left position
                cv::FONT_HERSHEY_DUPLEX,
                1.0,
                CV_RGB(100, 185, 100), //font color
                2);
            
            
            x++;
        }
        
    }
    img_details.angles = angles;
    img_details.area = area;
    img_details.circularity = circularity;
    img_details.perimeter = perimeter;
    imshow("eek", img);
    waitKey(0);
    return { perimeter, area, circularity, angles };
}

vector<vector<Point>> bounding_rect_details(String path, vector<vector<Point>> contours, char side) {
    Mat img = imread(path);
    //namedWindow("bounding rect");
    vector<vector<Point>> contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<vector<Point>> line_points(contours.size());
    int count = 0;
    vector<Moments> mu(contours.size());

    for (size_t i = 0; i < contours.size(); i++)
    {
        //char x = (char)i;
        try
        {
            //approximates a bounding polynomial based on contours
            
            approxPolyDP(contours[i], contours_poly[i], 1, true);
            RotatedRect rect = minAreaRect(contours_poly[i]);
            Point2f vertices[4];
            rect.points(vertices);

            
            
               
            //estimates a bounding rectangle based on the result of the approximation above
            boundRect[i] = boundingRect(contours_poly[i]);

            if (contourArea(contours[i])<500 || contourArea(contours[i])>50000) {
                std::cout << "small" << contours.size() << i << endl;
                continue;
            }
            if (contourArea(contours[i]) == 0 || (i != 0 && (int)(contourArea(contours[i]) / contourArea(contours[i - 1])) == 1)){// || (float)(contourArea(contours[i - 1]) / contourArea(contours[i])) == 0)) {
                cout << "same " << i << endl;
                continue;
            }
            //int flag = 0;
            mu[i] = moments(contours[i]);
            if (count != 0 && abs(img_details.comx[count - 1] - (mu[i].m10 / mu[i].m00)) < 10 && abs(img_details.comx[count - 1] - (mu[i].m01 / mu[i].m00) < 10)) {
                cout << "same  line 50" << endl;
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
                //line(img, vertices[0], vertices[1], Scalar(0, 255, 0), 2);
               
                line(img, start, end, Scalar(0, 255, 0), 2);
                start = vertices[0];
                end = vertices[1];

            }
            //right line
            else if (side == 'r') {
                start = boundRect[i].tl() + Point(boundRect[i].width, 0);
                end = start + Point(0, boundRect[i].height);
                drawContours(img, contours_poly, (int)i, Scalar(120, 0, 120), 1);
                rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(120, 120, 120), 1);
                //line(img, vertices[1], vertices[2], Scalar(0, 255, 0), 2);
                line(img, start, end, Scalar(0, 255, 0));
                start = vertices[2];
                end = vertices[3];               
                 
            }  
            else if (side == 'b') {
                //bottom
                start = boundRect[i].br();
                end = start - Point(boundRect[i].width, 0);
                drawContours(img, contours_poly, (int)i, Scalar(120, 0, 120), 1);
                rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(120, 120, 120), 1);
                //line(img, vertices[3], vertices[(4) % 4], Scalar(0, 255, 0), 2);
                
                line(img, start, end, Scalar(0, 255, 0));
                start = vertices[3];
                end = vertices[0];

            }
            else if (side == 't')
            {
                start = boundRect[i].br() - Point(0, abs(boundRect[i].height));
                end = start - Point(boundRect[i].width, 0);
                drawContours(img, contours_poly, (int)i, Scalar(120, 0, 120), 1);
                rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(120, 120, 120), 1);
                //line(img, vertices[2], vertices[(3) % 4], Scalar(0, 255, 0), 2);
                
                line(img, start, end, Scalar(0, 255, 0));
                start = vertices[1];
                end = vertices[2];
                

            }
            else
                continue;
            if (count>=1 && abs(line_points[count - 1][0].x - start.x) < 10 && abs(line_points[count - 1][0].y - start.y) < 20) {
                cout << "same line 291 " << start << line_points[count - 1][0] << endl;
                continue;
            }
            
            line(img, start, end, Scalar(0, 255, 0), 2);
            /*
            String text = to_string((count));
            putText(img, //target image
                text, //text
                start, //top-left position
                cv::FONT_HERSHEY_DUPLEX,
                1.0,
                CV_RGB(100, 185, 100), //font color
                2);
                */
            std::cout << "    =============================" << (count) << endl;
            line_points[(int)(count)] = { start, end };
            
            num = count;
            img_details.num_particles = num;
            count++;
        }
        catch (Exception& e)
        {
            std::cout << "either approxpoly, contour area, or min circle, have failed" << endl; // output exception message
        }
        
        std::cout << "rect size " << side << boundRect[i].size() << " " << line_points[count-1][0] << " " << count << endl;
        
    }
    if (side == 'l') {
        img_details.left_line = line_points;
        
    }
    else if (side == 'b') {
        img_details.bottom_line = line_points;
    }
    else if (side == 'r') {
        img_details.right_line = line_points;
    }
    else {
        img_details.top_line = line_points;
    }
    cv::imshow("analysis", img);
    cv::waitKey(0);
    return line_points;
}




analyses particle_analysis(String path, bool num, bool cmx, bool cmy , bool perimeter, bool area , bool circularity, bool angles, bool rect_t , bool rect_b , bool rect_l , bool rect_r ) 
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

    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 2);
    dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 2);
     
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
        std::cout << e.msg << endl; // output exception message
        return image_details;
    }
    //vector<vector<vector<float, Point>>> returns(8);
    int num_particles = 0;
    int* ptr = &num_particles;
    if(cmx)
        centre_mass(path, contours, 'x', ptr);
    if (cmy)
        image_details.comy = centre_mass(path, contours, 'y', ptr);
    if(perimeter)
        perimeter_area_circularity(img, contours);
    if(area)
        perimeter_area_circularity(img, contours);
    if(circularity)
        perimeter_area_circularity(img, contours);
    if(angles)
        perimeter_area_circularity(img, contours);
    if(rect_b)
        image_details.bottom_line = bounding_rect_details(path, contours, 'l');
    if (rect_l)
        image_details.left_line = bounding_rect_details(path, contours, 't');
    if (rect_r)
        image_details.right_line = bounding_rect_details(path, contours, 'r');
    if (rect_t)
        image_details.top_line = bounding_rect_details(path, contours, 'b');
    if (num)
        image_details.num_particles = num;
    // Display the result
    cv::imshow("analysis", img);
    cv::waitKey(0);
    cv::destroyWindow("analysis");
    return img_details;
}
