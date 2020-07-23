#include "pch.h"
#include "ImageSegmentation.h"
#include "ColorTrans.h"
#include "split.h"
#include <vector>

using std::vector;

void regionGrow(BYTE* srcImage, UINT iHeight, UINT iWidth, BYTE* growImage,
	CPoint pt, BYTE threshold, BYTE lowerBind = 0, BYTE upperBind = 255)
{
	/*
	*/
	//1

	//2.
	CPoint pToGrowing;                       //待生长点位置
	BYTE growValue = 0;	//待生长点灰度值
	BYTE srcValue = 0;	//生长起点灰度值
	BYTE curValue = 0;	//当前生长点灰度值
	UINT iSizeImage = iHeight * iWidth;

	//生长方向顺序数据
	int DIR[8][2] = { {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0} };
	vector<CPoint> growPtVector;                     //生长点栈
	growPtVector.push_back(pt);                         //将生长点压入栈中
	growImage[pt.x * iWidth + pt.y] = 255;              //标记生长点
	srcValue = srcImage[pt.x * iWidth + pt.y];         //记录生长点的灰度值

	while (!growPtVector.empty())						//生长栈不为空则生长
	{
		pt = growPtVector.back();
		growPtVector.pop_back();

		//分别对八个方向上的点进行生长
		for (int i = 0; i < 8; i++)
		{
			pToGrowing.x = pt.x + DIR[i][0];
			pToGrowing.y = pt.y + DIR[i][1];
			//检查是否边缘点
			if (pToGrowing.x < 0 || pToGrowing.y < 0 || pToGrowing.x >= iHeight || pToGrowing.y >= iWidth)
				continue;
			growValue = growImage[pToGrowing.x * iWidth + pToGrowing.y];       //当前待生长点的灰度值
			srcValue = srcImage[pt.x * iWidth + pt.y];
			if (growValue == 0)                    //如果标记点还没有被生长
			{
				curValue = srcImage[pToGrowing.x * iWidth + pToGrowing.y];
				if (curValue <= upperBind && curValue >= lowerBind)
				{
					if (abs(srcValue - curValue) < threshold)                   //在阈值范围内则生长
					{
						growImage[pToGrowing.x * iWidth + pToGrowing.y] = 255;      //标记为白色
						growPtVector.push_back(pToGrowing);                 //将下一个生长点压入栈中
					}
				}
			}
		}
	}
}


ImageSegmentation::ImageSegmentation()
{
}

ImageSegmentation::~ImageSegmentation()
{
}

bool ImageSegmentation::ManSeg(MyDIB& orgin_image, MyDIB& trans_image, int iThreshold)
{
	/*
	1.保证输入图像存在，且为8位或24位
	2.创建转换后的图像
	3.获取灰度存入pTransData
	4.释放开辟的空间
	*/
	//1
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	//2.
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	//3
	int nLineByte = trans_image.BytesPerline();
	int index = 0;
	int tmp_r, tmp_g, tmp_b;
	float fGray;
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			index = i * nLineByte + j * 3;
			orgin_image.GetPixelRGB(i + 1, j + 1, tmp_r, tmp_g, tmp_b);
			fGray = 0.299 * tmp_r + 0.587 * tmp_g + 0.114 * tmp_b;
			fGray = fGray >= iThreshold ? 255 : 0;
			pTransData[index] = pTransData[index + 1] = pTransData[index + 2] = BYTE(fGray);
		}
	}
	trans_image.SetData(pTransData);
	//4
	delete[] pTransData;
	return true;
}

int ImageSegmentation::OstuSThreshold(MyDIB& orgin_image)
{
	//步骤
	/*
	1.保证orgin_image存在图像数据，且图像数据是8bits或者24bits
	2.将原图像转变为8bits的灰度图,取原图数据
	3.获取图像属性信息
	4.统计各灰度频数
	5.定义统计变量
	6.统计直方图中像素点的总数
	7.遍历求得最优阈值（令阈值nT从0遍历到255）
	8.返回结果
	*/
	//1
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return 0;
	}
	//2
	MyDIB tmp_orgin_image;
	ColorTrans grayTrans;
	grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
	//3
	int i, j;
	BYTE* pDibBits = tmp_orgin_image.GetBits();
	int nLineByte = tmp_orgin_image.BytesPerline();
	int nWidth = tmp_orgin_image.GetWidth();
	int nHeight = tmp_orgin_image.GetHeight();
	//4
	int nGrayHistogram[256];
	memset(nGrayHistogram, 0, sizeof(int) * 256);
	int index;
	for (j = 0; j < nHeight; j++)
		for (i = 0; i < nWidth; i++)
		{
			index = nLineByte * j + i;
			nGrayHistogram[pDibBits[index]] ++;
		}
	//5				
	float u0, u1;	// c0组和c1组的均值
	float w0, w1;	// c0组和c1组的概率
	int nCount0;	// c0组的像素总数
	int nT, nBestT;	// 阈值和最佳阈值（对应方差最大时的阈值）
	float fVaria, fMaxVaria = 0;	// 方差和最大方差
	//6 
	int nSum = 0;
	for (i = 0; i < 256; i++)
		nSum += nGrayHistogram[i];
	//7
	for (nT = 0; nT < 256; nT++)
	{
		// 当阈值为nT时，计算c0组的均值和概率
		u0 = 0;
		nCount0 = 0;
		for (i = 0; i <= nT; i++)
		{
			u0 += i * nGrayHistogram[i];
			nCount0 += nGrayHistogram[i];
		}
		u0 /= nCount0;
		w0 = (float)nCount0 / nSum;

		// 当阈值为nT时，计算c1组的均值和概率
		u1 = 0;
		for (i = nT + 1; i < 256; i++)
			u1 += i * nGrayHistogram[i];
		u1 /= (nSum - nCount0);
		w1 = 1 - w0;

		// 计算两组间的方差
		fVaria = w0 * w1 * (u0 - u1) * (u0 - u1);

		// 记录最大方差和最佳阈值
		if (fVaria > fMaxVaria)
		{
			fMaxVaria = fVaria;
			nBestT = nT;
		}
	}
	//8
	return nBestT;
}

bool ImageSegmentation::RegionGrow(MyDIB& orgin_image, MyDIB& trans_image, int seed_val/*=100*/)
{
	//步骤
	/*
	1.保证orgin_image存在图像数据，且图像数据是8bits或者24bits
	2.将原图像转变为8bits的灰度图,取原图数据
	3.创建转换后的图像，开辟新图数据缓存

	5.将数据存入trans_image
	6.释放开辟的空间
	*/
	//1
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2
	MyDIB tmp_orgin_image;
	ColorTrans grayTrans;
	grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
	//3
	int image_size = tmp_orgin_image.GetSizeImage();
	int img_h = tmp_orgin_image.GetHeight();
	int img_w = tmp_orgin_image.GetWidth();
	int iBytelines = tmp_orgin_image.BytesPerline();
	//4
	trans_image.Empty();
	trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
	BYTE* pTransData = new BYTE[image_size];
	memset(pTransData, 0, image_size);
	ASSERT(trans_image.GetSizeImage() == tmp_orgin_image.GetSizeImage());
	//5
	BYTE* pRegion = new BYTE[image_size];
	BYTE* pOrginData = tmp_orgin_image.GetBits();
	memset(pRegion, 0, image_size);
	//6
	//int seed = OstuSThreshold(orgin_image);
	int seed = seed_val;
	//7
	int iThreshold = 5;
	//8
	vector<CPoint> growPtVector;                     //生长点栈
	//9
	int i, j;
	int ind_img;
	int total = 0;
	while (total==0)
	{
		for (i = 0; i < img_h; i++)
		{
			for (j = 0; j < img_w; j++)
			{
				ind_img = i * img_w + j;
				if (seed == pOrginData[ind_img])
				{
					total++;
					growPtVector.push_back(CPoint(i, j));
				}
			}
		}
		if (total==0)
		{
			seed++;
		}	
	}
	//10
	CPoint pt;
	while (!growPtVector.empty())
	{
		pt = growPtVector.back();
		growPtVector.pop_back();
		regionGrow(pOrginData, img_h, iBytelines, pRegion, pt, iThreshold);
	}
	//11
	for (i=0;i<img_h;i++)
	{
		for (j=0;j<img_w;j++)
		{
			ind_img = i * img_w + j;
			pTransData[ind_img] = pRegion[ind_img];
		}
	}
	//12
	trans_image.SetData(pTransData);
	//13
	delete[] pTransData;
	delete[] pRegion;
	return true;
}

bool ImageSegmentation::RegionSplitMerge(MyDIB& orgin_image, MyDIB& trans_image)
{
	//1
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24) {
		return false;
	}
	//2
	MyDIB tmp_orgin_image;
	ColorTrans grayTrans;
	grayTrans.Color2Gray(orgin_image, tmp_orgin_image);
	//3
	int iByteLines = tmp_orgin_image.BytesPerline();
	int img_h = tmp_orgin_image.GetHeight();
	int img_w = tmp_orgin_image.GetWidth();
	unsigned int img_size = tmp_orgin_image.GetSizeImage();
	BYTE* pOrginData = tmp_orgin_image.GetBits();
	//4
	trans_image.Empty();
	trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
	BYTE* pTransData = new BYTE[img_size];
	memset(pTransData, 0, img_size*sizeof(BYTE));
	//5
	int* pData = new int[img_size];
	memset(pData, 0, img_size * sizeof(int));
	areaSplitCombine(pOrginData, iByteLines, img_h, pData);
	//6
	int index = 0;
	for (int i = 0; i < img_h; i++)
	{
		for (int j = 0; j < img_w; j++)
		{
			index = i * iByteLines + j;
			pTransData[index] = (BYTE)pData[index];
		}
	}
	//7
	trans_image.SetData(pTransData);
	//8.
	delete[] pData;
	delete[] pTransData;
	return true;
}


