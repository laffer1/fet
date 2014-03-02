#include "gltrace.h"
#include "input.h"
GLsizei				g_w, g_h;
extern unsigned int nCallCount[];
extern GLboolean	gltraceInit ( void );
extern int			GetEXTProcIndex ( LPCSTR name );
extern int			fontBitmapString ( char *str );
extern GLboolean	bFSPenable;						/* enable the FSP counter */
extern GLboolean	bLogEnable;						/* enable normal logging */
extern GLfloat		*fpsColor;						/* color of the FPS counter */
GLboolean			bOptionKeydown = GL_FALSE;		/* is the key down for logging optional functions */
GLboolean			bOneframeKeydown = GL_FALSE;	/* is the key down for one-shot (frame) logging of functions */
GLboolean			bOnOffKeydown = GL_TRUE;		/* is the key down for logging optional functions */
GLboolean			bLogCalls;						/* TRUE if any logging is being done */

/*************************/

/*  For Displaying Time  */

/*************************/
extern GLfloat		*timeColor;						// For Time Color
GLboolean			bTimeKeyDown = GL_FALSE;		// Time Key Being Pushed?
extern GLboolean	bTimeEnabled;					// Time Enabled?

/* End Time */

/*************************/

/* For Trans Walls Hack  */

/*************************/
extern GLboolean	bTransEnabled;

/* End Trans */

/***********************/

/* For WireFrame Hack  */

/***********************/
extern GLboolean	bWireEnabled;

/* End WireFrame */
#define START( X )																			\
	if ( bCountCalls == GL_TRUE )															\
	{																						\
		nCallCount[FP_##X]++;																\
	}																						\
	if ( bLogCalls == GL_TRUE )																\
	{																						\
		if																					\
		(																					\
			((show_status[FP_##X] == SHOW_ALWAYS) && (bOnOffKeydown == GL_TRUE)) ||			\
			((show_status[FP_##X] == SHOW_IF_SHIFT_DOWN) && (bOptionKeydown == GL_TRUE)) || \
			(bOneframeKeydown == GL_TRUE)													\
		)																					\
		{																					\
			if ( bLogTime == GL_TRUE )														\
			{																				\
				write_time();																\
			} print( GLN[FP_##X] );															\
			print( "(" );																	\
			if ( bVerbose == GL_TRUE )														\
			{
#define END		  \
} print( ")\n" ); \
}					  \
}
#define RESULT	print( ") = " );
#define ENDRESULT			   \
}							   \
	if ( bVerbose == GL_TRUE ) \
	{						   \
		print( "\n" );		   \
	}						   \
	else					   \
	{						   \
		print( ")\n" );		   \
	}						   \
}							   \
}							   \
								\
	/*************************/

/* Function (Wire Frame) */


/*************************/
static void
EnableWireFrame ( void )
{
	if ( getKeyState( SV_WIRE) == GL_TRUE )
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glDisable( GL_TEXTURE_2D );
	}

	if ( getKeyState( SV_WIRE) == GL_FALSE )
	{
		glEnable( GL_TEXTURE_2D );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
}

/*************************/

/* Function (Wall Hack)  */


/*************************/
static void
EnableTransWalls ( void )
{
	if ( getKeyState( SV_WALL) == GL_TRUE )
	{
		glDisable( GL_DEPTH_TEST );
		glBlendFunc( GL_BLEND_COLOR, GL_COLOR );
	}
}


/* End Trans Walls */
static void
drawFPSMessage ( int x, int y, char *string, ... )
{
	char		buffer[256];
	va_list		args;
	GLfloat		color[4], position[4];
	GLboolean	texturing;
	va_start( args, string );
	vsprintf( buffer, string, args );
	va_end( args );

	/* save OpenGL state */
	GLV.glPushAttrib( GL_ALL_ATTRIB_BITS );
	GLV.glGetFloatv( GL_CURRENT_COLOR, color );
	GLV.glGetBooleanv( GL_TEXTURE_2D, &texturing );
	GLV.glGetFloatv( GL_CURRENT_RASTER_POSITION, position );

	/* set up the state we need */
	GLV.glDisable( GL_TEXTURE_2D );
	GLV.glColor3fv( fpsColor );
	GLV.glRasterPos2i( x, y );
	fontBitmapString( buffer );

	/* restore OpenGL state */
	GLV.glPopAttrib();
	GLV.glColor4fv( color );
	if ( texturing == GL_TRUE )
	{
		GLV.glEnable( GL_TEXTURE_2D );
	}

	GLV.glRasterPos2i( x, y );
}

/***********************/

/* For Displaying Time */


/***********************/
static void
drawTIMEMessage ( int x, int y, char *string, ... )
{
	char		buffer[256];
	va_list		args;
	GLfloat		color[4], position[4];
	GLboolean	texturing;
	va_start( args, string );
	vsprintf( buffer, string, args );
	va_end( args );

	/* save OpenGL state */
	GLV.glPushAttrib( GL_ALL_ATTRIB_BITS );
	GLV.glGetFloatv( GL_CURRENT_COLOR, color );
	GLV.glGetBooleanv( GL_TEXTURE_2D, &texturing );
	GLV.glGetFloatv( GL_CURRENT_RASTER_POSITION, position );

	/* set up the state we need */
	GLV.glDisable( GL_TEXTURE_2D );
	GLV.glColor3fv( timeColor );
	GLV.glRasterPos2i( x, y );
	GLV.glDisable( GL_DEPTH );
	fontBitmapString( buffer );

	/* restore OpenGL state */
	GLV.glPopAttrib();
	GLV.glColor4fv( color );

	/* Draw The Shit if texturing is TRUE */
	if ( texturing == GL_TRUE )
	{
		GLV.glEnable( GL_TEXTURE_2D );
	}

	GLV.glRasterPos4fv( position );
}


/* */
static void
renderTIME ( void )
{
	char		*timestring;
	SYSTEMTIME	SysTime;
	GetLocalTime( &SysTime );
	if ( SysTime.wHour > 12 )
	{
		SysTime.wHour -= 12;
	}

	if ( SysTime.wMinute < 10 )
	{
		timestring = "%d:0%d:%d";
	}
	else
	{
		timestring = "%d:%d:%d";
	}

	if ( getKeyState( SV_TIME) == GL_TRUE )
	{
		drawTIMEMessage( 5, 40, timestring, SysTime.wHour, SysTime.wMinute, SysTime.wSecond );
	}
}

/* End Time */

/******************************/

/* Function for Testing shit  */


/******************************/
static void
EnableTestMode ( void )
{
	char	*Sstring = "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
	if ( getKeyState( SV_TEST) == GL_TRUE )
	{
		drawTIMEMessage( 30, 100, Sstring );
	}
}


/* */
static void
renderFPS ( void )
{
	static int			frames = 0;
	static time_t		last;
	static GLboolean	first = GL_TRUE;
	static float		fps = 0.0f;
	frames++;
	if ( first == GL_TRUE )
	{
		time( &last );
		first = GL_FALSE;
	}
	else
	{
		time_t	now;
		int		diff;
		time( &now );

		/* update no more than once per second */
		diff = (int) ( now - last );
		if ( diff > 0 )
		{
			fps = (float) frames / diff;
			frames = 0;
			last = now;
		}
	}

	if ( getKeyState( GLT_FPS) == GL_TRUE )
	{
		drawFPSMessage( 5, 20, "FPS: %4.2f", fps );
	}
}


/* */
static void
updateKeys ( void )
{
	if ( bOneframeKeydown == GL_TRUE )
	{
		print( "\n\n««««« End of logging »»»»»\n\n" );
	}

	updateKeyState();
	bOptionKeydown = getKeyState( GLT_OPTION );
	bOneframeKeydown = getKeyState( GLT_ONEFRAME );
	bOnOffKeydown = ( GLboolean ) ( getKeyState( GLT_ON_OFF) && bLogEnable );
	if ( bOptionKeydown == GL_TRUE || bOneframeKeydown == GL_TRUE || bOnOffKeydown == GL_TRUE )
	{
		bLogCalls = GL_TRUE;
	}

	if ( bOneframeKeydown == GL_TRUE )
	{
		print( "\n\n««««« Begin one frame logging »»»»»\n\n" );
	}
}

#include "ext.h"


//
// OpenGL function handlers

//
void GLAPIENTRY
glAccum ( GLenum op, GLfloat value )
{
	START( glAccum );
	print_value( _GLenum, &op );
	print( "," );
	print_value( _GLfloat, &value );
	END;
	GLV.glAccum( op, value );
}


/* */
void GLAPIENTRY
glAlphaFunc ( GLenum func, GLclampf ref )
{
	START( glAlphaFunc );
	print_value( _GLenum, &func );
	print( "," );
	print_value( _GLclampf, &ref );
	END;
	GLV.glAlphaFunc( func, ref );
}


/* */
GLboolean GLAPIENTRY
glAreTexturesResident ( GLsizei n, const GLuint *textures, GLboolean *residences )
{
	GLboolean	result;
	int			i;
	result = GLV.glAreTexturesResident( n, textures, residences );
	START( glAreTexturesResident );
	print_value( _GLsizei, &n );
	print( "," );
	print( "[" );
	for ( i = 0; i < n; i++ )
	{
		if ( i != 0 )
		{
			print( "," );
		}

		print( "%d is %s", textures[i], (residences[i] == GL_TRUE) ? "TRUE" : "FALSE" );
	}

	print( "]" );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
void GLAPIENTRY
glArrayElement ( GLint i )
{
	START( glArrayElement );
	print_value( _GLint, &i );
	END;
	GLV.glArrayElement( i );
}


/* */
void GLAPIENTRY
glBegin ( GLenum mode )
{
	START( glBegin );
	print_value( _GLenum, &mode );
	END;
	GLV.glBegin( mode );
}


/* */
void GLAPIENTRY
glBindTexture ( GLenum target, GLuint texture )
{
	START( glBindTexture );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLuint, &texture );
	END;
	GLV.glBindTexture( target, texture );
}


/* */
void GLAPIENTRY
glBitmap
(
	GLsizei			width,
	GLsizei			height,
	GLfloat			xorig,
	GLfloat			yorig,
	GLfloat			xmove,
	GLfloat			ymove,
	const GLubyte	*bitmap
)
{
	START( glBitmap );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLsizei, &height );
	print( "," );
	print_value( _GLfloat, &xorig );
	print( "," );
	print_value( _GLfloat, &yorig );
	print( "," );
	print_value( _GLfloat, &xmove );
	print( "," );
	print_value( _GLfloat, &ymove );
	print( "," );
	print_value( HEX, &bitmap );
	END;
	GLV.glBitmap( width, height, xorig, yorig, xmove, ymove, bitmap );
}


/* */
void GLAPIENTRY
glBlendFunc ( GLenum sfactor, GLenum dfactor )
{
	useZeroOne = GL_TRUE;
	START( glBlendFunc );
	print_value( _GLenum, &sfactor );
	print( "," );
	print_value( _GLenum, &dfactor );
	END;
	GLV.glBlendFunc( sfactor, dfactor );
	useZeroOne = GL_FALSE;
}


/* */
void GLAPIENTRY
glCallList ( GLuint list )
{
	START( glCallList );
	print_value( _GLuint, &list );
	END;
	GLV.glCallList( list );
}


/* */
void GLAPIENTRY
glCallLists ( GLsizei n, GLenum type, const GLvoid *lists )
{
	START( glCallLists );
	print_value( _GLsizei, &n );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLvoid, &lists );
	END;
	GLV.glCallLists( n, type, lists );
}


/* */
void GLAPIENTRY
glClear ( GLbitfield mask )
{
	START( glClear );
	print_value( _GLbitfield, &mask );
	END;
	GLV.glClear( mask );
}


/* */
void GLAPIENTRY
glClearAccum ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
	START( glClearAccum );
	print_value( _GLfloat, &red );
	print( "," );
	print_value( _GLfloat, &green );
	print( "," );
	print_value( _GLfloat, &blue );
	print( "," );
	print_value( _GLfloat, &alpha );
	END;
	GLV.glClearAccum( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glClearColor ( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
	START( glClearColor );
	print_value( _GLclampf, &red );
	print( "," );
	print_value( _GLclampf, &green );
	print( "," );
	print_value( _GLclampf, &blue );
	print( "," );
	print_value( _GLclampf, &alpha );
	END;
	GLV.glClearColor( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glClearDepth ( GLclampd depth )
{
	START( glClearDepth );
	print_value( _GLclampd, &depth );
	END;
	GLV.glClearDepth( depth );
}


/* */
void GLAPIENTRY
glClearIndex ( GLfloat c )
{
	START( glClearIndex );
	print_value( _GLfloat, &c );
	END;
	GLV.glClearIndex( c );
}


/* */
void GLAPIENTRY
glClearStencil ( GLint s )
{
	START( glClearStencil );
	print_value( _GLint, &s );
	END;
	GLV.glClearStencil( s );
}


/* */
void GLAPIENTRY
glClipPlane ( GLenum plane, const GLdouble *equation )
{
	START( glClipPlane );
	print_value( _GLenum, &plane );
	print( "," );
	print_value( _GLdouble4, &equation );
	END;
	GLV.glClipPlane( plane, equation );
}


/* */
void GLAPIENTRY
glColor3b ( GLbyte red, GLbyte green, GLbyte blue )
{
	START( glColor3b );
	print_value( _GLbyte, &red );
	print( "," );
	print_value( _GLbyte, &green );
	print( "," );
	print_value( _GLbyte, &blue );
	END;
	GLV.glColor3b( red, green, blue );
}


/* */
void GLAPIENTRY
glColor3bv ( const GLbyte *v )
{
	START( glColor3bv );
	print_value( _GLbyte3, &v );
	END;
	GLV.glColor3bv( v );
}


/* */
void GLAPIENTRY
glColor3d ( GLdouble red, GLdouble green, GLdouble blue )
{
	START( glColor3d );
	print_value( _GLdouble, &red );
	print( "," );
	print_value( _GLdouble, &green );
	print( "," );
	print_value( _GLdouble, &blue );
	END;
	GLV.glColor3d( red, green, blue );
}


/* */
void GLAPIENTRY
glColor3dv ( const GLdouble *v )
{
	START( glColor3dv );
	print_value( _GLdouble3, &v );
	END;
	GLV.glColor3dv( v );
}


/* */
void GLAPIENTRY
glColor3f ( GLfloat red, GLfloat green, GLfloat blue )
{
	START( glColor3f );
	print_value( _GLfloat, &red );
	print( "," );
	print_value( _GLfloat, &green );
	print( "," );
	print_value( _GLfloat, &blue );
	END;
	GLV.glColor3f( red, green, blue );
}


/* */
void GLAPIENTRY
glColor3fv ( const GLfloat *v )
{
	START( glColor3fv );
	print_value( _GLfloat3, &v );
	END;
	GLV.glColor3fv( v );
}


/* */
void GLAPIENTRY
glColor3i ( GLint red, GLint green, GLint blue )
{
	START( glColor3i );
	print_value( _GLint, &red );
	print( "," );
	print_value( _GLint, &green );
	print( "," );
	print_value( _GLint, &blue );
	END;
	GLV.glColor3i( red, green, blue );
}


/* */
void GLAPIENTRY
glColor3iv ( const GLint *v )
{
	START( glColor3iv );
	print_value( _GLint3, &v );
	END;
	GLV.glColor3iv( v );
}


/* */
void GLAPIENTRY
glColor3s ( GLshort red, GLshort green, GLshort blue )
{
	START( glColor3s );
	print_value( _GLshort, &red );
	print( "," );
	print_value( _GLshort, &green );
	print( "," );
	print_value( _GLshort, &blue );
	END;
	GLV.glColor3s( red, green, blue );
}


/* */
void GLAPIENTRY
glColor3sv ( const GLshort *v )
{
	START( glColor3sv );
	print_value( _GLshort3, &v );
	END;
	GLV.glColor3sv( v );
}


/* */
void GLAPIENTRY
glColor3ub ( GLubyte red, GLubyte green, GLubyte blue )
{
	START( glColor3ub );
	print_value( _GLubyte, &red );
	print( "," );
	print_value( _GLubyte, &green );
	print( "," );
	print_value( _GLubyte, &blue );
	END;
	GLV.glColor3ub( red, green, blue );
}


/* */
void GLAPIENTRY
glColor3ubv ( const GLubyte *v )
{
	START( glColor3ubv );
	print_value( _GLubyte3, &v );
	END;
	GLV.glColor3ubv( v );
}


/* */
void GLAPIENTRY
glColor3ui ( GLuint red, GLuint green, GLuint blue )
{
	START( glColor3ui );
	print_value( _GLuint, &red );
	print( "," );
	print_value( _GLuint, &green );
	print( "," );
	print_value( _GLuint, &blue );
	END;
	GLV.glColor3ui( red, green, blue );
}


/* */
void GLAPIENTRY
glColor3uiv ( const GLuint *v )
{
	START( glColor3uiv );
	print_value( _GLuint3, &v );
	END;
	GLV.glColor3uiv( v );
}


/* */
void GLAPIENTRY
glColor3us ( GLushort red, GLushort green, GLushort blue )
{
	START( glColor3us );
	print_value( _GLushort, &red );
	print( "," );
	print_value( _GLushort, &green );
	print( "," );
	print_value( _GLushort, &blue );
	END;
	GLV.glColor3us( red, green, blue );
}


/* */
void GLAPIENTRY
glColor3usv ( const GLushort *v )
{
	START( glColor3usv );
	print_value( _GLushort3, &v );
	END;
	GLV.glColor3usv( v );
}


/* */
void GLAPIENTRY
glColor4b ( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha )
{
	START( glColor4b );
	print_value( _GLbyte, &red );
	print( "," );
	print_value( _GLbyte, &green );
	print( "," );
	print_value( _GLbyte, &blue );
	print( "," );
	print_value( _GLbyte, &alpha );
	END;
	GLV.glColor4b( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glColor4bv ( const GLbyte *v )
{
	START( glColor4bv );
	print_value( _GLbyte4, &v );
	END;
	GLV.glColor4bv( v );
}


/* */
void GLAPIENTRY
glColor4d ( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha )
{
	START( glColor4d );
	print_value( _GLdouble, &red );
	print( "," );
	print_value( _GLdouble, &green );
	print( "," );
	print_value( _GLdouble, &blue );
	print( "," );
	print_value( _GLdouble, &alpha );
	END;
	GLV.glColor4d( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glColor4dv ( const GLdouble *v )
{
	START( glColor4dv );
	print_value( _GLdouble4, &v );
	END;
	GLV.glColor4dv( v );
}


/* */
void GLAPIENTRY
glColor4f ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
	START( glColor4f );
	print_value( _GLfloat, &red );
	print( "," );
	print_value( _GLfloat, &green );
	print( "," );
	print_value( _GLfloat, &blue );
	print( "," );
	print_value( _GLfloat, &alpha );
	END;
	GLV.glColor4f( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glColor4fv ( const GLfloat *v )
{
	START( glColor4fv );
	print_value( _GLfloat4, &v );
	END;
	GLV.glColor4fv( v );
}


/* */
void GLAPIENTRY
glColor4i ( GLint red, GLint green, GLint blue, GLint alpha )
{
	START( glColor4i );
	print_value( _GLint, &red );
	print( "," );
	print_value( _GLint, &green );
	print( "," );
	print_value( _GLint, &blue );
	print( "," );
	print_value( _GLint, &alpha );
	END;
	GLV.glColor4i( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glColor4iv ( const GLint *v )
{
	START( glColor4iv );
	print_value( _GLint4, &v );
	END;
	GLV.glColor4iv( v );
}


/* */
void GLAPIENTRY
glColor4s ( GLshort red, GLshort green, GLshort blue, GLshort alpha )
{
	START( glColor4s );
	print_value( _GLshort, &red );
	print( "," );
	print_value( _GLshort, &green );
	print( "," );
	print_value( _GLshort, &blue );
	print( "," );
	print_value( _GLshort, &alpha );
	END;
	GLV.glColor4s( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glColor4sv ( const GLshort *v )
{
	START( glColor4sv );
	print_value( _GLshort4, &v );
	END;
	GLV.glColor4sv( v );
}


/* */
void GLAPIENTRY
glColor4ub ( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
	START( glColor4ub );
	print_value( _GLubyte, &red );
	print( "," );
	print_value( _GLubyte, &green );
	print( "," );
	print_value( _GLubyte, &blue );
	print( "," );
	print_value( _GLubyte, &alpha );
	END;
	GLV.glColor4ub( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glColor4ubv ( const GLubyte *v )
{
	START( glColor4ubv );
	print_value( _GLubyte4, &v );
	END;
	GLV.glColor4ubv( v );
}


/* */
void GLAPIENTRY
glColor4ui ( GLuint red, GLuint green, GLuint blue, GLuint alpha )
{
	START( glColor4ui );
	print_value( _GLuint, &red );
	print( "," );
	print_value( _GLuint, &green );
	print( "," );
	print_value( _GLuint, &blue );
	print( "," );
	print_value( _GLuint, &alpha );
	END;
	GLV.glColor4ui( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glColor4uiv ( const GLuint *v )
{
	START( glColor4uiv );
	print_value( _GLuint4, &v );
	END;
	GLV.glColor4uiv( v );
}


/* */
void GLAPIENTRY
glColor4us ( GLushort red, GLushort green, GLushort blue, GLushort alpha )
{
	START( glColor4us );
	print_value( _GLushort, &red );
	print( "," );
	print_value( _GLushort, &green );
	print( "," );
	print_value( _GLushort, &blue );
	print( "," );
	print_value( _GLushort, &alpha );
	END;
	GLV.glColor4us( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glColor4usv ( const GLushort *v )
{
	START( glColor4usv );
	print_value( _GLshort4, &v );
	END;
	GLV.glColor4usv( v );
}


/* */
void GLAPIENTRY
glColorMask ( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )
{
	START( glColorMask );
	print_value( _GLboolean, &red );
	print( "," );
	print_value( _GLboolean, &green );
	print( "," );
	print_value( _GLboolean, &blue );
	print( "," );
	print_value( _GLboolean, &alpha );
	END;
	GLV.glColorMask( red, green, blue, alpha );
}


/* */
void GLAPIENTRY
glColorMaterial ( GLenum face, GLenum mode )
{
	START( glColorMaterial );
	print_value( _GLenum, &face );
	print( "," );
	print_value( _GLenum, &mode );
	END;
	GLV.glColorMaterial( face, mode );
}


/* */
void GLAPIENTRY
glColorPointer ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
	START( glColorPointer );
	print_value( _GLint, &size );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLsizei, &stride );
	print( "," );
	print_value( _GLvoid, &pointer );
	END;
	GLV.glColorPointer( size, type, stride, pointer );
}


/* */
void GLAPIENTRY
glCopyPixels ( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type )
{
	START( glCopyPixels );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLsizei, &height );
	print( "," );
	print_value( _GLenum, &type );
	END;
	GLV.glCopyPixels( x, y, width, height, type );
}


/* */
void GLAPIENTRY
glCopyTexImage1D ( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border )
{
	START( glCopyTexImage1D );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLenum, &internalFormat );
	print( "," );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLint, &border );
	END;
	GLV.glCopyTexImage1D( target, level, internalFormat, x, y, width, border );
}


/* */
void GLAPIENTRY
glCopyTexImage2D
(
	GLenum	target,
	GLint	level,
	GLenum	internalFormat,
	GLint	x,
	GLint	y,
	GLsizei width,
	GLsizei height,
	GLint	border
)
{
	START( glCopyTexImage2D );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLenum, &internalFormat );
	print( "," );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLsizei, &height );
	print( "," );
	print_value( _GLint, &border );
	END;
	GLV.glCopyTexImage2D( target, level, internalFormat, x, y, width, height, border );
}


/* */
void GLAPIENTRY
glCopyTexSubImage1D ( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width )
{
	START( glCopyTexSubImage1D );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLint, &xoffset );
	print( "," );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLsizei, &width );
	END;
	GLV.glCopyTexSubImage1D( target, level, xoffset, x, y, width );
}


/* */
void GLAPIENTRY
glCopyTexSubImage2D
(
	GLenum	target,
	GLint	level,
	GLint	xoffset,
	GLint	yoffset,
	GLint	x,
	GLint	y,
	GLsizei width,
	GLsizei height
)
{
	START( glCopyTexSubImage2D );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLint, &xoffset );
	print( "," );
	print_value( _GLint, &yoffset );
	print( "," );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLsizei, &height );
	END;
	GLV.glCopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height );
}


/* */
void GLAPIENTRY
glCullFace ( GLenum mode )
{
	START( glCullFace );
	print_value( _GLenum, &mode );
	END;
	GLV.glCullFace( mode );
}


/* */
void GLAPIENTRY
glDeleteLists ( GLuint list, GLsizei range )
{
	START( glDeleteLists );
	print_value( _GLuint, &list );
	print( "," );
	print_value( _GLsizei, &range );
	END;
	GLV.glDeleteLists( list, range );
}


/* */
void GLAPIENTRY
glDeleteTextures ( GLsizei n, const GLuint *textures )
{
	START( glDeleteTextures );
	print_value( _GLsizei, &n );
	print( "," );
	print_value( _GLvoid, &textures );
	END;
	GLV.glDeleteTextures( n, textures );
}


/* */
void GLAPIENTRY
glDepthFunc ( GLenum func )
{
	START( glDepthFunc );
	print_value( _GLenum, &func );
	END;
	GLV.glDepthFunc( func );
}


/* */
void GLAPIENTRY
glDepthMask ( GLboolean flag )
{
	START( glDepthMask );
	print_value( _GLboolean, &flag );
	END;
	GLV.glDepthMask( flag );
}


/* */
void GLAPIENTRY
glDepthRange ( GLclampd zNear, GLclampd zFar )
{
	START( glDepthRange );
	print_value( _GLclampd, &zNear );
	print( "," );
	print_value( _GLclampd, &zFar );
	END;
	GLV.glDepthRange( zNear, zFar );
}


/* */
void GLAPIENTRY
glDisable ( GLenum cap )
{
	START( glDisable );
	print_value( _GLenum, &cap );
	END;
	GLV.glDisable( cap );
}


/* */
void GLAPIENTRY
glDisableClientState ( GLenum array )
{
	START( glDisableClientState );
	print_value( _GLenum, &array );
	END;
	GLV.glDisableClientState( array );
}


/* */
void GLAPIENTRY
glDrawArrays ( GLenum mode, GLint first, GLsizei count )
{
	START( glDrawArrays );
	print_value( _GLenum, &mode );
	print( "," );
	print_value( _GLint, &first );
	print( "," );
	print_value( _GLsizei, &count );
	END;
	GLV.glDrawArrays( mode, first, count );
}


/* */
void GLAPIENTRY
glDrawBuffer ( GLenum mode )
{
	START( glDrawBuffer );
	print_value( _GLenum, &mode );
	END;
	GLV.glDrawBuffer( mode );
}


/* */
void GLAPIENTRY
glDrawElements ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
	unsigned int	mode2 = mode;
	EnableTransWalls(); // Trans walls
	EnableWireFrame();	// Duh!
	START( glDrawElements );
	print_value( _GLenum, &mode );
	print( "," );
	print_value( _GLsizei, &count );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLvoid, &indices );
	END;
	GLV.glDrawElements( mode, count, type, indices );
}


/* */
void GLAPIENTRY
glDrawPixels ( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
	START( glDrawPixels );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLsizei, &height );
	print( "," );
	print_value( _GLenum, &format );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLvoid, &pixels );
	END;
	GLV.glDrawPixels( width, height, format, type, pixels );
}


/* */
void GLAPIENTRY
glEdgeFlag ( GLboolean flag )
{
	START( glEdgeFlag );
	print_value( _GLboolean, &flag );
	END;
	GLV.glEdgeFlag( flag );
}


/* */
void GLAPIENTRY
glEdgeFlagPointer ( GLsizei stride, const GLvoid *pointer )
{
	START( glEdgeFlagPointer );
	print_value( _GLsizei, &stride );
	print( "," );
	print_value( _GLvoid, &pointer );
	END;
	GLV.glEdgeFlagPointer( stride, pointer );
}


/* */
void GLAPIENTRY
glEdgeFlagv ( const GLboolean *flag )
{
	START( glEdgeFlagv );
	print_value( _GLboolean, &flag );
	END;
	GLV.glEdgeFlagv( flag );
}


/* */
void GLAPIENTRY
glEnable ( GLenum cap )
{
	START( glEnable );
	print_value( _GLenum, &cap );
	END;
	GLV.glEnable( cap );

	//	GLV.glEnable (GL_LIGHTING);
	//	GLV.glEnable (GL_LIGHTING_BIT);
	//	GLV.glEnable (GL_BLEND_COLOR);
	//	GLV.glEnable (GL_COLOR_MATRIX);
	GLV.glEnable( GL_EXTENSIONS );

	//	GLV.glEnable (GL_INTENSITY16);
	//	GLV.glEnable (GL_LUMINANCE16_ALPHA16);
	//	GLV.glEnable (GL_TEXTURE_BINDING_3D);
	//	GLV.glEnable (GL_TEXTURE_3D);
	GLV.glEnable( GL_TEXTURE_GEN_MODE );
	GLV.glEnable( GL_OBJECT_PLANE );
	GLV.glEnable( GL_EYE_PLANE );
	GLV.glEnable( GL_REPEAT );
	GLV.glEnable( GL_MODULATE );

	//	GLV.glEnable (GL_TEXTURE_DEPTH);
	//	GLV.glEnable (GL_TEXTURE_ENV_COLOR);
	GLV.glEnable( GL_NICEST );

	//GLV.glEnable (GL_LIGHT0->7);
	GLV.glEnable( GL_QUADRATIC_ATTENUATION );
	GLV.glEnable( GL_AMBIENT );
	GLV.glEnable( GL_SHININESS );
	GLV.glEnable( GL_EMISSION );
	GLV.glEnable( GL_SMOOTH );
	GLV.glEnable( GL_AMBIENT_AND_DIFFUSE );

	//	GLV.glEnable (GL_POLYGON_SMOOTH);
	//	GLV.glEnable (GL_4D_COLOR_TEXTURE);
	GLV.glEnable( GL_EXP );
	GLV.glEnable( GL_EXP2 );

	//	GLV.glEnable (GL_LOGIC_OP);
	//	GLV.glEnable (GL_INDEX_LOGIC_OP);
	//	GLV.glEnable (GL_COLOR_LOGIC_OP);
	//	GLV.glEnable (GL_LOGIC_OP_MODE);
	GLV.glEnable( GL_LUMINANCE );
}


/* */
void GLAPIENTRY
glEnableClientState ( GLenum array )
{
	START( glEnableClientState );
	print_value( _GLenum, &array );
	END;
	GLV.glEnableClientState( array );
}


/* */
void GLAPIENTRY
glEnd ( void )
{
	START( glEnd );
	END;
	GLV.glEnd();;
}


/* */
void GLAPIENTRY
glEndList ( void )
{
	START( glEndList );
	END;
	GLV.glEndList();;
}


/* */
void GLAPIENTRY
glEvalCoord1d ( GLdouble u )
{
	START( glEvalCoord1d );
	print_value( _GLdouble, &u );
	END;
	GLV.glEvalCoord1d( u );
}


/* */
void GLAPIENTRY
glEvalCoord1dv ( const GLdouble *u )
{
	START( glEvalCoord1dv );
	print_value( _GLdouble1, &u );
	END;
	GLV.glEvalCoord1dv( u );
}


/* */
void GLAPIENTRY
glEvalCoord1f ( GLfloat u )
{
	START( glEvalCoord1f );
	print_value( _GLfloat, &u );
	END;
	GLV.glEvalCoord1f( u );
}


/* */
void GLAPIENTRY
glEvalCoord1fv ( const GLfloat *u )
{
	START( glEvalCoord1fv );
	print_value( _GLfloat1, &u );
	END;
	GLV.glEvalCoord1fv( u );
}


/* */
void GLAPIENTRY
glEvalCoord2d ( GLdouble u, GLdouble v )
{
	START( glEvalCoord2d );
	print_value( _GLdouble, &u );
	print( "," );
	print_value( _GLdouble, &v );
	END;
	GLV.glEvalCoord2d( u, v );
}


/* */
void GLAPIENTRY
glEvalCoord2dv ( const GLdouble *u )
{
	START( glEvalCoord2dv );
	print_value( _GLdouble2, &u );
	END;
	GLV.glEvalCoord2dv( u );
}


/* */
void GLAPIENTRY
glEvalCoord2f ( GLfloat u, GLfloat v )
{
	START( glEvalCoord2f );
	print_value( _GLfloat, &u );
	print( "," );
	print_value( _GLfloat, &v );
	END;
	GLV.glEvalCoord2f( u, v );
}


/* */
void GLAPIENTRY
glEvalCoord2fv ( const GLfloat *u )
{
	START( glEvalCoord2fv );
	print_value( _GLfloat2, &u );
	END;
	GLV.glEvalCoord2fv( u );
}


/* */
void GLAPIENTRY
glEvalMesh1 ( GLenum mode, GLint i1, GLint i2 )
{
	START( glEvalMesh1 );
	print_value( _GLenum, &mode );
	print( "," );
	print_value( _GLint, &i1 );
	print( "," );
	print_value( _GLint, &i2 );
	END;
	GLV.glEvalMesh1( mode, i1, i2 );
}


/* */
void GLAPIENTRY
glEvalMesh2 ( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 )
{
	START( glEvalMesh2 );
	print_value( _GLenum, &mode );
	print( "," );
	print_value( _GLint, &i1 );
	print( "," );
	print_value( _GLint, &i2 );
	print( "," );
	print_value( _GLint, &j1 );
	print( "," );
	print_value( _GLint, &j2 );
	END;
	GLV.glEvalMesh2( mode, i1, i2, j1, j2 );
}


/* */
void GLAPIENTRY
glEvalPoint1 ( GLint i )
{
	START( glEvalPoint1 );
	print_value( _GLint, &i );
	END;
	GLV.glEvalPoint1( i );
}


/* */
void GLAPIENTRY
glEvalPoint2 ( GLint i, GLint j )
{
	START( glEvalPoint2 );
	print_value( _GLint, &i );
	print( "," );
	print_value( _GLint, &j );
	END;
	GLV.glEvalPoint2( i, j );
}


/* */
void GLAPIENTRY
glFeedbackBuffer ( GLsizei size, GLenum type, GLfloat *buffer )
{
	START( glFeedbackBuffer );
	print_value( _GLsizei, &size );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLvoid, &buffer );
	END;
	GLV.glFeedbackBuffer( size, type, buffer );
}


/* */
void GLAPIENTRY
glFinish ( void )
{
	if ( bFSPenable == GL_TRUE )
	{
		renderFPS();
	}

	if ( bTimeEnabled == GL_TRUE )
	{
		renderTIME();
	}

	START( glFinish );
	END;
	GLV.glFinish();
	updateKeys();
}


/* */
void GLAPIENTRY
glFlush ( void )
{
	if ( bFSPenable == GL_TRUE )
	{
		renderFPS();
	}

	if ( bTimeEnabled == GL_TRUE )
	{
		renderTIME();
	}

	START( glFlush );
	END;
	GLV.glFlush();
	updateKeys();
}


/* */
void GLAPIENTRY
glFogf ( GLenum pname, GLfloat param )
{
	START( glFogf );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLfloat, &param );
	END;
	GLV.glFogf( pname, param );
}


/* */
void GLAPIENTRY
glFogfv ( GLenum pname, const GLfloat *params )
{
	START( glFogfv );
	print_value( _GLenum, &pname );
	print( "," );
	if ( pname == GL_FOG_COLOR )
	{
		print_value( _GLfloat4, &params );
	}
	else
	{
		print_value( _GLfloat1, &params );
	}

	END;
	GLV.glFogfv( pname, params );
}


/* */
void GLAPIENTRY
glFogi ( GLenum pname, GLint param )
{
	START( glFogi );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLint, &param );
	END;
	GLV.glFogi( pname, param );
}


/* */
void GLAPIENTRY
glFogiv ( GLenum pname, const GLint *params )
{
	START( glFogiv );
	print_value( _GLenum, &pname );
	print( "," );
	if ( pname == GL_FOG_COLOR )
	{
		print_value( _GLint4, &params );
	}
	else
	{
		print_value( _GLint1, &params );
	}

	END;
	GLV.glFogiv( pname, params );
}


/* */
void GLAPIENTRY
glFrontFace ( GLenum mode )
{
	START( glFrontFace );
	print_value( _GLenum, &mode );
	END;
	GLV.glFrontFace( mode );
}


/* */
void GLAPIENTRY
glFrustum ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
	START( glFrustum );
	print_value( _GLdouble, &left );
	print( "," );
	print_value( _GLdouble, &right );
	print( "," );
	print_value( _GLdouble, &bottom );
	print( "," );
	print_value( _GLdouble, &top );
	print( "," );
	print_value( _GLdouble, &zNear );
	print( "," );
	print_value( _GLdouble, &zFar );
	END;
	GLV.glFrustum( left, right, bottom, top, zNear, zFar );
}


/* */
GLuint GLAPIENTRY
glGenLists ( GLsizei range )
{
	GLuint	result;
	result = GLV.glGenLists( range );
	START( glGenLists );
	print_value( _GLsizei, &range );
	RESULT;
	print_value( _GLuint, &result );
	ENDRESULT;
	return ( result );
}


/* */
void GLAPIENTRY
glGenTextures ( GLsizei n, GLuint *textures )
{
	START( glGenTextures );
	print_value( _GLsizei, &n );
	print( "," );
	print_value( _GLvoid, &textures );
	END;
	GLV.glGenTextures( n, textures );
}


/* */
void GLAPIENTRY
glGetBooleanv ( GLenum pname, GLboolean *params )
{
	GLV.glGetBooleanv( pname, params );
	START( glGetBooleanv );
	print_value( _GLenum, &pname );
	print( "," );
	if ( pname == GL_COLOR_WRITEMASK )
	{
		print_value( _GLboolean4, &params );
	}
	else
	{
		print_value( _GLboolean1, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetClipPlane ( GLenum plane, GLdouble *equation )
{
	GLV.glGetClipPlane( plane, equation );
	START( glGetClipPlane );
	print_value( _GLenum, &plane );
	print( "," );
	print_value( _GLdouble4, &equation );
	END;
}


/* */
void GLAPIENTRY
glGetDoublev ( GLenum pname, GLdouble *params )
{
	GLV.glGetDoublev( pname, params );
	START( glGetDoublev );
	print_value( _GLenum, &pname );
	print( "," );
	if ( (pname == GL_MODELVIEW_MATRIX) || (pname == GL_PROJECTION_MATRIX) || (pname == GL_TEXTURE_MATRIX) )
	{
		print_value( _GLdouble16, &params );
	}
	else if
		(
			(pname == GL_ACCUM_CLEAR_VALUE) ||
			(pname == GL_COLOR_CLEAR_VALUE) ||
			(pname == GL_CURRENT_COLOR) ||
			(pname == GL_CURRENT_RASTER_COLOR) ||
			(pname == GL_CURRENT_RASTER_POSITION) ||
			(pname == GL_CURRENT_RASTER_TEXTURE_COORDS) ||
			(pname == GL_CURRENT_TEXTURE_COORDS) ||
			(pname == GL_FOG_COLOR) ||
			(pname == GL_LIGHT_MODEL_AMBIENT) ||
			(pname == GL_MAP2_GRID_DOMAIN) ||
			(pname == GL_SCISSOR_BOX) ||
			(pname == GL_VIEWPORT)
		)
	{
		print_value( _GLdouble4, &params );
	}
	else if
		(
			(pname == GL_DEPTH_RANGE) ||
			(pname == GL_LINE_WIDTH_RANGE) ||
			(pname == GL_MAP1_GRID_DOMAIN) ||
			(pname == GL_MAP2_GRID_SEGMENTS) ||
			(pname == GL_MAX_VIEWPORT_DIMS) ||
			(pname == GL_POINT_SIZE_RANGE) ||
			(pname == GL_POLYGON_MODE)
		)
	{
		print_value( _GLdouble2, &params );
	}
	else
	{
		print_value( _GLdouble1, &params );
	}

	END;
}


/* */
GLenum GLAPIENTRY
glGetError ( void )
{
	GLenum	result;
	result = GLV.glGetError();
	START( glGetError );
	RESULT;
	print_value( _GLenum, &result );
	ENDRESULT;
	return ( result );
}


/* */
void GLAPIENTRY
glGetFloatv ( GLenum pname, GLfloat *params )
{
	GLV.glGetFloatv( pname, params );
	START( glGetFloatv );
	print_value( _GLenum, &pname );
	print( "," );
	if ( (pname == GL_MODELVIEW_MATRIX) || (pname == GL_PROJECTION_MATRIX) || (pname == GL_TEXTURE_MATRIX) )
	{
		print_value( _GLfloat16, &params );
	}
	else if
		(
			(pname == GL_ACCUM_CLEAR_VALUE) ||
			(pname == GL_COLOR_CLEAR_VALUE) ||
			(pname == GL_CURRENT_COLOR) ||
			(pname == GL_CURRENT_RASTER_COLOR) ||
			(pname == GL_CURRENT_RASTER_POSITION) ||
			(pname == GL_CURRENT_RASTER_TEXTURE_COORDS) ||
			(pname == GL_CURRENT_TEXTURE_COORDS) ||
			(pname == GL_FOG_COLOR) ||
			(pname == GL_LIGHT_MODEL_AMBIENT) ||
			(pname == GL_MAP2_GRID_DOMAIN) ||
			(pname == GL_SCISSOR_BOX) ||
			(pname == GL_VIEWPORT)
		)
	{
		print_value( _GLfloat4, &params );
	}
	else if
		(
			(pname == GL_DEPTH_RANGE) ||
			(pname == GL_LINE_WIDTH_RANGE) ||
			(pname == GL_MAP1_GRID_DOMAIN) ||
			(pname == GL_MAP2_GRID_SEGMENTS) ||
			(pname == GL_MAX_VIEWPORT_DIMS) ||
			(pname == GL_POINT_SIZE_RANGE) ||
			(pname == GL_POLYGON_MODE)
		)
	{
		print_value( _GLfloat2, &params );
	}
	else
	{
		print_value( _GLfloat1, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetIntegerv ( GLenum pname, GLint *params )
{
	GLV.glGetIntegerv( pname, params );
	START( glGetIntegerv );
	print_value( _GLenum, &pname );
	print( "," );
	if
	(
		(pname == GL_ACCUM_CLEAR_VALUE) ||
		(pname == GL_COLOR_CLEAR_VALUE) ||
		(pname == GL_CURRENT_COLOR) ||
		(pname == GL_CURRENT_RASTER_COLOR) ||
		(pname == GL_CURRENT_RASTER_POSITION) ||
		(pname == GL_CURRENT_RASTER_TEXTURE_COORDS) ||
		(pname == GL_CURRENT_TEXTURE_COORDS) ||
		(pname == GL_FOG_COLOR) ||
		(pname == GL_LIGHT_MODEL_AMBIENT) ||
		(pname == GL_MAP2_GRID_DOMAIN) ||
		(pname == GL_SCISSOR_BOX) ||
		(pname == GL_VIEWPORT)
	)
	{
		print_value( _GLint4, &params );
	}
	else if
		(
			(pname == GL_DEPTH_RANGE) ||
			(pname == GL_LINE_WIDTH_RANGE) ||
			(pname == GL_MAP1_GRID_DOMAIN) ||
			(pname == GL_MAP2_GRID_SEGMENTS) ||
			(pname == GL_MAX_VIEWPORT_DIMS) ||
			(pname == GL_POINT_SIZE_RANGE) ||
			(pname == GL_POLYGON_MODE)
		)
	{
		print_value( _GLint2, &params );
	}
	else
	{
		print_value( _GLint1, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetLightfv ( GLenum light, GLenum pname, GLfloat *params )
{
	GLV.glGetLightfv( light, pname, params );
	START( glGetLightfv );
	print_value( _GLenum, &light );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	if ( (pname == GL_AMBIENT) || (pname == GL_DIFFUSE) || (pname == GL_SPECULAR) || (pname == GL_POSITION) )
	{
		print_value( _GLfloat4, &params );
	}
	else if ( pname == GL_SPOT_DIRECTION )
	{
		print_value( _GLfloat3, &params );
	}
	else
	{
		print_value( _GLfloat1, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetLightiv ( GLenum light, GLenum pname, GLint *params )
{
	GLV.glGetLightiv( light, pname, params );
	START( glGetLightiv );
	print_value( _GLenum, &light );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	if ( (pname == GL_AMBIENT) || (pname == GL_DIFFUSE) || (pname == GL_SPECULAR) || (pname == GL_POSITION) )
	{
		print_value( _GLint4, &params );
	}
	else if ( pname == GL_SPOT_DIRECTION )
	{
		print_value( _GLint3, &params );
	}
	else
	{
		print_value( _GLint1, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetMapdv ( GLenum target, GLenum query, GLdouble *v )
{
	GLV.glGetMapdv( target, query, v );
	START( glGetMapdv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &query );
	print( "," );
	switch ( query )
	{
		case GL_MAP1_INDEX:
		case GL_MAP1_TEXTURE_COORD_1:
			print_value( _GLdouble1, &v );
			break;
		case GL_MAP1_TEXTURE_COORD_2:
			print_value( _GLdouble2, &v );
			break;
		case GL_MAP1_VERTEX_3:
		case GL_MAP1_NORMAL:
		case GL_MAP1_TEXTURE_COORD_3:
			print_value( _GLdouble3, &v );
			break;
		case GL_MAP1_VERTEX_4:
		case GL_MAP1_COLOR_4:
		case GL_MAP1_TEXTURE_COORD_4:
			print_value( _GLdouble4, &v );
			break;
	}

	END;
}


/* */
void GLAPIENTRY
glGetMapfv ( GLenum target, GLenum query, GLfloat *v )
{
	GLV.glGetMapfv( target, query, v );
	START( glGetMapfv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &query );
	print( "," );
	switch ( query )
	{
		case GL_MAP1_INDEX:
		case GL_MAP1_TEXTURE_COORD_1:
		case GL_MAP2_INDEX:
		case GL_MAP2_TEXTURE_COORD_1:
			print_value( _GLfloat1, &v );
			break;
		case GL_MAP1_TEXTURE_COORD_2:
		case GL_MAP2_TEXTURE_COORD_2:
			print_value( _GLfloat2, &v );
			break;
		case GL_MAP1_VERTEX_3:
		case GL_MAP1_NORMAL:
		case GL_MAP1_TEXTURE_COORD_3:
		case GL_MAP2_VERTEX_3:
		case GL_MAP2_NORMAL:
		case GL_MAP2_TEXTURE_COORD_3:
			print_value( _GLfloat3, &v );
			break;
		case GL_MAP1_VERTEX_4:
		case GL_MAP1_COLOR_4:
		case GL_MAP1_TEXTURE_COORD_4:
		case GL_MAP2_VERTEX_4:
		case GL_MAP2_COLOR_4:
		case GL_MAP2_TEXTURE_COORD_4:
			print_value( _GLfloat4, &v );
			break;
	}

	END;
}


/* */
void GLAPIENTRY
glGetMapiv ( GLenum target, GLenum query, GLint *v )
{
	GLV.glGetMapiv( target, query, v );
	START( glGetMapiv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &query );
	print( "," );
	switch ( query )
	{
		case GL_MAP1_INDEX:
		case GL_MAP1_TEXTURE_COORD_1:
		case GL_MAP2_INDEX:
		case GL_MAP2_TEXTURE_COORD_1:
			print_value( _GLint1, &v );
			break;
		case GL_MAP1_TEXTURE_COORD_2:
		case GL_MAP2_TEXTURE_COORD_2:
			print_value( _GLint2, &v );
			break;
		case GL_MAP1_VERTEX_3:
		case GL_MAP1_NORMAL:
		case GL_MAP1_TEXTURE_COORD_3:
		case GL_MAP2_VERTEX_3:
		case GL_MAP2_NORMAL:
		case GL_MAP2_TEXTURE_COORD_3:
			print_value( _GLint3, &v );
			break;
		case GL_MAP1_VERTEX_4:
		case GL_MAP1_COLOR_4:
		case GL_MAP1_TEXTURE_COORD_4:
		case GL_MAP2_VERTEX_4:
		case GL_MAP2_COLOR_4:
		case GL_MAP2_TEXTURE_COORD_4:
			print_value( _GLint4, &v );
			break;
	}

	END;
}


/* */
void GLAPIENTRY
glGetMaterialfv ( GLenum face, GLenum pname, GLfloat *params )
{
	GLV.glGetMaterialfv( face, pname, params );
	START( glGetMaterialfv );
	print_value( _GLenum, &face );
	print( "," );
	print_value( _GLenum, &pname );
	if ( pname == GL_SHININESS )
	{
		print_value( _GLfloat1, &params );
	}
	else if ( pname == GL_COLOR_INDEXES )
	{
		print_value( _GLfloat3, &params );
	}
	else
	{
		print_value( _GLfloat4, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetMaterialiv ( GLenum face, GLenum pname, GLint *params )
{
	GLV.glGetMaterialiv( face, pname, params );
	START( glGetMaterialiv );
	print_value( _GLenum, &face );
	print( "," );
	print_value( _GLenum, &pname );
	if ( pname == GL_SHININESS )
	{
		print_value( _GLint1, &params );
	}
	else if ( pname == GL_COLOR_INDEXES )
	{
		print_value( _GLint3, &params );
	}
	else
	{
		print_value( _GLint4, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetPixelMapfv ( GLenum map, GLfloat *values )
{
	GLV.glGetPixelMapfv( map, values );
	START( glGetPixelMapfv );
	print_value( _GLenum, &map );
	print( "," );
	print_value( _GLvoid, &values );
	END;
}


/* */
void GLAPIENTRY
glGetPixelMapuiv ( GLenum map, GLuint *values )
{
	GLV.glGetPixelMapuiv( map, values );
	START( glGetPixelMapuiv );
	print_value( _GLenum, &map );
	print( "," );
	print_value( _GLvoid, &values );
	END;
}


/* */
void GLAPIENTRY
glGetPixelMapusv ( GLenum map, GLushort *values )
{
	GLV.glGetPixelMapusv( map, values );
	START( glGetPixelMapusv );
	print_value( _GLenum, &map );
	print( "," );
	print_value( _GLvoid, &values );
	END;
}


/* */
void GLAPIENTRY
glGetPointerv ( GLenum pname, GLvoid **params )
{
	GLV.glGetPointerv( pname, params );
	START( glGetPointerv );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLvoid, &params );
	END;
}


/* */
void GLAPIENTRY
glGetPolygonStipple ( GLubyte *mask )
{
	GLV.glGetPolygonStipple( mask );
	START( glGetPolygonStipple );
	print_value( _GLvoid, &mask );
	END;
}

const GLubyte * GLAPIENTRY glGetString ( GLenum name )
{
	const GLubyte	*result;
	result = GLV.glGetString( name );
	START( glGetString );
	print_value( _GLenum, &name );
	RESULT;
	print_value( STR, &result );
	ENDRESULT;
	return ( result );
}


/* */
void GLAPIENTRY
glGetTexEnvfv ( GLenum target, GLenum pname, GLfloat *params )
{
	GLV.glGetTexEnvfv( target, pname, params );
	START( glGetTexEnvfv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &target );
	print( "," );
	if ( pname == GL_TEXTURE_ENV_COLOR )
	{
		print_value( _GLfloat4, &params );
	}
	else
	{
		print_value( _GLfloat1, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetTexEnviv ( GLenum target, GLenum pname, GLint *params )
{
	GLV.glGetTexEnviv( target, pname, params );
	START( glGetTexEnviv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &target );
	print( "," );
	if ( pname == GL_TEXTURE_ENV_COLOR )
	{
		print_value( _GLint4, &params );
	}
	else
	{
		print_value( _GLint1, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetTexGendv ( GLenum coord, GLenum pname, GLdouble *params )
{
	GLV.glGetTexGendv( coord, pname, params );
	START( glGetTexGendv );
	print_value( _GLenum, &coord );
	print( "," );
	print_value( _GLenum, &pname );
	if ( pname == GL_TEXTURE_GEN_MODE )
	{
		print_value( _GLdouble1, &params );
	}
	else
	{
		print_value( _GLdouble4, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetTexGenfv ( GLenum coord, GLenum pname, GLfloat *params )
{
	GLV.glGetTexGenfv( coord, pname, params );
	START( glGetTexGenfv );
	print_value( _GLenum, &coord );
	print( "," );
	print_value( _GLenum, &pname );
	if ( pname == GL_TEXTURE_GEN_MODE )
	{
		print_value( _GLfloat1, &params );
	}
	else
	{
		print_value( _GLfloat4, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetTexGeniv ( GLenum coord, GLenum pname, GLint *params )
{
	GLV.glGetTexGeniv( coord, pname, params );
	START( glGetTexGeniv );
	print_value( _GLenum, &coord );
	print( "," );
	print_value( _GLenum, &pname );
	if ( pname == GL_TEXTURE_GEN_MODE )
	{
		print_value( _GLint1, &params );
	}
	else
	{
		print_value( _GLint4, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetTexImage ( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels )
{
	GLV.glGetTexImage( target, level, format, type, pixels );
	START( glGetTexImage );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLenum, &format );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLvoid, &pixels );
	END;
}


/* */
void GLAPIENTRY
glGetTexLevelParameterfv ( GLenum target, GLint level, GLenum pname, GLfloat *params )
{
	GLV.glGetTexLevelParameterfv( target, level, pname, params );
	START( glGetTexLevelParameterfv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLfloat1, &params );
	END;
}


/* */
void GLAPIENTRY
glGetTexLevelParameteriv ( GLenum target, GLint level, GLenum pname, GLint *params )
{
	GLV.glGetTexLevelParameteriv( target, level, pname, params );
	START( glGetTexLevelParameteriv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLint1, &params );
	END;
}


/* */
void GLAPIENTRY
glGetTexParameterfv ( GLenum target, GLenum pname, GLfloat *params )
{
	GLV.glGetTexParameterfv( target, pname, params );
	START( glGetTexParameterfv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	if ( pname == GL_TEXTURE_BORDER_COLOR )
	{
		print_value( _GLfloat4, &params );
	}
	else
	{
		print_value( _GLfloat1, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glGetTexParameteriv ( GLenum target, GLenum pname, GLint *params )
{
	GLV.glGetTexParameteriv( target, pname, params );
	START( glGetTexParameteriv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	if ( pname == GL_TEXTURE_BORDER_COLOR )
	{
		print_value( _GLint4, &params );
	}
	else
	{
		print_value( _GLint1, &params );
	}

	END;
}


/* */
void GLAPIENTRY
glHint ( GLenum target, GLenum mode )
{
	START( glHint );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &mode );
	END;
	GLV.glHint( target, mode );
}


/* */
void GLAPIENTRY
glIndexMask ( GLuint mask )
{
	START( glIndexMask );
	print_value( _GLuint, &mask );
	END;
	GLV.glIndexMask( mask );
}


/* */
void GLAPIENTRY
glIndexPointer ( GLenum type, GLsizei stride, const GLvoid *pointer )
{
	START( glIndexPointer );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLsizei, &stride );
	print( "," );
	print_value( _GLvoid, &pointer );
	END;
	GLV.glIndexPointer( type, stride, pointer );
}


/* */
void GLAPIENTRY
glIndexd ( GLdouble c )
{
	START( glIndexd );
	print_value( _GLdouble, &c );
	END;
	GLV.glIndexd( c );
}


/* */
void GLAPIENTRY
glIndexdv ( const GLdouble *c )
{
	START( glIndexdv );
	print_value( _GLdouble1, &c );
	END;
	GLV.glIndexdv( c );
}


/* */
void GLAPIENTRY
glIndexf ( GLfloat c )
{
	START( glIndexf );
	print_value( _GLfloat, &c );
	END;
	GLV.glIndexf( c );
}


/* */
void GLAPIENTRY
glIndexfv ( const GLfloat *c )
{
	START( glIndexfv );
	print_value( _GLfloat1, &c );
	END;
	GLV.glIndexfv( c );
}


/* */
void GLAPIENTRY
glIndexi ( GLint c )
{
	START( glIndexi );
	print_value( _GLint, &c );
	END;
	GLV.glIndexi( c );
}


/* */
void GLAPIENTRY
glIndexiv ( const GLint *c )
{
	START( glIndexiv );
	print_value( _GLint1, &c );
	END;
	GLV.glIndexiv( c );
}


/* */
void GLAPIENTRY
glIndexs ( GLshort c )
{
	START( glIndexs );
	print_value( _GLshort, &c );
	END;
	GLV.glIndexs( c );
}


/* */
void GLAPIENTRY
glIndexsv ( const GLshort *c )
{
	START( glIndexsv );
	print_value( _GLshort1, &c );
	END;
	GLV.glIndexsv( c );
}


/* */
void GLAPIENTRY
glIndexub ( GLubyte c )
{
	START( glIndexub );
	print_value( _GLubyte, &c );
	END;
	GLV.glIndexub( c );
}


/* */
void GLAPIENTRY
glIndexubv ( const GLubyte *c )
{
	START( glIndexubv );
	print_value( _GLubyte1, &c );
	END;
	GLV.glIndexubv( c );
}


/* */
void GLAPIENTRY
glInitNames ( void )
{
	START( glInitNames );
	END;
	GLV.glInitNames();
}


/* */
void GLAPIENTRY
glInterleavedArrays ( GLenum format, GLsizei stride, const GLvoid *pointer )
{
	START( glInterleavedArrays );
	print_value( _GLenum, &format );
	print( "," );
	print_value( _GLsizei, &stride );
	print( "," );
	print_value( _GLvoid, &pointer );
	END;
	GLV.glInterleavedArrays( format, stride, pointer );
}


/* */
GLboolean GLAPIENTRY
glIsEnabled ( GLenum cap )
{
	GLboolean	result;
	result = GLV.glIsEnabled( cap );
	START( glIsEnabled );
	print_value( _GLenum, &cap );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
GLboolean GLAPIENTRY
glIsList ( GLuint list )
{
	GLboolean	result;
	result = GLV.glIsList( list );
	START( glIsEnabled );
	print_value( _GLuint, &list );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
GLboolean GLAPIENTRY
glIsTexture ( GLuint texture )
{
	GLboolean	result;
	result = GLV.glIsTexture( texture );
	START( glIsEnabled );
	print_value( _GLuint, &texture );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
void GLAPIENTRY
glLightModelf ( GLenum pname, GLfloat param )
{
	START( glLightModelf );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLfloat, &param );
	END;
	GLV.glLightModelf( pname, param );
}


/* */
void GLAPIENTRY
glLightModelfv ( GLenum pname, const GLfloat *params )
{
	START( glLightModelfv );
	print_value( _GLenum, &pname );
	print( "," );
	if ( pname == GL_LIGHT_MODEL_AMBIENT )
	{
		print_value( _GLfloat4, &params );
	}
	else
	{
		print_value( _GLfloat1, &params );
	}

	END;
	GLV.glLightModelfv( pname, params );
}


/* */
void GLAPIENTRY
glLightModeli ( GLenum pname, GLint param )
{
	START( glLightModeli );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLint, &param );
	END;
	GLV.glLightModeli( pname, param );
}


/* */
void GLAPIENTRY
glLightModeliv ( GLenum pname, const GLint *params )
{
	START( glLightModeliv );
	print_value( _GLenum, &pname );
	print( "," );
	if ( pname == GL_LIGHT_MODEL_AMBIENT )
	{
		print_value( _GLint4, &params );
	}
	else
	{
		print_value( _GLint1, &params );
	}

	END;
	GLV.glLightModeliv( pname, params );
}


/* */
void GLAPIENTRY
glLightf ( GLenum light, GLenum pname, GLfloat param )
{
	START( glLightf );
	print_value( _GLenum, &light );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLfloat, &param );
	END;
	GLV.glLightf( light, pname, param );
}


/* */
void GLAPIENTRY
glLightfv ( GLenum light, GLenum pname, const GLfloat *params )
{
	START( glLightf );
	print_value( _GLenum, &light );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	if ( (pname == GL_AMBIENT) || (pname == GL_DIFFUSE) || (pname == GL_SPECULAR) || (pname == GL_POSITION) )
	{
		print_value( _GLfloat4, &params );
	}
	else if ( pname == GL_SPOT_DIRECTION )
	{
		print_value( _GLfloat3, &params );
	}
	else
	{
		print_value( _GLfloat1, &params );
	}

	END;
	GLV.glLightfv( light, pname, params );
}


/* */
void GLAPIENTRY
glLighti ( GLenum light, GLenum pname, GLint param )
{
	START( glLighti );
	print_value( _GLenum, &light );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLint, &param );
	END;
	GLV.glLighti( light, pname, param );
}


/* */
void GLAPIENTRY
glLightiv ( GLenum light, GLenum pname, const GLint *params )
{
	START( glLighti );
	print_value( _GLenum, &light );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	if ( (pname == GL_AMBIENT) || (pname == GL_DIFFUSE) || (pname == GL_SPECULAR) || (pname == GL_POSITION) )
	{
		print_value( _GLint4, &params );
	}
	else if ( pname == GL_SPOT_DIRECTION )
	{
		print_value( _GLint3, &params );
	}
	else
	{
		print_value( _GLint1, &params );
	}

	END;
	GLV.glLightiv( light, pname, params );
}


/* */
void GLAPIENTRY
glLineStipple ( GLint factor, GLushort pattern )
{
	START( glLineStipple );
	print_value( _GLint, &factor );
	print( "," );
	print_value( _GLushort, &pattern );
	END;
	GLV.glLineStipple( factor, pattern );
}


/* */
void GLAPIENTRY
glLineWidth ( GLfloat width )
{
	START( glLineWidth );
	print_value( _GLfloat, &width );
	END;
	GLV.glLineWidth( width );
}


/* */
void GLAPIENTRY
glListBase ( GLuint base )
{
	START( glListBase );
	print_value( _GLuint, &base );
	END;
	GLV.glListBase( base );
}


/* */
void GLAPIENTRY
glLoadIdentity ( void )
{
	START( glLoadIdentity );
	END;
	GLV.glLoadIdentity();
}


/* */
void GLAPIENTRY
glLoadMatrixd ( const GLdouble *m )
{
	START( glLoadMatrixd );
	print_value( _GLdouble16, &m );
	END;
	GLV.glLoadMatrixd( m );
}


/* */
void GLAPIENTRY
glLoadMatrixf ( const GLfloat *m )
{
	START( glLoadMatrixf );
	print_value( _GLfloat16, &m );
	END;
	GLV.glLoadMatrixf( m );
}


/* */
void GLAPIENTRY
glLoadName ( GLuint name )
{
	START( glLoadName );
	print_value( _GLuint, &name );
	END;
	GLV.glLoadName( name );
}


/* */
void GLAPIENTRY
glLogicOp ( GLenum opcode )
{
	START( glLogicOp );
	print_value( _GLenum, &opcode );
	END;
	GLV.glLogicOp( opcode );
}


/* */
void GLAPIENTRY
glMap1d ( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points )
{
	START( glMap1d );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLdouble, &u1 );
	print( "," );
	print_value( _GLdouble, &u2 );
	print( "," );
	print_value( _GLint, &stride );
	print( "," );
	print_value( _GLint, &order );
	print( "," );
	switch ( target )
	{
		case GL_MAP1_INDEX:
		case GL_MAP1_TEXTURE_COORD_1:
			print_value( _GLdouble1, &points );
			break;
		case GL_MAP1_TEXTURE_COORD_2:
			print_value( _GLdouble2, &points );
			break;
		case GL_MAP1_VERTEX_3:
		case GL_MAP1_NORMAL:
		case GL_MAP1_TEXTURE_COORD_3:
			print_value( _GLdouble3, &points );
			break;
		case GL_MAP1_VERTEX_4:
		case GL_MAP1_COLOR_4:
		case GL_MAP1_TEXTURE_COORD_4:
			print_value( _GLdouble4, &points );
			break;
	}

	END;
	GLV.glMap1d( target, u1, u2, stride, order, points );
}


/* */
void GLAPIENTRY
glMap1f ( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points )
{
	START( glMap1f );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLfloat, &u1 );
	print( "," );
	print_value( _GLfloat, &u2 );
	print( "," );
	print_value( _GLint, &stride );
	print( "," );
	print_value( _GLint, &order );
	print( "," );
	switch ( target )
	{
		case GL_MAP1_INDEX:
		case GL_MAP1_TEXTURE_COORD_1:
			print_value( _GLfloat1, &points );
			break;
		case GL_MAP1_TEXTURE_COORD_2:
			print_value( _GLfloat2, &points );
			break;
		case GL_MAP1_VERTEX_3:
		case GL_MAP1_NORMAL:
		case GL_MAP1_TEXTURE_COORD_3:
			print_value( _GLfloat3, &points );
			break;
		case GL_MAP1_VERTEX_4:
		case GL_MAP1_COLOR_4:
		case GL_MAP1_TEXTURE_COORD_4:
			print_value( _GLfloat4, &points );
			break;
	}

	END;
	GLV.glMap1f( target, u1, u2, stride, order, points );
}


/* */
void GLAPIENTRY
glMap2d
(
	GLenum			target,
	GLdouble		u1,
	GLdouble		u2,
	GLint			ustride,
	GLint			uorder,
	GLdouble		v1,
	GLdouble		v2,
	GLint			vstride,
	GLint			vorder,
	const GLdouble	*points
)
{
	START( glMap2d );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLdouble, &u1 );
	print( "," );
	print_value( _GLdouble, &u2 );
	print( "," );
	print_value( _GLint, &ustride );
	print( "," );
	print_value( _GLint, &uorder );
	print( "," );
	print_value( _GLdouble, &v1 );
	print( "," );
	print_value( _GLdouble, &v2 );
	print( "," );
	print_value( _GLint, &vstride );
	print( "," );
	print_value( _GLint, &vorder );
	print( "," );
	switch ( target )
	{
		case GL_MAP2_INDEX:
		case GL_MAP2_TEXTURE_COORD_1:
			print_value( _GLdouble1, &points );
			break;
		case GL_MAP2_TEXTURE_COORD_2:
			print_value( _GLdouble2, &points );
			break;
		case GL_MAP2_VERTEX_3:
		case GL_MAP2_NORMAL:
		case GL_MAP2_TEXTURE_COORD_3:
			print_value( _GLdouble3, &points );
			break;
		case GL_MAP2_VERTEX_4:
		case GL_MAP2_COLOR_4:
		case GL_MAP2_TEXTURE_COORD_4:
			print_value( _GLdouble4, &points );
			break;
	}

	END;
	GLV.glMap2d( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points );
}


/* */
void GLAPIENTRY
glMap2f
(
	GLenum			target,
	GLfloat			u1,
	GLfloat			u2,
	GLint			ustride,
	GLint			uorder,
	GLfloat			v1,
	GLfloat			v2,
	GLint			vstride,
	GLint			vorder,
	const GLfloat	*points
)
{
	START( glMap2f );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLfloat, &u1 );
	print( "," );
	print_value( _GLfloat, &u2 );
	print( "," );
	print_value( _GLint, &ustride );
	print( "," );
	print_value( _GLint, &uorder );
	print( "," );
	print_value( _GLfloat, &v1 );
	print( "," );
	print_value( _GLfloat, &v2 );
	print( "," );
	print_value( _GLint, &vstride );
	print( "," );
	print_value( _GLint, &vorder );
	print( "," );
	switch ( target )
	{
		case GL_MAP2_INDEX:
		case GL_MAP2_TEXTURE_COORD_1:
			print_value( _GLfloat1, &points );
			break;
		case GL_MAP2_TEXTURE_COORD_2:
			print_value( _GLfloat2, &points );
			break;
		case GL_MAP2_VERTEX_3:
		case GL_MAP2_NORMAL:
		case GL_MAP2_TEXTURE_COORD_3:
			print_value( _GLfloat3, &points );
			break;
		case GL_MAP2_VERTEX_4:
		case GL_MAP2_COLOR_4:
		case GL_MAP2_TEXTURE_COORD_4:
			print_value( _GLfloat4, &points );
			break;
	}

	END;
	GLV.glMap2f( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points );
}


/* */
void GLAPIENTRY
glMapGrid1d ( GLint un, GLdouble u1, GLdouble u2 )
{
	START( glMapGrid1d );
	print_value( _GLint, &un );
	print( "," );
	print_value( _GLdouble, &u1 );
	print( "," );
	print_value( _GLdouble, &u2 );
	END;
	GLV.glMapGrid1d( un, u1, u2 );
}


/* */
void GLAPIENTRY
glMapGrid1f ( GLint un, GLfloat u1, GLfloat u2 )
{
	START( glMapGrid1f );
	print_value( _GLint, &un );
	print( "," );
	print_value( _GLfloat, &u1 );
	print( "," );
	print_value( _GLfloat, &u2 );
	END;
	GLV.glMapGrid1f( un, u1, u2 );
}


/* */
void GLAPIENTRY
glMapGrid2d ( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 )
{
	START( glMapGrid1d );
	print_value( _GLint, &un );
	print( "," );
	print_value( _GLdouble, &u1 );
	print( "," );
	print_value( _GLdouble, &u2 );
	print( "," );
	print_value( _GLint, &vn );
	print( "," );
	print_value( _GLdouble, &v1 );
	print( "," );
	print_value( _GLdouble, &v2 );
	END;
	GLV.glMapGrid2d( un, u1, u2, vn, v1, v2 );
}


/* */
void GLAPIENTRY
glMapGrid2f ( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 )
{
	START( glMapGrid1f );
	print_value( _GLint, &un );
	print( "," );
	print_value( _GLfloat, &u1 );
	print( "," );
	print_value( _GLfloat, &u2 );
	print( "," );
	print_value( _GLint, &vn );
	print( "," );
	print_value( _GLfloat, &v1 );
	print( "," );
	print_value( _GLfloat, &v2 );
	END;
	GLV.glMapGrid2f( un, u1, u2, vn, v1, v2 );
}


/* */
void GLAPIENTRY
glMaterialf ( GLenum face, GLenum pname, GLfloat param )
{
	START( glMaterialf );
	print_value( _GLenum, &face );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLfloat, &param );
	END;
	GLV.glMaterialf( face, pname, param );
}


/* */
void GLAPIENTRY
glMaterialfv ( GLenum face, GLenum pname, const GLfloat *params )
{
	START( glMaterialfv );
	print_value( _GLenum, &face );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_SPECULAR:
		case GL_EMISSION:
		case GL_AMBIENT_AND_DIFFUSE:
			print_value( _GLfloat4, &params );
			break;
		case GL_COLOR_INDEXES:
			print_value( _GLfloat3, &params );
			break;
		case GL_SHININESS:
		default:
			print_value( _GLfloat1, &params );
			break;
	}

	END;
	GLV.glMaterialfv( face, pname, params );
}


/* */
void GLAPIENTRY
glMateriali ( GLenum face, GLenum pname, GLint param )
{
	START( glMateriali );
	print_value( _GLenum, &face );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLint, &param );
	END;
	GLV.glMateriali( face, pname, param );
}


/* */
void GLAPIENTRY
glMaterialiv ( GLenum face, GLenum pname, const GLint *params )
{
	START( glMaterialiv );
	print_value( _GLenum, &face );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_AMBIENT:
		case GL_DIFFUSE:
		case GL_SPECULAR:
		case GL_EMISSION:
		case GL_AMBIENT_AND_DIFFUSE:
			print_value( _GLint4, &params );
			break;
		case GL_COLOR_INDEXES:
			print_value( _GLint3, &params );
			break;
		case GL_SHININESS:
		default:
			print_value( _GLint1, &params );
			break;
	}

	END;
	GLV.glMaterialiv( face, pname, params );
}


/* */
void GLAPIENTRY
glMatrixMode ( GLenum mode )
{
	START( glMatrixMode );
	print_value( _GLenum, &mode );
	END;
	GLV.glMatrixMode( mode );
}


/* */
void GLAPIENTRY
glMultMatrixd ( const GLdouble *m )
{
	START( glMultMatrixd );
	print_value( _GLdouble16, &m );
	END;
	GLV.glMultMatrixd( m );
}


/* */
void GLAPIENTRY
glMultMatrixf ( const GLfloat *m )
{
	START( glMultMatrixf );
	print_value( _GLfloat16, &m );
	END;
	GLV.glMultMatrixf( m );
}


/* */
void GLAPIENTRY
glNewList ( GLuint list, GLenum mode )
{
	START( glNewList );
	print_value( _GLuint, &list );
	print( "," );
	print_value( _GLenum, &mode );
	END;
	GLV.glNewList( list, mode );
}


/* */
void GLAPIENTRY
glNormal3b ( GLbyte nx, GLbyte ny, GLbyte nz )
{
	START( glNormal3b );
	print_value( _GLbyte, &nx );
	print( "," );
	print_value( _GLbyte, &ny );
	print( "," );
	print_value( _GLbyte, &nz );
	END;
	GLV.glNormal3b( nx, ny, nz );
}


/* */
void GLAPIENTRY
glNormal3bv ( const GLbyte *v )
{
	START( glNormal3bv );
	print_value( _GLbyte3, &v );
	END;
	GLV.glNormal3bv( v );
}


/* */
void GLAPIENTRY
glNormal3d ( GLdouble nx, GLdouble ny, GLdouble nz )
{
	START( glNormal3d );
	print_value( _GLdouble, &nx );
	print( "," );
	print_value( _GLdouble, &ny );
	print( "," );
	print_value( _GLdouble, &nz );
	END;
	GLV.glNormal3d( nx, ny, nz );
}


/* */
void GLAPIENTRY
glNormal3dv ( const GLdouble *v )
{
	START( glNormal3dv );
	print_value( _GLdouble3, &v );
	END;
	GLV.glNormal3dv( v );
}


/* */
void GLAPIENTRY
glNormal3f ( GLfloat nx, GLfloat ny, GLfloat nz )
{
	START( glNormal3f );
	print_value( _GLfloat, &nx );
	print( "," );
	print_value( _GLfloat, &ny );
	print( "," );
	print_value( _GLfloat, &nz );
	END;
	GLV.glNormal3f( nx, ny, nz );
}


/* */
void GLAPIENTRY
glNormal3fv ( const GLfloat *v )
{
	START( glNormal3fv );
	print_value( _GLfloat3, &v );
	END;
	GLV.glNormal3fv( v );
}


/* */
void GLAPIENTRY
glNormal3i ( GLint nx, GLint ny, GLint nz )
{
	START( glNormal3i );
	print_value( _GLint, &nx );
	print( "," );
	print_value( _GLint, &ny );
	print( "," );
	print_value( _GLint, &nz );
	END;
	GLV.glNormal3i( nx, ny, nz );
}


/* */
void GLAPIENTRY
glNormal3iv ( const GLint *v )
{
	START( glNormal3iv );
	print_value( _GLint3, &v );
	END;
	GLV.glNormal3iv( v );
}


/* */
void GLAPIENTRY
glNormal3s ( GLshort nx, GLshort ny, GLshort nz )
{
	START( glNormal3s );
	print_value( _GLshort, &nx );
	print( "," );
	print_value( _GLshort, &ny );
	print( "," );
	print_value( _GLshort, &nz );
	END;
	GLV.glNormal3s( nx, ny, nz );
}


/* */
void GLAPIENTRY
glNormal3sv ( const GLshort *v )
{
	START( glNormal3sv );
	print_value( _GLshort3, &v );
	END;
	GLV.glNormal3sv( v );
}


/* */
void GLAPIENTRY
glNormalPointer ( GLenum type, GLsizei stride, const GLvoid *pointer )
{
	START( glNormalPointer );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLsizei, &stride );
	print( "," );
	print_value( _GLvoid, &pointer );
	END;
	GLV.glNormalPointer( type, stride, pointer );
}


/* */
void GLAPIENTRY
glOrtho ( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
	START( glOrtho );
	print_value( _GLdouble, &left );
	print( "," );
	print_value( _GLdouble, &right );
	print( "," );
	print_value( _GLdouble, &bottom );
	print( "," );
	print_value( _GLdouble, &top );
	print( "," );
	print_value( _GLdouble, &zNear );
	print( "," );
	print_value( _GLdouble, &zFar );
	print( "," );
	END;
	GLV.glOrtho( left, right, bottom, top, zNear, zFar );
}


/* */
void GLAPIENTRY
glPassThrough ( GLfloat token )
{
	START( glPassThrough );
	print_value( _GLfloat, &token );
	END;
	GLV.glPassThrough( token );
}


/* */
void GLAPIENTRY
glPixelMapfv ( GLenum map, GLsizei mapsize, const GLfloat *values )
{
	int i;
	START( glPixelMapfv );
	print_value( _GLenum, &map );
	print( "," );
	print_value( _GLsizei, &mapsize );
	print( "," );
	print( "[" );
	for ( i = 0; i < mapsize; i++ )
	{
		if ( i != 0 )
		{
			print( "," );
		}

		print_value( _GLfloat, &values[i] );
	}

	print( "]" );
	END;
	GLV.glPixelMapfv( map, mapsize, values );
}


/* */
void GLAPIENTRY
glPixelMapuiv ( GLenum map, GLsizei mapsize, const GLuint *values )
{
	int i;
	START( glPixelMapuiv );
	print_value( _GLenum, &map );
	print( "," );
	print_value( _GLsizei, &mapsize );
	print( "," );
	print( "[" );
	for ( i = 0; i < mapsize; i++ )
	{
		if ( i != 0 )
		{
			print( "," );
		}

		print_value( _GLuint, &values[i] );
	}

	print( "]" );
	END;
	GLV.glPixelMapuiv( map, mapsize, values );
}


/* */
void GLAPIENTRY
glPixelMapusv ( GLenum map, GLsizei mapsize, const GLushort *values )
{
	int i;
	START( glPixelMapusv );
	print_value( _GLenum, &map );
	print( "," );
	print_value( _GLsizei, &mapsize );
	print( "," );
	print( "[" );
	for ( i = 0; i < mapsize; i++ )
	{
		if ( i != 0 )
		{
			print( "," );
		}

		print_value( _GLushort, &values[i] );
	}

	print( "]" );
	END;
	GLV.glPixelMapusv( map, mapsize, values );
}


/* */
void GLAPIENTRY
glPixelStoref ( GLenum pname, GLfloat param )
{
	START( glPixelStoref );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLfloat, &param );
	END;
	GLV.glPixelStoref( pname, param );
}


/* */
void GLAPIENTRY
glPixelStorei ( GLenum pname, GLint param )
{
	START( glPixelStorei );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLint, &param );
	END;
	GLV.glPixelStorei( pname, param );
}


/* */
void GLAPIENTRY
glPixelTransferf ( GLenum pname, GLfloat param )
{
	START( glPixelTransferf );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLfloat, &param );
	END;
	GLV.glPixelTransferf( pname, param );
}


/* */
void GLAPIENTRY
glPixelTransferi ( GLenum pname, GLint param )
{
	START( glPixelTransferi );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLint, &param );
	END;
	GLV.glPixelTransferi( pname, param );
}


/* */
void GLAPIENTRY
glPixelZoom ( GLfloat xfactor, GLfloat yfactor )
{
	START( glPixelZoom );
	print_value( _GLfloat, &xfactor );
	print( "," );
	print_value( _GLfloat, &yfactor );
	END;
	GLV.glPixelZoom( xfactor, yfactor );
}


/* */
void GLAPIENTRY
glPointSize ( GLfloat size )
{
	START( glPointSize );
	print_value( _GLfloat, &size );
	END;
	GLV.glPointSize( size );
}


/* */
void GLAPIENTRY
glPolygonMode ( GLenum face, GLenum mode )
{
	START( glPolygonMode );
	print_value( _GLenum, &face );
	print( "," );
	print_value( _GLenum, &mode );
	END;
	GLV.glPolygonMode( face, mode );
}


/* */
void GLAPIENTRY
glPolygonOffset ( GLfloat factor, GLfloat units )
{
	START( glPolygonOffset );
	print_value( _GLfloat, &factor );
	print( "," );
	print_value( _GLfloat, &units );
	END;
	GLV.glPolygonOffset( factor, units );
}


/* */
void GLAPIENTRY
glPolygonStipple ( const GLubyte *mask )
{
	START( glPolygonStipple );
	print_value( _GLvoid, &mask );
	END;
	GLV.glPolygonStipple( mask );
}


/* */
void GLAPIENTRY
glPopAttrib ( void )
{
	START( glPopAttrib );
	END;
	GLV.glPopAttrib();
}


/* */
void GLAPIENTRY
glPopClientAttrib ( void )
{
	START( glPopClientAttrib );
	END;
	GLV.glPopClientAttrib();
}


/* */
void GLAPIENTRY
glPopMatrix ( void )
{
	START( glPopMatrix );
	END;
	GLV.glPopMatrix();
}


/* */
void GLAPIENTRY
glPopName ( void )
{
	START( glPopName );
	END;
	GLV.glPopName();
}


/* */
void GLAPIENTRY
glPrioritizeTextures ( GLsizei n, const GLuint *textures, const GLclampf *priorities )
{
	int i;
	START( glPrioritizeTextures );
	print_value( _GLsizei, &n );
	print( "," );
	print( "[" );
	for ( i = 0; i < n; i++ )
	{
		if ( i != 0 )
		{
			print( "," );
		}

		print( "%ud is %f", textures[i], priorities[i] );
	}

	print( "]" );
	END;
	GLV.glPrioritizeTextures( n, textures, priorities );
}


/* */
void GLAPIENTRY
glPushAttrib ( GLbitfield mask )
{
	START( glPushAttrib );
	print_value( _GLbitfield, &mask );
	END;
	GLV.glPushAttrib( mask );
}


/* */
void GLAPIENTRY
glPushClientAttrib ( GLbitfield mask )
{
	START( glPushClientAttrib );
	print_value( _GLbitfield, &mask );
	END;
	GLV.glPushClientAttrib( mask );
}


/* */
void GLAPIENTRY
glPushMatrix ( void )
{
	START( glPushMatrix );
	END;
	GLV.glPushMatrix();
}


/* */
void GLAPIENTRY
glPushName ( GLuint name )
{
	START( glPushName );
	print_value( _GLuint, &name );
	END;
	GLV.glPushName( name );
}


/* */
void GLAPIENTRY
glRasterPos2d ( GLdouble x, GLdouble y )
{
	START( glRasterPos2d );
	print_value( _GLdouble, &x );
	print( "," );
	print_value( _GLdouble, &y );
	END;
	GLV.glRasterPos2d( x, y );
}


/* */
void GLAPIENTRY
glRasterPos2dv ( const GLdouble *v )
{
	START( glRasterPos2dv );
	print_value( _GLdouble2, &v );
	END;
	GLV.glRasterPos2dv( v );
}


/* */
void GLAPIENTRY
glRasterPos2f ( GLfloat x, GLfloat y )
{
	START( glRasterPos2f );
	print_value( _GLfloat, &x );
	print( "," );
	print_value( _GLfloat, &y );
	END;
	GLV.glRasterPos2f( x, y );
}


/* */
void GLAPIENTRY
glRasterPos2fv ( const GLfloat *v )
{
	START( glRasterPos2fv );
	print_value( _GLfloat2, &v );
	END;
	GLV.glRasterPos2fv( v );
}


/* */
void GLAPIENTRY
glRasterPos2i ( GLint x, GLint y )
{
	START( glRasterPos2i );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	END;
	GLV.glRasterPos2i( x, y );
}


/* */
void GLAPIENTRY
glRasterPos2iv ( const GLint *v )
{
	START( glRasterPos2iv );
	print_value( _GLint2, &v );
	END;
	GLV.glRasterPos2iv( v );
}


/* */
void GLAPIENTRY
glRasterPos2s ( GLshort x, GLshort y )
{
	START( glRasterPos2s );
	print_value( _GLshort, &x );
	print( "," );
	print_value( _GLshort, &y );
	END;
	GLV.glRasterPos2s( x, y );
}


/* */
void GLAPIENTRY
glRasterPos2sv ( const GLshort *v )
{
	START( glRasterPos2sv );
	print_value( _GLshort2, &v );
	END;
	GLV.glRasterPos2sv( v );
}


/* */
void GLAPIENTRY
glRasterPos3d ( GLdouble x, GLdouble y, GLdouble z )
{
	START( glRasterPos3d );
	print_value( _GLdouble, &x );
	print( "," );
	print_value( _GLdouble, &y );
	print( "," );
	print_value( _GLdouble, &z );
	END;
	GLV.glRasterPos3d( x, y, z );
}


/* */
void GLAPIENTRY
glRasterPos3dv ( const GLdouble *v )
{
	START( glRasterPos3dv );
	print_value( _GLdouble3, &v );
	END;
	GLV.glRasterPos3dv( v );
}


/* */
void GLAPIENTRY
glRasterPos3f ( GLfloat x, GLfloat y, GLfloat z )
{
	START( glRasterPos3f );
	print_value( _GLfloat, &x );
	print( "," );
	print_value( _GLfloat, &y );
	print( "," );
	print_value( _GLfloat, &z );
	END;
	GLV.glRasterPos3f( x, y, z );
}


/* */
void GLAPIENTRY
glRasterPos3fv ( const GLfloat *v )
{
	START( glRasterPos3fv );
	print_value( _GLfloat3, &v );
	END;
	GLV.glRasterPos3fv( v );
}


/* */
void GLAPIENTRY
glRasterPos3i ( GLint x, GLint y, GLint z )
{
	START( glRasterPos3i );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLint, &z );
	END;
	GLV.glRasterPos3i( x, y, z );
}


/* */
void GLAPIENTRY
glRasterPos3iv ( const GLint *v )
{
	START( glRasterPos3iv );
	print_value( _GLint3, &v );
	END;
	GLV.glRasterPos3iv( v );
}


/* */
void GLAPIENTRY
glRasterPos3s ( GLshort x, GLshort y, GLshort z )
{
	START( glRasterPos3s );
	print_value( _GLshort, &x );
	print( "," );
	print_value( _GLshort, &y );
	print( "," );
	print_value( _GLshort, &z );
	END;
	GLV.glRasterPos3s( x, y, z );
}


/* */
void GLAPIENTRY
glRasterPos3sv ( const GLshort *v )
{
	START( glRasterPos3sv );
	print_value( _GLshort3, &v );
	END;
	GLV.glRasterPos3sv( v );
}


/* */
void GLAPIENTRY
glRasterPos4d ( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
	START( glRasterPos4d );
	print_value( _GLdouble, &x );
	print( "," );
	print_value( _GLdouble, &y );
	print( "," );
	print_value( _GLdouble, &z );
	print( "," );
	print_value( _GLdouble, &w );
	END;
	GLV.glRasterPos4d( x, y, z, w );
}


/* */
void GLAPIENTRY
glRasterPos4dv ( const GLdouble *v )
{
	START( glRasterPos4dv );
	print_value( _GLdouble, &v );
	END;
	GLV.glRasterPos4dv( v );
}


/* */
void GLAPIENTRY
glRasterPos4f ( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
	START( glRasterPos4f );
	print_value( _GLfloat, &x );
	print( "," );
	print_value( _GLfloat, &y );
	print( "," );
	print_value( _GLfloat, &z );
	print( "," );
	print_value( _GLfloat, &w );
	END;
	GLV.glRasterPos4f( x, y, z, w );
}


/* */
void GLAPIENTRY
glRasterPos4fv ( const GLfloat *v )
{
	START( glRasterPos4fv );
	print_value( _GLfloat, &v );
	END;
	GLV.glRasterPos4fv( v );
}


/* */
void GLAPIENTRY
glRasterPos4i ( GLint x, GLint y, GLint z, GLint w )
{
	START( glRasterPos4i );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLint, &z );
	print( "," );
	print_value( _GLint, &w );
	END;
	GLV.glRasterPos4i( x, y, z, w );
}


/* */
void GLAPIENTRY
glRasterPos4iv ( const GLint *v )
{
	START( glRasterPos4iv );
	print_value( _GLint, &v );
	END;
	GLV.glRasterPos4iv( v );
}


/* */
void GLAPIENTRY
glRasterPos4s ( GLshort x, GLshort y, GLshort z, GLshort w )
{
	START( glRasterPos4i );
	print_value( _GLshort, &x );
	print( "," );
	print_value( _GLshort, &y );
	print( "," );
	print_value( _GLshort, &z );
	print( "," );
	print_value( _GLshort, &w );
	END;
	GLV.glRasterPos4s( x, y, z, w );
}


/* */
void GLAPIENTRY
glRasterPos4sv ( const GLshort *v )
{
	START( glRasterPos4sv );
	print_value( _GLshort, &v );
	END;
	GLV.glRasterPos4sv( v );
}


/* */
void GLAPIENTRY
glReadBuffer ( GLenum mode )
{
	START( glReadBuffer );
	print_value( _GLenum, &mode );
	END;
	GLV.glReadBuffer( mode );
}


/* */
void GLAPIENTRY
glReadPixels ( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
{
	GLV.glReadPixels( x, y, width, height, format, type, pixels );
	START( glReadPixels );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLsizei, &height );
	print( "," );
	print_value( _GLenum, &format );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );

	/*FIXME?*/
	print_value( _GLvoid, &pixels );
	END;
}


/* */
void GLAPIENTRY
glRectd ( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 )
{
	START( glRectd );
	print_value( _GLdouble, &x1 );
	print( "," );
	print_value( _GLdouble, &y1 );
	print( "," );
	print_value( _GLdouble, &x2 );
	print( "," );
	print_value( _GLdouble, &y2 );
	END;
	GLV.glRectd( x1, y1, x2, y2 );
}


/* */
void GLAPIENTRY
glRectdv ( const GLdouble *v1, const GLdouble *v2 )
{
	START( glRectdv );
	print_value( _GLdouble2, &v1 );
	print( "," );
	print_value( _GLdouble2, &v2 );
	END;
	GLV.glRectdv( v1, v2 );
}


/* */
void GLAPIENTRY
glRectf ( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
	START( glRectf );
	print_value( _GLfloat, &x1 );
	print( "," );
	print_value( _GLfloat, &y1 );
	print( "," );
	print_value( _GLfloat, &x2 );
	print( "," );
	print_value( _GLfloat, &y2 );
	END;
	GLV.glRectf( x1, y1, x2, y2 );
}


/* */
void GLAPIENTRY
glRectfv ( const GLfloat *v1, const GLfloat *v2 )
{
	START( glRectfv );
	print_value( _GLfloat2, &v1 );
	print( "," );
	print_value( _GLfloat2, &v2 );
	END;
	GLV.glRectfv( v1, v2 );
}


/* */
void GLAPIENTRY
glRecti ( GLint x1, GLint y1, GLint x2, GLint y2 )
{
	START( glRecti );
	print_value( _GLint, &x1 );
	print( "," );
	print_value( _GLint, &y1 );
	print( "," );
	print_value( _GLint, &x2 );
	print( "," );
	print_value( _GLint, &y2 );
	END;
	GLV.glRecti( x1, y1, x2, y2 );
}


/* */
void GLAPIENTRY
glRectiv ( const GLint *v1, const GLint *v2 )
{
	START( glRectiv );
	print_value( _GLint2, &v1 );
	print( "," );
	print_value( _GLint2, &v2 );
	END;
	GLV.glRectiv( v1, v2 );
}


/* */
void GLAPIENTRY
glRects ( GLshort x1, GLshort y1, GLshort x2, GLshort y2 )
{
	START( glRects );
	print_value( _GLshort, &x1 );
	print( "," );
	print_value( _GLshort, &y1 );
	print( "," );
	print_value( _GLshort, &x2 );
	print( "," );
	print_value( _GLshort, &y2 );
	END;
	GLV.glRects( x1, y1, x2, y2 );
}


/* */
void GLAPIENTRY
glRectsv ( const GLshort *v1, const GLshort *v2 )
{
	START( glRectsv );
	print_value( _GLshort2, &v1 );
	print( "," );
	print_value( _GLshort2, &v2 );
	END;
	GLV.glRectsv( v1, v2 );
}


/* */
GLint GLAPIENTRY
glRenderMode ( GLenum mode )
{
	GLint	result;
	result = GLV.glRenderMode( mode );
	START( glRenderMode );
	print_value( _GLenum, &mode );
	RESULT;
	print_value( _GLint, &result );
	ENDRESULT;
	return ( result );
}


/* */
void GLAPIENTRY
glRotated ( GLdouble angle, GLdouble x, GLdouble y, GLdouble z )
{
	START( glRotated );
	print_value( _GLdouble, &angle );
	print( "," );
	print_value( _GLdouble, &x );
	print( "," );
	print_value( _GLdouble, &y );
	print( "," );
	print_value( _GLdouble, &z );
	END;
	GLV.glRotated( angle, x, y, z );
}


/* */
void GLAPIENTRY
glRotatef ( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
	START( glRotatef );
	print_value( _GLfloat, &angle );
	print( "," );
	print_value( _GLfloat, &x );
	print( "," );
	print_value( _GLfloat, &y );
	print( "," );
	print_value( _GLfloat, &z );
	END;
	GLV.glRotatef( angle, x, y, z );
}


/* */
void GLAPIENTRY
glScaled ( GLdouble x, GLdouble y, GLdouble z )
{
	START( glScaled );
	print_value( _GLdouble, &x );
	print( "," );
	print_value( _GLdouble, &y );
	print( "," );
	print_value( _GLdouble, &z );
	END;
	GLV.glScaled( x, y, z );
}


/* */
void GLAPIENTRY
glScalef ( GLfloat x, GLfloat y, GLfloat z )
{
	START( glScalef );
	print_value( _GLfloat, &x );
	print( "," );
	print_value( _GLfloat, &y );
	print( "," );
	print_value( _GLfloat, &z );
	END;
	GLV.glScalef( x, y, z );
}


/* */
void GLAPIENTRY
glScissor ( GLint x, GLint y, GLsizei width, GLsizei height )
{
	START( glScissor );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLsizei, &height );
	END;
	GLV.glScissor( x, y, width, height );
}


/* */
void GLAPIENTRY
glSelectBuffer ( GLsizei size, GLuint *buffer )
{
	START( glSelectBuffer );
	print_value( _GLsizei, &size );
	print( "," );
	print_value( _GLvoid, &buffer );
	END;
	GLV.glSelectBuffer( size, buffer );
}


/* */
void GLAPIENTRY
glShadeModel ( GLenum mode )
{
	glColor3f( 0.0f, 0.7f, 0.0f );
	START( glShadeModel );
	print_value( _GLenum, &mode );
	END;
	GLV.glShadeModel( mode );
}


/* */
void GLAPIENTRY
glStencilFunc ( GLenum func, GLint ref, GLuint mask )
{
	START( glStencilFunc );
	print_value( _GLenum, &func );
	print( "," );
	print_value( _GLint, &ref );
	print( "," );
	print_value( _GLuint, &mask );
	END;
	GLV.glStencilFunc( func, ref, mask );
}


/* */
void GLAPIENTRY
glStencilMask ( GLuint mask )
{
	START( glStencilMask );
	print_value( _GLuint, &mask );
	END;
	GLV.glStencilMask( mask );
}


/* */
void GLAPIENTRY
glStencilOp ( GLenum fail, GLenum zfail, GLenum zpass )
{
	useZeroOne = GL_TRUE;
	START( glStencilOp );
	print_value( _GLenum, &fail );
	print( "," );
	print_value( _GLenum, &zfail );
	print( "," );
	print_value( _GLenum, &zpass );
	END;
	GLV.glStencilOp( fail, zfail, zpass );
	useZeroOne = GL_FALSE;
}


/* */
void GLAPIENTRY
glTexCoord1d ( GLdouble s )
{
	START( glTexCoord1d );
	print_value( _GLdouble, &s );
	END;
	GLV.glTexCoord1d( s );
}


/* */
void GLAPIENTRY
glTexCoord1dv ( const GLdouble *v )
{
	START( glTexCoord1dv );
	print_value( _GLdouble1, &v );
	END;
	GLV.glTexCoord1dv( v );
}


/* */
void GLAPIENTRY
glTexCoord1f ( GLfloat s )
{
	START( glTexCoord1f );
	print_value( _GLfloat, &s );
	END;
	GLV.glTexCoord1f( s );
}


/* */
void GLAPIENTRY
glTexCoord1fv ( const GLfloat *v )
{
	START( glTexCoord1fv );
	print_value( _GLfloat1, &v );
	END;
	GLV.glTexCoord1fv( v );
}


/* */
void GLAPIENTRY
glTexCoord1i ( GLint s )
{
	START( glTexCoord1i );
	print_value( _GLint, &s );
	END;
	GLV.glTexCoord1i( s );
}


/* */
void GLAPIENTRY
glTexCoord1iv ( const GLint *v )
{
	START( glTexCoord1iv );
	print_value( _GLint1, &v );
	END;
	GLV.glTexCoord1iv( v );
}


/* */
void GLAPIENTRY
glTexCoord1s ( GLshort s )
{
	START( glTexCoord1s );
	print_value( _GLshort, &s );
	END;
	GLV.glTexCoord1s( s );
}


/* */
void GLAPIENTRY
glTexCoord1sv ( const GLshort *v )
{
	START( glTexCoord1sv );
	print_value( _GLshort1, &v );
	END;
	GLV.glTexCoord1sv( v );
}


/* */
void GLAPIENTRY
glTexCoord2d ( GLdouble s, GLdouble t )
{
	START( glTexCoord2d );
	print_value( _GLdouble, &s );
	print( "," );
	print_value( _GLdouble, &t );
	END;
	GLV.glTexCoord2d( s, t );
}


/* */
void GLAPIENTRY
glTexCoord2dv ( const GLdouble *v )
{
	START( glTexCoord2dv );
	print_value( _GLdouble2, &v );
	END;
	GLV.glTexCoord2dv( v );
}


/* */
void GLAPIENTRY
glTexCoord2f ( GLfloat s, GLfloat t )
{
	START( glTexCoord2f );
	print_value( _GLfloat, &s );
	print( "," );
	print_value( _GLfloat, &t );
	END;
	GLV.glTexCoord2f( s, t );
}


/* */
void GLAPIENTRY
glTexCoord2fv ( const GLfloat *v )
{
	START( glTexCoord2fv );
	print_value( _GLfloat2, &v );
	END;
	GLV.glTexCoord2fv( v );
}


/* */
void GLAPIENTRY
glTexCoord2i ( GLint s, GLint t )
{
	START( glTexCoord2i );
	print_value( _GLint, &s );
	print( "," );
	print_value( _GLint, &t );
	END;
	GLV.glTexCoord2i( s, t );
}


/* */
void GLAPIENTRY
glTexCoord2iv ( const GLint *v )
{
	START( glTexCoord2iv );
	print_value( _GLint2, &v );
	END;
	GLV.glTexCoord2iv( v );
}


/* */
void GLAPIENTRY
glTexCoord2s ( GLshort s, GLshort t )
{
	START( glTexCoord2s );
	print_value( _GLshort, &s );
	print( "," );
	print_value( _GLshort, &t );
	END;
	GLV.glTexCoord2s( s, t );
}


/* */
void GLAPIENTRY
glTexCoord2sv ( const GLshort *v )
{
	START( glTexCoord2sv );
	print_value( _GLshort2, &v );
	END;
	GLV.glTexCoord2sv( v );
}


/* */
void GLAPIENTRY
glTexCoord3d ( GLdouble s, GLdouble t, GLdouble r )
{
	START( glTexCoord3d );
	print_value( _GLdouble, &s );
	print( "," );
	print_value( _GLdouble, &t );
	print( "," );
	print_value( _GLdouble, &r );
	END;
	GLV.glTexCoord3d( s, t, r );
}


/* */
void GLAPIENTRY
glTexCoord3dv ( const GLdouble *v )
{
	START( glTexCoord3dv );
	print_value( _GLdouble3, &v );
	END;
	GLV.glTexCoord3dv( v );
}


/* */
void GLAPIENTRY
glTexCoord3f ( GLfloat s, GLfloat t, GLfloat r )
{
	START( glTexCoord3f );
	print_value( _GLfloat, &s );
	print( "," );
	print_value( _GLfloat, &t );
	print( "," );
	print_value( _GLfloat, &r );
	END;
	GLV.glTexCoord3f( s, t, r );
}


/* */
void GLAPIENTRY
glTexCoord3fv ( const GLfloat *v )
{
	START( glTexCoord3fv );
	print_value( _GLfloat3, &v );
	END;
	GLV.glTexCoord3fv( v );
}


/* */
void GLAPIENTRY
glTexCoord3i ( GLint s, GLint t, GLint r )
{
	START( glTexCoord3i );
	print_value( _GLint, &s );
	print( "," );
	print_value( _GLint, &t );
	print( "," );
	print_value( _GLint, &r );
	END;
	GLV.glTexCoord3i( s, t, r );
}


/* */
void GLAPIENTRY
glTexCoord3iv ( const GLint *v )
{
	START( glTexCoord3iv );
	print_value( _GLint3, &v );
	END;
	GLV.glTexCoord3iv( v );
}


/* */
void GLAPIENTRY
glTexCoord3s ( GLshort s, GLshort t, GLshort r )
{
	START( glTexCoord3s );
	print_value( _GLshort, &s );
	print( "," );
	print_value( _GLshort, &t );
	print( "," );
	print_value( _GLshort, &r );
	END;
	GLV.glTexCoord3s( s, t, r );
}


/* */
void GLAPIENTRY
glTexCoord3sv ( const GLshort *v )
{
	START( glTexCoord3sv );
	print_value( _GLshort3, &v );
	END;
	GLV.glTexCoord3sv( v );
}


/* */
void GLAPIENTRY
glTexCoord4d ( GLdouble s, GLdouble t, GLdouble r, GLdouble q )
{
	START( glTexCoord4d );
	print_value( _GLdouble, &s );
	print( "," );
	print_value( _GLdouble, &t );
	print( "," );
	print_value( _GLdouble, &r );
	print( "," );
	print_value( _GLdouble, &q );
	END;
	GLV.glTexCoord4d( s, t, r, q );
}


/* */
void GLAPIENTRY
glTexCoord4dv ( const GLdouble *v )
{
	START( glTexCoord4dv );
	print_value( _GLdouble4, &v );
	END;
	GLV.glTexCoord4dv( v );
}


/* */
void GLAPIENTRY
glTexCoord4f ( GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
	START( glTexCoord2f );
	print_value( _GLfloat, &s );
	print( "," );
	print_value( _GLfloat, &t );
	print( "," );
	print_value( _GLfloat, &r );
	print( "," );
	print_value( _GLfloat, &q );
	END;
	GLV.glTexCoord4f( s, t, r, q );
}


/* */
void GLAPIENTRY
glTexCoord4fv ( const GLfloat *v )
{
	START( glTexCoord4fv );
	print_value( _GLfloat4, &v );
	END;
	GLV.glTexCoord4fv( v );
}


/* */
void GLAPIENTRY
glTexCoord4i ( GLint s, GLint t, GLint r, GLint q )
{
	START( glTexCoord4i );
	print_value( _GLint, &s );
	print( "," );
	print_value( _GLint, &t );
	print( "," );
	print_value( _GLint, &r );
	print( "," );
	print_value( _GLint, &q );
	END;
	GLV.glTexCoord4i( s, t, r, q );
}


/* */
void GLAPIENTRY
glTexCoord4iv ( const GLint *v )
{
	START( glTexCoord4iv );
	print_value( _GLint4, &v );
	END;
	GLV.glTexCoord4iv( v );
}


/* */
void GLAPIENTRY
glTexCoord4s ( GLshort s, GLshort t, GLshort r, GLshort q )
{
	START( glTexCoord4s );
	print_value( _GLshort, &s );
	print( "," );
	print_value( _GLshort, &t );
	print( "," );
	print_value( _GLshort, &r );
	print( "," );
	print_value( _GLshort, &q );
	END;
	GLV.glTexCoord4s( s, t, r, q );
}


/* */
void GLAPIENTRY
glTexCoord4sv ( const GLshort *v )
{
	START( glTexCoord4sv );
	print_value( _GLshort4, &v );
	END;
	GLV.glTexCoord4sv( v );
}


/* */
void GLAPIENTRY
glTexCoordPointer ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
	START( glTexCoordPointer );
	print_value( _GLint, &size );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLsizei, &stride );
	print( "," );
	print_value( _GLvoid, &pointer );
	END;
	GLV.glTexCoordPointer( size, type, stride, pointer );
}


/* */
void GLAPIENTRY
glTexEnvf ( GLenum target, GLenum pname, GLfloat param )
{
	GLenum	p = ( GLenum ) param;
	START( glTexEnvf );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLenum, &p );
	END;
	GLV.glTexEnvf( target, pname, param );
}


/* */
void GLAPIENTRY
glTexEnvfv ( GLenum target, GLenum pname, const GLfloat *params )
{
	START( glTexEnvfv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_TEXTURE_ENV_MODE:
			{
				GLenum	p = ( GLenum ) params[0];
				print_value( _GLenum, &p );
			}
			break;
		case GL_TEXTURE_ENV_COLOR:
			print_value( _GLfloat4, &params );
			break;
		default:
			break;
	}

	END;
	GLV.glTexEnvfv( target, pname, params );
}


/* */
void GLAPIENTRY
glTexEnvi ( GLenum target, GLenum pname, GLint param )
{
	GLenum	p = ( GLenum ) param;
	START( glTexEnvi );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLenum, &p );
	END;
	GLV.glTexEnvi( target, pname, param );
}


/* */
void GLAPIENTRY
glTexEnviv ( GLenum target, GLenum pname, const GLint *params )
{
	START( glTexEnviv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_TEXTURE_ENV_MODE:
			{
				GLenum	p = ( GLenum ) params[0];
				print_value( _GLenum, &p );
			}
			break;
		case GL_TEXTURE_ENV_COLOR:
			print_value( _GLint4, &params );
			break;
		default:
			break;
	}

	END;
	GLV.glTexEnviv( target, pname, params );
}


/* */
void GLAPIENTRY
glTexGend ( GLenum coord, GLenum pname, GLdouble param )
{
	GLenum	p = ( GLenum ) param;
	START( glTexGend );
	print_value( _GLenum, &coord );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLenum, &p );
	END;
	GLV.glTexGend( coord, pname, param );
}


/* */
void GLAPIENTRY
glTexGendv ( GLenum coord, GLenum pname, const GLdouble *params )
{
	START( glTexGendv );
	print_value( _GLenum, &coord );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_TEXTURE_GEN_MODE:
			{
				GLenum	p = ( GLenum ) params[0];
				print_value( _GLenum, &p );
			}
			break;
		case GL_OBJECT_PLANE:
		case GL_EYE_PLANE:
			print_value( _GLdouble4, &params );
			break;
		default:
			break;
	}

	END;
	GLV.glTexGendv( coord, pname, params );
}


/* */
void GLAPIENTRY
glTexGenf ( GLenum coord, GLenum pname, GLfloat param )
{
	GLenum	p = ( GLenum ) param;
	START( glTexGenf );
	print_value( _GLenum, &coord );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLenum, &p );
	END;
	GLV.glTexGenf( coord, pname, param );
}


/* */
void GLAPIENTRY
glTexGenfv ( GLenum coord, GLenum pname, const GLfloat *params )
{
	START( glTexGenfv );
	print_value( _GLenum, &coord );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_TEXTURE_GEN_MODE:
			{
				GLenum	p = ( GLenum ) params[0];
				print_value( _GLenum, &p );
			}
			break;
		case GL_OBJECT_PLANE:
		case GL_EYE_PLANE:
			print_value( _GLfloat4, &params );
			break;
		default:
			break;
	}

	END;
	GLV.glTexGenfv( coord, pname, params );
}


/* */
void GLAPIENTRY
glTexGeni ( GLenum coord, GLenum pname, GLint param )
{
	GLenum	p = ( GLenum ) param;
	START( glTexGeni );
	print_value( _GLenum, &coord );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	print_value( _GLenum, &p );
	END;
	GLV.glTexGeni( coord, pname, param );
}


/* */
void GLAPIENTRY
glTexGeniv ( GLenum coord, GLenum pname, const GLint *params )
{
	START( glTexGeniv );
	print_value( _GLenum, &coord );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_TEXTURE_GEN_MODE:
			{
				GLenum	p = ( GLenum ) params[0];
				print_value( _GLenum, &p );
			}
			break;
		case GL_OBJECT_PLANE:
		case GL_EYE_PLANE:
			print_value( _GLint4, &params );
			break;
		default:
			break;
	}

	END;
	GLV.glTexGeniv( coord, pname, params );
}


/* */
void GLAPIENTRY
glTexImage1D
(
	GLenum			target,
	GLint			level,
	GLint			internalformat,
	GLsizei			width,
	GLint			border,
	GLenum			format,
	GLenum			type,
	const GLvoid	*pixels
)
{
	START( glTexImage1D );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLint, &internalformat );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLint, &border );
	print( "," );
	print_value( _GLenum, &format );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLvoid, &pixels );
	END;
	GLV.glTexImage1D( target, level, internalformat, width, border, format, type, pixels );
}


/* */
void GLAPIENTRY
glTexImage2D
(
	GLenum			target,
	GLint			level,
	GLint			internalformat,
	GLsizei			width,
	GLsizei			height,
	GLint			border,
	GLenum			format,
	GLenum			type,
	const GLvoid	*pixels
)
{
	START( glTexImage2D );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLint, &internalformat );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLsizei, &height );
	print( "," );
	print_value( _GLint, &border );
	print( "," );
	print_value( _GLenum, &format );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLvoid, &pixels );
	END;
	GLV.glTexImage2D( target, level, internalformat, width, height, border, format, type, pixels );
}


/* */
void GLAPIENTRY
glTexParameterf ( GLenum target, GLenum pname, GLfloat param )
{
	GLenum	p = ( GLenum ) param;
	START( glTexParameterf );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_TEXTURE_PRIORITY:
			print_value( _GLfloat, &param );
			break;
		default:
			print_value( _GLenum, &p );
			break;
	}

	END;
	GLV.glTexParameterf( target, pname, param );
}


/* */
void GLAPIENTRY
glTexParameterfv ( GLenum target, GLenum pname, const GLfloat *params )
{
	START( glTexParameterfv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_TEXTURE_BORDER_COLOR:
			print_value( _GLfloat4, &params );
			break;
		case GL_TEXTURE_PRIORITY:
			print_value( _GLfloat1, &params );
			break;
		default:
			{
				GLenum	p = ( GLenum ) params;
				print_value( _GLenum, &p );
			}
			break;
	}

	END;
	GLV.glTexParameterfv( target, pname, params );
}


/* */
void GLAPIENTRY
glTexParameteri ( GLenum target, GLenum pname, GLint param )
{
	GLenum	p = ( GLenum ) param;
	START( glTexParameteri );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_TEXTURE_PRIORITY:
			print_value( _GLint, &param );
			break;
		default:
			print_value( _GLenum, &p );
			break;
	}

	END;
	GLV.glTexParameteri( target, pname, param );
}


/* */
void GLAPIENTRY
glTexParameteriv ( GLenum target, GLenum pname, const GLint *params )
{
	START( glTexParameteriv );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLenum, &pname );
	print( "," );
	switch ( pname )
	{
		case GL_TEXTURE_BORDER_COLOR:
			print_value( _GLint4, &params );
			break;
		case GL_TEXTURE_PRIORITY:
			print_value( _GLint1, &params );
			break;
		default:
			{
				GLenum	p = ( GLenum ) params;
				print_value( _GLenum, &p );
			}
			break;
	}

	END;
	GLV.glTexParameteriv( target, pname, params );
}


/* */
void GLAPIENTRY
glTexSubImage1D
(
	GLenum			target,
	GLint			level,
	GLint			xoffset,
	GLsizei			width,
	GLenum			format,
	GLenum			type,
	const GLvoid	*pixels
)
{
	START( glTexSubImage1D );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLint, &xoffset );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLenum, &format );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLvoid, &pixels );
	END;
	GLV.glTexSubImage1D( target, level, xoffset, width, format, type, pixels );
}


/* */
void GLAPIENTRY
glTexSubImage2D
(
	GLenum			target,
	GLint			level,
	GLint			xoffset,
	GLint			yoffset,
	GLsizei			width,
	GLsizei			height,
	GLenum			format,
	GLenum			type,
	const GLvoid	*pixels
)
{
	START( glTexSubImage2D );
	print_value( _GLenum, &target );
	print( "," );
	print_value( _GLint, &level );
	print( "," );
	print_value( _GLint, &xoffset );
	print( "," );
	print_value( _GLint, &yoffset );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLsizei, &height );
	print( "," );
	print_value( _GLenum, &format );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLvoid, &pixels );
	END;
	GLV.glTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels );
}


/* */
void GLAPIENTRY
glTranslated ( GLdouble x, GLdouble y, GLdouble z )
{
	START( glTranslated );
	print_value( _GLdouble, &x );
	print( "," );
	print_value( _GLdouble, &y );
	print( "," );
	print_value( _GLdouble, &z );
	END;
	GLV.glTranslated( x, y, z );
}


/* */
void GLAPIENTRY
glTranslatef ( GLfloat x, GLfloat y, GLfloat z )
{
	START( glTranslated );
	print_value( _GLfloat, &x );
	print( "," );
	print_value( _GLfloat, &y );
	print( "," );
	print_value( _GLfloat, &z );
	END;
	GLV.glTranslatef( x, y, z );
}


/* */
void GLAPIENTRY
glVertex2d ( GLdouble x, GLdouble y )
{
	START( glVertex2d );
	print_value( _GLdouble, &x );
	print( "," );
	print_value( _GLdouble, &y );
	END;
	GLV.glVertex2d( x, y );
}


/* */
void GLAPIENTRY
glVertex2dv ( const GLdouble *v )
{
	START( glVertex2dv );
	print_value( _GLdouble2, &v );
	END;
	GLV.glVertex2dv( v );
}


/* */
void GLAPIENTRY
glVertex2f ( GLfloat x, GLfloat y )
{
	START( glVertex2f );
	print_value( _GLfloat, &x );
	print( "," );
	print_value( _GLfloat, &y );
	END;
	GLV.glVertex2f( x, y );
}


/* */
void GLAPIENTRY
glVertex2fv ( const GLfloat *v )
{
	START( glVertex2fv );
	print_value( _GLfloat2, &v );
	END;
	GLV.glVertex2fv( v );
}


/* */
void GLAPIENTRY
glVertex2i ( GLint x, GLint y )
{
	START( glVertex2i );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	END;
	GLV.glVertex2i( x, y );
}


/* */
void GLAPIENTRY
glVertex2iv ( const GLint *v )
{
	START( glVertex2iv );
	print_value( _GLint2, &v );
	END;
	GLV.glVertex2iv( v );
}


/* */
void GLAPIENTRY
glVertex2s ( GLshort x, GLshort y )
{
	START( glVertex2s );
	print_value( _GLshort, &x );
	print( "," );
	print_value( _GLshort, &y );
	END;
	GLV.glVertex2s( x, y );
}


/* */
void GLAPIENTRY
glVertex2sv ( const GLshort *v )
{
	START( glVertex2sv );
	print_value( _GLshort2, &v );
	END;
	GLV.glVertex2sv( v );
}


/* */
void GLAPIENTRY
glVertex3d ( GLdouble x, GLdouble y, GLdouble z )
{
	START( glVertex3d );
	print_value( _GLdouble, &x );
	print( "," );
	print_value( _GLdouble, &y );
	print( "," );
	print_value( _GLdouble, &z );
	END;
	GLV.glVertex3d( x, y, z );
}


/* */
void GLAPIENTRY
glVertex3dv ( const GLdouble *v )
{
	START( glVertex3dv );
	print_value( _GLdouble3, &v );
	END;
	GLV.glVertex3dv( v );
}


/* */
void GLAPIENTRY
glVertex3f ( GLfloat x, GLfloat y, GLfloat z )
{
	START( glVertex3f );
	print_value( _GLfloat, &x );
	print( "," );
	print_value( _GLfloat, &y );
	print( "," );
	print_value( _GLfloat, &z );
	END;
	GLV.glVertex3f( x, y, z );
}


/* */
void GLAPIENTRY
glVertex3fv ( const GLfloat *v )
{
	START( glVertex3fv );
	print_value( _GLfloat3, &v );
	END;
	GLV.glVertex3fv( v );
}


/* */
void GLAPIENTRY
glVertex3i ( GLint x, GLint y, GLint z )
{
	START( glVertex3i );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLint, &z );
	END;
	GLV.glVertex3i( x, y, z );
}


/* */
void GLAPIENTRY
glVertex3iv ( const GLint *v )
{
	START( glVertex3iv );
	print_value( _GLint3, &v );
	END;
	GLV.glVertex3iv( v );
}


/* */
void GLAPIENTRY
glVertex3s ( GLshort x, GLshort y, GLshort z )
{
	START( glVertex2s );
	print_value( _GLshort, &x );
	print( "," );
	print_value( _GLshort, &y );
	print( "," );
	print_value( _GLshort, &z );
	END;
	GLV.glVertex3s( x, y, z );
}


/* */
void GLAPIENTRY
glVertex3sv ( const GLshort *v )
{
	START( glVertex3sv );
	print_value( _GLshort3, &v );
	END;
	GLV.glVertex3sv( v );
}


/* */
void GLAPIENTRY
glVertex4d ( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
	START( glVertex4d );
	print_value( _GLdouble, &x );
	print( "," );
	print_value( _GLdouble, &y );
	print( "," );
	print_value( _GLdouble, &z );
	print( "," );
	print_value( _GLdouble, &w );
	END;
	GLV.glVertex4d( x, y, z, w );
}


/* */
void GLAPIENTRY
glVertex4dv ( const GLdouble *v )
{
	START( glVertex4dv );
	print_value( _GLdouble4, &v );
	END;
	GLV.glVertex4dv( v );
}


/* */
void GLAPIENTRY
glVertex4f ( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
	START( glVertex4f );
	print_value( _GLfloat, &x );
	print( "," );
	print_value( _GLfloat, &y );
	print( "," );
	print_value( _GLfloat, &z );
	print( "," );
	print_value( _GLfloat, &w );
	END;
	GLV.glVertex4f( x, y, z, w );
}


/* */
void GLAPIENTRY
glVertex4fv ( const GLfloat *v )
{
	START( glVertex4fv );
	print_value( _GLfloat4, &v );
	END;
	GLV.glVertex4fv( v );
}


/* */
void GLAPIENTRY
glVertex4i ( GLint x, GLint y, GLint z, GLint w )
{
	START( glVertex4i );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLint, &z );
	print( "," );
	print_value( _GLint, &w );
	END;
	GLV.glVertex4i( x, y, z, w );
}


/* */
void GLAPIENTRY
glVertex4iv ( const GLint *v )
{
	START( glVertex4iv );
	print_value( _GLint4, &v );
	END;
	GLV.glVertex4iv( v );
}


/* */
void GLAPIENTRY
glVertex4s ( GLshort x, GLshort y, GLshort z, GLshort w )
{
	START( glVertex2s );
	print_value( _GLshort, &x );
	print( "," );
	print_value( _GLshort, &y );
	print( "," );
	print_value( _GLshort, &z );
	print( "," );
	print_value( _GLshort, &w );
	END;
	GLV.glVertex4s( x, y, z, w );
}


/* */
void GLAPIENTRY
glVertex4sv ( const GLshort *v )
{
	START( glVertex4sv );
	print_value( _GLshort4, &v );
	END;
	GLV.glVertex4sv( v );
}


/* */
void GLAPIENTRY
glVertexPointer ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
	START( glVertexPointer );
	print_value( _GLint, &size );
	print( "," );
	print_value( _GLenum, &type );
	print( "," );
	print_value( _GLsizei, &stride );
	print( "," );
	print_value( _GLvoid, &pointer );
	END;
	GLV.glVertexPointer( size, type, stride, pointer );
}


/* */
void GLAPIENTRY
glViewport ( GLint x, GLint y, GLsizei width, GLsizei height )
{
	START( glViewport );
	print_value( _GLint, &x );
	print( "," );
	print_value( _GLint, &y );
	print( "," );
	print_value( _GLsizei, &width );
	print( "," );
	print_value( _GLsizei, &height );
	END;
	GLV.glViewport( x, y, width, height );
}

#if defined WIN32 || defined WIN64


/* WGL functions */
int GLAPIENTRY
wglChoosePixelFormat ( HDC a, CONST PIXELFORMATDESCRIPTOR *b )
{
	int result;
	(void) gltraceInit();
	result = GLV.wglChoosePixelFormat( a, b );
	START( wglChoosePixelFormat );
	print_value( HEX, &a );
	print( "," );
	print_value( PFD, &b );
	RESULT;
	print_value( _GLint, &result );
	ENDRESULT;
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglCopyContext ( HGLRC a, HGLRC b, UINT c )
{
	BOOL	result;
	(void) gltraceInit();
	result = GLV.wglCopyContext( a, b, c );
	START( wglCopyContext );
	print_value( HEX, &a );
	print( "," );
	print_value( HEX, &b );
	print( "," );
	print_value( _GLuint, &c );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
HGLRC GLAPIENTRY
wglCreateContext ( HDC a )
{
	HGLRC	result;
	(void) gltraceInit();
	result = GLV.wglCreateContext( a );
	START( wglCreateContext );
	print_value( HEX, &a );
	RESULT;
	print_value( HEX, &result );
	ENDRESULT;
	return ( result );
}


/* */
HGLRC GLAPIENTRY
wglCreateLayerContext ( HDC a, int b )
{
	HGLRC	result;
	(void) gltraceInit();
	result = GLV.wglCreateLayerContext( a, b );
	START( wglCreateLayerContext );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLuint, &b );
	RESULT;
	print_value( HEX, &result );
	ENDRESULT;
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglDeleteContext ( HGLRC a )
{
	BOOL	result;
	(void) gltraceInit();
	result = GLV.wglDeleteContext( a );
	START( wglDeleteContext );
	print_value( HEX, &a );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglDescribeLayerPlane ( HDC a, int b, int c, UINT d, LPLAYERPLANEDESCRIPTOR e )
{
	BOOL	result;
	(void) gltraceInit();
	result = GLV.wglDescribeLayerPlane( a, b, c, d, e );
	START( wglDescribeLayerPlane );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLint, &b );
	print( "," );
	print_value( _GLint, &c );
	print( "," );
	print_value( _GLuint, &d );
	print( "," );
	print_value( LPD, &e );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
int GLAPIENTRY
wglDescribePixelFormat ( HDC a, int b, UINT c, LPPIXELFORMATDESCRIPTOR d )
{
	int result;
	(void) gltraceInit();
	result = GLV.wglDescribePixelFormat( a, b, c, d );
	START( wglDescribePixelFormat );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLint, &b );
	print( "," );
	print_value( _GLuint, &c );
	print( "," );
	print_value( PFD, &d );
	RESULT;
	print_value( _GLint, &result );
	ENDRESULT;
	return ( result );
}


/* */
HGLRC GLAPIENTRY
wglGetCurrentContext ( void )
{
	HGLRC	result;
	(void) gltraceInit();
	result = GLV.wglGetCurrentContext();
	START( wglGetCurrentContext );
	RESULT;
	print_value( HEX, &result );
	ENDRESULT;
	return ( result );
}


/* */
HDC GLAPIENTRY
wglGetCurrentDC ( void )
{
	HDC result;
	(void) gltraceInit();
	result = GLV.wglGetCurrentDC();
	START( wglGetCurrentDC );
	RESULT;
	print_value( HEX, &result );
	ENDRESULT;
	return ( result );
}


/* */
PROC GLAPIENTRY
wglGetDefaultProcAddress ( LPCSTR a )
{
	PROC	result;
	(void) gltraceInit();
	result = GLV.wglGetDefaultProcAddress( a );
	START( wglGetDefaultProcAddress );
	print_value( STR, &a );
	RESULT;
	print_value( HEX, &result );
	ENDRESULT;
	return ( result );
}


/* */
int GLAPIENTRY
wglGetLayerPaletteEntries ( HDC a, int b, int c, int d, COLORREF *e )
{
	int result;
	result = GLV.wglGetLayerPaletteEntries( a, b, c, d, e );
	START( wglGetLayerPaletteEntries );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLint, &b );
	print( "," );
	print_value( _GLint, &c );
	print( "," );
	print_value( _GLint, &d );
	print( "," );
	print_value( HEX, &e );
	RESULT;
	print_value( _GLint, &result );
	ENDRESULT;
	return ( result );
}


/* */
int GLAPIENTRY
wglGetPixelFormat ( HDC a )
{
	int result;
	(void) gltraceInit();
	result = GLV.wglGetPixelFormat( a );
	START( wglGetPixelFormat );
	print_value( HEX, &a );
	RESULT;
	print_value( _GLint, &result );
	ENDRESULT;
	return ( result );
}


/* */
PROC GLAPIENTRY
wglGetProcAddress ( LPCSTR a )
{
	int		index;
	PROC	result;
	(void) gltraceInit();
	result = GLV.wglGetProcAddress( a );
	START( wglGetProcAddress );
	print_value( STR, &a );
	RESULT;
	print_value( HEX, &result );
	ENDRESULT;
	index = GetEXTProcIndex( a );
	if ( index >= 0 && result != NULL )
	{
		PROC	result2;
		( (unsigned long *) (&GLV_EXT) )[index] = (unsigned long) result;
		result2 = ( PROC ) ( ((unsigned long *) (&GLV_EXT_RETURN))[index] );
		if ( result2 != NULL )
		{
			result = result2;
			print( "%s extension found\n", a );
		}
	}
	else
	{
		print( "%s extension NOT found\n", a );
	}

	return ( result );
}


/* */
BOOL GLAPIENTRY
wglMakeCurrent ( HDC a, HGLRC b )
{
	BOOL	result;
	(void) gltraceInit();
	result = GLV.wglMakeCurrent( a, b );
	START( wglMakeCurrent );
	print_value( HEX, &a );
	print( "," );
	print_value( HEX, &b );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglRealizeLayerPalette ( HDC a, int b, BOOL c )
{
	BOOL	result;
	result = GLV.wglRealizeLayerPalette( a, b, c );
	START( wglRealizeLayerPalette );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLint, &b );
	print( "," );
	print_value( _GLboolean, &c );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
int GLAPIENTRY
wglSetLayerPaletteEntries ( HDC a, int b, int c, int d, CONST COLORREF *e )
{
	int result;
	result = GLV.wglSetLayerPaletteEntries( a, b, c, d, e );
	START( wglSetLayerPaletteEntries );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLint, &b );
	print( "," );
	print_value( _GLint, &c );
	print( "," );
	print_value( _GLint, &d );
	print( "," );
	print_value( HEX, &e );
	RESULT;
	print_value( _GLint, &result );
	ENDRESULT;
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglSetPixelFormat ( HDC a, int b, CONST PIXELFORMATDESCRIPTOR *c )
{
	BOOL	result;
	(void) gltraceInit();
	result = GLV.wglSetPixelFormat( a, b, c );
	START( wglSetPixelFormat );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLint, &b );
	print( "," );
	print_value( PFD, &c );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglShareLists ( HGLRC a, HGLRC b )
{
	BOOL	result;
	result = GLV.wglShareLists( a, b );
	START( wglShareLists );
	print_value( HEX, &a );
	print( "," );
	print_value( HEX, &b );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglSwapBuffers ( HDC a )
{
	BOOL	result;

	// Render the FPS
	if ( bFSPenable == GL_TRUE )
	{
		renderFPS();
	}

	// Render the time!
	if ( bTimeEnabled == GL_TRUE )
	{
		renderTIME();
	}

	result = GLV.wglSwapBuffers( a );
	START( wglSwapBuffers );
	print_value( HEX, &a );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	updateKeys();
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglSwapLayerBuffers ( HDC a, UINT b )
{
	BOOL	result;

	/* is this right? */
	if ( bFSPenable == GL_TRUE )
	{
		renderFPS();
	}

	if ( bTimeEnabled == GL_TRUE )
	{
		renderTIME();
	}

	result = GLV.wglSwapLayerBuffers( a, b );
	START( wglSwapLayerBuffers );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLuint, &b );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	updateKeys();
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglUseFontBitmapsA ( HDC a, DWORD b, DWORD c, DWORD d )
{
	BOOL	result;
	result = GLV.wglUseFontBitmapsA( a, b, c, d );
	START( wglUseFontBitmapsA );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLint, &b );
	print( "," );
	print_value( _GLint, &c );
	print( "," );
	print_value( _GLint, &d );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglUseFontBitmapsW ( HDC a, DWORD b, DWORD c, DWORD d )
{
	BOOL	result;
	result = GLV.wglUseFontBitmapsW( a, b, c, d );
	START( wglUseFontBitmapsW );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLint, &b );
	print( "," );
	print_value( _GLint, &c );
	print( "," );
	print_value( _GLint, &d );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglUseFontOutlinesA ( HDC a, DWORD b, DWORD c, DWORD d, FLOAT e, FLOAT f, int g, LPGLYPHMETRICSFLOAT h )
{
	BOOL	result;
	result = GLV.wglUseFontOutlinesA( a, b, c, d, e, f, g, h );
	START( wglUseFontOutlinesA );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLint, &b );
	print( "," );
	print_value( _GLint, &c );
	print( "," );
	print_value( _GLint, &d );
	print( "," );
	print_value( _GLfloat, &e );
	print( "," );
	print_value( _GLfloat, &f );
	print( "," );
	print_value( _GLint, &g );
	print( "," );
	print_value( HEX, &h );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}


/* */
BOOL GLAPIENTRY
wglUseFontOutlinesW ( HDC a, DWORD b, DWORD c, DWORD d, FLOAT e, FLOAT f, int g, LPGLYPHMETRICSFLOAT h )
{
	BOOL	result;
	result = GLV.wglUseFontOutlinesW( a, b, c, d, e, f, g, h );
	START( wglUseFontOutlinesW );
	print_value( HEX, &a );
	print( "," );
	print_value( _GLint, &b );
	print( "," );
	print_value( _GLint, &c );
	print( "," );
	print_value( _GLint, &d );
	print( "," );
	print_value( _GLfloat, &e );
	print( "," );
	print_value( _GLfloat, &f );
	print( "," );
	print_value( _GLint, &g );
	print( "," );
	print_value( HEX, &h );
	RESULT;
	print_value( _GLboolean, &result );
	ENDRESULT;
	return ( result );
}

#else

/* GLX functions */
#endif
