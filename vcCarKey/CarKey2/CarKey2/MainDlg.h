#pragma once
#include "afxcmn.h"
#include "DumpCalcDlg.h"
#include "PortalDlg.h"
#include "CarKey2Dlg.h"
#include "ConnectHelpDlg.h"

// CMainDlg �Ի���

class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMainDlg();

// �Ի�������
	enum { IDD = IDD_MAIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CTabCtrl m_TabCtrl;
	CDumpCalcDlg m_DumpCalcDlg;
	CPortalDlg m_PortalDlg;
	CConnectHelpDlg m_ConnectHelpDlg;
	CStatusBar m_wndStatusBar;
	CPortalOP m_PortalOP;
	HANDLE m_hUsbEventHandle;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnClose();
	CCarKey2Dlg* m_pMainDlg;
};
