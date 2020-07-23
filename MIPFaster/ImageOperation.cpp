#include "pch.h"
#include "ImageOperation.h"
#include "ColorTrans.h"
#include "global.h"

ImageOperation::ImageOperation()
	:m_alpha(0.5)
	,m_beta(0.5)
{
}

ImageOperation::~ImageOperation()
{
}

void ImageOperation::setImagePara(const float alpha_in, const float beta_in)
{
	m_alpha = alpha_in;
	m_beta = beta_in;
}

void ImageOperation::getImagePara(float& alpha_out, float& beta_out)
{
	alpha_out = m_alpha;
	beta_out = m_beta;
}

bool ImageOperation::imageAdd(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1!=biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	/*if (orgin_image1.GetbiSizeImage()!=orgin_image2.GetbiSizeImage())
	{
		return false;
	}*/
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͬ���Ϊ24λͼ
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24λͼ
	int index = 0;
	int tmp_r1,tmp_g1,tmp_b1;
	int tmp_r2,tmp_g2,tmp_b2;
	float t_r, t_g, t_b;
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			index = i * nLineByte + j * 3;
			orgin_image1.GetPixelRGB(i + 1, j + 1, tmp_r1, tmp_g1, tmp_b1);
			orgin_image2.GetPixelRGB(i + 1, j + 1, tmp_r2, tmp_g2, tmp_b2);
			t_r = m_alpha * tmp_r1 + m_beta * tmp_r2;
			t_g = m_alpha * tmp_g1 + m_beta * tmp_g2;
			t_b = m_alpha * tmp_b1 + m_beta * tmp_b2;

			pTransData[index] = t_b < 0 ? 0 : (t_b > 255 ? 255 : BYTE(t_b));
			pTransData[index + 1] = t_g < 0 ? 0 : (t_g > 255 ? 255 : BYTE(t_g));
			pTransData[index + 2] = t_r < 0 ? 0 : (t_r > 255 ? 255 : BYTE(t_r));
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageSub(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͬ���Ϊ24λͼ
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24λͼ
	int index = 0;
	int tmp_r1, tmp_g1, tmp_b1;
	int tmp_r2, tmp_g2, tmp_b2;
	float t_r, t_g, t_b;
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			index = i * nLineByte + j * 3;
			orgin_image1.GetPixelRGB(i + 1, j + 1, tmp_r1, tmp_g1, tmp_b1);
			orgin_image2.GetPixelRGB(i + 1, j + 1, tmp_r2, tmp_g2, tmp_b2);
			t_r = m_alpha * tmp_r1 - m_beta * tmp_r2;
			t_g = m_alpha * tmp_g1 - m_beta * tmp_g2;
			t_b = m_alpha * tmp_b1 - m_beta * tmp_b2;


			pTransData[index] = t_b < 0 ? 0 : (t_b > 255 ? 255 : BYTE(t_b));
			pTransData[index + 1] = t_g < 0 ? 0 : (t_g > 255 ? 255 : BYTE(t_g));
			pTransData[index + 2] = t_r < 0 ? 0 : (t_r > 255 ? 255 : BYTE(t_r));
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageMul(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͬ���Ϊ24λͼ
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24λͼ
	int index = 0;
	int tmp_r1, tmp_g1, tmp_b1;
	int tmp_r2, tmp_g2, tmp_b2;
	float t_r, t_g, t_b;
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			index = i * nLineByte + j * 3;
			orgin_image1.GetPixelRGB(i + 1, j + 1, tmp_r1, tmp_g1, tmp_b1);
			orgin_image2.GetPixelRGB(i + 1, j + 1, tmp_r2, tmp_g2, tmp_b2);
			t_r = m_alpha * tmp_r1 * m_beta * tmp_r2;
			t_g = m_alpha * tmp_g1 * m_beta * tmp_g2;
			t_b = m_alpha * tmp_b1 * m_beta * tmp_b2;


			pTransData[index] = t_b < 0 ? 0 : (t_b > 255 ? 255 : BYTE(t_b));
			pTransData[index + 1] = t_g < 0 ? 0 : (t_g > 255 ? 255 : BYTE(t_g));
			pTransData[index + 2] = t_r < 0 ? 0 : (t_r > 255 ? 255 : BYTE(t_r));
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageDiv(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͬ���Ϊ24λͼ
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24λͼ
	int index = 0;
	int tmp_r1, tmp_g1, tmp_b1;
	int tmp_r2, tmp_g2, tmp_b2;
	float t_r, t_g, t_b;
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			index = i * nLineByte + j * 3;
			orgin_image1.GetPixelRGB(i + 1, j + 1, tmp_r1, tmp_g1, tmp_b1);
			orgin_image2.GetPixelRGB(i + 1, j + 1, tmp_r2, tmp_g2, tmp_b2);
			t_r = (m_alpha * tmp_r1) / (m_beta * tmp_r2+0.1);
			t_g = (m_alpha * tmp_g1) / (m_beta * tmp_g2+0.1);
			t_b = (m_alpha * tmp_b1) / (m_beta * tmp_b2+0.1);


			pTransData[index] = t_b < 0 ? 0 : (t_b > 255 ? 255 : BYTE(t_b));
			pTransData[index + 1] = t_g < 0 ? 0 : (t_g > 255 ? 255 : BYTE(t_g));
			pTransData[index + 2] = t_r < 0 ? 0 : (t_r > 255 ? 255 : BYTE(t_r));
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageAnd(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͬ���Ϊ24λͼ
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24λͼ
	int index = 0;
	int tmp_r1, tmp_g1, tmp_b1;
	int tmp_r2, tmp_g2, tmp_b2;
	float t_r, t_g, t_b;
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			index = i * nLineByte + j * 3;
			orgin_image1.GetPixelRGB(i + 1, j + 1, tmp_r1, tmp_g1, tmp_b1);
			orgin_image2.GetPixelRGB(i + 1, j + 1, tmp_r2, tmp_g2, tmp_b2);
			t_r = tmp_r1 & tmp_r2;
			t_g = tmp_g1 & tmp_g2;
			t_b = tmp_b1 & tmp_b2;

			pTransData[index] = BYTE(t_b);
			pTransData[index + 1] = BYTE(t_g);
			pTransData[index + 2] = BYTE(t_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageOr(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͬ���Ϊ24λͼ
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24λͼ
	int index = 0;
	int tmp_r1, tmp_g1, tmp_b1;
	int tmp_r2, tmp_g2, tmp_b2;
	float t_r, t_g, t_b;
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			index = i * nLineByte + j * 3;
			orgin_image1.GetPixelRGB(i + 1, j + 1, tmp_r1, tmp_g1, tmp_b1);
			orgin_image2.GetPixelRGB(i + 1, j + 1, tmp_r2, tmp_g2, tmp_b2);
			t_r = tmp_r1 | tmp_r2;
			t_g = tmp_g1 | tmp_g2;
			t_b = tmp_b1 | tmp_b2;

			pTransData[index] = BYTE(t_b);
			pTransData[index + 1] = BYTE(t_g);
			pTransData[index + 2] = BYTE(t_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageNot(MyDIB& orgin_image, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͳһ��Ϊ24λͼ
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin == 8) {}
	if (biBitsCount_orgin == 24)
	{
		ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24λͼ
	int index = 0;
	int tmp_r, tmp_g, tmp_b;
	float t_r, t_g, t_b;
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			index = i * nLineByte + j * 3;
			orgin_image.GetPixelRGB(i + 1, j + 1, tmp_r, tmp_g, tmp_b);
			t_r = ~tmp_r;
			t_g = ~tmp_g;
			t_b = ~tmp_b;


			pTransData[index] = BYTE(t_b);
			pTransData[index + 1] = BYTE(t_g);
			pTransData[index + 2] = BYTE(t_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageXor(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͬ���Ϊ24λͼ
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24λͼ
	int index = 0;
	int tmp_r1, tmp_g1, tmp_b1;
	int tmp_r2, tmp_g2, tmp_b2;
	float t_r, t_g, t_b;
	for (int i = 0; i < trans_image.GetHeight(); i++)
	{
		for (int j = 0; j < trans_image.GetWidth(); j++)
		{
			index = i * nLineByte + j * 3;
			orgin_image1.GetPixelRGB(i + 1, j + 1, tmp_r1, tmp_g1, tmp_b1);
			orgin_image2.GetPixelRGB(i + 1, j + 1, tmp_r2, tmp_g2, tmp_b2);
			t_r = tmp_r1 ^ tmp_r2;
			t_g = tmp_g1 ^ tmp_g2;
			t_b = tmp_b1 ^ tmp_b2;

			pTransData[index] = BYTE(t_b);
			pTransData[index + 1] = BYTE(t_g);
			pTransData[index + 2] = BYTE(t_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageRot(MyDIB& orgin_image, MyDIB& trans_image, double degree, bool flag/*=true*/)
{
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͳһ��Ϊ24λͼ
	trans_image.Empty();
	int img_w = orgin_image.GetWidth();
	int img_h = orgin_image.GetHeight();
	double angle = degree * PI / 180.;  //��ת�Ƕ�
	
	double sinf = sin(angle);
	double cosf = cos(angle);
	if (!flag)
	{
		sinf = sinf * (-1);
	}
	//int max_side = max(img_h,img_w);
	//int max_side = sqrt(img_w*img_w+img_h*img_h+10);
	int rotateH = img_w * fabs(sinf) + img_h * fabs(cosf);
	int	rotateW = img_w * fabs(cosf) + img_h * fabs(sinf);
	/*rotateH = max_side;
	rotateW = max_side;*/
	
	/*int tmp1 = ceil(abs(max_side * sinf));
	int tmp2 = ceil(abs(max_side * cosf));
	max_side = max(tmp1, tmp2);*/

	trans_image.CreateTrueColorDIB(CSize(rotateW, rotateH));
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0x00, trans_image.GetSizeImage());
	int nLineByte = trans_image.BytesPerline();	//24λͼ
	int index = 0;
	int tmp_r, tmp_g, tmp_b;
	for (int i = 0; i < rotateH; i++)//��ͼ��ĵ�i��
	{
		for (int j = 0; j < rotateW; j++)
		{		
			double ix = (j - rotateW * 0.5) * cosf - (i - rotateH * 0.5) * sinf + double(img_w) * 0.5;
			double iy = (i - rotateH * 0.5) * cosf + (j - rotateW * 0.5) * sinf + double(img_h) * 0.5;
			if (ix<0||iy<0||ix>img_w-1||iy>img_h-1)
			{
				continue;
			}
			else
			{

			}
			//����ֻ��������ڲ�ֵ������Ҫ���õ�Ч�����ɸ�Ϊ˫���Բ�ֵ���ο�resize�㷨
			//TRACE("ix:%d,iy:%d,imgH:%d,imgW:%d\n",int(ix),int(iy),img_h,img_w);
			//��ͼ��ĵ�i�У���j�У���ԭͼ��ĵ�j�еڣ�w-i����
			orgin_image.GetPixelRGB(int(iy)+1, int(ix)+1, tmp_r, tmp_g, tmp_b);
			index = i * nLineByte + j * 3;
			pTransData[index] = BYTE(tmp_b);
			pTransData[index + 1] = BYTE(tmp_g);
			pTransData[index + 2] = BYTE(tmp_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageRot90(MyDIB& orgin_image, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͳһ��Ϊ24λͼ
	trans_image.Empty();

	int img_w = orgin_image.GetWidth();
	int img_h = orgin_image.GetHeight();
	trans_image.CreateTrueColorDIB(CSize(img_h, img_w));

	if (biBitsCount_orgin == 8)
	{
	}
	if (biBitsCount_orgin == 24)
	{
		//ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
	}

	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());

	int nLineByte = trans_image.BytesPerline();	//24λͼ
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_w; i++)//��ͼ��ĵ�i��
	{
		for (int j = 0; j < img_h; j++)
		{
			index = i * nLineByte + j * 3;
			//��ͼ��ĵ�i�У���j�У���ԭͼ��ĵ�j�еڣ�w-i)��
			orgin_image.GetPixelRGB(j+1, img_w - i, tmp_r, tmp_g, tmp_b);
			pTransData[index] = BYTE(tmp_b);
			pTransData[index + 1] = BYTE(tmp_g);
			pTransData[index + 2] = BYTE(tmp_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageRot180(MyDIB& orgin_image, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͳһ��Ϊ24λͼ
	trans_image.Empty();

	int img_w = orgin_image.GetWidth();
	int img_h = orgin_image.GetHeight();
	trans_image.CreateTrueColorDIB(CSize(img_w, img_h));

	if (biBitsCount_orgin == 8)
	{
	}
	if (biBitsCount_orgin == 24)
	{
		ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
	}

	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());

	int nLineByte = trans_image.BytesPerline();	//24λͼ
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_h; i++)//��ͼ��ĵ�i��
	{
		for (int j = 0; j < img_w; j++)
		{
			index = i * nLineByte + j * 3;
			//��ͼ��ĵ�i�У���j�У���ԭͼ��ĵ�j�еڣ�w-i����
			orgin_image.GetPixelRGB(img_h - i, img_w - i, tmp_r, tmp_g, tmp_b);
			pTransData[index] = BYTE(tmp_b);
			pTransData[index + 1] = BYTE(tmp_g);
			pTransData[index + 2] = BYTE(tmp_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageRot270(MyDIB& orgin_image, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͳһ��Ϊ24λͼ
	trans_image.Empty();

	int img_w = orgin_image.GetWidth();
	int img_h = orgin_image.GetHeight();
	trans_image.CreateTrueColorDIB(CSize(img_h, img_w));

	if (biBitsCount_orgin == 8)
	{
	}
	if (biBitsCount_orgin == 24)
	{
		//ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
	}

	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());

	int nLineByte = trans_image.BytesPerline();	//24λͼ
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_w; i++)//��ͼ��ĵ�i��
	{
		for (int j = 0; j < img_h; j++)
		{
			index = i * nLineByte + j * 3;
			//��ͼ��ĵ�i�У���j�У���ԭͼ��ĵ�j�еڣ�w-i����
			orgin_image.GetPixelRGB(img_h - j , i+1, tmp_r, tmp_g, tmp_b);
			pTransData[index] = BYTE(tmp_b);
			pTransData[index + 1] = BYTE(tmp_g);
			pTransData[index + 2] = BYTE(tmp_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::iamgeTransposeMain(MyDIB& orgin_image, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͳһ��Ϊ24λͼ
	trans_image.Empty();

	int img_w = orgin_image.GetWidth();
	int img_h = orgin_image.GetHeight();
	trans_image.CreateTrueColorDIB(CSize(img_h, img_w));

	if (biBitsCount_orgin == 8)
	{
	}
	if (biBitsCount_orgin == 24)
	{
		//ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
	}

	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());

	int nLineByte = trans_image.BytesPerline();	//24λͼ
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_w; i++)//��ͼ��ĵ�i��
	{
		for (int j = 0; j < img_h; j++)
		{
			index = i * nLineByte + j * 3;
			//��ͼ��ĵ�i�У���j�У���ԭͼ��ĵ�j�еڣ�w-i����
			orgin_image.GetPixelRGB(img_h - j, img_w - i, tmp_r, tmp_g, tmp_b);
			pTransData[index] = BYTE(tmp_b);
			pTransData[index + 1] = BYTE(tmp_g);
			pTransData[index + 2] = BYTE(tmp_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::iamgeTransposeSub(MyDIB& orgin_image, MyDIB& trans_image)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͳһ��Ϊ24λͼ
	trans_image.Empty();

	int img_w = orgin_image.GetWidth();
	int img_h = orgin_image.GetHeight();
	trans_image.CreateTrueColorDIB(CSize(img_h, img_w));

	if (biBitsCount_orgin == 8)
	{
	}
	if (biBitsCount_orgin == 24)
	{
		//ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
	}

	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());

	int nLineByte = trans_image.BytesPerline();	//24λͼ
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_w; i++)//��ͼ��ĵ�i��
	{
		for (int j = 0; j < img_h; j++)
		{
			index = i * nLineByte + j * 3;
			//��ͼ��ĵ�i�У���j�У���ԭͼ��ĵ�j�е�i��
			orgin_image.GetPixelRGB(j + 1, i + 1, tmp_r, tmp_g, tmp_b);
			pTransData[index] = BYTE(tmp_b);
			pTransData[index + 1] = BYTE(tmp_g);
			pTransData[index + 2] = BYTE(tmp_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageMirror(MyDIB& orgin_image, MyDIB& trans_image, filpType flag/*=filpOrgin*/)	//true:���ҷ� false:���·�
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͳһ��Ϊ24λͼ
	trans_image.Empty();
	
	int img_w = orgin_image.GetWidth();
	int img_h = orgin_image.GetHeight();
	trans_image.CreateTrueColorDIB(CSize(img_w, img_h));

	if (biBitsCount_orgin == 8)
	{	
	}
	if (biBitsCount_orgin == 24)
	{
		ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
	}	
	
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());

	int nLineByte = trans_image.BytesPerline();	//24λͼ
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_h; i++)
	{
		for (int j = 0; j < img_w; j++)
		{
			index = i * nLineByte + j * 3;
			if (flag==filpHorizontal)
			{
				orgin_image.GetPixelRGB(i + 1, img_w - j, tmp_r, tmp_g, tmp_b);
			}
			else if(flag==filpVertical)
			{
				orgin_image.GetPixelRGB(img_h - i, j + 1, tmp_r, tmp_g, tmp_b);
			}
			else if (flag==filpDiagonal)//��ת180
			{
				orgin_image.GetPixelRGB(img_h - i, img_w - j, tmp_r, tmp_g, tmp_b);
			}
			else
			{
				orgin_image.GetPixelRGB(i + 1, j + 1, tmp_r, tmp_g, tmp_b);
			}
			pTransData[index] = BYTE(tmp_b);
			pTransData[index + 1] = BYTE(tmp_g);
			pTransData[index + 2] = BYTE(tmp_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}

bool ImageOperation::imageResize(MyDIB& orgin_image, MyDIB& trans_image, int aim_width, int aim_height, resizeType flag)
{
	//�����������ش������ͣ�ʹ������Ҫ�ڵ���ǰȷ���������
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//ֻ����8λ��24λͼ
	{
		return false;
	}
	if (aim_height<=0||aim_width<=0||aim_height>2*orgin_image.GetHeight()||aim_width>2*orgin_image.GetWidth())
	{
		return false;
	}

	//���ۼ�λͼ���Ƿ��ɫ��������ɺ�ͳһ��Ϊ24λͼ
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(CSize(aim_width, aim_height));
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	int nLineByte = trans_image.BytesPerline();	//24λͼ
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	int tmp_ra, tmp_ga, tmp_ba;
	int tmp_rb, tmp_gb, tmp_bb;
	int tmp_rc, tmp_gc, tmp_bc;
	int tmp_rd, tmp_gd, tmp_bd;
	int src_height = orgin_image.GetHeight();
	int src_width = orgin_image.GetWidth();
	double width_scale = double(src_width) / aim_width;	//1/width_scaleΪ�Ŵ����
	double height_scale = double(src_height) / aim_height;

	//SrcX=(dstX+0.5)* (srcWidth/dstWidth) -0.5
	//SrcY=(dstY+0.5) * (srcHeight/dstHeight)-0.5����Դͼ���Ŀ��ͼ�񼸺����Ķ���

	for (int i = 0; i < aim_height; i++)
	{
		//double orgin_h = i * height_scale;//(height_scale-1)/2+i * height_scale;
		double orgin_h = (i+0.5) * height_scale - 0.5;		//����ͼ������
		orgin_h = orgin_h < 0 ? 0 : (orgin_h > ((double)src_height -1) ? ((double)src_height - 1) : orgin_h);//�ڼ���
		int orgin_hl = (int)(floor(orgin_h));
		int orgin_hh = (int)(ceil(orgin_h));
		double distance_h = orgin_h - orgin_hl;
		ASSERT(distance_h>=0&& distance_h<=1);
		for (int j = 0; j < aim_width; j++)
		{	
			//double orgin_w = j * width_scale;//(width_scale-1)/2+j*width_scale;
			double orgin_w = (width_scale - 1) * 0.5 + j * width_scale;//(width_scale-1)/2+j*width_scale;//����ͼ������
			orgin_w = orgin_w < 0 ? 0 : (orgin_w > ((double)src_width -1) ? ((double)src_width -1) : orgin_w);	//�ڼ���
			if (flag == resize_bilinear)//˫���Բ�ֵ
			{
				int orgin_wl = (int)(floor(orgin_w));
				int orgin_wh = (int)(ceil(orgin_w));
				//ȡ�������������������
				double distance_w = orgin_w - orgin_wl;	
				ASSERT(distance_w >= 0 && distance_w <= 1);
				//ȡ����ֵ
				orgin_image.GetPixelRGB(orgin_hl + 1, orgin_wl + 1, tmp_ra, tmp_ga, tmp_ba);//����
				orgin_image.GetPixelRGB(orgin_hl + 1, orgin_wh + 1, tmp_rb, tmp_gb, tmp_bb);//����
				orgin_image.GetPixelRGB(orgin_hh + 1, orgin_wl + 1, tmp_rc, tmp_gc, tmp_bc);//����
				orgin_image.GetPixelRGB(orgin_hh + 1, orgin_wh + 1, tmp_rd, tmp_gd, tmp_bd);//����
				/* //˫���Բ�ֵ
				value0 = (src_x_1 - src_x) * src[src_y_0, src_x_0, n] + (src_x - src_x_0) * src[src_y_0, src_x_1, n]
				value1 = (src_x_1 - src_x) * src[src_y_1, src_x_0, n] + (src_x - src_x_0) * src[src_y_1, src_x_1, n]
				dst[dst_y, dst_x, n] = int((src_y_1 - src_y) * value0 + (src_y - src_y_0) * value1)*/
				//	//˫���Բ�ֵ
				//double	value0 = (1 - distance_w) * tmp_ra + distance_w * tmp_rb;
				//double	value1 = (1 - distance_w) * tmp_rc + distance_w * tmp_rd;
				//tmp_r = int((1 - distance_h) * value0 + (distance_h)*value1);
				tmp_r = round(tmp_ra * (1 - distance_w) * (1 - distance_h) +	
					tmp_rb * distance_w * (1 - distance_h) +
					tmp_rc * distance_h * (1 - distance_w) +					
					tmp_rd * distance_h * distance_w);
				tmp_g = round(tmp_ga * (1 - distance_w) * (1 - distance_h) +
					tmp_gb * distance_w * (1 - distance_h) +
					tmp_gc * distance_h * (1 - distance_w) +
					tmp_gd * distance_h * distance_w);
				tmp_b = round(tmp_ba * (1 - distance_w) * (1 - distance_h) +
					tmp_bb * distance_w * (1 - distance_h) +
					tmp_bc * distance_h * (1 - distance_w) +
					tmp_bd * distance_h * distance_w);
			}
			else if (flag == resize_nearest)
			{
				unsigned src_x = unsigned(round(orgin_w));
				unsigned src_y = unsigned(round(orgin_h));
				orgin_image.GetPixelRGB(src_y, src_x, tmp_r, tmp_g, tmp_b);
			}
			else
			{
				return false;
			}
			index = i * nLineByte + j * 3;
			pTransData[index] = BYTE(tmp_b);
			pTransData[index + 1] = BYTE(tmp_g);
			pTransData[index + 2] = BYTE(tmp_r);
		}
	}
	trans_image.SetData(pTransData);
	delete[] pTransData;
	return true;
}


