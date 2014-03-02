#define VERSION "OGLWrapper Beta 1.15"
#if defined WIN32 || defined WIN64
#ifdef _MSC_VER
#pragma warning( disable : 4201 )
#pragma warning( disable : 4214 )
#pragma warning( disable : 4115 )
#pragma warning( disable : 4514 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4273 )	/* No complaints about DLL linkage... */
#endif /* _MSC_VER */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef _MSC_VER
#pragma warning( default : 4201 )
#pragma warning( default : 4214 )
#pragma warning( default : 4115 )
#endif /* _MSC_VER */
#endif /* WIN32 */
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

/* the Mesa header file */
#include "gl.h"
#define ARY_CNT( x )	(int) ( sizeof((x)) / sizeof((x)[0]) )

/* Argument/return type names and their format specifiers */
typedef enum
{
	_GLvoid,
	HEX,
	_GLbyte,
	_GLbyte1,
	_GLbyte2,
	_GLbyte3,
	_GLbyte4,
	_GLubyte,
	_GLubyte1,
	_GLubyte2,
	_GLubyte3,
	_GLubyte4,
	_GLshort,
	_GLshort1,
	_GLshort2,
	_GLshort3,
	_GLshort4,
	_GLushort,
	_GLushort1,
	_GLushort2,
	_GLushort3,
	_GLushort4,
	_GLsizei,
	_GLint,
	_GLint1,
	_GLint2,
	_GLint3,
	_GLint4,
	_GLuint,
	_GLuint1,
	_GLuint2,
	_GLuint3,
	_GLuint4,
	_GLclampf,
	_GLfloat,
	_GLfloat1,
	_GLfloat2,
	_GLfloat3,
	_GLfloat4,
	_GLfloat16,
	_GLclampd,
	_GLdouble,
	_GLdouble1,
	_GLdouble2,
	_GLdouble3,
	_GLdouble4,
	_GLdouble16,
	STR,
	_GLenum,
	_GLboolean,
	_GLboolean1,
	_GLboolean4,
	_GLbitfield,
	_bufferregionbitfield,
	PFD,
	LPD
} TYPE;
typedef struct
{
	char	*name;
	TYPE	type;
} FORMAT;
extern char *GLN[];
extern char *GLN_EXT[];

/* Numerical identifiers */
enum GLID
{
	FP_glAccum					= 0,
	FP_glAlphaFunc,
	FP_glAreTexturesResident,
	FP_glArrayElement,
	FP_glBegin,
	FP_glBindTexture,
	FP_glBitmap,
	FP_glBlendFunc,
	FP_glCallList,
	FP_glCallLists,
	FP_glClear,
	FP_glClearAccum,
	FP_glClearColor,
	FP_glClearDepth,
	FP_glClearIndex,
	FP_glClearStencil,
	FP_glClipPlane,
	FP_glColor3b,
	FP_glColor3bv,
	FP_glColor3d,
	FP_glColor3dv,
	FP_glColor3f,
	FP_glColor3fv,
	FP_glColor3i,
	FP_glColor3iv,
	FP_glColor3s,
	FP_glColor3sv,
	FP_glColor3ub,
	FP_glColor3ubv,
	FP_glColor3ui,
	FP_glColor3uiv,
	FP_glColor3us,
	FP_glColor3usv,
	FP_glColor4b,
	FP_glColor4bv,
	FP_glColor4d,
	FP_glColor4dv,
	FP_glColor4f,
	FP_glColor4fv,
	FP_glColor4i,
	FP_glColor4iv,
	FP_glColor4s,
	FP_glColor4sv,
	FP_glColor4ub,
	FP_glColor4ubv,
	FP_glColor4ui,
	FP_glColor4uiv,
	FP_glColor4us,
	FP_glColor4usv,
	FP_glColorMask,
	FP_glColorMaterial,
	FP_glColorPointer,
	FP_glCopyPixels,
	FP_glCopyTexImage1D,
	FP_glCopyTexImage2D,
	FP_glCopyTexSubImage1D,
	FP_glCopyTexSubImage2D,
	FP_glCullFace,
	FP_glDeleteLists,
	FP_glDeleteTextures,
	FP_glDepthFunc,
	FP_glDepthMask,
	FP_glDepthRange,
	FP_glDisable,
	FP_glDisableClientState,
	FP_glDrawArrays,
	FP_glDrawBuffer,
	FP_glDrawElements,
	FP_glDrawPixels,
	FP_glEdgeFlag,
	FP_glEdgeFlagPointer,
	FP_glEdgeFlagv,
	FP_glEnable,
	FP_glEnableClientState,
	FP_glEnd,
	FP_glEndList,
	FP_glEvalCoord1d,
	FP_glEvalCoord1dv,
	FP_glEvalCoord1f,
	FP_glEvalCoord1fv,
	FP_glEvalCoord2d,
	FP_glEvalCoord2dv,
	FP_glEvalCoord2f,
	FP_glEvalCoord2fv,
	FP_glEvalMesh1,
	FP_glEvalMesh2,
	FP_glEvalPoint1,
	FP_glEvalPoint2,
	FP_glFeedbackBuffer,
	FP_glFinish,
	FP_glFlush,
	FP_glFogf,
	FP_glFogfv,
	FP_glFogi,
	FP_glFogiv,
	FP_glFrontFace,
	FP_glFrustum,
	FP_glGenLists,
	FP_glGenTextures,
	FP_glGetBooleanv,
	FP_glGetClipPlane,
	FP_glGetDoublev,
	FP_glGetError,
	FP_glGetFloatv,
	FP_glGetIntegerv,
	FP_glGetLightfv,
	FP_glGetLightiv,
	FP_glGetMapdv,
	FP_glGetMapfv,
	FP_glGetMapiv,
	FP_glGetMaterialfv,
	FP_glGetMaterialiv,
	FP_glGetPixelMapfv,
	FP_glGetPixelMapuiv,
	FP_glGetPixelMapusv,
	FP_glGetPointerv,
	FP_glGetPolygonStipple,
	FP_glGetString,
	FP_glGetTexEnvfv,
	FP_glGetTexEnviv,
	FP_glGetTexGendv,
	FP_glGetTexGenfv,
	FP_glGetTexGeniv,
	FP_glGetTexImage,
	FP_glGetTexLevelParameterfv,
	FP_glGetTexLevelParameteriv,
	FP_glGetTexParameterfv,
	FP_glGetTexParameteriv,
	FP_glHint,
	FP_glIndexMask,
	FP_glIndexPointer,
	FP_glIndexd,
	FP_glIndexdv,
	FP_glIndexf,
	FP_glIndexfv,
	FP_glIndexi,
	FP_glIndexiv,
	FP_glIndexs,
	FP_glIndexsv,
	FP_glIndexub,
	FP_glIndexubv,
	FP_glInitNames,
	FP_glInterleavedArrays,
	FP_glIsEnabled,
	FP_glIsList,
	FP_glIsTexture,
	FP_glLightModelf,
	FP_glLightModelfv,
	FP_glLightModeli,
	FP_glLightModeliv,
	FP_glLightf,
	FP_glLightfv,
	FP_glLighti,
	FP_glLightiv,
	FP_glLineStipple,
	FP_glLineWidth,
	FP_glListBase,
	FP_glLoadIdentity,
	FP_glLoadMatrixd,
	FP_glLoadMatrixf,
	FP_glLoadName,
	FP_glLogicOp,
	FP_glMap1d,
	FP_glMap1f,
	FP_glMap2d,
	FP_glMap2f,
	FP_glMapGrid1d,
	FP_glMapGrid1f,
	FP_glMapGrid2d,
	FP_glMapGrid2f,
	FP_glMaterialf,
	FP_glMaterialfv,
	FP_glMateriali,
	FP_glMaterialiv,
	FP_glMatrixMode,
	FP_glMultMatrixd,
	FP_glMultMatrixf,
	FP_glNewList,
	FP_glNormal3b,
	FP_glNormal3bv,
	FP_glNormal3d,
	FP_glNormal3dv,
	FP_glNormal3f,
	FP_glNormal3fv,
	FP_glNormal3i,
	FP_glNormal3iv,
	FP_glNormal3s,
	FP_glNormal3sv,
	FP_glNormalPointer,
	FP_glOrtho,
	FP_glPassThrough,
	FP_glPixelMapfv,
	FP_glPixelMapuiv,
	FP_glPixelMapusv,
	FP_glPixelStoref,
	FP_glPixelStorei,
	FP_glPixelTransferf,
	FP_glPixelTransferi,
	FP_glPixelZoom,
	FP_glPointSize,
	FP_glPolygonMode,
	FP_glPolygonOffset,
	FP_glPolygonStipple,
	FP_glPopAttrib,
	FP_glPopClientAttrib,
	FP_glPopMatrix,
	FP_glPopName,
	FP_glPrioritizeTextures,
	FP_glPushAttrib,
	FP_glPushClientAttrib,
	FP_glPushMatrix,
	FP_glPushName,
	FP_glRasterPos2d,
	FP_glRasterPos2dv,
	FP_glRasterPos2f,
	FP_glRasterPos2fv,
	FP_glRasterPos2i,
	FP_glRasterPos2iv,
	FP_glRasterPos2s,
	FP_glRasterPos2sv,
	FP_glRasterPos3d,
	FP_glRasterPos3dv,
	FP_glRasterPos3f,
	FP_glRasterPos3fv,
	FP_glRasterPos3i,
	FP_glRasterPos3iv,
	FP_glRasterPos3s,
	FP_glRasterPos3sv,
	FP_glRasterPos4d,
	FP_glRasterPos4dv,
	FP_glRasterPos4f,
	FP_glRasterPos4fv,
	FP_glRasterPos4i,
	FP_glRasterPos4iv,
	FP_glRasterPos4s,
	FP_glRasterPos4sv,
	FP_glReadBuffer,
	FP_glReadPixels,
	FP_glRectd,
	FP_glRectdv,
	FP_glRectf,
	FP_glRectfv,
	FP_glRecti,
	FP_glRectiv,
	FP_glRects,
	FP_glRectsv,
	FP_glRenderMode,
	FP_glRotated,
	FP_glRotatef,
	FP_glScaled,
	FP_glScalef,
	FP_glScissor,
	FP_glSelectBuffer,
	FP_glShadeModel,
	FP_glStencilFunc,
	FP_glStencilMask,
	FP_glStencilOp,
	FP_glTexCoord1d,
	FP_glTexCoord1dv,
	FP_glTexCoord1f,
	FP_glTexCoord1fv,
	FP_glTexCoord1i,
	FP_glTexCoord1iv,
	FP_glTexCoord1s,
	FP_glTexCoord1sv,
	FP_glTexCoord2d,
	FP_glTexCoord2dv,
	FP_glTexCoord2f,
	FP_glTexCoord2fv,
	FP_glTexCoord2i,
	FP_glTexCoord2iv,
	FP_glTexCoord2s,
	FP_glTexCoord2sv,
	FP_glTexCoord3d,
	FP_glTexCoord3dv,
	FP_glTexCoord3f,
	FP_glTexCoord3fv,
	FP_glTexCoord3i,
	FP_glTexCoord3iv,
	FP_glTexCoord3s,
	FP_glTexCoord3sv,
	FP_glTexCoord4d,
	FP_glTexCoord4dv,
	FP_glTexCoord4f,
	FP_glTexCoord4fv,
	FP_glTexCoord4i,
	FP_glTexCoord4iv,
	FP_glTexCoord4s,
	FP_glTexCoord4sv,
	FP_glTexCoordPointer,
	FP_glTexEnvf,
	FP_glTexEnvfv,
	FP_glTexEnvi,
	FP_glTexEnviv,
	FP_glTexGend,
	FP_glTexGendv,
	FP_glTexGenf,
	FP_glTexGenfv,
	FP_glTexGeni,
	FP_glTexGeniv,
	FP_glTexImage1D,
	FP_glTexImage2D,
	FP_glTexParameterf,
	FP_glTexParameterfv,
	FP_glTexParameteri,
	FP_glTexParameteriv,
	FP_glTexSubImage1D,
	FP_glTexSubImage2D,
	FP_glTranslated,
	FP_glTranslatef,
	FP_glVertex2d,
	FP_glVertex2dv,
	FP_glVertex2f,
	FP_glVertex2fv,
	FP_glVertex2i,
	FP_glVertex2iv,
	FP_glVertex2s,
	FP_glVertex2sv,
	FP_glVertex3d,
	FP_glVertex3dv,
	FP_glVertex3f,
	FP_glVertex3fv,
	FP_glVertex3i,
	FP_glVertex3iv,
	FP_glVertex3s,
	FP_glVertex3sv,
	FP_glVertex4d,
	FP_glVertex4dv,
	FP_glVertex4f,
	FP_glVertex4fv,
	FP_glVertex4i,
	FP_glVertex4iv,
	FP_glVertex4s,
	FP_glVertex4sv,
	FP_glVertexPointer,
	FP_glViewport,
#if defined WIN32 || defined WIN64
	FP_wglChoosePixelFormat,
	FP_wglCopyContext,
	FP_wglCreateContext,
	FP_wglCreateLayerContext,
	FP_wglDeleteContext,
	FP_wglDescribeLayerPlane,
	FP_wglDescribePixelFormat,
	FP_wglGetCurrentContext,
	FP_wglGetCurrentDC,
	FP_wglGetDefaultProcAddress,
	FP_wglGetLayerPaletteEntries,
	FP_wglGetPixelFormat,
	FP_wglGetProcAddress,
	FP_wglMakeCurrent,
	FP_wglRealizeLayerPalette,
	FP_wglSetLayerPaletteEntries,
	FP_wglSetPixelFormat,
	FP_wglShareLists,
	FP_wglSwapBuffers,
	FP_wglSwapLayerBuffers,
	FP_wglUseFontBitmapsA,
	FP_wglUseFontBitmapsW,
	FP_wglUseFontOutlinesA,
	FP_wglUseFontOutlinesW,
#else
#endif
	FP_LAST
};

/* Status values */
#define SUPPRESS			0		/* Never trace this function */
#define SHOW_ALWAYS			1		/* Trace this function */
#define SHOW_IF_SHIFT_DOWN	2		/* Trace this function only if user holding down */

/* SHIFT or ALT key */

/* Jump table vectors to GLTRACE entries (aka "real" OpenGL driver) */
struct
{
	void ( GLAPIENTRY *glAccum ) ( GLenum op, GLfloat value );
	void ( GLAPIENTRY *glAlphaFunc ) ( GLenum func, GLclampf ref );
	GLboolean ( GLAPIENTRY *glAreTexturesResident ) ( GLsizei n, const GLuint * textures, GLboolean * residences );
	void ( GLAPIENTRY *glArrayElement ) ( GLint i );
	void ( GLAPIENTRY *glBegin ) ( GLenum mode );
	void ( GLAPIENTRY *glBindTexture ) ( GLenum target, GLuint texture );
	void ( GLAPIENTRY *glBitmap )
		(
			GLsizei width,
			GLsizei height,
			GLfloat xorig,
			GLfloat yorig,
			GLfloat xmove,
			GLfloat ymove,
			const GLubyte * bitmap
		);
	void ( GLAPIENTRY *glBlendFunc ) ( GLenum sfactor, GLenum dfactor );
	void ( GLAPIENTRY *glCallList ) ( GLuint list );
	void ( GLAPIENTRY *glCallLists ) ( GLsizei n, GLenum type, const GLvoid * lists );
	void ( GLAPIENTRY *glClear ) ( GLbitfield mask );
	void ( GLAPIENTRY *glClearAccum ) ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
	void ( GLAPIENTRY *glClearColor ) ( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
	void ( GLAPIENTRY *glClearDepth ) ( GLclampd depth );
	void ( GLAPIENTRY *glClearIndex ) ( GLfloat c );
	void ( GLAPIENTRY *glClearStencil ) ( GLint s );
	void ( GLAPIENTRY *glClipPlane ) ( GLenum plane, const GLdouble * equation );
	void ( GLAPIENTRY *glColor3b ) ( GLbyte red, GLbyte green, GLbyte blue );
	void ( GLAPIENTRY *glColor3bv ) (const GLbyte *v);
	void ( GLAPIENTRY *glColor3d ) ( GLdouble red, GLdouble green, GLdouble blue );
	void ( GLAPIENTRY *glColor3dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glColor3f ) ( GLfloat red, GLfloat green, GLfloat blue );
	void ( GLAPIENTRY *glColor3fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glColor3i ) ( GLint red, GLint green, GLint blue );
	void ( GLAPIENTRY *glColor3iv ) (const GLint *v);
	void ( GLAPIENTRY *glColor3s ) ( GLshort red, GLshort green, GLshort blue );
	void ( GLAPIENTRY *glColor3sv ) (const GLshort *v);
	void ( GLAPIENTRY *glColor3ub ) ( GLubyte red, GLubyte green, GLubyte blue );
	void ( GLAPIENTRY *glColor3ubv ) (const GLubyte *v);
	void ( GLAPIENTRY *glColor3ui ) ( GLuint red, GLuint green, GLuint blue );
	void ( GLAPIENTRY *glColor3uiv ) (const GLuint *v);
	void ( GLAPIENTRY *glColor3us ) ( GLushort red, GLushort green, GLushort blue );
	void ( GLAPIENTRY *glColor3usv ) (const GLushort *v);
	void ( GLAPIENTRY *glColor4b ) ( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha );
	void ( GLAPIENTRY *glColor4bv ) (const GLbyte *v);
	void ( GLAPIENTRY *glColor4d ) ( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha );
	void ( GLAPIENTRY *glColor4dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glColor4f ) ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
	void ( GLAPIENTRY *glColor4fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glColor4i ) ( GLint red, GLint green, GLint blue, GLint alpha );
	void ( GLAPIENTRY *glColor4iv ) (const GLint *v);
	void ( GLAPIENTRY *glColor4s ) ( GLshort red, GLshort green, GLshort blue, GLshort alpha );
	void ( GLAPIENTRY *glColor4sv ) (const GLshort *v);
	void ( GLAPIENTRY *glColor4ub ) ( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha );
	void ( GLAPIENTRY *glColor4ubv ) (const GLubyte *v);
	void ( GLAPIENTRY *glColor4ui ) ( GLuint red, GLuint green, GLuint blue, GLuint alpha );
	void ( GLAPIENTRY *glColor4uiv ) (const GLuint *v);
	void ( GLAPIENTRY *glColor4us ) ( GLushort red, GLushort green, GLushort blue, GLushort alpha );
	void ( GLAPIENTRY *glColor4usv ) (const GLushort *v);
	void ( GLAPIENTRY *glColorMask ) ( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
	void ( GLAPIENTRY *glColorMaterial ) ( GLenum face, GLenum mode );
	void ( GLAPIENTRY *glColorPointer ) ( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer );
	void ( GLAPIENTRY *glCopyPixels ) ( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );
	void ( GLAPIENTRY *glCopyTexImage1D )
		(
			GLenum target,
			GLint level,
			GLenum internalFormat,
			GLint x,
			GLint y,
			GLsizei width,
			GLint border
		);
	void ( GLAPIENTRY *glCopyTexImage2D )
		(
			GLenum target,
			GLint level,
			GLenum internalFormat,
			GLint x,
			GLint y,
			GLsizei width,
			GLsizei height,
			GLint border
		);
	void ( GLAPIENTRY *glCopyTexSubImage1D )
		(
			GLenum target,
			GLint level,
			GLint xoffset,
			GLint x,
			GLint y,
			GLsizei width
		);
	void ( GLAPIENTRY *glCopyTexSubImage2D )
		(
			GLenum target,
			GLint level,
			GLint xoffset,
			GLint yoffset,
			GLint x,
			GLint y,
			GLsizei width,
			GLsizei height
		);
	void ( GLAPIENTRY *glCullFace ) ( GLenum mode );
	void ( GLAPIENTRY *glDeleteLists ) ( GLuint list, GLsizei range );
	void ( GLAPIENTRY *glDeleteTextures ) ( GLsizei n, const GLuint * textures );
	void ( GLAPIENTRY *glDepthFunc ) ( GLenum func );
	void ( GLAPIENTRY *glDepthMask ) ( GLboolean flag );
	void ( GLAPIENTRY *glDepthRange ) ( GLclampd zNear, GLclampd zFar );
	void ( GLAPIENTRY *glDisable ) ( GLenum cap );
	void ( GLAPIENTRY *glDisableClientState ) ( GLenum array );
	void ( GLAPIENTRY *glDrawArrays ) ( GLenum mode, GLint first, GLsizei count );
	void ( GLAPIENTRY *glDrawBuffer ) ( GLenum mode );
	void ( GLAPIENTRY *glDrawElements ) ( GLenum mode, GLsizei count, GLenum type, const GLvoid * indices );
	void ( GLAPIENTRY *glDrawPixels )
		(
			GLsizei width,
			GLsizei height,
			GLenum format,
			GLenum type,
			const GLvoid * pixels
		);
	void ( GLAPIENTRY *glEdgeFlag ) ( GLboolean flag );
	void ( GLAPIENTRY *glEdgeFlagPointer ) ( GLsizei stride, const GLvoid * pointer );
	void ( GLAPIENTRY *glEdgeFlagv ) (const GLboolean *flag);
	void ( GLAPIENTRY *glEnable ) ( GLenum cap );
	void ( GLAPIENTRY *glEnableClientState ) ( GLenum array );
	void ( GLAPIENTRY *glEnd ) (void);
	void ( GLAPIENTRY *glEndList ) (void);
	void ( GLAPIENTRY *glEvalCoord1d ) ( GLdouble u );
	void ( GLAPIENTRY *glEvalCoord1dv ) (const GLdouble *u);
	void ( GLAPIENTRY *glEvalCoord1f ) ( GLfloat u );
	void ( GLAPIENTRY *glEvalCoord1fv ) (const GLfloat *u);
	void ( GLAPIENTRY *glEvalCoord2d ) ( GLdouble u, GLdouble v );
	void ( GLAPIENTRY *glEvalCoord2dv ) (const GLdouble *u);
	void ( GLAPIENTRY *glEvalCoord2f ) ( GLfloat u, GLfloat v );
	void ( GLAPIENTRY *glEvalCoord2fv ) (const GLfloat *u);
	void ( GLAPIENTRY *glEvalMesh1 ) ( GLenum mode, GLint i1, GLint i2 );
	void ( GLAPIENTRY *glEvalMesh2 ) ( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );
	void ( GLAPIENTRY *glEvalPoint1 ) ( GLint i );
	void ( GLAPIENTRY *glEvalPoint2 ) ( GLint i, GLint j );
	void ( GLAPIENTRY *glFeedbackBuffer ) ( GLsizei size, GLenum type, GLfloat * buffer );
	void ( GLAPIENTRY *glFinish ) (void);
	void ( GLAPIENTRY *glFlush ) (void);
	void ( GLAPIENTRY *glFogf ) ( GLenum pname, GLfloat param );
	void ( GLAPIENTRY *glFogfv ) ( GLenum pname, const GLfloat * params );
	void ( GLAPIENTRY *glFogi ) ( GLenum pname, GLint param );
	void ( GLAPIENTRY *glFogiv ) ( GLenum pname, const GLint * params );
	void ( GLAPIENTRY *glFrontFace ) ( GLenum mode );
	void ( GLAPIENTRY *glFrustum )
		(
			GLdouble left,
			GLdouble right,
			GLdouble bottom,
			GLdouble top,
			GLdouble zNear,
			GLdouble zFar
		);
	GLuint ( GLAPIENTRY *glGenLists ) ( GLsizei range );
	void ( GLAPIENTRY *glGenTextures ) ( GLsizei n, GLuint * textures );
	void ( GLAPIENTRY *glGetBooleanv ) ( GLenum pname, GLboolean * params );
	void ( GLAPIENTRY *glGetClipPlane ) ( GLenum plane, GLdouble * equation );
	void ( GLAPIENTRY *glGetDoublev ) ( GLenum pname, GLdouble * params );
	GLenum ( GLAPIENTRY *glGetError ) (void);
	void ( GLAPIENTRY *glGetFloatv ) ( GLenum pname, GLfloat * params );
	void ( GLAPIENTRY *glGetIntegerv ) ( GLenum pname, GLint * params );
	void ( GLAPIENTRY *glGetLightfv ) ( GLenum light, GLenum pname, GLfloat * params );
	void ( GLAPIENTRY *glGetLightiv ) ( GLenum light, GLenum pname, GLint * params );
	void ( GLAPIENTRY *glGetMapdv ) ( GLenum target, GLenum query, GLdouble * v );
	void ( GLAPIENTRY *glGetMapfv ) ( GLenum target, GLenum query, GLfloat * v );
	void ( GLAPIENTRY *glGetMapiv ) ( GLenum target, GLenum query, GLint * v );
	void ( GLAPIENTRY *glGetMaterialfv ) ( GLenum face, GLenum pname, GLfloat * params );
	void ( GLAPIENTRY *glGetMaterialiv ) ( GLenum face, GLenum pname, GLint * params );
	void ( GLAPIENTRY *glGetPixelMapfv ) ( GLenum map, GLfloat * values );
	void ( GLAPIENTRY *glGetPixelMapuiv ) ( GLenum map, GLuint * values );
	void ( GLAPIENTRY *glGetPixelMapusv ) ( GLenum map, GLushort * values );
	void ( GLAPIENTRY *glGetPointerv ) ( GLenum pname, GLvoid **params );
	void ( GLAPIENTRY *glGetPolygonStipple ) ( GLubyte * mask );
	const GLubyte * ( GLAPIENTRY * glGetString ) ( GLenum name );
	void ( GLAPIENTRY *glGetTexEnvfv ) ( GLenum target, GLenum pname, GLfloat * params );
	void ( GLAPIENTRY *glGetTexEnviv ) ( GLenum target, GLenum pname, GLint * params );
	void ( GLAPIENTRY *glGetTexGendv ) ( GLenum coord, GLenum pname, GLdouble * params );
	void ( GLAPIENTRY *glGetTexGenfv ) ( GLenum coord, GLenum pname, GLfloat * params );
	void ( GLAPIENTRY *glGetTexGeniv ) ( GLenum coord, GLenum pname, GLint * params );
	void ( GLAPIENTRY *glGetTexImage ) ( GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels );
	void ( GLAPIENTRY *glGetTexLevelParameterfv ) ( GLenum target, GLint level, GLenum pname, GLfloat * params );
	void ( GLAPIENTRY *glGetTexLevelParameteriv ) ( GLenum target, GLint level, GLenum pname, GLint * params );
	void ( GLAPIENTRY *glGetTexParameterfv ) ( GLenum target, GLenum pname, GLfloat * params );
	void ( GLAPIENTRY *glGetTexParameteriv ) ( GLenum target, GLenum pname, GLint * params );
	void ( GLAPIENTRY *glHint ) ( GLenum target, GLenum mode );
	void ( GLAPIENTRY *glIndexMask ) ( GLuint mask );
	void ( GLAPIENTRY *glIndexPointer ) ( GLenum type, GLsizei stride, const GLvoid * pointer );
	void ( GLAPIENTRY *glIndexd ) ( GLdouble c );
	void ( GLAPIENTRY *glIndexdv ) (const GLdouble *c);
	void ( GLAPIENTRY *glIndexf ) ( GLfloat c );
	void ( GLAPIENTRY *glIndexfv ) (const GLfloat *c);
	void ( GLAPIENTRY *glIndexi ) ( GLint c );
	void ( GLAPIENTRY *glIndexiv ) (const GLint *c);
	void ( GLAPIENTRY *glIndexs ) ( GLshort c );
	void ( GLAPIENTRY *glIndexsv ) (const GLshort *c);
	void ( GLAPIENTRY *glIndexub ) ( GLubyte c );
	void ( GLAPIENTRY *glIndexubv ) (const GLubyte *c);
	void ( GLAPIENTRY *glInitNames ) (void);
	void ( GLAPIENTRY *glInterleavedArrays ) ( GLenum format, GLsizei stride, const GLvoid * pointer );
	GLboolean ( GLAPIENTRY *glIsEnabled ) ( GLenum cap );
	GLboolean ( GLAPIENTRY *glIsList ) ( GLuint list );
	GLboolean ( GLAPIENTRY *glIsTexture ) ( GLuint texture );
	void ( GLAPIENTRY *glLightModelf ) ( GLenum pname, GLfloat param );
	void ( GLAPIENTRY *glLightModelfv ) ( GLenum pname, const GLfloat * params );
	void ( GLAPIENTRY *glLightModeli ) ( GLenum pname, GLint param );
	void ( GLAPIENTRY *glLightModeliv ) ( GLenum pname, const GLint * params );
	void ( GLAPIENTRY *glLightf ) ( GLenum light, GLenum pname, GLfloat param );
	void ( GLAPIENTRY *glLightfv ) ( GLenum light, GLenum pname, const GLfloat * params );
	void ( GLAPIENTRY *glLighti ) ( GLenum light, GLenum pname, GLint param );
	void ( GLAPIENTRY *glLightiv ) ( GLenum light, GLenum pname, const GLint * params );
	void ( GLAPIENTRY *glLineStipple ) ( GLint factor, GLushort pattern );
	void ( GLAPIENTRY *glLineWidth ) ( GLfloat width );
	void ( GLAPIENTRY *glListBase ) ( GLuint base );
	void ( GLAPIENTRY *glLoadIdentity ) (void);
	void ( GLAPIENTRY *glLoadMatrixd ) (const GLdouble *m);
	void ( GLAPIENTRY *glLoadMatrixf ) (const GLfloat *m);
	void ( GLAPIENTRY *glLoadName ) ( GLuint name );
	void ( GLAPIENTRY *glLogicOp ) ( GLenum opcode );
	void ( GLAPIENTRY *glMap1d )
		(
			GLenum target,
			GLdouble u1,
			GLdouble u2,
			GLint stride,
			GLint order,
			const GLdouble * points
		);
	void ( GLAPIENTRY *glMap1f )
		(
			GLenum target,
			GLfloat u1,
			GLfloat u2,
			GLint stride,
			GLint order,
			const GLfloat * points
		);
	void ( GLAPIENTRY *glMap2d )
		(
			GLenum target,
			GLdouble u1,
			GLdouble u2,
			GLint ustride,
			GLint uorder,
			GLdouble v1,
			GLdouble v2,
			GLint vstride,
			GLint vorder,
			const GLdouble * points
		);
	void ( GLAPIENTRY *glMap2f )
		(
			GLenum target,
			GLfloat u1,
			GLfloat u2,
			GLint ustride,
			GLint uorder,
			GLfloat v1,
			GLfloat v2,
			GLint vstride,
			GLint vorder,
			const GLfloat * points
		);
	void ( GLAPIENTRY *glMapGrid1d ) ( GLint un, GLdouble u1, GLdouble u2 );
	void ( GLAPIENTRY *glMapGrid1f ) ( GLint un, GLfloat u1, GLfloat u2 );
	void ( GLAPIENTRY *glMapGrid2d ) ( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 );
	void ( GLAPIENTRY *glMapGrid2f ) ( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 );
	void ( GLAPIENTRY *glMaterialf ) ( GLenum face, GLenum pname, GLfloat param );
	void ( GLAPIENTRY *glMaterialfv ) ( GLenum face, GLenum pname, const GLfloat * params );
	void ( GLAPIENTRY *glMateriali ) ( GLenum face, GLenum pname, GLint param );
	void ( GLAPIENTRY *glMaterialiv ) ( GLenum face, GLenum pname, const GLint * params );
	void ( GLAPIENTRY *glMatrixMode ) ( GLenum mode );
	void ( GLAPIENTRY *glMultMatrixd ) (const GLdouble *m);
	void ( GLAPIENTRY *glMultMatrixf ) (const GLfloat *m);
	void ( GLAPIENTRY *glNewList ) ( GLuint list, GLenum mode );
	void ( GLAPIENTRY *glNormal3b ) ( GLbyte nx, GLbyte ny, GLbyte nz );
	void ( GLAPIENTRY *glNormal3bv ) (const GLbyte *v);
	void ( GLAPIENTRY *glNormal3d ) ( GLdouble nx, GLdouble ny, GLdouble nz );
	void ( GLAPIENTRY *glNormal3dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glNormal3f ) ( GLfloat nx, GLfloat ny, GLfloat nz );
	void ( GLAPIENTRY *glNormal3fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glNormal3i ) ( GLint nx, GLint ny, GLint nz );
	void ( GLAPIENTRY *glNormal3iv ) (const GLint *v);
	void ( GLAPIENTRY *glNormal3s ) ( GLshort nx, GLshort ny, GLshort nz );
	void ( GLAPIENTRY *glNormal3sv ) (const GLshort *v);
	void ( GLAPIENTRY *glNormalPointer ) ( GLenum type, GLsizei stride, const GLvoid * pointer );
	void ( GLAPIENTRY *glOrtho )
		(
			GLdouble left,
			GLdouble right,
			GLdouble bottom,
			GLdouble top,
			GLdouble zNear,
			GLdouble zFar
		);
	void ( GLAPIENTRY *glPassThrough ) ( GLfloat token );
	void ( GLAPIENTRY *glPixelMapfv ) ( GLenum map, GLsizei mapsize, const GLfloat * values );
	void ( GLAPIENTRY *glPixelMapuiv ) ( GLenum map, GLsizei mapsize, const GLuint * values );
	void ( GLAPIENTRY *glPixelMapusv ) ( GLenum map, GLsizei mapsize, const GLushort * values );
	void ( GLAPIENTRY *glPixelStoref ) ( GLenum pname, GLfloat param );
	void ( GLAPIENTRY *glPixelStorei ) ( GLenum pname, GLint param );
	void ( GLAPIENTRY *glPixelTransferf ) ( GLenum pname, GLfloat param );
	void ( GLAPIENTRY *glPixelTransferi ) ( GLenum pname, GLint param );
	void ( GLAPIENTRY *glPixelZoom ) ( GLfloat xfactor, GLfloat yfactor );
	void ( GLAPIENTRY *glPointSize ) ( GLfloat size );
	void ( GLAPIENTRY *glPolygonMode ) ( GLenum face, GLenum mode );
	void ( GLAPIENTRY *glPolygonOffset ) ( GLfloat factor, GLfloat units );
	void ( GLAPIENTRY *glPolygonStipple ) (const GLubyte *mask);
	void ( GLAPIENTRY *glPopAttrib ) (void);
	void ( GLAPIENTRY *glPopClientAttrib ) (void);
	void ( GLAPIENTRY *glPopMatrix ) (void);
	void ( GLAPIENTRY *glPopName ) (void);
	void ( GLAPIENTRY *glPrioritizeTextures ) ( GLsizei n, const GLuint * textures, const GLclampf * priorities );
	void ( GLAPIENTRY *glPushAttrib ) ( GLbitfield mask );
	void ( GLAPIENTRY *glPushClientAttrib ) ( GLbitfield mask );
	void ( GLAPIENTRY *glPushMatrix ) (void);
	void ( GLAPIENTRY *glPushName ) ( GLuint name );
	void ( GLAPIENTRY *glRasterPos2d ) ( GLdouble x, GLdouble y );
	void ( GLAPIENTRY *glRasterPos2dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glRasterPos2f ) ( GLfloat x, GLfloat y );
	void ( GLAPIENTRY *glRasterPos2fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glRasterPos2i ) ( GLint x, GLint y );
	void ( GLAPIENTRY *glRasterPos2iv ) (const GLint *v);
	void ( GLAPIENTRY *glRasterPos2s ) ( GLshort x, GLshort y );
	void ( GLAPIENTRY *glRasterPos2sv ) (const GLshort *v);
	void ( GLAPIENTRY *glRasterPos3d ) ( GLdouble x, GLdouble y, GLdouble z );
	void ( GLAPIENTRY *glRasterPos3dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glRasterPos3f ) ( GLfloat x, GLfloat y, GLfloat z );
	void ( GLAPIENTRY *glRasterPos3fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glRasterPos3i ) ( GLint x, GLint y, GLint z );
	void ( GLAPIENTRY *glRasterPos3iv ) (const GLint *v);
	void ( GLAPIENTRY *glRasterPos3s ) ( GLshort x, GLshort y, GLshort z );
	void ( GLAPIENTRY *glRasterPos3sv ) (const GLshort *v);
	void ( GLAPIENTRY *glRasterPos4d ) ( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
	void ( GLAPIENTRY *glRasterPos4dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glRasterPos4f ) ( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
	void ( GLAPIENTRY *glRasterPos4fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glRasterPos4i ) ( GLint x, GLint y, GLint z, GLint w );
	void ( GLAPIENTRY *glRasterPos4iv ) (const GLint *v);
	void ( GLAPIENTRY *glRasterPos4s ) ( GLshort x, GLshort y, GLshort z, GLshort w );
	void ( GLAPIENTRY *glRasterPos4sv ) (const GLshort *v);
	void ( GLAPIENTRY *glReadBuffer ) ( GLenum mode );
	void ( GLAPIENTRY *glReadPixels )
		(
			GLint x,
			GLint y,
			GLsizei width,
			GLsizei height,
			GLenum format,
			GLenum type,
			GLvoid * pixels
		);
	void ( GLAPIENTRY *glRectd ) ( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
	void ( GLAPIENTRY *glRectdv ) (const GLdouble *v1, const GLdouble *v2);
	void ( GLAPIENTRY *glRectf ) ( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
	void ( GLAPIENTRY *glRectfv ) (const GLfloat *v1, const GLfloat *v2);
	void ( GLAPIENTRY *glRecti ) ( GLint x1, GLint y1, GLint x2, GLint y2 );
	void ( GLAPIENTRY *glRectiv ) (const GLint *v1, const GLint *v2);
	void ( GLAPIENTRY *glRects ) ( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );
	void ( GLAPIENTRY *glRectsv ) (const GLshort *v1, const GLshort *v2);
	GLint ( GLAPIENTRY *glRenderMode ) ( GLenum mode );
	void ( GLAPIENTRY *glRotated ) ( GLdouble angle, GLdouble x, GLdouble y, GLdouble z );
	void ( GLAPIENTRY *glRotatef ) ( GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
	void ( GLAPIENTRY *glScaled ) ( GLdouble x, GLdouble y, GLdouble z );
	void ( GLAPIENTRY *glScalef ) ( GLfloat x, GLfloat y, GLfloat z );
	void ( GLAPIENTRY *glScissor ) ( GLint x, GLint y, GLsizei width, GLsizei height );
	void ( GLAPIENTRY *glSelectBuffer ) ( GLsizei size, GLuint * buffer );
	void ( GLAPIENTRY *glShadeModel ) ( GLenum mode );
	void ( GLAPIENTRY *glStencilFunc ) ( GLenum func, GLint ref, GLuint mask );
	void ( GLAPIENTRY *glStencilMask ) ( GLuint mask );
	void ( GLAPIENTRY *glStencilOp ) ( GLenum fail, GLenum zfail, GLenum zpass );
	void ( GLAPIENTRY *glTexCoord1d ) ( GLdouble s );
	void ( GLAPIENTRY *glTexCoord1dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glTexCoord1f ) ( GLfloat s );
	void ( GLAPIENTRY *glTexCoord1fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glTexCoord1i ) ( GLint s );
	void ( GLAPIENTRY *glTexCoord1iv ) (const GLint *v);
	void ( GLAPIENTRY *glTexCoord1s ) ( GLshort s );
	void ( GLAPIENTRY *glTexCoord1sv ) (const GLshort *v);
	void ( GLAPIENTRY *glTexCoord2d ) ( GLdouble s, GLdouble t );
	void ( GLAPIENTRY *glTexCoord2dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glTexCoord2f ) ( GLfloat s, GLfloat t );
	void ( GLAPIENTRY *glTexCoord2fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glTexCoord2i ) ( GLint s, GLint t );
	void ( GLAPIENTRY *glTexCoord2iv ) (const GLint *v);
	void ( GLAPIENTRY *glTexCoord2s ) ( GLshort s, GLshort t );
	void ( GLAPIENTRY *glTexCoord2sv ) (const GLshort *v);
	void ( GLAPIENTRY *glTexCoord3d ) ( GLdouble s, GLdouble t, GLdouble r );
	void ( GLAPIENTRY *glTexCoord3dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glTexCoord3f ) ( GLfloat s, GLfloat t, GLfloat r );
	void ( GLAPIENTRY *glTexCoord3fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glTexCoord3i ) ( GLint s, GLint t, GLint r );
	void ( GLAPIENTRY *glTexCoord3iv ) (const GLint *v);
	void ( GLAPIENTRY *glTexCoord3s ) ( GLshort s, GLshort t, GLshort r );
	void ( GLAPIENTRY *glTexCoord3sv ) (const GLshort *v);
	void ( GLAPIENTRY *glTexCoord4d ) ( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
	void ( GLAPIENTRY *glTexCoord4dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glTexCoord4f ) ( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
	void ( GLAPIENTRY *glTexCoord4fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glTexCoord4i ) ( GLint s, GLint t, GLint r, GLint q );
	void ( GLAPIENTRY *glTexCoord4iv ) (const GLint *v);
	void ( GLAPIENTRY *glTexCoord4s ) ( GLshort s, GLshort t, GLshort r, GLshort q );
	void ( GLAPIENTRY *glTexCoord4sv ) (const GLshort *v);
	void ( GLAPIENTRY *glTexCoordPointer ) ( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer );
	void ( GLAPIENTRY *glTexEnvf ) ( GLenum target, GLenum pname, GLfloat param );
	void ( GLAPIENTRY *glTexEnvfv ) ( GLenum target, GLenum pname, const GLfloat * params );
	void ( GLAPIENTRY *glTexEnvi ) ( GLenum target, GLenum pname, GLint param );
	void ( GLAPIENTRY *glTexEnviv ) ( GLenum target, GLenum pname, const GLint * params );
	void ( GLAPIENTRY *glTexGend ) ( GLenum coord, GLenum pname, GLdouble param );
	void ( GLAPIENTRY *glTexGendv ) ( GLenum coord, GLenum pname, const GLdouble * params );
	void ( GLAPIENTRY *glTexGenf ) ( GLenum coord, GLenum pname, GLfloat param );
	void ( GLAPIENTRY *glTexGenfv ) ( GLenum coord, GLenum pname, const GLfloat * params );
	void ( GLAPIENTRY *glTexGeni ) ( GLenum coord, GLenum pname, GLint param );
	void ( GLAPIENTRY *glTexGeniv ) ( GLenum coord, GLenum pname, const GLint * params );
	void ( GLAPIENTRY *glTexImage1D )
		(
			GLenum target,
			GLint level,
			GLint internalformat,
			GLsizei width,
			GLint border,
			GLenum format,
			GLenum type,
			const GLvoid * pixels
		);
	void ( GLAPIENTRY *glTexImage2D )
		(
			GLenum target,
			GLint level,
			GLint internalformat,
			GLsizei width,
			GLsizei height,
			GLint border,
			GLenum format,
			GLenum type,
			const GLvoid * pixels
		);
	void ( GLAPIENTRY *glTexParameterf ) ( GLenum target, GLenum pname, GLfloat param );
	void ( GLAPIENTRY *glTexParameterfv ) ( GLenum target, GLenum pname, const GLfloat * params );
	void ( GLAPIENTRY *glTexParameteri ) ( GLenum target, GLenum pname, GLint param );
	void ( GLAPIENTRY *glTexParameteriv ) ( GLenum target, GLenum pname, const GLint * params );
	void ( GLAPIENTRY *glTexSubImage1D )
		(
			GLenum target,
			GLint level,
			GLint xoffset,
			GLsizei width,
			GLenum format,
			GLenum type,
			const GLvoid * pixels
		);
	void ( GLAPIENTRY *glTexSubImage2D )
		(
			GLenum target,
			GLint level,
			GLint xoffset,
			GLint yoffset,
			GLsizei width,
			GLsizei height,
			GLenum format,
			GLenum type,
			const GLvoid * pixels
		);
	void ( GLAPIENTRY *glTranslated ) ( GLdouble x, GLdouble y, GLdouble z );
	void ( GLAPIENTRY *glTranslatef ) ( GLfloat x, GLfloat y, GLfloat z );
	void ( GLAPIENTRY *glVertex2d ) ( GLdouble x, GLdouble y );
	void ( GLAPIENTRY *glVertex2dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glVertex2f ) ( GLfloat x, GLfloat y );
	void ( GLAPIENTRY *glVertex2fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glVertex2i ) ( GLint x, GLint y );
	void ( GLAPIENTRY *glVertex2iv ) (const GLint *v);
	void ( GLAPIENTRY *glVertex2s ) ( GLshort x, GLshort y );
	void ( GLAPIENTRY *glVertex2sv ) (const GLshort *v);
	void ( GLAPIENTRY *glVertex3d ) ( GLdouble x, GLdouble y, GLdouble z );
	void ( GLAPIENTRY *glVertex3dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glVertex3f ) ( GLfloat x, GLfloat y, GLfloat z );
	void ( GLAPIENTRY *glVertex3fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glVertex3i ) ( GLint x, GLint y, GLint z );
	void ( GLAPIENTRY *glVertex3iv ) (const GLint *v);
	void ( GLAPIENTRY *glVertex3s ) ( GLshort x, GLshort y, GLshort z );
	void ( GLAPIENTRY *glVertex3sv ) (const GLshort *v);
	void ( GLAPIENTRY *glVertex4d ) ( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
	void ( GLAPIENTRY *glVertex4dv ) (const GLdouble *v);
	void ( GLAPIENTRY *glVertex4f ) ( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
	void ( GLAPIENTRY *glVertex4fv ) (const GLfloat *v);
	void ( GLAPIENTRY *glVertex4i ) ( GLint x, GLint y, GLint z, GLint w );
	void ( GLAPIENTRY *glVertex4iv ) (const GLint *v);
	void ( GLAPIENTRY *glVertex4s ) ( GLshort x, GLshort y, GLshort z, GLshort w );
	void ( GLAPIENTRY *glVertex4sv ) (const GLshort *v);
	void ( GLAPIENTRY *glVertexPointer ) ( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer );
	void ( GLAPIENTRY *glViewport ) ( GLint x, GLint y, GLsizei width, GLsizei height );
#if defined WIN32 || defined WIN64
	int ( GLAPIENTRY *wglChoosePixelFormat ) ( HDC a, CONST PIXELFORMATDESCRIPTOR * b );
	BOOL ( GLAPIENTRY *wglCopyContext ) ( HGLRC a, HGLRC b, UINT c );
	HGLRC ( GLAPIENTRY *wglCreateContext ) ( HDC a );
	HGLRC ( GLAPIENTRY *wglCreateLayerContext ) ( HDC a, int b );
	BOOL ( GLAPIENTRY *wglDeleteContext ) ( HGLRC a );
	BOOL ( GLAPIENTRY *wglDescribeLayerPlane ) ( HDC a, int b, int c, UINT d, LPLAYERPLANEDESCRIPTOR e );
	int ( GLAPIENTRY *wglDescribePixelFormat ) ( HDC a, int b, UINT c, LPPIXELFORMATDESCRIPTOR d );
	HGLRC ( GLAPIENTRY *wglGetCurrentContext ) (void);
	HDC ( GLAPIENTRY *wglGetCurrentDC ) (void);
	PROC ( GLAPIENTRY *wglGetDefaultProcAddress ) ( LPCSTR a );
	int ( GLAPIENTRY *wglGetLayerPaletteEntries ) ( HDC a, int b, int c, int d, COLORREF * e );
	int ( GLAPIENTRY *wglGetPixelFormat ) ( HDC a );
	PROC ( GLAPIENTRY *wglGetProcAddress ) ( LPCSTR a );
	BOOL ( GLAPIENTRY *wglMakeCurrent ) ( HDC a, HGLRC b );
	BOOL ( GLAPIENTRY *wglRealizeLayerPalette ) ( HDC a, int b, BOOL c );
	int ( GLAPIENTRY *wglSetLayerPaletteEntries ) ( HDC a, int b, int c, int d, CONST COLORREF * e );
	BOOL ( GLAPIENTRY *wglSetPixelFormat ) ( HDC a, int b, CONST PIXELFORMATDESCRIPTOR * c );
	BOOL ( GLAPIENTRY *wglShareLists ) ( HGLRC a, HGLRC b );
	BOOL ( GLAPIENTRY *wglSwapBuffers ) ( HDC a );
	BOOL ( GLAPIENTRY *wglSwapLayerBuffers ) ( HDC a, UINT b );
	BOOL ( GLAPIENTRY *wglUseFontBitmapsA ) ( HDC a, DWORD b, DWORD c, DWORD d );
	BOOL ( GLAPIENTRY *wglUseFontBitmapsW ) ( HDC a, DWORD b, DWORD c, DWORD d );
	BOOL ( GLAPIENTRY *wglUseFontOutlinesA )
		(
			HDC a,
			DWORD b,
			DWORD c,
			DWORD d,
			FLOAT e,
			FLOAT f,
			int g,
			LPGLYPHMETRICSFLOAT h
		);
	BOOL ( GLAPIENTRY *wglUseFontOutlinesW )
		(
			HDC a,
			DWORD b,
			DWORD c,
			DWORD d,
			FLOAT e,
			FLOAT f,
			int g,
			LPGLYPHMETRICSFLOAT h
		);
#else
#endif
}
GLV;

/* GLenum values from Win32 GL.H */
typedef struct
{
	char	*name;
	int		tag;
} GL_ENUM_PAIR;
extern GL_ENUM_PAIR GLenum_list[];
extern GL_ENUM_PAIR GLbitfield_list[];

/* Misc. globals */
extern GLboolean	useZeroOne;		/* To correct GL_ZERO and GL_ONE overlap with other enumerants */
extern GLboolean	bVerbose;		/* Controls the amount of info */
extern GLboolean	bCountCalls;	/* count the number of function calls */
extern GLboolean	bLogTime;		/* log the time of each function call */
void				print ( const char *fmt, ... );
void				print_value ( TYPE type, const void *value );
void				InitCallCount ( void );
void				PrintCallCounts ( void );
void				write_time ( void );

/* this is the format for printing the call counts */

/* to change it, see log.c */
const char			*fmt;
extern int			show_status[];
extern int			show_status_EXT[];
