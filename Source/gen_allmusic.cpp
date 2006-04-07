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
#include "Winamp/Gen.h"
#include "Winamp/wa_hotkeys.h" 
#include "Winamp/wa_ipc.h" 

#include "escape/escape.h"


#define PLUGIN_TITLE    "Allmusic Hotkey Winamp Plugin"
#define PLUGIN_VERSION  "1.0"



int init();
void config(); 
void quit();



WNDPROC WndprocMainBackup = NULL;
LRESULT CALLBACK WndprocMain( HWND hwnd, UINT message, WPARAM wp, LPARAM lp );

int IPC_GEN_HOTKEYS_ADD;
int IPC_GEN_ALLMUSIC_ARTIST;

const char * szUrlFormat = "http://www.allmusic.com/cg/amg.dll?SQL=%s&OPT1=1&Submit=Go&P=amg";



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



void BrowseArtist()
{
	// Get current index
	const LRESULT iPos = SendMessage( hMain, WM_WA_IPC, 0, IPC_GETLISTPOS );
	
	// Get filename
	char * szFilename = ( char * )SendMessage( hMain, WM_WA_IPC, iPos, IPC_GETPLAYLISTFILE );
	if( !szFilename ) return;
	
	// Get artist
	char szInitialArtist[ 1000 ] = "";
	extendedFileInfoStruct info;
	info.filename  = szFilename;
	info.metadata  = "artist";
	info.ret       = szInitialArtist;
	info.retlen    = sizeof( szInitialArtist );
	const bool bExInfoSupported = ( SendMessage( hMain, WM_WA_IPC, ( WPARAM )&info, IPC_GET_EXTENDED_FILE_INFO ) == 1 );
	if( !bExInfoSupported ) return;

	// Make URL
	char * szEscapedArtist = curl_escape( szInitialArtist, strlen( szInitialArtist ) );
	char * szFinalUrl = new char[ strlen( szUrlFormat ) + strlen( szEscapedArtist ) ];
	wsprintf( szFinalUrl, szUrlFormat, szEscapedArtist );
	curl_free( szEscapedArtist );

	// Show URL
	SendMessage( hMain, WM_WA_IPC, 0, IPC_MBOPENREAL );
	SendMessage( hMain, WM_WA_IPC, ( WPARAM )szFinalUrl, IPC_MBOPENREAL );
	delete [] szFinalUrl;
}



LRESULT CALLBACK WndprocMain( HWND hwnd, UINT message, WPARAM wp, LPARAM lp )
{
	switch( message )
	{
	case WM_WA_IPC:
		if( lp == IPC_GEN_ALLMUSIC_ARTIST )
		{
			BrowseArtist();
		}
		break;

	}
	return CallWindowProc( WndprocMainBackup, hwnd, message, wp, lp );
}



int init()
{
	// Get message IDs
	IPC_GEN_ALLMUSIC_ARTIST  = SendMessage( hMain, WM_WA_IPC, ( WPARAM )"IPC_GEN_ALLMUSIC_ARTIST", IPC_REGISTER_WINAMP_IPCMESSAGE );
	IPC_GEN_HOTKEYS_ADD      = SendMessage( hMain, WM_WA_IPC, ( WPARAM )"GenHotkeysAdd", IPC_REGISTER_WINAMP_IPCMESSAGE );


	// Add hotkey
	genHotkeysAddStruct hotkey;
	ZeroMemory( &hotkey, sizeof( genHotkeysAddStruct ) );
	hotkey.name    = "Allmusic: Browse for Artist";
	hotkey.flags   = 0;
	hotkey.uMsg    = WM_WA_IPC;
	hotkey.wParam  = 0;
	hotkey.lParam  = IPC_GEN_ALLMUSIC_ARTIST;
	hotkey.id      = "IPC_GEN_ALLMUSIC_ARTIST";
	hotkey.wnd     = 0;
	SendMessage( hMain, WM_WA_IPC, ( WPARAM )&hotkey, IPC_GEN_HOTKEYS_ADD );


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

}



extern "C" __declspec( dllexport ) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin()
{
	return &plugin;
}
