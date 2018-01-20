// Internet.h: interface for the CInternet class.
// Datum: 22.3.2003.
// Programer: Milan Milanovic (milan@milanovic.org)
// Klasa za pristup internetu za program MasterMind XP
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERNET_H__9D05E954_6272_4928_8135_999C28123ACF__INCLUDED_)
#define AFX_INTERNET_H__9D05E954_6272_4928_8135_999C28123ACF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

static const char *upis = _T("http://localhost/upis.php?ime=");
static const char *ispis = _T("http://localhost/ispisprg.php"); 
static const char *verzija_proveri = _T("http://milan.milanovic.org/linije/verzija.txt");

class CInternet  
{
public:
	CString* ProcitajSaWeba(bool&);
	bool UpisiNaWeb(CString,int);
	
	BOOL DaLiJeKonektovan (void); 

	CInternet();
	virtual ~CInternet();
	int procitajVerzijuSaInterneta(void);
};

#endif // !defined(AFX_INTERNET_H__9D05E954_6272_4928_8135_999C28123ACF__INCLUDED_)
