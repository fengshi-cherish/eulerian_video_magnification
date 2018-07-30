#include "VideoProcessor.h"
#include "evm_utils.h"

#include <opencv.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;

// Ϊ�˷���  ��������д�˲���Ȼ���ڷŵ�������
void Concats(vector<Mat>& frames, Mat& dst) {
	Size frameSize = frames.at(0).size();
	Mat temp(frameSize.width * frameSize.height, frames.size() - 1, CV_8UC3);
	for (int i = 0; i < frames.size() - 1; ++i) {
		Mat input = frames.at(i);
		Mat reshaped = input.reshape(3, input.cols * input.rows).clone();
		Mat line = temp.col(i);
		reshaped.copyTo(line);
	}
	temp.copyTo(dst);
}

void Deconcat(Mat& src, const cv::Size &FramesSize, vector<Mat> &frames) {
	for (int i = 0; i < src.cols; ++i) {
		Mat line = src.col(i).clone();
		Mat reshaped = line.reshape(3, FramesSize.height).clone();
		frames.push_back(reshaped);
	}
}
void test_concat_deconcat(vector<Mat>& concatframes) {
	Mat result;
	vector<Mat> result2;
	Concats(concatframes, result);
	imshow("concatresult", result);
	Deconcat(result, result.size(), result2);
	cout << result.cols << " " << result.rows << endl;
	cout << result2.size() << endl;
	Concats(result2, result);
	imshow("concatresult2", result);
	waitKey(0);
}

// �����˶��Ŵ�
int main(){
	VideoProcessor processor;
	if (!processor.InputFile("face.mp4")) return -1;
	
	//processor.MotionMagnify();
	processor.ColorMagnify();

	

	/*if (!cap.isOpened()) {
		cout << "û������ļ�" << endl;
		return -1;
	}
	Mat frame;
	vector<Mat> concatframes;
	while (1) {
		cap >> frame;
		if (waitKey(20) >= 0 || frame.empty()) break;
		concatframes.push_back(frame);

		imshow("test_videoprocessor", frame);
	}

	
	test_concat_deconcat(concatframes);*/


	return 0;
}