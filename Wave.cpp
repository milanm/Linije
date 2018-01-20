#include "StdAfx.h"
#include ".\wave.h"

CWave::CWave()
{

	InitVars();

}

CWave::CWave( const char *lpszFilename )
{

	InitVars();
	Load( lpszFilename );

}

CWave::CWave( const char *lpszResID, HINSTANCE hInstance )
{

	InitVars();
	Load( lpszResID, hInstance );

}

CWave::CWave( int nResID, HINSTANCE hInstance )
{

	InitVars();
	Load( nResID, hInstance );

}

void CWave::InitVars( void )
{

	m_bLoaded = FALSE;
	m_lpSoundData = NULL;
	m_hResHandle = NULL;
	m_nDevices = waveOutGetNumDevs();

}

CWave::~CWave()
{

	Close();

}

int CWave::DeviceCount( void )
{

	return( m_nDevices );

}

BOOL CWave::IsLoaded( void )
{

	return( m_bLoaded );

}

BOOL CWave::Load( const char *lpszFilename )
{

	Close();

	CFile File;
	if( !File.Open( lpszFilename, CFile::modeRead ) )
		return( FALSE );

	DWORD dwFileLength = (DWORD)File.GetLength();
	m_lpSoundData = new char [dwFileLength];
	if( m_lpSoundData == NULL )
		return( FALSE );

	if( File.Read( m_lpSoundData, dwFileLength )
		!= dwFileLength )
		return( FALSE );

	m_bLoaded = TRUE;

	return( TRUE );

}

BOOL CWave::Load( const char *lpszResID, HINSTANCE hInstance )
{

	Close();

	HANDLE hResInfo;
	hResInfo = FindResource ( hInstance, lpszResID, "WAVE" );
	if( hResInfo == NULL )
		return( FALSE );

	m_hResHandle = LoadResource( hInstance, (HRSRC) hResInfo );
	if( m_hResHandle == NULL )
		return( FALSE );

	m_lpSoundData = (char *) LockResource( m_hResHandle );
	if( m_lpSoundData == NULL )
		return( FALSE );

	m_bLoaded = TRUE;

	return( TRUE );

}

BOOL CWave::Load( int nResID, HINSTANCE hInstance )
{

	Close();

	HANDLE hResInfo;
	hResInfo = FindResource ( hInstance,
		MAKEINTRESOURCE( nResID ), "WAVE" );
	if( hResInfo == NULL )
		return( FALSE );

	m_hResHandle = LoadResource( hInstance, (HRSRC) hResInfo );
	if( m_hResHandle == NULL )
		return( FALSE );

	m_lpSoundData = (char *) LockResource( m_hResHandle );
	if( m_lpSoundData == NULL )
		return( FALSE );

	m_bLoaded = TRUE;

	return( TRUE );

}

BOOL CWave::Play( BOOL bLoop )
{

	if( !m_bLoaded )
		return( FALSE );

	Stop();
	
	DWORD dwFlags = SND_MEMORY | SND_ASYNC | SND_NODEFAULT;
	if( bLoop )
		dwFlags |= SND_LOOP;

	return( PlaySound( m_lpSoundData, NULL, dwFlags ) );

}

BOOL CWave::PlayFromDisk( const char *lpszFilename )
{

	Stop();
	
	return( PlaySound( lpszFilename, NULL,
		SND_FILENAME | SND_SYNC | SND_NODEFAULT ) );

}

BOOL CWave::PlayFromRes( const char *lpszResID,
						HINSTANCE hInstance )
{

	return( PlaySound( lpszResID, hInstance,
		SND_RESOURCE | SND_SYNC | SND_NODEFAULT ) );

}

BOOL CWave::PlayFromRes( int nResID, HINSTANCE hInstance )
{

	return( PlaySound( MAKEINTRESOURCE( nResID ),
		hInstance, SND_RESOURCE | SND_SYNC | SND_NODEFAULT ) );

}

BOOL CWave::Stop( void )
{

	return( PlaySound( NULL, NULL, NULL ) );

}

BOOL CWave::Close( void )
{

	Stop();

	if( m_hResHandle  != NULL ){
		UnlockResource( m_hResHandle );
		FreeResource( m_hResHandle );
		}
	else if( m_lpSoundData != NULL )
		delete [] m_lpSoundData;

	m_hResHandle = NULL;
	m_lpSoundData = NULL;

	return( TRUE );

}