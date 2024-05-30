//reading from the json using nlohmann

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
 
using json = nlohmann::json;
using namespace std;
using namespace nlohmann::literals;

vector<float> parsing_values(std::string camera_type) {
	cout << "parsing" << endl;
	std::ifstream f("C:\\Users\\rojard\\Downloads\\SGR Config.json");
	json data;
 	data = json::parse(f);
	//f >> data;
	vector<float> return_values(4);
	cout << "parsed" << endl;
	//can loop through the json and store their corresponding values
	json details_per_camera = data["SGRParametersCamwise"];
	
	int left = 0;
	for (json details : details_per_camera) {
		//it will have 2 -> one right camera, and one left camera
		if (details[0]["ThrCamName"] != camera_type && details[1]["ThrCamName"] != camera_type
			&& details[0]["NonThrCam"]["Name"] != camera_type && details[1]["NonThrCam"]["Name"] != camera_type) {
			continue;
		}
		if (details[left]["ThrCamName"] != camera_type || details[left]["NonThrCam"]["Name"] != camera_type) {
			left = 1;
		}
		//cout << "camera" << details[left]["ThrCamName"] << endl;

		return_values[0] = details[left]["ThresholdSettings"][0]["ThresholdRangeBGImage"]["LowerValue"];
		//cout << "lower threshold" << return_values[0] << endl;
		return_values[1] = details[left]["ThresholdSettings"][0]["ThresholdRangeBGImage"]["UpperValue"];
		//cout << "upper threshold" << return_values[1] << endl;
		return_values[2] = details[left]["MinAreaLimit"];
		//cout << "lower area" << return_values[2] << endl;
		return_values[3] = details[left]["MaxAreaLimit"];
		//cout << "upper area" << return_values[3] << endl;

		break;
	}
	return return_values;
}

