// UpdateSoft.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "UpdateSoft.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;
using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
 	if(argc!=3)
 	{
 		return 0;
 	}
	HMODULE hModule = ::GetModuleHandle(NULL);	
	if (hModule != NULL)
	{
		// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: ���Ĵ�������Է���������Ҫ
			_tprintf(_T("err: -1\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
		}
	}
	else
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("err: -2\n"));
		nRetCode = 1;
	}
	//FindProcess(_T("Mercedes Tool.exe"));
	//_tprintf(argv[1]);
	//_tprintf(_T("\n"));
//	getchar();
	if(GetFileAttributes(_T("Mercedes Tool.exe.up"))!=0xFFFFFFFF)
	{
// 		HANDLE hObject = ::CreateMutex(NULL,FALSE, _T("wflcyADTech20160622")); 
// 		if(GetLastError() == ERROR_ALREADY_EXISTS) 
// 		{ 
// 			MessageBox(NULL,_T("0"),_T("0"),MB_OK);
// 		}
// 		else
// 		{
// 			MessageBox(NULL,_T("1"),_T("0"),MB_OK);
// 		}
		CString strExe1,strExe2,strExe;
		strExe1=argv[1];
		strExe2=argv[2];
		strExe=strExe1+_T(" ")+strExe2;
// 		_tprintf(strExe+_T("\n"));
// 		getchar();
 		HWND h = FindWindow( NULL, strExe);
 		if (h>0)
 		{
			ULONG nProcessID;
			::GetWindowThreadProcessId(h, &nProcessID );
			HANDLE hProcessHandle = ::OpenProcess( PROCESS_TERMINATE, FALSE,nProcessID);
			::TerminateProcess( hProcessHandle,4);
			//_tprintf(_T("close soft\n"));
			//getchar();
 			//MessageBox(0,"��������������","��ʾ",0);
			while(true)
			{
				if(DeleteFile(_T("Mercedes Tool.exe")))
					break;
			}		
			//_tprintf(_T("delete soft\n"));
			//getchar();
			rename("Mercedes Tool.exe.up","Mercedes Tool.exe");
			//_tprintf(_T("rename soft soft\n"));
			//getchar();
			WinExec("Mercedes Tool.exe",SW_SHOW);
			//_tprintf(_T("run soft\n"));
 			//MessageBox(NULL,_T("0"),_T("0"),MB_OK);
 		}
		else
 		{
 			//MessageBox(0,"������û������","��ʾ",0);
 			//MessageBox(NULL,_T("1"),_T("0"),MB_OK);
 		}
		
		//MessageBox(NULL,_T("1"),_T("0"),MB_OK);
	}
	else
	{
		//WinExec("Mercedes Tool.exe",SW_SHOW);
	}
	return nRetCode;
}
