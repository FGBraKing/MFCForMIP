#pragma once
#include "MyDIB.h"
//ʵ�ֿռ����ͼ����ǿ����
//�������������Լ�ʵ�ֵ�MyDIB��
//ͨ���򵥵��޸ģ�Ҳ������ʵ�ֿ�ƽ̨�İ汾
//8bits�Ĳ�ͼҪ�ؽ���ɫ�壬���鷳��ͳһ�������Ĳ�ɫͼ��Ϊ24λ�����Ҷ�ͼ��Ϊ8λ����24λ���ɡ�
class SpatialEnhancement
{
public:
	SpatialEnhancement();
	~SpatialEnhancement();
	//����24λͼ���ɶԻҶ�ͼ�Ͳ�ͼ���޲��������8λ��ͼ���Ҷ�ͼ�Ĵ���ʽ����Ӽ�
	//����δ���8λ��24λͼ�ϣ���ʵ�����˺þá����Ƿ�Ҫ�ԻҶ�ͼ�Ͳ�ͼ�ֱ������������Ҳ����ԥ
	//�⼸������ʵ�ֵĳ���ֻ���Ǵ���Ҷ�ͼ����ˣ��Խ��յ�ͼ��ͳһת��Ϊ�Ҷ�ͼ���ٽ��д�������Ϊ8λ�ĻҶ�ͼ
	bool LinearTrans(MyDIB& orgin_image, MyDIB& trans_image,int from_min,int from_max,int gray_min,int gray_max);
	bool SegLinearTrans(MyDIB& orgin_image, MyDIB& trans_image, int s_min, int s_max, int d_min, int d_max);
	bool HistogramEqualizatio(MyDIB& orgin_image, MyDIB& trans_image);
	bool HistogramSpecification(MyDIB& orgin_image, MyDIB& trans_image, BYTE data_num, int* data_aim, float* freq_aim);
	//�⼸���������˲�����8λͼ��תΪ�Ҷ�ͼ���е�ͨ���Ĵ�����Ϊ8λ��ͼ������24λͼ�񣬶���ֱ�Ӷ���ͨ��������Ϊ24λͼ
	bool MinFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize);
	bool MaxFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize);
	bool MedianFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize);
	//���漸������ʵ�ֵķ�ʽ�ǣ���������8λ��24λͼͳһ����Ϊ24λͼ
	bool ImageConvolution(MyDIB& orgin_image, MyDIB& trans_image, float* pFilter, int kernelSize);
	bool PseudoColorEnhance(MyDIB& orgin_image, MyDIB& trans_image);
	bool HighPassFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize);
	bool LowPassFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize);


private:
	//ͳ�ƻҶ�Ƶ��
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

