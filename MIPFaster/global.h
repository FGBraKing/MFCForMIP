#pragma once
/*宏定义*/
#define PI 3.14159265358979323846
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr


/*结构体定义*/
typedef struct HSVARRAY{
	float h;
	float s;
	float v;
} HSVARRAY;