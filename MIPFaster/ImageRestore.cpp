#include "pch.h"
#include "ImageRestore.h"
#include "global.h"
#include "ColorTrans.h"
#include "ImageOperation.h"
#include "FourierTrans.h"

#include <complex>
#include <vector>
using std::complex;
using std::vector;


ImageRestore::ImageRestore()
{
}

ImageRestore::~ImageRestore()
{
}

bool ImageRestore::InverseFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& l_h)
{
	//����
/*
1.��֤orgin_image����ͼ�����ݣ���ͼ��������8bits����24bits
2.����Դͼ����п��ٸ���Ҷ�任�ĳ��ȣ�ͨ���򵥲�ֵ�õ��µĻҶ�ͼ
3.��ԭͼ��ת��Ϊ8bits�ĻҶ�ͼ
4.����ת�����ͼ��
5.��ͼ������ת�����и���Ҷ�任����Ҫ����ʽ,���˻�����ת��Ϊ���и���Ҷ�任����Ҫ����ʽ
6.��ͼ����˻��������и���Ҷ�任��
7.�õ���ԭ����
8.����Ƶ�����˲�
9.�˲������ݽ��и���Ҷ���任
10.ȡģ�������ݷ�����[0,255],����trans_image
11.�ͷſ��ٵĿռ�
*/
	//1
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2
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
	//3
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
	//4
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	//5
	unsigned int n = P * M;	
	ASSERT(n == trans_image.GetSizeImage());
	int index = 0;
	complex<double>* pCTSrc,* pCTH;
	pCTSrc = new complex<double>[n];
	pCTH = new complex<double>[n];
	double tmp_l;
	if (l_h >0)
	{
		tmp_l = 1.0 / (double(l_h) * l_h);
	}
	else
	{
		tmp_l = 0.0;
	}
	TRACE("tmp_l is :%f\n",tmp_l);
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			pCTSrc[index] = complex<double>((double)pOrginData[index], 0.0);
			if (i > (P - l_h) / 2 && i<(P + l_h) / 2 && j>(M - l_h) / 2 && j < (M + l_h) / 2)
			{
				pCTH[index] = complex<double>(tmp_l, 0.0);
				TRACE("i:%d\tj:%d\n",i,j);
			}
			else
			{
				pCTH[index] = complex<double>(0.0, 0.0);
			}		
		}
	}
	//6
	FourierTrans fourierTrans;
	complex<double>* pCFSrc, * pCFH;
	pCFSrc = new complex<double>[n];
	pCFH = new complex<double>[n];
	fourierTrans.FFT2D(pCTSrc, pCFSrc, M, P);
	fourierTrans.FFT2D(pCTH, pCFH, M, P);
	//7
	vector<complex<double>> filter_vec(n);
	double tmp_area = 0.0;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			//tmp_area = std::abs(pCFH[index]);
			tmp_area = pCFH[index].imag() * pCFH[index].imag() + pCFH[index].real() * pCFH[index].real();
			//TRACE("index:%d\ttmp_area:%f\n",index,tmp_area);
			if (tmp_area>1e-3)
			{
				filter_vec[index] = std::conj(pCFH[index]) / tmp_area;
			}
			else
			{
				if (index==0)
				{
					filter_vec[index] = complex<double>(0.0, 0.0);
				}
				else
				{
					filter_vec[index] = filter_vec[index-1];
				}
			}		
		}
	}
	//8
	complex<double>* pFilterResult;
	pFilterResult = new complex<double>[n];
	for (size_t i = 0; i < n; i++)
	{
		pFilterResult[i] = pCFSrc[i] * filter_vec[i];
	}
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			if ((i + j) % 2)
			{
				pFilterResult[index] *= -1.0;
			}
		}
	}
	//9
	fourierTrans.IFFT2D(pFilterResult, pCTSrc, M, P);
	//10
	double max_result = 0.0;
	vector<double> result_double_vec(n);
	std::transform(pCTSrc, pCTSrc + n, result_double_vec.begin(), static_cast<double(*)(const complex<double>&)>(std::abs));
	max_result = *max_element(result_double_vec.begin(), result_double_vec.end());
	TRACE("max_result:%f\n", max_result);
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			pTransData[index] = (unsigned char)(result_double_vec[index] / max_result * 255.0);
		}
	}
	trans_image.SetData(pTransData);
	//11
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCTH;
	delete[] pCFSrc;
	delete[] pCFH;
	delete[] pFilterResult;
	return true;
}

bool ImageRestore::WinnerFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& l_h, const double& k)
{
	//����
/*
1.��֤orgin_image����ͼ�����ݣ���ͼ��������8bits����24bits
2.����Դͼ����п��ٸ���Ҷ�任�ĳ��ȣ�ͨ���򵥲�ֵ�õ��µĻҶ�ͼ
3.��ԭͼ��ת��Ϊ8bits�ĻҶ�ͼ
4.����ת�����ͼ��
5.��ͼ������ת�����и���Ҷ�任����Ҫ����ʽ,���˻�����ת��Ϊ���и���Ҷ�任����Ҫ����ʽ
6.��ͼ����˻��������и���Ҷ�任��
7.�õ���ԭ����
8.����Ƶ�����˲�
9.�˲������ݽ��и���Ҷ���任
10.ȡģ�������ݷ�����[0,255],����trans_image
11.�ͷſ��ٵĿռ�
*/
//1
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2
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
	//3
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
	//4
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	//5
	unsigned int n = P * M;
	ASSERT(n == trans_image.GetSizeImage());
	int index = 0;
	complex<double>* pCTSrc, * pCTH;
	pCTSrc = new complex<double>[n];
	pCTH = new complex<double>[n];
	double tmp_l;
	if (l_h > 0)
	{
		tmp_l = 1.0 / (double(l_h) * l_h);
	}
	else
	{
		tmp_l = 0.0;
	}
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			pCTSrc[index] = complex<double>((double)pOrginData[index], 0.0);
			if (i > (P - l_h) / 2 && i<(P + l_h) / 2 && j>(M - l_h) / 2 && j < (M + l_h) / 2)
			{
				pCTH[index] = complex<double>(tmp_l, 0.0);

			}
			else
			{
				pCTH[index] = complex<double>(0.0, 0.0);
			}
			/*if ((i + j) % 2)
			{
				pCTSrc[index] *= -1.0;
				pCTH[index] *= -1.0;
			}*/
		}
	}
	//6
	FourierTrans fourierTrans;
	complex<double>* pCFSrc, * pCFH;
	pCFSrc = new complex<double>[n];
	pCFH = new complex<double>[n];
	fourierTrans.FFT2D(pCTSrc, pCFSrc, M, P);
	fourierTrans.FFT2D(pCTH, pCFH, M, P);
	//7
	vector<complex<double>> filter_vec(n);
	double tmp_area = 0.0;
	double tmp_constant = 0.0;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			//tmp_area = std::abs(pCFH[index]);
			tmp_area = pCFH[index].imag() * pCFH[index].imag() + pCFH[index].real() * pCFH[index].real();
			//TRACE("index:%d\ttmp_area:%f\n",index,tmp_area);
			tmp_constant = tmp_area / (tmp_area + k);
			if (tmp_area > 1e-3)
			{
				filter_vec[index] = std::conj(pCFH[index]) / tmp_area * tmp_constant;
			}
			else
			{
				if (index == 0)
				{
					filter_vec[index] = complex<double>(0.0, 0.0);
				}
				else
				{
					filter_vec[index] = filter_vec[index - 1];
				}
			}
		}
	}
	//8
	complex<double>* pFilterResult;
	pFilterResult = new complex<double>[n];
	for (size_t i = 0; i < n; i++)
	{
		pFilterResult[i] = pCFSrc[i] * filter_vec[i];
	}
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			if ((i+j)%2)
			{
				pFilterResult[index] *= -1.0;
			}
		}
	}
	//9
	fourierTrans.IFFT2D(pFilterResult, pCTSrc, M, P);
	//10
	double max_result = 0.0;
	vector<double> result_double_vec(n);
	std::transform(pCTSrc, pCTSrc + n, result_double_vec.begin(), static_cast<double(*)(const complex<double>&)>(std::abs));
	max_result = *max_element(result_double_vec.begin(), result_double_vec.end());
	TRACE("max_result:%f\n", max_result);
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			pTransData[index] = (unsigned char)(result_double_vec[index] / max_result * 255.0);
		}
	}
	trans_image.SetData(pTransData);
	//11
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCTH;
	delete[] pCFSrc;
	delete[] pCFH;
	delete[] pFilterResult;
	return true;
}
