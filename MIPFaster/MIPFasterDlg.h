
// MIPFasterDlg.h: 头文件
//

#pragma once
#include "MyDIB.h"
#include "ImageOperationDlg.h"

#define WM_HSV_UPDATE (WM_USER+1000)
// CMIPFasterDlg 对话框
class CMIPFasterDlg : public CDialogEx
{
// 构造
public:
	CMIPFasterDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CMIPFasterDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MIPFASTER_DIALOG };
#endif
	CStatusBar m_dlgStatusBar;
	CButton m_radio3;	//=((CButton *)GetDlgItem(IDC_RADIO_NAME))
	CButton m_radio5;
	CButton m_radio7;
	//滤波器类型
	CComboBox m_filterType;
	//结构元类型
	CComboBox m_structureType;
	//结构元大小
	int m_structureSize;
	// 滤波半径
	int m_fren;
	// 滤波器阶数
	int m_rank;
	// 逆滤波中退化函数的长度
	int m_inv_l;
	//维纳滤波器中参数
	int m_winner_l;
	double m_winner_k;
	// 分割阈值
	BYTE m_threshold;
	// 原图
	CStatic m_imageOrgin;
	CStatic m_imageTrans;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	MyDIB* m_orginImage;
	MyDIB* m_transImage;
	bool m_bTraned;
	bool m_resize;

	ImageOperationDlg* m_operationDlg;
	// 图像缩放控件
	CSliderCtrl m_slider_shrink;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnHsvUpdate(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedOk();
	afx_msg void OnClickedOpen();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonHsv();
	afx_msg void OnBnClickedButtonToGray();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedButtonLineartrans();
	afx_msg void OnBnClickedButtonSegtrans();
	afx_msg void OnBnClickedButtonHistogramEq();
	afx_msg void OnBnClickedButtonHistSpec();
	afx_msg void OnBnClickedButtonLowpassT();

	int getKernelSize();
	afx_msg void OnBnClickedButtonHighpassT();
	afx_msg void OnBnClickedButtonMinFilter();
	afx_msg void OnBnClickedButtonMedFilter();
	afx_msg void OnBnClickedButtonmaxFilter();
	afx_msg void OnBnClickedButtonPseColor();
	afx_msg void OnBnClickedButtonPlus();
	afx_msg void OnBnClickedButtonMinus();
	afx_msg void OnBnClickedButtonMulti();
	afx_msg void OnBnClickedButtonDiv();
	afx_msg void OnBnClickedButtonAnd();
	afx_msg void OnBnClickedButtonOr();
	afx_msg void OnBnClickedButtonXor();
	afx_msg void OnBnClickedButtonNot();
	afx_msg void OnBnClickedButtonFilpx();
	afx_msg void OnBnClickedButtonFilpy();
	afx_msg void OnBnClickedButtonFot90();
	afx_msg void OnBnClickedButtonFot180();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//	afx_msg void OnNMCustomdrawSliderShrink(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonFft();
	afx_msg void OnBnClickedButtonIfft();
	
	afx_msg void OnBnClickedButtonFilterRun();
	afx_msg void OnBnClickedButtonInv();
	afx_msg void OnBnClickedButtonWinner();
	
	afx_msg void OnBnClickedButtonErosion();
	
	afx_msg void OnBnClickedButtonDialation();
	afx_msg void OnBnClickedButtonOpening();
	afx_msg void OnBnClickedButtonClosing();
	afx_msg void OnBnClickedButtonSkeleton();
	
	afx_msg void OnBnClickedButtonMansrg();
	afx_msg void OnBnClickedButtonOstseg();
	afx_msg void OnBnClickedButtonGrow();
	afx_msg void OnBnClickedButtonMerge();
};
