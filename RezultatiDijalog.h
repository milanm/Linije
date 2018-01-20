#pragma once
#include "afxcmn.h"
#include "NewLabel.h"

// CRezultatiDijalog dialog

class CRezultatiDijalog : public CDialog
{
	DECLARE_DYNAMIC(CRezultatiDijalog)

	CBitmap m_bitmap;

	CBitmapButton m_x;
	CBitmapButton m_izlaz;

	CFont font;

	CRgn m_rgn; 

public:
	CRezultatiDijalog(CWnd* pParent = NULL);  
	virtual ~CRezultatiDijalog();

// Dialog Data
	enum { IDD = IDD_REZULTATI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDugme();
public:
	CListCtrl m_rezultati;
	CNewLabel naslov;
	int rezultat;
	bool unos;
	CString ime;
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnX();
	afx_msg void OnIzlazRez();
};
