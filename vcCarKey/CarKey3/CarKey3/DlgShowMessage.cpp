// DlgShowMessage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CarKey3.h"
#include "DlgShowMessage.h"
#include "afxdialogex.h"


// CDlgShowMessage �Ի���

IMPLEMENT_DYNAMIC(CDlgShowMessage, CDialogEx)

CDlgShowMessage::CDlgShowMessage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgShowMessage::IDD, pParent)
{

}

CDlgShowMessage::~CDlgShowMessage()
{
}

void CDlgShowMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Message);
}


BEGIN_MESSAGE_MAP(CDlgShowMessage, CDialogEx)
END_MESSAGE_MAP()


// CDlgShowMessage ��Ϣ�������


BOOL CDlgShowMessage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_Message.SetWindowText(m_strMessage);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
