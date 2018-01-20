#pragma once

// CDijalogOprogramu dialog

class CDijalogOprogramu : public CDialog
{
	DECLARE_DYNAMIC(CDijalogOprogramu)

public:
	CDijalogOprogramu(CWnd* pParent = NULL);  
	virtual ~CDijalogOprogramu();

	CBitmap m_bitmap;

	CBitmapButton m_x;
	CBitmapButton m_izlaz;

	CRgn m_rgn; 
	
	enum { IDD = IDD_OPROGRAMU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnIzlaz();
	afx_msg void OnX();
};
