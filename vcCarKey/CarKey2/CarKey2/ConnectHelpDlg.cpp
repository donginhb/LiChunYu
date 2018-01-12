// ConnectHelpDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CarKey2.h"
#include "ConnectHelpDlg.h"
#include "afxdialogex.h"


// CConnectHelpDlg �Ի���

IMPLEMENT_DYNAMIC(CConnectHelpDlg, CDialogEx)

CConnectHelpDlg::CConnectHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConnectHelpDlg::IDD, pParent)
{

}

CConnectHelpDlg::~CConnectHelpDlg()
{
	m_bkBrush.DeleteObject();
}

void CConnectHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_Pic);
	DDX_Control(pDX, IDC_RADIO1, m_DCL);
}


BEGIN_MESSAGE_MAP(CConnectHelpDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO1, &CConnectHelpDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CConnectHelpDlg::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CConnectHelpDlg ��Ϣ�������


HBRUSH CConnectHelpDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if(nCtlColor ==CTLCOLOR_DLG)
		return m_bkBrush; //������ɫˢ��
	if (nCtlColor = CTLCOLOR_STATIC)
	{
		switch(pWnd->GetDlgCtrlID()) 
		{		
		case IDC_RADIO1:
		case IDC_RADIO2:		
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


BOOL CConnectHelpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_bkBrush.CreateSolidBrush(RGB(166,202,240));
	m_DCL.SetCheck(true);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CConnectHelpDlg::OnBnClickedRadio1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CBitmap bitmap;  // CBitmap�������ڼ���λͼ   
	HBITMAP hBmp;    // ����CBitmap���ص�λͼ�ľ��   
	bitmap.LoadBitmap(IDB_BITMAP5);
	// ��λͼIDB_BITMAP1���ص�bitmap   
	hBmp = (HBITMAP)bitmap.GetSafeHandle();  // ��ȡbitmap����λͼ�ľ��   
	m_Pic.SetBitmap(hBmp);    // ����ͼƬ�ؼ�m_jzmPicture��λͼͼƬΪIDB_BITMAP1  
}


void CConnectHelpDlg::OnBnClickedRadio2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CBitmap bitmap;  // CBitmap�������ڼ���λͼ   
	HBITMAP hBmp;    // ����CBitmap���ص�λͼ�ľ��   
	bitmap.LoadBitmap(IDB_BITMAP16);
	// ��λͼIDB_BITMAP1���ص�bitmap   
	hBmp = (HBITMAP)bitmap.GetSafeHandle();  // ��ȡbitmap����λͼ�ľ��   
	m_Pic.SetBitmap(hBmp); 
}
