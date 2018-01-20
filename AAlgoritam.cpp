#include "StdAfx.h"
#include ".\aalgoritam.h"

CAAlgoritam::CAAlgoritam(void)
{
	mapa = 0; velicina = 0;	put = 0; odrediste_x = 0; odrediste_y = 0;
}

CAAlgoritam::CAAlgoritam(int **m, int vel)
{
	put = 0; velicina = vel;
	mapa = new Cvor*[velicina];
	for(int i = 0; i < velicina; i++)
	{
		mapa[i] = new Cvor[velicina];
		for(int j = 0; j < velicina; j++)
			mapa[i][j] = Cvor(i,j,(m[i][j] ? neprohodan: prohodan));
	}
}

CAAlgoritam::CAAlgoritam(Cvor **c, int vel)
{
	put = 0; velicina = vel;
	mapa = new Cvor*[velicina];
	for(int i = 0; i < velicina; i++)
	{
		mapa[i] = new Cvor[velicina];
		for(int j = 0; j < velicina; j++)
			mapa[i][j] = c[i][j];
	}
}

CAAlgoritam::~CAAlgoritam(void)
{
	for(int i = 0; i < velicina; i++) delete [] mapa[i];
	delete [] mapa; mapa = 0; ~put; ~oLista; velicina = 0;
}

void CAAlgoritam::ispisi_mapu()const
{
	if (velicina)
		for(int i = 0; i < velicina; i++)
		{
			for(int j = 0; j < velicina; j++)	
				printf("%d ", mapa[i][j].uzmi_status()); 
			printf("\n");
		}
}

Cvor &CAAlgoritam::operator[](int br)const
{
	return put[br];
}

bool CAAlgoritam::pronadji_put(int py,
							   int px,
							   int oy,
							   int ox)
{
	if (velicina <= 0) return false;

	if(((px < 0) || (px > velicina)) || ((py < 0) || (py > velicina))) return false;
	if(((ox < 0) || (ox > velicina)) || ((oy < 0) || (oy > velicina))) return false;

	~oLista;

	~put;

	if (px == ox && py == oy) return true;

	if(((mapa[ox][oy]).uzmi_status()) == neprohodan) return false;

	mapa[px][py].upisi_listu(uOtvorenojListi); oLista += mapa[px][py];

	do
	{
		if(!!oLista)
		{
			Cvor roditelj = -oLista; roditelj.upisi_listu(uZatvorenojListi);
			mapa[roditelj.uzmi_x()][roditelj.uzmi_y()] = roditelj;
	
			for(int j = ((roditelj.uzmi_y())-1); j <= ((roditelj.uzmi_y())+1); j++)
			{
				for(int i = ((roditelj.uzmi_x())-1); i <= ((roditelj.uzmi_x())+1); i++)
				{
					if (j > -1 && i > -1 && j < velicina && i < velicina)
					{
						if((mapa[i][j].uzmi_listu()) != uZatvorenojListi)
						{
							if((mapa[i][j].uzmi_status()) == prohodan)
							{
								int cosak = prohodan;

								int rx = roditelj.uzmi_x();
								int ry = roditelj.uzmi_y();
							
								if(i == rx-1)
								{
									if(j == ry-1) 
									{
										if((((mapa[rx-1][ry].uzmi_status()) == neprohodan) || (((mapa[rx][ry-1].uzmi_status()) == neprohodan))))
											cosak = neprohodan;
									}
									else if (j == ry+1)
									{
										if((((mapa[rx][ry+1].uzmi_status()) == neprohodan) || (((mapa[rx-1][ry].uzmi_status()) == neprohodan))))
											cosak = neprohodan;
									}
								}
								else if (i == rx+1)
								{
									if(j == ry-1)
									{
										if((((mapa[rx][ry-1].uzmi_status()) == neprohodan) || (((mapa[rx+1][ry].uzmi_status()) == neprohodan))))
											cosak = neprohodan;
									}
									else if (j == ry+1)
									{
										if((((mapa[rx+1][ry].uzmi_status()) == neprohodan) || (((mapa[rx][ry+1].uzmi_status()) == neprohodan))))
											cosak = neprohodan;
									}
								} 
								
								if(cosak == prohodan) 
								{
									int g_vred, temp_g, h_vred;

									if(mapa[i][j].uzmi_listu() != uOtvorenojListi)
									{
										if( abs(i-rx) == 1 && abs(j-ry) == 1 ) 
											g_vred = 14;
										else g_vred = 10;

										temp_g = g_vred + roditelj.uzmi_G();

										mapa[i][j].upisi_G(temp_g);

										h_vred = 10 * ( abs(i - ox) + abs (j - oy));

										mapa[i][j].upisi_H(h_vred);
										mapa[i][j].upisi_F(temp_g + h_vred);
										mapa[i][j].upisi_roditelja(roditelj);
										
										mapa[i][j].upisi_listu(uOtvorenojListi);

										oLista += mapa[i][j]; 

									}
									else if((mapa[i][j].uzmi_listu()) == uOtvorenojListi) 
									{
										if ( abs(i - rx) == 1 && abs(j - ry) == 1 )
											g_vred = 14; 
										else g_vred = 10;
										
										temp_g = g_vred + roditelj.uzmi_G();

										if(temp_g < (mapa[i][j].uzmi_G()))
										{
											mapa[i][j].upisi_roditelja(roditelj);
											mapa[i][j].upisi_G(temp_g);
											mapa[i][j].upisi_F((mapa[i][j].uzmi_H()) + temp_g);
											
											int uListi = oLista.u_listi(mapa[i][j].uzmi_x(), mapa[i][j].uzmi_y());
											if (uListi > -1)
											{
												oLista -= mapa[i][j];
												oLista += mapa[i][j];
											}
										}
									}
								}
							} 
						}
					}
				}
			} 
		}
		else
		{
			postoji_put = nepostoji; break;
		}
		if(oLista.u_listi(ox, oy) > -1)
		{
			postoji_put = pronadjen; break;
		}
	}while (1); 

	~oLista;

	if(postoji_put == pronadjen)
	{
		int x_put = ox, y_put = oy; int b=0;
		do
		{
			mapa[x_put][y_put].upisi_F(0); 
			put += mapa[x_put][y_put];
			Cvor dete; dete = mapa[x_put][y_put];
			x_put = dete.uzmi_roditelja().x;
			y_put = dete.uzmi_roditelja().y; 
		}while((x_put != px) || (y_put != py));

		put += mapa[px][py];
		mapa[px][py].upisi_status(prohodan);
		mapa[ox][oy].upisi_status(neprohodan); 
		mapa[ox][oy].upisi_boju(mapa[px][py].uzmi_boju()); 
		mapa[px][py].upisi_boju(-1);
		odrediste_x = ox; odrediste_y = oy; 
											
		return true;
	}

	postoji_put = nepostoji;

	return false;
}