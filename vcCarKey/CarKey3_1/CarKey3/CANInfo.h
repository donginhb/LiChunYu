#pragma once


// CCANInfo �Ի���

class CCANInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CCANInfo)

public:
	CCANInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCANInfo();

// �Ի�������
	enum { IDD = IDD_Dlg_CANInfo };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��	
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	byte* pBuf;
	afx_msg void OnBnClickedButton1();
};
