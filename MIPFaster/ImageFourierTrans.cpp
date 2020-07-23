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
	//1.��֤�任����ͼ����8bits��24bits
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2.������п��ٸ���Ҷ�任�ĳ���
	int img_h = orgin_image.GetHeight();
	int img_w = orgin_image.GetWidth();
	int P = 1;	//��
	int M = 1;	//��
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
	//3.��ԭͼ���в�ֵ�����ұ�֤������ǻҶ�ͼ,��û�а��ձ�׼�������0ֵ��
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
	//4.����ת�����ͼ��
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());	//CSize(M, P)��M����P:��
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];//ͼ��Ϊ8λ����ÿ�е��ֽ����ض�Ϊ4�ı�����û�в���Ķ�������
	///////////////ASSERT(trans_image.GetSizeImage() == tmp_orgin_image.GetSizeImage());
	//5.��ͼ������ת���ɸ���Ҷ�任��������Ҫ����ʽ��������Ҫ����,����Ƶ��ͼ��
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

	//6.���и���Ҷ�任
	FourierTrans fourierTrans;
	fourierTrans.FFT2D(pCTSrc, pCFSrc, M, P);
	//7.��Ƶ�������ݽ��к���.��ģ��ȡ�������������pTransData
	/*
	//version1
	double a, b, norm2;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			//ĳ�����صĻҶ��ۼ�Ƶ��*Ŀ��Ҷȼ�������õ������ĻҶ�
			index = i * M + j;	//8λ�ĻҶ�ͼ
			a = pCFSrc[index].real();
			b = pCFSrc[index].imag();
			norm2 = a * a + b * b;
			norm2 = sqrt(norm2);
			//norm2 = log10(norm2) / log10(2);//ȡ��������󲿷����ݶ�������[0,10]
			norm2 = norm2 / 10.0;	//10Ϊ�ҵľ�������
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
			index = i * M + j;	//8λ�ĻҶ�ͼ
			//TRACE("index:%d\tfft_result:%f\n",index,fft_result[index]);
			pTransData[index] = (unsigned char)(fft_result[index]/max_result*255.0);
		}
	}
	//8.��pTransData�����ݴ���ͼ����
	trans_image.SetData(pTransData);
	//9.�ͷ�������ڴ�
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCFSrc;
	return true;
}

//��Ϊ�۲쵽��ͼ�����ݶ���ȡģ�����ʵ����ֻ������ͼ��������ֲ��������ͼ����з��任���޷���ԭʱ��ͼ��ģ�������ûɶ����
bool ImageFourierTrans::inverseTransform(MyDIB& orgin_image, MyDIB& trans_image)
{
	//1.��֤�任����ͼ����8bits��24bits
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2.������п��ٸ���Ҷ�任�ĳ���(���ڸ���Ҷ���任������һ��Ӧ����������)
	int img_h = orgin_image.GetHeight();
	int img_w = orgin_image.GetWidth();
	int P = 1;	//��
	int M = 1;	//��
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
	//3.��ԭͼ���в�ֵ�����ұ�֤������ǻҶ�ͼ
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
	//4.����ת�����ͼ��
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());	//CSize(M, P)��M����P:��
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];//ͼ��Ϊ8λ����ÿ�е��ֽ����ض�Ϊ4�ı�����û�в���Ķ�������
	///////////////ASSERT(trans_image.GetSizeImage() == tmp_orgin_image.GetSizeImage());
	//5.��ͼ������ת���ɸ���Ҷ�任��������Ҫ����ʽ��ȡ���ݣ��Ը�����ʽ�洢��
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
	//6.���и���Ҷ���任
	FourierTrans fourierTrans;
	fourierTrans.IFFT2D(pCFSrc, pCTSrc, M, P);
	//7.�Է��任��ʱ�����ݽ��к���.ȡ������(��������任�Ĳ�������ΪҪ��ģ�����Բ���)����ģ��ȡ�������������pTransData
	double norm2;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			//ĳ�����صĻҶ��ۼ�Ƶ��*Ŀ��Ҷȼ�������õ������ĻҶ�
			index = i * M + j;	//8λ�ĻҶ�ͼ
			
			norm2 = std::abs(pCTSrc[index]);
			//norm2 = log10(norm2+1) / log10(2);
			if (norm2 > 255)
				norm2 = 255;
			if (norm2 < 0)
				norm2 = 0;
			pTransData[index] = (unsigned char)(norm2);
		}
	}
	//8.��pTransData�����ݴ���ͼ����
	trans_image.SetData(pTransData);
	//9.�ͷ�������ڴ�
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCFSrc;
	return true;
}
