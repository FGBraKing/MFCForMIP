#pragma once
#include "MyDIB.h"
//实现空间域的图像增强处理
//本代码依赖于自己实现的MyDIB类
//通过简单的修改，也很容易实现跨平台的版本
//8bits的彩图要重建调色板，略麻烦，统一将处理后的彩色图存为24位。而灰度图存为8位或者24位均可。
class SpatialEnhancement
{
public:
	SpatialEnhancement();
	~SpatialEnhancement();
	//对于24位图，可对灰度图和彩图做无差别处理；对于8位的图，灰度图的处理方式会更加简单
	//在如何处理8位和24位图上，着实纠结了好久。在是否要对灰度图和彩图分别处理，这个问题上也有犹豫
	//这几个处理，实现的初期只考虑处理灰度图，因此，对接收的图像统一转换为灰度图，再进行处理，保存为8位的灰度图
	bool LinearTrans(MyDIB& orgin_image, MyDIB& trans_image,int from_min,int from_max,int gray_min,int gray_max);
	bool SegLinearTrans(MyDIB& orgin_image, MyDIB& trans_image, int s_min, int s_max, int d_min, int d_max);
	bool HistogramEqualizatio(MyDIB& orgin_image, MyDIB& trans_image);
	bool HistogramSpecification(MyDIB& orgin_image, MyDIB& trans_image, BYTE data_num, int* data_aim, float* freq_aim);
	//这几个非线性滤波，对8位图像都转为灰度图进行单通道的处理，存为8位的图；而对24位图像，都是直接对三通道处理，存为24位图
	bool MinFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize);
	bool MaxFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize);
	bool MedianFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize);
	//下面几个处理实现的方式是，将处理后的8位和24位图统一保存为24位图
	bool ImageConvolution(MyDIB& orgin_image, MyDIB& trans_image, float* pFilter, int kernelSize);
	bool PseudoColorEnhance(MyDIB& orgin_image, MyDIB& trans_image);
	bool HighPassFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize);
	bool LowPassFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize);


private:
	//统计灰度频数
	void HistogramCount(BYTE* pImage, CSize imageSize,int grayNum[256]);
	void CalGrayFrequency(const CSize imageSize, const int grayNum[256], float frequency[256]);
	void CalCumulaFrequency(const float* frequency, float* cumu_freq, int num);
	void CalGrayRange(unsigned char* pDibData, int data_size);
	void LinearTransAlg(unsigned char orgin_pixel, unsigned char& trans_pixel, 
						unsigned char orgin_min, unsigned char orgin_max,
						unsigned char trans_min, unsigned char trans_max);

	template<typename T>
	void sort_filter(T* filter_data, int filter_size);

private:
	int m_grayMax;
	int m_grayMin;

};

