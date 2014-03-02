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


/*
  Semi-braindamaged Malloc for Quake 3 Virtual Machine.
  Copyright 2002  PhaethonH <phaethon@linux.ucla.edu>

  Permission granted to copy, modify, distribute, and use, provided this
  copyright notice remains intact.  Source is provided on an as-is basis,
  with no warranties.
*/
#include "bg_malloc.h"
#ifdef WIN32
#include <windows.h>
#else
#include <stdlib.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum
{
	MALLOC_MAGIC	= 0x6B8E6EB9,
	MALLOC_ARENA	= 1024 *8192,	/* 8MB of default heap memory. */
	_ALLOCA_RINGSIZE= 256,
	PAGESIZE		= 128,
};
static int _mem_size = MALLOC_ARENA;
static unsigned char *_mem_arena = NULL;
static int *_arena_metadata = 0;
static unsigned char *_arena_data = 0;
static int _arena_pages = 0;
static int _arena_freestart = 1;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Every PAGESIZE-bytes chunk of heap is attached to one integer metadata.
Metadata holds a key, the value of the starting address of the memory block.
The result is _arena_pages number of chunks, with a corresponding _arena_pages
number of metadata integers.
*/


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void
malloc_init ( void )
{
	if ( _mem_arena == NULL )
	{
		Com_Error( ERR_FATAL, "Try to use memory without heap init" );
	}

	if ( ((int *) _mem_arena)[0] != MALLOC_MAGIC )
	{
		int divider = PAGESIZE + sizeof( *_arena_metadata );
		_arena_pages = _mem_size / divider;
		_arena_metadata = (int *) _mem_arena;
		_arena_data = (unsigned char *) ( _arena_metadata + _arena_pages );
		( (int *) _mem_arena )[0] = MALLOC_MAGIC;
	}

	//Com_Printf("malloc_init: %d pages of %d bytes.  Metadata @ %08X, heap @ %08X\n", _arena_pages, PAGESIZE, _arena_metadata, _arena_data);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int
findspot ( int pages )
{
	int i, j;
	malloc_init();

	//Com_Printf("findspot: for %d pages\n", pages);
	//  for (i = 1; i < (_arena_pages - pages); i++)
	if ( _arena_freestart >= (_arena_pages - pages) )
	{
		_arena_freestart = 1;
	}

	//Com_Printf("findspot: for %d pages starting @ %d\n", pages, _arena_freestart);
	for ( i = _arena_freestart; i < (_arena_pages - pages); i++ )
	{
		if ( _arena_metadata[i] == 0 )
		{
			for ( j = 0; j < pages; j++ )
			{
				if ( _arena_metadata[i + j] != 0 )
				{
					break;
				}
			}

			if ( j < pages )	/* loop ended because of insufficient Q_free pages. */
			{
				i += j;			/* Skip the blocks we know we can't use. */
				continue;		/* with  for i = ... */
			}

			for ( j = 0; j < pages; j++ )
			{
				_arena_metadata[i + j] = (int) ( _arena_data + (i * PAGESIZE) );
			}

			//Com_Printf("findspot: returning page %d (x %d) @ %08X\n", i, pages, (_arena_data + (i * PAGESIZE)));
			_arena_freestart = i;
			return ( i );
		}
	}

	Com_Error( ERR_FATAL, "FATAL: Heap exhausted" );
	return ( 0 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int
usedblocks ( void *ptr )
{
	int i;
	int retval;
	malloc_init();
	retval = 0;
	for ( i = 0; i < _arena_pages; i++ )
	{
		if ( _arena_metadata[i] == (int) ptr )
		{
			retval++;
		}
	}

	return ( retval );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *
Q_malloc ( size_t size )
{
	int		pages;
	int		n;
	void	*retval;
	if ( _mem_arena == NULL )
	{
		Com_Error( ERR_FATAL, "Try to allocate memory of: %i bytes without heap init", size );
	}

	pages = ( size > 0 ) ? ( 1 + ((size - 1) / PAGESIZE) ) : 1;
	n = findspot( pages );
	retval = (void *) ( _arena_data + (n * PAGESIZE) );
	if ( retval == NULL )
	{
		Com_Error( ERR_FATAL, "Fail to allocate memory of: %i bytes", size );
	}

	//Com_Printf("Q_malloc: for %d bytes -> %08X\n", size, retval);
	return ( retval );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *
Q_calloc ( size_t nmemb, size_t size )
{
	char	*retval = (char *) ( Q_malloc( nmemb * size) );
	memset( retval, 0, nmemb * size );
	return (void *) retval;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
Q_free ( void *ptr )
{
	int n;
	malloc_init();

	//  n = 0;
	n = ( (unsigned char *) ptr - _arena_data ) / PAGESIZE;
	if ( (n < 0) || (n > _arena_pages) )
	{

		/* Outside heap.  Bad. */
		return;
	}

	_arena_freestart = n;	/* Next allocation tries here, to see if it fits. */
	while ( _arena_metadata[n] == (int) ptr )
	{
		_arena_metadata[n] = 0;
		n++;
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *
Q_realloc ( void *ptr, size_t size )
{
	void	*m;
	int		movesize;
	if ( size == 0 )
	{
		Q_free( ptr );
		return ( 0 );
	}

	m = Q_malloc( size );
	movesize = usedblocks( ptr ) * PAGESIZE;

	//Com_Printf("realloc: from %08X to %08X, old size %d, new size %d\n", ptr, m, movesize, size);
	if ( size < movesize )
	{
		movesize = size;
	}

	//Com_Printf("realloc: moving %d bytes\n", movesize);
	//  memmove(m, ptr, movesize);
	memcpy( m, ptr, movesize );
	Q_free( ptr );
	return ( m );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
BG_MemoryClose ( void )
{
#ifdef WIN32
	if ( _mem_arena != NULL )
	{
		LocalFree( (HLOCAL) _mem_arena );
	}

#else
	if ( _mem_arena != NULL )
	{
		free( _mem_arena );
	}
#endif
	_mem_size = MALLOC_ARENA;
	_mem_arena = NULL;
	_arena_metadata = NULL;
	_arena_data = NULL;
	_arena_pages = 0;
	_arena_freestart = 1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
BG_MemoryInit ( int hunkMegs )
{

	// Check if memory is already inited
	if ( _mem_arena == NULL )
	{
		_mem_size = hunkMegs * ( 1024 * 1024 );

		// We need to clamp it
		if ( _mem_size < MALLOC_ARENA )
		{
			_mem_size = MALLOC_ARENA;
		}

#ifdef WIN32
		_mem_arena = (unsigned char *) LocalAlloc( LPTR, _mem_size );
#else
		_mem_arena = (unsigned char *) malloc( _mem_size );
#endif
		if ( _mem_arena == NULL )
		{
			Com_Error( ERR_FATAL, "Not enougth system memory to alloc %d bytes", _mem_size );
		}

		malloc_init();
	}
}
