#include "StdAfx.h"
#include ".\otvorenalista.h"

COtvorenaLista::COtvorenaLista(void)
{
	pok = 0; vel = 0;
}

void COtvorenaLista::kopiraj(const COtvorenaLista &s)
{
	if(pok || vel) { pok=0; vel=0; }
	Elem *tek = s.pok, *novi, *tek1;
	while(tek)
	{
		novi = new Elem(); novi->cvor = tek->cvor; novi->rbroj = tek->rbroj;
		if(!pok) { pok = novi; tek1 = pok; }
		else { tek1->sled = novi; tek1 = novi;} 
		tek = tek->sled;
	}
	novi = tek = tek1 = 0; vel = s.vel;
}

void COtvorenaLista::brisi(COtvorenaLista &s)
{
	Elem *stari = s.pok;
	while(s.pok){ s.pok = (s.pok)->sled; delete stari; stari = s.pok; }
	s.pok = stari = 0; s.vel = 0;
}

COtvorenaLista::COtvorenaLista(int b)
{
	vel = b; Elem *novi = 0; pok = 0;
	for(int i = 0; i < vel; i++)
	{
		novi = new Elem();
		if(!pok) pok = novi;
		else { novi->sled = pok; pok = novi; }
	}
	novi=0;
}

COtvorenaLista::COtvorenaLista(const COtvorenaLista &s){ pok=0; kopiraj(s); }

COtvorenaLista::~COtvorenaLista(){ if(pok) brisi(*this); pok = 0; }

COtvorenaLista &COtvorenaLista::operator=(const COtvorenaLista &s)
{
	if( &s != this) kopiraj(s);
	return *this;
}

COtvorenaLista &COtvorenaLista::operator+=(Cvor& br)
{
	Elem *novi = new Elem(); novi->rbroj = vel + 1; novi->cvor = br;
	Elem *stari = 0, *tek = pok;
	if(!pok) pok = novi;
	else 
	{
		while (tek)
		{
			if(novi->cvor.uzmi_F() <= tek->cvor.uzmi_F()) // odrzavaj po min(F)
			{
				novi->sled = tek; tek = novi;
				if(!stari) pok = novi;
				else stari->sled = novi;
				break;
			}
			stari = tek; tek=tek->sled;
			if(!tek) stari->sled = novi;
		}
	}
	++vel; novi = 0; stari = 0; tek = 0;
	return *this;
}

COtvorenaLista &COtvorenaLista::operator+=(const COtvorenaLista &s)
{
	*this = *this + s; return *this;
}

COtvorenaLista &COtvorenaLista::operator-=(Cvor br)
{
	COtvorenaLista temp;
	Elem *tek = pok;
	while(tek)
	{
		if((tek->cvor.uzmi_x() != br.uzmi_x()) || (tek->cvor.uzmi_y() != br.uzmi_y())) 
			temp += tek->cvor;
		tek = tek->sled;
	}
	brisi(*this); kopiraj(temp); tek = 0;
	return *this;
}

Cvor &COtvorenaLista::operator[](int br)const
{
	Elem *tek = pok; 
	for(int i=0; i < br; i++) tek = tek->sled;
	return (tek->cvor);
}

Cvor COtvorenaLista::operator-()
{
	if(vel > 0)
	{
		Elem *stari = pok; pok = pok->sled;
		Cvor cr = stari->cvor; delete stari; --vel;
		return cr;
	}
	else return Cvor();
}

COtvorenaLista operator+(const COtvorenaLista &s1, const COtvorenaLista &s2)
{
	COtvorenaLista temp(s1); Elem *tek;
	for(tek = s2.pok; tek; tek = tek->sled) { temp += tek->rbroj; temp += tek->cvor; }
	tek = 0; return temp;
}

int COtvorenaLista::u_listi(int xx, int yy)const
{
	if (vel != 0)
	{
		Elem *tek = pok; int i = 0;
		while(tek)
		{
			if(((tek->cvor.uzmi_x()) == xx) && ((tek->cvor.uzmi_y()) == yy))
				return i;
			else tek = tek->sled;
			i++;
		}
	}
	return -1;
}
