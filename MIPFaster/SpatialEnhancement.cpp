#include "pch.h"
#include "SpatialEnhancement.h"
#include "ColorTrans.h"
#include "math.h"
#include <algorithm>

using std::sort;
SpatialEnhancement::SpatialEnhancement()
	:m_grayMax(0)
	,m_grayMin(0)
{
}

SpatialEnhancement::~SpatialEnhancement()
{
}

bool SpatialEnhancement::LinearTrans(MyDIB& orgin_image, MyDIB& trans_image, int from_min, int from_max,int gray_min, int gray_max)
{
	ASSERT(orgin_image.m_lpImage);
	//保证变换的是灰度图
	MyDIB tmp_orgin_image;
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)
	{
		return false;
	}
	else
	{
		ColorTrans grayTrans;
		grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
	}
	//获取变换所需参数
	BYTE* pOrginData = tmp_orgin_image.GetBits();
	int imageSize = tmp_orgin_image.GetSizeImage();
	CalGrayRange(pOrginData, imageSize);//仅计算8位的
	if (from_min<m_grayMin)
	{
		from_min = m_grayMin;
	}
	if (from_max > m_grayMax)
	{
		from_max = m_grayMax;
	}
	//初始化变换后的图
	trans_image.Empty();
	trans_image.CreateGrayDIB(CSize(tmp_orgin_image.GetWidth(), tmp_orgin_image.GetHeight()));
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	//这里必须保证变换前后的图像大小一致，不然可能会因没考虑4的倍数，这个原因而导致错误
	ASSERT(tmp_orgin_image.GetSizeImage()== trans_image.GetSizeImage());
	TRACE("m_grayMin:%d,m_grayMax:%d\n",m_grayMin,m_grayMax);
	////版本1：没考虑图像必须是4的倍数，不一定每个byte都有像素数据
	//for (int i=0;i<trans_image.GetSizeImage();i++)
	//{
	//	//pTransData[i] = 0;
	//	LinearTransAlg(pOrginData[i], pTransData[i], m_grayMin, m_grayMax, gray_min, gray_max);
	//}
	//版本2
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			int index = i * ((((trans_image.GetWidth() * 8) + 31) / 32) * 4) + j;	//8位的灰度图
			LinearTransAlg(pOrginData[index], pTransData[index], from_min, from_max, gray_min, gray_max);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool SpatialEnhancement::SegLinearTrans(MyDIB& orgin_image, MyDIB& trans_image, int s_min, int s_max, int d_min, int d_max)
{
	ASSERT(orgin_image.m_lpImage);
	//保证变换的是灰度图
	MyDIB tmp_orgin_image;
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)
	{
		return false;
	}
	else
	{
		ColorTrans grayTrans;
		grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
	}
	BYTE* pOrginData = tmp_orgin_image.GetBits();
	int imageSize = tmp_orgin_image.GetSizeImage();
	CalGrayRange(pOrginData, imageSize);//仅计算8位的
	int orgin_min = m_grayMin > 0 ? m_grayMin : 0;
	int orgin_max = m_grayMax < 255 ? m_grayMax : 255;
	trans_image.Empty();
	trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	//这里必须保证变换前后的图像大小一致，不然可能会因没考虑4的倍数，这个原因而导致错误
	ASSERT(tmp_orgin_image.GetSizeImage() == trans_image.GetSizeImage());
	//版本1：没考虑图像必须是4的倍数，不一定每个byte都有像素数据
	for (int i = 0; i < trans_image.GetSizeImage(); i++)
	{
		if(pOrginData[i]< s_min)
		{ 
			LinearTransAlg(pOrginData[i], pTransData[i], orgin_min, s_min, 0, d_min);
		}
		else if (pOrginData[i]>=s_min&&pOrginData[i]<=s_max)
		{
			LinearTransAlg(pOrginData[i], pTransData[i], s_min, s_max, d_min, d_max);
		} 
		else
		{
			LinearTransAlg(pOrginData[i], pTransData[i], s_max, orgin_max, d_max, 255);
		}
	}
	//版本2
	//for (int i = 0; i < trans_image.GetHeight(); i++)
	//{
	//	for (int j = 0; j < trans_image.GetWidth(); j++)
	//	{
	//		int index = i * ((((trans_image.GetWidth() * 8) + 31) / 32) * 4) + j;	//8位的灰度图
	//		//把上面那几个if拿下来，就可以了，版本1应该没问题，就不改了
	//	}
	//}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool SpatialEnhancement::HistogramEqualizatio(MyDIB& orgin_image, MyDIB& trans_image)
{
	ASSERT(orgin_image.m_lpImage);
	//保证变换的是灰度图
	MyDIB tmp_orgin_image;
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)
	{
		return false;
	}
	else
	{
		ColorTrans grayTrans;
		grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
	}
	BYTE* pOrginData = tmp_orgin_image.GetBits();
	//创建转换后的图像
	trans_image.Empty();
	trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	ASSERT(trans_image.GetSizeImage()== tmp_orgin_image.GetSizeImage());
	TRACE("GetDimensions():%d,%d,GetSizeImage():%d\n", tmp_orgin_image.GetDimensions(), trans_image.GetSizeImage());

	int gray_num[256];
	HistogramCount(tmp_orgin_image.GetBits(), tmp_orgin_image.GetDimensions(),gray_num);
	float gray_freq[256];
	CalGrayFrequency(tmp_orgin_image.GetDimensions(), gray_num, gray_freq);
	float cum_freq[256];
	CalCumulaFrequency(gray_freq, cum_freq,256);	//得到累计频率

	int index = 0;
	int orgin_rank = 256;		//均衡前灰度级别数
	int trans_rank = 256;		//转换后灰度级别数
	for (int i=0;i<trans_image.GetHeight();i++)
	{
		for (int j=0;j<trans_image.GetWidth();j++)
		{
			//某个像素的灰度累计频率*目标灰度级数，便得到均衡后的灰度
			index = i * ((((trans_image.GetWidth() * 8) + 31) / 32) * 4) + j;	//8位的灰度图
			pTransData[index] = (BYTE)(round(cum_freq[pOrginData[index]] * (trans_rank - 1)));
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool SpatialEnhancement::HistogramSpecification(MyDIB& orgin_image, MyDIB& trans_image, BYTE data_num, int* data_aim, float* freq_aim)
{
	ASSERT(orgin_image.m_lpImage);
	//保证变换的是灰度图
	MyDIB tmp_orgin_image;
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)
	{
		return false;
	}
	else
	{
		ColorTrans grayTrans;
		grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
	}
	BYTE* pOrginData = tmp_orgin_image.GetBits();
	//创建转换后的图像
	trans_image.Empty();
	trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	ASSERT(trans_image.GetSizeImage() == tmp_orgin_image.GetSizeImage());

	int gray_num[256];
	HistogramCount(tmp_orgin_image.GetBits(), tmp_orgin_image.GetDimensions(), gray_num);
	float gray_freq[256];
	CalGrayFrequency(tmp_orgin_image.GetDimensions(), gray_num, gray_freq);
	float cum_freq[256];
	CalCumulaFrequency(gray_freq, cum_freq,256);	//得到累计频率
	float* cum_freq_aim = new float[data_num];
	CalCumulaFrequency(freq_aim, cum_freq_aim, data_num);

	int nMap[256];//映射关系
	// 确定映射关系
	for (int i = 0; i < 256; i++)
	{
		// 最接近的规定直方图灰度级索引
		int m = 0;
		// 最小差值
		float min_value = 0.1f;
		// 枚举规定直方图各灰度
		for (int j = 0; j < data_num; j++)
		{
			// 当前差值
			float now_value = 0.0f;
			//  差值计算
			if (cum_freq[i] - cum_freq_aim[j] >= 0.0f)
				now_value = cum_freq[i] - cum_freq_aim[j];
			else
				now_value = cum_freq_aim[j] - cum_freq[i];
			//TRACE("cum_freq[i]:%f,cum_freq_aim[j]:%f,now_value:%f\n", cum_freq[i],cum_freq_aim[j], now_value);
			// 寻找最接近的规定直方图灰度级
			if (now_value < min_value)
			{
				// 最接近的灰度级
				m = j;
				// 最小差值
				min_value = now_value;
			}
		}

		// 建立灰度映射表
		nMap[i] = data_aim[m];
		//TRACE("nMap[i]:%d,i:%d,m:%d\n",nMap[i],i,m);
	}

	int index = 0;
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			//某个像素的灰度累计频率*目标灰度级数，便得到均衡后的灰度
			index = i * ((((trans_image.GetWidth() * 8) + 31) / 32) * 4) + j;	//8位的灰度图
			pTransData[index] = (BYTE)nMap[pOrginData[index]];
		}
	}
	trans_image.SetData(pTransData);
	delete[] cum_freq_aim;
	delete[] pTransData;
	return true;
}

bool SpatialEnhancement::MinFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize)
{
	ASSERT(orgin_image.m_lpImage);

	int half_kernel_size = kernelSize / 2;
	int total_kernel_size = kernelSize * kernelSize;
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	else if (biBitsCount_orgin == 8)						//8位的图像，只处理灰度图
	{
		MyDIB tmp_orgin_image;
		ColorTrans grayTrans;
		grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
		BYTE* pOrginData = tmp_orgin_image.GetBits();
		trans_image.Empty();
		trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
		BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
		//这里必须保证变换前后的图像大小一致，不然可能会因没考虑4的倍数，这个原因而导致错误
		ASSERT(tmp_orgin_image.GetSizeImage() == trans_image.GetSizeImage());
		int* tmp_filter_data=new int[total_kernel_size];
		int nLineByte = ((((trans_image.GetWidth() * 8) + 31) / 32) * 4);	//8位的灰度图
		for (int i = 0; i < trans_image.GetHeight(); i++)
		{
			for (int j = 0; j < trans_image.GetWidth(); j++)
			{
				int index = i * nLineByte + j;	
				if (i<half_kernel_size || j<half_kernel_size || i>trans_image.GetHeight() - half_kernel_size || j>trans_image.GetWidth() - half_kernel_size)
				{
					pTransData[index] = pOrginData[index];
				}
				else 
				{
					for (int k=0;k<kernelSize;k++)
					{
						for (int t=0;t<kernelSize;t++)
						{
							tmp_filter_data[k * kernelSize + t] = pOrginData[(i - half_kernel_size + k)*nLineByte+(j - half_kernel_size + t)];
						}
					}
					sort_filter(tmp_filter_data, total_kernel_size);
					pTransData[index] = tmp_filter_data[0];
				}
			}
		}
		trans_image.SetData(pTransData);
		delete[] tmp_filter_data;
		delete[] pTransData;
	}
	else//24为真彩图像
	{
		trans_image.Empty();
		trans_image.CreateTrueColorDIB(orgin_image.GetDimensions());
		BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
		ASSERT(orgin_image.GetSizeImage()==trans_image.GetSizeImage());
		BYTE* pOrginData = orgin_image.GetBits();
		memcpy(pTransData, pOrginData, trans_image.GetSizeImage());
		int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
		int index = 0;
		BYTE iR = 0;
		BYTE iG = 0;
		BYTE iB = 0;
		for (int i = 0; i < trans_image.GetHeight() - kernelSize + 1; i++)
		{
			for (int j = 0; j < trans_image.GetWidth() - kernelSize + 1; j++)
			{
				index = i * nLineByte + j * 3;
				iB = pOrginData[index];
				iG = pOrginData[index + 1];
				iR = pOrginData[index + 2];
				//iB = iG = iR = 255;

				for (int w = 0; w < kernelSize; w++)
				{
					for (int h = 0; h < kernelSize; h++)
					{
						index = (i + w) * nLineByte + (j + h) * 3;
						iB = pOrginData[index] < iB ? pOrginData[index] : iB;
						iG = pOrginData[index + 1] < iG ? pOrginData[index + 1] : iG;
						iR = pOrginData[index + 2] < iR ? pOrginData[index + 2] : iR;
					}
				}
				index = (i + half_kernel_size) * nLineByte + (j + half_kernel_size) * 3;
				pTransData[index] = iB;
				pTransData[index + 1] = iG;
				pTransData[index + 2] = iR;
			}
		}
		trans_image.SetData(pTransData);
		delete[] pTransData;
	}
	return true;
}

bool SpatialEnhancement::MaxFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize)
{
	ASSERT(orgin_image.m_lpImage);

	int half_kernel_size = kernelSize / 2;
	int total_kernel_size = kernelSize * kernelSize;
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	else if (biBitsCount_orgin == 8)						//8位的图像，只处理灰度图
	{
		MyDIB tmp_orgin_image;
		ColorTrans grayTrans;
		grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
		BYTE* pOrginData = tmp_orgin_image.GetBits();
		trans_image.Empty();
		trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
		BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
		//这里必须保证变换前后的图像大小一致，不然可能会因没考虑4的倍数，这个原因而导致错误
		ASSERT(tmp_orgin_image.GetSizeImage() == trans_image.GetSizeImage());
		int* tmp_filter_data = new int[total_kernel_size];
		int nLineByte = ((((trans_image.GetWidth() * 8) + 31) / 32) * 4);	//8位的灰度图
		for (int i = 0; i < trans_image.GetHeight(); i++)
		{
			for (int j = 0; j < trans_image.GetWidth(); j++)
			{
				int index = i * nLineByte + j;
				if (i<half_kernel_size || j<half_kernel_size || i>trans_image.GetHeight() - half_kernel_size || j>trans_image.GetWidth() - half_kernel_size)
				{
					pTransData[index] = pOrginData[index];
				}
				else
				{
					for (int k = 0; k < kernelSize; k++)
					{
						for (int t = 0; t < kernelSize; t++)
						{
							tmp_filter_data[k * kernelSize + t] = pOrginData[(i - half_kernel_size + k) * nLineByte + (j - half_kernel_size + t)];
						}
					}
					sort_filter(tmp_filter_data, total_kernel_size);
					pTransData[index] = tmp_filter_data[total_kernel_size-1];
				}
			}
		}
		trans_image.SetData(pTransData);
		delete[] tmp_filter_data;
		delete[] pTransData;
	}
	else//24为真彩图像
	{
		trans_image.Empty();
		trans_image.CreateTrueColorDIB(orgin_image.GetDimensions());
		BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
		ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
		BYTE* pOrginData = orgin_image.GetBits();
		memcpy(pTransData, pOrginData, trans_image.GetSizeImage());
		int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
		int index = 0;
		BYTE iR = 0;
		BYTE iG = 0;
		BYTE iB = 0;
		for (int i = 0; i < trans_image.GetHeight() - kernelSize + 1; i++)
		{
			for (int j = 0; j < trans_image.GetWidth() - kernelSize + 1; j++)
			{
				//iB = iG = iR = 0;
				index = i * nLineByte + j * 3;
				iB = pOrginData[index];
				iG = pOrginData[index + 1];
				iR = pOrginData[index + 2];
				for (int w = 0; w < kernelSize; w++)
				{
					for (int h = 0; h < kernelSize; h++)
					{
						index = (i + w) * nLineByte + (j + h) * 3;
						iB = pOrginData[index] > iB ? pOrginData[index] : iB;
						iG = pOrginData[index + 1] > iG ? pOrginData[index + 1] : iG;
						iR = pOrginData[index + 2] > iR ? pOrginData[index + 2] : iR;
					}
				}
				index = (i + half_kernel_size) * nLineByte + (j + half_kernel_size) * 3;
				pTransData[index] = iB;
				pTransData[index + 1] = iG;
				pTransData[index + 2] = iR;
			}
		}
		trans_image.SetData(pTransData);
		delete[] pTransData;
	}
	return true;
}

bool SpatialEnhancement::MedianFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize)
{
	ASSERT(orgin_image.m_lpImage);

	int half_kernel_size = kernelSize / 2;
	int total_kernel_size = kernelSize * kernelSize;
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	else if (biBitsCount_orgin == 8)						//8位的图像，只处理灰度图
	{
		MyDIB tmp_orgin_image;
		ColorTrans grayTrans;
		grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
		BYTE* pOrginData = tmp_orgin_image.GetBits();
		trans_image.Empty();
		trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
		BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
		//这里必须保证变换前后的图像大小一致，不然可能会因没考虑4的倍数，这个原因而导致错误
		ASSERT(tmp_orgin_image.GetSizeImage() == trans_image.GetSizeImage());
		int* tmp_filter_data = new int[total_kernel_size];
		int nLineByte = ((((trans_image.GetWidth() * 8) + 31) / 32) * 4);	//8位的灰度图
		for (int i = 0; i < trans_image.GetHeight(); i++)
		{
			for (int j = 0; j < trans_image.GetWidth(); j++)
			{
				int index = i * nLineByte + j;
				if (i<half_kernel_size || j<half_kernel_size || i>trans_image.GetHeight() - half_kernel_size || j>trans_image.GetWidth() - half_kernel_size)
				{
					pTransData[index] = pOrginData[index];
				}
				else
				{
					for (int k = 0; k < kernelSize; k++)
					{
						for (int t = 0; t < kernelSize; t++)
						{
							tmp_filter_data[k * kernelSize + t] = pOrginData[(i - half_kernel_size + k) * nLineByte + (j - half_kernel_size + t)];
						}
					}
					sort_filter(tmp_filter_data, total_kernel_size);
					pTransData[index] = tmp_filter_data[total_kernel_size/2];
				}
			}
		}
		trans_image.SetData(pTransData);
		delete[] tmp_filter_data;
		delete[] pTransData;
	}
	else//24为真彩图像
	{
		trans_image.Empty();
		trans_image.CreateTrueColorDIB(orgin_image.GetDimensions());
		BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
		ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
		BYTE* pOrginData = orgin_image.GetBits();
		memcpy(pTransData, pOrginData, trans_image.GetSizeImage());
		int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
		int index = 0;
		char* pR = new char[total_kernel_size];
		char* pG = new char[total_kernel_size];
		char* pB = new char[total_kernel_size];
		int iCntKernel = 0;
		for (int i = 0; i < trans_image.GetHeight() - kernelSize + 1; i++)
		{
			for (int j = 0; j < trans_image.GetWidth() - kernelSize + 1; j++)
			{
				iCntKernel = 0;
				for (int w = 0; w < kernelSize; w++)
				{
					for (int h = 0; h < kernelSize; h++, iCntKernel++)
					{
						index = (i + w) * nLineByte + (j + h) * 3;
						pB[iCntKernel] = pOrginData[index];
						pG[iCntKernel] = pOrginData[index + 1];
						pR[iCntKernel] = pOrginData[index + 2];
					}
				}
				// 排序
				sort_filter(pB, total_kernel_size);
				sort_filter(pR, total_kernel_size);
				sort_filter(pG, total_kernel_size);
				/*sort(pB, pB + total_kernel_size);
				sort(pR, pR + total_kernel_size);
				sort(pG, pG + total_kernel_size);*/
				// 赋值
				index = (i + half_kernel_size) * nLineByte + (j + half_kernel_size) * 3;
				pTransData[index] = pB[total_kernel_size / 2];
				pTransData[index + 1] = pG[total_kernel_size / 2];
				pTransData[index + 2] = pR[total_kernel_size / 2];
			}
		}
		trans_image.SetData(pTransData);
		delete[] pR;
		delete[] pG;
		delete[] pB;
		delete[] pTransData;
	}
	return true;
}

bool SpatialEnhancement::ImageConvolution(MyDIB& orgin_image, MyDIB& trans_image, float* pFilter, int kernelSize)
{
	ASSERT(orgin_image.m_lpImage);

	int half_kernel_size = kernelSize / 2;
	int total_kernel_size = kernelSize * kernelSize;
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	else if (biBitsCount_orgin == 24)
	{
		trans_image.Empty();
		trans_image.CreateTrueColorDIB(orgin_image.GetDimensions());
		BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
		TRACE("orgin_image.GetSizeImage():%d,orgin_image.GetSizeImage():%d\n", orgin_image.GetSizeImage(), trans_image.GetSizeImage());
		ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
		BYTE* pOrginData = orgin_image.GetBits();
		//memcpy(pTransData, pOrginData, trans_image.GetSizeImage());
		memset(pTransData, 0, trans_image.GetbiSizeImage());
		int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
		int index_image = 0;
		int index_filter = 0;

		float fR = 0;
		float fG = 0;
		float fB = 0;
		for (int i = 0; i < trans_image.GetHeight() - kernelSize + 1; i++)
		{
			for (int j = 0; j < trans_image.GetWidth() - kernelSize + 1; j++)
			{
				fR = fB = fG = 0;
				for (int w = 0; w < kernelSize; w++)
				{
					for (int h = 0; h < kernelSize; h++)
					{
						index_image = (i + w) * nLineByte + (j + h) * 3;
						index_filter = w * kernelSize + h;
						fB += pOrginData[index_image] * pFilter[index_filter];
						fG += pOrginData[index_image + 1] * pFilter[index_filter];
						fR += pOrginData[index_image + 2] * pFilter[index_filter];
					}
				}
				if (fR < 0)
				{
					fR = 0;
				}
				else if (fR > 255)
				{
					fR = 255;
				}

				if (fG < 0)
				{
					fG = 0;
				}
				else if (fG > 255)
				{
					fG = 255;
				}

				if (fB < 0)
				{
					fB = 0;
				}
				else if (fB > 255)
				{
					fB = 255;
				}
				// 赋值
				index_image = (i + half_kernel_size) * nLineByte + (j + half_kernel_size) * 3;
				pTransData[index_image] = (BYTE)fB;
				pTransData[index_image + 1] = (BYTE)fG;
				pTransData[index_image + 2] = (BYTE)fR;
			}
		}
		trans_image.SetData(pTransData);
		delete[] pTransData;
	}
	else
	{
		//对于8位的图，也可以用类似的方法获取RGB值，分别对RGB三个通道进行滤波，滤波完成放回原位；也可以滤波完保存成24位的图；也可以变成灰度滤波；
		//卷积后存回8位图，需要统计不同RGB的个数，然后重建调色板，比较麻烦。村委24位图比较简单
		//对于灰度图，存成8位或24位均简单。但为了能无差别处理灰度和彩图，这里统一存为24位图
		//这里实现滤波后变成24位图的版本
		//其实对于24位的图，也可以统一用下面代码实现
		trans_image.Empty();
		trans_image.CreateTrueColorDIB(orgin_image.GetDimensions());
		BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
		TRACE("orgin_image.GetSizeImage() :%d,trans_image.GetSizeImage():%d\n", orgin_image.GetSizeImage() ,trans_image.GetSizeImage());
		//ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
		memset(pTransData,0,trans_image.GetbiSizeImage());
		int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
		int index_image = 0;
		int index_filter = 0;

		float fR = 0;
		float fG = 0;
		float fB = 0;
		int img_R = 0;
		int img_G = 0;
		int img_B = 0;
		/*for (int i = 0; i < total_kernel_size; i++)
		{
			TRACE("pFilter[%d]:%f,\n", i, pFilter[i]);
		}*/
		for (int i = 0; i < trans_image.GetHeight() - kernelSize + 1; i++)
		{
			for (int j = 0; j < trans_image.GetWidth() - kernelSize + 1; j++)
			{
				fR = fB = fG = 0;
				for (int w = 0; w < kernelSize; w++)
				{
					for (int h = 0; h < kernelSize; h++)
					{
						//TRACE("fB:%f, fG:%f, fR:%f\n", fB, fG, fR);
						index_filter = w * kernelSize + h;
						orgin_image.GetPixelRGB(i + w + 1, j + h + 1, img_R, img_G, img_B);
						//TRACE("img_B:%d, img_G:%d, img_R:%d,pFilter[%d]:%f,\n", img_B, img_G, img_R, index_filter, pFilter[index_filter]);
						fB += img_B * pFilter[index_filter];
						fG += img_G * pFilter[index_filter];
						fR += img_R * pFilter[index_filter];
					}
				}
				//TRACE("fB:%f, fG:%f, fR:%f\n", fB, fG, fR);
				if (fR < 0)
				{
					fR = 0;
				}
				else if (fR > 255)
				{
					fR = 255;
				}

				if (fG < 0)
				{
					fG = 0;
				}
				else if (fG > 255)
				{
					fG = 255;
				}

				if (fB < 0)
				{
					fB = 0;
				}
				else if (fB > 255)
				{
					fB = 255;
				}
				// 赋值
				index_image = (i + half_kernel_size) * nLineByte + (j + half_kernel_size) * 3;
				pTransData[index_image] = (BYTE)fB;
				pTransData[index_image + 1] = (BYTE)fG;
				pTransData[index_image + 2] = (BYTE)fR;
			}
		}
		trans_image.SetData(pTransData);
		delete[] pTransData;
	}
	return true;
}

bool SpatialEnhancement::PseudoColorEnhance(MyDIB& orgin_image, MyDIB& trans_image)
{
	ASSERT(orgin_image.m_lpImage);

	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	else
	{
		trans_image.Empty();
		trans_image.CreateTrueColorDIB(orgin_image.GetDimensions());
		BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
		//ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
		memset(pTransData, 0, trans_image.GetbiSizeImage());
		int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
		int index_image = 0;

		int img_R = 0;
		int img_G = 0;
		int img_B = 0;
		int img_gray = 0;

		for (int i = 0; i < trans_image.GetHeight(); i++)
		{
			for (int j = 0; j < trans_image.GetWidth(); j++)
			{
				//y=(y2-y1)/(x2-x1)*(x-x1)+y1
				index_image = i * nLineByte + j * 3;
				orgin_image.GetPixelRGB(i + 1, j + 1, img_R, img_G, img_B);		
				img_gray = 0.299 * img_R + 0.587 * img_G + 0.114 * img_B;
				if (img_gray <64)
				{
					pTransData[index_image] = 255;
					pTransData[index_image + 1] = img_gray * 4;
					pTransData[index_image + 2] = 0;

				}
				else if (img_gray <128)
				{
					pTransData[index_image] = 4*(128-img_gray); 
					pTransData[index_image + 1] = 255;
					pTransData[index_image + 2] = 0;
				}
				else if (img_gray < 192)
				{
					pTransData[index_image] = 0;
					pTransData[index_image + 1] = 255;
					pTransData[index_image + 2] = 4 * (img_gray - 128);
					//BYTE(255.0 / 64.0 * (float)(img_gray - 128));
					//(255-0)/(192-128)(gray-128)+0
				}
				else
				{
					pTransData[index_image] = 0;
					pTransData[index_image + 1] = 4 * (255- img_gray);
					pTransData[index_image + 2] = 255;
				}
			}
		}
		trans_image.SetData(pTransData);
		delete[] pTransData;
	}
	return true;
}

bool SpatialEnhancement::HighPassFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize)
{
	ASSERT(orgin_image.m_lpImage);
	if (kernelSize<1)
	{
		return false;
	}
	if ((kernelSize + 1) % 2 )
		return false;
	int half_kernel_size = kernelSize / 2;
	int total_kernel_size = kernelSize * kernelSize;
	float* pFilter=new float[total_kernel_size];
	int i, j;
	if (kernelSize==5)
	{
		for (i = 0; i < kernelSize; i++)
		{
			for (j = 0; j < kernelSize; j++)
			{
				if (i == half_kernel_size && j == half_kernel_size)
				{
					pFilter[i * kernelSize + j] = 8;
				}
				else if (i==0||j==0||j==kernelSize-1||i==kernelSize-1)
				{
					pFilter[i * kernelSize + j] = -1;
				}
				else
				{
					pFilter[i * kernelSize + j] = 1;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < kernelSize; i++)
		{
			for (j = 0; j < kernelSize; j++)
			{
				if (i == half_kernel_size && j == half_kernel_size)
				{
					pFilter[i * kernelSize + j] = total_kernel_size - 1;
				}
				else
				{
					pFilter[i * kernelSize + j] = -1;
				}
			}
		}
	}
	for (int k = 0; k < total_kernel_size; k++)
	{
		TRACE("pFilter[%d]=%f\n", k, pFilter[k]);
	}
	bool val=ImageConvolution(orgin_image,trans_image,pFilter,kernelSize);
	delete[] pFilter;
	return val;
}

bool SpatialEnhancement::LowPassFilter(MyDIB& orgin_image, MyDIB& trans_image, int kernelSize)
{
	ASSERT(orgin_image.m_lpImage);
	if (kernelSize < 1)
	{
		return false;
	}
	if ((kernelSize + 1) % 2)
		return false;
	int total_kernel_size = kernelSize * kernelSize;
	float* pFilter = new float[total_kernel_size];
	for (int i = 0; i < total_kernel_size; i++)
	{
		pFilter[i] = 1.0 / (float)total_kernel_size;
		TRACE("pFilter[i]:%f,i:%d\n", pFilter[i],i);
	}
	bool val = ImageConvolution(orgin_image, trans_image, pFilter, kernelSize);
	delete[] pFilter;
	return val;
}


////**********************************下面是私有函数，不对外开放*******************************************////
void SpatialEnhancement::HistogramCount(BYTE* pImage, CSize imageSize, int grayNum[256])
{
	memset(grayNum, 0, sizeof(int)*256);
	//TRACE("GetDimensions():%d,%d,GetSizeImage():%d\n", imageSize.cx,imageSize.cy, ((((imageSize.cx * 8) + 31) / 32) * 4)*imageSize.cy);
	//cx:width,cy:height
	int index = 0;
	for (int i=0;i<imageSize.cy;i++)
	{
		for (int j=0;j<imageSize.cx;j++)
		{
			index = i* ((((imageSize.cx * 8) + 31) / 32) * 4)+j;
			/*if (index>790270)
			{
				TRACE("index:%d,pImage[index]:%d\n", index, pImage[index]);
			}	*/
			grayNum[pImage[index]]++;
		}
	}
	//for (int i = 0; i < 256; i++)
	//{
	//	TRACE("grayNum[i]:%d\n", grayNum[i]);
	//}
}

void SpatialEnhancement::CalGrayFrequency(const CSize imageSize, const int grayNum[256], float frequency[256])
{
	int tmp_size = imageSize.cx * imageSize.cy;
	//TRACE("imageSize.cx:%d, imageSize.cy:%d,tmp_size:%d\n", imageSize.cx, imageSize.cy, tmp_size);
	for (int i=0;i<256;i++)
	{
		//TRACE("grayNum[i]:%d,tmp_size:%d\n", grayNum[i], tmp_size);
		frequency[i] = grayNum[i] / (float)tmp_size;
		//TRACE("frequency[i]:%f,grayNum[i]:%d,tmp_size:%d\n", frequency[i], grayNum[i], tmp_size);
	}
}

void SpatialEnhancement::CalCumulaFrequency(const float* frequency, float* cumu_freq, int num)
{
	if (num < 0)	return;
	for (int i=0;i<num;i++)
	{
		if (i == 0)
		{
			cumu_freq[i] = frequency[0];
		}
		else
		{
			cumu_freq[i] = cumu_freq[i - 1] + frequency[i];
		}
	}
}

void SpatialEnhancement::CalGrayRange(unsigned char* pDibData, int data_size)
{
	if (data_size<=0)
	{
		m_grayMax = 255;
		m_grayMin = 0;
		return;
	}
	int i = 0;
	int tmp_min = 255;
	int tmp_max = 0;
	for (i=0;i<data_size;i++)
	{
		if (pDibData[i] > tmp_max)
			tmp_max = pDibData[i];
		if (pDibData[i]<tmp_min)
		{
			tmp_min = pDibData[i];
		}
	}
	ASSERT(tmp_min <= tmp_max);
	m_grayMin = tmp_min;
	m_grayMax = tmp_max;
}

void SpatialEnhancement::LinearTransAlg(unsigned char orgin_pixel, unsigned char& trans_pixel, 
										unsigned char orgin_min, unsigned char orgin_max, 
										unsigned char trans_min, unsigned char trans_max)
{
	if (orgin_pixel > orgin_max)
		trans_pixel = trans_max;
	else if (orgin_pixel < orgin_min)
		trans_pixel = trans_min;
	else
	{
		trans_pixel = (float)(trans_max - trans_min) / (orgin_max - orgin_min) * (orgin_pixel - orgin_min) + trans_min;
	}
}

template <typename T>
void SpatialEnhancement::sort_filter(T* filter_data, int filter_size)
{
	/*for (int k=0;k<filter_size;k++)
	{
		TRACE("start%%filter_data[%d]:%d\n",k,filter_data[k]);
	}*/
	T temp = 0;
	for (int j = filter_size - 1; j >= 0; j--)
	{
		for (int i = 0; i < j; i++)
		{
			if (filter_data[i] > filter_data[i + 1])
			{
				temp = filter_data[i];
				filter_data[i] = filter_data[i + 1];
				filter_data[i + 1] = temp;
			}
		}
	}
	/*for (int k = 0; k < filter_size; k++)
	{
		TRACE("end%%filter_data[%d]:%d\n", k, filter_data[k]);
	}*/
}



////////////////////////////////////8位灰度图的模板
	/*获取图像数据，准备转换后的数据*/
//ASSERT(orgin_image.m_lpImage);
////保证变换的是灰度图
//MyDIB tmp_orgin_image;
//int biBitsCount_orgin = orgin_image.GetbiBitCount();
//if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)
//{
//	return false;
//}
//else
//{
//	ColorTrans grayTrans;
//	grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
//}
//BYTE* pOrginData = tmp_orgin_image.GetBits();
//trans_image.Empty();
//trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
//BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
////这里必须保证变换前后的图像大小一致，不然可能会因没考虑4的倍数，这个原因而导致错误
//ASSERT(tmp_orgin_image.GetSizeImage() == trans_image.GetSizeImage());
//
//for (int i = 0; i < trans_image.GetHeight(); i++)
//{
//	for (int j = 0; j < trans_image.GetWidth(); j++)
//	{
//		//计算变换后的像素值
//		index = i * ((((trans_image.GetWidth() * 8) + 31) / 32) * 4) + j;	//8位的灰度图
//		pTransData[index] = .......
//	}
//}
//trans_image.SetData(pTransData);
//delete[] pTransData;
//return true;