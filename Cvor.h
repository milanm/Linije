#include "resource.h"
#pragma once

#define prohodan 0 
#define neprohodan 1 
#define pronadjen 2 
#define nepostoji 3 
#define uZatvorenojListi 4 
#define uOtvorenojListi 5
#define nijeUListi 6

#define crvena 7
#define crna 8
#define zelena 9
#define plava 10
#define zuta 11

struct Tacke
{ 
	int x, y; 
	Tacke(int xx=-1, int yy=-1){x=xx; y=yy;}
};

class Cvor
{
	Tacke lokacija;
	int F,
		G,
		H;
	Tacke roditelj; 
	int status; 
	int uListi; i
	int boja; 
public:
	Cvor(void);
	Cvor(int xx, int yy);
	Cvor(int xx, int yy, int sta);
	Cvor(int xx, int yy, int sta, int bo);
	Cvor(int xx, int yy, int FF, int GG, int HH, int sta, int li, int bo);
	Cvor(const Cvor &); 
	~Cvor(void){}

	Cvor &operator=(const Cvor&); 

	void upisi_x(int xx) { lokacija.x = xx; }
	int uzmi_x(void)const { return lokacija.x; }

	void upisi_y(int yy) { lokacija.y = yy; }
	int uzmi_y(void)const { return lokacija.y; }

	void upisi_F(int FF) { F = FF; }
	int uzmi_F(void)const { return F; }

	void upisi_G(int GG) { G = GG; }
	int uzmi_G(void)const { return G; }

	void upisi_H(int HH) { H = HH; }
	int uzmi_H(void)const { return H; }

	void upisi_status(int sta) { status = sta; }
	int uzmi_status(void)const { return status; }

	void upisi_roditelja(Cvor &rod) { roditelj.x = rod.uzmi_x(); roditelj.y = rod.uzmi_y(); }
	Tacke uzmi_roditelja(void)const { return roditelj; }

	void upisi_listu(int li) { uListi = li; }
	int uzmi_listu(void)const { return uListi; }

	void upisi_boju(int bo) { boja = bo; }
	int uzmi_boju(void)const { return boja; }
};