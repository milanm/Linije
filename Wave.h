#pragma once

#include <mmsystem.h>

class CWave
{
public:
	CWave();
	CWave( const char * );
	CWave( const char *, HINSTANCE );
	CWave( int, HINSTANCE );
	~CWave();

	int DeviceCount( void );

	BOOL Load( const char * );
	BOOL Load( const char *, HINSTANCE );
	BOOL Load( int, HINSTANCE );

	BOOL Play( BOOL bLoop = FALSE );
	BOOL PlayFromDisk( const char * );
	BOOL PlayFromRes( const char *, HINSTANCE );
	BOOL PlayFromRes( int, HINSTANCE );
	BOOL Stop( void );
	BOOL Close( void );

	BOOL IsLoaded( void );

protected:
	void InitVars( void );

	int m_nDevices;
	BOOL m_bLoaded;
	char *m_lpSoundData;
	HANDLE m_hResHandle;

};
