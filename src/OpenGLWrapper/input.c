#include <stdlib.h>
#include <ctype.h>
#include "gltrace.h"
#include "input.h"
#if defined WIN32 || defined WIN64
#define ISKEYDOWN( x )	( (GetAsyncKeyState( x) & 0x8000) != 0 )
#else

/* need to define ISKEYDOWN and all the VK_* constants */
#define ISKEYDOWN( x )
#endif
extern GLboolean	bTransEnabled;
static GLboolean	option = GL_FALSE;
static GLboolean	oneframe = GL_FALSE;
static GLboolean	fps = GL_TRUE;
static GLboolean	on_off = GL_FALSE;

/*****************************/

/*  Time Keys (on/off)       */

/*****************************/
static GLboolean	time_on = GL_TRUE;
static int			time_key = '5';

/*******************************/

/*  Tans Walls Keys (on/off)   */

/*******************************/
static GLboolean	trans_on = GL_FALSE;
static int			trans_key = '6';

/*******************************/

/*  Wire Frame Keys (on/off)   */

/*******************************/
static GLboolean	wire_on = GL_FALSE;
static int			wire_key = '7';
static int			option_key = '1';
static int			oneframe_key = '2';
static int			fps_key = '3';
static int			on_off_key = '4';
static GLboolean	test_on = GL_FALSE;
static int			test_key = '8';


/* */
static int
translateKey ( char *key )
{

	/* check for single char */
	if ( strlen( key) == 1 )
	{

		/* convert to upper for consistancy */
		return ( toupper( *key) );
	}

	if ( strcmp( key, "shift") == 0 )
	{
		return ( VK_SHIFT );
	}

	if ( strcmp( key, "alt") == 0 )
	{
		return ( VK_MENU );
	}

	if ( strcmp( key, "ctrl") == 0 )
	{
		return ( VK_CONTROL );
	}

	if ( strcmp( key, "tab") == 0 )
	{
		return ( VK_TAB );
	}

	if ( strcmp( key, "up") == 0 )
	{
		return ( VK_UP );
	}

	if ( strcmp( key, "down") == 0 )
	{
		return ( VK_DOWN );
	}

	if ( strcmp( key, "left") == 0 )
	{
		return ( VK_LEFT );
	}

	if ( strcmp( key, "right") == 0 )
	{
		return ( VK_RIGHT );
	}

	if ( strcmp( key, "insert") == 0 )
	{
		return ( VK_INSERT );
	}

	if ( strcmp( key, "delete") == 0 )
	{
		return ( VK_DELETE );
	}

	if ( strcmp( key, "home") == 0 )
	{
		return ( VK_HOME );
	}

	if ( strcmp( key, "end") == 0 )
	{
		return ( VK_END );
	}

	if ( strcmp( key, "pageup") == 0 )
	{
		return ( VK_PRIOR );
	}

	if ( strcmp( key, "pagedn") == 0 )
	{
		return ( VK_NEXT );
	}

	if ( strcmp( key, "backsp") == 0 )
	{
		return ( VK_BACK );
	}

	if ( strcmp( key, "enter") == 0 )
	{
		return ( VK_RETURN );
	}

	if ( strcmp( key, "F1") == 0 )
	{
		return ( VK_F1 );
	}

	if ( strcmp( key, "F2") == 0 )
	{
		return ( VK_F2 );
	}

	if ( strcmp( key, "F3") == 0 )
	{
		return ( VK_F3 );
	}

	if ( strcmp( key, "F4") == 0 )
	{
		return ( VK_F4 );
	}

	if ( strcmp( key, "F5") == 0 )
	{
		return ( VK_F5 );
	}

	if ( strcmp( key, "F6") == 0 )
	{
		return ( VK_F6 );
	}

	if ( strcmp( key, "F7") == 0 )
	{
		return ( VK_F7 );
	}

	if ( strcmp( key, "F8") == 0 )
	{
		return ( VK_F8 );
	}

	if ( strcmp( key, "F9") == 0 )
	{
		return ( VK_F9 );
	}

	if ( strcmp( key, "F10") == 0 )
	{
		return ( VK_F10 );
	}

	if ( strcmp( key, "F11") == 0 )
	{
		return ( VK_F11 );
	}

	if ( strcmp( key, "F12") == 0 )
	{
		return ( VK_F12 );
	}

	return ( 0 );
}


/* */
void
updateKeyState ( void )
{
	static GLboolean	test_toggle = GL_FALSE;
	static GLboolean	wire_toggle = GL_FALSE;		// Wire Frame
	static GLboolean	trans_toggle = GL_FALSE;	// Trans Walls
	static GLboolean	time_toggle = GL_FALSE;		// Time
	static GLboolean	oneframe_toggle = GL_FALSE;
	static GLboolean	fps_toggle = GL_FALSE;
	static GLboolean	on_off_toggle = GL_FALSE;

	/* toggle oneframe key, only valid for one frame */
	if ( ISKEYDOWN( oneframe_key) )
	{

		/* key is down, but was it up last frame? */
		if ( oneframe_toggle == GL_FALSE )
		{

			/* yes, so set to true for this frame */
			oneframe_toggle = GL_TRUE;
			oneframe = GL_TRUE;
		}
		else
		{

			/* no, so set to false and wait for key to be released */
			oneframe = GL_FALSE;
		}
	}
	else
	{
		oneframe_toggle = GL_FALSE;
	}

	option = ( GLboolean ) ( ISKEYDOWN( option_key) );

	/* toggle on_off key */
	if ( ISKEYDOWN( fps_key) )
	{
		if ( fps_toggle == GL_FALSE )
		{
			fps_toggle = GL_TRUE;
			fps = ( GLboolean ) ( fps == GL_TRUE ? GL_FALSE : GL_TRUE );
		}
	}
	else
	{
		fps_toggle = GL_FALSE;
	}

	/* toggle on_off key */
	if ( ISKEYDOWN( on_off_key) )
	{
		if ( on_off_toggle == GL_FALSE )
		{
			on_off_toggle = GL_TRUE;
			on_off = ( GLboolean ) ( on_off == GL_TRUE ? GL_FALSE : GL_TRUE );
		}
	}
	else
	{
		on_off_toggle = GL_FALSE;
	}

	// For Time
	if ( ISKEYDOWN( time_key) )
	{
		if ( time_toggle == GL_FALSE )
		{
			time_toggle = GL_TRUE;
			time_on = ( GLboolean ) ( time_on == GL_TRUE ? GL_FALSE : GL_TRUE );
		}
	}
	else
	{
		time_toggle = GL_FALSE;
	}

	// For Trans Walls
	if ( ISKEYDOWN( trans_key) )
	{
		if ( trans_toggle == GL_FALSE )
		{
			trans_toggle = GL_TRUE;
			trans_on = ( GLboolean ) ( trans_on == GL_TRUE ? GL_FALSE : GL_TRUE );
		}
	}
	else
	{
		trans_toggle = GL_FALSE;
	}

	// For Wire Frame
	if ( ISKEYDOWN( wire_key) )
	{
		if ( wire_toggle == GL_FALSE )
		{
			wire_toggle = GL_TRUE;
			wire_on = ( GLboolean ) ( wire_on == GL_TRUE ? GL_FALSE : GL_TRUE );
		}
	}
	else
	{
		wire_toggle = GL_FALSE;
	}

	// For Testing
	if ( ISKEYDOWN( test_key) )
	{
		if ( test_toggle == GL_FALSE )
		{
			test_toggle = GL_TRUE;
			test_on = ( GLboolean ) ( test_on == GL_TRUE ? GL_FALSE : GL_TRUE );
		}
	}
	else
	{
		test_toggle = GL_FALSE;
	}
}


/* */
GLboolean
setKey ( int function, char *key )
{
	switch ( function )
	{
		case GLT_OPTION:
			option_key = translateKey( key );
			break;
		case GLT_ONEFRAME:
			oneframe_key = translateKey( key );
			break;
		case GLT_FPS:
			fps_key = translateKey( key );
			break;
		case GLT_ON_OFF:
			on_off_key = translateKey( key );
			break;
		case SV_TIME:
			time_key = translateKey( key );
			break;
		case SV_WALL:
			trans_key = translateKey( key );
			break;
		case SV_WIRE:
			wire_key = translateKey( key );
			break;
		case SV_TEST:
			test_key = translateKey( key );
			break;
		default:
			return ( GL_FALSE );
	}
}


/* */
GLboolean
getKeyState ( int function )
{
	switch ( function )
	{
		case GLT_OPTION:
			return ( option );
			break;
		case GLT_ONEFRAME:
			return ( oneframe );
			break;
		case GLT_FPS:
			return ( fps );
			break;
		case GLT_ON_OFF:
			return ( on_off );
			break;
		case SV_TIME:
			return ( time_on );
		case SV_WALL:
			return ( trans_on );
		case SV_WIRE:
			return ( wire_on );
		case SV_TEST:
			return ( test_on );
		default:
			return ( GL_FALSE );
	}
}


/* */
void
setKeyState ( int function, GLboolean state )
{
	switch ( function )
	{
		case GLT_OPTION:
			option = state;
			break;
		case GLT_ONEFRAME:
			oneframe = state;
			break;
		case GLT_FPS:
			fps = state;
			break;
		case GLT_ON_OFF:
			on_off = state;
			break;
		case SV_TIME:
			time_on = state;
			break;
		case SV_WALL:
			trans_on = state;
		case SV_WIRE:
			wire_on = state;
		case SV_TEST:
			test_on = state;
			break;
		default:
			break;
	}
}
