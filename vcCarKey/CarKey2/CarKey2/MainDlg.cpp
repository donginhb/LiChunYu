// MainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CarKey2.h"
#include "MainDlg.h"
#include "afxdialogex.h"


// CMainDlg �Ի���

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainDlg::IDD, pParent)
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMainDlg::OnTcnSelchangeTab1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMainDlg ��Ϣ�������
BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_hUsbEventHandle = CCarKey2App::TheHIDDevice.RegisterForUsbEvents(this->GetSafeHwnd());
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString strTemp;

	strTemp.LoadString(IDS_Tab1);
	m_TabCtrl.InsertItem(0,strTemp);  

	strTemp.LoadString(IDS_Tab2);
	m_TabCtrl.InsertItem(1,strTemp);  

	strTemp.LoadString(IDS_Tab3);
	m_TabCtrl.InsertItem(2,strTemp); 
	m_DumpCalcDlg.Create(IDD_DumpCalc_DIALOG,GetDlgItem(IDC_TAB1));  
	m_PortalDlg.Create(IDD_Portal_DIALOG,GetDlgItem(IDC_TAB1));  
	m_ConnectHelpDlg.Create(IDD_ConnectHelpDlg,GetDlgItem(IDC_TAB1));  
	//m_login_tab2.Create(IDD_TAB2_DIALOG,GetDlgItem(IDC_LOGIN_TAB));

	//���IDC_TABTEST�ͻ�����С  
	CRect rs;  
	m_TabCtrl.GetClientRect(&rs);  
	//�����ӶԻ����ڸ������е�λ�ã�����ʵ���޸�  
 	rs.top+=25;  
 	rs.bottom-=1;  
 	rs.left+=1;  
 	rs.right-=1;  

	//�����ӶԻ���ߴ粢�ƶ���ָ��λ��  
	m_DumpCalcDlg.MoveWindow(&rs);  
	m_PortalDlg.MoveWindow(&rs);  
	m_ConnectHelpDlg.MoveWindow(&rs);
	//m_login_tab2.MoveWindow(&rs);  
	//�ֱ��������غ���ʾ  
	m_DumpCalcDlg.ShowWindow(true);  
	m_PortalDlg.ShowWindow(false);
	m_ConnectHelpDlg.ShowWindow(false);
	//m_login_tab2.ShowWindow(false);  

	//����Ĭ�ϵ�ѡ�  
	m_TabCtrl.SetCurSel(0); 
	if(m_PortalOP.bKLine)
		m_PortalDlg.m_raKLine.EnableWindow(TRUE);
	else
		m_PortalDlg.m_raKLine.EnableWindow(FALSE);
	if(m_PortalOP.bIR)
		m_PortalDlg.m_raIR.EnableWindow(TRUE);
	else
		m_PortalDlg.m_raIR.EnableWindow(FALSE);
	m_PortalDlg.m_raIR.SetCheck(TRUE);
	if(m_PortalOP.bCAN)
		m_PortalDlg.m_raCAN.EnableWindow(TRUE);
	else
		m_PortalDlg.m_raCAN.EnableWindow(FALSE);
	m_PortalDlg.m_CANConnect.EnableWindow(FALSE);
	m_PortalDlg.m_raOBD.EnableWindow(FALSE);
	m_PortalDlg.m_raDirect.EnableWindow(FALSE);
	m_PortalDlg.m_raDirect.SetCheck(TRUE);
	m_PortalDlg.m_raWork1.SetWindowText(m_PortalOP.strWork[0]);
	m_PortalDlg.m_raWork1.SetCheck(TRUE);
	if(m_PortalOP.strWork[1]!=_T(""))
	{
		m_PortalDlg.m_raWork2.SetWindowText(m_PortalOP.strWork[1]);
	}		
	else
		m_PortalDlg.m_raWork2.ShowWindow(FALSE);
	if(m_PortalOP.strWork[2]!=_T(""))
	{
		m_PortalDlg.m_raWork3.SetWindowText(m_PortalOP.strWork[2]);
	}		
	else
		m_PortalDlg.m_raWork3.ShowWindow(FALSE);
	m_PortalDlg.m_btReadInfo.EnableWindow(FALSE);
	m_PortalDlg.m_btCANWrite.EnableWindow(FALSE);
	m_PortalDlg.m_pDumpCalcDlg=&m_DumpCalcDlg;
	m_PortalDlg.m_pPortalOP=&m_PortalOP;

	m_PortalDlg.m_btELV.ShowWindow(m_PortalOP.bELV);

	m_DumpCalcDlg.m_PSW.SetWindowText(m_pMainDlg->m_strPSW);

	this->SetWindowText(m_PortalDlg.m_strModelName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CMainDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int CurSel = m_TabCtrl.GetCurSel();  
	switch(CurSel)  
	{  
	case 0:  
		m_DumpCalcDlg.ShowWindow(true);  
		m_PortalDlg.ShowWindow(false);  
		m_ConnectHelpDlg.ShowWindow(false); 
		break;  
	case 1:  
		m_DumpCalcDlg.ShowWindow(false);  
		m_PortalDlg.ShowWindow(true);  
		m_ConnectHelpDlg.ShowWindow(false); 
		break;  
	case 2:
		m_DumpCalcDlg.ShowWindow(false);  
		m_PortalDlg.ShowWindow(false);  
		m_ConnectHelpDlg.ShowWindow(true); 
		break;
	default: ;  
	}
	*pResult = 0;
}


BOOL CMainDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialogEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL CMainDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (message == 0x0219)
	{
		switch (wParam)
		{
		case 0x8000:
			if (!CCarKey2App::TheHIDDevice.MyDeviceDetected)
			{
				CCarKey2App::TheHIDDevice.FindDevice(); 
			}
			break;
		case 0x8004:
			// 			try
			// 			{
			CCarKey2App::TheHIDDevice.Close();
			// 			}
			// 			catch (CException* e)
			// 			{
			// 				throw _T("USB �Ƴ�����!!");
			// 			}
			//::SendMessage(this->GetSafeHwnd(),WM_CLOSE,0,0);
			break;
		default:
			break;
		}
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}


void CMainDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CCarKey2App::TheHIDDevice.UnregisterForUsbEvents(m_hUsbEventHandle);
	CDialogEx::OnClose();
}
