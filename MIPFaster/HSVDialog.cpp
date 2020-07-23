// HSVDialog.cpp: 实现文件
//

#include "pch.h"
#include "MIPFaster.h"
#include "HSVDialog.h"
#include "afxdialogex.h"
#include "MIPFasterDlg.h"
#include "global.h"

// HSVDialog 对话框

IMPLEMENT_DYNAMIC(HSVDialog, CDialogEx)

HSVDialog::HSVDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HSV, pParent)
	, m_editS(0)
	, m_editH(0)
	, m_editV(0)
{

}

HSVDialog::~HSVDialog()
{
}

void HSVDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_H, m_sliderH);
	DDX_Control(pDX, IDC_SLIDER_S, m_sliderS);
	DDX_Control(pDX, IDC_SLIDER_V, m_sliderV);
	DDX_Text(pDX, IDC_EDIT_S, m_editS);
	DDV_MinMaxFloat(pDX, m_editS, -100, 100);
	DDX_Text(pDX, IDC_EDIT_H, m_editH);
	DDV_MinMaxFloat(pDX, m_editH, -360, 360);
	DDX_Text(pDX, IDC_EDIT_V, m_editV);
	DDV_MinMaxFloat(pDX, m_editV, -100, 100);
}


BEGIN_MESSAGE_MAP(HSVDialog, CDialogEx)
	ON_BN_CLICKED(IDC_HSV_CLOSE, &HSVDialog::OnBnClickedHsvClose)
	ON_WM_HSCROLL()
ON_EN_CHANGE(IDC_EDIT_H, &HSVDialog::OnEnChangeEditH)
ON_EN_CHANGE(IDC_EDIT_S, &HSVDialog::OnEnChangeEditS)
ON_EN_CHANGE(IDC_EDIT_V, &HSVDialog::OnEnChangeEditV)
ON_BN_CLICKED(ID_HSV_APPLY, &HSVDialog::OnBnClickedHsvApply)
END_MESSAGE_MAP()


// HSVDialog 消息处理程序


void HSVDialog::OnBnClickedHsvClose()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(IDOK);
}


BOOL HSVDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_sliderH.SetRange(-360, 360); //设置滑动条的调节范围，这里的范围后面将会计算转换为[0,1]
	m_sliderH.SetPageSize(3);  //设置滑动条翻页步长
	m_sliderH.SetLineSize(3);  //设置滑动条按键步长
	m_sliderH.SetPos(0);
	m_sliderS.SetRange(-100, 100);
	m_sliderS.SetPageSize(1);
	m_sliderS.SetLineSize(1);
	m_sliderS.SetPos(0);
	m_sliderV.SetRange(-100, 100);
	m_sliderV.SetPageSize(1);
	m_sliderV.SetLineSize(1);
	m_sliderV.SetPos(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void HSVDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	HSVARRAY m_hsvArray;
	m_editH = m_sliderH.GetPos();
	m_editS = m_sliderS.GetPos();
	m_editV = m_sliderV.GetPos();
	UpdateData(FALSE);
	m_hsvArray.h = m_editH;
	m_hsvArray.s = m_editS;
	m_hsvArray.v = m_editV;
	HWND hwnd = GetParent()->m_hWnd;
	::SendMessage(hwnd, WM_HSV_UPDATE, 0, (LPARAM)(&m_hsvArray));
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void HSVDialog::OnEnChangeEditH()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_sliderH.SetPos(m_editH);
}


void HSVDialog::OnEnChangeEditS()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_sliderS.SetPos(m_editS);
}


void HSVDialog::OnEnChangeEditV()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_sliderV.SetPos(m_editV);
}


void HSVDialog::OnBnClickedHsvApply()
{
	HSVARRAY m_hsvArray;
	// TODO: 在此添加控件通知处理程序代码
	m_hsvArray.h = m_editH;
	m_hsvArray.s = m_editS;
	m_hsvArray.v = m_editV;
	GetParent()->SendMessage(WM_HSV_UPDATE,0,(LPARAM)(&m_hsvArray));
}
