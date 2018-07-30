#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include "evm_utils.h"
#include "spatialfilter.h"

#include <vector>
#include <opencv.hpp>


using namespace std;
using namespace cv;

enum spatialFilterType {LAPLACIAN, GAUSSIAN};
enum temporalFilterType {IIR, IDEAL}; 

// 本来时域滤波（包括IIR 和 理想带通滤波）需要重构出去，但是如果重构就会比较麻烦，很多私有变量都在类里面，所以就暂时保留
// FFTW来代替oepncv中的DFT
// 重构GraphUtils，因为很多接口都是C语言的，并且这个第三方库用的都是opencv1.0的数据结构，纯指针操作
class VideoProcessor {
public:
	explicit VideoProcessor();
	cv::VideoCapture getCapture();

	bool InputFile(const string& filename);
	//设置空间滤波方法
	void SetSpatialFilter(spatialFilterType type);
	//设置时间滤波方法
	void SetTemporalFilter(temporalFilterType type);
	//运动放大
	void MotionMagnify();

	// todo
	// 颜色放大
	void ColorMagnify();

private:
	//将多张图片拼成一张大图
	void Concat(vector<Mat>& frames, Mat& dst);

	// 将一张大图拆成多张图片
	void Deconcat(Mat& src, const cv::Size &FramesSize, vector<Mat> &frames);

	//进行空间滤波
	bool SpatialFilter(const Mat& src, vector<Mat>& pyramid);

	//进行时间滤波
	void TemporalFilter(const Mat& src, Mat& dst);
	
	// IIR 滤波
	void TemporalIIRFilter(const Mat& src, Mat& dst);

	// 理想带通滤波
	void TemporalIdealFilter(const Mat& src, Mat& dst);

	void CreateIdealBandpassFilter(Mat& filter, double fl, double fh, double rate);
	//放大运动
	void Amplify(const Mat& src, Mat& dst);

	//衰减I、Q通道
	void Attenuate(const Mat& src, Mat& dst);


private:

	VideoCapture capture;
	//处理过的帧数
	long fnumber;

	//总帧数
	long length;

	//帧率
	double rate;

	//空间滤波类型
	spatialFilterType spatialType;

	//时间滤波类型
	temporalFilterType temporalType;

	//图像金字塔的层数
	int levels;

	//delta
	float delta;
	//放大因子
	float alpha;
	
	float lambda;
	//截止波长
	float lambda_c;

	//low cut-oof低截止频率
	float fl;

	//high cut-off高截止频率
	float fh;

	// 当前金字塔层数
	int cur_level;

	// 低通IIR
	vector<Mat> lowpass1;
	vector<Mat> lowpass2;

	// 颜色衰减系数
	float chrom_attenuation;

	//放大因子
	float exaggeration_factor;
};

#endif