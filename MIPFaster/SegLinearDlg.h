#pragma once


// SegLinearDlg 对话框

class SegLinearDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SegLinearDlg)

public:
	SegLinearDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SegLinearDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SEGLINEAR_PARA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BYTE m_gray_x1;
	BYTE m_gray_x2;
	BYTE m_gray_y1;
	BYTE m_gray_y2;
	afx_msg void OnBnClickedButtonYes();
	afx_msg void OnBnClickedButtonCancle();
};
