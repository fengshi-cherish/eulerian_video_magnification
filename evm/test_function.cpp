#include "spatialfilter.h"
#include "evm_utils.h"

#include <iostream>
#include <opencv.hpp>
#include <vector>
#include <string>

#define LEVEL 5

using namespace std;
using namespace cv;


void test_buildgaussianpyr(const Mat& input) {
	vector<Mat> pyrmaid;

	BuildGaussianPyramid(input, pyrmaid, LEVEL);
	Mat result_img;
	CombineImg(pyrmaid, result_img);
	imshow("GaussianoPyrmaid", result_img);

	
}

void test_constructgaussian(const Mat& input) {
	vector<Mat> pyrmaid; 
	Mat reco_img;

	BuildGaussianPyramid(input, pyrmaid, LEVEL);
	ReconstructFromGassusianPyr(pyrmaid[LEVEL], reco_img, LEVEL);
	imshow("ReconstructImgFromGaussianPyr", reco_img);
	
}

void test_buildlaplacianpyr(const Mat& input) {
	vector<Mat> pyrmaid;
	
	BuildLaplacianPyrmaid(input, pyrmaid, LEVEL);
	Mat result_img;
	CombineImg(pyrmaid, result_img);
	imshow("LaplacianPyrmaid", result_img);

}

void test_constructlaplacian(const Mat& input){
	vector<Mat> pyrmaid;
	Mat result_img;

	BuildLaplacianPyrmaid(input, pyrmaid, LEVEL);
	ReconstructFromLaplacianPyr(pyrmaid, result_img, LEVEL);
	imshow("ReconstructImgFromLaplacianPyr", result_img);

}


void test_rgb2yiq(const Mat& input) {
	Mat_<Vec3f> result_img = Mat_<Vec3f>::zeros(input.rows, input.cols);
	Mat_<Vec3f> newinput;
	input.convertTo(newinput, CV_32FC3);
	Rgb2Yiq(newinput, result_img);
	imshow("rgb2yiq", result_img);
}

void test_yiq2rgb(const Mat& input) {
	Mat_<Vec3f> result_img = Mat_<Vec3f>::zeros(input.rows, input.cols);
	Mat_<Vec3f> newinput;
	input.convertTo(newinput, CV_32FC3);
	Rgb2Yiq(newinput, result_img);
	Yiq2Rgb(result_img, result_img);
	imshow("yiq2rgb", result_img);
}

void test_writedatatoCSV() {
	int a = -3;
	int b = 30;
	vector<float> result(10);
	for (int i = 0; i < 10; ++i) {
		result[i] = a * i + b;
	}
	WriteDataToCSV("write.csv", result);
}

void test_readdatafromCSV(){
	vector<float> temp;
	ReadDataFromCSV("write.csv", temp, 0);
	for (int i = 0; i < temp.size(); ++i) {
		cout << temp[i] << " ";
	}
}

void test_showgraph() {
	vector<float> temp;
	ReadDataFromCSV("write.csv", temp, 6);
	ShowGraph("showgraph", temp, temp.size());
}

void test_drawgraph(Mat& input) {
	vector<float> temp;
	ReadDataFromCSV("write.csv", temp, 6);
	DrawGraphInImg(temp, temp.size(), input, 400, 200, -110, 30.0, 0, 0, "1 lines", true);
	
	ReadDataFromCSV("write.csv", temp, 1);
	DrawGraphInImg(temp, temp.size(), input, 400, 200, -800, 40, 0, 250, "2 lines");
	ReadDataFromCSV("write.csv", temp, 5);
	DrawGraphInImg(temp, temp.size(), input, 400, 200, -800, 40, 0, 500, "3 lines");
	imshow("test_drawgraph", input);


}

void TestGaussianPyrmaid(const  Mat&  input) {
	test_buildgaussianpyr(input);
	test_constructgaussian(input);
}

void TestLaplacianPyrmaid(const Mat& input) {
	test_buildlaplacianpyr(input);
	test_constructlaplacian(input);
}

void TestColorConvert(Mat& input) {
	test_rgb2yiq(input);
	test_yiq2rgb(input);
}

void TestWriteAndReadCSV() {
	test_writedatatoCSV();
	test_readdatafromCSV();
}


void TestDrawAndShowGraph(Mat& input) {
	test_showgraph();
	test_drawgraph(input);
}
//
//int main() {
//	Mat  img = imread("lena.jpg");
//	
//
//	//imshow("src_img", img);
//
//	// 测试建立高斯金字塔和高斯金字塔重建图像
//	//TestGaussianPyrmaid(img);
//
//	// 测试建立拉普拉斯金字塔和拉普拉斯金字塔重建图像
//	//TestLaplacianPyrmaid(img);
//
//	// 测试图像在RGB和YIQ色域互相转换
//	//TestColorConvert(img);
//
//	// TODO
//	// 测试从文件中读取数据和将数据输入到文件中
//	//TestWriteAndReadCSV();
//
//	// 测试在图像中画和显示折线图
//	// TestDrawAndShowGraph(img);
//
//
//	waitKey();
//
//
//	return 0;
//}