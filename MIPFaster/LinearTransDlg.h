#pragma once


// LinearTransDlg 对话框

class LinearTransDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LinearTransDlg)

public:
	LinearTransDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~LinearTransDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LINEAR_PARA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BYTE m_linearPara_min;
	BYTE m_linearPara_max;
	afx_msg void OnBnClickedButtonLinearConfirm();
	afx_msg void OnBnClickedButtonLinearClose();
	BYTE m_linearPara_orgin_min;
	BYTE m_linearPara_orgin_max;
	afx_msg void OnEnChangeEditLinearPara4();
};
