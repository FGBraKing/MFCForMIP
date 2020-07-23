#pragma once
#include "MyDIB.h"

//Ƶ�����˲��㷨�����и���Ҷ�任Ϊ�˼򵥾���1ά�Ĵ�����2ά�ģ���ʵ��̫����������ʱ����Ը�
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
	//̬ͬ�˲�������ʵ���У�yHȡ1��yLȡ0���˲���ֱ��ʹ�ð�����˹��ͨ�˲���
	bool FreqHomomorphicFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0, const int& rank);
};

