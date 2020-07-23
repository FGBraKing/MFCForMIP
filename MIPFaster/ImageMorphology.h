#pragma once
#include "MyDIB.h"

/*图像形态学操作的算法，包含腐蚀、膨胀、开运算、闭运算、骨架细化等*/
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
	//结构元均以其几何中心为原点，且下面实现的结构元均关于原点中心对称
	//structureSize为结构元的大小，实际结构元的长为2*structurSize+1，便于寻找中心位置
	void getStructure(int* pStructure, int structureSize, structureType st_type);
};

