#pragma once
#include "afxwin.h"


// CDlgShowMessage �Ի���

class CDlgShowMessage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgShowMessage)

public:
	CDlgShowMessage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgShowMessage();

// �Ի�������
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Message;
	CString m_strMessage;
	virtual BOOL OnInitDialog();
};
