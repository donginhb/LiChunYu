#pragma once
#include "afxwin.h"


// CConnectHelpDlg �Ի���

class CConnectHelpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConnectHelpDlg)

public:
	CConnectHelpDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConnectHelpDlg();

// �Ի�������
	enum { IDD = IDD_ConnectHelpDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CBrush m_bkBrush;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	CStatic m_Pic;
	CButton m_DCL;
};
