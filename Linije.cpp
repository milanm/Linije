// Linije.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Linije.h"
#include "LinijeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLinijeApp

BEGIN_MESSAGE_MAP(CLinijeApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLinijeApp construction

CLinijeApp::CLinijeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	 EnableHtmlHelp();
}


// The one and only CLinijeApp object

CLinijeApp theApp;


// CLinijeApp initialization

BOOL CLinijeApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();
	
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	CWinApp::InitInstance();

	SetRegistryKey(_T("Abell Soft"));

	// ZA SADA SAMO WINXP!!!
	OSVERSIONINFO           osvi;
    // Initialize the OSVERSIONINFO structure.
    ZeroMemory( &osvi, sizeof( osvi ) );
    osvi.dwOSVersionInfoSize = sizeof( osvi );
        
    GetVersionEx( &osvi );  // Assume this function succeeds.

	if((osvi.dwMajorVersion != 5) || (osvi.dwMinorVersion != 1) || (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT))
	{ 
		AfxMessageBox("Vas racunar ne radi na Windows-u XP"); PostQuitMessage(0);
	}else
	{
	// create and load the frame with its resources
		if(FindWindow(NULL,"Linije 1.0")==NULL)
		{
			CLinijeDlg dlg;
			m_pMainWnd = &dlg;
			INT_PTR nResponse = dlg.DoModal();
		}
		else
		{
			AfxMessageBox("Linije su vec pokrenute!",MB_OK|MB_ICONINFORMATION);
			PostQuitMessage(0);
		}
	}
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
