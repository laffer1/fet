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
 * name:		cg_players.c
 *
 * desc:		handle the media and animation for player entities
 *
*/
#include "cg_local.h"
#include "../game/bg_classes.h"
#define SWING_RIGHT 1
#define SWING_LEFT	2
#define JUMP_HEIGHT 56
#define SWINGSPEED	0.3
static int			dp_realtime;
static float		jumpHeight;
animation_t			*lastTorsoAnim;
animation_t			*lastLegsAnim;
extern const char	*cg_skillRewards2[SK_NUM_SKILLS][NUM_SKILL_LEVELS - 1];
extern const char	*cg_skillRewards[SK_NUM_SKILLS][NUM_SKILL_LEVELS - 1];


/*
================
CG_EntOnFire
================
*/
qboolean
CG_EntOnFire ( centity_t *cent )
{
	if ( cent->currentState.number == cg.snap->ps.clientNum )
	{

		// TAT 11/15/2002 - the player is always starting out on fire, which is easily seen in cinematics
		//		so make sure onFireStart is not 0
		return
			(
				cg.snap->ps.onFireStart &&
				(cg.snap->ps.onFireStart < cg.time) &&
				((cg.snap->ps.onFireStart + 2000) > cg.time)
			);
	}
	else
	{
		return ( (cent->currentState.onFireStart < cg.time) && (cent->currentState.onFireEnd > cg.time) );
	}
}


/*
================
CG_IsCrouchingAnim
================
*/
qboolean
CG_IsCrouchingAnim ( animModelInfo_t *animModelInfo, int animNum )
{
	animation_t *anim;

	// FIXME: make compatible with new scripting
	animNum &= ~ANIM_TOGGLEBIT;

	//
	anim = BG_GetAnimationForIndex( animModelInfo, animNum );

	//
	if ( anim->movetype & ((1 << ANIM_MT_IDLECR) | (1 << ANIM_MT_WALKCR) | (1 << ANIM_MT_WALKCRBK)) )
	{
		return ( qtrue );
	}

	//
	return ( qfalse );
}


/*
================
CG_CustomSound
================
*/
sfxHandle_t
CG_CustomSound ( int clientNum, const char *soundName )
{
	if ( soundName[0] != '*' )
	{
		return ( trap_S_RegisterSound( soundName, qfalse) );
	}

	return ( 0 );
}

/*
=============================================================================

CLIENT INFO

=============================================================================
*/


/*
===================
CG_LoadClientInfo

Load it now, taking the disk hits.
This will usually be deferred to a safe time
===================
*/
static void
CG_LoadClientInfo ( int clientNum )
{
	int i;

	// reset any existing players and bodies, because they might be in bad
	// frames for this new model
	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		if ( cg_entities[i].currentState.clientNum == clientNum && cg_entities[i].currentState.eType == ET_PLAYER )
		{
			CG_ResetPlayerEntity( &cg_entities[i] );
		}
	}
}


/* */
void
CG_ParseTeamXPs ( int n )
{
	int			i, j;
	char		*cs = (char *) CG_ConfigString( CS_AXIS_MAPS_XP + n );
	const char	*token;
	for ( i = 0; i < MAX_MAPS_PER_CAMPAIGN; i++ )
	{
		for ( j = 0; j < SK_NUM_SKILLS; j++ )
		{
			token = COM_ParseExt( &cs, qfalse );
			if ( !token || !*token )
			{
				return;
			}

			if ( n == 0 )
			{
				cgs.tdbAxisMapsXP[j][i] = atoi( token );
			}
			else
			{
				cgs.tdbAlliedMapsXP[j][i] = atoi( token );
			}
		}
	}
}

void	CG_LimboPanel_SendSetupMsg ( qboolean forceteam );


/*
======================
CG_NewClientInfo
======================
*/
void
CG_NewClientInfo ( int clientNum )
{
	clientInfo_t	*ci;
	clientInfo_t	newInfo;
	const char		*configstring;
	const char		*v;

	//    int     oldclass;
	ci = &cgs.clientinfo[clientNum];
	configstring = CG_ConfigString( clientNum + CS_PLAYERS );
	if ( !*configstring )
	{
		memset( ci, 0, sizeof(*ci) );
		return; // player just left
	}

	// build into a temp buffer so the defer checks can use
	// the old value
	memset( &newInfo, 0, sizeof(newInfo) );

	// Gordon: grabbing some older stuff, if it's a new client, tinfo will update within one second anyway, otherwise you get the health thing flashing red
	// NOTE: why are we bothering to do all this setting up of a new clientInfo_t anyway? it was all for deffered clients iirc, which we dont have
	newInfo.location[0] = ci->location[0];
	newInfo.location[1] = ci->location[1];
	newInfo.health = ci->health;
	newInfo.fireteamData = ci->fireteamData;
	newInfo.clientNum = clientNum;
	newInfo.selected = ci->selected;
	newInfo.totalWeapAcc = ci->totalWeapAcc;

	// isolate the player's name
	v = Info_ValueForKey( configstring, "n" );
	Q_strncpyz( newInfo.name, v, sizeof(newInfo.name) );
	Q_strncpyz( newInfo.cleanname, v, sizeof(newInfo.cleanname) );
	Q_CleanStr( newInfo.cleanname );

	// bot skill
	v = Info_ValueForKey( configstring, "skill" );
	newInfo.botSkill = atoi( v );

	// team
	v = Info_ValueForKey( configstring, "t" );
	newInfo.team = atoi( v );

	// class
	v = Info_ValueForKey( configstring, "c" );
	newInfo.cls = atoi( v );

	// rank
	v = Info_ValueForKey( configstring, "r" );
	newInfo.rank = atoi( v );
	v = Info_ValueForKey( configstring, "f" );
	newInfo.fireteam = atoi( v );
	v = Info_ValueForKey( configstring, "m" );
	if ( *v )
	{
		int		i;
		char	buf[2];
		buf[1] = '\0';
		for ( i = 0; i < SK_NUM_SKILLS; i++ )
		{
			buf[0] = *v;
			newInfo.medals[i] = atoi( buf );
			v++;
		}
	}

	v = Info_ValueForKey( configstring, "ch" );
	if ( *v )
	{
		newInfo.character = cgs.gameCharacters[atoi( v )];
	}

	v = Info_ValueForKey( configstring, "s" );
	if ( *v )
	{
		int i;
		for ( i = 0; i < SK_NUM_SKILLS; i++ )
		{
			char	skill[2];
			skill[0] = v[i];
			skill[1] = '\0';
			newInfo.skill[i] = atoi( skill );
		}
	}

	// head model
	v = Info_ValueForKey( configstring, "hm" );
	newInfo.headModel = atoi( v );

	// head skin
	v = Info_ValueForKey( configstring, "hs" );
	newInfo.headSkin = atoi( v );

	// diguiseName
	v = Info_ValueForKey( configstring, "dn" );
	Q_strncpyz( newInfo.disguiseName, v, sizeof(newInfo.disguiseName) );

	// disguiseRank
	v = Info_ValueForKey( configstring, "dr" );
	newInfo.disguiseRank = atoi( v );

	// Gordon: weapon and latchedweapon ( FIXME: make these more secure )
	v = Info_ValueForKey( configstring, "w" );
	newInfo.weapon = atoi( v );
	v = Info_ValueForKey( configstring, "lw" );
	newInfo.latchedweapon = atoi( v );
	v = Info_ValueForKey( configstring, "sw" );
	newInfo.secondaryweapon = atoi( v );
	v = Info_ValueForKey( configstring, "ref" );
	newInfo.refStatus = atoi( v );

	// Gordon: detect rank/skill changes client side
	// CHRUKER: b019 - Make sure we have some valid clientinfo, otherwise people are thrown into spectator on map starts
	if ( clientNum == cg.clientNum && cgs.clientinfo[cg.clientNum].team > 0 )
	{
		int i;
		if ( newInfo.team != cgs.clientinfo[cg.clientNum].team )
		{
			if ( cgs.autoFireteamCreateEndTime != cg.time + 20000 )
			{
				cgs.autoFireteamCreateEndTime = 0;
			}

			if ( cgs.autoFireteamJoinEndTime != cg.time + 20000 )
			{
				cgs.autoFireteamJoinEndTime = 0;
			}
		}

		if ( newInfo.rank > cgs.clientinfo[cg.clientNum].rank )
		{
			CG_SoundPlaySoundScript( cgs.clientinfo[cg.clientNum].team == TEAM_ALLIES ? rankSoundNames_Allies[newInfo.rank] :
										 rankSoundNames_Axis[newInfo.rank], NULL, -1, qtrue );
			CG_AddPMItemBig( PM_RANK, va( "Promoted to rank %s!",
							 cgs.clientinfo[cg.clientNum].team == TEAM_AXIS ? rankNames_Axis[newInfo.rank] :
								 rankNames_Allies[newInfo.rank]), rankicons[newInfo.rank][0].shader );
		}

		// CHRUKER: b020 - Make sure player class and primary weapons are correct for
		// subsequent calls to CG_LimboPanel_SendSetupMsg
		CG_LimboPanel_Setup( qfalse );
		for ( i = 0; i < SK_NUM_SKILLS; i++ )
		{
			if ( newInfo.skill[i] > cgs.clientinfo[cg.clientNum].skill[i] )
			{

				// Gordon: slick hack so that funcs we call use teh new value now
				cgs.clientinfo[cg.clientNum].skill[i] = newInfo.skill[i];
				if ( newInfo.skill[i] == 4 && i == SK_HEAVY_WEAPONS )
				{
					if ( cgs.clientinfo[cg.clientNum].skill[SK_LIGHT_WEAPONS] >= 4 )
					{

						// CHRUKER: b020 - Only select SMG (2) if using the single gun (0)
						if ( cgs.ccSelectedWeapon2 == 0 )
						{

							//oldclass = cgs.ccSelectedClass;
							//cgs.ccSelectedClass = newInfo.cls;
							CG_LimboPanel_SetSelectedWeaponNumForSlot( 1, 2 );	// Selects SMG
							CG_LimboPanel_SendSetupMsg( qfalse );

							//cgs.ccSelectedClass = oldclass;
						}
					}
					else
					{

						//oldclass = cgs.ccSelectedClass;
						//cgs.ccSelectedClass = newInfo.cls;
						CG_LimboPanel_SetSelectedWeaponNumForSlot( 1, 1 );		// Selects SMG
						CG_LimboPanel_SendSetupMsg( qfalse );

						//cgs.ccSelectedClass = oldclass;
					}
				}

				if ( newInfo.skill[i] == 4 && i == SK_LIGHT_WEAPONS )
				{
					if ( cgs.clientinfo[cg.clientNum].skill[SK_HEAVY_WEAPONS] >= 4 )
					{

						// CHRUKER: b020 - Only select Akimbo guns (1) if using the single gun (0)
						if ( cgs.ccSelectedWeapon2 == 0 )
						{

							//oldclass = cgs.ccSelectedClass;
							//cgs.ccSelectedClass = newInfo.cls;
							CG_LimboPanel_SetSelectedWeaponNumForSlot( 1, 3 );
							CG_LimboPanel_SendSetupMsg( qfalse );

							//cgs.ccSelectedClass = oldclass;
						}
					}
					else
					{

						//oldclass = cgs.ccSelectedClass;
						//cgs.ccSelectedClass = newInfo.cls;
						CG_LimboPanel_SetSelectedWeaponNumForSlot( 1, 1 );
						CG_LimboPanel_SendSetupMsg( qfalse );

						//cgs.ccSelectedClass = oldclass;
					}
				}

				CG_AddPMItemBig( PM_SKILL, va( "Increased %s skill to level %i!", skillNames[i], newInfo.skill[i]),
								 cgs.media.skillPics[i] );
				if ( cg_snoopers.integer > 0 )
				{
					CG_PriorityCenterPrint( va( "You have been rewarded with %s", cg_skillRewards2[i][newInfo.skill[i] - 1]),
												SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.20), SMALLCHAR_WIDTH, 99999 );
				}
				else
				{
					CG_PriorityCenterPrint( va( "You have been rewarded with %s", cg_skillRewards[i][newInfo.skill[i] - 1]),
											SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.20), SMALLCHAR_WIDTH, 99999 );
				}
			}
		}

		if ( newInfo.team != cgs.clientinfo[cg.clientNum].team )
		{

			// clear these
			memset( cg.artilleryRequestPos, 0, sizeof(cg.artilleryRequestPos) );
			memset( cg.artilleryRequestTime, 0, sizeof(cg.artilleryRequestTime) );
		}

		trap_Cvar_Set( "authLevel", va( "%i", newInfo.refStatus) );
		if ( newInfo.refStatus != ci->refStatus )
		{
			if ( newInfo.refStatus <= RL_NONE )
			{
				const char	*info = CG_ConfigString( CS_SERVERINFO );
				trap_Cvar_Set( "cg_ui_voteFlags", Info_ValueForKey( info, "voteFlags") );
				CG_Printf( "[cgnotify]^3*** You have been stripped of your referee status! ***\n" );
			}
			else
			{
				trap_Cvar_Set( "cg_ui_voteFlags", "0" );
				CG_Printf( "[cgnotify]^2*** You have been authorized \"%s\" status ***\n",
						   ((newInfo.refStatus == RL_RCON) ? "rcon" : "referee") );
				CG_Printf( "Type: ^3ref^7 (by itself) for a list of referee commands.\n" );
			}
		}
	}

	// rain - passing the clientNum since that's all we need, and we
	// can't calculate it properly from the clientinfo
	CG_LoadClientInfo( clientNum );

	// replace whatever was there with the new one
	newInfo.infoValid = qtrue;
	*ci = newInfo;

	// make sure we have a character set
	if ( !ci->character )
	{
		ci->character = BG_GetCharacter( ci->team, ci->cls );
	}

	// Gordon: need to resort the fireteam list, incase ranks etc have changed
	CG_SortClientFireteam();
}


/*
=============================================================================

PLAYER ANIMATION

=============================================================================
*/
bg_playerclass_t *
CG_PlayerClassForClientinfo ( clientInfo_t *ci, centity_t *cent )
{
	int team, cls;
	if ( cent && cent->currentState.eType == ET_CORPSE )
	{
		return ( BG_GetPlayerClassInfo( cent->currentState.modelindex, cent->currentState.modelindex2) );
	}

	if ( cent && cent->currentState.powerups & (1 << PW_OPS_DISGUISED) )
	{
		team = ci->team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;

		// rain - fixed incorrect class determination (was & 6,
		// should be & 7)
		cls = ( cent->currentState.powerups >> PW_OPS_CLASS_1 ) & 7;
		return ( BG_GetPlayerClassInfo( team, cls) );
	}

	return ( BG_GetPlayerClassInfo( ci->team, ci->cls) );
}


/*
===============
CG_SetLerpFrameAnimation

may include ANIM_TOGGLEBIT
===============
*/
static void
CG_SetLerpFrameAnimation ( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation )
{
	animation_t		*anim;
	bg_character_t	*character = CG_CharacterForClientinfo( ci, cent );
	if ( !character )
	{
		return;
	}

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;
	if ( newAnimation < 0 || newAnimation >= character->animModelInfo->numAnimations )
	{
		CG_Error( "CG_SetLerpFrameAnimation: Bad animation number: %i", newAnimation );
	}

	anim = character->animModelInfo->animations[newAnimation];
	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;
	if ( cg_debugAnim.integer == 1 )
	{
		CG_Printf( "Anim: %i, %s\n", newAnimation, character->animModelInfo->animations[newAnimation]->name );
	}
}


/*
===============
CG_RunLerpFrame

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
void
CG_RunLerpFrame ( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, float speedScale )
{
	int			f;
	animation_t *anim;

	// debugging tool to get no animations
	if ( cg_animSpeed.integer == 0 )
	{
		lf->oldFrame = lf->frame = lf->backlerp = 0;
		return;
	}

	// see if the animation sequence is switching
	if ( ci && (newAnimation != lf->animationNumber || !lf->animation) )
	{
		CG_SetLerpFrameAnimation( cent, ci, lf, newAnimation );
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if ( cg.time >= lf->frameTime )
	{
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		// get the next frame based on the animation
		anim = lf->animation;
		if ( !anim->frameLerp )
		{
			return;								// shouldn't happen
		}

		if ( cg.time < lf->animationTime )
		{
			lf->frameTime = lf->animationTime;	// initial lerp
		}
		else
		{
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;
		}

		f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		f *= speedScale;						// adjust for haste, etc
		if ( f >= anim->numFrames )
		{
			f -= anim->numFrames;
			if ( anim->loopFrames )
			{
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			}
			else
			{
				f = anim->numFrames - 1;

				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}

		lf->frame = anim->firstFrame + f;
		lf->frameModel = anim->mdxFile;
		if ( cg.time > lf->frameTime )
		{
			lf->frameTime = cg.time;
			if ( cg_debugAnim.integer )
			{
				CG_Printf( "Clamp lf->frameTime\n" );
			}
		}
	}

	if ( lf->frameTime > cg.time + 200 )
	{
		lf->frameTime = cg.time;
	}

	if ( lf->oldFrameTime > cg.time )
	{
		lf->oldFrameTime = cg.time;
	}

	// calculate current lerp value
	if ( lf->frameTime == lf->oldFrameTime )
	{
		lf->backlerp = 0;
	}
	else
	{
		lf->backlerp = 1.0 - (float) ( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}


/*
===============
CG_ClearLerpFrame
===============
*/
static void
CG_ClearLerpFrame ( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int animationNumber )
{
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetLerpFrameAnimation( cent, ci, lf, animationNumber );
	if ( lf->animation )
	{
		lf->oldFrame = lf->frame = lf->animation->firstFrame;
		lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
	}
}


/*
===============
CG_SetLerpFrameAnimationRate

may include ANIM_TOGGLEBIT
===============
*/
void
CG_SetLerpFrameAnimationRate ( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation )
{
	animation_t		*anim, *oldanim;
	int				transitionMin = -1, oldAnimTime, oldAnimNum;
	qboolean		firstAnim = qfalse;
	bg_character_t	*character = CG_CharacterForClientinfo( ci, cent );
	if ( !character )
	{
		return;
	}

	oldAnimTime = lf->animationTime;
	oldanim = lf->animation;
	oldAnimNum = lf->animationNumber;
	if ( !lf->animation )
	{
		firstAnim = qtrue;
	}

	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;
	if ( newAnimation < 0 || newAnimation >= character->animModelInfo->numAnimations )
	{
		CG_Error( "CG_SetLerpFrameAnimationRate: Bad animation number: %i", newAnimation );
	}

	anim = character->animModelInfo->animations[newAnimation];
	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;
	if ( !(anim->flags & ANIMFL_FIRINGANIM) || (lf != &cent->pe.torso) )
	{
		if
		(
			(lf == &cent->pe.legs) &&
			(
				CG_IsCrouchingAnim( character->animModelInfo, newAnimation) != CG_IsCrouchingAnim( character->animModelInfo,
																								   oldAnimNum)
			)
		)
		{
			if ( anim->moveSpeed || (anim->movetype & ((1 << ANIM_MT_TURNLEFT) | (1 << ANIM_MT_TURNRIGHT))) )
			{											// if unknown movetype, go there faster
				transitionMin = lf->frameTime + 200;	// slowly raise/drop
			}
			else
			{
				transitionMin = lf->frameTime + 350;	// slowly raise/drop
			}
		}
		else if ( anim->moveSpeed )
		{
			transitionMin = lf->frameTime + 120;		// always do some lerping (?)
		}
		else
		{												// not moving, so take your time
			transitionMin = lf->frameTime + 170;		// always do some lerping (?)
		}

		if ( oldanim && oldanim->animBlend )
		{												//transitionMin < lf->frameTime + oldanim->animBlend) {
			transitionMin = lf->frameTime + oldanim->animBlend;
			lf->animationTime = transitionMin;
		}
		else
		{

			// slow down transitions according to speed
			if ( anim->moveSpeed && lf->animSpeedScale < 1.0 )
			{
				lf->animationTime += anim->initialLerp;
			}

			if ( lf->animationTime < transitionMin )
			{
				lf->animationTime = transitionMin;
			}
		}
	}

	// if first anim, go immediately
	if ( firstAnim )
	{
		lf->frameTime = cg.time - 1;
		lf->animationTime = cg.time - 1;
		lf->frame = anim->firstFrame;
		lf->frameModel = anim->mdxFile;
	}

	if ( cg_debugAnim.integer == 1 )
	{													// DHM - Nerve :: extra debug info
		CG_Printf( "Anim: %i, %s\n", newAnimation, character->animModelInfo->animations[newAnimation]->name );
	}
}


/*
===============
CG_RunLerpFrameRate

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
void
CG_RunLerpFrameRate ( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, centity_t *cent, int recursion )
{
	int			f;
	animation_t *anim, *oldAnim;
	animation_t *otherAnim = NULL;
	qboolean	isLadderAnim;
#define ANIM_SCALEMAX_LOW	1.1
#define ANIM_SCALEMAX_HIGH	1.6
#define ANIM_SPEEDMAX_LOW	100
#define ANIM_SPEEDMAX_HIGH	20

	// debugging tool to get no animations
	if ( cg_animSpeed.integer == 0 )
	{
		lf->oldFrame = lf->frame = lf->backlerp = 0;
		return;
	}

	isLadderAnim = lf->animation && ( lf->animation->flags & ANIMFL_LADDERANIM );
	oldAnim = lf->animation;

	// see if the animation sequence is switching
	if ( newAnimation != lf->animationNumber || !lf->animation )
	{
		CG_SetLerpFrameAnimationRate( cent, ci, lf, newAnimation );
	}

	// Ridah, make sure the animation speed is updated when possible
	anim = lf->animation;

	// check for forcing last frame
	if (	/*cent->currentState.eFlags & EF_FORCE_END_FRAME */
			// xkan, 12/27/2002 - In SP, corpse also stays at the last frame (of the death animation)
			// so that the death animation can end up in different positions
			// and the body will stay in that position
			/*||*/
				( /*CG_IsSinglePlayer() &&*/ cent->currentState.eType == ET_CORPSE)
		)
	{
		lf->oldFrame = lf->frame = anim->firstFrame + anim->numFrames - 1;
		lf->oldFrameModel = lf->frameModel = anim->mdxFile;
		lf->backlerp = 0;
		return;
	}

	if ( anim->moveSpeed && lf->oldFrameSnapshotTime )
	{
		float	moveSpeed;

		// calculate the speed at which we moved over the last frame
		if ( cg.latestSnapshotTime != lf->oldFrameSnapshotTime && cg.nextSnap )
		{
			if ( cent->currentState.number == cg.snap->ps.clientNum )
			{
				if ( isLadderAnim )
				{								// only use Z axis for speed
					lf->oldFramePos[0] = cent->lerpOrigin[0];
					lf->oldFramePos[1] = cent->lerpOrigin[1];
				}
				else
				{								// only use x/y axis
					lf->oldFramePos[2] = cent->lerpOrigin[2];
				}

				moveSpeed = Distance( cent->lerpOrigin, lf->oldFramePos ) / ( (float) (cg.time - lf->oldFrameTime) / 1000.0 );
			}
			else
			{
				if ( isLadderAnim )
				{								// only use Z axis for speed
					lf->oldFramePos[0] = cent->currentState.pos.trBase[0];
					lf->oldFramePos[1] = cent->currentState.pos.trBase[1];
				}

				moveSpeed = Distance( cent->lerpOrigin, lf->oldFramePos ) / ( (float) (cg.time - lf->oldFrameTime) / 1000.0 );
			}

			//
			// convert it to a factor of this animation's movespeed
			lf->animSpeedScale = moveSpeed / (float) anim->moveSpeed;
			lf->oldFrameSnapshotTime = cg.latestSnapshotTime;
		}
	}
	else
	{

		// move at normal speed
		lf->animSpeedScale = 1.0;
		lf->oldFrameSnapshotTime = cg.latestSnapshotTime;
	}

	// adjust with manual setting (pain anims)
	lf->animSpeedScale *= cent->pe.animSpeed;

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if ( cg.time >= lf->frameTime )
	{
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;
		VectorCopy( cent->lerpOrigin, lf->oldFramePos );

		// restrict the speed range
		if ( lf->animSpeedScale < 0.25 )
		{										// if it's too slow, then a really slow spped, combined with a sudden take-off, can leave them playing a really slow frame while they a moving really fast
			if ( lf->animSpeedScale < 0.01 && isLadderAnim )
			{
				lf->animSpeedScale = 0.0;
			}
			else
			{
				lf->animSpeedScale = 0.25;
			}
		}
		else if ( lf->animSpeedScale > ANIM_SCALEMAX_LOW )
		{
			if ( !(anim->flags & ANIMFL_LADDERANIM) )
			{

				// allow slower anims to speed up more than faster anims
				if ( anim->moveSpeed > ANIM_SPEEDMAX_LOW )
				{
					lf->animSpeedScale = ANIM_SCALEMAX_LOW;
				}
				else if ( anim->moveSpeed < ANIM_SPEEDMAX_HIGH )
				{
					if ( lf->animSpeedScale > ANIM_SCALEMAX_HIGH )
					{
						lf->animSpeedScale = ANIM_SCALEMAX_HIGH;
					}
				}
				else
				{
					lf->animSpeedScale = ANIM_SCALEMAX_HIGH - ( ANIM_SCALEMAX_HIGH - ANIM_SCALEMAX_LOW ) * (float) ( anim->moveSpeed - ANIM_SPEEDMAX_HIGH ) / (float) ( ANIM_SPEEDMAX_LOW - ANIM_SPEEDMAX_HIGH );
				}
			}
			else if ( lf->animSpeedScale > 4.0 )
			{
				lf->animSpeedScale = 4.0;
			}
		}

		if ( lf == &cent->pe.legs )
		{
			otherAnim = cent->pe.torso.animation;
		}
		else if ( lf == &cent->pe.torso )
		{
			otherAnim = cent->pe.legs.animation;
		}

		// get the next frame based on the animation
		if ( !lf->animSpeedScale )
		{

			// stopped on the ladder, so stay on the same frame
			f = lf->frame - anim->firstFrame;
			lf->frameTime += anim->frameLerp;	// don't wait too long before starting to move again
		}
		else if
			(
				lf->oldAnimationNumber != lf->animationNumber &&
				(
					!anim->moveSpeed ||
					lf->oldFrame < anim->firstFrame ||
					lf->oldFrame >= anim->firstFrame +
					anim->numFrames
				)
			)
		{										// Ridah, added this so walking frames don't always get reset to 0, which can happen in the middle of a walking anim, which looks wierd
			lf->frameTime = lf->animationTime;	// initial lerp
			if ( oldAnim && anim->moveSpeed )
			{									// keep locomotions going continuously
				f = ( lf->frame - oldAnim->firstFrame ) + 1;
				while ( f < 0 )
				{
					f += anim->numFrames;
				}
			}
			else
			{
				f = 0;
			}
		}
		else if
			(
				(lf == &cent->pe.legs) &&
				otherAnim &&
				!(anim->flags & ANIMFL_FIRINGANIM) &&
				((lf->animationNumber &~ANIM_TOGGLEBIT) == (cent->pe.torso.animationNumber &~ANIM_TOGGLEBIT)) &&
				(!anim->moveSpeed)
			)
		{

			// legs should synch with torso
			f = cent->pe.torso.frame - otherAnim->firstFrame;
			if ( f >= anim->numFrames || f < 0 )
			{
				f = 0;							// wait at the start for the legs to catch up (assuming they are still in an old anim)
			}

			lf->frameTime = cent->pe.torso.frameTime;
		}
		else if
			(
				(lf == &cent->pe.torso) &&
				otherAnim &&
				!(anim->flags & ANIMFL_FIRINGANIM) &&
				((lf->animationNumber &~ANIM_TOGGLEBIT) == (cent->pe.legs.animationNumber &~ANIM_TOGGLEBIT)) &&
				(otherAnim->moveSpeed)
			)
		{

			// torso needs to sync with legs
			f = cent->pe.legs.frame - otherAnim->firstFrame;
			if ( f >= anim->numFrames || f < 0 )
			{
				f = 0;							// wait at the start for the legs to catch up (assuming they are still in an old anim)
			}

			lf->frameTime = cent->pe.legs.frameTime;
		}
		else
		{
			lf->frameTime = lf->oldFrameTime + (int) ( (float) anim->frameLerp * (1.0 / lf->animSpeedScale) );
			if ( lf->frameTime < cg.time )
			{
				lf->frameTime = cg.time;
			}

			// check for skipping frames (eg. death anims play in slo-mo if low framerate)
			if ( anim->flags & ANIMFL_REVERSED )
			{
				if ( cg.time > lf->frameTime && !anim->moveSpeed )
				{
					f = ( anim->numFrames - 1 ) - ( (lf->frame - anim->firstFrame) - (1 + (cg.time - lf->frameTime) / anim->frameLerp) );
				}
				else
				{
					f = ( anim->numFrames - 1 ) - ( (lf->frame - anim->firstFrame) - 1 );
				}
			}
			else
			{
				if ( cg.time > lf->frameTime && !anim->moveSpeed )
				{
					f = ( lf->frame - anim->firstFrame ) + 1 + ( cg.time - lf->frameTime ) / anim->frameLerp;
				}
				else
				{
					f = ( lf->frame - anim->firstFrame ) + 1;
				}
			}

			if ( f < 0 )
			{
				f = 0;
			}
		}

		//f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		if ( f >= anim->numFrames )
		{
			f -= anim->numFrames;
			if ( anim->loopFrames )
			{
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			}
			else
			{
				f = anim->numFrames - 1;

				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}

		if ( anim->flags & ANIMFL_REVERSED )
		{
			lf->frame = anim->firstFrame + anim->numFrames - 1 - f;
			lf->frameModel = anim->mdxFile;
		}
		else
		{
			lf->frame = anim->firstFrame + f;
			lf->frameModel = anim->mdxFile;
		}

		if ( cg.time > lf->frameTime )
		{

			// Ridah, run the frame again until we move ahead of the current time, fixes walking speeds for zombie
			if ( /*!anim->moveSpeed ||*/ recursion > 4 )
			{
				lf->frameTime = cg.time;
			}
			else
			{
				CG_RunLerpFrameRate( ci, lf, newAnimation, cent, recursion + 1 );
			}

			if ( cg_debugAnim.integer > 3 )
			{
				CG_Printf( "Clamp lf->frameTime\n" );
			}
		}

		lf->oldAnimationNumber = lf->animationNumber;
	}

	// Gordon: BIG hack, occaisionaly (VERY occaisionally), the frametime gets totally wacked
	if ( lf->frameTime > cg.time + 5000 )
	{
		lf->frameTime = cg.time;
	}

	if ( lf->oldFrameTime > cg.time )
	{
		lf->oldFrameTime = cg.time;
	}

	// calculate current lerp value
	if ( lf->frameTime == lf->oldFrameTime )
	{
		lf->backlerp = 0;
	}
	else
	{
		lf->backlerp = 1.0 - (float) ( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}


/*
===============
CG_ClearLerpFrameRate
===============
*/
void
CG_ClearLerpFrameRate ( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int animationNumber )
{
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetLerpFrameAnimationRate( cent, ci, lf, animationNumber );
	if ( lf->animation )
	{
		lf->oldFrame = lf->frame = lf->animation->firstFrame;
		lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
	}
}


/*
===============
CG_PlayerAnimation
===============
*/
static void
CG_PlayerAnimation ( centity_t *cent, refEntity_t *body )
{
	clientInfo_t	*ci;
	int				clientNum;
	int				animIndex, tempIndex;
	bg_character_t	*character;
	clientNum = cent->currentState.clientNum;
	ci = &cgs.clientinfo[clientNum];
	character = CG_CharacterForClientinfo( ci, cent );
	if ( !character )
	{
		return;
	}

	if ( cg_noPlayerAnims.integer )
	{
		body->frame = body->oldframe = body->torsoFrame = body->oldTorsoFrame = 0;
		body->frameModel = body->oldframeModel = body->torsoFrameModel = body->oldTorsoFrameModel = character->animModelInfo->animations[0]->mdxFile;
		return;
	}

	// default to whatever the legs are currently doing
	animIndex = cent->currentState.legsAnim;

	// do the shuffle turn frames locally
	if ( !(cent->currentState.eFlags & EF_DEAD) && cent->pe.legs.yawing )
	{

		//CG_Printf("turn: %i\n", cg.time );
		tempIndex = BG_GetAnimScriptAnimation( clientNum, character->animModelInfo, cent->currentState.aiState,
											   (cent->pe.legs.yawing == SWING_RIGHT ? ANIM_MT_TURNRIGHT : ANIM_MT_TURNLEFT) );
		if ( tempIndex > -1 )
		{
			animIndex = tempIndex;
		}
	}

	// run the animation
	CG_RunLerpFrameRate( ci, &cent->pe.legs, animIndex, cent, 0 );
	body->oldframe = cent->pe.legs.oldFrame;
	body->frame = cent->pe.legs.frame;
	body->backlerp = cent->pe.legs.backlerp;
	body->frameModel = cent->pe.legs.frameModel;
	body->oldframeModel = cent->pe.legs.oldFrameModel;
	CG_RunLerpFrameRate( ci, &cent->pe.torso, cent->currentState.torsoAnim, cent, 0 );
	body->oldTorsoFrame = cent->pe.torso.oldFrame;
	body->torsoFrame = cent->pe.torso.frame;
	body->torsoBacklerp = cent->pe.torso.backlerp;
	body->torsoFrameModel = cent->pe.torso.frameModel;
	body->oldTorsoFrameModel = cent->pe.torso.oldFrameModel;
}

/*
=============================================================================

PLAYER ANGLES

=============================================================================
*/


/*
==================
CG_SwingAngles
==================
*/
static void
CG_SwingAngles
(
	float		destination,
	float		swingTolerance,
	float		clampTolerance,
	float		speed,
	float		*angle,
	qboolean	*swinging
)
{
	float	swing;
	float	move;
	float	scale;
	if ( !*swinging )
	{

		// see if a swing should be started
		swing = AngleSubtract( *angle, destination );
		if ( swing > swingTolerance || swing < -swingTolerance )
		{
			*swinging = qtrue;
		}
	}

	if ( !*swinging )
	{
		return;
	}

	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract( destination, *angle );
	scale = fabs( swing );
	scale *= 0.05;
	if ( scale < 0.5 )
	{
		scale = 0.5;
	}

	// swing towards the destination angle
	if ( swing >= 0 )
	{
		move = cg.frametime * scale * speed;
		if ( move >= swing )
		{
			move = swing;
			*swinging = qfalse;
		}
		else
		{
			*swinging = SWING_LEFT;		// left
		}

		*angle = AngleMod( *angle + move );
	}
	else if ( swing < 0 )
	{
		move = cg.frametime * scale * -speed;
		if ( move <= swing )
		{
			move = swing;
			*swinging = qfalse;
		}
		else
		{
			*swinging = SWING_RIGHT;	// right
		}

		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance )
	{
		*angle = AngleMod( destination - (clampTolerance - 1) );
	}
	else if ( swing < -clampTolerance )
	{
		*angle = AngleMod( destination + (clampTolerance - 1) );
	}
}


/*
=================
CG_AddPainTwitch
=================
*/
static void
CG_AddPainTwitch ( centity_t *cent, vec3_t torsoAngles )
{
	int		t;
	float	f;
	int		duration;
	float	direction;
	if ( !cent->pe.animSpeed )
	{

		// we need to inititialize this stuff
		cent->pe.painAnimLegs = -1;
		cent->pe.painAnimTorso = -1;
		cent->pe.animSpeed = 1.0;
	}

	if ( cent->currentState.eFlags & EF_DEAD )
	{
		cent->pe.painAnimLegs = -1;
		cent->pe.painAnimTorso = -1;
		cent->pe.animSpeed = 1.0;
		return;
	}

	if ( cent->pe.painDuration )
	{
		duration = cent->pe.painDuration;
	}
	else
	{
		duration = PAIN_TWITCH_TIME;
	}

	direction = (float) duration * 0.085;
	if ( direction > 30 )
	{
		direction = 30;
	}

	if ( direction < 10 )
	{
		direction = 10;
	}

	direction *= (float) ( cent->pe.painDirection * 2 ) - 1;
	t = cg.time - cent->pe.painTime;
	if ( t >= duration )
	{
		return;
	}

	f = 1.0 - (float) t / duration;
	if ( cent->pe.painDirection )
	{
		torsoAngles[ROLL] += 20 * f;
	}
	else
	{
		torsoAngles[ROLL] -= 20 * f;
	}
}


/*
=================
CG_AddLeanTwitch	// hope it will help
=================
*/
static void
CG_AddLeanTwitch ( centity_t *cent, vec3_t torsoAngles, vec3_t headAngles )
{

/*	if ( cent->currentState.eFlags & EF_PRONE )
	{
		if ( cent->pe.leanDirection < 0 )
		{
			torsoAngles[ROLL] += ( cent->currentState.constantLight / 1000 ) * 4.0;
			headAngles[ROLL] += ( cent->currentState.constantLight / 1000 ) * 4.0;
		}
		else
		{
			torsoAngles[ROLL] += ( cent->currentState.constantLight / 1000 ) * 2.3;
			headAngles[ROLL] += ( cent->currentState.constantLight / 1000 ) * 2.3;
		}
	}
	else
	{*/
		torsoAngles[ROLL] += ( cent->currentState.constantLight / 1000 ) * 2.3;
		headAngles[ROLL] += ( cent->currentState.constantLight / 1000 ) * 2.3;
//	}
}

/*
=========================================
		CG_tank_slope -- @fixme should be in pmove
=========================================
*/
#define POS_FW	0
#define POS_BC	1
#define POS_L	2
#define POS_R	3
#define POS_MAX 4


/*			tankmaxs
	 _____/
	|  0  |
    |     |
    |2 O 3|   -- tankmins[1]+tankmaxs[1]
	|     |
	|__1__|
   /   
tankmins
*/
void
CG_tank_slope ( centity_t *cent, vec3_t slopeangles, int yaw )
{
	int		i;
	trace_t trace;
	vec3_t	forward, right, up, start, end;
	vec3_t	dir, angles;
	vec3_t	testpos[POS_MAX];
	vec3_t	testangles;
	vec3_t	boxMins = { -8, -8, -8 };		// @fixme , tune this to be more smooth on delailed terrian (eg. railroad )
	vec3_t	boxMaxs = { 8, 8, 8 };
	float	pitch, roll;

	//	qboolean inAir=qfalse;
	testangles[0] = testangles[2] = 0;
	testangles[1] = yaw;
	AngleVectors( testangles, forward, right, up );
	for ( i = 0; i < POS_MAX; i++ )
	{
		float	fw, rt;
		fw = 32;							//tankMins[0]+tankMaxs[0]*0.5; // fixme later i have to be sure now
		rt = 16;							//tankMins[1]+tankMaxs[1]*0.5;
		VectorCopy( cent->lerpOrigin, start );
		switch ( i )
		{
			case POS_FW:
				VectorMA( start, fw, forward, start );
				break;
			case POS_BC:
				VectorMA( start, -fw, forward, start );
				break;
			case POS_L:
				VectorMA( start, -rt, right, start );
				break;
			case POS_R:
				VectorMA( start, rt, right, start );
				break;
		}

		VectorMA( start, -32, up, end );
		VectorMA( start, 16, up, start );	// raise the start pos
		trap_CM_BoxTrace( &trace, start, end, boxMins, boxMaxs, 0, MASK_PLAYERSOLID );
		VectorCopy( trace.endpos, testpos[i] );
	}

	// inair check

	/*	if ( cent->currentState.groundEntityNum == ENTITYNUM_NONE ) // note if we use point 0 it feels bad
		inAir = qtrue;*/

	// end
	VectorSubtract( testpos[0], testpos[1], dir );
	vectoangles( dir, angles );

	/*	if ( inAir && !cent->pe.lastSlopeInit ){
		cent->pe.lastSlopeTime = cg.time;
		cent->pe.lastSlopeInit = qtrue;
	} else if ( inAir ) { 
		cent->pe.lastSlope = 0.06*(cg.time - cent->pe.lastSlopeTime); // fixme? 
		if ( cent->pe.lastSlope > 45 ) // clamp it
			cent->pe.lastSlope = 45;
	} else if ( !inAir && cent->pe.lastSlopeInit ) { // bump a bit
		cent->pe.lastSlopeTime = cg.time;
		cent->pe.lastSlopeInit = qfalse;
	} else if ( cg.time - cent->pe.lastSlopeTime < 500 && !cent->pe.lastSlopeInit  ) { 
		cent->pe.lastSlope = -0.05*(cg.time - cent->pe.lastSlopeTime);
	} else if ( cent->pe.lastSlope != 0 ) { // now just go back
		if (cg.time - cent->pe.lastSlopeTime < 1000 && cent->pe.lastSlope < -0.05 )
		cent->pe.lastSlope += 0.05;
		else 
		cent->pe.lastSlope = 0;
		cent->pe.lastSlopeInit = qfalse;
	}
 */
	pitch = angles[0];						//+cent->pe.lastSlope;
	VectorSubtract( testpos[3], testpos[2], dir );
	vectoangles( dir, angles );
	roll = angles[0];
	slopeangles[0] = pitch;
	slopeangles[1] = 0;
	slopeangles[2] = roll;

	//	CG_Printf("Angles computed: %f %f %f \n" , slopeangles[0], slopeangles[1], slopeangles[2]);
}


/* */
void
CG_player_slope ( centity_t *cent, vec3_t slopeangles )
{
	int		i;
	trace_t trace;
	vec3_t	forward, right, up, start, end;
	vec3_t	dir, angles;
	vec3_t	testpos[POS_MAX];
	vec3_t	testangles;
	vec3_t	boxMins = { -8, -8, -8 };
	vec3_t	boxMaxs = { 8, 8, 8 };
	float	pitch, roll;

	// do not take player viewangle in mind
	testangles[0] = testangles[2] = 0;
	testangles[1] = cent->lerpAngles[1];
	AngleVectors( testangles, forward, right, up );
	for ( i = 0; i < POS_MAX; i++ )
	{
		float	fw, rt;
		fw = 16;
		rt = 8;
		VectorCopy( cent->lerpOrigin, start );
		switch ( i )
		{
			case POS_FW:
				VectorMA( start, fw, forward, start );
				break;
			case POS_BC:
				VectorMA( start, -fw, forward, start );
				break;
			case POS_L:
				VectorMA( start, -rt, right, start );
				break;
			case POS_R:
				VectorMA( start, rt, right, start );
				break;
		}

		VectorMA( start, -32, up, end );
		VectorMA( start, 16, up, start );	// raise the start pos
		trap_CM_BoxTrace( &trace, start, end, boxMins, boxMaxs, 0, MASK_PLAYERSOLID );
		VectorCopy( trace.endpos, testpos[i] );
	}

	VectorSubtract( testpos[0], testpos[1], dir );
	vectoangles( dir, angles );
	pitch = angles[0];
	VectorSubtract( testpos[3], testpos[2], dir );
	vectoangles( dir, angles );
	roll = angles[0];
	slopeangles[0] = pitch;
	slopeangles[1] = 0;
	slopeangles[2] = roll;
}


/*
===============
CG_PlayerAngles

Handles seperate torso motion

  legs pivot based on direction of movement

  head always looks exactly at cent->lerpAngles

  if motion < 20 degrees, show in head only
  if < 45 degrees, also show in torso
===============
*/
static void
CG_PlayerAngles ( centity_t *cent, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] )
{
	vec3_t			legsAngles, torsoAngles, headAngles;
	float			dest;
	vec3_t			velocity;
	float			speed;
	float			clampTolerance;
	int				legsSet, torsoSet;
	clientInfo_t	*ci;
	bg_character_t	*character;
	ci = &cgs.clientinfo[cent->currentState.clientNum];
	character = CG_CharacterForClientinfo( ci, cent );
	if ( !character )
	{
		return;
	}

	legsSet = cent->currentState.legsAnim &~ANIM_TOGGLEBIT;
	torsoSet = cent->currentState.torsoAnim &~ANIM_TOGGLEBIT;
	VectorCopy( cent->lerpAngles, headAngles );
	headAngles[YAW] = AngleMod( headAngles[YAW] );
	VectorClear( legsAngles );
	VectorClear( torsoAngles );

	// --------- yaw -------------
	// allow yaw to drift a bit, unless these conditions don't allow them
	if
	(
		!(
			BG_GetConditionBitFlag( cent->currentState.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE) ||
			BG_GetConditionBitFlag( cent->currentState.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLECR)
		)										/*
		||	 (BG_GetConditionValue( cent->currentState.clientNum, ANIM_COND_MOVETYPE, qfalse ) & ((1<<ANIM_MT_STRAFELEFT) | (1<<ANIM_MT_STRAFERIGHT)) )*/
	)
	{

		// always point all in the same direction
		cent->pe.torso.yawing = qtrue;			// always center
		cent->pe.torso.pitching = qtrue;		// always center
		cent->pe.legs.yawing = qtrue;			// always center
		// if firing, make sure torso and head are always aligned
	}
	else if ( BG_GetConditionValue( cent->currentState.clientNum, ANIM_COND_FIRING, qtrue) )
	{
		cent->pe.torso.yawing = qtrue;			// always center
		cent->pe.torso.pitching = qtrue;		// always center
	}

	// adjust legs for movement dir
	if ( cent->currentState.eFlags & EF_DEAD || cent->currentState.eFlags & EF_MOUNTEDTANK )
	{

		// don't let dead bodies twitch
		legsAngles[YAW] = headAngles[YAW];
		torsoAngles[YAW] = headAngles[YAW];
	}
	else
	{
		legsAngles[YAW] = headAngles[YAW] + cent->currentState.angles2[YAW];
		if ( !(cent->currentState.eFlags & EF_FIRING) )
		{										// aha this causes rotation delay
			torsoAngles[YAW] = headAngles[YAW] + 0.35 * cent->currentState.angles2[YAW];
			clampTolerance = 90;
		}
		else
		{										// must be firing
			torsoAngles[YAW] = headAngles[YAW]; // always face firing direction

			//if (fabs(cent->currentState.angles2[YAW]) > 30)
			//	legsAngles[YAW] = headAngles[YAW];
			clampTolerance = 60;
		}

		if ( cent->currentState.eFlags & EF_TANK )
		{
			CG_SwingAngles( torsoAngles[YAW], 0, clampTolerance, 0.5 * cg_swingSpeed.value, &cent->pe.torso.yawAngle,
							&cent->pe.torso.yawing );
			CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, 0.5 * cg_swingSpeed.value, &cent->pe.legs.yawAngle,
							&cent->pe.legs.yawing );
			torsoAngles[YAW] = cent->pe.torso.yawAngle;
			legsAngles[YAW] = cent->pe.legs.yawAngle;
		}
		else
		{

			// torso
			CG_SwingAngles( torsoAngles[YAW], 25, clampTolerance, cg_swingSpeed.value, &cent->pe.torso.yawAngle,
							&cent->pe.torso.yawing );

			// if the legs are yawing (facing heading direction), allow them to rotate a bit, so we don't keep calling
			// the legs_turn animation while an AI is firing, and therefore his angles will be randomizing according to their accuracy
			clampTolerance = 150;
			if ( BG_GetConditionBitFlag( ci->clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE) )
			{
				cent->pe.legs.yawing = qfalse;	// set it if they really need to swing
				CG_SwingAngles( legsAngles[YAW], 20, clampTolerance, 0.5 * cg_swingSpeed.value, &cent->pe.legs.yawAngle,
								&cent->pe.legs.yawing );
			}
			else if ( strstr( BG_GetAnimString( character->animModelInfo, legsSet), "strafe") )
			{

				// FIXME: what is this strstr hack??
				//if	( BG_GetConditionValue( ci->clientNum, ANIM_COND_MOVETYPE, qfalse ) & ((1<<ANIM_MT_STRAFERIGHT)|(1<<ANIM_MT_STRAFELEFT)) )
				cent->pe.legs.yawing = qfalse;	// set it if they really need to swing
				legsAngles[YAW] = headAngles[YAW];
				CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, cg_swingSpeed.value, &cent->pe.legs.yawAngle,
								&cent->pe.legs.yawing );
			}
			else if ( cent->pe.legs.yawing )
			{
				CG_SwingAngles( legsAngles[YAW], 0, clampTolerance, cg_swingSpeed.value, &cent->pe.legs.yawAngle,
								&cent->pe.legs.yawing );
			}
			else
			{
				CG_SwingAngles( legsAngles[YAW], 40, clampTolerance, cg_swingSpeed.value, &cent->pe.legs.yawAngle,
								&cent->pe.legs.yawing );
			}

			torsoAngles[YAW] = cent->pe.torso.yawAngle;
			legsAngles[YAW] = cent->pe.legs.yawAngle;
		}
	}											// tank

	// --------- pitch -------------
	// only show a fraction of the pitch angle in the torso
	if ( headAngles[PITCH] > 180 )
	{
		dest = ( -360 + headAngles[PITCH] ) * 0.75;
	}
	else
	{
		dest = headAngles[PITCH] * 0.75;
	}

	//CG_SwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
	//torsoAngles[PITCH] = cent->pe.torso.pitchAngle;
	if ( cent->currentState.eFlags & EF_PRONE )
	{
		torsoAngles[PITCH] = legsAngles[PITCH] - 3;
	}
	else
	{
		CG_SwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
		torsoAngles[PITCH] = cent->pe.torso.pitchAngle;
	}

	// --------- roll -------------
	// lean towards the direction of travel
	VectorCopy( cent->currentState.pos.trDelta, velocity );
	speed = VectorNormalize( velocity );
	if ( speed )
	{
		vec3_t	axis[3];
		float	side;
		if ( !(cent->currentState.eFlags & EF_TANK) )
		{
			speed *= 0.05;
		}
		else
		{
			speed *= 0.02;
		}

		AnglesToAxis( legsAngles, axis );
		side = speed * DotProduct( velocity, axis[1] );
		legsAngles[ROLL] -= side;
		side = speed * DotProduct( velocity, axis[0] );
		legsAngles[PITCH] += side;
	}

	if ( cent->currentState.eFlags & EF_TANK )
	{
		vec3_t	tankslope;
		CG_tank_slope( cent, tankslope, cent->lerpAngles[1] );
		torsoAngles[0] = headAngles[0] = legsAngles[0] = tankslope[0];
		torsoAngles[2] = headAngles[2] = legsAngles[2] = tankslope[2];
	}
	else if
		(
			cent->currentState.eFlags & EF_PRONE ||
			cent->currentState.eFlags & EF_DEAD ||
			cent->currentState.eType == ET_CORPSE
		)
	{											// FIXME do for prone as well
		vec3_t	slope;
		CG_player_slope( cent, slope );
		torsoAngles[0] += slope[0];
		headAngles[0] += slope[0];
		legsAngles[0] += slope[0];
		torsoAngles[2] += slope[2];
		headAngles[2] += slope[2];
		legsAngles[2] += slope[2];
	}

	if ( !(cent->currentState.eFlags & EF_TANK) )
	{

		// pain twitch
		CG_AddPainTwitch( cent, torsoAngles );

		// lean bend
		CG_AddLeanTwitch( cent, torsoAngles, headAngles );
	}

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
}


/*
==============
CG_BreathPuffs
==============
*/
static void
CG_BreathPuffs ( centity_t *cent, refEntity_t *head )
{
	clientInfo_t	*ci;
	vec3_t			up, forward;
	int				contents;
	vec3_t			mang, morg, maxis[3];

	//float a;
	float			r;
	ci = &cgs.clientinfo[cent->currentState.number];
	if ( !cg_enableBreath.integer )
	{
		return;
	}

	/*	if( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson ) {
		return;
	}*/

	// allow cg_enableBreath to force everyone to have breath
	if ( !(cent->currentState.eFlags & EF_BREATH) )
	{
		return;
	}

	contents = CG_PointContents( head->origin, 0 );
	if ( contents & (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA) )
	{
		return;
	}

	if ( ci->breathPuffTime > cg.time )
	{
		return;
	}

	CG_GetOriginForTag( cent, head, "tag_mouth", 0, morg, maxis );
	AxisToAngles( maxis, mang );
	AngleVectors( mang, forward, NULL, up );

	//push the origin out a tad so it's not right in the guys face (tad==4)
	VectorMA( morg, 4, forward, morg );
	forward[0] = up[0] * 8 + forward[0] * 5;
	forward[1] = up[1] * 8 + forward[1] * 5;
	forward[2] = up[2] * 8 + forward[2] * 5;
	r = random();
	CG_SmokePuff( morg, forward, r + 5, 1, 1, 1, 0.33f, 2000 + random() * 100, cg.time, cg.time + 400 + random() * 10,
				  0, cgs.media.smokeParticleShader );
	if ( !(cent->currentState.eFlags & EF_DEAD) )
	{
		ci->breathPuffTime = cg.time + 500 + random() * 1000;
	}
	else
	{
		ci->breathPuffTime = cg.time + 1000 + random() * 5000;
	}
}

/*
===============
CG_TrailItem
===============
*/

/*static void CG_TrailItem( centity_t *cent, qhandle_t hModel ) {
	refEntity_t		ent;
	vec3_t			angles;
	qboolean		ducking;

	// DHM - Nerve :: Don't draw icon above your own head
	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
		return;

	memset( &ent, 0, sizeof( ent ) );

	VectorCopy( cent->lerpAngles, angles );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
	AnglesToAxis( angles, ent.axis );

	// DHM - Nerve :: adjusted values
	VectorCopy( cent->lerpOrigin, ent.origin );

	// Account for ducking
	if ( cent->currentState.clientNum == cg.snap->ps.clientNum )
		ducking = (cg.snap->ps.pm_flags & PMF_DUCKED);
	else
		ducking = (qboolean)cent->currentState.animMovetype;

	if ( ducking )
		ent.origin[2] += 38;
	else
		ent.origin[2] += 56;

	ent.hModel = hModel;
	trap_R_AddRefEntityToScene( &ent );
}*/


/*
===============
CG_PlayerFloatSprite

Float a sprite over the player's head
DHM - Nerve :: added height parameter
===============
*/
static void
CG_PlayerFloatSprite ( centity_t *cent, qhandle_t shader, int height )
{
	int			rf;
	refEntity_t ent;
	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
	{
		rf = RF_THIRD_PERSON;	// only show in mirrors
	}
	else
	{
		rf = 0;
	}

	memset( &ent, 0, sizeof(ent) );
	VectorCopy( cent->lerpOrigin, ent.origin );
	ent.origin[2] += height;	// DHM - Nerve :: was '48'

	// Account for ducking
	if ( cent->currentState.clientNum == cg.snap->ps.clientNum )
	{
		if ( cg.snap->ps.pm_flags & PMF_DUCKED )
		{
			ent.origin[2] -= 18;
		}
	}
	else
	{
		if ( (qboolean) cent->currentState.animMovetype )
		{
			ent.origin[2] -= 18;
		}
	}

	ent.reType = RT_SPRITE;
	ent.customShader = shader;
	ent.radius = 6.66;
	ent.renderfx = rf;
	ent.shaderRGBA[0] = 255;
	ent.shaderRGBA[1] = 255;
	ent.shaderRGBA[2] = 255;
	ent.shaderRGBA[3] = 255;
	trap_R_AddRefEntityToScene( &ent );
}


/*
===============
CG_PlayerSprites

Float sprites over the player's head
===============
*/
static void
CG_PlayerSprites ( centity_t *cent )
{
	int				team;
	int				health;
	clientInfo_t	*ci = &cgs.clientinfo[cent->currentState.clientNum];
	if ( ci )
	{
		health = ci->health;
	}
	else
	{
		health = 0;
	}

#ifdef __BOT_COVERSPOT_DEBUG__
	if (cent->currentState.dl_intensity == 999)
	{// Moving to cover!
		CG_PlayerFloatSprite( cent, cgs.media.balloonShader, 56 );

		//cent->voiceChatSprite = cgs.media.voiceChatShader;
		//cent->voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer;
	}
	else if (cent->currentState.dl_intensity == 1000)
	{// At cover point!
		CG_PlayerFloatSprite( cent, cgs.media.voiceChatShader, 56 );

		//cent->voiceChatSprite = cgs.media.balloonShader;
		//cent->voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer;
	}
#endif //__BOT_COVERSPOT_DEBUG__

	if( cent->currentState.powerups & ( 1 << PW_REDFLAG ) ||
		cent->currentState.powerups & ( 1 << PW_BLUEFLAG ) ) {
		CG_PlayerFloatSprite( cent, cgs.media.objectiveShader, 56 );
		return;
	}

	if
	(
		(cent->currentState.powerups & (1 << PW_REDFLAG) || cent->currentState.powerups & (1 << PW_BLUEFLAG)) &&
		cgs.gametype != GT_WOLF_CTF
	)
	{
		CG_PlayerFloatSprite( cent, cgs.media.objectiveShader, 56 );
		return;
	}

	if ( cent->currentState.eFlags & EF_CONNECTION )
	{
		CG_PlayerFloatSprite( cent, cgs.media.disconnectIcon, 68 );
		return;
	}

	if ( cent->currentState.powerups & (1 << PW_INVULNERABLE) )
	{
		CG_PlayerFloatSprite( cent, cgs.media.spawnInvincibleShader, 56 );
		return;
	}

	team = cgs.clientinfo[cent->currentState.clientNum].team;

	// DHM - Nerve :: If this client is a medic, draw a 'revive' icon over
	//					dead players that are not in limbo yet.
	if
	(
		(cent->currentState.eFlags & EF_DEAD) &&
		cent->currentState.number == cent->currentState.clientNum &&
		health > FORCE_LIMBO_HEALTH &&
		cg.snap->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC &&
		(cg.snap->ps.ammoclip[BG_FindClipForWeapon( WP_MEDIC_SYRINGE)] > 0 || cg_realmedic.integer) &&
		cg.snap->ps.stats[STAT_HEALTH] > 0	//&& cg.snap->ps.persistant[PERS_TEAM] == team ) {
	)
	{
		if ( (cg.snap->ps.persistant[PERS_TEAM] == team) || (cent->currentState.powerups & (1 << PW_OPS_DISGUISED)) )
		{
			CG_PlayerFloatSprite( cent, cgs.media.medicReviveShader, 8 );
		}

		return;
	}

	// DHM - Nerve :: show voice chat signal so players know who's talking
	if ( cent->voiceChatSpriteTime > cg.time && cg.snap->ps.persistant[PERS_TEAM] == team )
	{
		CG_PlayerFloatSprite( cent, cent->voiceChatSprite, 56 );
		return;
	}

	// DHM - Nerve :: only show talk icon to team-mates
	if ( cent->currentState.eFlags & EF_TALK && cg.snap->ps.persistant[PERS_TEAM] == team )
	{
		CG_PlayerFloatSprite( cent, cgs.media.balloonShader, 48 );
		return;
	}
	{
		fireteamData_t	*ft;
		if ( (ft = CG_IsOnFireteam( cent->currentState.number)) )
		{
			if ( ft == CG_IsOnFireteam( cg.clientNum) && cgs.clientinfo[cent->currentState.number].selected )
			{
				CG_PlayerFloatSprite( cent, cgs.media.fireteamicons[ft->ident], 56 );
			}
		}
	}
}

/*
===============
CG_PlayerShadow

Returns the Z component of the surface being shadowed

  should it return a full plane instead of a Z?
===============
*/
#define SHADOW_DISTANCE 64
#define ZOFS			6.0
#define SHADOW_MIN_DIST 250.0
#define SHADOW_MAX_DIST 512.0
typedef struct
{
	char		*tagname;
	float		size;
	float		maxdist;
	float		maxalpha;
	qhandle_t	shader;
} shadowPart_t;


/* */
static qboolean
CG_PlayerShadow ( centity_t *cent, float *shadowPlane )
{
	vec3_t			end;
	trace_t			trace;
	float			dist, distFade;
	int				tagIndex, subIndex;
	vec3_t			origin, angles, axis[3];
	vec4_t			projection = { 0, 0, -1, 64 };
	shadowPart_t	shadowParts[] =
	{
		{ "tag_footleft", 10, 4, 1.0, 0 },
		{ "tag_footright", 10, 4, 1.0, 0 },
		{ "tag_torso", 18, 96, 0.8, 0 },
		{ NULL, 0 }
	};
	shadowParts[0].shader = cgs.media.shadowFootShader;						//DAJ pulled out of initliization
	shadowParts[1].shader = cgs.media.shadowFootShader;
	shadowParts[2].shader = cgs.media.shadowTorsoShader;
	*shadowPlane = 0;
	if ( cg_shadows.integer == 0 && cg_shadowPlanes.integer == 0 )
	{
		return ( qfalse );
	}

	// send a trace down from the player to the ground
	VectorCopy( cent->lerpOrigin, end );
	end[2] -= SHADOW_DISTANCE;
	trap_CM_BoxTrace( &trace, cent->lerpOrigin, end, NULL, NULL, 0, MASK_PLAYERSOLID );
	*shadowPlane = trace.endpos[2] + 1;

	// no shadows when dead
	if ( cent->currentState.eFlags & EF_DEAD )
	{
		return ( qfalse );
	}

	// add the mark as a temporary, so it goes directly to the renderer
	// without taking a spot in the cg_marks array
	dist = VectorDistance( cent->lerpOrigin, cg.refdef_current->vieworg );	//%	cg.snap->ps.origin );
	distFade = 1.0f;
	if ( !(cent->currentState.eFlags & EF_ZOOMING) && (dist > SHADOW_MIN_DIST) )
	{
		if ( dist > SHADOW_MAX_DIST )
		{
			if ( dist > SHADOW_MAX_DIST * 2 )
			{
				return ( qfalse );
			}
			else
			{																// fade out
				distFade = 1.0f - ( (dist - SHADOW_MAX_DIST) / SHADOW_MAX_DIST );
			}

			if ( distFade > 1.0f )
			{
				distFade = 1.0f;
			}
			else if ( distFade < 0.0f )
			{
				distFade = 0.0f;
			}
		}

		// set origin
		VectorCopy( cent->lerpOrigin, origin );

		// project it onto the shadow plane
		if ( origin[2] < *shadowPlane )
		{
			origin[2] = *shadowPlane;
		}

		// ydnar: add a bit of height so foot shadows don't clip into sloped geometry as much
		origin[2] += 18.0f;
		if ( cg_shadows.integer == 1 || cg_shadows.integer == 3 )
		{	// no mark for stencil or projection shadows
			CG_ImpactMark( cgs.media.shadowTorsoShader, origin, projection, 18.0f, cent->lerpAngles[YAW], distFade,
						   distFade, distFade, distFade, -1 );
		}
	}

	if ( dist < SHADOW_MAX_DIST )
	{		// show more detail
		// now add shadows for the various body parts
		for ( tagIndex = 0; shadowParts[tagIndex].tagname; tagIndex++ )
		{

			// grab each tag with this name
			for
			(
				subIndex = 0;
				(
					subIndex = CG_GetOriginForTag( cent, &cent->pe.bodyRefEnt, shadowParts[tagIndex].tagname, subIndex,
												   origin, axis)
				) >= 0;
				subIndex++
			)
			{

				// project it onto the shadow plane
				if ( origin[2] < *shadowPlane )
				{
					origin[2] = *shadowPlane;
				}

				// ydnar: add a bit of height so foot shadows don't clip into sloped geometry as much
				origin[2] += 5.0f;
				AxisToAngles( axis, angles );
				if ( cg_shadows.integer == 1 || cg_shadows.integer == 3 )
				{
					CG_ImpactMark( shadowParts[tagIndex].shader, origin, projection, shadowParts[tagIndex].size,
								   angles[YAW], distFade, distFade, distFade, distFade, -1 );
				}
			}
		}
	}

	return ( qtrue );
}


/*
===============
CG_PlayerSplash

Draw a mark at the water surface
===============
*/
static void
CG_PlayerSplash ( centity_t *cent )
{
	vec3_t		start, end;
	trace_t		trace;
	int			contents;
	polyVert_t	verts[4];
	if ( !cg_shadows.integer )
	{
		return;
	}

	VectorCopy( cent->lerpOrigin, end );
	end[2] -= 24;

	// if the feet aren't in liquid, don't make a mark
	// this won't handle moving water brushes, but they wouldn't draw right anyway...
	contents = CG_PointContents( end, 0 );
	if ( !(contents & (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA)) )
	{
		return;
	}

	VectorCopy( cent->lerpOrigin, start );
	start[2] += 32;

	// if the head isn't out of liquid, don't make a mark
	contents = CG_PointContents( start, 0 );
	if ( contents & (CONTENTS_SOLID | CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA) )
	{
		return;
	}

	// trace down to find the surface
	trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA) );
	if ( trace.fraction == 1.0 )
	{
		return;
	}

	// create a mark polygon
	VectorCopy( trace.endpos, verts[0].xyz );
	verts[0].xyz[0] -= 32;
	verts[0].xyz[1] -= 32;
	verts[0].st[0] = 0;
	verts[0].st[1] = 0;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;
	VectorCopy( trace.endpos, verts[1].xyz );
	verts[1].xyz[0] -= 32;
	verts[1].xyz[1] += 32;
	verts[1].st[0] = 0;
	verts[1].st[1] = 1;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;
	VectorCopy( trace.endpos, verts[2].xyz );
	verts[2].xyz[0] += 32;
	verts[2].xyz[1] += 32;
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;
	VectorCopy( trace.endpos, verts[3].xyz );
	verts[3].xyz[0] += 32;
	verts[3].xyz[1] -= 32;
	verts[3].st[0] = 1;
	verts[3].st[1] = 0;
	verts[3].modulate[0] = 255;
	verts[3].modulate[1] = 255;
	verts[3].modulate[2] = 255;
	verts[3].modulate[3] = 255;
	trap_R_AddPolyToScene( cgs.media.wakeMarkShader, 4, verts );
}

//==========================================================================


/*
===============
CG_AddRefEntityWithPowerups

Adds a piece with modifications or duplications for powerups
Also called by CG_Missile for quad rockets, but nobody can tell...
===============
*/
void
CG_AddRefEntityWithPowerups ( refEntity_t *ent, int powerups, int team, entityState_t *es, const vec3_t fireRiseDir )
{
	centity_t	*cent;
	refEntity_t backupRefEnt;	//, parentEnt;
	qboolean	onFire = qfalse;
	float		alpha = 0.0;
	float		fireStart, fireEnd;
	cent = &cg_entities[es->number];
	ent->entityNum = es->number;
	backupRefEnt = *ent;
	if ( CG_EntOnFire( &cg_entities[es->number]) )
	{
		ent->reFlags |= REFLAG_FORCE_LOD;
	}

	trap_R_AddRefEntityToScene( ent );
	if ( !onFire && CG_EntOnFire( &cg_entities[es->number]) )
	{
		onFire = qtrue;

		// set the alpha
		if ( ent->entityNum == cg.snap->ps.clientNum )
		{
			fireStart = cg.snap->ps.onFireStart;
			fireEnd = cg.snap->ps.onFireStart + 1500;
		}
		else
		{
			fireStart = es->onFireStart;
			fireEnd = es->onFireEnd;
		}

		alpha = ( cg.time - fireStart ) / 1500.0;
		if ( alpha > 1.0 )
		{
			alpha = ( fireEnd - cg.time ) / 1500.0;
			if ( alpha > 1.0 )
			{
				alpha = 1.0;
			}
		}
	}

	if ( onFire )
	{
		if ( alpha < 0.0 )
		{
			alpha = 0.0;
		}

		ent->shaderRGBA[3] = (unsigned char) ( 255.0 * alpha );
		VectorCopy( fireRiseDir, ent->fireRiseDir );
		if ( VectorCompare( ent->fireRiseDir, vec3_origin) )
		{
			VectorSet( ent->fireRiseDir, 0, 0, 1 );
		}

		ent->customShader = cgs.media.onFireShader;
		trap_R_AddRefEntityToScene( ent );
		ent->customShader = cgs.media.onFireShader2;
		trap_R_AddRefEntityToScene( ent );
		if ( ent->hModel == cent->pe.bodyRefEnt.hModel )
		{
			trap_S_AddLoopingSound( ent->origin, vec3_origin, cgs.media.flameCrackSound, (int) (255.0 * alpha), 0 );
		}
	}

	*ent = backupRefEnt;
}


/* */
char *
vtosf ( const vec3_t v )
{
	static int	index;
	static char str[8][64];
	char		*s;

	// use an array so that multiple vtos won't collide
	s = str[index];
	index = ( index + 1 ) & 7;
	Com_sprintf( s, 64, "(%f %f %f)", v[0], v[1], v[2] );
	return ( s );
}


/*
===============
CG_AnimPlayerConditions

	predict, or calculate condition for this entity, if it is not the local client
===============
*/
void
CG_AnimPlayerConditions ( bg_character_t *character, centity_t *cent )
{
	entityState_t	*es;
	int				legsAnim;
	clientInfo_t	*ci = &cgs.clientinfo[cent->currentState.number];
	if ( !character )
	{
#ifdef __NPC__
		if ( cent->currentState.eType == ET_NPC )
		{
			clientInfo_t	*ci = &cgs.clientinfo[cent->currentState.number];
			ci->character = character = CG_GetCharacter( ci->team, ci->cls );
		}
		else
		{
			return;
		}

#else //!__NPC__
		return;
#endif //__NPC__
	}

	if ( cg.snap && cg.snap->ps.clientNum == cent->currentState.number && !cg.renderingThirdPerson )
	{
		return;
	}

	es = &cent->currentState;

	// WEAPON
	if ( es->eFlags & EF_ZOOMING )
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_WEAPON, WP_BINOCULARS, qtrue );
	}
	else if
		(
			es->eFlags & EF_AIMING &&
			es->weapon != WP_AKIMBO_COLT &&
			es->weapon != WP_AKIMBO_SILENCEDCOLT &&
			es->weapon != WP_AKIMBO_LUGER &&
			es->weapon != WP_AKIMBO_SILENCEDLUGER &&
			es->weapon != WP_COLT &&
			es->weapon != WP_SILENCED_COLT &&
			es->weapon != WP_SILENCER &&
			es->weapon != WP_PANZERFAUST &&
			es->weapon != WP_LUGER
		)
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_WEAPON, WP_K43_SCOPE, qtrue );
	}
	else
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_WEAPON, es->weapon, qtrue );
	}

	// MOUNTED
	if ( (es->eFlags & EF_MG42_ACTIVE) || (es->eFlags & EF_MOUNTEDTANK) )
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOUNTED, MOUNTED_MG42, qtrue );
	}
	else if ( es->eFlags & EF_BROWNING_ACTIVE )
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOUNTED, MOUNTED_MG42, qtrue );
	}
	else if ( es->eFlags & EF_AAGUN_ACTIVE )
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOUNTED, MOUNTED_MG42 /*MOUNTED_AAGUN*/, qtrue );
	}
	else
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOUNTED, MOUNTED_UNUSED, qtrue );
	}

	// UNDERHAND
	// JAQUBOSS , lets make this optional
	BG_UpdateConditionValue( es->clientNum, ANIM_COND_UNDERHAND, cent->lerpAngles[0] > 0, qtrue );
	if ( es->eFlags & EF_CROUCHING )
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_CROUCHING, qtrue, qtrue );
	}
	else
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_CROUCHING, qfalse, qtrue );
	}

	if ( es->eFlags & EF_FIRING )
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_FIRING, qtrue, qtrue );
	}
	else
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_FIRING, qfalse, qtrue );
	}

	// reverse engineer the legs anim -> movetype (if possible)
	legsAnim = es->legsAnim &~ANIM_TOGGLEBIT;
#ifdef __NPC__
	if ( es->eType == ET_NPC )
	{
		ci->character = character = CG_GetCharacter( ci->team, ci->cls );	//CG_CharacterForClientinfo( ci, cent );
	}
#endif //__NPC__
	if ( character->animModelInfo->animations[legsAnim]->movetype )
	{
		BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOVETYPE,
								 character->animModelInfo->animations[legsAnim]->movetype, qfalse );
	}
}


/*
=================
CG_LightVerts
=================
*/
int
CG_LightVerts ( vec3_t normal, int numVerts, polyVert_t *verts )
{
	int		i, j;
	float	incoming;
	vec3_t	ambientLight;
	vec3_t	lightDir;
	vec3_t	directedLight;
	trap_R_LightForPoint( verts[0].xyz, ambientLight, directedLight, lightDir );
	for ( i = 0; i < numVerts; i++ )
	{
		incoming = DotProduct( normal, lightDir );
		if ( incoming <= 0 )
		{
			verts[i].modulate[0] = ambientLight[0];
			verts[i].modulate[1] = ambientLight[1];
			verts[i].modulate[2] = ambientLight[2];
			verts[i].modulate[3] = 255;
			continue;
		}

		j = ( ambientLight[0] + incoming * directedLight[0] );
		if ( j > 255 )
		{
			j = 255;
		}

		verts[i].modulate[0] = j;
		j = ( ambientLight[1] + incoming * directedLight[1] );
		if ( j > 255 )
		{
			j = 255;
		}

		verts[i].modulate[1] = j;
		j = ( ambientLight[2] + incoming * directedLight[2] );
		if ( j > 255 )
		{
			j = 255;
		}

		verts[i].modulate[2] = j;
		verts[i].modulate[3] = 255;
	}

	return ( qtrue );
}

static animation_t	multi_tankAnims[] =
{
	{ 0, "", 0, 1, 0, 1000 / 15, 1000 / 15 },

	// idle
	{ 0, "", 50, 15, 15, 1000 / 15, 1000 / 15 },

	// moving
	{ 0, "", 40, 10, 0, 1000 / 15, 1000 / 15 },

	// stopping
	{ 0, "", 50, -10, 0, 1000 / 15, 1000 / 15 },

	// starting
	{ 0, "", 65, 10, 0, 1000 / 7, 1000 / 7 },

	// shooting
};

// dhm - end


/*
==============
CG_TankSetAnim
==============
*/
static void
CG_TankSetAnim ( centity_t *cent, lerpFrame_t *lf, int newAnim )
{

	// transition animation

	/*	lf->animationNumber =   cent->currentState.frame;

	lf->animation = &multi_tankAnims[ cent->currentState.frame ];*/
	lf->animationNumber = newAnim;
	lf->animation = &multi_tankAnims[newAnim];
	lf->animationTime = lf->frameTime + lf->animation->initialLerp;
}

extern float	CG_ShadowPlane ( centity_t *cent );


/*
CG_TANK
*/
void
CG_Tank ( centity_t *cent )
{
	entityState_t	*cs;
	lerpFrame_t		*tanklf;
	refEntity_t		body;
	refEntity_t		barrel;
	refEntity_t		turret;
	refEntity_t		flash;
	refEntity_t		tracks;
	vec3_t			playerOrigin;
	vec3_t			lightorigin;
	int				clientNum;
	int				renderfx;
	static int		startTime;
	qboolean		thirdperson = qfalse;
	vec3_t			velocity;
	int				speed;

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.clientNum;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS )
	{
		CG_Error( "Bad clientNum on player entity" );
	}

	memset( &body, 0, sizeof(body) );
	memset( &turret, 0, sizeof(turret) );
	memset( &barrel, 0, sizeof(barrel) );
	memset( &flash, 0, sizeof(flash) );
	memset( &tracks, 0, sizeof(tracks) );
	VectorCopy( cent->lerpOrigin, playerOrigin );

	// get the rotation information @TODO fix all there
	CG_PlayerAngles( cent, body.axis, body.torsoAxis, turret.axis );

	// copy the torso rotation to the accessories
	AxisCopy( body.torsoAxis, barrel.axis );
	AxisCopy( body.torsoAxis, turret.axis );
	renderfx = 0;
	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
	{
		thirdperson = qtrue;						// only draw in mirrors
	}

	// draw the player in cameras
	if ( cg.cameraMode )
	{
		thirdperson = qfalse;
	}

	//	renderfx &= ~RF_THIRD_PERSON;
	renderfx |= RF_LIGHTING_ORIGIN;					// use the same origin for all
	VectorCopy( playerOrigin, lightorigin );
	lightorigin[2] += 31;

	// get a speed
	VectorCopy( cent->currentState.pos.trDelta, velocity );
	speed = VectorNormalize( velocity );

	//////////////
	//ANIMATIONS//
	cs = &cent->currentState;
	tanklf = &cent->lerpFrame;

	// initial setup
	if ( !tanklf->oldFrameTime )
	{
		tanklf->frameTime = tanklf->oldFrameTime = cg.time;
		CG_TankSetAnim( cent, tanklf, 0 );
		tanklf->frame = tanklf->oldFrame = tanklf->animation->firstFrame;
	}

	if ( cg.time - cent->muzzleFlashTime < 1200 && tanklf->animationNumber != 4 )
	{
		cent->pe.lightningFiring = 1;				// dont play start/stop sounds
		CG_TankSetAnim( cent, tanklf, 4 );
	}
	else if ( cg.time - cent->muzzleFlashTime >= 1200 && cent->pe.lightningFiring == 1 )
	{
		cent->pe.lightningFiring = 2;
	}

	if ( speed > 15 )
	{
		if ( tanklf->animationNumber != 1 && cent->pe.lightningFiring != 1 )
		{
			CG_TankSetAnim( cent, tanklf, 1 );
			if ( cent->pe.lightningFiring == 2 )
			{
				cent->pe.lightningFiring = 0;
			}
			else if ( cg.time - cent->pe.vehicleIgniteTime > 500 )
			{

				//trap_S_StartSound (NULL, cs->number, CHAN_AUTO, cgs.media.tankStartSound );
				cent->pe.vehicleIgniteTime = cg.time;
			}
		}
		else
		{
			trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.tankMoveSound, 255, 0 );
		}
	}
	else
	{
		if ( tanklf->animationNumber != 0 && cent->pe.lightningFiring != 1 )
		{
			CG_TankSetAnim( cent, tanklf, 0 );
			if ( cent->pe.lightningFiring == 2 )
			{
				cent->pe.lightningFiring = 0;
			}
			else if ( cg.time - cent->pe.vehicleIgniteTime > 500 )
			{
				trap_S_StartSound( NULL, cs->number, CHAN_AUTO, cgs.media.tankStopSound );
				cent->pe.vehicleIgniteTime = cg.time;
			}
		}
		else
		{
			trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.tankIdleSound, 255, 0 );
		}
	}

	CG_RunLerpFrame( cent, NULL, tanklf, 0, 1 );	// use existing lerp code rather than re-writing
	body.frame = tanklf->frame;
	body.oldframe = tanklf->oldFrame;
	body.backlerp = tanklf->backlerp;

	//
	// RENDERING
	//
	body.customSkin = 0;
	body.hModel = cgs.media.tankModel[cent->currentState.effect1Time];
	VectorCopy( playerOrigin, body.origin );
	VectorCopy( lightorigin, body.lightingOrigin );
	body.renderfx = renderfx;
	VectorCopy( body.origin, body.oldorigin );		// don't positionally lerp at all
	cent->pe.bodyRefEnt = body;

	// (SA) only need to set this once...
	VectorCopy( lightorigin, barrel.lightingOrigin );
	VectorCopy( lightorigin, turret.lightingOrigin );
	VectorCopy( lightorigin, tracks.lightingOrigin );
	barrel.renderfx = renderfx;
	turret.renderfx = renderfx;
	tracks.renderfx = renderfx;
	CG_AddRefEntityWithPowerups( &body, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
	if ( cg_shadowPlanes.integer )
	{
		body.shadowPlane = CG_ShadowPlane( cent );
		body.customShader = cgs.media.shadowShader;
		body.customSkin = 0;
		trap_R_AddRefEntityToScene( &body );
	}

	///////////////// COAX MuzzleFlash
	{
		vec3_t	angles;
		flash.hModel = cgs.media.tankFlashModel;
		angles[YAW] = 0;
		angles[PITCH] = 0;
		angles[ROLL] = crandom() * 10;
		AnglesToAxis( angles, flash.axis );
		CG_PositionRotatedEntityOnTag( &flash, &body, "tag_flash2" );
		VectorCopy( flash.origin, cent->pe.coaxMuzzle );
		if ( cg.time - cent->muzzleFlashTime2 < MUZZLE_FLASH_TIME )
		{
			CG_AddRefEntityWithPowerups( &flash, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
			trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );

			//	CG_AddFlashLighting(flash.origin);
		}
	}

	//////////////////////////

	/*	if ( thirdperson ) {
	flash.renderfx  |= RF_THIRD_PERSON;
	}*/

	////////////////////////// Jagdpanther barrel
	if ( cent->currentState.effect1Time == 1 )
	{
		vec3_t	turretAngles;
		vec3_t	tankAngles;
		VectorCopy( cent->lerpAngles, turretAngles );
		turretAngles[2] = 0;
		AxisToAngles( body.axis, tankAngles );
		turretAngles[1] = cent->currentState.effect2Time;
		turretAngles[0] = tankAngles[0] + cent->currentState.effect3Time;
		turretAngles[2] = tankAngles[2];
		barrel.hModel = cgs.media.tankBarrelModel[cent->currentState.effect1Time];
		barrel.customSkin = 0;
		CG_PositionEntityOnTag( &barrel, &body, "tag_turret", 0, NULL );
		AnglesToAxis( turretAngles, barrel.axis );
		CG_AddRefEntityWithPowerups( &barrel, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
		if ( cg_shadowPlanes.integer )
		{
			barrel.shadowPlane = CG_ShadowPlane( cent );
			barrel.customShader = cgs.media.shadowShader;
			barrel.customSkin = 0;
			trap_R_AddRefEntityToScene( &barrel );
		}
		{
			CG_PositionEntityOnTag( &flash, &barrel, "tag_flash", 0, NULL );
			AnglesToAxis( turretAngles, flash.axis );
			if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )
			{
				CG_AddRefEntityWithPowerups( &flash, cent->currentState.powerups, 0, &cent->currentState,
											 cent->fireRiseDir );
				trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );

				//CG_AddFlashLighting(flash.origin);
			}
		}

		////////////////// Churchill tower/barrel
	}
	else
	{
		vec3_t	turretAngles;
		vec3_t	tankAngles;
		AxisToAngles( body.axis, tankAngles );
		if ( cent->currentState.eFlags & EF_TURRETMOVE )
		{
			trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.tankTurretSound, 255, 0 );
			cent->pe.turretRotating = qtrue;
			cent->pe.turretStopTime = cg.time;
		}
		else if ( cg.time - cent->pe.turretStopTime < 300 )
		{
			trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.tankTurretSound, 255, 0 );
		}
		else if ( cent->pe.turretRotating && cg.time - cent->pe.turretStopTime > 400 )
		{
			trap_S_StartSound( NULL, cs->number, CHAN_AUTO, cgs.media.tankTurretEndSound );
			cent->pe.turretRotating = qfalse;
		}

		CG_tank_slope( cent, turretAngles, cent->currentState.effect2Time );

		//	turretAngles[0] = tankAngles[0];
		turretAngles[1] = cent->currentState.effect2Time;

		//	turretAngles[2] = tankAngles[2];
		turret.hModel = cgs.media.tankTurretModel[cent->currentState.effect1Time];
		turret.customSkin = 0;
		CG_PositionEntityOnTag( &turret, &body, "tag_turret", 0, NULL );
		AnglesToAxis( turretAngles, turret.axis );
		CG_AddRefEntityWithPowerups( &turret, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
		if ( cg_shadowPlanes.integer )
		{
			turret.shadowPlane = CG_ShadowPlane( cent );
			turret.customShader = cgs.media.shadowShader;
			turret.customSkin = 0;
			trap_R_AddRefEntityToScene( &turret );
		}

		barrel.hModel = cgs.media.tankBarrelModel[cent->currentState.effect1Time];
		CG_PositionEntityOnTag( &barrel, &turret, "tag_barrel", 0, NULL );
		turretAngles[0] = tankAngles[0] + cent->currentState.effect3Time;
		AnglesToAxis( turretAngles, barrel.axis );
		CG_AddRefEntityWithPowerups( &barrel, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
		if ( cg_shadowPlanes.integer )
		{
			barrel.shadowPlane = CG_ShadowPlane( cent );
			barrel.customShader = cgs.media.shadowShader;
			barrel.customSkin = 0;
			trap_R_AddRefEntityToScene( &barrel );
		}
		{
			CG_PositionEntityOnTag( &flash, &barrel, "tag_flash", 0, NULL );
			AnglesToAxis( turretAngles, flash.axis );
			if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )
			{
				CG_AddRefEntityWithPowerups( &flash, cent->currentState.powerups, 0, &cent->currentState,
											 cent->fireRiseDir );
				trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );

				//CG_AddFlashLighting(flash.origin);
			}
		}
	}

	// THIS WILL SET UP TRACKS SHADERS
	// @FIXME this is very ugly code
	VectorCopy( body.origin, tracks.origin );
	AxisCopy( body.axis, tracks.axis );
	if ( speed > 0 && cent->currentState.effect1Time == 0 )
	{
		tracks.customShader = cgs.media.tankShader[0];
	}
	else if ( speed > 0 && cent->currentState.effect1Time == 1 )
	{
		tracks.customShader = cgs.media.tankShader[1];
	}
	else if ( cent->currentState.effect1Time == 0 )
	{
		tracks.customShader = cgs.media.tankShader[2];
	}
	else if ( cent->currentState.effect1Time == 1 )
	{
		tracks.customShader = cgs.media.tankShader[3];
	}

	tracks.hModel = cgs.media.tankTracksModel[cent->currentState.effect1Time];
	CG_AddRefEntityWithPowerups( &tracks, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
	if ( cg_shadowPlanes.integer )
	{
		tracks.shadowPlane = CG_ShadowPlane( cent );
		tracks.customShader = cgs.media.shadowShader;
		tracks.customSkin = 0;
		trap_R_AddRefEntityToScene( &tracks );
	}

	//	memcpy( &cent->refEnt, &body, sizeof(refEntity_t) );
}

#ifdef __NPC__
extern void CG_NPC ( centity_t *cent );
#endif //__NPC__

/*
===============
CG_Player
===============
*/
void
CG_Player ( centity_t *cent )
{
	clientInfo_t	*ci;
	refEntity_t		body;
	refEntity_t		head;
	refEntity_t		acc;
	qhandle_t		painshader = 0;
	vec3_t			playerOrigin;
	vec3_t			lightorigin;
	int				clientNum, i;
	int				renderfx;
	qboolean		shadow;
	float			shadowPlane;
	qboolean		usingBinocs = qfalse;
	qboolean		usingRadio = qfalse;
	centity_t		*cgsnap;
	bg_character_t	*character;
	qboolean		thirdperson = qfalse;

	//	bg_character_t* headcharacter   = BG_GetCharacter( cgs.clientinfo[ cent->currentState.clientNum ].team, cgs.clientinfo[ cent->currentState.clientNum ].cls );
	float			hilightIntensity = 0.f;
#ifdef __NPC__

	/*	if (cent->currentState.number >= MAX_CLIENTS && cent->currentState.eType != ET_CORPSE )
	{// Fix... Somehow ET_NPC becomes ET_PLAYER sometimes...
		cent->currentState.eType = ET_NPC;
		CG_NPC(cent);
		return;
	}*/
#endif //__NPC__
	cgsnap = &cg_entities[cg.snap->ps.clientNum];
	shadow = qfalse;														// gjd added to make sure it was initialized
	shadowPlane = 0.0;														// ditto

	// if set to invisible, skip
	if ( cent->currentState.eFlags & EF_NODRAW )
	{
		return;
	}

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.clientNum;
	if ( (clientNum < 0 || clientNum >= MAX_CLIENTS)
#ifdef __NPC__
	&& cent->currentState.eType != ET_NPC
#endif //__NPC__
	)
	{

		//CG_Error( "CG_Player: Bad clientNum on player entity");
		return;
	}

	ci = &cgs.clientinfo[clientNum];

	// it is possible to see corpses from disconnected players that may
	// not have valid clientinfo
	if ( !ci->infoValid )
	{
		return;
	}

	character = CG_CharacterForClientinfo( ci, cent );
	if ( cent->currentState.eFlags & EF_MOUNTEDTANK )
	{
		VectorCopy( cg_entities[cg_entities[cent->currentState.clientNum].tagParent].mountedMG42Player.origin,
					playerOrigin );
	}
	else if
		(
			cent->currentState.eFlags & EF_MG42_ACTIVE ||
			cent->currentState.eFlags & EF_BROWNING_ACTIVE ||
			cent->currentState.eFlags & EF_AAGUN_ACTIVE
		)
	{																		// Arnout: see if we're attached to a gun
		centity_t	*mg42;
		int			num;

		// find the mg42 we're attached to
		for ( num = 0; num < cg.snap->numEntities; num++ )
		{
			mg42 = &cg_entities[cg.snap->entities[num].number];
			if
			(
				mg42->currentState.eType == ET_MG42_BARREL &&
				mg42->currentState.otherEntityNum == cent->currentState.number
			)
			{

				// found it, clamp behind gun
				vec3_t	forward, right, up;

				//AngleVectors (mg42->s.apos.trBase, forward, right, up);
				AngleVectors( cent->lerpAngles, forward, right, up );
				VectorMA( mg42->currentState.pos.trBase, -36, forward, playerOrigin );
				playerOrigin[2] = cent->lerpOrigin[2];
				break;
			}
		}

		if ( num == cg.snap->numEntities )
		{
			VectorCopy( cent->lerpOrigin, playerOrigin );
		}
	}
	else
	{
		VectorCopy( cent->lerpOrigin, playerOrigin );
	}

	memset( &body, 0, sizeof(body) );
	memset( &head, 0, sizeof(head) );
	memset( &acc, 0, sizeof(acc) );

	// get the rotation information
	CG_PlayerAngles( cent, body.axis, body.torsoAxis, head.axis );

	// FIXME: move this into CG_PlayerAngles
	if ( cgsnap == cent && (cg.snap->ps.pm_flags & PMF_LADDER) )
	{
		memcpy( body.torsoAxis, body.axis, sizeof(body.torsoAxis) );
	}

	// copy the torso rotation to the accessories
	AxisCopy( body.torsoAxis, acc.axis );

	// calculate client-side conditions
	CG_AnimPlayerConditions( character, cent );

	// get the animation state (after rotation, to allow feet shuffle)
	CG_PlayerAnimation( cent, &body );

	// forcibly set binoc animation
	if ( cent->currentState.eFlags & EF_ZOOMING )
	{
		usingBinocs = qtrue;
	}

	if ( cent->currentState.eFlags & EF_TALK )
	{
		usingRadio = qtrue;
	}

	// add the any sprites hovering above the player
	// rain - corpses don't get icons (fireteam check ran out of bounds)
	if ( cent->currentState.eType != ET_CORPSE )
	{
		CG_PlayerSprites( cent );
	}

	if ( cent->currentState.eType == ET_CORPSE && !cent->pe.corpseTeam )
	{
		cent->pe.corpseHSkin = ci->headSkin;
		cent->pe.corpseHModel = ci->headModel;
		cent->pe.corpseTeam = ci->team;
	}

	// add a water splash if partially in and out of water
	CG_PlayerSplash( cent );

	// get the player model information
	renderfx = 0;
	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
	{
		if ( !cg_firstPerson.integer )
		{
			renderfx = RF_THIRD_PERSON /* RF_SHADOW_PLANE | RF_NOSHADOW*/ ; // only draw in mirrors
		}

		thirdperson = qtrue;
	}

	// add the
	shadow = CG_PlayerShadow( cent, &shadowPlane );

	// draw the player in cameras
	if ( cg.cameraMode )
	{
		if ( !cg_firstPerson.integer )
		{
			renderfx &= ~RF_THIRD_PERSON;
		}

		thirdperson = qfalse;
	}

	if ( cg_shadows.integer == 3 )
	{
		renderfx |= RF_SHADOW_PLANE;
	}

	//	if ( cg_playerLitMode.integer > 0)  {
	renderfx |= RF_LIGHTING_ORIGIN;											// use the same origin for all

	//}
	// set renderfx for accessories
	acc.renderfx = renderfx;
	VectorCopy( playerOrigin, lightorigin );
	lightorigin[2] += 31;
	{
		vec3_t	dist;
		vec_t	distSquared;
		VectorSubtract( lightorigin, cg.refdef_current->vieworg, dist );
		distSquared = VectorLengthSquared( dist );
		if ( distSquared > Square( 384.f) )
		{
			renderfx |= RF_MINLIGHT;
			distSquared -= Square( 384.f );
			if ( distSquared > Square( 768.f) )
			{
				hilightIntensity = 1.f;
			}
			else
			{
				hilightIntensity = 1.f * ( distSquared / Square( 768.f) );
			}
		}
	}

	body.hilightIntensity = hilightIntensity;
	head.hilightIntensity = hilightIntensity;
	acc.hilightIntensity = hilightIntensity;

	//
	// add the body
	//
	if ( cent->currentState.eType == ET_CORPSE && cent->currentState.time2 == 1 )
	{
		body.hModel = character->undressedCorpseModel;
		body.customSkin = character->undressedCorpseSkin;
	}
	else
	{
		body.customSkin = character->skin;
		body.hModel = character->mesh;
	}

	VectorCopy( playerOrigin, body.origin );

	//	if ( cg_playerLitMode.integer > 0) {
	VectorCopy( lightorigin, body.lightingOrigin );

	//	}
	body.shadowPlane = shadowPlane;
	body.renderfx = renderfx;
	VectorCopy( body.origin, body.oldorigin );								// don't positionally lerp at all
	cent->pe.bodyRefEnt = body;

	// if the model failed, allow the default nullmodel to be displayed
	// Gordon: whoever wrote that comment sucks
	if ( !body.hModel )
	{
		return;
	}

	// (SA) only need to set this once...
	//	if ( cg_playerLitMode.integer > 0) {
	VectorCopy( lightorigin, acc.lightingOrigin );

	//	}
	CG_AddRefEntityWithPowerups( &body, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
	if ( cg_shadowPlanes.integer )
	{
		static qboolean again;
		if ( !again )
		{
			body.customSkin = 0;
			body.customShader = cgs.media.shadowShader;
			body.renderfx &= ~RF_THIRD_PERSON;
			again = qtrue;
		}
		else
		{
			again = qfalse;
		}

		if ( cg_shadowPlanes.integer != 2 )
		{		// just for fun :)
			again = qfalse;
		}

		CG_AddRefEntityWithPowerups( &body, cent->currentState.powerups, ci->team, &cent->currentState,
									 cent->fireRiseDir );
	}

	// legs for FP

	/*	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg_firstPerson.integer && !cg.renderingThirdPerson)
	{
	refEntity_t		legs;
	vec3_t		nullangle;

	nullangle[0] = 	nullangle[1] = 	nullangle[2] = 0;

	memset( &legs, 0, sizeof(legs) );
	memcpy(&legs, &body, sizeof(body));

	AnglesToAxis(nullangle, legs.torsoAxis);

	legs.renderfx &= ~RF_THIRD_PERSON;
	legs.renderfx |= RF_FIRST_PERSON;

	CG_AddRefEntityWithPowerups( &legs,	cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );

	}
*/

	/*	if( cent && cent->currentState.powerups & (1 << PW_OPS_DISGUISED) ) {
	head.customSkin =headcharacter->hudheadskin;
	head.hModel =headcharacter->hudhead;
	} else {
 	head.customSkin =character->hudheadskin;
	head.hModel =character->hudhead;
	}*/
	if ( cent->currentState.eType == ET_CORPSE )
	{
		if ( cent->pe.corpseTeam == TEAM_AXIS )
		{
			head.customSkin = headSkinsAxis[cent->pe.corpseHSkin].skin;
			head.hModel = headModelsAxis[cent->pe.corpseHModel].model;
		}
		else
		{
			head.customSkin = headSkinsAllies[cent->pe.corpseHSkin].skin;
			head.hModel = headModelsAllies[cent->pe.corpseHModel].model;
		}
	}
	else if ( ci->team == TEAM_AXIS )
	{
		head.customSkin = headSkinsAxis[ci->headSkin].skin;
		head.hModel = headModelsAxis[ci->headModel].model;
	}
	else
	{
		head.customSkin = headSkinsAllies[ci->headSkin].skin;
		head.hModel = headModelsAllies[ci->headModel].model;
	}

	//head.customSkin = character->hudheadskin;
	//	if ( cg_playerLitMode.integer > 0) {
	VectorCopy( lightorigin, head.lightingOrigin );

	//	}
	CG_PositionRotatedEntityOnTag( &head, &body, "tag_head" );
	if ( cent->currentState.number == cg.snap->ps.clientNum )
	{
		VectorCopy( head.origin, cg.headOrigin );
		AxisCopy( head.axis, cg.headAxis );
		cg.headView = qtrue;
	}

	//head.shadowPlane = shadowPlane;
	head.renderfx = renderfx;
	if ( thirdperson )
	{
		head.renderfx |= RF_THIRD_PERSON;
	}

	if ( cent->currentState.eFlags & EF_FIRING )
	{
		cent->pe.lastFiredWeaponTime = 0;
		cent->pe.weaponFireTime += cg.frametime;
	}
	else
	{
		if ( cent->pe.weaponFireTime > 500 && cent->pe.weaponFireTime )
		{
			cent->pe.lastFiredWeaponTime = cg.time;
		}

		cent->pe.weaponFireTime = 0;
	}

	if ( cg_blood.integer > 0 )
	{

		//Health = cg_entities[cent->currentState.clientNum].currentState.dl_intensity;
		if
		(
			cent->currentState.clientNum == cg.predictedPlayerState.clientNum &&
			!(cent->currentState.eType == ET_CORPSE)
		)
		{		//1
			if ( (cg.predictedPlayerState.eFlags & EF_DEAD) && cg.predictedPlayerState.eFlags & EF_HEADSHOT )
			{	//2
				painshader = cgs.media.hudDamagedStates[4];
			}
			else if ( cg.predictedPlayerState.eFlags & EF_DEAD )
			{	//3//2
				painshader = cgs.media.hudDamagedStates[3];
			}
			else if ( cg.predictedPlayerState.stats[STAT_HEALTH] < 5 )
			{	//4//3
				painshader = cgs.media.hudDamagedStates[3];
			}
			else if ( cg.predictedPlayerState.stats[STAT_HEALTH] < 20 )
			{	//5//4
				painshader = cgs.media.hudDamagedStates[2];
			}
			else if ( cg.predictedPlayerState.stats[STAT_HEALTH] < 40 )
			{	//6//5
				painshader = cgs.media.hudDamagedStates[1];
			}
			else if ( cg.predictedPlayerState.stats[STAT_HEALTH] < 60 )
			{	//7//6
				painshader = cgs.media.hudDamagedStates[0];
			}
			else
			{	//8//7
				painshader = 0;
			}	//8
		}
		else
		{		//9//1
			if
			(
				(cent->currentState.eFlags & EF_DEAD || cent->currentState.eType == ET_CORPSE) &&
				cent->currentState.eFlags & EF_HEADSHOT
			)
			{	//10
				painshader = cgs.media.hudDamagedStates[4];
			}
			else if ( cent->currentState.eFlags & EF_DEAD || cent->currentState.eType == ET_CORPSE )
			{	//11//10
				painshader = cgs.media.hudDamagedStates[3];
			}
			else if ( cent->currentState.dl_intensity < 5 )
			{	//12//11
				painshader = cgs.media.hudDamagedStates[3];
			}
			else if ( cent->currentState.dl_intensity < 20 )
			{	//13//12
				painshader = cgs.media.hudDamagedStates[2];
			}
			else if ( cent->currentState.dl_intensity < 40 )
			{	//14//13
				painshader = cgs.media.hudDamagedStates[1];
			}
			else if ( cent->currentState.dl_intensity < 60 )
			{	//15//14
				painshader = cgs.media.hudDamagedStates[0];
			}
			else
			{	//16//15
				painshader = 0;
			}	//16
		}		//9
	}
	else
	{
		painshader = 0;
	}

	if ( cent->currentState.eType != ET_CORPSE && !(cent->currentState.eFlags & EF_DEAD) )
	{
		hudHeadAnimNumber_t anim;
		if ( cent->pe.weaponFireTime > 500 )
		{
			anim = HD_ATTACK;
		}
		else if ( cg.time - cent->pe.lastFiredWeaponTime < 500 )
		{
			anim = HD_ATTACK_END;
		}
		else
		{
			anim = HD_IDLE1;
		}

		CG_HudHeadAnimation( character, &cent->pe.head, &head.oldframe, &head.frame, &head.backlerp, anim );
	}
	else
	{
		head.frame = 0;
		head.oldframe = 0;
		head.backlerp = 0.f;
	}

	// set blinking flag
	CG_AddRefEntityWithPowerups( &head, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
	if ( painshader /*&& (ci->health || cent->currentState.eFlags & EF_DEAD || cent->currentState.eType == ET_CORPSE)*/ )
	{
		head.customShader = 0;
		head.customSkin = painshader;
		trap_R_AddRefEntityToScene( &head );
	}

	cent->pe.headRefEnt = head;

	// set the shadowplane for accessories
	acc.shadowPlane = shadowPlane;
	head.shadowPlane = shadowPlane;
	body.shadowPlane = shadowPlane;
	if ( cg_shadowPlanes.integer )
	{
		head.renderfx &= ~RF_THIRD_PERSON;
		head.customShader = cgs.media.shadowShader;
		head.customSkin = 0;
		trap_R_AddRefEntityToScene( &head );
	}

	CG_BreathPuffs( cent, &head );

	/////////////
	if ( cg_shadowPlanes.integer )
	{
		if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
		{
			body.renderfx = RF_THIRD_PERSON /* RF_SHADOW_PLANE | RF_NOSHADOW*/ ;	// only draw in mirrors
		}

		// draw the player in cameras
		if ( cg.cameraMode )
		{
			body.renderfx &= ~RF_THIRD_PERSON;
		}
	}

	/////////////
	//
	// add the gun / barrel / flash
	//
	if ( !(cent->currentState.eFlags & EF_DEAD) /*&& !usingBinocs*/ )
	{								// NERVE - SMF
		CG_AddPlayerWeapon( &body, NULL, cent, qfalse );
	}

	//
	// add binoculars (if it's not the player)
	//
	if ( usingBinocs )
	{								// NERVE - SMF
		acc.hModel = cgs.media.thirdPersonBinocModel;
		CG_PositionEntityOnTag( &acc, &body, "tag_weapon", 0, NULL );
		CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
	}

	if ( cg_shadowPlanes.integer && usingBinocs )
	{
		acc.renderfx &= ~RF_THIRD_PERSON;
		acc.customShader = cgs.media.shadowShader;
		acc.customSkin = 0;
		trap_R_AddRefEntityToScene( &acc );
		acc.renderfx = renderfx;
		acc.customShader = 0;
	}

	if ( usingRadio )
	{
		acc.hModel = cgs.media.radiomodel;
		CG_PositionRotatedEntityOnTag( &acc, &body, "tag_weapon" );

		//CG_PositionEntityOnTag( &acc, &body, "tag_weapon", 0, NULL );
		CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
	}

	if ( cg_shadowPlanes.integer && usingRadio )
	{
		acc.renderfx &= ~RF_THIRD_PERSON;
		acc.customShader = cgs.media.shadowShader;
		acc.customSkin = 0;
		trap_R_AddRefEntityToScene( &acc );
		acc.renderfx = renderfx;
		acc.customShader = 0;
	}

	/// CTF , FIXME , ADD FOR NORMAL GAMETYPES TOO
	if ( cgs.gametype == GT_WOLF_CTF )
	{
		if ( cent->currentState.powerups & (1 << PW_BLUEFLAG) )
		{
			acc.hModel = cgs.media.blueflag;
			CG_PositionRotatedEntityOnTag( &acc, &body, "tag_back" );
			CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState,
										 cent->fireRiseDir );
			if ( cg_shadowPlanes.integer )
			{
				acc.renderfx &= ~RF_THIRD_PERSON;
				acc.customShader = cgs.media.shadowShader;
				acc.customSkin = 0;
				trap_R_AddRefEntityToScene( &acc );
				acc.renderfx = renderfx;
				acc.customShader = 0;
			}
		}

		if ( cent->currentState.powerups & (1 << PW_REDFLAG) )
		{
			acc.hModel = cgs.media.redflag;
			CG_PositionRotatedEntityOnTag( &acc, &body, "tag_back" );
			CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState,
										 cent->fireRiseDir );
			if ( cg_shadowPlanes.integer )
			{
				acc.renderfx &= ~RF_THIRD_PERSON;
				acc.customShader = cgs.media.shadowShader;
				acc.customSkin = 0;
				trap_R_AddRefEntityToScene( &acc );
				acc.renderfx = renderfx;
				acc.customShader = 0;
			}
		}
	}

	/// CTF
	if ( cent->currentState.density == WP_FLAMETHROWER )
	{
		acc.hModel = cgs.media.flamethrowerBackModel;
		CG_PositionEntityOnTag( &acc, &body, "tag_back", 0, NULL );
		CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
		if ( cg_shadowPlanes.integer )
		{
			acc.renderfx &= ~RF_THIRD_PERSON;
			acc.customShader = cgs.media.shadowShader;
			acc.customSkin = 0;
			trap_R_AddRefEntityToScene( &acc );
			acc.renderfx = renderfx;
			acc.customShader = 0;
		}

		if ( cgs.clientinfo[cent->currentState.clientNum].team == TEAM_AXIS )
		{
			acc.hModel = cgs.media.gmask;
			CG_PositionEntityOnTag( &acc, &head, "tag_mouth", 0, NULL );
			CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState,
										 cent->fireRiseDir );
			if ( cg_shadowPlanes.integer )
			{
				acc.renderfx &= ~RF_THIRD_PERSON;
				acc.customShader = cgs.media.shadowShader;
				acc.customSkin = 0;
				trap_R_AddRefEntityToScene( &acc );
				acc.renderfx = renderfx;
				acc.customShader = 0;
			}
		}
	}

	if
	(
		(cent->currentState.density == WP_MOBILE_MG42 || cent->currentState.density == WP_30CAL) &&
		cent->currentState.effect1Time > 0
	)
	{
		if ( cent->currentState.effect1Time <= 225 )
		{
			acc.hModel = cgs.media.bulletload;
		}

		if ( cent->currentState.effect1Time <= 375 && cent->currentState.effect1Time > 225 )
		{
			acc.hModel = cgs.media.bulletload2;
		}

		if ( cent->currentState.effect1Time > 375 )
		{
			acc.hModel = cgs.media.bulletload3;
		}

		CG_PositionEntityOnTag( &acc, &body, "tag_back", 0, NULL );
		CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
		if ( cg_shadowPlanes.integer )
		{
			acc.renderfx &= ~RF_THIRD_PERSON;
			acc.customShader = cgs.media.shadowShader;
			acc.customSkin = 0;
			trap_R_AddRefEntityToScene( &acc );
			acc.renderfx = renderfx;
			acc.customShader = 0;
		}
	}

	if ( cent->currentState.density == WP_MORTAR && cent->currentState.effect1Time > 0 )
	{
		if ( cent->currentState.effect1Time >= 12 )
		{
			acc.hModel = cgs.media.mortarload[11];
		}
		else
		{
			acc.hModel = cgs.media.mortarload[cent->currentState.effect1Time-1];
		}

		CG_PositionEntityOnTag( &acc, &body, "tag_back", 0, NULL );
		CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
		if ( cg_shadowPlanes.integer )
		{
			acc.renderfx &= ~RF_THIRD_PERSON;
			acc.customShader = cgs.media.shadowShader;
			acc.customSkin = 0;
			trap_R_AddRefEntityToScene( &acc );
			acc.renderfx = renderfx;
			acc.customShader = 0;
		}
	}

	//
	// add accessories
	//
	for ( i = ACC_BELT_LEFT; i < ACC_MAX; i++ )
	{
		if ( !(character->accModels[i]) )
		{
			continue;
		}

		acc.hModel = character->accModels[i];
		acc.customSkin = character->accSkins[i];

		// Gordon: looted corpses dont have any accsserories, evil looters :E
		if ( !(cent->currentState.eType == ET_CORPSE && cent->currentState.time2 == 1) )
		{
			switch ( i )
			{
				case ACC_BELT_LEFT:
					CG_PositionEntityOnTag( &acc, &body, "tag_bright", 0, NULL );
					break;
				case ACC_BELT_RIGHT:
					CG_PositionEntityOnTag( &acc, &body, "tag_bleft", 0, NULL );
					break;
				case ACC_BELT:
					CG_PositionEntityOnTag( &acc, &body, "tag_ubelt", 0, NULL );
					break;
				case ACC_BACK2:
				case ACC_BACK:
					if
					(
						cent->currentState.density == WP_MORTAR ||
						cent->currentState.density == WP_30CAL ||
						cent->currentState.density == WP_MOBILE_MG42 ||
						cent->currentState.density == WP_FLAMETHROWER
					)
					{
						continue;
					}

					CG_PositionEntityOnTag( &acc, &body, "tag_back", 0, NULL );
					break;
				case ACC_HAT:		//hat
				case ACC_RANK:
					if ( cent->currentState.eFlags & EF_HEADSHOT )
					{
						continue;
					}

				case ACC_MOUTH2:	// hat2
				case ACC_MOUTH3:	// hat3					
					if ( i == ACC_RANK )
					{
						if ( ci->rank == 0 )
						{
							continue;
						}

						acc.customShader = rankicons[ci->rank][1].shader;
					}

					CG_PositionEntityOnTag( &acc, &head, "tag_mouth", 0, NULL );
					break;

				// weapon and weapon2
				// these are used by characters who have permanent weapons attached to their character in the skin
				case ACC_WEAPON:	// weap
					CG_PositionEntityOnTag( &acc, &body, "tag_weapon", 0, NULL );
					break;
				case ACC_WEAPON2:	// weap2
					CG_PositionEntityOnTag( &acc, &body, "tag_weapon2", 0, NULL );
					break;
				default:
					continue;
			}

			CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState,
										 cent->fireRiseDir );
			if ( cg_shadowPlanes.integer )
			{
				acc.renderfx &= ~RF_THIRD_PERSON;
				acc.customShader = cgs.media.shadowShader;
				acc.customSkin = 0;
				trap_R_AddRefEntityToScene( &acc );
				acc.renderfx = renderfx;
				acc.customShader = 0;
			}
		}
	}
}

//=====================================================================
extern void CG_ClearWeapLerpFrame ( clientInfo_t *ci, lerpFrame_t *lf, int animationNumber );


/*
===============
CG_ResetPlayerEntity

A player just came into view or teleported, so reset all animation info
===============
*/
void
CG_ResetPlayerEntity ( centity_t *cent )
{
	if ( !(cent->currentState.eFlags & EF_DEAD) )
	{
		CG_ClearLerpFrameRate( cent, &cgs.clientinfo[cent->currentState.clientNum], &cent->pe.legs,
							   cent->currentState.legsAnim );
		CG_ClearLerpFrame( cent, &cgs.clientinfo[cent->currentState.clientNum], &cent->pe.torso,
						   cent->currentState.torsoAnim );
		memset( &cent->pe.legs, 0, sizeof(cent->pe.legs) );
		cent->pe.legs.yawAngle = cent->rawAngles[YAW];
		cent->pe.legs.yawing = qfalse;
		cent->pe.legs.pitchAngle = 0;
		cent->pe.legs.pitching = qfalse;
		memset( &cent->pe.torso, 0, sizeof(cent->pe.legs) );
		cent->pe.torso.yawAngle = cent->rawAngles[YAW];
		cent->pe.torso.yawing = qfalse;
		cent->pe.torso.pitchAngle = cent->rawAngles[PITCH];
		cent->pe.torso.pitching = qfalse;
	}

	BG_EvaluateTrajectory( &cent->currentState.pos, cg.time, cent->lerpOrigin, qfalse, cent->currentState.effect2Time );
	BG_EvaluateTrajectory( &cent->currentState.apos, cg.time, cent->lerpAngles, qtrue, cent->currentState.effect2Time );
	VectorCopy( cent->lerpOrigin, cent->rawOrigin );
	VectorCopy( cent->lerpAngles, cent->rawAngles );
	if ( cg_debugPosition.integer )
	{
		CG_Printf( "%i ResetPlayerEntity yaw=%i\n", cent->currentState.number, cent->pe.torso.yawAngle );
	}

	cent->pe.painAnimLegs = -1;
	cent->pe.painAnimTorso = -1;
	cent->pe.animSpeed = 1.0;
}


/* */
void
CG_GetBleedOrigin ( vec3_t head_origin, vec3_t body_origin, int fleshEntityNum )
{
	clientInfo_t	*ci;
	refEntity_t		body;
	refEntity_t		head;
	centity_t		*cent, backupCent;
	bg_character_t	*character;
	ci = &cgs.clientinfo[fleshEntityNum];
	if ( !ci->infoValid )
	{
		return;
	}

	character = CG_CharacterForClientinfo( ci, NULL );
	cent = &cg_entities[fleshEntityNum];
	backupCent = *cent;
	memset( &body, 0, sizeof(body) );
	memset( &head, 0, sizeof(head) );
	CG_PlayerAngles( cent, body.axis, body.torsoAxis, head.axis );
	CG_PlayerAnimation( cent, &body );
	body.hModel = character->mesh;
	if ( !body.hModel )
	{
		return;
	}

	head.hModel = character->hudhead;
	if ( !head.hModel )
	{
		return;
	}

	VectorCopy( cent->lerpOrigin, body.origin );
	VectorCopy( body.origin, body.oldorigin );

	// Ridah, restore the cent so we don't interfere with animation timings
	*cent = backupCent;
	CG_PositionRotatedEntityOnTag( &head, &body, "tag_head" );
	VectorCopy( head.origin, head_origin );
	VectorCopy( body.origin, body_origin );
}


/*
===============
CG_GetTag
===============
*/
qboolean
CG_GetTag ( int clientNum, char *tagname, orientation_t *or )
{
	clientInfo_t	*ci;
	centity_t		*cent;
	refEntity_t		*refent;
	vec3_t			tempAxis[3];
	vec3_t			org;
	int				i;
	ci = &cgs.clientinfo[clientNum];
	if ( cg.snap && clientNum == cg.snap->ps.clientNum && cg.renderingThirdPerson )
	{
		cent = &cg.predictedPlayerEntity;
	}
	else
	{
		cent = &cg_entities[ci->clientNum];
		if ( !cent->currentValid )
		{
			return ( qfalse );	// not currently in PVS
		}
	}

	refent = &cent->pe.bodyRefEnt;
	if ( trap_R_LerpTag( or, refent, tagname, 0) < 0 )
	{
		return ( qfalse );
	}

	VectorCopy( refent->origin, org );
	for ( i = 0; i < 3; i++ )
	{
		VectorMA( org, or->origin[i], refent->axis[i], org );
	}

	VectorCopy( org, or->origin );

	// rotate with entity
	MatrixMultiply( refent->axis, or->axis, tempAxis );
	memcpy( or->axis, tempAxis, sizeof(vec3_t) * 3 );
	return ( qtrue );
}


/*
===============
CG_GetWeaponTag
===============
*/
qboolean
CG_GetWeaponTag ( int clientNum, char *tagname, orientation_t *or )
{
	clientInfo_t	*ci;
	centity_t		*cent;
	refEntity_t		*refent;
	vec3_t			tempAxis[3];
	vec3_t			org;
	int				i;
	ci = &cgs.clientinfo[clientNum];
	if ( cg.snap && clientNum == cg.snap->ps.clientNum && cg.renderingThirdPerson )
	{
		cent = &cg.predictedPlayerEntity;
	}
	else
	{
		cent = &cg_entities[ci->clientNum];
		if ( !cent->currentValid )
		{
			return ( qfalse );	// not currently in PVS
		}
	}

	if ( cent->pe.gunRefEntFrame < cg.clientFrame - 1 )
	{
		return ( qfalse );
	}

	refent = &cent->pe.gunRefEnt;
	if ( trap_R_LerpTag( or, refent, tagname, 0) < 0 )
	{
		return ( qfalse );
	}

	VectorCopy( refent->origin, org );
	for ( i = 0; i < 3; i++ )
	{
		VectorMA( org, or->origin[i], refent->axis[i], org );
	}

	VectorCopy( org, or->origin );

	// rotate with entity
	MatrixMultiply( refent->axis, or->axis, tempAxis );
	memcpy( or->axis, tempAxis, sizeof(vec3_t) * 3 );
	return ( qtrue );
}

// =============
// Menu Versions
// =============


/*
==================
CG_SwingAngles_Limbo
==================
*/
static void
CG_SwingAngles_Limbo
(
	float		destination,
	float		swingTolerance,
	float		clampTolerance,
	float		speed,
	float		*angle,
	qboolean	*swinging
)
{
	float	swing;
	float	move;
	float	scale;
	if ( !*swinging )
	{

		// see if a swing should be started
		swing = AngleSubtract( *angle, destination );
		if ( swing > swingTolerance || swing < -swingTolerance )
		{
			*swinging = qtrue;
		}
	}

	if ( !*swinging )
	{
		return;
	}

	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract( destination, *angle );
	scale = fabs( swing );
	if ( scale < swingTolerance * 0.5 )
	{
		scale = 0.5;
	}
	else if ( scale < swingTolerance )
	{
		scale = 1.0;
	}
	else
	{
		scale = 2.0;
	}

	// swing towards the destination angle
	if ( swing >= 0 )
	{
		move = cg.frametime * scale * speed;
		if ( move >= swing )
		{
			move = swing;
			*swinging = qfalse;
		}

		*angle = AngleMod( *angle + move );
	}
	else if ( swing < 0 )
	{
		move = cg.frametime * scale * -speed;
		if ( move <= swing )
		{
			move = swing;
			*swinging = qfalse;
		}

		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance )
	{
		*angle = AngleMod( destination - (clampTolerance - 1) );
	}
	else if ( swing < -clampTolerance )
	{
		*angle = AngleMod( destination + (clampTolerance - 1) );
	}
}


/*
===============
CG_PlayerAngles_Limbo
===============
*/
void
CG_PlayerAngles_Limbo ( playerInfo_t *pi, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] )
{
	vec3_t	legsAngles, torsoAngles, headAngles;
	float	dest;
	VectorCopy( pi->viewAngles, headAngles );
	headAngles[YAW] = AngleMod( headAngles[YAW] );
	VectorClear( legsAngles );
	VectorClear( torsoAngles );
	torsoAngles[YAW] = 180;
	legsAngles[YAW] = 180;
	headAngles[YAW] = 180;

	// --------- pitch -------------
	// only show a fraction of the pitch angle in the torso
	if ( headAngles[PITCH] > 180 )
	{
		dest = ( -360 + headAngles[PITCH] ) * 0.75;
	}
	else
	{
		dest = headAngles[PITCH] * 0.75;
	}

	CG_SwingAngles_Limbo( dest, 15, 30, 0.1, &pi->torso.pitchAngle, &pi->torso.pitching );
	torsoAngles[PITCH] = pi->torso.pitchAngle;
	torsoAngles[ROLL] += cg.predictedPlayerState.leanf * 2.3;
	headAngles[ROLL] += cg.predictedPlayerState.leanf * 2.3;

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
	AnglesSubtract( legsAngles, pi->moveAngles, legsAngles );	// NERVE - SMF
	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
}


/* */
animation_t *
CG_GetLimboAnimation ( playerInfo_t *pi, const char *name )
{
	int				i;
	bg_character_t	*character = BG_GetCharacter( pi->teamNum, pi->classNum );
	if ( !character )
	{
		return ( NULL );
	}

	for ( i = 0; i < character->animModelInfo->numAnimations; i++ )
	{
		if ( !Q_stricmp( character->animModelInfo->animations[i]->name, name) )
		{
			return ( character->animModelInfo->animations[i] );
		}
	}

	return ( character->animModelInfo->animations[0] ); // safe fallback so we never end up without an animation (which will crash the game)
}


/* */
int
CG_GetSelectedWeapon ( void )
{
	return ( 0 );
}


/*static int tempPos = 0;
static int tempTime = 0;
*/
void
CG_DrawPlayer_Limbo
(
	float			x,
	float			y,
	float			w,
	float			h,
	playerInfo_t	*pi,
	int				time,
	clientInfo_t	*ci,
	qboolean		animatedHead,
	int				viewHeight
)
{
	refdef_t		refdef;
	refEntity_t		body;
	refEntity_t		head;

	//	refEntity_t		gun;
	//	refEntity_t		barrel;
	refEntity_t		acc;
	vec3_t			origin;
	int				renderfx;
	vec3_t			mins = { -16, -16, -24 };
	vec3_t			maxs = { 16, 16, 32 };
	float			len;
	vec4_t			hcolor = { 1, 0, 0, 0.5 };
	bg_character_t	*character = BG_GetCharacter( pi->teamNum, pi->classNum );
	int				i;
	vec3_t			myAngle;
	dp_realtime = time;
	CG_AdjustFrom640( &x, &y, &w, &h );
	y -= jumpHeight;
	memset( &refdef, 0, sizeof(refdef) );
	memset( &body, 0, sizeof(body) );
	memset( &head, 0, sizeof(head) );
	memset( &acc, 0, sizeof(acc) );
	refdef.rdflags = RDF_NOWORLDMODEL | RF_NOSHADOW;
	AxisClear( refdef.viewaxis );
	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;
	myAngle[0] = viewHeight;
	myAngle[1] = 0;
	myAngle[2] = 0;
	AnglesToAxis( myAngle, refdef.viewaxis );
	refdef.fov_x = 35;
	refdef.fov_y = 40;						// was 35

	// calculate distance so the player nearly fills the box
	// START Mad Doc - TDF
	// for "talking heads", we calc the origin differently
	// FIXME - this is stupid - should all be character driven - NO CODE HACKS FOR SPECIFIC THINGS THAT CAN BE DONE CLEANLY
	if ( animatedHead == qfalse )
	{

		// END Mad Doc - TDF	
		len = 0.9 * ( maxs[2] - mins[2] );	// NERVE - SMF - changed from 0.7
		origin[0] = pi->y - 70 + ( len / tan( DEG2RAD( refdef.fov_x) * 0.5) );
		origin[1] = 0.5 * ( mins[1] + maxs[1] );
		origin[2] = pi->z - 23 + ( -0.5 * (mins[2] + maxs[2]) );
	}
	else
	{

		// for "talking head" animations, we want to center just below the face
		// we precalculated this elsewhere
		VectorCopy( pi->headOrigin, origin );
	}

	// END Mad Doc - TDF
	refdef.time = dp_realtime;
	trap_R_SetColor( hcolor );
	trap_R_ClearScene();
	trap_R_SetColor( NULL );

	// get the rotation information
	CG_PlayerAngles_Limbo( pi, body.axis, body.torsoAxis, head.axis );
	renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW | RF_FORCENOLOD;
	acc.renderfx = renderfx;

	//
	// add the body
	//
	body.hModel = character->mesh;
	body.customSkin = character->skin;
	body.renderfx = renderfx;
	VectorCopy( origin, body.origin );
	VectorCopy( origin, body.lightingOrigin );
	VectorCopy( body.origin, body.oldorigin );
	if ( cg.time >= pi->torso.frameTime )
	{
		pi->torso.oldFrameTime = pi->torso.frameTime;
		pi->torso.oldFrame = pi->torso.frame;
		pi->torso.oldFrameModel = pi->torso.frameModel = pi->torso.animation->mdxFile;
		while ( cg.time >= pi->torso.frameTime )
		{
			pi->torso.frameTime += ( pi->torso.animation->duration / (float) (pi->torso.animation->numFrames) );
			pi->torso.frame++;
			if ( pi->torso.frame >= pi->torso.animation->firstFrame + pi->torso.animation->numFrames )
			{
				pi->torso.frame = pi->torso.animation->firstFrame;
			}
		}
	}

	if ( pi->torso.frameTime == pi->torso.oldFrameTime )
	{
		pi->torso.backlerp = 0;
	}
	else
	{
		pi->torso.backlerp = 1.0 - (float) ( cg.time - pi->torso.oldFrameTime ) / ( pi->torso.frameTime - pi->torso.oldFrameTime );
	}

	if ( cg.time >= pi->legs.frameTime )
	{
		pi->legs.oldFrameTime = pi->legs.frameTime;
		pi->legs.oldFrame = pi->legs.frame;
		pi->legs.oldFrameModel = pi->legs.frameModel = pi->legs.animation->mdxFile;
		while ( cg.time >= pi->legs.frameTime )
		{
			pi->legs.frameTime += ( pi->legs.animation->duration / (float) (pi->legs.animation->numFrames) );
			pi->legs.frame++;
			if ( pi->legs.frame >= pi->legs.animation->firstFrame + pi->legs.animation->numFrames )
			{
				pi->legs.frame = pi->legs.animation->firstFrame;
			}
		}
	}

	if ( pi->legs.frameTime == pi->legs.oldFrameTime )
	{
		pi->legs.backlerp = 0;
	}
	else
	{
		pi->legs.backlerp = 1.0 - (float) ( cg.time - pi->legs.oldFrameTime ) / ( pi->legs.frameTime - pi->legs.oldFrameTime );
	}

	body.oldTorsoFrame = pi->torso.oldFrame;
	body.torsoFrame = pi->torso.frame;
	body.torsoBacklerp = pi->torso.backlerp;
	body.torsoFrameModel = pi->torso.frameModel;
	body.oldTorsoFrameModel = pi->torso.oldFrameModel;
	body.oldframe = pi->legs.oldFrame;
	body.frame = pi->legs.frame;
	body.backlerp = pi->legs.backlerp;
	body.frameModel = pi->legs.frameModel;
	body.oldframeModel = pi->legs.oldFrameModel;
	trap_R_AddRefEntityToScene( &body );

	//
	// add the head
	//
	//	head.hModel = character->hudhead;
	if ( ci->team == TEAM_AXIS )
	{
		head.hModel = headModelsAxis[ci->headModel].model;
	}
	else
	{
		head.hModel = headModelsAllies[ci->headModel].model;
	}

	if ( !head.hModel )
	{
		return;
	}

	//head.customSkin =headcharacter->hudheadskin;
	//head.customSkin = character->headSkin;
	if ( ci->team == TEAM_AXIS )
	{
		head.customSkin = headSkinsAxis[ci->headSkin].skin;
	}
	else
	{
		head.customSkin = headSkinsAllies[ci->headSkin].skin;
	}

	VectorCopy( origin, head.lightingOrigin );
	CG_PositionRotatedEntityOnTag( &head, &body, "tag_head" );
	head.renderfx = renderfx;
	head.frame = 0;
	head.oldframe = 0;
	head.backlerp = 0.f;
	trap_R_AddRefEntityToScene( &head );
	AxisCopy( body.torsoAxis, acc.axis );
	VectorCopy( origin, acc.lightingOrigin );
	for ( i = ACC_BELT_LEFT; i < ACC_MAX; i++ )
	{
		if ( !(character->accModels[i]) )
		{
			continue;
		}

		acc.hModel = character->accModels[i];
		acc.customSkin = character->accSkins[i];
		acc.renderfx = renderfx;
		switch ( i )
		{
			case ACC_BELT_LEFT:
				CG_PositionEntityOnTag( &acc, &body, "tag_bright", 0, NULL );
				break;
			case ACC_BELT_RIGHT:
				CG_PositionEntityOnTag( &acc, &body, "tag_bleft", 0, NULL );
				break;
			case ACC_BELT:
				CG_PositionEntityOnTag( &acc, &body, "tag_ubelt", 0, NULL );
				break;
			case ACC_BACK2:
			case ACC_BACK:
				CG_PositionEntityOnTag( &acc, &body, "tag_back", 0, NULL );
				break;
			case ACC_HAT:					// hat
			case ACC_MOUTH2:				// hat2
			case ACC_MOUTH3:				// hat3
				CG_PositionEntityOnTag( &acc, &head, "tag_mouth", 0, NULL );
				break;

			// weapon and weapon2
			// these are used by characters who have permanent weapons attached to their character in the skin
			case ACC_WEAPON:				// weap
				CG_PositionEntityOnTag( &acc, &body, "tag_weapon", 0, NULL );
				break;
			case ACC_WEAPON2:				// weap2
				CG_PositionEntityOnTag( &acc, &body, "tag_weapon2", 0, NULL );
				break;
			default:
				continue;
		}

		trap_R_AddRefEntityToScene( &acc );
	}

	//
	// add the gun
	//

	/*	{
		int weap = CG_GetSelectedWeapon();

		memset( &gun, 0, sizeof(gun) );
		memset( &barrel, 0, sizeof(barrel) );

		gun.hModel = cg_weapons[weap].weaponModel[W_TP_MODEL].model;

		VectorCopy( origin, gun.lightingOrigin );
		CG_PositionEntityOnTag( &gun, &body, "tag_weapon", 0, NULL );
		gun.renderfx = renderfx;
		trap_R_AddRefEntityToScene( &gun );
	}*/
	CG_AddPlayerWeapon( &body, NULL, &cg.predictedPlayerEntity, qtrue );

	// Save out the old render info so we don't kill the LOD system here
	trap_R_SaveViewParms();

	//
	// add an accent light
	//
	origin[0] -= 100;						// + = behind, - = in front
	origin[1] += 100;						// + = left, - = right
	origin[2] += 100;						// + = above, - = below

	//%	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 0 );
	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 1.0, 0, 0 );
	origin[0] -= 100;
	origin[1] -= 100;
	origin[2] -= 100;

	//%	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 0 );
	trap_R_AddLightToScene( origin, 1000, 1.0, 1.0, 1.0, 1.0, 0, 0 );
	trap_R_RenderScene( &refdef );

	// Reset the view parameters
	trap_R_RestoreViewParms();
}

weaponType_t	weaponTypes[] =
{
	{ WP_MP40, "MP 40" },
	{ WP_THOMPSON, "THOMPSON" },
	{ WP_STEN, "STEN", },
	{ WP_PANZERFAUST, "PANZERFAUST", },
	{ WP_FLAMETHROWER, "FLAMETHROWER", },
	{ WP_KAR98, "K43+GPG40", },
	{ WP_CARBINE, "M1 GARAND+M7", },
	{ WP_FG42, "FG42", },
	{ WP_GARAND, "M1 GARAND", },
	{ WP_MOBILE_MG42, "MOBILE MG42", },
	{ WP_K43, "K43", },
	{ WP_MORTAR, "MORTAR", },
	{ WP_COLT, "COLT", },
	{ WP_LUGER, "LUGER", },
	{ WP_AKIMBO_COLT, "AKIMBO COLTS", },
	{ WP_AKIMBO_LUGER, "AKIMBO LUGERS", },
	{ WP_SILENCED_COLT, "COLT", },
	{ WP_SILENCER, "LUGER", },
	{ WP_AKIMBO_SILENCEDCOLT, "AKIMBO COLTS", },
	{ WP_AKIMBO_SILENCEDLUGER, "AKIMBO LUGERS", },
	{ WP_G43, "K43", },
	{ WP_M1GARAND, "M1 GARAND", },
	{ WP_30CAL, "1919 .30CAL", },
#ifdef NWEAPS
	{ WP_MP44, "Stg44", },
	{ WP_BAR, "1918 BAR", },
#endif
	{ WP_NONE, NULL, },
	{ -1, NULL, },
};


/* */
weaponType_t *
WM_FindWeaponTypeForWeapon ( weapon_t weapon )
{
	weaponType_t	*w = weaponTypes;
	if ( !weapon )
	{
		return ( NULL );
	}

	while ( w->weapindex != -1 )
	{
		if ( w->weapindex == weapon )
		{
			return ( w );
		}

		w++;
	}

	return ( NULL );
}


/* */
void
WM_RegisterWeaponTypeShaders ( void )
{
	weaponType_t	*w = weaponTypes;
	while ( w->weapindex )
	{
		w++;
	}
}


/* */
void
CG_MenuSetAnimation
(
	playerInfo_t	*pi,
	const char		*legsAnim,
	const char		*torsoAnim,
	qboolean		force,
	qboolean		clearpending
)
{
	lastLegsAnim = pi->legs.animation = CG_GetLimboAnimation( pi, legsAnim );
	lastTorsoAnim = pi->torso.animation = CG_GetLimboAnimation( pi, torsoAnim );
	if ( force )
	{
		pi->legs.oldFrame = pi->legs.frame = pi->legs.animation->firstFrame;
		pi->torso.oldFrame = pi->torso.frame = pi->torso.animation->firstFrame;
		pi->legs.frameTime = cg.time;
		pi->torso.frameTime = cg.time;
		pi->legs.oldFrameModel = pi->legs.frameModel = pi->legs.animation->mdxFile;
		pi->torso.oldFrameModel = pi->torso.frameModel = pi->torso.animation->mdxFile;
		pi->numPendingAnimations = 0;
	}
	else
	{
		pi->legs.oldFrame = pi->legs.frame;
		pi->legs.oldFrameModel = pi->legs.frameModel;
		pi->legs.frame = pi->legs.animation->firstFrame;
		pi->torso.oldFrame = pi->torso.frame;
		pi->torso.oldFrameModel = pi->torso.frameModel;
		pi->torso.frame = pi->torso.animation->firstFrame;
		pi->legs.frameTime += 200;	// Give them some time to lerp between animations
		pi->torso.frameTime += 200;
	}

	if ( clearpending )
	{
		pi->numPendingAnimations = 0;
	}
}


/* */
void
CG_MenuPendingAnimation ( playerInfo_t *pi, const char *legsAnim, const char *torsoAnim, int delay )
{
	if ( pi->numPendingAnimations >= 4 )
	{
		return;
	}

	if ( !pi->numPendingAnimations )
	{
		pi->pendingAnimations[pi->numPendingAnimations].pendingAnimationTime = cg.time + delay;
	}
	else
	{
		pi->pendingAnimations[pi->numPendingAnimations].pendingAnimationTime = pi->pendingAnimations[pi->
				numPendingAnimations - 1].pendingAnimationTime +
			delay;
	}

	pi->pendingAnimations[pi->numPendingAnimations].pendingLegsAnim = legsAnim;
	pi->pendingAnimations[pi->numPendingAnimations].pendingTorsoAnim = torsoAnim;
	lastLegsAnim = CG_GetLimboAnimation( pi, legsAnim );
	lastTorsoAnim = CG_GetLimboAnimation( pi, torsoAnim );
	pi->numPendingAnimations++;
}


/* */
void
CG_MenuCheckPendingAnimation ( playerInfo_t *pi )
{
	int i;
	if ( pi->numPendingAnimations <= 0 )
	{
		return;
	}

	if ( pi->pendingAnimations[0].pendingAnimationTime && pi->pendingAnimations[0].pendingAnimationTime < cg.time )
	{
		CG_MenuSetAnimation( pi, pi->pendingAnimations[0].pendingLegsAnim, pi->pendingAnimations[0].pendingTorsoAnim,
							 qfalse, qfalse );
		for ( i = 0; i < 3; i++ )
		{
			memcpy( &pi->pendingAnimations[i], &pi->pendingAnimations[i + 1], sizeof(pendingAnimation_t) );
		}

		pi->numPendingAnimations--;
	}
}


/* */
void
CG_SetHudHeadLerpFrameAnimation ( bg_character_t *ch, lerpFrame_t *lf, int newAnimation )
{
	animation_t *anim;
	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;
	if ( newAnimation < 0 || newAnimation >= MAX_HD_ANIMATIONS )
	{
		CG_Error( "Bad animation number (CG_SetHudHeadLerpFrameAnimation): %i", newAnimation );
	}

	anim = &ch->hudheadanimations[newAnimation];
	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;
}


/* */
void
CG_ClearHudHeadLerpFrame ( bg_character_t *ch, lerpFrame_t *lf, int animationNumber )
{
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetHudHeadLerpFrameAnimation( ch, lf, animationNumber );
	lf->oldFrame = lf->frame = lf->animation->firstFrame;
	lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
}


/* */
void
CG_RunHudHeadLerpFrame ( bg_character_t *ch, lerpFrame_t *lf, int newAnimation, float speedScale )
{
	int			f;
	animation_t *anim;

	// see if the animation sequence is switching
	if ( !lf->animation )
	{
		CG_ClearHudHeadLerpFrame( ch, lf, newAnimation );
	}
	else if ( newAnimation != lf->animationNumber )
	{
		CG_SetHudHeadLerpFrameAnimation( ch, lf, newAnimation );
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	if ( cg.time >= lf->frameTime )
	{
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;

		// get the next frame based on the animation
		anim = lf->animation;
		if ( !anim->frameLerp )
		{
			return;								// shouldn't happen
		}

		if ( cg.time < lf->animationTime )
		{
			lf->frameTime = lf->animationTime;	// initial lerp
		}
		else
		{
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;
		}

		f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		f *= speedScale;						// adjust for haste, etc
		if ( f >= anim->numFrames )
		{
			f -= anim->numFrames;
			if ( anim->loopFrames )
			{
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			}
			else
			{
				f = anim->numFrames - 1;

				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = cg.time;
			}
		}

		lf->frame = anim->firstFrame + f;
		lf->frameModel = anim->mdxFile;
		if ( cg.time > lf->frameTime )
		{
			lf->frameTime = cg.time;
		}
	}

	if ( lf->frameTime > cg.time + 200 )
	{
		lf->frameTime = cg.time;
	}

	if ( lf->oldFrameTime > cg.time )
	{
		lf->oldFrameTime = cg.time;
	}

	// calculate current lerp value
	if ( lf->frameTime == lf->oldFrameTime )
	{
		lf->backlerp = 0;
	}
	else
	{
		lf->backlerp = 1.0 - (float) ( cg.time - lf->oldFrameTime ) / ( lf->frameTime - lf->oldFrameTime );
	}
}


/* */
void
CG_HudHeadAnimation
(
	bg_character_t		*ch,
	lerpFrame_t			*lf,
	int					*oldframe,
	int					*frame,
	float				*backlerp,
	hudHeadAnimNumber_t animation
)
{

	//	centity_t *cent = &cg.predictedPlayerEntity;
	CG_RunHudHeadLerpFrame( ch, lf, (int) animation, 1.f );
	*oldframe = lf->oldFrame;
	*frame = lf->frame;
	*backlerp = lf->backlerp;
}
