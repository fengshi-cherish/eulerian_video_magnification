#ifndef SPATIALFILTER_H_
#define SPATIALFILTER_H_

#include <opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

// 将一组图片拼起来
// input_img 输入的一组图片
// output 将输入拼得的图片
void CombineImg(const vector<Mat>& input_img, Mat& output);

// 进行高斯金字塔变换
// pymaid 进行变换得到的金字塔图像数组
// input 输入的原图像
// level是金字塔的总层数
// 图像金字塔中，层级编号越高，图像越小
bool BuildGaussianPyramid(const Mat& input, vector<Mat>& pyramid, const int level);


// 根据高斯金字塔重建原图像
void ReconstructFromGassusianPyr(const Mat& src_img, Mat& output, const int level);

//进行拉普拉斯金字塔变换
//pyrmaid  存储变换得到的金字塔结果，每个元素对应金字塔的一层，层数越大，图像越小
//input 输入图像
//level 金字塔层数
bool BuildLaplacianPyrmaid(const Mat& input, vector<Mat>& pyrmaid, const int level);

// 根据拉普拉斯金字塔重建原图像
// output 输出结果
// pyrmaid 拉普拉斯金字塔
// level 金字塔层数
void ReconstructFromLaplacianPyr(const vector<Mat>& pyrmaid, Mat& output, const int level);

void upsamplingFromGaussianPyramid(const Mat &src, const int levels, Mat &dst);

#endif