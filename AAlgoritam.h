#pragma once

#include "OtvorenaLista.h"
#include "Cvor.h"

class CAAlgoritam
{
	Cvor **mapa; 
	int velicina;

	COtvorenaLista put; 
	int postoji_put; 

	COtvorenaLista oLista;

	int odrediste_x, odrediste_y; 
public:
	CAAlgoritam(void);
	CAAlgoritam(int **m, int vel);
	CAAlgoritam(Cvor **c, int vel);
	~CAAlgoritam(void);
    
	void ispisi_mapu()const; 
	Cvor **uzmi_mapu()const { return mapa; } 
	int uzmi_velicinu()const { return velicina; } 

	COtvorenaLista uzmi_put()const { return put; } 
	Cvor &operator[](int)const;
	int uzmi_duzinu() { return put(); }

	bool pronadji_put(int py, int px, int oy, int ox);
	int uzmi_postoji_put()const { return postoji_put; }
};

