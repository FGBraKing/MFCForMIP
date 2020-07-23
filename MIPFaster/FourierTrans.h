#pragma once
#define _USE_MATH_DEFINES
#include <complex>
#include <vector>
#include <algorithm>

using std::complex;
using std::size_t;
using std::vector;



class FourierTrans
{
public:
	FourierTrans();
	~FourierTrans();

	vector<complex<double> > computeDft(const vector<complex<double> >& input);
	void computeDft(const vector<double>& inreal, const vector<double>& inimag, vector<double>& outreal, vector<double>& outimag);

	void transform(vector<complex<double> >& vec);	//DFT
	void inverseTransform(vector<complex<double> >& vec);	//IDFT
	void transformRadix2(vector<complex<double> >& vec);	
	void transformBluestein(vector<complex<double> >& vec);
	void convolve(const vector<complex<double> >& vecx, const vector<complex<double> >& vecy, vector<complex<double> >& vecout);

	//2d傅里叶变换，先每行进行傅里叶，再转置，再对每行做傅里叶，再转置，得到结果（实际上就是先对行再对列进行傅里叶）
	//2d反变换，先取共轭，再2d傅里叶，在取共轭，除数据总数
	void FFT1D(complex<double>* pCTData, complex<double>* pCFData, int nLevel);
	void IFFT1D(complex<double>* pCFData, complex<double>* pCTData, int nLevel);
	void FFT2D(complex<double>* pCTData, complex<double>* pCFData, int nWidth, int nHeight);
	void IFFT2D(complex<double>* pCFData, complex<double>* pCTData, int nWidth, int nHeight);

	void Fourier(double* data, int height, int width, int iSign);

	void compute_dft_complex(const complex<double> input[], complex<double> output[], int n);
	void compute_dft_real_pair(const double inreal[], const double inimag[],
		double outreal[], double outimag[], int n);
};

