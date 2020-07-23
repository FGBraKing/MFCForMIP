#pragma once
#include "MyDIB.h"

/*ͼ����̬ѧ�������㷨��������ʴ�����͡������㡢�����㡢�Ǽ�ϸ����*/
typedef enum {
	square = 0,
	diamond,
	cross,
	circle,
	strucrureTypeNum
} structureType;


class ImageMorphology
{
public:
	ImageMorphology();
	~ImageMorphology();

public:
	bool Erosion(MyDIB& orgin_image, MyDIB& trans_image, const int* pStructure, int structureSize);
	bool Dilation(MyDIB& orgin_image, MyDIB& trans_image, const int* pStructure, int structureSize);
	bool Opening(MyDIB& orgin_image, MyDIB& trans_image, const int* pStructure, int structureSize);
	bool Closing(MyDIB& orgin_image, MyDIB& trans_image, const int* pStructure, int structureSize);
	bool Skeleton(MyDIB& orgin_image, MyDIB& trans_image, const int* pStructure, int structureSize);

public:
	bool Erosion(MyDIB& orgin_image, const int* pStructure, int structureSize);
	bool Dilation(MyDIB& orgin_image, const int* pStructure, int structureSize);
	bool Opening(MyDIB& orgin_image, const int* pStructure, int structureSize);
	bool Closing(MyDIB& orgin_image,  const int* pStructure, int structureSize);
	bool Skeleton(MyDIB& orgin_image, const int* pStructure, int structureSize);

public:
	bool Erosion(MyDIB& orgin_image, MyDIB& trans_image, int structureSize, structureType st_type);
	bool Dilation(MyDIB& orgin_image, MyDIB& trans_image, int structureSize, structureType st_type);
	bool Opening(MyDIB& orgin_image, MyDIB& trans_image, int structureSize, structureType st_type);
	bool Closing(MyDIB& orgin_image, MyDIB& trans_image, int structureSize, structureType st_type);
	bool Skeleton(MyDIB& orgin_image, MyDIB& trans_image, int structureSize, structureType st_type);

public:
	//�ṹԪ�����伸������Ϊԭ�㣬������ʵ�ֵĽṹԪ������ԭ�����ĶԳ�
	//structureSizeΪ�ṹԪ�Ĵ�С��ʵ�ʽṹԪ�ĳ�Ϊ2*structurSize+1������Ѱ������λ��
	void getStructure(int* pStructure, int structureSize, structureType st_type);
};

