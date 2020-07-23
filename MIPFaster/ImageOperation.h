#pragma once
#include "MyDIB.h"
//实现图像的加减乘除运算
//实现与或非预算
//实现图像的镜像、旋转、缩放运算
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
	//成员变量接口
	void setImagePara(const float alpha_in, const float beta_in);
	void getImagePara(float& alpha_out, float& beta_out);
	//算术运算
	bool imageAdd(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	bool imageSub(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	bool imageMul(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	bool imageDiv(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	//逻辑运算
	bool imageAnd(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	bool imageOr(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	bool imageNot(MyDIB& orgin_image, MyDIB& trans_image);
	bool imageXor(MyDIB& orgin_image1, MyDIB& orgin_image2, MyDIB& trans_image);
	//几何运算
	//这里旋转只采用最近邻插值，如需要更好的效果，可改为双线性插值，参考resize算法
	bool imageRot(MyDIB& orgin_image, MyDIB& trans_image, double degree, bool flag=true);	//true:顺时针 flase：逆时针
	bool imageRot90(MyDIB& orgin_image, MyDIB& trans_image);//顺时针旋转90度	
	bool imageRot270(MyDIB& orgin_image, MyDIB& trans_image);//顺时针旋转270度
	bool iamgeTransposeMain(MyDIB& orgin_image, MyDIB& trans_image);//主对角线转置，注意，图像的第一行是显示的最后一行
	bool iamgeTransposeSub(MyDIB& orgin_image, MyDIB& trans_image);//副对角线转置，注意，图像的第一行是显示的最后一行
	bool imageRot180(MyDIB& orgin_image, MyDIB& trans_image);//顺时针旋转180度
	bool imageMirror(MyDIB& orgin_image, MyDIB& trans_image, filpType flag= filpOrgin);	//true:左右翻 false:上下翻

	bool imageResize(MyDIB& orgin_image, MyDIB& trans_image, int aim_width, int aim_height, resizeType flag=resize_bilinear);
protected:
	//限制m_alpha和m_beta的范围为[0,1]
	float m_alpha;
	float m_beta;
};

