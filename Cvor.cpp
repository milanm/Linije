#include "StdAfx.h"
#include ".\cvor.h"
#include "resource.h"

Cvor::Cvor(void)
{
	lokacija.x = -1; lokacija.y = -1; roditelj.x = -1; roditelj.y = -1; F = 0; G = 0; H = 0; boja = -1; status = prohodan; uListi = nijeUListi;
}

Cvor::Cvor(int xx, int yy)
{
	lokacija.x = xx; lokacija.y = yy; roditelj.x = -1; roditelj.y = -1; F = 0; G = 0; H = 0; boja = -1; status = prohodan; uListi = nijeUListi;
}

Cvor::Cvor(int xx, int yy, int sta)
{
	lokacija.x = xx; lokacija.y = yy; roditelj.x = -1; roditelj.y = -1; F = 0; G = 0; H = 0; boja = -1; status = sta; uListi = nijeUListi;
}

Cvor::Cvor(int xx, int yy, int sta, int bo)
{
	lokacija.x = xx; lokacija.y = yy; boja = bo; roditelj.x = -1; roditelj.y = -1; F = 0; G = 0; H = 0; status = sta; uListi = nijeUListi;
}

Cvor::Cvor(int xx, int yy, int FF, int GG, int HH, int sta, int li, int bo)
{
	lokacija.x = xx; lokacija.y = yy; roditelj.x = -1; boja = bo; roditelj.y = -1; F = FF; G = GG; H = HH; status = sta; uListi = li;
}

Cvor::Cvor(const Cvor &cv)
{
	lokacija.x = cv.uzmi_x(); lokacija.y = cv.uzmi_y(); F = cv.uzmi_F(); G = cv.uzmi_G(); 
	status = cv.uzmi_status(); boja = cv.uzmi_boju(); roditelj.x = (cv.uzmi_roditelja()).x; 
	roditelj.y = (cv.uzmi_roditelja()).y; uListi = cv.uzmi_listu();
}

Cvor &Cvor::operator=(const Cvor&cv) 
{
	if(this!=&cv) 
	{
		lokacija.x = cv.uzmi_x(); lokacija.y = cv.uzmi_y(); F = cv.uzmi_F(); G = cv.uzmi_G(); 
		status = cv.uzmi_status(); boja = cv.uzmi_boju(); roditelj.x = (cv.uzmi_roditelja()).x; 
		roditelj.y = (cv.uzmi_roditelja()).y; uListi = cv.uzmi_listu();
	}
	return *this;
}