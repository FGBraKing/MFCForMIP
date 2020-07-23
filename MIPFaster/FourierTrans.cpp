#include "pch.h"
#include "FourierTrans.h"
#include "global.h"
#include <cmath>

using std::exp;
using std::cos;
using std::sin;

//x的最低位成了n的最高位。其实就是让x按位倒序 unsigned int==size_t
unsigned int bitReverse(unsigned int x, int log2n) {
	int n = 0;
	int mask = 0x1;
	for (int i = 0; i < log2n; i++) {
		n <<= 1;
		n |= (x & mask);
		x >>= 1;
	}
	return n;
}

static size_t reverseBits(size_t x, int n) {
	size_t result = 0;
	for (int i = 0; i < n; i++, x >>= 1)
		result = (result << 1) | (x & 1U);
	return result;
}

FourierTrans::FourierTrans()
{
}

FourierTrans::~FourierTrans()
{
}


vector<complex<double>> FourierTrans::computeDft(const vector<complex<double>>& input)
{
	vector<complex<double> > output;
	size_t n = input.size();
	for (size_t k = 0; k < n; k++) {  // For each output element
		complex<double> sum(0.0, 0.0);
		for (size_t t = 0; t < n; t++) {  // For each input element
			double angle = 2 * PI * t * k / n;
			sum += input[t] * exp(-angle);
		}
		output.push_back(sum);
	}
	return output;
}

void FourierTrans::computeDft(const vector<double>& inreal, const vector<double>& inimag, vector<double>& outreal, vector<double>& outimag)
{
	size_t n = inreal.size();
	for (size_t k = 0; k < n; k++) {  // For each output element
		double sumreal = 0;
		double sumimag = 0;
		for (size_t t = 0; t < n; t++) {  // For each input element
			double angle = 2 * PI * t * k / n;
			sumreal += inreal[t] * cos(angle) + inimag[t] * sin(angle);
			sumimag += -inreal[t] * sin(angle) + inimag[t] * cos(angle);
		}
		outreal[k] = sumreal;
		outimag[k] = sumimag;
	}

}


void FourierTrans::compute_dft_complex(const complex<double> input[], complex<double> output[], int n) {
	for (int k = 0; k < n; k++) {  // For each output element
		complex<double> sum = 0.0;
		for (int t = 0; t < n; t++) {  // For each input element
			double angle = 2 * PI * t * k / n;
			sum += input[t] * exp(-angle);
		}
		output[k] = sum;
	}
}

void FourierTrans::compute_dft_real_pair(const double inreal[], const double inimag[],
	double outreal[], double outimag[], int n) {

	for (int k = 0; k < n; k++) {  // For each output element
		double sumreal = 0;
		double sumimag = 0;
		for (int t = 0; t < n; t++) {  // For each input element
			double angle = 2 * PI * t * k / n;
			sumreal += inreal[t] * cos(angle) + inimag[t] * sin(angle);
			sumimag += -inreal[t] * sin(angle) + inimag[t] * cos(angle);
		}
		outreal[k] = sumreal;
		outimag[k] = sumimag;
	}
}


void FourierTrans::transform(std::vector<std::complex<double>>& vec)
{
	size_t n = vec.size();
	if (n == 0)
		return;
	else if ((n & (n - 1)) == 0)  // Is power of 2
		transformRadix2(vec);
	else  // More complicated algorithm for arbitrary sizes
		transformBluestein(vec);
}
//没有除n
void FourierTrans::inverseTransform(vector<complex<double>>& vec)
{
	std::transform(vec.cbegin(), vec.cend(), vec.begin(),
		static_cast<complex<double>(*)(const complex<double>&)>(std::conj));
	transform(vec);
	std::transform(vec.cbegin(), vec.cend(), vec.begin(),
		static_cast<complex<double>(*)(const complex<double>&)>(std::conj));
}

void FourierTrans::transformRadix2(vector<complex<double>>& vec)
{
	// Length variables
	size_t n = vec.size();
	int levels = 0;  // Compute levels = floor(log2(n))
	for (size_t temp = n; temp > 1U; temp >>= 1)
		levels++;
	if (static_cast<size_t>(1U) << levels != n)
		throw std::domain_error("Length is not a power of 2");

	// Trignometric table
	vector<complex<double> > expTable(n / 2);
	for (size_t i = 0; i < n / 2; i++)
		expTable[i] = std::polar(1.0, -2 * PI * i / n);

	// Bit-reversed addressing permutation
	for (size_t i = 0; i < n; i++) {
		size_t j = bitReverse(i, levels);
		if (j > i)
			std::swap(vec[i], vec[j]);
	}

	// Cooley-Tukey decimation-in-time radix-2 FFT
	for (size_t size = 2; size <= n; size *= 2) {
		size_t halfsize = size / 2;
		size_t tablestep = n / size;
		for (size_t i = 0; i < n; i += size) {
			for (size_t j = i, k = 0; j < i + halfsize; j++, k += tablestep) {
				complex<double> temp = vec[j + halfsize] * expTable[k];
				vec[j + halfsize] = vec[j] - temp;
				vec[j] += temp;
			}
		}
		if (size == n)  // Prevent overflow in 'size *= 2'
			break;
	}
}

void FourierTrans::transformBluestein(vector<complex<double>>& vec)
{
	// Find a power-of-2 convolution length m such that m >= n * 2 + 1
	size_t n = vec.size();
	size_t m = 1;
	while (m / 2 <= n) {
		if (m > SIZE_MAX / 2)
			throw std::length_error("Vector too large");
		m *= 2;
	}

	// Trignometric table
	vector<complex<double> > expTable(n);
	for (size_t i = 0; i < n; i++) {
		unsigned long long temp = static_cast<unsigned long long>(i)* i;
		temp %= static_cast<unsigned long long>(n) * 2;
		double angle = PI * temp / n;
		expTable[i] = std::polar(1.0, -angle);
	}

	// Temporary vectors and preprocessing
	vector<complex<double> > av(m);
	for (size_t i = 0; i < n; i++)
		av[i] = vec[i] * expTable[i];
	vector<complex<double> > bv(m);
	bv[0] = expTable[0];
	for (size_t i = 1; i < n; i++)
		bv[i] = bv[m - i] = std::conj(expTable[i]);

	// Convolution
	vector<complex<double> > cv(m);
	convolve(av, bv, cv);

	// Postprocessing
	for (size_t i = 0; i < n; i++)
		vec[i] = cv[i] * expTable[i];
}

void FourierTrans::convolve(const vector<complex<double>>& xvec, const vector<complex<double>>& yvec, vector<complex<double>>& outvec)
{
	size_t n = xvec.size();
	if (n != yvec.size() || n != outvec.size())
		throw std::domain_error("Mismatched lengths");
	vector<complex<double> > xv = xvec;
	vector<complex<double> > yv = yvec;
	transform(xv);
	transform(yv);
	for (size_t i = 0; i < n; i++)
		xv[i] *= yv[i];
	inverseTransform(xv);
	for (size_t i = 0; i < n; i++)  // Scaling (because this FFT implementation omits it)
		outvec[i] = xv[i] / static_cast<double>(n);
}


void FourierTrans::FFT1D(complex<double>* pCTData, complex<double>* pCFData, int nLevel)
{
	// 循环控制变量
	int		i;
	int     j;
	int     k;

	// 傅立叶变换点数
	int	nCount = 0;

	// 计算傅立叶变换点数
	nCount = (int)pow(2, nLevel);

	// 某一级的长度
	int		nBtFlyLen;
	nBtFlyLen = 0;

	// 变换系数的角度 ＝2 * PI * i / nCount
	double	dAngle;

	complex<double>* pCW;

	// 分配内存，存储傅立叶变化需要的系数表
	pCW = new complex<double>[nCount / 2];

	// 计算傅立叶变换的系数
	for (i = 0; i < nCount / 2; i++)
	{
		dAngle = -2 * PI * i / nCount;
		pCW[i] = complex<double>(cos(dAngle), sin(dAngle));
	}

	// 变换需要的工作空间
	complex<double>* pCWork1, * pCWork2;

	// 分配工作空间
	pCWork1 = new complex<double>[nCount];

	pCWork2 = new complex<double>[nCount];


	// 临时变量
	complex<double>* pCTmp;

	// 初始化，写入数据
	memcpy(pCWork1, pCTData, sizeof(complex<double>) * nCount);

	// 临时变量
	int nInter;
	nInter = 0;

	// 蝶形算法进行快速傅立叶变换
	for (k = 0; k < nLevel; k++)
	{
		for (j = 0; j < (int)pow(2, k); j++)
		{
			//计算长度
			nBtFlyLen = (int)pow(2, (nLevel - k));

			//倒序重排，加权计算
			for (i = 0; i < nBtFlyLen / 2; i++)
			{
				nInter = j * nBtFlyLen;
				pCWork2[i + nInter] =
					pCWork1[i + nInter] + pCWork1[i + nInter + nBtFlyLen / 2];
				pCWork2[i + nInter + nBtFlyLen / 2] =
					(pCWork1[i + nInter] - pCWork1[i + nInter + nBtFlyLen / 2])
					* pCW[(int)(i * pow(2, k))];
			}
		}

		// 交换 pCWork1和pCWork2的数据
		pCTmp = pCWork1;
		pCWork1 = pCWork2;
		pCWork2 = pCTmp;
	}

	// 重新排序
	for (j = 0; j < nCount; j++)
	{
		nInter = 0;
		for (i = 0; i < nLevel; i++)
		{
			if (j & (1 << i))
			{
				nInter += 1 << (nLevel - i - 1);
			}
		}
		pCFData[j] = pCWork1[nInter];
	}

	// 释放内存空间
	delete pCW;
	delete pCWork1;
	delete pCWork2;
	pCW = NULL;
	pCWork1 = NULL;
	pCWork2 = NULL;
}

void FourierTrans::IFFT1D(complex<double>* pCFData, complex<double>* pCTData, int nLevel)
{
	int		i;
	int nCount;
	nCount = (int)pow(2, nLevel);
	complex<double>* pCWork;
	pCWork = new complex<double>[nCount];
	memcpy(pCWork, pCFData, sizeof(complex<double>) * nCount);
	for (i = 0; i < nCount; i++)
	{
		pCWork[i] = complex<double>(pCWork[i].real(), -pCWork[i].imag());
	}
	FFT1D(pCWork, pCTData, nLevel);
	for (i = 0; i < nCount; i++)
	{
		pCTData[i] =
			complex<double>(pCTData[i].real() / nCount, -pCTData[i].imag() / nCount);
	}
	delete pCWork;
	pCWork = NULL;
}

void FourierTrans::FFT2D(complex<double>* pCTData, complex<double>* pCFData, int nWidth, int nHeight)
{
	// 循环控制变量
	int	x;
	int	y;

	// 临时变量
	double	dTmpOne;
	double  dTmpTwo;

	// 进行傅立叶变换的宽度和高度，（2的整数次幂）
	// 图像的宽度和高度不一定为2的整数次幂
	int		nTransWidth;
	int 	nTransHeight;

	// 计算进行傅立叶变换的宽度	（2的整数次幂）
	dTmpOne = log(nWidth) / log(2);
	dTmpTwo = ceil(dTmpOne);
	nTransWidth = (int)pow(2, dTmpTwo);

	// 计算进行傅立叶变换的高度 （2的整数次幂）
	dTmpOne = log(nHeight) / log(2);
	dTmpTwo = ceil(dTmpOne);
	nTransHeight = (int)pow(2, dTmpTwo);

	// x，y（行列）方向上的迭代次数
	int		nXLev;
	int		nYLev;

	// 计算x，y（行列）方向上的迭代次数
	nXLev = (int)(log(nTransWidth) / log(2) + 0.5);
	nYLev = (int)(log(nTransHeight) / log(2) + 0.5);

	for (y = 0; y < nTransHeight; y++)
	{
		// x方向进行快速傅立叶变换
		FFT1D(&pCTData[nTransWidth * y], &pCFData[nTransWidth * y], nXLev);
	}

	// pCFData中目前存储了pCTData经过行变换的结果
	// 为了直接利用FFT_1D，需要把pCFData的二维数据转置，再一次利用FFT_1D进行
	// 傅立叶行变换（实际上相当于对列进行傅立叶变换）
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransHeight * x + y] = pCFData[nTransWidth * y + x];
		}
	}

	for (x = 0; x < nTransWidth; x++)
	{
		FFT1D(&pCTData[x * nTransHeight], &pCFData[x * nTransHeight], nYLev);
	}
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransWidth * y + x] = pCFData[nTransHeight * x + y];
		}
	}

	memcpy(pCTData, pCFData, sizeof(complex<double>) * nTransHeight * nTransWidth);
}

void FourierTrans::IFFT2D(complex<double>* pCFData, complex<double>* pCTData, int nWidth, int nHeight)
{
	// 循环控制变量
	int	x;
	int	y;

	// 临时变量
	double	dTmpOne;
	double  dTmpTwo;

	// 进行傅立叶变换的宽度和高度，（2的整数次幂）
	// 图像的宽度和高度不一定为2的整数次幂
	int		nTransWidth;
	int 	nTransHeight;

	// 计算进行傅立叶变换的宽度	（2的整数次幂）
	dTmpOne = log(nWidth) / log(2);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransWidth = (int)dTmpTwo;

	// 计算进行傅立叶变换的高度 （2的整数次幂）
	dTmpOne = log(nHeight) / log(2);
	dTmpTwo = ceil(dTmpOne);
	dTmpTwo = pow(2, dTmpTwo);
	nTransHeight = (int)dTmpTwo;
	complex<double>* pCWork = new complex<double>[nTransWidth * nTransHeight];
	complex<double>* pCTmp;
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCFData[nTransWidth * y + x];
			pCWork[nTransWidth * y + x] = complex<double>(pCTmp->real(), -pCTmp->imag());
		}
	}
	FFT2D(pCWork, pCTData, nWidth, nHeight);
	for (y = 0; y < nTransHeight; y++)
	{
		for (x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCTData[nTransWidth * y + x];
			pCTData[nTransWidth * y + x] =
				complex<double>(pCTmp->real() / (nTransWidth * nTransHeight),
					-pCTmp->imag() / (nTransWidth * nTransHeight));
		}
	}
	delete pCWork;
	pCWork = NULL;
}


void FourierTrans::Fourier(double* data, int height, int width, int iSign)
{
	int idim;
	unsigned long i1, i2, i3, i2rev, i3rev, ip1, ip2, ip3, ifp1, ifp2;
	unsigned long ibit, k1, k2, n, nprev, nrem, ntot, nn[3];
	double tempi, tempr;
	double theta, wi, wpi, wpr, wr, wtemp;
	ntot = height * width;
	nn[1] = height;
	nn[2] = width;
	nprev = 1;
	for (idim = 2; idim >= 1; idim--)
	{
		n = nn[idim];
		nrem = ntot / (n * nprev);
		ip1 = nprev << 1;
		ip2 = ip1 * n;
		ip3 = ip2 * nrem;
		i2rev = 1;
		for (i2 = 1; i2 <= ip2; i2 += ip1)
		{
			if (i2 < i2rev)
			{
				for (i1 = i2; i1 <= i2 + ip1 - 2; i1 += 2)
				{
					for (i3 = i1; i3 <= ip3; i3 += ip2)
					{
						i3rev = i2rev + i3 - i2;
						SWAP(data[i3], data[i3rev]);
						SWAP(data[i3 + 1], data[i3rev + 1]);
					}
				}
			}
			ibit = ip2 >> 1;
			while (ibit >= ip1 && i2rev > ibit)
			{
				i2rev -= ibit;
				ibit >>= 1;
			}
			i2rev += ibit;
		}
		ifp1 = ip1;
		while (ifp1 < ip2)
		{
			ifp2 = ifp1 << 1;
			theta = iSign * PI * 2 / (ifp2 / ip1);
			wtemp = sin(0.5 * theta);
			wpr = -2.0 * wtemp * wtemp;
			wpi = sin(theta);
			wr = 1.0;
			wi = 0.0;
			for (i3 = 1; i3 <= ifp1; i3 += ip1)
			{
				for (i1 = i3; i1 <= i3 + ip1 - 2; i1 += 2)
				{
					for (i2 = i1; i2 <= ip3; i2 += ifp2)
					{
						k1 = i2;
						k2 = k1 + ifp1;
						tempr = wr * data[k2] - wi * data[k2 + 1];
						tempi = wr * data[k2 + 1] + wi * data[k2];
						data[k2] = data[k1] - tempr;
						data[k2 + 1] = data[k1 + 1] - tempi;
						data[k1] += tempr;
						data[k1 + 1] += tempi;
					}
				}
				wr = (wtemp = wr) * wpr - wi * wpi + wr;
				wi = wi * wpr + wtemp * wpi + wi;
			}
			ifp1 = ifp2;
		}
		nprev *= n;
	}
}







