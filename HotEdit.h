#if !defined(AFX_HOTEDIT_H__BE9A5170_B912_11D2_B882_0020182B6AB8__INCLUDED_)
#define AFX_HOTEDIT_H__BE9A5170_B912_11D2_B882_0020182B6AB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HotEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHotEdit window

class CHotEdit : public CEdit
{
// Construction
public:
	CHotEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHotEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHotEdit();

	// Generated message map functions
protected:
	virtual void DrawBorder(bool fHot = true);
	COLORREF m_clr3DHilight;
	COLORREF m_clr3DLight;
	COLORREF m_clr3DDkShadow;
	COLORREF m_clr3DShadow;
	COLORREF m_clr3DFace;
	bool m_fTimerSet;
	bool m_fGotFocus;
	static CBrush m_brush;
	static CBrush bela;
   
	//{{AFX_MSG(CHotEdit)
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnSysColorChange();
    BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM Param,
          LRESULT* pLResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOTEDIT_H__BE9A5170_B912_11D2_B882_0020182B6AB8__INCLUDED_)
