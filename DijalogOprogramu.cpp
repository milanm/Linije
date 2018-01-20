// DijalogOprogramu.cpp : implementation file
//

#include "stdafx.h"
#include "Linije.h"
#include "DijalogOprogramu.h"
#include ".\dijalogoprogramu.h"


// CDijalogOprogramu dialog

IMPLEMENT_DYNAMIC(CDijalogOprogramu, CDialog)
CDijalogOprogramu::CDijalogOprogramu(CWnd* pParent /*=NULL*/)
	: CDialog(CDijalogOprogramu::IDD, pParent)
{
}

CDijalogOprogramu::~CDijalogOprogramu()
{
}

void CDijalogOprogramu::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDijalogOprogramu, CDialog)
	ON_WM_SETCURSOR()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_DUGMIC1, OnIzlaz)
	ON_BN_CLICKED(IDC_X, OnX)
END_MESSAGE_MAP()


// CDijalogOprogramu message handlers

BOOL CDijalogOprogramu::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_bitmap.LoadBitmap(ID_OIGRI); 

	m_izlaz.LoadBitmaps(_T("ID_IZLAZ"),_T("ID_IZLAZ_KLIK"),_T("ID_IZLAZ"),_T("ID_IZLAZ"));
	m_izlaz.SubclassDlgItem(IDC_DUGMIC1,this); m_izlaz.SizeToContent();

	m_x.LoadBitmaps(_T("ID_XMALO"),_T("ID_XMALO_KLIK"),_T("ID_XMALO"),_T("ID_XMALO"));
	m_x.SubclassDlgItem(IDC_X,this); m_x.SizeToContent();
	
	CRect rc; GetWindowRect(&rc); rc -= rc.TopLeft();
	m_rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 20, 20);
	SetWindowRgn(m_rgn,TRUE);

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDijalogOprogramu::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor( AfxGetApp()->LoadCursor( IDC_CURSOR2 ) );

	return true;
}

UINT CDijalogOprogramu::OnNcHitTest(CPoint point)
{
	CPoint ptClient = point; ScreenToClient( &ptClient );
	CRect rClient; GetClientRect( &rClient ); rClient.bottom -= 229;

	if( rClient.PtInRect( ptClient ) ) { return HTCAPTION; }

	return HTCLIENT;
}

BOOL CDijalogOprogramu::OnEraseBkgnd(CDC* pDC)
{
	CDialog::OnEraseBkgnd(pDC);	
	if(!m_bitmap.m_hObject)	return true;
	
	CRect rect;
	GetClientRect(&rect);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dc.SelectObject(&m_bitmap);
	int bmw, bmh;
	BITMAP bmap;
	m_bitmap.GetBitmap(&bmap);
	bmw = bmap.bmWidth;
	bmh = bmap.bmHeight;
	int xo=0, yo=0;

	if(bmw < rect.Width()) xo = (rect.Width() - bmw)/2;
		else xo=0;
	
	if(bmh < rect.Height())	yo = (rect.Height()-bmh)/2;
		else yo=0;
	
	pDC->BitBlt (xo, yo, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);
		
	dc.SelectObject(pOldBitmap);

	return true;
}

void CDijalogOprogramu::OnIzlaz()
{
	OnOK();
}

void CDijalogOprogramu::OnX()
{
	OnOK();
}
