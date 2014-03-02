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
**
**	cg_atmospheric.c
**
**	Add atmospheric effects (e.g. rain, snow etc.) to view.
**
**	Current supported effects are rain and snow.
**
*/
#define ATM_NEW
#include "cg_local.h"
#define MAX_ATMOSPHERIC_HEIGHT	MAX_MAP_SIZE	// maximum world height
#define MIN_ATMOSPHERIC_HEIGHT	- MAX_MAP_SIZE	// minimum world height

extern int Q_TrueRand ( int low, int high );

vec3_t storm_position;
vec3_t storm_angles;

qboolean rain_initialized = qfalse;
qhandle_t mistshader1;
qhandle_t rainshader1;
qhandle_t rainshader2;

qhandle_t mistleftshader;
qhandle_t rainleftshader1;
qhandle_t rainleftshader2;

qhandle_t mistdownshader;
qhandle_t raindownshader1;
qhandle_t raindownshader2;

//qhandle_t mistquietshader;
//qhandle_t rainquietshader1;
//qhandle_t rainquietshader2;

qhandle_t rainSplash[8];

//int rain_quiet_time = 0;
int rain_left_end_time = 0;
int	next_rain_left_time = 0;

int rain_down_end_time = 0;
int	next_rain_down_time = 0;


void
CG_AddStormSprites ( void )
{
	byte			color[4];
	polyVert_t		verts[4];
	vec3_t			top, bottom;
	vec3_t			right, up, dir, angle, forward, up_pos;
	float			radius;
	float			halfSmokeSpriteWidth1, halfSmokeSpriteWidth2, halfSmokeSpriteHeight;
	trace_t			tr;

	radius = 640.f; // max radius
	halfSmokeSpriteHeight = 320;

	// Now render it
	VectorSubtract(storm_position, cg.refdef.vieworg, dir);
	vectoangles( dir, angle );
	AngleVectors( angle, forward, right, up );

	VectorCopy(storm_position, up_pos);
	VectorMA( up_pos, 320, right, up_pos );
	CG_Trace(&tr, cg.refdef.vieworg, NULL, NULL, up_pos, cg.clientNum, MASK_SHOT);
	halfSmokeSpriteWidth1 = VectorDistance(tr.endpos, cg.refdef.vieworg);

	VectorCopy(storm_position, up_pos);
	VectorMA( up_pos, -320, right, up_pos );
	CG_Trace(&tr, cg.refdef.vieworg, NULL, NULL, up_pos, cg.clientNum, MASK_SHOT);
	halfSmokeSpriteWidth2 = VectorDistance(tr.endpos, cg.refdef.vieworg);

	CG_Printf("halfSmokeSpriteWidth1 is %f. halfSmokeSpriteWidth2 is %f.\n", halfSmokeSpriteWidth1, halfSmokeSpriteWidth2);

	VectorMA( storm_position, halfSmokeSpriteHeight, up, top );
	VectorMA( storm_position, -halfSmokeSpriteHeight, up, bottom );
	color[0] = 0xff;
	color[1] = 0xff;
	color[2] = 0xff;
	color[3] = 0xff;

	VectorMA( top, halfSmokeSpriteWidth1, right, verts[0].xyz );
	verts[0].st[0] = 1;
	verts[0].st[1] = 0;
	memcpy( verts[0].modulate, color, 4 );
	VectorMA( top, -halfSmokeSpriteWidth2, right, verts[1].xyz );
	verts[1].st[0] = 0;
	verts[1].st[1] = 0;
	memcpy( verts[1].modulate, color, 4 );
	VectorMA( bottom, -halfSmokeSpriteWidth2, right, verts[2].xyz );
	verts[2].st[0] = 0;
	verts[2].st[1] = 1;
	memcpy( verts[2].modulate, color, 4 );
	VectorMA( bottom, halfSmokeSpriteWidth1, right, verts[3].xyz );
	verts[3].st[0] = 1;
	verts[3].st[1] = 1;
	memcpy( verts[3].modulate, color, 4 );
	
	if (rain_left_end_time > cg.time)
	{
		// Layer 1, far rain!
		trap_R_AddPolyToScene( rainleftshader1, 4, verts );
		// Layer 2, near rain!
		trap_R_AddPolyToScene( rainleftshader2, 4, verts );
		// Layer 3, mist!
		trap_R_AddPolyToScene( mistleftshader, 4, verts );
	}
	else if (rain_down_end_time > cg.time)
	{
		// Layer 1, far rain!
		trap_R_AddPolyToScene( raindownshader1, 4, verts );
		// Layer 2, near rain!
		trap_R_AddPolyToScene( raindownshader2, 4, verts );
		// Layer 3, mist!
		trap_R_AddPolyToScene( mistdownshader, 4, verts );
	}
	else
	{
		// Ready to switch to a variation next frame...
		if (next_rain_left_time <= cg.time)
		{
			rain_left_end_time = cg.time + Q_TrueRand(1000, 5000);
		}
		else if (rain_down_end_time <= cg.time)
		{
			rain_down_end_time = cg.time + Q_TrueRand(1000, 5000);
		}

		// Set up next change time...
		if (next_rain_left_time < cg.time && next_rain_down_time < cg.time && rand()%100 >= 50)
		{
			next_rain_left_time = cg.time + Q_TrueRand(1000, 15000);
		}
		else if (next_rain_left_time < cg.time && next_rain_down_time < cg.time)
		{
			next_rain_down_time = cg.time + Q_TrueRand(1000, 15000);
		}

		// Layer 1, far rain!
		trap_R_AddPolyToScene( rainshader1, 4, verts );
		// Layer 2, near rain!
		trap_R_AddPolyToScene( rainshader2, 4, verts );
		// Layer 3, mist!
		trap_R_AddPolyToScene( mistshader1, 4, verts );
	}
	
}

qhandle_t rain_splash;

void
CG_AddRainSplash ( vec3_t position )
{
/*	byte			color[4];
	polyVert_t		verts[4];
	vec3_t			top, bottom;
	vec3_t			right, up, dir, angle, forward;
	float			radius;
	float			halfSmokeSpriteWidth, halfSmokeSpriteHeight;*/
//	refEntity_t		ent;
	localEntity_t	*le;
	refEntity_t		*re;
	vec3_t			velocity;
	int				i;

	if (!rain_initialized)
	{
		rainshader1 = trap_R_RegisterShader( "textures/atmospheric/rainfar" );
		rainshader2 = trap_R_RegisterShader( "textures/atmospheric/rainnear" );
		mistshader1 = trap_R_RegisterShader( "textures/atmospheric/mistnear" );
		
		mistleftshader = trap_R_RegisterShader( "textures/atmospheric/mistnearleft" );
		rainleftshader1 = trap_R_RegisterShader( "textures/atmospheric/rainfarleft" );
		rainleftshader2 = trap_R_RegisterShader( "textures/atmospheric/rainnearleft" );

		mistdownshader = trap_R_RegisterShader( "textures/atmospheric/mistneardown" );
		raindownshader1 = trap_R_RegisterShader( "textures/atmospheric/rainfardown" );
		raindownshader2 = trap_R_RegisterShader( "textures/atmospheric/rainnearvert" );

		rainSplash[0] = trap_R_RegisterShader( "textures/atmospheric/rainbounces" );
		rainSplash[1] = trap_R_RegisterShader( "textures/atmospheric/rainbounces2" );
		rainSplash[2] = trap_R_RegisterShader( "textures/atmospheric/rainbouncesbig" );   // Rain bounces coming down.
		rainSplash[3] = trap_R_RegisterShader( "textures/atmospheric/rainbouncesbigup" );  // Rain bounces that come up. Used for floor bounces.
		rainSplash[4] = trap_R_RegisterShader( "textures/atmospheric/rainbouncesbigup2" );  // Rain bounces that come up. Used for floor bounces.
		rainSplash[5] = trap_R_RegisterShader( "textures/atmospheric/rainreflection" );
		rainSplash[6] = trap_R_RegisterShader( "textures/atmospheric/rain_impact" );
		rainSplash[7] = trap_R_RegisterShader( "textures/atmospheric/rain_impact2" );

		rain_splash = trap_R_RegisterModel( "models/foliage/raincircle0.md3" );

		rain_initialized = qtrue;
	}
/*
	radius = 8.f; // max radius
	halfSmokeSpriteWidth = 4;
	halfSmokeSpriteHeight = 4;

	// Now render it
	position[2] = BG_GetSkyGroundHeightAtPoint( position );
	VectorSubtract(position, cg.refdef.vieworg, dir);
	vectoangles( dir, angle );
	AngleVectors( angle, forward, right, up );

	VectorMA( position, halfSmokeSpriteHeight, up, top );
	VectorMA( position, -halfSmokeSpriteHeight, up, bottom );
	color[0] = 0xff;
	color[1] = 0xff;
	color[2] = 0xff;
	color[3] = 0xff;

	VectorMA( top, halfSmokeSpriteWidth, right, verts[0].xyz );
	verts[0].st[0] = 1;
	verts[0].st[1] = 0;
	memcpy( verts[0].modulate, color, 4 );
	VectorMA( top, -halfSmokeSpriteWidth, right, verts[1].xyz );
	verts[1].st[0] = 0;
	verts[1].st[1] = 0;
	memcpy( verts[1].modulate, color, 4 );
	VectorMA( bottom, -halfSmokeSpriteWidth, right, verts[2].xyz );
	verts[2].st[0] = 0;
	verts[2].st[1] = 1;
	memcpy( verts[2].modulate, color, 4 );
	VectorMA( bottom, halfSmokeSpriteWidth, right, verts[3].xyz );
	verts[3].st[0] = 1;
	verts[3].st[1] = 1;
	memcpy( verts[3].modulate, color, 4 );
	
	// Layer 1, far rain!
	trap_R_AddPolyToScene( rainSplash[Q_TrueRand(0,4)], 4, verts );
*/
/*
	memset( &ent, 0, sizeof( ent ) );

	ent.axis[0][0] = cg.refdef.viewaxis[0][0];
	ent.axis[0][1] = cg.refdef.viewaxis[0][1];
	ent.axis[0][2] = cg.refdef.viewaxis[0][2];
	ent.axis[1][0] = cg.refdef.viewaxis[1][0];
	ent.axis[1][1] = cg.refdef.viewaxis[1][1];
	ent.axis[1][2] = cg.refdef.viewaxis[1][2];
	ent.axis[2][0] = cg.refdef.viewaxis[2][0];
	ent.axis[2][1] = cg.refdef.viewaxis[2][1];
	ent.axis[2][2] = cg.refdef.viewaxis[2][2];

	// DHM - Nerve :: adjusted values
	position[2]+=128;
	VectorCopy( position, ent.origin );
	position[2] = BG_GetSkyGroundHeightAtPoint( position );
	CG_Printf("Rain splash at %f %f %f.\n", position[0], position[1], position[2]);

	ent.radius = 16;

	ent.hModel = rain_splash;
	trap_R_AddRefEntityToScene( &ent );
*/
/*
	position[2] = BG_GetSkyGroundHeightAtPoint( position );

	VectorSet(velocity, 0, 0, 0);

//	for ( i = 0; i < count; i++ )
	{
		le = CG_AllocLocalEntity();
		re = &le->refEntity;
		//VectorSet( velocity, velocity[0] + crandom() * randScale, velocity[1] + crandom() * randScale, velocity[2] + crandom() * randScale );
		VectorScale( velocity, 0, velocity );
		le->leType = LE_RAINSPLASH;
		le->startTime = cg.time;
		le->endTime = le->startTime + 200; // DHM - Nerve :: (removed) - (int)(0.5 * random() * duration);
		le->lastTrailTime = cg.time;
		VectorCopy( position, re->origin );
		AxisCopy( axisDefault, re->axis );
		le->pos.trType = TR_GRAVITY_LOW;
		VectorCopy( position, le->pos.trBase );
		VectorMA( le->pos.trBase, 2 + random() * 4, velocity, le->pos.trBase );
		VectorCopy( velocity, le->pos.trDelta );
		le->pos.trTime = cg.time;
		le->bounceFactor = 0.9;
	}*/
}

qboolean render_storm_2D = qfalse;

void CG_AddStorm ( void )
{// UQ1: Testing new shader based storm fx!
//	trace_t tr;
//	vec3_t up_pos;

	if (!rain_initialized)
	{
		rainshader1 = trap_R_RegisterShader( "textures/atmospheric/rainfar" );
		rainshader2 = trap_R_RegisterShader( "textures/atmospheric/rainnear" );
		mistshader1 = trap_R_RegisterShader( "textures/atmospheric/mistnear" );
		
		mistleftshader = trap_R_RegisterShader( "textures/atmospheric/mistnearleft" );
		rainleftshader1 = trap_R_RegisterShader( "textures/atmospheric/rainfarleft" );
		rainleftshader2 = trap_R_RegisterShader( "textures/atmospheric/rainnearleft" );

		mistdownshader = trap_R_RegisterShader( "textures/atmospheric/mistneardown" );
		raindownshader1 = trap_R_RegisterShader( "textures/atmospheric/rainfardown" );
		raindownshader2 = trap_R_RegisterShader( "textures/atmospheric/rainnearvert" );

		rainSplash[0] = trap_R_RegisterShader( "textures/atmospheric/rainbounces" );
		rainSplash[1] = trap_R_RegisterShader( "textures/atmospheric/rainbounces2" );
		rainSplash[2] = trap_R_RegisterShader( "textures/atmospheric/rainbouncesbig" );   // Rain bounces coming down.
		rainSplash[3] = trap_R_RegisterShader( "textures/atmospheric/rainbouncesbigup" );  // Rain bounces that come up. Used for floor bounces.
		rainSplash[4] = trap_R_RegisterShader( "textures/atmospheric/rainbouncesbigup2" );  // Rain bounces that come up. Used for floor bounces.
		rainSplash[5] = trap_R_RegisterShader( "textures/atmospheric/rainreflection" );
		rainSplash[6] = trap_R_RegisterShader( "textures/atmospheric/rain_impact" );
		rainSplash[7] = trap_R_RegisterShader( "textures/atmospheric/rain_impact2" );

		rain_splash = trap_R_RegisterModel( "models/foliage/raincircle0.md3" );
		rain_initialized = qtrue;
	}
/*
	VectorCopy(cg.refdef.vieworg, up_pos);
	up_pos[2]+= 99999.9f;
	CG_Trace(&tr, cg.refdef.vieworg, NULL, NULL, up_pos, cg.clientNum, MASK_SHOT);

	if (tr.surfaceFlags & SURF_SKY )
	{
		render_storm_2D = qtrue;
	}
	else
	{
		CG_AddStormSprites();
		render_storm_2D = qfalse;
	}*/
}


void CG_AddStorm_Outdoors ( void )
{// UQ1: Testing new shader based storm fx! This would only work if the whole map is outside :( looks cool though!
	if (!rain_initialized)
	{
/*
		textures/atmospheric/mistnear
		textures/atmospheric/mistnearquiet
		textures/atmospheric/mistnearleft
		textures/atmospheric/mistneardown

		textures/atmospheric/rainnear
		textures/atmospheric/rainnearleft
		textures/atmospheric/rainnearvert

		textures/atmospheric/rainfar
		textures/atmospheric/rainfarleft
		textures/atmospheric/rainfardown

		textures/atmospheric/bolt1
		textures/atmospheric/bolt2

		textures/atmospheric/viewdrop
		textures/atmospheric/viewdrop2

		textures/atmospheric/nodrop

		textures/atmospheric/rainimpact
		textures/atmospheric/rainbounces
		textures/atmospheric/rainbounces2
		textures/atmospheric/rainbouncesbig   // Rain bounces coming down.
		textures/atmospheric/rainbouncesbigup  // Rain bounces that come up. Used for floor bounces.
		textures/atmospheric/rainbouncesbigup2  // Rain bounces that come up. Used for floor bounces.
		textures/atmospheric/rainreflection
		textures/atmospheric/rain_impact
		textures/atmospheric/rain_impact2

		textures/atmospheric/stormmask
		textures/atmospheric/dryout

		textures/atmospheric/raincomingdown  // Rain bounces coming down.
		textures/atmospheric/raincomingdown2  // Rain bounces coming down.
*/
		rainshader1 = trap_R_RegisterShader( "textures/atmospheric/rainfar" );
		rainshader2 = trap_R_RegisterShader( "textures/atmospheric/rainnear" );
		mistshader1 = trap_R_RegisterShader( "textures/atmospheric/mistnear" );
		
		mistleftshader = trap_R_RegisterShader( "textures/atmospheric/mistnearleft" );
		rainleftshader1 = trap_R_RegisterShader( "textures/atmospheric/rainfarleft" );
		rainleftshader2 = trap_R_RegisterShader( "textures/atmospheric/rainnearleft" );

		mistdownshader = trap_R_RegisterShader( "textures/atmospheric/mistneardown" );
		raindownshader1 = trap_R_RegisterShader( "textures/atmospheric/rainfardown" );
		raindownshader2 = trap_R_RegisterShader( "textures/atmospheric/rainnearvert" );

//		mistquietshader = trap_R_RegisterShader( "textures/atmospheric/mistnearquiet" );	

		rain_initialized = qtrue;
	}

	if (rain_left_end_time > cg.time)
	{
		// Layer 1, far rain!
		CG_DrawPic( 0, 0, 640, 480, rainleftshader1 );
		// Layer 2, near rain!
		CG_DrawPic( 0, 0, 640, 480, rainleftshader2 );
		// Layer 3, mist!
		CG_DrawPic( 0, 0, 640, 480, mistleftshader );
	}
	else if (rain_down_end_time > cg.time)
	{
		// Layer 1, far rain!
		CG_DrawPic( 0, 0, 640, 480, raindownshader1 );
		// Layer 2, near rain!
		CG_DrawPic( 0, 0, 640, 480, raindownshader2 );
		// Layer 3, mist!
		CG_DrawPic( 0, 0, 640, 480, mistdownshader );
	}
	else
	{
		// Ready to switch to a variation next frame...
		if (next_rain_left_time <= cg.time)
		{
			rain_left_end_time = cg.time + Q_TrueRand(1000, 5000);
		}
		else if (rain_down_end_time <= cg.time)
		{
			rain_down_end_time = cg.time + Q_TrueRand(1000, 5000);
		}

		// Set up next change time...
		if (next_rain_left_time < cg.time && next_rain_down_time < cg.time && rand()%100 >= 50)
		{
			next_rain_left_time = cg.time + Q_TrueRand(1000, 15000);
		}
		else if (next_rain_left_time < cg.time && next_rain_down_time < cg.time)
		{
			next_rain_down_time = cg.time + Q_TrueRand(1000, 15000);
		}

		// Layer 1, far rain!
		CG_DrawPic( 0, 0, 640, 480, rainshader1 );
		// Layer 2, near rain!
		CG_DrawPic( 0, 0, 640, 480, rainshader2 );
		// Layer 3, mist!
		CG_DrawPic( 0, 0, 640, 480, mistshader1 );
	}

	//CG_Printf("Storm drawn!\n");
}

//int getgroundtime, getskytime, rendertime, checkvisibletime, generatetime;
//int n_getgroundtime, n_getskytime, n_rendertime, n_checkvisibletime, n_generatetime;
//static qboolean CG_LoadTraceMap( void );
//#define MAX_ATMOSPHERIC_PARTICLES		4000	// maximum # of particles
//#define MAX_ATMOSPHERIC_PARTICLES		16000	// maximum # of particles
#define MAX_ATMOSPHERIC_PARTICLES		8000	// maximum # of particles
#define MAX_ATMOSPHERIC_DISTANCE		1000	// maximum distance from refdef origin that particles are visible
#define MAX_ATMOSPHERIC_EFFECTSHADERS	6		// maximum different effectshaders for an atmospheric effect
//#define ATMOSPHERIC_DROPDELAY			1000
#define ATMOSPHERIC_DROPDELAY			200
#define ATMOSPHERIC_CUTHEIGHT			800

/*#define	ATMOSPHERIC_RAIN_SPEED		(cg_atmosphere_speed.value * DEFAULT_GRAVITY)
#define	ATMOSPHERIC_RAIN_HEIGHT		(150 + cg_atmosphere_height.integer *10)

#define	ATMOSPHERIC_SNOW_SPEED		(cg_atmosphere_speed.value/10 * DEFAULT_GRAVITY)
#define	ATMOSPHERIC_SNOW_HEIGHT		(3 + cg_atmosphere_height.integer)*/
#define ATMOSPHERIC_RAIN_SPEED	( 1.1f * DEFAULT_GRAVITY )
#define ATMOSPHERIC_RAIN_HEIGHT 50//150
#define ATMOSPHERIC_SNOW_SPEED	( 0.1f * DEFAULT_GRAVITY )
#define ATMOSPHERIC_SNOW_HEIGHT 3
typedef enum
{
	ATM_NONE,
	ATM_RAIN,
	ATM_SNOW,
	ATM_STORM
} atmFXType_t;
#ifndef ATM_NEW

/*
** Atmospheric Particles PolyPool
*/
static polyVert_t	atmPolyPool[MAX_ATMOSPHERIC_PARTICLES * 3];
static int			numParticlesInFrame;
static qhandle_t	atmPolyShader;


/* */
static void
CG_ClearPolyPool ( void )
{
	numParticlesInFrame = 0;
	atmPolyShader = 0;
}


/* */
static void
CG_RenderPolyPool ( void )
{
	if ( numParticlesInFrame )
	{
		trap_R_AddPolysToScene( atmPolyShader, 3, atmPolyPool, numParticlesInFrame );
		CG_ClearPolyPool();
	}
}
#endif // ATM_NEW

/* */
static void
CG_AddPolyToPool ( qhandle_t shader, const polyVert_t *verts )
{
#ifndef ATM_NEW
	if ( atmPolyShader && atmPolyShader != shader )
	{
		CG_RenderPolyPool();
	}

	if ( numParticlesInFrame == MAX_ATMOSPHERIC_PARTICLES )
	{
		CG_RenderPolyPool();
	}

	atmPolyShader = shader;
	memcpy( &atmPolyPool[numParticlesInFrame * 3], verts, 3 * sizeof(polyVert_t) );
	numParticlesInFrame++;
#else
	int				firstIndex;
	int				firstVertex;
	int				i;
	polyBuffer_t	*pPolyBuffer = CG_PB_FindFreePolyBuffer( shader, 3, 3 );
	if ( !pPolyBuffer )
	{
		return;
	}

	firstIndex = pPolyBuffer->numIndicies;
	firstVertex = pPolyBuffer->numVerts;
	for ( i = 0; i < 3; i++ )
	{
		VectorCopy( verts[i].xyz, pPolyBuffer->xyz[firstVertex + i] );
		pPolyBuffer->st[firstVertex + i][0] = verts[i].st[0];
		pPolyBuffer->st[firstVertex + i][1] = verts[i].st[1];
		pPolyBuffer->color[firstVertex + i][0] = verts[i].modulate[0];
		pPolyBuffer->color[firstVertex + i][1] = verts[i].modulate[1];
		pPolyBuffer->color[firstVertex + i][2] = verts[i].modulate[2];
		pPolyBuffer->color[firstVertex + i][3] = verts[i].modulate[3];
		pPolyBuffer->indicies[firstIndex + i] = firstVertex + i;
	}

	pPolyBuffer->numIndicies += 3;
	pPolyBuffer->numVerts += 3;
#endif // ATM_NEW
}

/*
**	CG_AtmosphericKludge
*/
static qboolean kludgeChecked, kludgeResult;


/* */
qboolean
CG_AtmosphericKludge ( void )
{

	// Activate rain for specified kludge maps that don't
	// have it specified for them.
	if ( kludgeChecked )
	{
		return ( kludgeResult );
	}

	kludgeChecked = qtrue;
	kludgeResult = qfalse;

	if( !Q_stricmp( cgs.mapname, "maps/fueldump.bsp" ) )
	{
		//CG_EffectParse( "T=RAIN,B=5 10,C=0.5 2,G=0.5 2,BV=30 100,GV=20 80,W=1 2,D=1000 1000" );
		CG_EffectParse( "T=SNOW,B=5 10,C=0.5,G=0.3 2,BV=20 30,GV=25 40,W=3 5,D=2000" );
		return( kludgeResult = qtrue );
	}

	/*if( !Q_stricmp( cgs.mapname, "maps/trainyard.bsp" ) )
	{
		//CG_EffectParse( "T=RAIN,B=5 10,C=0.5 2,G=0.5 2,BV=30 100,GV=20 80,W=1 2,D=1000 1000" );
		CG_EffectParse( "T=RAIN,B=5 10,C=0.5,G=0.5 2,BV=50 50,GV=200 200,W=1 2,D=1000" );
		return( kludgeResult = qtrue );
	}*/

	/*	if( !Q_stricmp( cgs.mapname, "maps/mp_railgun.bsp" ) )
	{
		//CG_EffectParse( "T=RAIN,B=5 10,C=0.5 2,G=0.5 2,BV=30 100,GV=20 80,W=1 2,D=1000 1000" );
//		CG_EffectParse( "T=SNOW,B=5 10,C=0.5,G=0.3 2,BV=50 50,GV=30 80,W=1 2,D=5000" );
		
		// snow storm, quite horizontally
		//CG_EffectParse( "T=SNOW,B=20 30,C=0.8,G=0.5 8,BV=100 100,GV=70 150,W=3 5,D=5000" );

		// mild snow storm, quite vertically - likely go for this
		//CG_EffectParse( "T=SNOW,B=5 10,C=0.5,G=0.3 2,BV=20 30,GV=25 40,W=3 5,D=5000" );
		CG_EffectParse( "T=SNOW,B=5 10,C=0.5,G=0.3 2,BV=20 30,GV=25 40,W=3 5,D=2000" );

		// cpu-cheap press event effect
		//CG_EffectParse( "T=SNOW,B=5 10,C=0.5,G=0.3 2,BV=20 30,GV=25 40,W=3 5,D=500" );
//		CG_EffectParse( "T=SNOW,B=5 10,C=0.5,G=0.3 2,BV=20 30,GV=25 40,W=3 5,D=750" );
		return( kludgeResult = qtrue );
	}*/

	/*if( !Q_stricmp( cgs.mapname, "maps/mp_goliath.bsp" ) ) {
		//CG_EffectParse( "T=SNOW,B=5 7,C=0.2,G=0.1 5,BV=15 25,GV=25 40,W=3 5,D=400" );
		CG_EffectParse( "T=SNOW,B=5 7,C=0.2,G=0.1 5,BV=15 25,GV=25 40,W=3 5,H=512,D=2000" );
		return( kludgeResult = qtrue );
	}*/

	/*if( !Q_stricmp( cgs.rawmapname, "sp_bruck_test006" ) ) {
		//T=SNOW,B=5 10,C=0.5,G=0.3 2,BV=20 30,GV=25 40,W=3 5,H=608,D=2000
		CG_EffectParse( "T=SNOW,B=5 10,C=0.5,G=0.3 2,BV=20 30,GV=25 40,W=3 5,H=512,D=2000 4000" );
		//CG_EffectParse( "T=SNOW,B=5 7,C=0.2,G=0.1 5,BV=15 25,GV=25 40,W=3 5,H=512,D=2000" );
		return( kludgeResult = qtrue );
	}*/
	return ( kludgeResult = qfalse );
}

typedef enum
{
	ACT_NOT,
	ACT_FALLING
} active_t;
typedef struct cg_atmosphericParticle_s
{
	vec3_t		pos, delta, deltaNormalized, colour;
	float		height, weight;
	active_t	active;
	int			nextDropTime;
	qhandle_t	*effectshader;
} cg_atmosphericParticle_t;
typedef struct cg_atmosphericEffect_s
{
	cg_atmosphericParticle_t	particles[MAX_ATMOSPHERIC_PARTICLES];
	qhandle_t					effectshaders[MAX_ATMOSPHERIC_EFFECTSHADERS];
	int							lastRainTime, numDrops;
	int							gustStartTime, gustEndTime;
	int							baseStartTime, baseEndTime;
	int							gustMinTime, gustMaxTime;
	int							changeMinTime, changeMaxTime;
	int							baseMinTime, baseMaxTime;
	float						baseWeight, gustWeight;
	int							baseDrops, gustDrops;
	int							baseHeightOffset;
	int							numEffectShaders;
	vec3_t						baseVec, gustVec;
	vec3_t						viewDir;
	qboolean ( *ParticleCheckVisible ) ( cg_atmosphericParticle_t * particle );
	qboolean ( *ParticleGenerate ) ( cg_atmosphericParticle_t * particle, vec3_t currvec, float currweight );
	void ( *ParticleRender ) ( cg_atmosphericParticle_t * particle );
	int dropsActive, oldDropsActive;
	int dropsRendered, dropsCreated, dropsSkipped;
} cg_atmosphericEffect_t;
static cg_atmosphericEffect_t	cg_atmFx;


/* */
static qboolean
CG_SetParticleActive ( cg_atmosphericParticle_t *particle, active_t active )
{
	particle->active = active;
	return ( active ? qtrue : qfalse );
}


/*
**	Raindrop management functions
*/
static qboolean
CG_RainParticleGenerate ( cg_atmosphericParticle_t *particle, vec3_t currvec, float currweight )
{

	// Attempt to 'spot' a raindrop somewhere below a sky texture.
	float	angle, distance;
	float	groundHeight, skyHeight;

	//	int msec = trap_Milliseconds();
	//	n_generatetime++;
	angle = random() * 2 * M_PI;
	distance = 20 + MAX_ATMOSPHERIC_DISTANCE * random();
	particle->pos[0] = cg.refdef_current->vieworg[0] + sin( angle ) * distance;
	particle->pos[1] = cg.refdef_current->vieworg[1] + cos( angle ) * distance;

	// ydnar: choose a spawn point randomly between sky and ground
	skyHeight = BG_GetSkyHeightAtPoint( particle->pos );
	if ( skyHeight == MAX_ATMOSPHERIC_HEIGHT )
	{
		return ( qfalse );
	}

	groundHeight = BG_GetSkyGroundHeightAtPoint( particle->pos );
	if ( groundHeight >= skyHeight )
	{
		return ( qfalse );
	}

	particle->pos[2] = groundHeight + random() * ( skyHeight - groundHeight );

	// make sure it doesn't fall from too far cause it then will go over our heads ('lower the ceiling')
	if ( cg_atmFx.baseHeightOffset > 0 )
	{
		if ( particle->pos[2] - cg.refdef_current->vieworg[2] > cg_atmFx.baseHeightOffset )
		{
			particle->pos[2] = cg.refdef_current->vieworg[2] + cg_atmFx.baseHeightOffset;
			if ( particle->pos[2] < groundHeight )
			{
				return ( qfalse );
			}
		}
	}

	// ydnar: rain goes in bursts
	{
		float	maxActiveDrops;

		// every 10 seconds allow max raindrops
		maxActiveDrops = 0.50 * cg_atmFx.numDrops + 0.001 * cg_atmFx.numDrops * ( 10000 - (cg.time % 10000) );
		if ( cg_atmFx.oldDropsActive > maxActiveDrops )
		{
			return ( qfalse );
		}
	}

	CG_SetParticleActive( particle, ACT_FALLING );
	particle->colour[0] = 0.6 + 0.2 * random() * 0xFF;
	particle->colour[1] = 0.6 + 0.2 * random() * 0xFF;
	particle->colour[2] = 0.6 + 0.2 * random() * 0xFF;
	VectorCopy( currvec, particle->delta );
	particle->delta[2] += crandom() * 100;
	VectorCopy( particle->delta, particle->deltaNormalized );
	VectorNormalizeFast( particle->deltaNormalized );
	particle->height = ATMOSPHERIC_RAIN_HEIGHT + crandom() * 100;
	particle->weight = currweight;
	particle->effectshader = &cg_atmFx.effectshaders[0];

	//	particle->effectshader = &cg_atmFx.effectshaders[ (int) (random() * ( cg_atmFx.numEffectShaders - 1 )) ];
	//	generatetime += trap_Milliseconds() - msec;
	return ( qtrue );
}


/* */
static qboolean
CG_RainParticleCheckVisible ( cg_atmosphericParticle_t *particle )
{

	// Check the raindrop is visible and still going, wrapping if necessary.
	float	moved;
	vec2_t	distance;

	//	int msec = trap_Milliseconds();
	if ( !particle || particle->active == ACT_NOT )
	{

		//		checkvisibletime += trap_Milliseconds() - msec;
		return ( qfalse );
	}

	moved = ( cg.time - cg_atmFx.lastRainTime ) * 0.001;	// Units moved since last frame
	VectorMA( particle->pos, moved, particle->delta, particle->pos );
	if ( particle->pos[2] + particle->height < BG_GetSkyGroundHeightAtPoint( particle->pos) )
	{
		// UQ1: Add splashes!
		CG_AddRainSplash( particle->pos );

		//		checkvisibletime += trap_Milliseconds() - msec;
		return ( CG_SetParticleActive( particle, ACT_NOT) );
	}

	distance[0] = particle->pos[0] - cg.refdef_current->vieworg[0];
	distance[1] = particle->pos[1] - cg.refdef_current->vieworg[1];
	if ( (distance[0] * distance[0] + distance[1] * distance[1]) > Square( MAX_ATMOSPHERIC_DISTANCE) )
	{

		// ydnar: just nuke this particle, let it respawn
		return ( CG_SetParticleActive( particle, ACT_NOT) );

		/*
		// Attempt to respot the particle at our other side
		particle->pos[0] -= 1.85f * distance[0];
		particle->pos[1] -= 1.85f * distance[1];

		// Valid spot?
		pointHeight = BG_GetSkyHeightAtPoint( particle->pos );
		if( pointHeight == MAX_ATMOSPHERIC_HEIGHT ) {
//			checkvisibletime += trap_Milliseconds() - msec;
			return CG_SetParticleActive( particle, ACT_NOT );
		}

		pointHeight = BG_GetSkyGroundHeightAtPoint( particle->pos );
		if( pointHeight == MAX_ATMOSPHERIC_HEIGHT || pointHeight >= particle->pos[2] ) {
//			checkvisibletime += trap_Milliseconds() - msec;
			return CG_SetParticleActive( particle, ACT_NOT );
		}
		*/
	}

	//	checkvisibletime += trap_Milliseconds() - msec;
	return ( qtrue );
}


/* */
static void
CG_RainParticleRender ( cg_atmosphericParticle_t *particle )
{

	// Draw a raindrop
	vec3_t		forward, right;
	polyVert_t	verts[3];
	vec2_t		line;
	float		len, frac, dist;
	vec3_t		start, finish;
	float		groundHeight;

	//	int			msec = trap_Milliseconds();
	//	n_rendertime++;
	if ( particle->active == ACT_NOT )
	{

		//		rendertime += trap_Milliseconds() - msec;
		return;
	}

	if ( CG_CullPoint( particle->pos) )
	{
		return;
	}

	VectorCopy( particle->pos, start );
	dist = DistanceSquared( particle->pos, cg.refdef_current->vieworg );

	// Make sure it doesn't clip through surfaces
	groundHeight = BG_GetSkyGroundHeightAtPoint( start );
	len = particle->height;
	if ( start[2] <= groundHeight )
	{

		// Stop snow going through surfaces.
		len = particle->height - groundHeight + start[2];
		frac = start[2];
		VectorMA( start, len - particle->height, particle->deltaNormalized, start );
	}

	if ( len <= 0 )
	{

		//		rendertime += trap_Milliseconds() - msec;
		return;
	}

	// fade nearby rain particles
	if ( dist < Square( 128.f) )
	{
		dist = .25f + .75f * ( dist / Square( 128.f) );
	}
	else
	{
		dist = 1.0f;
	}

	VectorCopy( particle->deltaNormalized, forward );
	VectorMA( start, -len, forward, finish );
	line[0] = DotProduct( forward, cg.refdef_current->viewaxis[1] );
	line[1] = DotProduct( forward, cg.refdef_current->viewaxis[2] );
	VectorScale( cg.refdef_current->viewaxis[1], line[1], right );
	VectorMA( right, -line[0], cg.refdef_current->viewaxis[2], right );
	VectorNormalize( right );

	// dist = 1.0;
	VectorCopy( finish, verts[0].xyz );
	verts[0].st[0] = 0.5f;
	verts[0].st[1] = 0;
	verts[0].modulate[0] = particle->colour[0];
	verts[0].modulate[1] = particle->colour[1];
	verts[0].modulate[2] = particle->colour[2];
	verts[0].modulate[3] = 100 * dist;
	VectorMA( start, -particle->weight, right, verts[1].xyz );
	verts[1].st[0] = 0;
	verts[1].st[1] = 1;
	verts[1].modulate[0] = particle->colour[0];
	verts[1].modulate[1] = particle->colour[1];
	verts[2].modulate[2] = particle->colour[2];
	verts[1].modulate[3] = 200 * dist;
	VectorMA( start, particle->weight, right, verts[2].xyz );
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[2].modulate[0] = particle->colour[0];
	verts[2].modulate[1] = particle->colour[1];
	verts[2].modulate[2] = particle->colour[2];
	verts[2].modulate[3] = 200 * dist;
	CG_AddPolyToPool( *particle->effectshader, verts );

	//	rendertime += trap_Milliseconds() - msec;
}


/*
**	Snow management functions
*/
static qboolean
CG_SnowParticleGenerate ( cg_atmosphericParticle_t *particle, vec3_t currvec, float currweight )
{

	// Attempt to 'spot' a snowflake somewhere below a sky texture.
	float	angle, distance;
	float	groundHeight, skyHeight;

	//	int msec = trap_Milliseconds();
	//	n_generatetime++;
	angle = random() * 2 * M_PI;
	distance = 20 + MAX_ATMOSPHERIC_DISTANCE * random();
	particle->pos[0] = cg.refdef_current->vieworg[0] + sin( angle ) * distance;
	particle->pos[1] = cg.refdef_current->vieworg[1] + cos( angle ) * distance;

	// ydnar: choose a spawn point randomly between sky and ground
	skyHeight = BG_GetSkyHeightAtPoint( particle->pos );
	if ( skyHeight == MAX_ATMOSPHERIC_HEIGHT )
	{
		return ( qfalse );
	}

	groundHeight = BG_GetSkyGroundHeightAtPoint( particle->pos );
	if ( groundHeight >= skyHeight )
	{
		return ( qfalse );
	}

	particle->pos[2] = groundHeight + random() * ( skyHeight - groundHeight );

	// make sure it doesn't fall from too far cause it then will go over our heads ('lower the ceiling')
	if ( cg_atmFx.baseHeightOffset > 0 )
	{
		if ( particle->pos[2] - cg.refdef_current->vieworg[2] > cg_atmFx.baseHeightOffset )
		{
			particle->pos[2] = cg.refdef_current->vieworg[2] + cg_atmFx.baseHeightOffset;
			if ( particle->pos[2] < groundHeight )
			{
				return ( qfalse );
			}
		}
	}

	CG_SetParticleActive( particle, ACT_FALLING );
	VectorCopy( currvec, particle->delta );
	particle->delta[2] += crandom() * 25;
	VectorCopy( particle->delta, particle->deltaNormalized );
	VectorNormalizeFast( particle->deltaNormalized );
	particle->height = ATMOSPHERIC_SNOW_HEIGHT + random() * 2;
	particle->weight = particle->height * 0.5f;
	particle->effectshader = &cg_atmFx.effectshaders[0];

	//	particle->effectshader = &cg_atmFx.effectshaders[ (int) (random() * ( cg_atmFx.numEffectShaders - 1 )) ];
	//	generatetime += trap_Milliseconds() - msec;
	return ( qtrue );
}


/* */
static qboolean
CG_SnowParticleCheckVisible ( cg_atmosphericParticle_t *particle )
{

	// Check the snowflake is visible and still going, wrapping if necessary.
	float	moved;
	vec2_t	distance;

	//	int msec = trap_Milliseconds();
	//	n_checkvisibletime++;
	if ( !particle || particle->active == ACT_NOT )
	{

		//		checkvisibletime += trap_Milliseconds() - msec;
		return ( qfalse );
	}

	moved = ( cg.time - cg_atmFx.lastRainTime ) * 0.001;	// Units moved since last frame
	VectorMA( particle->pos, moved, particle->delta, particle->pos );
	if ( particle->pos[2] < BG_GetSkyGroundHeightAtPoint( particle->pos) )
	{

		//		checkvisibletime += trap_Milliseconds() - msec;
		return ( CG_SetParticleActive( particle, ACT_NOT) );
	}

	distance[0] = particle->pos[0] - cg.refdef_current->vieworg[0];
	distance[1] = particle->pos[1] - cg.refdef_current->vieworg[1];
	if ( (distance[0] * distance[0] + distance[1] * distance[1]) > Square( MAX_ATMOSPHERIC_DISTANCE) )
	{

		// ydnar: just nuke this particle, let it respawn
		return ( CG_SetParticleActive( particle, ACT_NOT) );

		/*
		// Attempt to respot the particle at our other side
		particle->pos[0] -= 1.85f * distance[0];
		particle->pos[1] -= 1.85f * distance[1];

		// ydnar: place particle in random position between ground and sky
		groundHeight = BG_GetSkyGroundHeightAtPoint( particle->pos );
		skyHeight = BG_GetSkyHeightAtPoint( particle->pos );
		if( skyHeight == MAX_ATMOSPHERIC_HEIGHT )
			return CG_SetParticleActive( particle, ACT_NOT );
		particle->pos[ 2 ] = groundHeight + random() * (skyHeight - groundHeight);
		
		// ydnar: valid spot?
		if( particle->pos[ 2 ] <= groundHeight || particle->pos[ 2 ] >= skyHeight )
			return CG_SetParticleActive( particle, ACT_NOT );
		*/
	}

	//	checkvisibletime += trap_Milliseconds() - msec;
	return ( qtrue );
}


/* */
static void
CG_SnowParticleRender ( cg_atmosphericParticle_t *particle )
{

	// Draw a snowflake
	vec3_t		forward, right;
	polyVert_t	verts[3];
	vec2_t		line;
	float		len, frac, sinTumbling, cosTumbling, particleWidth, dist;
	vec3_t		start, finish;
	float		groundHeight;

	//	int			msec = trap_Milliseconds();
	//	n_rendertime++;
	if ( particle->active == ACT_NOT )
	{

		//		rendertime += trap_Milliseconds() - msec;
		return;
	}

	if ( CG_CullPoint( particle->pos) )
	{
		return;
	}

	VectorCopy( particle->pos, start );
	sinTumbling = sin( particle->pos[2] * 0.03125f * (0.5f * particle->weight) );
	cosTumbling = cos( (particle->pos[2] + particle->pos[1]) * 0.03125f * (0.5f * particle->weight) );
	start[0] += 24 * ( 1 - particle->deltaNormalized[2] ) * sinTumbling;
	start[1] += 24 * ( 1 - particle->deltaNormalized[2] ) * cosTumbling;

	// Make sure it doesn't clip through surfaces
	groundHeight = BG_GetSkyGroundHeightAtPoint( start );
	len = particle->height;
	if ( start[2] <= groundHeight )
	{

		// Stop snow going through surfaces.
		len = particle->height - groundHeight + start[2];
		frac = start[2];
		VectorMA( start, len - particle->height, particle->deltaNormalized, start );
	}

	if ( len <= 0 )
	{

		//		rendertime += trap_Milliseconds() - msec;
		return;
	}

	line[0] = particle->pos[0] - cg.refdef_current->vieworg[0];
	line[1] = particle->pos[1] - cg.refdef_current->vieworg[1];
	dist = DistanceSquared( particle->pos, cg.refdef_current->vieworg );

	// dist becomes scale
	if ( dist > Square( 500.f) )
	{
		dist = 1.f + ( (dist - Square( 500.f)) * (10.f / Square( 2000.f)) );
	}
	else
	{
		dist = 1.f;
	}

	len *= dist;
	VectorCopy( particle->deltaNormalized, forward );
	VectorMA( start, -(len /** sinTumbling*/ ), forward, finish );
	line[0] = DotProduct( forward, cg.refdef_current->viewaxis[1] );
	line[1] = DotProduct( forward, cg.refdef_current->viewaxis[2] );
	VectorScale( cg.refdef_current->viewaxis[1], line[1], right );
	VectorMA( right, -line[0], cg.refdef_current->viewaxis[2], right );
	VectorNormalize( right );
	particleWidth = dist * ( /*cosTumbling **/ particle->weight );
	VectorMA( finish, -particleWidth, right, verts[0].xyz );
	verts[0].st[0] = 0;
	verts[0].st[1] = 0;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;
	VectorMA( start, -particleWidth, right, verts[1].xyz );
	verts[1].st[0] = 0;
	verts[1].st[1] = 1;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;
	VectorMA( start, particleWidth, right, verts[2].xyz );
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;
	CG_AddPolyToPool( *particle->effectshader, verts );

	//	rendertime += trap_Milliseconds() - msec;
}


/*
**	Set up gust parameters.
*/
static void
CG_EffectGust ( void )
{

	// Generate random values for the next gust
	int diff;
	cg_atmFx.baseEndTime = cg.time + cg_atmFx.baseMinTime + ( rand() % (cg_atmFx.baseMaxTime - cg_atmFx.baseMinTime) );
	diff = cg_atmFx.changeMaxTime - cg_atmFx.changeMinTime;
	cg_atmFx.gustStartTime = cg_atmFx.baseEndTime + cg_atmFx.changeMinTime + ( diff ? (rand() % diff) : 0 );
	diff = cg_atmFx.gustMaxTime - cg_atmFx.gustMinTime;
	cg_atmFx.gustEndTime = cg_atmFx.gustStartTime + cg_atmFx.gustMinTime + ( diff ? (rand() % diff) : 0 );
	diff = cg_atmFx.changeMaxTime - cg_atmFx.changeMinTime;
	cg_atmFx.baseStartTime = cg_atmFx.gustEndTime + cg_atmFx.changeMinTime + ( diff ? (rand() % diff) : 0 );
}


/* */
static qboolean
CG_EffectGustCurrent ( vec3_t curr, float *weight, int *num )
{

	// Calculate direction for new drops.
	vec3_t	temp;
	float	frac;
	if ( cg.time < cg_atmFx.baseEndTime )
	{
		VectorCopy( cg_atmFx.baseVec, curr );
		*weight = cg_atmFx.baseWeight;
		*num = cg_atmFx.baseDrops;
	}
	else
	{
		VectorSubtract( cg_atmFx.gustVec, cg_atmFx.baseVec, temp );
		if ( cg.time < cg_atmFx.gustStartTime )
		{
			frac = ( (float) (cg.time - cg_atmFx.baseEndTime) ) / ( (float) (cg_atmFx.gustStartTime - cg_atmFx.baseEndTime) );
			VectorMA( cg_atmFx.baseVec, frac, temp, curr );
			*weight = cg_atmFx.baseWeight + ( cg_atmFx.gustWeight - cg_atmFx.baseWeight ) * frac;
			*num = cg_atmFx.baseDrops + ( (float) (cg_atmFx.gustDrops - cg_atmFx.baseDrops) ) * frac;
		}
		else if ( cg.time < cg_atmFx.gustEndTime )
		{
			VectorCopy( cg_atmFx.gustVec, curr );
			*weight = cg_atmFx.gustWeight;
			*num = cg_atmFx.gustDrops;
		}
		else
		{
			frac = 1.0 - ( (float) (cg.time - cg_atmFx.gustEndTime) ) / ( (float) (cg_atmFx.baseStartTime - cg_atmFx.gustEndTime) );
			VectorMA( cg_atmFx.baseVec, frac, temp, curr );
			*weight = cg_atmFx.baseWeight + ( cg_atmFx.gustWeight - cg_atmFx.baseWeight ) * frac;
			*num = cg_atmFx.baseDrops + ( (float) (cg_atmFx.gustDrops - cg_atmFx.baseDrops) ) * frac;
			if ( cg.time >= cg_atmFx.baseStartTime )
			{
				return ( qtrue );
			}
		}
	}

	return ( qfalse );
}


/* */
static void
CG_EP_ParseFloats ( char *floatstr, float *f1, float *f2 )
{

	// Parse the float or floats
	char	*middleptr;
	char	buff[64];
	Q_strncpyz( buff, floatstr, sizeof(buff) );
	for ( middleptr = buff; *middleptr && *middleptr != ' '; middleptr++ );
	if ( *middleptr )
	{
		*middleptr++ = 0;
		*f1 = atof( floatstr );
		*f2 = atof( middleptr );
	}
	else
	{
		*f1 = *f2 = atof( floatstr );
	}
}


/* */
static void
CG_EP_ParseInts ( char *intstr, int *i1, int *i2 )
{

	// Parse the int or ints
	char	*middleptr;
	char	buff[64];
	Q_strncpyz( buff, intstr, sizeof(buff) );
	for ( middleptr = buff; *middleptr && *middleptr != ' '; middleptr++ );
	if ( *middleptr )
	{
		*middleptr++ = 0;
		*i1 = atof( intstr );
		*i2 = atof( middleptr );
	}
	else
	{
		*i1 = *i2 = atof( intstr );
	}
}

atmFXType_t currentatmFXType = ATM_NONE;

/* */
void
CG_EffectParse ( const char *effectstr )
{

	// Split the string into it's component parts.
	float		bmin, bmax, cmin, cmax, gmin, gmax, bdrop, gdrop /*, wsplash, lsplash*/ ;
	int			count, bheight;
	char		*startptr, *eqptr, *endptr;
	char		workbuff[128];
	atmFXType_t atmFXType = ATM_NONE;
	if ( CG_AtmosphericKludge() )
	{
		return;
	}

	// Set up some default values
	cg_atmFx.baseVec[0] = cg_atmFx.baseVec[1] = 0;
	cg_atmFx.gustVec[0] = cg_atmFx.gustVec[1] = 100;
	bmin = 5;
	bmax = 10;
	cmin = 1;
	cmax = 1;
	gmin = 0;
	gmax = 2;
	bdrop = gdrop = 300;
	cg_atmFx.baseWeight = 0.7f;
	cg_atmFx.gustWeight = 1.5f;
	bheight = 0;

	// Parse the parameter string
	Q_strncpyz( workbuff, effectstr, sizeof(workbuff) );
	for ( startptr = workbuff; *startptr; )
	{
		for ( eqptr = startptr; *eqptr && *eqptr != '=' && *eqptr != ','; eqptr++ );
		if ( !*eqptr )
		{
			break;					// No more string
		}

		if ( *eqptr == ',' )
		{
			startptr = eqptr + 1;	// Bad argument, continue
			continue;
		}

		*eqptr++ = 0;
		for ( endptr = eqptr; *endptr && *endptr != ','; endptr++ );
		if ( *endptr )
		{
			*endptr++ = 0;
		}

		if ( atmFXType == ATM_NONE )
		{
			if ( Q_stricmp( startptr, "T") )
			{
				cg_atmFx.numDrops = 0;
				CG_Printf( "Atmospheric effect must start with a type.\n" );
				return;
			}

			if ( !Q_stricmp( eqptr, "RAIN") /* && cg_rain.integer != -1*/ )
			{
				atmFXType = ATM_RAIN;
				cg_atmFx.ParticleCheckVisible = &CG_RainParticleCheckVisible;
				cg_atmFx.ParticleGenerate = &CG_RainParticleGenerate;
				cg_atmFx.ParticleRender = &CG_RainParticleRender;
				cg_atmFx.baseVec[2] = cg_atmFx.gustVec[2] = -ATMOSPHERIC_RAIN_SPEED;
			}
			else if ( !Q_stricmp( eqptr, "SNOW") /*&& cg_snow.integer != -1 */ )
			{
				atmFXType = ATM_SNOW;
				cg_atmFx.ParticleCheckVisible = &CG_SnowParticleCheckVisible;
				cg_atmFx.ParticleGenerate = &CG_SnowParticleGenerate;
				cg_atmFx.ParticleRender = &CG_SnowParticleRender;
				cg_atmFx.baseVec[2] = cg_atmFx.gustVec[2] = -ATMOSPHERIC_SNOW_SPEED;
			}
			else if ( !Q_stricmp( eqptr, "STORM") )
			{// UQ1: Added... Only to be used on OUTDOOR maps!!!
				atmFXType = ATM_STORM;
				cg_atmFx.numDrops = 0;
				render_storm_2D = qtrue;
				CG_Printf("^3STORM IS ON!!!\n");
			}
			else
			{
				cg_atmFx.numDrops = 0;
				CG_Printf( "Only effect type 'rain' and 'snow' are supported.\n" );
				return;
			}
		}
		else
		{
			if ( !Q_stricmp( startptr, "B") )
			{
				CG_EP_ParseFloats( eqptr, &bmin, &bmax );
			}
			else if ( !Q_stricmp( startptr, "C") )
			{
				CG_EP_ParseFloats( eqptr, &cmin, &cmax );
			}
			else if ( !Q_stricmp( startptr, "G") )
			{
				CG_EP_ParseFloats( eqptr, &gmin, &gmax );
			}
			else if ( !Q_stricmp( startptr, "BV") )
			{
				CG_EP_ParseFloats( eqptr, &cg_atmFx.baseVec[0], &cg_atmFx.baseVec[1] );
			}
			else if ( !Q_stricmp( startptr, "GV") )
			{
				CG_EP_ParseFloats( eqptr, &cg_atmFx.gustVec[0], &cg_atmFx.gustVec[1] );
			}
			else if ( !Q_stricmp( startptr, "W") )
			{
				CG_EP_ParseFloats( eqptr, &cg_atmFx.baseWeight, &cg_atmFx.gustWeight );
			}
			else if ( !Q_stricmp( startptr, "D") )
			{
				CG_EP_ParseFloats( eqptr, &bdrop, &gdrop );
			}
			else if ( !Q_stricmp( startptr, "H") )
			{
				CG_EP_ParseInts( eqptr, &bheight, &bheight );
			}
			else
			{
				CG_Printf( "Unknown effect key '%s'.\n", startptr );
			}
		}

		startptr = endptr;
	}

	/*	if( cg_rain.integer == 1 ) {
				atmFXType = ATM_RAIN;
				cg_atmFx.ParticleCheckVisible = &CG_RainParticleCheckVisible;
				cg_atmFx.ParticleGenerate = &CG_RainParticleGenerate;
				cg_atmFx.ParticleRender = &CG_RainParticleRender;

				cg_atmFx.baseVec[2] = cg_atmFx.gustVec[2] = - ATMOSPHERIC_RAIN_SPEED;
			}
	  if(  cg_snow.integer == 1 ) {
				atmFXType = ATM_SNOW;
				cg_atmFx.ParticleCheckVisible = &CG_SnowParticleCheckVisible;
				cg_atmFx.ParticleGenerate = &CG_SnowParticleGenerate;
				cg_atmFx.ParticleRender = &CG_SnowParticleRender;

				cg_atmFx.baseVec[2] = cg_atmFx.gustVec[2] = - ATMOSPHERIC_SNOW_SPEED;
	  }*/
	if ( atmFXType == ATM_NONE || atmFXType == ATM_STORM || !BG_LoadTraceMap( cgs.rawmapname, cg.mapcoordsMins, cg.mapcoordsMaxs) )
	{

		// No effects
		cg_atmFx.numDrops = -1;
		return;
	}

	cg_atmFx.baseHeightOffset = bheight;
	if ( cg_atmFx.baseHeightOffset < 0 )
	{
		cg_atmFx.baseHeightOffset = 0;
	}

	cg_atmFx.baseMinTime = 1000 * bmin;
	cg_atmFx.baseMaxTime = 1000 * bmax;
	cg_atmFx.changeMinTime = 1000 * cmin;
	cg_atmFx.changeMaxTime = 1000 * cmax;
	cg_atmFx.gustMinTime = 1000 * gmin;
	cg_atmFx.gustMaxTime = 1000 * gmax;
	if ( atmFXType == ATM_RAIN )
	{
		cg_atmFx.baseDrops = bdrop*8;
		cg_atmFx.gustDrops = gdrop*8;
		cg_atmFx.numDrops = ( cg_atmFx.baseDrops > cg_atmFx.gustDrops ) ? cg_atmFx.baseDrops : cg_atmFx.gustDrops;
	}
	else
	{
		cg_atmFx.baseDrops = bdrop;
		cg_atmFx.gustDrops = gdrop;
		cg_atmFx.numDrops = ( cg_atmFx.baseDrops > cg_atmFx.gustDrops ) ? cg_atmFx.baseDrops : cg_atmFx.gustDrops;
	}

	if ( cg_atmFx.numDrops > MAX_ATMOSPHERIC_PARTICLES )
	{
		cg_atmFx.numDrops = MAX_ATMOSPHERIC_PARTICLES;
	}

	// Load graphics
	// Rain
	if ( atmFXType == ATM_RAIN )
	{
		cg_atmFx.numEffectShaders = 1;
		//cg_atmFx.effectshaders[0] = trap_R_RegisterShader( "gfx/misc/raindrop" );
		cg_atmFx.effectshaders[0] = trap_R_RegisterShader( "gfx/misc/raindrop2" );
		if ( !(cg_atmFx.effectshaders[0]) )
		{
			cg_atmFx.effectshaders[0] = -1;
			cg_atmFx.numEffectShaders = 0;
		}

		// Snow
	}
	else if ( atmFXType == ATM_SNOW )
	{
		cg_atmFx.numEffectShaders = 1;
		cg_atmFx.effectshaders[0] = trap_R_RegisterShader( "gfx/misc/snow" );

		// This really should never happen
	}
	else
	{
		cg_atmFx.numEffectShaders = 0;
	}

	// Initialise atmospheric effect to prevent all particles falling at the start
	for ( count = 0; count < cg_atmFx.numDrops; count++ )
	{
		cg_atmFx.particles[count].nextDropTime = ATMOSPHERIC_DROPDELAY + ( rand() % ATMOSPHERIC_DROPDELAY );
	}

	CG_EffectGust();

	currentatmFXType = atmFXType;
}

float best_particle_dist = 99999.9f;
extern float	HeightDistance ( vec3_t v1, vec3_t v2 );

/*
** Main render loop
*/
void
CG_AddAtmosphericEffects ( void )
{

	// Add atmospheric effects (e.g. rain, snow etc.) to view
	int							curr, max, currnum;
	cg_atmosphericParticle_t	*particle;
	vec3_t						currvec;
	float						currweight;
	if ( cg_atmFx.numDrops <= 0 || cg_atmFx.numEffectShaders == 0 || cg_atmosphericEffects.value <= 0 )
	{
		return;
	}

	best_particle_dist = 99999.9f;

#ifndef ATM_NEW
	CG_ClearPolyPool();
#endif // ATM_NEW
	max = cg_atmosphericEffects.value < 1 ? cg_atmosphericEffects.value * cg_atmFx.numDrops : cg_atmFx.numDrops;
	if ( CG_EffectGustCurrent( currvec, &currweight, &currnum) )
	{
		CG_EffectGust();	// Recalculate gust parameters
	}

	// ydnar: allow parametric management of drop count for swelling/waning precip
	cg_atmFx.oldDropsActive = cg_atmFx.dropsActive;
	cg_atmFx.dropsActive = 0;
	cg_atmFx.dropsRendered = cg_atmFx.dropsCreated = cg_atmFx.dropsSkipped = 0;

	//	getgroundtime = getskytime = rendertime = checkvisibletime = generatetime = 0;
	//	n_getgroundtime = n_getskytime = n_rendertime = n_checkvisibletime = n_generatetime = 0;
	VectorSet( cg_atmFx.viewDir, cg.refdef_current->viewaxis[0][0], cg.refdef_current->viewaxis[0][1], 0.f );
	for ( curr = 0; curr < max; curr++ )
	{
		float distance = 99999.9f;

		particle = &cg_atmFx.particles[curr];

		//%	if( !CG_SnowParticleCheckVisible( particle ) )
		if ( !cg_atmFx.ParticleCheckVisible( particle) )
		{

			// Effect has terminated / fallen from screen view

			/*
			if( !particle->nextDropTime )
			{
				// Stop rain being synchronized 
				particle->nextDropTime = cg.time + rand() % ATMOSPHERIC_DROPDELAY;
			}
			if( currnum < curr || particle->nextDropTime > cg.time )
			{
				cg_atmFx.dropsRendered++;
				continue;
			} */

			//%	if( !CG_SnowParticleGenerate( particle, currvec, currweight ) )
			if ( !cg_atmFx.ParticleGenerate( particle, currvec, currweight) )
			{

				// Ensure it doesn't attempt to generate every frame, to prevent
				// 'clumping' when there's only a small sky area available.
				particle->nextDropTime = cg.time + ATMOSPHERIC_DROPDELAY;
				continue;
			}
			else
			{
				cg_atmFx.dropsCreated++;
			}
		}
/*		else
		{
			distance = VectorDistance(cg.refdef.vieworg, particle->pos);

			if ( distance < best_particle_dist && HeightDistance(cg.refdef.vieworg, particle->pos) < 64)
			{
				trace_t tr;
				CG_Trace(&tr, cg.refdef.vieworg, NULL, NULL, particle->pos, cg.clientNum, MASK_SHOT);

				if (tr.fraction == 1)
				{
					best_particle_dist = distance;
					VectorCopy(particle->pos, storm_position);
					VectorCopy(particle->delta, storm_angles);
				}
			}
		}*/

		//%	CG_RainParticleRender( particle );
		cg_atmFx.ParticleRender( particle );
		cg_atmFx.dropsActive++;
	}

//	if ( currentatmFXType == ATM_RAIN && cg_atmFx.dropsActive > 0)
//		CG_AddStorm();

	//	CG_RenderPolyPool();
	cg_atmFx.lastRainTime = cg.time;

	//	CG_Printf( "Active: %d Generated: %d Rendered: %d Skipped: %d\n", cg_atmFx.dropsActive, cg_atmFx.dropsCreated, cg_atmFx.dropsRendered, cg_atmFx.dropsSkipped );
	//	CG_Printf( "gg: %i gs: %i rt: %i cv: %i ge: %i\n", getgroundtime, getskytime, rendertime, checkvisibletime, generatetime );
	//	CG_Printf( "\\-> %i \\-> %i \\-> %i \\-> %i \\-> %i\n", n_getgroundtime, n_getskytime, n_rendertime, n_checkvisibletime, n_generatetime );
}
