// RezultatiDijalog.cpp : implementation file
//

#include "stdafx.h"
#include "Linije.h"
#include "RezultatiDijalog.h"
#include ".\rezultatidijalog.h"
#include "Scores.h"
#include "UnosImena.h"

static char* rezultatiFajl = { "linije.rez" } ;
// CRezultatiDijalog dialog

IMPLEMENT_DYNAMIC(CRezultatiDijalog, CDialog)
CRezultatiDijalog::CRezultatiDijalog(CWnd* pParent /*=NULL*/)
	: CDialog(CRezultatiDijalog::IDD, pParent)
{
}

CRezultatiDijalog::~CRezultatiDijalog()
{
}

void CRezultatiDijalog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_rezultati);
}


BEGIN_MESSAGE_MAP(CRezultatiDijalog, CDialog)
	ON_BN_CLICKED(ID_DUGME,OnDugme)
	ON_WM_SETCURSOR()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_X, OnX)
	ON_BN_CLICKED(ID_IZLAZ_REZ, OnIzlazRez)
END_MESSAGE_MAP()


// CRezultatiDijalog message handlers

BOOL CRezultatiDijalog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CFile f; CFileException e; CScores scores; 
	if(f.Open( rezultatiFajl, CFile::modeRead , &e ) )
	{
		CArchive ar( &f, CArchive::load, 4096, NULL);	
		ar >> scores; ar.Close(); f.Close();
	}
	else
	{
		f.Open( rezultatiFajl,  CFile::modeCreate | CFile::modeWrite,  &e );
		CArchive ar( &f, CArchive::store, 4096, NULL);	
		CScores scores; ar << scores; ar.Close(); f.Close();
	}
	
	m_rezultati.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);
	InitializeFlatSB(m_rezultati.m_hWnd);
	FlatSB_SetScrollProp(m_rezultati.m_hWnd,WSB_PROP_CXVSCROLL,12,TRUE);
	FlatSB_SetScrollProp(m_rezultati.m_hWnd,WSB_PROP_CYHSCROLL,12,TRUE);
	FlatSB_SetScrollProp(m_rezultati.m_hWnd,WSB_PROP_HSTYLE,FSB_ENCARTA_MODE,TRUE);

	m_bitmap.LoadBitmap(ID_REZULTATI); 
	
	LV_COLUMN kol;
	kol.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	kol.fmt = LVCFMT_LEFT;

	kol.cx = 50;
	kol.iSubItem = 0;
	kol.pszText = "R.broj";
	m_rezultati.InsertColumn(0,&kol);

	kol.cx = 83;
	kol.iSubItem = 1;
	kol.pszText = "Ime";
	m_rezultati.InsertColumn(1,&kol);

	kol.cx = 58;
	kol.iSubItem = 2;
	kol.pszText = "Rezultat";
	m_rezultati.InsertColumn(2,&kol);

	for(int i = 0; i < 10; i++) 
	{
		CString temp; temp.Format("%d",i+1);
		m_rezultati.InsertItem(i,temp);
		m_rezultati.SetItemText(i,1,scores.names[i]);
		temp.Format("%d",scores.scores[i]);
		m_rezultati.SetItemText(i,2,temp);
	}

	m_rezultati.SetBkColor(RGB(195,204,235));
	m_rezultati.SetTextBkColor(RGB(195,204,235));
	m_rezultati.SetHotCursor(AfxGetApp()->LoadCursor( IDC_CURSOR2 ));

	m_izlaz.LoadBitmaps(_T("ID_IZLAZ"),_T("ID_IZLAZ_KLIK"),_T("ID_IZLAZ"),_T("ID_IZLAZ"));
	m_izlaz.SubclassDlgItem(ID_IZLAZ_REZ,this); m_izlaz.SizeToContent();

	m_x.LoadBitmaps(_T("ID_XMALO"),_T("ID_XMALO_KLIK"),_T("ID_XMALO"),_T("ID_XMALO"));
	m_x.SubclassDlgItem(IDC_X,this); m_x.SizeToContent();
	
	CRect rc; GetWindowRect(&rc); rc -= rc.TopLeft();
	m_rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 20, 20);
	SetWindowRgn(m_rgn,TRUE);

	CenterWindow();

	if(unos) 
	{
		CUnosImena unos; unos.DoModal(); ime = unos.ime; 
		if(ime == CString("")) ime = "Prazno"; OnDugme();
	}

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));    
	lf.lfHeight = 12;                    
	strcpy(lf.lfFaceName, "Tahoma");        
	lf.lfWeight = FW_BOLD;
	font.CreateFontIndirect(&lf);

	m_rezultati.GetHeaderCtrl()->SetFont(&font);
	m_rezultati.GetHeaderCtrl()->EnableWindow(FALSE);

	m_rezultati.SetFocus(); UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRezultatiDijalog::OnDugme() 
{
	UpdateData();
	CFile f;
	CFileException e; int sel = 0;
	if(f.Open( rezultatiFajl, CFile::modeRead , &e ) )
	{
		CArchive ar( &f, CArchive::load, 4096, NULL);	
		CScores scores; ar >> scores;
		
		for(int i=0;i<10;i++)
			if(rezultat>scores.scores[i])
			{
				sel = i;
				int stara = scores.scores[i];
				int stara1 = 0;
				CString staro_ime = scores.names[i];
				CString staro_ime1;
				scores.scores[i]=rezultat;
				scores.names[i]=ime;
				for(int j=i+1;j<10;j++) {
					stara1 = scores.scores[j];
					staro_ime1 = scores.names[j];
					scores.scores[j] = stara;
					scores.names[j] = staro_ime;
					stara = stara1;
					staro_ime = staro_ime1;
				}
				break;
			}
		ar.Close();
		f.Close();

		f.Open( rezultatiFajl, CFile::modeWrite|CFile::modeCreate , &e );
		CArchive ar1( &f, CArchive::store, 512, NULL);	
		ar1 << scores;
		ar1.Close();
		f.Close();
		
		m_rezultati.DeleteAllItems();
		for(i=0;i<10;i++) 
		{
			CString temp; temp.Format("%d",i+1);
			m_rezultati.InsertItem(i,temp);
			m_rezultati.SetItemText(i,1,scores.names[i]);
			temp.Format("%d",scores.scores[i]);
			m_rezultati.SetItemText(i,2,temp);
		}	
		// pokusavam da selektujem polje koje je upisanu u listu
		m_rezultati.SetSelectionMark(sel);
		m_rezultati.SetHotItem(sel);
		m_rezultati.SetCheck(sel, TRUE);
		m_rezultati.SetItemState(sel, LVIS_SELECTED, LVIS_SELECTED);
	}
	UpdateData(FALSE);
}

BOOL CRezultatiDijalog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor( AfxGetApp()->LoadCursor( IDC_CURSOR2 ) );

	return true;
}

UINT CRezultatiDijalog::OnNcHitTest(CPoint point)
{
	CPoint ptClient = point; ScreenToClient( &ptClient );
	CRect rClient; GetClientRect( &rClient ); rClient.bottom -= 289;

	if( rClient.PtInRect( ptClient ) ) { return HTCAPTION; }

	return HTCLIENT;
}

BOOL CRezultatiDijalog::OnEraseBkgnd(CDC* pDC)
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

void CRezultatiDijalog::OnX()
{
	OnOK();
}

void CRezultatiDijalog::OnIzlazRez()
{
	OnOK();
}
