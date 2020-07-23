#pragma once
#include "MyDIB.h"


//用于本次实验的fft的封装函数
class ImageFourierTrans
{
public:
	ImageFourierTrans();
	~ImageFourierTrans();

public:
	bool transform(MyDIB& orgin_image, MyDIB& trans_image);
	bool inverseTransform(MyDIB& orgin_image, MyDIB& trans_image);	//IDFT
};

