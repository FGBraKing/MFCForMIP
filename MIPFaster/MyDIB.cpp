/*********************************************************************************************************
* ģ������: DIB
* ժ    Ҫ: ʵ��Ҫ�����ͼ����
* ��ǰ�汾: 1.0.0
* ��    ��: �η�
* �������: 2020/01/07
* ��    ��:
* ע    ��: none
**********************************************************************************************************
* ȡ���汾:
* ��    ��:
* �������:
* �޸�����:
* �޸��ļ�:
*********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "pch.h"
#include "MyDIB.h"
/*********************************************************************************************************
*                                              ���Ա����ʵ��
*********************************************************************************************************/

/*********************************************************************************************************
* ��������:  MyDIB::MyDIB
* Ȩ    ��:  public
* ��������:  ���캯��
* ��    ��:  void
* �� �� ֵ:
* ע    ��:
*********************************************************************************************************/
MyDIB::MyDIB()
{
	m_lpBMIH = nullptr;
	m_lpvColorTable = nullptr;
	m_lpImage = nullptr;

	m_hBitmap = NULL;
	m_hPalette = NULL;
	//m_lpBMFH = new BITMAPFILEHEADER;
	Empty();
}

MyDIB::~MyDIB()
{
	Empty();
}

MyDIB::MyDIB(CString path)
{
	m_lpBMIH = nullptr;
	m_lpvColorTable = nullptr;
	m_lpImage = nullptr;

	m_hBitmap = NULL;
	m_hPalette = NULL;
	//m_lpBMFH = new BITMAPFILEHEADER;
	Empty();
	LoadImg(path);
}

MyDIB::MyDIB(CSize size, int nBitCount)
{
	m_lpBMIH = nullptr;
	m_lpvColorTable = nullptr;
	m_lpImage = nullptr;

	m_hBitmap = NULL;
	m_hPalette = NULL;
	//m_lpBMFH = new BITMAPFILEHEADER;
	Empty();

	ComputePaletteSize(nBitCount);

	m_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries];
	m_lpvColorTable = (LPBYTE)m_lpBMIH + sizeof(BITMAPINFOHEADER);
	m_lpImage = nullptr;

	//��д�ֶ�
	m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBMIH->biWidth = size.cx;
	m_lpBMIH->biHeight = size.cy;
	m_lpBMIH->biPlanes = 1;
	m_lpBMIH->biBitCount = nBitCount;
	m_lpBMIH->biCompression = BI_RGB;		//0
	m_lpBMIH->biSizeImage = 0;
	m_lpBMIH->biXPelsPerMeter = 0;
	m_lpBMIH->biYPelsPerMeter = 0;
	m_lpBMIH->biClrUsed = m_nColorTableEntries;
	m_lpBMIH->biClrImportant = m_nColorTableEntries;
	ComputeMetrics();
	if(m_nColorTableEntries)
		memset(m_lpvColorTable, 0, sizeof(RGBQUAD) * m_nColorTableEntries);
}

MyDIB::MyDIB(CBitmap& ddb, CPalette* palette)
{
	m_lpBMIH = nullptr;
	m_lpvColorTable = nullptr;
	m_lpImage = nullptr;

	m_hBitmap = NULL;
	m_hPalette = NULL;
	Empty();
	BITMAP ddbinfo;
	//���DDB�������Ϣ
	ddb.GetBitmap(&ddbinfo);			
	/*�������ͼ��ĸ߶ȿ�ȵ���Ϣ����дDIB�������BITMAPINFO*/
	ComputePaletteSize(ddbinfo.bmBitsPixel);//�����ɫ���С

	int cbHeaderSize = sizeof(BITMAPINFOHEADER) + m_nColorTableEntries * sizeof(RGBQUAD);
	m_lpBMIH = (LPBITMAPINFOHEADER) new char[cbHeaderSize];
	m_lpvColorTable = (LPBYTE)m_lpBMIH + sizeof(BITMAPINFOHEADER);

	//��дBITMAPINFO
	m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBMIH->biWidth = ddbinfo.bmWidth;
	m_lpBMIH->biHeight = ddbinfo.bmHeight;
	m_lpBMIH->biPlanes = ddbinfo.bmPlanes;	//����Ϊ1
	m_lpBMIH->biBitCount = short(ddbinfo.bmBitsPixel);
	m_lpBMIH->biCompression = BI_RGB;
	m_lpBMIH->biSizeImage = BytesPerline() * m_lpBMIH->biHeight;
	m_lpBMIH->biXPelsPerMeter = m_lpBMIH->biYPelsPerMeter = 3780;
	m_lpBMIH->biClrUsed = m_nColorTableEntries;
	m_lpBMIH->biClrImportant = m_nColorTableEntries;
	ComputeMetrics();
	if (m_nColorTableEntries)
		memset(m_lpvColorTable, 0, m_nColorTableEntries * sizeof(RGBQUAD));	//��RGBQUAD�����е�ÿ��Ԫ��ȫ����0
	//�ж��Ƿ��е�ɫ�壬�������palette�е�entry���Ƶ�BITMAPINFO��RGBQUAD������
	if (palette) CopyPalette(*palette);
	MakePalette();

	//ΪDIB�����������ռ�
	m_lpImage = (LPBYTE) new char[m_dwSizeImage];
	
	// ��DDB�е�ͼ����Ϣ���Ƶ�DIB��
	// GetDesktopWindow()�������洰�ڵľ��
	CWnd DesktopWnd;
	DesktopWnd.Attach(::GetDesktopWindow());
	CWindowDC dtpDC(&DesktopWnd);
	// ��ɴ�DDB��DIB��ת������
	::GetDIBits(dtpDC.m_hDC,
		HBITMAP(ddb),
		0,
		ddbinfo.bmHeight,
		LPVOID(m_lpImage),
		(LPBITMAPINFO)m_lpBMIH,
		DIB_RGB_COLORS
	);
	DesktopWnd.Detach();
}

void MyDIB::Empty()
{
	//���ͼ������
	//memset(m_lpBMFH, 0, sizeof(BITMAPFILEHEADER));
	if (m_lpBMIH)
		delete[] m_lpBMIH;
	//if (m_lpvColorTable && GetPaletteSize())			
	//	delete[](char*)m_lpvColorTable;
	if (m_lpImage)
		delete[](char*)m_lpImage;

	//������
	if (m_hPalette) ::DeleteObject(m_hPalette);
	if (m_hBitmap)  ::DeleteObject(m_hBitmap);

	//����Ա������ΪĬ��ֵ
	m_strPath = L"";
	m_dwSizeImage = 0;
	m_nColorTableEntries = 0;

	m_lpBMIH = nullptr;
	m_lpvColorTable = nullptr;
	m_lpImage = nullptr;


	m_hBitmap = NULL;
	m_hPalette = NULL;
}

void MyDIB::Serialize(CArchive& ar)
{
	DWORD dwPos;
	dwPos = ar.GetFile()->GetPosition();
	TRACE("CDib::Serialize -- pos = %d\n", dwPos);
	ar.Flush();
	dwPos = ar.GetFile()->GetPosition();
	TRACE("CDib::Serialize -- pos = %d\n", dwPos);
	if (ar.IsStoring()) {
		Write(ar.GetFile());
	}
	else {
		Read(ar.GetFile());
	}
}

BOOL MyDIB::Write(CFile* pFile)
{
	//�ļ�ͷ
	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4d42;  // 'BM'
	int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
	//bmfh.bfSize = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + nSizeHdr + m_dwSizeImage;
	// bfsizeʹ�õĵ�λ�ǲ�ȷ���ģ�ͨ������
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;

	try {
		pFile->Write((LPVOID)&bmfh, sizeof(BITMAPFILEHEADER));

		pFile->Write((LPVOID)m_lpBMIH, nSizeHdr);

		pFile->Write((LPVOID)m_lpImage, m_dwSizeImage);
	}
	catch (CException * pe) {
		pe->Delete();
		//AfxMessageBox("write error");
		return FALSE;
	}
	return TRUE;
}

BOOL MyDIB::Read(CFile* pFile)
{
	// 1. ���ļ�ͷ��ȡ��Ϣͷ+��ɫ��Ĵ�С
	// 2. ����Ϣͷ�õ�ͼ���С�͵�ɫ��
	// 3. ��ͼ��
	// ����ʹ���ļ�ͷ���bfSize
	Empty();
	int nCount, nSize;
	BITMAPFILEHEADER bmfh;
	try {
		nCount = pFile->Read((LPVOID)&bmfh, sizeof(BITMAPFILEHEADER));
		if (nCount != sizeof(BITMAPFILEHEADER)) {

			//throw new CException;
			return FALSE;
		}

		if (bmfh.bfType != 0x4d42) {

			//throw new CException;
			return FALSE;

		}

		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);  //��Ϣͷ�͵�ɫ���С

		m_lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
		m_lpvColorTable = (LPBYTE)m_lpBMIH + sizeof(BITMAPINFOHEADER);
		nCount = pFile->Read(m_lpBMIH, nSize); 

		ComputeMetrics();
		ComputePaletteSize(m_lpBMIH->biBitCount);
		MakePalette();

		m_lpImage = (LPBYTE) new char[m_dwSizeImage];
		nCount = pFile->Read(m_lpImage, m_dwSizeImage); // image only
	}

	catch (CException * pe) {
		//AfxMessageBox("Read error");
		pe->Delete();

		return FALSE;
	}

	return TRUE;
}

BOOL MyDIB::ReadSection(CFile* pFile, CDC* pDC)
{
	   // ��BMP�ļ����ߴ���һ��DIB section
	   // �������Դ��̵�λͼ���޸�
	   // 1. ���ļ�ͷ�õ���Ϣͷ+��ɫ��Ĵ�С
	   // 2. ����Ϣͷ�õ�ͼ���С�͵�ɫ��
	   // 3. ������Ϣͷ�Ĳ�������DIB section
	   // 4. ��CreateBidSection������ڴ��ж�ȡͼ��
	Empty();
	int nCount, nSize;
	BITMAPFILEHEADER bmfh;

	try {
		nCount = pFile->Read((LPVOID)&bmfh, sizeof(BITMAPFILEHEADER));
		if (nCount != sizeof(BITMAPFILEHEADER)) {

			//throw new CException;
			return FALSE;
		}

		if (bmfh.bfType != 0x4d42) {

			//throw new CException;
			return FALSE;
		}

		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
		m_lpvColorTable = (LPBYTE)m_lpBMIH + sizeof(BITMAPINFOHEADER);
		nCount = pFile->Read(m_lpBMIH, nSize); 

		if (m_lpBMIH->biCompression != BI_RGB) {

			//throw new CException;
			Empty();
			return FALSE;

		}

		ComputeMetrics();
		ComputePaletteSize(m_lpBMIH->biBitCount);
		MakePalette();
		UsePalette(pDC);

		m_hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), (LPBITMAPINFO)m_lpBMIH, DIB_RGB_COLORS, (LPVOID*)&m_lpImage, NULL, 0);

		ASSERT(m_lpImage != NULL);

		nCount = pFile->Read(m_lpImage, m_dwSizeImage); 

	}

	catch (CException * pe) {

		//AfxMessageBox("ReadSection error");
		pe->Delete();
		return FALSE;
	}

	return TRUE;

}

BOOL MyDIB::CreateTrueColorDIB(CSize img_size)
{
	Empty();
	m_nColorTableEntries = 0;

	m_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries];
	m_lpvColorTable = (LPBYTE)m_lpBMIH + sizeof(BITMAPINFOHEADER);
	m_lpImage = nullptr;
	//��д�ֶ�
	m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBMIH->biWidth = img_size.cx;
	m_lpBMIH->biHeight = img_size.cy;
	m_lpBMIH->biPlanes = 1;
	m_lpBMIH->biBitCount = 24;
	m_lpBMIH->biCompression = BI_RGB;		//0
	m_lpBMIH->biSizeImage = ((((img_size.cx * 24) + 31) / 32) * 4) * img_size.cy;
	m_lpBMIH->biXPelsPerMeter = 0;
	m_lpBMIH->biYPelsPerMeter = 0;
	m_lpBMIH->biClrUsed = m_nColorTableEntries;
	m_lpBMIH->biClrImportant = 256;
	ComputeMetrics();
	return TRUE;
}

BOOL MyDIB::CreateGrayDIB(CSize img_size)
{
	Empty();
	m_nColorTableEntries = 256;
	m_lpBMIH = (LPBITMAPINFOHEADER) new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries];
	m_lpvColorTable = (LPBYTE)m_lpBMIH + sizeof(BITMAPINFOHEADER);
	m_lpImage = nullptr;
	//��д�ֶ�
	m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBMIH->biWidth = img_size.cx;
	m_lpBMIH->biHeight = img_size.cy;
	m_lpBMIH->biPlanes = 1;
	m_lpBMIH->biBitCount = 8;
	m_lpBMIH->biCompression = BI_RGB;		//0
	m_lpBMIH->biSizeImage = ((((img_size.cx * 8) + 31) / 32) * 4) * img_size.cy;
	m_lpBMIH->biXPelsPerMeter = 0;
	m_lpBMIH->biYPelsPerMeter = 0;
	m_lpBMIH->biClrUsed = m_nColorTableEntries;
	m_lpBMIH->biClrImportant = m_nColorTableEntries;
	ComputeMetrics();
	RGBQUAD* tmp_color = (RGBQUAD*)m_lpvColorTable;
	for (int i = 0; i < m_nColorTableEntries; i++)
	{
		tmp_color[i].rgbBlue = tmp_color[i].rgbGreen = tmp_color[i].rgbRed = i;
		tmp_color[i].rgbReserved = 0;
	}
	return TRUE;
}

DWORD MyDIB::GetSizeFile()
{
	return GetSizeHeader() + GetSizeImage() + sizeof(BITMAPFILEHEADER);
}

int MyDIB::GetSizeHeader()
{
	return sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
}

int MyDIB::GetSizeImage()
{
	//TRACE("BytesPerline() * GetHeight():%d\n", BytesPerline() * GetHeight());
	//TRACE("m_dwSizeImage:%d\n", m_dwSizeImage);
	//��m_dwSizeImageΪ׼��BytesPerline() * GetHeight() ����ƫС   V1
	//��m_dwSizeImage��BytesPerline() * GetHeight()�нϴ��Ϊ׼
	//ASSERT(m_dwSizeImage== BytesPerline() * GetHeight());
	return m_dwSizeImage;
}

CSize MyDIB::GetDimensions()
{
	if (m_lpBMIH ==nullptr) return CSize(0, 0);

	return CSize((int)m_lpBMIH->biWidth, (int)m_lpBMIH->biHeight);
}

BOOL MyDIB::Draw(CDC* pDC, CPoint origin, CSize size)
{
	if (m_lpBMIH == nullptr) return FALSE;

	if (m_hPalette) {

		::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);

	}

	pDC->SetStretchBltMode(COLORONCOLOR);

	::StretchDIBits(pDC->GetSafeHdc(), origin.x, origin.y, size.cx, size.cy,

		0, 0, m_lpBMIH->biWidth, m_lpBMIH->biHeight,

		m_lpImage, (LPBITMAPINFO)m_lpBMIH, DIB_RGB_COLORS, SRCCOPY);

	return TRUE;
}

HBITMAP MyDIB::CreateSection(CDC* pDC)
{
	if (m_lpBMIH == nullptr) return NULL;

	if (m_lpImage) return NULL; // ������ͼ��ʱ�����createsection

	m_hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), (LPBITMAPINFO)m_lpBMIH,

		DIB_RGB_COLORS, (LPVOID*)&m_lpImage, NULL, 0);

	ASSERT(m_lpImage);

	return m_hBitmap;
}

UINT MyDIB::UsePalette(CDC* pDC, BOOL bBackground)
{
	if (m_hPalette == NULL) return 0;

	HDC hdc = pDC->GetSafeHdc();

	::SelectPalette(hdc, m_hPalette, bBackground);

	return ::RealizePalette(hdc);
}

BOOL MyDIB::MakePalette()
{
	if (m_nColorTableEntries == 0) return FALSE;

	if (m_hPalette) ::DeleteObject(m_hPalette);

	TRACE("CDib::MakePalette -- m_nColorTableEntries = %d\n", m_nColorTableEntries);

	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) + m_nColorTableEntries * sizeof(PALETTEENTRY)];

	pLogPal->palVersion = 0x300;
	pLogPal->palNumEntries = m_nColorTableEntries;
	LPRGBQUAD pDibQuad = (LPRGBQUAD)m_lpvColorTable;
	for (int i = 0; i < m_nColorTableEntries; i++) {
		pLogPal->palPalEntry[i].peRed = pDibQuad->rgbRed;
		pLogPal->palPalEntry[i].peGreen = pDibQuad->rgbGreen;
		pLogPal->palPalEntry[i].peBlue = pDibQuad->rgbBlue;
		pLogPal->palPalEntry[i].peFlags = 0;
		pDibQuad++;
	}
	m_hPalette = ::CreatePalette(pLogPal);
	delete pLogPal;
	return TRUE;
}

BOOL MyDIB::SetSystemPalette(CDC* pDC)
{
	if (m_nColorTableEntries) return FALSE;

	m_hPalette = ::CreateHalftonePalette(pDC->GetSafeHdc());

	return TRUE;
}

BOOL MyDIB::Compress(CDC* pDC, BOOL bCompress)
{
	// 1. DIBתΪGDI Bitmap������ѹ��
	// 2. ת����DIB
	// 3. ���ԭ��DIB
	// 4. �����µ�DIB
	if ((m_lpBMIH->biBitCount != 4) && (m_lpBMIH->biBitCount != 8)) return FALSE;
	if (m_hBitmap) return FALSE;
	TRACE("Compress: original palette size = %d\n", m_nColorTableEntries);
	//1ת��ΪGDI�������ѹ��
	HDC hdc = pDC->GetSafeHdc();
	HPALETTE hOldPalette = ::SelectPalette(hdc, m_hPalette, FALSE);
	HBITMAP hBitmap;  // temporary
	if ((hBitmap = CreateBitmap(pDC)) == NULL) return FALSE;
	int nSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
	LPBITMAPINFOHEADER lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
	memcpy(lpBMIH, m_lpBMIH, nSize);  // new header

	if (bCompress) {
		switch (lpBMIH->biBitCount) {
		case 4:
			lpBMIH->biCompression = BI_RLE4; break;
		case 8:
			lpBMIH->biCompression = BI_RLE8; break;
		default:
			ASSERT(FALSE);
		}
		// calls GetDIBits with null data pointer to get size of compressed DIB
		if (!::GetDIBits(pDC->GetSafeHdc(), hBitmap, 0, (UINT)lpBMIH->biHeight, NULL, (LPBITMAPINFO)lpBMIH, DIB_RGB_COLORS)) {

			//AfxMessageBox("Unable to compress this DIB");

			// probably a problem with the color table

			::DeleteObject(hBitmap);

			delete[] lpBMIH;

			::SelectPalette(hdc, hOldPalette, FALSE);

			return FALSE;

		}

		if (lpBMIH->biSizeImage == 0) {
			//AfxMessageBox("Driver can't do compression");
			::DeleteObject(hBitmap);
			delete[] lpBMIH;
			::SelectPalette(hdc, hOldPalette, FALSE);
			return FALSE;
		}

		else {

			m_dwSizeImage = lpBMIH->biSizeImage;

		}

	}

	else {

		lpBMIH->biCompression = BI_RGB; // decompress

		// figure the image size from the bitmap width and height

		DWORD dwBytes = ((DWORD)lpBMIH->biWidth * lpBMIH->biBitCount) / 32;

		if (((DWORD)lpBMIH->biWidth * lpBMIH->biBitCount) % 32) {
			dwBytes++;
		}
		dwBytes *= 4;
		m_dwSizeImage = dwBytes * lpBMIH->biHeight; // no compression
		lpBMIH->biSizeImage = m_dwSizeImage;

	}
	//2�ؽ�DIB
	LPBYTE lpImage = (LPBYTE) new char[m_dwSizeImage];

	VERIFY(::GetDIBits(pDC->GetSafeHdc(), hBitmap, 0, (UINT)lpBMIH->biHeight, lpImage, (LPBITMAPINFO)lpBMIH, DIB_RGB_COLORS));  //���Է���Ϊ�棬�Ӿ����ȡ����
	TRACE("dib successfully created - height = %d\n", lpBMIH->biHeight);
	//3 ���ԭ��DIB
	::DeleteObject(hBitmap);
	Empty();
	//4 �滻
	m_lpBMIH = lpBMIH;
	m_lpImage = lpImage;
	ComputeMetrics();
	ComputePaletteSize(m_lpBMIH->biBitCount);
	MakePalette();
	::SelectPalette(hdc, hOldPalette, FALSE);

	TRACE("Compress: new palette size = %d\n", m_nColorTableEntries);
	return TRUE;
}

HBITMAP MyDIB::CreateBitmap(CDC* pDC)
{
	if (m_dwSizeImage == 0) return NULL;
	HBITMAP hBitmap = ::CreateDIBitmap(pDC->GetSafeHdc(), m_lpBMIH, CBM_INIT, m_lpImage, (LPBITMAPINFO)m_lpBMIH, DIB_RGB_COLORS);

	ASSERT(hBitmap);
	return hBitmap;
}

LPBYTE MyDIB::GetBits()
{
	return m_lpImage;
}

long MyDIB::GetWidth()
{
	return m_lpBMIH->biWidth;
}

long MyDIB::GetHeight()
{
	return m_lpBMIH->biHeight;
}

WORD MyDIB::GetbiBitCount()
{
	return m_lpBMIH->biBitCount;
}

DWORD MyDIB::GetbiSizeImage()
{
	return m_lpBMIH->biSizeImage;
}

long MyDIB::BytesPerline()
{
	return ((((GetWidth() * GetbiBitCount()) + 31) / 32) * 4);
}

void MyDIB::CopyPalette(CPalette& palette)
{
	if (palette.GetEntryCount() < GetPaletteSize()) return;		//!=
	PPALETTEENTRY pe;
	pe = new PALETTEENTRY[palette.GetEntryCount()];
	palette.GetPaletteEntries(0, palette.GetEntryCount(), pe);		//���ǰcount����ɫ���pe

	RGBQUAD* tmp_color = (RGBQUAD*)m_lpvColorTable;
	for (int i = 0; i < GetPaletteSize(); i++)
	{
		tmp_color[i].rgbRed = pe[i].peRed;
		tmp_color[i].rgbGreen = pe[i].peGreen;
		tmp_color[i].rgbBlue = pe[i].peBlue;
		tmp_color[i].rgbReserved = 0;
	}
	delete[] pe;
}

int MyDIB::GetPaletteSize()
{
	return m_nColorTableEntries;
}

int MyDIB::SetToDC(CDC& dc, CRect& src, CPoint& dst)
{
	TRACE("left:%d,bottom:%d,top:%d,right:%d\n",src.left,src.bottom,src.top,src.right);
	return ::SetDIBitsToDevice(
		dc.m_hDC,
		dst.x,				// �ͻ�����ʼ��ʾλͼ��x���꣬����Ĭ��ӳ�䷽ʽΪWM_TEXT�������Ͻ�Ϊԭ��
		dst.y,				// y-coord of destination upper-left corner
		/*GetWidth(),
		GetHeight(),*/
		src.Width(),		//Ҫ��ʾ��λͼ�Ŀ��
		src.Height(),		// source rectangle height
		src.left,			//λͼ��ԭ��λ��λͼ���½�
		src.top,			// y-coord of source lower-left corner
		0,					//��ʼɨ���ߵ�λ��   
		m_lpBMIH->biHeight,	//ɨ���ߵ���Ŀ
		m_lpImage,
		(LPBITMAPINFO)m_lpBMIH,
		DIB_RGB_COLORS
	);
}

int MyDIB::StretchToDC(CDC& dc, CRect& dst, DWORD rop)
{
	return 	::StretchDIBits(
		dc.m_hDC,
		dst.left + 20,
		dst.top + 20,
		m_lpBMIH->biWidth > dst.Width() - 20 ? dst.Width() - 20 : m_lpBMIH->biWidth,
		m_lpBMIH->biHeight > dst.Height() - 20 ? dst.Height() - 20 : m_lpBMIH->biHeight,
		0,
		0,
		m_lpBMIH->biWidth,
		m_lpBMIH->biHeight,
		m_lpImage,
		(LPBITMAPINFO)m_lpBMIH,
		DIB_RGB_COLORS, SRCCOPY);
}

void MyDIB::ComputeMetrics()
{
	if (m_lpBMIH->biSize != sizeof(BITMAPINFOHEADER)) {
		TRACE("Not a valid Windows bitmap -- probably an OS/2 bitmap\n");
		//throw new CException;
		ASSERT(FALSE);
	}
	m_dwSizeImage = m_lpBMIH->biSizeImage;//����ԭ����Ϣͷ��������ֽ�����Ϊ4�ı���
	if (1) {	//m_dwSizeImage == 0
		DWORD dwBytes = ((DWORD)m_lpBMIH->biWidth * m_lpBMIH->biBitCount) / 32;
		if (((DWORD)m_lpBMIH->biWidth * m_lpBMIH->biBitCount) % 32) {
			dwBytes++;
		}
		dwBytes *= 4;
		DWORD tmp_size = dwBytes * m_lpBMIH->biHeight;			// no compression
		//m_dwSizeImage = tmp_size > m_dwSizeImage ? tmp_size : m_dwSizeImage; 
		////version1�����ü���ʹ洢���ֽ����нϴ����Ϊ�����ʵ����windows�д��ļ����Կ�������ƺ�Ҳ���õĽϴ�����ֵ
		//if (tmp_size > m_dwSizeImage) {
		//	m_dwSizeImage = tmp_size;
		//}
		//version2�����ü���õ��Ľ��������Ϊ4�ı���
		m_dwSizeImage = tmp_size;
	}
}

void MyDIB::ComputePaletteSize(int nBitCount)
{
	if ((m_lpBMIH == nullptr) || (m_lpBMIH->biClrUsed == 0)) {

		switch (nBitCount) {

		case 1:

			m_nColorTableEntries = 2;

			break;

		case 4:

			m_nColorTableEntries = 16;

			break;

		case 8:

			m_nColorTableEntries = 256;

			break;

		case 16:

		case 24:

		case 32:

			m_nColorTableEntries = 0;

			break;

		default:

			ASSERT(FALSE);

		}

	}

	else {

		m_nColorTableEntries = m_lpBMIH->biClrUsed;

	}

	ASSERT((m_nColorTableEntries >= 0) && (m_nColorTableEntries <= 256));
}

void MyDIB::LoadImg(CString path)
{
	if (!path)
	{
		AfxMessageBox(L"Error of m_strPath!");
		return;
	}
	CFile pFile(path, CFile::modeRead | CFile::typeBinary);
	if (!pFile)	return;
	Read(&pFile);
	pFile.Close();
	m_strPath = path;
}

void MyDIB::SaveImg(CString path)
{
	//��DIBΪ������ʾ���󲢷���
	if (!m_lpBMIH)
	{
		AfxMessageBox(L"DIB��Ϣ�����ڣ�");
		return;
	}

	//ͨ���ļ�������CFile����
	//���ļ���ʧ������ʾ���󲢷���
	CFile pFile;
	if (!pFile.Open(path, CFile::modeCreate | CFile::modeReadWrite))
	{
		AfxMessageBox(L"�����ļ�ʧ�ܣ�");
		return;
	}
	Write(&pFile);
	pFile.Close();
}

void MyDIB::SetData(LPBYTE data)
{
	try {
		if (m_hBitmap)
			memcpy(m_lpImage, data, m_dwSizeImage);
		else if (m_lpImage)
		{
			//delete[] m_lpImage;
			memcpy(m_lpImage, data, m_dwSizeImage);
		}
		else
		{
			m_lpImage = (LPBYTE) new char[m_dwSizeImage];
			memcpy(m_lpImage, data, m_dwSizeImage);
		}
	}
	catch (CException * pe) {

		//AfxMessageBox("setData error");
		TRACE("setData error!\n");
		pe->Delete();
		return ;
	}
}

BOOL MyDIB::GetPixelRGB(int row, int col, int& r, int& g, int& b)		
{
	BYTE* lpImage = (BYTE*)m_lpImage;
	RGBQUAD* lpvColorTable = (RGBQUAD*)m_lpvColorTable;
	int index = (row - 1) * BytesPerline() + (col - 1) * GetbiBitCount() / 8;
	switch (m_lpBMIH->biBitCount)
	{
		//��ʱֻ����8λ��24λ��
	case 1:return FALSE;

	case 2:return FALSE;

	case 4:return FALSE;

	case 8: {
		b = (int)(lpvColorTable[*(lpImage + index)]).rgbBlue;
		g = (int)(lpvColorTable[*(lpImage + index)]).rgbGreen;
		r = (int)(lpvColorTable[*(lpImage + index)]).rgbRed;
	
	}

	case 24: {
		b = (int)(*(lpImage + index + 0));
		g = (int)(*(lpImage + index + 1));
		r = (int)(*(lpImage + index + 2));
		break;
	}

	default: 
		return FALSE;
	}
	return TRUE;
}
