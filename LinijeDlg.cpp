// LinijeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Linije.h"
#include "LinijeDlg.h"
#include ".\linijedlg.h"
#include <stdlib.h>
#include "SplashScreenEx.h"
#include <time.h>
#include "OtvorenaLista.h"
#include "Cvor.h"
#include "AAlgoritam.h"
#include "SacuvajIgru.h"
#include <process.h>
#include "RezultatiDijalog.h"
#include "Scores.h"
#include <time.h>
#include "ProveriInternet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int VELICINA_MAPE = 10;

#define Slucajni( min, max ) ((rand() % (int)(((max) + 1) - (min))) + (min))

#define HKCU HKEY_CURRENT_USER
#define HKLM HKEY_LOCAL_MACHINE
LPCTSTR KEY_Linije = "Software\\Abell Soft\\Linije";
LPCTSTR VAL_Muzika = "Muzika";
LPCTSTR VAL_Prepreke = "Prepreke";

static char *muzika_midi[] = { "Zvuk.mid" };
static char *verzija = { "Linije 1.0" };
static char* sacuvajFajl = { "linije.sav" };
static char* rezultatiFajl = { "linije.rez" } ;

UINT pusti_zvuk(LPVOID param); 

struct th 
{
	int rb_zvuk;
	CWave zvuk;
	bool pusti;
};
th obj;

// Napomena: HINSTANCE se dobija sa AfxGetInstanceHandle();

// CLinijeDlg dialog

CLinijeDlg::CLinijeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLinijeDlg::IDD, pParent)
	, izracunava(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pok = 0; vel = 0; kliknuto_x = -1; kliknuto_y = -1;
	izracunava = false; broj_slobodnih_cvorova = 0;
	tool_tip = NULL; crvene = plave = zelene = zute = roze = rezultat = 0;
	lista_cvorova = 0; broj_prepreka = 5;
}

void CLinijeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON5, m_meni);
}

BEGIN_MESSAGE_MAP(CLinijeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_PORUKA, KlinutoNaDugmic)
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, KliknutoNaX)
	ON_BN_CLICKED(IDC_BUTTON2, KliknutoNaMin)
	ON_BN_CLICKED(IDC_BUTTON3, OnSistemski)
	ON_BN_CLICKED(IDC_BUTTON4, OnSledece)
	ON_BN_CLICKED(IDC_BUTTON5, prikaziPopUpMeni)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()


// CLinijeDlg message handlers

BOOL CLinijeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	BeginWaitCursor();

	CSplashScreenEx *pSplash=new CSplashScreenEx();
	pSplash->Create(this,"Linije 1.0",0,CSS_FADE | CSS_CENTERSCREEN | CSS_SHADOW|CSS_FADEOUT| CSS_TEXT_BOLD	);
	pSplash->SetBitmap(IDB_SPLASH,255,0,255);
	pSplash->SetTextFont("Verdana",100,CSS_TEXT_NORMAL);
	pSplash->SetTextRect(CRect(47,173,250,189));
	pSplash->SetTextColor(RGB(255,255,255));
	pSplash->SetTextFormat(DT_SINGLELINE | DT_CENTER|DT_VCENTER);
	pSplash->Show();
	pSplash->SetText(_T("Uèitavam grafiku..."));

	m_meni.setBitmapId(ID_MENI);
	m_meni.setType(FooButton::Type::staticButton);
	m_meni.setTextStyle(FooButton::Text::none);

	m_x.LoadBitmaps(_T("ID_XUP"),_T("ID_XDOWN"),_T("ID_XUP"),_T("ID_XUNACTIVE"));
	m_x.SubclassDlgItem(IDC_BUTTON1,this); m_x.SizeToContent();

	m_min.LoadBitmaps(_T("ID_MINUP"),_T("ID_MINDOWN"),_T("ID_MINUP"),_T("ID_MINUNACTIVE"));
	m_min.SubclassDlgItem(IDC_BUTTON2,this); m_min.SizeToContent();

	m_sistemski.LoadBitmaps(_T("ID_SISTEMSKI"),_T("ID_SISTEMSKI"),_T("ID_SISTEMSKI"),_T("ID_SISTEMSKI"));
	m_sistemski.SubclassDlgItem(IDC_BUTTON3,this); m_sistemski.SizeToContent();
	
	m_sledece.LoadBitmaps(_T("ID_SLEDECE"),_T("ID_SLEDECE_KLIK"),_T("ID_SLEDECE"),_T("ID_SLEDECE_NEAKTIVNO"));
	m_sledece.SubclassDlgItem(IDC_BUTTON4,this); m_sledece.SizeToContent();

	CRect rc; GetWindowRect(&rc); rc -= rc.TopLeft();
	m_rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 50, 50);
	SetWindowRgn(m_rgn,TRUE);

    tool_tip = new CToolTipCtrl;
    if(!tool_tip->Create(this)){ return TRUE; }
    tool_tip->AddTool(&m_x,"Close");
	tool_tip->AddTool(&m_min,"Minimize");
	tool_tip->AddTool(&m_sledece,_T("Sledeæe figure"));
	tool_tip->Activate(TRUE); 

	taskbar.Create(this);
	taskbar.SetSkin(IDB_REZULTAT);
	taskbar.SetTextFont("Impact",160,TN_TEXT_NORMAL,TN_TEXT_UNDERLINE);
 	taskbar.SetTextColor(RGB(255,255,0),RGB(0,0,200));
	taskbar.SetTextRect(CRect(10,50,taskbar.m_nSkinWidth-10,taskbar.m_nSkinHeight-25));

	this->SetWindowText(verzija); 

	m_bitmap.LoadBitmap(ID_BMP_1);
	
	rezultat_ekran.Create(_T("Rezultat"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(526, 110, 618, 135), this);
	rezultat_ekran.SetTextColor(RGB(0,255,0)); rezultat_ekran.SetTextBackColor(RGB(0,0,0));
	rezultat_ekran.SetText("0"); rezultat_ekran.SetFontBold(true); rezultat_ekran.SetFontName("Trebuchet MS");
	rezultat_ekran.SetFontSize(26); rezultat_ekran.SetBkColor(RGB(0,0,0));
	
	plave_ekran.Create(_T("Plave"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(514, 443, 530, 455), this);
	plave_ekran.SetTextColor(RGB(0,0,0)); plave_ekran.SetTextBackColor(RGB(180,178,178));
	plave_ekran.SetText("0"); plave_ekran.SetFontName("Tahoma");
	plave_ekran.SetFontSize(12); plave_ekran.SetBkColor(RGB(180,178,178));
		
	crvene_ekran.Create(_T("Crvene"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(540, 443, 555, 455), this);
	crvene_ekran.SetTextColor(RGB(0,0,0)); crvene_ekran.SetTextBackColor(RGB(180,178,178));
	crvene_ekran.SetText("0"); crvene_ekran.SetFontName("Tahoma");
	crvene_ekran.SetFontSize(12); crvene_ekran.SetBkColor(RGB(180,178,178));
	
	zelene_ekran.Create(_T("Zelene"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(564, 443, 579, 455), this);
	zelene_ekran.SetTextColor(RGB(0,0,0)); zelene_ekran.SetTextBackColor(RGB(180,178,178));
	zelene_ekran.SetText("0"); zelene_ekran.SetFontName("Tahoma");
	zelene_ekran.SetFontSize(12); zelene_ekran.SetBkColor(RGB(180,178,178));

	roze_ekran.Create(_T("Roze"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(591, 443, 605, 455), this);
	roze_ekran.SetTextColor(RGB(0,0,0)); roze_ekran.SetTextBackColor(RGB(180,178,178));
	roze_ekran.SetText("0"); roze_ekran.SetFontName("Tahoma");
	roze_ekran.SetFontSize(12); roze_ekran.SetBkColor(RGB(180,178,178));
	
	zute_ekran.Create(_T("Zute"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(616, 443, 631, 455), this);
	zute_ekran.SetTextColor(RGB(0,0,0)); zute_ekran.SetTextBackColor(RGB(180,178,178));
	zute_ekran.SetText("0"); zute_ekran.SetFontName("Tahoma");
	zute_ekran.SetFontSize(12); zute_ekran.SetBkColor(RGB(180,178,178));
	
	sledeca[0].Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(508, 200, 546, 238), this);
	sledeca[1].Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(551, 200, 589, 238), this);
	sledeca[2].Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(594, 200, 632, 238), this);
	sledeca[0].EnableWindow(FALSE); sledeca[1].EnableWindow(FALSE); sledeca[2].EnableWindow(FALSE);

	pSplash->SetText(_T("Uèitavam podatke..."));

	boja = new int[5];
	boja[0] = IDR_KUGLA; boja[1] = IDR_CUBE; boja[2] = IDR_ROMB; boja[3] = IDR_TROUGAO; boja[4] = IDR_ZVEZDA;
					
	map = new Cvor*[VELICINA_MAPE];
	for(int i = 0; i < VELICINA_MAPE; i++)
	{
		map[i] = new Cvor[VELICINA_MAPE];
		for(int j = 0; j < VELICINA_MAPE; j++)
		{
			map[i][j] = Cvor(i, j, prohodan);

			Cvorovi *novi = new Cvorovi(i,j);
			novi->sled = lista_cvorova;
			lista_cvorova = novi; broj_slobodnih_cvorova++;
		}
	}
	
	VERIFY (menu.LoadMenu (IDR_POPUP_MENU));
	pPopupMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu (0));
	pPopupMenu->SetMenuTitle(_T("Opcije"),MFT_ROUND|MFT_GRADIENT|MFT_CENTER|MFT_TOP_TITLE);
	pPopupMenu->SetMenuDrawMode(2);
	ASSERT (pPopupMenu != NULL);

	CFile f; CFileException e;
	if(!f.Open( sacuvajFajl, CFile::modeRead , &e ) )
	{
		menu.EnableMenuItem(CMD_3, MF_BYCOMMAND|MF_DISABLED| MF_GRAYED);
		pPopupMenu->EnableMenuItem(CMD_3, MF_BYCOMMAND|MF_DISABLED| MF_GRAYED);		
	}
	else f.Close();

	CNewMenuHelper myHelper(NEW_MENU_DIALOG_SUBCLASS|NEW_MENU_DIALOG_SYSTEM_MENU);

	HKEY hk;
	if (RegOpenKey(HKLM, KEY_Linije,&hk)!=ERROR_SUCCESS)
	{
		RegCreateKey(HKLM, KEY_Linije, &hk);
		DWORD val=1;
		RegSetValueEx(hk, VAL_Muzika, NULL, REG_DWORD, (BYTE*)&val, sizeof(val));
		RegSetValueEx(hk, VAL_Prepreke, NULL, REG_DWORD, (BYTE*)&val, sizeof(val));
		prepreke = false; pusti_muziku = true; 

		pPopupMenu->CheckMenuItem(32776, MF_BYCOMMAND|MF_UNCHECKED);
		pPopupMenu->CheckMenuItem(32774, MF_BYCOMMAND|MF_CHECKED);
	}
	else { 
		DWORD val=0; DWORD len=4; 
		RegQueryValueEx(hk, VAL_Muzika, NULL, NULL, (BYTE*)&val, &len);
		if(val==1) { pusti_muziku = true; pPopupMenu->CheckMenuItem(32774, MF_BYCOMMAND|MF_CHECKED); }
		else { pusti_muziku = false; pPopupMenu->CheckMenuItem(32774, MF_BYCOMMAND|MF_UNCHECKED); }
		RegQueryValueEx(hk, VAL_Prepreke, NULL, NULL, (BYTE*)&val, &len);
		if(val==1) { prepreke = true; pPopupMenu->CheckMenuItem(32776, MF_BYCOMMAND|MF_CHECKED); }
		else { prepreke = false; pPopupMenu->CheckMenuItem(32776, MF_BYCOMMAND|MF_UNCHECKED); }
	}
	RegCloseKey(hk);

	postavi_sledece();
	if(prepreke) dodajPrepreke(broj_prepreka); 
	dodajNaEkran(3); 
	
	pSplash->SetText(_T("Uèitavam muziku..."));

	muzika.Open(muzika_midi[0]); 
	if(pusti_muziku) muzika.Play();

	obj.pusti = pusti_muziku; 

	EndWaitCursor(); 

	pSplash->Hide(); 

	CenterWindow(); 

   	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLinijeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLinijeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT CLinijeDlg::OnNcHitTest(CPoint point)
{	
	CPoint ptClient = point; ScreenToClient( &ptClient );
	CRect rClient; GetClientRect( &rClient ); rClient.bottom -= 517;

	if( rClient.PtInRect( ptClient ) ) { return HTCAPTION; }

	return HTCLIENT;
}

BOOL CLinijeDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor( AfxGetApp()->LoadCursor( IDC_CURSOR2 ) );

	return true;
}

bool CLinijeDlg::dodajNaEkran(int cvorova)
{
	srand( (unsigned)time( NULL ) );

	if(broj_slobodnih_cvorova < cvorova) cvorova = broj_slobodnih_cvorova;

	for(int i = 0; i < cvorova; i++)
	{
		int x = -1, y = -1;

		int slucajni = rand() % broj_slobodnih_cvorova; int brojac = 0;
		Cvorovi* tek = lista_cvorova; Cvorovi* stari = 0;

		while(brojac < slucajni){ stari = tek; tek = tek->sled; brojac++; }
		x = tek->cvor.uzmi_x(); y = tek->cvor.uzmi_y();

		if(!stari) { Cvorovi* brisi = tek; tek = tek->sled; lista_cvorova = tek; delete brisi; broj_slobodnih_cvorova--; }
		else { Cvorovi* brisi = tek; tek = tek->sled; stari->sled = tek; delete brisi; broj_slobodnih_cvorova--; }
		

		int ekran_x = 53, ekran_y = 93;
		ekran_x += 37 * x; ekran_x += x * 2;
		ekran_y += 38 * y; ekran_y += y * 2;
		
		Slike *sl = new Slike; sl->sled = pok; pok = sl; sl = 0;
		pok->slika.Create(_T(""), WS_CHILD | SS_CENTER, CRect(ekran_x, ekran_y, ekran_x + 37, ekran_y + 38), this);

		int postavi_oblik = sledeca[i].uzmi_oblik();

		switch(postavi_oblik)
		{
			case IDR_CUBE: crvene++; break;
			case IDR_KUGLA: plave++; break;
			case IDR_ROMB: zute++; break;
			case IDR_TROUGAO: zelene++; break;
			case IDR_ZVEZDA: roze++; break;
			default: break;
		}

		map[ x ][ y ].upisi_status( neprohodan );
		map[ x ][ y ].upisi_boju( postavi_oblik );

		if ( pok->slika.Load( MAKEINTRESOURCE(postavi_oblik), _T("GIF") ) )
		{ 
			pok->slika.ModifyStyle(NULL, WS_VISIBLE); pok->slika.Draw(); pok->slika.upisi_oblik( postavi_oblik );
			pok->slika.cvor.upisi_boju( postavi_oblik ); 
			pok->slika.cvor.upisi_x( x ); pok->slika.cvor.upisi_y( y );
		}
		vel++; sl = 0; tek = stari = 0;
	}

	brojFiguraNaEkranu();
	postavi_sledece();

	obj.rb_zvuk = 0; AfxBeginThread(pusti_zvuk,&obj);

	return true;
}

bool CLinijeDlg::daLiPostoji(int x, int y) 
{
	Slike *tek = pok;
	while( tek )
	{
		if( ((tek->slika.cvor.uzmi_x()) == x) && ((tek->slika.cvor.uzmi_y() == y)) )
		{ tek = 0; return true; }
		tek = tek->sled;
	}
	tek = 0;
	return false;
}

LRESULT CLinijeDlg::KlinutoNaDugmic(WPARAM wParam, LPARAM lParam)
{
	obj.rb_zvuk = 3; AfxBeginThread(pusti_zvuk,&obj);

	if( (kliknuto_x == -1) || (kliknuto_y == -1) ) 
	{ kliknuto_x = (int)wParam; kliknuto_y = (int)lParam; return 0; }

	if( (kliknuto_x == (int)wParam) && (kliknuto_y == (int)lParam) ) 
	{ kliknuto_x = -1; kliknuto_y = -1; return 0; }

	Slike *tek = pok;
	while( tek )
	{
		if( ((tek->slika.cvor.uzmi_x()) == kliknuto_x) && ((tek->slika.cvor.uzmi_y() == kliknuto_y)) )
		{ 
			int oblik = tek->slika.uzmi_oblik(); tek->slika.upisi_misa( false ); tek->slika.upisi_klik( false ); tek->slika.upisi_oblik( oblik );
			if(tek->slika.Load( MAKEINTRESOURCE(oblik), _T("GIF") )) tek->slika.Draw();
			kliknuto_x = (int)wParam; kliknuto_y = (int)lParam;	break;
		}
		tek = tek->sled;
	}
	tek = 0; 

	return 0;
}


void CLinijeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( !izracunava ) 
	{
		if( ( (point.x > 53) && (point.x < 445) ) && ( (point.y > 93) && (point.y < 491) ) )
		{
			if( kliknuto_x >= 0 && kliknuto_y >= 0 ) 
			{
				izracunava = true;

				Slike *tek = pok; 
				while( (tek->slika.cvor.uzmi_x() != kliknuto_x) || (tek->slika.cvor.uzmi_y() != kliknuto_y) ) tek = tek->sled;

				point.x -= 53; int cilj_x = point.x / 39;
				point.y -= 93; int cilj_y = point.y / 40;

				if(cilj_x < 0) cilj_x = 0;
				else if(cilj_x > 9) cilj_x = 9;
				if(cilj_y < 0) cilj_y = 0;
				else if(cilj_y > 9) cilj_y = 9;
				if(map[cilj_x][cilj_y].uzmi_status() == neprohodan) 
				{ izracunava = false; tek = 0; return CDialog::OnLButtonDown(nFlags, point); }

				CAAlgoritam algoritam(map, VELICINA_MAPE); 
				if( algoritam.pronadji_put(kliknuto_y, kliknuto_x, cilj_y, cilj_x) )
				{ 
					COtvorenaLista put = algoritam.uzmi_put(); 
					
					int oblik = tek->slika.uzmi_oblik();
					tek->slika.cvor.upisi_x( cilj_x ); tek->slika.cvor.upisi_y( cilj_y );
					tek->slika.upisi_oblik( oblik ); tek->slika.Stop();
					tek->slika.Load( MAKEINTRESOURCE(oblik), _T("GIF") );
					tek->slika.upisi_misa( false ); tek->slika.upisi_klik( false );
					
					map[ cilj_x ][ cilj_y ].upisi_status( neprohodan );
					map[ cilj_x ][ cilj_y ].upisi_boju(map[ kliknuto_x ][ kliknuto_y ].uzmi_boju());
					map[ kliknuto_x ][ kliknuto_y ].upisi_status( prohodan );
					map[ kliknuto_x ][ kliknuto_y ].upisi_boju(-1);

					Cvorovi*tekk=lista_cvorova, *stari=0;
					while((tekk->cvor.uzmi_x()!=cilj_x) || (tekk->cvor.uzmi_y()!=cilj_y))
					{ stari = tekk; tekk=tekk->sled; }
					if(!stari) { Cvorovi* brisi = tekk; tekk = tekk->sled; lista_cvorova = tekk; delete brisi; }
					else { Cvorovi* brisi = tekk; tekk = tekk->sled; stari->sled = tekk; delete brisi; }
					Cvorovi *novi = new Cvorovi(kliknuto_x,kliknuto_y);
					novi->sled = lista_cvorova;	lista_cvorova = novi;

					int x_nova = 0; int y_nova = 0; int duzina_puta = algoritam.uzmi_duzinu(); int pauza = 100;

					if(duzina_puta < 5) pauza = 130;
					else if (duzina_puta >= 5 && duzina_puta <= 10) pauza = 120;
					else if(duzina_puta > 10) pauza = 110;

					for(int i = 0; i < duzina_puta; i++)
					{
						int x = put[i].uzmi_x(); int y = put[i].uzmi_y();
						x_nova = 53; x_nova += 37 * x; x_nova += x * 2;
						y_nova = 93; y_nova += 38 * y; y_nova += y * 2; 
						
						WINDOWPLACEMENT wnd; 
						wnd.flags = WPF_RESTORETOMAXIMIZED;
						wnd.showCmd = SW_SHOW;
						wnd.ptMaxPosition.x = x_nova; wnd.ptMaxPosition.y = y_nova;
						wnd.ptMinPosition.x = x_nova; wnd.ptMinPosition.y = y_nova;
						wnd.rcNormalPosition.left = x_nova;
						wnd.rcNormalPosition.top = y_nova;
						wnd.rcNormalPosition.right = x_nova + 37;
						wnd.rcNormalPosition.bottom = y_nova + 38;
						tek->slika.SetWindowPlacement( &wnd );
						tek->slika.Draw(); UpdateWindow(); Sleep(pauza);
					}

					if(!imaLiPonavljanja()) 
					{
					  dodajNaEkran(3);
				 	  while(imaLiPonavljanja()); 
					}

					m_sledece.EnableWindow(TRUE); 

					if(!broj_slobodnih_cvorova)  
					{ 
						obj.rb_zvuk = 2; AfxBeginThread(pusti_zvuk,&obj);
						proveriRezultat(rezultat); if(!rezultat) MessageBox("Igra je zavrsena!", "Linije 1.0"); rezultat = 0;
					}
				}
				else
				{
					int oblik = tek->slika.uzmi_oblik();
					tek->slika.upisi_oblik( oblik );
					if(tek->slika.Load( MAKEINTRESOURCE(oblik), _T("GIF") )) tek->slika.Draw();
					tek->slika.upisi_misa( false ); tek->slika.upisi_klik( false ); 
				}
				tek = 0; izracunava = false; kliknuto_x = -1; kliknuto_y = -1;
			}
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CLinijeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	HKEY hk;
	if (RegOpenKey(HKLM, KEY_Linije,&hk)==ERROR_SUCCESS)
	{
		DWORD val;
		if(pusti_muziku) val = 1;
		else val = 0;
		RegSetValueEx(hk, VAL_Muzika, NULL, REG_DWORD, (BYTE*)&val, sizeof(val));
		if(prepreke) val = 1;
		else val = 0;
		RegSetValueEx(hk, VAL_Prepreke, NULL, REG_DWORD, (BYTE*)&val, sizeof(val));
		RegCloseKey(hk);
	}

	Slike *stari = pok;
	while(pok){	pok = pok->sled; stari->slika.DestroyWindow(); delete stari; stari = pok; }
	pok = stari = 0;

	Cvorovi *star = lista_cvorova; 
	while(lista_cvorova) { lista_cvorova = lista_cvorova->sled; delete star; star = lista_cvorova; }
	star = lista_cvorova = 0; broj_slobodnih_cvorova = 0;

	for(int i = 0; i < VELICINA_MAPE; i++) delete [] map[i];
	delete [] map; map = 0;

	delete tool_tip;
	delete [] boja;

	if(pusti_muziku) { if(muzika.IsPlaying()) muzika.Stop(); muzika.Close(); }
}

BOOL CLinijeDlg::OnEraseBkgnd(CDC* pDC)
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

void CLinijeDlg::toClipboard(CWnd * wnd, BOOL FullWnd) 
{
     CDC dc;
     if(FullWnd)
        { /* full window */
         HDC hdc = ::GetWindowDC(wnd->m_hWnd);
         dc.Attach(hdc);
        } /* full window */
     else
        { /* client area only */
         HDC hdc = ::GetDC(wnd->m_hWnd);
         dc.Attach(hdc);
        } /* client area only */
 
     CDC memDC;
     memDC.CreateCompatibleDC(&dc);
 
     CBitmap bm;
     CRect r;
     if(FullWnd)
        wnd->GetWindowRect(&r);
     else
         wnd->GetClientRect(&r);
 
     CString s;
     wnd->GetWindowText(s);
     CSize sz(r.Width(), r.Height());
     bm.CreateCompatibleBitmap(&dc, sz.cx, sz.cy);
     CBitmap * oldbm = memDC.SelectObject(&bm);
     memDC.BitBlt(0, 0, sz.cx, sz.cy, &dc, 0, 0, SRCCOPY);
 
     if(wnd->OpenClipboard())
	 {
		::EmptyClipboard();
		::SetClipboardData(CF_BITMAP, bm.m_hObject);
		CloseClipboard();
	 }
 
     memDC.SelectObject(oldbm);
     bm.Detach();  // make sure bitmap not deleted with CBitmap object
                   // read my essay on this technique
	 ::ReleaseDC(wnd->m_hWnd,dc.Detach()); // 
}


bool CLinijeDlg::pronadji_ponavljanja(int *boja,
									   int n 
									   )
{
	int velicina = 10; 

	~ponavljanja;

	for(int b = 0; b < n; b++)
	{
		for(int i = 0; i < velicina; i++)
		{
			for(int j=0; j < velicina; j++)
			{
				if(((map[j][i].uzmi_boju() == boja[b])) && ((map[j][i].uzmi_status() == neprohodan))) ponavljanja += map[j][i]; 
				else
				{
					if((ponavljanja() >= 5) && (ponavljanja() <= 10)) return true;
					else ~ponavljanja;
				}
			}
			if((ponavljanja() >= 5) && (ponavljanja() <= 10)) return true;
			else ~ponavljanja;
		}

		for(int i = 0; i < velicina; i++)
		{
			for(int j = 0; j < velicina; j++)
			{
				if(((map[i][j].uzmi_boju() == boja[b])) && ((map[i][j].uzmi_status() == neprohodan))) ponavljanja += map[i][j]; 	
				else
				{
					if((ponavljanja() >= 5) && (ponavljanja() <= 10)) return true;
					else ~ponavljanja;
				}
			}
			if((ponavljanja() >= 5) && (ponavljanja() <= 10)) return true;
			else ~ponavljanja;	
		}

		for(int i = 0; i < velicina; i++)
		{
			for(int k = 0; k < velicina; k++)
			{
				for(int j = k, p = i; j < velicina; j++, p++)
				{
					if(((map[j][p].uzmi_boju() == boja[b])) && ((map[j][p].uzmi_status() == neprohodan))) ponavljanja += map[j][p]; 
					else
					{
						if((ponavljanja() >= 5) && (ponavljanja() <= 10)) return true;
						else ~ponavljanja;					
					}
				}
				if((ponavljanja() >= 5) && (ponavljanja() <= 10)) return true;
				else ~ponavljanja;						
			}
			if((ponavljanja() >= 5) && (ponavljanja() <= 10)) return true;
			else ~ponavljanja;							
		}

		for(int i = 0; i < velicina; i++)
		{
			for(int k = velicina-1; k >= 0; k--)
			{
				for(int j = k, p = i; p < velicina && j >= 0; j--, p++)
				{
					if(((map[j][p].uzmi_boju() == boja[b])) && ((map[j][p].uzmi_status() == neprohodan))) ponavljanja += map[j][p]; 
					else
					{
						if((ponavljanja() >= 5) && (ponavljanja() <= 10)) return true;
						else ~ponavljanja;					
					}
				}
				if((ponavljanja() >= 5) && (ponavljanja() <= 10)) return true;
				else ~ponavljanja;						
			}
			if((ponavljanja() >= 5) && (ponavljanja() <= 10)) return true;
			else ~ponavljanja;						
		}
	}

	return false;
}

void CLinijeDlg::izbaci_ponavljanja(void) 
{
	int duz = ponavljanja(); 

	if( duz )
	{
		int oblik = -1;
		for( int i = 0; i < duz; i++ )
		{
			Cvor temp = ponavljanja[i]; int x = temp.uzmi_x(); int y = temp.uzmi_y();
			Slike *tek = pok, *stari=0; 

			while( (tek->slika.cvor.uzmi_x() != x) || (tek->slika.cvor.uzmi_y() != y) ) { stari = tek;  tek = tek->sled; }
			oblik = tek->slika.uzmi_oblik(); 

			tek->slika.DestroyWindow(); vel--;

			if( !stari ) { Slike* brisi = tek; tek = tek->sled; pok = tek; delete brisi; }
			else { Slike* brisi = tek; tek = tek->sled; stari->sled = tek; delete brisi; }

			Cvorovi *novi = new Cvorovi(x,y);
			novi->sled = lista_cvorova;	lista_cvorova = novi; broj_slobodnih_cvorova++;

			map[ x ][ y ].upisi_status( prohodan );
			map[ x ][ y ].upisi_boju( -1 );

			novi = 0; tek = 0;
		}

		switch(oblik)
		{
			case IDR_CUBE: crvene -= duz; break;
			case IDR_KUGLA: plave -= duz; break;
			case IDR_ROMB: zute -= duz; break;
			case IDR_TROUGAO: zelene -= duz; break;
			case IDR_ZVEZDA: roze -= duz; break;
			default: break;
		}
		~ponavljanja; brojFiguraNaEkranu();
	}
}

void CLinijeDlg::OnClose()
{
	proveriRezultat( rezultat ); CDialog::OnClose(); CDialog::OnOK(); ::EndDialog(m_hWnd, NULL);
}

void CLinijeDlg::KliknutoNaX() { OnClose(); }

void CLinijeDlg::KliknutoNaMin() { ShowWindow(SW_MINIMIZE); }

void CLinijeDlg::dodajPrepreke(int n)
{
	if(broj_slobodnih_cvorova < 1) return;
	else if(broj_slobodnih_cvorova > 1 && broj_slobodnih_cvorova < n) n = broj_slobodnih_cvorova;
	
	srand( (unsigned)time( NULL ) );

	for(int i = 0; i < n; i++)
	{
		int x = -1, y = -1;

		int slucajni = rand() % broj_slobodnih_cvorova; int brojac = 0;
		Cvorovi* tek = lista_cvorova; Cvorovi* stari = 0;

		while(brojac < slucajni){ stari = tek; tek = tek->sled; brojac++; }
		x = tek->cvor.uzmi_x(); y = tek->cvor.uzmi_y();
		
		if(!stari) { Cvorovi* brisi = tek; tek = tek->sled; lista_cvorova = tek; delete brisi; broj_slobodnih_cvorova--; }
		else { Cvorovi* brisi = tek; tek = tek->sled; stari->sled = tek; delete brisi; broj_slobodnih_cvorova--; }
		
		int ekran_x = 53, ekran_y = 93;
		ekran_x += 37 * x; ekran_x += x * 2;
		ekran_y += 38 * y; ekran_y += y * 2;

		Slike *sl = new Slike; sl->sled = pok; pok = sl; sl = 0;
		pok->slika.Create(_T(""), WS_CHILD | SS_CENTER, CRect(ekran_x, ekran_y, ekran_x + 37, ekran_y + 38), this);
		
		map[ x ][ y ].upisi_status( neprohodan );
		map[ x ][ y ].upisi_boju( IDR_ZIDIC );

		if ( pok->slika.Load( MAKEINTRESOURCE(IDR_ZIDIC), _T("GIF") ) )
		{ 
			pok->slika.EnableWindow(FALSE); pok->slika.ModifyStyle(NULL, WS_VISIBLE); pok->slika.Draw(); pok->slika.upisi_oblik( IDR_ZIDIC );
			pok->slika.cvor.upisi_x( x ); pok->slika.cvor.upisi_y( y );
		}
		vel++; sl = 0; tek = stari = 0;
	}
}

void CLinijeDlg::postavi_sledece(void) 
{
	for(int i = 0; i < 3; i++)
	{
		int oblik = rand() % 5000; int postavi_oblik; 

		if( oblik >= 0 && oblik < 1000 ) postavi_oblik = IDR_KUGLA;
		else if( oblik >= 1000 && oblik < 2000 ) postavi_oblik = IDR_CUBE;
		else if( oblik >= 2000 && oblik < 3000 ) postavi_oblik = IDR_ROMB;
		else if( oblik >= 3000 && oblik < 4000 ) postavi_oblik = IDR_TROUGAO;
		else if( oblik >= 4000 && oblik < 5000 ) postavi_oblik = IDR_ZVEZDA;

		sledeca[i].UnLoad();
		if ( sledeca[i].Load( MAKEINTRESOURCE(postavi_oblik), _T("GIF") ) )
		{	
			sledeca[i].Draw(); sledeca[i].upisi_oblik( postavi_oblik );
			sledeca[i].cvor.upisi_boju( postavi_oblik );
		}
	}
}

void CLinijeDlg::upisi_rezultat(int rez) 
{
	rezultat += rez; CString temp; temp.Format("%d",rezultat);
	rezultat_ekran.SetText((LPCTSTR)temp);
}

void CLinijeDlg::OnSistemski() 
{
	RECT rect; m_sistemski.GetClientRect(&rect); ClientToScreen(&rect);
	::TrackPopupMenuEx(::GetSystemMenu(m_hWnd, FALSE),TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left+20, rect.top+27, m_hWnd, NULL);
}

BOOL CLinijeDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  if (0xF000 <= wParam)
    return (BOOL)DefWindowProc(WM_SYSCOMMAND, wParam, lParam);

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CLinijeDlg::PreTranslateMessage(MSG* pMsg) 
{
  if (NULL != tool_tip) tool_tip->RelayEvent(pMsg);

  return CDialog::PreTranslateMessage(pMsg);
}

void CLinijeDlg::brojFiguraNaEkranu(void) 
{
	CString temp;
	temp = crvene_ekran.GetText(); 
	if(atoi((LPCSTR)temp) != crvene){ temp.Format("%d",crvene); crvene_ekran.SetText(temp); }
	temp = zelene_ekran.GetText(); 
	if(atoi((LPCSTR)temp) != zelene){ temp.Format("%d",zelene); zelene_ekran.SetText(temp); }
	temp = plave_ekran.GetText(); 
	if(atoi((LPCSTR)temp) != plave){ temp.Format("%d",plave); plave_ekran.SetText(temp); }
	temp = zute_ekran.GetText(); 
	if(atoi((LPCSTR)temp) != zute){ temp.Format("%d",zute); zute_ekran.SetText(temp); }
	temp = roze_ekran.GetText(); 
	if(atoi((LPCSTR)temp) != roze){ temp.Format("%d",roze); roze_ekran.SetText(temp); }
}

void CLinijeDlg::OnSledece() 
{ 
	postavi_sledece(); m_sledece.EnableWindow(FALSE);
}

void CLinijeDlg::prikaziPopUpMeni()
{
  int nResult = m_meni.displayPopupMenu (pPopupMenu);

  CDijalogOprogramu dlg_oprogramu;
  CRezultatiDijalog dlg_rezultati; 
  CProveriInternet dlg_internet; 
  DWORD ids[3] ; ids[0] = IDD_LINIJE_DIALOG ; ids[1] = 0;

  switch(nResult)
  {
	case CMD_1: resetujIgru();
		break;
	case CMD_2: if(sacuvajIgru()) MessageBox("Igra je sacuvana !","Linije 1.0",MB_OK);
		break;
	case CMD_3: if(!ucitajIgru()) MessageBox("Igra nije sacuvana !","Linije 1.0",MB_OK);
		break;
	case CMD_4: 	
		dlg_rezultati.unos = false; dlg_rezultati.DoModal();
		break;
	case CMD_5:
		dlg_oprogramu.DoModal();
		break;
	case CMD_6:
		OnClose(); break;
	case 32774:
		if(pusti_muziku) { muzika.Stop(); pPopupMenu->CheckMenuItem(32774, MF_BYCOMMAND|MF_UNCHECKED); pusti_muziku = false; }
		else { muzika.Play(); pPopupMenu->CheckMenuItem(32774, MF_BYCOMMAND|MF_CHECKED); pusti_muziku = true; }
		obj.pusti = pusti_muziku;
		break;
	case 32776:
		if( prepreke )
		{
			pPopupMenu->CheckMenuItem(32776, MF_BYCOMMAND|MF_UNCHECKED);
			izbaciPrepreke(); prepreke = false;
		}
		else
		{
			pPopupMenu->CheckMenuItem(32776, MF_BYCOMMAND|MF_CHECKED);
			dodajPrepreke(5); prepreke = true;
		}
		break;
	case 32777: 
		toClipboard(this, FALSE);
		break;
	case 32778:
		::HtmlHelp(this->m_hWnd,"Linije.chm::/Pomoc.htm", HH_DISPLAY_TOPIC,NULL);
		break;
	case 32779: 
		dlg_internet.DoModal();
		break;
	default: break;
  }
}

void CLinijeDlg::resetujIgru(void) 
{
	Slike *stari = pok; WINDOWPLACEMENT win;
	while(pok) 
	{
		pok->slika.GetWindowPlacement(&win); pok->slika.UnLoad(); pok->slika.ModifyStyle(WS_VISIBLE, NULL);
		pok->slika.DestroyWindow(); InvalidateRect(&win.rcNormalPosition);
		pok = pok->sled; delete stari; stari = pok;	
	}
	pok = stari = 0;

	Cvorovi *star = lista_cvorova;
	while(lista_cvorova) { lista_cvorova = lista_cvorova->sled; delete star; star = lista_cvorova; }
	star = lista_cvorova = 0; broj_slobodnih_cvorova = 0;

	for(int i = 0; i < VELICINA_MAPE; i++) delete [] map[i];
	delete [] map; map = 0;

	pok = 0; vel = 0; kliknuto_x = -1; kliknuto_y = -1;	izracunava = false;
	crvene = plave = zelene = zute = roze = rezultat = 0;

	map = new Cvor*[VELICINA_MAPE];
	for(int i = 0; i < VELICINA_MAPE; i++)
	{
		map[i] = new Cvor[VELICINA_MAPE];
		for(int j = 0; j < VELICINA_MAPE; j++)
		{
			map[i][j] = Cvor(i, j, prohodan);

			Cvorovi *novi = new Cvorovi(i,j); novi->sled = lista_cvorova;
			lista_cvorova = novi; broj_slobodnih_cvorova++;
		}
	}
	upisi_rezultat(0); if(prepreke) dodajPrepreke(5); dodajNaEkran(3);
}

bool CLinijeDlg::sacuvajIgru()
{
	CFile f; CFileException e; 
	CSacuvajIgru sacuvaj(map,VELICINA_MAPE,rezultat); 
	if(f.Open( sacuvajFajl, CFile::modeWrite|CFile::modeCreate , &e ))
	{
		CArchive ar( &f, CArchive::store, 512, NULL);	
		ar << sacuvaj; ar.Close(); f.Close();
	}
	else return false;

	menu.EnableMenuItem(CMD_3, MF_BYCOMMAND|MF_ENABLED);
	pPopupMenu->EnableMenuItem(CMD_3, MF_BYCOMMAND|MF_ENABLED);	

	return true;
}

bool CLinijeDlg::ucitajIgru()
{
	CFile f; CFileException e; CSacuvajIgru ucitaj;
	if(f.Open( sacuvajFajl, CFile::modeRead , &e ) )
	{
		CArchive ar( &f, CArchive::load, 4096, NULL);	
		ar >> ucitaj; ar.Close(); f.Close();
	}
	else return false;

	Slike *stari = pok; WINDOWPLACEMENT win;
	while(pok)
	{	
		pok->slika.GetWindowPlacement(&win); pok->slika.UnLoad(); 
		pok->slika.ModifyStyle(WS_VISIBLE,NULL); pok->slika.DestroyWindow(); 
		InvalidateRect(&win.rcNormalPosition); pok = pok->sled; delete stari; stari = pok;	
	}
	pok = stari = 0;

	Cvorovi *star = lista_cvorova; 
	while(lista_cvorova) { lista_cvorova = lista_cvorova->sled; delete star; star = lista_cvorova; }
	star = lista_cvorova = 0; broj_slobodnih_cvorova = 0;

	for(int i = 0; i < VELICINA_MAPE; i++) delete [] map[i]; delete [] map; map = 0;

	pok = 0; vel = 0; kliknuto_x = -1; kliknuto_y = -1;	izracunava = false;
	crvene = plave = zelene = zute = roze = rezultat = 0;

	map = new Cvor*[VELICINA_MAPE]; bool ima_prepreka = false;
	for(int i = 0, brojac = 0; i < VELICINA_MAPE; i++)
	{
		map[i] = new Cvor[VELICINA_MAPE];
		for(int j = 0; j < VELICINA_MAPE; j++, brojac++)
		{
			map[i][j] = Cvor(i, j, ucitaj.status[brojac], ucitaj.boja[brojac]);

			if( map[i][j].uzmi_status() == neprohodan ) 
			{
				int ekran_x = 53, ekran_y = 93;
				ekran_x += 37 * i; ekran_x += i * 2;
				ekran_y += 38 * j; ekran_y += j * 2;
				
				Slike *sl = new Slike; sl->sled = pok; pok = sl; sl = 0;
				pok->slika.Create(_T(""), WS_CHILD | SS_CENTER, CRect(ekran_x, ekran_y, ekran_x + 37, ekran_y + 38), this);
				
				int postavi_oblik = ucitaj.boja[brojac];

				switch(postavi_oblik) 
				{
					case IDR_CUBE: crvene++; break;
					case IDR_KUGLA: plave++; break;
					case IDR_ROMB: zute++; break;
					case IDR_TROUGAO: zelene++; break;
					case IDR_ZVEZDA: roze++; break;
					case IDR_ZIDIC: ima_prepreka = true; break;
					default: break;
				}

				if ( pok->slika.Load( MAKEINTRESOURCE(postavi_oblik), _T("GIF") ) )
				{ 
					pok->slika.ModifyStyle(NULL,WS_VISIBLE); pok->slika.Draw(); pok->slika.upisi_oblik( postavi_oblik );
					pok->slika.cvor.upisi_boju( postavi_oblik );
					pok->slika.cvor.upisi_x( i ); pok->slika.cvor.upisi_y( j );
					if(postavi_oblik==IDR_ZIDIC) pok->slika.EnableWindow(FALSE);
				}
				vel++;
			}
			else 
			{
				Cvorovi *novi = new Cvorovi(i,j);
				novi->sled = lista_cvorova;
				lista_cvorova = novi; broj_slobodnih_cvorova++;
			}
		}
	}
	upisi_rezultat(ucitaj.rezultat); postavi_sledece(); brojFiguraNaEkranu();
	if(ima_prepreka) 
	{
		if( !prepreke )
		{
			pPopupMenu->CheckMenuItem(32776, MF_BYCOMMAND|MF_CHECKED);
			prepreke = true;
		}
	}
	else
	{
		if( prepreke )
		{
			pPopupMenu->CheckMenuItem(32776, MF_BYCOMMAND|MF_UNCHECKED);
			prepreke = false;
		}
	}
	return true;
}

void CLinijeDlg::izbaciPrepreke(void)
{
	for(int i = 0; i < 5; i++)
	{
		Slike *tek = pok, *stari=0; WINDOWPLACEMENT win;
		while( tek && (tek->slika.uzmi_oblik() != IDR_ZIDIC) ) { stari = tek;  tek = tek->sled;}
		
		if(tek)
		{	
			int x = tek->slika.cvor.uzmi_x(); int y = tek->slika.cvor.uzmi_y();
			tek->slika.GetWindowPlacement(&win); tek->slika.UnLoad(); tek->slika.ModifyStyle(WS_VISIBLE,NULL);
			tek->slika.DestroyWindow(); vel--;	InvalidateRect(&win.rcNormalPosition); 

			if( !stari ) { Slike* brisi = tek; tek = tek->sled; pok = tek; delete brisi; }
			else { Slike* brisi = tek; tek = tek->sled; stari->sled = tek; delete brisi; }

			Cvorovi *novi = new Cvorovi(x,y);
			novi->sled = lista_cvorova;	lista_cvorova = novi; broj_slobodnih_cvorova++;

			map[ x ][ y ].upisi_status( prohodan );
			map[ x ][ y ].upisi_boju( -1 );

			novi = 0;
		}
			tek = stari = 0; 
	}
}

UINT pusti_zvuk(LPVOID param)
{
	th *temp= (th*)param;

	if(!temp->pusti) return 0;
	
	switch(temp->rb_zvuk)
	{
		case 0:
			temp->zvuk.PlayFromDisk("AddBall.wav");
			break;
		case 1: 
			temp->zvuk.PlayFromDisk("bonus.wav");
			break;
		case 2: 
			temp->zvuk.PlayFromDisk("GameOver.wav");
			break;
		case 3:
			temp->zvuk.PlayFromDisk("loptica.wav");
			break;
		case 4: 
			temp->zvuk.PlayFromDisk("DelBall.wav");
			break;
		default: break;
	}
	return 0;
}

void CLinijeDlg::proveriRezultat(int rez)
{
	if(!rez) return;
	CFile f; CFileException e; int i=0;
	if(f.Open( rezultatiFajl, CFile::modeRead , &e ) )
	{
		CArchive ar( &f, CArchive::load, 4096, NULL);	
		CScores scores; ar >> scores;
		for(i=0;i<10;i++)
			if( rez > scores.scores[i] ) break;
		ar.Close();	f.Close();
	}
	if( i < 10 ) { CRezultatiDijalog dlg;  dlg.unos = true; dlg.rezultat = rez; dlg.DoModal(); }
}

BOOL CLinijeDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	::HtmlHelp(this->m_hWnd,"Linije.chm::/Pomoc.htm", HH_DISPLAY_TOPIC,NULL);

	return TRUE;
}

bool CLinijeDlg::imaLiPonavljanja(void)
{
	if(pronadji_ponavljanja(boja,5))
	{									
		int pon = ponavljanja();
		if(pon > 5) 
		{ 
			obj.rb_zvuk = 1; AfxBeginThread(pusti_zvuk,&obj);
			switch(pon) 
			{
				case 6: taskbar.Show(" 50 poena"); upisi_rezultat(150); break;
				case 7: taskbar.Show(" 100 poena"); upisi_rezultat(200); break;
				case 8: taskbar.Show(" 150 poena"); upisi_rezultat(250); break;
				case 9: taskbar.Show(" 200 poena"); upisi_rezultat(300); break;
				case 10: taskbar.Show(" 250 poena"); upisi_rezultat(350); break;
				default: upisi_rezultat(100); break;
			}
		}
		else 
		{ 
			upisi_rezultat(100);
			obj.rb_zvuk = 4; AfxBeginThread(pusti_zvuk,&obj);
		}

		izbaci_ponavljanja(); return true;
	}
	else return false;
}