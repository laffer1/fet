#include "g_local.h"
#ifdef __NPC__
extern vmCvar_t bot_thinktime;
vec3_t			tankNPCMins = { -64, -128, -16 };
vec3_t			tankNPCMaxs = { 64, 128, 96 };


//===========================================================================
// Routine      : AIMOD_NAVIGATION_FindTankSpawnNode

// Description  : Find the closest node to the player within a certain range
int
AIMOD_NAVIGATION_FindTankSpawnNode ( gentity_t *bot, int r, int type )
{
	int		i;
	float	closest;
	float	nonLOSclosest;
	float	nonFOVclosest;
	float	dist;
	int		node = -1;
	int		secondbest = -1;
	vec3_t	v;
	float	rng;
	vec3_t	maxs, mins;
	VectorCopy( playerMins, mins );
	VectorCopy( playerMaxs, maxs );

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );
	closest = rng + 1;						// Cuz if we found none in the range, we just found none .
	nonLOSclosest = rng + 1;				// Cuz if we found none in the range, we just found none .
	nonFOVclosest = rng + 1;				// Cuz if we found none in the range, we just found none .

	//
	// Will attempt a close only check first with vis checks...
	//
	closest = 1024.0f;
	for ( i = 0; i < number_of_nodes; i++ ) // Randomize a little???
	{
		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
		dist = VectorLength( v );
		if
		(
			dist < closest &&
			dist < 1024 &&
			dist > 512 &&
			i != bot->current_node &&
			i != bot->next_node &&
			nodes[i].origin[2] - bot->r.currentOrigin[2] < 32
		)									// Height check... never a node above us too far..
		{
			if ( !(nodes[i].type & NODE_ICE) )
			{								// Not on ice (slick)...
				if ( OrgVisible( bot->r.currentOrigin, nodes[i].origin, bot->s.number) )
				{							// Never go onto ice!
					secondbest = node;
					node = i;
					closest = dist;
				}
			}
		}
	}

	if ( secondbest > 0 && Q_TrueRand( 0, 2) == 2 )
	{
		return ( secondbest );
	}
	else if ( node > 0 )
	{
		return ( node );
	}

	//
	// Now a thorough, but fast check! No vis checks!
	//
	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );
	closest = rng + 1;						// Cuz if we found none in the range, we just found none .
	nonLOSclosest = rng + 1;				// Cuz if we found none in the range, we just found none .
	nonFOVclosest = rng + 1;				// Cuz if we found none in the range, we just found none .
	for ( i = 0; i < number_of_nodes; i++ ) // Randomize a little???
	{
		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
		dist = VectorLength( v );
		if
		(
			dist < closest &&
			dist < 1024 &&
			dist > 512 &&
			i != bot->current_node &&
			i != bot->next_node &&
			nodes[i].origin[2] - bot->r.currentOrigin[2] < 32
		)									// Height check... never a node above us too far..
		{
			if ( !(nodes[i].type & NODE_ICE) )
			{								// Not on ice (slick)...
				//if (OrgVisible(bot->r.currentOrigin, nodes[i].origin, bot->s.number))
				{							// Never go onto ice!
					secondbest = node;
					node = i;
					closest = dist;
				}
			}
		}
	}

	if ( secondbest > 0 && Q_TrueRand( 0, 2) == 2 )
	{
		return ( secondbest );
	}
	else
	{
		return ( node );
	}
}


//===========================================================================
// Routine      : NPC_Tank_Spawn

// Description  : Spawn a Tank NPC.
void
NPC_Tank_Spawn ( vec3_t spawn_location )
{
	gentity_t	*NPC = G_Spawn();
	vec3_t		spawn_angles;
	char		tagname[MAX_QPATH] = "models/mapobjects/tanks_sd/churchhill.tag";
	VectorCopy( spawn_location, NPC->s.origin );
	VectorCopy( spawn_location, NPC->r.currentOrigin );
	NPC->last_node = AIMOD_NAVIGATION_FindTankSpawnNode( NPC, NODE_DENSITY, NODEFIND_ALL );
	VectorCopy( nodes[NPC->last_node].origin, NPC->s.origin );
	NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
	VectorCopy( nodes[NPC->current_node].origin, NPC->pos1 );
	VectorSubtract( NPC->pos1, spawn_location, NPC->move_vector );
	vectoangles( NPC->move_vector, spawn_angles );
	G_SetAngle( NPC, spawn_angles );
	NPC_Setup( NPC );
	NPC->model = "models/mapobjects/tanks_sd/churchhill.md3";
	NPC->s.modelindex = G_ModelIndex( NPC->model );
	NPC->tagNumber = trap_LoadTag( tagname );
	NPC->s.nextWeapon = G_ModelIndex( "models/mapobjects/tanks_sd/churchhill_turret.md3" );
	NPC->NPC_Class = CLASS_VEHICLE;
	NPC->s.modelindex2 = NPC->NPC_Class;	// For cgame access...

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	VectorCopy( tankNPCMins, NPC->r.mins );
	VectorCopy( tankNPCMaxs, NPC->r.maxs );

	//G_Printf("^4*** ^3NPC DEBUG^5: NPC (tank) Spawned...\n");
}


//===========================================================================
// Routine      : NPC_AI_Vehicle_Move

// Description  : Main move function for vehicle NPCs.
void
NPC_AI_Vehicle_Move ( gentity_t *NPC )
{

}


//===========================================================================
// Routine      : NPC_AI_Vehicle_Think

// Description  : Main think function for flying NPCs.
void
NPC_AI_Vehicle_Think ( gentity_t *NPC )
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


//
// Jaquboss NPC Tank Code...

//
void
tank_think ( gentity_t *self )
{											// delete itselfs when its used
	self->nextthink = level.time + 50;
	self->s.dl_intensity = self->health;	// send health to clients
	if ( self->active )
	{
		self->think = G_FreeEntity;
	}
}


/* */
void
tank_stopusing ( gentity_t *self )
{

}


// this is similar to tankplayer Death
void
tank_die ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod )
{
	vec3_t	dir;
	vec3_t	origin;
	self->takedamage = qfalse;
	self->s.eType = ET_INVISIBLE;
	self->r.contents = 0;
	self->clipmask = 0;
	self->TankMarker = 0;

	// splash damage
	{
		vec3_t	origin;
		trace_t tr;
		VectorCopy( self->r.currentOrigin, origin );
		origin[2] += 4;
		trap_Trace( &tr, origin, vec3_origin, vec3_origin, origin, ENTITYNUM_NONE, MASK_SHOT );
		G_RadiusDamage( origin, NULL, self, 55, 300, self, MOD_EXPLOSIVE );
	}

	BG_EvaluateTrajectory( &self->s.pos, level.time, origin, qfalse, self->s.effect2Time );
	SnapVector( origin );
	G_SetOrigin( self, origin );

	// we don't have a valid direction, so just point straight up
	dir[0] = dir[1] = 0;
	dir[2] = 1;
	{
		gentity_t	*tent = G_TempEntity( self->r.currentOrigin, EV_MISSILE_MISS_LARGE );
		tent->s.eventParm = DirToByte( dir );
	}
	{
		gentity_t	*tent = G_TempEntity( self->r.currentOrigin, EV_SHAKE );
		tent->s.onFireStart = 500;
		tent->r.svFlags |= SVF_BROADCAST;
	}

	G_FreeEntity( self );
}


/* */
void
tank_use ( gentity_t *ent, gentity_t *other, gentity_t *activator )
{

	// HUH?!
	trap_LinkEntity( ent );
}

void			tank_spawn_think ( gentity_t *self );
void			panzer_spawn_think ( gentity_t *self );
extern vec3_t	tankMins;
extern vec3_t	tankMaxs;


/* */
void
tank_spawn ( gentity_t *ent )
{
	gentity_t	*tank;
	vec3_t		offset;
	if ( g_knifeonly.integer != 1 )
	{

		// Spawn the barrel
		tank = G_Spawn();
		tank->classname = "npc_churchilltank";
		tank->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
		tank->r.contents = CONTENTS_TRIGGER | CONTENTS_SOLID;
		tank->r.svFlags = 0;
		tank->s.eType = ET_VEHICLE;
		tank->health = 100;
		tank->s.dl_intensity = 100;
		tank->s.effect1Time = 0;
		tank->s.modelindex = G_ModelIndex( "models/mapobjects/tanks_sd/churchhill_full.md3" );
		tank->sound3to2 = -1;
		VectorSet( offset, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] );
		G_SetOrigin( tank, offset );

		//VectorSet (tank->r.mins, -112, -112, -24);
		//VectorSet (tank->r.maxs, 112, 112, 78);
		VectorCopy( tankMins, tank->r.mins );
		VectorCopy( tankMaxs, tank->r.maxs );
		tank->s.apos.trTime = 0;
		tank->s.apos.trDuration = 0;
		tank->s.apos.trType = TR_LINEAR_STOP;
		VectorCopy( ent->s.angles, tank->s.angles );
		VectorCopy( tank->s.angles, tank->s.apos.trBase );
		VectorCopy( tank->s.angles, tank->s.apos.trDelta );
		VectorCopy( ent->s.angles, tank->s.angles2 );
		tank->think = tank_think;
		tank->use = tank_use;
		tank->die = tank_die;
		tank->nextthink = level.time + FRAMETIME;
		tank->timestamp = level.time + 1000;
		tank->s.number = tank - g_entities;
		tank->takedamage = qtrue;
		tank->spawnflags = ent->spawnflags;
		tank->TankMarker = ent->TankMarker;
		trap_LinkEntity( tank );
	}

	ent->think = tank_spawn_think;
	ent->nextthink = level.time + FRAMETIME;
}


/* */
void
panzer_spawn ( gentity_t *ent )
{
	gentity_t	*tank;
	vec3_t		offset;
	if ( g_knifeonly.integer != 1 )
	{

		// Spawn the barrel
		tank = G_Spawn();
		tank->classname = "npc_panzertank";
		tank->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
		tank->r.contents = CONTENTS_TRIGGER | CONTENTS_SOLID;
		tank->r.svFlags = 0;
		tank->s.eType = ET_VEHICLE;
		tank->health = 100;
		tank->s.dl_intensity = 100;
		tank->s.effect1Time = 1;
		tank->s.modelindex = G_ModelIndex( "models/mapobjects/tanks_sd/jagdpanther_full.md3" );
		tank->sound3to2 = -1;
		VectorSet( offset, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] );
		G_SetOrigin( tank, offset );

		//VectorSet (tank->r.mins, -112, -112, -24);
		//VectorSet (tank->r.maxs, 112, 112, 78);
		VectorCopy( tankMins, tank->r.mins );
		VectorCopy( tankMaxs, tank->r.maxs );
		tank->s.apos.trTime = 0;
		tank->s.apos.trDuration = 0;
		tank->s.apos.trType = TR_LINEAR_STOP;
		VectorCopy( ent->s.angles, tank->s.angles );
		VectorCopy( tank->s.angles, tank->s.apos.trBase );
		VectorCopy( tank->s.angles, tank->s.apos.trDelta );
		VectorCopy( ent->s.angles, tank->s.angles2 );
		tank->think = tank_think;
		tank->use = tank_use;
		tank->die = tank_die;
		tank->nextthink = level.time + FRAMETIME;
		tank->timestamp = level.time + 1000;
		tank->s.number = tank - g_entities;
		tank->takedamage = qtrue;
		tank->spawnflags = ent->spawnflags;
		tank->TankMarker = ent->TankMarker;
		trap_LinkEntity( tank );
	}

	ent->think = panzer_spawn_think;
	ent->nextthink = level.time + FRAMETIME;

	//	G_FreeEntity (ent);
}


/* */
void
tank_spawn_think ( gentity_t *self )
{
	if ( !self->TankMarker )
	{			// init
		//	G_Printf("Tank spawner %i activated .. \n", self->s.number );
		self->TankMarker = self->s.number;
		self->think = tank_spawn;
		self->nextthink = level.time + FRAMETIME;
	}
	else
	{
		int			i;
		qboolean	found = qfalse;
		for ( i = 0; i < level.num_entities; i++ )
		{
			if ( g_entities[i].TankMarker == self->TankMarker && i != self->TankMarker )
			{	// k , we found our tank, just sleep
				self->think = tank_spawn_think;
				self->nextthink = level.time + g_tankSpawnDelay.integer;	// @TODO cvar here , interval between tanks spawn
				found = qtrue;

				//			G_Printf("Tank number %i found .. \n", g_entities[i].TankMarker );
				break;
			}
			else
			{																// keep searching
				continue;
			}
		}	// for ...

		if ( !found )
		{	// time to spawn another tank
			//	G_Printf("Tank %i not found, spawning another .. \n", self->TankMarker );
			self->think = tank_spawn;
			self->nextthink = level.time + FRAMETIME;
		}
	}
}


/* */
void
panzer_spawn_think ( gentity_t *self )
{
	if ( !self->TankMarker )
	{			// init
		self->TankMarker = self->s.number;
		self->think = panzer_spawn;
		self->nextthink = level.time + FRAMETIME;
	}
	else
	{
		int			i;
		qboolean	found = qfalse;
		for ( i = 0; i < level.num_entities; i++ )
		{
			if ( g_entities[i].TankMarker == self->TankMarker && i != self->TankMarker )
			{	// k , we found our tank, just sleep
				self->think = panzer_spawn_think;
				self->nextthink = level.time + g_tankSpawnDelay.integer;	// @TODO cvar here , interval between tanks spawn
				found = qtrue;
				break;
			}
			else
			{																// keep searching
				continue;
			}
		}	// for ...

		if ( !found )
		{	// time to spawn another tank
			self->think = panzer_spawn;
			self->nextthink = level.time + FRAMETIME;
		}
	}
}


/* */
void
SP_tank ( gentity_t *self )
{
	self->think = tank_spawn_think;
	self->nextthink = level.time + FRAMETIME;
}


/* */
void
SP_panzer ( gentity_t *self )
{
	self->think = panzer_spawn_think;
	self->nextthink = level.time + FRAMETIME;
}
#endif //__NPC__
