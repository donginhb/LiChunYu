// CANInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CarKey3.h"
#include "CANInfo.h"
#include "afxdialogex.h"
#include "..\..\CarKeyCommon\CommFunc.h"


// CCANInfo �Ի���

IMPLEMENT_DYNAMIC(CCANInfo, CDialogEx)

CCANInfo::CCANInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCANInfo::IDD, pParent)
{

}
CCANInfo::~CCANInfo()
{
	
}

void CCANInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCANInfo, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CCANInfo::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCANInfo ��Ϣ�������


BOOL CCANInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString strTemp;	
	CString strInfo=CCarKey3App::TheHIDDevice.GetCANInfoStr(pBuf);
	strTemp.LoadString(IDS_FinishCANInfo);
	//MessageBox(strInfo,strTemp);
	this->SetWindowText(strTemp);
	SetDlgItemText(IDC_STATIC1,strInfo); 
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CCANInfo::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog fileDlgS(FALSE);
	CString strTemp;
	CString str;
	char ljh[11];
	for (int i = 0; i < 10; i++)
		ljh[i] = pBuf[0x4 + i];
	ljh[10] = 0;
	str=ljh;
	strTemp.LoadString(IDS_SaveInfoTitle);
	fileDlgS.m_ofn.lpstrTitle=strTemp;

	fileDlgS.m_ofn.lpstrFilter=_T("Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0");
	fileDlgS.m_ofn.lpstrDefExt=_T("txt");

	CString strFileName=str+_T("Info");//+CCommFunc::byteToHexStr(CCarKey2App::TheHIDDevice.HexID,4,_T(""))+_T("��־_");

	fileDlgS.m_ofn.lpstrFile=strFileName.GetBuffer(MAX_PATH);
	fileDlgS.m_ofn.nMaxFile = MAX_PATH;
	if (IDOK==fileDlgS.DoModal())
	{
		CFile file(fileDlgS.GetPathName(),CFile::modeCreate|CFile::modeWrite);
		int len;
		GetDlgItemText(IDC_STATIC1,str); 
		char* pLog=CCommFunc::CStringToChar(str,&len);
		file.Write(pLog,len*sizeof(CHAR));
		file.Close();
		delete[] pLog;
	}
	strFileName.ReleaseBuffer();
}
