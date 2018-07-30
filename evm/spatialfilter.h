#ifndef SPATIALFILTER_H_
#define SPATIALFILTER_H_

#include <opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

// ��һ��ͼƬƴ����
// input_img �����һ��ͼƬ
// output ������ƴ�õ�ͼƬ
void CombineImg(const vector<Mat>& input_img, Mat& output);

// ���и�˹�������任
// pymaid ���б任�õ��Ľ�����ͼ������
// input �����ԭͼ��
// level�ǽ��������ܲ���
// ͼ��������У��㼶���Խ�ߣ�ͼ��ԽС
bool BuildGaussianPyramid(const Mat& input, vector<Mat>& pyramid, const int level);


// ���ݸ�˹�������ؽ�ԭͼ��
void ReconstructFromGassusianPyr(const Mat& src_img, Mat& output, const int level);

//����������˹�������任
//pyrmaid  �洢�任�õ��Ľ����������ÿ��Ԫ�ض�Ӧ��������һ�㣬����Խ��ͼ��ԽС
//input ����ͼ��
//level ����������
bool BuildLaplacianPyrmaid(const Mat& input, vector<Mat>& pyrmaid, const int level);

// ����������˹�������ؽ�ԭͼ��
// output ������
// pyrmaid ������˹������
// level ����������
void ReconstructFromLaplacianPyr(const vector<Mat>& pyrmaid, Mat& output, const int level);

void upsamplingFromGaussianPyramid(const Mat &src, const int levels, Mat &dst);

#endif