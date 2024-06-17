#include "opencv2/opencv.hpp"


//need to add a thing that will threshold colour images also. cvtColor then threshold? or split it into streams and threshold them 

void threshold_img(cv::Mat imgsrc, cv::Mat imgdst, std::vector<float> range, int replace_value) {
	if (replace_value == 0) {
		if (imgdst.empty()) {
			imgsrc = cv::Mat(imgsrc.size().width, imgsrc.size().height, CV_8UC1, cv::Scalar(0, 0, 0));
		}
		else {
			imgdst = cv::Mat(imgsrc.size().width, imgsrc.size().height, CV_8UC1, cv::Scalar(0, 0, 0));
		}
		return;
	}
	
	try
	{
		if (imgdst.empty() == false) {
			threshold(imgsrc, imgdst, range[0], range[1], cv::THRESH_BINARY);
		}
		else {
			threshold(imgsrc, imgsrc, range[0], range[1], cv::THRESH_BINARY);

		}
	}
	catch (cv::Exception& e) {
		std::cout << e.msg << std::endl;
	}
	return;
}