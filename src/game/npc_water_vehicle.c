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
extern void		NPC_die ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath );
vec3_t			LCVPMins = { -200, -76, 0 };
vec3_t			LCVPMaxs = { 200, 76, 16 };
void			SetLCVPAngles ( gentity_t *NPC, float thinktime );
void			NPC_LCVP_Free ( gentity_t *ent );
void			SetDestroyerAngles ( gentity_t *NPC, float thinktime );
void			NPC_Destroyer_Free ( gentity_t *ent );
qboolean		NPC_LCVP_Check_Disembark_Passengers ( gentity_t *ent );

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

//

// LCVP - Lander Boats...

//

//

// Lander boat list structure...

//
typedef struct lcvps_s	// LCVP List Structure
{
	vec3_t		origin; // LCVP Start Origin
	int			wpIndex;
	int			FirstWpNumber;
	qboolean	inUse;
	int			entityNum;
} lcvps_t;
lcvps_t lcvps[100];
int		num_lcvps = 0;

/*
//
// Lander boat nodes structure...
//

typedef struct lcvp_node_s           // Node Structure
{
    vec3_t      origin;         // Node Origin
    qboolean	land_point;
	qboolean	reverse;
	int			wpIndex;
	int			wpNumber;
} lcvp_node_t;  

lcvp_node_t lcvp_nodes[2000];

//
// END - Lander boat nodes structure...
//
*/
int		num_lcvp_nodes = 0;

//
// END - Lander boat list structure...
//
int		lcvpWPRenderTime = 0;
int		lcvpWPRenderedFrame = 0;


/* */
float
SetLCVPAngle ( float angle, float ideal_angle, float speed )
{
	float	move;
	angle = AngleMod( angle );
	ideal_angle = AngleMod( ideal_angle );
	if ( angle == ideal_angle )
	{
		return ( angle );
	}

	move = ideal_angle - angle;
	if ( ideal_angle > angle )
	{
		if ( move > 180.0 )
		{
			move -= 360.0;
		}
	}
	else
	{
		if ( move < -180.0 )
		{
			move += 360.0;
		}
	}

	if ( move > 0 )
	{
		if ( move > speed )
		{
			move = speed;
		}
	}
	else
	{
		if ( move < -speed )
		{
			move = -speed;
		}
	}

	return ( AngleMod( angle + move) );
}


//===========================================================================
// Routine      : BotSetViewAngles

// Description  : Set bot view angles over time...
void
SetLCVPAngles ( gentity_t *NPC, float thinktime )
{	// Just keep bot->bot_ideal_view_angles correct and call this to make it realistic...
	float	diff, factor, maxchange, anglespeed;
	int		i;
	if ( NPC->npc_ideal_view_angles[PITCH] > 180 )
	{
		NPC->npc_ideal_view_angles[PITCH] -= 360;
	}

	//
	factor = 0.15;
	maxchange = 240;
	maxchange *= thinktime;
	for ( i = 0; i < 2; i++ )
	{
		diff = fabs( AngleDifference( NPC->npc_view_angles[i], NPC->npc_ideal_view_angles[i]) );
		anglespeed = diff * factor;
		if ( anglespeed > maxchange )
		{
			anglespeed = maxchange;
		}

		NPC->npc_view_angles[i] = SetLCVPAngle( NPC->npc_view_angles[i], NPC->npc_ideal_view_angles[i], anglespeed );
	}

	if ( NPC->npc_view_angles[PITCH] > 180 )
	{
		NPC->npc_view_angles[PITCH] -= 360;
	}

	G_SetAngle( NPC, NPC->npc_view_angles );
}


/* */
void
LanderWaypointRender ( void )
{
	int			i, inc_checker;
	gentity_t	*plum;
	if ( num_lcvp_nodes <= 0 )
	{
		return;
	}

	if ( lcvpWPRenderTime > level.time )
	{
		return;
	}

	lcvpWPRenderTime = level.time + 300;	// 100
	lcvpWPRenderedFrame = 0;
	i = 0;
	inc_checker = 0;
	while ( i < num_lcvp_nodes )
	{
		if ( lcvpWPRenderedFrame > 128 )
		{
			break;
		}
		{
			vec3_t	up;
			VectorCopy( lcvp_nodes[i].origin, up );
			up[2] += 64;
			plum = G_TempEntity( up, EV_SCOREPLUM );
			plum->r.svFlags |= SVF_BROADCAST;
			plum->s.time = i;
			plum->s.otherEntityNum = 0;
			plum->s.weapon = 1;

			// Make it appear larger!
			plum->s.density = 1;
			lcvpWPRenderedFrame++;
		}

		if ( lcvpWPRenderedFrame > 128 )
		{
			break;							//don't render too many at once
		}

		i++;
	}
}


/* */
void
NPC_LCVP_Setup_Waypoints ( gentity_t *ent )
{
	int			i = 0;

	// ent->health is our first waypoint, so...
	int			current_waypoint = ent->health;
	qboolean	found = qtrue;

	// Use pathsize to store the number of waypoints this lander has..
	ent->pathsize = 0;
	while ( found )
	{
		found = qfalse; // Set this to false and let it be re-set in the for loop...
		for ( i = 0; i < num_lcvp_nodes; i++ )
		{

			// Check this is for this lander...
			if ( lcvp_nodes[i].wpIndex != ent->partofstage )
			{
				continue;
			}

			// Check that this is the lander's next waypoint...
			if ( lcvp_nodes[i].wpNumber != current_waypoint )
			{
				continue;
			}

			VectorCopy( lcvp_nodes[i].origin, ent->lcvp_nodes[ent->pathsize].origin );
			ent->lcvp_nodes[ent->pathsize].land_point = lcvp_nodes[i].land_point;
			ent->lcvp_nodes[ent->pathsize].reverse = lcvp_nodes[i].reverse;
			ent->lcvp_nodes[ent->pathsize].wpIndex = lcvp_nodes[i].wpIndex;
			ent->lcvp_nodes[ent->pathsize].wpNumber = lcvp_nodes[i].wpNumber;
			ent->pathsize++;

			// We found another waypoint this round...
			found = qtrue;
			break;
		}

		if ( !found )
		{
			break;
		}

		// Incrument the waypoint number...
		current_waypoint++;
	}

	// Remove the extra one...
	ent->pathsize--;
}

extern float	*G_GetTagOrigin ( gentity_t *parent, char *tagname, vec3_t *offset );
extern qboolean G_TagValid ( gentity_t *parent, char *tagname );


/* */
void
G_SetEntitySpawnpoint ( gentity_t *entity, gentity_t *LCVP, int spawnPointNum )
{
	gentity_t	*parent = G_Spawn();
	vec3_t		origin;
	char		tagname[MAX_QPATH];
	COM_StripExtension( LCVP->model, tagname );
	Q_strcat( tagname, MAX_QPATH, ".tag" );
	LCVP->tagNumber = trap_LoadTag( tagname );

	// Add the spawn platform...
	parent->model = "models/pop/vehicles/pop_lcvp_spawns.md3";
	parent->s.modelindex = G_ModelIndex( parent->model );
	Q_strncpyz( parent->tagName, "tag_origin", sizeof(parent->tagName) );
	parent->tagParent = LCVP;
	COM_StripExtension( parent->model, tagname );
	Q_strcat( tagname, MAX_QPATH, ".tag" );
	parent->tagNumber = trap_LoadTag( tagname );
	if ( !G_PositionEntityOnTag( parent, LCVP, "tag_origin") )
	{
		G_Printf( "G_PositionEntityOnTag: Failed to position entity on tag!\n" );
	}

	parent->r.currentOrigin[2] += 18;
	parent->s.origin[2] += 18;
	VectorCopy( LCVP->r.currentAngles, parent->r.currentAngles );
	trap_LinkEntity( parent );
	if ( !G_TagValid( parent, va( "tag_spawnpoint%i", spawnPointNum)) )
	{
		G_Printf( "TAG ERROR: %s does not exist on model %s (%i).\n", va( "tag_spawnpoint%i", spawnPointNum),
				  parent->model, parent->s.modelindex );
	}

	VectorCopy( G_GetTagOrigin( parent, va( "tag_spawnpoint%i", spawnPointNum), NULL), origin );
	G_SetOrigin( entity, origin );
	if ( entity->client )
	{
		VectorCopy( origin, entity->client->ps.origin );
	}
	else if ( entity->NPC_client )
	{
		VectorCopy( origin, entity->NPC_client->ps.origin );
	}

	G_SetAngle( entity, LCVP->r.currentAngles );
	if ( entity->client )
	{
		VectorCopy( LCVP->r.currentAngles, entity->client->ps.viewangles );
	}
	else if ( entity->NPC_client )
	{
		VectorCopy( LCVP->r.currentAngles, entity->NPC_client->ps.viewangles );
	}

	// Remove the spawn platform...
	trap_UnlinkEntity( parent );
	G_FreeEntity( parent );
}


/* */
void
NPC_LCVP_Disembark_Passengers ( gentity_t *ent )
{	// Release all players/bots/NPCs to a spawnpoint on the boat when it lands...
	int j;

	/*
	// Free the contents...
	for (j = 0; j < 10; j++)
	{
		gentity_t *filler = &g_entities[ent->npc_spawnpoints[j]];
		vec3_t back;

		if (!filler || ent->npc_spawnpoints[j] < 0)
			continue;

		filler->s.dl_intensity = TAG_UNHOOK;

		if (filler->client)
		{
			filler->client->ps.eFlags &= ~EF_TAGCONNECT;
			filler->client->ps.stats[STAT_TAG_CONNECT_INFO] = TAG_UNHOOK;
		}

		filler->s.otherEntityNum = -1;
		filler->s.otherEntityNum2 = -1;
		G_SetEntitySpawnpoint( filler, ent, j );

		// Set a forwards direction for non-routed maps if needed... Get them off the damn boat!
		AngleVectors( filler->r.currentAngles, back, NULL, NULL );
		back[0]=0-back[0];
		back[1]=0-back[1];
		back[2]=0-back[2];
		VectorAdd(filler->r.currentOrigin, back, filler->oldOrigin);

		ent->npc_spawnpoints[j] = -1;
	}
*/
}


/* */
qboolean
NPC_LCVP_Check_Wait ( gentity_t *ent )
{	// See if any players are still close to the boat...
	vec3_t	mins, maxs;
	int		e = 0;
	int		numListedEntities = 0;
	int		entityList[MAX_GENTITIES];
	VectorCopy( ent->r.currentOrigin, mins );
	VectorAdd( LCVPMins, mins, mins );
	mins[0] -= 256;
	mins[1] -= 256;
	mins[2] -= 256;
	VectorCopy( ent->r.currentOrigin, maxs );
	VectorAdd( LCVPMaxs, maxs, maxs );
	maxs[0] += 256;
	maxs[1] += 256;
	maxs[2] += 256;
	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
	for ( e = 0; e < numListedEntities; e++ )
	{
		gentity_t	*tempent = &g_entities[entityList[e]];
		if ( !tempent )
		{
			continue;
		}

		if ( !tempent->client && !tempent->NPC_client )
		{
			continue;
		}

		// Found one...
		return ( qtrue );
	}

	return ( qfalse );
}


/* */
qboolean
NPC_LCVP_Check_Disembark_Passengers ( gentity_t *ent )
{
	int i = 0;
	if ( ent->beStillEng )
	{
		return ( qfalse );	// Already let all passengers off the boat...
	}

	if ( !ent->lcvp_nodes[ent->current_node].land_point )
	{
		return ( qfalse );
	}

	if ( VectorDistance( ent->lcvp_nodes[ent->current_node].origin, ent->r.currentOrigin) <= 64 )
	{						// We hit our landing point... Release the passengers and wait until they leave the boat...
		if ( !ent->beStill )
		{					// Release passengers...
			NPC_LCVP_Disembark_Passengers( ent );
			ent->beStill = qtrue;
		}

		if ( NPC_LCVP_Check_Wait( ent) )
		{					// Should we wait?
			return ( qtrue );
		}

		ent->beStillEng = qtrue;
		return ( qfalse );
	}

	return ( qfalse );
}


/* */
void
NPC_LCVP_Free ( gentity_t *ent )
{
	int			i = 0;
	int			j = 0;
	qboolean	found = qfalse;

	/*
	// Find the LCVP in our array...
	for (i = 0; i < num_lcvps; i++)
	{
		if (!lcvps[i].inUse)
			continue;

		if (lcvps[i].wpIndex != ent->lcvp_nodes[0].wpIndex)
			continue;

		lcvps[i].inUse = qfalse;
		lcvps[i].entityNum = -1;
		found = qtrue;

		// Free the contents...
		for (j = 0; j < 10; j++)
		{
			gentity_t *filler = &g_entities[ent->npc_spawnpoints[j]];

			if (!filler || ent->npc_spawnpoints[j] < 0)
				continue;

			filler->s.dl_intensity = TAG_UNHOOK;
			filler->s.otherEntityNum = -1;
			filler->s.otherEntityNum2 = -1;

			if (filler->client)
			{
				filler->client->ps.eFlags &= ~EF_TAGCONNECT;
				filler->client->ps.stats[STAT_TAG_CONNECT_INFO] = TAG_UNHOOK;
			}

			ent->npc_spawnpoints[j] = -1;
		}

		// Remove driver...
		g_entities[ent->npc_gunner2].s.dl_intensity = TAG_UNHOOK;
		g_entities[ent->npc_gunner2].s.otherEntityNum = -1;
		g_entities[ent->npc_gunner2].s.otherEntityNum2 = -1;

		if (g_entities[ent->npc_gunner2].client)
		{
			g_entities[ent->npc_gunner2].client->ps.eFlags &= ~EF_TAGCONNECT;
		}

		// Remove gunner1...
		g_entities[ent->npc_driver].s.dl_intensity = TAG_UNHOOK;
		g_entities[ent->npc_driver].s.otherEntityNum = -1;
		g_entities[ent->npc_driver].s.otherEntityNum2 = -1;

		if (g_entities[ent->npc_driver].client)
			g_entities[ent->npc_driver].client->ps.eFlags &= ~EF_TAGCONNECT;

		// Remove gunner2...
		g_entities[ent->npc_gunner1].s.dl_intensity = TAG_UNHOOK;
		g_entities[ent->npc_gunner1].s.otherEntityNum = -1;
		g_entities[ent->npc_gunner1].s.otherEntityNum2 = -1;

		if (g_entities[ent->npc_gunner1].client)
			g_entities[ent->npc_gunner1].client->ps.eFlags &= ~EF_TAGCONNECT;

		G_FreeEntity(ent);
		ent->s.eType = 0;
		break;
	}
*/
}


/* */
void
NPC_LCVP_Setup ( gentity_t *ent )
{
	int			i = 0;
	qboolean	found = qfalse;

	// Init passenger release for this entity...
	ent->beStill = qfalse;

	// Init the landed flag...
	ent->beStillEng = qfalse;

	// Find a free LCVP...
	for ( i = 0; i < num_lcvps; i++ )
	{
		if ( lcvps[i].inUse )
		{
			continue;
		}

		G_SetOrigin( ent, lcvps[i].origin );
		VectorCopy( lcvps[i].origin, ent->s.origin );
		VectorCopy( lcvps[i].origin, ent->r.currentOrigin );
		ent->health = lcvps[i].FirstWpNumber;
		ent->partofstage = lcvps[i].wpIndex;
		lcvps[i].inUse = qtrue;
		lcvps[i].entityNum = ent->s.number;
		found = qtrue;
		break;
	}

	if ( !found )
	{
		return;
	}

	NPC_LCVP_Setup_Waypoints( ent );
	ent->current_node = 0;
	ent->next_node = 1;
}


// To fill LCVP's with NPC humanoids... Return's the new NPC's entitynum...
int
LCVP_NPC_Filler_Spawn ( gentity_t *CallEnt )
{
	vec3_t		fwd;
	gentity_t	*NPC = G_Spawn();
	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	VectorCopy( CallEnt->r.currentOrigin, NPC->s.origin );
	AngleVectors( CallEnt->r.currentAngles, fwd, NULL, NULL );
	NPC->s.origin[0] += ( fwd[0] * 64 );
	NPC->s.origin[1] += ( fwd[1] * 64 );
	NPC->s.origin[2] += ( fwd[2] * 64 );
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;		// For cgame access...
	NPC->s.teamNum = TEAM_ALLIES;

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->playerType = Q_TrueRand( PC_SOLDIER, NUM_PLAYER_CLASSES - 1 );
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );
	{											// Set up an NPC client structure...
		gclient_t	*client;
		gentity_t	*ent = NPC;
		NPC->NPC_client = G_Alloc( sizeof(*client) );
		client = NPC->NPC_client;
		memset( client, 0, sizeof(*client) );
		memset( &client->ps, 0, sizeof(client->ps) );
		memset( &client->ps.weapons, 0, sizeof(client->ps.weapons) );
		memset( client->ps.ammo, 0, sizeof(client->ps.ammo) );
		memset( client->ps.ammoclip, 0, sizeof(client->ps.ammoclip) );
		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;	// DHM - Nerve
		client->pers.character = NPC->character;
		client->pers.characterIndex = NPC->characterIndex;
		client->sess.playerType = NPC->playerType;
		client->pers.character->animModelInfo = BG_GetCharacterForPlayerstate( &client->ps )->animModelInfo;
		client->ps.legsAnim = 0;
		client->ps.torsoAnim = 0;
		client->ps.legsTimer = 0;
		client->ps.torsoTimer = 0;
		VectorCopy( NPC->s.origin, NPC->NPC_client->ps.origin );

		// Now init all the values we will need!!!
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam;
		client->ps.persistant[PERS_HWEAPON_USE] = 0;

		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
		client->ps.eFlags = 0;
		ent->s.groundEntityNum = ENTITYNUM_NONE;
		ent->inuse = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = MASK_PLAYERSOLID;
		ent->props_frame_state = -1;
		ent->waterlevel = 0;
		ent->watertype = 0;
		ent->flags = 0;
		VectorCopy( playerMins, ent->r.mins );
		VectorCopy( playerMaxs, ent->r.maxs );

		// Ridah, setup the bounding boxes and viewheights for prediction
		VectorCopy( ent->r.mins, client->ps.mins );
		VectorCopy( ent->r.maxs, client->ps.maxs );
		client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
		client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
		client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
		client->ps.crouchMaxZ = client->ps.maxs[2] - ( client->ps.standViewHeight - client->ps.crouchViewHeight );
		client->ps.runSpeedScale = 0.8;
		client->ps.sprintSpeedScale = 1.1;
		client->ps.crouchSpeedScale = 0.25;
		client->ps.weaponstate = WEAPON_READY;

		// Rafael
		client->pmext.sprintTime = SPRINTTIME;
		client->ps.sprintExertTime = 0;
		client->ps.friction = 1.0;
		client->pers.cmd.weapon = ent->s.weapon;
		client->latched_buttons = 0;
		client->latched_wbuttons = 0;			//----(SA)	added

		// xkan, 1/13/2003 - reset death time
		client->deathTime = 0;

		// run a client frame to drop exactly to the floor,
		// initialize animations and other things
		client->ps.commandTime = level.time - 100;
		client->pers.cmd.serverTime = level.time;
		NPCThink( ent - g_entities );

		// positively link the client, even if the command times are weird
		BG_NPCStateToEntityState( &client->ps, &ent->s, qtrue );
		VectorCopy( ent->NPC_client->ps.origin, ent->r.currentOrigin );
		trap_LinkEntity( ent );

		// clear entity state values
		BG_NPCStateToEntityState( &client->ps, &ent->s, qtrue );
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;
		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	return ( NPC->s.number );
}

extern void moveit ( gentity_t *ent, float yaw, float dist );


/* */
void
NPC_LCVP_Touch ( gentity_t *self, gentity_t *other, trace_t *trace )
{
	float	ratio;
	vec3_t	v;
	if ( other->r.currentOrigin[2] > (self->r.currentOrigin[2] + 10 + 31) )
	{
		return;
	}

	ratio = 1.5;
	VectorSubtract( self->r.currentOrigin, other->r.currentOrigin, v );
	moveit( self, vectoyaw( v), (20 * ratio * FRAMETIME) * .001 );
}


/* */
void
NPC_LCVP_DoSpawn ( void )
{
	gentity_t	*NPC = G_Spawn();
	vec3_t		spawn_angles;
	int			posNum = 0;

	// Setup the NPC origin and waypoints...
	NPC_LCVP_Setup( NPC );

	// Set NPC angle...
	VectorSubtract( NPC->s.origin, NPC->lcvp_nodes[NPC->next_node].origin, NPC->move_vector );
	vectoangles( NPC->move_vector, spawn_angles );
	G_SetAngle( NPC, spawn_angles );
	VectorCopy( spawn_angles, NPC->npc_ideal_view_angles );
	NPC->model = "models/pop/vehicles/pop_lcvp_hull.md3";
	NPC->s.modelindex = G_ModelIndex( NPC->model );
	NPC->NPC_Class = CLASS_LCVP;
	NPC->s.modelindex2 = NPC->NPC_Class;	// For cgame access...

	// Solids...
	NPC->clipmask = CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID;
	NPC->touch = NPC_LCVP_Touch;
	VectorCopy( LCVPMins, NPC->r.mins );
	VectorCopy( LCVPMaxs, NPC->r.maxs );

	// We use this value to free the entity after it has gone off the map a bit... Hopefully no longer visible...
	NPC->freetime = level.time + 60000;
	if ( NPC->pathsize <= 0 )
	{
		NPC_LCVP_Free( NPC );
		return;
	}

	VectorCopy( NPC->s.origin, NPC->oldOrigin );

	// Setup the basic NPC stuff...
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_LCVP;
	NPC->s.modelindex2 = NPC->NPC_Class;	// For cgame access...

	// Set up driver...
	{										// Fill with humanoid NPCs...
		gentity_t	*filler = NULL;
		NPC->npc_driver = LCVP_NPC_Filler_Spawn( NPC );
		filler = &g_entities[NPC->npc_driver];
		filler->s.dl_intensity = TAG_HOOK_DRIVER;
		filler->s.otherEntityNum = NPC->s.number;
		if ( filler->client )
		{
			filler->client->ps.eFlags |= EF_TAGCONNECT;
		}
	}

	// Set up port gunner...
	{										// Fill with humanoid NPCs...
		gentity_t	*filler = NULL;
		NPC->npc_driver = LCVP_NPC_Filler_Spawn( NPC );
		filler = &g_entities[NPC->npc_driver];
		filler->s.dl_intensity = TAG_HOOK_GUNNER1;
		filler->s.otherEntityNum = NPC->s.number;
		if ( filler->client )
		{
			filler->client->ps.eFlags |= EF_TAGCONNECT;
		}
	}

	// Set up starboard gunner...
	{										// Fill with humanoid NPCs...
		gentity_t	*filler = NULL;
		NPC->npc_driver = LCVP_NPC_Filler_Spawn( NPC );
		filler = &g_entities[NPC->npc_driver];
		filler->s.dl_intensity = TAG_HOOK_GUNNER2;
		filler->s.otherEntityNum = NPC->s.number;
		if ( filler->client )
		{
			filler->client->ps.eFlags |= EF_TAGCONNECT;
		}
	}

	// Set up the spawnpoints...
	{
		int i;
		for ( i = 0; i < 10; i++ )
		{									// Fill with humanoid NPCs...
			gentity_t	*filler = NULL;
			NPC->npc_spawnpoints[i] = LCVP_NPC_Filler_Spawn( NPC );
			filler = &g_entities[NPC->npc_spawnpoints[i]];
			filler->s.dl_intensity = TAG_HOOK;
			filler->s.otherEntityNum = NPC->s.number;
			filler->s.otherEntityNum2 = i;
			if ( filler->client )
			{
				filler->client->ps.eFlags |= EF_TAGCONNECT;
			}
		}
	}

	NPC->spawnTime = level.time;
}

int next_LCVP_time = 0;


/* */
void
Init_LCVP_Timer ( void )
{
	next_LCVP_time = 5000;
}


/* */
void
NPC_LCVP_Check ( void )
{	// Call this each frame to add LCVPs to a map...
	if ( num_lcvps <= 0 )
	{
		return;
	}

	if ( level.restarted )
	{
		return;
	}

	if ( level.intermissiontime )
	{
		return;
	}

	if ( next_LCVP_time > level.time )
	{
		return;
	}

	// Ready to spawn the next landing craft...
	NPC_LCVP_DoSpawn();

	//next_LCVP_time = (level.dwBlueReinfOffset + level.timeCurrent - level.startTime) % g_bluelimbotime.integer;
	next_LCVP_time = level.time + Q_TrueRand( 10000, 30000 );
}


/* */
qboolean
CheckLCVPsOnThisMap ( void )
{	// Does this map use LCVPs? FIXME: Check chosen spawnpoint when implemented...
	if ( num_lcvps > 0 )
	{
		return ( qtrue );
	}

	return ( qfalse );
}


/* */
int
GetCurrentLCVPEntityNum ( void )
{
	int			i = 0;
	qboolean	found = qfalse;
	int			entityNum = -1;

	// Find an inuse LCVP...
	for ( i = 0; i < num_lcvps; i++ )
	{
		int			j = 0;
		gentity_t	*NPC = NULL;
		qboolean	good = qfalse;
		if ( !lcvps[i].inUse )
		{
			continue;
		}

		NPC = &g_entities[lcvps[i].entityNum];
		for ( j = 0; j < 10; j++ )
		{		// Fill with humanoid NPCs...
			if ( NPC->npc_spawnpoints[j] < MAX_CLIENTS )
			{

			}
			else if ( NPC->beStill )
			{	// Already disembarked...
			}
			else if ( g_entities[NPC->npc_spawnpoints[j]].s.eType == ET_NPC )
			{	// We can replace this NPC...
				//if (NPC->spawnTime < level.time - 100)
				{
					good = qtrue;
					break;
				}
			}
		}

		if ( !good )
		{
			continue;
		}

		found = qtrue;
		entityNum = lcvps[i].entityNum;
		break;
	}

	return ( entityNum );
}


/* */
int
GetCurrentLCVPSpawnpointNum ( int entityNum )
{
	int			i = 0;
	qboolean	found = qfalse;
	int			spawnpoint = -1;
	gentity_t	*NPC = &g_entities[entityNum];

	// Find an inuse LCVP...
	for ( i = 0; i < 10; i++ )
	{
		if ( NPC->npc_spawnpoints[i] < MAX_CLIENTS )
		{

		}
		else if ( g_entities[NPC->npc_spawnpoints[i]].s.eType == ET_NPC )
		{
			spawnpoint = i;
			break;
		}
	}

	return ( spawnpoint );
}


/*QUAKED npc_lander_enter (1 0 0) (-16 -16 -16) (16 16 16)
-------- KEYS --------
"partofstage"	REQUIRED: The waypoint index this is associated with.
"health"	REQUIRED: First waypoint number to head for. Waypoints are followed in sequential order!
-------- SPAWNFLAGS --------
-------- NOTES --------
Add a lander NPC starting point to the map. 
Angles will be created from the lander's first waypoint.

partofstage is required!
*/
void
NPC_LCVP_Spawn ( gentity_t *ent )
{	//classname: "npc_lander_enter" - Simply adds info to the lcvp array...
	G_SpawnInt( "partofstage", "0", &ent->partofstage );
	G_SpawnInt( "health", "0", &ent->health );
	VectorCopy( ent->s.origin, lcvps[num_lcvps].origin );
	lcvps[num_lcvps].FirstWpNumber = ent->health;
	lcvps[num_lcvps].wpIndex = ent->partofstage;
	lcvps[num_lcvps].inUse = qfalse;
	if ( !ent->partofstage )
	{
		G_Printf( "^1*** ^3ERROR^5: \"npc_lander_enter\" with no partofstage!\n" );
		return;
	}

	// Incrument the number of lcvps...
	num_lcvps++;

	// Don't need this entity any more... Free it for other things...
	G_FreeEntity( ent );
}


/*QUAKED npc_lander_waypoint (1 0 0) (-16 -16 -16) (16 16 16)
-------- KEYS --------
"partofstage"	REQUIRED: The waypoint index this is associated with.
"mass" defaults to 0.  Move Forward (0) or Back (1) to this waypoint.
"dmg" defaults to 0.  Rotate the lander Straight (0) Left (1) or Right (2) at this waypoint.
"health"	REQUIRED: This waypoint's number. Waypoints are followed in sequential order!
-------- SPAWNFLAGS --------
-------- NOTES --------
Lander waypoint.

partofstage is required!
*/
void
NPC_LCVP_Waypoint ( gentity_t *ent )
{	// Store in a temporary array...
	int mass;
	VectorCopy( ent->s.origin, lcvp_nodes[num_lcvp_nodes].origin );
	G_SpawnInt( "partofstage", "0", &ent->partofstage );
	G_SpawnInt( "dmg", "0", &ent->damage );
	G_SpawnInt( "health", "0", &ent->health );
	G_SpawnInt( "mass", "0", &mass );
	lcvp_nodes[num_lcvp_nodes].land_point = qfalse;
	lcvp_nodes[num_lcvp_nodes].reverse = mass;
	lcvp_nodes[num_lcvp_nodes].wpIndex = ent->partofstage;
	lcvp_nodes[num_lcvp_nodes].wpNumber = ent->health;
	if ( !ent->partofstage )
	{
		G_Printf( "^1*** ^3ERROR^5: \"npc_lander_waypoint\" with no partofstage!\n" );
		return;
	}

	// Incrument the counter...
	num_lcvp_nodes++;

	// Don't need this entity any more... Free it for other things...
	G_FreeEntity( ent );
}


/*QUAKED npc_lander_waypoint_target (1 0 0) (-16 -16 -16) (16 16 16)
-------- KEYS --------
"partofstage"	REQUIRED: The waypoint index this is associated with.
"mass" defaults to 0.  Move Forward (0) or Back (1) to this waypoint.
"dmg" defaults to 0.  Rotate the lander Straight (0) Left (1) or Right (2) at this waypoint.
"health"	REQUIRED: This waypoint's number. Waypoints are followed in sequential order!
-------- SPAWNFLAGS --------
-------- NOTES --------
Lander target waypoint. (Where it lets troops off).

partofstage is required!
*/
void
NPC_LCVP_Waypoint_Target ( gentity_t *ent )
{	// Store in a temporary array...
	int mass;
	VectorCopy( ent->s.origin, lcvp_nodes[num_lcvp_nodes].origin );
	G_SpawnInt( "partofstage", "0", &ent->partofstage );
	G_SpawnInt( "dmg", "0", &ent->damage );
	G_SpawnInt( "health", "0", &ent->health );
	G_SpawnInt( "mass", "0", &mass );
	lcvp_nodes[num_lcvp_nodes].land_point = qtrue;
	lcvp_nodes[num_lcvp_nodes].reverse = mass;
	lcvp_nodes[num_lcvp_nodes].wpIndex = ent->partofstage;
	lcvp_nodes[num_lcvp_nodes].wpNumber = ent->health;
	if ( !ent->partofstage )
	{
		G_Printf( "^1*** ^3ERROR^5: \"npc_lander_waypoint_target\" with no partofstage!\n" );
		return;
	}

	// Incrument the counter...
	num_lcvp_nodes++;

	// Don't need this entity any more... Free it for other things...
	G_FreeEntity( ent );
}

//

// END - LCVP - Lander Boats...

//

//

// Destroyer Boats...

//

//

// Destroyer boat list structure...

//
typedef struct Destroyer_s	// LCVP List Structure
{
	vec3_t		origin;		// LCVP Start Origin
	int			wpIndex;
	int			FirstWpNumber;
	qboolean	inUse;
} Destroyer_t;
Destroyer_t Destroyers[100];
int			num_Destroyers = 0;
int			num_Destroyer_nodes = 0;

//
// END - Destroyer boat list structure...
//
int			DestroyerWPRenderTime = 0;
int			DestroyerWPRenderedFrame = 0;


/* */
float
SetDestroyerAngle ( float angle, float ideal_angle, float speed )
{
	float	move;
	angle = AngleMod( angle );
	ideal_angle = AngleMod( ideal_angle );
	if ( angle == ideal_angle )
	{
		return ( angle );
	}

	move = ideal_angle - angle;
	if ( ideal_angle > angle )
	{
		if ( move > 180.0 )
		{
			move -= 360.0;
		}
	}
	else
	{
		if ( move < -180.0 )
		{
			move += 360.0;
		}
	}

	if ( move > 0 )
	{
		if ( move > speed )
		{
			move = speed;
		}
	}
	else
	{
		if ( move < -speed )
		{
			move = -speed;
		}
	}

	return ( AngleMod( angle + move) );
}


//===========================================================================
// Routine      : BotSetViewAngles

// Description  : Set bot view angles over time...
void
SetDestroyerAngles ( gentity_t *NPC, float thinktime )
{	// Just keep bot->bot_ideal_view_angles correct and call this to make it realistic...
	float	diff, factor, maxchange, anglespeed;
	int		i;
	if ( NPC->npc_ideal_view_angles[PITCH] > 180 )
	{
		NPC->npc_ideal_view_angles[PITCH] -= 360;
	}

	//
	factor = 0.15;
	maxchange = 240;
	maxchange *= thinktime;
	for ( i = 0; i < 2; i++ )
	{
		diff = fabs( AngleDifference( NPC->npc_view_angles[i], NPC->npc_ideal_view_angles[i]) );
		anglespeed = diff * factor;
		if ( anglespeed > maxchange )
		{
			anglespeed = maxchange;
		}

		NPC->npc_view_angles[i] = SetDestroyerAngle( NPC->npc_view_angles[i], NPC->npc_ideal_view_angles[i], anglespeed );
	}

	if ( NPC->npc_view_angles[PITCH] > 180 )
	{
		NPC->npc_view_angles[PITCH] -= 360;
	}

	G_SetAngle( NPC, NPC->npc_view_angles );
}


/* */
void
DestroyerWaypointRender ( void )
{
	int			i, inc_checker;
	gentity_t	*plum;
	if ( num_Destroyer_nodes <= 0 )
	{
		return;
	}

	if ( DestroyerWPRenderTime > level.time )
	{
		return;
	}

	DestroyerWPRenderTime = level.time + 300;	// 100
	DestroyerWPRenderedFrame = 0;
	i = 0;
	inc_checker = 0;
	while ( i < num_Destroyer_nodes )
	{
		if ( DestroyerWPRenderedFrame > 128 )
		{
			break;
		}
		{
			vec3_t	up;
			VectorCopy( Destroyer_nodes[i].origin, up );
			up[2] += 64;
			plum = G_TempEntity( up, EV_SCOREPLUM );
			plum->r.svFlags |= SVF_BROADCAST;
			plum->s.time = i;
			plum->s.otherEntityNum = 0;
			plum->s.weapon = 1;

			// Make it appear larger!
			plum->s.density = 1;
			DestroyerWPRenderedFrame++;
		}

		if ( DestroyerWPRenderedFrame > 128 )
		{
			break;								//don't render too many at once
		}

		i++;
	}
}


/* */
void
NPC_Destroyer_Setup_Waypoints ( gentity_t *ent )
{
	int			i = 0;

	// ent->health is our first waypoint, so...
	int			current_waypoint = ent->health;
	qboolean	found = qtrue;

	// Use pathsize to store the number of waypoints this lander has..
	ent->pathsize = 0;
	while ( found )
	{
		found = qfalse; // Set this to false and let it be re-set in the for loop...
		for ( i = 0; i < num_Destroyer_nodes; i++ )
		{

			// Check this is for this lander...
			if ( Destroyer_nodes[i].wpIndex != ent->partofstage )
			{
				continue;
			}

			// Check that this is the lander's next waypoint...
			if ( Destroyer_nodes[i].wpNumber != current_waypoint )
			{
				continue;
			}

			VectorCopy( Destroyer_nodes[i].origin, ent->Destroyer_nodes[ent->pathsize].origin );
			ent->Destroyer_nodes[ent->pathsize].land_point = Destroyer_nodes[i].land_point;
			ent->Destroyer_nodes[ent->pathsize].reverse = Destroyer_nodes[i].reverse;
			ent->Destroyer_nodes[ent->pathsize].wpIndex = Destroyer_nodes[i].wpIndex;
			ent->Destroyer_nodes[ent->pathsize].wpNumber = Destroyer_nodes[i].wpNumber;
			ent->pathsize++;

			// We found another waypoint this round...
			found = qtrue;
			break;
		}

		if ( !found )
		{
			break;
		}

		// Incrument the waypoint number...
		current_waypoint++;
	}

	// Remove the extra one...
	ent->pathsize--;
}


/* */
void
NPC_Destroyer_Free ( gentity_t *ent )
{
	int			i = 0;
	qboolean	found = qfalse;

	// Find the Destroyer in our array...
	for ( i = 0; i < num_Destroyers; i++ )
	{
		if ( !Destroyers[i].inUse )
		{
			continue;
		}

		if ( Destroyers[i].wpIndex != ent->Destroyer_nodes[0].wpIndex )
		{
			continue;
		}

		Destroyers[i].inUse = qfalse;
		found = qtrue;
		break;
	}
}


/* */
void
NPC_Destroyer_Setup ( gentity_t *ent )
{
	int			i = 0;
	qboolean	found = qfalse;

	// Find a free Destroyer...
	for ( i = 0; i < num_Destroyers; i++ )
	{
		if ( Destroyers[i].inUse )
		{
			continue;
		}

		G_SetOrigin( ent, Destroyers[i].origin );
		VectorCopy( Destroyers[i].origin, ent->s.origin );
		VectorCopy( Destroyers[i].origin, ent->r.currentOrigin );
		ent->health = Destroyers[i].FirstWpNumber;
		ent->partofstage = Destroyers[i].wpIndex;
		Destroyers[i].inUse = qtrue;
		found = qtrue;
		break;
	}

	if ( !found )
	{
		return;
	}

	NPC_Destroyer_Setup_Waypoints( ent );
	ent->current_node = 0;
	ent->next_node = 1;
}


/* */
void
NPC_Destroyer_DoSpawn ( void )
{
	gentity_t	*NPC = G_Spawn();
	vec3_t		spawn_angles;
	int			posNum = 0;

	// Setup the NPC origin and waypoints...
	NPC_Destroyer_Setup( NPC );

	// Set NPC angle...
	VectorSubtract( NPC->s.origin, NPC->Destroyer_nodes[NPC->next_node].origin, NPC->move_vector );
	vectoangles( NPC->move_vector, spawn_angles );
	G_SetAngle( NPC, spawn_angles );
	VectorCopy( spawn_angles, NPC->npc_ideal_view_angles );
	NPC->model = "models/Saberpeak_md3/SP_ship.md3";
	NPC->s.modelindex = G_ModelIndex( NPC->model );
	NPC->NPC_Class = CLASS_DESTROYER;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	VectorCopy( LCVPMins, NPC->r.mins );
	VectorCopy( LCVPMaxs, NPC->r.maxs );

	// We use this value to free the entity after it has gone off the map a bit... Hopefully no longer visible...
	NPC->freetime = level.time + 60000;
	if ( NPC->pathsize <= 0 )
	{
		NPC_Destroyer_Free( NPC );
		G_FreeEntity( NPC );
	}

	VectorCopy( NPC->s.origin, NPC->oldOrigin );

	// Setup the basic NPC stuff...
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_DESTROYER;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...
}

int next_Destroyer_time = 9000;


/* */
void
NPC_Destroyer_Check ( void )
{	// Call this each frame to add Destroyers to a map...
	if ( num_Destroyers <= 0 )
	{
		return;
	}

	if ( level.time <= 9000 )
	{
		return;
	}

	if ( level.restarted )
	{
		return;
	}

	if ( level.intermissiontime )
	{
		return;
	}

	if ( next_Destroyer_time > level.time )
	{
		return;
	}

	// Ready to spawn the next landing craft...
	NPC_Destroyer_DoSpawn();
	next_Destroyer_time = ( level.dwBlueReinfOffset + level.timeCurrent - level.startTime ) % g_bluelimbotime.integer;
}


/*QUAKED npc_destroyer_enter (1 0 0) (-16 -16 -16) (16 16 16)
-------- KEYS --------
"partofstage"	REQUIRED: The waypoint index this is associated with.
"health"	REQUIRED: First waypoint number to head for. Waypoints are followed in sequential order!
-------- SPAWNFLAGS --------
-------- NOTES --------
Add a lander NPC starting point to the map. 
Angles will be created from the lander's first waypoint.

partofstage is required!
*/
void
NPC_Destroyer_Spawn ( gentity_t *ent )
{	//classname: "npc_destroyer_enter" - Simply adds info to the Destroyer array...
	G_SpawnInt( "partofstage", "0", &ent->partofstage );
	G_SpawnInt( "health", "0", &ent->health );
	VectorCopy( ent->s.origin, Destroyers[num_Destroyers].origin );
	Destroyers[num_Destroyers].FirstWpNumber = ent->health;
	Destroyers[num_Destroyers].wpIndex = ent->partofstage;
	Destroyers[num_Destroyers].inUse = qfalse;
	if ( !ent->partofstage )
	{
		G_Printf( "^1*** ^3ERROR^5: \"npc_destroyer_enter\" with no partofstage!\n" );
		return;
	}

	// Incrument the number of Destroyers...
	num_Destroyers++;

	// Don't need this entity any more... Free it for other things...
	G_FreeEntity( ent );
}


/*QUAKED npc_destroyer_waypoint (1 0 0) (-16 -16 -16) (16 16 16)
-------- KEYS --------
"partofstage"	REQUIRED: The waypoint index this is associated with.
"mass" defaults to 0.  Move Forward (0) or Back (1) to this waypoint.
"dmg" defaults to 0.  Rotate the lander Straight (0) Left (1) or Right (2) at this waypoint.
"health"	REQUIRED: This waypoint's number. Waypoints are followed in sequential order!
-------- SPAWNFLAGS --------
-------- NOTES --------
Lander waypoint.

partofstage is required!
*/
void
NPC_Destroyer_Waypoint ( gentity_t *ent )
{	// Store in a temporary array...
	int mass;
	VectorCopy( ent->s.origin, Destroyer_nodes[num_Destroyer_nodes].origin );
	G_SpawnInt( "partofstage", "0", &ent->partofstage );
	G_SpawnInt( "dmg", "0", &ent->damage );
	G_SpawnInt( "health", "0", &ent->health );
	G_SpawnInt( "mass", "0", &mass );
	Destroyer_nodes[num_Destroyer_nodes].land_point = qfalse;
	Destroyer_nodes[num_Destroyer_nodes].reverse = mass;
	Destroyer_nodes[num_Destroyer_nodes].wpIndex = ent->partofstage;
	Destroyer_nodes[num_Destroyer_nodes].wpNumber = ent->health;
	if ( !ent->partofstage )
	{
		G_Printf( "^1*** ^3ERROR^5: \"npc_Destroyer_waypoint\" with no partofstage!\n" );
		return;
	}

	// Incrument the counter...
	num_Destroyer_nodes++;

	// Don't need this entity any more... Free it for other things...
	G_FreeEntity( ent );
}


/*QUAKED npc_destroyer_waypoint_target (1 0 0) (-16 -16 -16) (16 16 16)
-------- KEYS --------
"partofstage"	REQUIRED: The waypoint index this is associated with.
"mass" defaults to 0.  Move Forward (0) or Back (1) to this waypoint.
"dmg" defaults to 0.  Rotate the lander Straight (0) Left (1) or Right (2) at this waypoint.
"health"	REQUIRED: This waypoint's number. Waypoints are followed in sequential order!
-------- SPAWNFLAGS --------
-------- NOTES --------
Lander target waypoint. (Where it lets troops off).

partofstage is required!
*/
void
NPC_Destroyer_Waypoint_Target ( gentity_t *ent )
{	// Store in a temporary array...
	int mass;
	VectorCopy( ent->s.origin, Destroyer_nodes[num_Destroyer_nodes].origin );
	G_SpawnInt( "partofstage", "0", &ent->partofstage );
	G_SpawnInt( "dmg", "0", &ent->damage );
	G_SpawnInt( "health", "0", &ent->health );
	G_SpawnInt( "mass", "0", &mass );
	Destroyer_nodes[num_Destroyer_nodes].land_point = qtrue;
	Destroyer_nodes[num_Destroyer_nodes].reverse = mass;
	Destroyer_nodes[num_Destroyer_nodes].wpIndex = ent->partofstage;
	Destroyer_nodes[num_Destroyer_nodes].wpNumber = ent->health;
	if ( !ent->partofstage )
	{
		G_Printf( "^1*** ^3ERROR^5: \"npc_destroyer_waypoint_target\" with no partofstage!\n" );
		return;
	}

	// Incrument the counter...
	num_Destroyer_nodes++;

	// Don't need this entity any more... Free it for other things...
	G_FreeEntity( ent );
}


//
// END - Destroyer Boats...

//
void
NPC_AI_Boat_Move ( gentity_t *NPC )
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
	if ( NPC->NPC_Class == CLASS_BOAT )
	{

	}
	else if ( NPC->NPC_Class == CLASS_LCVP )
	{					// Planes type NPC routing here... Just head forward for now...
		vec3_t	fwd;
		trace_t tr;
		vec3_t	uporg;
		vec3_t	downorg;
		float	diff;
		if ( NPC->lcvp_nodes[NPC->current_node].land_point )
		{
			if ( NPC_LCVP_Check_Disembark_Passengers( NPC) )
			{
				return; // Don't move if letting passengers disembark...
			}
		}

		trap_UnlinkEntity( NPC );
		VectorCopy( LCVPMins, NPC->r.mins );
		VectorCopy( LCVPMaxs, NPC->r.maxs );
		NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
		if ( VectorDistance( NPC->lcvp_nodes[NPC->current_node].origin, NPC->r.currentOrigin) <= 272 )
		{
			if ( NPC->current_node == NPC->pathsize )
			{
				G_AddEvent( NPC, EV_DESTROY_PARTICLES, 0 );
			}
		}

		if ( NPC->lcvp_nodes[NPC->current_node].land_point )
		{
			if ( VectorDistance( NPC->lcvp_nodes[NPC->current_node].origin, NPC->r.currentOrigin) <= 64 )
			{			// We need to land at this point... Should always get closer to this one...
				NPC->last_node = NPC->current_node;
				NPC->current_node++;
				NPC->next_node++;
				if ( NPC->current_node > NPC->pathsize )
				{		// We've left the map area.. Remove the entity... Exit the procedure...
					NPC_LCVP_Free( NPC );
					return;
				}
			}
		}
		else if ( VectorDistance( NPC->lcvp_nodes[NPC->current_node].origin, NPC->r.currentOrigin) <= 256 )
		{
			NPC->last_node = NPC->current_node;
			NPC->current_node++;
			NPC->next_node++;
			if ( NPC->current_node > NPC->pathsize )
			{			// We've left the map area.. Remove the entity... Exit the procedure...
				NPC_LCVP_Free( NPC );
				return;
			}
		}

		VectorCopy( NPC->r.currentOrigin, uporg );
		uporg[2] += 64;
		VectorCopy( NPC->r.currentOrigin, downorg );
		downorg[2] -= 64;
		trap_Trace( &tr, uporg, NULL, NULL, downorg, NPC->s.number, MASK_WATER );
		NPC->lcvp_nodes[NPC->current_node].origin[2] = tr.endpos[2];
		VectorSubtract( NPC->lcvp_nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
		vectoangles( NPC->move_vector, angles );
		angles[2] = 0;
		VectorCopy( angles, NPC->npc_ideal_view_angles );
		NPC->npc_ideal_view_angles[2] = 0;
		if ( NPC->lcvp_nodes[NPC->current_node].reverse )
		{
			SetLCVPAngles( NPC, 1000 );
		}
		else
		{
			SetLCVPAngles( NPC, 100 );
		}

		NPC->npc_view_angles[2] = 0;
		AngleVectors( NPC->npc_view_angles, fwd, NULL, NULL );
		VectorCopy( NPC->r.currentOrigin, origin );
		diff = fabs( AngleDifference( NPC->npc_view_angles[YAW], NPC->npc_ideal_view_angles[YAW]) );
		if ( NPC->lcvp_nodes[NPC->current_node].reverse && diff > 18 )
		{
			origin[0] -= fwd[0] * 8;
			origin[1] -= fwd[1] * 8;
			origin[2] -= fwd[2] * 8;
			NPC->npc_velocity[0] = fwd[0] * 8;
			NPC->npc_velocity[1] = fwd[1] * 8;
			NPC->npc_velocity[2] = fwd[2] * 8;
		}
		else
		{
			origin[0] += fwd[0] * 8;
			origin[1] += fwd[1] * 8;
			origin[2] += fwd[2] * 8;
			NPC->npc_velocity[0] = 0 - fwd[0] * 8;
			NPC->npc_velocity[1] = 0 - fwd[1] * 8;
			NPC->npc_velocity[2] = 0 - fwd[2] * 8;
		}

		VectorSubtract( origin, NPC->r.currentOrigin, move );
		VectorSubtract( NPC->npc_view_angles, NPC->r.currentAngles, amove );
		if ( !G_NPCPush( NPC, move, amove, &obstacle) )
		{

			//script_mover_blocked( NPC, obstacle );
		}

		VectorCopy( origin, NPC->s.pos.trBase );
		VectorCopy( NPC->npc_view_angles, NPC->s.apos.trBase );
		NPC->s.pos.trDuration = bot_thinktime.integer;
		NPC->s.pos.trTime = level.time + bot_thinktime.integer;
		NPC->s.apos.trDuration = bot_thinktime.integer;
		NPC->s.apos.trTime = level.time + bot_thinktime.integer;
		NPC->s.pos.trType = TR_INTERPOLATE;
		NPC->s.apos.trType = TR_INTERPOLATE;
		NPC->physicsObject = qtrue;
		VectorScale( fwd, 8, NPC->s.pos.trDelta );
		VectorCopy( NPC->npc_view_angles, NPC->s.apos.trDelta );

		// Store Old origin for velocity setting...
		VectorCopy( NPC->r.currentOrigin, NPC->oldOrigin );
		trap_LinkEntity( NPC );
		return;
	}
	else if ( NPC->NPC_Class == CLASS_DESTROYER )
	{					// Planes type NPC routing here... Just head forward for now...
		vec3_t	fwd;
		trace_t tr;
		vec3_t	uporg;
		vec3_t	downorg;
		float	diff;
		if ( VectorDistance( NPC->Destroyer_nodes[NPC->current_node].origin, NPC->r.currentOrigin) <= 256 /*128*/ /*16*/ )
		{
			NPC->last_node = NPC->current_node;
			NPC->current_node++;
			NPC->next_node++;
			if ( NPC->next_node > NPC->pathsize )
			{			// We've left the map area.. Remove the entity... Exit the procedure...
				NPC->last_node = NPC->current_node;
				NPC->current_node = 0;
				NPC->next_node = 1;
			}
		}

		VectorCopy( LCVPMins, NPC->r.mins );
		VectorCopy( LCVPMaxs, NPC->r.maxs );
		VectorCopy( NPC->r.currentOrigin, uporg );
		uporg[2] += 64;
		VectorCopy( NPC->r.currentOrigin, downorg );
		downorg[2] -= 64;
		trap_Trace( &tr, uporg, NULL, NULL, downorg, NPC->s.number, MASK_WATER );
		NPC->Destroyer_nodes[NPC->current_node].origin[2] = tr.endpos[2] + 32;
		VectorSubtract( NPC->Destroyer_nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
		vectoangles( NPC->move_vector, angles );
		angles[2] = 0;
		VectorCopy( angles, NPC->npc_ideal_view_angles );
		NPC->npc_ideal_view_angles[2] = 0;
		SetDestroyerAngles( NPC, 100 );
		NPC->npc_view_angles[2] = 0;
		AngleVectors( NPC->npc_view_angles, fwd, NULL, NULL );
		VectorCopy( NPC->r.currentOrigin, origin );
		diff = fabs( AngleDifference( NPC->npc_view_angles[YAW], NPC->npc_ideal_view_angles[YAW]) );
		if ( NPC->Destroyer_nodes[NPC->current_node].reverse && diff > 18 )
		{
			origin[0] -= fwd[0] * 8;
			origin[1] -= fwd[1] * 8;
			origin[2] -= fwd[2] * 8;
			NPC->npc_velocity[0] = fwd[0] * 8;
			NPC->npc_velocity[1] = fwd[1] * 8;
			NPC->npc_velocity[2] = fwd[2] * 8;
		}
		else
		{
			origin[0] += fwd[0] * 8;
			origin[1] += fwd[1] * 8;
			origin[2] += fwd[2] * 8;
			NPC->npc_velocity[0] = 0 - fwd[0] * 8;
			NPC->npc_velocity[1] = 0 - fwd[1] * 8;
			NPC->npc_velocity[2] = 0 - fwd[2] * 8;
		}

		VectorSubtract( origin, NPC->r.currentOrigin, move );
		VectorSubtract( NPC->npc_view_angles, NPC->r.currentAngles, amove );
		if ( !G_NPCPush( NPC, move, amove, &obstacle) )
		{

			//script_mover_blocked( NPC, obstacle );
		}

		VectorCopy( origin, NPC->s.pos.trBase );
		VectorCopy( NPC->npc_view_angles, NPC->s.apos.trBase );
		NPC->s.pos.trDuration = bot_thinktime.integer;
		NPC->s.pos.trTime = level.time + bot_thinktime.integer;
		NPC->s.apos.trDuration = bot_thinktime.integer;
		NPC->s.apos.trTime = level.time + bot_thinktime.integer;
		NPC->s.pos.trType = TR_INTERPOLATE;
		NPC->s.apos.trType = TR_INTERPOLATE;
		NPC->physicsObject = qtrue;
		VectorScale( fwd, 8, NPC->s.pos.trDelta );
		VectorCopy( NPC->npc_view_angles, NPC->s.apos.trDelta );

		// Store Old origin for velocity setting...
		VectorCopy( NPC->r.currentOrigin, NPC->oldOrigin );
		return;
	}
}


//===========================================================================
// Routine      : NPC_AI_Boat_Think

// Description  : Main think function for water NPCs.
void
NPC_AI_Boat_Think ( gentity_t *NPC )
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
