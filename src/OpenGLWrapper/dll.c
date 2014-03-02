#include "dll.h"
#if defined WIN32 || defined WIN64
#ifdef _MSC_VER
#pragma warning( disable : 4201 )
#pragma warning( disable : 4214 )
#pragma warning( disable : 4115 )
#pragma warning( disable : 4514 )
#pragma warning( disable : 4127 )
#endif /* _MSC_VER */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef _MSC_VER
#pragma warning( default : 4201 )
#pragma warning( default : 4214 )
#pragma warning( default : 4115 )
#endif /* _MSC_VER */

/* */
void *
dllLoad ( char *name )
{
	HMODULE handle;
	handle = LoadLibrary( name );
	return (void *) handle;
}


/* */
FUNCTION
dllGetFunction ( void *handle, char *name )
{
	return ( FUNCTION ) GetProcAddress( (HMODULE) handle, name );
}


/* */
void
dllUnload ( void *handle )
{
	if ( handle )
	{
		(void) FreeLibrary( (HMODULE) handle );
	}
}

#else

/* assume Linux or Solaris */
#include <dlfcn.h>


/* */
void *
dllLoad ( char *name )
{
	void	*handle;
	handle = dlopen( name, RTLD_LAZY | RTLD_LOCAL );
	return ( handle );
}


/* */
void *
dllGetFunction ( void *handle, char *name )
{
	return ( dlsym( handle, name) );
}


/* */
void
dllUnload ( void *handle )
{
	if ( handle )
	{
		dlclose( handle );
	}
}
#endif
