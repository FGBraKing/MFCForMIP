#pragma once

class MyDIB
{
public:
	MyDIB();
	~MyDIB();
	MyDIB(CString path);
	MyDIB(CSize size, int nBitCount); //建立 BITMAPINFOHEADER
	MyDIB(CBitmap& ddb, CPalette* palette);	// 从DDB中获得DIB
	//MyBMP(CArchive& ar);

//prop
protected:
	DWORD m_dwSizeImage;			//位图的大小，以字节为单位
	int m_nColorTableEntries;		//调色板数量

	//HANDLE m_hFile;				//文件句柄
	//HANDLE m_hMap;				//文件映射后的句柄
	//LPVOID m_lpvFile;				//一个文件映射对象
	//HGLOBAL m_hGlobal;			//一个全局句柄
public:
	CString m_strPath;				// 图像路径
	//BITMAPINFOHEADER m_infoHead;
	//RGBQUAD m_color[1];

	HBITMAP m_hBitmap;				//图像句柄
	HPALETTE m_hPalette;			//调色板句柄

	//LPBITMAPFILEHEADER m_lpBMFH;	//文件头指针

	LPBITMAPINFOHEADER m_lpBMIH;	//信息头指针
	LPVOID  m_lpvColorTable;		//调色板指针		

	LPBYTE  m_lpImage;				//图像数据指针
//method
public:
	void Empty();
	void Serialize(CArchive& ar);
	BOOL Write(CFile* pFile);
	BOOL Read(CFile* pFile);
	BOOL ReadSection(CFile* pFile, CDC* pDC = NULL);		//用这个读取的时候才能有图像句柄
	BOOL CreateTrueColorDIB(CSize img_size);		//建立一个24位真彩色图像
	BOOL CreateGrayDIB(CSize img_size);				//建立一个8位的灰度图像

	DWORD GetSizeFile();					// 返回文件的大小 
	int GetSizeHeader();
	int GetSizeImage();							//位图的大小，以字节为单位。此为计算得到的数据
	CSize GetDimensions();
	BOOL Draw(CDC* pDC, CPoint origin, CSize size); // 绘图
	HBITMAP CreateSection(CDC* pDC = NULL);						//创建DIB，根据信息头创建，此时还没有图像数据
	UINT UsePalette(CDC* pDC, BOOL bBackground = FALSE);		//使用调色板
	BOOL MakePalette();											//获取调色板句柄
	BOOL SetSystemPalette(CDC* pDC);							//使用系统调色板
	BOOL Compress(CDC* pDC, BOOL bCompress = TRUE);				// FALSE means decompress
	HBITMAP CreateBitmap(CDC* pDC);								//创建DDB，使用CreateDIBitmap

	LPBYTE GetBits();					// 返回像素数组
	long GetWidth();					// 获取图像的宽度
	long GetHeight();					// 获取图像的高度
	WORD GetbiBitCount();				// 每个像素所需的位数
	DWORD GetbiSizeImage();				//位图的大小，以字节为单位。此为信息头中数据

	long BytesPerline();				// 获得图像每行扫描线所需的字节数
	void CopyPalette(CPalette&);		// 复制调色板
	int	GetPaletteSize();				// 获得调色板的数量

	int SetToDC(CDC& dc, CRect& src, CPoint& dst);	// 在指定DC上输出，不改变大小
	int StretchToDC(CDC& dc, CRect& dst, DWORD rop = SRCCOPY);	// 在指定DC上缩放输出

	void LoadImg(CString path);
	void SaveImg(CString path);
	void SetData(LPBYTE  data);
	BOOL GetPixelRGB(int row, int col, int& r, int& g, int& b);	

protected:
	//void DetachMapFile();						//分离映射文件
	void ComputeMetrics();						//m_dwSizeImage,计算图的大小
	void ComputePaletteSize(int nBitCount);		//m_nColorTableEntries，计算调色板数量

};

