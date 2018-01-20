// Internet.cpp: implementation of the CInternet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Linije.h"
#include "Internet.h"
#include ".\internet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInternet::CInternet()
{

}

CInternet::~CInternet()
{

}

BOOL CInternet::DaLiJeKonektovan()
{
	int proveri = AfxSocketInit();
    CSocket server;
	HKEY kljuc;
    BOOL rezultat = true;
 
    if ( RegOpenKeyEx ( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_SET_VALUE, &kljuc) != ERROR_SUCCESS) { AfxMessageBox("Greska u registry bazi !"); }
 
    if ( !server.Create() ) rezultat = false;
	else if ( !server.Connect( "www.microsoft.com", 80 ) ) //  207.46.130.150  
	{         
        server.Close();
        rezultat = false;
	}
	 
	RegCloseKey( kljuc );
    return ( rezultat );
}

bool CInternet::UpisiNaWeb(CString Ime, int Rezultat)
{
	if( DaLiJeKonektovan( ) )
	{ 
		CInternetSession internet;
		CString URL;
		CString tempRez; tempRez.Format("%d",Rezultat);
		URL += upis; URL += Ime; URL += "&rezultat="; URL += tempRez;

		try
		{
			internet.OpenURL( URL );
		}
		catch ( CInternetException* m_pIzuzetak )
		{
			m_pIzuzetak->Delete();
			return FALSE;
		}
	}
	else return FALSE;
	return TRUE;
}

CString* CInternet::ProcitajSaWeba(bool& ok)
{
	CString *niz = new CString[20];
	
	if( DaLiJeKonektovan( ) )
	{ 
		CInternetSession internet;
		CInternetFile* fajl = NULL;

		try
		{
			fajl = (CInternetFile*) internet.OpenURL( ispis,1,INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_RELOAD );

		}
		catch ( CInternetException* m_pIzuzetak )
		{
			m_pIzuzetak->Delete();
			ok = FALSE; fajl = NULL; return NULL;
		}
		
		for(int i=0,j=0;i<20;i++) fajl->ReadString(niz[i]);

		fajl = NULL; ok = TRUE;
	} else { ok = FALSE; return NULL; }
	return niz;
}

int CInternet::procitajVerzijuSaInterneta(void)
{
	int verzija = 0;
	CString string;
	if( DaLiJeKonektovan( ) )
	{ 
		CInternetSession internet;
		CInternetFile* fajl = NULL;

		try
		{
			fajl = (CInternetFile*) internet.OpenURL( verzija_proveri,1,INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_RELOAD );

		}
		catch ( CInternetException* m_pIzuzetak )
		{
			m_pIzuzetak->Delete();
			fajl = NULL; return NULL;
		}
		
		fajl->ReadString(string);
		verzija = atoi((LPCSTR)string);
		fajl = NULL;
	} else return -1;
	return verzija;
}
