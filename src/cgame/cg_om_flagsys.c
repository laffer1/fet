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
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
Unique1's Client Game FlagSys Code.
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
CG_FlagSetAnim
Flag_RunLerpFrame
CG_Flag
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
*/
#include "../game/q_global_defines.h"
#ifdef __SUPREMACY__
#include "cg_local.h"
extern float	VectorDistance ( vec3_t v1, vec3_t v2 );
int				capture_start_time = 0;

// JPW NERVE moved these up
#define WCP_ANIM_NOFLAG				0
#define WCP_ANIM_RAISE_AXIS			1
#define WCP_ANIM_RAISE_AMERICAN		2
#define WCP_ANIM_AXIS_RAISED		3
#define WCP_ANIM_AMERICAN_RAISED	4
#define WCP_ANIM_AXIS_TO_AMERICAN	5
#define WCP_ANIM_AMERICAN_TO_AXIS	6
#define WCP_ANIM_AXIS_FALLING		7
#define WCP_ANIM_AMERICAN_FALLING	8
#ifdef __SUPREMACY__
#define WCP_ANIM_RAISE_NEUTRAL			9
#define WCP_ANIM_NEUTRAL_RAISED			10
#define WCP_ANIM_AMERICAN_TO_NEUTRAL	11
#define WCP_ANIM_AXIS_TO_NEUTRAL		12
#define WCP_ANIM_NEUTRAL_TO_AMERICAN	13
#define WCP_ANIM_NEUTRAL_TO_AXIS		14
#define WCP_ANIM_NEUTRAL_FALLING		15
#endif

// DHM - Nerve :: capture and hold flag
static animation_t	multi_flagpoleAnims2[] =
{
	{ 0, "", 0, 1, 0, 1000 / 15, 1000 / 15 },

	// (no flags, idle)
	{ 0, "", 0, 15, 0, 5000 / 15, 5000 / 15 },

	// (axis flag rising)
	{ 0, "", 490, 15, 0, 5000 / 15, 5000 / 15 },

	// (american flag rising)
	{ 0, "", 20, 211, 211, 1000 / 15, 1000 / 15 },

	// (axis flag raised)
	{ 0, "", 255, 211, 211, 1000 / 15, 1000 / 15 },

	// (american flag raised)
	{ 0, "", 235, 15, 0, 5000 / 15, 5000 / 15 },

	// (axis switching to american)
	{ 0, "", 470, 15, 0, 5000 / 15, 5000 / 15 },

	// (american switching to axis)
	{ 0, "", 510, 15, 0, 5000 / 15, 5000 / 15 },

	// (axis flag falling)
	{ 0, "", 530, 15, 0, 5000 / 15, 5000 / 15 } // (american flag falling)
};


/*
===============
Flag_RunLerpFrame

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
void
Flag_RunLerpFrame ( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, float speedScale )
{
	int			f;
	animation_t *anim;

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

			//CG_Printf("No frameLerp!\n");
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

			//			if( cg_debugAnims.integer ) {
			//				CG_Printf( "Clamp lf->frameTime\n");
			//			}
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
==============
CG_FlagSetAnim
==============
*/
static void
CG_FlagSetAnim ( centity_t *cent, lerpFrame_t *lf, int newAnim )
{	// transition animation
	lf->animationNumber = cent->currentState.frame;
	lf->animation = &multi_flagpoleAnims2[cent->currentState.frame];
	lf->animationTime = lf->frameTime + lf->animation->initialLerp;
}


/*
===============
CG_Flag
===============
*/
void
CG_Flag ( centity_t *cent )
{
	refEntity_t		model;
	lerpFrame_t		*traplf;
	entityState_t	*cs;
	playerState_t	*ps;
	int				captureEntityNum;
	cs = &cent->currentState;
	ps = &cg.predictedPlayerState;
	captureEntityNum = ps->stats[STAT_CAPTURE_ENTITYNUM];
	cg.captureEntityNum = ps->stats[STAT_CAPTURE_ENTITYNUM];
	if ( !ps || !ps->stats[STAT_FLAG_CAPTURE] )
	{

	}
	else if
		(
			ps->stats[STAT_FLAG_CAPTURE] > 0 &&
			ps->stats[STAT_CAPTURE_ENTITYNUM] == cent->currentState.number &&
			ps->stats[STAT_CAPTURE_ENTITYNUM] > 0
		)
	{	// We're taking this flag!
		cg.captureFlagPercent = cent->currentState.time2;
		cg.capturingFlag = qtrue;
		capture_start_time = cg.time;
	}
	else if
		(
			ps->stats[STAT_FLAG_CAPTURE] > 0 &&
			ps->stats[STAT_CAPTURE_ENTITYNUM] != cent->currentState.number &&
			ps->stats[STAT_CAPTURE_ENTITYNUM] > 0
		)
	{	// We're taking another flag!
		cg.capturingFlag = qtrue;
	}
	else
	{	// No capturing in process!
		cg.captureFlagPercent = 0;
		cg.capturingFlag = qfalse;
	}

	if ( !ps || !ps->stats[STAT_FLAG_CAPTURE] )
	{

	}
	else if
		(
			ps->stats[STAT_CAPTURE_ENTITYNUM] > 0 &&
			VectorDistance( cg.refdef.vieworg, cg_entities[ps->stats[STAT_CAPTURE_ENTITYNUM]].lerpOrigin) >
				(float) cg_entities[captureEntityNum].currentState.effect1Time * 0.5
		)
	{
		cg.captureFlagPercent = 0;
		cg.capturingFlag = qfalse;
	}

	if ( cg.captureFlagPercent < 0 )
	{
		cg.captureFlagPercent = 0;
	}

	// show the flag base
	memset( &model, 0, sizeof(model) );
	model.reType = RT_MODEL;
	VectorCopy( cent->lerpOrigin, model.lightingOrigin );
	VectorCopy( cent->lerpOrigin, model.origin );
	VectorCopy( cent->lerpOrigin, model.oldorigin );
	AnglesToAxis( cent->currentState.angles, model.axis );
	if ( cent->currentState.modelindex == TEAM_AXIS )
	{	// s.frame is the animation number
		if ( cent->om_flag_frame == WCP_ANIM_NOFLAG )
		{
			model.frame = WCP_ANIM_RAISE_AXIS;
			trap_S_StartSound( model.origin, cent->currentState.number, CHAN_AUTO,
							   trap_S_RegisterSound( "sound/movers/doors/door6_open.wav", qfalse) );
		}
		else if ( cg.time < cent->lerpFrame.frameTime && cent->om_flag_frame == WCP_ANIM_RAISE_AXIS )
		{
			model.frame = WCP_ANIM_RAISE_AXIS;
		}
		else
		{
			model.frame = WCP_ANIM_AXIS_RAISED;
		}
	}
	else if ( cent->currentState.modelindex == TEAM_ALLIES )
	{	// s.frame is the animation number
		if ( cent->om_flag_frame == WCP_ANIM_NOFLAG )
		{
			model.frame = WCP_ANIM_RAISE_AMERICAN;
			trap_S_StartSound( model.origin, cent->currentState.number, CHAN_AUTO,
							   trap_S_RegisterSound( "sound/movers/doors/door6_open.wav", qfalse) );
		}
		else if ( cg.time < cent->lerpFrame.frameTime && cent->om_flag_frame == WCP_ANIM_RAISE_AMERICAN )
		{
			model.frame = WCP_ANIM_RAISE_AMERICAN;
		}
		else
		{
			model.frame = WCP_ANIM_AMERICAN_RAISED;
		}
	}
	else if ( cent->currentState.modelindex == TEAM_FREE )
	{	// s.frame is the animation number
		model.frame = WCP_ANIM_NOFLAG;
	}
	else
	{	// s.frame is the animation number
		if
		(
			cg.time >= cent->lerpFrame.frameTime &&
			(cent->om_flag_frame == WCP_ANIM_AMERICAN_FALLING || cent->om_flag_frame == WCP_ANIM_AXIS_FALLING)
		)
		{
			model.frame = WCP_ANIM_NOFLAG;
		}
		else if ( cent->om_flag_frame == WCP_ANIM_AMERICAN_FALLING )
		{
			model.frame = WCP_ANIM_AMERICAN_FALLING;
		}
		else if ( cent->om_flag_frame == WCP_ANIM_AXIS_FALLING )
		{
			model.frame = WCP_ANIM_AXIS_FALLING;
		}
		else if ( cg.time >= cent->lerpFrame.frameTime && cent->om_flag_frame == WCP_ANIM_AMERICAN_RAISED )
		{
			model.frame = WCP_ANIM_AMERICAN_FALLING;
			trap_S_StartSound( model.origin, cent->currentState.number, CHAN_AUTO,
							   trap_S_RegisterSound( "sound/movers/doors/door6_open.wav", qfalse) );
		}
		else if ( cg.time >= cent->lerpFrame.frameTime && cent->om_flag_frame == WCP_ANIM_AXIS_RAISED )
		{
			model.frame = WCP_ANIM_AXIS_FALLING;
			trap_S_StartSound( model.origin, cent->currentState.number, CHAN_AUTO,
							   trap_S_RegisterSound( "sound/movers/doors/door6_open.wav", qfalse) );
		}
		else
		{
			model.frame = WCP_ANIM_NOFLAG;
		}
	}

	cs->frame = model.frame;
	cent->om_flag_frame = model.frame;
	if ( !cent->currentState.modelindex2 )
	{
		cent->currentState.modelindex2 = trap_R_RegisterModel( "models/multiplayer/flagpole/flagpole.md3" );
	}

	model.hModel = cgs.gameModels[cent->currentState.modelindex2];	//trap_R_RegisterModel( "models/multiplayer/flagpole/flagpole.md3" );

	//model.hModel = trap_R_RegisterModel( "models/multiplayer/flagpole/flagpole.md3" );
	//	if (!model.hModel)
	//		model.hModel = trap_R_RegisterModel( "models/multiplayer/flagpole/flagpole.md3" );
	if ( cent->currentState.weapon )
	{																// We have animated models.
		traplf = &cent->lerpFrame;

		// initial setup
		if ( !traplf->oldFrameTime )
		{
			traplf->frameTime = traplf->oldFrameTime = cg.time;
			CG_FlagSetAnim( cent, traplf, cs->frame );
			traplf->frame = traplf->oldFrame = traplf->animation->firstFrame;
		}

		// transition to new anim if requested
		if ( (traplf->animationNumber != cs->frame) || !traplf->animation )
		{
			CG_FlagSetAnim( cent, traplf, cs->frame );

			//CG_Printf("Transition to anim %s (%i). cs->frame: (%i) model.frame: (%i)!\n", traplf->animation->animationName, traplf->animationNumber, cs->frame, model.frame);
		}

		Flag_RunLerpFrame( cent, NULL, traplf, 0, 1 );				// Lerp the flag for anims...
		model.frame = traplf->frame;
		model.oldframe = traplf->oldFrame;
		model.backlerp = traplf->backlerp;
	}

	//CG_Printf("Drawing flag! modelindex is %i\n", cent->currentState.modelindex2);
#ifdef __NPC__
	if ( cent->currentState.eType != ET_NPC )
	{																//do not do this for NPCs
#endif
		trap_R_AddRefEntityToScene( &model );
#ifdef __NPC__
	}
#endif
}
#endif //__SUPREMACY__
