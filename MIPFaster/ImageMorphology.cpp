#include "pch.h"
#include "ImageMorphology.h"
#include "global.h"
#include "ColorTrans.h"


//pStructure�Ĵ�СΪ(2*structureSize+1)*(2*structureSize)
void ImageMorphology::getStructure(int* pStructure, int structureSize, structureType st_type)
{
	if (structureSize < 0)
		return;
	int strcut_length = 2 * structureSize + 1;
	memset(pStructure, 0, strcut_length * strcut_length * sizeof(int));
	int i, j,index;
	for (i = 0; i < strcut_length; i++)
	{
		for (j = 0; j < strcut_length; j++)
		{
			index = i * strcut_length + j;
			switch (st_type) 
			{
				case square:
				{
					pStructure[index] = 1;
					break;
				}
				case diamond:
				{
					if ((i+j)>= structureSize&&(i+j)<=3*structureSize&&(j-i)<=structureSize&&(i-j)<=structureSize)
					{
						pStructure[index] = 1;
					}
					break;
				}
				case cross:
				{
					if (i==structureSize||j==structureSize)
					{
						pStructure[index] = 1;
					}
					break;
				}
				case circle:
				{
					if ((i - structureSize) * (i - structureSize) + (j - structureSize) * (j - structureSize) <= structureSize * structureSize)
					{
						pStructure[index] = 1;
					}
					break;
				}
				default:
				{
					break;
				}
			}		
		}
	}
}

ImageMorphology::ImageMorphology()
{
}

ImageMorphology::~ImageMorphology()
{
}

bool ImageMorphology::Erosion(MyDIB& orgin_image, MyDIB& trans_image, const int* pStructure, int structureSize)
{
	//����
	/*
	1.��֤orgin_image����ͼ�����ݣ���ͼ��������8bits����24bits
	2.��ԭͼ��ת��Ϊ8bits�ĻҶ�ͼ,ȡԭͼ����
	3.����ת�����ͼ�񣬿�����ͼ���ݻ���
	4.ʹ�ýṹԪ��ʴ��ģ���Ӧ����ֻҪ��0��Ϊ0
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
	BYTE* pOrginData = tmp_orgin_image.GetBits();
	//3
	trans_image.Empty();
	trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData,0,trans_image.GetSizeImage());
	ASSERT(trans_image.GetSizeImage() == tmp_orgin_image.GetSizeImage());
	//4
	int strcut_length = 2 * structureSize + 1;
	int img_h = trans_image.GetHeight();
	int img_w = trans_image.GetWidth();
	int i, j, m, n;
	int ind_im,ind_sut,ind_rel;
	for (i=0;i<img_w;i++)//i:�ڼ���
	{
		for (j=0;j<img_h;j++)//j:�ڼ���
		{
			ind_im = j * img_w + i;
			pTransData[ind_im] = pOrginData[ind_im];
			for (m=0;m<strcut_length;m++)//m:�ڼ���
			{
				for (n=0;n<strcut_length;n++)//n:�ڼ���
				{
					ind_sut = m * strcut_length + n;
					if (pStructure[ind_sut]==0)
					{
						continue;
					}
					if (j-structureSize+m<0||i-structureSize+n<0||j-structureSize+m>=img_h||i-structureSize+n>=img_w)
					{
						pTransData[ind_im] = 0;
						break;
					}
					ind_rel = (j - structureSize + m) * img_w + (i - structureSize + n);
					if (pOrginData[ind_rel]==0)
					{
						pTransData[ind_im] = 0;
						break;
					}
				}
			}
		}
	}
	//5
	trans_image.SetData(pTransData);
	//6
	delete[] pTransData;
	return true;
}

bool ImageMorphology::Dilation(MyDIB& orgin_image, MyDIB& trans_image, const int* pStructure, int structureSize)
{
	//����
/*
1.��֤orgin_image����ͼ�����ݣ���ͼ��������8bits����24bits
2.��ԭͼ��ת��Ϊ8bits�ĻҶ�ͼ,ȡԭͼ����
3.����ת�����ͼ�񣬿�����ͼ���ݻ���
4.ʹ�ýṹԪ�����͡�ģ���Ӧ����ֻҪ��1��Ϊ1
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
	BYTE* pOrginData = tmp_orgin_image.GetBits();
	//3
	trans_image.Empty();
	trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	ASSERT(trans_image.GetSizeImage() == tmp_orgin_image.GetSizeImage());
	//4
	int strcut_length = 2 * structureSize + 1;
	int img_h = trans_image.GetHeight();
	int img_w = trans_image.GetWidth();
	int i, j, m, n;
	int ind_im, ind_sut, ind_rel;
	for (i = 0; i < img_w; i++)//i:�ڼ���
	{
		for (j = 0; j < img_h; j++)//j:�ڼ���
		{
			ind_im = j * img_w + i;
			pTransData[ind_im] = pOrginData[ind_im];
			for (m = 0; m < strcut_length; m++)//m:�ڼ���
			{
				for (n = 0; n < strcut_length; n++)//n:�ڼ���
				{
					ind_sut = m * strcut_length + n;
					if (pStructure[ind_sut] == 0)
					{
						continue;
					}
					if (j - structureSize + m < 0 || i - structureSize + n < 0 || j - structureSize + m >= img_h || i - structureSize + n >= img_w)
					{
						continue;
					}
					ind_rel = (j - structureSize + m) * img_w + (i - structureSize + n);
					if (pOrginData[ind_rel] > 0)
					{
						if (structureSize==m&&structureSize==n)
							pTransData[ind_im] = pOrginData[ind_im];
						else
							pTransData[ind_im] = (unsigned char)255;
						break;
					}
				}
			}
		}
	}
	//5
	trans_image.SetData(pTransData);
	//6
	delete[] pTransData;
	return true;
}

bool ImageMorphology::Opening(MyDIB& orgin_image, MyDIB& trans_image, const int* pStructure, int structureSize)
{
	MyDIB tmp_img;
	if (Erosion(orgin_image, tmp_img, pStructure, structureSize))
	{
		return Dilation(tmp_img, trans_image, pStructure, structureSize);
	}
	return false;
}

bool ImageMorphology::Closing(MyDIB& orgin_image, MyDIB& trans_image, const int* pStructure, int structureSize)
{
	MyDIB tmp_img;
	if (Dilation(orgin_image, tmp_img, pStructure, structureSize))
	{
		return Erosion(tmp_img, trans_image, pStructure, structureSize);
	}
	return false;
}

bool ImageMorphology::Skeleton(MyDIB& orgin_image, MyDIB& trans_image, const int* pStructure, int structureSize)
{
	//����
	/*
	1.��֤orgin_image����ͼ�����ݣ���ͼ��������8bits����24bits
	2.��ԭͼ��ת��Ϊ8bits�ĻҶ�ͼ
	3.����ת�����ͼ��
	4.��ȡͼ����ز���
	5.�������ݻ��棬����ʼ��
	6.��ͼ��Ǽ�
	7.����������trans_image
	8.�ͷ��ڴ�
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
	trans_image.Empty();
	trans_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
	ASSERT(trans_image.GetSizeImage() == tmp_orgin_image.GetSizeImage());
	//4
	LONG lWidth = tmp_orgin_image.GetWidth();			// ���ԭͼ��Ŀ��
	LONG lHeight = tmp_orgin_image.GetHeight();			// ���ԭͼ��ĸ߶�
	unsigned long ulSize = tmp_orgin_image.GetSizeImage();
	LPBYTE lpDIBBits = (LPBYTE)tmp_orgin_image.GetBits();	// �ҵ�ԭͼ�����ʼλ��
	//5
	LPBYTE	lpRst_of_nEroDIBBits;		// ָ��n�θ�ʴ��������ָ��
	LPBYTE	lpRst_of_nSXDIBBits;		// ָ��n������󼯺ϲ���ָ��
	LPBYTE	lpRst_of_nSkeDIBBits;		// ָ��n�������õ��ĹǼܽ���Ļ���ָ��
												//�����ڴ��Ա���ͼ��
	lpRst_of_nEroDIBBits = new BYTE[ulSize];
	lpRst_of_nSXDIBBits = new BYTE[ulSize];
	lpRst_of_nSkeDIBBits = new BYTE[ulSize];

	memset(lpRst_of_nEroDIBBits, (BYTE)0, ulSize);
	memset(lpRst_of_nSXDIBBits, (BYTE)0, ulSize);
	memset(lpRst_of_nSkeDIBBits, (BYTE)0, ulSize);
	//6 
	long i, j;			//ѭ������
	unsigned char mark = 1;//��־����
	while (mark)
	{
		Erosion(tmp_orgin_image, pStructure, structureSize);					// �ýṹԪ�ظ�ʴԭͼ��
		lpDIBBits = tmp_orgin_image.GetBits();
		memcpy(lpRst_of_nEroDIBBits, lpDIBBits, ulSize);	// �����n�θ�ʴ���
		mark = 0;
		// ���ñ�־mark�����ж��Ƿ����whileѭ��
		for (j = 0; j < lHeight; j++)
		{
			for (i = 0; i < lWidth; i++)
			{
				if ((*(lpDIBBits + lWidth * j + i) == 255))
				{
					mark = 1;
					i = lWidth;
					j = lHeight;
				}

			}
		}

		Opening(tmp_orgin_image, pStructure, structureSize);	//������
		lpDIBBits = tmp_orgin_image.GetBits();
													// ��n�θ�ʴ�����뿪����Ĳ�
		for (j = 0; j < lHeight; j++)
		{
			for (i = 0; i < lWidth; i++)
			{
				if ((*(lpRst_of_nEroDIBBits + lWidth * j + i) == 255) && (*(lpDIBBits + lWidth * j + i) == 0))
				{
					*(lpRst_of_nSXDIBBits + lWidth * j + i) = 255;
				}

			}
		}

		//���n�������ĹǼ�
		for (j = 0; j < lHeight; j++)
		{
			for (i = 0; i < lWidth; i++)
			{
				if ((*(lpRst_of_nSkeDIBBits + lWidth * j + i) == 255) || (*(lpRst_of_nSXDIBBits + lWidth * j + i) == 255))
				{
					*(lpRst_of_nSkeDIBBits + lWidth * j + i) = 255;
				}

			}
		}

		memcpy(lpDIBBits, lpRst_of_nEroDIBBits, ulSize); // ��n�θ�ʴ�������ԭͼ��

	}

	memcpy(lpDIBBits, lpRst_of_nSkeDIBBits, ulSize);//��������õĹǼܽ�����Ƶ�ԭͼ��

	//7
	trans_image.SetData(lpDIBBits);
	//8
	delete[]	lpRst_of_nEroDIBBits;
	delete[]	lpRst_of_nSXDIBBits;
	delete[]	lpRst_of_nSkeDIBBits;
	return true;
}



//************************************
bool ImageMorphology::Erosion(MyDIB& orgin_image, const int* pStructure, int structureSize)
{
	//����
	/*
	1.��֤orgin_image����ͼ�����ݣ���ͼ��������8bits����24bits
	2.��ԭͼ��ת��Ϊ8bits�ĻҶ�ͼ,��ȡ��ز���
	3.ȡԭͼ����,����ͼ��,������ͼ���ݻ���
	4.ʹ�ýṹԪ��ʴ��ģ���Ӧ����ֻҪ��0��Ϊ0
	5.�����ݴ��orgin_image
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
	int img_h = tmp_orgin_image.GetHeight();
	int img_w = tmp_orgin_image.GetWidth();
	unsigned long img_size = tmp_orgin_image.GetSizeImage();
	BYTE* pOrginData = tmp_orgin_image.GetBits();
	//3
	orgin_image.Empty();
	orgin_image.CreateGrayDIB(CSize(img_w, img_h));
	BYTE* pTransData = new BYTE[img_size];
	memset(pTransData, 0, img_size);
	//4
	int strcut_length = 2 * structureSize + 1;
	int i, j, m, n;
	int ind_im, ind_sut, ind_rel;
	for (i = 0; i < img_w; i++)//i:�ڼ���
	{
		for (j = 0; j < img_h; j++)//j:�ڼ���
		{
			ind_im = j * img_w + i;
			pTransData[ind_im] = pOrginData[ind_im];
			for (m = 0; m < strcut_length; m++)//m:�ڼ���
			{
				for (n = 0; n < strcut_length; n++)//n:�ڼ���
				{
					ind_sut = m * strcut_length + n;
					if (pStructure[ind_sut] == 0)
					{
						continue;
					}
					if (j - structureSize + m < 0 || i - structureSize + n < 0 || j - structureSize + m >= img_h || i - structureSize + n >= img_w)
					{
						pTransData[ind_im] = 0;
						break;
					}
					ind_rel = (j - structureSize + m) * img_w + (i - structureSize + n);
					if (pOrginData[ind_rel] == 0)
					{
						pTransData[ind_im] = 0;
						break;
					}
				}
			}
		}
	}
	//5
	orgin_image.SetData(pTransData);
	//6
	delete[] pTransData;
	return true;
}

bool ImageMorphology::Dilation(MyDIB& orgin_image, const int* pStructure, int structureSize)
{
	//����
	/*
	1.��֤orgin_image����ͼ�����ݣ���ͼ��������8bits����24bits
	2.��ԭͼ��ת��Ϊ8bits�ĻҶ�ͼ,��ȡ��ز���
	3.ȡԭͼ����,����ͼ��,������ͼ���ݻ���
	4.ʹ�ýṹԪ���͡�ģ���Ӧ����ֻҪ��0��Ϊ0
	5.�����ݴ��orgin_image
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
	int img_h = tmp_orgin_image.GetHeight();
	int img_w = tmp_orgin_image.GetWidth();
	unsigned long img_size = tmp_orgin_image.GetSizeImage();
	BYTE* pOrginData = tmp_orgin_image.GetBits();
	//3
	orgin_image.Empty();
	orgin_image.CreateGrayDIB(CSize(img_w, img_h));
	BYTE* pTransData = new BYTE[img_size];
	memset(pTransData, 0, img_size);
	//4
	int strcut_length = 2 * structureSize + 1;
	int i, j, m, n;
	int ind_im, ind_sut, ind_rel;
	for (i = 0; i < img_w; i++)//i:�ڼ���
	{
		for (j = 0; j < img_h; j++)//j:�ڼ���
		{
			ind_im = j * img_w + i;
			pTransData[ind_im] = pOrginData[ind_im];
			for (m = 0; m < strcut_length; m++)//m:�ڼ���
			{
				for (n = 0; n < strcut_length; n++)//n:�ڼ���
				{
					ind_sut = m * strcut_length + n;
					if (pStructure[ind_sut] == 0)
					{
						continue;
					}
					if (j - structureSize + m < 0 || i - structureSize + n < 0 || j - structureSize + m >= img_h || i - structureSize + n >= img_w)
					{
						continue;
					}
					ind_rel = (j - structureSize + m) * img_w + (i - structureSize + n);
					if (pOrginData[ind_rel] > 0)
					{
						if (structureSize == m && structureSize == n)
							pTransData[ind_im] = pOrginData[ind_im];
						else
							pTransData[ind_im] = (unsigned char)255;
						break;
					}
				}
			}
		}
	}
	//5
	orgin_image.SetData(pTransData);
	//6
	delete[] pTransData;
	return true;
}

bool ImageMorphology::Opening(MyDIB& orgin_image, const int* pStructure, int structureSize)
{
	if (Erosion(orgin_image, pStructure, structureSize))
	{
		return Dilation(orgin_image, pStructure, structureSize);
	}
	return false;
}

bool ImageMorphology::Closing(MyDIB& orgin_image, const int* pStructure, int structureSize)
{
	if (Dilation(orgin_image, pStructure, structureSize))
	{
		return Erosion(orgin_image, pStructure, structureSize);
	}
	return false;
}

bool ImageMorphology::Skeleton(MyDIB& orgin_image, const int* pStructure, int structureSize)
{
	MyDIB tmp_orgin_image;
	if (!Skeleton(orgin_image, tmp_orgin_image, pStructure, structureSize))
	{
		return false;
	}
	orgin_image.Empty();
	orgin_image.CreateGrayDIB(tmp_orgin_image.GetDimensions());
	orgin_image.SetData(tmp_orgin_image.GetBits());
	return true;
}



//**********************************
bool ImageMorphology::Erosion(MyDIB& orgin_image, MyDIB& trans_image, int structureSize, structureType st_type)
{
	/*
	1.���ɽṹԪ
	2.��ʴ
	*/
	if (structureSize < 0)
		return false;
	int strcut_length = 2 * structureSize + 1;
	int* pStructure = new int[strcut_length * strcut_length];
	getStructure(pStructure, structureSize,st_type);
	/*TRACE("dtructureType;%d\n",st_type);
	for (int i = 0; i < strcut_length; i++)
	{
		for (int j = 0; j < strcut_length; j++)
		{
			TRACE("%d\t", pStructure[i * strcut_length + j]);
		}
		TRACE("\n");
	}*/
	int success= Erosion(orgin_image, trans_image, pStructure, structureSize);
	delete[] pStructure;
	return success;
}

bool ImageMorphology::Dilation(MyDIB& orgin_image, MyDIB& trans_image, int structureSize, structureType st_type)
{
	if (structureSize < 0)
		return false;
	int strcut_length = 2 * structureSize + 1;
	int* pStructure = new int[strcut_length * strcut_length];
	getStructure(pStructure, structureSize, st_type);
	int success = Dilation(orgin_image, trans_image, pStructure, structureSize);
	delete[] pStructure;
	return success;
}

bool ImageMorphology::Opening(MyDIB& orgin_image, MyDIB& trans_image, int structureSize, structureType st_type)
{
	if (structureSize < 0)
		return false;
	int strcut_length = 2 * structureSize + 1;
	int* pStructure = new int[strcut_length * strcut_length];
	getStructure(pStructure, structureSize, st_type);
	int success = Opening(orgin_image, trans_image, pStructure, structureSize);
	delete[] pStructure;
	return success;
}

bool ImageMorphology::Closing(MyDIB& orgin_image, MyDIB& trans_image, int structureSize, structureType st_type)
{
	if (structureSize < 0)
		return false;
	int strcut_length = 2 * structureSize + 1;
	int* pStructure = new int[strcut_length * strcut_length];
	getStructure(pStructure, structureSize, st_type);
	int success = Closing(orgin_image, trans_image, pStructure, structureSize);
	delete[] pStructure;
	return success;
}

bool ImageMorphology::Skeleton(MyDIB& orgin_image, MyDIB& trans_image, int structureSize, structureType st_type)
{
	if (structureSize < 0)
		return false;
	int strcut_length = 2 * structureSize + 1;
	int* pStructure = new int[strcut_length * strcut_length];
	getStructure(pStructure, structureSize, st_type);
	int success = Skeleton(orgin_image, trans_image, pStructure, structureSize);
	delete[] pStructure;
	return success;
}
