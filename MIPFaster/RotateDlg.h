#pragma once


// RotateDlg 对话框

class RotateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RotateDlg)

public:
	RotateDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RotateDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ROTATE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_angle;
	afx_msg void OnChangeEditAngle();
	afx_msg void OnBnClickedCancal();
	afx_msg void OnBnClickedButtonAngleYes();
};
