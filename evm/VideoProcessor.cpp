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

// 控制视频的输入
bool VideoProcessor::InputFile(const string& filename) {
	fnumber = 0;

	if (capture.open(filename)) {
		length = capture.get(CV_CAP_PROP_FRAME_COUNT);
		rate = capture.get(CV_CAP_PROP_FPS);
		return true;
	}
	return false;
}

// 设置空间滤波方法
void VideoProcessor::SetSpatialFilter(spatialFilterType type) {
	spatialType = type;
}

// 设置时域滤波方法
void VideoProcessor::SetTemporalFilter(temporalFilterType type) {
	temporalType = type;
}

// 进行空间滤波
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

// 进行时域滤波
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

// 将所有的帧拼成一张大图，大图中的每一列是原来单帧reshap后得到的
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

// 将一张大图拆成多帧
void VideoProcessor::Deconcat(Mat& src, const cv::Size &FramesSize, vector<Mat> &frames) {
	for (int i = 0; i < length - 1; ++i) {
		Mat line = src.col(i).clone();
		Mat reshaped = line.reshape(3, FramesSize.height).clone();
		frames.push_back(reshaped);
	}
}

// 利用两个一阶低通IIR组成一个带通IIR滤波器
void VideoProcessor::TemporalIIRFilter(const Mat& src, Mat& dst) {
	Mat temp1 = (1 - fh) * lowpass1[cur_level] + fh * src;
	Mat temp2 = (1 - fl) * lowpass2[cur_level] + fl * src;
	lowpass1[cur_level] = temp1;
	lowpass2[cur_level] = temp2;
	dst = lowpass1[cur_level] - lowpass2[cur_level];

}

// 对用图像金字塔拼成的大图进行理想带通滤波
 void VideoProcessor::TemporalIdealFilter(const Mat& src, Mat& dst) {
	Mat channels[3];

	split(src, channels);

	for (int i = 0; i < 3; ++i) {
		Mat curr_img = channels[i];
		Mat temp_img;

		int width = getOptimalDFTSize(curr_img.cols);
		int height = getOptimalDFTSize(curr_img.rows);

		// 进行边界扩充
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

// 进行1D理想带通滤波
void VideoProcessor::CreateIdealBandpassFilter(Mat& filter, double fl, double fh, double rate) {
	int width = filter.cols;
	int height = filter.rows;

	fl = 2 * fl * width / rate;
	fh = 2 * fh * width / rate;

	double response;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			//  滤波器响应
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
		// 计算当前的alpha值
		curr_alpha = lambda / delta / 8 - 1;
		curr_alpha *= exaggeration_factor;
		if (cur_level == levels || cur_level == 0) // 忽略最高层和最低频率基带
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

// 衰减 I,Q通道放大后的结果
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

		// 1. 转颜色空间到Lab颜色空间
		cv::cvtColor(input, input, CV_BGR2Lab);

		// 2. 对单帧图像进行空间滤波
		Mat s = input.clone();
		SpatialFilter(s, pyramid);

		// 3. 对单张图像生成的图像金字塔进行时域滤波
		// 并且放大运动
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

			// 对每个空间频率带进行放大
			Size filter_size = filtered.at(0).size();
			int w = filter_size.width;
			int h = filter_size.height;

			delta = lambda_c / 8.0 / (1.0 + alpha);
			// 为了可视化，将符合lambda>lambda_c的基带进一步放大，方便可视化
			exaggeration_factor = 2.0;

			// 计算最低空间频率的特征波长lambda，3是经验常数
			lambda = sqrt(w * w + h * h) / 3;

			for (int i = levels; i >= 0; --i) {
				cur_level = i;

				Amplify(filtered.at(i), filtered.at(i));
				lambda /= 2.0;
			}
		}

		// 4.  从放大之后的金字塔重建原图像
		ReconstructFromLaplacianPyr(filtered, motion, levels);

		// 5.  衰减I、Q通道的放大结果 
		Attenuate(motion, motion);

		// 6. 将结果加到原图上去，忽略第一帧
		if (fnumber > 0)
			s += motion;

		//将图像转换回RGB 通道和CV_8UC3
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
	// 将所有下采样拼成的大图
	Mat concat_img;
	// 滤波完的大图
	Mat concat_filtered;
	if (!capture.isOpened()) return;

	// 1. 进行空间滤波
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

	// 2. 将多张帧拼成一张大图
	Concat(downsampled_frames, concat_img);

	// 3. 进行时域滤波
	TemporalFilter(concat_img, concat_filtered);


	// 4. 进行放大
	Amplify(concat_filtered, concat_filtered);

	// 5. 将大图拆成多张小图
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