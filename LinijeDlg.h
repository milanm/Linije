// LinijeDlg.h : header file
//
#pragma once

#include "Midi.h"
#include "Wave.h"
#include "TaskbarNotifier.h"
#include "PictureEx.h"
#include "OtvorenaLista.h"
#include "afxext.h"
#include "NewLabel.h"
#include "FooButton.h"
#include "NewMenu.h"
#include "DijalogOProgramu.h"
#include "RezultatiDijalog.h"

// CLinijeDlg dialog
class CLinijeDlg : public CDialog
{
	struct Slike { int rbroj; CPictureEx slika; Slike *sled; };

	struct Cvorovi
	{ 
		Cvor cvor; Cvorovi *sled;
		Cvorovi(int x, int y){ cvor.upisi_x(x); cvor.upisi_y(y); }
	};

	Slike *pok; 
	int vel; 

	CBitmap m_bitmap; 
	FooButton m_meni;
	
// Construction
public:
	CLinijeDlg(CWnd* pParent = NULL);	

// Dialog Data
	enum { IDD = IDD_LINIJE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Implementation
protected:
	HICON m_hIcon;
	CMidi muzika; 
	CWave zvuk;

	CNewLabel rezultat_ekran; int rezultat;
	CNewLabel crvene_ekran; int crvene;
	CNewLabel plave_ekran; int plave;
	CNewLabel zelene_ekran; int zelene;
	CNewLabel zute_ekran; int zute;
	CNewLabel roze_ekran; int roze;

	CTaskbarNotifier taskbar; 

	int kliknuto_x, kliknuto_y;
	Cvor **map;
	Cvorovi *lista_cvorova;
	int broj_slobodnih_cvorova; 
	bool prepreke; 
	bool pusti_muziku; 
	COtvorenaLista ponavljanja; 

	bool pronadji_ponavljanja(int *boja, int n);
	bool izracunava; 
	int broj_prepreka;
	int *boja; 

	CNewMenu menu;
	CNewMenu* pPopupMenu;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT KlinutoNaDugmic(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point); 
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	afx_msg void KliknutoNaX();
	afx_msg void KliknutoNaMin();
	afx_msg void OnSistemski();
	afx_msg void OnSledece();

	bool dodajNaEkran(int cvorova);
	bool daLiPostoji(int x, int y); 
	void toClipboard(CWnd * wnd, BOOL FullWnd); 
	void izbaci_ponavljanja(void); 
	void dodajPrepreke(int n); 
	void postavi_sledece(void); 
	void upisi_rezultat(int rez); 
	void brojFiguraNaEkranu(void); 
	void prikaziPopUpMeni();
	void resetujIgru(void); 
	bool sacuvajIgru();
	bool ucitajIgru(); 
	void izbaciPrepreke(void);
	void proveriRezultat(int rez); 
	
	CBitmapButton m_x;
	CBitmapButton m_min;
	CBitmapButton m_sistemski;
	CBitmapButton m_sledece;

	CPictureEx sledeca[3];
	CToolTipCtrl *tool_tip; 
	CRgn m_rgn; 
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	bool imaLiPonavljanja(void);
};
