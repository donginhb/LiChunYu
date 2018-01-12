// ISM7GDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CarKey2.h"
#include "ISM7GDlg.h"
#include "afxdialogex.h"


// CISM7GDlg �Ի���

IMPLEMENT_DYNAMIC(CISM7GDlg, CDialogEx)

CISM7GDlg::CISM7GDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CISM7GDlg::IDD, pParent)
{
	m_bkBrush.CreateSolidBrush(RGB(166,202,240));
}

CISM7GDlg::~CISM7GDlg()
{
	m_bkBrush.DeleteObject();
}

void CISM7GDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CISM7GDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CISM7GDlg ��Ϣ�������


HBRUSH CISM7GDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
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
			pDC->SetBkColor(RGB(166,202,240));
			pDC->SetBkMode(TRANSPARENT);      //����ʾ���ֱ���
			hbr = (HBRUSH)::GetStockObject(NULL_BRUSH); //�༭�򱳾���ע�⣺�����ֱ�������һ����˼��
			break;
		case IDC_RADIO1:
		case IDC_RADIO2:
		case IDC_RADIO3:
		case IDC_RADIO4:
		case IDC_RADIO5:
		case IDC_RADIO6:
			// 			pDC->SetBkColor(RGB(166,202,240));
			// 			pDC->SetBkMode(TRANSPARENT);      //����ʾ���ֱ���
			// 			hbr = (HBRUSH)::GetStockObject(NULL_BRUSH);
			pDC->SetBkMode(TRANSPARENT);
			CRect rc;
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
			CDC* dc = GetDC();
			pDC->BitBlt(0,0,rc.Width(),rc.Height(),dc,rc.left,rc.top,SRCCOPY);
			//�Ѹ����ڱ���ͼƬ�Ȼ�����ť��
			ReleaseDC(dc);
			hbr = (HBRUSH) ::GetStockObject(NULL_BRUSH); 
			break;
		}
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
