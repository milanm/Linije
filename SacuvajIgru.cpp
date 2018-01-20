// SacuvajIgru.cpp : implementation file
//

#include "stdafx.h"
#include "Linije.h"
#include "SacuvajIgru.h"
#include <iostream>

// CSacuvajIgru

CSacuvajIgru::CSacuvajIgru()
{
	for(int i=0; i<100; i++)
	{
			x[i] = y[i] = status[i] = boja[i] = -1;
	}
	rezultat = velicina = 0;
}

CSacuvajIgru::~CSacuvajIgru()
{
	velicina = 0; rezultat = 0;
}


// CSacuvajIgru member functions
CSacuvajIgru::CSacuvajIgru(Cvor** cv, int vel, int rez)
{
	velicina = vel; rezultat = rez;
	for(int i = 0, brojac = 0; i < velicina; i++)
		for(int j = 0; j < velicina; j++, brojac++)
		{
			x[brojac] = cv[i][j].uzmi_x(); 
			y[brojac] = cv[i][j].uzmi_y();
			status[brojac] = cv[i][j].uzmi_status();
			boja[brojac] = cv[i][j].uzmi_boju();
		}
}

CArchive& AFXAPI operator<<(CArchive &ar, const CSacuvajIgru &p)
{
	ar << p.rezultat; ar << p.velicina;
	for(int i = 0; i < (p.velicina*p.velicina); i++)
	{
		ar << p.x[i]; ar << p.y[i]; ar << p.status[i]; ar << p.boja[i];
	}
	return ar;
}

CArchive& AFXAPI operator>>(CArchive &ar, CSacuvajIgru &p)
{
	ar >> p.rezultat; ar >> p.velicina;
	for(int i = 0; i < (p.velicina*p.velicina); i++)
	{
		ar >> p.x[i]; ar >> p.y[i]; ar >> p.status[i]; ar >> p.boja[i];
	}
	return ar;
}