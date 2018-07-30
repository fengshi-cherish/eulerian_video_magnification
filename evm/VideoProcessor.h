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

// ����ʱ���˲�������IIR �� �����ͨ�˲�����Ҫ�ع���ȥ����������ع��ͻ�Ƚ��鷳���ܶ�˽�б������������棬���Ծ���ʱ����
// FFTW������oepncv�е�DFT
// �ع�GraphUtils����Ϊ�ܶ�ӿڶ���C���Եģ�����������������õĶ���opencv1.0�����ݽṹ����ָ�����
class VideoProcessor {
public:
	explicit VideoProcessor();
	cv::VideoCapture getCapture();

	bool InputFile(const string& filename);
	//���ÿռ��˲�����
	void SetSpatialFilter(spatialFilterType type);
	//����ʱ���˲�����
	void SetTemporalFilter(temporalFilterType type);
	//�˶��Ŵ�
	void MotionMagnify();

	// todo
	// ��ɫ�Ŵ�
	void ColorMagnify();

private:
	//������ͼƬƴ��һ�Ŵ�ͼ
	void Concat(vector<Mat>& frames, Mat& dst);

	// ��һ�Ŵ�ͼ��ɶ���ͼƬ
	void Deconcat(Mat& src, const cv::Size &FramesSize, vector<Mat> &frames);

	//���пռ��˲�
	bool SpatialFilter(const Mat& src, vector<Mat>& pyramid);

	//����ʱ���˲�
	void TemporalFilter(const Mat& src, Mat& dst);
	
	// IIR �˲�
	void TemporalIIRFilter(const Mat& src, Mat& dst);

	// �����ͨ�˲�
	void TemporalIdealFilter(const Mat& src, Mat& dst);

	void CreateIdealBandpassFilter(Mat& filter, double fl, double fh, double rate);
	//�Ŵ��˶�
	void Amplify(const Mat& src, Mat& dst);

	//˥��I��Qͨ��
	void Attenuate(const Mat& src, Mat& dst);


private:

	VideoCapture capture;
	//�������֡��
	long fnumber;

	//��֡��
	long length;

	//֡��
	double rate;

	//�ռ��˲�����
	spatialFilterType spatialType;

	//ʱ���˲�����
	temporalFilterType temporalType;

	//ͼ��������Ĳ���
	int levels;

	//delta
	float delta;
	//�Ŵ�����
	float alpha;
	
	float lambda;
	//��ֹ����
	float lambda_c;

	//low cut-oof�ͽ�ֹƵ��
	float fl;

	//high cut-off�߽�ֹƵ��
	float fh;

	// ��ǰ����������
	int cur_level;

	// ��ͨIIR
	vector<Mat> lowpass1;
	vector<Mat> lowpass2;

	// ��ɫ˥��ϵ��
	float chrom_attenuation;

	//�Ŵ�����
	float exaggeration_factor;
};

#endif