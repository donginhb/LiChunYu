
// CarKey3Dlg.h : ͷ�ļ�
//

#pragma once
#include "afxext.h"


// CCarKey3Dlg �Ի���
class CCarKey3Dlg : public CDialogEx
{
// ����
public:
	CCarKey3Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CARKEY3_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	void HIDUI();
	void ReHIDUI();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:	
	CString m_strPSW;
	CStatusBarCtrl m_StatusBar;
	HANDLE m_hUsbEventHandle;
	CBitmapButton m_btnMKEY;
	CBitmapButton m_btnEIS;
	CBitmapButton m_btnESL;
	CBitmapButton m_btnNEC_CHIP;
	CBitmapButton m_btnNEC_KEYLESS;
	CBitmapButton m_btnNEC_ESL;
	bool m_bShow;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnPSW();
	afx_msg void OnLanguage32775();
	afx_msg void OnLanguageEnglish();
	void LoadString();
	afx_msg void OnBnEIS();
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnBnKey();
	afx_msg void OnBnESL();
	
	afx_msg void OnBnECU();
	afx_msg void OnBnIG();
};