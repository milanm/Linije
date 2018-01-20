// HotEdit.cpp : implementation file
//
// If you need any help with this edit control, email me: alan@benchmarx.co.uk

#include "stdafx.h"
#include "HotEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBrush CHotEdit::m_brush(RGB(245,245,210));
CBrush CHotEdit::bela(RGB(255,255,255));

/////////////////////////////////////////////////////////////////////////////
// CHotEdit

CHotEdit::CHotEdit()
{
	m_clr3DFace = GetSysColor(COLOR_3DFACE);
	m_clr3DLight = GetSysColor(COLOR_3DLIGHT);
	m_clr3DHilight = GetSysColor(COLOR_3DHILIGHT);
	m_clr3DShadow = GetSysColor(COLOR_3DSHADOW);
	m_clr3DDkShadow = GetSysColor(COLOR_3DDKSHADOW);

	m_fGotFocus = false;
	m_fTimerSet = false;
}

CHotEdit::~CHotEdit()
{
}


BEGIN_MESSAGE_MAP(CHotEdit, CEdit)
	//{{AFX_MSG_MAP(CHotEdit)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_NCMOUSEMOVE()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHotEdit message handlers

void CHotEdit::OnPaint() 
{
	// call the default message handler to repaint the text etc.
	Default();

	// now redraw the border
	if (m_fGotFocus) {
		DrawBorder();
	} else {
		DrawBorder(false);
	}
}

void CHotEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);

	// set internal flag and redraw border
	m_fGotFocus = true;
	RedrawWindow();

	DrawBorder();
}

void CHotEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);

	// set internal flag and redraw border
	m_fGotFocus = false;
	RedrawWindow();
	DrawBorder(false);
}

void CHotEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	// no point setting the timer if we already have focus, the border will be drawn all the time if
	// we have focus
	if (!m_fGotFocus) {
		// if the timer isn't already set, set it.
		if (!m_fTimerSet) {
			DrawBorder();
			SetTimer(1, 10, NULL);
			m_fTimerSet = true;
		}
	}

	CEdit::OnMouseMove(nFlags, point);
}

// The timer is used to periodically check whether the mouse cursor is within
// this control. Once it isn't, the timer is killed
void CHotEdit::OnTimer(UINT nIDEvent) 
{
	POINT pt;
	GetCursorPos(&pt);
	CRect rcItem;
	GetWindowRect(&rcItem);

	// if the mouse cursor within the control?
	if(!rcItem.PtInRect(pt)) {
		KillTimer(1);

		m_fTimerSet = false;

		if (!m_fGotFocus) {
			DrawBorder(false);
		}
		return;
	}

	CEdit::OnTimer(nIDEvent);
}

// Scroll bars are actually in the non-client area of the window
// so we want to check for movement here too
void CHotEdit::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	if (!m_fTimerSet) {
		DrawBorder();
		SetTimer(1, 10, NULL);
		m_fTimerSet = true;
	}
	
	CEdit::OnNcMouseMove(nHitTest, point);
}

// When the user has changed the system colours we want to
// update the member variables accordingly
void CHotEdit::OnSysColorChange() 
{
	CEdit::OnSysColorChange();
	
	m_clr3DFace = GetSysColor(COLOR_3DFACE);
	m_clr3DLight = GetSysColor(COLOR_3DLIGHT);
	m_clr3DHilight = GetSysColor(COLOR_3DHILIGHT);
	m_clr3DShadow = GetSysColor(COLOR_3DSHADOW);
	m_clr3DDkShadow = GetSysColor(COLOR_3DDKSHADOW);
}

// This is the guts of this control. It draws the border of the control
// as flat when the control is not active. When the control becomes active
// the border is drawn according to the styles applied to it
// If the control is disabled, the border is drawn irrespective of whether
// the control is hot or not.
void CHotEdit::DrawBorder(bool fHot)
{
	CRect rcItem;
	DWORD dwExStyle = GetExStyle();
	CDC* pDC = GetDC();
	COLORREF clrBlack;
	int nBorderWidth = 0;
	int nLoop;

	GetWindowRect(&rcItem);
	ScreenToClient(&rcItem);

	clrBlack = RGB(0, 0, 0);

	if (!IsWindowEnabled()) {
		fHot = true;
	}

	if (dwExStyle & WS_EX_DLGMODALFRAME) {
		nBorderWidth += 3;
	}
	if (dwExStyle & WS_EX_CLIENTEDGE) {
		nBorderWidth += 2;
	}
	if (dwExStyle & WS_EX_STATICEDGE && !(dwExStyle & WS_EX_DLGMODALFRAME)) {
		nBorderWidth ++;
	}

	if(IsWindowEnabled())
	{
		
	CRect rect = rcItem;
	rect.top = rect.top + 1;
	rect.left = rect.left + 1;
	rect.bottom = rect.bottom - 1;
	rect.right = rect.right - 1;
	pDC->Draw3dRect(rect,  m_clr3DHilight,  m_clr3DHilight);

	}

	// blank the border
	for (nLoop = 0; nLoop < nBorderWidth; nLoop++) {
		pDC->Draw3dRect(rcItem, m_clr3DFace, m_clr3DFace);
		rcItem.DeflateRect(1, 1);
	}
	rcItem.InflateRect(1, 1);

	if (fHot) {

		if (dwExStyle & WS_EX_CLIENTEDGE) {
			pDC->Draw3dRect(rcItem, m_clr3DDkShadow, m_clr3DLight);
			rcItem.InflateRect(1, 1);
			pDC->Draw3dRect(rcItem, m_clr3DShadow, m_clr3DHilight);
			rcItem.InflateRect(1, 1);
		}

		if (dwExStyle & WS_EX_STATICEDGE && !(dwExStyle & WS_EX_DLGMODALFRAME)) {
			pDC->Draw3dRect(rcItem, m_clr3DShadow, m_clr3DHilight);
			rcItem.InflateRect(1, 1);
		}

		if (dwExStyle & WS_EX_DLGMODALFRAME) {
			pDC->Draw3dRect(rcItem, m_clr3DFace, m_clr3DFace);
			rcItem.InflateRect(1, 1);
			pDC->Draw3dRect(rcItem, m_clr3DHilight, m_clr3DShadow);
			rcItem.InflateRect(1, 1);
			pDC->Draw3dRect(rcItem, m_clr3DLight, m_clr3DDkShadow);
		}
	}
	else
	{
		CRect rect = rcItem;
		rect.top = rect.top;
		rect.left = rect.left;
		rect.bottom = rect.bottom;
		rect.right = rect.right;
		pDC->Draw3dRect(rect,  m_clr3DShadow,  m_clr3DShadow);
	}

	ReleaseDC(pDC);
}

BOOL CHotEdit::OnChildNotify(UINT message, WPARAM wParam,
                               LPARAM lParam, LRESULT* pLResult)
   {
   // If "message" is not the message you're after, do default processing:

   // For 16-bit applications change WM_CTLCOLOREDIT to WM_CTLCOLOR
      if (message != WM_CTLCOLOREDIT)
      {
         return CEdit::OnChildNotify(message,wParam,lParam,pLResult);
      }

	
	  HDC hdcChild = (HDC)wParam;
	  if(m_fGotFocus)
	  {

      SetTextColor(hdcChild, RGB(0,0,255));

      SetBkColor(hdcChild, RGB(245,245,210));

   // Send what would have been the return value of OnCtlColor() - the
   // brush handle - back in pLResult:
      *pLResult = (LRESULT)(m_brush.GetSafeHandle());
	  }
	  else
		 *pLResult = (LRESULT)(bela.GetSafeHandle());
	
   // Return TRUE to indicate that the message was handled:
      return TRUE;
   }


