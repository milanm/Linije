#pragma once
#include "afxwin.h"


// CProveriInternet dialog

class CProveriInternet : public CDialog
{
	DECLARE_DYNAMIC(CProveriInternet)

public:
	CProveriInternet(CWnd* pParent = NULL);  
	virtual ~CProveriInternet();

// Dialog Data
	enum { IDD = IDD_INTERNET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnProveri();
	CStatic m_verzija;
	virtual BOOL OnInitDialog();
};
