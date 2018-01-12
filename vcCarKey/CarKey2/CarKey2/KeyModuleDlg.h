#pragma once
#include "afxmaskededit.h"
#include ".\..\..\CarKeyCommon\WFMaskedEidt.h"
#include ".\..\..\CarKeyCommon\wfedit.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "CarKey2Dlg.h"

// CKeyModuleDlg �Ի���

class CKeyModuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CKeyModuleDlg)

public:
	CKeyModuleDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CKeyModuleDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_KeyModule };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CWfEdit m_rKeyID;
	CWfEdit m_rKeyNum;
	CWfEdit m_rLife;
	CWfEdit m_rLifeHex;
	CWfEdit m_rStatus;
	CWfEdit m_rVersion;
	CWfEdit m_rMark;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	HANDLE m_hUsbEventHandle;
	afx_msg void OnClose();
	CComboBox m_OptType;
	afx_msg void OnBnReadKeyData();
	CEdit m_Log;
	CString m_strLog;
	CProgressCtrl m_Progress;
	void ShowLog(CString str);
	afx_msg void OnBnOpenFile();
	CWfEdit m_wPWD;
	CWfEdit m_wKeyNum;
	CWfEdit m_wKeyID;
	CBrush m_bkBrush;
	afx_msg void OnBnWrite();
	afx_msg void OnBnRenew();
	afx_msg void OnBnClearLog();
	afx_msg void OnBnSaveLog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnReadPSW();
	CButton m_btReadPSW;
	CButton m_btSavePSW;
	bool m_bGetPSW;
	byte m_PSWBuf[0x18];
	afx_msg void OnBnSavePSW();
	void KeyMessageDlg(CString str);
	CCarKey2Dlg* m_pMainDlg;
};
