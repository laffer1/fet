enum GLID_EXT
{
	FP_glPointParameterfEXT			= 0,
	FP_glPointParameterfvEXT,
	FP_glMultiTexCoord1dARB,
	FP_glMultiTexCoord1dvARB,
	FP_glMultiTexCoord1fARB,
	FP_glMultiTexCoord1fvARB,
	FP_glMultiTexCoord1iARB,
	FP_glMultiTexCoord1ivARB,
	FP_glMultiTexCoord1sARB,
	FP_glMultiTexCoord1svARB,
	FP_glMultiTexCoord2dARB,
	FP_glMultiTexCoord2dvARB,
	FP_glMultiTexCoord2fARB,
	FP_glMultiTexCoord2fvARB,
	FP_glMultiTexCoord2iARB,
	FP_glMultiTexCoord2ivARB,
	FP_glMultiTexCoord2sARB,
	FP_glMultiTexCoord2svARB,
	FP_glMultiTexCoord3dARB,
	FP_glMultiTexCoord3dvARB,
	FP_glMultiTexCoord3fARB,
	FP_glMultiTexCoord3fvARB,
	FP_glMultiTexCoord3iARB,
	FP_glMultiTexCoord3ivARB,
	FP_glMultiTexCoord3sARB,
	FP_glMultiTexCoord3svARB,
	FP_glMultiTexCoord4dARB,
	FP_glMultiTexCoord4dvARB,
	FP_glMultiTexCoord4fARB,
	FP_glMultiTexCoord4fvARB,
	FP_glMultiTexCoord4iARB,
	FP_glMultiTexCoord4ivARB,
	FP_glMultiTexCoord4sARB,
	FP_glMultiTexCoord4svARB,
	FP_glActiveTextureARB,
	FP_glClientActiveTextureARB,
	FP_glLockArraysEXT,
	FP_glUnlockArraysEXT,
	FP_glCullParameterdvEXT,
	FP_glCullParameterfvEXT,
	FP_glAddSwapHintRectWIN,
	FP_glVertexWeightfEXT,
	FP_glVertexWeightfvEXT,
	FP_glVertexWeightPointerEXT,
	FP_wglSwapIntervalEXT,
	FP_wglGetSwapIntervalEXT,
	FP_wglGetExtensionsStringARB,
	FP_glLoadTransposeMatrixfARB,
	FP_glLoadTransposeMatrixdARB,
	FP_glMultTransposeMatrixfARB,
	FP_glMultTransposeMatrixdARB,
	FP_wglCreateBufferRegionARB,
	FP_wglDeleteBufferRegionARB,
	FP_wglSaveBufferRegionARB,
	FP_wglRestoreBufferRegionARB,
	FP_glSampleCoverageARB,
	FP_wglGetPixelFormatAttribivARB,
	FP_wglGetPixelFormatAttribfvARB,
	FP_wglChoosePixelFormatARB,
	FP_wglMakeContextCurrentARB,
	FP_wglGetCurrentReadDCARB,
	FP_EXT_LAST
};
unsigned int	nCallCountEXT[FP_EXT_LAST];
typedef struct
{

	/* EXT_point_parameters */
	void ( GLAPIENTRY *glPointParameterfEXT ) ( GLenum pname, GLfloat param );
	void ( GLAPIENTRY *glPointParameterfvEXT ) ( GLenum pname, const GLfloat * params );

	/* ARB_multitexture */
	void ( GLAPIENTRY *glMultiTexCoord1dARB ) ( GLenum target, GLdouble s );
	void ( GLAPIENTRY *glMultiTexCoord1dvARB ) ( GLenum target, const GLdouble * v );
	void ( GLAPIENTRY *glMultiTexCoord1fARB ) ( GLenum target, GLfloat s );
	void ( GLAPIENTRY *glMultiTexCoord1fvARB ) ( GLenum target, const GLfloat * v );
	void ( GLAPIENTRY *glMultiTexCoord1iARB ) ( GLenum target, GLint s );
	void ( GLAPIENTRY *glMultiTexCoord1ivARB ) ( GLenum target, const GLint * v );
	void ( GLAPIENTRY *glMultiTexCoord1sARB ) ( GLenum target, GLshort s );
	void ( GLAPIENTRY *glMultiTexCoord1svARB ) ( GLenum target, const GLshort * v );
	void ( GLAPIENTRY *glMultiTexCoord2dARB ) ( GLenum target, GLdouble s, GLdouble t );
	void ( GLAPIENTRY *glMultiTexCoord2dvARB ) ( GLenum target, const GLdouble * v );
	void ( GLAPIENTRY *glMultiTexCoord2fARB ) ( GLenum target, GLfloat s, GLfloat t );
	void ( GLAPIENTRY *glMultiTexCoord2fvARB ) ( GLenum target, const GLfloat * v );
	void ( GLAPIENTRY *glMultiTexCoord2iARB ) ( GLenum target, GLint s, GLint t );
	void ( GLAPIENTRY *glMultiTexCoord2ivARB ) ( GLenum target, const GLint * v );
	void ( GLAPIENTRY *glMultiTexCoord2sARB ) ( GLenum target, GLshort s, GLshort t );
	void ( GLAPIENTRY *glMultiTexCoord2svARB ) ( GLenum target, const GLshort * v );
	void ( GLAPIENTRY *glMultiTexCoord3dARB ) ( GLenum target, GLdouble s, GLdouble t, GLdouble r );
	void ( GLAPIENTRY *glMultiTexCoord3dvARB ) ( GLenum target, const GLdouble * v );
	void ( GLAPIENTRY *glMultiTexCoord3fARB ) ( GLenum target, GLfloat s, GLfloat t, GLfloat r );
	void ( GLAPIENTRY *glMultiTexCoord3fvARB ) ( GLenum target, const GLfloat * v );
	void ( GLAPIENTRY *glMultiTexCoord3iARB ) ( GLenum target, GLint s, GLint t, GLint r );
	void ( GLAPIENTRY *glMultiTexCoord3ivARB ) ( GLenum target, const GLint * v );
	void ( GLAPIENTRY *glMultiTexCoord3sARB ) ( GLenum target, GLshort s, GLshort t, GLshort r );
	void ( GLAPIENTRY *glMultiTexCoord3svARB ) ( GLenum target, const GLshort * v );
	void ( GLAPIENTRY *glMultiTexCoord4dARB ) ( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q );
	void ( GLAPIENTRY *glMultiTexCoord4dvARB ) ( GLenum target, const GLdouble * v );
	void ( GLAPIENTRY *glMultiTexCoord4fARB ) ( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q );
	void ( GLAPIENTRY *glMultiTexCoord4fvARB ) ( GLenum target, const GLfloat * v );
	void ( GLAPIENTRY *glMultiTexCoord4iARB ) ( GLenum target, GLint s, GLint t, GLint r, GLint q );
	void ( GLAPIENTRY *glMultiTexCoord4ivARB ) ( GLenum target, const GLint * v );
	void ( GLAPIENTRY *glMultiTexCoord4sARB ) ( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q );
	void ( GLAPIENTRY *glMultiTexCoord4svARB ) ( GLenum target, const GLshort * v );
	void ( GLAPIENTRY *glActiveTextureARB ) ( GLenum target );
	void ( GLAPIENTRY *glClientActiveTextureARB ) ( GLenum target );

	/* EXT_compiled_vertex_array */
	void ( GLAPIENTRY *glLockArraysEXT ) ( GLint first, GLsizei count );
	void ( GLAPIENTRY *glUnlockArraysEXT ) (void);

	/* EXT_cull_vertex */
	void ( GLAPIENTRY *glCullParameterdvEXT ) ( GLenum pname, GLdouble * params );
	void ( GLAPIENTRY *glCullParameterfvEXT ) ( GLenum pname, GLfloat * params );

	/* WIN_swap_hint */
	void ( GLAPIENTRY *glAddSwapHintRectWIN ) ( GLint x, GLint y, GLsizei width, GLsizei height );

	/* EXT_vertex_weighting */
	void ( GLAPIENTRY *glVertexWeightfEXT ) ( GLfloat weight );
	void ( GLAPIENTRY *glVertexWeightfvEXT ) ( GLfloat * weight );
	void ( GLAPIENTRY *glVertexWeightPointerEXT ) ( GLint size, GLenum type, GLsizei stride, GLvoid * pointer );

	/* EXT_swap_control */
	BOOL ( GLAPIENTRY *wglSwapIntervalEXT ) (int interval);
	int ( GLAPIENTRY *wglGetSwapIntervalEXT ) (void);

	/* WGL_ARB_extensions_string */
	const char * ( GLAPIENTRY * wglGetExtensionsStringARB ) ( HDC hdc );

	/* ARB_transpose_matrix */
	void ( GLAPIENTRY *glLoadTransposeMatrixfARB ) (const GLfloat *m);
	void ( GLAPIENTRY *glLoadTransposeMatrixdARB ) (const GLdouble *m);
	void ( GLAPIENTRY *glMultTransposeMatrixfARB ) (const GLfloat *m);
	void ( GLAPIENTRY *glMultTransposeMatrixdARB ) (const GLdouble *m);

	/* WGL_ARB_buffer_region */
	HANDLE ( GLAPIENTRY *wglCreateBufferRegionARB ) ( HDC hDC, int iLayerPlane, UINT uType );
	void ( GLAPIENTRY *wglDeleteBufferRegionARB ) ( HANDLE hRegion );
	BOOL ( GLAPIENTRY *wglSaveBufferRegionARB ) ( HANDLE hRegion, int x, int y, int width, int height );
	BOOL ( GLAPIENTRY *wglRestoreBufferRegionARB )
		(
			HANDLE hRegion,
			int x,
			int y,
			int width,
			int height,
			int xSrc,
			int ySrc
		);

	/* ARB_multisample */
	void ( GLAPIENTRY *glSampleCoverageARB ) ( GLclampf value, GLboolean invert );

	/* WGL_ARB_pixel_format */
	BOOL ( GLAPIENTRY *wglGetPixelFormatAttribivARB )
		(
			HDC hdc,
			int iPixelFormat,
			int iLayerPlane,
			UINT nAttributes,
			const int *piAttributes,
			int *piValues
		);
	BOOL ( GLAPIENTRY *wglGetPixelFormatAttribfvARB )
		(
			HDC hdc,
			int iPixelFormat,
			int iLayerPlane,
			UINT nAttributes,
			const int *piAttributes,
			FLOAT * pfValues
		);
	BOOL ( GLAPIENTRY *wglChoosePixelFormatARB )
		(
			HDC hdc,
			const int *piAttribIList,
			const FLOAT * pfAttribFList,
			UINT nMaxFormats,
			int *piFormats,
			UINT * nNumFormats
		);

	/* WGL_ARB_make_current_read */
	BOOL ( GLAPIENTRY *wglMakeContextCurrentARB ) ( HDC hDrawDC, HDC hReadDC, HGLRC hglrc );
	HDC ( GLAPIENTRY *wglGetCurrentReadDCARB ) (void);
}
glv_ext_t;

/*/ EXT_point_parameter */
void GLAPIENTRY glPointParameterfEXT ( GLenum pname, GLfloat param );
void GLAPIENTRY glPointParameterfvEXT ( GLenum pname, const GLfloat *params );

/* GL_ARB_multitexture */
void GLAPIENTRY glMultiTexCoord1dARB ( GLenum target, GLdouble s );
void GLAPIENTRY glMultiTexCoord1dvARB ( GLenum target, const GLdouble *v );
void GLAPIENTRY glMultiTexCoord1fARB ( GLenum target, GLfloat s );
void GLAPIENTRY glMultiTexCoord1fvARB ( GLenum target, const GLfloat *v );
void GLAPIENTRY glMultiTexCoord1iARB ( GLenum target, GLint s );
void GLAPIENTRY glMultiTexCoord1ivARB ( GLenum target, const GLint *v );
void GLAPIENTRY glMultiTexCoord1sARB ( GLenum target, GLshort s );
void GLAPIENTRY glMultiTexCoord1svARB ( GLenum target, const GLshort *v );
void GLAPIENTRY glMultiTexCoord2dARB ( GLenum target, GLdouble s, GLdouble t );
void GLAPIENTRY glMultiTexCoord2dvARB ( GLenum target, const GLdouble *v );
void GLAPIENTRY glMultiTexCoord2fARB ( GLenum target, GLfloat s, GLfloat t );
void GLAPIENTRY glMultiTexCoord2fvARB ( GLenum target, const GLfloat *v );
void GLAPIENTRY glMultiTexCoord2iARB ( GLenum target, GLint s, GLint t );
void GLAPIENTRY glMultiTexCoord2ivARB ( GLenum target, const GLint *v );
void GLAPIENTRY glMultiTexCoord2sARB ( GLenum target, GLshort s, GLshort t );
void GLAPIENTRY glMultiTexCoord2svARB ( GLenum target, const GLshort *v );
void GLAPIENTRY glMultiTexCoord3dARB ( GLenum target, GLdouble s, GLdouble t, GLdouble r );
void GLAPIENTRY glMultiTexCoord3dvARB ( GLenum target, const GLdouble *v );
void GLAPIENTRY glMultiTexCoord3fARB ( GLenum target, GLfloat s, GLfloat t, GLfloat r );
void GLAPIENTRY glMultiTexCoord3fvARB ( GLenum target, const GLfloat *v );
void GLAPIENTRY glMultiTexCoord3iARB ( GLenum target, GLint s, GLint t, GLint r );
void GLAPIENTRY glMultiTexCoord3ivARB ( GLenum target, const GLint *v );
void GLAPIENTRY glMultiTexCoord3sARB ( GLenum target, GLshort s, GLshort t, GLshort r );
void GLAPIENTRY glMultiTexCoord3svARB ( GLenum target, const GLshort *v );
void GLAPIENTRY glMultiTexCoord4dARB ( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q );
void GLAPIENTRY glMultiTexCoord4dvARB ( GLenum target, const GLdouble *v );
void GLAPIENTRY glMultiTexCoord4fARB ( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q );
void GLAPIENTRY glMultiTexCoord4fvARB ( GLenum target, const GLfloat *v );
void GLAPIENTRY glMultiTexCoord4iARB ( GLenum target, GLint s, GLint t, GLint r, GLint q );
void GLAPIENTRY glMultiTexCoord4ivARB ( GLenum target, const GLint *v );
void GLAPIENTRY glMultiTexCoord4sARB ( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q );
void GLAPIENTRY glMultiTexCoord4svARB ( GLenum target, const GLshort *v );
void GLAPIENTRY glActiveTextureARB ( GLenum target );
void GLAPIENTRY glClientActiveTextureARB ( GLenum target );

/* GL_EXT_compiled_vertex_array */
void GLAPIENTRY glLockArraysEXT ( GLint first, GLsizei count );
void GLAPIENTRY glUnlockArraysEXT ( void );

/* EXT_cull_vertex */
void GLAPIENTRY glCullParameterdvEXT ( GLenum pname, GLdouble *params );
void GLAPIENTRY glCullParameterfvEXT ( GLenum pname, GLfloat *params );

/* WIN_swap_hint */
void GLAPIENTRY glAddSwapHintRectWIN ( GLint x, GLint y, GLsizei width, GLsizei height );

/* EXT_vertex_weighting */
void GLAPIENTRY glVertexWeightfEXT ( GLfloat weight );
void GLAPIENTRY glVertexWeightfvEXT ( GLfloat *weight );
void GLAPIENTRY glVertexWeightPointerEXT ( GLint size, GLenum type, GLsizei stride, GLvoid *pointer );

/* EXT_swap_control */
BOOL GLAPIENTRY wglSwapIntervalEXT ( int interval );
int GLAPIENTRY	wglGetSwapIntervalEXT ( void );

/* WGL_ARB_extensions_string */
const char *GLAPIENTRY wglGetExtensionsStringARB( HDC hdc );

/* ARB_transpose_matrix */
void GLAPIENTRY		glLoadTransposeMatrixfARB ( const GLfloat *m );
void GLAPIENTRY		glLoadTransposeMatrixdARB ( const GLdouble *m );
void GLAPIENTRY		glMultTransposeMatrixfARB ( const GLfloat *m );
void GLAPIENTRY		glMultTransposeMatrixdARB ( const GLdouble *m );

/* WGL_ARB_buffer_region */
HANDLE GLAPIENTRY	wglCreateBufferRegionARB ( HDC hDC, int iLayerPlane, UINT uType );
void GLAPIENTRY		wglDeleteBufferRegionARB ( HANDLE hRegion );
BOOL GLAPIENTRY		wglSaveBufferRegionARB ( HANDLE hRegion, int x, int y, int width, int height );
BOOL GLAPIENTRY		wglRestoreBufferRegionARB
					(
						HANDLE	hRegion,
						int		x,
						int		y,
						int		width,
						int		height,
						int		xSrc,
						int		ySrc
					);

/* ARB_multisample */
void GLAPIENTRY		glSampleCoverageARB ( GLclampf value, GLboolean invert );

/* WGL_ARB_pixel_format */
BOOL GLAPIENTRY		wglGetPixelFormatAttribivARB
					(
						HDC			hdc,
						int			iPixelFormat,
						int			iLayerPlane,
						UINT		nAttributes,
						const int	*piAttributes,
						int			*piValues
					);
BOOL GLAPIENTRY		wglGetPixelFormatAttribfvARB
					(
						HDC			hdc,
						int			iPixelFormat,
						int			iLayerPlane,
						UINT		nAttributes,
						const int	*piAttributes,
						FLOAT		*pfValues
					);
BOOL GLAPIENTRY		wglChoosePixelFormatARB
					(
						HDC			hdc,
						const int	*piAttribIList,
						const FLOAT *pfAttribFList,
						UINT		nMaxFormats,
						int			*piFormats,
						UINT		*nNumFormats
					);

/* WGL_ARB_make_current_read */
BOOL GLAPIENTRY		wglMakeContextCurrentARB ( HDC hDrawDC, HDC hReadDC, HGLRC hglrc );
HDC GLAPIENTRY		wglGetCurrentReadDCARB ( void );
glv_ext_t			GLV_EXT;
glv_ext_t			GLV_EXT_RETURN =
{
	glPointParameterfEXT,
	glPointParameterfvEXT,
	glMultiTexCoord1dARB,
	glMultiTexCoord1dvARB,
	glMultiTexCoord1fARB,
	glMultiTexCoord1fvARB,
	glMultiTexCoord1iARB,
	glMultiTexCoord1ivARB,
	glMultiTexCoord1sARB,
	glMultiTexCoord1svARB,
	glMultiTexCoord2dARB,
	glMultiTexCoord2dvARB,
	glMultiTexCoord2fARB,
	glMultiTexCoord2fvARB,
	glMultiTexCoord2iARB,
	glMultiTexCoord2ivARB,
	glMultiTexCoord2sARB,
	glMultiTexCoord2svARB,
	glMultiTexCoord3dARB,
	glMultiTexCoord3dvARB,
	glMultiTexCoord3fARB,
	glMultiTexCoord3fvARB,
	glMultiTexCoord3iARB,
	glMultiTexCoord3ivARB,
	glMultiTexCoord3sARB,
	glMultiTexCoord3svARB,
	glMultiTexCoord4dARB,
	glMultiTexCoord4dvARB,
	glMultiTexCoord4fARB,
	glMultiTexCoord4fvARB,
	glMultiTexCoord4iARB,
	glMultiTexCoord4ivARB,
	glMultiTexCoord4sARB,
	glMultiTexCoord4svARB,
	glActiveTextureARB,
	glClientActiveTextureARB,
	glLockArraysEXT,
	glUnlockArraysEXT,
	glCullParameterdvEXT,
	glCullParameterfvEXT,
	glAddSwapHintRectWIN,
	glVertexWeightfEXT,
	glVertexWeightfvEXT,
	glVertexWeightPointerEXT,
	wglSwapIntervalEXT,
	wglGetSwapIntervalEXT,
	wglGetExtensionsStringARB,
	glLoadTransposeMatrixfARB,
	glLoadTransposeMatrixdARB,
	glMultTransposeMatrixfARB,
	glMultTransposeMatrixdARB,
	wglCreateBufferRegionARB,
	wglDeleteBufferRegionARB,
	wglSaveBufferRegionARB,
	wglRestoreBufferRegionARB,
	glSampleCoverageARB,
	wglGetPixelFormatAttribivARB,
	wglGetPixelFormatAttribfvARB,
	wglChoosePixelFormatARB,
	wglMakeContextCurrentARB,
	wglGetCurrentReadDCARB
};
char				*GLN_EXT[] =
{
	"glPointParameterfEXT",
	"glPointParameterfvEXT",
	"glMultiTexCoord1dARB",
	"glMultiTexCoord1dvARB",
	"glMultiTexCoord1fARB",
	"glMultiTexCoord1fvARB",
	"glMultiTexCoord1iARB",
	"glMultiTexCoord1ivARB",
	"glMultiTexCoord1sARB",
	"glMultiTexCoord1svARB",
	"glMultiTexCoord2dARB",
	"glMultiTexCoord2dvARB",
	"glMultiTexCoord2fARB",
	"glMultiTexCoord2fvARB",
	"glMultiTexCoord2iARB",
	"glMultiTexCoord2ivARB",
	"glMultiTexCoord2sARB",
	"glMultiTexCoord2svARB",
	"glMultiTexCoord3dARB",
	"glMultiTexCoord3dvARB",
	"glMultiTexCoord3fARB",
	"glMultiTexCoord3fvARB",
	"glMultiTexCoord3iARB",
	"glMultiTexCoord3ivARB",
	"glMultiTexCoord3sARB",
	"glMultiTexCoord3svARB",
	"glMultiTexCoord4dARB",
	"glMultiTexCoord4dvARB",
	"glMultiTexCoord4fARB",
	"glMultiTexCoord4fvARB",
	"glMultiTexCoord4iARB",
	"glMultiTexCoord4ivARB",
	"glMultiTexCoord4sARB",
	"glMultiTexCoord4svARB",
	"glActiveTextureARB",
	"glClientActiveTextureARB",
	"glLockArraysEXT",
	"glUnlockArraysEXT",
	"glCullParameterdvEXT",
	"glCullParameterfvEXT",
	"glAddSwapHintRectWIN",
	"glVertexWeightfEXT",
	"glVertexWeightfvEXT",
	"glVertexWeightPointerEXT",
	"wglSwapIntervalEXT",
	"wglGetSwapIntervalEXT",
	"wglGetExtensionsStringARB",
	"glLoadTransposeMatrixfARB",
	"glLoadTransposeMatrixdARB",
	"glMultTransposeMatrixfARB",
	"glMultTransposeMatrixdARB",
	"wglCreateBufferRegionARB",
	"wglDeleteBufferRegionARB",
	"wglSaveBufferRegionARB",
	"wglRestoreBufferRegionARB",
	"glSampleCoverageARB",
	"wglGetPixelFormatAttribivARB",
	"wglGetPixelFormatAttribfvARB",
	"wglChoosePixelFormatARB",
	"wglMakeContextCurrentARB",
	"wglGetCurrentReadDCARB"
};
int					show_status_EXT[
ARY_CNT( GLN_EXT )
];

/* defines for extensions */
#define GL_DISTANCE_ATTENUATION_EXT 0x8129
#define START_EXT( X )																			\
	if ( bCountCalls == GL_TRUE )																\
	{																							\
		nCallCountEXT[FP_##X]++;																\
	}																							\
	if ( bLogCalls == GL_TRUE )																	\
	{																							\
		if																						\
		(																						\
			((show_status_EXT[FP_##X] == SHOW_ALWAYS) && (bOnOffKeydown == GL_TRUE)) ||			\
			((show_status_EXT[FP_##X] == SHOW_IF_SHIFT_DOWN) && (bOptionKeydown == GL_TRUE)) || \
			(bOneframeKeydown == GL_TRUE)														\
		)																						\
		{																						\
			if ( bLogTime == GL_TRUE )															\
			{																					\
				write_time();																	\
			} print( GLN_EXT[FP_##X] );															\
			print( "(" );																		\
			if ( bVerbose == GL_TRUE )															\
			{																					\
																					 \
																					 \
			/* internal functions */
			void
			extInitCallCount ( void )
			{
				int i;
				for ( i = 0; i < FP_EXT_LAST; i++ )
				{
					nCallCountEXT[i] = 0;
				}

				for ( i = 0; i < ARY_CNT( show_status_EXT); i++ )
				{
					show_status_EXT[i] = SHOW_ALWAYS;
				}
			}


			/* */
			void
			extPrintCallCounts ( void )
			{
				int i;
				for ( i = 0; i < FP_EXT_LAST; i++ )
				{
					if ( nCallCountEXT[i] > 0 )
					{
						print( fmt, GLN_EXT[i], nCallCountEXT[i] );
					}
				}
			}


			/* */
			int
			GetEXTProcIndex ( LPCSTR name )
			{
				int i;
				for ( i = 0; i < ARY_CNT( GLN_EXT); i++ )
				{
					if ( strcmp( GLN_EXT[i], name) == 0 )
					{
						return ( i );
					}
				}

				return ( -1 );
			}


			/* GL_EXT_point_parameters */
			void GLAPIENTRY
			glPointParameterfEXT ( GLenum pname, GLfloat param )
			{
				START_EXT( glPointParameterfEXT );
				print_value( _GLenum, &pname );
				print( "," );
				print_value( _GLfloat, &param );
				END;
				GLV_EXT.glPointParameterfEXT( pname, param );
			}


			/* */
			void GLAPIENTRY
			glPointParameterfvEXT ( GLenum pname, const GLfloat *params )
			{
				START_EXT( glPointParameterfEXT );
				print_value( _GLenum, &pname );
				print( "," );
				switch ( pname )
				{
					case GL_DISTANCE_ATTENUATION_EXT:
						print_value( _GLfloat3, &params );
						break;
					default:
						print_value( _GLfloat1, &params );
						break;
				}

				END;
				GLV_EXT.glPointParameterfvEXT( pname, params );
			}


			/* GL_ARB_multitexture */
			void GLAPIENTRY
			glMultiTexCoord1dARB ( GLenum target, GLdouble s )
			{
				START_EXT( glMultiTexCoord1dARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLdouble, &s );
				END;
				GLV_EXT.glMultiTexCoord1dARB( target, s );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord1dvARB ( GLenum target, const GLdouble *v )
			{
				START_EXT( glMultiTexCoord1dvARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLdouble1, &v );
				END;
				GLV_EXT.glMultiTexCoord1dvARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord1fARB ( GLenum target, GLfloat s )
			{
				START_EXT( glMultiTexCoord1fARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLfloat, &s );
				END;
				GLV_EXT.glMultiTexCoord1fARB( target, s );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord1fvARB ( GLenum target, const GLfloat *v )
			{
				START_EXT( glMultiTexCoord1fvARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLfloat1, &v );
				END;
				GLV_EXT.glMultiTexCoord1fvARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord1iARB ( GLenum target, GLint s )
			{
				START_EXT( glMultiTexCoord1iARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLint, &s );
				END;
				GLV_EXT.glMultiTexCoord1iARB( target, s );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord1ivARB ( GLenum target, const GLint *v )
			{
				START_EXT( glMultiTexCoord1ivARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLint1, &v );
				END;
				GLV_EXT.glMultiTexCoord1ivARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord1sARB ( GLenum target, GLshort s )
			{
				START_EXT( glMultiTexCoord1sARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLshort, &s );
				END;
				GLV_EXT.glMultiTexCoord1sARB( target, s );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord1svARB ( GLenum target, const GLshort *v )
			{
				START_EXT( glMultiTexCoord1svARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLshort1, &v );
				END;
				GLV_EXT.glMultiTexCoord1svARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord2dARB ( GLenum target, GLdouble s, GLdouble t )
			{
				START_EXT( glMultiTexCoord2dARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLdouble, &s );
				print( "," );
				print_value( _GLdouble, &t );
				END;
				GLV_EXT.glMultiTexCoord2dARB( target, s, t );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord2dvARB ( GLenum target, const GLdouble *v )
			{
				START_EXT( glMultiTexCoord2dvARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLdouble2, &v );
				END;
				GLV_EXT.glMultiTexCoord2dvARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord2fARB ( GLenum target, GLfloat s, GLfloat t )
			{
				START_EXT( glMultiTexCoord2fARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLfloat, &s );
				print( "," );
				print_value( _GLfloat, &t );
				END;
				GLV_EXT.glMultiTexCoord2fARB( target, s, t );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord2fvARB ( GLenum target, const GLfloat *v )
			{
				START_EXT( glMultiTexCoord2fvARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLfloat2, &v );
				END;
				GLV_EXT.glMultiTexCoord2fvARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord2iARB ( GLenum target, GLint s, GLint t )
			{
				START_EXT( glMultiTexCoord2iARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLint, &s );
				print( "," );
				print_value( _GLint, &t );
				END;
				GLV_EXT.glMultiTexCoord2iARB( target, s, t );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord2ivARB ( GLenum target, const GLint *v )
			{
				START_EXT( glMultiTexCoord2ivARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLint2, &v );
				END;
				GLV_EXT.glMultiTexCoord2ivARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord2sARB ( GLenum target, GLshort s, GLshort t )
			{
				START_EXT( glMultiTexCoord2sARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLshort, &s );
				print( "," );
				print_value( _GLshort, &t );
				END;
				GLV_EXT.glMultiTexCoord2sARB( target, s, t );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord2svARB ( GLenum target, const GLshort *v )
			{
				START_EXT( glMultiTexCoord2svARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLshort2, &v );
				END;
				GLV_EXT.glMultiTexCoord2svARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord3dARB ( GLenum target, GLdouble s, GLdouble t, GLdouble r )
			{
				START_EXT( glMultiTexCoord3dARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLdouble, &s );
				print( "," );
				print_value( _GLdouble, &t );
				print( "," );
				print_value( _GLdouble, &r );
				END;
				GLV_EXT.glMultiTexCoord3dARB( target, s, t, r );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord3dvARB ( GLenum target, const GLdouble *v )
			{
				START_EXT( glMultiTexCoord3dvARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLdouble3, &v );
				END;
				GLV_EXT.glMultiTexCoord3dvARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord3fARB ( GLenum target, GLfloat s, GLfloat t, GLfloat r )
			{
				START_EXT( glMultiTexCoord3fARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLfloat, &s );
				print( "," );
				print_value( _GLfloat, &t );
				print( "," );
				print_value( _GLfloat, &r );
				END;
				GLV_EXT.glMultiTexCoord3fARB( target, s, t, r );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord3fvARB ( GLenum target, const GLfloat *v )
			{
				START_EXT( glMultiTexCoord3fvARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLfloat3, &v );
				END;
				GLV_EXT.glMultiTexCoord3fvARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord3iARB ( GLenum target, GLint s, GLint t, GLint r )
			{
				START_EXT( glMultiTexCoord3iARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLint, &s );
				print( "," );
				print_value( _GLint, &t );
				print( "," );
				print_value( _GLint, &r );
				END;
				GLV_EXT.glMultiTexCoord3iARB( target, s, t, r );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord3ivARB ( GLenum target, const GLint *v )
			{
				START_EXT( glMultiTexCoord3ivARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLint3, &v );
				END;
				GLV_EXT.glMultiTexCoord3ivARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord3sARB ( GLenum target, GLshort s, GLshort t, GLshort r )
			{
				START_EXT( glMultiTexCoord3sARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLshort, &s );
				print( "," );
				print_value( _GLshort, &t );
				print( "," );
				print_value( _GLshort, &r );
				END;
				GLV_EXT.glMultiTexCoord3sARB( target, s, t, r );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord3svARB ( GLenum target, const GLshort *v )
			{
				START_EXT( glMultiTexCoord3svARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLshort3, &v );
				END;
				GLV_EXT.glMultiTexCoord3svARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord4dARB ( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q )
			{
				START_EXT( glMultiTexCoord4dARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLdouble, &s );
				print( "," );
				print_value( _GLdouble, &t );
				print( "," );
				print_value( _GLdouble, &r );
				print( "," );
				print_value( _GLdouble, &q );
				END;
				GLV_EXT.glMultiTexCoord4dARB( target, s, t, r, q );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord4dvARB ( GLenum target, const GLdouble *v )
			{
				START_EXT( glMultiTexCoord4dvARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLdouble, &v );
				END;
				GLV_EXT.glMultiTexCoord4dvARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord4fARB ( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q )
			{
				START_EXT( glMultiTexCoord4fARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLfloat, &s );
				print( "," );
				print_value( _GLfloat, &t );
				print( "," );
				print_value( _GLfloat, &r );
				print( "," );
				print_value( _GLfloat, &q );
				END;
				GLV_EXT.glMultiTexCoord4fARB( target, s, t, r, q );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord4fvARB ( GLenum target, const GLfloat *v )
			{
				START_EXT( glMultiTexCoord4fvARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLfloat4, &v );
				END;
				GLV_EXT.glMultiTexCoord4fvARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord4iARB ( GLenum target, GLint s, GLint t, GLint r, GLint q )
			{
				START_EXT( glMultiTexCoord4iARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLint, &s );
				print( "," );
				print_value( _GLint, &t );
				print( "," );
				print_value( _GLint, &r );
				print( "," );
				print_value( _GLint, &q );
				END;
				GLV_EXT.glMultiTexCoord4iARB( target, s, t, r, q );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord4ivARB ( GLenum target, const GLint *v )
			{
				START_EXT( glMultiTexCoord4ivARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLint, &v );
				END;
				GLV_EXT.glMultiTexCoord4ivARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord4sARB ( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q )
			{
				START_EXT( glMultiTexCoord4sARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLshort, &s );
				print( "," );
				print_value( _GLshort, &t );
				print( "," );
				print_value( _GLshort, &r );
				print( "," );
				print_value( _GLshort, &q );
				END;
				GLV_EXT.glMultiTexCoord4sARB( target, s, t, r, q );
			}


			/* */
			void GLAPIENTRY
			glMultiTexCoord4svARB ( GLenum target, const GLshort *v )
			{
				START_EXT( glMultiTexCoord4svARB );
				print_value( _GLenum, &target );
				print( "," );
				print_value( _GLshort, &v );
				END;
				GLV_EXT.glMultiTexCoord4svARB( target, v );
			}


			/* */
			void GLAPIENTRY
			glActiveTextureARB ( GLenum target )
			{
				START_EXT( glActiveTextureARB );
				print_value( _GLenum, &target );
				END;
				GLV_EXT.glActiveTextureARB( target );
			}


			/* */
			void GLAPIENTRY
			glClientActiveTextureARB ( GLenum target )
			{
				START_EXT( glClientActiveTextureARB );
				print_value( _GLenum, &target );
				END;
				GLV_EXT.glClientActiveTextureARB( target );
			}


			/* LockArraysEXT */
			void GLAPIENTRY
			glLockArraysEXT ( GLint first, GLsizei count )
			{
				START_EXT( glLockArraysEXT );
				print_value( _GLint, &first );
				print( "," );
				print_value( _GLsizei, &count );
				END;
				GLV_EXT.glLockArraysEXT( first, count );
			}


			/* */
			void GLAPIENTRY
			glUnlockArraysEXT ( void )
			{
				START_EXT( glUnlockArraysEXT );
				END;
				GLV_EXT.glUnlockArraysEXT();
			}


			/* EXT_cull_vertex */
			void GLAPIENTRY
			glCullParameterdvEXT ( GLenum pname, GLdouble *params )
			{
				START_EXT( glCullParameterdvEXT );
				print_value( _GLenum, &pname );
				print( "," );
				print_value( _GLdouble4, &params );
				END;
				GLV_EXT.glCullParameterdvEXT( pname, params );
			}


			/* */
			void GLAPIENTRY
			glCullParameterfvEXT ( GLenum pname, GLfloat *params )
			{
				START_EXT( glCullParameterfvEXT );
				print_value( _GLenum, &pname );
				print( "," );
				print_value( _GLfloat4, &params );
				END;
				GLV_EXT.glCullParameterfvEXT( pname, params );
			}


			/* WIN_swap_hint */
			void GLAPIENTRY
			glAddSwapHintRectWIN ( GLint x, GLint y, GLsizei width, GLsizei height )
			{
				START_EXT( glAddSwapHintRectWIN );
				print_value( _GLint, &x );
				print( "," );
				print_value( _GLint, &y );
				print( "," );
				print_value( _GLsizei, &width );
				print( "," );
				print_value( _GLsizei, &height );
				print( "," );
				END;
				GLV_EXT.glAddSwapHintRectWIN( x, y, width, height );
			}


			/* EXT_vertex_weighting */
			void GLAPIENTRY
			glVertexWeightfEXT ( GLfloat weight )
			{
				START_EXT( glVertexWeightfEXT );
				print_value( _GLfloat, &weight );
				END;
				GLV_EXT.glVertexWeightfEXT( weight );
			}


			/* */
			void GLAPIENTRY
			glVertexWeightfvEXT ( GLfloat *weight )
			{
				START_EXT( glVertexWeightfvEXT );
				print_value( _GLfloat1, &weight );
				END;
				GLV_EXT.glVertexWeightfvEXT( weight );
			}


			/* */
			void GLAPIENTRY
			glVertexWeightPointerEXT ( GLint size, GLenum type, GLsizei stride, GLvoid *pointer )
			{
				START_EXT( glVertexWeightPointerEXT );
				print_value( _GLint, &size );
				print( "," );
				print_value( _GLenum, &type );
				print( "," );
				print_value( _GLsizei, &stride );
				print( "," );
				print_value( _GLvoid, &pointer );
				END;
				GLV_EXT.glVertexWeightPointerEXT( size, type, stride, pointer );
			}


			/* EXT_swap_control */
			BOOL GLAPIENTRY
			wglSwapIntervalEXT ( int interval )
			{
				BOOL	result;
				result = GLV_EXT.wglSwapIntervalEXT( interval );
				START_EXT( wglSwapIntervalEXT );
				print_value( _GLint, &interval );
				RESULT;
				print_value( _GLboolean, &result );
				ENDRESULT;
				return ( result );
			}


			/* */
			int GLAPIENTRY
			wglGetSwapIntervalEXT ( void )
			{
				int result;
				result = GLV_EXT.wglGetSwapIntervalEXT();
				START_EXT( wglGetSwapIntervalEXT );
				RESULT;
				print_value( _GLint, &result );
				ENDRESULT;
				return ( result );
			}

			/* WGL_ARB_extensions_string */
			const char *GLAPIENTRY wglGetExtensionsStringARB ( HDC hdc )
			{
				const char	*result;
				result = GLV_EXT.wglGetExtensionsStringARB( hdc );
				START_EXT( wglGetExtensionsStringARB );
				print_value( HEX, &hdc );
				RESULT;
				print_value( STR, &result );
				ENDRESULT;
				return ( result );
			}


			/* ARB_transpose_matrix */
			void GLAPIENTRY
			glLoadTransposeMatrixfARB ( const GLfloat *m )
			{
				START_EXT( glLoadTransposeMatrixfARB );
				print_value( _GLfloat16, &m );
				END;
				GLV_EXT.glLoadTransposeMatrixfARB( m );
			}


			/* */
			void GLAPIENTRY
			glLoadTransposeMatrixdARB ( const GLdouble *m )
			{
				START_EXT( glLoadTransposeMatrixdARB );
				print_value( _GLdouble16, &m );
				END;
				GLV_EXT.glLoadTransposeMatrixdARB( m );
			}


			/* */
			void GLAPIENTRY
			glMultTransposeMatrixfARB ( const GLfloat *m )
			{
				START_EXT( glMultTransposeMatrixfARB );
				print_value( _GLfloat16, &m );
				END;
				GLV_EXT.glMultTransposeMatrixfARB( m );
			}


			/* */
			void GLAPIENTRY
			glMultTransposeMatrixdARB ( const GLdouble *m )
			{
				START_EXT( glMultTransposeMatrixdARB );
				print_value( _GLdouble16, &m );
				END;
				GLV_EXT.glMultTransposeMatrixdARB( m );
			}


			/* WGL_ARB_buffer_region */
			HANDLE GLAPIENTRY
			wglCreateBufferRegionARB ( HDC hDC, int iLayerPlane, UINT uType )
			{
				HANDLE	result;
				result = GLV_EXT.wglCreateBufferRegionARB( hDC, iLayerPlane, uType );
				START_EXT( wglCreateBufferRegionARB );
				print_value( HEX, &hDC );
				print( "," );
				print_value( _GLint, &iLayerPlane );
				print( "," );
				print_value( _bufferregionbitfield, &uType );
				RESULT;
				print_value( HEX, &result );
				ENDRESULT;
				return ( result );
			}


			/* */
			void GLAPIENTRY
			wglDeleteBufferRegionARB ( HANDLE hRegion )
			{
				START_EXT( wglDeleteBufferRegionARB );
				print_value( HEX, &hRegion );
				END;
				GLV_EXT.wglDeleteBufferRegionARB( hRegion );
			}


			/* */
			BOOL GLAPIENTRY
			wglSaveBufferRegionARB ( HANDLE hRegion, int x, int y, int width, int height )
			{
				BOOL	result;
				result = GLV_EXT.wglSaveBufferRegionARB( hRegion, x, y, width, height );
				START_EXT( wglSaveBufferRegionARB );
				print_value( HEX, &hRegion );
				print( "," );
				print_value( _GLint, &x );
				print( "," );
				print_value( _GLuint, &y );
				print( "," );
				print_value( _GLuint, &width );
				print( "," );
				print_value( _GLuint, &height );
				RESULT;
				print_value( _GLboolean, &result );
				ENDRESULT;
				return ( result );
			}


			/* */
			BOOL GLAPIENTRY
			wglRestoreBufferRegionARB ( HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc )
			{
				BOOL	result;
				result = GLV_EXT.wglRestoreBufferRegionARB( hRegion, x, y, width, height, xSrc, ySrc );
				START_EXT( wglRestoreBufferRegionARB );
				print_value( HEX, &hRegion );
				print( "," );
				print_value( _GLint, &x );
				print( "," );
				print_value( _GLuint, &y );
				print( "," );
				print_value( _GLuint, &width );
				print( "," );
				print_value( _GLuint, &height );
				print( "," );
				print_value( _GLuint, &xSrc );
				print( "," );
				print_value( _GLuint, &ySrc );
				RESULT;
				print_value( _GLboolean, &result );
				ENDRESULT;
				return ( result );
			}


			/* ARB_multisample */
			void GLAPIENTRY
			glSampleCoverageARB ( GLclampf value, GLboolean invert )
			{
				START_EXT( glSampleCoverageARB );
				print_value( _GLclampf, &value );
				print( "," );
				print_value( _GLboolean, &invert );
				END;
				GLV_EXT.glSampleCoverageARB( value, invert );
			}

			/* WGL_ARB_pixel_format */

			/* lots of extra code for this one... */
			GL_ENUM_PAIR	WGLenum_list[] =
			{
				{ "Invalid value", 0x0000 },
				{ "WGL_NUMBER_PIXEL_FORMATS_ARB", 0x2000 },
				{ "WGL_DRAW_TO_WINDOW_ARB", 0x2001 },
				{ "WGL_DRAW_TO_BITMAP_ARB", 0x2002 },
				{ "WGL_ACCELERATION_ARB", 0x2003 },
				{ "WGL_NEED_PALETTE_ARB", 0x2004 },
				{ "WGL_NEED_SYSTEM_PALETTE_ARB", 0x2005 },
				{ "WGL_SWAP_LAYER_BUFFERS_ARB", 0x2006 },
				{ "WGL_SWAP_METHOD_ARB", 0x2007 },
				{ "WGL_NUMBER_OVERLAYS_ARB", 0x2008 },
				{ "WGL_NUMBER_UNDERLAYS_ARB", 0x2009 },
				{ "WGL_TRANSPARENT_ARB", 0x200A },
				{ "WGL_TRANSPARENT_RED_VALUE_ARB", 0x2037 },
				{ "WGL_TRANSPARENT_GREEN_VALUE_ARB", 0x2038 },
				{ "WGL_TRANSPARENT_BLUE_VALUE_ARB", 0x2039 },
				{ "WGL_TRANSPARENT_ALPHA_VALUE_ARB", 0x203A },
				{ "WGL_TRANSPARENT_INDEX_VALUE_ARB", 0x203B },
				{ "WGL_SHARE_DEPTH_ARB", 0x200C },
				{ "WGL_SHARE_STENCIL_ARB", 0x200D },
				{ "WGL_SHARE_ACCUM_ARB", 0x200E },
				{ "WGL_SUPPORT_GDI_ARB", 0x200F },
				{ "WGL_SUPPORT_OPENGL_ARB", 0x2010 },
				{ "WGL_DOUBLE_BUFFER_ARB", 0x2011 },
				{ "WGL_STEREO_ARB", 0x2012 },
				{ "WGL_PIXEL_TYPE_ARB", 0x2013 },
				{ "WGL_COLOR_BITS_ARB", 0x2014 },
				{ "WGL_RED_BITS_ARB", 0x2015 },
				{ "WGL_RED_SHIFT_ARB", 0x2016 },
				{ "WGL_GREEN_BITS_ARB", 0x2017 },
				{ "WGL_GREEN_SHIFT_ARB", 0x2018 },
				{ "WGL_BLUE_BITS_ARB", 0x2019 },
				{ "WGL_BLUE_SHIFT_ARB", 0x201A },
				{ "WGL_ALPHA_BITS_ARB", 0x201B },
				{ "WGL_ALPHA_SHIFT_ARB", 0x201C },
				{ "WGL_ACCUM_BITS_ARB", 0x201D },
				{ "WGL_ACCUM_RED_BITS_ARB", 0x201E },
				{ "WGL_ACCUM_GREEN_BITS_ARB", 0x201F },
				{ "WGL_ACCUM_BLUE_BITS_ARB", 0x2020 },
				{ "WGL_ACCUM_ALPHA_BITS_ARB", 0x2021 },
				{ "WGL_DEPTH_BITS_ARB", 0x2022 },
				{ "WGL_STENCIL_BITS_ARB", 0x2023 },
				{ "WGL_AUX_BUFFERS_ARB", 0x2024 },
				{ "WGL_NO_ACCELERATION_ARB", 0x2025 },
				{ "WGL_GENERIC_ACCELERATION_ARB", 0x2026 },
				{ "WGL_FULL_ACCELERATION_ARB", 0x2027 },
				{ "WGL_SWAP_EXCHANGE_ARB", 0x2028 },
				{ "WGL_SWAP_COPY_ARB", 0x2029 },
				{ "WGL_SWAP_UNDEFINED_ARB", 0x202A },
				{ "WGL_TYPE_RGBA_ARB", 0x202B },
				{ "WGL_TYPE_COLORINDEX_ARB", 0x202C }
			};


			/* */
			char *
			getWGLString ( int attribute )
			{
				int i;
				for ( i = 0; i < ARY_CNT( WGLenum_list); i++ )
				{
					if ( WGLenum_list[i].tag == attribute )
					{
						return ( WGLenum_list[i].name );
					}
				}

				return ( WGLenum_list[0].name );
			}


			/* */
			BOOL GLAPIENTRY
			wglGetPixelFormatAttribivARB
			(
				HDC			hdc,
				int			iPixelFormat,
				int			iLayerPlane,
				UINT		nAttributes,
				const int	*piAttributes,
				int			*piValues
			)
			{
				BOOL	result;
				int		i;
				result = GLV_EXT.wglGetPixelFormatAttribivARB( hdc, iPixelFormat, iLayerPlane, nAttributes,
															   piAttributes, piValues );
				START_EXT( wglGetPixelFormatAttribivARB );
				print_value( HEX, &hdc );
				print( "," );
				print_value( _GLint, &iPixelFormat );
				print( "," );
				print_value( _GLint, &iLayerPlane );
				print( "," );
				print_value( _GLuint, &nAttributes );
				print( "," );
				print( "[" );
				for ( i = 0; i < (int) nAttributes; i++ )
				{
					if ( i != 0 )
					{
						print( "," );
					}

					print( "%s = %d", getWGLString( piAttributes[i]), piValues[i] );
				}

				print( "]" );
				RESULT;
				print_value( _GLboolean, &result );
				ENDRESULT;
				return ( result );
			}


			/* */
			BOOL GLAPIENTRY
			wglGetPixelFormatAttribfvARB
			(
				HDC			hdc,
				int			iPixelFormat,
				int			iLayerPlane,
				UINT		nAttributes,
				const int	*piAttributes,
				FLOAT		*pfValues
			)
			{
				BOOL	result;
				int		i;
				result = GLV_EXT.wglGetPixelFormatAttribfvARB( hdc, iPixelFormat, iLayerPlane, nAttributes,
															   piAttributes, pfValues );
				START_EXT( wglGetPixelFormatAttribfvARB );
				print_value( HEX, &hdc );
				print( "," );
				print_value( _GLint, &iPixelFormat );
				print( "," );
				print_value( _GLint, &iLayerPlane );
				print( "," );
				print_value( _GLuint, &nAttributes );
				print( "," );
				print( "[" );
				for ( i = 0; i < (int) nAttributes; i++ )
				{
					if ( i != 0 )
					{
						print( "," );
					}

					print( "%s = %f", getWGLString( piAttributes[i]), pfValues[i] );
				}

				print( "]" );
				RESULT;
				print_value( _GLboolean, &result );
				ENDRESULT;
				return ( result );
			}


			/* */
			BOOL GLAPIENTRY
			wglChoosePixelFormatARB
			(
				HDC			hdc,
				const int	*piAttribIList,
				const FLOAT *pfAttribFList,
				UINT		nMaxFormats,
				int			*piFormats,
				UINT		*nNumFormats
			)
			{
				BOOL	result;
				result = GLV_EXT.wglChoosePixelFormatARB( hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats,
														  nNumFormats );
				START_EXT( wglChoosePixelFormatARB );
				print_value( HEX, &hdc );
				print( "," );

				/* FIXME */
				print_value( _GLint1, &piAttribIList );
				print( "," );
				print_value( _GLfloat1, &pfAttribFList );
				print( "," );
				print_value( _GLuint, &nMaxFormats );
				print( "," );
				print_value( _GLint, &piFormats );
				print( "," );
				print_value( _GLuint1, &nNumFormats );
				RESULT;
				print_value( _GLboolean, &result );
				ENDRESULT;
				return ( result );
			}


			/* WGL_ARB_make_current_read */
			BOOL GLAPIENTRY
			wglMakeContextCurrentARB ( HDC hDrawDC, HDC hReadDC, HGLRC hglrc )
			{
				BOOL	result;
				result = GLV_EXT.wglMakeContextCurrentARB( hDrawDC, hReadDC, hglrc );
				START_EXT( wglMakeContextCurrentARB );
				print_value( HEX, &hDrawDC );
				print( "," );
				print_value( HEX, &hReadDC );
				print( "," );
				print_value( HEX, &hglrc );
				RESULT;
				print_value( _GLboolean, &result );
				ENDRESULT;
				return ( result );
			}


			/* */
			HDC GLAPIENTRY
			wglGetCurrentReadDCARB ( void )
			{
				HDC result;
				result = GLV_EXT.wglGetCurrentReadDCARB();
				START_EXT( wglGetCurrentReadDCARB );
				RESULT;
				print_value( HEX, &result );
				ENDRESULT;
				return ( result );
			}
			