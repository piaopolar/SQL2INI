
// SQL2INI.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSQL2INIApp:
// �йش����ʵ�֣������ SQL2INI.cpp
//

class CSQL2INIApp : public CWinApp
{
public:
	CSQL2INIApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSQL2INIApp theApp;