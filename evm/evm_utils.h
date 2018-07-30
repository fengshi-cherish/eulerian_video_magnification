#ifndef EVM_UTILS_H_
#define EVM_UTILS_H_


#include <opencv.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;


// RGB转YIQ图像      来源 ofxEvm
// input 输入图像,
// output 输出结果
// 要保证输入输出的图像必须为Mat_<Vec3f>格式
void Rgb2Yiq(const Mat_<Vec3f> input, Mat_<Vec3f> &output);

// YIQ转RGB图像      来源 ofxEvm
// input 输入图像,
// output 输出结果
// 要保证输入输出的图像必须为Mat_<Vec3f>格式
void Yiq2Rgb(const Mat_<Vec3f> input, Mat_<Vec3f> &output);

// 在一个窗口画出一维float数组的折线图
// win_name 窗口名，不能为空字符串
// inputdata 要显示的数组数据
// data_length 数据的长度
// delay_ms 窗口显示的时间（单位ms），默认为0，即一直显示
void ShowGraph(const string win_name, const vector<float>& inputdata, const int data_length, int delay_ms = 0);


// 在一幅图像中画出一维float数组的折线图
// inputdata 要显示的一维数组
// data_length 数据的长度
// input 输入的图像
// min 显示的数据的最小值
// max 显示的数据的最大值
// heigh_graph 要画的图的高
// width_graph 要画的图的宽 
// x_ROI, y_ROI 要画的图在图像中的位置,默认从左上角开始画
// graph_name 折线图的名字，默认为空
// resetcolor 画曲线的颜色是否更换，true时，颜色重置为颜色，默认为false，颜色从蓝色开始，然后是绿，红等、、、
void  DrawGraphInImg(const vector<float>& inputdata, const int data_length, Mat& dstImg,int width_graph, 
	int height_graph, float min = 0.0, float max = 0.0, int x_ROI = 0, int y_ROI = 0, string graph_name = 0, bool resetcolor = false);

// 从csv文件中读取指定行的数据
// filename 读取文件的路径和文件名
// output 输出的数组
// line 读取的数据的行数，默认是0，即默认读取第一行数据
void ReadDataFromCSV(const string filename, vector<float>& output, int line = 0);

// 将数据写入到CSV文件中
// filename 要写入的文件的路径和名称
// input 要写入的数据，以追加的方式写入
void WriteDataToCSV(const string filename, const vector<float>& input);
#endif