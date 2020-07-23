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
	CPoint pToGrowing;                       //��������λ��
	BYTE growValue = 0;	//��������Ҷ�ֵ
	BYTE srcValue = 0;	//�������Ҷ�ֵ
	BYTE curValue = 0;	//��ǰ������Ҷ�ֵ
	UINT iSizeImage = iHeight * iWidth;

	//��������˳������
	int DIR[8][2] = { {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0} };
	vector<CPoint> growPtVector;                     //������ջ
	growPtVector.push_back(pt);                         //��������ѹ��ջ��
	growImage[pt.x * iWidth + pt.y] = 255;              //���������
	srcValue = srcImage[pt.x * iWidth + pt.y];         //��¼������ĻҶ�ֵ

	while (!growPtVector.empty())						//����ջ��Ϊ��������
	{
		pt = growPtVector.back();
		growPtVector.pop_back();

		//�ֱ�԰˸������ϵĵ��������
		for (int i = 0; i < 8; i++)
		{
			pToGrowing.x = pt.x + DIR[i][0];
			pToGrowing.y = pt.y + DIR[i][1];
			//����Ƿ��Ե��
			if (pToGrowing.x < 0 || pToGrowing.y < 0 || pToGrowing.x >= iHeight || pToGrowing.y >= iWidth)
				continue;
			growValue = growImage[pToGrowing.x * iWidth + pToGrowing.y];       //��ǰ��������ĻҶ�ֵ
			srcValue = srcImage[pt.x * iWidth + pt.y];
			if (growValue == 0)                    //�����ǵ㻹û�б�����
			{
				curValue = srcImage[pToGrowing.x * iWidth + pToGrowing.y];
				if (curValue <= upperBind && curValue >= lowerBind)
				{
					if (abs(srcValue - curValue) < threshold)                   //����ֵ��Χ��������
					{
						growImage[pToGrowing.x * iWidth + pToGrowing.y] = 255;      //���Ϊ��ɫ
						growPtVector.push_back(pToGrowing);                 //����һ��������ѹ��ջ��
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
	1.��֤����ͼ����ڣ���Ϊ8λ��24λ
	2.����ת�����ͼ��
	3.��ȡ�Ҷȴ���pTransData
	4.�ͷſ��ٵĿռ�
	*/
	//1
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();
	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//ֻ����8λ��24λͼ
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
	//����
	/*
	1.��֤orgin_image����ͼ�����ݣ���ͼ��������8bits����24bits
	2.��ԭͼ��ת��Ϊ8bits�ĻҶ�ͼ,ȡԭͼ����
	3.��ȡͼ��������Ϣ
	4.ͳ�Ƹ��Ҷ�Ƶ��
	5.����ͳ�Ʊ���
	6.ͳ��ֱ��ͼ�����ص������
	7.�������������ֵ������ֵnT��0������255��
	8.���ؽ��
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
	float u0, u1;	// c0���c1��ľ�ֵ
	float w0, w1;	// c0���c1��ĸ���
	int nCount0;	// c0�����������
	int nT, nBestT;	// ��ֵ�������ֵ����Ӧ�������ʱ����ֵ��
	float fVaria, fMaxVaria = 0;	// �������󷽲�
	//6 
	int nSum = 0;
	for (i = 0; i < 256; i++)
		nSum += nGrayHistogram[i];
	//7
	for (nT = 0; nT < 256; nT++)
	{
		// ����ֵΪnTʱ������c0��ľ�ֵ�͸���
		u0 = 0;
		nCount0 = 0;
		for (i = 0; i <= nT; i++)
		{
			u0 += i * nGrayHistogram[i];
			nCount0 += nGrayHistogram[i];
		}
		u0 /= nCount0;
		w0 = (float)nCount0 / nSum;

		// ����ֵΪnTʱ������c1��ľ�ֵ�͸���
		u1 = 0;
		for (i = nT + 1; i < 256; i++)
			u1 += i * nGrayHistogram[i];
		u1 /= (nSum - nCount0);
		w1 = 1 - w0;

		// ���������ķ���
		fVaria = w0 * w1 * (u0 - u1) * (u0 - u1);

		// ��¼��󷽲�������ֵ
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
	//����
	/*
	1.��֤orgin_image����ͼ�����ݣ���ͼ��������8bits����24bits
	2.��ԭͼ��ת��Ϊ8bits�ĻҶ�ͼ,ȡԭͼ����
	3.����ת�����ͼ�񣬿�����ͼ���ݻ���

	5.�����ݴ���trans_image
	6.�ͷſ��ٵĿռ�
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
	vector<CPoint> growPtVector;                     //������ջ
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


