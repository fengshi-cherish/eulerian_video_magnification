#include "VideoProcessor.h"
#include "spatialfilter.h"

#include <vector>
#include <opencv.hpp>


using namespace std;
using namespace cv;

VideoProcessor::VideoProcessor(): 
fnumber(0), length(0), cur_level(0), levels(4), alpha(10), fl(0.05), fh(0.4), rate(0), 
delta(0), lambda_c(80), lambda(0), chrom_attenuation(0.1), exaggeration_factor(2.0) {

}

// ������Ƶ������
bool VideoProcessor::InputFile(const string& filename) {
	fnumber = 0;

	if (capture.open(filename)) {
		length = capture.get(CV_CAP_PROP_FRAME_COUNT);
		rate = capture.get(CV_CAP_PROP_FPS);
		return true;
	}
	return false;
}

// ���ÿռ��˲�����
void VideoProcessor::SetSpatialFilter(spatialFilterType type) {
	spatialType = type;
}

// ����ʱ���˲�����
void VideoProcessor::SetTemporalFilter(temporalFilterType type) {
	temporalType = type;
}

// ���пռ��˲�
bool VideoProcessor::SpatialFilter(const Mat& src, vector<Mat>& pyramid) {
	switch (spatialType) {
	case LAPLACIAN:
		return BuildLaplacianPyrmaid(src, pyramid, levels); 
		break;
	case GAUSSIAN:
		return BuildGaussianPyramid(src, pyramid, levels); 
		break;
	default:
		return false; 
		break;
	}
}

// ����ʱ���˲�
void VideoProcessor::TemporalFilter(const Mat& src, Mat& dst) {
	switch (temporalType) {
	case IIR:
		TemporalIIRFilter(src, dst); 
		break;
	case IDEAL:
		TemporalIdealFilter(src, dst); 
		break;
	default:
		 break;
	}
	return;
}

// �����е�֡ƴ��һ�Ŵ�ͼ����ͼ�е�ÿһ����ԭ����֡reshap��õ���
void VideoProcessor::Concat(vector<Mat>& frames, Mat& dst) {
	Size frameSize = frames.at(0).size();
	Mat temp(frameSize.width * frameSize.height, frames.size() - 1, CV_32FC3);
	for (int i = 0; i < frames.size() - 1; ++i) {
		Mat input = frames.at(i);
		Mat reshaped = input.reshape(3, input.cols * input.rows).clone();
		Mat line = temp.col(i);
		reshaped.copyTo(line);
	}
	temp.copyTo(dst);
}

// ��һ�Ŵ�ͼ��ɶ�֡
void VideoProcessor::Deconcat(Mat& src, const cv::Size &FramesSize, vector<Mat> &frames) {
	for (int i = 0; i < length - 1; ++i) {
		Mat line = src.col(i).clone();
		Mat reshaped = line.reshape(3, FramesSize.height).clone();
		frames.push_back(reshaped);
	}
}

// ��������һ�׵�ͨIIR���һ����ͨIIR�˲���
void VideoProcessor::TemporalIIRFilter(const Mat& src, Mat& dst) {
	Mat temp1 = (1 - fh) * lowpass1[cur_level] + fh * src;
	Mat temp2 = (1 - fl) * lowpass2[cur_level] + fl * src;
	lowpass1[cur_level] = temp1;
	lowpass2[cur_level] = temp2;
	dst = lowpass1[cur_level] - lowpass2[cur_level];

}

// ����ͼ�������ƴ�ɵĴ�ͼ���������ͨ�˲�
 void VideoProcessor::TemporalIdealFilter(const Mat& src, Mat& dst) {
	Mat channels[3];

	split(src, channels);

	for (int i = 0; i < 3; ++i) {
		Mat curr_img = channels[i];
		Mat temp_img;

		int width = getOptimalDFTSize(curr_img.cols);
		int height = getOptimalDFTSize(curr_img.rows);

		// ���б߽�����
		copyMakeBorder(curr_img, temp_img, 0, height - curr_img.rows, 0, width - curr_img.cols, BORDER_CONSTANT, Scalar::all(0));

		Mat complex_img;

		dft(temp_img, temp_img, DFT_ROWS | DFT_SCALE);

		Mat filter = temp_img.clone();
		CreateIdealBandpassFilter(filter, fl, fh, rate);

		mulSpectrums(temp_img, filter, temp_img, DFT_ROWS);

		idft(temp_img, temp_img, DFT_ROWS | DFT_SCALE);

		channels[i] = temp_img(Rect(0, 0, curr_img.cols, curr_img.rows));
	}
	merge(channels, 3, dst);

	normalize(dst, dst, 0, 1, CV_MINMAX);

}

// ����1D�����ͨ�˲�
void VideoProcessor::CreateIdealBandpassFilter(Mat& filter, double fl, double fh, double rate) {
	int width = filter.cols;
	int height = filter.rows;

	fl = 2 * fl * width / rate;
	fh = 2 * fh * width / rate;

	double response;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			//  �˲�����Ӧ
			if (j >= fl && j <= fh) response = 1.0f;
			else response = 0.0f;
			filter.at<float>(i, j) = response;
		}
	}
}

void VideoProcessor::Amplify(const Mat& src, Mat& dst) {
	float curr_alpha;
	switch (spatialType) {
	case LAPLACIAN:
		// ���㵱ǰ��alphaֵ
		curr_alpha = lambda / delta / 8 - 1;
		curr_alpha *= exaggeration_factor;
		if (cur_level == levels || cur_level == 0) // ������߲�����Ƶ�ʻ���
			dst = src * 0;
		else
			dst = src * cv::min(alpha, curr_alpha);
		break;
	case GAUSSIAN:
		dst = src * alpha;
		break;
	default:
		break;
	}

}

// ˥�� I,Qͨ���Ŵ��Ľ��
void VideoProcessor::Attenuate(const Mat& src, Mat& dst) {
	Mat planes[3];
	split(src, planes);
	planes[1] = planes[1] * chrom_attenuation;
	planes[2] = planes[2] * chrom_attenuation;
	merge(planes, 3, dst);
}



void VideoProcessor::MotionMagnify() {

	SetSpatialFilter(LAPLACIAN);
	SetTemporalFilter(IIR);

	Mat input;
	Mat output;
	Mat motion;

	vector<Mat> pyramid;
	vector<Mat> filtered;

	if (!capture.isOpened()) return;

	while (1) {
		capture >> input;
		if (waitKey(20) >= 0 || input.empty()) break;

		input.convertTo(input, CV_32FC3, 1.0 / 255.0f);

		// 1. ת��ɫ�ռ䵽Lab��ɫ�ռ�
		cv::cvtColor(input, input, CV_BGR2Lab);

		// 2. �Ե�֡ͼ����пռ��˲�
		Mat s = input.clone();
		SpatialFilter(s, pyramid);

		// 3. �Ե���ͼ�����ɵ�ͼ�����������ʱ���˲�
		// ���ҷŴ��˶�
		if (fnumber == 0) {
			lowpass1 = pyramid;
			lowpass2 = pyramid;
			filtered = pyramid;
		}
		else {
			for (int i = 0; i < levels; ++i) {
				
				cur_level = i;
				TemporalFilter(pyramid.at(i), filtered.at(i));
			}

			// ��ÿ���ռ�Ƶ�ʴ����зŴ�
			Size filter_size = filtered.at(0).size();
			int w = filter_size.width;
			int h = filter_size.height;

			delta = lambda_c / 8.0 / (1.0 + alpha);
			// Ϊ�˿��ӻ���������lambda>lambda_c�Ļ�����һ���Ŵ󣬷�����ӻ�
			exaggeration_factor = 2.0;

			// ������Ϳռ�Ƶ�ʵ���������lambda��3�Ǿ��鳣��
			lambda = sqrt(w * w + h * h) / 3;

			for (int i = levels; i >= 0; --i) {
				cur_level = i;

				Amplify(filtered.at(i), filtered.at(i));
				lambda /= 2.0;
			}
		}

		// 4.  �ӷŴ�֮��Ľ������ؽ�ԭͼ��
		ReconstructFromLaplacianPyr(filtered, motion, levels);

		// 5.  ˥��I��Qͨ���ķŴ��� 
		Attenuate(motion, motion);

		// 6. ������ӵ�ԭͼ��ȥ�����Ե�һ֡
		if (fnumber > 0)
			s += motion;

		//��ͼ��ת����RGB ͨ����CV_8UC3
		output = s.clone();
		cv::cvtColor(output, output, CV_Lab2BGR);
		output.convertTo(output, CV_8UC3, 255.0, 1.0 / 255.0);

		cv::imshow("result", output);
		fnumber++;
	}



}


void VideoProcessor::ColorMagnify() {
	fl = 0.83;
	fh = 1;
	alpha = 150;
	exaggeration_factor = 2;
	SetSpatialFilter(GAUSSIAN);
	SetTemporalFilter(IDEAL);

	Mat input;
	Mat output;
	Mat motion;

	Mat temp;
	vector<Mat> frames;
	vector<Mat> downsampled_frames;
	vector<Mat> filtered_frames;
	vector<Mat> show_img;
	// �������²���ƴ�ɵĴ�ͼ
	Mat concat_img;
	// �˲���Ĵ�ͼ
	Mat concat_filtered;
	if (!capture.isOpened()) return;

	// 1. ���пռ��˲�
	while (1) {
		capture >> input;
		if (waitKey(20) >= 0 || input.empty()) break;

		input.convertTo(temp, CV_32FC3);
		frames.push_back(temp.clone());
		vector<Mat> pyramid;
		SpatialFilter(temp, pyramid);
		downsampled_frames.push_back(pyramid.at(levels - 1));
		
		fnumber++;
	}

	// 2. ������֡ƴ��һ�Ŵ�ͼ
	Concat(downsampled_frames, concat_img);

	// 3. ����ʱ���˲�
	TemporalFilter(concat_img, concat_filtered);


	// 4. ���зŴ�
	Amplify(concat_filtered, concat_filtered);

	// 5. ����ͼ��ɶ���Сͼ
	Deconcat(concat_filtered, downsampled_frames.at(0).size(), filtered_frames);

	fnumber = 0;
	for (int i = 0; i < length - 1; ++i) {
		upsamplingFromGaussianPyramid(filtered_frames.at(i), levels, motion);
		resize(motion, motion, frames.at(i).size());
		temp = frames.at(i) + motion;
		output = temp.clone();
		double minVal, maxVal;
		minMaxLoc(output, &minVal, &maxVal);
		output.convertTo(output,CV_8UC3, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
		//output.convertTo(output, CV_8UC3, 1.0 / 255, 0);
		if (waitKey(20) >= 0) break;
		imshow("ColorMagnify", output);
		show_img.push_back(output.clone());
		fnumber++;
	}

	/*for(int i = 0;i < length - 1; ++i) {
		if (waitKey(20) >= 0) break;
		imshow("ColorMagnify", show_img[i]);
	}*/


 
}