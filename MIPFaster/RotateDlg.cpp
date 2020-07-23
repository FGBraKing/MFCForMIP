// RotateDlg.cpp: 实现文件
//

#include "pch.h"
#include "MIPFaster.h"
#include "RotateDlg.h"
#include "afxdialogex.h"


// RotateDlg 对话框

IMPLEMENT_DYNAMIC(RotateDlg, CDialogEx)

RotateDlg::RotateDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ROTATE, pParent)
	, m_angle(0)
{

}

RotateDlg::~RotateDlg()
{
}

void RotateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ANGLE, m_angle);
	DDV_MinMaxInt(pDX, m_angle, -360, 360);
}


BEGIN_MESSAGE_MAP(RotateDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_ANGLE, &RotateDlg::OnChangeEditAngle)
	ON_BN_CLICKED(IDC_CANCAL, &RotateDlg::OnBnClickedCancal)
	ON_BN_CLICKED(IDC_BUTTON_ANGLE_YES, &RotateDlg::OnBnClickedButtonAngleYes)
END_MESSAGE_MAP()


// RotateDlg 消息处理程序


void RotateDlg::OnChangeEditAngle()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
}


void RotateDlg::OnBnClickedCancal()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(IDCANCEL);
}


void RotateDlg::OnBnClickedButtonAngleYes()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	EndDialog(IDOK);
}
