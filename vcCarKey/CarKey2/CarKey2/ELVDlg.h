#pragma once


// CELVDlg �Ի���

class CELVDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CELVDlg)

public:
	CELVDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CELVDlg();
	CBrush m_bkBrush;

// �Ի�������
	enum { IDD = IDD_ELV_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
