/*
                                                                                                            
  **    ***    ***            ***   ****                                   *               ***              
  **    ***    ***            ***  *****                                 ***               ***              
  ***  *****  ***             ***  ***                                   ***                                
  ***  *****  ***    *****    *** ******   *****    *** ****    ******  ******    *****    ***  *** ****    
  ***  *****  ***   *******   *** ******  *******   *********  ***  *** ******   *******   ***  *********   
  ***  ** **  ***  **** ****  ***  ***   ***   ***  ***   ***  ***       ***    ***   ***  ***  ***   ***   
  *** *** *** ***  ***   ***  ***  ***   *********  ***   ***  ******    ***    *********  ***  ***   ***   
   ****** ******   ***   ***  ***  ***   *********  ***   ***   ******   ***    *********  ***  ***   ***   
   *****   *****   ***   ***  ***  ***   ***        ***   ***    ******  ***    ***        ***  ***   ***   
   *****   *****   **** ****  ***  ***   ****  ***  ***   ***       ***  ***    ****  ***  ***  ***   ***   
   ****     ****    *******   ***  ***    *******   ***   ***  ***  ***  *****   *******   ***  ***   ***   
    ***     ***      *****    ***  ***     *****    ***   ***   ******    ****    *****    ***  ***   ***   
                                                                                                            
            ******** **                 ******                        *  **  **                             
            ******** **                 ******                       **  **  **                             
               **    **                 **                           **  **                                 
               **    ** **    ***       **      ** *  ****   ** **  **** **  **  ** **    ***               
               **    ******  *****      *****   ****  ****   ****** **** **  **  ******  *****              
               **    **  **  ** **      *****   **   **  **  **  **  **  **  **  **  **  ** **              
               **    **  **  *****      **      **   **  **  **  **  **  **  **  **  **  *****              
               **    **  **  **         **      **   **  **  **  **  **  **  **  **  **  **                 
               **    **  **  ** **      **      **   **  **  **  **  **  **  **  **  **  ** **              
               **    **  **  *****      **      **    ****   **  **  *** **  **  **  **  *****              
               **    **  **   ***       **      **    ****   **  **  *** **  **  **  **   ***               

*/


// BASS Simple Console Test, copyright (c) 1999-2004 Ian Luck.
#include "../game/q_shared.h"
#include "../game/bg_public.h"
#ifdef __MUSIC_ENGINE__
#include <windows.h>
#include <process.h>
#include <mmsystem.h>
#include <stdio.h>
#include <conio.h>

//#include "bass.h"
extern void			*BG_Alloc ( int size );
extern void			*BG_DebugAlloc ( int size, char *procedure );
#ifdef CGAMEDLL
extern void QDECL	CG_Printf ( const char *msg, ... );		// for CGame messages...
#else
extern void QDECL	Com_Printf ( const char *msg, ... );	// for UI messages...
#endif
extern float		CG_GetMusicVolume ( void );				// cg_main.c
extern int			trap_FS_GetFileList ( const char *path, const char *extension, char *listbuf, int bufsize );
extern int			trap_FS_FOpenFile ( const char *qpath, fileHandle_t *f, fsMode_t mode );
extern void			trap_FS_Read ( void *buffer, int len, fileHandle_t f );
extern void			trap_FS_FCloseFile ( fileHandle_t f );
extern int			Q_irand ( int value1, int value2 );
extern void			BG_Free ( void *ptr );
extern void			trap_Cvar_Register ( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags );
vmCvar_t			fs_game;
#define MAX_SOUNDPATH	512
#define BASSDEF( f )	( WINAPI * f )						// define the functions as pointers
#include "bass.h"
char				tempfile[MAX_PATH];						// temporary BASS.DLL
HINSTANCE			bass = 0;								// bass handle


/* load BASS and the required functions */
qboolean
LoadBASS ( void )
{
	BYTE	*data;
	HANDLE	hres, hfile;
	DWORD	len, c;
	char	temppath[MAX_PATH];

	/* get the BASS.DLL resource */

	if
	(
#ifdef CGAMEDLL
		!(hres = FindResource( GetModuleHandle( "cgame_mp_x86.dll"), "BASS_DLL", RT_RCDATA)) ||
		!(len = SizeofResource( GetModuleHandle( "cgame_mp_x86.dll"), hres)) ||
		!(hres = LoadResource( GetModuleHandle( "cgame_mp_x86.dll"), hres)) ||
#else
		!(hres = FindResource( GetModuleHandle( "ui_mp_x86.dll"), "BASS_DLL", RT_RCDATA)) ||
		!(len = SizeofResource( GetModuleHandle( "ui_mp_x86.dll"), hres)) ||
		!(hres = LoadResource( GetModuleHandle( "ui_mp_x86.dll"), hres)) ||
#endif
		!(data = LockResource( hres))
	)
	{
#ifndef CGAMEDLL
		Com_Printf( "Error: Can't get the BASS.DLL resource\n" );
#else
		CG_Printf( "Error: Can't get the BASS.DLL resource\n" );
#endif
		return ( qfalse );
	}

	/* get a temporary filename */
	GetTempPath( MAX_PATH, temppath );
	GetTempFileName( temppath, "bas", 0, tempfile );

	/* write BASS.DLL to the temporary file */
	if
	(
		INVALID_HANDLE_VALUE ==
			(hfile = CreateFile( tempfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL))
	)
	{
#ifndef CGAMEDLL
		Com_Printf( "Error: Can't write BASS.DLL\n" );
#else
		CG_Printf( "Error: Can't write BASS.DLL\n" );
#endif
		return ( qfalse );
	}

	WriteFile( hfile, data, len, &c, NULL );
	CloseHandle( hfile );

	/* load the temporary BASS.DLL library */
	if ( !(bass = LoadLibrary( tempfile)) )
	{
#ifndef CGAMEDLL
		Com_Printf( "Error: Can't load BASS.DLL\n" );
#else
		CG_Printf( "Error: Can't load BASS.DLL\n" );
#endif
		return ( qfalse );
	}

/* "load" all the BASS functions that are to be used */
#define LOADBASSFUNCTION( f )	*( (void **) &f ) = GetProcAddress( bass, #f )
	LOADBASSFUNCTION( BASS_ErrorGetCode );
	LOADBASSFUNCTION( BASS_Init );
	LOADBASSFUNCTION( BASS_Free );
	LOADBASSFUNCTION( BASS_GetCPU );
	LOADBASSFUNCTION( BASS_MusicLoad );
	LOADBASSFUNCTION( BASS_MusicGetLength );
	LOADBASSFUNCTION( BASS_StreamCreateFile );
	LOADBASSFUNCTION( BASS_StreamCreateURL );
	LOADBASSFUNCTION( BASS_StreamGetLength );
	LOADBASSFUNCTION( BASS_StreamGetFilePosition );
	LOADBASSFUNCTION( BASS_ChannelPlay );
	LOADBASSFUNCTION( BASS_ChannelBytes2Seconds );
	LOADBASSFUNCTION( BASS_ChannelIsActive );
	LOADBASSFUNCTION( BASS_ChannelSlideAttributes );
	LOADBASSFUNCTION( BASS_ChannelIsSliding );
	LOADBASSFUNCTION( BASS_ChannelGetPosition );
	LOADBASSFUNCTION( BASS_ChannelGetLevel );
	LOADBASSFUNCTION( BASS_ChannelSetSync );
	LOADBASSFUNCTION( BASS_GetVersion );
	LOADBASSFUNCTION( BASS_ErrorGetCode );
	LOADBASSFUNCTION( BASS_MusicLoad );
	LOADBASSFUNCTION( BASS_MusicGetAttribute );
	LOADBASSFUNCTION( BASS_MusicGetLength );
	LOADBASSFUNCTION( BASS_ChannelBytes2Seconds );
	LOADBASSFUNCTION( BASS_ChannelSetSync );
	LOADBASSFUNCTION( BASS_ChannelPlay );
	LOADBASSFUNCTION( BASS_ChannelSetAttributes );
	LOADBASSFUNCTION( BASS_GetDeviceDescription );
	LOADBASSFUNCTION( BASS_StreamFree );
	LOADBASSFUNCTION( BASS_ChannelIsActive );
	LOADBASSFUNCTION( BASS_ChannelGetLevel );
	LOADBASSFUNCTION( BASS_ChannelGetPosition );
	return ( qtrue );
}

static qboolean BASS_DLL_READY = qfalse;


/* */
qboolean
CheckBASSLoaded ( void )
{
	if ( BASS_DLL_READY == qfalse )
	{
		BASS_DLL_READY = LoadBASS();
	}

	return ( BASS_DLL_READY );
}


/* free the BASS library from memory and delete the temporary file */
void
FreeBASS ( void )
{
	if ( !bass )
	{
		return;
	}

	FreeLibrary( bass );
	bass = 0;
	DeleteFile( tempfile );
}

typedef struct soundengine_MyMusic_s	// Sounds Table Structure
{
	char	musicFile[MAX_SOUNDPATH];	// Sound name.
} soundengine_MyMusic_t;				// MyMusic Table Typedef
soundengine_MyMusic_t	MyMusic[MAX_SOUNDPATH];
int						MyMusicTotal = 0;
qboolean				MyMusicLoaded = qfalse;


// display error messages
static void
Error ( char *text )
{
#ifndef CGAMEDLL
	Com_Printf( "Error(%d): %s\n", BASS_ErrorGetCode(), text );
#else
	CG_Printf( "Error(%d): %s\n", BASS_ErrorGetCode(), text );
#endif
	BASS_Free();

	//ExitProcess(0);
}

static DWORD	starttime;


// looping synchronizer, resets the clock
static void CALLBACK
LoopSync ( HSYNC handle, DWORD channel, DWORD data, DWORD user )
{
	starttime = timeGetTime();
}

static DWORD	chan, act, music_time, level;
static BOOL		ismod;
static QWORD	pos;
static QWORD	length;
static int		a;
#ifndef CGAMEDLL


/* */
void QDECL
CG_Error ( const char *msg, ... )
{
	Com_Printf( "%s\n", msg );
}

#else
extern void QDECL	CG_Error ( const char *msg, ... );
#endif
extern void			trap_Cvar_Set ( const char *var_name, const char *value );


// update stream title from metadata
void
DoMeta ( char *meta )
{
	char	*p;

	//	CG_Printf("Meta: %s.\n", meta);
	if ( meta && (p = strstr( meta, "StreamTitle='")) )
	{
		char	name[512];
		p = strdup( p + 13 );
		strchr( p, ';' )[-1] = 0;
		strcpy( name, p );
		trap_Cvar_Set( "s_radioStationInfo3", va( "%s", name) );
		free( p );
	}
}


/* */
void CALLBACK
MetaSync ( HSYNC handle, DWORD channel, DWORD data, DWORD user )
{
	DoMeta( (char *) data );
}


/* */
void CALLBACK
StatusProc ( void *buffer, DWORD length, DWORD user )
{

	//	if (buffer && !length)
	//		MESS(32,WM_SETTEXT,0,buffer); // display connection status
} char			radio_station[255];
DWORD			cthread = 0;
qboolean		Playing_Radio = qfalse;
qboolean		Already_Playing = qfalse;
extern vmCvar_t s_radioStation;
extern vmCvar_t s_radioStationOverride;
extern vmCvar_t s_radioStationInfo1;
extern vmCvar_t s_radioStationInfo2;
extern vmCvar_t s_radioStationInfo3;
extern vmCvar_t s_inetRadio;
#pragma warning( disable : 4028 )
#pragma warning( disable : 4024 )


/* */
void __cdecl
OpenURL ( DWORD index )
{
	BASS_StreamFree( chan );	// close old stream
	if ( !(chan = BASS_StreamCreateURL( radio_station, 0, BASS_STREAM_META | BASS_STREAM_STATUS, StatusProc, 0)) )
	{

		//		Error("Can't play the stream");
	}
	else
	{
		while ( 1 )
		{						// pre-buffer...
			//char text[20];
			DWORD	progress, len = BASS_StreamGetFilePosition( chan, BASS_FILEPOS_END );
			if ( len == -1 )
			{
				goto done;		// something's gone wrong! (eg. BASS_Free called)
			}

			progress =
				(
					BASS_StreamGetFilePosition( chan, BASS_FILEPOS_DOWNLOAD) -
					BASS_StreamGetFilePosition( chan, BASS_FILEPOS_DECODE)
				) *
				100 /
				len;			// percentage of buffer filled

			//			CG_Printf("ICY: %s\n", BASS_StreamGetTags(chan,BASS_TAG_ICY));

			/*			{
				CG_Printf("ICY: %s\n", BASS_StreamGetTags(chan,BASS_TAG_ICY));
				char *icy=BASS_StreamGetTags(chan,BASS_TAG_ICY);
				if (icy)
				{
					CG_Printf("ICY: %s\n", icy);

					for (;*icy;icy+=strlen(icy)+1) {
						if (!memcmp(icy,"icy-name:",9))
							//MESS(31,WM_SETTEXT,0,icy+9);
							trap_Cvar_Set( "s_radioStationInfo1", va( "%s", icy+9 ) );
						if (!memcmp(icy,"icy-br:",7)) {
							char br[30]="bitrate: ";
							//strcat(br,icy+7);
							//MESS(32,WM_SETTEXT,0,br);
							trap_Cvar_Set( "s_radioStationInfo2", va( "%s", icy+7 ) );
						}
					}
				}
			}*/
			if ( progress > 75 )
			{
				break;			// over 75% full, enough
			}

			//sprintf(text,"buffering... %d%%",progress);
			//			MESS(31,WM_SETTEXT,0,text);
			Sleep( 50 );
		}

		// get the stream title and set sync for subsequent titles
		//DoMeta(BASS_StreamGetTags(chan,BASS_TAG_META));
		BASS_ChannelSetSync( chan, BASS_SYNC_META, 0, &MetaSync, 0 );

		// play it!
		BASS_ChannelPlay( chan, FALSE );
	}

done:
	cthread = 0;
	Already_Playing = qfalse;

	/* _endthread given to terminate */
	_endthread();
}


/* */
void
CloseURL ( void )
{
	cthread = 0;
	Already_Playing = qfalse;
}

void		CG_StartMusicTrackFile ( int argc, char *music, qboolean loop );
extern void *BG_Alloc ( int size );
extern void BG_Free ( void *ptr );


/* */
void
CG_StartMusicTrack ( int argc, char *music, qboolean loop )
{
	if ( Playing_Radio )
	{
		strcpy( radio_station, music );

		// open URL in a new thread (so that main thread is free)
		cthread = _beginthread( OpenURL, 0, LOWORD( 0) );
	}
	else
	{
		CG_StartMusicTrackFile( argc, music, loop );
	}
}


/* */
void
CG_StartMusicTrackFile ( int argc, char *music, qboolean loop)
{
	if ( !CheckBASSLoaded() )
	{
		return;
	}

	// check that BASS 2.1 was loaded
	if ( BASS_GetVersion() != MAKELONG( 2, 1) )
	{
#ifndef CGAMEDLL
		Com_Printf( "BASS version 2.1 was not loaded\n" );
#else
		CG_Printf( "BASS version 2.1 was not loaded\n" );
#endif
		return;
	}

	if ( argc != 2 )
	{

		//CG_Printf("\tusage: contest <file>\n");
		return;
	}

	//BASS_SetConfig(BASS_CONFIG_NET_PREBUF,0); // minimize automatic pre-buffering, so we can do it (and display it)
	// try streaming the file/url
	if
	(
	(chan = BASS_StreamCreateFile( FALSE, music, 0, 0, loop ? BASS_SAMPLE_LOOP : 0 )) ||
		(chan = BASS_StreamCreateURL( music, 0, loop ? BASS_STREAM_META | BASS_STREAM_STATUS | BASS_SAMPLE_LOOP : BASS_STREAM_META | BASS_STREAM_STATUS , NULL, 0))
	)
	{
		pos = BASS_StreamGetLength( chan );
		if ( BASS_StreamGetFilePosition( chan, BASS_FILEPOS_DOWNLOAD) != -1 )
		{

			// streaming from the internet

			/*if (pos)
				CG_Printf("streaming internet file [%I64d bytes]",pos);
			else
				CG_Printf("streaming internet file");*/
			while ( 1 )
			{												// pre-buffer...
				DWORD	progress, len = BASS_StreamGetFilePosition( chan, BASS_FILEPOS_END );
				if ( len == -1 )
				{
					CG_Error( "Can't buffer!\n" );			//return;//goto done; // something's gone wrong! (eg. BASS_Free called)
				}

				progress =
					(
						BASS_StreamGetFilePosition( chan, BASS_FILEPOS_DOWNLOAD) -
						BASS_StreamGetFilePosition( chan, BASS_FILEPOS_DECODE)
					) *
					100 /
					len;									// percentage of buffer filled
				Sleep( 50 );
			}
			{												// get the broadcast name and bitrate
				//char *icy=BASS_StreamGetTags(chan,BASS_TAG_ICY);
				// get the stream title and set sync for subsequent titles
				BASS_StreamGetTags( chan, BASS_TAG_META );
				BASS_ChannelSetSync( chan, BASS_SYNC_META, 0, &MetaSync, 0 );

				// play it!
				BASS_ChannelPlay( chan, FALSE );
			}
		}

		//else
		//CG_Printf("streaming file [%I64d bytes]",pos);
		ismod = FALSE;
	}
	else
	{

		// try loading the MOD (with looping, sensitive ramping, and calculate the duration)
		if
		(
			!(
				chan = BASS_MusicLoad( FALSE, music, 0, 0, loop ? BASS_MUSIC_RAMPS | BASS_MUSIC_CALCLEN | BASS_SAMPLE_LOOP :  BASS_MUSIC_RAMPS | BASS_MUSIC_CALCLEN, 0)
			)
		)
		{													// not a MOD either
			Error( "Can't play the file" );
		}

		// count channels
		for ( a = 0; BASS_MusicGetAttribute( chan, BASS_MUSIC_ATTRIB_VOL_CHAN + a) != -1; a++ );

		//CG_Printf("playing MOD music \"%s\" [%d chans, %d orders]",BASS_MusicGetName(chan),a,BASS_MusicGetLength(chan,FALSE));
		pos = BASS_MusicGetLength( chan, TRUE );
		ismod = TRUE;
	}

	// display the music_time length
	if ( pos )
	{
		music_time = ( DWORD ) BASS_ChannelBytes2Seconds( chan, pos );

		//CG_Printf(" %d:%02d\n",music_time/60,music_time%60);
	}

	//else // no music_time length available
	//CG_Printf("\n");
	// set a synchronizer for when it reaches the end, and start playback
	BASS_ChannelSetSync( chan, BASS_SYNC_END, 0, &LoopSync, 0 );
	BASS_ChannelPlay( chan, FALSE );
	starttime = timeGetTime();
	{
		int volume = CG_GetMusicVolume() * 100;
		BASS_ChannelSetAttributes( chan, -1 /*leave crrent*/, volume, -101 /*leave crrent*/ );
	}
}

// Kamikazee: What's this doing here?
/*#ifdef __SUPREMACY__
#define GAME_VERSION	"ET Enhanced"
#else //!__SUPREMACY__
#define GAME_VERSION	"AIMod-ET"
#endif //__SUPREMACY__*/

qboolean
CG_CheckMyMusic ( void ){
	fileHandle_t	f;
	char	*loadPath;
	int				len;

	trap_Cvar_Register( &fs_game, "fs_game", "", CVAR_SERVERINFO | CVAR_ROM );
	loadPath = va( "%s/MyMusic/MyMusic.list", fs_game.string );
	len = trap_FS_FOpenFile( loadPath, &f, FS_READ );

	if ( !f )
	{
		return qfalse;
	}

	// empty file?
	if ( len <= 0 )
	{
		return qfalse;
	}

	trap_FS_FCloseFile( f );

	return qtrue;
}

/* */
void
CG_LoadMyMusic ( void )
{				// Load music playlist from external file.
	char	*s, *t;
	int		len = 0;
	FILE	*f;
	char	*buf, *buf2;
	char	*loadPath;
	int		num = 0;
	trap_Cvar_Register( &fs_game, "fs_game", "", CVAR_SERVERINFO | CVAR_ROM );
#ifdef __FULL_VERSION__
	loadPath = va( "etmain/MyMusic/MyMusic.list\0" );
#else //!__FULL_VERSION__
	loadPath = va( "%s/MyMusic/MyMusic.list\0", fs_game.string );
#endif //__FULL_VERSION__
	f = fopen( va( "%s", loadPath), "r" );
	if ( !f )
	{
		return;
	}

	buf2 = (char *) BG_DebugAlloc( 1 + 1, "CG_LoadMyMusic" );

	// Calculate the length of this data file...
	while ( fread( buf2, 1, 1, f) )
	{
		len++;
	}

	fclose( f );
	if ( !len )
	{			//empty file
		BG_Free( buf2 );
		return;
	}

	// OK, we have a length value now... Re-Open the file and read into the main buffer...
	f = fopen( va( "%s", loadPath), "r" );
	if ( (buf = (char *) BG_DebugAlloc( len + 1, "CG_LoadMyMusic")) == 0 )
	{			//alloc memory for buffer
		BG_Free( buf2 );
		BG_Free( buf );
		return;
	}

	fread( buf, len, 1, f );
	fclose( f );
#ifndef CGAMEDLL
	Com_Printf( "^4*** ^3%s^4: ^5Loading MyMusic playlist from file ^7%s^5.\n", GAME_VERSION, loadPath );
#else
	CG_Printf( "^4*** ^3%s^4: ^5Loading MyMusic playlist from file ^7%s^5.\n", GAME_VERSION, loadPath );
#endif
	for ( t = s = buf; *t; /* */ )
	{
		num++;
		s = strchr( s, '\n' );
		if ( !s || num > len )
		{
			break;
		}

		while ( *s == '\n' )
		{
			*s++ = 0;
		}

		if ( *t )
		{
			if ( !Q_strncmp( "//", va( "%s", t), 2) == 0 && strlen( va( "%s", t)) > 0 )
			{	// Not a comment either... Record it in our list...
#ifdef __FULL_VERSION__
				strcpy( MyMusic[MyMusicTotal].musicFile, va( "etmain/MyMusic/%s", t) );
#else //!__FULL_VERSION__
				strcpy( MyMusic[MyMusicTotal].musicFile, va( "%s/MyMusic/%s", fs_game.string, t) );
#endif //__FULL_VERSION__
				MyMusicTotal++;
			}
		}

		t = s;
	}

	MyMusicLoaded = qtrue;
#ifndef CGAMEDLL
	Com_Printf( "^4*** ^3%s^4: ^5Loaded ^7%i^5 MyMusic playlist tracks.\n", GAME_VERSION, MyMusicTotal );
#else
	CG_Printf( "^4*** ^3%s^4: ^5Loaded ^7%i^5 MyMusic playlist tracks.\n", GAME_VERSION, MyMusicTotal );
#endif

	// Remove the extra MyMusicTotal++ from above...
	MyMusicTotal--;
	BG_Free( buf );
	BG_Free( buf2 );
}

#define MAX_STATION_TEXT	8192


/* */
void
CG_LoadInternetMusic ( void )
{							// Load internet music sites from external file.
	char			*s, *t;
	int				len = 0;
	fileHandle_t	f = 0;
	char			*buf;	//, *buf2;

	//char			buf[MAX_STATION_TEXT];
	char			*loadPath;
	int				num = 0;
	trap_Cvar_Register( &fs_game, "fs_game", "", CVAR_SERVERINFO | CVAR_ROM );
	loadPath = va( "MyMusic/internet_radio.list\0", fs_game.string );
	len = trap_FS_FOpenFile( va( "%s", loadPath), &f, FS_READ );
	if ( !f || !len )
	{
		trap_FS_FCloseFile( f );
		return;
	}

	buf = (char *) BG_DebugAlloc( 1 + 1, "CG_LoadInternetMusic" );

	//trap_FS_Read(&buf, len, f);
	trap_FS_Read( buf, len + 1, f );
	buf[len] = 0;

	// done
	trap_FS_FCloseFile( f );
#ifndef CGAMEDLL
	Com_Printf( "^4*** ^3%s^4: ^5Loading internet music site list from file ^7%s^5.\n", GAME_VERSION, loadPath );
#else
	CG_Printf( "^4*** ^3%s^4: ^5Loading internet music site list from file ^7%s^5.\n", GAME_VERSION, loadPath );
#endif
	for ( t = s = buf; *t; /* */ )
	{
		num++;
		s = strchr( s, '\n' );
		if ( !s || num > len )
		{
			break;
		}

		while ( *s == '\n' )
		{
			*s++ = 0;
		}

		if ( *t )
		{
			if ( !Q_strncmp( "//", va( "%s", t), 2) == 0 && strlen( va( "%s", t)) > 0 )
			{				// Not a comment either... Record it in our list...
				//strcpy(MyMusic[MyMusicTotal].musicFile, va("%s", t));
				Q_strncpyz( MyMusic[MyMusicTotal].musicFile, va( "%s", t), strlen( va( "%s", t)) );
				if ( strlen( va( "%s", MyMusic[MyMusicTotal].musicFile)) > 0 )
				{

					//CG_Printf("Loaded: \"%s\"\n", MyMusic[MyMusicTotal].musicFile);
					MyMusicTotal++;
				}
			}
		}

		t = s;
	}

	MyMusicLoaded = qtrue;
#ifndef CGAMEDLL
	Com_Printf( "^4*** ^3%s^4: ^5Loaded ^7%i^5 internet music sites.\n", GAME_VERSION, MyMusicTotal );
#else
	CG_Printf( "^4*** ^3%s^4: ^5Loaded ^7%i^5 internet music sites.\n", GAME_VERSION, MyMusicTotal );
#endif

	// Remove the extra MyMusicTotal++ from above...
	MyMusicTotal--;
}


/* */
void
CG_MyMusicPlay ( void )
{
	int selection = 0;
	if ( MyMusicTotal > 0 )
	{
		selection = rand() % MyMusicTotal;	//Q_TrueRand(0, MyMusicTotal);
	}

	if ( CG_GetMusicVolume() <= 0.01 )
	{
		if ( cthread )
		{
			CloseURL();
		}

		if ( chan )
		{
			BASS_StreamFree( chan );
		}

		return;
	}

	if ( Already_Playing /*&& BASS_ChannelIsActive(chan)*/ )
	{
		return;
	}

	while ( selection > 0 && !MyMusic[MyMusicTotal].musicFile )
	{
		selection = rand() % MyMusicTotal;	//Q_TrueRand(0, MyMusicTotal);
	}

	if ( Playing_Radio )
	{
		if ( !Q_stricmp( s_radioStation.string, "") && !Q_stricmp( s_radioStationOverride.string, "") )
		{
#ifndef CGAMEDLL
			Com_Printf( "^4*** ^3%s^4: ^5Playing internet radio: ^7%s^5.\n", GAME_VERSION, MyMusic[selection].musicFile );
#else
			CG_Printf( "^4*** ^3%s^4: ^5Playing internet radio: ^7%s^5.\n", GAME_VERSION, MyMusic[selection].musicFile );
#endif
		}
		else
		{
			if ( Q_stricmp( s_radioStationOverride.string, "") )
			{
#ifndef CGAMEDLL
				Com_Printf( "^4*** ^3%s^4: ^5Playing internet radio: ^7%s^5.\n", GAME_VERSION,
							s_radioStationOverride.string );
#else
				CG_Printf( "^4*** ^3%s^4: ^5Playing internet radio: ^7%s^5.\n", GAME_VERSION,
						   s_radioStationOverride.string );
#endif
			}
			else
			{
#ifndef CGAMEDLL
				Com_Printf( "^4*** ^3%s^4: ^5Playing internet radio: ^7%s^5.\n", GAME_VERSION, s_radioStation.string );
#else
				CG_Printf( "^4*** ^3%s^4: ^5Playing internet radio: ^7%s^5.\n", GAME_VERSION, s_radioStation.string );
#endif
			}
		}
	}
	else
	{
#ifndef CGAMEDLL
		Com_Printf( "^4*** ^3%s^4: ^5Playing MyMusic playlist file ^7%s^5.\n", GAME_VERSION,
					MyMusic[selection].musicFile );
#else
		CG_Printf( "^4*** ^3%s^4: ^5Playing MyMusic playlist file ^7%s^5.\n", GAME_VERSION, MyMusic[selection].musicFile );
#endif
	}

	if
	(
		!Playing_Radio ||
		(Playing_Radio && !Q_stricmp( s_radioStation.string, "") && !Q_stricmp( s_radioStationOverride.string, ""))
	)
	{
		CG_StartMusicTrack( 2, MyMusic[selection].musicFile, qfalse );
	}
	else
	{
		if ( Q_stricmp( s_radioStationOverride.string, "") )
		{
			CG_StartMusicTrack( 2, s_radioStationOverride.string, qfalse );
		}
		else
		{
			CG_StartMusicTrack( 2, s_radioStation.string, qfalse );
		}
	}

	length = BASS_StreamGetLength( chan );
	if ( Playing_Radio )
	{
		Already_Playing = qtrue;
	}
}


/* */
void
CG_SoundEngineInfo ( void )
{
	int i;
	for ( i = 0; i < 1024; i++ )
	{
		if ( BASS_GetDeviceDescription( i) )
#ifndef CGAMEDLL
			Com_Printf( "Found sound device %i (%s).\n", i, BASS_GetDeviceDescription( i) );
#else
		CG_Printf( "Found sound device %i (%s).\n", i, BASS_GetDeviceDescription( i) );
#endif
	}
}

qboolean	engine_initialized = qfalse;


/* */
void
CG_DoMusic ( void )
{
	if ( !CheckBASSLoaded() )
	{
		return;
	}

	if ( BASS_GetVersion() != MAKELONG( 2, 1) )
	{
#ifndef CGAMEDLL
		Com_Printf( "BASS version 2.1 was not loaded\n" );
#else
		CG_Printf( "BASS version 2.1 was not loaded\n" );
#endif
		return;
	}

	// setup output - default device
	if ( !engine_initialized )
	{
		if ( cthread )
		{
			CloseURL();
		}

		if ( chan )
		{
			BASS_StreamFree( chan );
		}

		BASS_Free();
		CG_SoundEngineInfo();
		if ( !BASS_Init( 1, 44100, 0, 0, NULL) )
		{
			Error( va( "Can't initialize device: %s.", BASS_GetDeviceDescription( 1)) );
			return;
		}
		else
		{
			engine_initialized = qtrue;
		}
	}
#ifdef CGAMEDLL
#ifdef __INETRADIO__
	if ( !MyMusicLoaded && (s_inetRadio.integer || CG_CheckMyMusic())  )
	{
		BASS_StreamFree( chan );
		if ( cthread )
		{
			CloseURL();
		}

		CG_LoadMyMusic();
		if ( MyMusicTotal <= 0 && s_inetRadio.integer  )
		{
			CG_LoadInternetMusic();
			if
			(
				MyMusicTotal > 0 ||
				Q_stricmp( s_radioStation.string, "") ||
				Q_stricmp( s_radioStationOverride.string, "")
			)
			{
				Playing_Radio = qtrue;
				MyMusicLoaded = qtrue;
			}
		}
	}
#endif
#endif
	if ( CG_GetMusicVolume() <= 0.01  )
	{
		if ( cthread )
		{
			CloseURL();
		}

		if ( chan )
		{
			BASS_StreamFree( chan );
		}

		return;
	}

	if ( chan )
	{
		int volume = CG_GetMusicVolume() * 100;
		BASS_ChannelSetAttributes( chan, -1 /*leave crrent*/, volume, -101 /*leave crrent*/ );
		pos = BASS_ChannelGetPosition( chan );
	}
#ifdef CGAMEDLL
#ifdef __INETRADIO__
	if ( MyMusicTotal > 0 || (Q_stricmp( s_radioStation.string, "") || Q_stricmp( s_radioStationOverride.string, "") && s_inetRadio.integer)  )
	{	// MyMusic overrides the game music, if the player has some installed...
		if (!Playing_Radio && !CG_CheckMyMusic())
		{
		} else if ( !chan || pos >= length )
		{

			//chan = 0;
			BASS_StreamFree( chan );
			CG_MyMusicPlay();
		}
		else if ( act = BASS_ChannelIsActive( chan) )
		{

			// display some stuff and wait a bit
			music_time = timeGetTime() - starttime;
			level = BASS_ChannelGetLevel( chan );
			pos = BASS_ChannelGetPosition( chan );
		}
		else
		{
			BASS_StreamFree( chan );
			CG_MyMusicPlay();
		}
	}
#endif
#endif

#if defined( __SUPREMACY__ ) || defined( __FULL_VERSION__ )
	else if ( act = BASS_ChannelIsActive( chan) )
	{

		// display some stuff and wait a bit
		music_time = timeGetTime() - starttime;
		level = BASS_ChannelGetLevel( chan );
		pos = BASS_ChannelGetPosition( chan );
	}
#endif //defined(__SUPREMACY__) || defined(__FULL_VERSION__)
}


/* */
void
CG_StopMusic ( void )
{

	// wind the frequency down...
	//BASS_ChannelSlideAttributes(chan,1000,-1,-101,500);
	//Sleep(300);
	// ...and fade-out to avoid a "click"
	//BASS_ChannelSlideAttributes(chan,-1,-2,-101,200);
	//while (BASS_ChannelIsSliding(chan))
	//	Sleep(1);
	if ( cthread )
	{
		CloseURL();
	}

	if ( chan )
	{
		BASS_StreamFree( chan );
	}

	BASS_Free();
	FreeBASS();
	engine_initialized = qfalse;
	BASS_DLL_READY = qfalse;
}

vmCvar_t	fs_game;
/* */
void
CG_SoundEngineStartMusicTrack ( char *sound, qboolean loop )
{
	char				filename[MAX_QPATH];

	if ( !CheckBASSLoaded() )
	{
		return;
	}

	if ( BASS_GetVersion() != MAKELONG( 2, 1) )
	{
#ifndef CGAMEDLL
		Com_Printf( "BASS version 2.1 was not loaded\n" );
#else
		CG_Printf( "BASS version 2.1 was not loaded\n" );
#endif
		return;
	}

	trap_Cvar_Register( &fs_game, "fs_game", "", CVAR_SERVERINFO | CVAR_ROM );

	Q_strncpyz( filename, va("%s/", fs_game.string), MAX_QPATH );
	Q_strcat( filename, MAX_QPATH, va( "%s", sound) );



	// setup output - default device
	if ( !engine_initialized )
	{
		CG_SoundEngineInfo();
		if ( !BASS_Init( 1, 44100, 0, 0, NULL) )
		{
			Error( va( "Can't initialize device: %s.", BASS_GetDeviceDescription( 1)) );
			return;
		}
		else
		{
			engine_initialized = qtrue;
		}
	}

	if ( MyMusicTotal > 0 )
	{
		return;
	}

#if defined( __SUPREMACY__ ) || defined( __FULL_VERSION__ )
	if ( chan )
	{
		BASS_StreamFree( chan );
	}

	CG_StartMusicTrack( 2, va( "%s", filename), loop );
#endif //defined(__SUPREMACY__) || defined(__FULL_VERSION__)
}

#ifndef __SOUND_ENGINE__


/* */
void
CG_SoundEngineStartStreamingSound ( char *sound, qboolean loop )
{
	if ( BASS_GetVersion() != MAKELONG( 2, 1) )
	{
#ifndef CGAMEDLL
		Com_Printf( "BASS version 2.1 was not loaded\n" );
#else
		CG_Printf( "BASS version 2.1 was not loaded\n" );
#endif
		return;
	}

	// setup output - default device
	if ( !engine_initialized )
	{
		CG_SoundEngineInfo();
		if ( !BASS_Init( 1, 44100, 0, 0, NULL) )
		{
			Error( va( "Can't initialize device: %s.", BASS_GetDeviceDescription( 1)) );
			return;
		}
		else
		{
			engine_initialized = qtrue;
		}
	}

	if ( MyMusicTotal > 0 )
	{
		return;
	}

	if ( chan )
	{
		BASS_StreamFree( chan );
	}

	if ( act = BASS_ChannelIsActive( chan) )
	{

		// display some stuff and wait a bit
		music_time = timeGetTime() - starttime;
		level = BASS_ChannelGetLevel( chan );
		pos = BASS_ChannelGetPosition( chan );
	}

	CG_StartMusicTrack( 2, va( "%s", sound), loop );
}
#endif //__SOUND_ENGINE__
#endif //__MUSIC_ENGINE__
