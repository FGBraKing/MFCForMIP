#pragma once
#include "MyDIB.h"

// ImageOperationDlg 对话框

class ImageOperationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ImageOperationDlg)

public:
	ImageOperationDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ImageOperationDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_OPERATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	MyDIB* m_image;
	float m_alpha;
	float m_beta;
	afx_msg void OnBnClickedButtonOperaClose();
	afx_msg void OnBnClickedButtonOperaOpen();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();

	afx_msg void OnChangeEditAlpha();
	afx_msg void OnChangeEditBeta();
	//virtual void OnCancel();
	//virtual void PostNcDestroy();
};
