#pragma once

// HSVDialog 对话框

class HSVDialog : public CDialogEx
{
	DECLARE_DYNAMIC(HSVDialog)

public:
	HSVDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~HSVDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HSV };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_sliderH;
	CSliderCtrl m_sliderS;
	CSliderCtrl m_sliderV;
	float m_editS;
	float m_editH;
	float m_editV;
	afx_msg void OnBnClickedHsvClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	//HSVARRAY m_hsvArray;

public:
	afx_msg void OnEnChangeEditH();
	afx_msg void OnEnChangeEditS();
	afx_msg void OnEnChangeEditV();
	afx_msg void OnBnClickedHsvApply();
};
