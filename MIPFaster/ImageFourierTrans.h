#pragma once
#include "MyDIB.h"


//���ڱ���ʵ���fft�ķ�װ����
class ImageFourierTrans
{
public:
	ImageFourierTrans();
	~ImageFourierTrans();

public:
	bool transform(MyDIB& orgin_image, MyDIB& trans_image);
	bool inverseTransform(MyDIB& orgin_image, MyDIB& trans_image);	//IDFT
};

