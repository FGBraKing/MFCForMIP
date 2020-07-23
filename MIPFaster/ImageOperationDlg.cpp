// ImageOperationDlg.cpp: 实现文件
//

#include "pch.h"
#include "MIPFaster.h"
#include "ImageOperationDlg.h"
#include "afxdialogex.h"


// ImageOperationDlg 对话框

IMPLEMENT_DYNAMIC(ImageOperationDlg, CDialogEx)

ImageOperationDlg::ImageOperationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_OPERATION, pParent)
	, m_image(nullptr)
	, m_alpha(0.5)
	, m_beta(0.5)
{

}

ImageOperationDlg::~ImageOperationDlg()
{
	if (m_image)
	{
		delete m_image;
	}
}

void ImageOperationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ALPHA, m_alpha);
	DDV_MinMaxFloat(pDX, m_alpha, 0, 1);
	DDX_Text(pDX, IDC_EDIT_BETA, m_beta);
	DDV_MinMaxFloat(pDX, m_beta, 0, 1);
}


BEGIN_MESSAGE_MAP(ImageOperationDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OPERA_CLOSE, &ImageOperationDlg::OnBnClickedButtonOperaClose)
	ON_BN_CLICKED(IDC_BUTTON_OPERA_OPEN, &ImageOperationDlg::OnBnClickedButtonOperaOpen)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_EDIT_ALPHA, &ImageOperationDlg::OnChangeEditAlpha)
	ON_EN_CHANGE(IDC_EDIT_BETA, &ImageOperationDlg::OnChangeEditBeta)
END_MESSAGE_MAP()


// ImageOperationDlg 消息处理程序


void ImageOperationDlg::OnBnClickedButtonOperaClose()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_image)
	{
		delete m_image;
	}
	m_image = nullptr;
	EndDialog(IDCLOSE);
}


void ImageOperationDlg::OnBnClickedButtonOperaOpen()
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
			if (m_image)
			{
				delete m_image;
			}
			m_image = new MyDIB(fPath);
		}
		else
		{
			AfxMessageBox(_T("文件格式不符合要求！"));
		}
		OnPaint();
	}

}


void ImageOperationDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	if (m_image && m_image->GetBits())
	{
		CWnd* pWnd = GetDlgItem(IDC_STATIC_OPERA_IMAGEB);
		CDC* pDC = pWnd->GetDC();
		CRect rectWnd;
		pWnd->GetClientRect(&rectWnd);	//取窗口大小
		m_image->Draw(pDC, (CPoint(0, 0)), CSize(rectWnd.Width(), rectWnd.Height()));
		//m_orginImage->SetToDC(*pLeftDC, rectWnd, (CPoint(0, 0)));
	}
}


BOOL ImageOperationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	/*m_image = nullptr;
	m_alpha = 0.5;
	m_beta = 0.5;*/
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void ImageOperationDlg::OnChangeEditAlpha()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
}


void ImageOperationDlg::OnChangeEditBeta()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
}


//void ImageOperationDlg::OnCancel()
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//	//CDialogEx::OnCancel();
//	DestroyWindow();
//}
//
//
//void ImageOperationDlg::PostNcDestroy()
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//	CDialogEx::PostNcDestroy();
//	delete this;
//}
