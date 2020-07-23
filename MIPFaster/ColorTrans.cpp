#include "pch.h"
#include "ColorTrans.h"

void RGB2HSV(int r, int g, int b, float& h, float& s, float& v)
{
	//��һ��
	float R = r / 255.0f;
	float G = g / 255.0f;
	float B = b / 255.0f;

	float H = 0.0f;
	float S = 0.0f;
	float V = 0.0f;

	float RGBmax = max(max(R, G), B);
	float RGBmin = min(min(R, G), B);

	V = RGBmax * 100;

	if (RGBmax != RGBmin)
	{
		S = (((float)(RGBmax - RGBmin) / RGBmax)) * 100;

		if (R == RGBmax)
			H = (G - B) / (RGBmax - RGBmin);

		if (G == RGBmax)
			H = 2 + (B - R) / (RGBmax - RGBmin);

		if (B == RGBmax)
			H = 4 + (R - G) / (RGBmax - RGBmin);

		H = H * 60;

		if (H < 0)
			H = H + 360;
	}
	h = H;				//0-360
	s = S;				//0-100
	v = V;				//0-100
}

void HSV2RGB(float h, float s, float v, int& r, int& g, int& b)
{
	float H, S, V;
	H = h;			//0-360
	S = s;			//0-100
	V = 0.01f * v * 255;	//0-255
	float R = 0.0;
	float G = 0.0;
	float B = 0.0;
	if (S == 0)
	{
		R = V;
		G = V;
		B = V;
	}
	else
	{
		H = H / 60;
		int num = (int)H;

		float f = H - num;
		float a = (V) * (1 - (S / 100));
		float b = (V) * (1 - (S / 100) * f);
		float c = (V) * (1 - (S / 100) * (1 - f));


		switch (num)
		{
		case 0: R = V; G = c; B = a; break;
		case 1: R = b; G = V; B = a; break;
		case 2: R = a; G = V; B = c; break;
		case 3: R = a; G = b; B = V; break;
		case 4: R = c; G = a; B = V; break;
		case 5: R = V; G = a; B = b; break;
		default: break;
		}
	}
	r = int(R);
	g = int(G);
	b = int(B);
}


ColorTrans::ColorTrans()
{
}

ColorTrans::~ColorTrans()
{
}

BOOL ColorTrans::HSVAdjust(MyDIB& orgin_image, MyDIB& trans_image, float Hratio, float Sratio, float Vratio)
{
	int index = 0;
	float h, s, v;
	int iR, iG, iB;
	trans_image.Empty();
	trans_image.CreateTrueColorDIB(orgin_image.GetDimensions());
	BYTE* pDibBits = new BYTE[trans_image.GetSizeImage()];
	for (int i = 0; i < orgin_image.GetHeight(); i++)
	{
		for (int j = 0; j < orgin_image.GetWidth(); j++)
		{
			index = i * trans_image.BytesPerline() + j * 3;
			h = 0;
			s = 0;
			v = 0;
			iR = 0;
			iG = 0;
			iB = 0;
			orgin_image.GetPixelRGB(i+1, j+1, iR, iG, iB);
			// BGR˳��
			RGB2HSV(iR, iG,iB,h, s, v);
			//Hratio -360-360
			if (h + Hratio < 0)
			{
				h = 0;
			}
			else if (h + Hratio > 360)
			{
				h = 360;
			}
			else
			{
				h = h + Hratio;
			}
			//Sratio -100-100
			if (s + Sratio < 0)
			{
				s = 0;
			}
			else if (s + Sratio > 100)
			{
				s = 100;
			}
			else
			{
				s = s + Sratio;
			}

			if (v + Vratio < 0)
			{
				v = 0;
			}
			else if (v + Vratio>100)
			{
				v = 100;
			}
			else
			{
				v = v + Vratio;
			}
			HSV2RGB(h, s, v, iR, iG, iB);
			pDibBits[index] = (BYTE)iB;
			pDibBits[index + 1] = (BYTE)iG;
			pDibBits[index + 2] = (BYTE)iR;
		}
	}
	trans_image.SetData(pDibBits);
	delete[] pDibBits;
	return TRUE;
}

BOOL ColorTrans::Color2Gray(MyDIB& orgin_image, MyDIB& trans_image)
{
	int index = 0;
	int iR, iG, iB;
	int iGray;
	int biBitCount = orgin_image.GetbiBitCount();
	//ֻ����8λ��24λ��ת��
	if (biBitCount != 8 && biBitCount != 24)
		return FALSE;
	trans_image.Empty();
	trans_image.CreateGrayDIB(CSize(orgin_image.GetWidth(), orgin_image.GetHeight()));
	BYTE* pDibBits = new BYTE[trans_image.GetSizeImage()];

	for (int i = 0; i < orgin_image.GetHeight(); i++)
	{
		for (int j = 0; j < orgin_image.GetWidth(); j++)
		{
			index = i * trans_image.BytesPerline() + j;
			iR = 0;
			iG = 0;
			iB = 0;
			orgin_image.GetPixelRGB(i + 1, j + 1, iR, iG, iB);
			iGray = 0.299 * iR + 0.587 * iG + 0.114 * iB;
			pDibBits[index] = iGray;
		}
	}
	trans_image.SetData(pDibBits);
	delete[] pDibBits;
	return TRUE;
}

BOOL ColorTrans::Gray2Color(MyDIB& orgin_image, MyDIB& trans_image, PALETTEENTRY* colorBar)
{
	//��Ϊ24λ
	//����trans_image�ռ�
	//������ÿ�����ص�ĻҶ�
	//���ÿ�����ص�ĵ�ɫ��
	//��trans_image��ÿ�����ظ���RGB��ͨ����ɫֵ
	//��ʵ��
	return 0;
}

BOOL ColorTrans::TuerGray2Color(MyDIB& orgin_image, MyDIB& trans_image)
{
	/*��8λ�ĻҶ�ͼ��Ϊ24λ�ĻҶ�ͼ
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
	if (biBitsCount_orgin == 8) 
	{
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

				pTransData[index] = BYTE(fGray);
				pTransData[index + 1] = BYTE(fGray);
				pTransData[index + 2] = BYTE(fGray);
			}
		}
		trans_image.SetData(pTransData);
	}
	if (biBitsCount_orgin == 24)
	{
		ASSERT(orgin_image.GetSizeImage() == trans_image.GetSizeImage());
		trans_image.SetData(orgin_image.GetBits());
	}
	//4
	delete[] pTransData;
	return true;
}


