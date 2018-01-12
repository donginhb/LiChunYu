// ELVDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CarKey2.h"
#include "ELVDlg.h"
#include "afxdialogex.h"


// CELVDlg �Ի���

IMPLEMENT_DYNAMIC(CELVDlg, CDialogEx)

CELVDlg::CELVDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CELVDlg::IDD, pParent)
{
	m_bkBrush.CreateSolidBrush(RGB(166,202,240));
}

CELVDlg::~CELVDlg()
{
	m_bkBrush.DeleteObject();
}

void CELVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CELVDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CELVDlg ��Ϣ�������


HBRUSH CELVDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if(nCtlColor ==CTLCOLOR_DLG)
		return m_bkBrush; //������ɫˢ��
	if (nCtlColor = CTLCOLOR_STATIC)
	{
		switch(pWnd->GetDlgCtrlID()) 
		{
		case IDC_STATIC1:
		case IDC_STATIC2:
		case IDC_STATIC3:
		case IDC_STATIC4:
		case IDC_STATIC5:
		case IDC_STATIC6:
		case IDC_STATIC7:
		case IDC_STATIC8:
		case IDC_STATIC9:
		case IDC_STATIC10:
		case IDC_STATIC11:
		case IDC_STATIC12:
		case IDC_STATIC13:
			pDC->SetBkColor(RGB(166,202,240));
			pDC->SetBkMode(TRANSPARENT);      //����ʾ���ֱ���
			hbr = (HBRUSH)::GetStockObject(NULL_BRUSH); //�༭�򱳾���ע�⣺�����ֱ�������һ����˼��
			break;
		}
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
