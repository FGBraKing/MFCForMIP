
// MIPFasterDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MIPFaster.h"
#include "MIPFasterDlg.h"
#include "afxdialogex.h"
#include "ColorTrans.h"
#include "HSVDialog.h"
#include "LinearTransDlg.h"
#include "SegLinearDlg.h"
#include "SpatialEnhancement.h"
#include "ImageOperation.h"
#include "ImageFourierTrans.h"
#include "ImageFreqFilter.h"
#include "ImageRestore.h"
#include "ImageMorphology.h"
#include "ImageSegmentation.h"
#include "RotateDlg.h"
#include "global.h"

#define MIN(a, b)  (((a) < (b)) ? (a) : (b)) 
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMIPFasterDlg 对话框
static UINT  indicators[] = {	
	ID_INDICATOR_POS,
	ID_INDICATOR_RGB,
	ID_INDICATOR_HSV,
	ID_INDICATOR_TIME
};


CMIPFasterDlg::CMIPFasterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MIPFASTER_DIALOG, pParent)
	, m_fren(0)
	, m_rank(0)
	, m_inv_l(0)
	, m_winner_l(0)
	, m_winner_k(0)
	, m_structureSize(0)
	, m_threshold(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_orginImage = nullptr;
	m_transImage = nullptr;
	m_bTraned = false;
	m_resize = false;
	m_operationDlg = nullptr;
}

CMIPFasterDlg::~CMIPFasterDlg()
{
	if (m_orginImage)
		delete m_orginImage;
	if (m_transImage)
		delete m_transImage;
	if (m_operationDlg)
	{
		m_operationDlg->DestroyWindow();
		delete m_operationDlg;
		//m_operationDlg->DestroyWindow();
	}
}

void CMIPFasterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_ORGIN, m_imageOrgin);
	DDX_Control(pDX, IDC_IMAGE_TRANS, m_imageTrans);
	DDX_Control(pDX, IDC_RADIO_FILTER3, m_radio3);
	DDX_Control(pDX, IDC_RADIO_FILTER5, m_radio5);
	DDX_Control(pDX, IDC_RADIO_FILTER7, m_radio7);
	DDX_Control(pDX, IDC_SLIDER_SHRINK, m_slider_shrink);
	DDX_Control(pDX, IDC_COMBO_FILTER, m_filterType);
	DDX_Text(pDX, IDC_EDIT_FREN, m_fren);
	DDV_MinMaxInt(pDX, m_fren, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDIT_RANK, m_rank);
	DDV_MinMaxInt(pDX, m_rank, 0, 256);
	DDX_Text(pDX, IDC_EDIT_INV_L, m_inv_l);
	DDV_MinMaxInt(pDX, m_inv_l, 0, 1024);
	DDX_Text(pDX, IDC_EDIT_WIN_L, m_winner_l);
	DDV_MinMaxInt(pDX, m_winner_l, 0, 1024);
	DDX_Text(pDX, IDC_EDIT_WIN_K, m_winner_k);
	DDV_MinMaxDouble(pDX, m_winner_k, 0.0, DBL_MAX);
	DDX_Text(pDX, IDC_EDIT7, m_structureSize);
	DDV_MinMaxInt(pDX, m_structureSize, 0, 127);
	DDX_Control(pDX, IDC_COMBO_MORPH, m_structureType);
	DDX_Text(pDX, IDC_EDIT_SEG_PARA, m_threshold);
	DDV_MinMaxByte(pDX, m_threshold, 0, 255);
}

BEGIN_MESSAGE_MAP(CMIPFasterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMIPFasterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_OPEN, &CMIPFasterDlg::OnClickedOpen)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_HSV_UPDATE, &CMIPFasterDlg::OnHsvUpdate)
	ON_BN_CLICKED(IDC_BUTTON_HSV, &CMIPFasterDlg::OnBnClickedButtonHsv)
	ON_BN_CLICKED(IDC_BUTTON_TO_GRAY, &CMIPFasterDlg::OnBnClickedButtonToGray)
	ON_BN_CLICKED(IDC_CLOSE, &CMIPFasterDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_SAVE, &CMIPFasterDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON_LINEARTRANS, &CMIPFasterDlg::OnBnClickedButtonLineartrans)
	ON_BN_CLICKED(IDC_BUTTON_SEGTRANS, &CMIPFasterDlg::OnBnClickedButtonSegtrans)
	ON_BN_CLICKED(IDC_BUTTON_HISTOGRAM_EQ, &CMIPFasterDlg::OnBnClickedButtonHistogramEq)
	ON_BN_CLICKED(IDC_BUTTON_HIST_SPEC, &CMIPFasterDlg::OnBnClickedButtonHistSpec)
	ON_BN_CLICKED(IDC_BUTTON_LOWPASS_T, &CMIPFasterDlg::OnBnClickedButtonLowpassT)
	ON_BN_CLICKED(IDC_BUTTON_HIGHPASS_T, &CMIPFasterDlg::OnBnClickedButtonHighpassT)
	ON_BN_CLICKED(IDC_BUTTON_MIN_FILTER, &CMIPFasterDlg::OnBnClickedButtonMinFilter)
	ON_BN_CLICKED(IDC_BUTTON_MED_FILTER, &CMIPFasterDlg::OnBnClickedButtonMedFilter)
	ON_BN_CLICKED(IDC_BUTTONMAX_FILTER, &CMIPFasterDlg::OnBnClickedButtonmaxFilter)
	ON_BN_CLICKED(IDC_BUTTON_PSE_COLOR, &CMIPFasterDlg::OnBnClickedButtonPseColor)
	ON_BN_CLICKED(IDC_BUTTON_PLUS, &CMIPFasterDlg::OnBnClickedButtonPlus)
	ON_BN_CLICKED(IDC_BUTTON_MINUS, &CMIPFasterDlg::OnBnClickedButtonMinus)
	ON_BN_CLICKED(IDC_BUTTON_MULTI, &CMIPFasterDlg::OnBnClickedButtonMulti)
	ON_BN_CLICKED(IDC_BUTTON_DIV, &CMIPFasterDlg::OnBnClickedButtonDiv)
	ON_BN_CLICKED(IDC_BUTTON_AND, &CMIPFasterDlg::OnBnClickedButtonAnd)
	ON_BN_CLICKED(IDC_BUTTON_OR, &CMIPFasterDlg::OnBnClickedButtonOr)
	ON_BN_CLICKED(IDC_BUTTON_XOR, &CMIPFasterDlg::OnBnClickedButtonXor)
	ON_BN_CLICKED(IDC_BUTTON_NOT, &CMIPFasterDlg::OnBnClickedButtonNot)
	ON_BN_CLICKED(IDC_BUTTON_FILPX, &CMIPFasterDlg::OnBnClickedButtonFilpx)
	ON_BN_CLICKED(IDC_BUTTON_FILPY, &CMIPFasterDlg::OnBnClickedButtonFilpy)
	ON_BN_CLICKED(IDC_BUTTON_FOT90, &CMIPFasterDlg::OnBnClickedButtonFot90)
	ON_BN_CLICKED(IDC_BUTTON_FOT180, &CMIPFasterDlg::OnBnClickedButtonFot180)
	ON_WM_HSCROLL()
//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SHRINK, &CMIPFasterDlg::OnNMCustomdrawSliderShrink)
	ON_BN_CLICKED(IDC_BUTTON_FFT, &CMIPFasterDlg::OnBnClickedButtonFft)
	ON_BN_CLICKED(IDC_BUTTON_IFFT, &CMIPFasterDlg::OnBnClickedButtonIfft)
	ON_BN_CLICKED(IDC_BUTTON_FILTER_RUN, &CMIPFasterDlg::OnBnClickedButtonFilterRun)
	ON_BN_CLICKED(IDC_BUTTON_INV, &CMIPFasterDlg::OnBnClickedButtonInv)
	ON_BN_CLICKED(IDC_BUTTON_WINNER, &CMIPFasterDlg::OnBnClickedButtonWinner)
	ON_BN_CLICKED(IDC_BUTTON_EROSION, &CMIPFasterDlg::OnBnClickedButtonErosion)
	ON_BN_CLICKED(IDC_BUTTON_DIALATION, &CMIPFasterDlg::OnBnClickedButtonDialation)
	ON_BN_CLICKED(IDC_BUTTON_OPENING, &CMIPFasterDlg::OnBnClickedButtonOpening)
	ON_BN_CLICKED(IDC_BUTTON_CLOSING, &CMIPFasterDlg::OnBnClickedButtonClosing)
	ON_BN_CLICKED(IDC_BUTTON_SKELETON, &CMIPFasterDlg::OnBnClickedButtonSkeleton)
	ON_BN_CLICKED(IDC_BUTTON_MANSRG, &CMIPFasterDlg::OnBnClickedButtonMansrg)
	ON_BN_CLICKED(IDC_BUTTON_OSTSEG, &CMIPFasterDlg::OnBnClickedButtonOstseg)
	ON_BN_CLICKED(IDC_BUTTON_GROW, &CMIPFasterDlg::OnBnClickedButtonGrow)
	ON_BN_CLICKED(IDC_BUTTON_MERGE, &CMIPFasterDlg::OnBnClickedButtonMerge)
END_MESSAGE_MAP()


// CMIPFasterDlg 消息处理程序

BOOL CMIPFasterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	/*设置状态栏*/

	if (!m_dlgStatusBar.Create(this) || !m_dlgStatusBar.SetIndicators(indicators, 4))	//建立
	{
		MessageBox(_T("未能成功创建状态栏"));
	}
	m_dlgStatusBar.SetPaneInfo(0, ID_INDICATOR_POS, SBPS_STRETCH, 200);
	m_dlgStatusBar.SetPaneInfo(1, ID_INDICATOR_RGB, SBPS_STRETCH, 200);
	m_dlgStatusBar.SetPaneInfo(2, ID_INDICATOR_HSV, SBPS_STRETCH,200);
	m_dlgStatusBar.SetPaneInfo(3, ID_INDICATOR_TIME, SBPS_STRETCH, 200);
	m_dlgStatusBar.GetStatusBarCtrl().SetBkColor(RGB(250, 165, 66));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, AFX_IDW_CONTROLBAR_FIRST);
	/*启用定时器，1s运行一次*/
	SetTimer(1, 1000, NULL);
	//设置单选按钮
	m_radio3.SetCheck(TRUE);
	m_radio5.SetCheck(FALSE);
	m_radio7.SetCheck(FALSE);

	m_slider_shrink.SetRange(0, 200); //设置滑动条的调节范围，这里的范围后面将会计算转换为[0,2]
	m_slider_shrink.SetPageSize(10);  //设置滑动条翻页步长
	m_slider_shrink.SetLineSize(0.1);  //设置滑动条按键步长
	m_slider_shrink.SetPos(100);
	//设置滤波器类型默认值
	TRACE("filter num is :%d\n", m_filterType.GetCount());
	//m_filterType.SelectString(0, _T("请选择"));
	m_filterType.SetCurSel(5);
	if (!m_operationDlg)
	{
		m_operationDlg = new ImageOperationDlg;
	}
	m_operationDlg->Create(IDD_DIALOG_OPERATION, this);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE

}

//  如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMIPFasterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	if (m_orginImage&&m_orginImage->GetBits())
	{
		CWnd* pLeftWnd = GetDlgItem(IDC_IMAGE_ORGIN);
		CDC* pLeftDC = pLeftWnd->GetDC();
		CRect rectWnd;
		pLeftWnd->GetClientRect(&rectWnd);	//取窗口大小
		m_orginImage->Draw(pLeftDC, (CPoint(0, 0)), CSize(rectWnd.Width(), rectWnd.Height()));
		//m_orginImage->SetToDC(*pLeftDC, rectWnd, (CPoint(0, 0)));
	}
	if (m_transImage&& m_transImage->GetBits()&&m_bTraned)
	{
		CWnd* pRightWnd = GetDlgItem(IDC_IMAGE_TRANS);
		CDC* pRightDC = pRightWnd->GetDC();
		CRect rectWnd;
		pRightWnd->GetClientRect(&rectWnd);	//取窗口大小
		
		if (m_resize == false)
		{
			m_transImage->Draw(pRightDC, (CPoint(0, 0)), CSize(rectWnd.Width(), rectWnd.Height()));
		}
		else
		{
			m_transImage->SetToDC(*pRightDC,rectWnd,CPoint(0,0));
		}
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMIPFasterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMIPFasterDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CMIPFasterDlg::OnClickedOpen()
{
	/*打开文件对话框*/
	CString filter = L"所有文件(*.bmp,*.jpg,*.tiff)|*.bmp;*.jpg;*.gif;*.tiff| BMP(*.bmp)|*.bmp|JPG(*.jpg)|*.jpg|GIF(*.gif)|TIFF(*.tiff)|*.tiff";
	CFileDialog fileDLg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter, NULL);
	if (IDOK == fileDLg.DoModal())
	{
		CString fPath = fileDLg.GetPathName();			// 获取文件路径  
		CString fEnt = fileDLg.GetFileExt();			// 获取文件扩展名  
		fEnt.MakeLower();								// 将文件扩展名转化为小写字母  

		if (fEnt.Compare(_T("bmp")) == 0)
		{	// 如果打开的是bmp文件
			if (m_orginImage)
			{
				delete m_orginImage;
			}
			m_orginImage = new MyDIB(fPath);
		}
		else
		{
			AfxMessageBox(_T("文件格式不符合要求！"));
		}
		OnPaint();
	}
}


void CMIPFasterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:{
		CTime curTime = CTime::GetCurrentTime();
		// 格式化curTime，将字符串保存到strTime   
		CString strTime = curTime.Format(_T("%Y年%m月%d日 %H:%M:%S"));
		// 在状态栏的时间窗格中显示系统时间字符串   
		m_dlgStatusBar.SetPaneText(3, strTime);
		break;
	}
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CMIPFasterDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString strPos;
	CString strRGB;
	CString strHSV;

	COLORREF color;
	float h, s, v;
	//获取鼠标相对图片控件原点的坐标
	////TRACE("point,x:%d\ty:%d\n", point.x, point.y);
	CRect rectL, rectR;
	GetDlgItem(IDC_IMAGE_ORGIN)->GetWindowRect(&rectL);
	GetDlgItem(IDC_IMAGE_TRANS)->GetWindowRect(&rectR);
	/*TRACE("LEFT:left:%d\ttop:%d\theight:%d\twidth:%d\t\n", rectL.left, rectL.top, rectL.Height(), rectL.Width());
	TRACE("RIGHT:left:%d\ttop:%d\theight:%d\twidth:%d\t\n", rectR.left, rectR.top, rectR.Height(), rectR.Width());*/
	ScreenToClient(rectL);
	ScreenToClient(rectR);
	CPoint pointLeft, pointRight;
	pointLeft.x = point.x - rectL.left;
	pointLeft.y = point.y - rectL.top;
	pointRight.x = point.x - rectR.left;
	pointRight.y = point.y - rectR.top;
	////TRACE("pointLeft:%d\t%d\n", pointLeft.x, pointLeft.y);
	//判断当前鼠标位于图片控件/控件外
	if (pointLeft.x>0&&pointLeft.x<rectL.Height()&&pointLeft.y>0&&pointLeft.y<rectL.Width())	//左边图像控件
	{
		strPos.Format(_T("坐标: (%d, %d)"), pointLeft.x, pointLeft.y);
		color = m_imageOrgin.GetDC()->GetPixel(pointLeft);										//从设备而非DIB中读颜色
		strRGB.Format(_T("RGB(%d, %d, %d)"), GetRValue(color), GetGValue(color), GetBValue(color));
		RGB2HSV(GetRValue(color), GetGValue(color), GetBValue(color), h, s, v);
		strHSV.Format(_T("HSV(%.3f, %.3f, %.3f)"), h, s, v);
	}
	else if (pointRight.x > 0 && pointRight.x < rectR.Height() && pointRight.y>0 && pointRight.y < rectR.Width())
	{
		strPos.Format(_T("坐标: (%d, %d)"), pointRight.x, pointRight.y);
		color = m_imageTrans.GetDC()->GetPixel(pointRight);
		strRGB.Format(_T("RGB(%d, %d, %d)"), GetRValue(color), GetGValue(color), GetBValue(color));
		RGB2HSV(GetRValue(color), GetGValue(color), GetBValue(color), h, s, v);
		strHSV.Format(_T("HSV(%.3f, %.3f, %.3f)"), h, s, v);
	}
	else
	{
		strPos.Format(_T("坐标: (%d, %d)"), point.x, point.y);
		strRGB.Format(_T("RGB(%d, %d, %d)"), 128,128,128);
		RGB2HSV(128,128,128, h, s, v);
		strHSV.Format(_T("HSV(%.3f, %.3f, %.3f)"), h, s, v);
	}

	m_dlgStatusBar.SetPaneText(0, strPos);
	m_dlgStatusBar.SetPaneText(1, strRGB);
	m_dlgStatusBar.SetPaneText(2, strHSV);
	CDialogEx::OnMouseMove(nFlags, point);
}


afx_msg LRESULT CMIPFasterDlg::OnHsvUpdate(WPARAM wParam, LPARAM lParam)
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return 0;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	//TRACE("I can accept the message!\n");
	HSVARRAY hsvArray = *((HSVARRAY*)lParam);
	ColorTrans hsvTrans;
	hsvTrans.HSVAdjust(*m_orginImage, *m_transImage, hsvArray.h, hsvArray.s, hsvArray.v);
	m_bTraned = true;
	m_resize = false;
	OnPaint();
	return 1;
}

void CMIPFasterDlg::OnBnClickedButtonHsv()
{
	// TODO: 在此添加控件通知处理程序代码
	HSVDialog hsvDlg(this);
	if (m_orginImage)
	{
		if (IDOK == hsvDlg.DoModal())
		{
		}
	}
}


void CMIPFasterDlg::OnBnClickedButtonToGray()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ColorTrans grayTrans;
	grayTrans.Color2Gray(*m_orginImage, *m_transImage);
	m_bTraned = true;
	m_resize = false;
	OnPaint();
}


void CMIPFasterDlg::OnBnClickedClose()
{
	// TODO: 在此添加控件通知处理程序代码
	PostMessage(WM_CLOSE);
}


void CMIPFasterDlg::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_transImage&&m_transImage->GetBits()&&m_bTraned)
	{
		CString strFilePathName;
		CFileDialog dlg(FALSE);	///TRUE为OPEN对话框，FALSE为SAVE AS对话框
		if (IDOK == dlg.DoModal())
			strFilePathName = dlg.GetPathName();
		m_transImage->SaveImg(strFilePathName);
	}
}


void CMIPFasterDlg::OnBnClickedButtonLineartrans()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	LinearTransDlg linearTransDlg(this);

	if (IDOK == linearTransDlg.DoModal())
	{
		int gray_min = linearTransDlg.m_linearPara_min;
		int gray_max = linearTransDlg.m_linearPara_max;
		int orgin_min = linearTransDlg.m_linearPara_orgin_min;
		int orgin_max = linearTransDlg.m_linearPara_orgin_max;
		if (!m_transImage)
		{
			m_transImage = new MyDIB();
		}
		SpatialEnhancement spatialTrans;
		if (spatialTrans.LinearTrans(*m_orginImage, *m_transImage, orgin_min, orgin_max, gray_min, gray_max))
		{
			m_bTraned = true;
			m_resize = false;
			OnPaint();
		}	
	}
}


void CMIPFasterDlg::OnBnClickedButtonSegtrans()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	SegLinearDlg segLinearTransDlg(this);

	if (IDOK == segLinearTransDlg.DoModal())
	{
		int gray_x1 = segLinearTransDlg.m_gray_x1;
		int gray_x2 = segLinearTransDlg.m_gray_x2;
		int gray_y1 = segLinearTransDlg.m_gray_y1;
		int gray_y2 = segLinearTransDlg.m_gray_y2;
		if (!m_transImage)
		{
			m_transImage = new MyDIB();
		}
		SpatialEnhancement spatialTrans;
		TRACE("x1:%d,x2:%d,y1:%d,y2:%d\n", gray_x1, gray_x2, gray_y1, gray_y2);
		if (spatialTrans.SegLinearTrans(*m_orginImage, *m_transImage, gray_x1, gray_x2, gray_y1, gray_y2))
		{
			m_bTraned = true;
			m_resize = false;
			OnPaint();
		}	
	}
}


void CMIPFasterDlg::OnBnClickedButtonHistogramEq()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	SpatialEnhancement spatialTrans;
	if (spatialTrans.HistogramEqualizatio(*m_orginImage, *m_transImage))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonHistSpec()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}

	int n[128];
	float p[128];
	float a = 1.0f / (127.0f * 64.0f);
	for (int i = 0; i < 128; i++)
	{
		n[i] = i * 2;
		p[i] = a * i;
	}
	SpatialEnhancement spatialTrans;
	if (spatialTrans.HistogramSpecification(*m_orginImage, *m_transImage,128,n,p))
	{
		TRACE("now is in HistogramSpecification\n");
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonLowpassT()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	SpatialEnhancement spatialTrans;
	int kernelSize = getKernelSize();
	if (spatialTrans.LowPassFilter(*m_orginImage, *m_transImage,kernelSize))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}

int CMIPFasterDlg::getKernelSize()
{
	if (m_radio3.GetCheck())
	{
		return 3;
	}
	else if(m_radio5.GetCheck())
	{
		return 5;
	}
	else if (m_radio7.GetCheck())
	{
		return 7;
	}
	ASSERT(FALSE);
}


void CMIPFasterDlg::OnBnClickedButtonHighpassT()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	SpatialEnhancement spatialTrans;
	int kernelSize = getKernelSize();
	if (spatialTrans.HighPassFilter(*m_orginImage, *m_transImage, kernelSize))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonMinFilter()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	SpatialEnhancement spatialTrans;
	int kernelSize = getKernelSize();
	if (spatialTrans.MinFilter(*m_orginImage, *m_transImage, kernelSize))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonMedFilter()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	SpatialEnhancement spatialTrans;
	int kernelSize = getKernelSize();
	if (spatialTrans.MedianFilter(*m_orginImage, *m_transImage, kernelSize))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonmaxFilter()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	SpatialEnhancement spatialTrans;
	int kernelSize = getKernelSize();
	if (spatialTrans.MaxFilter(*m_orginImage, *m_transImage, kernelSize))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonPseColor()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	SpatialEnhancement spatialTrans;
	if (spatialTrans.PseudoColorEnhance(*m_orginImage, *m_transImage))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonPlus()
{
	// TODO: 在此添加控件通知处理程序代码
	m_operationDlg->ShowWindow(SW_SHOW);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	if (m_operationDlg->m_image&&m_operationDlg->m_image->GetBits())
	{
		MyDIB* img_plus = m_operationDlg->m_image;

		TRACE("ImageA###GetbiSizeImage:%d,GetSizeImage:%d\n",m_orginImage->GetbiSizeImage(),m_orginImage->GetSizeImage());
		TRACE("ImageB###GetbiSizeImage:%d,GetSizeImage:%d\n", img_plus->GetbiSizeImage(), img_plus->GetSizeImage());
		ImageOperation imageOperation;
		imageOperation.setImagePara(m_operationDlg->m_alpha, m_operationDlg->m_beta);
		if (imageOperation.imageAdd(*m_orginImage, *img_plus, *m_transImage))
		{
			m_bTraned = true;
			m_resize = false;
			OnPaint();
		}
	}
}
///*
// * ShowWindow() Commands
// */
//#define SW_HIDE             0
//#define SW_SHOWNORMAL       1
//#define SW_NORMAL           1
//#define SW_SHOWMINIMIZED    2
//#define SW_SHOWMAXIMIZED    3
//#define SW_MAXIMIZE         3
//#define SW_SHOWNOACTIVATE   4
//#define SW_SHOW             5
//#define SW_MINIMIZE         6
//#define SW_SHOWMINNOACTIVE  7
//#define SW_SHOWNA           8
//#define SW_RESTORE          9
//#define SW_SHOWDEFAULT      10
//#define SW_FORCEMINIMIZE    11
//#define SW_MAX              11

void CMIPFasterDlg::OnBnClickedButtonMinus()
{
	m_operationDlg->ShowWindow(SW_SHOW);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	if (m_operationDlg->m_image && m_operationDlg->m_image->GetBits())
	{
		MyDIB* img_plus = m_operationDlg->m_image;

		TRACE("ImageA###GetbiSizeImage:%d,GetSizeImage:%d\n", m_orginImage->GetbiSizeImage(), m_orginImage->GetSizeImage());
		TRACE("ImageB###GetbiSizeImage:%d,GetSizeImage:%d\n", img_plus->GetbiSizeImage(), img_plus->GetSizeImage());
		ImageOperation imageOperation;
		imageOperation.setImagePara(m_operationDlg->m_alpha, m_operationDlg->m_beta);
		if (imageOperation.imageSub(*m_orginImage, *img_plus, *m_transImage))
		{
			m_bTraned = true;
			m_resize = false;
			OnPaint();
		}
	}
}


void CMIPFasterDlg::OnBnClickedButtonMulti()
{
	m_operationDlg->ShowWindow(SW_SHOW);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	if (m_operationDlg->m_image && m_operationDlg->m_image->GetBits())
	{
		MyDIB* img_plus = m_operationDlg->m_image;

		TRACE("ImageA###GetbiSizeImage:%d,GetSizeImage:%d\n", m_orginImage->GetbiSizeImage(), m_orginImage->GetSizeImage());
		TRACE("ImageB###GetbiSizeImage:%d,GetSizeImage:%d\n", img_plus->GetbiSizeImage(), img_plus->GetSizeImage());
		ImageOperation imageOperation;
		imageOperation.setImagePara(m_operationDlg->m_alpha, m_operationDlg->m_beta);
		if (imageOperation.imageMul(*m_orginImage, *img_plus, *m_transImage))
		{
			m_bTraned = true;
			m_resize = false;
			OnPaint();
		}
	}
}


void CMIPFasterDlg::OnBnClickedButtonDiv()
{
	m_operationDlg->ShowWindow(SW_SHOW);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	if (m_operationDlg->m_image && m_operationDlg->m_image->GetBits())
	{
		MyDIB* img_plus = m_operationDlg->m_image;

		TRACE("ImageA###GetbiSizeImage:%d,GetSizeImage:%d\n", m_orginImage->GetbiSizeImage(), m_orginImage->GetSizeImage());
		TRACE("ImageB###GetbiSizeImage:%d,GetSizeImage:%d\n", img_plus->GetbiSizeImage(), img_plus->GetSizeImage());
		ImageOperation imageOperation;
		imageOperation.setImagePara(m_operationDlg->m_alpha, m_operationDlg->m_beta);
		if (imageOperation.imageDiv(*m_orginImage, *img_plus, *m_transImage))
		{
			m_bTraned = true;
			m_resize = false;
			OnPaint();
		}
	}
}


void CMIPFasterDlg::OnBnClickedButtonAnd()
{
	m_operationDlg->ShowWindow(SW_SHOW);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	if (m_operationDlg->m_image && m_operationDlg->m_image->GetBits())
	{
		MyDIB* img_plus = m_operationDlg->m_image;

		TRACE("ImageA###GetbiSizeImage:%d,GetSizeImage:%d\n", m_orginImage->GetbiSizeImage(), m_orginImage->GetSizeImage());
		TRACE("ImageB###GetbiSizeImage:%d,GetSizeImage:%d\n", img_plus->GetbiSizeImage(), img_plus->GetSizeImage());
		ImageOperation imageOperation;
		imageOperation.setImagePara(m_operationDlg->m_alpha, m_operationDlg->m_beta);
		if (imageOperation.imageAnd(*m_orginImage, *img_plus, *m_transImage))
		{
			m_bTraned = true;
			m_resize = false;
			OnPaint();
		}
	}
}


void CMIPFasterDlg::OnBnClickedButtonOr()
{
	m_operationDlg->ShowWindow(SW_SHOW);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	if (m_operationDlg->m_image && m_operationDlg->m_image->GetBits())
	{
		MyDIB* img_plus = m_operationDlg->m_image;

		TRACE("ImageA###GetbiSizeImage:%d,GetSizeImage:%d\n", m_orginImage->GetbiSizeImage(), m_orginImage->GetSizeImage());
		TRACE("ImageB###GetbiSizeImage:%d,GetSizeImage:%d\n", img_plus->GetbiSizeImage(), img_plus->GetSizeImage());
		ImageOperation imageOperation;
		imageOperation.setImagePara(m_operationDlg->m_alpha, m_operationDlg->m_beta);
		if (imageOperation.imageOr(*m_orginImage, *img_plus, *m_transImage))
		{
			m_bTraned = true;
			m_resize = false;
			OnPaint();
		}
	}
}


void CMIPFasterDlg::OnBnClickedButtonXor()
{
	m_operationDlg->ShowWindow(SW_SHOW);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	if (m_operationDlg->m_image && m_operationDlg->m_image->GetBits())
	{
		MyDIB* img_plus = m_operationDlg->m_image;

		TRACE("ImageA###GetbiSizeImage:%d,GetSizeImage:%d\n", m_orginImage->GetbiSizeImage(), m_orginImage->GetSizeImage());
		TRACE("ImageB###GetbiSizeImage:%d,GetSizeImage:%d\n", img_plus->GetbiSizeImage(), img_plus->GetSizeImage());
		ImageOperation imageOperation;
		imageOperation.setImagePara(m_operationDlg->m_alpha, m_operationDlg->m_beta);
		if (imageOperation.imageXor(*m_orginImage, *img_plus, *m_transImage))
		{
			m_bTraned = true;
			m_resize = false;
			OnPaint();
		}
	}
}


void CMIPFasterDlg::OnBnClickedButtonNot()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageOperation imageOperation;
	if (imageOperation.imageNot(*m_orginImage, *m_transImage))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonFilpx()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageOperation imageOperation;
	if (imageOperation.imageMirror(*m_orginImage, *m_transImage,filpHorizontal))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonFilpy()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageOperation imageOperation;
	if (imageOperation.imageMirror(*m_orginImage, *m_transImage, filpVertical))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonFot90()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageOperation imageOperation;
	if (imageOperation.imageMirror(*m_orginImage, *m_transImage, filpDiagonal))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonFot180()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	/*CEdit myEdit;
	CRect myRect(10,10,100,100);
	myEdit.Create(ES_NUMBER, myRect,this, 0);
	myEdit.ShowWindow(SW_SHOW);*/
	RotateDlg rotateDlg;
	if (rotateDlg.DoModal()==IDOK)
	{
		int angle = rotateDlg.m_angle;
		ImageOperation imageOperation;
		if (imageOperation.imageRot(*m_orginImage, *m_transImage, angle))
		{
			m_bTraned = true;
			m_resize = false;
			OnPaint();
		}
	}
}


void CMIPFasterDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);

	int nID = pScrollBar->GetDlgCtrlID();
	TRACE("nID:%d\n",nID);
	if (nID == IDC_SLIDER_SHRINK)
	{
		TRACE("IDC_SLIDER_SHRINK:%d\n", IDC_SLIDER_SHRINK);
		switch (nSBCode)
		{
		default:
			float scale_img = nPos / 200.0;	//pScrollBar->GetScrollPos()
			int aim_width = m_orginImage->GetWidth() * scale_img;
			int aim_heitht = m_orginImage->GetHeight() * scale_img;
			if (!m_transImage)
			{
				m_transImage = new MyDIB();
			}
			ImageOperation imageOperation;
			if (imageOperation.imageResize(*m_orginImage, *m_transImage, aim_width,aim_heitht,resize_bilinear))
			{
				m_bTraned = true;
				m_resize = true;
				//ValidateRect();
				CWnd* pRightWnd = GetDlgItem(IDC_IMAGE_TRANS);
				CDC* pRightDC = pRightWnd->GetDC();
				CRect rectWnd;
				pRightWnd->GetClientRect(&rectWnd);	//取窗口大小
				pRightDC->FillSolidRect(&rectWnd, RGB(200, 200, 200));
				//InvalidateRect(rectWnd,TRUE);
				OnPaint();
			}
			break;
		}
	}
}


//void CMIPFasterDlg::OnNMCustomdrawSliderShrink(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
//}


void CMIPFasterDlg::OnBnClickedButtonFft()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageFourierTrans imageFourierTrans;
	if (imageFourierTrans.transform(*m_orginImage, *m_transImage))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonIfft()
{
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageFourierTrans imageFourierTrans;
	if (imageFourierTrans.inverseTransform(*m_orginImage, *m_transImage))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonFilterRun()
{
	// TODO: 在此添加控件通知处理程序代码
	int cur_filter = m_filterType.GetCurSel();
	if (cur_filter>=5||cur_filter<0)
	{
		AfxMessageBox(L"请选择一个合适的滤波类型！");
		cur_filter = 5;
		m_filterType.SetCurSel(cur_filter);
		return;
	}
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}

	ImageFreqFilter imageFreqFilter;
	bool filter_result = false;
	switch (cur_filter)
	{
	case 0: {filter_result = imageFreqFilter.FreqHomomorphicFilter(*m_orginImage, *m_transImage, m_fren, m_rank); break; }
	case 1: {filter_result = imageFreqFilter.FreqButworthLowPassFilter(*m_orginImage, *m_transImage, m_fren, m_rank); break; }
	case 2: {filter_result = imageFreqFilter.FreqButworthHighPassFilter(*m_orginImage, *m_transImage, m_fren, m_rank); break; }
	case 3: {filter_result = imageFreqFilter.FreqLowPassFilter(*m_orginImage, *m_transImage, m_fren); break; }
	case 4: {filter_result = imageFreqFilter.FreqHighPassFilter(*m_orginImage, *m_transImage, m_fren); break; }
	default:
		break;
	}
	if (filter_result)
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonInv()
{
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageRestore imageRestore;
	if (imageRestore.InverseFilter(*m_orginImage, *m_transImage, m_inv_l))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonWinner()
{
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageRestore imageRestore;
	if (imageRestore.WinnerFilter(*m_orginImage, *m_transImage, m_winner_l, m_winner_k))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonErosion()
{
	// TODO: 在此添加控件通知处理程序代码
	int cur_structure = m_structureType.GetCurSel();
	if (cur_structure > 3 || cur_structure < 0)
	{
		AfxMessageBox(L"请选择一个合适的结构元类型！");
		cur_structure = 2;
		m_structureType.SetCurSel(cur_structure);
		return;
	}
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageSegmentation imageSegmentation;
	MyDIB tmp_image;
	if (!imageSegmentation.ManSeg(*m_orginImage, tmp_image, 100))
	{
		return;
	}

	ImageMorphology imageMorphology;
	structureType struct_type;
	switch (cur_structure)
	{
	case 0: { struct_type = cross; break; }
	case 1: { struct_type = circle; break; }
	case 2: { struct_type = square; break; }
	case 3: { struct_type = diamond; break; }
	default:
		struct_type = cross;
		break;
	}
	if (imageMorphology.Erosion(tmp_image, *m_transImage, m_structureSize, struct_type))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonDialation()
{
	// TODO: 在此添加控件通知处理程序代码
	int cur_structure = m_structureType.GetCurSel();
	if (cur_structure > 3 || cur_structure < 0)
	{
		AfxMessageBox(L"请选择一个合适的结构元类型！");
		cur_structure = 2;
		m_structureType.SetCurSel(cur_structure);
		return;
	}
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}

	ImageSegmentation imageSegmentation;
	MyDIB tmp_image;
	if (!imageSegmentation.ManSeg(*m_orginImage, tmp_image, 100))
	{
		return;
	}

	ImageMorphology imageMorphology;
	structureType struct_type;
	switch (cur_structure)
	{
	case 0: { struct_type = circle; break; }
	case 1: { struct_type = cross; break; }
	case 2: { struct_type = diamond; break; }
	case 3: { struct_type = square; break; }
	default:
		struct_type = cross;
		break;
	}
	if (imageMorphology.Dilation(tmp_image, *m_transImage, m_structureSize, struct_type))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonOpening()
{
	// TODO: 在此添加控件通知处理程序代码
	int cur_structure = m_structureType.GetCurSel();
	if (cur_structure > 3 || cur_structure < 0)
	{
		AfxMessageBox(L"请选择一个合适的结构元类型！");
		cur_structure = 2;
		m_structureType.SetCurSel(cur_structure);
		return;
	}
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}

	ImageSegmentation imageSegmentation;
	MyDIB tmp_image;
	if (!imageSegmentation.ManSeg(*m_orginImage, tmp_image, 100))
	{
		return;
	}

	ImageMorphology imageMorphology;
	structureType struct_type;
	switch (cur_structure)
	{
	case 0: { struct_type = circle; break; }
	case 1: { struct_type = cross; break; }
	case 2: { struct_type = diamond; break; }
	case 3: { struct_type = square; break; }
	default:
		struct_type = cross;
		break;
	}
	if (imageMorphology.Opening(tmp_image, *m_transImage, m_structureSize, struct_type))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonClosing()
{
	// TODO: 在此添加控件通知处理程序代码
	int cur_structure = m_structureType.GetCurSel();
	if (cur_structure > 3 || cur_structure < 0)
	{
		AfxMessageBox(L"请选择一个合适的结构元类型！");
		cur_structure = 2;
		m_structureType.SetCurSel(cur_structure);
		return;
	}
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}

	ImageSegmentation imageSegmentation;
	MyDIB tmp_image;
	if (!imageSegmentation.ManSeg(*m_orginImage, tmp_image, 100))
	{
		return;
	}

	ImageMorphology imageMorphology;
	structureType struct_type;
	switch (cur_structure)
	{
	case 0: { struct_type = circle; break; }
	case 1: { struct_type = cross; break; }
	case 2: { struct_type = diamond; break; }
	case 3: { struct_type = square; break; }
	default:
		struct_type = cross;
		break;
	}
	if (imageMorphology.Closing(tmp_image, *m_transImage, m_structureSize, struct_type))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonSkeleton()
{
	// TODO: 在此添加控件通知处理程序代码
	int cur_structure = m_structureType.GetCurSel();
	if (cur_structure > 3 || cur_structure < 0)
	{
		AfxMessageBox(L"请选择一个合适的结构元类型！");
		cur_structure = 2;
		m_structureType.SetCurSel(cur_structure);
		return;
	}
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}

	ImageSegmentation imageSegmentation;
	MyDIB tmp_image;
	if (!imageSegmentation.ManSeg(*m_orginImage, tmp_image, 100))
	{
		return;
	}

	ImageMorphology imageMorphology;
	structureType struct_type;
	switch (cur_structure)
	{
	case 0: { struct_type = circle; break; }
	case 1: { struct_type = cross; break; }
	case 2: { struct_type = diamond; break; }
	case 3: { struct_type = square; break; }
	default:
		struct_type = cross;
		break;
	}
	if (imageMorphology.Skeleton(tmp_image, *m_transImage, m_structureSize, struct_type))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonMansrg()
{
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageSegmentation imageSegmentation;
	if (imageSegmentation.ManSeg(*m_orginImage, *m_transImage, m_threshold))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonOstseg()
{
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageSegmentation imageSegmentation;
	int otuThreshold = imageSegmentation.OstuSThreshold(*m_orginImage);
	if (imageSegmentation.ManSeg(*m_orginImage, *m_transImage, otuThreshold))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonGrow()
{
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageSegmentation imageSegmentation;
	if (imageSegmentation.RegionGrow(*m_orginImage, *m_transImage, m_threshold))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}


void CMIPFasterDlg::OnBnClickedButtonMerge()
{
	UpdateData(TRUE);
	if (!m_orginImage || !m_orginImage->m_lpImage)
		return;
	if (!m_transImage)
	{
		m_transImage = new MyDIB();
	}
	ImageSegmentation imageSegmentation;
	if (imageSegmentation.RegionSplitMerge(*m_orginImage, *m_transImage))
	{
		m_bTraned = true;
		m_resize = false;
		OnPaint();
	}
}
