#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"



class CLkIF2TestVCApp : public CWinApp
{
public:
	CLkIF2TestVCApp();

	public:
	virtual BOOL InitInstance();


	DECLARE_MESSAGE_MAP()
};

extern CLkIF2TestVCApp theApp;