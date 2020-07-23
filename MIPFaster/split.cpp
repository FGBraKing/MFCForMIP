#include "pch.h"
#include "split.h"
#include <stack>

struct SplitArea //分列区域
{
	unsigned int w;                 // 这一块图像的宽度
	unsigned int h;					// 这一块图像的高度
	unsigned int left;				// 相对源图像数据的偏移宽度
	unsigned int top;				// 相对源图像数据的偏移高度
};

static void setAreaType(const SplitArea& splitArea, int w, int h, int* resultArray, int type);
static unsigned  char getMeanValue(const SplitArea& splitArea, unsigned char* srcImage, int w, int h);
static int getAreaType(const SplitArea& splitArea, unsigned char* srcImage, int w, int h, int* resultArray, int& type);
static bool checkConformity(const SplitArea& splitArea, unsigned char* srcImage, int w, int h); //一致性栓检查

void areaSplitCombine(unsigned char* srcImage, int w, int h, int* resultArray)
{
	//结果矩阵清零
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			resultArray[i * w + j] = 0;
		}
	}

	//初始区域（即整幅图片）压栈
	std::stack<SplitArea> myStack;
	SplitArea splitArea0;
	splitArea0.w = w;
	splitArea0.h = h;
	splitArea0.left = 0;
	splitArea0.top = 0;
	myStack.push(splitArea0); //原始图像的区域信息栈

	// 区域类型清为零
	int areaType = 0;

	//分裂与合并
	while (!myStack.empty())
	{
		//区域出栈
		SplitArea myArea = myStack.top();
		myStack.pop();

		if (checkConformity(myArea, srcImage, w, h)) //判断是否符合一致性准则。
		{
			//符合一致性准则，进行合并操作（与任何邻域区域比较，符合合并准则则合并之）
			int t = getAreaType(myArea, srcImage, w, h, resultArray, areaType); //得到本区域的类型
			setAreaType(myArea, w, h, resultArray, t);//设置本区域的类型
		}
		else
		{
			if ((myArea.w >= 2) && (myArea.h >= 2)) //当相区域的宽度与高度在于2时可以继续分裂
			{
				//当前区域分裂为4个相等区域，并压栈。 
				//说明：这里是从右到左，从下到上的顺序压栈，目的是得到从左到右从上到下的出栈顺序。

				for (int T = 1; T >= 0; T--)
				{
					for (int L = 1; L >= 0; L--)
					{
						SplitArea childArea;

						childArea.w = myArea.w / 2;
						childArea.h = myArea.h / 2;

						childArea.top = myArea.top + childArea.h * T;
						childArea.left = myArea.left + childArea.w * L;

						myStack.push(childArea);
					}
				}
			}
		}
	}

}

//设置本区域的类型
void setAreaType(const SplitArea& splitArea, int w, int h, int* resultArray, int type)
{
	for (int i = 0; i < splitArea.h; i++)
	{
		for (int j = 0; j < splitArea.w; j++)
		{
			resultArray[(splitArea.top + i) * w + (splitArea.left + j)] = type;
		}
	}
}

//计算区域的平均值
unsigned  char getMeanValue(const SplitArea& splitArea, unsigned char* srcImage, int w, int h)
{
	int sum = 0;
	for (int i = 0; i < splitArea.h; i++)
	{
		for (int j = 0; j < splitArea.w; j++)
		{
			sum += srcImage[(i + splitArea.h) * w + j];
		}
	}

	int count = splitArea.h * splitArea.w;

	unsigned char avg = sum / count;
	return avg;
}

int getAreaType(const SplitArea& splitArea, unsigned char* srcImage, int w, int h, int* resultArray, int& type)
{
	int areaValue = srcImage[splitArea.top * w + splitArea.left];
	int nTop = splitArea.top - 1; //邻居的Y坐标			
	if (nTop >= 0)
	{
		int left = splitArea.left;
		int right = splitArea.left + splitArea.w + 1;

		if (right > w)
		{
			right -= 1;
		}


		for (int i = left; i < right; i++)
		{
			if (resultArray[nTop * w + i] != 0)
			{
				if (areaValue == srcImage[nTop * w + i])
				{
					return resultArray[nTop * w + i];
				}
			}
		}
	}


	int nLeft = splitArea.left - 1; //邻居的X坐标
	if (nLeft >= 0)
	{
		int top = splitArea.top;
		int bottom = splitArea.top + splitArea.h;

		for (int i = top; i < bottom; i++)
		{
			if (resultArray[top * w + nLeft] != 0)
			{
				if (areaValue == srcImage[top * w + nLeft])
				{
					return resultArray[top * w + nLeft];
				}
			}
		}

	}
	return ++type;
}

bool checkConformity(const SplitArea& splitArea, unsigned char* srcImage, int w, int h) //一致性栓检查
{
	unsigned char p0 = srcImage[splitArea.top * w + splitArea.left];
	bool ret = true;
	for (int i = 0; i < splitArea.h; i++)
	{
		for (int j = 0; j < splitArea.w; j++)
		{
			unsigned char px = srcImage[(splitArea.top + i) * w + (splitArea.left + j)];
			if (px != p0)
			{
				ret = false;
			}

			if (!ret)
			{
				break;
			}
		}

		if (!ret)
		{
			break;
		}
	}

	return ret;
}
