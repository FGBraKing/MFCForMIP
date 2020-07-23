#pragma once

class MyDIB
{
public:
	MyDIB();
	~MyDIB();
	MyDIB(CString path);
	MyDIB(CSize size, int nBitCount); //���� BITMAPINFOHEADER
	MyDIB(CBitmap& ddb, CPalette* palette);	// ��DDB�л��DIB
	//MyBMP(CArchive& ar);

//prop
protected:
	DWORD m_dwSizeImage;			//λͼ�Ĵ�С�����ֽ�Ϊ��λ
	int m_nColorTableEntries;		//��ɫ������

	//HANDLE m_hFile;				//�ļ����
	//HANDLE m_hMap;				//�ļ�ӳ���ľ��
	//LPVOID m_lpvFile;				//һ���ļ�ӳ�����
	//HGLOBAL m_hGlobal;			//һ��ȫ�־��
public:
	CString m_strPath;				// ͼ��·��
	//BITMAPINFOHEADER m_infoHead;
	//RGBQUAD m_color[1];

	HBITMAP m_hBitmap;				//ͼ����
	HPALETTE m_hPalette;			//��ɫ����

	//LPBITMAPFILEHEADER m_lpBMFH;	//�ļ�ͷָ��

	LPBITMAPINFOHEADER m_lpBMIH;	//��Ϣͷָ��
	LPVOID  m_lpvColorTable;		//��ɫ��ָ��		

	LPBYTE  m_lpImage;				//ͼ������ָ��
//method
public:
	void Empty();
	void Serialize(CArchive& ar);
	BOOL Write(CFile* pFile);
	BOOL Read(CFile* pFile);
	BOOL ReadSection(CFile* pFile, CDC* pDC = NULL);		//�������ȡ��ʱ�������ͼ����
	BOOL CreateTrueColorDIB(CSize img_size);		//����һ��24λ���ɫͼ��
	BOOL CreateGrayDIB(CSize img_size);				//����һ��8λ�ĻҶ�ͼ��

	DWORD GetSizeFile();					// �����ļ��Ĵ�С 
	int GetSizeHeader();
	int GetSizeImage();							//λͼ�Ĵ�С�����ֽ�Ϊ��λ����Ϊ����õ�������
	CSize GetDimensions();
	BOOL Draw(CDC* pDC, CPoint origin, CSize size); // ��ͼ
	HBITMAP CreateSection(CDC* pDC = NULL);						//����DIB��������Ϣͷ��������ʱ��û��ͼ������
	UINT UsePalette(CDC* pDC, BOOL bBackground = FALSE);		//ʹ�õ�ɫ��
	BOOL MakePalette();											//��ȡ��ɫ����
	BOOL SetSystemPalette(CDC* pDC);							//ʹ��ϵͳ��ɫ��
	BOOL Compress(CDC* pDC, BOOL bCompress = TRUE);				// FALSE means decompress
	HBITMAP CreateBitmap(CDC* pDC);								//����DDB��ʹ��CreateDIBitmap

	LPBYTE GetBits();					// ������������
	long GetWidth();					// ��ȡͼ��Ŀ��
	long GetHeight();					// ��ȡͼ��ĸ߶�
	WORD GetbiBitCount();				// ÿ�����������λ��
	DWORD GetbiSizeImage();				//λͼ�Ĵ�С�����ֽ�Ϊ��λ����Ϊ��Ϣͷ������

	long BytesPerline();				// ���ͼ��ÿ��ɨ����������ֽ���
	void CopyPalette(CPalette&);		// ���Ƶ�ɫ��
	int	GetPaletteSize();				// ��õ�ɫ�������

	int SetToDC(CDC& dc, CRect& src, CPoint& dst);	// ��ָ��DC����������ı��С
	int StretchToDC(CDC& dc, CRect& dst, DWORD rop = SRCCOPY);	// ��ָ��DC���������

	void LoadImg(CString path);
	void SaveImg(CString path);
	void SetData(LPBYTE  data);
	BOOL GetPixelRGB(int row, int col, int& r, int& g, int& b);	

protected:
	//void DetachMapFile();						//����ӳ���ļ�
	void ComputeMetrics();						//m_dwSizeImage,����ͼ�Ĵ�С
	void ComputePaletteSize(int nBitCount);		//m_nColorTableEntries�������ɫ������

};

