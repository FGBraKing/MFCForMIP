#pragma once
#include "MyDIB.h"


class ImageRestore
{
public:
	ImageRestore();
	~ImageRestore();

public:
	bool InverseFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& l_h);
	bool WinnerFilter(MyDIB& orgin_image, MyDIB& trans_image, const int& l_h, const double& k);
};

