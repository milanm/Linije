// ProveriInternet.cpp : implementation file
//

#include "stdafx.h"
#include "Linije.h"
#include "ProveriInternet.h"
#include ".\proveriinternet.h"
#include "Internet.h"

// CProveriInternet dialog

IMPLEMENT_DYNAMIC(CProveriInternet, CDialog)
CProveriInternet::CProveriInternet(CWnd* pParent /*=NULL*/)
	: CDialog(CProveriInternet::IDD, pParent)
{
}

CProveriInternet::~CProveriInternet()
{
}

void CProveriInternet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC12, m_verzija);
}


BEGIN_MESSAGE_MAP(CProveriInternet, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnProveri)
END_MESSAGE_MAP()


// CProveriInternet message handlers

void CProveriInternet::OnProveri()
{
	BeginWaitCursor();
	CInternet internet; int procitaj = internet.procitajVerzijuSaInterneta();
	EndWaitCursor();

	if(procitaj== -1) { MessageBox("Racunar nije spojen na Internet","Linije 1.0"); return; }
	else if(!procitaj) { MessageBox("Greska u citanju!","Linije 1.0"); return; }

	if(procitaj > build_verzija) MessageBox("Postoji novija verzija!");
	else MessageBox("Ne postoji novija verzija!");

	return;
}

BOOL CProveriInternet::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString ver; ver.Format("%d",build_verzija);
	m_verzija.SetWindowText(ver);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
