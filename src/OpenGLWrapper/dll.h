#ifndef DLL_H
#define DLL_H
#ifdef __cplusplus
extern "C"
{
#endif
	typedef void ( *FUNCTION ) ();
	void		*dllLoad ( char *name );
	FUNCTION	dllGetFunction ( void *handle, char *name );
	void		dllUnload ( void *handle );
#ifdef __cplusplus
}
#endif
#endif /* DLL_H */
