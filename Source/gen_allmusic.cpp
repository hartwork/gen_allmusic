//////////////////////////////////////////////////////////////////////////////// 
// Allmusic Hotkey Winamp Plugin
// 
// Copyright © 2006  Sebastian Pipping <webmaster@hartwork.org>
// 
// -->  http://www.hartwork.org
// 
// This source code is released under the GNU General Public License (GPL).
// See GPL.txt for details. Any non-GPL usage is strictly forbidden.
////////////////////////////////////////////////////////////////////////////////


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>
#include "Winamp/Gen.h"
#include "Winamp/wa_hotkeys.h" 
#include "Winamp/wa_ipc.h" 


#define PLUGIN_TITLE    "Allmusic Hotkey Winamp Plugin"
#define PLUGIN_VERSION  "1.4"

#define OPT1_ARTIST  1
#define OPT1_ALBUM   2
#define OPT1_SONG    3


int init();
void config(); 
void quit();



WNDPROC WndprocMainBackup = NULL;
LRESULT CALLBACK WndprocMain( HWND hwnd, UINT message, WPARAM wp, LPARAM lp );

int IPC_GEN_HOTKEYS_ADD;
int IPC_GEN_ALLMUSIC_ARTIST;
int IPC_GEN_ALLMUSIC_ALBUM;
int IPC_GEN_ALLMUSIC_SONG;

char * szLastFile = NULL;
char * szLastUrl = NULL;



const char * const szUrlFormat = "http://www.allmusic.com/cg/amg.dll?SQL=%s&OPT1=%i&Submit=Go&P=amg";



winampGeneralPurposePlugin plugin = {
	GPPHDR_VER,
	PLUGIN_TITLE " " PLUGIN_VERSION,
	init,
	config, 
	quit,
	NULL,
	NULL
}; 

HWND & hMain = plugin.hwndParent;



void BrowseAllmusic( int opt1, bool bForce )
{
	// Get current index
	const LRESULT iPos = SendMessage( hMain, WM_WA_IPC, 0, IPC_GETLISTPOS );

	
	// Get filename
	char * szFilename = ( char * )SendMessage( hMain, WM_WA_IPC, iPos, IPC_GETPLAYLISTFILE );
	if( !szFilename ) return;

	
	// Get artist
	char szUnescaped[ 1000 ] = "";
	extendedFileInfoStruct info;
	info.filename  = szFilename;
	
	switch( opt1 )
	{
	case OPT1_SONG:		info.metadata = "title";  break;
	case OPT1_ALBUM:	info.metadata = "album";  break;
	default:			info.metadata = "artist"; break;
	}
	
	info.ret       = szUnescaped;
	info.retlen    = sizeof( szUnescaped );
	const bool bExInfoSupported = ( SendMessage( hMain, WM_WA_IPC, ( WPARAM )&info, IPC_GET_EXTENDED_FILE_INFO ) == 1 );
	if( !bExInfoSupported ) return;


	// Make URL
	const int iLen = strlen( szUnescaped );
	for( int i = 0; i < iLen; i++ ) { if( szUnescaped[ i ] == ' ' ) { szUnescaped[ i ] = '|'; } }
	char * szFinalUrl = new char[ strlen( szUrlFormat ) + iLen ];
	wsprintf( szFinalUrl, szUrlFormat, szUnescaped, opt1 );
	
	// Update necessary?
	if( szLastUrl )
	{
		// Same URL?
		if( !strcmp( szFinalUrl, szLastUrl ) )
		{
			// Very same URL again
			if( !bForce )
			{
				delete [] szFinalUrl;
				return;
			}
		}
		else
		{
			// New URL
			const int iBytesToCopy = sizeof( char ) * ( strlen( szFinalUrl ) + 1 );
			/* if( szLastUrl ) */ free( szLastUrl );
			szLastUrl = ( char * )malloc( iBytesToCopy );
			memcpy( szLastUrl, szFinalUrl, iBytesToCopy );
		}
	}
	else
	{
		// First URL ever
		const int iBytesToCopy = sizeof( char ) * ( strlen( szFinalUrl ) + 1 );
		szLastUrl = ( char * )malloc( iBytesToCopy );
		memcpy( szLastUrl, szFinalUrl, iBytesToCopy );
	}


	// Show URL
	if( bForce ) SendMessage( hMain, WM_WA_IPC, 0, IPC_MBOPENREAL );
	SendMessage( hMain, WM_WA_IPC, ( WPARAM )szFinalUrl, IPC_MBOPENREAL );
	delete [] szFinalUrl;
}



LRESULT CALLBACK WndprocMain( HWND hwnd, UINT message, WPARAM wp, LPARAM lp )
{
	switch( message )
	{
	case WM_WA_IPC:
		switch( lp )
		{
		case IPC_CB_MISC:
			// Start of playback?
			if( wp != IPC_CB_MISC_STATUS ) break;
			if( SendMessage( hwnd, WM_WA_IPC, 0, IPC_ISPLAYING ) != 1 ) break;
			if( SendMessage( hwnd, WM_WA_IPC, 0, IPC_GETOUTPUTTIME ) >= 0 )
			{
				const char * const szFile = ( char * )SendMessage(
					hwnd,
					WM_WA_IPC,
					SendMessage( hwnd, WM_WA_IPC, 0, IPC_GETLISTPOS ),
					IPC_GETPLAYLISTFILE
				);
				
				if( szFile )
				{
					if( !szLastFile )
					{
						// First file ever
						const int iBytesToCopy = sizeof( char ) * ( strlen( szFile ) + 1 );
						szLastFile = ( char * )malloc( iBytesToCopy );
						memcpy( szLastFile, szFile, iBytesToCopy );

						BrowseAllmusic( OPT1_ARTIST, false );	
					}
					else if( strcmp( szLastFile, szFile ) )
					{
						// New file
						const int iBytesToCopy = sizeof( char ) * ( strlen( szFile ) + 1 );
						/* if( szLastFile ) */ free( szLastFile );
						szLastFile = ( char * )malloc( iBytesToCopy );
						memcpy( szLastFile, szFile, iBytesToCopy );

						BrowseAllmusic( OPT1_ARTIST, false );	
					}
				}
			}
			break;
				
		default:
			{
				if( lp == IPC_GEN_ALLMUSIC_ARTIST )
				{
					BrowseAllmusic( OPT1_ARTIST, true );
				}
				else if( lp == IPC_GEN_ALLMUSIC_ALBUM )
				{
					BrowseAllmusic( OPT1_ALBUM, true );
				}
				else if( lp == IPC_GEN_ALLMUSIC_SONG )
				{
					BrowseAllmusic( OPT1_SONG, true );
				}
			}
			break;
		
		}
	}
	return CallWindowProc( WndprocMainBackup, hwnd, message, wp, lp );
}



int init()
{
	// Get message IDs
	IPC_GEN_HOTKEYS_ADD      = SendMessage( hMain, WM_WA_IPC, ( WPARAM )"GenHotkeysAdd", IPC_REGISTER_WINAMP_IPCMESSAGE );
	IPC_GEN_ALLMUSIC_ARTIST  = SendMessage( hMain, WM_WA_IPC, ( WPARAM )"IPC_GEN_ALLMUSIC_ARTIST", IPC_REGISTER_WINAMP_IPCMESSAGE );
	IPC_GEN_ALLMUSIC_ALBUM   = SendMessage( hMain, WM_WA_IPC, ( WPARAM )"IPC_GEN_ALLMUSIC_ALBUM", IPC_REGISTER_WINAMP_IPCMESSAGE );
	IPC_GEN_ALLMUSIC_SONG    = SendMessage( hMain, WM_WA_IPC, ( WPARAM )"IPC_GEN_ALLMUSIC_SONG", IPC_REGISTER_WINAMP_IPCMESSAGE );


	// Add hotkey
	genHotkeysAddStruct hotkey;
	ZeroMemory( &hotkey, sizeof( genHotkeysAddStruct ) );
	hotkey.flags   = 0;
	hotkey.uMsg    = WM_WA_IPC;
	hotkey.wParam  = 0;
	hotkey.wnd     = 0;

	hotkey.name    = "Allmusic: Browse for Artist";
	hotkey.id      = "IPC_GEN_ALLMUSIC_ARTIST";
	hotkey.lParam  =  IPC_GEN_ALLMUSIC_ARTIST;
	SendMessage( hMain, WM_WA_IPC, ( WPARAM )&hotkey, IPC_GEN_HOTKEYS_ADD );

	hotkey.name    = "Allmusic: Browse for Album";
	hotkey.id      = "IPC_GEN_ALLMUSIC_ALBUM";
	hotkey.lParam  =  IPC_GEN_ALLMUSIC_ALBUM;
	SendMessage( hMain, WM_WA_IPC, ( WPARAM )&hotkey, IPC_GEN_HOTKEYS_ADD );

	hotkey.name    = "Allmusic: Browse for Song";
	hotkey.id      = "IPC_GEN_ALLMUSIC_SONG";
	hotkey.lParam  =  IPC_GEN_ALLMUSIC_SONG;
	SendMessage( hMain, WM_WA_IPC, ( WPARAM )&hotkey, IPC_GEN_HOTKEYS_ADD );


	// Deny Winamp to refresh the browser itself on track change.
	// Note: The initial page is shown nevertheless - no idea why.
	SendMessage( hMain, WM_WA_IPC, 1, IPC_MBBLOCK );


	// Exchange window procedure
	WndprocMainBackup = ( WNDPROC )GetWindowLong( hMain, GWL_WNDPROC );
	if( WndprocMainBackup != NULL )
	{
		SetWindowLong( hMain, GWL_WNDPROC, ( LONG )WndprocMain );
	}

	return 0;
} 



void config()
{
	MessageBox(
		NULL,
		PLUGIN_TITLE " " PLUGIN_VERSION "\n"
			"\n"
			"Copyright © 2006 Sebastian Pipping  \n"
			"<webmaster@hartwork.org>\n"
			"\n"
			"-->  http://www.hartwork.org",
		"About",
		MB_ICONINFORMATION
	);
}



void quit()
{
	if( szLastFile ) free( szLastFile );
	if( szLastUrl ) free( szLastUrl );
}



extern "C" __declspec( dllexport ) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin()
{
	return &plugin;
}
