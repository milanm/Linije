// Scores.cpp: implementation of the CScores class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Scores.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScores::CScores()
{
	duz = 10;
	for(int i=0;i<10;i++) { names[i] = "Prazno"; scores[i] = 0;}
}

CScores::CScores(int *rez, CString *imena, int n)
{
	duz = n;
	for(int i=0;i<duz;i++) { names[i] = imena[i]; scores[i] = rez[i]; }
}

CScores::~CScores()
{
	duz = 0;
}

CArchive& AFXAPI operator<<(CArchive &ar, const CScores &p)
{
	ar << p.duz;
	for(int i=0;i<p.duz;i++) { ar << p.names[i]; ar << p.scores[i]; }

	return ar;
}

CArchive& AFXAPI operator>>(CArchive &ar, CScores &p)
{
	ar >> p.duz;
	for(int i=0;i<p.duz;i++) { ar >> p.names[i]; ar >> p.scores[i]; }

	return ar;
}