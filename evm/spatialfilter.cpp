#include "spatialfilter.h"

#include <iostream>
#include <opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;


void CombineImg(const vector<Mat>& input_img, Mat& output) {
	assert(input_img.size() > 0);
	int rows = input_img[0].rows;
	int sum_cols = 0;
	for (auto it = input_img.begin(); it != input_img.end(); ++it) {
		sum_cols += (*it).cols;
	}

	output = Mat::zeros(rows, sum_cols, input_img[0].type());

	int col_start = 0;
	for (auto it = input_img.begin(); it != input_img.end(); ++it) {
		(*it).copyTo(output(Rect(col_start, 0, (*it).rows, (*it).cols)));
		col_start += (*it).cols;
	}

}



bool BuildGaussianPyramid(const Mat& input, vector<Mat>& pyramid, const int level) {
	if (level <= 1) {
		cerr << "level should be larger than 1!" << endl;
		return false;
	}
	pyramid.clear();
	pyramid.push_back(input);

	Mat current_img = input;
	for (int i = 0; i < level; ++i) {
		Mat down_img;
		pyrDown(current_img, down_img);
		pyramid.push_back(down_img);
		current_img = down_img;
	}

	return true;
}


void ReconstructFromGassusianPyr(const Mat& src_img, Mat& output, const int level) {

	Mat current_img = src_img;
	for (int i = 0; i < level; ++i) {
		Mat up_img;
		pyrUp(current_img, up_img);
		current_img = up_img;
	}

	current_img.copyTo(output);
}



bool BuildLaplacianPyrmaid(const Mat& input, vector<Mat>& pyrmaid, const int level) {
	if (level < 1) {
		cerr << "level should be larger than 1" << endl;
		return false;
	}
	pyrmaid.clear();
	Mat current_img = input;
	for (int i = 0; i < level; ++i) {
		Mat down_img, up_img;
		pyrDown(current_img, down_img); 
		pyrUp(down_img, up_img, current_img.size());
		Mat result_img = current_img - up_img;
		pyrmaid.push_back(result_img);
		current_img = down_img;
	}
	pyrmaid.push_back(current_img);

	return true;
}

void upsamplingFromGaussianPyramid(const cv::Mat &src,
	const int levels,
	cv::Mat &dst)
{
	cv::Mat currentLevel = src.clone();
	for (int i = 0; i < levels; ++i) {
		cv::Mat up;
		cv::pyrUp(currentLevel, up);
		currentLevel = up;
	}
	currentLevel.copyTo(dst);
}



void ReconstructFromLaplacianPyr(const vector<Mat>& pyrmaid, Mat& output, const int level) {
	Mat current_img = pyrmaid[level];
	for (int i = level-1; i >= 0; --i) {
		Mat up_img;
		pyrUp(current_img, up_img, pyrmaid[i].size());
		current_img = up_img + pyrmaid[i];
	}
	output = current_img;
}
