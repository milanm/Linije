#pragma once
#include "afxwin.h"
#include "HotEdit.h"
#include "NewLabel.h"

// CUnosImena dialog

class CUnosImena : public CDialog
{
	DECLARE_DYNAMIC(CUnosImena)
	
	CBitmap m_bitmap;

	CBitmapButton m_izlaz;

	CRgn m_rgn; 
public:
	CUnosImena(CWnd* pParent = NULL);  
	virtual ~CUnosImena();

// Dialog Data
	enum { IDD = IDD_UNOS_IMENA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CHotEdit m_unos;
	CString ime;
	virtual BOOL OnInitDialog();
	afx_msg void OnURedu();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnBnClickedIzlazIme();
};
