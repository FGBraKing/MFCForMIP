#include "pch.h"
#include "ImageFourierTrans.h"
#include "ColorTrans.h"
#include "FourierTrans.h"
#include "ImageOperation.h"
#include "global.h"
#include <complex>
#include <vector>
using std::vector;
using std::complex;

ImageFourierTrans::ImageFourierTrans() {}
ImageFourierTrans::~ImageFourierTrans() {}

bool ImageFourierTrans::transform(MyDIB& orgin_image, MyDIB& trans_image)
{
	//1.保证变换的是图像是8bits或24bits
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2.计算进行快速傅里叶变换的长度
	int img_h = orgin_image.GetHeight();
	int img_w = orgin_image.GetWidth();
	int P = 1;	//高
	int M = 1;	//宽
	int h_power = 0;
	int w_power = 0;
	while (P < img_h) {
		h_power++;
		P *= 2;
	}
	while (M < img_w) {
		w_power++;
		M *= 2;
	}
	//3.对原图进行插值，并且保证处理的是灰度图,（没有按照标准步骤插入0值）
	MyDIB resized_image,gray_image;
	ColorTrans grayTrans;
	ImageOperation resizeTrans;
	if (P==img_h&&M==img_w)
	{
		grayTrans.Color2Gray(orgin_image, gray_image);
	}
	else
	{
		resizeTrans.imageResize(orgin_image, resized_image, M, P);
		grayTrans.Color2Gray(resized_image, gray_image);
	}
	BYTE* pOrginData = gray_image.GetBits();
	//4.创建转换后的图像
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());	//CSize(M, P)，M：宽，P:高
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];//图像为8位，且每行的字节数必定为4的倍数，没有补零的多余数据
	///////////////ASSERT(trans_image.GetSizeImage() == tmp_orgin_image.GetSizeImage());
	//5.将图像数据转换成傅里叶变换函数所需要的形式（根据需要补零,移中频域图像）
	int index = 0;
	int bPerline = trans_image.BytesPerline();
	size_t n = P * M;
	complex<double>* pCTSrc, * pCFSrc;
	pCTSrc = new complex<double>[n];
	pCFSrc = new complex<double>[n];
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			if ((i + j) % 2 == 0) {
				pCTSrc[index] = complex<double>((double)pOrginData[index], 0.0);
			}
			else {
				pCTSrc[index] = complex<double>((double)(pOrginData[index] * (-1)), 0.0);
			}
			pCFSrc[index] = complex<double>(0.0, 0.0);
		}
	}

	//complex<double> tmp_data(0.0, 0.0);
	//vector<complex<double>> output(n, tmp_data);
	//for (int i = 0; i < P; i++)
	//{
	//	for (int j = 0; j < M; j++)
	//	{
	//		index = i * M + j;
	//		if ((i + j) % 2 == 0)
	//		{
	//			output[index] = pOrginData[i * bPerline + j];
	//		}
	//		else
	//		{
	//			output[index] = (-1) * pOrginData[i * bPerline + j];
	//		}
	//	}
	//}

	//6.进行傅里叶变换
	FourierTrans fourierTrans;
	fourierTrans.FFT2D(pCTSrc, pCFSrc, M, P);
	//7.对频率域数据进行后处理.求模，取对数，结果存入pTransData
	/*
	//version1
	double a, b, norm2;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			//某个像素的灰度累计频率*目标灰度级数，便得到均衡后的灰度
			index = i * M + j;	//8位的灰度图
			a = pCFSrc[index].real();
			b = pCFSrc[index].imag();
			norm2 = a * a + b * b;
			norm2 = sqrt(norm2);
			//norm2 = log10(norm2) / log10(2);//取对数后绝大部分数据都在区间[0,10]
			norm2 = norm2 / 10.0;	//10为我的经验所得
			if (norm2 > 255)
				norm2 = 255;
			if (norm2 < 0)
				norm2 = 0;
			pTransData[index] = (unsigned char)(norm2);
		}
	}
	*/
	//version2
	vector<double> fft_result(M*P);
	double max_result = 0.0;
	for (size_t i=0;i<P*M;i++)
	{
		fft_result[i] = std::abs(pCFSrc[i]);
		fft_result[i] = log10(fft_result[i]+1)/log10(2);
	}
	max_result = *max_element(fft_result.begin(), fft_result.end());
	TRACE("max_result:%f\n", max_result);
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;	//8位的灰度图
			//TRACE("index:%d\tfft_result:%f\n",index,fft_result[index]);
			pTransData[index] = (unsigned char)(fft_result[index]/max_result*255.0);
		}
	}
	//8.将pTransData的数据存入图像类
	trans_image.SetData(pTransData);
	//9.释放申请的内存
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCFSrc;
	return true;
}

//因为观察到的图像数据都是取模后的正实数，只保留了图像的能量分布情况，对图像进行反变换是无法还原时域图像的，本函数没啥意义
bool ImageFourierTrans::inverseTransform(MyDIB& orgin_image, MyDIB& trans_image)
{
	//1.保证变换的是图像是8bits或24bits
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2.计算进行快速傅里叶变换的长度(对于傅里叶反变换，长度一般应已满足条件)
	int img_h = orgin_image.GetHeight();
	int img_w = orgin_image.GetWidth();
	int P = 1;	//高
	int M = 1;	//宽
	int h_power = 0;
	int w_power = 0;
	while (P < img_h) {
		h_power++;
		P *= 2;
	}
	while (M < img_w) {
		w_power++;
		M *= 2;
	}
	//3.对原图进行插值，并且保证处理的是灰度图
	MyDIB resized_image, gray_image;
	ColorTrans grayTrans;
	ImageOperation resizeTrans;
	if (P == img_h && M == img_w)
	{
		grayTrans.Color2Gray(orgin_image, gray_image);
	}
	else
	{
		resizeTrans.imageResize(orgin_image, resized_image, M, P);
		grayTrans.Color2Gray(resized_image, gray_image);
	}
	BYTE* pOrginData = gray_image.GetBits();
	//4.创建转换后的图像
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());	//CSize(M, P)，M：宽，P:高
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];//图像为8位，且每行的字节数必定为4的倍数，没有补零的多余数据
	///////////////ASSERT(trans_image.GetSizeImage() == tmp_orgin_image.GetSizeImage());
	//5.将图像数据转换成傅里叶变换函数所需要的形式（取数据，以复数形式存储）
	int index = 0;
	int bPerline = trans_image.BytesPerline();
	size_t n = P * M;
	complex<double>* pCTSrc, * pCFSrc;
	pCTSrc = new complex<double>[n];
	pCFSrc = new complex<double>[n];
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			pCTSrc[index] = complex<double>(0.0, 0.0);
			pCFSrc[index] = complex<double>((double)pOrginData[index], 0.0);
		}
	}
	//6.进行傅里叶反变换
	FourierTrans fourierTrans;
	fourierTrans.IFFT2D(pCFSrc, pCTSrc, M, P);
	//7.对反变换后时域数据进行后处理.取消移中(相对于正变换的操作，因为要求模，可以不用)，求模，取对数，结果存入pTransData
	double norm2;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			//某个像素的灰度累计频率*目标灰度级数，便得到均衡后的灰度
			index = i * M + j;	//8位的灰度图
			
			norm2 = std::abs(pCTSrc[index]);
			//norm2 = log10(norm2+1) / log10(2);
			if (norm2 > 255)
				norm2 = 255;
			if (norm2 < 0)
				norm2 = 0;
			pTransData[index] = (unsigned char)(norm2);
		}
	}
	//8.将pTransData的数据存入图像类
	trans_image.SetData(pTransData);
	//9.释放申请的内存
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCFSrc;
	return true;
}
