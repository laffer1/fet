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


// This file holds all commands related to XP save
#include "g_local.h"

#define MAX_XPSTORAGEITEMS	MAX_CLIENTS

static	XPStorage_t xpstorage[MAX_XPSTORAGEITEMS];


/*
=================
G_FindXPBackup
=================
*/
XPStorage_t *
G_FindXPBackup ( char* from )
{
	int i;
	XPStorage_t* storage = 0;

	if ( from == 0 || *from == 0 )
	{
		return 0;
	}

	i = 0;
	while(	i < MAX_XPSTORAGEITEMS
			&& xpstorage[i].timeadded != 0
			&& Q_stricmp( xpstorage[i].guid, from) == 0) {
		i++;
	}

	if ( i < MAX_XPSTORAGEITEMS )
	{
		storage = xpstorage + i;
		storage->timeadded = 0;
	}

	return storage;
}


/*
=================
=================
*/
void
G_StoreXPBackup ( void )
{
	int		i;
	char	s[MAX_STRING_CHARS];
	for ( i = 0; i < MAX_XPSTORAGEITEMS; i++ )
	{

		if ( xpstorage[i].timeadded == 0 )
		{
			trap_Cvar_Set( va( "xpbackup%i", i), "" );
		}
		else
		{

			Com_sprintf( s, sizeof(s), "%32s %f %f %f %f %f %f %f %d %d", xpstorage[i].guid,
						 xpstorage[i].skills[0], xpstorage[i].skills[1], xpstorage[i].skills[2],
						 xpstorage[i].skills[3], xpstorage[i].skills[4], xpstorage[i].skills[5],
						 xpstorage[i].skills[6], xpstorage[i].lives, xpstorage[i].overall_killrating );
			trap_Cvar_Set( va( "xpbackup%i", i), s );
			G_Printf( "xpbackup%i: %s\n", i, s );
		}
	}
}


/*
=================
=================
*/
void
G_ReadXPBackup ( void )
{
	int		i;
	char	s[MAX_STRING_CHARS];
	for ( i = 0; i < MAX_XPSTORAGEITEMS; i++ )
	{
		trap_Cvar_VariableStringBuffer( va( "xpbackup%i", i), s, sizeof(s) );
		if ( *s != 0 )
		{
			sscanf( s, "%32s %f %f %f %f %f %f %f %d %d", &xpstorage[i].guid, &xpstorage[i].skills[0],
					&xpstorage[i].skills[1], &xpstorage[i].skills[2], &xpstorage[i].skills[3],
					&xpstorage[i].skills[4], &xpstorage[i].skills[5], &xpstorage[i].skills[6],
					&xpstorage[i].lives, &xpstorage[i].overall_killrating );
			xpstorage[i].timeadded = level.time;
			G_Printf( "xpbackup%i: %s\n", i, s );
		}
	}
}


/*
=================
=================
*/
void
G_ClearXPBackup ( void )
{
	int i;
	for ( i = 0; i < MAX_XPSTORAGEITEMS; i++ )
	{
		xpstorage[i].timeadded = 0;
	}
}


/*
=================
=================
*/
void
G_AddXPBackup ( gentity_t *ent )
{
	int			i;
	int			best = -1;
	int			besttime;
	const char	*guid;
	char		userinfo[MAX_INFO_STRING];

	trap_GetUserinfo( ent - g_entities, userinfo, sizeof(userinfo) );
	guid = Info_ValueForKey( userinfo, "cl_guid" );

	i = 0;
	// level.time should be larger then any item in the list
	besttime = level.time;

	while (  i < MAX_XPSTORAGEITEMS
			&& xpstorage[i].timeadded != 0
			&& Q_stricmp( xpstorage[i].guid, guid) == 0 )
	{
		if ( xpstorage[i].timeadded < besttime )
		{
			besttime = xpstorage[i].timeadded;
			best = i;
		}
		i++;
	}

	if( i < MAX_XPSTORAGEITEMS ) {
		best = i;
	}

	G_Printf( va( "Saving XP data for %s\n", guid) );

	xpstorage[best].timeadded = level.time;
	Q_strncpyz( xpstorage[best].guid, guid, 33 );

	for ( i = 0; i < SK_NUM_SKILLS; i++ )
	{
		xpstorage[best].skills[i] = ent->client->sess.skillpoints[i];
	}

	xpstorage[best].lives = -999;
	if ( ent->client->maxlivescalced )
	{
		xpstorage[best].lives = ent->client->ps.persistant[PERS_RESPAWNS_LEFT] - 1;
	}

	//josh:
	//	xpstorage[best].overall_killrating = ent->client->sess.overall_killrating;
}


/*
=================
=================
*/
void
G_ClearXPBackupLives ( void )
{
	int i;
	for ( i = 0; i < MAX_XPSTORAGEITEMS; i++ )
	{
		xpstorage[i].lives = -999;
	}
}

/*
=================
Svcmd_PrintXPStorage
=================
Kamikazee: Debug routine, called from ConsoleCommand(...)
*/
void
Svcmd_PrintXPStorage () {
	int i;

	for ( i = 0; i < MAX_XPSTORAGEITEMS; i++ )
	{
		if ( xpstorage[i].timeadded )
		{
				G_Printf( "storage%i: %32s %f %f %f %f %f %f %f %d %d", i, xpstorage[i].guid,
						 xpstorage[i].skills[0], xpstorage[i].skills[1], xpstorage[i].skills[2],
						 xpstorage[i].skills[3], xpstorage[i].skills[4], xpstorage[i].skills[5],
						 xpstorage[i].skills[6], xpstorage[i].lives, xpstorage[i].overall_killrating );
		}
	}
}
