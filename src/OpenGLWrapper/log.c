#include <sys/types.h>
#include <sys/timeb.h>
#include "gltrace.h"

/* Misc. globals */
extern void			extInitCallCount ( void );
extern void			extPrintCallCounts ( void );
extern struct timeb starttime;				/* Time when trace started */
extern FILE			*text_file;				/* Output text file (NULL for stdout) */
extern char			*GLenum_names[65536];	/* Pointer to text string for each GLenum value */
extern GLboolean	useZeroOne;				/* To correct GL_ZERO and GL_ONE overlap with other enumerants */
extern int			show_status[];
extern int			show_status_EXT[];
extern const char	*fmt = "  %-32s %d\n";

/* Keep track of how many times each GL function/extension is called. */
unsigned int		nCallCount[FP_LAST];
extern unsigned int nCallCountEXT[];
GL_ENUM_PAIR		PFD_flags_list[] =
{
	{ "PFD_DOUBLEBUFFER", PFD_DOUBLEBUFFER },
	{ "PFD_STEREO", PFD_STEREO },
	{ "PFD_DRAW_TO_WINDOW", PFD_DRAW_TO_WINDOW },
	{ "PFD_DRAW_TO_BITMAP", PFD_DRAW_TO_BITMAP },
	{ "PFD_SUPPORT_GDI", PFD_SUPPORT_GDI },
	{ "PFD_SUPPORT_OPENGL", PFD_SUPPORT_OPENGL },
	{ "PFD_GENERIC_FORMAT", PFD_GENERIC_FORMAT },
	{ "PFD_NEED_PALETTE", PFD_NEED_PALETTE },
	{ "PFD_NEED_SYSTEM_PALETTE", PFD_NEED_SYSTEM_PALETTE },
	{ "PFD_SWAP_EXCHANGE", PFD_SWAP_EXCHANGE },
	{ "PFD_SWAP_COPY", PFD_SWAP_COPY },
	{ "PFD_SWAP_LAYER_BUFFERS", PFD_SWAP_LAYER_BUFFERS },
	{ "PFD_GENERIC_ACCELERATED", PFD_GENERIC_ACCELERATED },
	{ "PFD_DEPTH_DONTCARE", PFD_DEPTH_DONTCARE },
	{ "PFD_DOUBLEBUFFER_DONTCARE", PFD_DOUBLEBUFFER_DONTCARE },
	{ "PFD_STEREO_DONTCARE", PFD_STEREO_DONTCARE }
};
GL_ENUM_PAIR		LPD_flags_list[] =
{
	{ "LPD_DOUBLEBUFFER", LPD_DOUBLEBUFFER },
	{ "LPD_STEREO", LPD_STEREO },
	{ "LPD_SUPPORT_GDI", LPD_SUPPORT_GDI },
	{ "LPD_SUPPORT_OPENGL", LPD_SUPPORT_OPENGL },
	{ "LPD_SWAP_EXCHANGE", LPD_SWAP_EXCHANGE },
	{ "LPD_SWAP_COPY", LPD_SWAP_COPY },
	{ "LPD_TRANSPARENT", LPD_TRANSPARENT },
	{ "LPD_SHARE_DEPTH", LPD_SHARE_DEPTH },
	{ "LPD_SHARE_STENCIL", LPD_SHARE_STENCIL },
	{ "LPD_SHARE_ACCUM", LPD_SHARE_ACCUM }
};
GL_ENUM_PAIR		GLbitfield_list[] =
{
	{ "GL_CURRENT_BIT", 0x00000001 },
	{ "GL_POINT_BIT", 0x00000002 },
	{ "GL_LINE_BIT", 0x00000004 },
	{ "GL_POLYGON_BIT", 0x00000008 },
	{ "GL_POLYGON_STIPPLE_BIT", 0x00000010 },
	{ "GL_PIXEL_MODE_BIT", 0x00000020 },
	{ "GL_LIGHTING_BIT", 0x00000040 },
	{ "GL_FOG_BIT", 0x00000080 },
	{ "GL_DEPTH_BUFFER_BIT", 0x00000100 },
	{ "GL_ACCUM_BUFFER_BIT", 0x00000200 },
	{ "GL_STENCIL_BUFFER_BIT", 0x00000400 },
	{ "GL_VIEWPORT_BIT", 0x00000800 },
	{ "GL_TRANSFORM_BIT", 0x00001000 },
	{ "GL_ENABLE_BIT", 0x00002000 },
	{ "GL_COLOR_BUFFER_BIT", 0x00004000 },
	{ "GL_HINT_BIT", 0x00008000 },
	{ "GL_EVAL_BIT", 0x00010000 },
	{ "GL_LIST_BIT", 0x00020000 },
	{ "GL_TEXTURE_BIT", 0x00040000 },
	{ "GL_SCISSOR_BIT", 0x00080000 },
	{ "GL_MULTISAMPLE_BIT_ARB", 0x20000000 }

	/*	{ "GL_ALL_ATTRIB_BITS",                0x000fffff} */
};
GL_ENUM_PAIR		bufferregionbitfield_list[] =
{
	{ "WGL_FRONT_COLOR_BUFFER_BIT_ARB", 0x00000001 },
	{ "WGL_BACK_COLOR_BUFFER_BIT_ARB", 0x00000002 },
	{ "WGL_DEPTH_BUFFER_BIT_ARB", 0x00000004 },
	{ "WGL_STENCIL_BUFFER_BIT_ARB", 0x00000008 }
};


/*
  Send output to file or debug console
*/
void
print ( const char *fmt, ... )
{
	static char work_string[4096];
	if ( (fmt == NULL) || (strlen( fmt) > sizeof(work_string)) )
	{
		strcpy( work_string, "(String missing or too large)" );
	}
	else
	{
		va_list ap;
		va_start( ap, fmt );
		(void) vsprintf( work_string, fmt, ap );
		va_end( ap );
	}

	if ( text_file == NULL )
	{
		printf( work_string );
	}
	else
	{
		(void) fputs( work_string, text_file );
	}
}


/*
  Print typed value by reference
*/
void
print_value ( TYPE type, const void *value )
{
	int						i, j;
	unsigned int			*v = (unsigned int *) value;
	signed char				*sc = *(signed char **) value;
	unsigned char			*uc = *(unsigned char **) value;
	short					*ss = *(short **) value;
	unsigned short			*us = *(unsigned short **) value;
	int						*si = *(int **) value;
	unsigned int			*ui = *(unsigned int **) value;
	float					*f = *(float **) value;
	double					*d = *(double **) value;
	PIXELFORMATDESCRIPTOR	*desc;
	LAYERPLANEDESCRIPTOR	*lpd;
	switch ( type )
	{
		default:
			print( "0x%X", *(unsigned *) v );
			break;
		case _GLvoid:
		case HEX:
			print( "0x%X", *(unsigned *) v );
			break;
		case _GLclampd:
		case _GLdouble:
			print( "%f", *(double *) v );
			break;
		case _GLdouble1:
			print( " { %f } ", d[0] );
			break;
		case _GLdouble2:
			print( " { %f, %f } ", d[0], d[1] );
			break;
		case _GLdouble3:
			print( " { %f, %f, %f } ", d[0], d[1], d[2] );
			break;
		case _GLdouble4:
			print( " { %f, %f, %f, %f } ", d[0], d[1], d[2], d[3] );
			break;
		case _GLdouble16:
			print( "\n" );
			print( " { %f, %f, %f, %f } ", d[0], d[4], d[8], d[12] );
			print( " { %f, %f, %f, %f } ", d[1], d[5], d[9], d[13] );
			print( " { %f, %f, %f, %f } ", d[2], d[6], d[10], d[14] );
			print( " { %f, %f, %f, %f } ", d[3], d[7], d[11], d[15] );
			break;
		case STR:
			print( "%s", *(char **) v );
			break;
		case _GLbyte:
			print( "%d", *(signed char *) v );
			break;
		case _GLbyte1:
			print( "{ %d } ", sc[0] );
			break;
		case _GLbyte2:
			print( "{ %d, %d } ", sc[0], sc[1] );
			break;
		case _GLbyte3:
			print( "{ %d, %d, %d } ", sc[0], sc[1], sc[2] );
			break;
		case _GLbyte4:
			print( "{ %d, %d, %d, %d } ", sc[0], sc[1], sc[2], sc[3] );
			break;
		case _GLubyte:
			print( "%u", *(unsigned char *) v );
			break;
		case _GLubyte1:
			print( "{ %u } ", uc[0] );
			break;
		case _GLubyte2:
			print( "{ %u, %u } ", uc[0], uc[1] );
			break;
		case _GLubyte3:
			print( "{ %u, %u, %u } ", uc[0], uc[1], uc[2] );
			break;
		case _GLubyte4:
			print( "{ %u, %u, %u, %u } ", uc[0], uc[1], uc[2], uc[3] );
			break;
		case _GLshort:
			print( "%d", *(short *) v );
			break;
		case _GLshort1:
			print( "{ %d } ", ss[0] );
			break;
		case _GLshort2:
			print( "{ %d, %d } ", ss[0], ss[1] );
			break;
		case _GLshort3:
			print( "{ %d, %d, %d } ", ss[0], ss[1], ss[2] );
			break;
		case _GLshort4:
			print( "{ %d, %d, %d, %d } ", ss[0], ss[1], ss[2], ss[3] );
			break;
		case _GLushort:
			print( "%u", *(unsigned short *) v );
			break;
		case _GLushort1:
			print( "{ %u } ", us[0] );
			break;
		case _GLushort2:
			print( "{ %u, %u } ", us[0], us[1] );
			break;
		case _GLushort3:
			print( "{ %u, %u, %u } ", us[0], us[1], us[2] );
			break;
		case _GLushort4:
			print( "{ %u, %u, %u, %u } ", us[0], us[1], us[2], us[3] );
			break;
		case _GLsizei:
		case _GLint:
			print( "%d", *(int *) v );
			break;
		case _GLint1:
			print( "{ %d } ", si[0] );
			break;
		case _GLint2:
			print( "{ %d, %d } ", si[0], si[1] );
			break;
		case _GLint3:
			print( "{ %d, %d, %d } ", si[0], si[1], si[2] );
			break;
		case _GLint4:
			print( "{ %d, %d, %d, %d } ", si[0], si[1], si[2], si[3] );
			break;
		case _GLuint:
			print( "%u", *(unsigned *) v );
			break;
		case _GLuint1:
			print( "{ %u } ", ui[0] );
			break;
		case _GLuint2:
			print( "{ %u, %u } ", ui[0], ui[1] );
			break;
		case _GLuint3:
			print( "{ %u, %u, %u } ", ui[0], ui[1], ui[2] );
			break;
		case _GLuint4:
			print( "{ %u, %u, %u, %u } ", ui[0], ui[1], ui[2], ui[3] );
			break;
		case _GLclampf:
		case _GLfloat:
			print( "%f", *(float *) v );
			break;
		case _GLfloat1:
			print( " { %f } ", f[0] );
			break;
		case _GLfloat2:
			print( " { %f, %f } ", f[0], f[1] );
			break;
		case _GLfloat3:
			print( " { %f, %f, %f } ", f[0], f[1], f[2] );
			break;
		case _GLfloat4:
			print( " { %f, %f, %f, %f } ", f[0], f[1], f[2], f[3] );
			break;
		case _GLfloat16:
			print( "\n" );
			print( " { %f, %f, %f, %f } ", f[0], f[4], f[8], f[12] );
			print( " { %f, %f, %f, %f } ", f[1], f[5], f[9], f[13] );
			print( " { %f, %f, %f, %f } ", f[2], f[6], f[10], f[14] );
			print( " { %f, %f, %f, %f } ", f[3], f[7], f[11], f[15] );
			break;
		case _GLboolean:
			print( "%s", (*(unsigned char *) v == GL_TRUE) ? "TRUE" : "FALSE" );
			break;
		case _GLboolean1:
			print( " { %s } ", (uc[0] == GL_TRUE) ? "TRUE" : "FALSE" );
			break;
		case _GLboolean4:
			print( " { %s, %s, %s, %s } ", (uc[0] == GL_TRUE) ? "TRUE" : "FALSE", (uc[1] == GL_TRUE) ? "TRUE" : "FALSE",
				   (uc[2] == GL_TRUE) ? "TRUE" : "FALSE", (uc[3] == GL_TRUE) ? "TRUE" : "FALSE" );
			break;

		/*
	  Print GLenum type from master name list
	*/
		case _GLenum:
			i = *(int *) v;
			if ( GLenum_names[i] == NULL )
			{
				print( "[%X]", i );
			}
			else
			{
				if ( (i < 2) && (useZeroOne == GL_TRUE) )
				{
					if ( i == 1 )
					{
						print( "GL_ONE" );
					}
					else
					{
						print( "GL_ZERO" );
					}
				}
				else
				{
					print( "%s", GLenum_names[i] );
				}
			}
			break;
		case _GLbitfield:
			{
				GLboolean	first = GL_TRUE;
				i = *(int *) v;
				for ( j = 0; j < ARY_CNT( GLbitfield_list); j++ )
				{
					if ( (GLbitfield_list[j].tag & i) != 0 )
					{
						if ( first == GL_FALSE )
						{
							print( " | " );
						}
						else
						{
							first = GL_FALSE;
						}

						print( "%s", GLbitfield_list[j].name );
					}
				}
			}
			break;
		case _bufferregionbitfield:
			{
				GLboolean	first = GL_TRUE;
				i = *(int *) v;
				for ( j = 0; j < ARY_CNT( bufferregionbitfield_list); j++ )
				{
					if ( (bufferregionbitfield_list[j].tag & i) != 0 )
					{
						if ( first == GL_FALSE )
						{
							print( " | " );
						}
						else
						{
							first = GL_FALSE;
						}

						print( "%s", bufferregionbitfield_list[j].name );
					}
				}
			}
			break;

		/*
	  (Example: How to dump contents of OpenGL-specific structure)
	*/
		case PFD:
			desc = *(PIXELFORMATDESCRIPTOR **) v;
			if ( desc != NULL )
			{
				print( "\n" );
				print( "     WORD  nSize = 0x%X\n", desc->nSize );
				print( "     WORD  nVersion = 0x%X\n", desc->nVersion );
				print( "     DWORD dwFlags = " );
				{
					GLboolean	first = GL_TRUE;
					for ( j = 0; j < ARY_CNT( PFD_flags_list); j++ )
					{
						if ( (PFD_flags_list[j].tag & desc->dwFlags) != 0 )
						{
							if ( first == GL_FALSE )
							{
								print( " | " );
							}
							else
							{
								first = GL_FALSE;
							}

							print( "%s", PFD_flags_list[j].name );
						}
					}

					print( "\n" );
				}

				print( "     BYTE  iPixelType = %s\n",
					   desc->iPixelType == (BYTE) PFD_TYPE_RGBA ? "PFD_TYPE_RGBA" : "PFD_TYPE_COLORINDEX" );
				print( "     BYTE  cColorBits = %d\n", desc->cColorBits );
				print( "     BYTE  cRedBits = %d\n", desc->cRedBits );
				print( "     BYTE  cRedShift = %d\n", desc->cRedShift );
				print( "     BYTE  cGreenBits = %d\n", desc->cGreenBits );
				print( "     BYTE  cGreenShift = %d\n", desc->cGreenShift );
				print( "     BYTE  cBlueBits = %d\n", desc->cBlueBits );
				print( "     BYTE  cBlueShift = %d\n", desc->cBlueShift );
				print( "     BYTE  cAlphaBits = %d\n", desc->cAlphaBits );
				print( "     BYTE  cAlphaShift = %d\n", desc->cAlphaShift );
				print( "     BYTE  cAccumBits = %d\n", desc->cAccumBits );
				print( "     BYTE  cAccumRedBits = %d\n", desc->cAccumRedBits );
				print( "     BYTE  cAccumGreenBits = %d\n", desc->cAccumGreenBits );
				print( "     BYTE  cAccumBlueBits = %d\n", desc->cAccumBlueBits );
				print( "     BYTE  cAccumAlphaBits = %d\n", desc->cAccumAlphaBits );
				print( "     BYTE  cDepthBits = %d\n", desc->cDepthBits );
				print( "     BYTE  cStencilBits = %d\n", desc->cStencilBits );
				print( "     BYTE  cAuxBuffers = %d\n", desc->cAuxBuffers );
				print( "     BYTE  iLayerType = 0x%X\n", desc->iLayerType );
				print( "     BYTE  bReserved = 0x%X\n", desc->bReserved );
				print( "     DWORD dwLayerMask = 0x%X\n", desc->dwLayerMask );
				print( "     DWORD dwVisibleMask = 0x%X\n", desc->dwVisibleMask );
				print( "     DWORD dwDamageMask = 0x%X\n", desc->dwDamageMask );
			}
			break;
		case LPD:
			lpd = *(LAYERPLANEDESCRIPTOR **) v;
			if ( lpd != NULL )
			{
				print( "\n" );
				print( "     WORD  nSize = 0x%X\n", lpd->nSize );
				print( "     WORD  nVersion = 0x%X\n", lpd->nVersion );
				print( "     DWORD dwFlags = " );
				{
					GLboolean	first = GL_TRUE;
					for ( j = 0; j < ARY_CNT( LPD_flags_list); j++ )
					{
						if ( (LPD_flags_list[j].tag & lpd->dwFlags) != 0 )
						{
							if ( first == GL_FALSE )
							{
								print( " | " );
							}
							else
							{
								first = GL_FALSE;
							}

							print( "%s", LPD_flags_list[j].name );
						}
					}

					print( "\n" );
				}

				print( "     BYTE  iPixelType = %s\n",
					   lpd->iPixelType == (BYTE) LPD_TYPE_RGBA ? "LPD_TYPE_RGBA" : "LPD_TYPE_COLORINDEX" );
				print( "     BYTE  cColorBits = %d\n", lpd->cColorBits );
				print( "     BYTE  cRedBits = %d\n", lpd->cRedBits );
				print( "     BYTE  cRedShift = %d\n", lpd->cRedShift );
				print( "     BYTE  cGreenBits = %d\n", lpd->cGreenBits );
				print( "     BYTE  cGreenShift = %d\n", lpd->cGreenShift );
				print( "     BYTE  cBlueBits = %d\n", lpd->cBlueBits );
				print( "     BYTE  cBlueShift = %d\n", lpd->cBlueShift );
				print( "     BYTE  cAlphaBits = %d\n", lpd->cAlphaBits );
				print( "     BYTE  cAlphaShift = %d\n", lpd->cAlphaShift );
				print( "     BYTE  cAccumBits = %d\n", lpd->cAccumBits );
				print( "     BYTE  cAccumRedBits = %d\n", lpd->cAccumRedBits );
				print( "     BYTE  cAccumGreenBits = %d\n", lpd->cAccumGreenBits );
				print( "     BYTE  cAccumBlueBits = %d\n", lpd->cAccumBlueBits );
				print( "     BYTE  cAccumAlphaBits = %d\n", lpd->cAccumAlphaBits );
				print( "     BYTE  cDepthBits = %d\n", lpd->cDepthBits );
				print( "     BYTE  cStencilBits = %d\n", lpd->cStencilBits );
				print( "     BYTE  cAuxBuffers = %d\n", lpd->cAuxBuffers );
				print( "     BYTE  iLayerPlane = 0x%X\n", lpd->iLayerPlane );
				print( "     BYTE  bReserved = 0x%X\n", lpd->bReserved );
				print( "     COLORREF crTransparent = 0x%X\n", lpd->crTransparent );
			}
			break;
	}
}


/* Print timestamp */
void
write_time ( void )
{
	struct timeb	tb;
	int				t, h, m, s, c;
	ftime( &tb );
	t = ( tb.time * 1000 ) + tb.millitm - ( starttime.time * 1000 ) - starttime.millitm;
	h = t / 3600000L;
	m = ( t / 60000L ) % 60L;
	s = ( t / 1000L ) % 60L;
	c = t % 1000L;
	print( "[%.02d:%.02d:%.02d.%.03d] ", h, m, s, c );
	s = tb.time - starttime.time;
	h = s / 360L;
	m = ( s / 60L ) % 60L;
	s = s % 60L;
	c = ( tb.millitm + 1000 - starttime.millitm ) % 1000L;
	print( "[%.02d:%.02d:%.02d.%.03d] ", h, m, s, c );
}


/* Initialise call counters to zero */
void
InitCallCount ( void )
{
	int i;
	if ( bCountCalls == GL_FALSE )
	{
		return;
	}

	for ( i = 0; i < FP_LAST; i++ )
	{
		nCallCount[i] = 0;
	}

	extInitCallCount();
}

extern char *GLN[];
extern char *GLN_EXT[];


/* Print call count tables */
void
PrintCallCounts ( void )
{
	int i;
	if ( bCountCalls == GL_FALSE )
	{
		return;
	}

	print( "\nCall Counts:\n" );

	/* NOTE: Do not print functions that were not called */
	for ( i = 0; i < FP_LAST; i++ )
	{
		if ( nCallCount[i] > 0 )
		{
			print( fmt, GLN[i], nCallCount[i] );
		}
	}

	print( "\n" );
	extPrintCallCounts();
	print( "\n" );
}
