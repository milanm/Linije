#pragma once

#include "Cvor.h"

// CSacuvajIgru command target

class CSacuvajIgru : public CObject
{
public:
	int x[100];
	int y[100];
	int status[100];
	int boja[100];
	int rezultat;
	int velicina;
	CSacuvajIgru(Cvor **, int, int);
	CSacuvajIgru();
	virtual ~CSacuvajIgru();

	friend CArchive& AFXAPI operator<<(CArchive &ar, const CSacuvajIgru &p);
	friend CArchive& AFXAPI operator>>(CArchive &ar, CSacuvajIgru &p);
};


