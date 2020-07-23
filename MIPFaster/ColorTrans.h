#pragma once
#include "MyDIB.h"

void RGB2HSV(int r, int g, int b, float& h, float& s, float& v);
void HSV2RGB(float h, float s, float v, int& r, int& g, int& b);


class ColorTrans
{
public:
	ColorTrans();
	~ColorTrans();
	//ʹ��const���ڲ������������֮������
	//������������޸�orgin_image��ֵ
	BOOL HSVAdjust(/*const*/ MyDIB& orgin_image, MyDIB& trans_image, float Hratio, float Sratio, float Vratio);
	BOOL Color2Gray(/*const*/ MyDIB& orgin_image, MyDIB& trans_image);
	//������α��ɫ��ǿ���ú���ͨ��ָ����ͬ�Ҷȵĵ�ɫ�壬���Ҷ�ͼ��Ϊ��ͼ
	BOOL Gray2Color(/*const*/ MyDIB& orgin_image, MyDIB& trans_image, PALETTEENTRY* colorBar);
	BOOL TuerGray2Color(MyDIB& orgin_image, MyDIB& trans_image);

};

