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


//
// g_mem.c
//
#ifdef __ID_MEM__
#include "g_local.h"
#include "bg_malloc.h"

// Ridah, increased this (fixes Dan's crash)
#define POOLSIZE	( 4 * 1024 * 1024 )
static char memoryPool[POOLSIZE];
static int	allocPoint;


/* */
void *
G_Alloc ( int size )
{
	char	*p;
	if ( g_debugAlloc.integer )
	{
		G_Printf( "G_Alloc of %i bytes (%i left)\n", size, POOLSIZE - allocPoint - ((size + 31) &~31) );
	}

	if ( allocPoint + size > POOLSIZE )
	{
		G_Error( "G_Alloc: failed on allocation of %u bytes\n", size );
		return ( NULL );
	}

	p = &memoryPool[allocPoint];
	allocPoint += ( size + 31 ) &~31;
	return ( p );
}


/* */
void
G_InitMemory ( void )
{
	extern vmCvar_t g_hunkMegs;
	allocPoint = 0;
	BG_MemoryInit( g_hunkMegs.integer );
}


/* */
void
Svcmd_GameMem_f ( void )
{
	G_Printf( "Game memory status: %i out of %i bytes allocated\n", allocPoint, POOLSIZE );
}

#else //!__ID_MEM__

//
// g_mem.c
//
#include "g_local.h"


/* */
void *
G_Alloc ( int size )
{
	return ( Q_malloc( size) );
}


/* */
void
G_InitMemory ( void )
{
	extern vmCvar_t g_hunkMegs;
	BG_MemoryInit( g_hunkMegs.integer );
}


/* */
void
Svcmd_GameMem_f ( void )
{
	G_Printf( "Game memory status: new system\n" );
}
#endif // !__ID_MEM__
