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


// cg_view.c -- setup all the parameters (position, angle, etc)
// for a 3D rendering
#include "cg_local.h"

//========================
extern pmove_t	cg_pmove;

//========================

/*
=============================================================================

  MODEL TESTING

The viewthing and gun positioning tools from Q2 have been integrated and
enhanced into a single model testing facility.

Model viewing can begin with either "testmodel <modelname>" or "testgun <modelname>".

The names must be the full pathname after the basedir, like 
"models/weapons/v_launch/tris.md3" or "players/male/tris.md3"

Testmodel will create a fake entity 100 units in front of the current view
position, directly facing the viewer.  It will remain immobile, so you can
move around it to view it from different angles.

Testgun will cause the model to follow the player around and supress the real
view weapon model.  The default frame 0 of most guns is completely off screen,
so you will probably have to cycle a couple frames to see it.

"nextframe", "prevframe", "nextskin", and "prevskin" commands will change the
frame or skin of the testmodel.  These are bound to F5, F6, F7, and F8 in
q3default.cfg.

If a gun is being tested, the "gun_x", "gun_y", and "gun_z" variables will let
you adjust the positioning.

Note that none of the model testing features update while the game is paused, so
it may be convenient to test with deathmatch set to 1 so that bringing down the
console doesn't pause the game.

=============================================================================
*/


/*
=================
CG_TestModel_f

Creates an entity in front of the current position, which
can then be moved around
=================
*/
void
CG_TestModel_f ( void )
{
	vec3_t	angles;
	memset( &cg.testModelEntity, 0, sizeof(cg.testModelEntity) );
	if ( trap_Argc() < 2 )
	{
		return;
	}

	Q_strncpyz( cg.testModelName, CG_Argv( 1), MAX_QPATH );
	cg.testModelEntity.hModel = trap_R_RegisterModel( cg.testModelName );
	if ( trap_Argc() == 3 )
	{
		cg.testModelEntity.backlerp = atof( CG_Argv( 2) );
		cg.testModelEntity.frame = 1;
		cg.testModelEntity.oldframe = 0;
	}

	if ( !cg.testModelEntity.hModel )
	{
		CG_Printf( "Can't register model\n" );
		return;
	}

	VectorMA( cg.refdef.vieworg, 100, cg.refdef.viewaxis[0], cg.testModelEntity.origin );
	angles[PITCH] = 0;
	angles[YAW] = 180 + cg.refdefViewAngles[1];
	angles[ROLL] = 0;
	AnglesToAxis( angles, cg.testModelEntity.axis );
	cg.testGun = qfalse;
}


/*
=================
CG_TestGun_f

Replaces the current view weapon with the given model
=================
*/
void
CG_TestGun_f ( void )
{
	CG_TestModel_f();
	cg.testGun = qtrue;
	cg.testModelEntity.renderfx = RF_MINLIGHT | RF_DEPTHHACK | RF_FIRST_PERSON | RF_NOSHADOW;
}


/* */
void
CG_TestModelNextFrame_f ( void )
{
	cg.testModelEntity.frame++;
	CG_Printf( "frame %i\n", cg.testModelEntity.frame );
}


/* */
void
CG_TestModelPrevFrame_f ( void )
{
	cg.testModelEntity.frame--;
	if ( cg.testModelEntity.frame < 0 )
	{
		cg.testModelEntity.frame = 0;
	}

	CG_Printf( "frame %i\n", cg.testModelEntity.frame );
}


/* */
void
CG_TestModelNextSkin_f ( void )
{
	cg.testModelEntity.skinNum++;
	CG_Printf( "skin %i\n", cg.testModelEntity.skinNum );
}


/* */
void
CG_TestModelPrevSkin_f ( void )
{
	cg.testModelEntity.skinNum--;
	if ( cg.testModelEntity.skinNum < 0 )
	{
		cg.testModelEntity.skinNum = 0;
	}

	CG_Printf( "skin %i\n", cg.testModelEntity.skinNum );
}


/* */
static void
CG_AddTestModel ( void )
{
	int i;

	// re-register the model, because the level may have changed
	cg.testModelEntity.hModel = trap_R_RegisterModel( cg.testModelName );
	if ( !cg.testModelEntity.hModel )
	{
		CG_Printf( "Can't register model\n" );
		return;
	}

	// if testing a gun, set the origin reletive to the view origin
	if ( cg.testGun )
	{
		VectorCopy( cg.refdef.vieworg, cg.testModelEntity.origin );
		VectorCopy( cg.refdef.viewaxis[0], cg.testModelEntity.axis[0] );
		VectorCopy( cg.refdef.viewaxis[1], cg.testModelEntity.axis[1] );
		VectorCopy( cg.refdef.viewaxis[2], cg.testModelEntity.axis[2] );

		// allow the position to be adjusted
		for ( i = 0; i < 3; i++ )
		{
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[0][i] * cg_gun_x.value;
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[1][i] * cg_gun_y.value;
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[2][i] * cg_gun_z.value;
		}
	}

	trap_R_AddRefEntityToScene( &cg.testModelEntity );
}

//============================================================================

/*
=================
CG_CalcVrect

Sets the coordinates of the rendered window
=================
*/


//static float letterbox_frac = 1.0f;	// used for transitioning to letterbox for cutscenes // TOD-O: add to cg. // TTimo: unused
void
CG_Letterbox ( float xsize, float ysize, qboolean center )
{

	// normal aspect is xx:xx
	// letterbox is yy:yy  (85% of 'normal' height)
	if ( cg_letterbox.integer )
	{
		float	lbheight, lbdiff;
		lbheight = ysize * 0.85;
		lbdiff = ysize - lbheight;
		if ( !center )
		{
			int offset = ( cgs.glconfig.vidHeight * (.5f * lbdiff) ) / 100;
			offset &= ~1;
			cg.refdef.y += offset;
		}

		ysize = lbheight;
	}

	cg.refdef.width = cgs.glconfig.vidWidth * xsize / 100;
	cg.refdef.width &= ~1;
	cg.refdef.height = cgs.glconfig.vidHeight * ysize / 100;
	cg.refdef.height &= ~1;
	if ( center )
	{
		cg.refdef.x = ( cgs.glconfig.vidWidth - cg.refdef.width ) / 2;
		cg.refdef.y = ( cgs.glconfig.vidHeight - cg.refdef.height ) / 2;
	}
}


/* */
float
HeightDistance ( vec3_t v1, vec3_t v2 )
{
	vec3_t	dir;
	vec3_t	v1a, v2a;
	VectorCopy( v1, v1a );
	VectorCopy( v2, v2a );
	v2a[0] = v1a[0];
	v2a[1] = v1a[1];
	VectorSubtract( v2a, v1a, dir );
	return ( VectorLength( dir) );
}


/* */
static void
CG_CalcVrect ( void )
{
	if ( cg.showGameView && cg_bigLimbo.integer )
	{
		float	x, y, w, h;
		x = LIMBO_3D_X;
		y = LIMBO_3D_Y;
		w = LIMBO_3D_W;
		h = LIMBO_3D_H;
		CG_AdjustFrom640( &x, &y, &w, &h );
		cg.refdef.x = x;
		cg.refdef.y = y;
		cg.refdef.width = w;
		cg.refdef.height = h;
		CG_Letterbox( (LIMBO_3D_W / 640.f) * 100, (LIMBO_3D_H / 480.f) * 100, qfalse );
		return;
	}

	CG_Letterbox( 100, 100, qtrue );
}

/*
===============
CG_OffsetThirdPersonView

===============
*/
#define FOCUS_DISTANCE	cg_3rdPersonFocus.integer	//800	//512


/* */
void
CG_OffsetThirdPersonView ( void )
{
	vec3_t			forward, right, up;
	vec3_t			view;
	vec3_t			focusAngles;
	trace_t			trace;
	static vec3_t	mins = { -4, -4, -4 };
	static vec3_t	maxs = { 4, 4, 4 };
	vec3_t			focusPoint;
	float			focusDist;
	float			forwardScale, sideScale;
	if ( cg_3rdPersonAngle.value < 0 )
	{
		trap_Cvar_Set( "cg_3rdPersonAngle", "0" );
		CG_Printf( "Values under zero are not allowed for cg_3rdpersonAngle\n" );
	}

	if ( cg_3rdPersonAngle.value > cg_3rdPerson_maxangle.integer )
	{
		trap_Cvar_Set( "cg_3rdPersonAngle", va( "%i", cg_3rdPerson_maxangle.integer) );
		CG_Printf( "Maximal value for cg_3rdPersonAngle is on this server is ^3'%i'.\n", cg_3rdPerson_maxangle.integer );
	}

	if ( cg_3rdPersonRange.value < 0 )
	{
		trap_Cvar_Set( "cg_3rdPersonRange", "80" );
		CG_Printf( "Values under zero are not allowed for cg_3rdpersonRange\n" );
	}

	if ( cg_3rdPersonRange.value > cg_3rdPerson_maxrange.integer )
	{
		trap_Cvar_Set( "cg_3rdPersonRange", va( "%i", cg_3rdPerson_maxrange.integer) );
		CG_Printf( "Maximal value for cg_3rdPersonRange is on this server is ^3'%i'.\n", cg_3rdPerson_maxrange.integer );
	}

	cg.refdef_current->vieworg[2] += cg.predictedPlayerState.viewheight;
	VectorCopy( cg.refdefViewAngles, focusAngles );

	// rain - if dead, look at medic or allow freelook if none in range
	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 )
	{

		// rain - #254 - force yaw to 0 if we're tracking a medic
		if ( cg.snap->ps.viewlocked != 7 )
		{

			// rain - do short2angle AFTER the network part
			focusAngles[YAW] = SHORT2ANGLE( cg.predictedPlayerState.stats[STAT_DEAD_YAW] );
			cg.refdefViewAngles[YAW] = SHORT2ANGLE( cg.predictedPlayerState.stats[STAT_DEAD_YAW] );
		}
	}

	// jaquboss we nead leaning in thirdperson too...
	if ( cg.predictedPlayerState.leanf != 0 )
	{

		//add leaning offset
		vec3_t	right;
		cg.refdefViewAngles[2] += cg.predictedPlayerState.leanf / 2.0f;
		AngleVectors( cg.refdefViewAngles, NULL, right, NULL );
		VectorMA( cg.refdef_current->vieworg, cg.predictedPlayerState.leanf, right, cg.refdef_current->vieworg );
	}

	//focusAngles[PITCH] = 90;
	if ( focusAngles[PITCH] > 45 )
	{
		focusAngles[PITCH] = 45;	// don't go too far overhead
	}

	AngleVectors( focusAngles, forward, NULL, NULL );
	if ( cg_3rdPerson.integer == 2 )
	{
		VectorCopy( cg.predictedPlayerState.origin, focusPoint );
	}
	else
	{
		VectorMA( cg.refdef_current->vieworg, FOCUS_DISTANCE, forward, focusPoint );
	}

	VectorCopy( cg.refdef_current->vieworg, view );
	if ( cg.snap->ps.eFlags & EF_TANK )
	{
		view[2] += 32;
		if ( cg.snap->ps.persistant[PERS_TANKUSED] == 2 )
		{							// another 32 points up for panther
			view[2] += 32;
		}
	}
	else
	{
		view[2] += 8;
	}

	cg.refdefViewAngles[PITCH] *= 0.5;
	AngleVectors( cg.refdefViewAngles, forward, right, up );
	forwardScale = cos( cg_3rdPersonAngle.value / 180 * M_PI );
	sideScale = sin( cg_3rdPersonAngle.value / 180 * M_PI );
	if ( cg.snap->ps.eFlags & EF_TANK )
	{
		VectorMA( view, -cg_3rdPersonRange.value * 2.5 * forwardScale, forward, view );
		VectorMA( view, -cg_3rdPersonRange.value * 2.5 * sideScale, right, view );
	}
	else
	{
		VectorMA( view, -cg_3rdPersonRange.value * forwardScale, forward, view );
		VectorMA( view, -cg_3rdPersonRange.value * sideScale, right, view );
	}

	// trace a ray from the origin to the viewpoint to make sure the view isn't
	// in a solid block.  Use an 8 by 8 block to prevent the view from near clipping anything
	CG_Trace( &trace, cg.refdef_current->vieworg, mins, maxs, view, cg.predictedPlayerState.clientNum, MASK_SOLID );
	if ( trace.fraction != 1.0 )
	{
		VectorCopy( trace.endpos, view );
		view[2] += ( 1.0 - trace.fraction ) * 32;

		// try another trace to this position, because a tunnel may have the ceiling
		// close enogh that this is poking out
		CG_Trace( &trace, cg.refdef_current->vieworg, mins, maxs, view, cg.predictedPlayerState.clientNum, MASK_SOLID );
		VectorCopy( trace.endpos, view );
	}

	VectorCopy( view, cg.refdef_current->vieworg );

	// select pitch to look at focus point from vieword
	VectorSubtract( focusPoint, cg.refdef_current->vieworg, focusPoint );
	focusDist = sqrt( focusPoint[0] * focusPoint[0] + focusPoint[1] * focusPoint[1] );
	if ( focusDist < 1 )
	{
		focusDist = 1;				// should never happen
	}

	cg.refdefViewAngles[PITCH] = -180 / M_PI * atan2( focusPoint[2], focusDist );
	cg.refdefViewAngles[YAW] -= cg_3rdPersonAngle.value;
}


// this causes a compiler bug on mac MrC compiler
static void
CG_StepOffset ( void )
{
	int timeDelta;

	// smooth out stair climbing
	timeDelta = cg.time - cg.stepTime;

	// Ridah
	if ( timeDelta < 0 )
	{
		cg.stepTime = cg.time;
	}

	if ( timeDelta < STEP_TIME )
	{
		cg.refdef_current->vieworg[2] -= cg.stepChange * ( STEP_TIME - timeDelta ) / STEP_TIME;
	}
}


/*
================
CG_KickAngles
================
*/
void
CG_KickAngles ( void )
{
	const vec3_t	centerSpeed = { 2400, 2400, 2400 };
	const float		recoilCenterSpeed = 200;
	const float		recoilIgnoreCutoff = 15;
	const float		recoilMaxSpeed = 50;
	const vec3_t	maxKickAngles = { 10, 10, 10 };
	float			idealCenterSpeed, kickChange;
	int				i, frametime, t;
	float			ft;
#define STEP	20
	char			buf[32];				// NERVE - SMF
	cg.recoilPitchAngle = 0;
	for ( t = cg.frametime; t > 0; t -= STEP )
	{
		if ( t > STEP )
		{
			frametime = STEP;
		}
		else
		{
			frametime = t;
		}

		ft = ( (float) frametime / 1000 );

		// kickAngles is spring-centered
		for ( i = 0; i < 3; i++ )
		{
			if ( cg.kickAVel[i] || cg.kickAngles[i] )
			{

				// apply centering forces to kickAvel
				if ( cg.kickAngles[i] && frametime )
				{
					idealCenterSpeed = -( 2.0 * (cg.kickAngles[i] > 0) - 1.0 ) * centerSpeed[i];
					if ( idealCenterSpeed )
					{
						cg.kickAVel[i] += idealCenterSpeed * ft;
					}
				}

				// add the kickAVel to the kickAngles
				kickChange = cg.kickAVel[i] * ft;
				if ( cg.kickAngles[i] && (cg.kickAngles[i] < 0) != (kickChange < 0) )
				{							// slower when returning to center
					kickChange *= 0.06;
				}

				// check for crossing back over the center point
				if ( !cg.kickAngles[i] || ((cg.kickAngles[i] + kickChange) < 0) == (cg.kickAngles[i] < 0) )
				{
					cg.kickAngles[i] += kickChange;
					if ( !cg.kickAngles[i] && frametime )
					{
						cg.kickAVel[i] = 0;
					}
					else if ( fabs( cg.kickAngles[i]) > maxKickAngles[i] )
					{
						cg.kickAngles[i] = maxKickAngles[i] * ( (2 * (cg.kickAngles[i] > 0)) - 1 );
						cg.kickAVel[i] = 0; // force Avel to return us to center rather than keep going outside range
					}
				}
				else
				{							// about to cross, so just zero it out
					cg.kickAngles[i] = 0;
					cg.kickAVel[i] = 0;
				}
			}
		}

		// recoil is added to input viewangles per frame
		if ( cg.recoilPitch )
		{

			// apply max recoil
			if ( fabs( cg.recoilPitch) > recoilMaxSpeed )
			{
				if ( cg.recoilPitch > 0 )
				{
					cg.recoilPitch = recoilMaxSpeed;
				}
				else
				{
					cg.recoilPitch = -recoilMaxSpeed;
				}
			}

			// apply centering forces to kickAvel
			if ( frametime )
			{
				idealCenterSpeed = -( 2.0 * (cg.recoilPitch > 0) - 1.0 ) * recoilCenterSpeed * ft;
				if ( idealCenterSpeed )
				{
					if ( fabs( idealCenterSpeed) < fabs( cg.recoilPitch) )
					{
						cg.recoilPitch += idealCenterSpeed;
					}
					else
					{						// back zero out
						cg.recoilPitch = 0;
					}
				}
			}
		}

		if ( fabs( cg.recoilPitch) > recoilIgnoreCutoff )
		{
			cg.recoilPitchAngle += cg.recoilPitch * ft;
		}
	}

	// NERVE - SMF - only change cg_recoilPitch cvar when we need to
	trap_Cvar_VariableStringBuffer( "cg_recoilPitch", buf, sizeof(buf) );
	if ( atof( buf) != cg.recoilPitchAngle )
	{

		// encode the kick angles into a 24bit number, for sending to the client exe
		trap_Cvar_Set( "cg_recoilPitch", va( "%f", cg.recoilPitchAngle) );
	}
}


/*
CG_Concussive
*/
void
CG_Concussive ( centity_t *cent )
{
	float	length;
	vec3_t	vec;

	//
	float	pitchRecoilAdd, pitchAdd;
	float	yawRandom;
	vec3_t	recoil;

	//
	if ( !cg.renderingThirdPerson && cent->currentState.density == cg.snap->ps.clientNum )
	{

		//
		pitchRecoilAdd = 0;
		pitchAdd = 0;
		yawRandom = 0;

		//
		VectorSubtract( cg.snap->ps.origin, cent->currentState.origin, vec );
		length = VectorLength( vec );

		// pitchAdd = 12+rand()%3;
		// yawRandom = 6;
		if ( length > 1024 )
		{
			return;
		}

		pitchAdd = ( 32 / length ) * 64;
		yawRandom = ( 32 / length ) * 64;

		// recoil[YAW] = crandom()*yawRandom;
		if ( rand() % 100 > 50 )
		{
			recoil[YAW] = -yawRandom;
		}
		else
		{
			recoil[YAW] = yawRandom;
		}

		recoil[ROLL] = -recoil[YAW];	// why not
		recoil[PITCH] = -pitchAdd;

		// scale it up a bit (easier to modify this while tweaking)
		VectorScale( recoil, 30, recoil );

		// set the recoil
		VectorCopy( recoil, cg.kickAVel );

		// set the recoil
		cg.recoilPitch -= pitchRecoilAdd;
	}
}

/*
==============
CG_ZoomSway
	sway for scoped weapons.
	this takes aimspread into account so the view settles after a bit
==============
*/
#ifndef PMOVE_SWAY


/* */
static void
CG_ZoomSway ( void )
{
	float	spreadfrac;
	float	phase;
	float	pitchAmp, yawAmp;
	float	pitchMinAmp, yawMinAmp;
	if ( !(cg.snap->ps.eFlags & EF_AIMING || cg.snap->ps.eFlags & EF_ZOOMING) )
	{
		return;
	}

	if
	(
		cg.snap->ps.eFlags & EF_MG42_ACTIVE ||
		cg.snap->ps.eFlags & EF_BROWNING_ACTIVE ||
		cg.snap->ps.eFlags & EF_AAGUN_ACTIVE
	)
	{	// don't draw when on mg_42
		return;
	}

	//	spreadfrac = ent->client->currentAimSpreadScale;
	spreadfrac = (float) cg.snap->ps.aimSpreadScale / 255;
	if ( spreadfrac > 1 )
	{
		spreadfrac = 1;
	}
	else if ( spreadfrac < 0 )
	{
		spreadfrac = 0;
	}

	if
	(
		(BG_IsScopedWeapon2( cg.snap->ps.weapon) && !(cg.snap->ps.eFlags & EF_PRONE)) ||
		(cg.snap->ps.weaponTime > 50 && cg.snap->ps.weaponstate == WEAPON_FIRING)
	)
	{
		pitchAmp = 10 * ZOOM_PITCH_AMPLITUDE;
		yawAmp = 10 * ZOOM_YAW_AMPLITUDE;
		pitchMinAmp = 10 * ZOOM_PITCH_MIN_AMPLITUDE;
		yawMinAmp = 10 * ZOOM_YAW_MIN_AMPLITUDE;
	}
	else
	{
		pitchAmp = 6 * ZOOM_PITCH_AMPLITUDE;
		yawAmp = 6 * ZOOM_YAW_AMPLITUDE;
		pitchMinAmp = 6 * ZOOM_PITCH_MIN_AMPLITUDE;
		yawMinAmp = 6 * ZOOM_YAW_MIN_AMPLITUDE;
	}

	// rotate 'forward' vector by the sway
	phase = cg.time / 1000.0 * ZOOM_PITCH_FREQUENCY * M_PI * 2;
	cg.refdefViewAngles[PITCH] += ZOOM_PITCH_AMPLITUDE * sin( phase ) * ( spreadfrac + pitchMinAmp );
	phase = cg.time / 1000.0 * ZOOM_YAW_FREQUENCY * M_PI * 2;
	cg.refdefViewAngles[YAW] += ZOOM_YAW_AMPLITUDE * sin( phase ) * ( spreadfrac + yawMinAmp );
}
#endif

/*
==============
CG_ScreenSway
 
==============
*/
#if 0	// dropped


/* */
static void
CG_ScreenSway ( void )
{
	float	healthfrac;
	float	phase;
	float	staminafrac;
	if ( cg.zoomval )
	{	// zoomed , so keep it just for zoomsway..
		return;
	}

	if
	(
		cg.snap->ps.eFlags & EF_MG42_ACTIVE ||
		cg.snap->ps.eFlags & EF_BROWNING_ACTIVE ||
		cg.snap->ps.eFlags & EF_AAGUN_ACTIVE
	)
	{	// don't draw when on mg_42
		return;
	}

	if ( cg.snap->ps.stats[STAT_HEALTH] < 1 || cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
	{	// dead man lays still
		return;
	}

	healthfrac = ( (float) 100 / cg.snap->ps.stats[STAT_HEALTH] ) / 1.3f;	// maybe it shouldnt be much reduced , but it shouldnt make player feel drunk..
	if ( healthfrac > 3 )
	{
		healthfrac = 3;
	}

	staminafrac = ( 1 / (cg.pmext.sprintTime / (float) SPRINTTIME) ) * 2;
	if ( staminafrac > 3 )
	{
		staminafrac = 3;
	}

	if ( cg.snap->ps.eFlags & EF_PRONE )
	{
		phase = cg.time / 1000.0 * ZOOM_PITCH_FREQUENCY * M_PI * 2;
		cg.refdefViewAngles[PITCH] += ZOOM_PITCH_AMPLITUDE * sin( phase ) * ( staminafrac + ZOOM_PITCH_MIN_AMPLITUDE ) * ( healthfrac + ZOOM_PITCH_MIN_AMPLITUDE ) * 0.33;
		phase = cg.time / 1000.0 * ZOOM_YAW_FREQUENCY * M_PI * 2;
		cg.refdefViewAngles[YAW] += ZOOM_YAW_AMPLITUDE * sin( phase ) * ( staminafrac + ZOOM_YAW_MIN_AMPLITUDE ) * ( healthfrac + ZOOM_YAW_MIN_AMPLITUDE ) * 0.33;
	}
	else if ( cg.snap->ps.eFlags & EF_CROUCHING )
	{
		phase = cg.time / 1000.0 * ZOOM_PITCH_FREQUENCY * M_PI * 2;
		cg.refdefViewAngles[PITCH] += ZOOM_PITCH_AMPLITUDE * sin( phase ) * ( staminafrac + ZOOM_PITCH_MIN_AMPLITUDE ) * ( healthfrac + ZOOM_PITCH_MIN_AMPLITUDE ) * 0.66;
		phase = cg.time / 1000.0 * ZOOM_YAW_FREQUENCY * M_PI * 2;
		cg.refdefViewAngles[YAW] += ZOOM_YAW_AMPLITUDE * sin( phase ) * ( staminafrac + ZOOM_YAW_MIN_AMPLITUDE ) * ( healthfrac + ZOOM_YAW_MIN_AMPLITUDE ) * 0.66;
	}
	else
	{
		phase = cg.time / 1000.0 * ZOOM_PITCH_FREQUENCY * M_PI * 2;
		cg.refdefViewAngles[PITCH] += ZOOM_PITCH_AMPLITUDE * sin( phase ) * ( staminafrac + ZOOM_PITCH_MIN_AMPLITUDE ) * ( healthfrac + ZOOM_PITCH_MIN_AMPLITUDE );
		phase = cg.time / 1000.0 * ZOOM_YAW_FREQUENCY * M_PI * 2;
		cg.refdefViewAngles[YAW] += ZOOM_YAW_AMPLITUDE * sin( phase ) * ( staminafrac + ZOOM_YAW_MIN_AMPLITUDE ) * ( healthfrac + ZOOM_YAW_MIN_AMPLITUDE );
	}
}
#endif

/*
===============
CG_OffsetFirstPersonView

===============
*/


//extern float tank_slope(centity_t *cent);
static void
CG_OffsetFirstPersonView ( void )
{
	float		*origin;
	float		*angles;
	float		bob;
	float		ratio;
	float		delta;
	float		speed;
	float		f;
	vec3_t		predictedVelocity;
	int			timeDelta;
	qboolean	useLastValidBob = qfalse;
	if ( cg.snap->ps.pm_type == PM_INTERMISSION )
	{
		return;
	}

	origin = cg.refdef_current->vieworg;
	angles = cg.refdefViewAngles;
	if
	(
		(cg.snap->ps.weapon == WP_MOBILE_MG42_SET || cg.snap->ps.weapon == WP_30CAL_SET) &&
		!(cg.snap->ps.eFlags & EF_AIMING)
	)
	{
		float	yawDiff = cg.refdefViewAngles[YAW] - cg.pmext.mountedWeaponAngles[YAW];
		vec3_t	forward, point;
		float	oldZ = origin[2];
		AngleVectors( cg.pmext.mountedWeaponAngles, forward, NULL, NULL );
		if ( yawDiff > 180 )
		{
			yawDiff -= 360;
		}
		else if ( yawDiff < -180 )
		{
			yawDiff += 360;
		}

		VectorMA( origin, 31, forward, point );
		AngleVectors( cg.refdefViewAngles, forward, NULL, NULL );
		VectorMA( point, -32, forward, origin );
		origin[2] = oldZ;
	}
	else if ( cg.snap->ps.weapon == WP_MORTAR_SET )
	{
		float	yawDiff = cg.refdefViewAngles[YAW] - cg.pmext.mountedWeaponAngles[YAW];
		vec3_t	forward, point;
		float	oldZ = origin[2];
		AngleVectors( cg.pmext.mountedWeaponAngles, forward, NULL, NULL );
		if ( yawDiff > 180 )
		{
			yawDiff -= 360;
		}
		else if ( yawDiff < -180 )
		{
			yawDiff += 360;
		}

		VectorMA( origin, 31, forward, point );
		AngleVectors( cg.refdefViewAngles, forward, NULL, NULL );
		VectorMA( point, -32, forward, origin );
		origin[2] = oldZ;
	}

	// Jaquboss , TODO get something like STAT_DEAD_YAW only clientside , or allow it for following players
	// if dead, fix the angle and don't add any kick
	if ( !(cg.snap->ps.pm_flags & PMF_LIMBO) && cg.snap->ps.stats[STAT_HEALTH] <= 0 )
	{
		angles[ROLL] = 40;
		angles[PITCH] = -15;

		// rain - #254 - force yaw to 0 if we're tracking a medic
		// rain - medic tracking doesn't seem to happen in this case?
		if ( cg.snap->ps.viewlocked == 7 )
		{
			angles[YAW] = 0;
		}
		else
		{

			// rain - do short2angle AFTER the network part
			angles[YAW] = SHORT2ANGLE( cg.snap->ps.stats[STAT_DEAD_YAW] );
		}

		origin[2] += cg.predictedPlayerState.viewheight;
		return;
	}

	// add angles based on weapon kick
	VectorAdd( angles, cg.kick_angles, angles );

	// RF, add new weapon kick angles
	CG_KickAngles();
	VectorAdd( angles, cg.kickAngles, angles );

	// RF, pitch is already added
	// add angles based on damage kick
	if ( cg.damageTime )
	{
		ratio = cg.time - cg.damageTime;
		if ( ratio < DAMAGE_DEFLECT_TIME )
		{
			ratio /= DAMAGE_DEFLECT_TIME;
			angles[PITCH] += ratio * cg.v_dmg_pitch;
			angles[ROLL] += ratio * cg.v_dmg_roll;
		}
		else
		{
			ratio = 1.0 - ( ratio - DAMAGE_DEFLECT_TIME ) / DAMAGE_RETURN_TIME;
			if ( ratio > 0 )
			{
				angles[PITCH] += ratio * cg.v_dmg_pitch;
				angles[ROLL] += ratio * cg.v_dmg_roll;
			}
		}
	}

	// add angles based on velocity
	VectorCopy( cg.predictedPlayerState.velocity, predictedVelocity );
	delta = DotProduct( predictedVelocity, cg.refdef_current->viewaxis[0] );

	//	angles[PITCH] += delta * cg_runpitch.value;
	angles[PITCH] += delta * 0.01;
	delta = DotProduct( predictedVelocity, cg.refdef_current->viewaxis[1] );

	//	angles[ROLL] -= delta * cg_runroll.value;
	angles[ROLL] -= delta * 0.01;

	// add angles based on bob
	// make sure the bob is visible even at low speeds
	speed = cg.xyspeed > 200 ? cg.xyspeed : 200;
	if ( !cg.bobfracsin && cg.lastvalidBobfracsin > 0 )
	{

		// 200 msec to get back to center from 1
		// that's 1/200 per msec = 0.005 per msec
		cg.lastvalidBobfracsin -= 0.005 * cg.frametime;
		useLastValidBob = qtrue;
	}

	//	delta = useLastValidBob ? cg.lastvalidBobfracsin * cg_bobpitch.value * speed : cg.bobfracsin * cg_bobpitch.value * speed;
	delta = useLastValidBob ? cg.lastvalidBobfracsin * 0.01 * speed : cg.bobfracsin * 0.01 * speed;
	if ( cg.predictedPlayerState.pm_flags & PMF_DUCKED )
	{
		delta *= 0.8;	// crouching // jaquboss lowered , it was crazy
	}

	angles[PITCH] += delta;

	//	delta = useLastValidBob ? cg.lastvalidBobfracsin * cg_bobroll.value * speed : cg.bobfracsin * cg_bobroll.value * speed;
	delta = useLastValidBob ? cg.lastvalidBobfracsin * 0.005 * speed : cg.bobfracsin * 0.005 * speed;
	if ( cg.predictedPlayerState.pm_flags & PMF_DUCKED )
	{
		delta *= 0.8;	// crouching accentuates roll
	}

	if ( useLastValidBob )
	{
		if ( cg.lastvalidBobcycle & 1 )
		{
			delta = -delta;
		}
	}
	else if ( cg.bobcycle & 1 )
	{
		delta = -delta;
	}

	angles[ROLL] += delta;

	//===================================
	// add view height
	origin[2] += cg.predictedPlayerState.viewheight;

	// smooth out duck height changes
	timeDelta = cg.time - cg.duckTime;
	if ( cg.predictedPlayerState.eFlags & EF_PRONE )
	{
		if ( timeDelta < 0 )
		{				// Ridah
			cg.duckTime = cg.time - PRONE_TIME;
		}

		if ( timeDelta < PRONE_TIME )
		{
			cg.refdef_current->vieworg[2] -= cg.duckChange * ( PRONE_TIME - timeDelta ) / PRONE_TIME;
		}
	}
	else
	{
		if ( timeDelta < 0 )
		{				// Ridah
			cg.duckTime = cg.time - DUCK_TIME;
		}

		if ( timeDelta < DUCK_TIME )
		{
			cg.refdef_current->vieworg[2] -= cg.duckChange * ( DUCK_TIME - timeDelta ) / DUCK_TIME;
		}
	}

	// add bob height
	//	bob = cg.bobfracsin * cg.xyspeed * cg_bobup.value;
	bob = cg.bobfracsin * cg.xyspeed * 0.01;
	if ( bob > 6 )
	{
		bob = 6;
	}

	origin[2] += bob;

	// add fall height
	delta = cg.time - cg.landTime;
	if ( delta < 0 )
	{					// Ridah
		cg.landTime = cg.time - ( LAND_DEFLECT_TIME + LAND_RETURN_TIME );
	}

	if ( delta < LAND_DEFLECT_TIME )
	{
		f = delta / LAND_DEFLECT_TIME;
		cg.refdef_current->vieworg[2] += cg.landChange * f;
	}
	else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME )
	{
		delta -= LAND_DEFLECT_TIME;
		f = 1.0 - ( delta / LAND_RETURN_TIME );
		cg.refdef_current->vieworg[2] += cg.landChange * f;
	}

	if ( cg.snap->ps.eFlags & EF_TANK )
	{
		vec3_t	forward;

		//	cg.refdefViewAngles[0] += tank_slope(&cg_entities[cg.predictedPlayerState.clientNum]);
		switch ( cg.snap->ps.persistant[PERS_TANKUSED] )
		{
			default:
			case 1:		// churchill
				// add turret rotation
				cg.refdefViewAngles[1] = cg.predictedPlayerState.stats[STAT_DEAD_YAW];
				cg.refdef_current->vieworg[2] += 64;
				break;
			case 2:		// jagdpanther
				AngleVectors( cg.refdefViewAngles, forward, NULL, NULL );
				VectorMA( cg.refdef_current->vieworg, 16, forward, cg.refdef_current->vieworg );
				cg.refdef_current->vieworg[2] += 76;
				break;
		}
	}

	// add step offset
	CG_StepOffset();
	CG_ZoomSway();

	//	CG_ScreenSway();
	// adjust for 'lean'
	if ( cg.predictedPlayerState.leanf != 0 )
	{

		//add leaning offset
		vec3_t	right;
		cg.refdefViewAngles[2] += cg.predictedPlayerState.leanf / 2.0f;
		AngleVectors( cg.refdefViewAngles, NULL, right, NULL );
		VectorMA( cg.refdef_current->vieworg, cg.predictedPlayerState.leanf, right, cg.refdef_current->vieworg );
	}

	// add kick offset
	VectorAdd( origin, cg.kick_origin, origin );

	// pivot the eye based on a neck length
#if 0
	{
#define NECK_LENGTH 8
		vec3_t	forward, up;
		cg.refdef_current->vieworg[2] -= NECK_LENGTH;
		AngleVectors( cg.refdefViewAngles, forward, NULL, up );
		VectorMA( cg.refdef_current->vieworg, 3, forward, cg.refdef_current->vieworg );
		VectorMA( cg.refdef_current->vieworg, NECK_LENGTH, up, cg.refdef_current->vieworg );
	}
#endif
	if ( cg.headView && cg_firstPerson.integer > 0 /* && cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR*/ )
	{
		VectorCopy( cg.headOrigin, cg.refdef_current->vieworg );
		if ( cg_firstPerson.integer > 1 )
		{				// copy head angle if
			AxisCopy( cg.headAxis, cg.refdef_current->viewaxis );
		}
		{
			vec3_t	forward, up;
			AngleVectors( cg.refdefViewAngles, forward, NULL, up );
			VectorMA( cg.refdef_current->vieworg, 1, forward, cg.refdef_current->vieworg );
			VectorMA( cg.refdef_current->vieworg, 3, up, cg.refdef_current->vieworg );
		}
	}
}

//======================================================================
//
// Zoom controls
//
// probably move to server variables
float	zoomTable[ZOOM_MAX_ZOOMS][2] =
{

	// max {out,in}
	{ 0, 0 },
	{ 36, 36 },

	//	binoc
	{ 20, 20 },

	//	sniper
	{ 60, 60 },

	//	snooper
	{ 55, 55 },

	//	fg42
	{ 55, 55 },

	//	mg42
	{ 70, 70 }			//	ads
};


/* */
void
CG_AdjustZoomVal ( float val, int type )
{
	cg.zoomval += val;
	if ( cg.zoomval > zoomTable[type][ZOOM_OUT] )
	{
		cg.zoomval = zoomTable[type][ZOOM_OUT];
	}

	if ( cg.zoomval < zoomTable[type][ZOOM_IN] )
	{
		cg.zoomval = zoomTable[type][ZOOM_IN];
	}
}


/* */
void
CG_ZoomIn_f ( void )
{

	// Gordon: fixed being able to "latch" your zoom by weaponcheck + quick zoomin
	// OSP - change for zoom view in demos
	if
	(
		cg_entities[cg.snap->ps.clientNum].currentState.weapon == WP_GARAND &&
		cg_entities[cg.snap->ps.clientNum].currentState.eFlags & EF_AIMING
	)
	{																// can i ask ppl why there is cg_entities used?
		CG_AdjustZoomVal( -(cg_zoomStepSniper.value), ZOOM_SNIPER );
	}
	else if
		(
			cg_entities[cg.snap->ps.clientNum].currentState.weapon == WP_K43 &&
			cg_entities[cg.snap->ps.clientNum].currentState.eFlags & EF_AIMING
		)
	{
		CG_AdjustZoomVal( -(cg_zoomStepSniper.value), ZOOM_SNIPER );
	}
	else if ( cg.zoomedBinoc )
	{
		CG_AdjustZoomVal( -(cg_zoomStepSniper.value), ZOOM_ADS );	// JPW NERVE per atvi request all use same vals to match menu (was zoomStepBinoc, ZOOM_BINOC);
	}
}


/* */
void
CG_ZoomOut_f ( void )
{
	if
	(
		cg_entities[cg.snap->ps.clientNum].currentState.weapon == WP_GARAND &&
		cg_entities[cg.snap->ps.clientNum].currentState.eFlags & EF_AIMING
	)
	{
		CG_AdjustZoomVal( cg_zoomStepSniper.value, ZOOM_SNIPER );
	}
	else if
		(
			cg_entities[cg.snap->ps.clientNum].currentState.weapon == WP_K43 &&
			cg_entities[cg.snap->ps.clientNum].currentState.eFlags & EF_AIMING
		)
	{
		CG_AdjustZoomVal( cg_zoomStepSniper.value, ZOOM_SNIPER );
	}
	else if ( cg.zoomedBinoc )
	{
		CG_AdjustZoomVal( cg_zoomStepSniper.value, ZOOM_ADS );	// JPW NERVE per atvi request all use same vals to match menu (was zoomStepBinoc, ZOOM_BINOC);
	}
}


/*
==============
CG_Zoom
==============
*/
void
CG_Zoom ( void )
{

	// OSP - Fix for demo playback
	if ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback )
	{
		cg.predictedPlayerState.eFlags = cg.snap->ps.eFlags;
		cg.predictedPlayerState.weapon = cg.snap->ps.weapon;

		// check for scope wepon in use, and switch to if necessary
		// OSP - spec/demo scaling allowances
		if ( cg.predictedPlayerState.weapon == WP_FG42 && cg.predictedPlayerState.eFlags & EF_AIMING )
		{
			cg.zoomval = ( cg.zoomval == 0 ) ? cg_zoomDefaultSniper.value : cg.zoomval; // JPW NERVE was DefaultFG, changed per atvi req
		}
		else if ( cg.predictedPlayerState.weapon == WP_GARAND && cg.predictedPlayerState.eFlags & EF_AIMING )
		{
			cg.zoomval = ( cg.zoomval == 0 ) ? cg_zoomDefaultSniper.value : cg.zoomval;
		}
		else if ( cg.predictedPlayerState.weapon == WP_K43 && cg.predictedPlayerState.eFlags & EF_AIMING )
		{
			cg.zoomval = ( cg.zoomval == 0 ) ? cg_zoomDefaultSniper.value : cg.zoomval;
		}
		else if ( !(cg.predictedPlayerState.eFlags & EF_ZOOMING) )
		{
			cg.zoomval = 0;
		}
	}

	if ( cg.predictedPlayerState.eFlags & EF_ZOOMING || cg.predictedPlayerState.eFlags & EF_AIMING )
	{
		if ( cg.zoomedBinoc )
		{
			return;
		}

		cg.zoomedBinoc = qtrue;
		cg.zoomTime = cg.time;
		cg.zoomval = cg_zoomDefaultSniper.value;
	}
	else
	{
		if ( !cg.zoomedBinoc )
		{
			return;
		}

		cg.zoomedBinoc = qfalse;
		cg.zoomTime = cg.time;
		cg.zoomval = 0;
	}
}

/*
====================
CG_CalcFov

Fixed fov at intermissions, otherwise account for fov variable and zooms.
====================
*/
#define WAVE_AMPLITUDE	1
#define WAVE_FREQUENCY	0.4


/* */
static int
CG_CalcFov ( void )
{
	static float	lastfov = 90;								// for transitions back from zoomed in modes
	float			x;
	float			phase;
	float			v;
	int				contents;
	float			fov_x, fov_y;
	float			zoomFov;
	float			f;
	int				inwater;
	CG_Zoom();
	if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 && !(cg.snap->ps.pm_flags & PMF_FOLLOW) )
	{
		cg.zoomedBinoc = qfalse;
		cg.zoomTime = 0;
		cg.zoomval = 0;
	}

	if ( cg.predictedPlayerState.pm_type == PM_INTERMISSION )
	{

		// if in intermission, use a fixed value
		fov_x = 90;
	}
	else
	{
		fov_x = cg_fov.value;
		if ( !developer.integer )
		{
			if ( fov_x < 90 )
			{
				fov_x = 90;
			}
			else if ( fov_x > 160 )
			{
				fov_x = 160;
			}
		}

		if ( !developer.integer  )
		{

			// account for zooms
			if ( cg.zoomval )
			{
				zoomFov = cg.zoomval;							// (SA) use user scrolled amount
				if ( zoomFov < 1 )
				{
					zoomFov = 1;
				}
				else if ( zoomFov > 160 )
				{
					zoomFov = 160;
				}
			}
			else
			{
				zoomFov = lastfov;
			}

			if ( cg.zoomedSniper 
#ifdef __PORTAL_SCOPE__
				&& !cg_portalScope.integer
#endif
				)
			{
				fov_x = cg.zoomval;
			}

			// do smooth transitions for the binocs
			if ( !cg.renderingThirdPerson )
			{
				if ( cg.predictedPlayerState.eFlags & EF_AIMING )
				{
					if ( cg.zoomedSniper 
#ifdef __PORTAL_SCOPE__
						&& !cg_portalScope.integer
#endif
						)
					{
						zoomFov = cg.zoomval;
					}
					else if ( cg.snap->ps.eFlags & EF_MG42_ACTIVE || cg.snap->ps.eFlags & EF_BROWNING_ACTIVE )
					{
						zoomFov = 55;							// TODO
					}
					else if ( cg.snap->ps.eFlags & EF_MOUNTEDTANK )
					{
						zoomFov = 60;
					}
					else if ( cg.snap->ps.weapon == WP_MOBILE_MG42_SET || cg.snap->ps.weapon == WP_30CAL_SET )
					{
						zoomFov = 70;
					}
					else if
						(
							cg.snap->ps.weapon == WP_CARBINE ||
							cg.snap->ps.weapon == WP_M1GARAND ||
							cg.snap->ps.weapon == WP_G43 ||
							cg.snap->ps.weapon == WP_KAR98
						)
					{
						zoomFov = 65;
					}
					else
					{
						zoomFov = 70;
					}
				}

				if ( cg.zoomedBinoc )
				{												// binoc zooming in
					f = ( cg.time - cg.zoomTime ) / (float) ZOOM_TIME;
					if ( f > 1.0 )
					{
						fov_x = zoomFov;
					}
					else
					{
						fov_x = fov_x + f * ( zoomFov - fov_x );
					}

					lastfov = fov_x;
				}
				else if ( cg.zoomval )
				{												// zoomed by sniper/snooper
					fov_x = cg.zoomval;
					lastfov = fov_x;
				}
				else
				{												// binoc zooming out
					f = ( cg.time - cg.zoomTime ) / (float) ZOOM_TIME;
					if ( f > 1.0 )
					{
						fov_x = fov_x;
					}
					else
					{
						fov_x = zoomFov + f * ( fov_x - zoomFov );
					}
				}
			}
		}
	}

	if
	(
		cgs.clientinfo[cg.clientNum].skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] >= 4 &&
		cg_snoopers.integer > 0 &&
		!(cg.predictedPlayerState.eFlags & EF_DEAD) &&
		(cg.weaponSelect == WP_K43 || cg.weaponSelect == WP_GARAND) &&
		cg.thermal &&
		cg.zoomedSniper &&
		!cg.renderingThirdPerson
	)
	{
		cg.refdef_current->rdflags |= RDF_SNOOPERVIEW;
	}
	else
	{
		cg.refdef_current->rdflags &= ~RDF_SNOOPERVIEW;
	}

	if ( cg.showGameView && cg_bigLimbo.integer )
	{
		fov_x = fov_y = 60.f;
	}

	// Arnout: this is weird... (but ensures square pixel ratio!)
	x = cg.refdef_current->width / tan( fov_x / 360 * M_PI );
	fov_y = atan2( cg.refdef_current->height, x );
	fov_y = fov_y * 360 / M_PI;

	// And this seems better - but isn't really
	//fov_y = fov_x / cgs.glconfig.windowAspect;
	// warp if underwater
	//if ( cg_pmove.waterlevel == 3 ) {
	contents = CG_PointContents( cg.refdef.vieworg, -1 );
	if ( contents & (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA) )
	{
		phase = cg.time / 1000.0 * WAVE_FREQUENCY * M_PI * 2;
		v = WAVE_AMPLITUDE * sin( phase );
		fov_x += v;
		fov_y -= v;
		inwater = qtrue;
		cg.refdef_current->rdflags |= RDF_UNDERWATER;
	}
	else
	{
		cg.refdef_current->rdflags &= ~RDF_UNDERWATER;
		inwater = qfalse;
	}

	// set it
	cg.refdef_current->fov_x = fov_x;
	cg.refdef_current->fov_y = fov_y;

	// rain - allow freelook when dead until we tap out into limbo
	if
	(
		cg.snap->ps.pm_type == PM_FREEZE ||						/*(cg.snap->ps.pm_type == PM_DEAD && (cg.snap->ps.pm_flags & PMF_LIMBO)) ||*/
		cg.snap->ps.pm_flags & PMF_TIME_LOCKPLAYER
	)
	{

		// No movement for pauses
		cg.zoomSensitivity = 0;
	}
	else if ( !cg.zoomedBinoc )
	{

		// NERVE - SMF - fix for zoomed in/out movement bug
		if ( cg.zoomval )
		{
			cg.zoomSensitivity = 0.6 * ( cg.zoomval / 90.f );	// NERVE - SMF - changed to get less sensitive as you zoom in
		}
		else
		{
			cg.zoomSensitivity = 1;
		}

		// -NERVE - SMF
	}
	else
	{
		cg.zoomSensitivity = cg.refdef_current->fov_y / 75.0;
	}

	return ( inwater );
}

/*
==============
CG_UnderwaterSounds
==============
*/
#define UNDERWATER_BIT	16


/* */
static void
CG_UnderwaterSounds ( void )
{

	//	trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.underWaterSound, 255, 0 );
	trap_S_AddLoopingSound( cg.snap->ps.origin, vec3_origin, cgs.media.underWaterSound, 255 | (1 << UNDERWATER_BIT), 0 );
}


/*
===============
CG_DamageBlendBlob

===============
*/
static void
CG_DamageBlendBlob ( void )
{
	int				t, i;
	int				maxTime;
	refEntity_t		ent;
	qboolean		pointDamage;
	viewDamage_t	*vd;
	float			redFlash;

	// Gordon: no damage blend blobs if in limbo or spectator, and in the limbo menu
	if ( (cg.snap->ps.pm_flags & PMF_LIMBO || cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR) && cg.showGameView )
	{
		return;
	}

	// ragePro systems can't fade blends, so don't obscure the screen
	if ( cgs.glconfig.hardwareType == GLHW_RAGEPRO )
	{
		return;
	}

	if ( !cg_blood.integer )
	{
		return;
	}

	redFlash = 0;
	for ( i = 0; i < MAX_VIEWDAMAGE; i++ )
	{
		vd = &cg.viewDamage[i];
		if ( !vd->damageValue )
		{
			continue;
		}

		maxTime = vd->damageDuration;
		t = cg.time - vd->damageTime;
		if ( t <= 0 || t >= maxTime )
		{
			vd->damageValue = 0;
			continue;
		}

		pointDamage = !( !vd->damageX && !vd->damageY );

		// if not point Damage, only do flash blend
		if ( !pointDamage )
		{
			redFlash += 10.0 * ( 1.0 - (float) t / maxTime );
			continue;
		}

		memset( &ent, 0, sizeof(ent) );
		ent.reType = RT_SPRITE;
		ent.renderfx = RF_FIRST_PERSON | RF_NOSHADOW;
		VectorMA( cg.refdef_current->vieworg, 8, cg.refdef_current->viewaxis[0], ent.origin );
		VectorMA( ent.origin, vd->damageX * -8, cg.refdef_current->viewaxis[1], ent.origin );
		VectorMA( ent.origin, vd->damageY * 8, cg.refdef_current->viewaxis[2], ent.origin );
		ent.radius = vd->damageValue * 0.4 * ( 0.5 + 0.5 * (float) t / maxTime ) * ( 0.75 + 0.5 * fabs( sin( vd->damageTime)) );
		ent.customShader = cgs.media.viewBloodAni[(int) ( floor( ((float) t / maxTime) * 4.9) )];	//cgs.media.viewBloodShader;
		ent.shaderRGBA[0] = 255;
		ent.shaderRGBA[1] = 255;
		ent.shaderRGBA[2] = 255;
		ent.shaderRGBA[3] = 255 *
			(
				(cg_bloodDamageBlend.value > 1.0f) ? 1.0f :
					(cg_bloodDamageBlend.value < 0.0f) ? 0.0f : cg_bloodDamageBlend.value
			);
		trap_R_AddRefEntityToScene( &ent );
		redFlash += ent.radius;
	}
}


/*
===============
CG_DrawScreenFade
===============
*/
static void
CG_DrawScreenFade ( void )
{

	/* moved over to cg_draw.c
	static int lastTime;
	int elapsed, time;
	refEntity_t		ent;

	if (cgs.fadeStartTime + cgs.fadeDuration < cg.time) {
		cgs.fadeAlphaCurrent = cgs.fadeAlpha;
	} else if (cgs.fadeAlphaCurrent != cgs.fadeAlpha) {
		elapsed = (time = trap_Milliseconds()) - lastTime;	// we need to use trap_Milliseconds() here since the cg.time gets modified upon reloading
		lastTime = time;
		if (elapsed < 500 && elapsed > 0) {
			if (cgs.fadeAlphaCurrent > cgs.fadeAlpha) {
				cgs.fadeAlphaCurrent -= ((float)elapsed/(float)cgs.fadeDuration);
				if (cgs.fadeAlphaCurrent < cgs.fadeAlpha)
					cgs.fadeAlphaCurrent = cgs.fadeAlpha;
			} else {
				cgs.fadeAlphaCurrent += ((float)elapsed/(float)cgs.fadeDuration);
				if (cgs.fadeAlphaCurrent > cgs.fadeAlpha)
					cgs.fadeAlphaCurrent = cgs.fadeAlpha;
			}
		}
	}
	// now draw the fade
	if (cgs.fadeAlphaCurrent > 0.0) {
		memset( &ent, 0, sizeof( ent ) );
		ent.reType = RT_SPRITE;
		ent.renderfx = RF_FIRST_PERSON;

		VectorMA( cg.refdef_current->vieworg, 8, cg.refdef_current->viewaxis[0], ent.origin );
		ent.radius = 80;	// occupy entire screen
		ent.customShader = cgs.media.viewFadeBlack;
		ent.shaderRGBA[3] = (int)(255.0 * cgs.fadeAlphaCurrent);
		
		trap_R_AddRefEntityToScene( &ent );
	}
*/
}


/*
===============
CG_CalcViewValues

Sets cg.refdef view values
===============
*/
int
CG_CalcViewValues ( void )
{
	playerState_t	*ps;
	memset( cg.refdef_current, 0, sizeof(cg.refdef) );

	// calculate size of 3D view
	CG_CalcVrect();
	ps = &cg.predictedPlayerState;
	if ( cg.cameraMode )
	{
		vec3_t	origin, angles;
		float	fov = 90;
		float	x;
		if ( trap_getCameraInfo( CAM_PRIMARY, cg.time, &origin, &angles, &fov) )
		{
			VectorCopy( origin, cg.refdef_current->vieworg );
			angles[ROLL] = 0;
			angles[PITCH] = -angles[PITCH];				// (SA) compensate for reversed pitch (this makes the game match the editor, however I'm guessing the real fix is to be done there)
			VectorCopy( angles, cg.refdefViewAngles );
			AnglesToAxis( cg.refdefViewAngles, cg.refdef_current->viewaxis );
			x = cg.refdef.width / tan( fov / 360 * M_PI );
			cg.refdef_current->fov_y = atan2( cg.refdef_current->height, x );
			cg.refdef_current->fov_y = cg.refdef_current->fov_y * 360 / M_PI;
			cg.refdef_current->fov_x = fov;

			// FIXME: this is really really bad
			trap_SendClientCommand( va( "setCameraOrigin %f %f %f", origin[0], origin[1], origin[2]) );
			return ( 0 );
		}
		else
		{
			cg.cameraMode = qfalse;
			trap_Cvar_Set( "cg_letterbox", "0" );
			trap_SendClientCommand( "stopCamera" );
			trap_stopCamera( CAM_PRIMARY );				// camera off in client
			CG_Fade( 0, 0, 0, 255, 0, 0 );				// go black
			CG_Fade( 0, 0, 0, 0, cg.time + 200, 1500 ); // then fadeup
		}
	}

	// intermission view
	if ( ps->pm_type == PM_INTERMISSION )
	{
		VectorCopy( ps->origin, cg.refdef_current->vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
		AnglesToAxis( cg.refdefViewAngles, cg.refdef_current->viewaxis );
		return ( CG_CalcFov() );
	}

	if ( cg.bobfracsin > 0 && !ps->bobCycle )
	{
		cg.lastvalidBobcycle = cg.bobcycle;
		cg.lastvalidBobfracsin = cg.bobfracsin;
	}

	cg.bobcycle = ( ps->bobCycle & 128 ) >> 7;
	cg.bobfracsin = fabs( sin( (ps->bobCycle & 127) / 127.0 * M_PI) );
	cg.xyspeed = sqrt( ps->velocity[0] * ps->velocity[0] + ps->velocity[1] * ps->velocity[1] );
	if ( cg.showGameView && cg_bigLimbo.integer )
	{
		VectorCopy( cgs.ccPortalPos, cg.refdef_current->vieworg );
		if ( cg.showGameView && cgs.ccPortalEnt != -1 && cg_bigLimbo.integer )
		{
			vec3_t	vec;
			VectorSubtract( cg_entities[cgs.ccPortalEnt].lerpOrigin, cg.refdef_current->vieworg, vec );
			vectoangles( vec, cg.refdefViewAngles );
		}
		else
		{
			VectorCopy( cgs.ccPortalAngles, cg.refdefViewAngles );
		}
	}
	else if ( ps->eFlags & EF_MOUNTEDTANK )
	{
		centity_t	*tank = &cg_entities[cg_entities[cg.snap->ps.clientNum].tagParent];
		VectorCopy( tank->mountedMG42Player.origin, cg.refdef_current->vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
	}
	else
	{
		VectorCopy( ps->origin, cg.refdef_current->vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
	}

	if ( !cg.showGameView || cg_bigLimbo.integer == 0 )
	{

		// add error decay
		if ( cg_errorDecay.value > 0 )
		{
			int		t;
			float	f;
			t = cg.time - cg.predictedErrorTime;
			f = ( cg_errorDecay.value - t ) / cg_errorDecay.value;
			if ( f > 0 && f < 1 )
			{
				VectorMA( cg.refdef_current->vieworg, f, cg.predictedError, cg.refdef_current->vieworg );
			}
			else
			{
				cg.predictedErrorTime = 0;
			}
		}

		// Ridah, lock the viewangles if the game has told us to
		if ( ps->viewlocked && !(cg.renderingThirdPerson) )
		{
			if ( ps->viewlocked != 7 && ps->viewlocked != 3 && ps->viewlocked != 2 )
			{
				BG_EvaluateTrajectory( &cg_entities[ps->viewlocked_entNum].currentState.apos, cg.time,
									   cg.refdefViewAngles, qtrue,
									   cg_entities[ps->viewlocked_entNum].currentState.effect2Time );
			}

			if ( ps->viewlocked == 2 && !(cg.renderingThirdPerson) )
			{
				cg.refdefViewAngles[0] += crandom();
				cg.refdefViewAngles[1] += crandom();
			}
		}

		if ( cg.renderingThirdPerson )
		{

			// back away from character
			CG_OffsetThirdPersonView();
		}
		else
		{

			// offset for local bobbing and kicks
			CG_OffsetFirstPersonView();
			if ( cg.editingSpeakers )
			{
				CG_SetViewanglesForSpeakerEditor();
			}
		}

		// Ridah, lock the viewangles if the game has told us to
		if ( ps->viewlocked == 7 && !(cg.renderingThirdPerson) )
		{
			centity_t	*tent;
			vec3_t		vec;
			tent = &cg_entities[ps->viewlocked_entNum];
			VectorCopy( tent->lerpOrigin, vec );
			VectorSubtract( vec, cg.refdef_current->vieworg, vec );
			vectoangles( vec, cg.refdefViewAngles );
		}
		else if ( ps->viewlocked == 4 && !(cg.renderingThirdPerson) )
		{
			vec3_t	fwd;
			AngleVectors( cg.refdefViewAngles, fwd, NULL, NULL );
			VectorMA( cg_entities[ps->viewlocked_entNum].lerpOrigin, 16, fwd, cg.refdef_current->vieworg );
		}
		else if ( ps->viewlocked && !(cg.renderingThirdPerson) )
		{
			vec3_t	fwd;
			float	oldZ;

			// set our position to be behind it
			oldZ = cg.refdef_current->vieworg[2];
			AngleVectors( cg.refdefViewAngles, fwd, NULL, NULL );
			if ( cg.predictedPlayerState.eFlags & EF_AAGUN_ACTIVE )
			{
				VectorMA( cg_entities[ps->viewlocked_entNum].lerpOrigin, 0, fwd, cg.refdef_current->vieworg );
			}
			else
			{
				VectorMA( cg_entities[ps->viewlocked_entNum].lerpOrigin, -34, fwd, cg.refdef_current->vieworg );
			}

			cg.refdef_current->vieworg[2] = oldZ;
		}

		// done.
	}

	// position eye reletive to origin
	if ( !cg.headView || cg_firstPerson.integer < 2 || cg.renderingThirdPerson )
	{
		AnglesToAxis( cg.refdefViewAngles, cg.refdef_current->viewaxis );
	}

	if ( cg.hyperspace )
	{
		cg.refdef.rdflags |= RDF_NOWORLDMODEL | RDF_HYPERSPACE | RF_NOSHADOW;
	}

	// field of view
	return ( CG_CalcFov() );
}


//=========================================================================
char *
CG_MustParse ( char **pString, const char *pErrorMsg )
{
	char	*token = COM_Parse( pString );
	if ( !*token )
	{
		CG_Error( pErrorMsg );
	}

	return ( token );
}


/* */
void
CG_ParseSkyBox ( void )
{
	int		fogStart, fogEnd;
	char	*cstr, *token;
	vec4_t	fogColor;
	cstr = (char *) CG_ConfigString( CS_SKYBOXORG );
	if ( !*cstr )
	{
		cg.skyboxEnabled = qfalse;
		return;
	}

	token = CG_MustParse( &cstr, "CG_ParseSkyBox: error parsing skybox configstring\n" );
	cg.skyboxViewOrg[0] = atof( token );
	token = CG_MustParse( &cstr, "CG_ParseSkyBox: error parsing skybox configstring\n" );
	cg.skyboxViewOrg[1] = atof( token );
	token = CG_MustParse( &cstr, "CG_ParseSkyBox: error parsing skybox configstring\n" );
	cg.skyboxViewOrg[2] = atof( token );
	token = CG_MustParse( &cstr, "CG_ParseSkyBox: error parsing skybox configstring\n" );
	cg.skyboxViewFov = atoi( token );
	if ( !cg.skyboxViewFov )
	{
		cg.skyboxViewFov = 90;
	}

	// setup fog the first time, ignore this part of the configstring after that
	token = CG_MustParse( &cstr, "CG_ParseSkyBox: error parsing skybox configstring.  No fog state\n" );
	if ( atoi( token) )
	{														// this camera has fog
		token = CG_MustParse( &cstr, "CG_DrawSkyBoxPortal: error parsing skybox configstring.  No fog[0]\n" );
		fogColor[0] = atof( token );
		token = CG_MustParse( &cstr, "CG_DrawSkyBoxPortal: error parsing skybox configstring.  No fog[1]\n" );
		fogColor[1] = atof( token );
		token = CG_MustParse( &cstr, "CG_DrawSkyBoxPortal: error parsing skybox configstring.  No fog[2]\n" );
		fogColor[2] = atof( token );
		token = COM_ParseExt( &cstr, qfalse );
		fogStart = atoi( token );
		token = COM_ParseExt( &cstr, qfalse );
		fogEnd = atoi( token );
		trap_R_SetFog( FOG_PORTALVIEW, fogStart, fogEnd, fogColor[0], fogColor[1], fogColor[2], 1.1f );
	}
	else
	{
		trap_R_SetFog( FOG_PORTALVIEW, 0, 0, 0, 0, 0, 0 );	// init to null
	}

	cg.skyboxEnabled = qtrue;
}


/*
==============
CG_ParseTagConnects
==============
*/
void
CG_ParseTagConnects ( void )
{
	int i;
	for ( i = CS_TAGCONNECTS; i < CS_TAGCONNECTS + MAX_TAGCONNECTS; i++ )
	{
		CG_ParseTagConnect( i );
	}
}


/* */
void
CG_ParseTagConnect ( int tagNum )
{
	char	*token, *pString = (char *) CG_ConfigString( tagNum );	// Gordon: bleh, i hate that cast away of the const
	int		entNum;
	if ( !*pString )
	{
		return;
	}

	token = CG_MustParse( &pString, "Invalid TAGCONNECT configstring\n" );
	entNum = atoi( token );
	if ( entNum < 0 || entNum >= MAX_GENTITIES )
	{
		CG_Error( "Invalid TAGCONNECT entitynum\n" );
	}

	token = CG_MustParse( &pString, "Invalid TAGCONNECT configstring\n" );
	cg_entities[entNum].tagParent = atoi( token );
	if ( cg_entities[entNum].tagParent < 0 || cg_entities[entNum].tagParent >= MAX_GENTITIES )
	{
		CG_Error( "Invalid TAGCONNECT tagparent\n" );
	}

	token = CG_MustParse( &pString, "Invalid TAGCONNECT configstring\n" );
	Q_strncpyz( cg_entities[entNum].tagName, token, MAX_QPATH );
}


/*
==============
CG_DrawSkyBoxPortal
==============
*/
void
CG_DrawSkyBoxPortal ( qboolean fLocalView )
{
	refdef_t		rd;
	static float	lastfov = 90;		// for transitions back from zoomed in modes
	if ( !cg_skybox.integer || !cg.skyboxEnabled )
	{
		return;
	}

	memcpy( &rd, cg.refdef_current, sizeof(refdef_t) );
	VectorCopy( cg.skyboxViewOrg, rd.vieworg );
	if ( fLocalView ) //@FIXME!!!
	{
		float	fov_x;
		float	fov_y;
		float	x;
		float	zoomFov;
		float	f;
		if ( cg.predictedPlayerState.pm_type == PM_INTERMISSION )
		{

			// if in intermission, use a fixed value
			fov_x = 90;
		}
		else
		{

			// user selectable
			fov_x = cg_fov.value;
			if ( fov_x < 1 )
			{
				fov_x = 1;
			}
			else if ( fov_x > 160 )
			{
				fov_x = 160;
			}

			// account for zooms
			if ( cg.zoomval )
			{
				zoomFov = cg.zoomval;	// (SA) use user scrolled amount
				if ( zoomFov < 1 )
				{
					zoomFov = 1;
				}
				else if ( zoomFov > 160 )
				{
					zoomFov = 160;
				}
			}
			else
			{
				zoomFov = lastfov;
			}

			// do smooth transitions for the binocs
			if ( !cg.renderingThirdPerson )
			{
				if ( cg.predictedPlayerState.eFlags & EF_AIMING )
				{
					if ( cg.zoomedSniper
#ifdef __PORTAL_SCOPE__						
						&& !cg_portalScope.integer
#endif
						)
					{
						zoomFov = 32.5;
					}
					else if ( cg.snap->ps.eFlags & EF_MG42_ACTIVE || cg.snap->ps.eFlags & EF_BROWNING_ACTIVE )
					{
						zoomFov = 55;	// TODO
					}
					else if ( cg.snap->ps.eFlags & EF_MOUNTEDTANK )
					{
						zoomFov = 60;
					}
					else if ( cg.snap->ps.weapon == WP_MOBILE_MG42_SET || cg.snap->ps.weapon == WP_30CAL_SET )
					{
						zoomFov = 70;
					}
					else if
						(
							cg.snap->ps.weapon == WP_CARBINE ||
							cg.snap->ps.weapon == WP_M1GARAND ||
							cg.snap->ps.weapon == WP_G43 ||
							cg.snap->ps.weapon == WP_KAR98
						)
					{
						zoomFov = 65;
					}
					else
					{
						zoomFov = 70;
					}
				}
			}

			// do smooth transitions for the binocs
			if ( cg.zoomedBinoc )
			{							// binoc zooming in
				f = ( cg.time - cg.zoomTime ) / (float) ZOOM_TIME;
				fov_x = ( f > 1.0 ) ? zoomFov : fov_x + f * ( zoomFov - fov_x );
				lastfov = fov_x;
			}
			else if ( cg.zoomval )
			{							// zoomed by sniper/snooper
				fov_x = cg.zoomval;
				lastfov = fov_x;
			}
			else
			{							// binoc zooming out
				f = ( cg.time - cg.zoomTime ) / (float) ZOOM_TIME;
				fov_x = ( f > 1.0 ) ? fov_x : zoomFov + f * ( fov_x - zoomFov );
			}
		}

		if
		(
			cgs.clientinfo[cg.clientNum].skill[SK_MILITARY_INTELLIGENCE_AND_SCOPED_WEAPONS] >= 4 &&
			cg_snoopers.integer > 0 &&
			!(cg.predictedPlayerState.eFlags & EF_DEAD) &&
			(cg.weaponSelect == WP_K43 || cg.weaponSelect == WP_GARAND) &&
			cg.thermal &&
			cg.zoomedSniper &&
			!cg.renderingThirdPerson
		)
		{
			rd.rdflags |= RDF_SNOOPERVIEW;
		}
		else
		{
			rd.rdflags &= ~RDF_SNOOPERVIEW;
		}

		if
		(
			BG_PlayerMounted( cg.snap->ps.eFlags) ||
			cg.predictedPlayerState.weapon == WP_MOBILE_MG42_SET ||
			cg.predictedPlayerState.weapon == WP_30CAL_SET
		)
		{
			if ( !cg.renderingThirdPerson )
			{
				fov_x = 55;
			}
		}

		x = rd.width / tan( fov_x / 360 * M_PI );
		fov_y = atan2( rd.height, x );
		fov_y = fov_y * 360 / M_PI;
		rd.fov_x = fov_x;
		rd.fov_y = fov_y;
	}

	rd.time = cg.time;
	rd.rdflags |= RDF_SKYBOXPORTAL;

	// draw the skybox
	trap_R_RenderScene( &rd );
}

//=========================================================================

/*
**  Frustum code
*/

// some culling bits
typedef struct plane_s
{
	vec3_t	normal;
	float	dist;
} plane_t;
static plane_t	frustum[4];


//
//	CG_SetupFrustum

//
void
CG_SetupFrustum ( void )
{
	int		i;
	float	xs, xc;
	float	ang;
	ang = cg.refdef_current->fov_x / 180 * M_PI * 0.5f;
	xs = sin( ang );
	xc = cos( ang );
	VectorScale( cg.refdef_current->viewaxis[0], xs, frustum[0].normal );
	VectorMA( frustum[0].normal, xc, cg.refdef_current->viewaxis[1], frustum[0].normal );
	VectorScale( cg.refdef_current->viewaxis[0], xs, frustum[1].normal );
	VectorMA( frustum[1].normal, -xc, cg.refdef_current->viewaxis[1], frustum[1].normal );
	ang = cg.refdef.fov_y / 180 * M_PI * 0.5f;
	xs = sin( ang );
	xc = cos( ang );
	VectorScale( cg.refdef_current->viewaxis[0], xs, frustum[2].normal );
	VectorMA( frustum[2].normal, xc, cg.refdef_current->viewaxis[2], frustum[2].normal );
	VectorScale( cg.refdef_current->viewaxis[0], xs, frustum[3].normal );
	VectorMA( frustum[3].normal, -xc, cg.refdef_current->viewaxis[2], frustum[3].normal );
	for ( i = 0; i < 4; i++ )
	{
		frustum[i].dist = DotProduct( cg.refdef_current->vieworg, frustum[i].normal );
	}
}


//
//	CG_CullPoint - returns true if culled

//
qboolean
CG_CullPoint ( vec3_t pt )
{
	int		i;
	plane_t *frust;

	// check against frustum planes
	for ( i = 0; i < 4; i++ )
	{
		frust = &frustum[i];
		if ( (DotProduct( pt, frust->normal) - frust->dist) < 0 )
		{
			return ( qtrue );
		}
	}

	return ( qfalse );
}


/* */
qboolean
CG_CullPointAndRadius ( const vec3_t pt, vec_t radius )
{
	int		i;
	plane_t *frust;

	// check against frustum planes
	for ( i = 0; i < 4; i++ )
	{
		frust = &frustum[i];
		if ( (DotProduct( pt, frust->normal) - frust->dist) < -radius )
		{
			return ( qtrue );
		}
	}

	return ( qfalse );
}

//=========================================================================
extern void CG_SetupDlightstyles ( void );

//#define DEBUGTIME_ENABLED
#ifdef DEBUGTIME_ENABLED
#define DEBUGTIME	elapsed = ( trap_Milliseconds() - dbgTime );				   \
	if ( dbgCnt++ == 1 )														   \
	{																			   \
		CG_Printf( "t%i:%i ", dbgCnt, elapsed = (trap_Milliseconds() - dbgTime) ); \
	} dbgTime += elapsed;
#else
#define DEBUGTIME
#endif
#ifdef _DEBUG

//#define FAKELAG
#ifdef FAKELAG
extern int	snapshotDelayTime;
#endif // FAKELAG
#endif // _DEBUG

//#define __PORTAL_SCOPE__
#ifdef __PORTAL_SCOPE__

void CG_DrawScopeView ( void ) {
	refdef_t refdef;
	float	fovx;
	float	x,y,w,h;

	memset( &refdef, 0, sizeof( refdef_t ) );

	memcpy(refdef.areamask, cg.snap->areamask, sizeof(refdef.areamask));

	cg.subscene = qtrue; // allow us parsing packetentities again

	// @todo read from weapon model
	x = 320-128;
	y = 240-128;
	w = 256;
	h = 256;

	CG_DrawPic( x, y, w, h, cgs.media.scopeShaderTrans );

	CG_AdjustFrom640( &x, &y, &w, &h );

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.fov_x = cg.zoomval; // @fixmeheredamnit
	fovx = refdef.width / tan( refdef.fov_x / 360 * M_PI );
	refdef.fov_y = atan2( refdef.height, fovx );
	refdef.fov_y = refdef.fov_y * 360 / M_PI;

	VectorCopy(cg.refdef_current->vieworg , refdef.vieworg);
	AxisCopy(cg.refdef_current->viewaxis , refdef.viewaxis);

	cg.refdef_current = &refdef;

	trap_R_ClearScene();

	CG_SetupFrustum();
	CG_DrawSkyBoxPortal(qfalse);

	if(!cg.hyperspace) {
		CG_AddPacketEntities();
		CG_AddMarks();
		CG_AddParticles();
		CG_AddLocalEntities();

		CG_AddSmokeSprites();
		CG_AddAtmosphericEffects();

		CG_AddFlameChunks();
		CG_AddTrails();		// this must come last, so the trails dropped this frame get drawn
	}

	refdef.time = cg.time;
	trap_SetClientLerpOrigin(refdef.vieworg[0], refdef.vieworg[1], refdef.vieworg[2]);

	trap_R_RenderScene(&refdef);

	// need them back because draw code recompute from 640X480
	x = 320-128;
	y = 240-128;
	w = 256;
	h = 256;

	CG_DrawPic( x, y, w, h, cgs.media.scopeShader );

	cg.refdef_current = &cg.refdef;
	cg.subscene = qfalse;	// done ...	

}
#endif

/*
=================
CG_DrawActiveFrame

Generates and draws a game scene and status information at the given time.
=================
*/

qboolean	CG_CalcMuzzlePoint ( int entityNum, vec3_t muzzle );


//vec3_t previous_view;

//vec3_t previous_view2;
void
CG_DrawActiveFrame ( int serverTime, stereoFrame_t stereoView, qboolean demoPlayback )
{
	int inwater;
#ifdef DEBUGTIME_ENABLED
	int dbgTime = trap_Milliseconds(), elapsed;
	int dbgCnt = 0;
#endif
	cg.time = serverTime;
	cgDC.realTime = cg.time;
	cg.demoPlayback = demoPlayback;
#ifdef FAKELAG
	cg.time -= snapshotDelayTime;
#endif // _DEBUG
#ifdef DEBUGTIME_ENABLED
	CG_Printf( "\n" );
#endif

	DEBUGTIME

	// update cvars
	CG_UpdateCvars ();
	if ( cg.forcecvar )
	{
		CG_RestoreProfile();
		cg.forcecvar = qfalse;
	}

	DEBUGTIME

	// if we are only updating the screen as a loading
	// pacifier, don't even try to read snapshots
	if ( cg.infoScreenText[0] != 0 )
	{
		CG_DrawInformation( qfalse );
		return;
	}

	CG_PB_ClearPolyBuffers();
	CG_UpdatePMLists();

	// any looped sounds will be respecified as entities
	// are added to the render list
	trap_S_ClearLoopingSounds();
	CG_UpdateBufferedSoundScripts();

	DEBUGTIME

	// set up cg.snap and possibly cg.nextSnap
	CG_ProcessSnapshots ();
	DEBUGTIME

	// if we haven't received any snapshots yet, all
	// we can draw is the information screen
	if ( !cg.snap || (cg.snap->snapFlags & SNAPFLAG_NOT_ACTIVE) )
	{
		CG_DrawInformation( qfalse );
		return;
	}

	// check for server set weapons we might not know about
	// (FIXME: this is a hack for the time being since a scripted "selectweapon" does
	// not hit the first snap, the server weapon set in cg_playerstate.c line 219 doesn't
	// do the trick)
	if ( !cg.weaponSelect && cg.snap->ps.weapon && cg.holster == qfalse )
	{
		cg.weaponSelect = cg.snap->ps.weapon;
		cg.weaponSelect_last = cg.snap->ps.weapon;
		cg.weaponSelectTime = cg.time;
	}

	if ( cg.weaponSelect == WP_FG42SCOPE )
	{
		float	spd;
		spd = VectorLength( cg.snap->ps.velocity );
		if ( spd > 180.0f )
		{
			CG_FinishWeaponChange( WP_FG42SCOPE, WP_FG42 );
		}
	}

	DEBUGTIME if ( !cg.lightstylesInited )
	{
		CG_SetupDlightstyles();
	}

	DEBUGTIME

	// if we have been told not to render, don't
	if ( cg_norender.integer )
	{
		return;
	}

	// this counter will be bumped for every valid scene we generate
	cg.clientFrame++;

	// update cg.predictedPlayerState
	CG_PredictPlayerState();
	DEBUGTIME 

	// OSP -- MV handling
	if ( cg.mvCurrentMainview != NULL && cg.snap->ps.pm_type != PM_INTERMISSION )
	{
		CG_mvDraw( cg.mvCurrentMainview );

		// FIXME: not valid for demo playback
		//trap_Key_SetCatcher(trap_Key_GetCatcher() & ~KEYCATCH_CGAME);
		//trap_Key_SetCatcher( KEYCATCH_UI );
		cg.zoomSensitivity = mv_sensitivity.value / int_sensitivity.value;
	}
	else
	{

		// clear all the render lists
		trap_R_ClearScene();

		//	CG_testDraw(stereoView); // TESTING TODO
		DEBUGTIME

		// Decide on third person view, copy of g_thirdperson
		if ( cg_3rdPerson_allowed.integer != 1 )
		{
			cg.renderingThirdPerson = ( cg.snap->ps.stats[STAT_HEALTH] <= 0 ) ||
				(
				cg.showGameView &&
				cg_bigLimbo.integer
				) ||
				( 
					cg_thirdPerson.integer
					&& !(cg.predictedPlayerState.eFlags & EF_ZOOMING)
					&& !(BG_IsScopedWeapon2( cg.predictedPlayerState.weapon) && cg.predictedPlayerState.eFlags & EF_AIMING)
				);
				/*|| cg_entities[cg.snap->ps.clientNum].currentState.eFlags & EF_TANK*/ ;
		}
		else
		{
			// Kamikazee: Third person is now only allowed for spectators when not using speclock,
			//	or when cheats are on.
			cg.renderingThirdPerson =
				/*(
					cg_3rdPerson.integer &&
					!(cg.predictedPlayerState.eFlags & EF_ZOOMING) &&
					!(BG_IsScopedWeapon2( cg.predictedPlayerState.weapon) && cg.predictedPlayerState.eFlags & EF_AIMING)
				) ||*/
				( cg.snap->ps.stats[STAT_HEALTH] <= 0 ) ||
				( cg.showGameView && cg_bigLimbo.integer ) ||
				(
					cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR
					&& cg.predictedPlayerState.pm_flags & PMF_FOLLOW
					&& cg_3rdPerson.integer > 0
				) ||
				(
					cg_thirdPerson.integer
					&& !(cg.predictedPlayerState.eFlags & EF_ZOOMING)
					&& !(BG_IsScopedWeapon2( cg.predictedPlayerState.weapon) && cg.predictedPlayerState.eFlags & EF_AIMING)
				);
				/*|| cg_entities[cg.snap->ps.clientNum].currentState.eFlags & EF_TANK*/ ;
		}

		// build cg.refdef
		inwater = CG_CalcViewValues();
		CG_SetupFrustum();

		DEBUGTIME

		// RF, draw the skyboxportal
		CG_DrawSkyBoxPortal ( qtrue );
		DEBUGTIME if ( inwater )
		{
			CG_UnderwaterSounds();
		}

		DEBUGTIME

		// first person blend blobs, done after AnglesToAxis
		if ( !cg.renderingThirdPerson )
		{
			CG_DamageBlendBlob();
		}

		DEBUGTIME cg.headView = qfalse;

		// build the render lists
		if ( !cg.hyperspace )
		{
			CG_AddPacketEntities(); // adter calcViewValues, so predicted player state is correct
			CG_AddMarks();

			DEBUGTIME	CG_AddScriptSpeakers ();
			DEBUGTIME

			// Rafael particles
			CG_AddParticles ();

			// done.
			DEBUGTIME	CG_AddLocalEntities ();
			DEBUGTIME	CG_AddSmokeSprites ();
			DEBUGTIME	CG_AddAtmosphericEffects ();
		}

		// Rafael mg42
		if ( (!cg.showGameView || cg_bigLimbo.integer == 0) && !cgs.dbShowing )
		{
			CG_AddViewWeapon( &cg.predictedPlayerState );
		}

#ifdef __PARTICLE_SYSTEM__

		// New Particle System...
		//after CG_AddViewWeapon
		if ( !cg.hyperspace )
		{
			CG_AddNewParticles();
		}
#endif //__PARTICLE_SYSTEM__

		// NERVE - SMF - play buffered voice chats
		CG_PlayBufferedVoiceChats();
		DEBUGTIME

		// Ridah, trails
		if ( !cg.hyperspace )
		{
			CG_AddFlameChunks();
			CG_AddTrails();			// this must come last, so the trails dropped this frame get drawn
		}

		// done.
		DEBUGTIME

		// finish up the rest of the refdef
		if ( cg.testModelEntity.hModel )
		{
			CG_AddTestModel();
		}

		cg.refdef.time = cg.time;
		memcpy( cg.refdef.areamask, cg.snap->areamask, sizeof(cg.refdef.areamask) );
		DEBUGTIME

		// warning sounds when powerup is wearing off
		//CG_PowerupTimerSounds();
		// make sure the lagometerSample and frame timing isn't done twice when in stereo
		if ( stereoView != STEREO_RIGHT )
		{
			cg.frametime = cg.time - cg.oldTime;
			if ( cg.frametime < 0 )
			{
				cg.frametime = 0;
			}

			cg.oldTime = cg.time;
			CG_AddLagometerFrameInfo();
		}

		DEBUGTIME

		// Ridah, fade the screen
		CG_DrawScreenFade ();
		DEBUGTIME

		// DHM - Nerve :: let client system know our predicted origin
		trap_SetClientLerpOrigin ( cg.refdef.vieworg[0], cg.refdef.vieworg[1], cg.refdef.vieworg[2] );
		
		// actually issue the rendering calls
		CG_DrawActive( stereoView );

		DEBUGTIME

#ifdef __PORTAL_SCOPE__
		if ( cg.zoomedSniper && cg_portalScope.integer )
			CG_DrawScopeView();
#endif

		// update audio positions
		trap_S_Respatialize ( cg.snap->ps.clientNum, cg.refdef.vieworg, cg.refdef.viewaxis, inwater );
	}

	if ( cg_stats.integer )
	{
		CG_Printf( "cg.clientFrame:%i\n", cg.clientFrame );
	}

	DEBUGTIME
	{
		qboolean	limbo = qfalse;
		if ( cg.showGameView && cg_bigLimbo.integer )
		{
			limbo = qtrue;
		}

		// let the client system know what our weapon, holdable item and zoom settings are
		trap_SetUserCmdValue( cg.weaponSelect, limbo ? 0x01 : 0x00, cg.zoomSensitivity, cg.identifyClientRequest );
	}
}
