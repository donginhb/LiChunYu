#pragma once
#include "afxwin.h"


// CInputPSW �Ի���

class CInputPSW : public CDialogEx
{
	DECLARE_DYNAMIC(CInputPSW)

public:
	virtual BOOL OnInitDialog();
	CInputPSW(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputPSW();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	byte PSW[8];
	afx_msg void OnBnClickedButton1();
	CEdit m_PSW;
	bool bPSW;
};
