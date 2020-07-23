#include "pch.h"
#include "split.h"
#include <stack>

struct SplitArea //��������
{
	unsigned int w;                 // ��һ��ͼ��Ŀ��
	unsigned int h;					// ��һ��ͼ��ĸ߶�
	unsigned int left;				// ���Դͼ�����ݵ�ƫ�ƿ��
	unsigned int top;				// ���Դͼ�����ݵ�ƫ�Ƹ߶�
};

static void setAreaType(const SplitArea& splitArea, int w, int h, int* resultArray, int type);
static unsigned  char getMeanValue(const SplitArea& splitArea, unsigned char* srcImage, int w, int h);
static int getAreaType(const SplitArea& splitArea, unsigned char* srcImage, int w, int h, int* resultArray, int& type);
static bool checkConformity(const SplitArea& splitArea, unsigned char* srcImage, int w, int h); //һ����˨���

void areaSplitCombine(unsigned char* srcImage, int w, int h, int* resultArray)
{
	//�����������
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			resultArray[i * w + j] = 0;
		}
	}

	//��ʼ���򣨼�����ͼƬ��ѹջ
	std::stack<SplitArea> myStack;
	SplitArea splitArea0;
	splitArea0.w = w;
	splitArea0.h = h;
	splitArea0.left = 0;
	splitArea0.top = 0;
	myStack.push(splitArea0); //ԭʼͼ���������Ϣջ

	// ����������Ϊ��
	int areaType = 0;

	//������ϲ�
	while (!myStack.empty())
	{
		//�����ջ
		SplitArea myArea = myStack.top();
		myStack.pop();

		if (checkConformity(myArea, srcImage, w, h)) //�ж��Ƿ����һ����׼��
		{
			//����һ����׼�򣬽��кϲ����������κ���������Ƚϣ����Ϻϲ�׼����ϲ�֮��
			int t = getAreaType(myArea, srcImage, w, h, resultArray, areaType); //�õ������������
			setAreaType(myArea, w, h, resultArray, t);//���ñ����������
		}
		else
		{
			if ((myArea.w >= 2) && (myArea.h >= 2)) //��������Ŀ����߶�����2ʱ���Լ�������
			{
				//��ǰ�������Ϊ4��������򣬲�ѹջ�� 
				//˵���������Ǵ��ҵ��󣬴��µ��ϵ�˳��ѹջ��Ŀ���ǵõ������Ҵ��ϵ��µĳ�ջ˳��

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

//���ñ����������
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

//���������ƽ��ֵ
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
	int nTop = splitArea.top - 1; //�ھӵ�Y����			
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


	int nLeft = splitArea.left - 1; //�ھӵ�X����
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

bool checkConformity(const SplitArea& splitArea, unsigned char* srcImage, int w, int h) //һ����˨���
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
