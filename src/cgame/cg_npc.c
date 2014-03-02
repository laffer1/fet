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


// Most of this is a copy of cg_player.. except
// for CG_NPC itself. NPCs will need to customize,
// so I may as well copy and alter..
#include "cg_local.h"
#include "../game/bg_classes.h"
#ifdef __NPC__
#define SWING_RIGHT 1
#define SWING_LEFT	2
#define JUMP_HEIGHT 56
#define SWINGSPEED	0.3
static int			dp_realtime;
static float		jumpHeight;
animation_t			*lastTorsoAnim;
animation_t			*lastLegsAnim;
extern const char	*cg_skillRewards[SK_NUM_SKILLS][NUM_SKILL_LEVELS - 1];
extern void			CG_AnimPlayerConditions ( bg_character_t *character, centity_t *cent );
extern void			CG_RunLerpFrameRate
					(
						clientInfo_t	*ci,
						lerpFrame_t		*lf,
						int				newAnimation,
						centity_t		*cent,
						int				recursion
					);
extern void			CGRefEntityToTag ( refEntity_t *ent, tag_t *tag );
extern void			CGTagToRefEntity ( refEntity_t *ent, tag_t *tag );


/* */
bg_character_t *
CG_GetCharacter ( int team, int classNum )
{
	return ( BG_GetCharacter( team, classNum) );
}


/*
==============
CG_NPCBreathPuffs
==============
*/
static void
CG_NPCBreathPuffs ( centity_t *cent, refEntity_t *head )
{
	clientInfo_t	*ci;
	vec3_t			up, forward;
	int				contents;
	vec3_t			mang, morg, maxis[3];
	ci = &cgs.clientinfo[cent->currentState.number];
	if ( !cg_enableBreath.integer )
	{
		return;
	}

	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
	{
		return;
	}

	if ( !(cent->currentState.eFlags & EF_DEAD) )
	{
		return;
	}

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
	CG_SmokePuff( morg, forward, 4, 1, 1, 1, 0.5f, 2000, cg.time, cg.time + 400, 0, cgs.media.shotgunSmokePuffShader );
	ci->breathPuffTime = cg.time + 3000 + random() * 1000;
}


/*
===============
CG_NPCFloatSprite

Float a sprite over the player's head
DHM - Nerve :: added height parameter
===============
*/
static void
CG_NPCFloatSprite ( centity_t *cent, qhandle_t shader, int height )
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
CG_NPCSprites

Float sprites over the player's head
===============
*/
static void
CG_NPCSprites ( centity_t *cent )
{
	int team;
	if ( cent->currentState.powerups & (1 << PW_REDFLAG) || cent->currentState.powerups & (1 << PW_BLUEFLAG) )
	{
		CG_NPCFloatSprite( cent, cgs.media.objectiveShader, 56 );
		return;
	}

	//	if ( cent->currentState.eFlags & EF_CONNECTION ) {
	//		CG_NPCFloatSprite( cent, cgs.media.disconnectIcon, 48 );
	//		return;
	//	}
	if ( cent->currentState.powerups & (1 << PW_INVULNERABLE) )
	{
		CG_NPCFloatSprite( cent, cgs.media.spawnInvincibleShader, 56 );
		return;
	}

	team = cgs.clientinfo[cent->currentState.clientNum].team;

	// DHM - Nerve :: If this client is a medic, draw a 'revive' icon over
	//					dead players that are not in limbo yet.
	if
	(
		(cent->currentState.eFlags & EF_DEAD) &&
		cent->currentState.number == cent->currentState.clientNum &&
		cg.snap->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC &&
		cg.snap->ps.stats[STAT_HEALTH] > 0 &&
		cg.snap->ps.persistant[PERS_TEAM] == team
	)
	{
		CG_NPCFloatSprite( cent, cgs.media.medicReviveShader, 8 );
		return;
	}

	// DHM - Nerve :: show voice chat signal so players know who's talking
	if ( cent->voiceChatSpriteTime > cg.time && cg.snap->ps.persistant[PERS_TEAM] == team )
	{
		CG_NPCFloatSprite( cent, cent->voiceChatSprite, 56 );
		return;
	}

	// DHM - Nerve :: only show talk icon to team-mates
	if ( cent->currentState.eFlags & EF_TALK && cg.snap->ps.persistant[PERS_TEAM] == team )
	{
		CG_NPCFloatSprite( cent, cgs.media.balloonShader, 48 );
		return;
	}
	{
		fireteamData_t	*ft;
		if ( (ft = CG_IsOnFireteam( cent->currentState.number)) )
		{
			if ( ft == CG_IsOnFireteam( cg.clientNum) && cgs.clientinfo[cent->currentState.number].selected )
			{
				CG_NPCFloatSprite( cent, cgs.media.fireteamicons[ft->ident], 56 );
			}
		}
	}
}

/*
===============
CG_NPCShadow

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
CG_NPCShadow ( centity_t *cent, float *shadowPlane )
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
	if ( cg_shadows.integer == 0 )
	{
		return ( qfalse );
	}

	// send a trace down from the player to the ground
	VectorCopy( cent->lerpOrigin, end );
	end[2] -= SHADOW_DISTANCE;
	trap_CM_BoxTrace( &trace, cent->lerpOrigin, end, NULL, NULL, 0, MASK_PLAYERSOLID );

	// no shadow if too high
	//%	if ( trace.fraction == 1.0 || trace.fraction == 0.0f ) {
	//%		return qfalse;
	//%	}
	*shadowPlane = trace.endpos[2] + 1;
	if ( cg_shadows.integer != 1 )
	{																		// no mark for stencil or projection shadows
		return ( qtrue );
	}

	// no shadows when dead
	if ( cent->currentState.eFlags & EF_DEAD )
	{
		return ( qfalse );
	}

	// fade the shadow out with height
	//%	alpha = 1.0 - trace.fraction;
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

		//%	alpha *= distFade;
		// ydnar: decal remix
		//%	CG_ImpactMark( cgs.media.shadowTorsoShader, trace.endpos, trace.plane.normal,
		//%		0, alpha,alpha,alpha,1, qfalse, 16, qtrue, -1 );
		CG_ImpactMark( cgs.media.shadowTorsoShader, origin, projection, 18.0f, cent->lerpAngles[YAW], distFade,
					   distFade, distFade, distFade, -1 );
		return ( qtrue );
	}

	if ( dist < SHADOW_MAX_DIST )
	{	// show more detail
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
#if 0
				alpha = 1.0 - ( (origin[2] - (*shadowPlane + ZOFS)) / shadowParts[tagIndex].maxdist );
				if ( alpha < 0 )
				{
					continue;
				}

				if ( alpha > shadowParts[tagIndex].maxalpha )
				{
					alpha = shadowParts[tagIndex].maxalpha;
				}

				alpha *= ( 1.0 - distFade );
				origin[2] = *shadowPlane;
#endif
				AxisToAngles( axis, angles );

				// ydnar: decal remix
				//%	CG_ImpactMark( shadowParts[tagIndex].shader, origin, trace.plane.normal,
				//%		angles[YAW]/*cent->pe.legs.yawAngle*/, alpha,alpha,alpha,1, qfalse, shadowParts[tagIndex].size, qtrue, -1 );
				//%	CG_ImpactMark( shadowParts[ tagIndex ].shader, origin, up,
				//%			cent->lerpAngles[ YAW ], 1.0f, 1.0f, 1.0f, 1.0f, qfalse, shadowParts[ tagIndex ].size, qtrue, -1 );
				CG_ImpactMark( shadowParts[tagIndex].shader, origin, projection, shadowParts[tagIndex].size,
							   angles[YAW], distFade, distFade, distFade, distFade, -1 );
			}
		}
	}

	return ( qtrue );
}


/*
===============
CG_NPCSplash

Draw a mark at the water surface
===============
*/
static void
CG_NPCSplash ( centity_t *cent )
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


/*
===============
CG_NPCAnimation
===============
*/
static void
CG_NPCAnimation ( centity_t *cent, refEntity_t *body )
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
==================
CG_NPCSwingAngles
==================
*/
static void
CG_NPCSwingAngles
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
CG_NPCAddPainTwitch
=================
*/
static void
CG_NPCAddPainTwitch ( centity_t *cent, vec3_t torsoAngles )
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
===============
CG_NPCAngles

Handles seperate torso motion

  legs pivot based on direction of movement

  head always looks exactly at cent->lerpAngles

  if motion < 20 degrees, show in head only
  if < 45 degrees, also show in torso
===============
*/
static void
CG_NPCAngles ( centity_t *cent, vec3_t legs[3], vec3_t torso[3], vec3_t head[3] )
{
	vec3_t			legsAngles, torsoAngles, headAngles;
	float			dest;
	vec3_t			velocity;
	float			speed;
	float			clampTolerance;
	int				legsSet, torsoSet;
	clientInfo_t	*ci;
	bg_character_t	*character;
	ci = &cgs.clientinfo[cent->currentState.number];
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
		{
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

			// torso
			CG_NPCSwingAngles( torsoAngles[YAW], 0, clampTolerance, 0.06f, &cent->pe.torso.yawAngle,
							   &cent->pe.torso.yawing );
			CG_NPCSwingAngles( legsAngles[YAW], 0, clampTolerance, 0.06f, &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
		}
		else
		{

			// torso
			CG_NPCSwingAngles( torsoAngles[YAW], 25, clampTolerance, cg_swingSpeed.value, &cent->pe.torso.yawAngle,
							   &cent->pe.torso.yawing );

			// if the legs are yawing (facing heading direction), allow them to rotate a bit, so we don't keep calling
			// the legs_turn animation while an AI is firing, and therefore his angles will be randomizing according to their accuracy
			clampTolerance = 150;
			if ( BG_GetConditionBitFlag( ci->clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE) )
			{
				cent->pe.legs.yawing = qfalse;	// set it if they really need to swing
				CG_NPCSwingAngles( legsAngles[YAW], 20, clampTolerance, 0.5 * cg_swingSpeed.value,
								   &cent->pe.legs.yawAngle, &cent->pe.legs.yawing );
			}
			else if ( strstr( BG_GetAnimString( character->animModelInfo, legsSet), "strafe") )
			{

				// FIXME: what is this strstr hack??
				//if	( BG_GetConditionValue( ci->clientNum, ANIM_COND_MOVETYPE, qfalse ) & ((1<<ANIM_MT_STRAFERIGHT)|(1<<ANIM_MT_STRAFELEFT)) )
				cent->pe.legs.yawing = qfalse;	// set it if they really need to swing
				legsAngles[YAW] = headAngles[YAW];
				CG_NPCSwingAngles( legsAngles[YAW], 0, clampTolerance, cg_swingSpeed.value, &cent->pe.legs.yawAngle,
								   &cent->pe.legs.yawing );
			}
			else if ( cent->pe.legs.yawing )
			{
				CG_NPCSwingAngles( legsAngles[YAW], 0, clampTolerance, cg_swingSpeed.value, &cent->pe.legs.yawAngle,
								   &cent->pe.legs.yawing );
			}
			else
			{
				CG_NPCSwingAngles( legsAngles[YAW], 40, clampTolerance, cg_swingSpeed.value, &cent->pe.legs.yawAngle,
								   &cent->pe.legs.yawing );
			}
		}

		torsoAngles[YAW] = cent->pe.torso.yawAngle;
		legsAngles[YAW] = cent->pe.legs.yawAngle;
	}

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

	//CG_NPCSwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
	//torsoAngles[PITCH] = cent->pe.torso.pitchAngle;
	if ( cent->currentState.eFlags & EF_PRONE )
	{
		torsoAngles[PITCH] = legsAngles[PITCH] - 3;
	}
	else
	{
		CG_NPCSwingAngles( dest, 15, 30, 0.1, &cent->pe.torso.pitchAngle, &cent->pe.torso.pitching );
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
		if ( cent->currentState.eFlags & EF_TANK )
		{
			speed *= 0.02;
		}
		else
		{
			speed *= 0.05;
		}

		AnglesToAxis( legsAngles, axis );
		side = speed * DotProduct( velocity, axis[1] );
		legsAngles[ROLL] -= side;
		side = speed * DotProduct( velocity, axis[0] );
		legsAngles[PITCH] += side;
	}

	if ( cent->currentState.eFlags & EF_TANK )
	{
		trace_t trf, trb;
		vec3_t	forward, front, back, front2, back2;
		vec3_t	slopeangles, yawangles;
		yawangles[0] = yawangles[2] = 0;
		yawangles[1] = cent->lerpAngles[1];
		AngleVectors( yawangles, forward, NULL, NULL );

		// front
		VectorCopy( cent->lerpOrigin, front2 );
		VectorMA( front2, 32, forward, front2 );
		front2[2] -= 128;
		VectorCopy( cent->lerpOrigin, front );
		VectorMA( front, 32, forward, front );
		front[2] += 16;

		// back
		VectorCopy( cent->lerpOrigin, back2 );
		VectorMA( back2, -32, forward, back2 );
		back2[2] -= 128;
		VectorCopy( cent->lerpOrigin, back );
		VectorMA( back, -32, forward, back );
		back[2] += 16;

		// back
		CG_Trace( &trb, back, NULL, NULL, back2, cent->currentState.number, CONTENTS_SOLID );

		// front
		CG_Trace( &trf, front, NULL, NULL, front2, cent->currentState.number, CONTENTS_SOLID );

		// disable all just now
		if ( trf.endpos[2] - trb.endpos[2] > 0 )
		{
			slopeangles[0] = ( trb.endpos[2] - trf.endpos[2] ) / 2;
		}
		else
		{
			slopeangles[0] = ( trb.endpos[2] - trf.endpos[2] ) / 2;
		}

		slopeangles[0] = ( 4 * slopeangles[0] + cent->pe.lastSlope ) / 5;	// interpolate with older angle
		torsoAngles[0] = headAngles[0] = legsAngles[0] = slopeangles[0];
		cent->pe.lastSlope = slopeangles[0];
	}

	// pain twitch
	CG_NPCAddPainTwitch( cent, torsoAngles );

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
	AnglesToAxis( legsAngles, legs );
	AnglesToAxis( torsoAngles, torso );
	AnglesToAxis( headAngles, head );
}


/*
===============
CG_NPC_Vehicle
===============
*/
static void
CG_NPC_Vehicle ( centity_t *cent )
{
	refEntity_t		ent;
	refEntity_t		ent2;
	orientation_t	or;
	entityState_t	*s1;
	s1 = &cent->currentState;

	// create the render entity
	memset( &ent, 0, sizeof(ent) );
	memset( &ent2, 0, sizeof(ent2) );
	VectorCopy( cent->lerpOrigin, ent.origin );
	VectorCopy( cent->lerpOrigin, ent.oldorigin );
	VectorCopy( cent->lerpOrigin, ent2.origin );
	VectorCopy( cent->lerpOrigin, ent2.oldorigin );
	AnglesToAxis( cent->lerpAngles, ent.axis );
	AnglesToAxis( cent->lerpAngles, ent2.axis );
	ent.renderfx = 0;
	ent.skinNum = 0;
	ent2.renderfx = 0;
	ent2.skinNum = 0;
	ent.hModel = cgs.gameModels[s1->modelindex];
	trap_R_AddRefEntityToScene( &ent );
	if ( trap_R_LerpTag( &or, &ent, "tag_turret", 0) < 0 )
	{
		CG_Printf( "Tag tag_turret not found!\n" );
	}
	else
	{

		//CG_Printf("Tag tag_turret found at %f %f %f (parent at %f %f %f).\n", or.origin[0], or.origin[1], or.origin[2], ent.origin[0], ent.origin[1], ent.origin[2]);
		ent2.hModel = cgs.gameModels[s1->nextWeapon];
		VectorCopy( or.origin, ent2.origin );
		ent2.origin[0] += ent.origin[0];
		ent2.origin[1] += ent.origin[1];
		ent2.origin[2] += ent.origin[2];
		VectorCopy( or.origin, ent2.oldorigin );
		ent2.oldorigin[0] += ent.oldorigin[0];
		ent2.oldorigin[1] += ent.oldorigin[1];
		ent2.oldorigin[2] += ent.oldorigin[2];
		trap_R_AddRefEntityToScene( &ent2 );
		memcpy( &cent->refEnt2, &ent2, sizeof(refEntity_t) );
	}

	// add the secondary model
#ifdef __BLAH__
	if ( s1->nextWeapon /*s1->modelindex2*/ )
	{
		ent.hModel = cgs.gameModels[s1->nextWeapon /*s1->modelindex2*/ ];
		ent.frame = s1->frame;
		if ( s1->torsoAnim )
		{
			if ( cg.time >= cent->lerpFrame.frameTime )
			{
				cent->lerpFrame.oldFrameTime = cent->lerpFrame.frameTime;
				cent->lerpFrame.oldFrame = cent->lerpFrame.frame;

				//				while( cg.time >= cent->lerpFrame.frameTime ) {
				//					cent->lerpFrame.frameTime += s1->weapon;
				//					cent->lerpFrame.frame++;
				//					if( cent->lerpFrame.frame >= s1->legsAnim + s1->torsoAnim ) {
				//						cent->lerpFrame.frame = s1->legsAnim;						
				//					}
				//				}
			}

			if ( cent->lerpFrame.frameTime == cent->lerpFrame.oldFrameTime )
			{
				cent->lerpFrame.backlerp = 0;
			}
			else
			{
				cent->lerpFrame.backlerp = 1.0 - (float) ( cg.time - cent->lerpFrame.oldFrameTime ) / ( cent->lerpFrame.frameTime - cent->lerpFrame.oldFrameTime );
			}

			ent.frame = cent->lerpFrame.frame + s1->frame;			// offset
			if ( ent.frame >= s1->legsAnim + s1->torsoAnim )
			{
				ent.frame -= s1->torsoAnim;
			}

			ent.oldframe = cent->lerpFrame.oldFrame + s1->frame;	// offset
			if ( ent.oldframe >= s1->legsAnim + s1->torsoAnim )
			{
				ent.oldframe -= s1->torsoAnim;
			}

			ent.backlerp = cent->lerpFrame.backlerp;
		}

		if ( cent->trailTime )
		{
			cent->lerpFrame.oldFrame = cent->lerpFrame.frame;
			cent->lerpFrame.frame = s1->legsAnim;
			cent->lerpFrame.oldFrameTime = cent->lerpFrame.frameTime;
			cent->lerpFrame.frameTime = cg.time;
			ent.oldframe = ent.frame;
			ent.frame = s1->legsAnim;
			ent.backlerp = 0;
		}

		if ( cent->nextState.animMovetype != s1->animMovetype )
		{
			cent->trailTime = 1;
		}
		else
		{
			cent->trailTime = 0;
		}

		trap_R_AddRefEntityToScene( &ent );
		memcpy( &cent->refEnt, &ent, sizeof(refEntity_t) );
	}
	else
	{
		trap_R_AddRefEntityToScene( &ent );
	}
#endif //__BLAH__
}


/* */
void
CG_NPCAttachBitsToTank
(
	centity_t	*tank,
	refEntity_t *mg42base,
	refEntity_t *mg42upper,
	refEntity_t *mg42gun,
	refEntity_t *player,
	refEntity_t *flash,
	vec_t		*playerangles,
	const char	*tagName,
	qboolean	browning
)
{
	refEntity_t ent;
	vec3_t		angles;
	int			i;
	memset( mg42base, 0, sizeof(refEntity_t) );
	memset( mg42gun, 0, sizeof(refEntity_t) );
	memset( mg42upper, 0, sizeof(refEntity_t) );
	memset( player, 0, sizeof(refEntity_t) );
	memset( flash, 0, sizeof(refEntity_t) );
	mg42base->hModel = cgs.media.hMountedMG42Base;
	mg42upper->hModel = cgs.media.hMountedMG42Nest;
	if ( browning )
	{
		mg42gun->hModel = cgs.media.hMountedBrowning;
	}
	else
	{
		mg42gun->hModel = cgs.media.hMountedMG42;
	}

	if ( !CG_AddCEntity_Filter( tank) )
	{
		return;
	}

	if ( tank->tankframe != cg.clientFrame )
	{
		tank->tankframe = cg.clientFrame;
		memset( &ent, 0, sizeof(refEntity_t) );
		if ( tank->currentState.solid == SOLID_BMODEL )
		{
			ent.hModel = cgs.gameModels[tank->currentState.modelindex2];
		}
		else
		{
			ent.hModel = cgs.gameModels[tank->currentState.modelindex];
		}

		ent.frame = tank->lerpFrame.frame;
		ent.oldframe = tank->lerpFrame.oldFrame;
		ent.backlerp = tank->lerpFrame.backlerp;
		AnglesToAxis( tank->lerpAngles, ent.axis );

		//VectorCopy(		tank->lerpOrigin, ent.origin	);
		VectorCopy( tank->refEnt2.origin, ent.origin );
		AxisClear( mg42base->axis );
		CG_PositionEntityOnTag( mg42base, &ent, tagName, 0, NULL );
		VectorCopy( playerangles, angles );
		angles[PITCH] = 0;
		for ( i = 0; i < MAX_CLIENTS; i++ )
		{

			// Gordon: is this entity mounted on a tank, and attached to _OUR_ turret entity (which could be us)
			if
			(
				cg_entities[i].currentValid &&
				cg_entities[i].currentState.eFlags & EF_MOUNTEDTANK &&
				cg_entities[i].tagParent == tank - cg_entities
			)
			{
				angles[YAW] -= tank->lerpAngles[YAW];
				angles[PITCH] -= tank->lerpAngles[PITCH];
				break;
			}
		}

		AnglesToAxis( angles, mg42upper->axis );
		CG_PositionRotatedEntityOnTag( mg42upper, mg42base, "tag_mg42nest" );
		VectorCopy( playerangles, angles );
		angles[YAW] = 0;
		angles[ROLL] = 0;
		AnglesToAxis( angles, mg42gun->axis );
		CG_PositionRotatedEntityOnTag( mg42gun, mg42upper, "tag_mg42" );
		CG_PositionEntityOnTag( player, mg42upper, "tag_playerpo", 0, NULL );
		CG_PositionEntityOnTag( flash, mg42gun, "tag_flash", 0, NULL );
		CGRefEntityToTag( mg42base, &tank->mountedMG42Base );
		CGRefEntityToTag( mg42upper, &tank->mountedMG42Nest );
		CGRefEntityToTag( mg42gun, &tank->mountedMG42 );
		CGRefEntityToTag( player, &tank->mountedMG42Player );
		CGRefEntityToTag( flash, &tank->mountedMG42Flash );
	}

	CGTagToRefEntity( mg42base, &tank->mountedMG42Base );
	CGTagToRefEntity( mg42upper, &tank->mountedMG42Nest );
	CGTagToRefEntity( mg42gun, &tank->mountedMG42 );
	CGTagToRefEntity( player, &tank->mountedMG42Player );
	CGTagToRefEntity( flash, &tank->mountedMG42Flash );
}


/* */
void
CG_NPC_Vehicle_PostProcess ( centity_t *cent )
{
	refEntity_t mg42base;
	refEntity_t mg42upper;
	refEntity_t mg42gun;
	refEntity_t player;
	refEntity_t flash;
	vec_t		*angles;
	angles = cent->currentState.angles;

	//cg_entities[cent->currentState.effect3Time].tankparent = cent-cg_entities;
	CG_NPCAttachBitsToTank( cent, &mg42base, &mg42upper, &mg42gun, &player, &flash, angles, "tag_player",
							cent->currentState.density & 8 ? qtrue : qfalse );
	VectorCopy( mg42base.origin, mg42base.lightingOrigin );
	VectorCopy( mg42base.origin, mg42base.oldorigin );
	VectorCopy( mg42upper.origin, mg42upper.lightingOrigin );
	VectorCopy( mg42upper.origin, mg42upper.oldorigin );
	VectorCopy( mg42gun.origin, mg42gun.lightingOrigin );
	VectorCopy( mg42gun.origin, mg42gun.oldorigin );
	trap_R_AddRefEntityToScene( &mg42base );
	trap_R_AddRefEntityToScene( &mg42upper );
	trap_R_AddRefEntityToScene( &mg42gun );
}

/*
typedef struct animation_s {
	qhandle_t	mdxFile;
	char		name[MAX_QPATH];
	int			firstFrame;
	int			numFrames;
	int			loopFrames;			// 0 to numFrames
	int			frameLerp;			// msec between frames
	int			initialLerp;		// msec to get to first frame
	int			moveSpeed;
	int			animBlend;			// take this long to blend to next anim
} animation_t;
*/
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

/*
==============
CG_TANK - For driveable tanks...
==============
*/
extern void CG_RunLerpFrame ( centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, float speedScale );


/* */
void
CG_NPCTank ( centity_t *cent )
{
	entityState_t	*cs;
	lerpFrame_t		*tanklf;
	refEntity_t		body;
	refEntity_t		head;
	refEntity_t		acc;
	refEntity_t		flash;
	refEntity_t		tracks;
	vec3_t			playerOrigin;
	vec3_t			lightorigin;
	int				clientNum;
	int				renderfx;
	static int		startTime;
	float			shadowPlane;

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.clientNum;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS )
	{
		CG_Error( "Bad clientNum on player entity" );
	}

	/*	if ( cg.snap->ps.clientNum == cent->currentState.clientNum && (cg.snap->ps.eFlags & EF_TANK)) 
	{
		if ( cg.weapAltHoldTime < cg.time && cg.weapAltHoldTime != 0 ) 
		{    
			cg.weapAltHoldTime = 0;
			trap_SendConsoleCommand( "-attack2\n" );
			CG_Printf("Attack2 released!\n");
		}  
	}*/
	memset( &body, 0, sizeof(body) );
	memset( &body, 0, sizeof(head) );
	memset( &acc, 0, sizeof(acc) );
	memset( &flash, 0, sizeof(flash) );
	memset( &tracks, 0, sizeof(tracks) );
	VectorCopy( cent->lerpOrigin, playerOrigin );

	// get the rotation information @TODO fix all there
	CG_NPCAngles( cent, body.axis, body.torsoAxis, head.axis );

	// copy the torso rotation to the accessories
	AxisCopy( body.torsoAxis, acc.axis );

	// @TODO make this code so good that we can replace just turret with camera
	renderfx = 0;
	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
	{
		renderfx = RF_THIRD_PERSON;										// only draw in mirrors
	}

	if ( cg_shadows.integer == 3 || cg_shadows.integer == 4 )
	{
		renderfx |= RF_SHADOW_PLANE;
	}

#ifdef __FIXED_SHADOWS3__
	if ( cg_shadows.integer == 3 || cg_shadows.integer == 4 )
	{
		CG_ShadowPlane( cent, &shadowPlane );
	}
#endif //__FIXED_SHADOWS3__

	// draw the player in cameras
	if ( cg.cameraMode )
	{
		renderfx &= ~RF_THIRD_PERSON;
	}

	//
	renderfx |= RF_LIGHTING_ORIGIN;										// use the same origin for all

	// set renderfx for accessories
	acc.renderfx = renderfx;
	VectorCopy( playerOrigin, lightorigin );
	lightorigin[2] += 31;

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
		cent->pe.lightningFiring = 1;									// dont play start/stop sounds
		CG_TankSetAnim( cent, tanklf, 4 );
	}
	else if ( cg.time - cent->muzzleFlashTime >= 1200 && cent->pe.lightningFiring == 1 )
	{
		cent->pe.lightningFiring = 2;
	}

	if ( cent->currentState.effect2Time )
	{
		if ( tanklf->animationNumber != 1 && cent->pe.lightningFiring != 1 )
		{
			CG_TankSetAnim( cent, tanklf, 1 );
			if ( cent->pe.lightningFiring == 2 )
			{
				cent->pe.lightningFiring = 0;
			}
			else
			{

				//trap_S_StartSound (NULL, cs->number, CHAN_AUTO, cgs.media.tankStartSound ); // weird :(
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
			else
			{
				trap_S_StartSound( NULL, cs->number, CHAN_AUTO, cgs.media.tankStopSound );
			}
		}
		else
		{
			trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.tankIdleSound, 255, 0 );
		}
	}

	CG_RunLerpFrame( cent, NULL, tanklf, 0, 1 );						// use existing lerp code rather than re-writing
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
	body.shadowPlane = shadowPlane;
	VectorCopy( body.origin, body.oldorigin );							// don't positionally lerp at all
	cent->pe.bodyRefEnt = body;

	// (SA) only need to set this once...
	VectorCopy( lightorigin, acc.lightingOrigin );
	CG_AddRefEntityWithPowerups( &body, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
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
		}
	}

	// TURRET
	{
		vec3_t	turretAngles;
		vec3_t	tankAngles;
		float	diffirence;
		VectorCopy( cent->lerpAngles, turretAngles );
		turretAngles[2] = 0;
		turretAngles[0] = 0;
		AxisToAngles( body.axis, tankAngles );
		VectorCopy( lightorigin, acc.lightingOrigin );
		acc.renderfx = renderfx;
		acc.shadowPlane = shadowPlane;
		diffirence = abs( tankAngles[1] - turretAngles[1] );
		if ( cent->currentState.effect1Time == 1 )
		{
			turretAngles[1] = tankAngles[1];
		}

		if ( diffirence > 10 && diffirence < 350 && cent->currentState.effect1Time != 1 )
		{
			trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.tankTurretSound, 255, 0 );
			cent->pe.turretRotating = qtrue;
		}

		if ( (diffirence <= 10 || diffirence >= 350) && cent->pe.turretRotating && cent->currentState.effect1Time != 1 )
		{
			trap_S_StartSound( NULL, cs->number, CHAN_AUTO, cgs.media.tankTurretEndSound );
			cent->pe.turretRotating = qfalse;
		}

		acc.hModel = cgs.media.tankTurretModel[cent->currentState.effect1Time];
		acc.customSkin = 0;
		CG_PositionEntityOnTag( &acc, &body, "tag_turret", 0, NULL );
		AnglesToAxis( turretAngles, acc.axis );
		CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, 0, &cent->currentState, cent->fireRiseDir );
		{
			CG_PositionEntityOnTag( &flash, &acc, "tag_flash", 0, NULL );
			AnglesToAxis( turretAngles, flash.axis );
			if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )
			{
				CG_AddRefEntityWithPowerups( &flash, cent->currentState.powerups, 0, &cent->currentState,
											 cent->fireRiseDir );
				trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );
			}
		}
	}

	// THIS WILL SET UP TRACKS SHADERS
	VectorCopy( lightorigin, tracks.lightingOrigin );
	VectorCopy( body.origin, tracks.origin );
	AxisCopy( body.axis, tracks.axis );
	tracks.renderfx = body.renderfx;
	tracks.shadowPlane = shadowPlane;
	if ( cent->currentState.effect2Time > 0 && cent->currentState.effect1Time == 0 )
	{
		tracks.customShader = cgs.media.tankShader[0];
	}
	else if ( cent->currentState.effect2Time > 0 && cent->currentState.effect1Time == 1 )
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

	//	memcpy( &cent->refEnt, &body, sizeof(refEntity_t) );
}

/*
===============
CG_NPC
===============
*/
extern qboolean classes_registerred;
extern void		CG_RegisterPlayerClasses ( void );
extern qboolean CG_SpawnSmokeSprite ( centity_t *cent, float dist );	// For smoking planes hehehe...
extern void		CG_RenderSmokeGrenadeSmoke ( centity_t *cent, const weaponInfo_t *weapon );
extern void		CG_CallMissile ( centity_t *cent );
extern int		Q_irand ( int value1, int value2 );


/* */
void
CG_NPC ( centity_t *cent )
{
	clientInfo_t	*ci;
	refEntity_t		body;
	refEntity_t		head;
	refEntity_t		acc;
	vec3_t			playerOrigin;
	vec3_t			lightorigin;
	int				clientNum, i;
	int				renderfx;
	qboolean		shadow;
	float			shadowPlane;

	//	float			gumsflappin = 0;	// talking amplitude
	qboolean		usingBinocs = qfalse;
	centity_t		*cgsnap;
	bg_character_t	*character;
	float			hilightIntensity = 0.f;
	team_t			team;
	if ( !classes_registerred )
	{
		CG_RegisterPlayerClasses();
	}

#ifdef __WEAPON_AIM_OLD__
	if
	(
		(cent->currentState.eFlags & EF_AIMING) &&
		cent->currentState.weapon != WP_COLT &&
		cent->currentState.weapon != WP_LUGER &&
		cent->currentState.weapon != WP_PANZERFAUST
#ifdef __ETE__
		&&
		cent->currentState.weapon != WP_BAZOOKA
#endif //__ETE__
		&&
		cent->currentState.weapon != WP_MOBILE_MG42
	)
	{
		cent->currentState.torsoAnim = 78;
	}
#endif //__WEAPON_AIM_OLD__
	cgsnap = &cg_entities[cg.snap->ps.clientNum];
	shadow = qfalse;											// gjd added to make sure it was initialized
	shadowPlane = 0.0;											// ditto

	// if set to invisible, skip
	if ( cent->currentState.eFlags & EF_NODRAW )
	{
		return;
	}

	if ( cent->currentState.modelindex2 == CLASS_PLANE_BOMBER )
	{															// A bomber plane model...
		refEntity_t model;
		memset( &model, 0, sizeof(model) );
		model.reType = RT_MODEL;
		VectorCopy( cent->lerpOrigin, playerOrigin );
		VectorCopy( playerOrigin, model.lightingOrigin );
		VectorCopy( playerOrigin, model.origin );
		VectorCopy( playerOrigin, model.oldorigin );
		AnglesToAxis( cent->lerpAngles, model.axis );
		model.hModel = cgs.gameModels[cent->currentState.modelindex];
		trap_R_AddRefEntityToScene( &model );
		cent->pe.bodyRefEnt = model;
		{
			vec3_t		angles;
			refEntity_t prop;
			memset( &prop, 0, sizeof(refEntity_t) );
			prop.hModel = cgs.media.hPropeller;
			VectorCopy( cent->lerpAngles, angles );
			if ( cent->usehighlightOrigin )
			{
				VectorCopy( cent->origin2, angles );
			}

			angles[YAW] += 20;
			if ( angles[YAW] > 360 )
			{
				angles[YAW] = 0;
			}

			angles[ROLL] = 0;
			VectorCopy( angles, cent->origin2 );
			cent->usehighlightOrigin = qtrue;
			AnglesToAxis( angles, prop.axis );
			CG_PositionRotatedEntityOnTag( &prop, &model, "tag_prop" );
			trap_R_AddRefEntityToScene( &prop );
		}

		//CGRefEntityToTag( mg42base,		&tank->mountedMG42Base		);
		//CG_PositionEntityOnTag( player, mg42upper,	"tag_playerpo", 0, NULL );
		return;
	}

	/*	else if ( cent->currentState.modelindex2 == CLASS_LCVP )
	{// A LCVP lander boat model...
		refEntity_t		model;
		bg_character_t	*character = CG_GetCharacter( TEAM_ALLIES, PC_SOLDIER );

		character->animModelInfo = BG_GetCharacter( TEAM_ALLIES, PC_SOLDIER )->animModelInfo;

		memset(&model, 0, sizeof(model));
		model.reType = RT_MODEL;
		
		VectorCopy( cent->lerpOrigin, playerOrigin );
		VectorCopy( playerOrigin, model.lightingOrigin );
		VectorCopy( playerOrigin, model.origin );
		VectorCopy( playerOrigin, model.oldorigin);

		AnglesToAxis( cent->lerpAngles, model.axis );

		model.entityNum = cent->currentState.number;

		model.hModel = cgs.gameModels[cent->currentState.modelindex];

		trap_R_AddRefEntityToScene( &model );

		cent->pe.bodyRefEnt = model;

		// Add the spawnpoints...
		{
			vec3_t			angles;
			refEntity_t		prop;
			memset( &prop,	0,	sizeof( refEntity_t	));
			
			prop.hModel = cgs.media.hLCVPSpawns;

			VectorCopy( cent->lerpAngles, angles );
			angles[YAW] = 0;
			angles[ROLL] = 0;
			VectorCopy(angles, cent->origin2);

			AnglesToAxis( angles, prop.axis );

			//CG_PositionRotatedEntityOnTag( &prop, &model, "tag_cargo" );
			CG_PositionRotatedEntityOnTag( &prop, &model, "tag_origin" );
			//prop.origin[2]+=16;
			trap_R_AddRefEntityToScene( &prop );

			// Make spawns refent2...
			cent->refEnt2 = prop;
		}

		// Add the door/ramp...
		{
			vec3_t			angles;
			refEntity_t		prop;
			memset( &prop,	0,	sizeof( refEntity_t	));
			
			prop.hModel = cgs.media.hLCVPRamp;

			VectorCopy( cent->lerpAngles, angles );
			angles[YAW] = 0;
			angles[ROLL] = 0;
			VectorCopy(angles, cent->origin2);

			AnglesToAxis( angles, prop.axis );
			CG_PositionRotatedEntityOnTag( &prop, &model, "tag_ramp" );
			trap_R_AddRefEntityToScene( &prop );
		}

		// Add the propeller mist/bubbles...
		if (!cent->highlightTime)
		{
			if (cent->entityPS)
				CG_DestroyParticleSystem( &cent->entityPS );

			cent->entityPS = CG_SpawnNewParticleSystem( cgs.media.mistTrailPS );
			
			// We're set up...
			cent->highlightTime = 1;
		}

		CG_SetParticleSystemTag( cent->entityPS, model, model.hModel, "tag_propwash" );
		CG_SetParticleSystemCent( cent->entityPS, cent );
		CG_AttachParticleSystemToTag( cent->entityPS );

		// Add the port mg...
		{
			vec3_t			angles;
			refEntity_t		gun;
			refEntity_t		nest;
			refEntity_t		player;

			memset( &player,	0,	sizeof( refEntity_t	));
			player.customSkin	= character->skin;
			player.hModel		= character->mesh;

			VectorCopy( cent->lerpAngles, angles );
			angles[YAW] = 0;
			angles[ROLL] = 0;
			VectorCopy(angles, cent->origin2);

			AnglesToAxis( angles, player.axis );
			CG_PositionRotatedEntityOnTag( &player, &model, "tag_portgunner" );
			trap_R_AddRefEntityToScene( &player );

			// The nest...
			memset( &nest,	0,	sizeof( refEntity_t	));
			
			nest.hModel = cgs.media.hMountedMG42Nest;

			AnglesToAxis( angles, nest.axis );
			CG_PositionRotatedEntityOnTag( &nest, &model, "tag_portmg" );
			nest.origin[2]-=10;
			trap_R_AddRefEntityToScene( &nest );

			// And the gun...
			memset( &gun,	0,	sizeof( refEntity_t	));
			
			gun.hModel = cgs.media.hLCVPMG;

			AnglesToAxis( angles, gun.axis );
			CG_PositionRotatedEntityOnTag( &gun, &nest, "tag_mg42" );
			trap_R_AddRefEntityToScene( &gun );
		}

		// Add the starboard mg...
		{
			vec3_t			angles;
			refEntity_t		gun;
			refEntity_t		nest;
			refEntity_t		player;
			
			memset( &player,	0,	sizeof( refEntity_t	));
			player.customSkin	= character->skin;
			player.hModel		= character->mesh;

			VectorCopy( cent->lerpAngles, angles );
			angles[YAW] = 0;
			angles[ROLL] = 0;
			VectorCopy(angles, cent->origin2);

			AnglesToAxis( angles, player.axis );
			CG_PositionRotatedEntityOnTag( &player, &model, "tag_stbdgunner" );
			trap_R_AddRefEntityToScene( &player );

			// The nest...
			memset( &nest,	0,	sizeof( refEntity_t	));
			
			nest.hModel = cgs.media.hMountedMG42Nest;

			AnglesToAxis( angles, nest.axis );
			CG_PositionRotatedEntityOnTag( &nest, &model, "tag_stbdmg" );
			nest.origin[2]-=10;
			trap_R_AddRefEntityToScene( &nest );

			// And the gun...
			memset( &gun,	0,	sizeof( refEntity_t	));
			
			gun.hModel = cgs.media.hLCVPMG;

			AnglesToAxis( angles, gun.axis );
			CG_PositionRotatedEntityOnTag( &gun, &nest, "tag_mg42" );
			trap_R_AddRefEntityToScene( &gun );
		}

		return;
	}
	else if ( cent->currentState.modelindex2 == CLASS_DESTROYER )
	{// A Destroyer boat model...
		refEntity_t		model;

		memset(&model, 0, sizeof(model));
		model.reType = RT_MODEL;
		
		VectorCopy( cent->lerpOrigin, playerOrigin );
		VectorCopy( playerOrigin, model.lightingOrigin );
		VectorCopy( playerOrigin, model.origin );
		VectorCopy( playerOrigin, model.oldorigin);

		AnglesToAxis( cent->lerpAngles, model.axis );

		model.hModel = cgs.gameModels[cent->currentState.modelindex];

		trap_R_AddRefEntityToScene( &model );

		cent->pe.bodyRefEnt = model;

		// Add the propeller mist/bubbles...
		
		// Add the turret gun...
		{
			vec3_t			offset;
			refEntity_t		prop;
			memset( &prop,	0,	sizeof( refEntity_t	));

			VectorSet(offset, 255, 0, -12);
			
			prop.hModel = cgs.media.hDestroyerGun;

			//VectorCopy( cent->lerpAngles, angles );
			//angles[PITCH] = 0;
			//angles[YAW] = 90;
			//angles[ROLL] = 0;

			//AnglesToAxis( angles, prop.axis );
			//CG_PositionRotatedEntityOnTag( &prop, &model, "tag_cannon" );
			CG_PositionEntityOnTag( &prop, &model, "tag_cannon", 0, &offset );
			trap_R_AddRefEntityToScene( &prop );

			// Add the fire flashes every so often...
			if (Q_irand(0, 1000) < 5 && cent->tankparent < cg.time)
			{
				if (!cent->lastFuseSparkTime)
				{
					cent->entityPS2 = CG_SpawnNewParticleSystem( cgs.media.flameTrailPS );
			
					// We're set up...
					cent->lastFuseSparkTime = 1;
				}

				CG_SetParticleSystemTag( cent->entityPS2, prop, prop.hModel, "tag_rocket1" );
				CG_SetParticleSystemCent( cent->entityPS2, cent );
				CG_AttachParticleSystemToTag( cent->entityPS2 );
				cent->tankparent = cg.time + 200;
			}

			if (cent->tankparent < cg.time && cent->entityPS2)
			{
				CG_DestroyParticleSystem( &cent->entityPS2 );
				cent->lastFuseSparkTime = 0;
			}

			if (Q_irand(0, 1000) < 5 && cent->trailTime < cg.time)
			{
				if (!cent->muzzleFlashTime)
				{
					cent->entityPS3 = CG_SpawnNewParticleSystem( cgs.media.flameTrailPS );
			
					// We're set up...
					cent->muzzleFlashTime = 1;
					cent->trailTime = cg.time + 200;
				}

				CG_SetParticleSystemTag( cent->entityPS3, prop, prop.hModel, "tag_rocket2" );
				CG_SetParticleSystemCent( cent->entityPS3, cent );
				CG_AttachParticleSystemToTag( cent->entityPS3 );
			}

			if (cent->trailTime < cg.time && cent->entityPS3)
			{
				CG_DestroyParticleSystem( &cent->entityPS3 );
				cent->muzzleFlashTime = 0;
			}
		}

		// Add the flag...
		{
			vec3_t			angles;//, offset;
			refEntity_t		prop;
			memset( &prop,	0,	sizeof( refEntity_t	));

			//VectorSet(offset, 255, 0, -12);
			
			prop.hModel = cgs.media.hDestroyerFlag;

			VectorCopy( cent->lerpAngles, angles );
			angles[PITCH] = 0;
			angles[YAW] = 90;
			angles[ROLL] = 0;

			AnglesToAxis( angles, prop.axis );
			CG_PositionRotatedEntityOnTag( &prop, &model, "tag_flag" );
			//CG_PositionEntityOnTag( &prop, &model, "tag_flag", 0, NULL );
			trap_R_AddRefEntityToScene( &prop );
		}

		// Add the radar...
		{
			vec3_t			angles;//, offset;
			refEntity_t		prop;
			memset( &prop,	0,	sizeof( refEntity_t	));

			//VectorSet(offset, 255, 0, -12);
			
			prop.hModel = cgs.media.hDestroyerRadar;

			VectorCopy( cent->lerpAngles, angles );
			angles[PITCH] = 0;
			angles[YAW] = 90;
			angles[ROLL] = 0;

			AnglesToAxis( angles, prop.axis );
			CG_PositionRotatedEntityOnTag( &prop, &model, "tag_radar" );
			//CG_PositionEntityOnTag( &prop, &model, "tag_radar", 0, NULL );
			trap_R_AddRefEntityToScene( &prop );
		}

		return;
	}*/
	else if ( cent->currentState.modelindex2 == CLASS_PLANE )
	{															// A bomber plane model...
		refEntity_t model;
		memset( &model, 0, sizeof(model) );
		model.reType = RT_MODEL;
		VectorCopy( cent->lerpOrigin, playerOrigin );
		VectorCopy( playerOrigin, model.lightingOrigin );
		VectorCopy( playerOrigin, model.origin );
		VectorCopy( playerOrigin, model.oldorigin );
		AnglesToAxis( cent->lerpAngles, model.axis );
		model.hModel = cgs.gameModels[cent->currentState.modelindex];
		if ( cent->currentState.weapon == WP_SMOKETRAIL )
		{														// Add smoke effects...
			if ( !cent->highlightTime )
			{													// Which tag should we attach flames to?
				if ( cent->entityPS )
				{
					CG_DestroyParticleSystem( &cent->entityPS );
				}

				if ( cent->entityPS2 )
				{
					CG_DestroyParticleSystem( &cent->entityPS2 );
				}

				cent->highlightTime = Q_irand( 1, 3 );			//Q_TrueRand(1,3);
				if ( cent->highlightTime <= 2 )
				{
					cent->entityPS = CG_SpawnNewParticleSystem( cgs.media.flameTrailPS );
				}

				if ( cent->highlightTime > 2 )
				{
					cent->entityPS2 = CG_SpawnNewParticleSystem( cgs.media.flameTrailPS );
				}
			}

			if ( cent->highlightTime == 1 )
			{													// Gun0
				CG_SetParticleSystemTag( cent->entityPS, model, model.hModel, "tag_gun0" );
			}
			else if ( cent->highlightTime == 3 )
			{													// Gun1
				CG_SetParticleSystemTag( cent->entityPS, model, model.hModel, "tag_gun1" );
			}
			else
			{													// Guns 1 and 2
				CG_SetParticleSystemTag( cent->entityPS, model, model.hModel, "tag_gun0" );
				CG_SetParticleSystemTag( cent->entityPS, model, model.hModel, "tag_gun1" );
			}

			CG_SetParticleSystemCent( cent->entityPS, cent );
			CG_AttachParticleSystemToTag( cent->entityPS );
		}

		trap_R_AddRefEntityToScene( &model );
		{
			vec3_t		angles;
			refEntity_t prop;
			memset( &prop, 0, sizeof(refEntity_t) );
			prop.hModel = cgs.media.hPropeller;
			VectorCopy( cent->lerpAngles, angles );
			if ( cent->usehighlightOrigin )
			{
				VectorCopy( cent->origin2, angles );
			}

			angles[YAW] += 20;
			if ( angles[YAW] > 360 )
			{
				angles[YAW] = 0;
			}

			angles[ROLL] = 0;
			VectorCopy( angles, cent->origin2 );
			cent->usehighlightOrigin = qtrue;
			AnglesToAxis( angles, prop.axis );
			CG_PositionRotatedEntityOnTag( &prop, &model, "tag_prop" );
			trap_R_AddRefEntityToScene( &prop );
		}

		if ( cent->currentState.aiState == 1 )
		{
			if ( cent->entityPS )
			{
				CG_DestroyParticleSystem( &cent->entityPS );
			}
		}

		return;
	}
	else if ( cent->currentState.modelindex2 == CLASS_VEHICLE )
	{															// A tank model...
		CG_NPC_Vehicle( cent );
		CG_NPC_Vehicle_PostProcess( cent );
		return;
	}

	// the client number is stored in clientNum.  It can't be derived
	// from the entity number, because a single client may have
	// multiple corpses on the level using the same clientinfo
	clientNum = cent->currentState.number;
	cent->NPC_client = cgs.clientinfo[clientNum];
	ci = &cent->NPC_client;

	// FIXME: We need to set these values server side and allow different characters... Unique1
	team = cent->currentState.teamNum;
	ci->team = team;

	//	character = CG_CharacterForClientinfo( ci, cent );
	ci->cls = cent->currentState.density;						//cent->currentState.modelindex2;//PC_SOLDIER;
	character = CG_GetCharacter( ci->team, ci->cls );			//BG_GetCharacter( ci->team, ci->cls );
	ci->character = character;
	ci->character->animModelInfo = BG_GetCharacter( team, ci->cls )->animModelInfo;
	Q_strncpyz( ci->name, "NPC", sizeof("NPC") );
	ci->clientNum = clientNum;
#ifdef __NPC__

	/*if( cg_entities[cg_entities[cent->currentState.clientNum].tagParent].currentState.eType == ET_NPC 
		&& cg_entities[cent->currentState.clientNum].currentState.dl_intensity == TAG_UNHOOK ) 
	{// Need to release the tag!
		cent->tagParent = -1;
		Q_strncpyz( cent->tagName, "", sizeof(cent->tagName) );
		cent->currentState.eFlags &= ~EF_MOUNTEDTANK;
		//CG_Printf("Disconnect entity %i.\n", cent->currentState.number);
	}

	if( cg_entities[cent->currentState.clientNum].currentState.dl_intensity == TAG_HOOK ) 
	{// Need to join to a spawn tag!
		cent->tagParent = cent->currentState.otherEntityNum;
		Q_strncpyz( cent->tagName, va("tag_spawnpoint%i", cent->currentState.otherEntityNum2), sizeof(cent->tagName) );
		cent->currentState.eFlags |= EF_MOUNTEDTANK;
		//CG_Printf("Connect entity %i to %i.\n", cent->currentState.number, cent->currentState.otherEntityNum);
	}
	else if( cg_entities[cent->currentState.clientNum].currentState.dl_intensity == TAG_HOOK_DRIVER ) 
	{// Need to join to a driver tag!
		cent->tagParent = cent->currentState.otherEntityNum;
		Q_strncpyz( cent->tagName, "tag_driver", sizeof(cent->tagName) );
		cent->currentState.eFlags |= EF_MOUNTEDTANK;
		//CG_Printf("Connect entity %i to %i.\n", cent->currentState.number, cent->currentState.otherEntityNum);
	}
	else if( cg_entities[cent->currentState.clientNum].currentState.dl_intensity == TAG_HOOK_GUNNER1 ) 
	{// Need to join to a gunner tag!
		cent->tagParent = cent->currentState.otherEntityNum;
		Q_strncpyz( cent->tagName, "tag_gunner1", sizeof(cent->tagName) );
		cent->currentState.eFlags |= EF_MOUNTEDTANK;
		//CG_Printf("Connect entity %i to %i.\n", cent->currentState.number, cent->currentState.otherEntityNum);
	}
	else if( cg_entities[cent->currentState.clientNum].currentState.dl_intensity == TAG_HOOK_GUNNER2 ) 
	{// Need to join to a gunner tag!
		cent->tagParent = cent->currentState.otherEntityNum;
		Q_strncpyz( cent->tagName, "tag_gunner2", sizeof(cent->tagName) );
		cent->currentState.eFlags |= EF_MOUNTEDTANK;
		//CG_Printf("Connect entity %i to %i.\n", cent->currentState.number, cent->currentState.otherEntityNum);
	}

	if( cg_entities[cg_entities[cent->currentState.clientNum].tagParent].currentState.eType == ET_NPC ) 
	{// Attached to an NPC...
		if (!Q_stricmp( cg_entities[cent->currentState.clientNum].tagName, "tag_driver" ))
		{
			refEntity_t refent = cg_entities[cg_entities[cent->currentState.clientNum].tagParent].pe.bodyRefEnt;
			VectorCopy(CG_GetTagOrigin( refent, cg_entities[cent->currentState.clientNum].tagName, NULL ), playerOrigin);
			VectorCopy(cg_entities[refent.entityNum].lerpAngles, cent->currentState.angles);
			playerOrigin[2]+=18; // If not a gunner, we need to correct the spawnpoint... grrr...
		}
		else
		{
			refEntity_t refent = cg_entities[cg_entities[cent->currentState.clientNum].tagParent].refEnt2;			
			VectorCopy(CG_GetTagOrigin( refent, cg_entities[cent->currentState.clientNum].tagName, NULL ), playerOrigin);
			VectorCopy(cg_entities[refent.entityNum].lerpAngles, cent->currentState.angles);
			
			if (Q_stricmp( cg_entities[cent->currentState.clientNum].tagName, "tag_portgunner" ) && Q_stricmp( cg_entities[cent->currentState.clientNum].tagName, "tag_stbdgunner" ))
				playerOrigin[2]+=18; // If not a gunner, we need to correct the spawnpoint... grrr...
		}
	} 
	else */
	if ( cg_entities[cent->currentState.clientNum].currentState.dl_intensity == TAG_HOOK )
	{															// Need to join to a spawn tag!
		cg_entities[cent->currentState.clientNum].tagParent = cg_entities[cent->currentState.clientNum].currentState.otherEntityNum;
		Q_strncpyz( cg_entities[cent->currentState.clientNum].tagName,
					va( "tag_spawnpoint%i", cg_entities[cent->currentState.clientNum].currentState.otherEntityNum2),
					sizeof(cg_entities[cent->currentState.clientNum].tagName) );
		cg_entities[cent->currentState.clientNum].currentState.eFlags |= EF_MOUNTEDTANK;

		//CG_Printf("Connect entity %i to %i (position %i).\n", cg_entities[cent->currentState.clientNum].currentState.number, cg_entities[cent->currentState.clientNum].currentState.otherEntityNum, cg_entities[cent->currentState.clientNum].currentState.otherEntityNum2);
	}
	else if ( cg_entities[cent->currentState.clientNum].currentState.dl_intensity == TAG_HOOK_DRIVER )
	{															// Need to join to a driver tag!
		cg_entities[cent->currentState.clientNum].tagParent = cg_entities[cent->currentState.clientNum].currentState.otherEntityNum;
		Q_strncpyz( cg_entities[cent->currentState.clientNum].tagName, "tag_driver",
					sizeof(cg_entities[cent->currentState.clientNum].tagName) );
		cg_entities[cent->currentState.clientNum].currentState.eFlags |= EF_MOUNTEDTANK;

		//CG_Printf("Connect entity %i to %i (As Driver).\n", cg_entities[cent->currentState.clientNum].currentState.number, cg_entities[cent->currentState.clientNum].currentState.otherEntityNum);
	}
	else if ( cg_entities[cent->currentState.clientNum].currentState.dl_intensity == TAG_HOOK_GUNNER1 )
	{															// Need to join to a gunner tag!
		cg_entities[cent->currentState.clientNum].tagParent = cent->currentState.otherEntityNum;
		Q_strncpyz( cg_entities[cent->currentState.clientNum].tagName, "tag_gunner1",
					sizeof(cg_entities[cent->currentState.clientNum].tagName) );
		cg_entities[cent->currentState.clientNum].currentState.eFlags |= EF_MOUNTEDTANK;

		//CG_Printf("Connect entity %i to %i (As Gunner1).\n", cg_entities[cent->currentState.clientNum].currentState.number, cg_entities[cent->currentState.clientNum].currentState.otherEntityNum);
	}
	else if ( cg_entities[cent->currentState.clientNum].currentState.dl_intensity == TAG_HOOK_GUNNER2 )
	{															// Need to join to a gunner tag!
		cg_entities[cent->currentState.clientNum].tagParent = cg_entities[cent->currentState.clientNum].currentState.otherEntityNum;
		Q_strncpyz( cg_entities[cent->currentState.clientNum].tagName, "tag_gunner2",
					sizeof(cg_entities[cent->currentState.clientNum].tagName) );
		cg_entities[cent->currentState.clientNum].currentState.eFlags |= EF_MOUNTEDTANK;

		//CG_Printf("Connect entity %i to %i (As Gunner2).\n", cg_entities[cent->currentState.clientNum].currentState.number, cg_entities[cent->currentState.clientNum].currentState.otherEntityNum);
	}

	if
	(
		cg_entities[cg_entities[cent->currentState.clientNum].tagParent].currentState.eType == ET_NPC &&
		cg_entities[cent->currentState.clientNum].currentState.dl_intensity == TAG_UNHOOK
	)
	{															// Need to release the tag!
		cg_entities[cent->currentState.clientNum].tagParent = -1;
		Q_strncpyz( cg_entities[cent->currentState.clientNum].tagName, "",
					sizeof(cg_entities[cent->currentState.clientNum].tagName) );
		cg_entities[cent->currentState.clientNum].currentState.eFlags &= ~EF_MOUNTEDTANK;

		//CG_Printf("Disconnect entity %i.\n", cg_entities[cent->currentState.clientNum].currentState.number);
	}

	if ( cg_entities[cg_entities[cent->currentState.clientNum].tagParent].currentState.eType == ET_NPC )
	{															// Attached to an NPC...
		if ( !Q_stricmp( cg_entities[cent->currentState.clientNum].tagName, "tag_driver") )
		{
			refEntity_t refent = cg_entities[cg_entities[cent->currentState.clientNum].tagParent].pe.bodyRefEnt;
			VectorCopy( CG_GetTagOrigin( refent, cg_entities[cent->currentState.clientNum].tagName, NULL), playerOrigin );
			VectorCopy( cg_entities[refent.entityNum].lerpAngles,
						cg_entities[cent->currentState.clientNum].currentState.angles );
			playerOrigin[2] += 18;
		}
		else
		{
			refEntity_t refent = cg_entities[cg_entities[cent->currentState.clientNum].tagParent].refEnt2;
			VectorCopy( CG_GetTagOrigin( refent, cg_entities[cent->currentState.clientNum].tagName, NULL), playerOrigin );
			if
			(
				Q_stricmp( cg_entities[cent->currentState.clientNum].tagName, "tag_portgunner") &&
				Q_stricmp( cg_entities[cent->currentState.clientNum].tagName, "tag_stbdgunner")
			)
			{
				playerOrigin[2] += 18;							// If not a gunner, we need to correct the spawnpoint... grrr...
			}

			VectorCopy( cg_entities[refent.entityNum].lerpAngles,
						cg_entities[cent->currentState.clientNum].currentState.angles );
		}
	}
	else
#endif //__NPC__
		if ( cent->currentState.eFlags & EF_MOUNTEDTANK )
		{
			VectorCopy( cg_entities[cg_entities[cent->currentState.clientNum].tagParent].mountedMG42Player.origin,
						playerOrigin );
		}
		else if ( cent->currentState.eFlags & EF_MG42_ACTIVE || cent->currentState.eFlags & EF_AAGUN_ACTIVE )
		{														// Arnout: see if we're attached to a gun
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
	CG_NPCAngles( cent, body.axis, body.torsoAxis, head.axis );

	// FIXME: move this into CG_NPCAngles
	if ( cgsnap == cent && (cg.snap->ps.pm_flags & PMF_LADDER) )
	{
		memcpy( body.torsoAxis, body.axis, sizeof(body.torsoAxis) );
	}

	// copy the torso rotation to the accessories
	AxisCopy( body.torsoAxis, acc.axis );
#ifdef __NPC__
	if ( cg_entities[cg_entities[cent->currentState.clientNum].tagParent].currentState.eType == ET_NPC )
	{															// Attached to an NPC... Ready/MG42 anim...
		if ( !Q_stricmp( cg_entities[cent->currentState.clientNum].tagName, "tag_driver") )
		{

			//pm->ps->weaponstate == WEAPON_RELAXING
			//cg.snap->ps.weaponstate = WEAPON_RELAXING;
			ci->weaponState = WSTATE_IDLE;
			cent->currentState.legsAnim = LEGS_IDLE;
			cent->currentState.torsoAnim = TORSO_RELAX;			//TORSO_READY;
		}
		else
		{
			ci->weaponState = WSTATE_IDLE;
			if ( cent->currentState.legsAnim != LEGS_IDLE && cent->currentState.legsAnim != LEGS_IDLE_ALT )
			{
				if ( Q_TrueRand( 0, 1) == 0 )
				{
					cent->currentState.legsAnim = LEGS_IDLE;	//LEGS_IDLE_ALT
				}
				else
				{
					cent->currentState.legsAnim = LEGS_IDLE_ALT;
				}
			}

			if (												/*cent->currentState.torsoAnim != TORSO_READY 
				&& cent->currentState.torsoAnim != TORSO_READY2
				&& cent->currentState.torsoAnim != TORSO_READY3
				&& cent->currentState.torsoAnim != TORSO_READY4
				&& cent->currentState.torsoAnim != TORSO_READY5*/
					cent->currentState.torsoAnim != TORSO_RELAX
				)
			{

				/*int choice = Q_TrueRand(0,4);

				if (choice == 0)
				{
					cent->currentState.torsoAnim = TORSO_READY;
				}
				else if (choice == 1)
				{
					cent->currentState.torsoAnim = TORSO_READY2;
				}
				else if (choice == 2)
				{
					cent->currentState.torsoAnim = TORSO_READY3;
				}
				else if (choice == 3)
				{
					cent->currentState.torsoAnim = TORSO_READY4;
				}
				else
				{
					cent->currentState.torsoAnim = TORSO_READY5;
				}*/
				cent->currentState.torsoAnim = TORSO_RELAX;
			}
		}
	}
#endif //__NPC__

	// calculate client-side conditions
	CG_AnimPlayerConditions( character, cent );

	// get the animation state (after rotation, to allow feet shuffle)
	CG_NPCAnimation( cent, &body );

	// forcibly set binoc animation
	if ( cent->currentState.eFlags & EF_ZOOMING )
	{
		usingBinocs = qtrue;
	}

	// add the any sprites hovering above the player
	// rain - corpses don't get icons (fireteam check ran out of bounds)
	if ( cent->currentState.eType != ET_CORPSE )
	{
		CG_NPCSprites( cent );
	}

	// add a water splash if partially in and out of water
	CG_NPCSplash( cent );
#ifdef __FIXED_SHADOWS3__
	shadow = CG_NPCShadow( cent, &shadowPlane );
#endif

	// get the player model information
	renderfx = 0;
	if ( cent->currentState.number == cg.snap->ps.clientNum && !cg.renderingThirdPerson )
	{
		renderfx = RF_THIRD_PERSON;								// only draw in mirrors
	}

	// draw the player in cameras
	if ( cg.cameraMode )
	{
		renderfx &= ~RF_THIRD_PERSON;
	}

	if ( (cg_shadows.integer == 3 || cg_shadows.integer == 4) && shadow )
	{
		renderfx |= RF_SHADOW_PLANE;
	}

	renderfx |= RF_LIGHTING_ORIGIN;								// use the same origin for all
#ifdef __XRAY__
	renderfx |= RF_CAMERA_PLAYER;
#endif

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

			//CG_Printf( "%f\n", hilightIntensity );
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
	VectorCopy( lightorigin, body.lightingOrigin );
	body.shadowPlane = shadowPlane;
	body.renderfx = renderfx;
	VectorCopy( body.origin, body.oldorigin );					// don't positionally lerp at all
	cent->pe.bodyRefEnt = body;

	// if the model failed, allow the default nullmodel to be displayed
	// Gordon: whoever wrote that comment sucks
	if ( !body.hModel )
	{
		return;
	}

	// (SA) only need to set this once...
	VectorCopy( lightorigin, acc.lightingOrigin );
#ifdef __XRAY__
	if ( CG_XRayActive() )
	{
		body.customShader = cgs.media.xrayViewPlayer;
	}
#endif
	CG_AddRefEntityWithPowerups( &body, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
#ifdef __FIXED_SHADOWS3__
	if ( cg_shadows.integer == 3 || cg_shadows.integer == 4 )
	{
		body.customSkin = 0;
		body.customShader = cgs.media.shadowShader;
		CG_AddRefEntityWithPowerups( &body, cent->currentState.powerups, ci->team, &cent->currentState,
									 cent->fireRiseDir );
	}
#endif
	if ( !(head.hModel = character->hudhead) )
	{
		return;
	}

	head.customSkin = character->hudheadskin;
	VectorCopy( lightorigin, head.lightingOrigin );
	CG_PositionRotatedEntityOnTag( &head, &body, "tag_head" );
	head.shadowPlane = shadowPlane;
	head.renderfx = renderfx;
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

#ifdef __XRAY__
	if ( CG_XRayActive() )
	{
		head.customShader = cgs.media.xrayViewPlayer;
	}
#endif

	// set blinking flag
	CG_AddRefEntityWithPowerups( &head, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
	cent->pe.headRefEnt = head;

	// add the
#ifndef __FIXED_SHADOWS3__
	shadow = CG_NPCShadow( cent, &shadowPlane );
#endif

	// set the shadowplane for accessories
	acc.shadowPlane = shadowPlane;
	CG_NPCBreathPuffs( cent, &head );

	//
	// add the gun / barrel / flash
	//
	if ( !(cent->currentState.eFlags & EF_DEAD) /*&& !usingBinocs*/ )
	{															// NERVE - SMF
		CG_AddPlayerWeapon( &body, NULL, cent, qfalse );
	}

	//
	// add binoculars (if it's not the player)
	//

	/*	if( usingBinocs ) {			// NERVE - SMF
		acc.hModel = cgs.media.thirdPersonBinocModel;
		CG_PositionEntityOnTag( &acc, &body, "tag_weapon", 0, NULL );
		CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState, cent->fireRiseDir );
	}*/

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
					CG_PositionEntityOnTag( /*ET_NPC,*/ &acc, &body, "tag_bright", 0, NULL );
					break;
				case ACC_BELT_RIGHT:
					CG_PositionEntityOnTag( /*ET_NPC,*/ &acc, &body, "tag_bleft", 0, NULL );
					break;
				case ACC_BELT:
					CG_PositionEntityOnTag( /*ET_NPC,*/ &acc, &body, "tag_ubelt", 0, NULL );
					break;
				case ACC_BACK:
					CG_PositionEntityOnTag( /*ET_NPC,*/ &acc, &body, "tag_back", 0, NULL );
					break;
				case ACC_HAT:									//hat
				case ACC_RANK:
					if ( cent->currentState.eFlags & EF_HEADSHOT )
					{
						continue;
					}

				case ACC_MOUTH2:								// hat2
				case ACC_MOUTH3:								// hat3
					if ( i == ACC_RANK )
					{
						if ( ci->rank == 0 )
						{
							continue;
						}

						acc.customShader = rankicons[ci->rank][1].shader;
					}

					CG_PositionEntityOnTag( /*ET_NPC,*/ &acc, &head, "tag_mouth", 0, NULL );
					break;

				// weapon and weapon2
				// these are used by characters who have permanent weapons attached to their character in the skin
				case ACC_WEAPON:								// weap
					CG_PositionEntityOnTag( /*ET_NPC,*/ &acc, &body, "tag_weapon", 0, NULL );
					break;
				case ACC_WEAPON2:								// weap2
					CG_PositionEntityOnTag( /*ET_NPC,*/ &acc, &body, "tag_weapon2", 0, NULL );
					break;
				default:
					continue;
			}

#ifdef __XRAY__
			if ( CG_XRayActive() )
			{
				acc.customShader = cgs.media.xrayMap;
			}
#endif
			CG_AddRefEntityWithPowerups( &acc, cent->currentState.powerups, ci->team, &cent->currentState,
										 cent->fireRiseDir );
		}
	}

	//	CG_Printf("NPC Drawn!!!\n");
}
#endif //__NPC__
