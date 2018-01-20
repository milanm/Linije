#pragma once

#include <mmsystem.h>

class CMidi
{
public:
	CMidi(void);
	~CMidi(void);

	int DeviceCount( void );

	BOOL Open( const char * );
	BOOL Close( void );

	BOOL Play( void );
	BOOL Stop( void );
	BOOL Pause( void );
	BOOL IsPlaying( void );

	BOOL GetLength( int *, int * );
	int GetMinutes( void );
	int GetSeconds( void );
	void showError(DWORD dwError);

protected:
	BOOL m_bOpened, m_bPaused, m_bPlaying;
	WORD m_wDeviceID;
	int m_nDevices;
};
