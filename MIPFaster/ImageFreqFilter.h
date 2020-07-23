#pragma once
#include "MyDIB.h"

//频率域滤波算法，其中傅里叶变换为了简单均由1维的代替了2维的，其实不太合理，后期有时间可以改
class ImageFreqFilter
{
public:
	ImageFreqFilter();
	~ImageFreqFilter();

public:
	bool FreqHighPassFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0);
	bool FreqLowPassFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0);
	bool FreqButworthHighPassFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0, const int& rank);
	bool FreqButworthLowPassFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0, const int& rank);
	//同态滤波，下面实现中，yH取1，yL取0，滤波器直接使用巴特沃斯高通滤波器
	bool FreqHomomorphicFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0, const int& rank);
};

