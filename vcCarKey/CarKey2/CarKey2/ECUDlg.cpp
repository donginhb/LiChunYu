// ECUDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CarKey2.h"
#include "ECUDlg.h"
#include "afxdialogex.h"


// CECUDlg �Ի���

IMPLEMENT_DYNAMIC(CECUDlg, CDialogEx)

CECUDlg::CECUDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CECUDlg::IDD, pParent)
{
	m_bkBrush.CreateSolidBrush(RGB(166,202,240));
}

CECUDlg::~CECUDlg()
{
	m_bkBrush.DeleteObject();
}

void CECUDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CECUDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CECUDlg ��Ϣ�������


HBRUSH CECUDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
			pDC->SetBkColor(RGB(166,202,240));
			pDC->SetBkMode(TRANSPARENT);      //����ʾ���ֱ���
			hbr = (HBRUSH)::GetStockObject(NULL_BRUSH); //�༭�򱳾���ע�⣺�����ֱ�������һ����˼��
			break;
		}
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
