// SegLinearDlg.cpp: 实现文件
//

#include "pch.h"
#include "MIPFaster.h"
#include "SegLinearDlg.h"
#include "afxdialogex.h"


// SegLinearDlg 对话框

IMPLEMENT_DYNAMIC(SegLinearDlg, CDialogEx)

SegLinearDlg::SegLinearDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SEGLINEAR_PARA, pParent)
	, m_gray_x1(0)
	, m_gray_x2(0)
	, m_gray_y1(0)
	, m_gray_y2(0)
{

}

SegLinearDlg::~SegLinearDlg()
{
}

void SegLinearDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_gray_x1);
	DDV_MinMaxByte(pDX, m_gray_x1, 0, 255);
	DDX_Text(pDX, IDC_EDIT2, m_gray_x2);
	DDV_MinMaxByte(pDX, m_gray_x2, 0, 255);
	DDX_Text(pDX, IDC_EDIT3, m_gray_y1);
	DDV_MinMaxByte(pDX, m_gray_y1, 0, 255);
	DDX_Text(pDX, IDC_EDIT4, m_gray_y2);
	DDV_MinMaxByte(pDX, m_gray_y2, 0, 255);
}


BEGIN_MESSAGE_MAP(SegLinearDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_YES, &SegLinearDlg::OnBnClickedButtonYes)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, &SegLinearDlg::OnBnClickedButtonCancle)
END_MESSAGE_MAP()


// SegLinearDlg 消息处理程序


void SegLinearDlg::OnBnClickedButtonYes()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_gray_x2 <= m_gray_x1)
	{
		m_gray_x1 = 50;
		m_gray_x2 = 200;
		AfxMessageBox(L"x1应该小于x2！");
	}
	if (m_gray_y2 <= m_gray_y1)
	{
		m_gray_y1 = 25;
		m_gray_y2 = 225;
		AfxMessageBox(L"y1应该小于y2！");
	}
	UpdateData(FALSE);
	EndDialog(IDOK);
}


void SegLinearDlg::OnBnClickedButtonCancle()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(IDCANCEL);
}
