#pragma once
#include "afxcmn.h"


// CUpdateSoft �Ի���

class CUpdateSoft : public CDialogEx
{
	DECLARE_DYNAMIC(CUpdateSoft)

public:
	CUpdateSoft(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUpdateSoft();

// �Ի�������
	enum { IDD = IDD_DLG_UpdateSoft };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	int m_nUpType;
	bool m_bOK;
	bool m_bClose;
	virtual BOOL OnInitDialog();
	CProgressCtrl m_ProgressCtrl;
};
