#pragma once
#include "MyDIB.h"
//ʵ��ͼ��ļӼ��˳�����
//ʵ������Ԥ��
//ʵ��ͼ��ľ�����ת����������
typedef enum {
	filpVertical = 0,
	filpHorizontal,
	filpDiagonal,
	filpOrgin,
	filpTypeNum
} filpType;

typedef enum {
	resize_bilinear = 0,
	resize_nearest,
	resize_typeNum
} resizeType;

class ImageOperation
{
public:
	ImageOperation();
	~ImageOperation();
	//��Ա�����ӿ�
	void setImagePara(const float alpha_in, const float beta_in);
	void getImagePara(float& alpha_out, float& beta_out);
	//��������
	bool imageAdd(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	bool imageSub(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	bool imageMul(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	bool imageDiv(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	//�߼�����
	bool imageAnd(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	bool imageOr(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	bool imageNot(MyDIB& orgin_image, MyDIB& trans_image);
	bool imageXor(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	//��������
	//������תֻ��������ڲ�ֵ������Ҫ���õ�Ч�����ɸ�Ϊ˫���Բ�ֵ���ο�resize�㷨
	bool imageRot(MyDIB& orgin_image, MyDIB& trans_image, double degree, bool flag=true);	//true:˳ʱ�� flase����ʱ��
	bool imageRot90(MyDIB& orgin_image, MyDIB& trans_image);//˳ʱ����ת90��	
	bool imageRot270(MyDIB& orgin_image, MyDIB& trans_image);//˳ʱ����ת270��
	bool iamgeTransposeMain(MyDIB& orgin_image, MyDIB& trans_image);//���Խ���ת�ã�ע�⣬ͼ��ĵ�һ������ʾ�����һ��
	bool iamgeTransposeSub(MyDIB& orgin_image, MyDIB& trans_image);//���Խ���ת�ã�ע�⣬ͼ��ĵ�һ������ʾ�����һ��
	bool imageRot180(MyDIB& orgin_image, MyDIB& trans_image);//˳ʱ����ת180��
	bool imageMirror(MyDIB& orgin_image, MyDIB& trans_image, filpType flag= filpOrgin);	//true:���ҷ� false:���·�

	bool imageResize(MyDIB& orgin_image, MyDIB& trans_image, int aim_width, int aim_height, resizeType flag=resize_bilinear);
protected:
	//����m_alpha��m_beta�ķ�ΧΪ[0,1]
	float m_alpha;
	float m_beta;
};

