#pragma once
#include "MyDIB.h"

void RGB2HSV(int r, int g, int b, float& h, float& s, float& v);
void HSV2RGB(float h, float s, float v, int& r, int& g, int& b);


class ColorTrans
{
public:
	ColorTrans();
	~ColorTrans();
	//使用const后，内部函数与变量与之不兼容
	//下面操作不可修改orgin_image的值
	BOOL HSVAdjust(/*const*/ MyDIB& orgin_image, MyDIB& trans_image, float Hratio, float Sratio, float Vratio);
	BOOL Color2Gray(/*const*/ MyDIB& orgin_image, MyDIB& trans_image);
	//区别于伪彩色增强。该函数通过指定不同灰度的调色板，将灰度图变为彩图
	BOOL Gray2Color(/*const*/ MyDIB& orgin_image, MyDIB& trans_image, PALETTEENTRY* colorBar);
	BOOL TuerGray2Color(MyDIB& orgin_image, MyDIB& trans_image);

};

