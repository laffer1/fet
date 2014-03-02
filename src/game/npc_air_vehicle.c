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


#include "g_local.h"
#ifdef __NPC__
extern vmCvar_t bot_thinktime;

// Some simple anims ready for use... Unique1... define __ANIM_INFO__ to get more from player moves.
#define NPC_LEGS_ANIM_WALK			37
#define NPC_LEGS_ANIM_WALK_BACK		556
#define NPC_LEGS_ANIM_STAND			575
#define NPC_LEGS_ANIM_CROUCH		42
#define NPC_LEGS_ANIM_CROUCH_WALK	63
#define NPC_LEGS_ANIM_JUMP			612
#define NPC_LEGS_ANIM_RUN			549
#define NPC_TORSO_ANIM_WALK			37
#define NPC_TORSO_ANIM_WALK_BACK	44
#define NPC_TORSO_ANIM_STAND		575
#define NPC_TORSO_ANIM_CROUCH		575
#define NPC_TORSO_ANIM_CROUCH_WALK	575
#define NPC_TORSO_ANIM_JUMP			100
#define NPC_TORSO_ANIM_RUN			37
vec3_t			planeMins = { -48.0f, -48.0f, -24.0f };
vec3_t			planeMaxs = { +48.0f, +48.0f, +48.0f };


/* */
float
G_GetSkyHeightAtPoint ( vec3_t pos )
{
	vec3_t	roof;
	trace_t trace;
	VectorCopy( pos, roof );
	roof[2] += 64000;
	trap_Trace( &trace, pos, NULL, NULL, roof, -1, MASK_SHOT );
	if ( trace.endpos[2] == roof[2] )
	{
		VectorCopy( pos, roof );
		roof[2] -= 64000;
		trap_Trace( &trace, pos, NULL, NULL, roof, -1, MASK_SHOT );
	}

	return ( trace.endpos[2] );
}


//===========================================================================
// Routine      : NPC_Bomber_Spawn

// Description  : Spawn a bomber NPC.
void
NPC_Bomber_Spawn ( vec3_t marker_origin, int marker_entitynum, gentity_t *owner )
{
	gentity_t	*NPC = G_Spawn();
	vec3_t		fwd, tmpangles, traceangle, roof, spawnpoint_origin, spawnpoint_angles, spawn_location, spawn_angles,
		exit_location;
	trace_t		trace;
	gentity_t	*spawnpoint;

	// Now we need to find a spawn point for the plane...
	roof[2] = G_GetSkyHeightAtPoint( marker_origin ) - 8;
	roof[0] = marker_origin[0];
	roof[1] = marker_origin[1];
	spawnpoint = SelectCTFSpawnPoint( owner->client->sess.sessionTeam, owner->client->pers.teamState.state,
									  spawnpoint_origin, spawnpoint_angles, owner->client->sess.spawnObjectiveIndex );
	spawn_location[2] = roof[2];
	spawn_location[1] = spawnpoint->s.origin[1];
	spawn_location[0] = spawnpoint->s.origin[0];
	VectorSubtract( spawn_location, roof, tmpangles );
	vectoangles( tmpangles, traceangle );
	AngleVectors( traceangle, fwd, NULL, NULL );
	VectorCopy( roof, spawn_location );
	spawn_location[0] += ( fwd[0] * 64000 );
	spawn_location[1] += ( fwd[1] * 64000 );
	spawn_location[2] += ( fwd[2] * 64000 );
	trap_Trace( &trace, roof, NULL, NULL, spawn_location, -1, MASK_SHOT );
	VectorCopy( trace.endpos, spawn_location );

	// We should now have a spawnpoint and angles to follow...
	// Now we need to find an exit point for the plane...
	VectorSubtract( roof, spawn_location, tmpangles );
	vectoangles( tmpangles, traceangle );
	AngleVectors( traceangle, fwd, NULL, NULL );
	VectorCopy( spawn_location, exit_location );
	exit_location[0] += ( fwd[0] * 64000 );
	exit_location[1] += ( fwd[1] * 64000 );
	exit_location[2] += ( fwd[2] * 64000 );
	trap_Trace( &trace, spawn_location, NULL, NULL, exit_location, -1, MASK_SHOT );
	VectorCopy( trace.endpos, exit_location );
	VectorCopy( exit_location, NPC->pos1 ); // Remember exit point as NPC->pos1

	// We should now have a point we can remove the plane at...
	VectorSubtract( NPC->pos1, spawn_location, NPC->move_vector );
	vectoangles( NPC->move_vector, spawn_angles );
	G_SetAngle( NPC, spawn_angles );
	VectorCopy( spawn_location, NPC->s.origin );
	NPC->count2 = marker_entitynum;
	NPC->parent = owner;
	NPC->parent->Airstrike_Ready = qfalse;
	NPC_Setup( NPC );
	NPC->model = "models/mapobjects/vehicles/m109.mdc";
	NPC->s.modelindex = G_ModelIndex( NPC->model );
	NPC->NPC_Class = CLASS_PLANE_BOMBER;
	NPC->s.modelindex2 = NPC->NPC_Class;	// For cgame access...
	G_GlobalClientEvent( EV_AIRSTRIKEMESSAGE, 2, owner->s.number );
}

int		cinematic_plane_enterpos_total = -1;
vec3_t	cinematic_plane_enterpos[32];
int		cinematic_plane_exitpos_total = -1;
vec3_t	cinematic_plane_exitpos[32];


/* */
void
NPC_CinematicPlane_Enter ( gentity_t *ent )
{
	cinematic_plane_enterpos_total++;
	VectorCopy( ent->s.origin, cinematic_plane_enterpos[cinematic_plane_enterpos_total] );
	G_FreeEntity( ent );
}


/* */
void
NPC_CinematicPlane_Exit ( gentity_t *ent )
{
	cinematic_plane_exitpos_total++;
	VectorCopy( ent->s.origin, cinematic_plane_exitpos[cinematic_plane_exitpos_total] );
	G_FreeEntity( ent );
}


//===========================================================================
// Routine      : NPC_Plane_Spawn

// Description  : Spawn a cinematic plane NPC.
void
NPC_Plane_Spawn ( vec3_t marker_origin )
{
	gentity_t	*NPC = G_Spawn();
	vec3_t		spawn_location, spawn_angles;
	int			posNum = Q_TrueRand( 0, cinematic_plane_enterpos_total );
	VectorCopy( marker_origin, NPC->pos1 );
	VectorCopy( cinematic_plane_enterpos[posNum], spawn_location );
	VectorSubtract( NPC->pos1, spawn_location, NPC->move_vector );
	vectoangles( NPC->move_vector, spawn_angles );
	G_SetAngle( NPC, spawn_angles );
	VectorCopy( spawn_location, NPC->s.origin );
	NPC_Setup( NPC );

	// Is this plane damaged (smoking)???
	NPC->s.onFireStart = Q_irand( 0, 1 );		//Q_TrueRand(0,1);
	NPC->model = "models/mapobjects/vehicles/m109.mdc";
	NPC->s.modelindex = G_ModelIndex( NPC->model );
	NPC->NPC_Class = CLASS_PLANE;
	NPC->s.modelindex2 = NPC->NPC_Class;		// For cgame access...

	// We use this value to free the entity after it has gone off the map a bit... Hopefully no longer visible...
	NPC->freetime = level.time + 60000;

	//G_Printf("Plane spawned!\n");
}

extern int	BotMP_Pick_Random_Goal ( void );	// unique_goal.c
int			next_plane_time = 5000;

qboolean	good_height_found = qfalse;
float		last_good_height;

float
BG_GetSkyHeightAtPoint2 ( vec3_t pos )
{
	trace_t tr;
	vec3_t	try_pos, up_pos;

	VectorCopy(pos, try_pos);
	try_pos[2]-=512;
	VectorCopy(pos, up_pos);
	up_pos[2]+=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, ENTITYNUM_NONE, MASK_SHOT | MASK_OPAQUE | MASK_WATER | MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000)
		return ( tr.endpos[2] - 64 );

	VectorCopy(pos, try_pos);
	try_pos[2]+=64;
	VectorCopy(pos, up_pos);
	up_pos[2]+=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, -1, MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000)
		return ( tr.endpos[2] - 64 );

	VectorCopy(pos, try_pos);
	try_pos[2]+=64;
	VectorCopy(pos, up_pos);
	up_pos[2]-=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, -1, MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000 && tr.endpos[2] > -60000)
		return ( tr.endpos[2] - 64 );

	VectorCopy(pos, try_pos);
	try_pos[2]+=64;
	try_pos[0]+=256;
	VectorCopy(pos, up_pos);
	up_pos[2]-=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, -1, MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000 && tr.endpos[2] > -60000)
		return ( tr.endpos[2] - 64 );

	VectorCopy(pos, try_pos);
	try_pos[2]+=64;
	try_pos[0]-=256;
	VectorCopy(pos, up_pos);
	up_pos[2]-=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, -1, MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000 && tr.endpos[2] > -60000)
		return ( tr.endpos[2] - 64 );

	VectorCopy(pos, try_pos);
	try_pos[2]+=64;
	try_pos[1]+=256;
	VectorCopy(pos, up_pos);
	up_pos[2]-=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, -1, MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000 && tr.endpos[2] > -60000)
		return ( tr.endpos[2] - 64 );

	VectorCopy(pos, try_pos);
	try_pos[2]+=64;
	try_pos[1]-=256;
	VectorCopy(pos, up_pos);
	up_pos[2]-=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, -1, MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000 && tr.endpos[2] > -60000)
		return ( tr.endpos[2] - 64 );

	VectorCopy(pos, try_pos);
	try_pos[2]+=64;
	try_pos[0]+=256;
	try_pos[1]-=256;
	VectorCopy(pos, up_pos);
	up_pos[2]-=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, -1, MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000 && tr.endpos[2] > -60000)
		return ( tr.endpos[2] - 64 );

	VectorCopy(pos, try_pos);
	try_pos[2]+=64;
	try_pos[0]+=256;
	try_pos[1]+=256;
	VectorCopy(pos, up_pos);
	up_pos[2]-=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, -1, MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000 && tr.endpos[2] > -60000)
		return ( tr.endpos[2] - 64 );

	VectorCopy(pos, try_pos);
	try_pos[2]+=64;
	try_pos[0]-=256;
	try_pos[1]-=256;
	VectorCopy(pos, up_pos);
	up_pos[2]-=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, -1, MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000 && tr.endpos[2] > -60000)
		return ( tr.endpos[2] - 64 );

	VectorCopy(pos, try_pos);
	try_pos[2]+=64;
	try_pos[0]-=256;
	try_pos[1]+=256;
	VectorCopy(pos, up_pos);
	up_pos[2]-=65500;

	trap_Trace( &tr, try_pos, NULL, NULL, up_pos, -1, MASK_SOLID );

	if (VectorDistance(tr.endpos, pos) > 8 && tr.endpos[2] < 60000 && tr.endpos[2] > -60000)
		return ( tr.endpos[2] - 64 );

	good_height_found = qfalse;
	
	if (good_height_found)
		return last_good_height;

	// If all else fails!
	return pos[2] - 64;
}

/* */
void
NPC_Plane_Cinematics ( void )
{	// Call this each frame to add cinematic planes to a map...
	if ( level.time <= 5000 )
	{
		return;
	}

	if ( level.restarted )
	{
		return;
	}

	if ( next_plane_time > level.time )
	{
		return;
	}
	else
	{
		vec3_t	center_origin;
		int		posNum = Q_irand( 0, cinematic_plane_exitpos_total );
		next_plane_time = level.time + Q_TrueRand( 500, 8000 );
		VectorCopy( cinematic_plane_exitpos[posNum], center_origin );
		center_origin[2] = BG_GetSkyHeightAtPoint2( center_origin ) - 8;
		if ( center_origin[2] > 60000 )
		{
			center_origin[2] = cinematic_plane_exitpos[posNum][2];
		}

		if (!good_height_found && VectorDistance(center_origin, cinematic_plane_exitpos[posNum]) > 4)
		{
			last_good_height = center_origin[2];
			good_height_found = qtrue;
		}

		G_Printf("Spawn cinematic plane at %f %f %f. Sky height %f.\n", center_origin[0], center_origin[1], center_origin[2], BG_GetSkyHeightAtPoint2( center_origin ));
		NPC_Plane_Spawn( center_origin );
	}
}


/* */
void
NPC_AI_Air_Move ( gentity_t *NPC )
{
	vec3_t		origin, angles, move, amove;
	gentity_t	*obstacle = NULL;
	G_SetMovedir( NPC->s.angles, NPC->movedir );
	NPC->s.pos.trDelta[0] = 120;
	NPC->s.pos.trDelta[1] = 120;
	NPC->s.pos.trDelta[2] = 120;
	NPC->instantVelocity[0] = 120;
	NPC->instantVelocity[1] = 120;
	NPC->instantVelocity[2] = 120;
	trap_SnapVector( NPC->instantVelocity );
	trap_SnapVector( NPC->s.pos.trDelta );
	if ( NPC->NPC_Class == CLASS_PLANE )
	{								// Planes type NPC routing here... Just head forward for now...
		vec3_t	fwd;
		if ( VectorDistance( NPC->pos1, NPC->r.currentOrigin) <= 64 )
		{							// We've left the map area.. Remove the entity... Exit the procedure...
			NPC->freetime = level.time + 6000;
			G_AddEvent( NPC, EV_DESTROY_PARTICLES, 0 );
		}

		if ( NPC->freetime < level.time )
		{
			G_FreeEntity( NPC );
			return;
		}

		NPC->soundLoop = G_SoundIndex( "sound/weapons/airstrike/airstrike_plane_early.wav" );
		NPC->s.loopSound = NPC->soundLoop;
		AngleVectors( NPC->r.currentAngles, fwd, NULL, NULL );
		VectorCopy( NPC->r.currentOrigin, origin );
		if ( NPC->s.onFireStart > 0 )
		{
			NPC->s.weapon = WP_SMOKETRAIL;
		}

		origin[0] += ( fwd[0] * 64 );
		origin[1] += ( fwd[1] * 64 );
		VectorSubtract( origin, NPC->r.currentOrigin, NPC->move_vector );
		vectoangles( NPC->move_vector, angles );
		G_SetAngle( NPC, angles );
		VectorSubtract( origin, NPC->r.currentOrigin, move );
		VectorSubtract( angles, NPC->r.currentAngles, amove );
		if ( !G_NPCPush( NPC, move, amove, &obstacle) )
		{

			//script_mover_blocked( NPC, obstacle );
		}

		angles[PITCH] = 0;			// Always look straight.. No looking at ground as we walk...
		G_SetAngle( NPC, angles );
		VectorCopy( origin, NPC->s.pos.trBase );
		VectorCopy( angles, NPC->s.apos.trBase );

		// Interpolation...
		NPC->s.pos.trDuration = bot_thinktime.integer;
		NPC->s.pos.trTime = level.time + bot_thinktime.integer;
		NPC->s.apos.trDuration = bot_thinktime.integer;
		NPC->s.apos.trTime = level.time + bot_thinktime.integer;
		NPC->s.pos.trType = TR_INTERPOLATE;
		NPC->s.apos.trType = TR_INTERPOLATE;
		NPC->physicsObject = qtrue;
		VectorScale( fwd, 8, NPC->s.pos.trDelta );
		VectorCopy( /*NPC->s.angles*/ NPC->npc_view_angles, NPC->s.apos.trDelta );

		// End Interpolation...

		/*if (VectorDistance(NPC->r.currentOrigin, NPC->pos2) < 2048)
		{// Do we need AAGUN fire at this plane yet???
			vec3_t temp_angle;
			gentity_t *fake_aagun = G_Spawn();

			fake_aagun->s.weapon = WP_MOBILE_MG42;
			VectorCopy(NPC->pos2, fake_aagun->s.origin);
			G_SetOrigin(NPC, NPC->pos2);
			VectorSubtract( NPC->r.currentOrigin, NPC->pos2, temp_angle );
			vectoangles( temp_angle, angles );
			angles[0]+=Q_TrueRand(-16, 16);
			angles[1]+=Q_TrueRand(-16, 16);
			angles[2]+=Q_TrueRand(-16, 16);
			G_SetAngle(NPC, angles);
			Bullet_Fire( fake_aagun, MG42_SPREAD_MP, 0, qfalse );

			G_FreeEntity(fake_aagun);
		}
		
		if (VectorDistance(NPC->r.currentOrigin, NPC->pos3) < 2048)
		{// Do we need AAGUN fire at this plane yet???
			vec3_t temp_angle;
			gentity_t *fake_aagun = G_Spawn();

			fake_aagun->s.weapon = WP_MOBILE_MG42;
			VectorCopy(NPC->pos3, fake_aagun->s.origin);
			G_SetOrigin(NPC, NPC->pos3);
			VectorSubtract( NPC->r.currentOrigin, NPC->pos3, temp_angle );
			vectoangles( temp_angle, angles );
			angles[0]+=Q_TrueRand(-16, 16);
			angles[1]+=Q_TrueRand(-16, 16);
			angles[2]+=Q_TrueRand(-16, 16);
			G_SetAngle(NPC, angles);
			Bullet_Fire( fake_aagun, MG42_SPREAD_MP, 0, qfalse );

			G_FreeEntity(fake_aagun);
		}*/
		return;
	}
	else if ( NPC->NPC_Class == CLASS_PLANE_BOMBER )
	{								// Bomber planes type NPC routing here... Just head forward for now... FIXME: Should link this into air strike code...
		vec3_t	fwd, testpos;
		if ( VectorDistance( NPC->pos1, NPC->r.currentOrigin) <= 64 )
		{							// We've left the map area.. Remove the entity... Exit the procedure...
			NPC->freetime = level.time + 6000;
			G_AddEvent( NPC, EV_DESTROY_PARTICLES, 0 );
		}

		if ( NPC->freetime < level.time )
		{
			G_FreeEntity( NPC );
			return;
		}

		VectorCopy( g_entities[NPC->count2].r.currentOrigin, testpos );
		testpos[2] = NPC->r.currentOrigin[2];
		if ( VectorDistance( testpos, NPC->r.currentOrigin) < 256 )
		{							// This is an airstrike marker.. Drop a bomb here...
			//G_Printf("^1*** ^3DEBUG^5: A bomber plane (ent #^7%i^5) has found it's marker point... FIXME: Add bomb...\n", NPC->s.number);
			NPC->parent->Airstrike_Ready = qtrue;
		}

		if ( VectorDistance( testpos, NPC->r.currentOrigin) < 1024 && !NPC->action )
		{							// Close to drop point.. Make plane sound...
			gentity_t	*te = G_TempEntity( NPC->s.pos.trBase, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/weapons/airstrike/airstrike_plane_npc.wav" );
			te->r.svFlags |= SVF_BROADCAST;
			NPC->action = qtrue;	// Using NPC->action to mark we've done the sound...
		}

		//		if (NPC->action)
		//			NPC->soundLoop = G_SoundIndex( "sound/weapons/airstrike/airstrike_plane_late.wav");
		//		else
		NPC->soundLoop = G_SoundIndex( "sound/weapons/airstrike/airstrike_plane_early.wav" );
		NPC->s.loopSound = NPC->soundLoop;
		AngleVectors( NPC->r.currentAngles, fwd, NULL, NULL );
		VectorCopy( NPC->r.currentOrigin, origin );
		origin[0] += ( fwd[0] * 64 );
		origin[1] += ( fwd[1] * 64 );
		VectorSubtract( NPC->pos1 /*origin*/, NPC->r.currentOrigin, NPC->move_vector );
		vectoangles( NPC->move_vector, angles );
		G_SetAngle( NPC, angles );
		VectorSubtract( origin, NPC->r.currentOrigin, move );
		VectorSubtract( angles, NPC->r.currentAngles, amove );
		if ( !G_NPCPush( NPC, move, amove, &obstacle) )
		{

			//script_mover_blocked( NPC, obstacle );
		}

		angles[PITCH] = 0;			// Always look straight.. No looking at ground as we walk...
		G_SetAngle( NPC, angles );
		VectorCopy( origin, NPC->s.pos.trBase );
		VectorCopy( angles, NPC->s.apos.trBase );

		// Interpolation...
		NPC->s.pos.trDuration = bot_thinktime.integer;
		NPC->s.pos.trTime = level.time + bot_thinktime.integer;
		NPC->s.apos.trDuration = bot_thinktime.integer;
		NPC->s.apos.trTime = level.time + bot_thinktime.integer;
		NPC->s.pos.trType = TR_INTERPOLATE;
		NPC->s.apos.trType = TR_INTERPOLATE;
		NPC->physicsObject = qtrue;
		VectorScale( fwd, 8, NPC->s.pos.trDelta );
		VectorCopy( /*NPC->s.angles*/ NPC->npc_view_angles, NPC->s.apos.trDelta );

		// End Interpolation...
		return;
	}
	else if ( NPC->NPC_Class == CLASS_PLANE_LANDER )
	{								// Paratrooper planes type NPC routing here... Just head forward for now...
		vec3_t	fwd;
		if ( !PointInBounds( NPC->r.currentOrigin, level.mapcoordsMins, level.mapcoordsMaxs) )
		{							// We've left the map area.. Remove the entity... Exit the procedure...
			//G_Printf("^1*** ^3DEBUG^5: A paratrooper plane (ent #^7%i^5) has left the map boundaries.. Freeing the entity...\n", NPC->s.number);
			G_FreeEntity( NPC );
			return;
		}

		AngleVectors( NPC->r.currentAngles, fwd, NULL, NULL );
		VectorCopy( NPC->r.currentOrigin, origin );
		origin[0] += ( fwd[0] * 32 );
		origin[1] += ( fwd[1] * 32 );
		VectorSubtract( origin, NPC->r.currentOrigin, NPC->move_vector );
		vectoangles( NPC->move_vector, angles );

		// Interpolation...
		NPC->s.pos.trDuration = bot_thinktime.integer;
		NPC->s.pos.trTime = level.time + bot_thinktime.integer;
		NPC->s.apos.trDuration = bot_thinktime.integer;
		NPC->s.apos.trTime = level.time + bot_thinktime.integer;
		NPC->s.pos.trType = TR_INTERPOLATE;
		NPC->s.apos.trType = TR_INTERPOLATE;
		NPC->physicsObject = qtrue;
		VectorScale( fwd, 8, NPC->s.pos.trDelta );
		VectorCopy( /*NPC->s.angles*/ NPC->npc_view_angles, NPC->s.apos.trDelta );

		// End Interpolation...
		G_SetAngle( NPC, angles );
	}

	VectorSubtract( origin, NPC->r.currentOrigin, move );
	VectorSubtract( angles, NPC->r.currentAngles, amove );
	if
	(
		NPC->enemy &&
		NPC->action > level.time - 100 &&
		VectorDistance( NPC->r.currentOrigin, NPC->enemy->r.currentOrigin) < 256
	)
	{
		return;						// We're close enough to our enemy now... Don't move any more...
	}

	if ( !G_MoverPush( NPC, move, amove, &obstacle) )
	{

		//script_mover_blocked( NPC, obstacle );
	}

	if ( nodes[NPC->next_node].type == NODE_JUMP )
	{
		NPC->s.legsAnim = NPC_LEGS_ANIM_JUMP;
		NPC->s.torsoAnim = NPC_TORSO_ANIM_JUMP;
	}
	else
	{
		NPC->s.legsAnim = NPC_LEGS_ANIM_WALK;
		NPC->s.torsoAnim = NPC_TORSO_ANIM_WALK;
	}

	angles[PITCH] = 0;				// Always look straight.. No looking at ground as we walk...
	G_SetAngle( NPC, angles );
	VectorCopy( origin, NPC->s.pos.trBase );
	VectorCopy( angles, NPC->s.apos.trBase );
}


//===========================================================================
// Routine      : NPC_AI_Air_Think

// Description  : Main think function for flying NPCs.
void
NPC_AI_Air_Think ( gentity_t *NPC )
{
	if ( level.intermissiontime )
	{		// Don't think in level intermission time.
		return;
	}

	if ( NPC->nextthink )
	{
		if ( NPC->nextthink > level.time )
		{
			return;
		}
	}

	if ( !NPC->s.eType )
	{
		return;
	}

	if ( NPC->s.eType != ET_NPC || NPC->s.number < MAX_CLIENTS )
	{
		return;
	}

	if ( NPC->health )
	{
		if ( NPC->health <= 0 )
		{	// Free the NPC's entity... It's dead!
			if ( NPC->NPC_Class == CLASS_HUMANOID )
			{
				NPC->die;
			}
			else
			{
				G_FreeEntity( NPC );
			}
		}
	}

	NPC_AI_Move( NPC );

	// think again baby
	NPC->nextthink = level.time + bot_thinktime.integer;
}
#endif //__NPC__
