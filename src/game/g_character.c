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
**  Character loading
*/
#include "g_local.h"
#include "etpro_mdx.h"
static char text[100000];	// <- was causing callstacks >64k
qboolean	characters_loaded = qfalse;


/*
==================
G_CalcMoveSpeeds; adapted from BG_CalcMoveSpeeds
==================
*/
static void
G_CalcMoveSpeeds ( bg_character_t *character )
{
	char			*tags[2] = { "tag_footleft", "tag_footright" };
	vec3_t			oldPos[2] = { { 0, 0, 0 }, { 0, 0, 0 } };
	grefEntity_t	refent;
	animation_t		*anim;
	int				i, j, k;
	float			totalSpeed;
	int				numSpeed;
	int				lastLow, low;
	orientation_t	o[2];
	memset( &refent, 0, sizeof(refent) );
	refent.hModel = character->mesh;
	for ( i = 0; i < character->animModelInfo->numAnimations; i++ )
	{
		anim = character->animModelInfo->animations[i];
		if ( anim->moveSpeed >= 0 )
		{
			continue;
		}

		totalSpeed = 0;
		lastLow = -1;
		numSpeed = 0;

		// for each frame
		for ( j = 0; j < anim->numFrames; j++ )
		{
			refent.frame = anim->firstFrame + j;
			refent.oldframe = refent.frame;
			refent.torsoFrameModel = refent.oldTorsoFrameModel = refent.frameModel = refent.oldframeModel = anim->mdxFile;

			// for each foot
			for ( k = 0; k < 2; k++ )
			{
				if ( trap_R_LerpTag( &o[k], &refent, tags[k], 0) < 0 )
				{
					G_Error( "G_CalcMoveSpeeds: unable to find tag %s, cannot calculate movespeed", tags[k] );
				}
			}

			// find the contact foot
			if ( anim->flags & ANIMFL_LADDERANIM )
			{
				if ( o[0].origin[0] > o[1].origin[0] )
				{
					low = 0;
				}
				else
				{
					low = 1;
				}

				totalSpeed += fabs( oldPos[low][2] - o[low].origin[2] );
			}
			else
			{
				if ( o[0].origin[2] < o[1].origin[2] )
				{
					low = 0;
				}
				else
				{
					low = 1;
				}

				totalSpeed += fabs( oldPos[low][0] - o[low].origin[0] );
			}

			numSpeed++;

			// save the positions
			for ( k = 0; k < 2; k++ )
			{
				VectorCopy( o[k].origin, oldPos[k] );
			}

			lastLow = low;
		}

		// record the speed
		anim->moveSpeed = rint( (totalSpeed / numSpeed) * 1000.0 / anim->frameLerp );
	}
}


/*
=====================
G_ParseAnimationFiles

  Read in all the configuration and script files for this model.
=====================
*/
static qboolean
G_ParseAnimationFiles ( bg_character_t *character, const char *animationGroup, const char *animationScript )
{
	char			filename[MAX_QPATH];
	fileHandle_t	f;
	int				len;

	// set the name of the animationGroup and animationScript in the animModelInfo structure
	Q_strncpyz( character->animModelInfo->animationGroup, animationGroup,
				sizeof(character->animModelInfo->animationGroup) );
	Q_strncpyz( character->animModelInfo->animationScript, animationScript,
				sizeof(character->animModelInfo->animationScript) );
	BG_R_RegisterAnimationGroup( animationGroup, character->animModelInfo );

	// calc movespeed values if required
	G_CalcMoveSpeeds( character );

	// load the script file
	len = trap_FS_FOpenFile( animationScript, &f, FS_READ );
	if ( len <= 0 )
	{
		return ( qfalse );
	}

	if ( len >= sizeof(text) - 1 )
	{
		G_Printf( "File %s is too long\n", filename );
		return ( qfalse );
	}

	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	BG_AnimParseAnimScript( character->animModelInfo, &level.animScriptData, animationScript, text );
	return ( qtrue );
}


/*
==================
G_CheckForExistingAnimModelInfo

  If this player model has already been parsed, then use the existing information.
  Otherwise, set the modelInfo pointer to the first free slot.

  returns qtrue if existing model found, qfalse otherwise
==================
*/
static qboolean
G_CheckForExistingAnimModelInfo
(
	const char		*animationGroup,
	const char		*animationScript,
	animModelInfo_t **animModelInfo
)
{
	int				i;
	animModelInfo_t *trav, *firstFree = NULL;
	for ( i = 0, trav = level.animScriptData.modelInfo; i < MAX_ANIMSCRIPT_MODELS; i++, trav++ )
	{
		if ( *trav->animationGroup && *trav->animationScript )
		{
			if
			(
				!Q_stricmp( trav->animationGroup, animationGroup) &&
				!Q_stricmp( trav->animationScript, animationScript)
			)
			{

				// found a match, use this animModelInfo
				*animModelInfo = trav;
				return ( qtrue );
			}
		}
		else if ( !firstFree )
		{
			firstFree = trav;
		}
	}

	if ( !firstFree )
	{
		G_Error( "unable to find a free modelinfo slot, cannot continue\n" );
	}
	else
	{
		*animModelInfo = firstFree;

		// clear the structure out ready for use
		memset( *animModelInfo, 0, sizeof(*animModelInfo) );
	}

	// qfalse signifies that we need to parse the information from the script files
	return ( qfalse );
}


/*
===================
G_LoadHeadSkins
===================
*/
qboolean
G_LoadHeadSkins ( const char *file, int team )
{
	int				count;
	headSkinsDef_t	headDef;
	memset( &headDef, 0, sizeof(headDef) );
	count = BG_ParseHeadSkinsFile( file, &headDef );
	if ( !count )
	{
		return ( qfalse );
	}

	count--;
	G_Printf( "%i player head skins for %s \n", count, (team == TEAM_AXIS ? "Axis" : "Allied") );
	if ( team == TEAM_AXIS )
	{
		trap_Cvar_Set( "g_n1x", va( "%i", count) );
	}
	else
	{
		trap_Cvar_Set( "g_n1a", va( "%i", count) );
	}

	return ( qtrue );
}


/*
===================
G_LoadHeadModels
===================
*/
qboolean
G_LoadHeadModels ( const char *file, int team )
{
	int				count;
	headModelsDef_t headDef;
	memset( &headDef, 0, sizeof(headDef) );
	count = BG_ParseHeadModelsFile( file, &headDef );
	if ( !count )
	{
		return ( qfalse );
	}

	count--;
	G_Printf( "%i player head models for %s \n", count, (team == TEAM_AXIS ? "Axis" : "Allied") );
	if ( team == TEAM_AXIS )
	{
		trap_Cvar_Set( "g_n2x", va( "%i", count) );
	}
	else
	{
		trap_Cvar_Set( "g_n2a", va( "%i", count) );
	}

	return ( qtrue );
}


/*
===================
G_RegisterCharacter
===================
*/
qboolean
G_RegisterCharacter ( const char *characterFile, bg_character_t *character )
{
	bg_characterDef_t	characterDef;
	memset( &characterDef, 0, sizeof(characterDef) );
	if ( !BG_ParseCharacterFile( characterFile, &characterDef) )
	{
		return ( qfalse );	// the parser will provide the error message
	}

	// Register mesh
	if ( !(character->mesh = trap_R_RegisterModel( characterDef.mesh)) )
	{
		G_Printf( S_COLOR_YELLOW "WARNING: failed to register mesh '%s' referenced from '%s'\n", characterDef.mesh,
				  characterFile );
	}

	// Parse Animation Files
	if
	(
		!G_CheckForExistingAnimModelInfo( characterDef.animationGroup, characterDef.animationScript,
										  &character->animModelInfo)
	)
	{
		if ( !G_ParseAnimationFiles( character, characterDef.animationGroup, characterDef.animationScript) )
		{
			G_Printf( S_COLOR_YELLOW "WARNING: failed to load animation files referenced from '%s'\n", characterFile );
			return ( qfalse );
		}

#ifdef BONE_HITTESTS
		{
			char	hitsfile[MAX_QPATH], *sep;

			// zinx - mdx hits
			Q_strncpyz( hitsfile, characterDef.animationGroup, sizeof(hitsfile) - 4 );
			if ( (sep = strrchr( hitsfile, '.')) )
			{				// FIXME: should abort on /'s
				strcpy( sep, ".hit" );
			}
			else
			{
				strcat( sep, ".hit" );
			}

			mdx_RegisterHits( character->animModelInfo, hitsfile );
		}
#endif
	}

	return ( qtrue );
}


/*
=======================
G_RegisterPlayerClasses
=======================
*/
void
G_RegisterPlayerClasses ( void )
{
	bg_playerclass_t	*classInfo;
	bg_character_t		*character;
	int					team, cls;
	char				filename[MAX_QPATH];
	for ( team = TEAM_AXIS; team <= TEAM_ALLIES; team++ )
	{
		for ( cls = PC_SOLDIER; cls < NUM_PLAYER_CLASSES; cls++ )
		{
			classInfo = BG_GetPlayerClassInfo( team, cls );
			character = BG_GetCharacter( team, cls );
			Q_strncpyz( character->characterFile, classInfo->characterFile, sizeof(character->characterFile) );
			switch ( team )
			{
				default:
				case TEAM_ALLIES:
					if ( g_camo_allies.string[0] )
					{
						Q_strncpyz( filename, "", sizeof(filename) );
						Q_strcat( filename, sizeof(filename), "/characters/" );
						Q_strcat( filename, sizeof(filename), g_camo_allies.string );
						Q_strcat( filename, sizeof(filename), "/" );
						Q_strcat( filename, sizeof(filename), character->characterFile );
					}
					else
					{
						Q_strcat( filename, sizeof(filename), "/characters/temperate/" );
						Q_strcat( filename, sizeof(filename), character->characterFile );
					}
					break;
				case TEAM_AXIS:
					if ( g_camo_axis.string[0] )
					{
						Q_strncpyz( filename, "", sizeof(filename) );
						Q_strcat( filename, sizeof(filename), "/characters/" );
						Q_strcat( filename, sizeof(filename), g_camo_axis.string );
						Q_strcat( filename, sizeof(filename), "/" );
						Q_strcat( filename, sizeof(filename), character->characterFile );
					}
					else
					{
						Q_strcat( filename, sizeof(filename), "/characters/temperate/" );
						Q_strcat( filename, sizeof(filename), character->characterFile );
					}
					break;
			}

			//if( !G_RegisterCharacter( character->characterFile, character ) ) {
			if ( !G_RegisterCharacter( filename, character) )
			{
				G_Error( "ERROR: G_RegisterPlayerClasses: failed to load character file '%s' for the %s %s\n", filename,
						 (team == TEAM_AXIS ? "Axis" : "Allied"), BG_ClassnameForNumber( classInfo->classNum) );

				//G_Error( "ERROR: G_RegisterPlayerClasses: failed to load character file '%s' for the %s %s\n", character->characterFile, (team == TEAM_AXIS ? "Axis" : "Allied"), BG_ClassnameForNumber( classInfo->classNum ) );
			}
		}
	}

	characters_loaded = qtrue;
}


/*
=======================
G_RegisterHeads
=======================
*/
void
G_RegisterHeadSkins ( void )
{
	int		team;
	char	path[MAX_QPATH];
	for ( team = TEAM_AXIS; team <= TEAM_ALLIES; team++ )
	{		//1
		//	Q_strncpyz( path, path, sizeof(path) );
		switch ( team )
		{	//2
			default:
			case TEAM_ALLIES:
				if ( g_camo_allies.string[0] )
				{
					Com_sprintf( path, MAX_QPATH, "/characters/%s/allied/headSkins.txt", g_camo_allies.string );
				}
				else
				{
					Com_sprintf( path, MAX_QPATH, "/characters/temperate/allied/headSkins.txt" );
				}
				break;
			case TEAM_AXIS:

				/*
	if(g_camo_axis.string[0]) {//5
		Q_strncpyz(path,
			"",
			sizeof(path));
		Q_strcat(path, sizeof(path), "/characters/");
		Q_strcat(path, sizeof(path), g_camo_axis.string);
		Q_strcat(path, sizeof(path), "/axis/headSkins.txt");
	} else {//56
		Q_strcat(path, sizeof(path), "/characters/temperate/axis/headSkins.txt");
	}//6
	*/
				if ( g_camo_axis.string[0] )
				{
					Com_sprintf( path, MAX_QPATH, "/characters/%s/axis/headSkins.txt", g_camo_axis.string );
				}
				else
				{
					Com_sprintf( path, MAX_QPATH, "/characters/temperate/axis/headSkins.txt" );
				}
				break;
		}	//3

		if ( !G_LoadHeadSkins( path, team) )
		{	//7
			char	path2[MAX_QPATH];
			if ( team == TEAM_ALLIES && g_camo_allies.string[0] )
			{
				Com_sprintf( path2, MAX_QPATH, "/characters/temperate/allied/headSkins.txt" );
				if ( !G_LoadHeadSkins( path2, team) )
				{
					G_Error( "ERROR: No head skins found: failed to load headskins file '%s' for the %s \n", path2,
							 (team == TEAM_AXIS ? "Axis" : "Allied") );
				}
			}
			else if ( team == TEAM_AXIS && g_camo_axis.string[0] )
			{
				Com_sprintf( path2, MAX_QPATH, "/characters/temperate/axis/headSkins.txt" );
				if ( !G_LoadHeadSkins( path2, team) )
				{
					G_Error( "No head skins found: failed to load headskins file '%s' for the %s \n", path2,
							 (team == TEAM_AXIS ? "Axis" : "Allied") );
				}
			}
			else
			{
				G_Error( "No head skins found: failed to load headskins file '%s' for the %s \n", path,
						 (team == TEAM_AXIS ? "Axis" : "Allied") );
			}
		}	//7
	}		//1
}


/*
=======================
G_RegisterHeads
=======================
*/
void
G_RegisterHeadModels ( void )
{
	int		team;
	char	path[MAX_QPATH];
	for ( team = TEAM_AXIS; team <= TEAM_ALLIES; team++ )
	{		//1
		switch ( team )
		{	//2
			default:
			case TEAM_ALLIES:
				if ( g_camo_allies.string[0] )
				{
					Com_sprintf( path, MAX_QPATH, "/characters/%s/allied/headModels.txt", g_camo_allies.string );
				}
				else
				{
					Com_sprintf( path, MAX_QPATH, "/characters/temperate/allied/headModels.txt" );
				}
				break;
			case TEAM_AXIS:
				if ( g_camo_allies.string[0] )
				{
					Com_sprintf( path, MAX_QPATH, "/characters/%s/axis/headModels.txt", g_camo_axis.string );
				}
				else
				{
					Com_sprintf( path, MAX_QPATH, "/characters/temperate/axis/headModels.txt" );
				}
				break;
		}	//3

		if ( !G_LoadHeadModels( path, team) )
		{	//7
			if ( team == TEAM_ALLIES && g_camo_allies.string[0] )
			{
				Com_sprintf( path, MAX_QPATH, "/characters/temperate/allied/headModels.txt" );
				if ( !G_LoadHeadModels( path, team) )
				{
					G_Error( "ERROR: No head models found: failed to load headModels file '%s' for the %s \n", path,
							 (team == TEAM_AXIS ? "Axis" : "Allied") );
				}
			}
			else if ( team == TEAM_AXIS && g_camo_axis.string[0] )
			{
				Com_sprintf( path, MAX_QPATH, "/characters/temperate/axis/headModels.txt" );
				if ( !G_LoadHeadModels( path, team) )
				{
					G_Error( "ERROR: No head models found: failed to load headModels file '%s' for the %s \n", path,
							 (team == TEAM_AXIS ? "Axis" : "Allied") );
				}
			}
			else
			{
				G_Error( "ERROR: No head models found: failed to load headModels file '%s' for the %s \n", path,
						 (team == TEAM_AXIS ? "Axis" : "Allied") );
			}
		}	//7
	}		//1
}


/*
=================
G_UpdateCharacter
=================
*/
void
G_UpdateCharacter ( gclient_t *client )
{
	char			infostring[MAX_INFO_STRING];
	char			*s;
	int				characterIndex;
	bg_character_t	*character;
	trap_GetUserinfo( client->ps.clientNum, infostring, sizeof(infostring) );
	s = Info_ValueForKey( infostring, "ch" );
	if ( *s )
	{
		characterIndex = atoi( s );
		if ( characterIndex < 0 || characterIndex >= MAX_GAME_CHARACTERS /*MAX_CHARACTERS*/ )
		{
			goto set_default_character;
		}

		if ( client->pers.characterIndex != characterIndex )
		{
			client->pers.characterIndex = characterIndex;
			trap_GetConfigstring( CS_CHARACTERS + characterIndex, infostring, MAX_INFO_STRING );
			if ( !(client->pers.character = BG_FindCharacter( infostring)) )
			{

				// not found - create it (this should never happen as we should have everything precached)
				client->pers.character = BG_FindFreeCharacter( infostring );
				if ( !client->pers.character )
				{
					goto set_default_character;
				}

				Q_strncpyz( client->pers.character->characterFile, infostring,
							sizeof(client->pers.character->characterFile) );
				if ( !G_RegisterCharacter( infostring, client->pers.character) )
				{
					G_Printf( S_COLOR_YELLOW "WARNING: G_UpdateCharacter: failed to load character file '%s' for %s\n",
							  infostring, client->pers.netname );
					goto set_default_character;
				}
			}

			// RF, reset anims so client's dont freak out
			// xkan: this can only be done if the model really changed - otherwise, the
			// animation may get screwed up if we are in the middle of some animation
			// and we come into this function;
			// plus, also reset the timer so we can properly start the next animation
			client->ps.legsAnim = 0;
			client->ps.torsoAnim = 0;
			client->ps.legsTimer = 0;
			client->ps.torsoTimer = 0;
		}

		return;
	}

set_default_character:

	// set default character
	character = BG_GetCharacter( client->sess.sessionTeam, client->sess.playerType );
	if ( client->pers.character != character )
	{
		client->pers.characterIndex = -1;
		client->pers.character = character;
		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;
	}
}
