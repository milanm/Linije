// Scores.h: interface for the CScores class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCORES_H__9A23ABE7_6773_4DDA_B270_11775D9D0024__INCLUDED_)
#define AFX_SCORES_H__9A23ABE7_6773_4DDA_B270_11775D9D0024__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScores : public CObject  
{
public:
	int duz;
	int scores[10];
	CString names[10];
	CScores();
	CScores(int *, CString *, int);

	virtual ~CScores();
	friend CArchive& AFXAPI operator<<(CArchive &ar, const CScores &p);
	friend CArchive& AFXAPI operator>>(CArchive &ar, CScores &p);
};

#endif // !defined(AFX_SCORES_H__9A23ABE7_6773_4DDA_B270_11775D9D0024__INCLUDED_)
