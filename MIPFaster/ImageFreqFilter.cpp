#include "pch.h"
#include "ImageFreqFilter.h"
#include "global.h"
#include "ColorTrans.h"
#include "ImageOperation.h"
#include "FourierTrans.h"

#include <complex>
#include <vector>
using std::complex;
using std::vector;


ImageFreqFilter::ImageFreqFilter()
{
}

ImageFreqFilter::~ImageFreqFilter()
{
}
//理想高通滤波。只处理灰度图，彩图转换为灰度图再处理
bool ImageFreqFilter::FreqHighPassFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0)
{
	//步骤
	/*
	1.保证orgin_image存在图像数据，且图像数据是8bits或者24bits
	2.计算源图像进行快速傅里叶变换的长度，通过简单插值得到新的灰度图 
	3.将原图像转变为8bits的灰度图
	4.求出d0的取值范围，判断d0选取是否合理（省略）
	5.创建转换后的图像
	6.将图像数据转换进行傅里叶变换所需要的形式
	7.对图像进行傅里叶变换，
	8.生成频率域的滤波器
	9.频率域滤波
	10.滤波后数据进行傅里叶反变换
	11.取模
	12.将数据放缩至[0,255],存入trans_image
	13.释放开辟的空间
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
	double d_max = sqrt(float(M * M + P * P));
	//ASSERT(d0 >= 0 && d0 <= int(d_max));
	//5
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());	
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	//6
	unsigned int n = P * M;
	ASSERT(n == trans_image.GetSizeImage());
	int index = 0;
	/*
	//version1,采用的是1D的傅里叶变换，有逻辑错误，不采用
	vector<complex<double>> img_orgin(n);
	vector<complex<double>> filter_vec(n);
	for (int i=0;i<P;i++){
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			//img_orgin.push_back(complex<double>(double(pOrginData[index]), 0.0));
			img_orgin[index] = complex<double>(double(pOrginData[index]), 0.0);
			if (i * i + j * j >= d0 * d0)
				filter_vec[index] = complex<double>(1.0, 0.0);
			else
				filter_vec[index] = complex<double>(0.0, 0.0);
		}
	}
	*/
	//version2，采用2D的傅里叶变换
	complex<double>* pCTSrc;
	pCTSrc = new complex<double>[n];
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			if ((i + j) % 2 == 0) {
				pCTSrc[index] = complex<double>((double)pOrginData[index], 0.0);
			}
			else {
				pCTSrc[index] = complex<double>((double)(pOrginData[index] * (-1.0)), 0.0);
			}
		}
	}
	//7
	FourierTrans fourierTrans;
	/*
	//version1
	fourierTrans.transform(img_orgin);
	*/
	//version2
	complex<double>* pCFSrc;
	pCFSrc = new complex<double>[n];
	fourierTrans.FFT2D(pCTSrc, pCFSrc, M, P);
	//8.
	vector<complex<double>> filter_vec(n);
	int shift_x = P / 2;
	int shift_y = M / 2;
	TRACE("shift_x:%d\tshift_y:%d\n",shift_x,shift_y);
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			if ((i - shift_x) * (i - shift_x) + (j - shift_y) * (j - shift_y) >= d0* d0)
				filter_vec[index] = complex<double>(1.0, 0.0);
			else
				filter_vec[index] = complex<double>(0.0, 0.0);	
		}
	}

	//9
	//vector<complex<double>> result_vec(n);
	complex<double>* filter_result;
	filter_result = new complex<double>[n];
	for (size_t i = 0; i < n; i++)
	{
		filter_result[i] = pCFSrc[i] * filter_vec[i];
		/*if (filter_vec[i].real() < 1.0)
		{
			TRACE("this i is 0:i:%d\n",i);
		}*/
	}
	//10
	/*
	fourierTrans.inverseTransform(result_vec);
	for (size_t i = 0; i < n; i++)  // Scaling (because this FFT implementation omits it)
	{
		result_vec[i] = result_vec[i] / static_cast<double>(n);
	}
	*/
	fourierTrans.IFFT2D(filter_result, pCTSrc, M, P);
	//11
	double max_result = 0.0;
	vector<double> result_double_vec(n);
	for (size_t i = 0; i < n; i++)  // Scaling (because this FFT implementation omits it)
	{
		//TRACE("i:%d\treal[i]:%fimag[i]:%f\n", i, result_vec[i].real(),result_vec[i].imag());
		result_double_vec[i] = std::abs(pCTSrc[i]);
		/*if (result_double_vec[i]>256)
		{
			TRACE("i:%d\tresult[i]:%f\n", i, result_double_vec[i]);
		}*/
	}
	max_result = *max_element(result_double_vec.begin(), result_double_vec.end());
	TRACE("max_result:%f\n", max_result);
	//12
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;	
			pTransData[index] = (unsigned char)(result_double_vec[index]/max_result*255.0);
		}
	}
	trans_image.SetData(pTransData);
	//13
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCFSrc;
	delete[] filter_result;

	return true;
}

bool ImageFreqFilter::FreqLowPassFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0)
{
	//步骤
	/*
	1.保证orgin_image存在图像数据，且图像数据是8bits或者24bits
	2.计算源图像进行快速傅里叶变换的长度，通过简单插值得到新的灰度图
	3.将原图像转变为8bits的灰度图
	4.求出d0的取值范围，判断d0选取是否合理（省略）
	5.创建转换后的图像
	6.将图像数据转换进行傅里叶变换所需要的形式
	7.对图像进行傅里叶变换，
	8.生成频率域的滤波器
	9.频率域滤波
	10.滤波后数据进行傅里叶反变换
	11.取模
	12.将数据放缩至[0,255],存入trans_image
	13.释放开辟的空间
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
	double d_max = sqrt(float(M * M + P * P))/2;
	//5
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	//6
	unsigned int n = P * M;
	ASSERT(n == trans_image.GetSizeImage());
	int index = 0;
	complex<double>* pCTSrc;
	pCTSrc = new complex<double>[n];
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			if ((i + j) % 2 == 0) {
				pCTSrc[index] = complex<double>((double)pOrginData[index], 0.0);
			}
			else {
				pCTSrc[index] = complex<double>((double)(pOrginData[index] * (-1.0)), 0.0);
			}
		}
	}
	//7
	FourierTrans fourierTrans;
	complex<double>* pCFSrc;
	pCFSrc = new complex<double>[n];
	fourierTrans.FFT2D(pCTSrc, pCFSrc, M, P);
	//8.
	vector<complex<double>> filter_vec(n);
	int shift_x = P / 2;
	int shift_y = M / 2;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			if ((i - shift_x) * (i - shift_x) + (j - shift_y) * (j - shift_y) < d0 * d0)
				filter_vec[index] = complex<double>(1.0, 0.0);
			else
				filter_vec[index] = complex<double>(0.0, 0.0);
		}
	}
	//9
	complex<double>* pFilterResult;
	pFilterResult = new complex<double>[n];
	for (size_t i = 0; i < n; i++)
	{
		pFilterResult[i] = pCFSrc[i] * filter_vec[i];
	}
	//10
	fourierTrans.IFFT2D(pFilterResult, pCTSrc, M, P);
	//11
	double max_result = 0.0;
	vector<double> result_double_vec(n);
	/*for (size_t i = 0; i < n; i++)
	{
		result_double_vec[i] = std::abs(pCTSrc[i]);
	}*/
	std::transform(pCTSrc, pCTSrc+n, result_double_vec.begin(),
		static_cast<double(*)(const complex<double>&)>(std::abs));

	max_result = *max_element(result_double_vec.begin(), result_double_vec.end());
	TRACE("max_result:%f\n", max_result);
	//12
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			pTransData[index] = (unsigned char)(result_double_vec[index] / max_result * 255.0);
		}
	}
	trans_image.SetData(pTransData);
	//13
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCFSrc;
	delete[] pFilterResult;
	return true;
}

bool ImageFreqFilter::FreqButworthHighPassFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0, const int& rank)
{
	//步骤
/*
1.保证orgin_image存在图像数据，且图像数据是8bits或者24bits
2.计算源图像进行快速傅里叶变换的长度，通过简单插值得到新的灰度图
3.将原图像转变为8bits的灰度图
4.求出d0的取值范围，判断d0选取是否合理（省略）
5.创建转换后的图像
6.将图像数据转换进行傅里叶变换所需要的形式
7.对图像进行傅里叶变换，
8.生成频率域的滤波器
9.频率域滤波
10.滤波后数据进行傅里叶反变换
11.取模
12.将数据放缩至[0,255],存入trans_image
13.释放开辟的空间
*/
	//1
	ASSERT(orgin_image.m_lpImage);
	ASSERT(rank >= 0);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2
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
	double d_max = sqrt(float(M * M + P * P)) / 2;
	//5
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	//6
	unsigned int n = P * M;
	ASSERT(n == trans_image.GetSizeImage());
	int index = 0;
	complex<double>* pCTSrc;
	pCTSrc = new complex<double>[n];
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			if ((i + j) % 2 == 0) {
				pCTSrc[index] = complex<double>((double)pOrginData[index], 0.0);
			}
			else {
				pCTSrc[index] = complex<double>((double)(pOrginData[index] * (-1.0)), 0.0);
			}
		}
	}
	//7
	FourierTrans fourierTrans;
	complex<double>* pCFSrc;
	pCFSrc = new complex<double>[n];
	fourierTrans.FFT2D(pCTSrc, pCFSrc, M, P);
	//8.
	vector<complex<double>> filter_vec(n);
	int shift_x = P / 2;
	int shift_y = M / 2;
	double d_uv = 0.0;
	double tmp_real = 0.0;
	double tmp_imag = 0.0;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			d_uv = sqrt((i - shift_x) * (i - shift_x) + (j - shift_y) * (j - shift_y));
			tmp_real = 1.0 / (1 + pow(d0 / d_uv, 2 * rank));
			filter_vec[index] = complex<double>(tmp_real, tmp_imag);
		}
	}
	//9
	complex<double>* pFilterResult;
	pFilterResult = new complex<double>[n];
	for (size_t i = 0; i < n; i++)
	{
		pFilterResult[i] = pCFSrc[i] * filter_vec[i];
	}
	//10
	fourierTrans.IFFT2D(pFilterResult, pCTSrc, M, P);
	//11
	double max_result = 0.0;
	vector<double> result_double_vec(n);
	std::transform(pCTSrc, pCTSrc + n, result_double_vec.begin(), static_cast<double(*)(const complex<double>&)>(std::abs));
	max_result = *max_element(result_double_vec.begin(), result_double_vec.end());
	TRACE("max_result:%f\n", max_result);
	//12
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			pTransData[index] = (unsigned char)(result_double_vec[index] / max_result * 255.0);
		}
	}
	trans_image.SetData(pTransData);
	//13
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCFSrc;
	delete[] pFilterResult;
	return true;
}

bool ImageFreqFilter::FreqButworthLowPassFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0, const int& rank)
{
	//步骤
/*
1.保证orgin_image存在图像数据，且图像数据是8bits或者24bits
2.计算源图像进行快速傅里叶变换的长度，通过简单插值得到新的灰度图
3.将原图像转变为8bits的灰度图
4.求出d0的取值范围，判断d0选取是否合理（省略）
5.创建转换后的图像
6.将图像数据转换进行傅里叶变换所需要的形式
7.对图像进行傅里叶变换，
8.生成频率域的滤波器
9.频率域滤波
10.滤波后数据进行傅里叶反变换
11.取模
12.将数据放缩至[0,255],存入trans_image
13.释放开辟的空间
*/
//1
	ASSERT(orgin_image.m_lpImage);
	ASSERT(rank >= 0);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2
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
	double d_max = sqrt(float(M * M + P * P)) / 2;
	//5
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	//6
	unsigned int n = P * M;
	ASSERT(n == trans_image.GetSizeImage());
	int index = 0;
	complex<double>* pCTSrc;
	pCTSrc = new complex<double>[n];
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			if ((i + j) % 2 == 0) {
				pCTSrc[index] = complex<double>((double)pOrginData[index], 0.0);
			}
			else {
				pCTSrc[index] = complex<double>((double)(pOrginData[index] * (-1.0)), 0.0);
			}
		}
	}
	//7
	FourierTrans fourierTrans;
	complex<double>* pCFSrc;
	pCFSrc = new complex<double>[n];
	fourierTrans.FFT2D(pCTSrc, pCFSrc, M, P);
	//8.
	vector<complex<double>> filter_vec(n);
	int shift_x = P / 2;
	int shift_y = M / 2;
	double d_uv = 0.0;
	double tmp_real = 0.0;
	double tmp_imag = 0.0;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			d_uv = sqrt((i - shift_x) * (i - shift_x) + (j - shift_y) * (j - shift_y));
			tmp_real = 1.0 / (1 + pow(d_uv / d0, 2 * rank));
			filter_vec[index] = complex<double>(tmp_real, tmp_imag);
		}
	}
	//9
	complex<double>* pFilterResult;
	pFilterResult = new complex<double>[n];
	for (size_t i = 0; i < n; i++)
	{
		pFilterResult[i] = pCFSrc[i] * filter_vec[i];
	}
	//10
	fourierTrans.IFFT2D(pFilterResult, pCTSrc, M, P);
	//11
	double max_result = 0.0;
	vector<double> result_double_vec(n);
	std::transform(pCTSrc, pCTSrc + n, result_double_vec.begin(), static_cast<double(*)(const complex<double>&)>(std::abs));
	max_result = *max_element(result_double_vec.begin(), result_double_vec.end());
	TRACE("max_result:%f\n", max_result);
	//12
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			pTransData[index] = (unsigned char)(result_double_vec[index] / max_result * 255.0);
		}
	}
	trans_image.SetData(pTransData);
	//13
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCFSrc;
	delete[] pFilterResult;
	return true;
}

bool ImageFreqFilter::FreqHomomorphicFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0, const int& rank)
{
	//步骤
	/*
	1.保证orgin_image存在图像数据，且图像数据是8bits或者24bits
	2.计算源图像进行快速傅里叶变换的长度，通过简单插值得到新的灰度图
	3.将原图像转变为8bits的灰度图
	4.求出d0的取值范围，判断d0选取是否合理（省略）
	5.创建转换后的图像
	6.将图像数据转换进行傅里叶变换所需要的形式
	7.对图像进行傅里叶变换，
	8.生成频率域的滤波器
	9.频率域滤波
	10.滤波后数据进行傅里叶反变换
	11.取模,后处理
	12.将数据放缩至[0,255],存入trans_image
	13.释放开辟的空间
	*/
	//1
	ASSERT(orgin_image.m_lpImage);
	ASSERT(rank >= 0);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2
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
	double d_max = sqrt(float(M * M + P * P)) / 2;
	//5
	trans_image.Empty();
	trans_image.CreateGrayDIB(gray_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	//6
	unsigned int n = P * M;
	ASSERT(n == trans_image.GetSizeImage());
	int index = 0;
	complex<double>* pCTSrc;
	pCTSrc = new complex<double>[n];
	double tmp_orgin_data = 0.0;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			tmp_orgin_data = log((double)pOrginData[index] + 1.0);
			if ((i + j) % 2 == 0) {
				pCTSrc[index] = complex<double>(tmp_orgin_data, 0.0);
			}
			else {
				pCTSrc[index] = complex<double>(tmp_orgin_data * (-1.0), 0.0);
			}
		}
	}
	//7
	FourierTrans fourierTrans;
	complex<double>* pCFSrc;
	pCFSrc = new complex<double>[n];
	fourierTrans.FFT2D(pCTSrc, pCFSrc, M, P);
	//8.
	vector<complex<double>> filter_vec(n);
	int shift_x = P / 2;
	int shift_y = M / 2;
	double d_uv = 0.0;
	double tmp_real = 0.0;
	double tmp_imag = 0.0;
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			d_uv = sqrt((i - shift_x) * (i - shift_x) + (j - shift_y) * (j - shift_y));
			tmp_real = 1.75*(1.0 / (1 + pow(d0 / d_uv, 2 * rank)))+0.25;
			filter_vec[index] = complex<double>(tmp_real, tmp_imag);
		}
	}
	//9
	complex<double>* pFilterResult;
	pFilterResult = new complex<double>[n];
	for (size_t i = 0; i < n; i++)
	{
		pFilterResult[i] = pCFSrc[i] * filter_vec[i];
	}
	//10
	fourierTrans.IFFT2D(pFilterResult, pCTSrc, M, P);
	//11
	double max_result = 0.0;
	vector<double> result_double_vec(n);
	/*std::transform(pCTSrc, pCTSrc + n, result_double_vec.begin(), static_cast<double(*)(const complex<double>&)>(std::abs));
	std::transform(result_double_vec.begin(), result_double_vec.end(), result_double_vec.begin(),
		static_cast<double(*)(const double)>(std::exp));*/
	complex<double> tmp_abs;
	for (size_t i = 0; i < n; i++)
	{
		tmp_abs = std::exp(pCTSrc[i]) - 1.0;
		result_double_vec[i] = std::abs(tmp_abs);
	}
	max_result = *max_element(result_double_vec.begin(), result_double_vec.end());
	TRACE("max_result:%f\n", max_result);
	//12
	for (int i = 0; i < P; i++) {
		for (int j = 0; j < M; j++) {
			index = i * M + j;
			pTransData[index] = (unsigned char)(result_double_vec[index] / max_result * 255.0);
		}
	}
	trans_image.SetData(pTransData);
	//13
	delete[] pTransData;
	delete[] pCTSrc;
	delete[] pCFSrc;
	delete[] pFilterResult;
	return true;
}





//备份--使用1D傅里叶滤波的错误示例
//bool ImageFreqFilter::FreqLowPassFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& d0)
//{
//	//步骤
///*
//1.保证orgin_image存在图像数据，且图像数据是8bits或者24bits
//2.计算源图像进行快速傅里叶变换的长度，通过简单插值得到新的灰度图
//3.将原图像转变为8bits的灰度图
//4.求出d0的取值范围，判断d0选取是否合理（省略）
//5.创建转换后的图像
//6.将图像数据转换进行傅里叶变换所需要的形式
//7.对图像进行傅里叶变换，
//8.生成频率域的滤波器
//9.频率域滤波
//10.滤波后数据进行傅里叶反变换
//11.取模
//12.将数据放缩至[0,255],存入trans_image
//13.释放开辟的空间
//*/
////1
//	ASSERT(orgin_image.m_lpImage);
//	int biBitsCount_orgin = orgin_image.GetbiBitCount();
//	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
//		return false;
//	}
//	//2
//	int img_h = orgin_image.GetHeight();
//	int img_w = orgin_image.GetWidth();
//	int P = 1;	//高
//	int M = 1;	//宽
//	int h_power = 0;
//	int w_power = 0;
//	while (P < img_h) {
//		h_power++;
//		P *= 2;
//	}
//	while (M < img_w) {
//		w_power++;
//		M *= 2;
//	}
//	//3
//	MyDIB resized_image, gray_image;
//	ColorTrans grayTrans;
//	ImageOperation resizeTrans;
//if (P == img_h && M == img_w)
//{
//	grayTrans.Color2Gray(orgin_image, gray_image);
//}
//else
//{
//	resizeTrans.imageResize(orgin_image, resized_image, M, P);
//	grayTrans.Color2Gray(resized_image, gray_image);
//}
//	BYTE* pOrginData = gray_image.GetBits();
//	//4
//	double d_max = sqrt(float(M * M + P * P));
//	//ASSERT(d0 >= 0 && d0 <= int(d_max));
//	//5
//	trans_image.Empty();
//	trans_image.CreateGrayDIB(gray_image.GetDimensions());
//	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
//	//6
//	unsigned int n = P * M;
//	ASSERT(n == trans_image.GetSizeImage());
//	vector<complex<double>> img_orgin(n);
//	vector<complex<double>> filter_vec(n);
//	int index = 0;
//	for (int i = 0; i < P; i++) {
//		for (int j = 0; j < M; j++) {
//			index = i * M + j;
//			//img_orgin.push_back(complex<double>(double(pOrginData[index]), 0.0));
//			img_orgin[index] = complex<double>(double(pOrginData[index]), 0.0);
//			if (i * i + j * j >= d0 * d0)
//				filter_vec[index] = complex<double>(1.0, 0.0);
//			else
//				filter_vec[index] = complex<double>(0.0, 0.0);
//		}
//	}
//	//7
//	FourierTrans fourierTrans;
//	fourierTrans.transform(img_orgin);
//	//8.与第6步一起实现
//	/*
//	vector<complex<double>> filter_data(n);
//	for (int i = 0; i < P; i++) {
//		for (int j = 0; j < M; j++) {
//			if(i*i+j*j>d0*d0)
//
//		}
//	}
//	*/
//	//9
//	vector<complex<double>> result_vec(n);
//	for (size_t i = 0; i < n; i++)
//	{
//		result_vec[i] = img_orgin[i] * filter_vec[i];
//	}
//	//10
//	fourierTrans.inverseTransform(result_vec);
//	/*
//	for (size_t i = 0; i < n; i++)  // Scaling (because this FFT implementation omits it)
//	{
//		result_vec[i] = result_vec[i] / static_cast<double>(n);
//	}
//	*/
//	//11
//	double max_result = 0.0;
//	vector<double> result_double_vec(n);
//	for (size_t i = 0; i < n; i++)  // Scaling (because this FFT implementation omits it)
//	{
//		//TRACE("i:%d\treal[i]:%fimag[i]:%f\n", i, result_vec[i].real(),result_vec[i].imag());
//		result_double_vec[i] = std::abs(result_vec[i]);
//		/*if (result_double_vec[i]>256)
//		{
//			TRACE("i:%d\tresult[i]:%f\n", i, result_double_vec[i]);
//		}*/
//	}
//	max_result = *max_element(result_double_vec.begin(), result_double_vec.end());
//	TRACE("max_result:%f\n", max_result);
//	//12
//	for (int i = 0; i < P; i++) {
//		for (int j = 0; j < M; j++) {
//			index = i * M + j;
//			pTransData[index] = (unsigned char)(result_double_vec[index] / max_result * 255.0);
//		}
//	}
//	trans_image.SetData(pTransData);
//	//13
//	delete[] pTransData;
//	return true;
//}
