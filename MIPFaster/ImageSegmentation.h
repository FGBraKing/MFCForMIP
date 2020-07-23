#pragma once
#include "MyDIB.h"

class ImageSegmentation
{
public:
	ImageSegmentation();
	~ImageSegmentation();

public:
	bool ManSeg(MyDIB& orgin_image, MyDIB& trans_image, int iThreshold);
	int  OstuSThreshold(MyDIB& orgin_image);
	bool RegionGrow(MyDIB& orgin_image, MyDIB& trans_image, int seed_val=100);
	bool RegionSplitMerge(MyDIB& orgin_image, MyDIB& trans_image);

};

