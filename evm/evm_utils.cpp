#include "evm_utils.h"
#include "GraphUtils.h" /* setGraphColor, drawFloatGraph, showFloatGraph */

#include <cassert> /* assert */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <opencv.hpp>

using namespace std;
using namespace cv;

// TODO1
// 需要重构画图的几个函数 ShowGraph 
// 因为这几个函数用的数据结构即接口都比较老，实际上都是用的C的接口函数
// 尤其是opencv方面，担心会出现问题，等之后记得重构GraphUtils

void Rgb2Yiq(const Mat_<Vec3f> input, Mat_<Vec3f> &output) {
	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			Vec3f p = input.at<Vec3f>(i, j);
			output.at<Vec3f>(i, j).val[2] = (0.2990   * p.val[2] + 0.587000 * p.val[1] + 0.114000 * p.val[0]) / 255;
			output.at<Vec3f>(i, j).val[1] = (0.595716 * p.val[2] - 0.274453 * p.val[1] - 0.321263 * p.val[0]) / 255;
			output.at<Vec3f>(i, j).val[0] = (0.211456 * p.val[2] - 0.522591 * p.val[1] + 0.311135 * p.val[0]) / 255;
		}
	}
}


void Yiq2Rgb(const Mat_<Vec3f> input, Mat_<Vec3f> &output) {
	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			Vec3f p = input.at<Vec3f>(i, j);
			output.at<Vec3f>(i, j).val[2] = (1 * p.val[2] + 0.9563 * p.val[1] + 0.6210 * p.val[0]);
			output.at<Vec3f>(i, j).val[1] = (1 * p.val[2] - 0.2721 * p.val[1] - 0.6474 * p.val[0]);
			output.at<Vec3f>(i, j).val[0] = (1 * p.val[2] - 1.1070 * p.val[1] + 1.7046 * p.val[0]);
		}
	}
}



void ShowGraph(const string win_name, const vector<float>& inputdata, const int data_length, int delay_ms) {
	assert(!win_name.empty());

	const char *p = win_name.c_str();
	showFloatGraph(p, &inputdata[0], data_length, delay_ms);

}


void  DrawGraphInImg(const vector<float>& inputdata, const int data_length, Mat& dstImg, int width_graph,
						int height_graph, float min, float max, int x_ROI, int y_ROI, string graph_name, bool resetcolor) {
	assert(!dstImg.empty());
	if (resetcolor) setGraphColor(0);
	

	IplImage input_Ipl = dstImg;
	CvRect ROI = cvRect(x_ROI, y_ROI, width_graph, height_graph);
	cvSetImageROI(&input_Ipl, ROI);

	
	char* p = (char*)graph_name.c_str();
	
	drawFloatGraph(&inputdata[0], data_length, &input_Ipl, min, max, width_graph, height_graph, p);
	//cvShowImage("temp", &input_Ipl);
}

void ReadDataFromCSV(const string filename, vector<float>& output, int line) {

	ifstream in_csv(filename, ios::in);
	assert(in_csv.is_open());

	string line_string;
	vector<vector<float> > all_data;
	
	while (getline(in_csv, line_string)) {
		stringstream ss(line_string);
		string temp_float;
		vector<float> line_data;

		while (getline(ss, temp_float, ',')) 
			line_data.push_back(stof(temp_float));

		all_data.push_back(line_data);
	}
 	
	output = all_data[line];
}


void WriteDataToCSV(const string filename, const vector<float>& input) {
	ofstream out_csv(filename, ios::app);
	assert(input.size() != 0);
	string temp;
	for (auto it = input.begin(); it != input.end() - 1; ++it){
		temp = temp + to_string(*it) + ',';
	}
	temp = temp + to_string(*(input.end() - 1)) + '\n';

	out_csv << temp;
	out_csv.close();
}