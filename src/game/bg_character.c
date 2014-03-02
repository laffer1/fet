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
**  Character definition code
*/
#include "q_shared.h"
#include "bg_public.h"
#ifdef GAMEDLL
#include "g_local.h"
#endif
#ifdef CGAMEDLL
#include "../cgame/cg_local.h"
#endif
bg_character_t	alliedClassCharacters[NUM_PLAYER_CLASSES];
bg_character_t	axisClassCharacters[NUM_PLAYER_CLASSES];
#ifdef __NPC__


/* */
animModelInfo_t *
BG_NPCAnimModelInfo ( int team, int classNum )
{
	if ( team == TEAM_AXIS )
	{
		return ( axisClassCharacters[classNum].animModelInfo );
	}
	else
	{
		return ( alliedClassCharacters[classNum].animModelInfo );
	}
}
#endif //__NPC__

/* */
static qboolean
BG_PCF_ParseError ( int handle, char *format, ... )
{
	int			line;
	char		filename[128];
	va_list		argptr;
	static char string[4096];
	va_start( argptr, format );
	Q_vsnprintf( string, sizeof(string), format, argptr );
	va_end( argptr );
	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine( handle, filename, &line );
	Com_Printf( S_COLOR_RED "ERROR: %s, line %d: %s\n", filename, line, string );
	trap_PC_FreeSource( handle );
	return ( qfalse );
}


/* */
int
BG_ParseHeadSkinsFile ( const char *filename, headSkinsDef_t *headDef )
{
	pc_token_t	token;
	int			handle;
	int			i = 0;
	int			count = 0;
	handle = trap_PC_LoadSource( filename );
	if ( !handle )
	{
		return ( qfalse );
	}

	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "headSkins") )
	{
		return ( BG_PCF_ParseError( handle, "expected 'headSkins'") );
	}

	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "{") )
	{
		return ( BG_PCF_ParseError( handle, "expected '{'") );
	}

	while ( 1 )
	{		// while
		if ( !trap_PC_ReadToken( handle, &token) )
		{	///
			break;
		}	///

		if ( token.string[0] == '}' )
		{	///
			break;
		}	///

		if ( i > MAX_CUSTOM_SKINS )
		{
			break;
		}

		if ( !Q_stricmp( token.string, va( "skin%i", i)) )
		{
			if ( !PC_String_ParseNoAlloc( handle, headDef->skinPath[i], sizeof(headDef->skinPath[i])) )
			{
				return ( BG_PCF_ParseError( handle, "expected skin filename") );
			}

			count++;
			i++;
		}
	}		// while

	trap_PC_FreeSource( handle );
	return ( count );
}


/* */
int
BG_ParseHeadModelsFile ( const char *filename, headModelsDef_t *headDef )
{
	pc_token_t	token;
	int			handle;
	int			i = 0;
	int			count = 0;
	handle = trap_PC_LoadSource( filename );
	if ( !handle )
	{
		return ( qfalse );
	}

	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "headModels") )
	{
		return ( BG_PCF_ParseError( handle, "expected 'headModels'") );
	}

	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "{") )
	{
		return ( BG_PCF_ParseError( handle, "expected '{'") );
	}

	while ( 1 )
	{		// while
		if ( !trap_PC_ReadToken( handle, &token) )
		{	///
			break;
		}	///

		if ( token.string[0] == '}' )
		{	///
			break;
		}	///

		if ( i > MAX_CUSTOM_MODELS )
		{
			break;
		}

		if ( !Q_stricmp( token.string, va( "model%i", i)) )
		{
			if ( !PC_String_ParseNoAlloc( handle, headDef->modelPath[i], sizeof(headDef->modelPath[i])) )
			{
				return ( BG_PCF_ParseError( handle, "expected mesh filename") );
			}

			//Com_Printf( "Smrdis! :) '%i' , %s \n", i,  headDef->modelPath[i] );
			count++;
			i++;
		}
	}		// while

	trap_PC_FreeSource( handle );
	return ( count );
}


/*
int BG_ParseHeadModelsFile( const char *filename, headModelsDef_t *headDef  )
{
	pc_token_t	token;
	int			handle;
	int			count;

	handle = trap_PC_LoadSource( filename );

	if( !handle )
		return qfalse;

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "headModels" ) ) {
		return BG_PCF_ParseError( handle, "expected 'headModels'" );
	}

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) ) {
		return BG_PCF_ParseError( handle, "expected '{'" );
	}

	while( 1 ) {// while

		if( !trap_PC_ReadToken( handle, &token ) ) {///
			break;
		}///

		if( token.string[0] == '}' ) {///
			break;
		}///

			if( !Q_stricmp( token.string, "model0" ) ) {
					if( !PC_String_ParseNoAlloc( handle, headDef->modelPath[0], sizeof(headDef->modelPath[0]) ) ) 
						return BG_PCF_ParseError( handle, "expected mesh filename" );

					count = 1;
//					G_Error( "Smrdis! :)  %s \n",   headDef->modelPath[0] );

				}
			if( !Q_stricmp( token.string, "model1" ) ) {
					if( !PC_String_ParseNoAlloc( handle, headDef->modelPath[1], sizeof(headDef->modelPath[1]) ) ) 
						return BG_PCF_ParseError( handle, "expected mesh filename" );

					count = 2;
				}
			if( !Q_stricmp( token.string, "model2") ) {
					if( !PC_String_ParseNoAlloc( handle, headDef->modelPath[2], sizeof(headDef->modelPath[2]) ) ) 
						return BG_PCF_ParseError( handle, "expected mesh filename" );

					count = 3;
				}
			if( !Q_stricmp( token.string, "model3") ) {
					if( !PC_String_ParseNoAlloc( handle, headDef->modelPath[3], sizeof(headDef->modelPath[3]) ) ) 
						return BG_PCF_ParseError( handle, "expected mesh filename" );

					count = 4;
				}
			if( !Q_stricmp( token.string, "model4") ) {
					if( !PC_String_ParseNoAlloc( handle, headDef->modelPath[4], sizeof(headDef->modelPath[4]) ) ) 
						return BG_PCF_ParseError( handle, "expected mesh filename" );

					count = 5;
				}

		} // while

	trap_PC_FreeSource( handle );

	return count;
}

*/
qboolean
BG_ParseCharacterFile ( const char *filename, bg_characterDef_t *characterDef )
{
	pc_token_t	token;
	int			handle;
	handle = trap_PC_LoadSource( filename );
	if ( !handle )
	{
		return ( qfalse );
	}

	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "characterDef") )
	{
		return ( BG_PCF_ParseError( handle, "expected 'characterDef'") );
	}

	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "{") )
	{
		return ( BG_PCF_ParseError( handle, "expected '{'") );
	}

	while ( 1 )
	{
		if ( !trap_PC_ReadToken( handle, &token) )
		{
			break;
		}

		if ( token.string[0] == '}' )
		{
			break;
		}

		if ( !Q_stricmp( token.string, "mesh") )
		{
			if ( !PC_String_ParseNoAlloc( handle, characterDef->mesh, sizeof(characterDef->mesh)) )
			{
				return ( BG_PCF_ParseError( handle, "expected mesh filename") );
			}
		}
		else if ( !Q_stricmp( token.string, "weaponskin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, characterDef->weaponskin, sizeof(characterDef->weaponskin)) )
			{
				return ( BG_PCF_ParseError( handle, "expected weaponskin filename") );
			}
		}
		else if ( !Q_stricmp( token.string, "animationGroup") )
		{
			if ( !PC_String_ParseNoAlloc( handle, characterDef->animationGroup, sizeof(characterDef->animationGroup)) )
			{
				return ( BG_PCF_ParseError( handle, "expected animationGroup filename") );
			}
		}
		else if ( !Q_stricmp( token.string, "animationScript") )
		{
			if ( !PC_String_ParseNoAlloc( handle, characterDef->animationScript, sizeof(characterDef->animationScript)) )
			{
				return ( BG_PCF_ParseError( handle, "expected animationScript filename") );
			}
		}
		else if ( !Q_stricmp( token.string, "skin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, characterDef->skin, sizeof(characterDef->skin)) )
			{
				return ( BG_PCF_ParseError( handle, "expected skin filename") );
			}
		}
		else if ( !Q_stricmp( token.string, "undressedCorpseModel") )
		{
			if
			(
				!PC_String_ParseNoAlloc( handle, characterDef->undressedCorpseModel,
										 sizeof(characterDef->undressedCorpseModel))
			)
			{
				return ( BG_PCF_ParseError( handle, "expected undressedCorpseModel filename") );
			}
		}
		else if ( !Q_stricmp( token.string, "undressedCorpseSkin") )
		{
			if
			(
				!PC_String_ParseNoAlloc( handle, characterDef->undressedCorpseSkin,
										 sizeof(characterDef->undressedCorpseSkin))
			)
			{
				return ( BG_PCF_ParseError( handle, "expected undressedCorpseSkin filename") );
			}
		}
		else if ( !Q_stricmp( token.string, "hudhead") )
		{
			if ( !PC_String_ParseNoAlloc( handle, characterDef->hudhead, sizeof(characterDef->hudhead)) )
			{
				return ( BG_PCF_ParseError( handle, "expected hudhead filename") );
			}
		}
		else if ( !Q_stricmp( token.string, "hudheadskin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, characterDef->hudheadskin, sizeof(characterDef->hudheadskin)) )
			{
				return ( BG_PCF_ParseError( handle, "expected hudhead filename") );
			}
		}
		else if ( !Q_stricmp( token.string, "hudheadanims") )
		{
			if ( !PC_String_ParseNoAlloc( handle, characterDef->hudheadanims, sizeof(characterDef->hudheadanims)) )
			{
				return ( BG_PCF_ParseError( handle, "expected hudheadanims filename") );
			}
		}
		else
		{
			return ( BG_PCF_ParseError( handle, "unknown token '%s'", token.string) );
		}
	}

	trap_PC_FreeSource( handle );
	return ( qtrue );
}


/* */
bg_character_t *
BG_GetCharacter ( int team, int cls )
{
	switch ( team )
	{
		default:
		case TEAM_AXIS:
			return ( &axisClassCharacters[cls] );
		case TEAM_ALLIES:
			return ( &alliedClassCharacters[cls] );
	}
}


/* */
bg_character_t *
BG_GetCharacterForPlayerstate ( playerState_t *ps )
{

	// FIXME: add disguise?
	switch ( ps->persistant[PERS_TEAM] )
	{
		default:
		case TEAM_AXIS:
			return ( &axisClassCharacters[ps->stats[STAT_PLAYER_CLASS]] );
		case TEAM_ALLIES:
			return ( &alliedClassCharacters[ps->stats[STAT_PLAYER_CLASS]] );
	}
}

//
// Character Pool - used for custom characters
//
bg_character_t	bg_characterPool[MAX_GAME_CHARACTERS /*MAX_CHARACTERS*/ ];
qboolean		bg_characterPoolInuse[MAX_GAME_CHARACTERS /*MAX_CHARACTERS*/ ];


/* */
void
BG_ClearCharacterPool ( void )
{
	memset( &bg_characterPool, 0, sizeof(bg_characterPool) );
	memset( &bg_characterPoolInuse, 0, sizeof(bg_characterPoolInuse) );
}


/* */
bg_character_t *
BG_FindFreeCharacter ( const char *characterFile )
{
	int i;

	// see if we already got it
	for ( i = 0; i < MAX_GAME_CHARACTERS /*MAX_CHARACTERS*/ ; i++ )
	{
		if ( !bg_characterPoolInuse[i] )
		{
			continue;
		}

		if ( !Q_stricmp( characterFile, bg_characterPool[i].characterFile) )
		{
			return ( &bg_characterPool[i] );
		}
	}

	// else get a free one
	for ( i = 0; i < MAX_GAME_CHARACTERS /*MAX_CHARACTERS*/ ; i++ )
	{
		if ( !bg_characterPoolInuse[i] )
		{
			bg_characterPoolInuse[i] = qtrue;
			Q_strncpyz( bg_characterPool[i].characterFile, characterFile, sizeof(bg_characterPool[i].characterFile) );
			return ( &bg_characterPool[i] );
		}
	}

	// should never get here
	return ( NULL );
}


/* */
bg_character_t *
BG_FindCharacter ( const char *characterFile )
{
	int i;

	// see if we already got it
	for ( i = 0; i < MAX_GAME_CHARACTERS /*MAX_CHARACTERS*/ ; i++ )
	{
		if ( !bg_characterPoolInuse[i] )
		{
			continue;
		}

		if ( !Q_stricmp( characterFile, bg_characterPool[i].characterFile) )
		{
			return ( &bg_characterPool[i] );
		}
	}

	// nope didn't find it
	return ( NULL );
}


/* */
qboolean
BG_ParseLoadoutFile ( const char *filename, bg_playerclass_t *characterDef )
{
	return ( qtrue );
}
