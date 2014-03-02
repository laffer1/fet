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
#include "cg_local.h"
char	bigTextBuffer[100000];

// TTimo - defined but not used
#if 0


/*
======================
CG_ParseGibModels

Read a configuration file containing gib models for use with this character
======================
*/
static qboolean
CG_ParseGibModels ( bg_playerclass_t *classInfo )
{
	char			*text_p;
	int				len;
	int				i;
	char			*token;
	fileHandle_t	f;
	memset( classInfo->gibModels, 0, sizeof(classInfo->gibModels) );

	// load the file
	len = trap_FS_FOpenFile( "models/players/gibs.cfg" ), &f, FS_READ );

	//len = trap_FS_FOpenFile( va("models/players/%s/gibs.cfg", classInfo->modelPath), &f, FS_READ );
	if ( len <= 0 )
	{
		return ( qfalse );
	}

	if ( len >= sizeof(bigTextBuffer) - 1 )
	{

		//CG_Printf( "File %s too long\n", va("models/players/%s/gibs.cfg", classInfo->modelPath) );
		return ( qfalse );
	}

	trap_FS_Read( bigTextBuffer, len, f );
	bigTextBuffer[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = bigTextBuffer;
	for ( i = 0; i < MAX_GIB_MODELS; i++ )
	{
		token = COM_Parse( &text_p );
		if ( !token )
		{
			break;
		}

		// cache this model
		classInfo->gibModels[i] = trap_R_RegisterModel( token );
	}

	return ( qtrue );
}
#endif

/*
======================
CG_ParseHudHeadConfig
======================
*/
static qboolean
CG_ParseHudHeadConfig ( const char *filename, animation_t *hha )
{
	char			*text_p;
	int				len;
	int				i;
	float			fps;
	char			*token;
	fileHandle_t	f;

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 )
	{
		return ( qfalse );
	}

	if ( len >= sizeof(bigTextBuffer) - 1 )
	{
		CG_Printf( "File %s too long\n", filename );
		return ( qfalse );
	}

	trap_FS_Read( bigTextBuffer, len, f );
	bigTextBuffer[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = bigTextBuffer;
	for ( i = 0; i < MAX_HD_ANIMATIONS; i++ )
	{
		token = COM_Parse( &text_p );	// first frame
		if ( !token )
		{
			break;
		}

		hha[i].firstFrame = atoi( token );
		token = COM_Parse( &text_p );	// length
		if ( !token )
		{
			break;
		}

		hha[i].numFrames = atoi( token );
		token = COM_Parse( &text_p );	// fps
		if ( !token )
		{
			break;
		}

		fps = atof( token );
		if ( fps == 0 )
		{
			fps = 1;
		}

		hha[i].frameLerp = 1000 / fps;
		hha[i].initialLerp = 1000 / fps;
		token = COM_Parse( &text_p );	// looping frames
		if ( !token )
		{
			break;
		}

		hha[i].loopFrames = atoi( token );
		if ( hha[i].loopFrames > hha[i].numFrames )
		{
			hha[i].loopFrames = hha[i].numFrames;
		}
		else if ( hha[i].loopFrames < 0 )
		{
			hha[i].loopFrames = 0;
		}
	}

	if ( i != MAX_HD_ANIMATIONS )
	{
		CG_Printf( "Error parsing hud head animation file: %s", filename );
		return ( qfalse );
	}

	return ( qtrue );
}


/*
==================
CG_CalcMoveSpeeds
==================
*/
static void
CG_CalcMoveSpeeds ( bg_character_t *character )
{
	char			*tags[2] = { "tag_footleft", "tag_footright" };
	vec3_t			oldPos[2];
	refEntity_t		refent;
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
					CG_Error( "CG_CalcMoveSpeeds: unable to find tag %s, cannot calculate movespeed", tags[k] );
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
		anim->moveSpeed = (int) ( (totalSpeed / numSpeed) * 1000.0 / anim->frameLerp );
	}
}


/*
======================
CG_ParseAnimationFiles

  Read in all the configuration and script files for this model.
======================
*/
static qboolean
CG_ParseAnimationFiles ( bg_character_t *character, const char *animationGroup, const char *animationScript )
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
	CG_CalcMoveSpeeds( character );

	// load the script file
	len = trap_FS_FOpenFile( animationScript, &f, FS_READ );
	if ( len <= 0 )
	{
		return ( qfalse );
	}

	if ( len >= sizeof(bigTextBuffer) - 1 )
	{
		CG_Printf( "File %s is too long\n", filename );
		return ( qfalse );
	}

	trap_FS_Read( bigTextBuffer, len, f );
	bigTextBuffer[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	BG_AnimParseAnimScript( character->animModelInfo, &cgs.animScriptData, animationScript, bigTextBuffer );
	return ( qtrue );
}


/*
==================
CG_CheckForExistingAnimModelInfo

  If this player model has already been parsed, then use the existing information.
  Otherwise, set the modelInfo pointer to the first free slot.

  returns qtrue if existing model found, qfalse otherwise
==================
*/
static qboolean
CG_CheckForExistingAnimModelInfo
(
	const char		*animationGroup,
	const char		*animationScript,
	animModelInfo_t **animModelInfo
)
{
	int				i;
	animModelInfo_t *trav, *firstFree = NULL;
	for ( i = 0, trav = cgs.animScriptData.modelInfo; i < MAX_ANIMSCRIPT_MODELS; i++, trav++ )
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
		CG_Error( "unable to find a free modelinfo slot, cannot continue\n" );
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
==============
CG_RegisterAcc
==============
*/
static qboolean
CG_RegisterAcc ( const char *modelName, int *model, const char *skinname, qhandle_t *skin )
{
	char	filename[MAX_QPATH];
	*model = trap_R_RegisterModel( modelName );
	if ( !*model )
	{
		return ( qfalse );
	}

	COM_StripExtension( modelName, filename );
	Q_strcat( filename, sizeof(filename), va( "_%s.skin", skinname) );
	*skin = trap_R_RegisterSkin( filename );
	return ( qtrue );
}

typedef struct
{
	char		*type;
	accType_t	index;
} acc_t;
static acc_t	cg_accessories[] =
{
	{ "md3_beltr", ACC_BELT_LEFT },
	{ "md3_beltl", ACC_BELT_RIGHT },
	{ "md3_belt", ACC_BELT },
	{ "md3_back", ACC_BACK },
	{ "md3_weapon", ACC_WEAPON },
	{ "md3_weapon2", ACC_WEAPON2 },
	{ "md3_back2", ACC_BACK2 },
};
static int		cg_numAccessories = sizeof( cg_accessories ) / sizeof( cg_accessories[0] );
static acc_t	cg_headAccessories[] =
{
	{ "md3_hat", ACC_HAT },
	{ "md3_rank", ACC_RANK },
	{ "md3_hat2", ACC_MOUTH2 },
	{ "md3_hat3", ACC_MOUTH3 },
};
static int		cg_numHeadAccessories = sizeof( cg_headAccessories ) / sizeof( cg_headAccessories[0] );


/*
====================
CG_RegisterCharacter
====================
*/
qboolean
CG_RegisterCharacter ( const char *characterFile, bg_character_t *character )
{
	bg_characterDef_t	characterDef;
	char				*filename;
	char				buf[MAX_QPATH];
	char				accessoryname[MAX_QPATH];
	int					i;
	memset( &characterDef, 0, sizeof(characterDef) );
	if ( !BG_ParseCharacterFile( characterFile, &characterDef) )
	{
		return ( qfalse );	// the parser will provide the error message
	}

	// Register Weapons Skins
	if ( !(character->weaponskin = trap_R_RegisterSkin( characterDef.weaponskin)) )
	{
		CG_Printf( S_COLOR_YELLOW "WARNING: failed to register weaponskin '%s' referenced from '%s'\n",
				   characterDef.weaponskin, characterFile );
	}

	// Register Mesh
	if ( !(character->mesh = trap_R_RegisterModel( characterDef.mesh)) )
	{
		CG_Printf( S_COLOR_YELLOW "WARNING: failed to register mesh '%s' referenced from '%s'\n", characterDef.mesh,
				   characterFile );
	}

	// Register Skin
	COM_StripExtension( characterDef.mesh, buf );
	filename = va( "%s_%s.skin", buf, characterDef.skin );
	if ( !(character->skin = trap_R_RegisterSkin( filename)) )
	{
		CG_Printf( S_COLOR_YELLOW "WARNING: failed to register skin '%s' referenced from '%s'\n", filename,
				   characterFile );
	}
	else
	{
		for ( i = 0; i < cg_numAccessories; i++ )
		{
			if ( trap_R_GetSkinModel( character->skin, cg_accessories[i].type, accessoryname) )
			{
				if
				(
					!CG_RegisterAcc( accessoryname, &character->accModels[cg_accessories[i].index], characterDef.skin,
									 &character->accSkins[cg_accessories[i].index])
				)
				{
					CG_Printf( S_COLOR_YELLOW "WARNING: failed to register accessory '%s' referenced from '%s'->'%s'\n",
							   accessoryname, characterFile, filename );
				}
			}
		}

		for ( i = 0; i < cg_numHeadAccessories; i++ )
		{
			if ( trap_R_GetSkinModel( character->skin, cg_headAccessories[i].type, accessoryname) )
			{
				if
				(
					!CG_RegisterAcc( accessoryname, &character->accModels[cg_headAccessories[i].index],
									 characterDef.skin, &character->accSkins[cg_headAccessories[i].index])
				)
				{
					CG_Printf( S_COLOR_YELLOW "WARNING: failed to register accessory '%s' referenced from '%s'->'%s'\n",
							   accessoryname, characterFile, filename );
				}
			}
		}
	}

	// Register Undressed Corpse Media
	if ( *characterDef.undressedCorpseModel )
	{

		// Register Undressed Corpse Model
		if ( !(character->undressedCorpseModel = trap_R_RegisterModel( characterDef.undressedCorpseModel)) )
		{
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to register undressed corpse model '%s' referenced from '%s'\n",
					   characterDef.undressedCorpseModel, characterFile );
		}

		// Register Undressed Corpse Skin
		COM_StripExtension( characterDef.undressedCorpseModel, buf );
		filename = va( "%s_%s.skin", buf, characterDef.undressedCorpseSkin );
		if ( !(character->undressedCorpseSkin = trap_R_RegisterSkin( filename)) )
		{
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to register undressed corpse skin '%s' referenced from '%s'\n",
					   filename, characterFile );
		}
	}

	// Register the head for the hud
	if ( *characterDef.hudhead )
	{

		// Register Hud Head Model
		if ( !(character->hudhead = trap_R_RegisterModel( characterDef.hudhead)) )
		{
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to register hud head model '%s' referenced from '%s'\n",
					   characterDef.hudhead, characterFile );
		}

		if ( *characterDef.hudheadskin && !(character->hudheadskin = trap_R_RegisterSkin( characterDef.hudheadskin)) )
		{
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to register hud head skin '%s' referenced from '%s'\n",
					   characterDef.hudheadskin, characterFile );
		}

		if ( *characterDef.hudheadanims )
		{
			if ( !CG_ParseHudHeadConfig( characterDef.hudheadanims, character->hudheadanimations) )
			{
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to register hud head animations '%s' referenced from '%s'\n",
						   characterDef.hudheadanims, characterFile );
			}
		}
		else
		{
			CG_Printf( S_COLOR_YELLOW "WARNING: no hud head animations supplied in '%s'\n", characterFile );
		}
	}

	// Parse Animation Files
	if
	(
		!CG_CheckForExistingAnimModelInfo( characterDef.animationGroup, characterDef.animationScript,
										   &character->animModelInfo)
	)
	{
		if ( !CG_ParseAnimationFiles( character, characterDef.animationGroup, characterDef.animationScript) )
		{
			CG_Printf( S_COLOR_YELLOW "WARNING: failed to load animation files referenced from '%s'\n", characterFile );
			return ( qfalse );
		}
	}

	// STILL MISSING: GIB MODELS (OPTIONAL?)
	return ( qtrue );
}


/* */
bg_character_t *
CG_CharacterForClientinfo ( clientInfo_t *ci, centity_t *cent )
{
	int team, cls;
	if ( cent && cent->currentState.eType == ET_CORPSE )
	{
		if ( cent->currentState.onFireStart >= 0 )
		{
			return ( cgs.gameCharacters[cent->currentState.onFireStart] );
		}
		else
		{
			if ( cent->currentState.modelindex < 4 )
			{
				return ( BG_GetCharacter( cent->currentState.modelindex, cent->currentState.modelindex2) );
			}
			else
			{
				return ( BG_GetCharacter( cent->currentState.modelindex - 4, cent->currentState.modelindex2) );
			}
		}
	}

	if ( cent && cent->currentState.powerups & (1 << PW_OPS_DISGUISED) )
	{
		team = ci->team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;
		cls = ( cent->currentState.powerups >> PW_OPS_CLASS_1 ) & 7;
		return ( BG_GetCharacter( team, cls) );
	}

	if ( ci->character )
	{
		return ( ci->character );
	}

	return ( BG_GetCharacter( ci->team, ci->cls) );
}


/* */
bg_character_t *
CG_CharacterForPlayerstate ( playerState_t *ps )
{
	int team, cls;
	if ( ps->powerups[PW_OPS_DISGUISED] )
	{
		team = cgs.clientinfo[ps->clientNum].team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;
		cls = 0;
		if ( ps->powerups[PW_OPS_CLASS_1] )
		{
			cls |= 1;
		}

		if ( ps->powerups[PW_OPS_CLASS_2] )
		{
			cls |= 2;
		}

		if ( ps->powerups[PW_OPS_CLASS_3] )
		{
			cls |= 4;
		}

		return ( BG_GetCharacter( team, cls) );
	}

	return ( BG_GetCharacter( cgs.clientinfo[ps->clientNum].team, cgs.clientinfo[ps->clientNum].cls) );
}


/*
===================
CG_LoadHeadSkins
===================
*/
qboolean
CG_LoadHeadSkins ( const char *file, int team )
{
	int				count=-1;
	int				i;
	headSkinsDef_t	headDef;
	memset( &headDef, 0, sizeof(headDef) );
	count = BG_ParseHeadSkinsFile( file, &headDef );

	if ( !count )
	{
		return ( qfalse );
	}

	//count--;
	if ( team == TEAM_AXIS )
	{
		for ( i = 0; i < count; i++ )
		{
			if ( !(headSkinsAxis[i].skin = trap_R_RegisterSkin( headDef.skinPath[i])) )
			{
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to register headskin '%s'\n", headDef.skinPath[i] );
			}
		}
	}
	else
	{
		for ( i = 0; i < count; i++ )
		{
			if ( !(headSkinsAllies[i].skin = trap_R_RegisterSkin( headDef.skinPath[i])) )
			{
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to register headskin '%s'\n", headDef.skinPath[i] );
			}
		}
	}

	count--;
	CG_Printf( "%i player head skins for %s \n", count, (team == TEAM_AXIS ? "Axis" : "Allied") );
	if ( team == TEAM_AXIS )
	{
		trap_Cvar_Set( "cg_n1x", va( "%i", count) );
	}
	else
	{
		trap_Cvar_Set( "cg_n1a", va( "%i", count) );
	}

	return ( qtrue );
}


/*
===================
CG_LoadHeadModels
===================
*/
qboolean
CG_LoadHeadModels ( const char *file, int team )
{
	int				count;
	int				i;
	headModelsDef_t headDef;
	memset( &headDef, 0, sizeof(headDef) );
	count = BG_ParseHeadModelsFile( file, &headDef );
	if ( !count )
	{
		return ( qfalse );
	}

	if ( team == TEAM_AXIS )
	{
		for ( i = 0; i < count; i++ )
		{
			if ( !(headModelsAxis[i].model = trap_R_RegisterModel( headDef.modelPath[i])) )
			{
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to register headmodel '%s'\n", headDef.modelPath[i] );
			}
		}
	}
	else
	{
		for ( i = 0; i < count; i++ )
		{
			if ( !(headModelsAllies[i].model = trap_R_RegisterModel( headDef.modelPath[i])) )
			{
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to register headmodel '%s'\n", headDef.modelPath[i] );
			}
		}
	}

	count--;
	CG_Printf( "%i player head models for %s \n", count, (team == TEAM_AXIS ? "Axis" : "Allied") );
	if ( team == TEAM_AXIS )
	{
		trap_Cvar_Set( "cg_n2x", va( "%i", count) );
	}
	else
	{
		trap_Cvar_Set( "cg_n2a", va( "%i", count) );
	}

	return ( qtrue );
}


/*
=======================
CG_RegisterHeadsSkins
=======================
*/
void
CG_RegisterHeadSkins ( void )
{
	int		team;
	char	path[MAX_QPATH];
	for ( team = TEAM_AXIS; team <= TEAM_ALLIES; team++ )
	{		//1
		switch ( team )
		{	//2
			default:
			case TEAM_ALLIES:
				if ( cg_camo_allies.string[0] )
				{
					Com_sprintf( path, MAX_QPATH, "/characters/%s/allied/headSkins.txt", cg_camo_allies.string );
				}
				else
				{
					Com_sprintf( path, MAX_QPATH, "/characters/temperate/allied/headSkins.txt" );
				}
				break;
			case TEAM_AXIS:
				if ( cg_camo_axis.string[0] )
				{
					Com_sprintf( path, MAX_QPATH, "/characters/%s/axis/headSkins.txt", cg_camo_axis.string );
				}
				else
				{
					Com_sprintf( path, MAX_QPATH, "/characters/temperate/axis/headSkins.txt" );
				}
				break;
		}	//3

		if ( !CG_LoadHeadSkins( path, team) )
		{	//7
			char	path2[MAX_QPATH];
			if ( team == TEAM_ALLIES && cg_camo_allies.string[0] )
			{
				Com_sprintf( path2, MAX_QPATH, "/characters/temperate/allied/headSkins.txt" );
				if ( !CG_LoadHeadSkins( path2, team) )
				{
					CG_Error( "ERROR: No head skins found: failed to load headskins file '%s' for the %s \n", path2,
							  (team == TEAM_AXIS ? "Axis" : "Allied") );
				}
			}
			else if ( team == TEAM_AXIS && cg_camo_axis.string[0] )
			{
				Com_sprintf( path2, MAX_QPATH, "/characters/temperate/axis/headSkins.txt" );
				if ( !CG_LoadHeadSkins( path2, team) )
				{
					CG_Error( "No head skins found: failed to load headskins file '%s' for the %s \n", path2,
							  (team == TEAM_AXIS ? "Axis" : "Allied") );
				}
			}
			else
			{
				CG_Error( "No head skins found: failed to load headskins file '%s' for the %s \n", path,
						  (team == TEAM_AXIS ? "Axis" : "Allied") );
			}
		}	//7
	}		//1
}


/*
=======================
G_RegisterHeadsModels
=======================
*/
void
CG_RegisterHeadModels ( void )
{
	int		team;
	char	path[MAX_QPATH];
	for ( team = TEAM_AXIS; team <= TEAM_ALLIES; team++ )
	{		//1
		switch ( team )
		{	//2
			default:
			case TEAM_ALLIES:
				if ( cg_camo_allies.string[0] )
				{
					Com_sprintf( path, MAX_QPATH, "/characters/%s/allied/headModels.txt", cg_camo_allies.string );
				}
				else
				{
					Com_sprintf( path, MAX_QPATH, "/characters/temperate/allied/headModels.txt" );
				}
				break;
			case TEAM_AXIS:
				if ( cg_camo_allies.string[0] )
				{
					Com_sprintf( path, MAX_QPATH, "/characters/%s/axis/headModels.txt", cg_camo_axis.string );
				}
				else
				{
					Com_sprintf( path, MAX_QPATH, "/characters/temperate/axis/headModels.txt" );
				}
				break;
		}	//3

		if ( !CG_LoadHeadModels( path, team) )
		{	//7
			if ( team == TEAM_ALLIES && cg_camo_allies.string[0] )
			{
				Com_sprintf( path, MAX_QPATH, "/characters/temperate/allied/headModels.txt" );
				if ( !CG_LoadHeadModels( path, team) )
				{
					CG_Error( "ERROR: No head models found: failed to load headModels file '%s' for the %s \n", path,
							  (team == TEAM_AXIS ? "Axis" : "Allied") );
				}
			}
			else if ( team == TEAM_AXIS && cg_camo_axis.string[0] )
			{
				Com_sprintf( path, MAX_QPATH, "/characters/temperate/axis/headModels.txt" );
				if ( !CG_LoadHeadModels( path, team) )
				{
					CG_Error( "ERROR: No head models found: failed to load headModels file '%s' for the %s \n", path,
							  (team == TEAM_AXIS ? "Axis" : "Allied") );
				}
			}
			else
			{
				CG_Error( "ERROR: No head models found: failed to load headModels file '%s' for the %s \n", path,
						  (team == TEAM_AXIS ? "Axis" : "Allied") );
			}
		}	//7
	}		//1
}

/*
========================
CG_RegisterPlayerClasses
========================
*/
qboolean	classes_registerred = qfalse;


/* */
void
CG_RegisterPlayerClasses ( void )
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
					if ( cg_camo_allies.string[0] )
					{
						Q_strncpyz( filename, "", sizeof(filename) );
						Q_strcat( filename, sizeof(filename), "/characters/" );
						Q_strcat( filename, sizeof(filename), cg_camo_allies.string );
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
					if ( cg_camo_axis.string[0] )
					{
						Q_strncpyz( filename, "", sizeof(filename) );
						Q_strcat( filename, sizeof(filename), "/characters/" );
						Q_strcat( filename, sizeof(filename), cg_camo_axis.string );
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

			if ( !CG_RegisterCharacter( filename, character) )
			{
				CG_Error( "ERROR: CG_RegisterPlayerClasses: failed to load character file '%s' for the %s %s\n",
						  filename, (team == TEAM_AXIS ? "Axis" : "Allied"), BG_ClassnameForNumber( classInfo->classNum) );
			}

			if ( !(classInfo->icon = trap_R_RegisterShaderNoMip( classInfo->iconName)) )
			{
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to load class icon '%s' for the %s %s\n",
						   classInfo->iconName, (team == TEAM_AXIS ? "Axis" : "Allied"),
						   BG_ClassnameForNumber( classInfo->classNum) );
			}

			if ( !(classInfo->arrow = trap_R_RegisterShaderNoMip( classInfo->iconArrow)) )
			{
				CG_Printf( S_COLOR_YELLOW "WARNING: failed to load icon arrow '%s' for the %s %s\n",
						   classInfo->iconArrow, (team == TEAM_AXIS ? "Axis" : "Allied"),
						   BG_ClassnameForNumber( classInfo->classNum) );
			}
		}
	}

	classes_registerred = qtrue;
}

//
// UQ1: Use this instead to load ALL the characters into memory, from all directories..
// UQ1: No more limitations!
//
extern bg_playerclass_t bg_allies_playerclasses[NUM_PLAYER_CLASSES];
extern bg_playerclass_t bg_axis_playerclasses[NUM_PLAYER_CLASSES];
int						num_game_characters = 0;


/* */
void
CG_LoadAllCharacters ( void )
{
	int			numdirs;
	char		dirlist[1024];
	char		*dirptr;
	int			i, j, dirlen;
	qboolean	found = qfalse;

	//	cgs.gameCharacters = malloc(sizeof(cgs.gameCharacters)+1);
	//	memset(&cgs.gameCharacters, 0. sizeof(cgs.gameCharacters)
	numdirs = trap_FS_GetFileList( "characters", "", dirlist, 1024 );
	dirptr = dirlist;
	for ( i = 0; i < numdirs; i++, dirptr += dirlen + 1 )
	{
		dirlen = strlen( dirptr );
		if ( dirlen <= 0 )
		{
			continue;
		}

		if ( !Q_stricmpn( dirptr, ".svn/", 5) )
		{
			continue;	// UQ1: Because we are using SVN, need to strip this one hehe.
		}

		for ( j = 0; j < NUM_PLAYER_CLASSES; j++ )
		{
			{
				char				filename[MAX_QPATH];
				char				tempname[MAX_QPATH];
				bg_playerclass_t	*classInfo = &bg_allies_playerclasses[j];
				Q_strncpyz( filename, "characters/", MAX_QPATH );
				Q_strncpyz( tempname, dirptr, dirlen );
				Q_strcat( filename, MAX_QPATH, tempname );
				Q_strcat( filename, MAX_QPATH, va( "/%s", classInfo->characterFile) );
				CG_Printf( "Load %s.\n", filename );
				if ( !BG_FindCharacter( filename) )
				{
					cgs.gameCharacters[num_game_characters] = BG_FindFreeCharacter( filename );
					Q_strncpyz( cgs.gameCharacters[num_game_characters]->characterFile, filename,
								sizeof(cgs.gameCharacters[num_game_characters]->characterFile) );
					if ( !CG_RegisterCharacter( filename, cgs.gameCharacters[num_game_characters]) )
					{
						CG_Error( "ERROR: CG_LoadAllCharacters: failed to load character file '%s'\n", filename );
					}
				}
			}

			num_game_characters++;
			{
				char				filename[MAX_QPATH];
				char				tempname[MAX_QPATH];
				bg_playerclass_t	*classInfo = &bg_axis_playerclasses[j];
				Q_strncpyz( filename, "characters/", MAX_QPATH );
				Q_strncpyz( tempname, dirptr, dirlen );
				Q_strcat( filename, MAX_QPATH, tempname );
				Q_strcat( filename, MAX_QPATH, va( "/%s", classInfo->characterFile) );
				CG_Printf( "Load %s.\n", filename );
				if ( !BG_FindCharacter( filename) )
				{
					cgs.gameCharacters[num_game_characters] = BG_FindFreeCharacter( filename );
					Q_strncpyz( cgs.gameCharacters[num_game_characters]->characterFile, filename,
								sizeof(cgs.gameCharacters[num_game_characters]->characterFile) );
					if ( !CG_RegisterCharacter( filename, cgs.gameCharacters[num_game_characters]) )
					{
						CG_Error( "ERROR: CG_LoadAllCharacters: failed to load character file '%s'\n", filename );
					}
				}
			}

			num_game_characters++;
		}
	}
}
