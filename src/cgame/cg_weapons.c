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
 * name:		cg_weapons.c
 *
 * desc:		events and effects dealing with weapons
 *
*/
#include "cg_local.h"
#include "../game/bg_classes.h"

//----(SA)	
// forward decs
static int		getAltWeapon ( int weapnum );
int				getEquivWeapon ( int weapnum );
int				CG_WeaponIndex ( int weapnum, int *bank, int *cycle );
static qboolean CG_WeaponHasAmmo ( int i );
char			cg_fxflags;
extern int		weapBanksMultiPlayer[MAX_WEAP_BANKS_MP][MAX_WEAPS_IN_BANK_MP];		// JPW NERVE moved to bg_misc.c so I can get a droplist
extern int		weapBanksMultiPlayer_alt[MAX_WEAP_BANKS_MP][MAX_WEAPS_IN_BANK_MP];	// JPW NERVE moved to bg_misc.c so I can get a droplist

// jpw
//----(SA)	end
extern void		CG_ImpactMarkOld
				(
					qhandle_t		markShader,
					const vec3_t	origin,
					const vec3_t	dir,
					float			orientation,
					float			red,
					float			green,
					float			blue,
					float			alpha,
					qboolean		alphaFade,
					float			radius,
					qboolean		temporary,
					int				duration
				);


/*
void CG_R_AddProjectedDlight( vec3_t origin, float radius, float r, float g, float b, float a, qhandle_t Shader ) {
	vec3_t			end;
	trace_t			trace;
	int				i;

//     _____    ____
//    |\ 2__\  |    |\
//    |3|    | |  6 | |
//    | | 5  | |    |4|
//	   \|____| |____| |
//				\_1__\|


	if (!radius || !Shader || ! cg_dynamicLight.integer )
		return;

	for ( i=0; i < 6; i++){

		VectorCopy( origin, end );
		switch ( i ){
		case 0:
			end[2] -= radius;;
			break;
		case 1:
			end[2] += radius;;
			break;
		case 2:
			end[0] -= radius;;
			break;
		case 3:
			end[0] += radius;;
			break;
		case 4:
			end[1] -= radius;;
			break;
		case 5:
			end[1] += radius;;
			break;
		default:
			break;
		}


		trap_CM_BoxTrace( &trace, origin, end, NULL, NULL, 0, MASK_PLAYERSOLID );

		if ( trace.fraction == 1.0 )
			continue;
	
		CG_ImpactMarkOld( Shader, trace.endpos, trace.plane.normal, 0, r,g,b,a, qfalse, 80, qtrue, -1 );
	}

}
*/
void
CG_R_AddProjectedDlight ( vec3_t origin, float radius, float r, float g, float b, float a, qhandle_t Shader )
{
	vec3_t	end;
	trace_t trace;
	int		i;
	float	dist;
	float	scale;
	vec4_t	projection;
	vec4_t	color;
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	//     _____    ____
	//    |\ 2__\  |    |\
	//    |3|    | |  6 | |
	//    | | 5  | |    |4|
	//	   \|____| |____| |
	//				\_1__\|
	if ( !radius || !Shader || !cg_dynamicLight.integer )
	{
		return;
	}

	for ( i = 0; i < 6; i++ )
	{
		VectorCopy( origin, end );
		switch ( i )
		{
			case 0:
				end[2] -= radius * 2;
				break;
			case 1:
				end[2] += radius * 2;
				break;
			case 2:
				end[0] -= radius * 2;
				break;
			case 3:
				end[0] += radius * 2;
				break;
			case 4:
				end[1] -= radius * 2;
				break;
			case 5:
				end[1] += radius * 2;
				break;
			default:
				break;
		}

		trap_CM_BoxTrace( &trace, origin, end, NULL, NULL, 0, MASK_PLAYERSOLID );
		if ( trace.startsolid )
		{
			continue;
		}

		if ( trace.fraction == 1.0 )
		{
			continue;
		}

		// check if projection origin matches plane, do not add down/up projection to very step planes, use sides instead
		switch ( i )
		{
			case 0:

				//down
				if ( trace.plane.normal[2] < 0.7 )
				{
					continue;
				}
				break;
			case 1:

				//up
				if ( trace.plane.normal[2] < 0.7 )
				{
					continue;
				}
				break;
			case 2:

				//sides
				if ( trace.plane.normal[2] > 0.7 )
				{
					continue;
				}
				break;
			case 3:

				//sides
				if ( trace.plane.normal[2] > 0.7 )
				{
					continue;
				}
				break;
			case 4:

				//sides
				if ( trace.plane.normal[2] > 0.7 )
				{
					continue;
				}
				break;
			case 5:

				//sides
				if ( trace.plane.normal[2] > 0.7 )
				{
					continue;
				}
				break;
			default:
				break;
		}

		dist = VectorDistance( origin, trace.endpos );
		scale = ( radius / dist ) * 0.85;
		if ( scale > 1.3 )
		{
			scale = 1.3;
		}

		if ( scale < 0.2 )
		{
			scale = 0.2;
		}

		// fixme , rewrite it to don't do so artifacts
		//	CG_ImpactMarkOld( Shader, trace.endpos, trace.plane.normal, 0, r,g,b,a, qfalse, radius*scale, qtrue, -1 );
		VectorSet( projection, 0, 0, -1 );	// @fixme was is das?!
		projection[3] = radius;
		trap_R_ProjectDecal( Shader, 1, (vec3_t *) origin, projection, color, -1, -1 >> 4 );
	}

	// add junior light for models
	//	trap_R_AddLightToScene( origin, radius*2, a*2, r, g, b, 0, REF_JUNIOR_DLIGHT );
}

/*
void AddDlights( centity_t *cent, float r, float g, float b, float a, qhandle_t Shader ) {
	vec3_t			end;
	trace_t			trace;

	if (! cg_dynamicLight.integer )
		return;

	VectorCopy( cent->lerpOrigin, end );
	end[2] -= 64;

	trap_CM_BoxTrace( &trace, cent->lerpOrigin, end, NULL, NULL, 0, MASK_PLAYERSOLID );


	// no dlights when dead
	if( cent->currentState.eFlags & EF_DEAD ) {
		return;
	}
				
	CG_ImpactMarkOld( Shader, trace.endpos, trace.plane.normal, 0, r,g,b,a, qfalse, 80, qtrue, -1 );
}
*/


/*
==============
CG_SetSniperZoom
==============
*/
void
CG_SetSniperZoom ( void )
{
	int zoomindex;
	if ( !cg.zoomedSniper )
	{
		return;
	}

	// Keep binocs swaying
	if ( !(cg.snap->ps.eFlags & EF_ZOOMING) )
	{
		cg.zoomval = 0;
	}

	cg.zoomedScope = 0;
	switch ( cg.snap->ps.weapon )
	{
		default:
			return;

		// no sniper zoom, get out.
		case WP_FG42:
			cg.zoomval = cg_zoomDefaultSniper.value;	// JPW NERVE changed from defaultFG per atvi req
			cg.zoomedScope = 1;							// TOD-O: add to zoomTable
			zoomindex = ZOOM_SNIPER;					// JPW NERVE was FG42SCOPE
			break;
		case WP_GARAND:
			cg.zoomval = cg_zoomDefaultSniper.value;
			cg.zoomedScope = 900;						// TOD-O: add to zoomTable
			zoomindex = ZOOM_SNIPER;
			break;
		case WP_K43:
			cg.zoomval = cg_zoomDefaultSniper.value;
			cg.zoomedScope = 900;						// TOD-O: add to zoomTable
			zoomindex = ZOOM_SNIPER;
			break;
	}

	// constrain user preferred fov to weapon limitations
	if ( cg.zoomval > zoomTable[zoomindex][ZOOM_OUT] )
	{
		cg.zoomval = zoomTable[zoomindex][ZOOM_OUT];
	}

	if ( cg.zoomval < zoomTable[zoomindex][ZOOM_IN] )
	{
		cg.zoomval = zoomTable[zoomindex][ZOOM_IN];
	}

	//	cg.zoomTime		= cg.time;
}


/*
==============
CG_StartWeaponAnim
==============
*/
static void
CG_StartWeaponAnim ( int anim )
{
	if ( cg.predictedPlayerState.pm_type >= PM_DEAD )
	{
		return;
	}

	if ( cg.pmext.weapAnimTimer > 0 )
	{
		return;
	}

	/*
	if( cg.predictedPlayerState.weapon == WP_NONE)
		return;*/
	cg.predictedPlayerState.weapAnim = ( (cg.predictedPlayerState.weapAnim & ANIM_TOGGLEBIT) ^ ANIM_TOGGLEBIT ) | anim;
}


/* */
static void
CG_ContinueWeaponAnim ( int anim )
{

	/*	if( cg.predictedPlayerState.weapon == WP_NONE )
		return;
*/
	if ( (cg.predictedPlayerState.weapAnim &~ANIM_TOGGLEBIT) == anim )
	{
		return;
	}

	if ( cg.pmext.weapAnimTimer > 0 )
	{
		return; // a high priority animation is running
	}

	CG_StartWeaponAnim( anim );
}


/*
==============
CG_MachineGunEjectBrassNew
==============
*/
void
CG_MachineGunEjectBrassNew ( centity_t *cent )
{
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			velocity, xvelocity;
	float			waterScale = 1.0f;
	vec3_t			v[3];
	if ( cg_brassTime.integer <= 0 )
	{
		return;
	}

	le = CG_AllocLocalEntity();
	re = &le->refEntity;
	velocity[0] = -50 + 25 * crandom();				// JPW NERVE
	velocity[1] = -100 + 40 * crandom();			// JPW NERVE
	velocity[2] = 200 + 50 * random();				// JPW NERVE
	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + cg_brassTime.integer + ( cg_brassTime.integer / 4 ) * random();
	le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time - ( rand() & 15 );
	AnglesToAxis( cent->lerpAngles, v );
	VectorCopy( cent->pe.ejectBrassCasingOrigin, re->origin );
	VectorCopy( re->origin, le->pos.trBase );
	if ( CG_PointContents( re->origin, -1) & (CONTENTS_WATER | CONTENTS_SLIME) )
	{												//----(SA)	modified since slime is no longer deadly
		//	if ( CG_PointContents( re->origin, -1 ) & CONTENTS_WATER ) {
		waterScale = 0.10;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );
	AxisCopy( axisDefault, re->axis );
	re->hModel = cgs.media.smallgunBrassModel;
	le->bounceFactor = 0.4 * waterScale;
	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->angles.trBase[0] = ( rand() & 31 ) + 60;	// bullets should come out horizontal not vertical JPW NERVE
	le->angles.trBase[1] = rand() & 255;			// random spin from extractor
	le->angles.trBase[2] = rand() & 31;
	le->angles.trDelta[0] = 2;
	le->angles.trDelta[1] = 1;
	le->angles.trDelta[2] = 0;
	le->leFlags = LEF_TUMBLE;
	{
		int		contents;
		vec3_t	end;
		VectorCopy( cent->lerpOrigin, end );
		end[2] -= 24;
		contents = CG_PointContents( end, 0 );
		if ( contents & (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA) )
		{
			le->leBounceSoundType = LEBS_NONE;
		}
		else
		{
			le->leBounceSoundType = LEBS_BRASS;
		}
	}

	le->leMarkType = LEMT_NONE;
}


/*
==============
CG_MachineGunEjectBrassFalckonet // copy of CG_MachineGunEjectBrassNew , however with ammo brass choice and for every cent
==============
*/
void
CG_MachineGunEjectBrassFalckonet ( centity_t *cent )
{
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			velocity, xvelocity;
	float			waterScale = 1.0f;
	vec3_t			v[3];
	if ( cg_brassTime.integer <= 0 )
	{
		return;
	}

	le = CG_AllocLocalEntity();
	re = &le->refEntity;
	velocity[0] = -50 + 25 * crandom();				// JPW NERVE
	velocity[1] = -100 + 40 * crandom();			// JPW NERVE
	velocity[2] = 200 + 50 * random();				// JPW NERVE
	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + cg_brassTime.integer + ( cg_brassTime.integer / 4 ) * random();
	le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time - ( rand() & 15 );
	AnglesToAxis( cent->lerpAngles, v );
	VectorCopy( cent->pe.ejectBrassCasingOrigin, re->origin );
	VectorCopy( re->origin, le->pos.trBase );
	if ( CG_PointContents( re->origin, -1) & (CONTENTS_WATER | CONTENTS_SLIME) )
	{												//----(SA)	modified since slime is no longer deadly
		waterScale = 0.10;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );
	AxisCopy( axisDefault, re->axis );
	if ( cg_weapons[cent->currentState.weapon].brassModel )
	{
		re->hModel = cg_weapons[cent->currentState.weapon].brassModel;
	}
	else
	{
		switch ( cent->currentState.weapon )
		{
			case WP_LUGER:
			case WP_COLT:
			case WP_SILENCER:
			case WP_SILENCED_COLT:
				re->hModel = cgs.media.smallgunBrassModel;
				break;
			case WP_30CAL:
			case WP_30CAL_SET:
				re->hModel = cgs.media.machinegunBrassModel;
				break;
			case WP_MOBILE_MG42:
			case WP_MOBILE_MG42_SET:
				re->hModel = cgs.media.machinegunBrassModel;
				break;
			case WP_G43:
			case WP_M1GARAND:
			case WP_KAR98:
			case WP_CARBINE:
			case WP_K43:
			case WP_MP40:
			case WP_THOMPSON:
			case WP_STEN:
	#ifdef NWEAPS
			case WP_MP44:
			case WP_BAR:
	#endif
			default:
				re->hModel = cgs.media.machinegunBrassModel;
		}
	}

	le->bounceFactor = 0.4 * waterScale;
	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->angles.trBase[0] = ( rand() & 31 ) + 60;	// bullets should come out horizontal not vertical JPW NERVE
	le->angles.trBase[1] = rand() & 255;			// random spin from extractor
	le->angles.trBase[2] = rand() & 31;
	le->angles.trDelta[0] = 2;
	le->angles.trDelta[1] = 1;
	le->angles.trDelta[2] = 0;
	le->leFlags = LEF_TUMBLE;
	{
		int		contents;
		vec3_t	end;
		VectorCopy( cent->lerpOrigin, end );
		end[2] -= 24;
		contents = CG_PointContents( end, 0 );
		if ( contents & (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA) )
		{
			le->leBounceSoundType = LEBS_NONE;
		}
		else
		{
			le->leBounceSoundType = LEBS_BRASS;
		}
	}

	le->leMarkType = LEMT_NONE;
}


/*
==========================
CG_MachineGunEjectBrass
==========================
*/
void
CG_MachineGunEjectBrass ( centity_t *cent )
{
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			velocity, xvelocity;
	vec3_t			offset, xoffset;
	float			waterScale = 1.0f;
	vec3_t			v[3];
	if ( cg_brassTime.integer <= 0 )
	{
		return;
	}

	/*	if (!(cg.snap->ps.persistant[PERS_HWEAPON_USE]) && (cent->currentState.clientNum == cg.snap->ps.clientNum) && (!(cent->currentState.eFlags & EF_MG42_ACTIVE && (!cent->currentState.eFlags & EF_BROWNING_ACTIVE) && (!cent->currentState.eFlags & EF_AAGUN_ACTIVE)))) {
		CG_MachineGunEjectBrassNew (cent);
		return;
	}*/
	if
	(
		!(cent->currentState.eFlags & EF_MG42_ACTIVE) &&
		!(cent->currentState.eFlags & EF_BROWNING_ACTIVE) &&
		!(cent->currentState.eFlags & EF_AAGUN_ACTIVE) &&
		!(cent->currentState.eFlags & EF_MOUNTEDTANK)
	)
	{
		CG_MachineGunEjectBrassFalckonet( cent );
		return;
	}

	le = CG_AllocLocalEntity();
	re = &le->refEntity;
	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + cg_brassTime.integer + ( cg_brassTime.integer / 4 ) * random();
	le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time - ( rand() & 15 );
	AnglesToAxis( cent->lerpAngles, v );

	// JPW NERVE new brass handling behavior because the SP stuff just doesn't cut it for MP
	if ( cent->currentState.eFlags & EF_MG42_ACTIVE || cent->currentState.eFlags & EF_BROWNING_ACTIVE )
	{
		offset[0] = 25;
		offset[1] = -4;
		offset[2] = 28;
		velocity[0] = -20 + 40 * crandom();		// JPW NERVE -- more reasonable brass ballistics for a machinegun
		velocity[1] = -150 + 40 * crandom();	// JPW NERVE
		velocity[2] = 100 + 50 * crandom();		// JPW NERVE
		re->hModel = cgs.media.machinegunBrassModel;
		le->angles.trBase[0] = 90;				//rand()&31; // JPW NERVE belt-fed rounds should come out horizontal
		le->angles.trBase[1] = rand() & 255;
		le->angles.trBase[2] = rand() & 31;
		le->angles.trDelta[0] = 2;
		le->angles.trDelta[1] = 1;
		le->angles.trDelta[2] = 0;
	}
	else
	{
		return;									// jaquboss , other weapons should get here , and AAGUN shouldnt have this..
	}

	/*		switch (cent->currentState.weapon) {
		case WP_LUGER:
		case WP_COLT:
		case WP_SILENCER:
		case WP_SILENCED_COLT:
			offset[0] = 24;
			offset[1] = -4;
			offset[2] = 36;

	re->hModel = cgs.media.smallgunBrassModel;

			break;
		case WP_30CAL:
		case WP_30CAL_SET:
			offset[0] = 12; 
			offset[1] = -4;
			offset[2] = 24;	

	re->hModel = cgs.media.machinegunBrassModel;

			break;
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
			offset[0] = 12; 
			offset[1] = -4;
			offset[2] = 24;	

	re->hModel = cgs.media.machinegunBrassModel;

			break;
		case WP_G43:
		case WP_M1GARAND:
		case WP_KAR98:
		case WP_CARBINE:
		case WP_K43:
		case WP_MP40:
		case WP_THOMPSON:
		case WP_STEN:
#ifdef NWEAPS
		case WP_MP44:
		case WP_BAR:
#endif
		default:

	re->hModel = cgs.media.machinegunBrassModel;

			offset[0] = 16;
			offset[1] = -4;
			offset[2] = 24;
			break;
		}
		velocity[0] = -50+25*crandom();
		velocity[1] = -100+40*crandom(); 
		velocity[2] = 200+50*random();
		le->angles.trBase[0] = (rand()&15) + 82; // bullets should come out horizontal not vertical JPW NERVE
		le->angles.trBase[1] = rand()&255; // random spin from extractor
		le->angles.trBase[2] = rand()&31;
		le->angles.trDelta[0] = 2;
		le->angles.trDelta[1] = 1;
		le->angles.trDelta[2] = 0;
	}
// jpw
*/
	xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
	xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
	xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];
	VectorAdd( cent->lerpOrigin, xoffset, re->origin );
	VectorCopy( re->origin, le->pos.trBase );
	if ( CG_PointContents( re->origin, -1) & (CONTENTS_WATER | CONTENTS_SLIME) )
	{											//----(SA)	modified since slime is no longer deadly
		waterScale = 0.10;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );
	AxisCopy( axisDefault, re->axis );
	le->bounceFactor = 0.4 * waterScale;
	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->leFlags = LEF_TUMBLE;
	{
		int		contents;
		vec3_t	end;
		VectorCopy( cent->lerpOrigin, end );
		end[2] -= 24;
		contents = CG_PointContents( end, 0 );
		if ( contents & (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA) )
		{
			le->leBounceSoundType = LEBS_NONE;
		}
		else
		{
			le->leBounceSoundType = LEBS_BRASS;
		}
	}

	le->leMarkType = LEMT_NONE;
}

//----(SA)	added


/*
==============
CG_PanzerFaustEjectBrass
	toss the 'used' panzerfaust casing (unit is one-shot, disposable)
==============
*/
static void
CG_PanzerFaustEjectBrass ( centity_t *cent )
{
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			velocity, xvelocity;
	vec3_t			offset, xoffset;
	float			waterScale = 1.0f;
	vec3_t			v[3];
	le = CG_AllocLocalEntity();
	re = &le->refEntity;
	velocity[0] = 16;
	velocity[1] = -200;
	velocity[2] = 0;
	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + ( cg_brassTime.integer * 8 ) + ( cg_brassTime.integer * random() );
	le->pos.trType = TR_GRAVITY;
	le->pos.trTime = cg.time - ( rand() & 15 );
	AnglesToAxis( cent->lerpAngles, v );
	offset[0] = -24;											// forward
	offset[1] = -4;												// left
	offset[2] = 24;												// up
	xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
	xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
	xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];
	VectorAdd( cent->lerpOrigin, xoffset, re->origin );
	VectorCopy( re->origin, le->pos.trBase );
	if ( CG_PointContents( re->origin, -1) & (CONTENTS_WATER | CONTENTS_SLIME) )
	{
		waterScale = 0.10;
	}

	xvelocity[0] = velocity[0] * v[0][0] + velocity[1] * v[1][0] + velocity[2] * v[2][0];
	xvelocity[1] = velocity[0] * v[0][1] + velocity[1] * v[1][1] + velocity[2] * v[2][1];
	xvelocity[2] = velocity[0] * v[0][2] + velocity[1] * v[1][2] + velocity[2] * v[2][2];
	VectorScale( xvelocity, waterScale, le->pos.trDelta );
	AxisCopy( axisDefault, re->axis );

	// (SA) make it bigger
	//	le->sizeScale = 3.0f;
	// Jaquboss , pretty funny but HELL NO!
	le->sizeScale = 2.0f;
	if ( !&cg_weapons[cent->currentState.weapon].brassModel )
	{
		re->hModel = cgs.media.panzerfaustBrassModel;
		le->sizeScale = 3.0f;									// Jaquboss , this was used by original
	}
	else
	{
		re->hModel = cg_weapons[cent->currentState.weapon].brassModel;
	}

	le->bounceFactor = 0.4 * waterScale;
	le->angles.trType = TR_LINEAR;
	le->angles.trTime = cg.time;
	le->angles.trBase[0] = 0;
	le->angles.trBase[1] = cent->currentState.apos.trBase[1];	// rotate to match the player
	le->angles.trBase[2] = 0;
	le->angles.trDelta[0] = 0;
	le->angles.trDelta[1] = 0;
	le->angles.trDelta[2] = 0;
	le->leFlags = LEF_TUMBLE | LEF_SMOKING;						// (SA) probably doesn't need to be 'tumble' since it doesn't really rotate much when flying
	le->leBounceSoundType = LEBS_NONE;
	le->leMarkType = LEMT_NONE;
}

// JPW NERVE -- compute random wind vector for smoke puff


/*
==========================
CG_GetWindVector
==========================
*/
void
CG_GetWindVector ( vec3_t dir )
{
	dir[0] = random() * 0.25;
	dir[1] = cgs.smokeWindDir;	// simulate a little wind so it looks natural
	dir[2] = random();			// one direction (so smoke goes side-like)
	VectorNormalize( dir );
}

// jpw
// JPW NERVE -- LT pyro for marking air strikes
// jpw


/*
==========================
CG_PyroSmokeTrail
==========================
*/
void
CG_PyroSmokeTrail ( centity_t *ent, const weaponInfo_t *wi )
{
	int				step;
	vec3_t			origin, lastPos, dir;
	int				contents;
	int				lastContents, startTime;
	entityState_t	*es;
	int				t;
	float			rnd;
	localEntity_t	*le;
	team_t			team;

	/*if (ent->currentState.weapon == WP_SMOKE_MARKER || ent->currentState.weapon == WP_SMOKE_MARKER){
		vec3_t	check, check2;
		trace_t	tr;

		VectorCopy( ent->lerpOrigin ,check );
		check[2] -= 8.f;
		VectorCopy( ent->lerpOrigin , check2 );
		check2[2] += 16.f;

	CG_Trace( &tr, check, NULL, NULL, check2, -1, CONTENTS_BODY|CONTENTS_CORPSE );

	if (tr.fraction < 1 ){
		return;
	} 

	}*/
	if ( ent->currentState.weapon == WP_LANDMINE )
	{
		if ( ent->currentState.teamNum < 8 )
		{
			ent->miscTime = 0;
			return;
		}
		else if ( ent->currentState.teamNum < 12 )
		{
			if ( !ent->miscTime )
			{
				ent->trailTime = cg.time;
				ent->miscTime = cg.time;

				// Arnout: play the armed sound - weird place to do it but saves us sending an event
				trap_S_StartSound( NULL, ent->currentState.number, CHAN_WEAPON, cgs.media.minePrimedSound );
			}
		}

		if ( cg.time - ent->miscTime > 1000 )
		{
			return;
		}

		if ( ent->currentState.otherEntityNum2 )
		{
			team = TEAM_AXIS;
		}
		else
		{
			team = TEAM_ALLIES;
		}
	}
	else
	{
		team = ent->currentState.teamNum;
	}

	step = 30;
	es = &ent->currentState;
	startTime = ent->trailTime;
	t = step * ( (startTime + step) / step );
	BG_EvaluateTrajectory( &es->pos, cg.time, origin, qfalse, es->effect2Time );
	contents = CG_PointContents( origin, -1 );
	BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos, qfalse, es->effect2Time );
	lastContents = CG_PointContents( lastPos, -1 );
	ent->trailTime = cg.time;

	/* smoke pyro works fine in water (well, it's dye in real life, might wanna change this in-game)
	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) )
		return;
*/

	// drop fire trail sprites
	for ( ; t <= ent->trailTime; t += step )
	{
		BG_EvaluateTrajectory( &es->pos, t, lastPos, qfalse, es->effect2Time );
		rnd = random();

		//VectorCopy (ent->lerpOrigin, lastPos);
		if ( ent->currentState.density )																// corkscrew effect
		{
			vec3_t	right;
			vec3_t	angles;
			VectorCopy( ent->currentState.apos.trBase, angles );
			angles[ROLL] += cg.time % 360;
			AngleVectors( angles, NULL, right, NULL );
			VectorMA( lastPos, ent->currentState.density, right, lastPos );
		}

		dir[0] = crandom() * 5;																			// compute offset from flare base
		dir[1] = crandom() * 5;
		dir[2] = 0;
		VectorAdd( lastPos, dir, origin );																// store in origin
		rnd = random();
		CG_GetWindVector( dir );
		if ( ent->currentState.weapon == WP_LANDMINE )
		{
			VectorScale( dir, 45, dir );
		}
		else
		{
			VectorScale( dir, 65, dir );
		}

		if ( ent->currentState.weapon != WP_PANZERFAUST && ent->currentState.weapon != WP_PANZERFAUST )
		{
			if ( team == TEAM_ALLIES )
			{																							// allied team, generate blue smoke
				le = CG_SmokePuff( origin, dir, 25 + rnd * 110,											// width
								  rnd * 0.5 + 0.5, rnd * 0.5 + 0.5, 1, 0.5, 4800 + (rand() % 2800),		// duration was 2800+
								   t, 0, 0, cgs.media.smokePuffShader );
			}
			else
			{
				le = CG_SmokePuff( origin, dir, 25 + rnd * 110,											// width
								  1.0, rnd * 0.5 + 0.5, rnd * 0.5 + 0.5, 0.5, 4800 + (rand() % 2800),	// duration was 2800+
								   t, 0, 0, cgs.media.smokePuffShader );
			}
		}
		else
		{
			le = CG_SmokePuff( origin, dir, 35 + rnd * 110, rnd * 0.5 + 0.5, rnd * 0.5 + 0.5, rnd * 0.5 + 0.5, rnd * 0.5,
							   9800 + (rand() % 2800), t, 0, 0, cgs.media.panzerfaustsmokeParticleShader );
		}

		//			CG_ParticleExplosion( "expblue", lastPos, vec3_origin, 100 + (int)(rnd*400), 4, 4 );	// fire "flare"
		// use the optimized local entity add
		//		le->leType = LE_SCALE_FADE;
		/* this one works
		if (rand()%4)
			CG_ParticleExplosion( "blacksmokeanim", origin, dir, 2800+(int)(random()*1500), 15, 45+(int)(rnd*90) );	// smoke blacksmokeanim
		else
			CG_ParticleExplosion( "expblue", lastPos, vec3_origin, 100 + (int)(rnd*400), 4, 4 );	// fire "flare"
*/
	}
}

// jpw
// Ridah, new trail effects


/*
==========================
CG_RocketTrail
==========================
*/
void
CG_RocketTrail ( centity_t *ent, const weaponInfo_t *wi )
{
	int				step;
	vec3_t			origin, lastPos, dir;
	int				contents;
	int				lastContents, startTime;
	entityState_t	*es;
	int				t;
	float			rnd;
	localEntity_t	*le;
	team_t			team;
	if ( ent->currentState.weapon == WP_LANDMINE )
	{
		if ( ent->currentState.teamNum < 8 )
		{
			ent->miscTime = 0;
			return;
		}
		else if ( ent->currentState.teamNum < 12 )
		{
			if ( !ent->miscTime )
			{
				ent->trailTime = cg.time;
				ent->miscTime = cg.time;

				// Arnout: play the armed sound - weird place to do it but saves us sending an event
				trap_S_StartSound( NULL, ent->currentState.number, CHAN_WEAPON, cgs.media.minePrimedSound );
			}
		}

		if ( cg.time - ent->miscTime > 1000 )
		{
			return;
		}

		if ( ent->currentState.otherEntityNum2 )
		{
			team = TEAM_AXIS;
		}
		else
		{
			team = TEAM_ALLIES;
		}
	}
	else
	{
		team = ent->currentState.teamNum;
	}

	step = 30;
	es = &ent->currentState;
	startTime = ent->trailTime;
	t = step * ( (startTime + step) / step );
	BG_EvaluateTrajectory( &es->pos, cg.time, origin, qfalse, es->effect2Time );
	contents = CG_PointContents( origin, -1 );
	BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos, qfalse, es->effect2Time );
	lastContents = CG_PointContents( lastPos, -1 );
	ent->trailTime = cg.time;

	/* smoke pyro works fine in water (well, it's dye in real life, might wanna change this in-game)
	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) )
		return;
*/

	// drop fire trail sprites
	for ( ; t <= ent->trailTime; t += step )
	{
		BG_EvaluateTrajectory( &es->pos, t, lastPos, qfalse, es->effect2Time );
		rnd = random();

		//VectorCopy (ent->lerpOrigin, lastPos);
		if ( ent->currentState.density )	// corkscrew effect
		{
			vec3_t	right;
			vec3_t	angles;
			VectorCopy( ent->currentState.apos.trBase, angles );
			angles[ROLL] += cg.time % 360;
			AngleVectors( angles, NULL, right, NULL );
			VectorMA( lastPos, ent->currentState.density, right, lastPos );
		}

		dir[0] = crandom() * 5;				// compute offset from flare base
		dir[1] = crandom() * 5;
		dir[2] = 0;
		VectorAdd( lastPos, dir, origin );	// store in origin
		rnd = random();
		CG_GetWindVector( dir );
		VectorScale( dir, 65, dir );
		le = CG_SmokePuff( origin, dir, 45 + rnd * 120, rnd * 0.5 + 0.5, rnd * 0.5 + 0.5, rnd * 0.5 + 0.5, rnd * 1.0,
						   9800 + (rand() % 2800), t, 0, 0, cgs.media.panzerfaustsmokeParticleShader );
	}
}

// JPW NERVE


/*
==========================
CG_DynamiteTrail
==========================
*/
static void
CG_DynamiteTrail ( centity_t *ent, const weaponInfo_t *wi )
{
	vec3_t	origin;
	float	mult;
	BG_EvaluateTrajectory( &ent->currentState.pos, cg.time, origin, qfalse, ent->currentState.effect2Time );
	if ( ent->currentState.teamNum < 4 )
	{
		mult = 0.004f * ( cg.time - ent->currentState.effect1Time ) / 30000.0f;
		trap_R_AddLightToScene( origin, 320, fabs( sin( (cg.time - ent->currentState.effect1Time) * mult)), 1.0, 0.0,
								0.0, 0, REF_FORCE_DLIGHT );
	}
	else
	{
		mult = 1 - ( (cg.time - ent->trailTime) / 15500.0f );
		trap_R_AddLightToScene( origin, 320, mult, 1.0, 1.0, 0, 0, REF_FORCE_DLIGHT );
	}
}

// jpw
// Ridah


/*
==========================
CG_GrenadeTrail
==========================
*/
static void
CG_GrenadeTrail ( centity_t *ent, const weaponInfo_t *wi )
{
	int				step;
	vec3_t			origin, lastPos;
	int				contents;
	int				lastContents, startTime;
	entityState_t	*es;
	int				t;
	step = 15;	// nice and smooth curves
	es = &ent->currentState;
	startTime = ent->trailTime;
	t = step * ( (startTime + step) / step );
	BG_EvaluateTrajectory( &es->pos, cg.time, origin, qfalse, es->effect2Time );
	contents = CG_PointContents( origin, -1 );

	// if object (e.g. grenade) is stationary, don't toss up smoke
	if ( es->pos.trType == TR_STATIONARY )
	{
		ent->trailTime = cg.time;
		return;
	}

	BG_EvaluateTrajectory( &es->pos, ent->trailTime, lastPos, qfalse, es->effect2Time );
	lastContents = CG_PointContents( lastPos, -1 );
	ent->trailTime = cg.time;
	if ( contents & (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA) )
	{
		if ( contents & lastContents & CONTENTS_WATER )
		{
			CG_BubbleTrail( lastPos, origin, 2, 8 );
		}

		return;
	}

	//----(SA)	trying this back on for DM
	// spawn smoke junctions
	for ( ; t <= ent->trailTime; t += step )
	{
		BG_EvaluateTrajectory( &es->pos, t, origin, qfalse, es->effect2Time );
		ent->headJuncIndex = CG_AddSmokeJunc( ent->headJuncIndex, ent, cgs.media.smokeTrailShader, origin,
											  //												1500, 0.3, 10, 50 );
											 1000, 0.3, 2, 20 );
		ent->lastTrailTime = cg.time;
	}

	//----(SA)	end	
}

// done.

/*
==========================
CG_NailgunEjectBrass
==========================
*/

/*
// TTimo: defined but not used
static void CG_NailgunEjectBrass( centity_t *cent ) {
	localEntity_t	*smoke;
	vec3_t			origin;
	vec3_t			v[3];
	vec3_t			offset;
	vec3_t			xoffset;
	vec3_t			up;

	AnglesToAxis( cent->lerpAngles, v );

	offset[0] = 0;
	offset[1] = -12;
	offset[2] = 24;

	xoffset[0] = offset[0] * v[0][0] + offset[1] * v[1][0] + offset[2] * v[2][0];
	xoffset[1] = offset[0] * v[0][1] + offset[1] * v[1][1] + offset[2] * v[2][1];
	xoffset[2] = offset[0] * v[0][2] + offset[1] * v[1][2] + offset[2] * v[2][2];
	VectorAdd( cent->lerpOrigin, xoffset, origin );

	VectorSet( up, 0, 0, 64 );

	smoke = CG_SmokePuff( origin, up, 32, 1, 1, 1, 0.33f, 700, cg.time, 0, 0, cgs.media.smokePuffShader );
	// use the optimized local entity add
	smoke->leType = LE_SCALE_FADE;
}
*/


/*
==========================
CG_RailTrail
	SA: re-inserted this as a debug mechanism for bullets
==========================
*/
void
CG_RailTrail2 ( vec3_t color, vec3_t start, vec3_t end )
{
	localEntity_t	*le;
	refEntity_t		*re;
	le = CG_AllocLocalEntity();
	re = &le->refEntity;
	le->leType = LE_FADE_RGB;
	le->startTime = cg.time;
	le->endTime = cg.time + cg_railTrailTime.value;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );
	re->shaderTime = cg.time / 1000.0f;
	re->reType = RT_RAIL_CORE;
	re->customShader = cgs.media.railCoreShader;
	VectorCopy( start, re->origin );
	VectorCopy( end, re->oldorigin );

	//	// still allow different colors so we can tell AI shots from player shots, etc.

	/*	if(ci) {
		le->color[0] = ci->color[0] * 0.75;
		le->color[1] = ci->color[1] * 0.75;
		le->color[2] = ci->color[2] * 0.75;
	} else {*/
	le->color[0] = color[0];
	le->color[1] = color[1];
	le->color[2] = color[2];

	//	}
	le->color[3] = 1.0f;
	AxisClear( re->axis );
}

//void CG_RailTrailBox( clientInfo_t *ci, vec3_t start, vec3_t end) {


/*
==============
CG_RailTrail
	modified so we could draw boxes for debugging as well
==============
*/
void
CG_RailTrail ( vec3_t color, vec3_t start, vec3_t end, int type )
{		//----(SA)	added 'type'
	vec3_t	diff, v1, v2, v3, v4, v5, v6;
	vec3_t	red = { 1, 0, 0 };
	if ( !color )
	{
		color = red;
	}

	if ( !type )
	{	// just a line
		CG_RailTrail2( red, start, end );
		return;
	}

	// type '1' (box)
	VectorSubtract( start, end, diff );
	VectorCopy( start, v1 );
	VectorCopy( start, v2 );
	VectorCopy( start, v3 );
	v1[0] -= diff[0];
	v2[1] -= diff[1];
	v3[2] -= diff[2];
	CG_RailTrail2( color, start, v1 );
	CG_RailTrail2( color, start, v2 );
	CG_RailTrail2( color, start, v3 );
	VectorCopy( end, v4 );
	VectorCopy( end, v5 );
	VectorCopy( end, v6 );
	v4[0] += diff[0];
	v5[1] += diff[1];
	v6[2] += diff[2];
	CG_RailTrail2( color, end, v4 );
	CG_RailTrail2( color, end, v5 );
	CG_RailTrail2( color, end, v6 );
	CG_RailTrail2( color, v2, v6 );
	CG_RailTrail2( color, v6, v1 );
	CG_RailTrail2( color, v1, v5 );
	CG_RailTrail2( color, v2, v4 );
	CG_RailTrail2( color, v4, v3 );
	CG_RailTrail2( color, v3, v5 );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/*
======================
CG_ParseWeaponConfig
	read information for weapon animations (first/length/fps)
======================
*/
static qboolean
CG_ParseWeaponConfig ( const char *filename, weaponInfo_t *wi )
{
	char			*text_p, *prev;
	int				len;
	int				i;
	float			fps;
	char			*token;
	qboolean		newfmt = qfalse;		//----(SA)	
	char			text[20000];
	fileHandle_t	f;

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 )
	{
		return ( qfalse );
	}

	if ( len >= sizeof(text) - 1 )
	{
		CG_Printf( "File %s too long\n", filename );
		return ( qfalse );
	}

	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = text;

	// read optional parameters
	while ( 1 )
	{
		prev = text_p;						// so we can unget
		token = COM_Parse( &text_p );
		if ( !token )
		{									// get the variable
			break;
		}

		if ( !Q_stricmp( token, "newfmt") )
		{
			newfmt = qtrue;
			continue;
		}

		// if it is a number, start parsing animations
		if ( token[0] >= '0' && token[0] <= '9' )
		{
			text_p = prev;					// unget the token
			break;
		}

		Com_Printf( "unknown token in weapon cfg '%s' is %s\n", token, filename );
	}

	for ( i = 0; i < MAX_WP_ANIMATIONS; i++ )
	{
		token = COM_Parse( &text_p );		// first frame
		if ( !token )
		{
			break;
		}

		wi->weapAnimations[i].firstFrame = atoi( token );
		token = COM_Parse( &text_p );		// length
		if ( !token )
		{
			break;
		}

		wi->weapAnimations[i].numFrames = atoi( token );
		token = COM_Parse( &text_p );		// fps
		if ( !token )
		{
			break;
		}

		fps = atof( token );
		if ( fps == 0 )
		{
			fps = 1;
		}

		wi->weapAnimations[i].frameLerp = 1000 / fps;
		wi->weapAnimations[i].initialLerp = 1000 / fps;
		token = COM_Parse( &text_p );		// looping frames
		if ( !token )
		{
			break;
		}

		wi->weapAnimations[i].loopFrames = atoi( token );
		if ( wi->weapAnimations[i].loopFrames > wi->weapAnimations[i].numFrames )
		{
			wi->weapAnimations[i].loopFrames = wi->weapAnimations[i].numFrames;
		}
		else if ( wi->weapAnimations[i].loopFrames < 0 )
		{
			wi->weapAnimations[i].loopFrames = 0;
		}

		// store animation/draw bits in '.moveSpeed'
		wi->weapAnimations[i].moveSpeed = 0;
		if ( newfmt )
		{
			token = COM_Parse( &text_p );	// barrel anim bits
			if ( !token )
			{
				break;
			}

			wi->weapAnimations[i].moveSpeed = atoi( token );
			token = COM_Parse( &text_p );	// animated weapon
			if ( !token )
			{
				break;
			}

			if ( atoi( token) )
			{
				wi->weapAnimations[i].moveSpeed |= ( 1 << W_MAX_PARTS );	// set the bit one higher than can be set by the barrel bits
			}

			token = COM_Parse( &text_p );									// barrel hide bits (so objects can be flagged to not be drawn during all sequences (a reloading hand that comes in from off screen for that one animation for example)
			if ( !token )
			{
				break;
			}

			wi->weapAnimations[i].moveSpeed |= ( (atoi( token)) << 8 );		// use 2nd byte for draw bits
		}
	}

	if ( i != MAX_WP_ANIMATIONS )
	{
		CG_Printf( "Error parsing weapon animation file: %s", filename );
		return ( qfalse );
	}

	return ( qtrue );
}


/* */
static qboolean
CG_RW_ParseError ( int handle, char *format, ... )
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
static qboolean
CG_RW_ParseWeaponLinkPart ( int handle, weaponInfo_t *weaponInfo, modelViewType_t viewType )
{
	pc_token_t	token;
	char		filename[MAX_QPATH];
	int			part;
	partModel_t *partModel;
	if ( !PC_Int_Parse( handle, &part) )
	{
		return ( CG_RW_ParseError( handle, "expected part index") );
	}

	if ( part < 0 || part >= W_MAX_PARTS )
	{
		return ( CG_RW_ParseError( handle, "part index out of bounds") );
	}

	partModel = &weaponInfo->partModels[viewType][part];
	memset( partModel, 0, sizeof(*partModel) );
	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "{") )
	{
		return ( CG_RW_ParseError( handle, "expected '{'") );
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

		if ( !Q_stricmp( token.string, "tag") )
		{
			if ( !PC_String_ParseNoAlloc( handle, partModel->tagName, sizeof(partModel->tagName)) )
			{
				return ( CG_RW_ParseError( handle, "expected tag name") );
			}
		}
		else if ( !Q_stricmp( token.string, "model") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected model filename") );
			}
			else
			{
				partModel->model = trap_R_RegisterModel( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "skin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected skin filename") );
			}
			else
			{
				partModel->skin[0] = trap_R_RegisterSkin( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "axisSkin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected skin filename") );
			}
			else
			{
				partModel->skin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "alliedSkin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected skin filename") );
			}
			else
			{
				partModel->skin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}

			////////////////////////////////////////////////////////////////////////////////////
			// Reading skins for characters
		}
		else if ( !Q_stricmp( token.string, "useCharacterSkin") )
		{
			partModel->useCharacterSkin = qtrue;

			////////////////////////////////////////////////////////////////////////////////////
			// class based skins

			/*} else if( !Q_stricmp( token.string, "axisSoldierSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				partModel->soldierskin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "alliedSoldierSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				partModel->soldierskin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "axisMedicSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				partModel->medicskin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "alliedMedicSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				partModel->medicskin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "axisFieldopsSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				partModel->fieldopsskin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "alliedFieldopsSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				partModel->fieldopsskin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "axisEngineerSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				partModel->engineerskin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "alliedEngineerSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				partModel->engineerskin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "axisCovertopsSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				partModel->covertopsskin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "alliedCovertopsSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				partModel->covertopsskin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}*/

			/////////////////////////////////////////////////////////////////////////////
		}
		else
		{
			return ( CG_RW_ParseError( handle, "unknown token '%s'", token.string) );
		}
	}

	return ( qtrue );
}


/* */
static qboolean
CG_RW_ParseWeaponLink ( int handle, weaponInfo_t *weaponInfo, modelViewType_t viewType )
{
	pc_token_t	token;
	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "{") )
	{
		return ( CG_RW_ParseError( handle, "expected '{'") );
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

		if ( !Q_stricmp( token.string, "part") )
		{
			if ( !CG_RW_ParseWeaponLinkPart( handle, weaponInfo, viewType) )
			{
				return ( qfalse );
			}
		}
		else
		{
			return ( CG_RW_ParseError( handle, "unknown token '%s'", token.string) );
		}
	}

	return ( qtrue );
}


/* */
static qboolean
CG_RW_ParseViewType ( int handle, weaponInfo_t *weaponInfo, modelViewType_t viewType )
{
	pc_token_t	token;
	char		filename[MAX_QPATH];
	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "{") )
	{
		return ( CG_RW_ParseError( handle, "expected '{'") );
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

		if ( !Q_stricmp( token.string, "model") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected model filename") );
			}
			else
			{
				weaponInfo->weaponModel[viewType].model = trap_R_RegisterModel( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "skin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected skin filename") );
			}
			else
			{
				weaponInfo->weaponModel[viewType].skin[0] = trap_R_RegisterSkin( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "axisSkin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected skin filename") );
			}
			else
			{
				weaponInfo->weaponModel[viewType].skin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}

			////////////////////////////////////////////////////////////////////////////////////
			// Jaquboss , better way
			/*} else if( !Q_stricmp( token.string, "axisSoldierSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				weaponInfo->weaponModel[viewType].soldierskin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "alliedSoldierSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				weaponInfo->weaponModel[viewType].soldierskin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "axisMedicSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				weaponInfo->weaponModel[viewType].medicskin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "alliedMedicSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				weaponInfo->weaponModel[viewType].medicskin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "axisFieldopsSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				weaponInfo->weaponModel[viewType].fieldopsskin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "alliedFieldopsSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				weaponInfo->weaponModel[viewType].fieldopsskin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "axisEngineerSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				weaponInfo->weaponModel[viewType].engineerskin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}*/
		}
		else if ( !Q_stricmp( token.string, "useCharacterSkin") )
		{
			weaponInfo->weaponModel[viewType].useCharacterSkin = qtrue;

			/*
			} else if( !Q_stricmp( token.string, "alliedEngineerSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				weaponInfo->weaponModel[viewType].engineerskin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "axisCovertopsSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				weaponInfo->weaponModel[viewType].covertopsskin[TEAM_AXIS] = trap_R_RegisterSkin( filename );
			}
		}	else if( !Q_stricmp( token.string, "alliedCovertopsSkin" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected skin filename" );
			} else {
				weaponInfo->weaponModel[viewType].covertopsskin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}*/

			/////////////////////////////////////////////////////////////////////////////////////
		}
		else if ( !Q_stricmp( token.string, "alliedSkin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected skin filename") );
			}
			else
			{
				weaponInfo->weaponModel[viewType].skin[TEAM_ALLIES] = trap_R_RegisterSkin( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "flashModel") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected flashModel filename") );
			}
			else
			{
				weaponInfo->flashModel[viewType] = trap_R_RegisterModel( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "weaponLink") )
		{
			if ( !CG_RW_ParseWeaponLink( handle, weaponInfo, viewType) )
			{
				return ( qfalse );
			}
		}
		else
		{
			return ( CG_RW_ParseError( handle, "unknown token '%s'", token.string) );
		}
	}

	return ( qtrue );
}


/* */
static qboolean
CG_RW_ParseModModel ( int handle, weaponInfo_t *weaponInfo )
{
	char	filename[MAX_QPATH];
	int		mod;
	if ( !PC_Int_Parse( handle, &mod) )
	{
		return ( CG_RW_ParseError( handle, "expected mod index") );
	}

	if ( mod < 0 || mod >= 6 )
	{
		return ( CG_RW_ParseError( handle, "mod index out of bounds") );
	}

	if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
	{
		return ( CG_RW_ParseError( handle, "expected model filename") );
	}
	else
	{
		weaponInfo->modModels[mod] = trap_R_RegisterModel( filename );
		if ( !weaponInfo->modModels[mod] )
		{	// maybe it's a shader
			weaponInfo->modModels[mod] = trap_R_RegisterShader( filename );
		}
	}

	return ( qtrue );
}


/* */
static qboolean
CG_RW_ParseClient ( int handle, weaponInfo_t *weaponInfo )
{
	pc_token_t	token;
	char		filename[MAX_QPATH];
	int			i;
	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "{") )
	{
		return ( CG_RW_ParseError( handle, "expected '{'") );
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

		if ( !Q_stricmp( token.string, "standModel") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected standModel filename") );
			}
			else
			{
				weaponInfo->standModel = trap_R_RegisterModel( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "droppedAnglesHack") )
		{
			weaponInfo->droppedAnglesHack = qtrue;
		}
		else if ( !Q_stricmp( token.string, "usecharacterskin") )
		{
			weaponInfo->droppedAnglesHack = qtrue;
		}
		else if ( !Q_stricmp( token.string, "distanceHack") )
		{
			weaponInfo->distanceHack = qtrue;
		}
		else if ( !Q_stricmp( token.string, "pickupModel") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected pickupModel filename") );
			}
			else
			{
				weaponInfo->weaponModel[W_PU_MODEL].model = trap_R_RegisterModel( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "backModel") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected backModel filename") );
			}
			else
			{
				weaponInfo->backModel = trap_R_RegisterModel( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "pickupSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected pickupSound filename") );
			}
			else
			{

				//weaponInfo->pickupSound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "weaponConfig") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected weaponConfig filename") );
			}
			else
			{
				if ( !CG_ParseWeaponConfig( filename, weaponInfo) )
				{

				}
			}
		}
		else if ( !Q_stricmp( token.string, "handsModel") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected handsModel filename") );
			}
			else
			{
				weaponInfo->handsModel = trap_R_RegisterModel( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "adshandsModel") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected adshandsModel filename") );
			}
			else
			{
				weaponInfo->adshandsModel = trap_R_RegisterModel( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "flashDlightColor") )
		{
			if ( !PC_Vec_Parse( handle, &weaponInfo->flashDlightColor) )
			{
				return ( CG_RW_ParseError( handle, "expected flashDlightColor as r g b") );
			}
		}
		else if ( !Q_stricmp( token.string, "flashSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected flashSound filename") );
			}
			else
			{
				for ( i = 0; i < 4; i++ )
				{
					if ( !weaponInfo->flashSound[i] )
					{
						weaponInfo->flashSound[i] = trap_S_RegisterSound( filename, qfalse );
						break;
					}
				}

				if ( i == 4 )
				{
					CG_Printf( S_COLOR_YELLOW "WARNING: only up to 4 flashSounds supported per weapon\n" );
				}
			}
		}
		else if ( !Q_stricmp( token.string, "flashEchoSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected flashEchoSound filename") );
			}
			else
			{
				for ( i = 0; i < 4; i++ )
				{
					if ( !weaponInfo->flashEchoSound[i] )
					{
						weaponInfo->flashEchoSound[i] = trap_S_RegisterSound( filename, qfalse );
						break;
					}
				}

				if ( i == 4 )
				{
					CG_Printf( S_COLOR_YELLOW "WARNING: only up to 4 flashEchoSounds supported per weapon\n" );
				}
			}
		}
		else if ( !Q_stricmp( token.string, "lastShotSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected lastShotSound filename") );
			}
			else
			{
				for ( i = 0; i < 4; i++ )
				{
					if ( !weaponInfo->lastShotSound[i] )
					{
						weaponInfo->lastShotSound[i] = trap_S_RegisterSound( filename, qfalse );
						break;
					}
				}

				if ( i == 4 )
				{
					CG_Printf( S_COLOR_YELLOW "WARNING: only up to 4 lastShotSound supported per weapon\n" );
				}
			}
		}
		else if ( !Q_stricmp( token.string, "readySound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected readySound filename") );
			}
			else
			{
				weaponInfo->readySound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "firingSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected firingSound filename") );
			}
			else
			{
				weaponInfo->firingSound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "overheatSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected overheatSound filename") );
			}
			else
			{
				weaponInfo->overheatSound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "reloadSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected reloadSound filename") );
			}
			else
			{
				weaponInfo->reloadSound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "reloadFastSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected reloadFastSound filename") );
			}
			else
			{
				weaponInfo->reloadFastSound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "spinupSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected spinupSound filename") );
			}
			else
			{
				weaponInfo->spinupSound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "spindownSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected spindownSound filename") );
			}
			else
			{
				weaponInfo->spindownSound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "switchSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected switchSound filename") );
			}
			else
			{
				weaponInfo->switchSound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "weaponIcon") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected weaponIcon filename") );
			}
			else
			{
				weaponInfo->weaponIcon[0] = trap_R_RegisterShader( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "weaponSelectedIcon") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected weaponSelectedIcon filename") );
			}
			else
			{
				weaponInfo->weaponIcon[1] = trap_R_RegisterShader( filename );
			}

			/*} else if( !Q_stricmp( token.string, "ammoIcon" ) ) {
			if( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename) ) ) {
				return CG_RW_ParseError( handle, "expected ammoIcon filename" );
			} else {
				weaponInfo->ammoIcon = trap_R_RegisterShader( filename );
			}*/
		}
		else if ( !Q_stricmp( token.string, "missileModel") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected missileModel filename") );
			}
			else
			{
				weaponInfo->missileModel = trap_R_RegisterModel( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "missileAlliedSkin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected skin filename") );
			}
			else
			{
				weaponInfo->missileAlliedSkin = trap_R_RegisterSkin( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "missileAxisSkin") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected skin filename") );
			}
			else
			{
				weaponInfo->missileAxisSkin = trap_R_RegisterSkin( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "missileSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected missileSound filename") );
			}
			else
			{
				weaponInfo->missileSound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "missileSound") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected missileSound filename") );
			}
			else
			{
				weaponInfo->missileSound = trap_S_RegisterSound( filename, qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "brassModel") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected brassModel filename") );
			}
			else
			{
				weaponInfo->brassModel = trap_R_RegisterModel( filename );
			}
		}
		else if ( !Q_stricmp( token.string, "ADSPos") )
		{
			if ( !PC_Vec_Parse( handle, &weaponInfo->ADSPos) )
			{
				return ( CG_RW_ParseError( handle, "expected ADSPos as x y z") );
			}
		}
		else if ( !Q_stricmp( token.string, "missileTrailFunc") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected missileTrailFunc") );
			}
			else
			{
				if ( !Q_stricmp( filename, "GrenadeTrail") )
				{
					weaponInfo->missileTrailFunc = CG_GrenadeTrail;
				}
				else if ( !Q_stricmp( filename, "RocketTrail") )
				{
					weaponInfo->missileTrailFunc = CG_RocketTrail;
				}
				else if ( !Q_stricmp( filename, "PyroSmokeTrail") )
				{
					weaponInfo->missileTrailFunc = CG_PyroSmokeTrail;
				}
				else if ( !Q_stricmp( filename, "DynamiteTrail") )
				{
					weaponInfo->missileTrailFunc = CG_DynamiteTrail;
				}
			}
		}
		else if ( !Q_stricmp( token.string, "missileDlight") )
		{
			if ( !PC_Float_Parse( handle, &weaponInfo->missileDlight) )
			{
				return ( CG_RW_ParseError( handle, "expected missileDlight value") );
			}
		}
		else if ( !Q_stricmp( token.string, "missileDlightColor") )
		{
			if ( !PC_Vec_Parse( handle, &weaponInfo->missileDlightColor) )
			{
				return ( CG_RW_ParseError( handle, "expected missileDlightColor as r g b") );
			}
		}
		else if ( !Q_stricmp( token.string, "ejectBrassFunc") )
		{
			if ( !PC_String_ParseNoAlloc( handle, filename, sizeof(filename)) )
			{
				return ( CG_RW_ParseError( handle, "expected ejectBrassFunc") );
			}
			else
			{
				if ( !Q_stricmp( filename, "MachineGunEjectBrass") )
				{
					weaponInfo->ejectBrassFunc = CG_MachineGunEjectBrass;
				}
				else if ( !Q_stricmp( filename, "PanzerFaustEjectBrass") )
				{
					weaponInfo->ejectBrassFunc = CG_PanzerFaustEjectBrass;
				}
			}
		}
		else if ( !Q_stricmp( token.string, "modModel") )
		{
			if ( !CG_RW_ParseModModel( handle, weaponInfo) )
			{
				return ( qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "firstPerson") )
		{
			if ( !CG_RW_ParseViewType( handle, weaponInfo, W_FP_MODEL) )
			{
				return ( qfalse );
			}
		}
		else if ( !Q_stricmp( token.string, "thirdPerson") )
		{
			if ( !CG_RW_ParseViewType( handle, weaponInfo, W_TP_MODEL) )
			{
				return ( qfalse );
			}
		}
		else
		{
			return ( CG_RW_ParseError( handle, "unknown token '%s'", token.string) );
		}
	}

	return ( qtrue );
}


/* */
static qboolean
CG_RegisterWeaponFromWeaponFile ( const char *filename, weaponInfo_t *weaponInfo )
{
	pc_token_t	token;
	int			handle;
	handle = trap_PC_LoadSource( filename );
	if ( !handle )
	{
		return ( qfalse );
	}

	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "weaponDef") )
	{
		return ( CG_RW_ParseError( handle, "expected 'weaponDef'") );
	}

	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "{") )
	{
		return ( CG_RW_ParseError( handle, "expected '{'") );
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

		if ( !Q_stricmp( token.string, "client") )
		{
			if ( !CG_RW_ParseClient( handle, weaponInfo) )
			{
				return ( qfalse );
			}
		}
		else
		{
			return ( CG_RW_ParseError( handle, "unknown token '%s'", token.string) );
		}
	}

	trap_PC_FreeSource( handle );
	return ( qtrue );
}


/*
=================
CG_RegisterWeapon
=================
*/
void
CG_RegisterWeapon ( int weaponNum, qboolean force )
{
	weaponInfo_t	*weaponInfo;
	char			*filename;
	if ( weaponNum <= 0 || weaponNum >= WP_NUM_WEAPONS )
	{
		return;
	}

	weaponInfo = &cg_weapons[weaponNum];
	if ( weaponInfo->registered && !force )
	{
		return;
	}

	memset( weaponInfo, 0, sizeof(*weaponInfo) );
	weaponInfo->registered = qtrue;
	switch ( weaponNum )
	{
		case WP_KNIFE:
			filename = "knife.weap";
			break;
		case WP_LUGER:
			filename = "luger.weap";
			break;
		case WP_COLT:
			filename = "colt.weap";
			break;
		case WP_MP40:
			filename = "mp40.weap";
			break;
		case WP_THOMPSON:
			filename = "thompson.weap";
			break;
		case WP_STEN:
			filename = "sten.weap";
			break;
		case WP_GRENADE_LAUNCHER:
			filename = "grenade.weap";
			break;
		case WP_GRENADE_PINEAPPLE:
			filename = "pineapple.weap";
			break;
		case WP_PANZERFAUST:
			filename = "panzerfaust.weap";
			break;
		case WP_FLAMETHROWER:
			filename = "flamethrower.weap";
			break;
		case WP_AMMO:
			filename = "ammopack.weap";
			break;
		case WP_SMOKETRAIL:
			filename = "smoketrail.weap";
			break;
		case WP_MEDKIT:
			filename = "medpack.weap";
			break;
		case WP_PLIERS:
			filename = "pliers.weap";
			break;
		case WP_SMOKE_MARKER:
			filename = "smokemarker.weap";
			break;
		case WP_DYNAMITE:
			filename = "dynamite.weap";
			break;
		case WP_MEDIC_ADRENALINE:
			filename = "adrenaline.weap";
			break;
		case WP_MEDIC_SYRINGE:
			filename = "syringe.weap";
			break;
		case WP_BINOCULARS:
			filename = "binocs.weap";
			break;
		case WP_KAR98:
			filename = "kar98.weap";
			break;
		case WP_GPG40:
			filename = "gpg40.weap";
			break;
		case WP_CARBINE:
			filename = "m1_garand.weap";
			break;
		case WP_M7:
			filename = "m7.weap";
			break;
		case WP_GARAND:
		case WP_GARAND_SCOPE:
			filename = "m1_garand_s.weap";
			break;
		case WP_FG42:
		case WP_FG42SCOPE:
			filename = "fg42.weap";
			break;
		case WP_LANDMINE:
			filename = "landmine.weap";
			break;
		case WP_SATCHEL:
			filename = "satchel.weap";
			break;
		case WP_SATCHEL_DET:
			filename = "satchel_det.weap";
			break;
		case WP_SMOKE_BOMB:
			filename = "smokegrenade.weap";
			break;
		case WP_30CAL_SET:
		case WP_30CAL:
			filename = "30cal.weap";
			break;
		case WP_MOBILE_MG42_SET:	// do we need a seperate file for this?
		case WP_MOBILE_MG42:
			filename = "mg42.weap";
			break;
		case WP_SILENCER:
			filename = "silenced_luger.weap";
			break;
		case WP_SILENCED_COLT:
			filename = "silenced_colt.weap";
			break;
		case WP_K43:
		case WP_K43_SCOPE:
			filename = "k43.weap";
			break;
		case WP_MORTAR:
			filename = "mortar.weap";
			break;
		case WP_MORTAR_SET:
			filename = "mortar_set.weap";
			break;
		case WP_AKIMBO_LUGER:
			filename = "akimbo_luger.weap";
			break;
		case WP_AKIMBO_SILENCEDLUGER:
			filename = "akimbo_silenced_luger.weap";
			break;
		case WP_AKIMBO_COLT:
			filename = "akimbo_colt.weap";
			break;
		case WP_AKIMBO_SILENCEDCOLT:
			filename = "akimbo_silenced_colt.weap";
			break;
		case WP_MAPMORTAR:
			filename = "mapmortar.weap";
			break;

		// do we really need this?
		case WP_ARTY:
			return;

		// to shut the game up
		case WP_G43:
			filename = "g43.weap";
			break;
		case WP_M1GARAND:
			filename = "m1garand.weap";
			break;
	#ifdef NWEAPS
		case WP_MP44:
			filename = "mp44.weap";
			break;
		case WP_BAR:
			filename = "bar.weap";
			break;
	#endif
		default:
			CG_Printf( S_COLOR_RED "WARNING: trying to register weapon %i but there is no weapon file entry for it.\n",
					   weaponNum );
			return;
	}

	if ( !CG_RegisterWeaponFromWeaponFile( va( "weapons/%s", filename), weaponInfo) )
	{
		CG_Printf( S_COLOR_RED "WARNING: failed to register media for weapon %i from %s\n", weaponNum, filename );
	}
}


/*
=================
CG_RegisterItemVisuals

The server says this item is used on this level
=================
*/
void
CG_RegisterItemVisuals ( int itemNum )
{
	itemInfo_t	*itemInfo;
	gitem_t		*item;
	int			i;
	itemInfo = &cg_items[itemNum];
	if ( itemInfo->registered )
	{
		return;
	}

	item = &bg_itemlist[itemNum];
	memset( itemInfo, 0, sizeof(&itemInfo) );
	if ( item->giType == IT_WEAPON )
	{
		return;
	}

	for ( i = 0; i < MAX_ITEM_MODELS; i++ )
	{
		itemInfo->models[i] = trap_R_RegisterModel( item->world_model[i] );
	}

	if ( item->icon )
	{
		itemInfo->icons[0] = trap_R_RegisterShader( item->icon );
		if ( item->giType == IT_HOLDABLE )
		{

			// (SA) register alternate icons (since holdables can have multiple uses, they might have different icons to represent how many uses are left)
			for ( i = 1; i < MAX_ITEM_ICONS; i++ )
			{
				itemInfo->icons[i] = trap_R_RegisterShader( va( "%s%i", item->icon, i + 1) );
			}
		}
	}

	itemInfo->registered = qtrue;	//----(SA)	moved this down after the registerweapon()
}

/*
========================================================================================

VIEW WEAPON

========================================================================================
*/

//
// weapon animations
//


/*
==============
CG_GetPartFramesFromWeap
	get animation info from the parent if necessary
==============
*/
qboolean
CG_GetPartFramesFromWeap ( centity_t *cent, refEntity_t *part, refEntity_t *parent, int partid, weaponInfo_t *wi )
{
	int			i;
	int			frameoffset = 0;
	animation_t *anim;
	anim = cent->pe.weap.animation;
	if ( partid == W_MAX_PARTS )
	{
		return ( qtrue );	// primary weap model drawn for all frames right now
	}

	// check draw bit
	if ( anim->moveSpeed & (1 << (partid + 8)) )
	{						// hide bits are in high byte
		return ( qfalse );	// not drawn for current sequence
	}

	// find part's start frame for this animation sequence
	// rain - & out ANIM_TOGGLEBIT or we'll go way out of bounds
	for ( i = 0; i < (cent->pe.weap.animationNumber &~ANIM_TOGGLEBIT); i++ )
	{
		if ( wi->weapAnimations[i].moveSpeed & (1 << partid) )
		{					// this part has animation for this sequence
			frameoffset += wi->weapAnimations[i].numFrames;
		}
	}

	// now set the correct frame into the part
	if ( anim->moveSpeed & (1 << partid) )
	{
		part->backlerp = parent->backlerp;
		part->oldframe = frameoffset + ( parent->oldframe - anim->firstFrame );
		part->frame = frameoffset + ( parent->frame - anim->firstFrame );
	}

	return ( qtrue );
}


/*
===============
CG_SetWeapLerpFrameAnimation

may include ANIM_TOGGLEBIT
===============
*/
static void
CG_SetWeapLerpFrameAnimation ( weaponInfo_t *wi, lerpFrame_t *lf, int newAnimation )
{
	animation_t *anim;
	lf->animationNumber = newAnimation;
	newAnimation &= ~ANIM_TOGGLEBIT;
	if ( newAnimation < 0 || newAnimation >= MAX_WP_ANIMATIONS )
	{
		CG_Error( "Bad animation number (CG_SWLFA): %i", newAnimation );
	}

	anim = &wi->weapAnimations[newAnimation];
	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;
	if ( cg_debugAnim.integer & 2 )
	{
		CG_Printf( "Weap Anim: %d\n", newAnimation );
	}
}


/*
===============
CG_ClearWeapLerpFrame
===============
*/
void
CG_ClearWeapLerpFrame ( weaponInfo_t *wi, lerpFrame_t *lf, int animationNumber )
{
	lf->frameTime = lf->oldFrameTime = cg.time;
	CG_SetWeapLerpFrameAnimation( wi, lf, animationNumber );
	lf->oldFrame = lf->frame = lf->animation->firstFrame;
	lf->oldFrameModel = lf->frameModel = lf->animation->mdxFile;
}


/*
===============
CG_RunWeapLerpFrame

Sets cg.snap, cg.oldFrame, and cg.backlerp
cg.time should be between oldFrameTime and frameTime after exit
===============
*/
static void
CG_RunWeapLerpFrame ( clientInfo_t *ci, weaponInfo_t *wi, lerpFrame_t *lf, int newAnimation, float speedScale )
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
	if ( !lf->animation )
	{
		CG_ClearWeapLerpFrame( wi, lf, newAnimation );
	}
	else if ( newAnimation != lf->animationNumber )
	{
		if ( (newAnimation &~ANIM_TOGGLEBIT) == PM_RaiseAnimForWeapon( cg.snap->ps.nextWeapon) )
		{
			CG_ClearWeapLerpFrame( wi, lf, newAnimation );	// clear when switching to raise (since it should be out of view anyway)
		}
		else
		{
			CG_SetWeapLerpFrameAnimation( wi, lf, newAnimation );
		}
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
			return;											// shouldn't happen
		}

		if ( cg.time < lf->animationTime )
		{
			lf->frameTime = lf->animationTime;				// initial lerp
		}
		else
		{
			lf->frameTime = lf->oldFrameTime + anim->frameLerp;
		}

		f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		f *= speedScale;									// adjust for haste, etc
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
==============
CG_WeaponAnimation
==============
*/


//----(SA)	modified.  this is now client-side only (server does not dictate weapon animation info)
static void
CG_WeaponAnimation ( playerState_t *ps, weaponInfo_t *weapon, int *weapOld, int *weap, float *weapBackLerp )
{
	centity_t		*cent = &cg.predictedPlayerEntity;
	clientInfo_t	*ci = &cgs.clientinfo[ps->clientNum];
	if ( cg_noPlayerAnims.integer )
	{
		*weapOld = *weap = 0;
		return;
	}

	CG_RunWeapLerpFrame( ci, weapon, &cent->pe.weap, ps->weapAnim, 1 );
	*weapOld = cent->pe.weap.oldFrame;
	*weap = cent->pe.weap.frame;
	*weapBackLerp = cent->pe.weap.backlerp;
	if ( cg_debugAnim.integer == 3 )
	{
		CG_Printf( "oldframe: %d   frame: %d   backlerp: %f\n", cent->pe.weap.oldFrame, cent->pe.weap.frame,
				   cent->pe.weap.backlerp );
	}
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// (SA) it wasn't used anyway


/*
==============
CG_CalculateWeaponPosition
==============
*/
static void
CG_CalculateWeaponPosition ( vec3_t origin, vec3_t angles )
{
	float	scale;
	int		delta;
	float	fracsin;
	float	spd;
	VectorCopy( cg.refdef_current->vieworg, origin );
	VectorCopy( cg.refdefViewAngles, angles );

	/*if( cg.predictedPlayerState.eFlags & EF_MOUNTEDTANK && !(cg.predictedPlayerState.eFlags & EF_AIMING) ) {
		angles[PITCH] = cg.refdefViewAngles[PITCH] / 1.2;
	}*/
	if
	(
		!(cg.predictedPlayerState.eFlags & EF_AIMING) &&
		cg.predictedPlayerState.weapon != WP_MORTAR_SET &&
		cg.predictedPlayerState.weapon != WP_MOBILE_MG42_SET &&
		cg.predictedPlayerState.weapon != WP_30CAL_SET &&
		cg.predictedPlayerState.weaponstate != WEAPON_RELOADING
	)
	{
		if ( cg.predictedPlayerState.eFlags & EF_MOUNTEDTANK )
		{
			angles[PITCH] = cg.refdefViewAngles[PITCH] / 1.3;
		}
		else if
			(
				cg.predictedPlayerState.eFlags & EF_BROWNING_ACTIVE ||
				cg.predictedPlayerState.eFlags & EF_MG42_ACTIVE
			)
		{

			//	angles[PITCH] = cg.refdefViewAngles[PITCH] / (-1.5);
			angles[PITCH] = cg.refdefViewAngles[PITCH] / 1.2;

			//origin[0] += cg.refdefViewAngles[PITCH] / 7;
			VectorMA( origin, 1.1f * angles[PITCH], cg.refdef_current->viewaxis[0], origin );
		}
		else
		{
			angles[PITCH] = cg.refdefViewAngles[PITCH] / 1.1;
		}
	}

	if
	(
		!cg.renderingThirdPerson &&
		!cg_firstPerson.integer &&
		(
			cg.predictedPlayerState.weapon == WP_MORTAR_SET ||
			cg.predictedPlayerState.weapon == WP_MOBILE_MG42_SET ||
			cg.predictedPlayerState.weapon == WP_30CAL_SET
		) &&
		!(cg.predictedPlayerState.eFlags & EF_AIMING) &&
		cg.predictedPlayerState.weaponstate != WEAPON_RAISING
	)
	{
		angles[PITCH] = cg.pmext.mountedWeaponAngles[PITCH];
	}

	if ( cg.predictedPlayerState.eFlags & EF_PRONE_MOVING )
	{
		int pronemovingtime = cg.time - cg.proneMovingTime;
		if ( pronemovingtime > 0 )
		{	// div by 0
			float	factor = pronemovingtime > 200 ? 1.f : 1.f / ( 200.f / (float) pronemovingtime );
			VectorMA( origin, factor * -20, cg.refdef_current->viewaxis[0], origin );
			VectorMA( origin, factor * 3, cg.refdef_current->viewaxis[1], origin );
		}
	}
	else
	{
		int pronenomovingtime = cg.time - -cg.proneMovingTime;
		if ( pronenomovingtime < 200 )
		{
			float	factor = pronenomovingtime == 0 ? 1.f : 1.f - ( 1.f / (200.f / (float) pronenomovingtime) );
			VectorMA( origin, factor * -20, cg.refdef_current->viewaxis[0], origin );
			VectorMA( origin, factor * 3, cg.refdef_current->viewaxis[1], origin );
		}
	}

	////////////////////////////////////////////////////////////////////////
	spd = VectorLength( cg.predictedPlayerState.velocity );
	if ( spd && cg_gun_pullback.integer > 0 && !(cg.predictedPlayerState.eFlags & EF_AIMING) )
	{
		VectorMA( origin, spd * -0.027, cg.refdef_current->viewaxis[0], origin );
		VectorMA( origin, spd * 0.0015, cg.refdef_current->viewaxis[1], origin );
		VectorMA( origin, spd * -0.012, cg.refdef_current->viewaxis[2], origin );
	}

	// adjust 'lean' into weapon
	if ( cg.predictedPlayerState.leanf != 0 && !(cg.predictedPlayerState.eFlags & EF_AIMING) )
	{
		vec3_t	right, up;
		float	myfrac = 1.0f;
		switch ( cg.predictedPlayerState.weapon )
		{
			case WP_FLAMETHROWER:
			case WP_KAR98:
			case WP_CARBINE:
			case WP_GPG40:
			case WP_M7:
			case WP_K43:
				myfrac = 2.0f;
				break;
			case WP_GARAND:
				myfrac = 3.0f;
				break;
		}

		// reverse the roll on the weapon so it stays relatively level
		angles[ROLL] -= cg.predictedPlayerState.leanf / ( myfrac * 2.0f );
		AngleVectors( angles, NULL, right, up );
		VectorMA( origin, angles[ROLL], right, origin );

		// pitch the gun down a bit to show that firing is not allowed when leaning
		angles[PITCH] += ( abs( cg.predictedPlayerState.leanf) / 2.0f );

		// this gives you some impression that the weapon stays in relatively the same
		// position while you lean, so you appear to 'peek' over the weapon
		AngleVectors( cg.refdefViewAngles, NULL, right, NULL );
		VectorMA( origin, -cg.predictedPlayerState.leanf / 4.0f, right, origin );
	}
#ifdef __FREEHAND__
	// free hand move
	{
		vec3_t	right, up;

		angles[PITCH] = cg.predictedPlayerState.stats[STAT_CROSSHAIR_PITCH];
		angles[YAW] = cg.predictedPlayerState.stats[STAT_CROSSHAIR_YAW];

		AngleVectors( angles, NULL, right, up );

		VectorMA( origin, 0.2*(cg.predictedPlayerState.viewangles[YAW]-cg.predictedPlayerState.stats[STAT_CROSSHAIR_YAW]), right, origin );
		VectorMA( origin, 0.2*(cg.predictedPlayerState.viewangles[PITCH]-cg.predictedPlayerState.stats[STAT_CROSSHAIR_PITCH]), up, origin );
	}
#endif
	// on odd legs, invert some angles
	if ( cg.bobcycle & 1 )
	{
		scale = -cg.xyspeed;
	}
	else
	{
		scale = cg.xyspeed;
	}

	// gun angles from bobbing
	angles[ROLL] += scale * cg.bobfracsin * 0.005;
	angles[YAW] += scale * cg.bobfracsin * 0.01;
	angles[PITCH] += cg.xyspeed * cg.bobfracsin * 0.005;

	// drop the weapon when landing
	delta = cg.time - cg.landTime;
	if ( delta < LAND_DEFLECT_TIME )
	{
		origin[2] += cg.landChange * 0.25 * delta / LAND_DEFLECT_TIME;
	}
	else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME )
	{
		origin[2] += cg.landChange * 0.25 * ( LAND_DEFLECT_TIME + LAND_RETURN_TIME - delta ) / LAND_RETURN_TIME;
	}

	// idle drift
	if
	(
		(!(cg.predictedPlayerState.eFlags & EF_MOUNTEDTANK)) &&
		!(cg.predictedPlayerState.eFlags & EF_AIMING) &&
		cg.predictedPlayerState.weapon != WP_MORTAR_SET &&
		cg.predictedPlayerState.weapon != WP_MOBILE_MG42_SET &&
		cg.predictedPlayerState.weapon != WP_30CAL_SET
	)
	{
		scale = 120;
		fracsin = sin( cg.time * 0.001 );
		angles[ROLL] += scale * fracsin * 0.01;
		angles[YAW] += scale * fracsin * 0.01;
		angles[PITCH] += scale * fracsin * 0.01;
	}

	// RF, subtract the kickAngles
	VectorMA( angles, -1.0, cg.kickAngles, angles );
}

// Ridah


/*
===============
CG_FlamethrowerFlame
===============
*/
static void
CG_FlamethrowerFlame ( centity_t *cent, vec3_t origin )
{

	/*if ( cent->currentState.eFlags & EF_LADDER ) {
		return;
	}*/
	if ( cent->currentState.constantLight != 0 && !(cent->currentState.eFlags & EF_AIMING) )
	{
		return;
	}

	if ( cent->currentState.weapon != WP_FLAMETHROWER )
	{
		return;
	}

	CG_FireFlameChunks( cent, origin, cent->lerpAngles, 1.0, qtrue );
	return;
}

// done.


/*
========================
CG_AddWeaponWithPowerups
========================
*/
static void
CG_AddWeaponWithPowerups ( refEntity_t *gun, int powerups, playerState_t *ps, centity_t *cent )
{

	// add powerup effects
	// DHM - Nerve :: no powerup effects on weapons
	trap_R_AddRefEntityToScene( gun );
}


/* */
void
CG_AddFlashLighting ( vec3_t origin, vec4_t rgb )
{
	int			rf = 0;
	refEntity_t ent;
	int			shader = trap_R_RegisterShader( "test_light" );
	memset( &ent, 0, sizeof(ent) );
	VectorCopy( origin, ent.origin );
	ent.reType = RT_SPRITE;
	ent.customShader = shader;

	//ent.radius = 16.66;
	//ent.radius = 2.0;
	//if (VectorDistance(cg.refdef.vieworg, origin) > 512)
	{
		float	dist = VectorDistance( cg.refdef.vieworg, origin );
		if ( dist < 1000 )
		{
			ent.radius = 120.0 * ( dist * 0.001 );
		}
		else
		{
			ent.radius = 120.0;
		}
	}

	//else
	//	ent.radius = 30.0;
	if ( ent.radius < 30 )
	{
		ent.radius = 30.0;
	}

	ent.renderfx = rf;

	/*
	ent.shaderRGBA[0] = 255;
	ent.shaderRGBA[1] = 255;
	ent.shaderRGBA[2] = 150;
	ent.shaderRGBA[3] = ent.shaderRGBA[3] = 180;*/

	//ent.shaderRGBA[3] = ent.shaderRGBA[3] = 80;
	ent.shaderRGBA[0] = rgb[0];
	ent.shaderRGBA[1] = rgb[1];
	ent.shaderRGBA[2] = rgb[2];
	ent.shaderRGBA[3] = rgb[3];
	trap_R_AddRefEntityToScene( &ent );

	//CG_Printf("Flash light drawn!\n");
}

/*
=============
CG_AddPlayerWeapon

Used for both the view weapon (ps is valid) and the world modelother character models (ps is NULL)
The main player will have this called for BOTH cases, so effects like light and
sound should only be done on the world model case.
=============
*/
static qboolean debuggingweapon = qfalse;


/* */
void
CG_AddPlayerWeapon ( refEntity_t *parent, playerState_t *ps, centity_t *cent, qboolean UI )
{											// UQ1: I have totally cleaned this procedure... It was a f*king mess!!!
	refEntity_t			gun;
	refEntity_t			barrel;
	refEntity_t			flash;
	vec3_t				angles;
	weapon_t			weaponNum;
	weaponInfo_t		*weapon;
	centity_t			*nonPredictedCent;
	qboolean			firing;				// Ridah
	qboolean			akimboFire = qfalse;
	clientInfo_t		*ci;
	bg_character_t		*character;
	qboolean			drawpart;
	int					i;
	qboolean			isPlayer;
	vec3_t				porg, gorg, norm;	// player/gun origin
	float				gdist;
	trace_t				tr;
	bg_playerclass_t	*classInfo;
	classInfo = BG_GetPlayerClassInfo( cgs.clientinfo[cent->currentState.clientNum].team,
									   cgs.clientinfo[cent->currentState.clientNum].cls );

	/////////////////////////////////
	ci = &cgs.clientinfo[cent->currentState.clientNum];
	character = CG_CharacterForClientinfo( ci, cent );

	/////////////////////////////////
	// (SA) might as well have this check consistant throughout the routine
	isPlayer = ( qboolean ) ( cent->currentState.clientNum == cg.snap->ps.clientNum );
	weaponNum = cent->currentState.weapon;
	if ( ps && cg.cameraMode )
	{
		return;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// jaquboss
																											/////////////////////////////////////////////////////////////////////////////////
	if ( !ps || cg.renderingThirdPerson || cg_firstPerson.integer )
	{
		refEntity_t		backgun;
		refEntity_t		backs;
		weapon_t		backweap;
		weaponInfo_t	*backweapon;
		memset( &backgun, 0, sizeof(backgun) );
		VectorCopy( parent->lightingOrigin, backgun.lightingOrigin );
		backgun.shadowPlane = parent->shadowPlane;
		backgun.renderfx = parent->renderfx;
		memset( &backs, 0, sizeof(backs) );
		VectorCopy( parent->lightingOrigin, backs.lightingOrigin );
		backs.shadowPlane = parent->shadowPlane;
		backs.renderfx = parent->renderfx;
		backweap = cent->currentState.density;
		if ( backweap )
		{
			if
			(
				(
					backweap != weaponNum &&
					(!(BG_IsScopedWeapon( weaponNum))) &&
					weaponNum != WP_GPG40 // dont make player primaries appear when we have it's secondaries
					&&
					weaponNum != WP_M7 &&
					weaponNum != WP_MORTAR_SET &&
					weaponNum != WP_30CAL_SET &&
					weaponNum != WP_MOBILE_MG42_SET
				) ||
				(cent->currentState.eFlags & EF_MG42_ACTIVE) ||
				(cent->currentState.eFlags & EF_TALK) ||
				(cent->currentState.eFlags & EF_BROWNING_ACTIVE) ||
				(cent->currentState.eFlags & EF_AAGUN_ACTIVE) ||
				(cent->currentState.eFlags & EF_MOUNTEDTANK) ||
				//(cent->currentState.eFlags & EF_LADDER ) ||
				(cg.snap->ps.pm_flags & PMF_LADDER && isPlayer)
			)
			{
				backweapon = &cg_weapons[backweap];
				if
				(
					(character->accModels[ACC_BACK] || backweap == WP_MORTAR || backweap == WP_FLAMETHROWER) &&
					backweap != WP_MOBILE_MG42 &&
					backweap != WP_30CAL
				)
				{
					backs.hModel = cgs.media.BacksModel2;
				}
				else
				{
					backs.hModel = cgs.media.BacksModel;
				}

				CG_PositionEntityOnTag( &backs, parent, "tag_back", 0, NULL );
				CG_AddWeaponWithPowerups( &backs, cent->currentState.powerups, ps, cent );
				if ( backweap != WP_FLAMETHROWER )
				{
					backgun.hModel = backweapon->weaponModel[W_PU_MODEL].model;
				}
				else
				{
					backgun.hModel = backweapon->weaponModel[W_TP_MODEL].model;
				}

				CG_PositionEntityOnTag( &backgun, &backs, "tag_backweap", 0, NULL );
				CG_AddWeaponWithPowerups( &backgun, cent->currentState.powerups, ps, cent );
			}
		}

		if ( cg_shadowPlanes.integer && !UI )
		{
			backgun.renderfx &= ~RF_THIRD_PERSON;
			backgun.customShader = cgs.media.shadowShader;
			backgun.customSkin = 0;
			trap_R_AddRefEntityToScene( &backgun );
		}
	}

	/////////////////////////////////////////////////////////////////////////////////
	// don't draw any weapons when the binocs are up
	if ( cent->currentState.eFlags & EF_ZOOMING )
	{
		return;
	}

	if ( !ps && cent->currentState.eFlags & EF_TALK )
	{
		return;
	}

#ifdef NAZI_SALUTE
	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_AXIS && cg_nazi.integer > 0 )
	{
		if ( !ps && cg.snap->ps.pm_flags & PMF_TIME_LOCKPLAYER && isPlayer )
		{									// jaquboss need something to no draw weapons when salute..
			return;
		}									// not now , but it will be good to have it with true salute anim , or if we will decide to have nazi salute anim ( that is only hardly possible )
	}
#endif
	if ( ps && cg.zoomedSniper
#ifdef __PORTAL_SCOPE__
		&& !cg_portalScope.integer 
#endif
		)
	{
		return;
	}

	if ( weaponNum == WP_GRENADE_PINEAPPLE || weaponNum == WP_GRENADE_LAUNCHER )
	{
		if ( ps && !ps->ammoclip[weaponNum] )
		{
			return;
		}
	}

	// no weapon when on mg_42
	if ( cent->currentState.eFlags & EF_MOUNTEDTANK )
	{
		if ( !ps && !UI )
		{
			if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME * 2 )
			{
				if
				(
					cg_entities[cg_entities[cg_entities[cent->currentState.number].tagParent].tankparent].
						currentState.density & 8
				)
				{							// should we use a browning?
					trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.browningloop, 255, 0 );
				}
				else
				{
					trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.mg42loop, 255, 0 );
				}

				cent->pe.lightningFiring = qtrue;
			}
		}

		if ( cent->pe.lightningFiring && !(cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME * 2) )
		{
			sfxHandle_t calmSound;
			if
			(
				cg_entities[cg_entities[cg_entities[cent->currentState.number].tagParent].tankparent].currentState.
					density & 8
			)
			{
				calmSound = cgs.media.browningcalm;
			}
			else
			{
				calmSound = cgs.media.mg42calm;
			}

			if ( calmSound )
			{
				trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, calmSound );
			}

			cent->pe.lightningFiring = qfalse;
		}

		if ( isPlayer && !cg.renderingThirdPerson && !cg_firstPerson.integer )
		{
			return;
		}

		if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )
		{
			memset( &flash, 0, sizeof(flash) );
			flash.renderfx = RF_LIGHTING_ORIGIN;
			flash.hModel = cgs.media.mg42muzzleflash;
			VectorCopy( cg_entities[cg_entities[cent->currentState.number].tagParent].mountedMG42Flash.origin,
						flash.origin );
			AxisCopy( cg_entities[cg_entities[cent->currentState.number].tagParent].mountedMG42Flash.axis, flash.axis );
			trap_R_AddRefEntityToScene( &flash );

			// ydnar: add dynamic light
			trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );

			//CG_AddFlashLighting( flash.origin );
		}

		return;
	}

	if
	(
		cent->currentState.eFlags & EF_MG42_ACTIVE ||
		cent->currentState.eFlags & EF_BROWNING_ACTIVE ||
		cent->currentState.eFlags & EF_AAGUN_ACTIVE
	)
	{
		if ( !ps && !UI )
		{
			if
			(
				(cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME * 2) &&
				cent->currentState.eFlags & EF_BROWNING_ACTIVE
			)
			{
				trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.browningloop, 255, 0 );
				cent->pe.lightningFiring = qtrue;
			}
			else if
				(
					(cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME * 2) &&
					cent->currentState.eFlags & EF_MG42_ACTIVE
				)
			{
				trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, cgs.media.mg42loop, 255, 0 );
				cent->pe.lightningFiring = qtrue;
			}
		}

		if ( cent->pe.lightningFiring && !(cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME * 2) )
		{
			sfxHandle_t calmSound;
			if ( cent->currentState.eFlags & EF_BROWNING_ACTIVE )
			{
				calmSound = cgs.media.browningcalm;
			}
			else
			{
				calmSound = cgs.media.mg42calm;
			}

			if ( calmSound )
			{
				trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, calmSound );
			}

			cent->pe.lightningFiring = qfalse;
		}

		// Arnout: MG42 Muzzle Flash
		if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )
		{
			if
			(
				(isPlayer && cg.snap->ps.eFlags & EF_AAGUN_ACTIVE) ||
				cg_firstPerson.integer ||
				cg.renderingThirdPerson ||
				!isPlayer
			)
			{
				CG_MG42EFX( cent );
			}
		}

		return;
	}

	if ( (!ps || cg.renderingThirdPerson || cg_firstPerson.integer) && cent->currentState.eFlags & EF_PRONE_MOVING )
	{
		return;
	}

	weapon = &cg_weapons[weaponNum];
	if ( BG_IsAkimboWeapon( weaponNum) )
	{
		if ( isPlayer )
		{
			akimboFire = BG_AkimboFireSequence( weaponNum,
												cg.predictedPlayerState.ammoclip[BG_FindClipForWeapon( weaponNum)],
												cg.predictedPlayerState.ammoclip[BG_FindClipForWeapon( BG_AkimboSidearm( weaponNum))] );
		}
		else if ( ps )
		{
			akimboFire = BG_AkimboFireSequence( weaponNum, ps->ammoclip[BG_FindClipForWeapon( weaponNum)],
												ps->ammoclip[BG_FindClipForWeapon( BG_AkimboSidearm( weaponNum))] );
		}

		// Gordon: FIXME: alternate for other clients, store flip-flop on cent or smuffin
	}

	// add the weapon
	memset( &gun, 0, sizeof(gun) );
	VectorCopy( parent->lightingOrigin, gun.lightingOrigin );
	if ( !ps || cg.renderingThirdPerson || cg_firstPerson.integer )
	{
		gun.shadowPlane = parent->shadowPlane;
	}

	gun.renderfx = parent->renderfx;

	////////////////////////////////////////////////////////////////////////////////////////////////////
																											/// first person
																											////////////////////////////////////////////////////////////////////////////////////////////////////
	if ( ps )
	{
		team_t	team = ps->persistant[PERS_TEAM];
		int		plclass = ps->stats[STAT_PLAYER_CLASS];
		gun.hModel = weapon->weaponModel[W_FP_MODEL].model;
		CG_WeaponAnimation( ps, weapon, &gun.oldframe, &gun.frame, &gun.backlerp ); //jaquboss , animate using vertex animation ( moving fingers , for example )
		if ( (weaponNum != WP_SATCHEL) && (cent->currentState.powerups & (1 << PW_OPS_DISGUISED)) )
		{
			team = team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;
			plclass = 0;
			if ( cent->currentState.powerups & (1 << PW_OPS_CLASS_1) )
			{
				plclass |= 1;
			}

			if ( cent->currentState.powerups & (1 << PW_OPS_CLASS_2) )
			{
				plclass |= 2;
			}

			if ( cent->currentState.powerups & (1 << PW_OPS_CLASS_3) )
			{
				plclass |= 4;
			}
		}

		if ( weapon->weaponModel[W_FP_MODEL].useCharacterSkin )
		{
			bg_character_t	*character;
			character = CG_CharacterForClientinfo( ci, cent );
			gun.customSkin = character->weaponskin;
		}
		else if ( weapon->weaponModel[W_FP_MODEL].skin[team] )
		{
			gun.customSkin = weapon->weaponModel[W_FP_MODEL].skin[team];
		}
		else
		{
			gun.customSkin = weapon->weaponModel[W_FP_MODEL].skin[0];				// if not loaded it's 0 so doesn't do any harm
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
																											/// thirdperson	
																											/////////////////////////////////////////////////////////////////////////////////////////////////////////
	else
	{
		team_t	team = cgs.clientinfo[cent->currentState.clientNum].team;
		if ( (weaponNum != WP_SATCHEL) && cent->currentState.powerups & (1 << PW_OPS_DISGUISED) )
		{
			team = team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;
		}

		if
		(
			(
				cent->currentState.eFlags & EF_AIMING &&
				(weaponNum == WP_MOBILE_MG42_SET || weaponNum == WP_MOBILE_MG42)
			) ||
			((weaponNum == WP_MOBILE_MG42_SET) && !(cent->currentState.eFlags & EF_PRONE))
		)
		{																//
			gun.hModel = weapon->weaponModel[W_PU_MODEL].model;
		}
		else
		{
			gun.hModel = weapon->weaponModel[W_TP_MODEL].model;
		}

		if ( (team == TEAM_AXIS) && weapon->weaponModel[W_TP_MODEL].skin[TEAM_AXIS] )
		{
			gun.customSkin = weapon->weaponModel[W_FP_MODEL].skin[TEAM_AXIS];
		}
		else if ( (team == TEAM_ALLIES) && weapon->weaponModel[W_TP_MODEL].skin[TEAM_ALLIES] )
		{
			gun.customSkin = weapon->weaponModel[W_TP_MODEL].skin[TEAM_ALLIES];
		}
		else
		{
			gun.customSkin = weapon->weaponModel[W_TP_MODEL].skin[0];	// if not loaded it's 0 so doesn't do any harm
		}
	}

	if ( !gun.hModel )
	{
		if ( debuggingweapon )
		{
			CG_Printf( "returning due to: !gun.hModel\n" );
		}

		return;
	}

	if ( !ps && cg.snap->ps.pm_flags & PMF_LADDER && isPlayer )			//----(SA) player on ladder
	{
		if ( debuggingweapon )
		{
			CG_Printf( "returning due to: !ps && cg.snap->ps.pm_flags & PMF_LADDER\n" );
		}

		return;
	}

	if ( !ps && !UI )
	{

		// add weapon ready sound
		if ( (cent->currentState.eFlags & EF_FIRING) && weapon->firingSound )
		{																// lightning gun and guantlet make a different sound when fire is held down
			trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, weapon->firingSound, 255, 0 );
			cent->pe.lightningFiring = qtrue;
		}
		else if ( weapon->readySound )
		{
			trap_S_AddLoopingSound( cent->lerpOrigin, vec3_origin, weapon->readySound, 255, 0 );
		}
	}

	if ( cent->pe.lightningFiring && !(cent->currentState.eFlags & EF_FIRING) )
	{
		if ( weapon->spindownSound )
		{
			trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, weapon->spindownSound );
		}

		cent->pe.lightningFiring = qfalse;
	}

	// Ridah
	firing = ( (cent->currentState.eFlags & EF_FIRING) != 0 );
	if
	(
		ps &&
		!cg.renderingThirdPerson &&
		!cg_firstPerson.integer &&
		cg.predictedPlayerState.weapon == WP_MORTAR_SET &&
		cg.predictedPlayerState.weaponstate != WEAPON_RAISING
	)
	{
		vec3_t	angles;
		angles[YAW] = angles[ROLL] = 0.f;
		if ( cg_gun_realisticposition.integer > 0 )
		{
			angles[PITCH] = -1.3 * AngleNormalize180( cg.pmext.mountedWeaponAngles[PITCH] - ps->viewangles[PITCH] );
		}
		else
		{
			angles[PITCH] = -.4f * AngleNormalize180( cg.pmext.mountedWeaponAngles[PITCH] - ps->viewangles[PITCH] );	// TODO!!
		}

		AnglesToAxis( angles, gun.axis );
		CG_PositionRotatedEntityOnTag( &gun, parent, "tag_weapon" );
	}
	else if
		(
			(!ps || cg.renderingThirdPerson || cg_firstPerson.integer) &&
			(weaponNum == WP_MORTAR_SET || weaponNum == WP_MORTAR)
		)
	{
		CG_PositionEntityOnTag( &gun, parent, "tag_weapon2", 0, NULL );
	}
	else
	{
		CG_PositionEntityOnTag( &gun, parent, "tag_weapon", 0, NULL );
	}

	if ( ps )
	{
		drawpart = CG_GetPartFramesFromWeap( cent, &gun, parent, W_MAX_PARTS, weapon );									// W_MAX_PARTS specifies this as the primary view model
	}
	else
	{
		drawpart = qtrue;
	}

	if ( drawpart )
	{
		if ( weaponNum == WP_AMMO )
		{
			if ( ps )
			{
				if ( cgs.clientinfo[ps->clientNum].skill[SK_SIGNALS] >= 1 )
				{
					gun.customShader = weapon->modModels[0];
				}
			}
			else
			{
				if ( cgs.clientinfo[cent->currentState.clientNum].skill[SK_SIGNALS] >= 1 )
				{
					gun.customShader = weapon->modModels[0];
				}
			}
		}

		if ( !ps )
		{
			if ( weaponNum == WP_MEDIC_SYRINGE )
			{
				if ( cgs.clientinfo[cent->currentState.clientNum].skill[SK_FIRST_AID] >= 3 )
				{
					gun.customShader = weapon->modModels[0];
				}
			}
		}

		CG_AddWeaponWithPowerups( &gun, cent->currentState.powerups, ps, cent );
		if ( !ps && !UI && cg_shadowPlanes.integer )
		{
			gun.renderfx &= ~RF_THIRD_PERSON;
			gun.customShader = cgs.media.shadowShader;
			gun.customSkin = 0;
			trap_R_AddRefEntityToScene( &gun );
		}
	}

	if
	(
		(!ps || cg.renderingThirdPerson || cg_firstPerson.integer) &&
		(
			weaponNum == WP_AKIMBO_COLT ||
			weaponNum == WP_AKIMBO_SILENCEDCOLT ||
			weaponNum == WP_AKIMBO_LUGER ||
			weaponNum == WP_AKIMBO_SILENCEDLUGER
		)
	)
	{

		// add to other hand as well
		CG_PositionEntityOnTag( &gun, parent, "tag_weapon2", 0, NULL );
		CG_AddWeaponWithPowerups( &gun, cent->currentState.powerups, ps, cent );
		if ( cg_shadowPlanes.integer && !UI )
		{
			gun.renderfx &= ~RF_THIRD_PERSON;
			gun.customShader = cgs.media.shadowShader;
			gun.customSkin = 0;
			trap_R_AddRefEntityToScene( &gun );
		}
	}

	if ( (!ps || cg.renderingThirdPerson || cg_firstPerson.integer) && !UI )
	{				// jaquboss , old wont work
		refEntity_t brass;
		if ( BG_IsAkimboWeapon( weaponNum) )
		{
			if ( !cent->akimboFire )
			{
				CG_PositionRotatedEntityOnTag( &brass, parent, "tag_weapon2" );
			}
			else
			{
				CG_PositionRotatedEntityOnTag( &brass, parent, "tag_weapon" );
			}
		}
		else
		{
			CG_PositionRotatedEntityOnTag( &brass, parent, "tag_weapon" );
		}

		VectorCopy( brass.origin, cent->pe.ejectBrassCasingOrigin );
	}
	else if ( !UI )
	{
		refEntity_t brass;
		if ( BG_IsAkimboWeapon( weaponNum) && akimboFire )
		{
			CG_PositionRotatedEntityOnTag( &brass, parent, "tag_brass2" );
		}
		else
		{
			CG_PositionRotatedEntityOnTag( &brass, parent, "tag_brass" );
		}

		VectorCopy( brass.origin, cent->pe.ejectBrassCasingOrigin );
	}

	memset( &barrel, 0, sizeof(barrel) );
	VectorCopy( parent->lightingOrigin, barrel.lightingOrigin );
	if ( !(ps) || cg.renderingThirdPerson || cg_firstPerson.integer )
	{
		barrel.shadowPlane = parent->shadowPlane;
	}

	barrel.renderfx = parent->renderfx;

	// add barrels
	// attach generic weapon parts to the first person weapon.
	// if a barrel should be attached for third person, add it in the (!ps) section below
	angles[YAW] = angles[PITCH] = 0;
	if ( ps )
	{
		qboolean	spunpart;
		for ( i = W_PART_1; i < W_MAX_PARTS; i++ )
		{
			if ( weaponNum == WP_MORTAR_SET && (i == W_PART_4 || i == W_PART_5) )
			{
				if
				(
					ps &&
					!cg.renderingThirdPerson &&
					!cg_firstPerson.integer &&
					cg.predictedPlayerState.weaponstate != WEAPON_RAISING
				)
				{
					continue;
				}
			}

			spunpart = qfalse;
			barrel.hModel = weapon->partModels[W_FP_MODEL][i].model;
			if ( weaponNum == WP_MORTAR_SET )
			{
				if ( i == W_PART_3 )
				{
					if
					(
						ps &&
						!cg.renderingThirdPerson &&
						!cg_firstPerson.integer &&
						cg.predictedPlayerState.weaponstate != WEAPON_RAISING
					)
					{
						angles[PITCH] = angles[YAW] = 0.f;
						angles[ROLL] = .8f * AngleNormalize180( cg.pmext.mountedWeaponAngles[YAW] - ps->viewangles[YAW] );
						spunpart = qtrue;
					}
				}
				else if ( i == W_PART_1 || i == W_PART_2 )
				{
					if
					(
						ps &&
						!cg.renderingThirdPerson &&
						!cg_firstPerson.integer &&
						cg.predictedPlayerState.weaponstate != WEAPON_RAISING
					)
					{
						angles[YAW] = angles[ROLL] = 0.f;
						angles[PITCH] = -.4f * AngleNormalize180( cg.pmext.mountedWeaponAngles[PITCH] - ps->viewangles[PITCH] );
						spunpart = qtrue;
					}
				}
			}
			else if ( weaponNum == WP_MOBILE_MG42_SET || weaponNum == WP_30CAL_SET )
			{

			}

			if ( spunpart )
			{
				AnglesToAxis( angles, barrel.axis );
			}

			if ( barrel.hModel )
			{
				if ( spunpart )
				{
					CG_PositionRotatedEntityOnTag( &barrel, parent, weapon->partModels[W_FP_MODEL][i].tagName );
				}
				else
				{
					CG_PositionEntityOnTag( &barrel, parent, weapon->partModels[W_FP_MODEL][i].tagName, 0, NULL );
				}

				drawpart = CG_GetPartFramesFromWeap( cent, &barrel, parent, i, weapon );
				if ( weaponNum == WP_MORTAR_SET && (i == W_PART_1 || i == W_PART_2) )
				{
					if
					(
						ps &&
						!cg.renderingThirdPerson &&
						!cg_firstPerson.integer &&
						cg.predictedPlayerState.weaponstate != WEAPON_RAISING
					)
					{
						if ( cg_gun_realisticposition.integer > 0 )
						{
							VectorMA( barrel.origin, 1.4f * angles[PITCH], cg.refdef_current->viewaxis[0], barrel.origin );
						}
						else
						{
							VectorMA( barrel.origin, .5f * angles[PITCH], cg.refdef_current->viewaxis[0], barrel.origin );
						}
					}
				}

				/////////////////////////////////////////////////////////////////////////////////////
																																																																																																												///	new part skinning
																																																																																																												/////////////////////////////////////////////////////////////////////////////////////
				if ( drawpart )
				{	//0
					team_t	team = ps->persistant[PERS_TEAM];
					int		plclass = ps->stats[STAT_PLAYER_CLASS];
					CG_WeaponAnimation( ps, weapon, &gun.oldframe, &gun.frame, &gun.backlerp ); // ok , it looks ok now
					if ( cent->currentState.powerups & (1 << PW_OPS_DISGUISED) )
					{					// i dunno why this wasnt here
						team = team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;
						plclass = 0;	// jaquboss ,this should make disguise work with new sleeve code..
						if ( cent->currentState.powerups & (1 << PW_OPS_CLASS_1) )
						{
							plclass |= 1;
						}

						if ( cent->currentState.powerups & (1 << PW_OPS_CLASS_2) )
						{
							plclass |= 2;
						}

						if ( cent->currentState.powerups & (1 << PW_OPS_CLASS_3) )
						{
							plclass |= 4;
						}
					}

					if ( weapon->partModels[W_FP_MODEL][i].useCharacterSkin )
					{
						bg_character_t	*character;
						character = CG_CharacterForClientinfo( ci, cent );
						barrel.customSkin = character->weaponskin;
					}
					else if ( weapon->partModels[W_FP_MODEL][i].skin[team] )
					{
						barrel.customSkin = weapon->partModels[W_FP_MODEL][i].skin[team];
					}
					else
					{
						barrel.customSkin = weapon->partModels[W_FP_MODEL][i].skin[0];	// if not loaded it's 0 so doesn't do any harm
					}

					if ( weaponNum == WP_MEDIC_SYRINGE && i == W_PART_1 )
					{
						if ( cgs.clientinfo[ps->clientNum].skill[SK_FIRST_AID] >= 3 )
						{
							barrel.customShader = weapon->modModels[0];
						}
					}

					CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );
					if ( weaponNum == WP_SATCHEL_DET && i == W_PART_1 )
					{
						float		rangeSquared;
						qboolean	inRange;
						refEntity_t satchelDetPart;
						if ( cg.satchelCharge )
						{
							rangeSquared = DistanceSquared( cg.satchelCharge->lerpOrigin,
															cg.predictedPlayerEntity.lerpOrigin );
						}
						else
						{
							rangeSquared = Square( 2001.f );
						}

						if ( rangeSquared <= Square( 2000) )
						{
							inRange = qtrue;
						}
						else
						{
							inRange = qfalse;
						}

						memset( &satchelDetPart, 0, sizeof(satchelDetPart) );
						VectorCopy( parent->lightingOrigin, satchelDetPart.lightingOrigin );
						if ( !ps || cg.renderingThirdPerson || cg_firstPerson.integer )
						{
							satchelDetPart.shadowPlane = parent->shadowPlane;
						}

						satchelDetPart.renderfx = parent->renderfx;
						satchelDetPart.hModel = weapon->modModels[0];
						CG_PositionEntityOnTag( &satchelDetPart, &barrel, "tag_rlight", 0, NULL );
						satchelDetPart.customShader = inRange ? weapon->modModels[2] : weapon->modModels[3];
						CG_AddWeaponWithPowerups( &satchelDetPart, cent->currentState.powerups, ps, cent );
						CG_PositionEntityOnTag( &satchelDetPart, &barrel, "tag_glight", 0, NULL );
						satchelDetPart.customShader = inRange ? weapon->modModels[5] : weapon->modModels[4];
						CG_AddWeaponWithPowerups( &satchelDetPart, cent->currentState.powerups, ps, cent );
						satchelDetPart.hModel = weapon->modModels[1];
						angles[PITCH] = angles[ROLL] = 0.f;
						if ( inRange )
						{
							angles[YAW] = -30.f + ( 60.f * (rangeSquared / Square( 2000)) );
						}
						else
						{
							angles[YAW] = 30.f;
						}

						AnglesToAxis( angles, satchelDetPart.axis );
						CG_PositionRotatedEntityOnTag( &satchelDetPart, &barrel, "tag_needle" );
						satchelDetPart.customShader = weapon->modModels[2];
						CG_AddWeaponWithPowerups( &satchelDetPart, cent->currentState.powerups, ps, cent );
					}
					else if ( weaponNum == WP_MORTAR_SET && i == W_PART_3 )
					{
						if
						(
							ps &&
							!cg.renderingThirdPerson &&
							!cg_firstPerson.integer &&
							cg.predictedPlayerState.weaponstate != WEAPON_RAISING
						)
						{
							refEntity_t bipodLeg;
							memset( &bipodLeg, 0, sizeof(bipodLeg) );
							VectorCopy( parent->lightingOrigin, bipodLeg.lightingOrigin );
							if ( !ps || cg.renderingThirdPerson || cg_firstPerson.integer )
							{
								bipodLeg.shadowPlane = parent->shadowPlane;
							}

							bipodLeg.renderfx = parent->renderfx;
							bipodLeg.hModel = weapon->partModels[W_FP_MODEL][3].model;
							CG_PositionEntityOnTag( &bipodLeg, &barrel, "tag_barrel4", 0, NULL );
							CG_AddWeaponWithPowerups( &bipodLeg, cent->currentState.powerups, ps, cent );
							bipodLeg.hModel = weapon->partModels[W_FP_MODEL][4].model;
							CG_PositionEntityOnTag( &bipodLeg, &barrel, "tag_barrel5", 0, NULL );
							CG_AddWeaponWithPowerups( &bipodLeg, cent->currentState.powerups, ps, cent );
						}
					}
				}
			}
		}
	}

	// add the scope model to the rifle if you've got it
	if ( isPlayer && !cg.renderingThirdPerson && !cg_firstPerson.integer )
	{									// (SA) for now just do it on the first person weapons
		if ( weaponNum == WP_CARBINE || weaponNum == WP_KAR98 || weaponNum == WP_GPG40 || weaponNum == WP_M7 )
		{
			if
			(
				(
					cg.snap->ps.ammo[BG_FindAmmoForWeapon( WP_GPG40)] ||
					cg.snap->ps.ammo[BG_FindAmmoForWeapon( WP_M7)] ||
					cg.snap->ps.ammoclip[BG_FindAmmoForWeapon( WP_GPG40)] ||
					cg.snap->ps.ammoclip[BG_FindAmmoForWeapon( WP_M7)]
				)
			)
			{
				int anim = cg.snap->ps.weapAnim &~ANIM_TOGGLEBIT;
				if
				(
					anim == PM_AltSwitchFromForWeapon( weaponNum) ||
					anim == PM_AltSwitchToForWeapon( weaponNum) ||
					anim == PM_IdleAnimForWeapon( weaponNum)
				)
				{
					barrel.hModel = weapon->modModels[0];
					barrel.customSkin = 0;
					if ( barrel.hModel )
					{
						CG_PositionEntityOnTag( &barrel, parent, "tag_scope", 0, NULL );
						CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );
					}
				}
			}
		}
		else if ( weaponNum == WP_GARAND )
		{
			barrel.hModel = weapon->modModels[0];
			if ( barrel.hModel )
			{
				barrel.customSkin = 0;
				CG_PositionEntityOnTag( &barrel, &gun, "tag_scope2", 0, NULL );
				CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );
			}

#ifdef __SCOPED_SILENCED__
			barrel.hModel = weapon->modModels[1];
			barrel.customSkin = 0;
			CG_PositionEntityOnTag( &barrel, &gun, "tag_flash", 0, NULL );
			CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );
#endif
		}
/*		else if ( weaponNum == WP_K43 )
		{
			barrel.hModel = weapon->modModels[0];
			barrel.customSkin = 0;
			if ( barrel.hModel )
			{
				CG_PositionEntityOnTag( &barrel, &gun, "tag_scope", 0, NULL );
				CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );
			}

#ifdef __SCOPED_SILENCED__
			barrel.hModel = weapon->modModels[1];
			barrel.customSkin = 0;
			CG_PositionEntityOnTag( &barrel, &gun, "tag_flash", 0, NULL );
			CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );
#endif
		}
*/
	}

	// 3rd person attachements
	else
	{
		if ( weaponNum == WP_M7 /*|| weaponNum == WP_GPG40  || weaponNum == WP_CARBINE || weaponNum == WP_KAR98*/ )
		{

			// the holder
			barrel.customSkin = 0;
			barrel.hModel = weapon->modModels[1];
			CG_PositionEntityOnTag( &barrel, &gun, "tag_flash", 0, NULL );
			CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );

			// the grenade - have to always enabled it, no means of telling if another person has a grenade loaded or not atm :/
			if ( weaponNum == WP_M7 /*|| weaponNum == WP_CARBINE*/ )
			{
				barrel.hModel = weapon->missileModel;
				barrel.customSkin = 0;
				CG_PositionEntityOnTag( &barrel, &barrel, "tag_prj", 0, NULL );
				CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );
			}
		}
		else if
			(
				weaponNum == WP_GARAND ||
				weaponNum == WP_GARAND_SCOPE ||
				weaponNum == WP_K43 ||
				weaponNum == WP_K43_SCOPE
			)
		{

			// the holder
			barrel.customSkin = 0;
			barrel.hModel = weapon->modModels[2];
			CG_PositionEntityOnTag( &barrel, &gun, "tag_scope", 0, NULL );
			CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );
		}
		else if ( weaponNum == WP_MOBILE_MG42 || weaponNum == WP_30CAL )
		{
			barrel.hModel = weapon->modModels[0];
			barrel.customSkin = 0;
			barrel.frame = 1;
			CG_PositionEntityOnTag( &barrel, &gun, "tag_bipod", 0, NULL );
			CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );
		}
		else if ( weaponNum == WP_MOBILE_MG42_SET || weaponNum == WP_30CAL_SET )
		{
			barrel.hModel = weapon->modModels[0];
			barrel.customSkin = 0;
			barrel.frame = 0;
			CG_PositionEntityOnTag( &barrel, &gun, "tag_bipod", 0, NULL );
			CG_AddWeaponWithPowerups( &barrel, cent->currentState.powerups, ps, cent );
		}
	}

	// make sure we aren't looking at cg.predictedPlayerEntity for LG
	nonPredictedCent = &cg_entities[cent->currentState.clientNum];

	// if the index of the nonPredictedCent is not the same as the clientNum
	// then this is a fake player (like on the single player podiums), so
	// go ahead and use the cent
	if ( (nonPredictedCent - cg_entities) != cent->currentState.clientNum )
	{
		nonPredictedCent = cent;
	}

	// add the flash
	memset( &flash, 0, sizeof(flash) );
	VectorCopy( parent->lightingOrigin, flash.lightingOrigin );

	//flash.shadowPlane = parent->shadowPlane;
	flash.renderfx = parent->renderfx;
	if ( ps )
	{
		flash.hModel = weapon->flashModel[W_FP_MODEL];
	}
	else
	{
		flash.hModel = weapon->flashModel[W_TP_MODEL];
	}

	angles[YAW] = 0;
	angles[PITCH] = 0;
	angles[ROLL] = crandom() * 10;
	AnglesToAxis( angles, flash.axis );
	if ( /*isPlayer &&*/ BG_IsAkimboWeapon( weaponNum) )
	{
		if ( !ps || cg.renderingThirdPerson || cg_firstPerson.integer )
		{
			if ( !cent->akimboFire )
			{
				CG_PositionRotatedEntityOnTag( &flash, parent, "tag_weapon" );
				VectorMA( flash.origin, 10, flash.axis[0], flash.origin );
			}
			else
			{
				CG_PositionRotatedEntityOnTag( &flash, &gun, "tag_flash" );
			}
		}
		else
		{
			if ( !cent->akimboFire )
			{
				CG_PositionRotatedEntityOnTag( &flash, parent, "tag_flash2" );
			}
			else
			{
				CG_PositionRotatedEntityOnTag( &flash, parent, "tag_flash" );
			}
		}
	}
	else
	{
		CG_PositionRotatedEntityOnTag( &flash, &gun, "tag_flash" );
	}

	VectorCopy( flash.origin, cent->pe.flashOrigin );
	cent->pe.MuzzleFlash = qfalse;		// activate later

	// store this position for other cgame elements to access
	cent->pe.gunRefEnt = gun;
	cent->pe.gunRefEntFrame = cg.clientFrame;
	if ( (weaponNum == WP_FLAMETHROWER) && (nonPredictedCent->currentState.eFlags & EF_FIRING) )
	{									// continuous flash
	}
	else
	{

		// continuous smoke after firing
#define BARREL_SMOKE_TIME	1000
		if ( ps || cg.renderingThirdPerson || !isPlayer || cg_firstPerson.integer )
		{
			if
			(
				weaponNum == WP_STEN ||
				weaponNum == WP_MOBILE_MG42 ||
				weaponNum == WP_MOBILE_MG42_SET ||
				weaponNum == WP_30CAL_SET ||
				weaponNum == WP_30CAL
			)
			{

				// hot smoking gun
				if ( cg.time - cent->overheatTime < 3000 )
				{
					if ( !(rand() % 3) )
					{
						float	alpha;
						alpha = 1.0f - ( (float) (cg.time - cent->overheatTime) / 3000.0f );
						alpha *= 0.25f; // .25 max alpha
						CG_ParticleImpactSmokePuffExtended( cgs.media.smokeParticleShader, flash.origin, 1000, 8, 20,
															30, alpha, 8.f );
					}
				}
			}
			else if ( weaponNum == WP_PANZERFAUST )
			{
				if ( cg.time - cent->muzzleFlashTime < BARREL_SMOKE_TIME )
				{
					if ( !(rand() % 5) )
					{
						float	alpha;
						alpha = 0.85f - ( (float) (cg.time - cent->muzzleFlashTime) / ((float) (BARREL_SMOKE_TIME) / 2) );	// what fraction of BARREL_SMOKE_TIME are we at , uch jaquboss lowered alpha

						//alpha *= 0.25f;		// .25 max alpha
						CG_ParticleImpactSmokePuffExtended( cgs.media.panzerfaustsmokeParticleShader, flash.origin,
															9000, 1, 2, 12, alpha, 25.f );
					}
				}
			}
		}

		if ( weaponNum == WP_MORTAR_SET )
		{
			if ( ps && !cg.renderingThirdPerson && !cg_firstPerson.integer && cg.time - cent->muzzleFlashTime < 800 )
			{
				CG_ParticleImpactSmokePuffExtended( cgs.media.smokeParticleShader, flash.origin, 700, 16, 20, 30, .12f,
													8.f );
			}
		}

		// impulse flash
		if ( cg.time - cent->muzzleFlashTime > MUZZLE_FLASH_TIME )
		{

			// Ridah, blue ignition flame if not firing flamer
			if ( weaponNum != WP_FLAMETHROWER )
			{
				return;
			}
		}
	}

	// weapons that don't need to go any further as they have no flash or light
	if
	(
		weaponNum == WP_GRENADE_LAUNCHER ||
		weaponNum == WP_GRENADE_PINEAPPLE ||
		weaponNum == WP_KNIFE ||
		weaponNum == WP_DYNAMITE ||
		weaponNum == WP_GPG40 ||
		weaponNum == WP_M7 ||
		weaponNum == WP_LANDMINE ||
		weaponNum == WP_SATCHEL ||
		weaponNum == WP_SATCHEL_DET ||
		weaponNum == WP_SMOKE_BOMB ||
		weaponNum == WP_MEDIC_SYRINGE ||
		weaponNum == WP_MEDIC_ADRENALINE ||
		cg.predictedPlayerState.weaponstate == WEAPON_RAISING ||
		cg.predictedPlayerState.weaponstate == WEAPON_DROPPING
	)
	{
		cent->muzzleFlashTime = 0;
		return;
	}

	// weaps with barrel smoke
	if (  ps || cg.renderingThirdPerson || !isPlayer || cg_firstPerson.integer )
	{
		if ( weaponNum == WP_STEN )
		{
			if ( cg.time - cent->muzzleFlashTime < 100 && !(cent->currentState.eFlags & EF_AIMING))
			{
				CG_ParticleImpactSmokePuffExtended( cgs.media.smokeParticleShader, flash.origin, 500, 8, 20, 30, 0.25f,
													8.f );
			}
		}
	}

	if (  ps || cg.renderingThirdPerson || !isPlayer || cg_firstPerson.integer )
	{
		if
		(
			weaponNum == WP_KAR98 ||
			weaponNum == WP_CARBINE ||
			weaponNum == WP_GARAND ||
			weaponNum == WP_GARAND_SCOPE ||
			weaponNum == WP_FG42 ||
			weaponNum == WP_FG42SCOPE ||
			weaponNum == WP_MOBILE_MG42_SET ||
			weaponNum == WP_MOBILE_MG42 ||
			weaponNum == WP_30CAL_SET ||
			weaponNum == WP_30CAL ||
			weaponNum == WP_M1GARAND ||
			weaponNum == WP_G43 ||
			weaponNum == WP_K43 ||
			weaponNum == WP_K43_SCOPE
		)
		{
			if ( cg.time - cent->muzzleFlashTime < 100 && !(cent->currentState.eFlags & EF_AIMING) )
			{
				CG_ParticleImpactSmokePuffExtended( cgs.media.smokeParticleShader, flash.origin, 600, 8, 20, 30, 0.30f,
													15.f );
			}
		}
	}

	if ( ps || cg.renderingThirdPerson || !isPlayer || cg_firstPerson.integer   )
	{
		if
		(
			weaponNum == WP_STEN ||
			weaponNum == WP_LUGER ||
			weaponNum == WP_COLT ||
			weaponNum == WP_MP40 ||
			weaponNum == WP_THOMPSON ||
#ifdef NWEAPS
			weaponNum == WP_BAR ||
			weaponNum == WP_MP44 ||
#endif
			weaponNum == WP_SILENCER ||
			weaponNum == WP_SILENCED_COLT ||
			weaponNum == WP_AKIMBO_LUGER ||
			weaponNum == WP_AKIMBO_SILENCEDLUGER ||
			weaponNum == WP_AKIMBO_COLT ||
			weaponNum == WP_AKIMBO_SILENCEDCOLT
		)
		{
			if ( cg.time - cent->muzzleFlashTime < 100 && !(cent->currentState.eFlags & EF_AIMING) )
			{
				CG_ParticleImpactSmokePuffExtended( cgs.media.smokeParticleShader, flash.origin, 500, 8, 20, 30, 0.25f,
													8.f );
			}
		}
	}

	if ( ps || cg.renderingThirdPerson || !isPlayer || cg_firstPerson.integer )
	{
		if ( weaponNum == WP_MORTAR_SET || weaponNum == WP_PANZERFAUST )
		{
			if ( cg.time - cent->muzzleFlashTime < 100 )
			{
				CG_ParticleImpactSmokePuffExtended( cgs.media.smokeParticleShader, flash.origin, 1000, 8, 20, 30, 0.50f,
													20.f );
			}
		}
	}

#ifdef __PARTICLE_SYSTEM__
	if ( weapon->flashPS )
	{
		if ( weaponNum != WP_FLAMETHROWER )
		{															//Ridah, hide the flash also for now
			// RF, changed this so the muzzle flash stays onscreen for long enough to be seen
			if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )
			{
				cent->muzzlePS = CG_SpawnNewParticleSystem( weapon->flashPS );
				if ( BG_IsAkimboWeapon( weaponNum) )
				{
					if ( !ps || cg.renderingThirdPerson )
					{
						if ( !cent->akimboFire )
						{
							CG_PositionRotatedEntityOnTag( &flash, parent, "tag_weapon" );
							VectorMA( flash.origin, 10, flash.axis[0], flash.origin );
							CG_SetParticleSystemOrigin( cent->muzzlePS, flash.origin );
							CG_SetParticleSystemCent( cent->muzzlePS, cent );
							CG_AttachParticleSystemToOrigin( cent->muzzlePS );
						}
						else
						{
							CG_SetParticleSystemTag( cent->muzzlePS, gun, gun.hModel, "tag_flash" );
							CG_SetParticleSystemCent( cent->muzzlePS, cent );
							CG_AttachParticleSystemToTag( cent->muzzlePS );
						}
					}
					else
					{
						if ( !cent->akimboFire )
						{
							CG_SetParticleSystemTag( cent->muzzlePS, gun, gun.hModel, "tag_flash2" );
							CG_SetParticleSystemCent( cent->muzzlePS, cent );
							CG_AttachParticleSystemToTag( cent->muzzlePS );
						}
						else
						{
							CG_SetParticleSystemTag( cent->muzzlePS, gun, gun.hModel, "tag_flash" );
							CG_SetParticleSystemCent( cent->muzzlePS, cent );
							CG_AttachParticleSystemToTag( cent->muzzlePS );
						}
					}
				}
				else
				{
					CG_SetParticleSystemTag( cent->muzzlePS, gun, gun.hModel, "tag_flash" );
					CG_SetParticleSystemCent( cent->muzzlePS, cent );
					CG_AttachParticleSystemToTag( cent->muzzlePS );
				}
			}
		}
	}
	else
#endif //__PARTICLE_SYSTEM__
		if ( flash.hModel )
		{
			if ( weaponNum != WP_FLAMETHROWER )
			{														//Ridah, hide the flash also for now
				// RF, changed this so the muzzle flash stays onscreen for long enough to be seen
				if ( cg.time - cent->muzzleFlashTime < MUZZLE_FLASH_TIME )
				{

					//if (firing) {	// Ridah
					cent->pe.MuzzleFlash = qtrue;
					trap_R_AddRefEntityToScene( &flash );
				}
			}
		}

	if ( weaponNum == WP_FLAMETHROWER )
	{
		qboolean	visible = qfalse;
		if ( weaponNum == WP_FLAMETHROWER )
		{
			if ( cg_dynamicLight.integer ) { // use a lightmap-like shader?
				CG_R_AddProjectedDlight( flash.origin, 48, 1, 0, 0, .75f, cgs.media.flameMark );
				trap_R_AddLightToScene2( flash.origin, 55, 255, 0, 0, .95 , 0, REF_JUNIOR_DLIGHT ); // light up the models as well
			} else {// or ET engine vlight?
				trap_R_AddLightToScene2( flash.origin, 48, 25 + (rand() & 130) / 32, 0, 0, .75f, cgs.media.flameMark, 0 );
			}
			//AddDlights(cent, 0, 0, 1, 1, cgs.media.flameMark);
			//			CG_R_AddProjectedDlight( flash.origin, 80, 0.25f, 0.25f, 1.0f, 1.0f, cgs.media.flameMark );
		}

		VectorCopy( cent->currentState.pos.trBase, gorg );
		VectorCopy( cg.refdef_current->vieworg, porg );
		VectorSubtract( gorg, porg, norm );
		gdist = VectorNormalize( norm );
		CG_Trace( &tr, flash.origin, NULL, NULL, cg.refdef_current->vieworg, -1, MASK_SOLID );
		if ( tr.fraction == 1 )
		{
			visible = qtrue;
		}

		if ( isPlayer && !cg.renderingThirdPerson )
		{
			visible = qtrue;
		}

		if ( gdist < cg_coronafardist.integer || (isPlayer && !cg.renderingThirdPerson) )
		{
			if ( cg.predictedPlayerState.leanf == 0 || cg.predictedPlayerState.eFlags & EF_AIMING )
			{
				if ( cent->currentState.eFlags & EF_FIRING )
				{
					trap_R_AddCoronaToScene( flash.origin, 0.8, 0.8, 0.2, 12, cent->currentState.number, visible );
				}
				else
				{
					trap_R_AddCoronaToScene( flash.origin, 0.2, 0.2, 1, 6, cent->currentState.number, visible );
				}
			}
			else
			{
				trap_R_AddCoronaToScene( flash.origin, 0.2, 0.2, 1, 6, cent->currentState.number, visible );
			}
		}
	}

	if ( ps || cg.renderingThirdPerson || !isPlayer || cg_firstPerson.integer )
	{

		// ydnar: no flamethrower flame on prone moving
		// ydnar: or dead players
		if ( firing && !(cent->currentState.eFlags & (EF_PRONE_MOVING | EF_DEAD)) )
		{

			// Ridah, Flamethrower effect
			CG_FlamethrowerFlame( cent, flash.origin );

			// make a dlight for the flash
			if ( weapon->flashDlightColor[0] || weapon->flashDlightColor[1] || weapon->flashDlightColor[2] )
			{

				//%	trap_R_AddLightToScene( flash.origin, 200 + (rand()&31), weapon->flashDlightColor[0],
				//%		weapon->flashDlightColor[1], weapon->flashDlightColor[2], 0 );
				if ( cg_dynamicLight.integer ) { // use a lightmap-like shader?
					CG_R_AddProjectedDlight( flash.origin, 96, 1, weapon->flashDlightColor[0], weapon->flashDlightColor[1], weapon->flashDlightColor[2], cgs.media.gunflash );
					trap_R_AddLightToScene2( flash.origin, 128, 1.25 + (rand() & 31) / 128, weapon->flashDlightColor[0], weapon->flashDlightColor[1], weapon->flashDlightColor[2] , 0, REF_JUNIOR_DLIGHT ); // light up the models as well
				} else {// or ET engine vlight?
					trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, weapon->flashDlightColor[0],
											weapon->flashDlightColor[1], weapon->flashDlightColor[2], cgs.media.gunflash, 0 );
				}

				VectorCopy( cent->currentState.pos.trBase, gorg );	// for coronas
				VectorCopy( cg.refdef_current->vieworg, porg );
				VectorSubtract( gorg, porg, norm );
				gdist = VectorNormalize( norm );

				//AddDlights(cent, 1, 1, 0.5, 1, cgs.media.gunflash);
				//CG_R_AddProjectedDlight( flash.origin, 120, 1.0f, 1.0f, 0.5f, 1.0f, cgs.media.gunflash );
				CG_Trace( &tr, flash.origin, NULL, NULL, cg.refdef_current->vieworg, -1, MASK_SOLID );
				if ( tr.fraction == 1 )
				{
					if ( gdist < cg_coronafardist.integer && cg_corona_flash.integer )
					{
						trap_R_AddCoronaToScene( flash.origin, weapon->flashDlightColor[1], weapon->flashDlightColor[2],
												 weapon->flashDlightColor[3], 6, cent->currentState.number, qtrue );
					}
				}
			}
		}
		else
		{
			if ( weaponNum == WP_FLAMETHROWER )
			{
				vec3_t	angles;
				AxisToAngles( flash.axis, angles );

				// JPW NERVE
				weaponNum = BG_FindAmmoForWeapon( WP_FLAMETHROWER );
				if ( ps )
				{
					if ( ps->ammoclip[weaponNum] )
					{
						CG_FireFlameChunks( cent, flash.origin, angles, 1.0, qfalse );
					}
				}
				else
				{
					CG_FireFlameChunks( cent, flash.origin, angles, 1.0, qfalse );
				}

				// jpw
			}
		}
	}
}

/*
==============
CG_GetAds // todo read from weap file.. , a pretty simple , but who will rewrite weapon files : )

This sets position of gun when using ironsights , sometimes conbined with other animation file
==============
*/


// TODO !

//		WP_BAR , WP_MP44
float
CG_GetAds ( int weapon, int axis )
{
	if ( cg_weapons[weapon].ADSPos[axis] )
	{
		return ( cg_weapons[weapon].ADSPos[axis] );
	}

	if ( axis == 0 )
	{								// X
		switch ( weapon )
		{
	#ifdef NWEAPS
			case WP_MP44:
				return ( 0 );
				break;
			case WP_BAR:
				return ( 5 );
				break;
	#endif
			case WP_AKIMBO_COLT:
			case WP_AKIMBO_SILENCEDCOLT:
			case WP_AKIMBO_LUGER:
			case WP_AKIMBO_SILENCEDLUGER:
				return ( 10 );
				break;
			case WP_30CAL_SET:
			case WP_30CAL:
				return ( -10 );
				break;
			case WP_MOBILE_MG42_SET:
			case WP_MOBILE_MG42:
				return ( -5 );
				break;
			case WP_SILENCED_COLT:
			case WP_COLT:
				return ( 4 );
				break;
			case WP_LUGER:
				return ( 4 );
				break;
			case WP_SILENCER:
				return ( 6 );
				break;
			case WP_STEN:
				if ( cg_gun_realisticposition.integer > 0 )
				{
					return ( 15 );	// more forward
				}
				else
				{
					return ( 10 );
				}
				break;
			case WP_PANZERFAUST:
				return ( 0 );
				break;
			case WP_THOMPSON:
				return ( 0 );
				break;
			case WP_GARAND:
				return ( -2 );
				break;
			case WP_K43:
				return ( 17 );
				break;
			case WP_FG42:
				return ( -5 );
				break;
			case WP_GPG40:
			case WP_KAR98:
			case WP_G43:
				return ( -1 );
				break;

			/*case WP_GPG40:
		return 1;
		break;*/
			case WP_MP40:
				return ( 4 );
				break;
			case WP_M7:
			case WP_M1GARAND:
			case WP_CARBINE:
				return ( -6 );
				break;
		}
	}

	if ( axis == 1 )
	{								// Y
		switch ( weapon )
		{
	#ifdef NWEAPS
			case WP_MP44:
				return ( 8.5 );
				break;
			case WP_BAR:
				return ( 8.5 );
				break;
	#endif
			case WP_30CAL_SET:
				return ( -4.4 );
				break;
			case WP_MOBILE_MG42_SET:
				return ( -0.2 );
				break;
			case WP_30CAL:
				return ( 13.3 );
				break;
			case WP_MOBILE_MG42:
				return ( 17.5 );
				break;
			case WP_FLAMETHROWER:
				return ( 9 );
				break;
			case WP_SILENCED_COLT:
			case WP_COLT:
				return ( 9.6 );
				break;
			case WP_LUGER:
				return ( 9.2 );
				break;
			case WP_SILENCER:
				return ( 9.5 );
				break;
			case WP_STEN:
				return ( 8 );
				break;
			case WP_PANZERFAUST:
				return ( 5.5 );
				break;
			case WP_THOMPSON:
				return ( 7.625 );
				break;
			case WP_GPG40:
			case WP_KAR98:
			case WP_G43:
				return ( 9.1 );		// .05
				break;

			/*
	case WP_GPG40:
		return 9.25;
		break;*/
			case WP_MP40:
				return ( 8.75 );
				break;
			case WP_GARAND:
				return ( 7.5 );
				break;
			case WP_K43:
				return ( 15.2 );
				break;
			case WP_FG42:
				return ( 9 );
				break;
			case WP_M7:
			case WP_M1GARAND:
			case WP_CARBINE:
				return ( 9.6 );
				break;
		}
	}

	if ( axis == 2 )
	{								// Z
		switch ( weapon )
		{
	#ifdef NWEAPS
			case WP_MP44:
				return ( 3.5 );
				break;
			case WP_BAR:
				return ( 6 );
				break;
	#endif
			case WP_AKIMBO_COLT:
			case WP_AKIMBO_SILENCEDCOLT:
			case WP_AKIMBO_LUGER:
			case WP_AKIMBO_SILENCEDLUGER:
				return ( 8 );
				break;
			case WP_30CAL_SET:
				return ( 11.2 );
				break;
			case WP_MOBILE_MG42_SET:
				return ( 10.4 );
				break;
			case WP_30CAL:
				return ( 6.8 );
				break;
			case WP_MOBILE_MG42:
				return ( 6 );
				break;
			case WP_SILENCED_COLT:
				return ( 8 );
				break;
			case WP_COLT:
				return ( 8.0 );
				break;
			case WP_LUGER:
				return ( 8.8 );
				break;
			case WP_SILENCER:
				return ( 9.3 );
				break;
			case WP_STEN:
				return ( 7.8 );
				break;
			case WP_PANZERFAUST:
				return ( 2.4 );
				break;
			case WP_THOMPSON:
				return ( 5.9 );
				break;
			case WP_GPG40:
			case WP_KAR98:
			case WP_G43:
				return ( -1 );
				break;

			/*	case WP_GPG40:
		return 2.6;
		break;*/
			case WP_MP40:
				return ( 4.9 );
				break;
			case WP_M7:
			case WP_M1GARAND:
			case WP_CARBINE:
				return ( 4.9 );
				break;
			case WP_GARAND:
				return ( 5 );
				break;
			case WP_K43:
				return ( 4.2 );

				//return 0.5;
				break;
			case WP_FG42:
				return ( 0.8 );
				break;
		}
	}

	return ( 0 );
}


// RTCW CODE
void
CG_AddPlayerFoot ( refEntity_t *parent, playerState_t *ps, centity_t *cent )
{
	refEntity_t		wolfkick;
	vec3_t			kickangle;
	weaponInfo_t	*weapon;
	weapon_t		weaponNum;
	int				frame;
	static int		oldtime = 0;
	if ( !(cg.snap->ps.persistant[PERS_WOLFKICK]) )
	{
		oldtime = 0;
		return;
	}

	weaponNum = cent->currentState.weapon;
	weapon = &cg_weapons[weaponNum];
	memset( &wolfkick, 0, sizeof(wolfkick) );
	VectorCopy( parent->lightingOrigin, wolfkick.lightingOrigin );
	wolfkick.shadowPlane = parent->shadowPlane;

	// note to self we want this to lerp and advance frame
	wolfkick.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON;;
	wolfkick.hModel = cgs.media.wolfkickModel;
	{
		bg_character_t	*character;
		character = CG_CharacterForClientinfo( &cgs.clientinfo[cg.snap->ps.clientNum], cent );
		if ( character && character->weaponskin )
		{
			wolfkick.customSkin = character->weaponskin;
		}
	}

	VectorCopy( cg.refdef.vieworg, wolfkick.origin );

	//----(SA)	allow offsets for testing boot model
	if ( cg_gun_x.value )
	{
		VectorMA( wolfkick.origin, cg_gun_x.value, cg.refdef.viewaxis[0], wolfkick.origin );
	}

	if ( cg_gun_y.value )
	{
		VectorMA( wolfkick.origin, cg_gun_y.value, cg.refdef.viewaxis[1], wolfkick.origin );
	}

	if ( cg_gun_z.value )
	{
		VectorMA( wolfkick.origin, cg_gun_z.value, cg.refdef.viewaxis[2], wolfkick.origin );
	}

	//----(SA)	end
	VectorCopy( cg.refdefViewAngles, kickangle );
	if ( kickangle[0] < 0 )
	{
		kickangle[0] = 0;	//----(SA)	avoid "Rockette" syndrome :)
	}

	AnglesToAxis( kickangle, wolfkick.axis );
	frame = cg.snap->ps.persistant[PERS_WOLFKICK];

	//	CG_Printf("frame: %d\n", frame);
	wolfkick.frame = frame;
	wolfkick.oldframe = frame - 1;
	wolfkick.backlerp = 1 - cg.frameInterpolation;
	trap_R_AddRefEntityToScene( &wolfkick );
}


/*
==============
CG_AddViewWeapon

Add the weapon, and flash for the player's view
==============
*/
void
CG_AddViewWeapon ( playerState_t *ps )
{
	refEntity_t		hand;
	float			fovOffset;
	vec3_t			angles;
	vec3_t			gunoff;
	vec3_t			gunoff_w;
	vec3_t			gunpos;
	weaponInfo_t	*weapon;
	float			gunx;
	float			guny;
	float			gunz;
	float			f;
	if ( ps->persistant[PERS_TEAM] == TEAM_SPECTATOR )
	{
		return;
	}

	if ( ps->pm_type == PM_INTERMISSION )
	{
		return;
	}

	// no gun if in third person view
	if ( cg.renderingThirdPerson || cg_firstPerson.integer )
	{
		return;
	}

	if ( cg.predictedPlayerState.eFlags & EF_TANK )
	{
		return;
	}

	if ( cg.editingSpeakers )
	{
		return;
	}

	// allow the gun to be completely removed
	if ( (!cg_drawGun.integer && developer.integer > 0) )
	{
		vec3_t	origin;

		//bani - #589
		if
		(
			cg.predictedPlayerState.eFlags & EF_FIRING &&
			!(cg.predictedPlayerState.eFlags & (EF_MG42_ACTIVE | EF_MOUNTEDTANK))
		)
		{

			// special hack for flamethrower...
			VectorCopy( cg.refdef_current->vieworg, origin );
			VectorMA( origin, 18, cg.refdef_current->viewaxis[0], origin );
			VectorMA( origin, -7, cg.refdef_current->viewaxis[1], origin );
			VectorMA( origin, -4, cg.refdef_current->viewaxis[2], origin );

			// Ridah, Flamethrower effect
			CG_FlamethrowerFlame( &cg.predictedPlayerEntity, origin );
		}

		if ( cg.binocZoomTime )
		{
			if ( cg.binocZoomTime < 0 )
			{
				if ( -cg.binocZoomTime + 500 + 200 < cg.time )
				{
					cg.binocZoomTime = 0;
				}
			}
			else
			{
				if ( cg.binocZoomTime + 500 < cg.time )
				{
					trap_SendConsoleCommand( "+zoom\n" );
					cg.binocZoomTime = 0;
				}
				else
				{

				}
			}
		}

		return;
	}

	// don't draw if testing a gun model
	if ( cg.testGun )
	{
		return;
	}

	/*	if ( ps->eFlags & EF_MG42_ACTIVE || ps->eFlags & EF_BROWNING_ACTIVE || ps->eFlags & EF_AAGUN_ACTIVE ) {
		return;
	}*/
	if ( ps->eFlags & EF_AAGUN_ACTIVE )
	{
		return;
	}

	// drop gun lower at higher fov
	if ( !(cg.predictedPlayerState.eFlags & EF_AIMING) && cg_fov.integer > 90 )
	{
		fovOffset = -0.2 * ( cg_fov.integer - 90 );
	}
	else
	{
		fovOffset = 0;
	}

	// Gordon: mounted gun drawing
	if ( ps->eFlags & EF_MOUNTEDTANK || ps->eFlags & EF_MG42_ACTIVE || ps->eFlags & EF_BROWNING_ACTIVE )
	{
		refEntity_t flash;
		vec3_t		adspos;
		int			i;
		memset( &hand, 0, sizeof(hand) );
		CG_CalculateWeaponPosition( hand.origin, angles );
		AnglesToAxis( angles, hand.axis );
		hand.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON | RF_MINLIGHT | RF_NOSHADOW;
		if
		(
			(
				cg_entities[cg_entities[cg_entities[ps->clientNum].tagParent].tankparent].currentState.density & 8 ||
				ps->eFlags & EF_BROWNING_ACTIVE
			) &&
			!(ps->eFlags & EF_MG42_ACTIVE)
		)
		{													// should we use a browning?
			adspos[0] = -5;
			adspos[1] = 10.4;
			adspos[2] = 8.4;
			hand.hModel = cgs.media.hMountedFPBrowning;
		}
		else
		{
			adspos[0] = 4;
			adspos[1] = 13.6;
			adspos[2] = 6.7;
			if ( ps->eFlags & EF_AIMING || ps->eFlags & EF_MG42_ACTIVE )
			{
				hand.hModel = cgs.media.hMountedFPMG42;
			}
			else
			{
				hand.hModel = cgs.media.hMountedFPMG42_2;
			}
		}

		gunoff[0] = cg_gun_x.value;
		gunoff[1] = cg_gun_y.value;
		gunoff[2] = cg_gun_z.value;
		for ( i = 0; i < 3; i++ )
		{													//1
			if ( ps->eFlags & EF_AIMING )
			{												//2
				f = ( cg.time - cg.zoomTime ) / (float) 300;
				if ( f > 1.0 )
				{											//3
					gunoff_w[i] = adspos[i];
				}
				else
				{											//3 4
					gunoff_w[i] = gunoff[i] + f * ( adspos[i] - gunoff[i] );
				}											//4
			}
			else
			{												// 2 5
				f = ( cg.time - cg.zoomTime ) / (float) 300;
				if ( f > 1.0 )
				{											//6
					gunoff_w[i] = 0;
				}
				else
				{											// 67
					gunoff_w[i] = adspos[i] + f * ( gunoff[i] - adspos[i] );
				}											//7
			}												//5
		}

		if ( !(ps->eFlags & EF_MOUNTEDTANK) )
		{
			gunoff[1] = adspos[1];
			if ( !ps->eFlags & EF_AIMING )
			{
				if ( ps->eFlags & EF_MG42_ACTIVE )
				{
					gunoff[0] = cg_gun_x.value + gunoff_w[0] - 8;
				}
				else
				{
					gunoff[0] = cg_gun_x.value + gunoff_w[0] - 18;
				}

				gunoff[2] = cg_gun_z.value + gunoff_w[2] + 5;
			}
			else
			{
				gunoff[2] = cg_gun_z.value + gunoff_w[2];
			}
		}
		else
		{
			gunoff[0] = cg_gun_x.value + gunoff_w[0];
			gunoff[1] = cg_gun_y.value + gunoff_w[1];
			gunoff[2] = cg_gun_z.value + gunoff_w[2];
		}

		if ( cg.time - cg.predictedPlayerEntity.muzzleFlashTime < MUZZLE_FLASH_TIME )
		{
			gunoff[0] += random() * 2.f;
		}

		VectorMA( hand.origin, gunoff[0] + 20, cg.refdef_current->viewaxis[0], hand.origin );
		VectorMA( hand.origin, gunoff[1] - 10, cg.refdef_current->viewaxis[1], hand.origin );
		VectorMA( hand.origin, gunoff[2] + (-8 + fovOffset), cg.refdef_current->viewaxis[2], hand.origin );
		CG_AddWeaponWithPowerups( &hand, cg.predictedPlayerEntity.currentState.powerups, ps, &cg.predictedPlayerEntity );
		if ( cg.time - cg.predictedPlayerEntity.overheatTime < 3000 )
		{
			if ( !(rand() % 3) )
			{
				float	alpha;
				alpha = 1.0f - ( (float) (cg.time - cg.predictedPlayerEntity.overheatTime) / 3000.0f );
				alpha *= 0.25f;								// .25 max alpha
				CG_ParticleImpactSmokePuffExtended( cgs.media.smokeParticleShader, cg.tankflashorg, 1000, 8, 20, 30,
													alpha, 8.f );
			}
		}
		{
			memset( &flash, 0, sizeof(flash) );
			flash.renderfx = ( RF_LIGHTING_ORIGIN | RF_DEPTHHACK | RF_NOSHADOW );
			flash.hModel = cgs.media.mg42muzzleflash;
			angles[YAW] = 0;
			angles[PITCH] = 0;
			angles[ROLL] = crandom() * 10;
			AnglesToAxis( angles, flash.axis );
			CG_PositionRotatedEntityOnTag( &flash, &hand, "tag_flash" );
			VectorMA( flash.origin, 22, flash.axis[0], flash.origin );
			VectorCopy( flash.origin, cg.tankflashorg );
			if ( cg.time - cg.predictedPlayerEntity.muzzleFlashTime < MUZZLE_FLASH_TIME )
			{
				trap_R_AddRefEntityToScene( &flash );
			}
		}

		return;
	}

	if ( ps->weapon > WP_NONE )
	{
		weapon = &cg_weapons[ps->weapon];
		memset( &hand, 0, sizeof(hand) );

		/* TODO ?
		WP_SMOKE_MARKER
		WP_LANDMINE
		WP_SATCHEL
		WP_SATCHEL_DET
		WP_SMOKE_BOMB
		WP_SILENCER	- just TODO		
		WP_SILENCED_COLT
		*/

		// set up gun position
		CG_CalculateWeaponPosition( hand.origin, angles );	// jaquboss

		// TODO ! WP_BAR , WP_MP44
		if ( cg_gun_realisticposition.value > 0 )
		{
			if
			(
				ps->weapon == WP_MP40 ||
				ps->weapon == WP_THOMPSON ||
				ps->weapon == WP_FG42 ||
				ps->weapon == WP_MP44 ||
				ps->weapon == WP_BAR
			)
			{
				gunpos[0] = 0;
				gunpos[1] = 5;
				gunpos[2] = 0;
			}
			else if ( ps->weapon == WP_STEN )
			{
				gunpos[0] = 10;
				gunpos[1] = 5;
				gunpos[2] = 0;
			}
			else if ( ps->weapon == WP_KNIFE )
			{
				gunpos[0] = 15;
				gunpos[1] = -3;
				gunpos[2] = -2;
			}
			else if ( ps->weapon == WP_FLAMETHROWER )
			{
				gunpos[0] = 5;
				gunpos[1] = 3;
				gunpos[2] = -2;
			}
			else if ( ps->weapon == WP_MOBILE_MG42 )
			{
				gunpos[0] = 5;
				gunpos[1] = 7;
				gunpos[2] = -5;
			}
			else if ( ps->weapon == WP_MOBILE_MG42_SET )
			{
				gunpos[0] = 20;
				gunpos[1] = 0;
				gunpos[2] = 0;
			}
			else if ( ps->weapon == WP_30CAL_SET )
			{
				gunpos[0] = 25;
				gunpos[1] = 0;
				gunpos[2] = 0;
			}
			else if ( ps->weapon == WP_MORTAR )
			{
				gunpos[0] = -2;
				gunpos[1] = -3;
				gunpos[2] = -5;
			}
			else if ( ps->weapon == WP_PANZERFAUST )
			{
				gunpos[0] = -1;
				gunpos[1] = -1;
				gunpos[2] = 3;
			}
			else if ( ps->weapon == WP_K43 )
			{
				gunpos[0] = 15;
				gunpos[1] = 7;
				gunpos[2] = 0;
			}
			else if ( ps->weapon == WP_GPG40 || ps->weapon == WP_KAR98 || ps->weapon == WP_G43 )
			{
				gunpos[0] = 15;
				gunpos[1] = 5;
				gunpos[2] = 0;
			}
			else if ( ps->weapon == WP_CARBINE || ps->weapon == WP_M7 || ps->weapon == WP_GARAND )
			{
				gunpos[0] = 0;
				gunpos[1] = 3;
				gunpos[2] = 0;
			}
			else if ( ps->weapon == WP_COLT /*|| ps->weapon == WP_SILENCED_COLT*/ )
			{												// TODO redo silenced colt anims and enabl it
				gunpos[0] = 10 /*15*/ ;
				gunpos[1] = 0;
				gunpos[2] = 0;
			}
			else
			{
				gunpos[0] = 0;
				gunpos[1] = 0;
				gunpos[2] = 0;
			}
		}
		else
		{
			gunpos[0] = 0;
			gunpos[1] = 0;
			gunpos[2] = 0;
		}

		//#define ADS_DELAY 250
#define ADS_DELAY	300
		gunoff[0] = cg_gun_x.value;
		gunoff[1] = cg_gun_y.value;
		gunoff[2] = cg_gun_z.value;
		if ( ps->eFlags & EF_AIMING )
		{
			if
			(
				(
					ps->weapon == WP_COLT ||
					ps->weapon == WP_30CAL_SET ||
					ps->weapon == WP_MOBILE_MG42_SET ||
					ps->weapon == WP_GPG40 ||
					ps->weapon == WP_G43 ||
					ps->weapon == WP_KAR98
				) &&
				cg_gun_realisticposition.integer > 0
			)
			{
				gunx = CG_GetAds( ps->weapon, 0 );
			}
			else
			{
				gunx = CG_GetAds( ps->weapon, 0 ) - gunpos[0];
			}

			f = ( cg.time - cg.zoomTime ) / (float) ADS_DELAY;
			if ( f > 1.0 )
			{
				gunoff_w[0] = gunx + cg_gun_x.value;
			}
			else
			{
				gunoff_w[0] = gunoff[0] + f * ( gunx - gunoff[0] ) + cg_gun_x.value;
			}
		}
		else
		{
			if
			(
				(
					ps->weapon == WP_30CAL_SET ||
					ps->weapon == WP_MOBILE_MG42_SET ||
					ps->weapon == WP_GPG40 ||
					ps->weapon == WP_G43 ||
					ps->weapon == WP_KAR98
				) &&
				cg_gun_realisticposition.integer > 0
			)
			{
				gunx = CG_GetAds( ps->weapon, 0 );
			}
			else
			{
				gunx = CG_GetAds( ps->weapon, 0 ) - gunpos[0];
			}

			f = ( cg.time - cg.zoomTime ) / (float) ADS_DELAY;
			if ( f > 1.0 )
			{
				gunoff_w[0] = 0 + cg_gun_x.value;
			}
			else
			{
				gunoff_w[0] = gunx + f * ( gunoff[0] - gunx ) + cg_gun_x.value;
			}
		}

		if ( ps->eFlags & EF_AIMING )
		{
			guny = CG_GetAds( ps->weapon, 1 ) - gunpos[1];
			f = ( cg.time - cg.zoomTime ) / (float) ADS_DELAY;
			if ( f > 1.0 )
			{
				gunoff_w[1] = guny + cg_gun_y.value;
			}
			else
			{
				gunoff_w[1] = gunoff[1] + f * ( guny - gunoff[1] ) + cg_gun_y.value;
			}
		}
		else
		{
			guny = CG_GetAds( ps->weapon, 1 ) - gunpos[1];
			f = ( cg.time - cg.zoomTime ) / (float) ADS_DELAY;
			if ( f > 1.0 )
			{
				gunoff_w[1] = 0 + cg_gun_y.value;
			}
			else
			{
				gunoff_w[1] = guny + f * ( gunoff[1] - guny ) + cg_gun_y.value;
			}
		}

		if ( ps->eFlags & EF_AIMING )
		{
			gunz = CG_GetAds( ps->weapon, 2 ) - gunpos[2];
			f = ( cg.time - cg.zoomTime ) / (float) ADS_DELAY;
			if ( f > 1.0 )
			{
				gunoff_w[2] = gunz + cg_gun_z.value;
			}
			else
			{
				gunoff_w[2] = gunoff[2] + f * ( gunz - gunoff[2] ) + cg_gun_z.value;
			}
		}
		else
		{
			gunz = CG_GetAds( ps->weapon, 2 ) - gunpos[2];
			f = ( cg.time - cg.zoomTime ) / (float) ADS_DELAY;
			if ( f > 1.0 )
			{

				//gunoff[2] = gunoff[2];
				gunoff_w[2] = 0 + cg_gun_z.value;
			}
			else
			{
				gunoff_w[2] = gunz + f * ( gunoff[2] - gunz ) + cg_gun_z.value;
			}
		}

		gunoff[0] = gunoff_w[0] + gunpos[0];
		gunoff[1] = gunoff_w[1] + gunpos[1];
		gunoff[2] = gunoff_w[2] + gunpos[2];
		if
		(
			gunoff[0] == CG_GetAds( ps->weapon, 0) &&
			gunoff[1] == CG_GetAds( ps->weapon, 1) &&
			gunoff[2] == CG_GetAds( ps->weapon, 2)
		)
		{
			if ( BG_IsScopedWeapon2( ps->weapon) && cg.predictedPlayerState.eFlags & EF_AIMING )
			{
				cg.zoomedSniper = qtrue;
				CG_SetSniperZoom();
			}
		}
		else
		{
			cg.zoomedSniper = qfalse;
		}

		if ( !(cg.predictedPlayerState.eFlags & EF_AIMING) )
		{
			cg.zoomedSniper = qfalse;
		}

		VectorMA( hand.origin, gunoff[0], cg.refdef_current->viewaxis[0], hand.origin );
		VectorMA( hand.origin, gunoff[1], cg.refdef_current->viewaxis[1], hand.origin );
		VectorMA( hand.origin, (gunoff[2] + fovOffset), cg.refdef_current->viewaxis[2], hand.origin );
		AnglesToAxis( angles, hand.axis );
		if ( cg_gun_frame.integer )
		{
			hand.frame = hand.oldframe = cg_gun_frame.integer;
			hand.backlerp = 0;
		}
		else
		{													// get the animation state
			if ( cg.binocZoomTime )
			{
				if ( cg.binocZoomTime < 0 )
				{
					if ( -cg.binocZoomTime + 500 + 200 < cg.time )
					{
						cg.binocZoomTime = 0;
					}
					else
					{
						if ( -cg.binocZoomTime + 200 < cg.time )
						{
							CG_ContinueWeaponAnim( WEAP_ALTSWITCHFROM );
						}
						else
						{
							CG_ContinueWeaponAnim( WEAP_IDLE2 );
						}
					}
				}
				else
				{
					if ( cg.binocZoomTime + 500 < cg.time )
					{
						trap_SendConsoleCommand( "+zoom\n" );
						cg.binocZoomTime = 0;
						CG_ContinueWeaponAnim( WEAP_IDLE2 );
					}
					else
					{
						CG_ContinueWeaponAnim( WEAP_ALTSWITCHTO );
					}
				}
			}

			CG_WeaponAnimation( ps, weapon, &hand.oldframe, &hand.frame, &hand.backlerp );	//----(SA)	changed
		}

		if ( cg.predictedPlayerState.eFlags & EF_AIMING && weapon->adshandsModel )
		{
			hand.hModel = weapon->adshandsModel;
		}
		else
		{
			hand.hModel = weapon->handsModel;
		}

		hand.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON | RF_MINLIGHT | RF_NOSHADOW;			//----(SA)	

		// add everything onto the hand
		CG_AddPlayerWeapon( &hand, ps, &cg.predictedPlayerEntity, qfalse );

		// Ridah
	}

	// Rafael
	// add the foot
	// RTCW CODE
	CG_AddPlayerFoot( &hand, ps, &cg.predictedPlayerEntity );
}

/*
==============================================================================

WEAPON SELECTION

==============================================================================
*/
#define WP_ICON_X		38					// new sizes per MK
#define WP_ICON_X_WIDE	72					// new sizes per MK
#define WP_ICON_Y		38
#define WP_ICON_SPACE_Y 10
#define WP_DRAW_X		640 - WP_ICON_X - 4 // 4 is 'selected' border width		
#define WP_DRAW_X_WIDE	640 - WP_ICON_X_WIDE - 4
#define WP_DRAW_Y		4

// secondary fire icons
#define WP_ICON_SEC_X	18					// new sizes per MK
#define WP_ICON_SEC_Y	18


/*
==============
CG_WeaponHasAmmo
	check for ammo
==============
*/
static qboolean
CG_WeaponHasAmmo ( int i )
{

	// ydnar: certain weapons don't have ammo
	if ( i == WP_KNIFE || i == WP_PLIERS )
	{
		return ( qtrue );
	}

	if
	(
		!(cg.predictedPlayerState.ammo[BG_FindAmmoForWeapon( i)]) &&
		!(cg.predictedPlayerState.ammoclip[BG_FindClipForWeapon( i)])
	)
	{
		return ( qfalse );
	}

	return ( qtrue );
}


/*
===============
CG_WeaponSelectable
===============
*/
qboolean
CG_WeaponSelectable ( int i )
{

	// allow the player to unselect all weapons
	if ( i == WP_NONE )
	{
		return ( qtrue );
	}

	// if holding a melee weapon (chair/shield/etc.) only allow single-handed weapons

	/*	if(cg.snap->ps.eFlags & EF_MELEE_ACTIVE) {
		if(!(WEAPS_ONE_HANDED & (1<<i)))
			return qfalse;
	}*/
	if ( BG_PlayerMounted( cg.predictedPlayerState.eFlags) )
	{
		return ( qfalse );
	}

	// check for weapon
	if ( !(COM_BitCheck( cg.predictedPlayerState.weapons, i)) )
	{
		return ( qfalse );
	}

	if ( !CG_WeaponHasAmmo( i) )
	{
		return ( qfalse );
	}

	return ( qtrue );
}


/*
==============
CG_WeaponIndex
==============
*/
int
CG_WeaponIndex ( int weapnum, int *bank, int *cycle )
{
	static int	bnk, cyc;
	if ( weapnum <= 0 || weapnum >= WP_NUM_WEAPONS )
	{
		if ( bank )
		{
			*bank = 0;
		}

		if ( cycle )
		{
			*cycle = 0;
		}

		return ( 0 );
	}

	for ( bnk = 0; bnk < MAX_WEAP_BANKS_MP; bnk++ )
	{
		for ( cyc = 0; cyc < MAX_WEAPS_IN_BANK_MP; cyc++ )
		{
			if ( !weapBanksMultiPlayer[bnk][cyc] )
			{
				break;
			}

			// found the current weapon
			if ( weapBanksMultiPlayer[bnk][cyc] == weapnum )
			{
				if ( bank )
				{
					*bank = bnk;
				}

				if ( cycle )
				{
					*cycle = cyc;
				}

				return ( 1 );
			}

			// jpw
		}
	}

	// failed to find the weapon in the table
	// probably an alternate
	return ( 0 );
}


/*
==============
getNextWeapInBank
	Pass in a bank and cycle and this will return the next valid weapon higher in the cycle.
	if the weap passed in is above highest in a cycle (MAX_WEAPS_IN_BANK), this will safely loop around
==============
*/
static int
getNextWeapInBank ( int bank, int cycle )
{
	cycle++;
	cycle = cycle % MAX_WEAPS_IN_BANK_MP;
	if ( weapBanksMultiPlayer[bank][cycle] )
	{	// return next weapon in bank if there is one
		return ( weapBanksMultiPlayer[bank][cycle] );
	}
	else
	{	// return first in bank
		return ( weapBanksMultiPlayer[bank][0] );
	}
}


/* */
static int
getNextWeapInBankBynum ( int weapnum )
{
	int bank, cycle;
	if ( !CG_WeaponIndex( weapnum, &bank, &cycle) )
	{
		return ( weapnum );
	}

	return ( getNextWeapInBank( bank, cycle) );
}


/*
==============
getPrevWeapInBank
	Pass in a bank and cycle and this will return the next valid weapon lower in the cycle.
	if the weap passed in is the lowest in a cycle (0), this will loop around to the
	top (MAX_WEAPS_IN_BANK-1) and start down from there looking for a valid weapon position
==============
*/
static int
getPrevWeapInBank ( int bank, int cycle )
{
	cycle--;
	if ( cycle < 0 )
	{
		cycle = MAX_WEAPS_IN_BANK_MP - 1;
	}

	while ( !weapBanksMultiPlayer[bank][cycle] )
	{
		cycle--;
		if ( cycle < 0 )
		{
			cycle = MAX_WEAPS_IN_BANK_MP - 1;
		}
	}

	return ( weapBanksMultiPlayer[bank][cycle] );
}


/* */
static int
getPrevWeapInBankBynum ( int weapnum )
{
	int bank, cycle;
	if ( !CG_WeaponIndex( weapnum, &bank, &cycle) )
	{
		return ( weapnum );
	}

	return ( getPrevWeapInBank( bank, cycle) );
}


/*
==============
getNextBankWeap
	Pass in a bank and cycle and this will return the next valid weapon in a higher bank.
	sameBankPosition: if there's a weapon in the next bank at the same cycle,
	return that	(colt returns thompson for example) rather than the lowest weapon
==============
*/
static int
getNextBankWeap ( int bank, int cycle, qboolean sameBankPosition )
{
	bank++;
	bank = bank % MAX_WEAP_BANKS_MP;
	if ( sameBankPosition && weapBanksMultiPlayer[bank][cycle] )
	{
		return ( weapBanksMultiPlayer[bank][cycle] );
	}
	else
	{
		return ( weapBanksMultiPlayer[bank][0] );
	}
}


/*
==============
getPrevBankWeap
	Pass in a bank and cycle and this will return the next valid weapon in a lower bank.
	sameBankPosition: if there's a weapon in the prev bank at the same cycle,
	return that	(thompson returns colt for example) rather than the highest weapon
==============
*/
static int
getPrevBankWeap ( int bank, int cycle, qboolean sameBankPosition )
{
	int i;
	bank--;
	if ( bank < 0 )
	{								// don't go below 0, cycle up to top
		bank += MAX_WEAP_BANKS_MP;	// JPW NERVE
	}

	bank = bank % MAX_WEAP_BANKS_MP;
	if ( sameBankPosition && weapBanksMultiPlayer[bank][cycle] )
	{
		return ( weapBanksMultiPlayer[bank][cycle] );
	}
	else
	{								// find highest weap in bank
		for ( i = MAX_WEAPS_IN_BANK_MP - 1; i >= 0; i-- )
		{
			if ( weapBanksMultiPlayer[bank][i] )
			{
				return ( weapBanksMultiPlayer[bank][i] );
			}
		}

		// if it gets to here, no valid weaps in this bank, go down another bank
		return ( getPrevBankWeap( bank, cycle, sameBankPosition) );
	}
}


/*
==============
getAltWeapon
==============
*/
static int
getAltWeapon ( int weapnum )
{
	if ( weapAlts[weapnum] )
	{
		return ( weapAlts[weapnum] );
	}

	return ( weapnum );
}


/*
==============
getEquivWeapon
	return the id of the opposite team's weapon.
	Passing the weapnum of the mp40 returns the id of the thompson, and likewise
	passing the weapnum of the thompson returns the id of the mp40.
	No equivalent available will return the weapnum passed in.
==============
*/
int
getEquivWeapon ( int weapnum )
{
	int num = weapnum;
	switch ( weapnum )
	{

		// going from german to american
		case WP_LUGER:
			num = WP_COLT;
			break;
		case WP_MP40:
			num = WP_THOMPSON;
			break;
		case WP_GRENADE_LAUNCHER:
			num = WP_GRENADE_PINEAPPLE;
			break;
		case WP_KAR98:
			num = WP_CARBINE;
			break;
		case WP_SILENCER:
			num = WP_SILENCED_COLT;
			break;
		case WP_G43:
			num = WP_M1GARAND;
			break;
	#ifdef NWEAPS
		case WP_MP44:
			num = WP_BAR;
			break;	// hmm this is weird..
	#endif

		// going from american to german
		case WP_COLT:
			num = WP_LUGER;
			break;
		case WP_THOMPSON:
			num = WP_MP40;
			break;
		case WP_GRENADE_PINEAPPLE:
			num = WP_GRENADE_LAUNCHER;
			break;
		case WP_CARBINE:
			num = WP_KAR98;
			break;
		case WP_SILENCED_COLT:
			num = WP_SILENCER;
			break;
		case WP_M1GARAND:
			num = WP_G43;
			break;
	#ifdef NWEAPS
		case WP_BAR:
			num = WP_MP44;
			break;	// hmm this is weird..
	#endif
	}

	return ( num );
}


/*
==============
CG_PlaySwitchSound
	Get special switching sounds if they're there
==============
*/
void
CG_PlaySwitchSound ( int lastweap, int newweap )
{
	sfxHandle_t switchsound;
	switchsound = cgs.media.selectSound;
	if ( getAltWeapon( lastweap) == newweap )
	{	// alt switch
		switch ( newweap )
		{
			case WP_SILENCER:
			case WP_LUGER:
			case WP_SILENCED_COLT:
			case WP_COLT:
			case WP_GPG40:
			case WP_M7:
			case WP_MORTAR:
			case WP_MORTAR_SET:
			case WP_MOBILE_MG42:
			case WP_MOBILE_MG42_SET:
			case WP_30CAL_SET:
			case WP_30CAL:
				switchsound = cg_weapons[newweap].switchSound;
				break;
			case WP_CARBINE:
			case WP_KAR98:
				if ( cg.predictedPlayerState.ammoclip[lastweap] )
				{
					switchsound = cg_weapons[newweap].switchSound;
				}
				break;
			default:
				return;
		}
	}
	else
	{
		return;
	}

	trap_S_StartSound( NULL, cg.snap->ps.clientNum, CHAN_WEAPON, switchsound );
}


/*
==============
CG_FinishWeaponChange
==============
*/
void
CG_FinishWeaponChange ( int lastweap, int newweap )
{
	int newbank;
	if ( cg.predictedPlayerState.pm_flags & PMF_LADDER )
	{
		return;
	}

	if ( cg.binocZoomTime )
	{
		return;
	}

	if ( newweap != WP_NONE )
	{

	}

	cg.mortarImpactTime = -2;
	switch ( newweap )
	{
		case WP_LUGER:
			if ( (cg.pmext.silencedSideArm & 1) && lastweap != WP_SILENCER )
			{
				newweap = WP_SILENCER;
				cg.weaponSelect = newweap;
				cg.weaponSelect_last = newweap;
			}
			break;
		case WP_SILENCER:
			if ( !(cg.pmext.silencedSideArm & 1) && lastweap != WP_LUGER )
			{
				newweap = WP_LUGER;
				cg.weaponSelect = newweap;
				cg.weaponSelect_last = newweap;
			}
			break;
		case WP_COLT:
			if ( (cg.pmext.silencedSideArm & 1) && lastweap != WP_SILENCED_COLT )
			{
				newweap = WP_SILENCED_COLT;
				cg.weaponSelect = newweap;
				cg.weaponSelect_last = newweap;
			}
			break;
		case WP_SILENCED_COLT:
			if ( !(cg.pmext.silencedSideArm & 1) && lastweap != WP_COLT )
			{
				newweap = WP_COLT;
				cg.weaponSelect = newweap;
				cg.weaponSelect_last = newweap;
			}
			break;
		case WP_CARBINE:
			if ( (cg.pmext.silencedSideArm & 2) && lastweap != WP_M7 )
			{
				newweap = WP_M7;
				cg.weaponSelect = newweap;
				cg.weaponSelect_last = newweap;
			}
			break;
		case WP_M7:
			if ( !(cg.pmext.silencedSideArm & 2) && lastweap != WP_CARBINE )
			{
				newweap = WP_CARBINE;
				cg.weaponSelect = newweap;
				cg.weaponSelect_last = newweap;
			}
			break;
		case WP_KAR98:
			if ( (cg.pmext.silencedSideArm & 2) && lastweap != WP_GPG40 )
			{
				newweap = WP_GPG40;
				cg.weaponSelect = newweap;
				cg.weaponSelect_last = newweap;
			}
			break;
		case WP_GPG40:
			if ( !(cg.pmext.silencedSideArm & 2) && lastweap != WP_KAR98 )
			{
				newweap = WP_KAR98;
				cg.weaponSelect = newweap;
				cg.weaponSelect_last = newweap;
			}
			break;
	}

	if ( lastweap == WP_BINOCULARS && cg.snap->ps.eFlags & EF_ZOOMING )
	{
		trap_SendConsoleCommand( "-zoom\n" );
	}

	cg.weaponSelectTime = cg.time;				// flash the weapon icon

	// NERVE - SMF
	if ( cg.newCrosshairIndex )
	{
		trap_Cvar_Set( "cg_drawCrossHair", va( "%d", cg.newCrosshairIndex - 1) );
	}

	cg.newCrosshairIndex = 0;

	// -NERVE - SMF
	// remember which weapon in this bank was last selected so when cycling back
	// to this bank, that weap will be highlighted first
	if ( CG_WeaponIndex( newweap, &newbank, NULL) )
	{
		cg.lastWeapSelInBank[newbank] = newweap;
	}

	if ( lastweap == newweap )
	{											// no need to do any more than flash the icon
		return;
	}

	CG_PlaySwitchSound( lastweap, newweap );	// Gordon: grabbed from SP

	//	CG_SetSniperZoom(lastweap, newweap);
	// setup for a user call to CG_LastWeaponUsed_f()
	if ( lastweap == cg.lastFiredWeapon )
	{

		// don't set switchback for some weaps...
		switch ( lastweap )
		{
			case WP_FG42SCOPE:
			case WP_GARAND_SCOPE:
			case WP_K43_SCOPE:
				break;
			default:
				cg.switchbackWeapon = lastweap;
				break;
		}
	}
	else
	{

		// if this ended up having the switchback be the same
		// as the new weapon, set the switchback to the prev
		// selected weapon will become the switchback
		if ( cg.switchbackWeapon == newweap )
		{
			cg.switchbackWeapon = lastweap;
		}
	}

	cg.weaponSelect = newweap;
	if ( newweap != WP_NONE )
	{
		cg.weaponSelect_last = newweap;
	}
}

extern pmove_t	cg_pmove;


/*
==============
CG_AltfireWeapon_f
	for example, switching between WP_MAUSER and WP_SNIPERRIFLE
==============
*/
void
CG_AltWeapon_f ( void )
{
	int original, num;
	if ( !cg.snap )
	{
		return;
	}

	// Overload for spec mode when following
	if ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.mvTotalClients > 0 )
	{

		//CG_toggleSwing_f();
		return;
	}

	if ( cg.weaponSelect == WP_GARAND || cg.weaponSelect == WP_K43 )
	{
		if ( cg.thermal )
		{
			cg.thermal = qfalse;
		}
		else
		{
			cg.thermal = qtrue;
		}
	}

	// Need ground for this
	if ( cg.weaponSelect == WP_MORTAR )
	{
		int		contents;
		vec3_t	point;
		if ( cg.predictedPlayerState.groundEntityNum == ENTITYNUM_NONE )
		{
			return;
		}

		if ( !cg.predictedPlayerState.ammoclip[WP_MORTAR] )
		{
			return;
		}

		if ( cg.predictedPlayerState.eFlags & EF_PRONE )
		{
			return;
		}

		if ( cg_pmove.waterlevel == 3 )
		{
			return;
		}

		// ydnar: don't allow set if moving
		if ( VectorLengthSquared( cg.snap->ps.velocity) )
		{
			return;
		}

		// eurgh, need it here too else we play sounds :/
		point[0] = cg.snap->ps.origin[0];
		point[1] = cg.snap->ps.origin[1];
		point[2] = cg.snap->ps.origin[2] + cg.snap->ps.crouchViewHeight;
		contents = CG_PointContents( point, cg.snap->ps.clientNum );
		if ( contents & MASK_WATER )
		{
			return;
		}

		/*	} else if( cg.weaponSelect == WP_MOBILE_MG42 || cg.weaponSelect == WP_30CAL ) {
		if( !(cg.predictedPlayerState.eFlags & EF_PRONE) ) {
			return;
		}
	}*/
	}
	else if ( cg.weaponSelect == WP_MOBILE_MG42 || cg.weaponSelect == WP_30CAL )
	{
		qboolean	allowed = qfalse;
		trace_t		trace;
		trace_t		trace2;
		vec3_t		playerOrigin, newOrigin, oldOrigin, forward, right, up;
		VectorCopy( cg.predictedPlayerState.origin, oldOrigin );
		VectorCopy( cg.predictedPlayerState.origin, newOrigin );
		VectorCopy( cg.predictedPlayerState.origin, playerOrigin );
		AngleVectors( cg.predictedPlayerState.viewangles, forward, right, up );
		VectorMA( oldOrigin, 19, forward, oldOrigin );
		VectorMA( newOrigin, 19, forward, newOrigin );
		VectorMA( oldOrigin, cg.predictedPlayerState.viewheight - 5, up, oldOrigin );
		VectorMA( playerOrigin, cg.predictedPlayerState.viewheight - 5, up, playerOrigin );
		VectorMA( newOrigin, cg.predictedPlayerState.viewheight - 15, up, newOrigin );
		CG_Trace( &trace, oldOrigin, NULL, NULL, newOrigin, -1, CONTENTS_SOLID );
		CG_Trace( &trace2, playerOrigin, NULL, NULL, oldOrigin, -1, CONTENTS_SOLID );
		if ( cg.predictedPlayerState.eFlags & EF_PRONE || (trace2.fraction == 1 && trace.fraction < 0.35) )
		{
			allowed = qtrue;
		}

		/*
		CG_Printf( "1:%f \n", trace.fraction );
		CG_Printf( "2:%f \n", trace2.fraction );

		CG_RailTrail( &cgs.clientinfo[cg.predictedPlayerState.clientNum], oldOrigin, trace.endpos, cg.predictedPlayerEntity.currentState.dmgFlags);
		CG_RailTrail( &cgs.clientinfo[cg.predictedPlayerState.clientNum], playerOrigin, trace2.endpos, cg.predictedPlayerEntity.currentState.dmgFlags);
*/
		if ( !(allowed) )
		{
			return;
		}
	}

	if ( cg.time - cg.weaponSelectTime < cg_weaponCycleDelay.integer )
	{
		return; // force pause so holding it down won't go too fast
	}

	// Don't try to switch when in the middle of reloading.
	if ( cg.snap->ps.weaponstate == WEAPON_RELOADING )
	{
		return;
	}

	original = cg.weaponSelect;
	num = getAltWeapon( original );
	if ( original == WP_BINOCULARS )
	{
		if ( cg.snap->ps.eFlags & EF_ZOOMING )
		{
			trap_SendConsoleCommand( "-zoom\n" );
			cg.binocZoomTime = -cg.time;
		}
		else
		{
			if ( !cg.binocZoomTime )
			{
				cg.binocZoomTime = cg.time;
			}
		}
	}

	// Arnout: don't allow another weapon switch when we're still swapping the gpg40, to prevent animation breaking
	if
	(
		(cg.snap->ps.weaponstate == WEAPON_RAISING || cg.snap->ps.weaponstate == WEAPON_DROPPING) &&
		(
			(original == WP_GPG40 || num == WP_GPG40 || original == WP_M7 || num == WP_M7) ||
			(
			original == WP_SILENCER ||
			num == WP_SILENCER ||
			original == WP_SILENCED_COLT ||
			num == WP_SILENCED_COLT
			) ||
				(
					original == WP_AKIMBO_SILENCEDCOLT ||
					num == WP_AKIMBO_SILENCEDCOLT ||
					original == WP_AKIMBO_SILENCEDLUGER ||
					num == WP_AKIMBO_SILENCEDLUGER
				) ||
				(original == WP_MORTAR_SET || num == WP_MORTAR_SET) ||
				(original == WP_30CAL_SET || num == WP_30CAL_SET) ||
				(original == WP_MOBILE_MG42_SET || num == WP_MOBILE_MG42_SET)
		)
	)
	{
		return;
	}

	if ( CG_WeaponSelectable( num) )
	{			// new weapon is valid
		CG_FinishWeaponChange( original, num );
	}
}


/*
==============
CG_NextWeap

  switchBanks - curweap is the last in a bank, 'qtrue' means go to the next available bank, 'qfalse' means loop to the head of the bank
==============
*/
void
CG_NextWeap ( qboolean switchBanks )
{
	int			bank = 0, cycle = 0, newbank = 0, newcycle = 0;
	int			num, curweap;
	qboolean	nextbank = qfalse;									// need to switch to the next bank of weapons?
	int			i, j;
	num = curweap = cg.weaponSelect;
	if ( curweap == WP_MORTAR_SET || curweap == WP_MOBILE_MG42_SET || curweap == WP_30CAL_SET )
	{
		return;
	}

	switch ( num )
	{
		case WP_SILENCER:
			curweap = num = WP_LUGER;
			break;
		case WP_SILENCED_COLT:
			curweap = num = WP_COLT;
			break;
		case WP_GPG40:
			curweap = num = WP_KAR98;
			break;
		case WP_M7:
			curweap = num = WP_CARBINE;
			break;
		case WP_MORTAR_SET:
			curweap = num = WP_MORTAR;
			break;
	}

	CG_WeaponIndex( curweap, &bank, &cycle );						// get bank/cycle of current weapon

	// if you're using an alt mode weapon, try switching back to the parent first
	if ( curweap >= WP_BEGINSECONDARY && curweap <= WP_LASTSECONDARY )
	{
		num = getAltWeapon( curweap );								// base any further changes on the parent
		if ( CG_WeaponSelectable( num) )
		{															// the parent was selectable, drop back to that
			CG_FinishWeaponChange( curweap, num );
			return;
		}
	}

	if ( cg_cycleAllWeaps.integer || !switchBanks )
	{
		for ( i = 0; i < MAX_WEAPS_IN_BANK_MP; i++ )
		{
			num = getNextWeapInBankBynum( num );
			CG_WeaponIndex( num, NULL, &newcycle );					// get cycle of new weapon.  if it's lower than the original, then it cycled around
			if ( switchBanks )
			{
				if ( newcycle <= cycle )
				{
					nextbank = qtrue;
					break;
				}
			}
			else
			{														// don't switch banks if you get to the end
				if ( num == curweap )
				{													// back to start, just leave it where it is
					return;
				}
			}

			if ( CG_WeaponSelectable( num) )
			{
				break;
			}
			else
			{
				qboolean	found = qfalse;
				switch ( num )
				{
					case WP_CARBINE:
						if ( (found = CG_WeaponSelectable( WP_M7)) )
						{
							num = WP_M7;
						}
						break;
					case WP_KAR98:
						if ( (found = CG_WeaponSelectable( WP_GPG40)) )
						{
							num = WP_GPG40;
						}
						break;
				}

				if ( found )
				{
					break;
				}
			}
		}
	}
	else
	{
		nextbank = qtrue;
	}

	if ( nextbank )
	{
		for ( i = 0; i < MAX_WEAP_BANKS_MP; i++ )
		{
			if ( cg_cycleAllWeaps.integer )
			{
				num = getNextBankWeap( bank + i, cycle, qfalse );	// cycling all weaps always starts the next bank at the bottom
			}
			else
			{
				if ( cg.lastWeapSelInBank[bank + i + 1] )
				{
					num = cg.lastWeapSelInBank[bank + i + 1];
				}
				else
				{
					num = getNextBankWeap( bank + i, cycle, qtrue );
				}
			}

			if ( num == 0 )
			{
				continue;
			}

			//                      if(num == WP_BINOCULARS) {
			//                              continue;
			//                      }
			if ( CG_WeaponSelectable( num) )
			{														// first entry in bank was selectable, no need to scan the bank
				break;
			}
			else
			{
				qboolean	found = qfalse;
				switch ( num )
				{
					case WP_CARBINE:
						if ( (found = CG_WeaponSelectable( WP_M7)) )
						{
							num = WP_M7;
						}
						break;
					case WP_KAR98:
						if ( (found = CG_WeaponSelectable( WP_GPG40)) )
						{
							num = WP_GPG40;
						}
						break;
				}

				if ( found )
				{
					break;
				}
			}

			CG_WeaponIndex( num, &newbank, &newcycle );				// get the bank of the new weap
			for ( j = newcycle; j < MAX_WEAPS_IN_BANK_MP; j++ )
			{
				num = getNextWeapInBank( newbank, j );

				/*                              if(num == WP_BINOCULARS) {
                                        continue;
                                }*/
				if ( CG_WeaponSelectable( num) )
				{													// found selectable weapon
					break;
				}
				else
				{
					qboolean	found = qfalse;
					switch ( num )
					{
						case WP_CARBINE:
							if ( (found = CG_WeaponSelectable( WP_M7)) )
							{
								num = WP_M7;
							}
							break;
						case WP_KAR98:
							if ( (found = CG_WeaponSelectable( WP_GPG40)) )
							{
								num = WP_GPG40;
							}
							break;
					}

					if ( found )
					{
						break;
					}
				}

				num = 0;
			}

			if ( num )
			{														// a selectable weapon was found in the current bank
				break;
			}
		}
	}

	CG_FinishWeaponChange( curweap, num );							//----(SA)
}

/*
==============
CG_PrevWeap

  switchBanks - curweap is the last in a bank
		'qtrue'  - go to the next available bank
		'qfalse' - loop to the head of the bank
==============
*/


/*
==============
CG_PrevWeap

  switchBanks - curweap is the last in a bank
                'qtrue'  - go to the next available bank
                'qfalse' - loop to the head of the bank
==============
*/
void
CG_PrevWeap ( qboolean switchBanks )
{
	int			bank = 0, cycle = 0, newbank = 0, newcycle = 0;
	int			num, curweap;
	qboolean	prevbank = qfalse;									// need to switch to the next bank of weapons?
	int			i, j;
	num = curweap = cg.weaponSelect;
	if ( curweap == WP_MORTAR_SET || curweap == WP_MOBILE_MG42_SET || curweap == WP_30CAL_SET )
	{
		return;
	}

	switch ( num )
	{
		case WP_SILENCER:
			curweap = num = WP_LUGER;
			break;
		case WP_SILENCED_COLT:
			curweap = num = WP_COLT;
			break;
		case WP_GPG40:
			curweap = num = WP_KAR98;
			break;
		case WP_M7:
			curweap = num = WP_CARBINE;
			break;
		case WP_MORTAR_SET:
			curweap = num = WP_MORTAR;
			break;
	}

	CG_WeaponIndex( curweap, &bank, &cycle );						// get bank/cycle of current weapon

	// if you're using an alt mode weapon, try switching back to the parent first
	if ( curweap >= WP_BEGINSECONDARY && curweap <= WP_LASTSECONDARY )
	{
		num = getAltWeapon( curweap );								// base any further changes on the parent
		if ( CG_WeaponSelectable( num) )
		{															// the parent was selectable, drop back to that
			CG_FinishWeaponChange( curweap, num );
			return;
		}
	}

	// initially, just try to find a lower weapon in the current bank
	if ( cg_cycleAllWeaps.integer || !switchBanks )
	{
		for ( i = cycle; i >= 0; i-- )
		{
			num = getPrevWeapInBankBynum( num );
			CG_WeaponIndex( num, NULL, &newcycle );					// get cycle of new weapon.  if it's greater than the original, then it cycled around
			if ( switchBanks )
			{
				if ( newcycle > (cycle - 1) )
				{
					prevbank = qtrue;
					break;
				}
			}
			else
			{														// don't switch banks if you get to the end
				if ( num == curweap )
				{													// back to start, just leave it where it is
					return;
				}
			}

			//                              if(num == WP_BINOCULARS) {
			//                                      continue;
			//                              }
			if ( CG_WeaponSelectable( num) )
			{
				break;
			}
			else
			{
				qboolean	found = qfalse;
				switch ( num )
				{
					case WP_CARBINE:
						if ( (found = CG_WeaponSelectable( WP_M7)) )
						{
							num = WP_M7;
						}
						break;
					case WP_KAR98:
						if ( (found = CG_WeaponSelectable( WP_GPG40)) )
						{
							num = WP_GPG40;
						}
						break;
				}

				if ( found )
				{
					break;
				}
			}
		}
	}
	else
	{
		prevbank = qtrue;
	}

	// cycle to previous bank.
	//      if cycleAllWeaps: find highest weapon in bank
	//              else: try to find weap in bank that matches cycle position
	//                      else: use base weap in bank
	if ( prevbank )
	{
		for ( i = 0; i < MAX_WEAP_BANKS_MP; i++ )
		{
			if ( cg_cycleAllWeaps.integer )
			{
				num = getPrevBankWeap( bank - i, cycle, qfalse );	// cycling all weaps always starts the next bank at the bottom
			}
			else
			{
				num = getPrevBankWeap( bank - i, cycle, qtrue );
			}

			if ( num == 0 )
			{
				continue;
			}

			if ( CG_WeaponSelectable( num) )
			{														// first entry in bank was selectable, no need to scan the bank
				break;
			}
			else
			{
				qboolean	found = qfalse;
				switch ( num )
				{
					case WP_CARBINE:
						if ( (found = CG_WeaponSelectable( WP_M7)) )
						{
							num = WP_M7;
						}
						break;
					case WP_KAR98:
						if ( (found = CG_WeaponSelectable( WP_GPG40)) )
						{
							num = WP_GPG40;
						}
						break;
				}

				if ( found )
				{
					break;
				}
			}

			CG_WeaponIndex( num, &newbank, &newcycle );				// get the bank of the new weap
			for ( j = MAX_WEAPS_IN_BANK_MP; j > 0; j-- )
			{
				num = getPrevWeapInBank( newbank, j );
				if ( CG_WeaponSelectable( num) )
				{													// found selectable weapon
					break;
				}
				else
				{
					qboolean	found = qfalse;
					switch ( num )
					{
						case WP_CARBINE:
							if ( (found = CG_WeaponSelectable( WP_M7)) )
							{
								num = WP_M7;
							}
							break;
						case WP_KAR98:
							if ( (found = CG_WeaponSelectable( WP_GPG40)) )
							{
								num = WP_GPG40;
							}
							break;
					}

					if ( found )
					{
						break;
					}
				}

				num = 0;
			}

			if ( num )
			{														// a selectable weapon was found in the current bank
				break;
			}
		}
	}

	CG_FinishWeaponChange( curweap, num );							//----(SA)
}


/*
==============
CG_LastWeaponUsed_f
==============
*/
void
CG_LastWeaponUsed_f ( void )
{
	int lastweap;

	//fretn - #447
	//osp-rtcw & et pause bug
	if ( cg.snap->ps.pm_type == PM_FREEZE )
	{
		return;
	}

	if ( cg.time - cg.weaponSelectTime < cg_weaponCycleDelay.integer )
	{
		return;						// force pause so holding it down won't go too fast
	}

	if ( cg.weaponSelect == WP_MORTAR_SET || cg.weaponSelect == WP_MOBILE_MG42_SET || cg.weaponSelect == WP_30CAL_SET )
	{
		return;
	}

	cg.weaponSelectTime = cg.time;	// flash the current weapon icon

	// don't switchback if reloading (it nullifies the reload)
	if ( cg.snap->ps.weaponstate == WEAPON_RELOADING )
	{
		return;
	}

	if ( !cg.switchbackWeapon )
	{
		cg.switchbackWeapon = cg.weaponSelect;
		return;
	}

	if ( CG_WeaponSelectable( cg.switchbackWeapon) )
	{
		lastweap = cg.weaponSelect;
		CG_FinishWeaponChange( cg.weaponSelect, cg.switchbackWeapon );
	}
	else
	{								// switchback no longer selectable, reset cycle
		cg.switchbackWeapon = 0;
	}
}


/*
==============
CG_NextWeaponInBank_f 
==============
*/
void
CG_NextWeaponInBank_f ( void )
{

	//fretn - #447
	//osp-rtcw & et pause bug
	if ( cg.snap->ps.pm_type == PM_FREEZE )
	{
		return;
	}

	if ( cg.time - cg.weaponSelectTime < cg_weaponCycleDelay.integer )
	{
		return;						// force pause so holding it down won't go too fast
	}

	// this cvar is an option that lets the player use his weapon switching keys (probably the mousewheel)
	// for zooming (binocs/snooper/sniper/etc.)
	if ( cg.zoomval )
	{
		if ( cg_useWeapsForZoom.integer == 1 )
		{
			CG_ZoomIn_f();
			return;
		}
		else if ( cg_useWeapsForZoom.integer == 2 )
		{
			CG_ZoomOut_f();
			return;
		}
	}

	cg.weaponSelectTime = cg.time;	// flash the current weapon icon
	CG_NextWeap( qfalse );
}


/*
==============
CG_PrevWeaponInBank_f 
==============
*/
void
CG_PrevWeaponInBank_f ( void )
{

	//fretn - #447
	//osp-rtcw & et pause bug
	if ( cg.snap->ps.pm_type == PM_FREEZE )
	{
		return;
	}

	if ( cg.time - cg.weaponSelectTime < cg_weaponCycleDelay.integer )
	{
		return;						// force pause so holding it down won't go too fast
	}

	// this cvar is an option that lets the player use his weapon switching keys (probably the mousewheel)
	// for zooming (binocs/snooper/sniper/etc.)
	if ( cg.zoomval )
	{
		if ( cg_useWeapsForZoom.integer == 2 )
		{
			CG_ZoomIn_f();
			return;
		}
		else if ( cg_useWeapsForZoom.integer == 1 )
		{
			CG_ZoomOut_f();
			return;
		}
	}

	cg.weaponSelectTime = cg.time;	// flash the current weapon icon
	CG_PrevWeap( qfalse );
}


/*
==============
CG_NextWeapon_f
==============
*/
void
CG_NextWeapon_f ( void )
{
	if ( !cg.snap )
	{
		return;
	}

	//fretn - #447
	//osp-rtcw & et pause bug
	if ( cg.snap->ps.pm_type == PM_FREEZE )
	{
		return;
	}

	// Overload for MV clients
	if ( cg.mvTotalClients > 0 )
	{
		CG_mvToggleView_f();
		return;
	}

	if ( cg.snap->ps.pm_flags & PMF_FOLLOW )
	{
		return;
	}

	// this cvar is an option that lets the player use his weapon switching keys (probably the mousewheel)
	// for zooming (binocs/snooper/sniper/etc.)
	if ( cg.zoomval )
	{
		if ( cg_useWeapsForZoom.integer == 1 )
		{
			CG_ZoomIn_f();
			return;
		}
		else if ( cg_useWeapsForZoom.integer == 2 )
		{
			CG_ZoomOut_f();
			return;
		}
	}

	if ( cg.time - cg.weaponSelectTime < cg_weaponCycleDelay.integer )
	{
		return;						// force pause so holding it down won't go too fast
	}

	cg.weaponSelectTime = cg.time;	// flash the current weapon icon

	// Don't try to switch when in the middle of reloading.
	// cheatinfo:	The server actually would let you switch if this check were not
	//				present, but would discard the reload.  So the when you switched
	//				back you'd have to start the reload over.  This seems bad, however
	//				the delay for the current reload is already in effect, so you'd lose
	//				the reload time twice.  (the first pause for the current weapon reload,
	//				and the pause when you have to reload again 'cause you canceled this one)
	if ( cg.snap->ps.weaponstate == WEAPON_RELOADING )
	{
		return;
	}

	CG_NextWeap( qtrue );
}


/*
==============
CG_PrevWeapon_f
==============
*/
void
CG_PrevWeapon_f ( void )
{
	if ( !cg.snap )
	{
		return;
	}

	// Overload for MV clients
	if ( cg.mvTotalClients > 0 )
	{
		CG_mvSwapViews_f();
		return;
	}

	//fretn - #447
	//osp-rtcw & et pause bug
	if ( cg.snap->ps.pm_type == PM_FREEZE )
	{
		return;
	}

	if ( cg.snap->ps.pm_flags & PMF_FOLLOW )
	{
		return;
	}

	// this cvar is an option that lets the player use his weapon switching keys (probably the mousewheel)
	// for zooming (binocs/snooper/sniper/etc.)
	if ( cg.zoomval )
	{
		if ( cg_useWeapsForZoom.integer == 1 )
		{
			CG_ZoomOut_f();
			return;
		}
		else if ( cg_useWeapsForZoom.integer == 2 )
		{
			CG_ZoomIn_f();
			return;
		}
	}

	if ( cg.time - cg.weaponSelectTime < cg_weaponCycleDelay.integer )
	{
		return;						// force pause so holding it down won't go too fast
	}

	cg.weaponSelectTime = cg.time;	// flash the current weapon icon

	// Don't try to switch when in the middle of reloading.
	if ( cg.snap->ps.weaponstate == WEAPON_RELOADING )
	{
		return;
	}

	CG_PrevWeap( qtrue );
}


/*
==============
CG_HideWeapon_f

==============
*/
void
CG_HideWeapon_f ( void )
{
	if ( !cg.snap )
	{
		return;
	}

	if ( cg.snap->ps.pm_flags & PMF_FOLLOW )
	{
		return;
	}

	if ( cg.time - cg.weaponSelectTime < cg_weaponCycleDelay.integer )
	{
		return;						// force pause so holding it down won't go too fast
	}

	if ( cg.weaponSelect == WP_MORTAR_SET || cg.weaponSelect == WP_MOBILE_MG42_SET || cg.weaponSelect == WP_30CAL_SET )
	{
		return;
	}

	cg.weaponSelectTime = cg.time;	// flash the current weapon icon

	// Don't try to switch when in the middle of reloading.
	if ( cg.snap->ps.weaponstate == WEAPON_RELOADING )
	{
		return;
	}

	if ( cg.weaponSelect != WP_NONE )
	{
		cg.holster = qtrue;
		CG_FinishWeaponChange( cg.weaponSelect, WP_NONE );
	}
	else
	{
		cg.holster = qfalse;
		CG_FinishWeaponChange( cg.weaponSelect, cg.weaponSelect_last );
	}
}


/*
==============
CG_WeaponBank_f
	weapon keys are not generally bound directly('bind 1 weapon 1'),
	rather the key is bound to a given bank ('bind 1 weaponbank 1')
==============
*/
void
CG_WeaponBank_f ( void )
{
	int num, i, curweap;
	int curbank = 0, curcycle = 0, bank = 0, cycle = 0;
	if ( !cg.snap )
	{
		return;
	}

	//fretn - #447
	//osp-rtcw & et pause bug
	if ( cg.snap->ps.pm_type == PM_FREEZE )
	{
		return;
	}

	if ( cg.snap->ps.pm_flags & PMF_FOLLOW )
	{
		return;
	}

	if ( cg.time - cg.weaponSelectTime < cg_weaponCycleDelay.integer )
	{
		return;										// force pause so holding it down won't go too fast
	}

	if ( cg.weaponSelect == WP_MORTAR_SET || cg.weaponSelect == WP_MOBILE_MG42_SET || cg.weaponSelect == WP_30CAL_SET )
	{
		return;
	}

	cg.weaponSelectTime = cg.time;					// flash the current weapon icon

	// Don't try to switch when in the middle of reloading.
	if ( cg.snap->ps.weaponstate == WEAPON_RELOADING )
	{
		return;
	}

	bank = atoi( CG_Argv( 1) );
	if ( bank <= 0 || bank > MAX_WEAP_BANKS_MP )
	{
		return;
	}

	curweap = cg.weaponSelect;
	CG_WeaponIndex( curweap, &curbank, &curcycle ); // get bank/cycle of current weapon
	if ( !cg.lastWeapSelInBank[bank] )
	{
		num = weapBanksMultiPlayer[bank][0];
		cycle -= 1;									// cycle up to first weap
	}
	else
	{
		num = cg.lastWeapSelInBank[bank];
		CG_WeaponIndex( num, &bank, &cycle );
		if ( bank != curbank )
		{
			cycle -= 1;
		}
	}

	for ( i = 0; i < MAX_WEAPS_IN_BANK_MP; i++ )
	{
		num = getNextWeapInBank( bank, cycle + i );
		if ( CG_WeaponSelectable( num) )
		{
			break;
		}
		else
		{
			qboolean	found = qfalse;
			switch ( num )
			{
				case WP_CARBINE:
					if ( (found = CG_WeaponSelectable( WP_M7)) )
					{
						num = WP_M7;
					}
					break;
				case WP_KAR98:
					if ( (found = CG_WeaponSelectable( WP_GPG40)) )
					{
						num = WP_GPG40;
					}
					break;
			}

			if ( found )
			{
				break;
			}
		}
	}

	if ( i == MAX_WEAPS_IN_BANK_MP )
	{
		return;
	}

	// Arnout: don't allow another weapon switch when we're still swapping the gpg40, to prevent animation breaking
	if
	(
		(cg.snap->ps.weaponstate == WEAPON_RAISING || cg.snap->ps.weaponstate == WEAPON_DROPPING) &&
		(
			(curweap == WP_GPG40 || num == WP_GPG40 || curweap == WP_M7 || num == WP_M7) ||
			(curweap == WP_SILENCER || num == WP_SILENCER || curweap == WP_SILENCED_COLT || num == WP_SILENCED_COLT) ||
			(curweap == WP_MORTAR_SET || num == WP_MORTAR_SET)
		)
	)
	{
		return;
	}

	CG_FinishWeaponChange( curweap, num );
}


/*
===============
CG_Weapon_f
===============
*/
void
CG_Weapon_f ( void )
{
	int num;
	if ( !cg.snap )
	{
		return;
	}

	//fretn - #447
	//osp-rtcw & et pause bug
	if ( cg.snap->ps.pm_type == PM_FREEZE )
	{
		return;
	}

	if ( cg.snap->ps.pm_flags & PMF_FOLLOW )
	{
		return;
	}

	if ( cg.weaponSelect == WP_MORTAR_SET || cg.weaponSelect == WP_MOBILE_MG42_SET || cg.weaponSelect == WP_30CAL_SET )
	{
		return;
	}

	num = atoi( CG_Argv( 1) );

	// JPW NERVE
	// weapon bind should execute weaponbank instead -- for splitting out class weapons, per Id request
	if ( num < MAX_WEAP_BANKS_MP )
	{
		CG_WeaponBank_f();
	}

	return;

	// jpw
}


/*
===================
CG_OutOfAmmoChange

The current weapon has just run out of ammo
===================
*/
void
CG_OutOfAmmoChange ( qboolean allowforceswitch )
{
	int i;
	int bank, cycle;
	int equiv = WP_NONE;

	//
	// trivial switching
	//
	if
	(
		cg.weaponSelect == WP_PLIERS ||
		(cg.weaponSelect == WP_SATCHEL_DET && cg.predictedPlayerState.ammo[WP_SATCHEL_DET])
	)
	{
		return;
	}

	if ( allowforceswitch )
	{
		if ( cg.weaponSelect == WP_SMOKE_BOMB )
		{
			if ( CG_WeaponSelectable( WP_LUGER) )
			{
				cg.weaponSelect = WP_LUGER;
				CG_FinishWeaponChange( cg.predictedPlayerState.weapon, WP_LUGER );
				return;
			}
			else if ( CG_WeaponSelectable( WP_COLT) )
			{
				cg.weaponSelect = WP_COLT;
				CG_FinishWeaponChange( cg.predictedPlayerState.weapon, WP_COLT );
				return;
			}
		}
		else if ( cg.weaponSelect == WP_LANDMINE )
		{
			if ( CG_WeaponSelectable( WP_PLIERS) )
			{
				cg.weaponSelect = WP_PLIERS;
				CG_FinishWeaponChange( cg.predictedPlayerState.weapon, WP_PLIERS );
				return;
			}
		}
		else if ( cg.weaponSelect == WP_SATCHEL )
		{
			if ( CG_WeaponSelectable( WP_SATCHEL_DET) )
			{
				cg.weaponSelect = WP_SATCHEL_DET;
				return;
			}
		}
		else if ( cg.weaponSelect == WP_MORTAR_SET )
		{
			cg.weaponSelect = WP_MORTAR;
			return;
		}
		else if ( cg.weaponSelect == WP_MOBILE_MG42_SET )
		{
			cg.weaponSelect = WP_MOBILE_MG42;
			return;
		}
		else if ( cg.weaponSelect == WP_30CAL_SET )
		{
			cg.weaponSelect = WP_30CAL;
			return;
		}

		// JPW NERVE -- early out if we just dropped dynamite, go to pliers
		if ( cg.weaponSelect == WP_DYNAMITE )
		{
			if ( CG_WeaponSelectable( WP_PLIERS) )
			{
				cg.weaponSelect = WP_PLIERS;
				CG_FinishWeaponChange( cg.predictedPlayerState.weapon, WP_PLIERS );
				return;
			}
		}

		// JPW NERVE -- early out if we just fired Panzerfaust, go to pistola, then grenades
		if ( cg.weaponSelect == WP_PANZERFAUST )
		{

			// CHRUKER: b021 - Actually use a SMG first if we got it
			for ( i = 0; i < MAX_WEAPS_IN_BANK_MP; i++ )
			{
				for ( i = 0; i < MAX_WEAPS_IN_BANK_MP; i++ )
				{

					// CHRUKER: b021 - Make sure we don't reselect the panzer. (That took me a while to figure that out :-)
					if
					(
						weapBanksMultiPlayer[3][i] != WP_PANZERFAUST &&
						CG_WeaponSelectable( weapBanksMultiPlayer[3][i])
					)
					{	// find a smg
						cg.weaponSelect = weapBanksMultiPlayer[3][i];
						CG_FinishWeaponChange( cg.predictedPlayerState.weapon, cg.weaponSelect );
						return;
					}
				}		// b021

				if ( CG_WeaponSelectable( weapBanksMultiPlayer[2][i]) )
				{		// find a pistol
					cg.weaponSelect = weapBanksMultiPlayer[2][i];
					CG_FinishWeaponChange( cg.predictedPlayerState.weapon, cg.weaponSelect );
					return;
				}
			}

			for ( i = 0; i < MAX_WEAPS_IN_BANK_MP; i++ )
			{
				if ( CG_WeaponSelectable( weapBanksMultiPlayer[4][i]) )
				{		// find a grenade
					cg.weaponSelect = weapBanksMultiPlayer[4][i];
					CG_FinishWeaponChange( cg.predictedPlayerState.weapon, cg.weaponSelect );
					return;
				}
			}
		}

		// if you're using an alt mode weapon, try switching back to the parent
		// otherwise, switch to the equivalent if you've got it
		if ( cg.weaponSelect >= WP_BEGINSECONDARY && cg.weaponSelect <= WP_LASTSECONDARY )
		{
			cg.weaponSelect = equiv = getAltWeapon( cg.weaponSelect );						// base any further changes on the parent
			if ( CG_WeaponSelectable( equiv) )
			{																				// the parent was selectable, drop back to that
				CG_FinishWeaponChange( cg.predictedPlayerState.weapon, cg.weaponSelect );	//----(SA)	
				return;
			}
		}

		// now try the opposite team's equivalent weap
		equiv = getEquivWeapon( cg.weaponSelect );
		if ( equiv != cg.weaponSelect && CG_WeaponSelectable( equiv) )
		{
			cg.weaponSelect = equiv;
			CG_FinishWeaponChange( cg.predictedPlayerState.weapon, cg.weaponSelect );		//----(SA)	
			return;
		}
	}

	//
	// more complicated selection
	//
	// didn't have available alternative or equivalent, try another weap in the bank
	CG_WeaponIndex( cg.weaponSelect, &bank, &cycle );										// get bank/cycle of current weapon

	// JPW NERVE -- more useful weapon changes -- check if rifle or pistol is still working, and use that if available
	for ( i = 0; i < MAX_WEAPS_IN_BANK_MP; i++ )
	{
		if ( CG_WeaponSelectable( weapBanksMultiPlayer[3][i]) )
		{	// find a rifle
			cg.weaponSelect = weapBanksMultiPlayer[3][i];
			CG_FinishWeaponChange( cg.predictedPlayerState.weapon, cg.weaponSelect );
			return;
		}
	}

	for ( i = 0; i < MAX_WEAPS_IN_BANK_MP; i++ )
	{
		if ( CG_WeaponSelectable( weapBanksMultiPlayer[2][i]) )
		{	// find a pistol
			cg.weaponSelect = weapBanksMultiPlayer[2][i];
			CG_FinishWeaponChange( cg.predictedPlayerState.weapon, cg.weaponSelect );
			return;
		}
	}

	// otherwise just do something
	for ( i = cycle; i < MAX_WEAPS_IN_BANK_MP; i++ )
	{
		equiv = getNextWeapInBank( bank, i );
		if ( CG_WeaponSelectable( equiv) )
		{	// found a reasonable replacement
			cg.weaponSelect = equiv;
			CG_FinishWeaponChange( cg.predictedPlayerState.weapon, cg.weaponSelect );	//----(SA)	
			return;
		}
	}

	// still nothing available, just go to the next
	// available weap using the regular selection scheme
	CG_NextWeap( qtrue );
}


/*
===================================================================================================

WEAPON EVENTS

===================================================================================================
*/
void
CG_MG42EFX ( centity_t *cent )
{

	// Arnout: complete overhaul of this one
	centity_t	*mg42;
	int			num;
	vec3_t		forward, point;
	refEntity_t flash;

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
			VectorCopy( mg42->currentState.pos.trBase, point );
			AngleVectors( cent->lerpAngles, forward, NULL, NULL );
			VectorMA( point, 40, forward, point );
			memset( &flash, 0, sizeof(flash) );
			flash.renderfx = RF_LIGHTING_ORIGIN;
			flash.hModel = cgs.media.mg42muzzleflash;
			VectorCopy( point, flash.origin );
			AnglesToAxis( cent->lerpAngles, flash.axis );
			trap_R_AddRefEntityToScene( &flash );

			// ydnar: add dynamic light
			trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );

			//CG_AddFlashLighting( flash.origin );
			return;
		}
		else if
			(
				mg42->currentState.eType == ET_AAGUN &&
				mg42->currentState.otherEntityNum == cent->currentState.number
			)
		{

			//	vec3_t	forward , right , up, point1 , point2 , point3, point4;
			refEntity_t flash, aagun;
			int			rnd;

			// found it, clamp behind gun
			//	rnd = mg42->currentState.modelindex2;
			rnd = mg42->currentState.modelindex2 + 1 % 4;

			/*	VectorCopy( mg42->currentState.pos.trBase, point1 );
			VectorCopy( mg42->currentState.pos.trBase, point2 );
			VectorCopy( mg42->currentState.pos.trBase, point3 );
			VectorCopy( mg42->currentState.pos.trBase, point4 );
			AngleVectors( cent->lerpAngles, forward, right, up );

			VectorMA( point1, 165, forward, point1 );
			VectorMA( point1, -30, right, point1 );
			VectorMA( point1, 25, up, point1 );

			VectorMA( point2, 165, forward, point2 );
			VectorMA( point2, -30, right, point2 );
			VectorMA( point2, -15, up, point2 );

			VectorMA( point3, 165, forward, point3 );
			VectorMA( point3, 30, right, point3 );
			VectorMA( point3, 25, up, point3 );

			VectorMA( point4, 165, forward, point4 );
			VectorMA( point4, 30, right, point4 );
			VectorMA( point4, -15, up, point4 );

			memset (&flash, 0, sizeof (flash));
			flash.renderfx = RF_LIGHTING_ORIGIN;
			flash.hModel = cgs.media.mg42muzzleflash;
			if ( rnd == 1) {
			VectorCopy( point1, flash.origin );
			} else if ( rnd == 2) {
			VectorCopy( point2, flash.origin );
			} else if ( rnd == 3) {
			VectorCopy( point3, flash.origin );
			} else if ( rnd == 4) {
			VectorCopy( point4, flash.origin );
			}

			AnglesToAxis( cent->lerpAngles, flash.axis );
			*/
			memset( &flash, 0, sizeof(flash) );
			flash.renderfx = RF_LIGHTING_ORIGIN;
			flash.hModel = cgs.media.mg42muzzleflash;
			memset( &aagun, 0, sizeof(aagun) );
			aagun.hModel = cgs.media.aagunTagmodel;

			//aagun.frame = rnd-1;
			VectorCopy( mg42->lerpOrigin, aagun.origin );
			AnglesToAxis( cent->lerpAngles, aagun.axis );
			trap_R_AddRefEntityToScene( &aagun );
			if ( rnd == 1 )
			{
				CG_PositionEntityOnTag( &flash, &aagun, "tag_flash1", 0, NULL );
			}
			else if ( rnd == 2 )
			{
				CG_PositionEntityOnTag( &flash, &aagun, "tag_flash2", 0, NULL );
			}
			else if ( rnd == 3 )
			{
				CG_PositionEntityOnTag( &flash, &aagun, "tag_flash3", 0, NULL );
			}
			else if ( rnd == 4 )
			{
				CG_PositionEntityOnTag( &flash, &aagun, "tag_flash4", 0, NULL );
			}

			trap_R_AddRefEntityToScene( &flash );

			// ydnar: add dynamic light
			trap_R_AddLightToScene( flash.origin, 320, 1.25 + (rand() & 31) / 128, 1.0, 0.6, 0.23, 0, 0 );

			//			CG_AddFlashLighting( flash.origin );
			return;
		}
	}
}

//----(SA)	


/*
==============
CG_MortarEFX
	Right now mostly copied directly from Raf's MG42 FX, but with the optional addtion of smoke
==============
*/
void
CG_MortarEFX ( centity_t *cent )
{
	refEntity_t flash;
	if ( cent->currentState.density & 1 )
	{

		// smoke
		CG_ParticleImpactSmokePuff( cgs.media.smokePuffShader, cent->currentState.origin );
	}

	if ( cent->currentState.density & 2 )
	{

		// light
		//%	trap_R_AddLightToScene( cent->currentState.origin, 200 + (rand()&31), 1.0, 1.0, 1.0, 0 );
		trap_R_AddLightToScene( cent->currentState.origin, 256, 0.75 + 8.0 / (rand() & 31), 1.0, 1.0, 1.0, 0, 0 );

		// muzzle flash
		memset( &flash, 0, sizeof(flash) );
		flash.renderfx = RF_LIGHTING_ORIGIN;
		flash.hModel = cgs.media.mg42muzzleflash;
		VectorCopy( cent->currentState.origin, flash.origin );
		AnglesToAxis( cg.refdefViewAngles, flash.axis );
		trap_R_AddRefEntityToScene( &flash );
	}
}

//----(SA)	end
// RF


/*
==============
CG_WeaponFireRecoil
==============
*/
void
CG_WeaponFireRecoil ( int weapon )
{
	float	pitchRecoilAdd, pitchAdd;
	float	yawRandom;
	vec3_t	recoil;

	//
	pitchRecoilAdd = 0;
	pitchAdd = 0;
	yawRandom = 0;

	//
	switch ( weapon )
	{
		case WP_LUGER:
		case WP_SILENCER:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
		case WP_COLT:
		case WP_SILENCED_COLT:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
			pitchAdd = 2 + rand() % 3;
			yawRandom = 2;
			break;
		case WP_G43:
		case WP_M1GARAND:
		case WP_KAR98:
		case WP_CARBINE:
			pitchAdd = 2 + rand() % 2;		//----(SA)	for DM
			yawRandom = 1 + rand() % 2;		//----(SA)	for DM
			break;
		case WP_K43:
		case WP_GARAND:
			if ( !(cg.predictedPlayerState.eFlags & EF_AIMING) )
			{
				pitchAdd = 2 + rand() % 2;	//----(SA)	for DM
				yawRandom = 1 + rand() % 2; //----(SA)	for DM
			}
			break;
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
			pitchAdd = 0.3 + rand() % 5;
			break;
		case WP_30CAL_SET:
		case WP_MOBILE_MG42_SET:
			pitchAdd = 0.3 + rand() % 2;
			break;
		case WP_FG42SCOPE:
		case WP_FG42:
			if ( !(cg.predictedPlayerState.eFlags & EF_AIMING) )
			{
				pitchAdd = 2 + rand() % 3;
				yawRandom = 2 + rand() % 3;
			}
			break;
		case WP_30CAL:
		case WP_MOBILE_MG42:
			pitchAdd = 4 + rand() % 4;
			yawRandom = 4 + rand() % 4;
			break;
		case WP_MP40:
	#ifdef NWEAPS \
		// TODO!
		case WP_MP44:
		case WP_BAR:
	#endif
		case WP_THOMPSON:
		case WP_STEN:
			if ( !(cg.predictedPlayerState.eFlags & EF_AIMING) )
			{
			pitchAdd = 1 + rand() % 3;
			yawRandom = 1 + rand() % 3;
			}
			break;
		case WP_PANZERFAUST:
			pitchAdd = -18 - rand() % 8;
			yawRandom = 5 - rand() % 10;

			// push the player back instead
			break;
		default:
			return;
	}

	// calc the recoil
	recoil[YAW] = crandom() * yawRandom;
	recoil[ROLL] = -recoil[YAW];			// why not
	recoil[PITCH] = -pitchAdd;

	// scale it up a bit (easier to modify this while tweaking)
	VectorScale( recoil, 30, recoil );

	// set the recoil
	VectorCopy( recoil, cg.kickAVel );

	// set the recoil
	cg.recoilPitch -= pitchRecoilAdd;
}


/*
================
CG_FireWeapon

Caused by an EV_FIRE_WEAPON event

================
*/
void
CG_FireWeapon2 ( centity_t *cent )
{
	trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, cgs.media.hWeaponSnd );
	cent->muzzleFlashTime2 = cg.time;
}


/* */
void
CG_FireWeapon ( centity_t *cent )
{
	entityState_t	*ent;
	int				c;
	weaponInfo_t	*weap;
	sfxHandle_t		*firesound;
	sfxHandle_t		*fireEchosound;
	ent = &cent->currentState;

	// Arnout: quick hack for EF_MOUNTEDTANK, need to change this - likely it needs to use viewlocked as well
	if ( cent->currentState.eFlags & EF_MOUNTEDTANK )
	{
		if
		(
			cg_entities[cg_entities[cg_entities[cent->currentState.number].tagParent].tankparent].currentState.
				density & 8
		)
		{																		// should we use a browning?
			//	trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, cgs.media.hWeaponSnd_2 );
		}
		else
		{

			//trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, cgs.media.hWeaponSnd );
		}

		cent->muzzleFlashTime = cg.time;
		return;
	}

	if ( cent->currentState.eFlags & EF_TANK )
	{
		trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, cgs.media.tankFireSound );
		cent->muzzleFlashTime = cg.time;
		return;
	}

	if ( cent->currentState.eFlags & EF_BROWNING_ACTIVE )
	{

		//trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, cgs.media.hWeaponSnd_2 );
		if ( cg_brassTime.integer > 0 )
		{
			CG_MachineGunEjectBrass( cent );
		}

		cent->muzzleFlashTime = cg.time;
		return;
	}

	// Rafael - mg42
	if ( BG_PlayerMounted( cent->currentState.eFlags) )
	{
		if ( cent->currentState.eFlags & EF_AAGUN_ACTIVE )
		{
			trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, cgs.media.hflakWeaponSnd );
		}
		else
		{

			//	trap_S_AddLoopingSound( cent->lerpOrigin,  vec3_origin, cgs.media.mg42loop, 255, 0 );
			//	trap_S_StartSound( NULL, cent->currentState.number, CHAN_WEAPON, cgs.media.hWeaponSnd );
		}

		if ( cg_brassTime.integer > 0 )
		{
			CG_MachineGunEjectBrass( cent );
		}

		cent->muzzleFlashTime = cg.time;
		return;
	}

	if ( ent->weapon == WP_NONE )
	{
		return;
	}

	if ( ent->weapon >= WP_NUM_WEAPONS )
	{
		CG_Error( "CG_FireWeapon: ent->weapon >= WP_NUM_WEAPONS" );
		return;
	}

	weap = &cg_weapons[ent->weapon];
	if ( cent->currentState.clientNum == cg.snap->ps.clientNum
#ifdef __NPC__
	&& cent->currentState.eType != ET_NPC
#endif
	)
	{
		cg.lastFiredWeapon = ent->weapon;										//----(SA)	added
	}

	// mark the entity as muzzle flashing, so when it is added it will
	// append the flash to the weapon model
	cent->muzzleFlashTime = cg.time;

	// RF, kick angles
	if ( ent->number == cg.snap->ps.clientNum )
	{
		CG_WeaponFireRecoil( ent->weapon );
	}

	if ( ent->weapon == WP_MORTAR_SET )
	{
		if ( ent->clientNum == cg.snap->ps.clientNum
#ifdef __NPC__
		&& cent->currentState.eType != ET_NPC
#endif
		)
		{
			cg.mortarImpactTime = -1;
			cg.mortarFireAngles[PITCH] = cg.predictedPlayerState.viewangles[PITCH];
			cg.mortarFireAngles[YAW] = cg.predictedPlayerState.viewangles[YAW];
		}
	}

	// lightning gun only does this this on initial press
	if ( ent->weapon == WP_FLAMETHROWER )
	{
		if ( cent->pe.lightningFiring )
		{
			return;
		}
	}
	else if
		(
			ent->weapon == WP_GRENADE_LAUNCHER ||
			ent->weapon == WP_GRENADE_PINEAPPLE ||
			ent->weapon == WP_DYNAMITE ||
			ent->weapon == WP_SMOKE_MARKER ||
			ent->weapon == WP_LANDMINE ||
			ent->weapon == WP_SATCHEL ||
			ent->weapon == WP_SMOKE_BOMB
		)
	{																			// JPW NERVE
		if ( ent->apos.trBase[0] > 0 )
		{																		// underhand
			return;
		}
	}

	if ( ent->weapon == WP_GPG40 )
	{
		if ( ent->clientNum == cg.snap->ps.clientNum )
		{
			cg.weaponSelect = WP_KAR98;
		}
	}
	else if ( ent->weapon == WP_M7 )
	{
		if ( ent->clientNum == cg.snap->ps.clientNum )
		{
			cg.weaponSelect = WP_CARBINE;
		}
	}

	if ( (cent->currentState.event &~EV_EVENT_BITS) == EV_FIRE_WEAPON_LASTSHOT )
	{
		firesound = &weap->lastShotSound[0];
		fireEchosound = &weap->flashEchoSound[0];

		// try to use the lastShotSound, but don't assume it's there.
		// if a weapon without the sound calls it, drop back to regular fire sound
		for ( c = 0; c < 4; c++ )
		{
			if ( !firesound[c] )
			{
				break;
			}
		}

		if ( !c )
		{
			firesound = &weap->flashSound[0];
			fireEchosound = &weap->flashEchoSound[0];
		}
	}
	else
	{
		firesound = &weap->flashSound[0];
		fireEchosound = &weap->flashEchoSound[0];
	}

	if
	(
		!(cent->currentState.eFlags & EF_ZOOMING) &&
		(cent->currentState.weapon != WP_MEDKIT || cg_realmedic.integer == 0)
	)
	{																			// JPW NERVE -- don't play sounds or eject brass if zoomed in
		// play a sound
		for ( c = 0; c < 4; c++ )
		{
			if ( !firesound[c] )
			{
				break;
			}
		}

		if ( c > 0 )
		{
			c = rand() % c;
			if ( firesound[c] )
			{
				trap_S_StartSound( NULL, ent->number, CHAN_WEAPON, firesound[c] );
				if ( fireEchosound && fireEchosound[c] )						// check for echo
				{
					centity_t	*cent;
					vec3_t		porg, gorg, norm;								// player/gun origin
					float		gdist;
					cent = &cg_entities[ent->number];
					VectorCopy( cent->currentState.pos.trBase, gorg );
					VectorCopy( cg.refdef_current->vieworg, porg );
					VectorSubtract( gorg, porg, norm );
					gdist = VectorNormalize( norm );
					if ( gdist > 256 && gdist < 6000 )
					{															// temp dist.  TOD-O: use numbers that are weapon specific , JaquboSS , changed a bit :D
						// use gorg as the new sound origin
						VectorMA( cg.refdef_current->vieworg, 64, norm, gorg ); // sound-on-a-stick
						trap_S_StartSoundEx( gorg, ent->number, CHAN_WEAPON, fireEchosound[c], SND_NOCUT );
					}
				}
			}
		}

		// do brass ejection
		if ( weap->ejectBrassFunc && cg_brassTime.integer > 0 )
		{
			weap->ejectBrassFunc( cent );
		}
	}	// jpw
}

// Ridah


/*
=================
CG_AddSparks
=================
*/
void
CG_AddSparks ( vec3_t origin, vec3_t dir, int speed, int duration, int count, float randScale )
{
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			velocity;
	int				i;
	for ( i = 0; i < count; i++ )
	{
		le = CG_AllocLocalEntity();
		re = &le->refEntity;
		VectorSet( velocity, dir[0] + crandom() * randScale, dir[1] + crandom() * randScale, dir[2] + crandom() * randScale );
		VectorScale( velocity, (float) speed, velocity );
		le->leType = LE_SPARK;
		le->startTime = cg.time;
		le->endTime = le->startTime + duration - (int) ( 0.5 * random() * duration );
		le->lastTrailTime = cg.time;
		VectorCopy( origin, re->origin );
		AxisCopy( axisDefault, re->axis );
		le->pos.trType = TR_GRAVITY_LOW;
		VectorCopy( origin, le->pos.trBase );
		VectorMA( le->pos.trBase, 2 + random() * 4, dir, le->pos.trBase );
		VectorCopy( velocity, le->pos.trDelta );
		le->pos.trTime = cg.time;
		le->refEntity.customShader = cgs.media.sparkParticleShader;
		le->bounceFactor = 0.9;
	}
}


/*
=================
CG_AddBulletParticles
=================
*/
void
CG_AddBulletParticles ( vec3_t origin, vec3_t dir, int speed, int duration, int count, float randScale )
{
	vec3_t	velocity, pos;
	int		i;

	// add the falling particles
	for ( i = 0; i < count; i++ )
	{
		VectorSet( velocity, dir[0] + crandom() * randScale, dir[1] + crandom() * randScale, dir[2] + crandom() * randScale );
		VectorScale( velocity, (float) speed, velocity );
		VectorCopy( origin, pos );
		VectorMA( pos, 2 + random() * 4, dir, pos );
		CG_ParticleBulletDebris( pos, velocity, 300 + rand() % 300 );
	}
}


/*
=================
CG_AddDirtBulletParticles
=================
*/
void
CG_AddDirtBulletParticles
(
	vec3_t		origin,
	vec3_t		dir,
	int			speed,
	int			duration,
	int			count,
	float		randScale,
	float		width,
	float		height,
	float		alpha,
	qhandle_t	shader
)
{
	vec3_t	velocity, pos;
	int		i;

	// add the big falling particle
	VectorSet( velocity, 0, 0, (float) speed );
	VectorCopy( origin, pos );
	CG_ParticleDirtBulletDebris_Core( pos, velocity, duration, width, height, alpha, shader );	//600 + rand()%300 ); // keep central one
	for ( i = 0; i < count; i++ )
	{
		VectorSet( velocity, dir[0] * crandom() * speed * randScale, dir[1] * crandom() * speed * randScale,
				   dir[2] * random() * speed );
		CG_ParticleDirtBulletDebris_Core( pos, velocity, duration + (rand() % (duration >> 1)), width, height, alpha,
										  shader );
	}
}


/*
=================
CG_AddDebris
=================
*/
void
CG_AddDebris ( vec3_t origin, vec3_t dir, int speed, int duration, int count )
{
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			velocity, unitvel;
	float			timeAdd;
	int				i;
	for ( i = 0; i < count; i++ )
	{
		le = CG_AllocLocalEntity();
		re = &le->refEntity;
		VectorSet( unitvel, dir[0] + crandom() * 0.9, dir[1] + crandom() * 0.9,
				   fabs( dir[2]) > 0.5 ? dir[2] * (0.2 + 0.8 * random()) : random() * 0.6 );
		VectorScale( unitvel, (float) speed + (float) speed * 0.5 * crandom(), velocity );
		le->leType = LE_DEBRIS;
		le->startTime = cg.time;
		le->endTime = le->startTime + duration + (int) ( (float) duration * 0.8 * crandom() );
		le->lastTrailTime = cg.time;
		VectorCopy( origin, re->origin );
		AxisCopy( axisDefault, re->axis );
		le->pos.trType = TR_GRAVITY_LOW;
		VectorCopy( origin, le->pos.trBase );
		VectorCopy( velocity, le->pos.trDelta );
		le->pos.trTime = cg.time;
		timeAdd = 10.0 + random() * 40.0;
		BG_EvaluateTrajectory( &le->pos, cg.time + (int) timeAdd, le->pos.trBase, qfalse, -1 );
		le->bounceFactor = 0.5;
		le->effectWidth = 5 + random() * 5;
		le->effectFlags |= 1;	// smoke trail
	}
}

// done.


/*
==============
CG_WaterRipple
==============
*/
void
CG_WaterRipple ( qhandle_t shader, vec3_t loc, vec3_t dir, int size, int lifetime )
{
	localEntity_t	*le;
	refEntity_t		*re;
	le = CG_AllocLocalEntity();
	le->leType = LE_SCALE_FADE;
	le->leFlags = LEF_PUFF_DONT_SCALE;
	le->startTime = cg.time;
	le->endTime = cg.time + lifetime;
	le->lifeRate = 1.0 / ( le->endTime - le->startTime );
	re = &le->refEntity;
	VectorCopy( loc, re->origin );
	re->shaderTime = cg.time / 1000.0f;
	re->reType = RT_SPLASH;
	re->radius = size;
	re->customShader = shader;
	re->shaderRGBA[0] = 0xff;
	re->shaderRGBA[1] = 0xff;
	re->shaderRGBA[2] = 0xff;
	re->shaderRGBA[3] = 0xff;
	le->color[3] = 1.0;
}


/*
=================
CG_MissileHitWall

Caused by an EV_MISSILE_MISS event, or directly by local bullet tracing

ClientNum is a dummy field used to define what sort of effect to spawn
=================
*/
void
CG_MissileHitWall ( int weapon, int clientNum, vec3_t origin, vec3_t dir, int surfFlags )
{													//	(SA) modified to send missilehitwall surface parameters
	qhandle_t		mod, mark, shader;
	sfxHandle_t		sfx, sfx2;
	localEntity_t	*le;
	qboolean		isSprite, alphaFade = qfalse;
	int				r, duration, lightOverdraw, i, j, markDuration, volume;
	trace_t			trace;
	vec3_t			lightColor, tmpv, tmpv2, sprOrg, sprVel;
	float			radius, light, sfx2range = 0;
	vec4_t			projection, color;
	vec3_t			markOrigin;
	mark = 0;
	radius = 32;
	sfx = 0;
	sfx2 = 0;
	mod = 0;
	shader = 0;
	light = 0;
	VectorSet( lightColor, 1, 1, 0 );

	// Ridah
	lightOverdraw = 0;
	volume = 127;

	// set defaults
	isSprite = qfalse;
	duration = 600;
	markDuration = -1;
	if ( surfFlags & SURF_SKY )
	{
		return;
	}

	switch ( weapon )
	{
		case WP_KNIFE:
			i = rand() % 4;
			if ( !surfFlags )
			{
				sfx = cgs.media.sfx_knifehit[4];	// different values for different types (stone/metal/wood/etc.)
				mark = cgs.media.bulletMarkShader;
				radius = 1 + rand() % 2;
				CG_AddBulletParticles( origin, dir, 20, 800, 3 + rand() % 6, 1.0 );
			}
			else
			{
				sfx = cgs.media.sfx_knifehit[i];
			}

			// ydnar: set mark duration
			markDuration = cg_markTime.integer;
			break;
		case WP_LUGER:
		case WP_SILENCER:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
		case WP_COLT:
		case WP_SILENCED_COLT:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
		case WP_MP40:
		case WP_THOMPSON:
		case WP_STEN:
		case WP_GARAND:
		case WP_FG42:
		case WP_FG42SCOPE:
		case WP_KAR98:
		case WP_CARBINE:
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
		case WP_K43:
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
		case WP_G43:
		case WP_M1GARAND:
		case WP_30CAL:
		case WP_30CAL_SET:
	#ifdef NWEAPS
		case WP_MP44:
		case WP_BAR:
	#endif

			// actually yeah.  meant that.  very rare.
			r = ( rand() & 3 ) + 1;					// JPW NERVE increased spark frequency so players can tell where rounds are coming from in MP
			volume = 64;

			// clientNum is a dummy field used to define what sort of effect to spawn
			if ( !clientNum )
			{

				// RF, why is this here? we need sparks if clientNum = 0, used for warzombie
				CG_AddSparks( origin, dir, 350, 200, 15 + rand() % 7, 0.2 );
			}
			else if ( clientNum == 1 )
			{										// just do a little smoke puff
				vec3_t	d, o;
				VectorMA( origin, 12, dir, o );
				VectorScale( dir, 7, d );
				d[2] += 16;

				// DHM - Nerve :: use dirt images
				if ( (surfFlags & SURF_GRASS || surfFlags & SURF_GRAVEL || surfFlags & SURF_SNOW) )
				{									// JPW NERVE added SURF_SNOW
					// some debris particles
					// JPW NERVE added surf_snow
					if ( surfFlags & SURF_SNOW )
					{
						CG_AddDirtBulletParticles( origin, dir, 190, 900, 5, 0.25, 80, 32, 0.5,
												   cgs.media.dirtParticle2Shader );
					}
					else
					{
						CG_AddDirtBulletParticles( origin, dir, 190, 900, 5, 0.5, 80, 16, 0.5, cgs.media.dirtParticle1Shader );
					}
				}
				else
				{

					//CG_ParticleImpactSmokePuff (cgs.media.smokeParticleShader, o);
					// some debris particles
					CG_AddBulletParticles( origin, dir, 20, 800, 3 + rand() % 6, 1.0 );		// rand scale

					// just do a big one
					if ( surfFlags & SURF_METAL || surfFlags & SURF_ROOF )
					{
						if ( sfx && (rand() % 3 == 0) )
						{
							CG_AddSparks( origin, dir, 250, 10000, 4 + rand() % 7, 0.75 );	// rand scale
						}
					}
					else
					{
						if ( sfx && (rand() % 3 == 0) )
						{
							CG_AddSparks( origin, dir, 450, 300, 3 + rand() % 3, 0.5 );		// rand scale
						}

						// Jauqboss add smoke :P
						CG_ParticleImpactSmokePuffExtended( cgs.media.panzerfaustsmokeParticleShader, origin, 1000, 16, 5,
															5, 0.15f, 5.f );
					}
				}
			}
			else if ( clientNum == 2 )
			{
				sfx = 0;
				mark = 0;

				// (SA) needed to do the CG_WaterRipple using a localent since I needed the timer reset on the shader for each shot
				CG_WaterRipple( cgs.media.wakeMarkShaderAnim, origin, tv( 0, 0, 1), 32, 1000 );
				CG_AddDirtBulletParticles( origin, dir, 190, 900, 5, 0.5, 80, 16, 0.125, cgs.media.dirtParticle2Shader );
				break;

				// play a water splash
				mod = cgs.media.waterSplashModel;
				shader = cgs.media.waterSplashShader;
				duration = 250;
			}

			// Ridah, optimization, only spawn the bullet hole if we are close
			// enough to see it, this way we can leave other marks around a lot
			// longer, since most of the time we can't actually see the bullet holes
			// (SA) small modification.  only do this for non-rifles (so you can see your shots hitting when you're zooming with a rifle scope)
			if
			(
				weapon == WP_FG42SCOPE ||
				weapon == WP_GARAND_SCOPE ||
				weapon == WP_K43_SCOPE ||
				(Distance( cg.refdef_current->vieworg, origin) < cg_markdistance.integer)
			)
			{
				if ( clientNum )
				{

					// mark and sound can potentially use the surface for override values
					mark = cgs.media.bulletMarkShader;										// default
					alphaFade = qtrue;														// max made the bullet mark alpha (he'll make everything in the game out of 1024 textures, all with alpha blend funcs yet...)

					//%	radius = 1.5f + rand()%2;	// slightly larger for DM
					radius = 1.0f + 0.5f * ( rand() % 2 );
	#define MAX_IMPACT_SOUNDS	5
					if ( surfFlags & SURF_METAL || surfFlags & SURF_ROOF )
					{
						sfx = cgs.media.sfx_bullet_metalhit[rand() % MAX_IMPACT_SOUNDS];
						mark = cgs.media.bulletMarkShaderMetal;
						alphaFade = qtrue;
					}
					else if ( surfFlags & SURF_WOOD )
					{
						sfx = cgs.media.sfx_bullet_woodhit[rand() % MAX_IMPACT_SOUNDS];
						mark = cgs.media.bulletMarkShaderWood;
						alphaFade = qtrue;
						radius += 0.4f;														// experimenting with different mark sizes per surface
					}
					else if ( surfFlags & SURF_GLASS )
					{
						sfx = cgs.media.sfx_bullet_glasshit[rand() % MAX_IMPACT_SOUNDS];
						mark = cgs.media.bulletMarkShaderGlass;
						alphaFade = qtrue;
					}
					else
					{
						sfx = cgs.media.sfx_bullet_stonehit[rand() % MAX_IMPACT_SOUNDS];
						mark = cgs.media.bulletMarkShader;
						alphaFade = qtrue;
					}

					// ydnar: set mark duration
					markDuration = cg_markTime.integer;
				}
			}
			break;
		case WP_MAPMORTAR:
			sfx = cgs.media.sfx_rockexp;
			sfx2 = cgs.media.sfx_rockexpDist;
			sfx2range = 1200;
			mark = cgs.media.burnMarkShader;
			markDuration = cg_markTime.integer * 3;
			radius = 96;													// ydnar: bigger mark radius
			light = 300;
			isSprite = qtrue;
			duration = 1000;
			lightColor[0] = 0.75;
			lightColor[1] = 0.5;
			lightColor[2] = 0.1;
			VectorScale( dir, 16, sprVel );
			if ( CG_PointContents( origin, 0) & CONTENTS_WATER )
			{
				VectorCopy( origin, tmpv );
				tmpv[2] += 10000;
				trap_CM_BoxTrace( &trace, tmpv, origin, NULL, NULL, 0, MASK_WATER );
				CG_WaterRipple( cgs.media.wakeMarkShaderAnim, trace.endpos, dir, 150, 1000 );
				CG_AddDirtBulletParticles( trace.endpos, dir, 900, 1800, 15, 0.5, 350, 128, 0.125,
										   cgs.media.dirtParticle2Shader );
			}
			else
			{
				VectorCopy( origin, tmpv );
				tmpv[2] += 20;
				VectorCopy( origin, tmpv2 );
				tmpv2[2] -= 20;
				trap_CM_BoxTrace( &trace, tmpv, tmpv2, NULL, NULL, 0, MASK_SHOT );
				if ( trace.surfaceFlags & SURF_GRASS || trace.surfaceFlags & SURF_GRAVEL )
				{
					CG_AddDirtBulletParticles( origin, dir, 600, 2000, 10, 0.5, 275, 125, 0.25,
											   cgs.media.dirtParticle1Shader );
				}

				for ( i = 0; i < 5; i++ )
				{
					for ( j = 0; j < 3; j++ )
					{
						sprOrg[j] = origin[j] + 64 * dir[j] + 24 * crandom();
					}

					sprVel[2] += rand() % 50;
					CG_ParticleExplosion( "blacksmokeanim", sprOrg, sprVel, 3500 + rand() % 250, 10, 250 + rand() % 60,
										  qfalse );
				}

				VectorMA( origin, 24, dir, sprOrg );
				VectorScale( dir, 64, sprVel );
				CG_ParticleExplosion( "explode1", sprOrg, sprVel, 1000, 20, 300, qtrue );
			}
			break;
		case WP_DYNAMITE:
			shader = cgs.media.rocketExplosionShader;
			sfx = cgs.media.sfx_dynamiteexp;
			sfx2 = cgs.media.sfx_dynamiteexpDist;
			sfx2range = 400;
			mark = cgs.media.burnMarkShader;
			markDuration = cg_markTime.integer * 3;
			radius = 128;													// ydnar: bigger mark radius
			light = 300;
			isSprite = qtrue;
			duration = 1000;
			lightColor[0] = 0.75;
			lightColor[1] = 0.5;
			lightColor[2] = 0.1;

			// JPW NERVE
			// biggie dynamite explosions that mean it -- dynamite is biggest explode, so it gets extra crap thrown on
			// check for water/dirt spurt
			if ( CG_PointContents( origin, 0) & CONTENTS_WATER )
			{
				VectorCopy( origin, tmpv );
				tmpv[2] += 10000;
				trap_CM_BoxTrace( &trace, tmpv, origin, NULL, NULL, 0, MASK_WATER );
				CG_WaterRipple( cgs.media.wakeMarkShaderAnim, trace.endpos, dir, 300, 2000 );
				CG_AddDirtBulletParticles( trace.endpos, dir, 400 + random() * 200, 900, 15, 0.5, 512, 128, 0.125,
										   cgs.media.dirtParticle2Shader );
				CG_AddDirtBulletParticles( trace.endpos, dir, 400 + random() * 600, 1400, 15, 0.5, 128, 512, 0.125,
										   cgs.media.dirtParticle2Shader );
			}
			else
			{
				VectorSet( tmpv, origin[0], origin[1], origin[2] + 20 );
				VectorSet( tmpv2, origin[0], origin[1], origin[2] - 20 );
				trap_CM_BoxTrace( &trace, tmpv, tmpv2, NULL, NULL, 0, MASK_SHOT );
				if ( trace.surfaceFlags & SURF_GRASS || trace.surfaceFlags & SURF_GRAVEL )
				{
					CG_AddDirtBulletParticles( origin, dir, 400 + random() * 200, 3000, 10, 0.5, 400, 256, 0.25,
											   cgs.media.dirtParticle1Shader );
				}

				for ( i = 0; i < 3; i++ )
				{
					for ( j = 0; j < 3; j++ )
					{
						sprOrg[j] = origin[j] + 150 * crandom();
						sprVel[j] = 0.35 * crandom();
					}

					VectorAdd( sprVel, trace.plane.normal, sprVel );
					VectorScale( sprVel, 130, sprVel );
					CG_ParticleExplosion( "blacksmokeanim", sprOrg, sprVel, 6000 + random() * 2000, 40, 400 + random() * 200,
										  qfalse );							// JPW NERVE was blacksmokeanimb
				}

				for ( i = 0; i < 4; i++ )
				{															// JPW random vector based on plane normal so explosions move away from walls/dirt/etc
					for ( j = 0; j < 3; j++ )
					{
						sprOrg[j] = origin[j] + 100 * crandom();
						sprVel[j] = 0.65 * crandom();						// wider fireball spread
					}

					VectorAdd( sprVel, trace.plane.normal, sprVel );
					VectorScale( sprVel, random() * 100 + 300, sprVel );
					CG_ParticleExplosion( "explode1", sprOrg, sprVel, 1000 + rand() % 1450, 40, 400 + random() * 200,
										  (i == 0 ? qtrue : qfalse) );
				}

				CG_AddDebris( origin, dir, 400 + random() * 200, rand() % 2000 + 1400, 12 + rand() % 12 );
			}
			break;
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_GPG40:
		case WP_M7:
		case WP_SATCHEL:
		case WP_LANDMINE:
		case WP_MORTAR_SET:
			sfx2range = 1200;
			if ( weapon == WP_GPG40 || weapon == WP_M7 )
			{
				sfx2range = 800;
			}

			if ( weapon == WP_SATCHEL )
			{
				sfx = cgs.media.sfx_satchelexp;
				sfx2 = cgs.media.sfx_satchelexpDist;
			}
			else if ( weapon == WP_LANDMINE )
			{
				sfx = cgs.media.sfx_landmineexp;
				sfx2 = cgs.media.sfx_landmineexpDist;
			}
			else if ( weapon == WP_MORTAR_SET )
			{
				sfx = sfx2 = 0;
			}
			else if
				(
					weapon == WP_GRENADE_LAUNCHER ||
					weapon == WP_GRENADE_PINEAPPLE ||
					weapon == WP_GPG40 ||
					weapon == WP_M7
				)
			{
				sfx = cgs.media.sfx_grenexp;
				sfx2 = cgs.media.sfx_grenexpDist;
			}
			else
			{
				sfx = cgs.media.sfx_rockexp;
				sfx2 = cgs.media.sfx_rockexpDist;
			}

			shader = cgs.media.rocketExplosionShader;						// copied from RL
			sfx2range = 400;
			mark = cgs.media.burnMarkShader;
			markDuration = cg_markTime.integer * 3;
			radius = 64;
			light = 300;
			isSprite = qtrue;
			duration = 1000;
			lightColor[0] = 0.75;
			lightColor[1] = 0.5;
			lightColor[2] = 0.1;

			// Ridah, explosion sprite animation
			VectorMA( origin, 16, dir, sprOrg );
			VectorScale( dir, 100, sprVel );
			if ( CG_PointContents( origin, 0) & CONTENTS_WATER )
			{
				sfx = cgs.media.sfx_rockexpWater;
				VectorCopy( origin, tmpv );
				tmpv[2] += 10000;
				trap_CM_BoxTrace( &trace, tmpv, origin, NULL, NULL, 0, MASK_WATER );
				CG_WaterRipple( cgs.media.wakeMarkShaderAnim, trace.endpos, dir, 150, 1000 );
				CG_AddDirtBulletParticles( trace.endpos, dir, 400, 900, 15, 0.5, 256, 128, 0.125,
										   cgs.media.dirtParticle2Shader );
			}
			else
			{
				VectorCopy( origin, tmpv );
				tmpv[2] += 20;
				VectorCopy( origin, tmpv2 );
				tmpv2[2] -= 20;
				trap_CM_BoxTrace( &trace, tmpv, tmpv2, NULL, NULL, 0, MASK_SHOT );
				if ( trace.surfaceFlags & SURF_GRASS || trace.surfaceFlags & SURF_GRAVEL )
				{
					CG_AddDirtBulletParticles( origin, dir, 400, 2000, 10, 0.5, 200, 75, 0.25, cgs.media.dirtParticle1Shader );
				}

				CG_ParticleExplosion( "explode1", sprOrg, sprVel, 700, 60, 240, qtrue );
				CG_AddDebris( origin, dir, 280, 1400, 7 + rand() % 2 );
			}
			break;
		case WP_PANZERFAUST:
		case VERYBIGEXPLOSION:
		case WP_ARTY:
		case WP_SMOKE_MARKER:
			sfx = cgs.media.sfx_rockexp;
			sfx2 = cgs.media.sfx_rockexpDist;
			if ( weapon == VERYBIGEXPLOSION || weapon == WP_ARTY )
			{
				sfx = cgs.media.sfx_artilleryExp[rand() % 3];
				sfx2 = cgs.media.sfx_artilleryDist;
			}
			else if ( weapon == WP_SMOKE_MARKER )
			{
				sfx = cgs.media.sfx_airstrikeExp[rand() % 3];
				sfx2 = cgs.media.sfx_airstrikeDist;
			}

			sfx2range = 800;
			mark = cgs.media.burnMarkShader;
			markDuration = cg_markTime.integer * 3;
			radius = 128;													// ydnar: bigger mark radius
			light = 600;
			isSprite = qtrue;
			duration = 1000;

			// Ridah, changed to flamethrower colors
			lightColor[0] = 0.75;
			lightColor[1] = 0.5;
			lightColor[2] = 0.1;

			// explosion sprite animation
			VectorMA( origin, 24, dir, sprOrg );
			VectorScale( dir, 64, sprVel );
			if ( CG_PointContents( origin, 0) & CONTENTS_WATER )
			{
				VectorCopy( origin, tmpv );
				tmpv[2] += 10000;
				trap_CM_BoxTrace( &trace, tmpv, origin, NULL, NULL, 0, MASK_WATER );
				CG_WaterRipple( cgs.media.wakeMarkShaderAnim, trace.endpos, dir, 300, 2000 );
				CG_AddDirtBulletParticles( trace.endpos, dir, 400 + random() * 200, 900, 15, 0.5, 512, 128, 0.125,
										   cgs.media.dirtParticle2Shader );
				CG_AddDirtBulletParticles( trace.endpos, dir, 400 + random() * 600, 1400, 15, 0.5, 128, 512, 0.125,
										   cgs.media.dirtParticle2Shader );
			}
			else
			{
				VectorCopy( origin, tmpv );
				tmpv[2] += 20;
				VectorCopy( origin, tmpv2 );
				tmpv2[2] -= 20;
				trap_CM_BoxTrace( &trace, tmpv, tmpv2, NULL, NULL, 0, MASK_SHOT );
				if ( trace.surfaceFlags & SURF_GRASS || trace.surfaceFlags & SURF_GRAVEL )
				{
					CG_AddDirtBulletParticles( origin, dir, 400 + random() * 200, 3000, 10, 0.5, 400, 256, 0.25,
											   cgs.media.dirtParticle1Shader );
				}

				CG_ParticleExplosion( "explode1", sprOrg, sprVel, 1600, 20, 200 + random() * 400, qtrue );
				for ( i = 0; i < 4; i++ )
				{															// JPW random vector based on plane normal so explosions move away from walls/dirt/etc
					for ( j = 0; j < 3; j++ )
					{
						sprOrg[j] = origin[j] + 50 * crandom();
						sprVel[j] = 0.35 * crandom();
					}

					VectorAdd( sprVel, trace.plane.normal, sprVel );
					VectorScale( sprVel, 300, sprVel );
					CG_ParticleExplosion( "explode1", sprOrg, sprVel, 1600, 40, 260 + rand() % 120, qfalse );
				}

				CG_AddDebris( origin, dir, 400 + random() * 200, rand() % 2000 + 1000, 5 + rand() % 5 );
			}
			break;
		default:
		case WP_FLAMETHROWER:
			return;
			break;
	}

	// done.
	if ( sfx )
	{
		trap_S_StartSoundVControl( origin, -1, CHAN_AUTO, sfx, volume );
	}

	if ( sfx2 )
	{																		// distant sounds for weapons with a broadcast fire sound (so you /always/ hear dynamite explosions)
		vec3_t	porg, gorg, norm;											// player/gun origin
		float	gdist;
		VectorCopy( origin, gorg );
		VectorCopy( cg.refdef_current->vieworg, porg );
		VectorSubtract( gorg, porg, norm );
		gdist = VectorNormalize( norm );
		if ( gdist > 1200 && gdist < 8000 )
		{																	// 1200 is max cam shakey dist (2*600) use gorg as the new sound origin
			VectorMA( cg.refdef_current->vieworg, sfx2range, norm, gorg );	// JPW NERVE non-distance falloff makes more sense; sfx2range was gdist*0.2

			// sfx2range is variable to give us minimum volume control different explosion sizes (see mortar, panzerfaust, and grenade)
			trap_S_StartSoundEx( gorg, -1, CHAN_WEAPON, sfx2, SND_NOCUT );
		}
	}

	if ( mod )
	{
		le = CG_MakeExplosion( origin, dir, mod, shader, duration, isSprite );
		le->light = light;
		le->lightOverdraw = lightOverdraw;
		VectorCopy( lightColor, le->lightColor );
	}

	// ydnar: omnidirectional explosion marks
	if ( mark == cgs.media.burnMarkShader )
	{
		VectorSet( projection, 0, 0, -1 );
		projection[3] = radius;
		Vector4Set( color, 1.0f, 1.0f, 1.0f, 1.0f );
		trap_R_ProjectDecal( mark, 1, (vec3_t *) origin, projection, color, markDuration, (markDuration >> 4) );
	}
	else if ( mark )
	{
		VectorSubtract( vec3_origin, dir, projection );
		projection[3] = radius * 32;
		VectorMA( origin, -16.0f, projection, markOrigin );

		// jitter markorigin a bit so they don't end up on an ordered grid
		markOrigin[0] += ( random() - 0.5f );
		markOrigin[1] += ( random() - 0.5f );
		markOrigin[2] += ( random() - 0.5f );
		CG_ImpactMark( mark, markOrigin, projection, radius, random() * 360.0f, 1.0f, 1.0f, 1.0f, 1.0f, markDuration );
	}
}


/*
==============
CG_SMOKEY
==============
*/
void
CG_Smokey ( int clientNum, vec3_t origin, vec3_t dir )
{
	qhandle_t		mod;
	qhandle_t		mark;
	qhandle_t		shader;
	sfxHandle_t		sfx;
	float			radius;
	float			light;
	vec3_t			lightColor;
	localEntity_t	*le;
	qboolean		isSprite;
	int				duration;
	int				lightOverdraw;
	vec3_t			sprOrg, sprVel;
	vec4_t			projection, color;
	mark = 0;
	radius = 32;
	sfx = 0;
	mod = 0;
	shader = 0;
	light = 0;
	lightColor[0] = 1;
	lightColor[1] = 1;
	lightColor[2] = 0;

	// Ridah
	lightOverdraw = 0;

	// set defaults
	isSprite = qfalse;
	duration = 600;
	shader = cgs.media.panzerfaustsmokeParticleShader;								// copied from RL
	sfx = cgs.media.sfx_flakimpact;
	mark = cgs.media.bulletMarkShader;
	radius = 80;
	light = 0;
	isSprite = qtrue;
	duration = 1000;
	lightColor[0] = 0.0;
	lightColor[1] = 0.0;
	lightColor[2] = 0.0;

	// Ridah, explosion sprite animation
	VectorMA( origin, 16, dir, sprOrg );
	VectorScale( dir, 64, sprVel );

	// Ridah, throw some debris
	CG_AddDebris( origin, dir, 280, // speed
	1400, // duration
	1 + rand() % 2 );	 // count
	if ( sfx )
	{
		trap_S_StartSound( origin, -1, CHAN_AUTO, sfx );
	}

	//
	// create the explosion
	//
	//if ( mod ) {
	le = CG_MakeExplosion( origin, dir, mod, shader, duration, isSprite );
	le->light = light;

	// Ridah
	le->lightOverdraw = lightOverdraw;
	VectorCopy( lightColor, le->lightColor );

	//
	// impact mark
	//
	// ydnar: testing omnidirectional marks
	VectorSet( projection, 0, 0, -1 );
	projection[3] = radius;
	Vector4Set( color, 1.0f, 1.0f, 1.0f, 1.0f );
}


/*
==============
CG_MissileHitWallSmall
==============
*/
void
CG_MissileHitWallSmall ( int weapon, int clientNum, vec3_t origin, vec3_t dir )
{
	qhandle_t		mod;
	qhandle_t		mark;
	qhandle_t		shader;
	sfxHandle_t		sfx;
	float			radius;
	float			light;
	vec3_t			lightColor;
	localEntity_t	*le;
	qboolean		isSprite;
	int				duration;
	int				lightOverdraw;
	vec3_t			sprOrg, sprVel;
	vec4_t			projection, color;
	mark = 0;
	radius = 32;
	sfx = 0;
	mod = 0;
	shader = 0;
	light = 0;
	lightColor[0] = 1;
	lightColor[1] = 1;
	lightColor[2] = 0;

	// Ridah
	lightOverdraw = 0;

	// set defaults
	isSprite = qfalse;
	duration = 600;
	shader = cgs.media.rocketExplosionShader;	// copied from RL
	sfx = cgs.media.sfx_rockexp;
	mark = cgs.media.burnMarkShader;
	radius = 80;
	light = 300;
	isSprite = qtrue;
	duration = 1000;
	lightColor[0] = 0.75;
	lightColor[1] = 0.5;
	lightColor[2] = 0.1;

	// Ridah, explosion sprite animation
	VectorMA( origin, 16, dir, sprOrg );
	VectorScale( dir, 64, sprVel );
	CG_ParticleExplosion( "explode1", sprOrg, sprVel, 600, 6, 50, qtrue );

	// Ridah, throw some debris
	CG_AddDebris( origin, dir, 280, // speed
	1400, // duration
	// 15 + rand()%5 );	// count
	7 + rand() % 2 );	// count
	if ( sfx )
	{
		trap_S_StartSound( origin, -1, CHAN_AUTO, sfx );
	}

	//
	// create the explosion
	//
	if ( mod )
	{
		le = CG_MakeExplosion( origin, dir, mod, shader, duration, isSprite );
		le->light = light;

		// Ridah
		le->lightOverdraw = lightOverdraw;
		VectorCopy( lightColor, le->lightColor );
	}

	//
	// impact mark
	//
	// ydnar: testing omnidirectional marks
#if 0
	VectorSubtract( vec3_origin, dir, projection );
	projection[3] = radius * 3;
	VectorMA( origin, -4.0f, projection, markOrigin );
	CG_ImpactMark( mark, markOrigin, projection, radius, random() * 360.0f, 1.0f, 1.0f, 1.0f, 1.0f, cg_markTime.integer );
#else
	VectorSet( projection, 0, 0, -1 );
	projection[3] = radius;
	Vector4Set( color, 1.0f, 1.0f, 1.0f, 1.0f );
	trap_R_ProjectDecal( mark, 1, (vec3_t *) origin, projection, color, cg_markTime.integer, (cg_markTime.integer >> 4) );
#endif
}


/*
=================
CG_MissileHitPlayer
=================
*/
void
CG_MissileHitPlayer ( centity_t *cent, int weapon, vec3_t origin, vec3_t dir, int entityNum )
{
	CG_Bleed( origin, entityNum );

	// some weapons will make an explosion with the blood, while
	// others will just make the blood
	switch ( weapon )
	{
		case WP_GRENADE_LAUNCHER:
		case WP_PANZERFAUST:
			CG_MissileHitWall( weapon, 0, origin, dir, 0 ); // JPW NERVE like the old one
			break;
		case WP_KNIFE:
			CG_MissileHitWall( weapon, 0, origin, dir, 1 ); // JPW NERVE this one makes the hitting fleshy sound. whee
			break;
		default:
			break;
	}
}

/*
============================================================================

VENOM GUN TRACING

============================================================================
*/

//----(SA)	all changes to venom below should be mine
#define DEFAULT_VENOM_COUNT 10

//#define DEFAULT_VENOM_SPREAD 20
//#define DEFAULT_VENOM_SPREAD 400
#define DEFAULT_VENOM_SPREAD	700

/*
============================================================================

BULLETS

============================================================================
*/


/*
===============
CG_SpawnTracer
===============
*/
void
CG_SpawnTracer ( int sourceEnt, vec3_t pstart, vec3_t pend )
{
	localEntity_t	*le;
	float			dist;
	vec3_t			dir, ofs;
	orientation_t	or;
	vec3_t			start, end;
	VectorCopy( pstart, start );
	VectorCopy( pend, end );

	// DHM - make MG42 tracers line up
	if
	(
		cg_entities[sourceEnt].currentState.eFlags & EF_MG42_ACTIVE ||
		cg_entities[sourceEnt].currentState.eFlags & EF_BROWNING_ACTIVE
	)
	{
		start[2] -= 42;
	}

	VectorSubtract( end, start, dir );
	dist = VectorNormalize( dir );
	if ( dist < 2.0 * cg_tracerLength.value )
	{
		return; // segment isnt long enough, dont bother
	}

	if ( sourceEnt < cgs.maxclients )
	{

		// for visual purposes, find the actual tag_weapon for this client
		// and offset the start and end accordingly
		if
		(
			!(cg_entities[sourceEnt].currentState.eFlags & EF_MG42_ACTIVE) &&
			!(cg_entities[sourceEnt].currentState.eFlags & EF_BROWNING_ACTIVE) &&
			!(cg_entities[sourceEnt].currentState.eFlags & EF_AAGUN_ACTIVE)
		)
		{		// not MG42
			if ( CG_GetWeaponTag( sourceEnt, "tag_flash", &or) )
			{
				VectorSubtract( or.origin, start, ofs );
				if ( VectorLength( ofs) < 64 )
				{
					VectorAdd( start, ofs, start );
				}
			}
		}
	}

	// subtract the length of the tracer from the end point, so we dont go through the end point
	VectorMA( end, -cg_tracerLength.value, dir, end );
	dist = VectorDistance( start, end );
	le = CG_AllocLocalEntity();
	le->leType = LE_MOVING_TRACER;
	le->startTime = cg.time - ( cg.frametime ? (rand() % cg.frametime) / 2 : 0 );
	le->endTime = le->startTime + 1000.0 * dist / cg_tracerSpeed.value;
	le->pos.trType = TR_LINEAR;
	le->pos.trTime = le->startTime;
	VectorCopy( start, le->pos.trBase );
	VectorScale( dir, cg_tracerSpeed.value, le->pos.trDelta );
}


/*
===============
CG_DrawTracer
===============
*/
void
CG_DrawTracer ( vec3_t start, vec3_t finish )
{
	vec3_t		forward, right;
	polyVert_t	verts[4];
	vec3_t		line;
	VectorSubtract( finish, start, forward );
	line[0] = DotProduct( forward, cg.refdef_current->viewaxis[1] );
	line[1] = DotProduct( forward, cg.refdef_current->viewaxis[2] );
	VectorScale( cg.refdef_current->viewaxis[1], line[1], right );
	VectorMA( right, -line[0], cg.refdef_current->viewaxis[2], right );
	VectorNormalize( right );
	VectorMA( finish, cg_tracerWidth.value, right, verts[0].xyz );
	verts[0].st[0] = 1;
	verts[0].st[1] = 1;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;
	VectorMA( finish, -cg_tracerWidth.value, right, verts[1].xyz );
	verts[1].st[0] = 1;
	verts[1].st[1] = 0;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;
	VectorMA( start, -cg_tracerWidth.value, right, verts[2].xyz );
	verts[2].st[0] = 0;
	verts[2].st[1] = 0;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;
	VectorMA( start, cg_tracerWidth.value, right, verts[3].xyz );
	verts[3].st[0] = 0;
	verts[3].st[1] = 1;
	verts[3].modulate[0] = 255;
	verts[3].modulate[1] = 255;
	verts[3].modulate[2] = 255;
	verts[3].modulate[3] = 255;
	trap_R_AddPolyToScene( cgs.media.tracerShader, 4, verts );
}


/*
===============
CG_Tracer
===============
*/
void
CG_Tracer ( vec3_t source, vec3_t dest, int sparks )
{
	float	len, begin, end;
	vec3_t	start, finish;
	vec3_t	midpoint;
	vec3_t	forward;

	// tracer
	VectorSubtract( dest, source, forward );
	len = VectorNormalize( forward );

	// start at least a little ways from the muzzle
	if ( len < 100 && !sparks )
	{
		return;
	}

	begin = 50 + random() * ( len - 60 );
	end = begin + cg_tracerLength.value;
	if ( end > len )
	{
		end = len;
	}

	VectorMA( source, begin, forward, start );
	VectorMA( source, end, forward, finish );
	CG_DrawTracer( start, finish );
	midpoint[0] = ( start[0] + finish[0] ) * 0.5;
	midpoint[1] = ( start[1] + finish[1] ) * 0.5;
	midpoint[2] = ( start[2] + finish[2] ) * 0.5;
}

/*
======================
CG_CalcMuzzlePoint
======================
*/
#if 0


/* */
qboolean
CG_CalcMuzzlePoint ( int entityNum, vec3_t muzzle )
{
	vec3_t		forward, right, up;
	centity_t	*cent;
	if ( entityNum == cg.snap->ps.clientNum && !(cg.snap->ps.eFlags & EF_TANK) )
	{

		// Arnout: see if we're attached to a gun
		if ( cg.snap->ps.eFlags & EF_MG42_ACTIVE || cg.snap->ps.eFlags & EF_BROWNING_ACTIVE )
		{
			centity_t	*mg42 = &cg_entities[cg.snap->ps.viewlocked_entNum];
			vec3_t		forward;
			AngleVectors( cg.snap->ps.viewangles, forward, NULL, NULL );
			VectorMA( mg42->currentState.pos.trBase, 40, forward, muzzle ); // wsa -36, made 40 to be in sync with the actual muzzleflash drawing
			muzzle[2] += cg.snap->ps.viewheight;
		}
		else if ( cg.snap->ps.eFlags & EF_AAGUN_ACTIVE )
		{
			centity_t	*aagun = &cg_entities[cg.snap->ps.viewlocked_entNum];
			vec3_t		forward, right, up;
			AngleVectors( cg.snap->ps.viewangles, forward, right, up );
			VectorCopy( aagun->lerpOrigin, muzzle );						// Gordon: modelindex2 will already have been incremented on the server, so work out what it WAS then
			BG_AdjustAAGunMuzzleForBarrel( muzzle, forward, right, up, (aagun->currentState.modelindex2 + 3) % 4 );
		}
		else if ( cg.snap->ps.eFlags & EF_MOUNTEDTANK || cg_firstPerson.integer )
		{
			if ( cg.renderingThirdPerson )
			{
				centity_t	*tank = &cg_entities[cg_entities[cg.snap->ps.clientNum].tagParent];
				VectorCopy( tank->mountedMG42Flash.origin, muzzle );
				AngleVectors( cg.snap->ps.viewangles, forward, NULL, NULL );
				VectorMA( muzzle, 14, forward, muzzle );
			}
			else
			{

				//bani - fix firstperson tank muzzle origin if drawgun is off
				if ( !cg_drawGun.integer )
				{
					VectorCopy( cg.snap->ps.origin, muzzle );
					AngleVectors( cg.snap->ps.viewangles, forward, right, up );
					VectorMA( muzzle, 48, forward, muzzle );
					muzzle[2] += cg.snap->ps.viewheight;
					VectorMA( muzzle, 8, right, muzzle );
				}
				else
				{
					VectorCopy( cg.tankflashorg, muzzle );
				}
			}
		}
		else
		{
			vec3_t	forward, right, up;
			float	horizontal;
			cent = &cg_entities[cg.snap->ps.clientNum];
			VectorCopy( cg.snap->ps.origin, muzzle );
			muzzle[2] += cg.snap->ps.viewheight;
			AngleVectors( cg.snap->ps.viewangles, forward, right, up );
			if ( cent->currentState.constantLight > 0 )
			{
				horizontal = ( cent->currentState.constantLight / 1000 ) * ( -0.33 );	//cg.predictedPlayerState.leanf * (-0.33);
			}
			else
			{
				horizontal = ( cent->currentState.constantLight / 1000 ) * 0.33;		//cg.predictedPlayerState.leanf * 0.33;
			}

			VectorMA( muzzle, horizontal, up, muzzle );
			VectorMA( muzzle,															/*cg.predictedPlayerState.leanf*/
					 (cent->currentState.constantLight / 1000) * 1.1, right, muzzle );
			if ( cg.snap->ps.weapon == WP_MOBILE_MG42_SET || cg.snap->ps.weapon == WP_30CAL_SET )
			{
				VectorMA( muzzle, 36, forward, muzzle );
			}
			else
			{
				VectorMA( muzzle, 14, forward, muzzle );
			}
		}

		return ( qtrue );
	}

	cent = &cg_entities[entityNum];
	if ( cent->currentState.eFlags & EF_MG42_ACTIVE || cent->currentState.eFlags & EF_BROWNING_ACTIVE )
	{
		vec3_t	forward;

		// ydnar: this is silly--the entity is a mg42 barrel, so just use itself
		if ( cent->currentState.eType == ET_MG42_BARREL )
		{
			VectorCopy( cent->currentState.pos.trBase, muzzle );
			AngleVectors( cent->lerpAngles, forward, NULL, NULL );
			VectorMA( muzzle, 40, forward, muzzle );
			muzzle[2] += DEFAULT_VIEWHEIGHT;
		}
	}
	else if ( cent->currentState.eFlags & EF_MOUNTEDTANK )
	{
		centity_t	*tank = &cg_entities[cent->tagParent];
		VectorCopy( tank->mountedMG42Flash.origin, muzzle );
	}
	else if ( cent->currentState.eFlags & EF_TANK )
	{
		VectorCopy( cent->currentState.pos.trBase, muzzle );
		AngleVectors( cent->lerpAngles, forward, right, NULL );
		VectorMA( muzzle, 64, forward, muzzle );
		if ( cent->currentState.effect1Time == 0 )
		{
			VectorMA( muzzle, -16, right, muzzle );
		}
		else
		{
			VectorMA( muzzle, 32, right, muzzle );
		}

		muzzle[2] += 48;
	}
	else if ( cent->currentState.eFlags & EF_AAGUN_ACTIVE )
	{
		centity_t	*aagun;
		int			num;

		// find the mg42 we're attached to
		for ( num = 0; num < cg.snap->numEntities; num++ )
		{
			aagun = &cg_entities[cg.snap->entities[num].number];
			if
			(
				aagun->currentState.eType == ET_AAGUN &&
				aagun->currentState.otherEntityNum == cent->currentState.number
			)
			{

				// found it
				vec3_t	forward, right, up;
				AngleVectors( cg.snap->ps.viewangles, forward, right, up );
				VectorCopy( aagun->lerpOrigin, muzzle );								// Gordon: modelindex2 will already have been incremented on the server, so work out what it WAS then
				BG_AdjustAAGunMuzzleForBarrel( muzzle, forward, right, up, (aagun->currentState.modelindex2 + 3) % 4 );
			}
		}
	}
	else
	{
		vec3_t	forward, right, up;
		float	horizontal;
		AngleVectors( cg.snap->ps.viewangles, forward, right, up );
		VectorCopy( cent->currentState.pos.trBase, muzzle );

		//	if (cg.predictedPlayerState.leanf > 0){
		if ( cent->currentState.constantLight > 0 )
		{
			horizontal = ( cent->currentState.constantLight / 1000 ) * ( -0.35 );		//cg.predictedPlayerState.leanf * (-0.35); // fit it !
		}
		else
		{
			horizontal = ( cent->currentState.constantLight / 1000 ) * 0.35;			//cg.predictedPlayerState.leanf * 0.35;
		}

		VectorMA( muzzle, horizontal, up, muzzle );
		VectorMA( muzzle,																/*cg.predictedPlayerState.leanf*/
				 (cent->currentState.constantLight / 1000) * 1.1, right, muzzle );
		if ( cent->currentState.eFlags & EF_PRONE )
		{
			muzzle[2] += PRONE_VIEWHEIGHT;
			if ( cent->currentState.weapon == WP_MOBILE_MG42_SET || cent->currentState.weapon == WP_30CAL_SET )
			{
				VectorMA( muzzle, 36, forward, muzzle );
			}
			else
			{
				VectorMA( muzzle, 14, forward, muzzle );
			}
		}
		else
		{
			muzzle[2] += DEFAULT_VIEWHEIGHT;
			VectorMA( muzzle, 14, forward, muzzle );
		}
	}

	return ( qfalse );
}
#endif

/* */
qboolean
CG_CalcMuzzlePoint ( int entityNum, vec3_t muzzle )
{
	vec3_t		forward, right, up;
	centity_t	*cent;
	float		horizontal;
	if ( entityNum == cg.snap->ps.clientNum && !(cg.snap->ps.eFlags & EF_TANK) )
	{

		// Arnout: see if we're attached to a gun
		if ( cg.snap->ps.eFlags & EF_MG42_ACTIVE )
		{
			centity_t	*mg42 = &cg_entities[cg.snap->ps.viewlocked_entNum];
			vec3_t		forward;
			AngleVectors( cg.snap->ps.viewangles, forward, NULL, NULL );
			VectorMA( mg42->currentState.pos.trBase, 40, forward, muzzle ); // wsa -36, made 40 to be in sync with the actual muzzleflash drawing
			muzzle[2] += cg.snap->ps.viewheight;
		}
		else if ( cg.snap->ps.eFlags & EF_AAGUN_ACTIVE )
		{
			centity_t	*aagun = &cg_entities[cg.snap->ps.viewlocked_entNum];
			vec3_t		forward, right, up;
			AngleVectors( cg.snap->ps.viewangles, forward, right, up );
			VectorCopy( aagun->lerpOrigin, muzzle );						// Gordon: modelindex2 will already have been incremented on the server, so work out what it WAS then
			BG_AdjustAAGunMuzzleForBarrel( muzzle, forward, right, up, (aagun->currentState.modelindex2 + 3) % 4 );
		}
		else if ( cg.snap->ps.eFlags & EF_MOUNTEDTANK )
		{
			if ( cg.renderingThirdPerson || cg_firstPerson.integer )
			{
				centity_t	*tank = &cg_entities[cg_entities[cg.snap->ps.clientNum].tagParent];
				VectorCopy( tank->mountedMG42Flash.origin, muzzle );
				AngleVectors( cg.snap->ps.viewangles, forward, NULL, NULL );
				VectorMA( muzzle, 14, forward, muzzle );
			}
			else
			{

				//bani - fix firstperson tank muzzle origin if drawgun is off
				if ( !cg_drawGun.integer )
				{
					VectorCopy( cg.snap->ps.origin, muzzle );
					AngleVectors( cg.snap->ps.viewangles, forward, right, up );
					VectorMA( muzzle, 48, forward, muzzle );
					muzzle[2] += cg.snap->ps.viewheight;
					VectorMA( muzzle, 8, right, muzzle );
				}
				else
				{
					VectorCopy( cg.tankflashorg, muzzle );
				}
			}
		}
		else
		{
			VectorCopy( cg.snap->ps.origin, muzzle );
			muzzle[2] += cg.snap->ps.viewheight;
			AngleVectors( cg.snap->ps.viewangles, forward, right, up );
			horizontal = fabs( cg.snap->ps.leanf * 0.35 );
			VectorMA( muzzle, horizontal, up, muzzle );
			VectorMA( muzzle, cg.snap->ps.leanf * 1.1, right, muzzle );
			if ( cg.snap->ps.weapon == WP_MOBILE_MG42_SET )
			{
				VectorMA( muzzle, 36, forward, muzzle );
			}
			else
			{
				VectorMA( muzzle, 14, forward, muzzle );
			}
		}

		return ( qtrue );
	}

	cent = &cg_entities[entityNum];

	//----(SA)	removed check.  is this still necessary?  (this way works for ai's firing mg42)  should I check for mg42?
	//	if ( !cent->currentValid ) {	
	//		return qfalse;
	//	}
	//----(SA)	end
	if ( cent->currentState.eFlags & EF_MG42_ACTIVE )
	{

		//		centity_t	*mg42;
		//		int			num;
		vec3_t	forward;

		// ydnar: this is silly--the entity is a mg42 barrel, so just use itself
#if 0

		// find the mg42 we're attached to
		for ( num = 0; num < cg.snap->numEntities; num++ )
		{
			mg42 = &cg_entities[cg.snap->entities[num].number];
			if ( mg42->currentState.eType == ET_MG42_BARREL )
			{
				if ( mg42->currentState.number == cent->currentState.number )
				{

					// found it
					VectorCopy( mg42->currentState.pos.trBase, muzzle );
					AngleVectors( cent->lerpAngles, forward, NULL, NULL );

					//VectorMA( muzzle, -36, forward, muzzle );
					VectorMA( muzzle, 40, forward, muzzle );
					muzzle[2] += DEFAULT_VIEWHEIGHT;
					break;
				}
			}
		}

#else
		if ( cent->currentState.eType == ET_MG42_BARREL )
		{
			VectorCopy( cent->currentState.pos.trBase, muzzle );
			AngleVectors( cent->lerpAngles, forward, NULL, NULL );
			VectorMA( muzzle, 40, forward, muzzle );
			muzzle[2] += DEFAULT_VIEWHEIGHT;
		}
#endif
	}
	else if ( cent->currentState.eFlags & EF_MOUNTEDTANK )
	{
		centity_t	*tank = &cg_entities[cent->tagParent];
		VectorCopy( tank->mountedMG42Flash.origin, muzzle );
	}
	else if ( cent->currentState.eFlags & EF_TANK )
	{
		VectorCopy( cent->currentState.pos.trBase, muzzle );
		AngleVectors( cent->lerpAngles, forward, right, NULL );
		VectorMA( muzzle, 64, forward, muzzle );
		if ( cent->currentState.effect1Time == 0 )
		{
			VectorMA( muzzle, -16, right, muzzle );
		}
		else
		{
			VectorMA( muzzle, 32, right, muzzle );
		}

		muzzle[2] += 48;
	}
	else if ( cent->currentState.eFlags & EF_AAGUN_ACTIVE )
	{
		centity_t	*aagun;
		int			num;

		// find the mg42 we're attached to
		for ( num = 0; num < cg.snap->numEntities; num++ )
		{
			aagun = &cg_entities[cg.snap->entities[num].number];
			if
			(
				aagun->currentState.eType == ET_AAGUN &&
				aagun->currentState.otherEntityNum == cent->currentState.number
			)
			{

				// found it
				vec3_t	forward, right, up;
				AngleVectors( cg.snap->ps.viewangles, forward, right, up );
				VectorCopy( aagun->lerpOrigin, muzzle );					// Gordon: modelindex2 will already have been incremented on the server, so work out what it WAS then
				BG_AdjustAAGunMuzzleForBarrel( muzzle, forward, right, up, (aagun->currentState.modelindex2 + 3) % 4 );
			}
		}
	}
	else
	{
		VectorCopy( cent->currentState.pos.trBase, muzzle );
		AngleVectors( cent->currentState.apos.trBase, forward, right, up );
		horizontal = fabs( (cent->currentState.constantLight / 1000) * 0.35 );
		VectorMA( muzzle, horizontal, up, muzzle );
		VectorMA( muzzle, (cent->currentState.constantLight / 1000) * 1.1, right, muzzle );
		if ( cent->currentState.eFlags & EF_PRONE )
		{
			muzzle[2] += PRONE_VIEWHEIGHT;
			if ( cent->currentState.weapon == WP_MOBILE_MG42_SET )
			{
				VectorMA( muzzle, 36, forward, muzzle );
			}
			else
			{
				VectorMA( muzzle, 14, forward, muzzle );
			}
		}
		else
		{
			muzzle[2] += DEFAULT_VIEWHEIGHT;
			VectorMA( muzzle, 14, forward, muzzle );
		}
	}

	return ( qtrue );
}


/* */
void
SnapVectorTowards ( vec3_t v, vec3_t to )
{
	int i;
	for ( i = 0; i < 3; i++ )
	{
		if ( to[i] <= v[i] )
		{
			v[i] = floor( v[i] );
		}
		else
		{
			v[i] = ceil( v[i] );
		}
	}
}


/*
======================
CG_Bullet

Renders bullet effects.
======================
*/
void
CG_Bullet
(
	vec3_t		end,
	int			sourceEntityNum,
	vec3_t		normal,
	qboolean	flesh,
	int			fleshEntityNum,
	int			otherEntNum2,
	float		waterfraction,
	int			seed,
	qboolean	kick
)
{
	trace_t		trace, trace2;
	int			sourceContentType, destContentType;
	vec3_t		dir;
	vec3_t		start, trend;						// JPW
	vec4_t		projection;
	static int	lastBloodSpat;
	centity_t	*cent;

	//	vec3_t green = {0, 1, 0};
	cent = &cg_entities[fleshEntityNum];

	// JPW NERVE -- don't ever shoot if we're binoced in // :S
	if
	(
		cg_entities[sourceEntityNum].currentState.eFlags & EF_ZOOMING &&
		cg_entities[sourceEntityNum].currentState.weapon == WP_BINOCULARS
	)
	{
		return;
	}

	// Arnout: snap tracers for MG42 to viewangle of client when antilag is enabled
	if
	(
		cgs.antilag &&
		otherEntNum2 == cg.snap->ps.clientNum &&
		(
			cg_entities[otherEntNum2].currentState.eFlags & EF_MG42_ACTIVE ||
			cg_entities[otherEntNum2].currentState.eFlags & EF_BROWNING_ACTIVE
		)
	)
	{
		vec3_t	muzzle, forward, right, up;
		float	r, u;
		trace_t tr;
		AngleVectors( cg.predictedPlayerState.viewangles, forward, right, up );
		VectorCopy( cg_entities[cg.snap->ps.viewlocked_entNum].currentState.pos.trBase, muzzle );
		if ( cg_entities[cg.snap->ps.viewlocked_entNum].currentState.onFireStart )
		{
			VectorMA( muzzle, 16, up, muzzle );
		}

		r = Q_crandom( &seed ) * MG42_SPREAD_MP;
		u = Q_crandom( &seed ) * MG42_SPREAD_MP;
		VectorMA( muzzle, 8192, forward, end );
		VectorMA( end, u, up, end );
		CG_Trace( &tr, muzzle, NULL, NULL, end, otherEntNum2, MASK_SHOT );
		SnapVectorTowards( tr.endpos, muzzle );
		VectorCopy( tr.endpos, end );
	}

	// if the shooter is currently valid, calc a source point and possibly
	// do trail effects
	if ( sourceEntityNum >= 0 && cg_tracerChance.value > 0 )
	{
		if ( CG_CalcMuzzlePoint( sourceEntityNum, start) )
		{
			sourceContentType = CG_PointContents( start, 0 );
			destContentType = CG_PointContents( end, 0 );

			// do a complete bubble trail if necessary
			if ( (sourceContentType == destContentType) && (sourceContentType & CONTENTS_WATER) )
			{
				CG_BubbleTrail( start, end, .5, 8 );
			}
			else if ( (sourceContentType & CONTENTS_WATER) )
			{										// bubble trail from water into air
				trap_CM_BoxTrace( &trace, end, start, NULL, NULL, 0, CONTENTS_WATER );
				CG_BubbleTrail( start, trace.endpos, .5, 8 );
			}
			else if ( (destContentType & CONTENTS_WATER) )
			{										// bubble trail from air into water
				// only add bubbles if effect is close to viewer
				if ( Distance( cg.snap->ps.origin, end) < 1024 )
				{
					trap_CM_BoxTrace( &trace, start, end, NULL, NULL, 0, CONTENTS_WATER );
					CG_BubbleTrail( end, trace.endpos, .5, 8 );
				}
			}

			// if not flesh, then do a moving tracer
			if ( flesh )
			{

				// draw a tracer
				if ( !kick && random() < cg_tracerChance.value )
				{
					CG_Tracer( start, end, 0 );
				}
			}
			else
			{										// (not flesh)
				if ( otherEntNum2 >= 0 && otherEntNum2 != ENTITYNUM_NONE )
				{
					CG_SpawnTracer( otherEntNum2, start, end );
				}
				else
				{
					CG_SpawnTracer( sourceEntityNum, start, end );
				}
			}
		}
	}

	// impact splash and mark
	if ( flesh )
	{
		vec3_t			origin;
		localEntity_t	*le;						// JPW NERVE
		float			rnd, tmpf;					// JPW NERVE
		vec3_t			smokedir, tmpv, tmpv2;		// JPW NERVE
		int				i, headshot;				// JPW NERVE
		if ( fleshEntityNum < MAX_CLIENTS )
		{
			CG_Bleed( end, fleshEntityNum );
		}

		// JPW NERVE smoke puffs (sometimes with some blood)
		VectorSubtract( end, start, smokedir );		// get a nice "through the body" vector
		VectorNormalize( smokedir );

		// all this to come up with a decent center-body displacement of bullet impact point
		VectorSubtract( cent->currentState.pos.trBase, end, tmpv );
		tmpv[2] = 0;
		tmpf = VectorLength( tmpv );
		VectorScale( smokedir, tmpf, tmpv );
		VectorAdd( end, tmpv, origin );

		// whee, got a bullet impact point projected to center body
		CG_GetOriginForTag( cent, &cent->pe.headRefEnt, "tag_mouth", 0, tmpv, NULL );
		tmpv[2] += 5;
		VectorSubtract( tmpv, origin, tmpv2 );
		headshot = ( VectorLength( tmpv2) < 10 );
		if ( headshot && cg_blood.integer )
		{
			for ( i = 0; i < 5; i++ )
			{
				rnd = random();
				VectorScale( smokedir, 25.0 + random() * 25, tmpv );
				tmpv[0] += crandom() * 25.0f;
				tmpv[1] += crandom() * 25.0f;
				tmpv[2] += crandom() * 25.0f;
				CG_GetWindVector( tmpv2 );
				VectorScale( tmpv2, 35, tmpv2 );	// was 75, before that 55
				tmpv2[2] = 0;
				VectorAdd( tmpv, tmpv2, tmpv );
				le = CG_SmokePuff( origin, tmpv, 5 + rnd * 10, 1, rnd * 0.8, rnd * 0.8, 0.5, 500 + (rand() % 800),
								   cg.time, 0, 0, cgs.media.fleshSmokePuffShader );
			}
		}
		else
		{

			// puff out the front (more dust no blood)
			for ( i = 0; i < 10; i++ )
			{
				rnd = random();
				VectorScale( smokedir, -35.0 + random() * 25, tmpv );
				tmpv[0] += crandom() * 25.0f;
				tmpv[1] += crandom() * 25.0f;
				tmpv[2] += crandom() * 25.0f;
				CG_GetWindVector( tmpv2 );
				VectorScale( tmpv2, 35, tmpv2 );	// was 75, before that 55
				tmpv2[2] = 0;
				VectorAdd( tmpv, tmpv2, tmpv );
				le = CG_SmokePuff( origin, tmpv, 5 + rnd * 10, rnd * 0.3f + 0.5f, rnd * 0.3f + 0.5f, rnd * 0.3f + 0.5f,
								   0.125f, 500 + (rand() % 300), cg.time, 0, 0, cgs.media.smokePuffShader );
			}
		}

		// jpw
		// play the bullet hit flesh sound
		// HACK, if this is not us getting hit, make it quieter // JPW NERVE pulled hack, we like loud impact sounds for MP
		if ( fleshEntityNum == cg.snap->ps.clientNum )
		{

			//CG_SoundPlayIndexedScript( cgs.media.bulletHitFleshScript, NULL, fleshEntityNum );
			trap_S_StartSound( NULL, fleshEntityNum, CHAN_BODY, cgs.media.sfx_bullet_stonehit[rand() % MAX_IMPACT_SOUNDS] );
		}
		else
		{

			//CG_SoundPlayIndexedScript( cgs.media.bulletHitFleshScript, cg_entities[fleshEntityNum].currentState.origin, ENTITYNUM_WORLD ); // JPW NERVE changed from ,origin, to this
			trap_S_StartSound( cg_entities[fleshEntityNum].currentState.origin, ENTITYNUM_WORLD, CHAN_BODY,
							   cgs.media.sfx_bullet_stonehit[rand() % MAX_IMPACT_SOUNDS] );
		}

		// if we haven't dropped a blood spat in a while, check if this is a good scenario
		if ( cg_blood.integer && (lastBloodSpat > cg.time || lastBloodSpat < cg.time - 500) )
		{
			vec4_t	color;
			if ( CG_CalcMuzzlePoint( sourceEntityNum, start) )
			{
				VectorSubtract( end, start, dir );
				VectorNormalize( dir );
				VectorMA( end, 128, dir, trend );
				trap_CM_BoxTrace( &trace, end, trend, NULL, NULL, 0, MASK_SHOT &~CONTENTS_BODY );
				if ( trace.fraction < 1 )
				{

					//%	CG_ImpactMark( cgs.media.bloodDotShaders[rand()%5], trace.endpos, trace.plane.normal, random()*360,
					//%		1,1,1,1, qtrue, 15+random()*20, qfalse, cg_bloodTime.integer * 1000 );
#if 0
					VectorSubtract( vec3_origin, dir, projection );
					projection[3] = 64;
					VectorMA( trace.endpos, -8.0f, projection, markOrigin );
					CG_ImpactMark( cgs.media.bloodDotShaders[rand() % 5], markOrigin, projection, 15.0f + random() * 20.0f,
								   360.0f * random(), 1.0f, 1.0f, 1.0f, 1.0f, cg_bloodTime.integer * 1000 );
#else
					VectorSet( projection, 0, 0, -1 );
					projection[3] = 15.0f + random() * 20.0f;
					Vector4Set( color, 1.0f, 1.0f, 1.0f, 1.0f );
					trap_R_ProjectDecal( cgs.media.bloodDotShaders[rand() % 5], 1, (vec3_t *) origin, projection, color,
										 cg_bloodTime.integer * 1000, (cg_bloodTime.integer * 1000) >> 4 );
#endif
					lastBloodSpat = cg.time;
				}
				else if ( lastBloodSpat < cg.time - 1000 )
				{

					// drop one on the ground?
					VectorCopy( end, trend );
					trend[2] -= 64;
					trap_CM_BoxTrace( &trace, end, trend, NULL, NULL, 0, MASK_SHOT &~CONTENTS_BODY );
					if ( trace.fraction < 1 )
					{

						//%	CG_ImpactMark( cgs.media.bloodDotShaders[rand()%5], trace.endpos, trace.plane.normal, random()*360,
						//%		1,1,1,1, qtrue, 15+random()*10, qfalse, cg_bloodTime.integer * 1000 );
#if 0
						VectorSubtract( vec3_origin, dir, projection );
						projection[3] = 64;
						VectorMA( trace.endpos, -8.0f, projection, markOrigin );
						CG_ImpactMark( cgs.media.bloodDotShaders[rand() % 5], markOrigin, projection,
									   15.0f + random() * 10.0f, 360.0f * random(), 1.0f, 1.0f, 1.0f, 1.0f,
									   cg_bloodTime.integer * 1000 );
#else
						VectorSet( projection, 0, 0, -1 );
						projection[3] = 15.0f + random() * 20.0f;
						Vector4Set( color, 1.0f, 1.0f, 1.0f, 1.0f );
						trap_R_ProjectDecal( cgs.media.bloodDotShaders[rand() % 5], 1, (vec3_t *) origin, projection,
											 color, cg_bloodTime.integer * 1000, (cg_bloodTime.integer * 1000) >> 4 );
#endif
						lastBloodSpat = cg.time;
					}
				}
			}
		}
	}
	else
	{												// (not flesh)
		// Gordon: all bullet weapons have the same fx, and this stops pvs issues causing grenade explosions
		int fromweap = WP_MP40;						// cg_entities[sourceEntityNum].currentState.weapon;

		// TODO: not sure what kind of effect were going to do
		if ( kick )
		{
			return;
		}

		if
		(
			!fromweap ||
			cg_entities[sourceEntityNum].currentState.eFlags & EF_MG42_ACTIVE ||
			cg_entities[sourceEntityNum].currentState.eFlags & EF_TANK ||
			cg_entities[sourceEntityNum].currentState.eFlags & EF_BROWNING_ACTIVE ||
			cg_entities[sourceEntityNum].currentState.eFlags & EF_MOUNTEDTANK
		)
		{											// mounted
			fromweap = WP_MP40;
		}

		if ( CG_CalcMuzzlePoint( sourceEntityNum, start) || cg.snap->ps.persistant[PERS_HWEAPON_USE] )
		{
			if ( waterfraction )
			{
				vec3_t	dist;
				vec3_t	end2;
				vec3_t	dir = { 0, 0, 1 };
				VectorSubtract( end, start, dist );
				VectorMA( start, waterfraction, dist, end2 );
				trap_S_StartSound( end, -1, CHAN_AUTO, cgs.media.sfx_bullet_waterhit[rand() % 5] );
				CG_MissileHitWall( fromweap, 2, end2, dir, 0 );

				//CG_MissileHitWall( fromweap, 1, end, normal, trace.surfaceFlags);
				CG_MissileHitWall( fromweap, 1, end, trace.plane.normal, 0 );
			}
			else
			{

				// Arnout: but this does!
				VectorSubtract( end, start, dir );
				VectorNormalizeFast( dir );
				VectorMA( end, 4, dir, end );

				//			if (cg_debugBullets.integer == 10){
				//				CG_RailTrail2( green, start, end );
				//			}
				CG_Trace( &trace, start, NULL, NULL, end, 0, MASK_SHOT );

				// JPW NERVE -- water check
				CG_Trace( &trace2, start, NULL, NULL, end, 0, MASK_WATER | MASK_SHOT );
				if ( trace.fraction != trace2.fraction )
				{

					//trap_CM_BoxTrace( &trace2, start, end, NULL, NULL, -1, MASK_WATER );
					trap_S_StartSound( end, -1, CHAN_AUTO, cgs.media.sfx_bullet_waterhit[rand() % 5] );
					CG_Trace( &trace2, start, NULL, NULL, end, -1, MASK_WATER );
					CG_MissileHitWall( fromweap, 2, trace2.endpos, trace2.plane.normal, trace2.surfaceFlags );
					return;
				}

				VectorSubtract( vec3_origin, dir, dir );
				CG_MissileHitWall( fromweap, 1, trace.endpos, dir, trace.surfaceFlags );

				//CG_MissileHitWall( fromweap, 1, end, normal, trace.surfaceFlags);	// smoke puff	//	(SA) modified to send missilehitwall surface parameters
				//%	CG_MissileHitWall( fromweap, 1, trace.endpos, trace.plane.normal, trace.surfaceFlags);	// smoke puff	//	(SA) modified to send missilehitwall surface parameters
				// ydnar: better bullet marks
				//				if (cg_debugBullets.integer == 10){
				//				CG_RailTrail2( green, start, trace.endpos );
				//				}
			}
		}
	}
}
