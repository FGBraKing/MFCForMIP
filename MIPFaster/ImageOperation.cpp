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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1!=biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//只处理8位和24位图
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
	//无论几位图，是否彩色，处理完成后同意存为24位图
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//只处理8位和24位图
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后同意存为24位图
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//只处理8位和24位图
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后同意存为24位图
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//只处理8位和24位图
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后同意存为24位图
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//只处理8位和24位图
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后同意存为24位图
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//只处理8位和24位图
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后同意存为24位图
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后统一存为24位图
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin == 8) {}
	if (biBitsCount_orgin == 24)
	{
		ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image1.m_lpImage);
	ASSERT(orgin_image2.m_lpImage);
	int biBitsCount_orgin1 = orgin_image1.GetbiBitCount();
	int biBitsCount_orgin2 = orgin_image2.GetbiBitCount();
	if (biBitsCount_orgin1 != biBitsCount_orgin2)
	{
		return false;
	}
	if (biBitsCount_orgin1 != 8 && biBitsCount_orgin1 != 24)	//只处理8位和24位图
	{
		return false;
	}
	if (orgin_image1.GetSizeImage() != orgin_image2.GetSizeImage())
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后同意存为24位图
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image1.GetDimensions());
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	if (biBitsCount_orgin1 == 8) {}
	if (biBitsCount_orgin1 == 24)
	{
		ASSERT(orgin_image1.GetSizeImage() == trans_image.GetSizeImage());
	}
	int nLineByte = ((((trans_image.GetWidth() * 24) + 31) / 32) * 4);	//24位图
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

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后统一存为24位图
	trans_image.Empty();
	int img_w = orgin_image.GetWidth();
	int img_h = orgin_image.GetHeight();
	double angle = degree * PI / 180.;  //旋转角度
	
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
	int nLineByte = trans_image.BytesPerline();	//24位图
	int index = 0;
	int tmp_r, tmp_g, tmp_b;
	for (int i = 0; i < rotateH; i++)//新图像的第i行
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
			//这里只采用最近邻插值，如需要更好的效果，可改为双线性插值，参考resize算法
			//TRACE("ix:%d,iy:%d,imgH:%d,imgW:%d\n",int(ix),int(iy),img_h,img_w);
			//新图像的第i行，第j列，是原图像的第j行第（w-i）列
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后统一存为24位图
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

	int nLineByte = trans_image.BytesPerline();	//24位图
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_w; i++)//新图像的第i行
	{
		for (int j = 0; j < img_h; j++)
		{
			index = i * nLineByte + j * 3;
			//新图像的第i行，第j列，是原图像的第j行第（w-i)列
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后统一存为24位图
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

	int nLineByte = trans_image.BytesPerline();	//24位图
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_h; i++)//新图像的第i行
	{
		for (int j = 0; j < img_w; j++)
		{
			index = i * nLineByte + j * 3;
			//新图像的第i行，第j列，是原图像的第j行第（w-i）列
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后统一存为24位图
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

	int nLineByte = trans_image.BytesPerline();	//24位图
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_w; i++)//新图像的第i行
	{
		for (int j = 0; j < img_h; j++)
		{
			index = i * nLineByte + j * 3;
			//新图像的第i行，第j列，是原图像的第j行第（w-i）列
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后统一存为24位图
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

	int nLineByte = trans_image.BytesPerline();	//24位图
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_w; i++)//新图像的第i行
	{
		for (int j = 0; j < img_h; j++)
		{
			index = i * nLineByte + j * 3;
			//新图像的第i行，第j列，是原图像的第j行第（w-i）列
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后统一存为24位图
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

	int nLineByte = trans_image.BytesPerline();	//24位图
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	for (int i = 0; i < img_w; i++)//新图像的第i行
	{
		for (int j = 0; j < img_h; j++)
		{
			index = i * nLineByte + j * 3;
			//新图像的第i行，第j列，是原图像的第j行第i列
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

bool ImageOperation::imageMirror(MyDIB& orgin_image, MyDIB& trans_image, filpType flag/*=filpOrgin*/)	//true:左右翻 false:上下翻
{
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	//无论几位图，是否彩色，处理完成后统一存为24位图
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

	int nLineByte = trans_image.BytesPerline();	//24位图
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
			else if (flag==filpDiagonal)//旋转180
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
	//本函数不返回错误类型，使用者需要在调用前确保运算可行
	ASSERT(orgin_image.m_lpImage);
	int biBitsCount_orgin = orgin_image.GetbiBitCount();

	if (biBitsCount_orgin != 8 && biBitsCount_orgin != 24)	//只处理8位和24位图
	{
		return false;
	}
	if (aim_height<=0||aim_width<=0||aim_height>2*orgin_image.GetHeight()||aim_width>2*orgin_image.GetWidth())
	{
		return false;
	}

	//无论几位图，是否彩色，处理完成后统一存为24位图
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(CSize(aim_width, aim_height));
	BYTE* pTransData = new BYTE[trans_image.GetSizeImage()];
	memset(pTransData, 0, trans_image.GetSizeImage());
	int nLineByte = trans_image.BytesPerline();	//24位图
	int index = 0;
	int tmp_r, tmp_g, tmp_b;

	int tmp_ra, tmp_ga, tmp_ba;
	int tmp_rb, tmp_gb, tmp_bb;
	int tmp_rc, tmp_gc, tmp_bc;
	int tmp_rd, tmp_gd, tmp_bd;
	int src_height = orgin_image.GetHeight();
	int src_width = orgin_image.GetWidth();
	double width_scale = double(src_width) / aim_width;	//1/width_scale为放大比例
	double height_scale = double(src_height) / aim_height;

	//SrcX=(dstX+0.5)* (srcWidth/dstWidth) -0.5
	//SrcY=(dstY+0.5) * (srcHeight/dstHeight)-0.5，将源图像和目标图像几何中心对齐

	for (int i = 0; i < aim_height; i++)
	{
		//double orgin_h = i * height_scale;//(height_scale-1)/2+i * height_scale;
		double orgin_h = (i+0.5) * height_scale - 0.5;		//对齐图像中心
		orgin_h = orgin_h < 0 ? 0 : (orgin_h > ((double)src_height -1) ? ((double)src_height - 1) : orgin_h);//第几行
		int orgin_hl = (int)(floor(orgin_h));
		int orgin_hh = (int)(ceil(orgin_h));
		double distance_h = orgin_h - orgin_hl;
		ASSERT(distance_h>=0&& distance_h<=1);
		for (int j = 0; j < aim_width; j++)
		{	
			//double orgin_w = j * width_scale;//(width_scale-1)/2+j*width_scale;
			double orgin_w = (width_scale - 1) * 0.5 + j * width_scale;//(width_scale-1)/2+j*width_scale;//对齐图像中心
			orgin_w = orgin_w < 0 ? 0 : (orgin_w > ((double)src_width -1) ? ((double)src_width -1) : orgin_w);	//第几列
			if (flag == resize_bilinear)//双线性插值
			{
				int orgin_wl = (int)(floor(orgin_w));
				int orgin_wh = (int)(ceil(orgin_w));
				//取整后距离左边整数点距离
				double distance_w = orgin_w - orgin_wl;	
				ASSERT(distance_w >= 0 && distance_w <= 1);
				//取像素值
				orgin_image.GetPixelRGB(orgin_hl + 1, orgin_wl + 1, tmp_ra, tmp_ga, tmp_ba);//左上
				orgin_image.GetPixelRGB(orgin_hl + 1, orgin_wh + 1, tmp_rb, tmp_gb, tmp_bb);//右上
				orgin_image.GetPixelRGB(orgin_hh + 1, orgin_wl + 1, tmp_rc, tmp_gc, tmp_bc);//左下
				orgin_image.GetPixelRGB(orgin_hh + 1, orgin_wh + 1, tmp_rd, tmp_gd, tmp_bd);//右下
				/* //双线性插值
				value0 = (src_x_1 - src_x) * src[src_y_0, src_x_0, n] + (src_x - src_x_0) * src[src_y_0, src_x_1, n]
				value1 = (src_x_1 - src_x) * src[src_y_1, src_x_0, n] + (src_x - src_x_0) * src[src_y_1, src_x_1, n]
				dst[dst_y, dst_x, n] = int((src_y_1 - src_y) * value0 + (src_y - src_y_0) * value1)*/
				//	//双线性插值
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


