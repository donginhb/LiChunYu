
// wfHashCalc.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CwfHashCalcApp:
// �йش����ʵ�֣������ wfHashCalc.cpp
//

class CwfHashCalcApp : public CWinApp
{
public:
	CwfHashCalcApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CwfHashCalcApp theApp;