#pragma once


// CISM7GDlg �Ի���

class CISM7GDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CISM7GDlg)

public:
	CISM7GDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CISM7GDlg();
	CBrush m_bkBrush;
// �Ի�������
	enum { IDD = IDD_ISM7G_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
