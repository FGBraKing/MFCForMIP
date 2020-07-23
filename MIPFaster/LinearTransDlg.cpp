// LinearTransDlg.cpp: 实现文件
//

#include "pch.h"
#include "MIPFaster.h"
#include "LinearTransDlg.h"
#include "afxdialogex.h"


// LinearTransDlg 对话框

IMPLEMENT_DYNAMIC(LinearTransDlg, CDialogEx)

LinearTransDlg::LinearTransDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LINEAR_PARA, pParent)
	, m_linearPara_min(0)
	, m_linearPara_max(0)
	, m_linearPara_orgin_min(0)
	, m_linearPara_orgin_max(0)
{

}

LinearTransDlg::~LinearTransDlg()
{
}

void LinearTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LINEAR_PARA1, m_linearPara_min);
	DDV_MinMaxByte(pDX, m_linearPara_min, 0, 255);
	DDX_Text(pDX, IDC_EDIT_LINEAR_PARA2, m_linearPara_max);
	DDV_MinMaxByte(pDX, m_linearPara_max, 0, 255);
	DDX_Text(pDX, IDC_EDIT_LINEAR_PARA3, m_linearPara_orgin_min);
	DDV_MinMaxByte(pDX, m_linearPara_orgin_min, 0, 255);
	DDX_Text(pDX, IDC_EDIT_LINEAR_PARA4, m_linearPara_orgin_max);
	DDV_MinMaxByte(pDX, m_linearPara_orgin_max, 0, 255);
}


BEGIN_MESSAGE_MAP(LinearTransDlg, CDialogEx)
	ON_BN_CLICKED(ID_BUTTON_LINEAR_CONFIRM, &LinearTransDlg::OnBnClickedButtonLinearConfirm)
	ON_BN_CLICKED(IDC_BUTTON_LINEAR_CLOSE, &LinearTransDlg::OnBnClickedButtonLinearClose)
	ON_EN_CHANGE(IDC_EDIT_LINEAR_PARA4, &LinearTransDlg::OnEnChangeEditLinearPara4)
END_MESSAGE_MAP()


// LinearTransDlg 消息处理程序


void LinearTransDlg::OnBnClickedButtonLinearConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_linearPara_orgin_max <= m_linearPara_orgin_min)
	{
		m_linearPara_orgin_max = 255;
		m_linearPara_orgin_min = 0;
		AfxMessageBox(L"灰度级差至少为1！");
	}
	if (m_linearPara_max <= m_linearPara_min)
	{
		m_linearPara_max = 255;
		m_linearPara_min = 0;
		AfxMessageBox(L"灰度级差至少为1！");
	}
	UpdateData(FALSE);
	EndDialog(IDOK);
}


void LinearTransDlg::OnBnClickedButtonLinearClose()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(IDCANCEL);
}


void LinearTransDlg::OnEnChangeEditLinearPara4()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
