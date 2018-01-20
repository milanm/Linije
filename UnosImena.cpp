// UnosImena.cpp : implementation file
//

#include "stdafx.h"
#include "Linije.h"
#include "UnosImena.h"
#include ".\unosimena.h"


// CUnosImena dialog

IMPLEMENT_DYNAMIC(CUnosImena, CDialog)
CUnosImena::CUnosImena(CWnd* pParent /*=NULL*/)
	: CDialog(CUnosImena::IDD, pParent)
{
}

CUnosImena::~CUnosImena()
{
}

void CUnosImena::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_unos);
}


BEGIN_MESSAGE_MAP(CUnosImena, CDialog)
	ON_BN_CLICKED(IDOK, OnURedu)
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(ID_IZLAZ_IME, OnBnClickedIzlazIme)
END_MESSAGE_MAP()


// CUnosImena message handlers

BOOL CUnosImena::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bitmap.LoadBitmap(ID_UNESI);

	m_izlaz.LoadBitmaps(_T("ID_IZLAZ"),_T("ID_IZLAZ_KLIK"),_T("ID_IZLAZ"),_T("ID_IZLAZ"));
	m_izlaz.SubclassDlgItem(ID_IZLAZ_IME,this); m_izlaz.SizeToContent();

	CRect rc; GetWindowRect(&rc); rc -= rc.TopLeft(); 
	m_rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 10, 10);
	SetWindowRgn(m_rgn,TRUE);

	m_unos.SetFocus();

	return FALSE;
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUnosImena::OnURedu()
{
	UpdateData();
	m_unos.GetWindowText(ime);
	OnOK();
}

BOOL CUnosImena::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor( AfxGetApp()->LoadCursor( IDC_CURSOR2 ) );

	return true;
}

BOOL CUnosImena::OnEraseBkgnd(CDC* pDC)
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

UINT CUnosImena::OnNcHitTest(CPoint point)
{
	CPoint ptClient = point; ScreenToClient( &ptClient );
	CRect rClient; GetClientRect( &rClient ); rClient.bottom -= 81;

	if( rClient.PtInRect( ptClient ) ) { return HTCAPTION; }

	return HTCLIENT;
}

void CUnosImena::OnBnClickedIzlazIme()
{
	OnURedu();
}
