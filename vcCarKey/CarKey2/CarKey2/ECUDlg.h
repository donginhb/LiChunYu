#pragma once
#include "afxwin.h"


// CECUDlg �Ի���

class CECUDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CECUDlg)

public:
	CECUDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CECUDlg();

// �Ի�������
	enum { IDD = IDD_ECU_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CBrush m_bkBrush;
};
