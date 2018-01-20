// Linije.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

const int build_verzija = 4724;

// CLinijeApp:
// See Linije.cpp for the implementation of this class
//

class CLinijeApp : public CWinApp
{
public:
	CLinijeApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CLinijeApp theApp;