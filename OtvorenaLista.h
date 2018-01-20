#pragma once

#include "Cvor.h"

// Struktura koja sadrzi atribute liste
struct Elem
{ 
  int rbroj; Cvor cvor; Elem *sled;
  Elem(int b = 0, Elem *s = 0){ rbroj = b; sled = s; }
};

// Klasa koja sadrzi cvorove u otvorenoj listi
// Napomena: ne postoji klasa za zatvorenu listu,
// vec se samo u cvoru upise da je u zatvorenoj listi
class COtvorenaLista
{
	Elem *pok; // pokazivac na vrh liste
	int vel; // velicina liste
	void kopiraj(const COtvorenaLista &); // kopira ulaznu listu u tekucu
	void brisi(COtvorenaLista &); // brise ulaznu listu
public:
	COtvorenaLista(void);
	COtvorenaLista(int); // konstruktor pravi (int) clanova liste
	COtvorenaLista(const COtvorenaLista &); // konstruktor kopije
	~COtvorenaLista(); // destruktor

	COtvorenaLista &operator=(const COtvorenaLista &); // kopiranje liste sa desne strane u tekucu
	COtvorenaLista &operator+=(Cvor&); // dodavanje cvora na vrh liste sa najmanjom F vrednoscu
	COtvorenaLista &operator+=(const COtvorenaLista &); // dodavanje nove liste na tekucu
	COtvorenaLista &operator-=(Cvor); // brisanje date vrednosti iz liste
	COtvorenaLista &operator~() { brisi(*this); return *this; } // brise listu
	COtvorenaLista &operator++() { ++vel; return *this; } // povecava velicinu liste za 1

	Cvor &operator[](int)const; // pristup odredjenom clanu liste
	Cvor operator-(); // uzimanje cvora sa vrha liste
	int operator!()const { return vel==0; } // da li je lista prazna
	int operator()() { return vel; } // velicina liste

	int u_listi(int xx, int yy)const; // da li cvor(x,y) postoji u listi, -1 ne postoji

	friend COtvorenaLista operator+(const COtvorenaLista &, const COtvorenaLista &); // sabiranje dve liste
};