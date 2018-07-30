#ifndef EVM_UTILS_H_
#define EVM_UTILS_H_


#include <opencv.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;


// RGBתYIQͼ��      ��Դ ofxEvm
// input ����ͼ��,
// output ������
// Ҫ��֤���������ͼ�����ΪMat_<Vec3f>��ʽ
void Rgb2Yiq(const Mat_<Vec3f> input, Mat_<Vec3f> &output);

// YIQתRGBͼ��      ��Դ ofxEvm
// input ����ͼ��,
// output ������
// Ҫ��֤���������ͼ�����ΪMat_<Vec3f>��ʽ
void Yiq2Rgb(const Mat_<Vec3f> input, Mat_<Vec3f> &output);

// ��һ�����ڻ���һάfloat���������ͼ
// win_name ������������Ϊ���ַ���
// inputdata Ҫ��ʾ����������
// data_length ���ݵĳ���
// delay_ms ������ʾ��ʱ�䣨��λms����Ĭ��Ϊ0����һֱ��ʾ
void ShowGraph(const string win_name, const vector<float>& inputdata, const int data_length, int delay_ms = 0);


// ��һ��ͼ���л���һάfloat���������ͼ
// inputdata Ҫ��ʾ��һά����
// data_length ���ݵĳ���
// input �����ͼ��
// min ��ʾ�����ݵ���Сֵ
// max ��ʾ�����ݵ����ֵ
// heigh_graph Ҫ����ͼ�ĸ�
// width_graph Ҫ����ͼ�Ŀ� 
// x_ROI, y_ROI Ҫ����ͼ��ͼ���е�λ��,Ĭ�ϴ����Ͻǿ�ʼ��
// graph_name ����ͼ�����֣�Ĭ��Ϊ��
// resetcolor �����ߵ���ɫ�Ƿ������trueʱ����ɫ����Ϊ��ɫ��Ĭ��Ϊfalse����ɫ����ɫ��ʼ��Ȼ�����̣���ȡ�����
void  DrawGraphInImg(const vector<float>& inputdata, const int data_length, Mat& dstImg,int width_graph, 
	int height_graph, float min = 0.0, float max = 0.0, int x_ROI = 0, int y_ROI = 0, string graph_name = 0, bool resetcolor = false);

// ��csv�ļ��ж�ȡָ���е�����
// filename ��ȡ�ļ���·�����ļ���
// output ���������
// line ��ȡ�����ݵ�������Ĭ����0����Ĭ�϶�ȡ��һ������
void ReadDataFromCSV(const string filename, vector<float>& output, int line = 0);

// ������д�뵽CSV�ļ���
// filename Ҫд����ļ���·��������
// input Ҫд������ݣ���׷�ӵķ�ʽд��
void WriteDataToCSV(const string filename, const vector<float>& input);
#endif