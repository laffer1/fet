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


#include "../game/q_shared.h"
#ifdef __BOT__
#include "../game/g_local.h"
#include "../botai/ai_main.h"
//#define NODE_INVALID	- 1
#ifdef __BOT_AAS__
extern int			BotGetArea ( int entnum );
int					next_aas_position_time[MAX_CLIENTS];
extern bot_state_t	botstates[MAX_CLIENTS];
extern void			BotRoamGoal ( bot_state_t *bs, vec3_t goal );
extern void			trap_BotUpdateEntityItems ( void );
extern int			AINode_MP_Seek_NBG ( bot_state_t *bs );
#include "../game/botlib.h"
#include "../game/be_ai_move.h"
#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE	150
#endif

#ifdef __ETE__
vmCvar_t	g_realmedic;
vmCvar_t	g_noSelfHeal;
#endif //__ETE__


//===========================================================================
// Routine      : AIMOD_AAS_MOVEMENT_Move

// Description  : AAS Movement extras... Unused..
void
AIMOD_AAS_MOVEMENT_Move ( gentity_t *bot, usercmd_t *ucmd )
{																// Make use of AAS stuff...
	return;
}
#endif //__BOT_AAS__

//
// Externals
//
extern int			next_weapon_change[MAX_CLIENTS];
extern int			numAxisOnlyNodes;
extern int			numAlliedOnlyNodes;
extern int			myteam, otherteam;
extern void			Set_Best_AI_weapon ( gentity_t *bot );
extern node_t		nodes[MAX_NODES];
extern int			AIMOD_Calculate_Goal ( gentity_t *bot );
extern void			Cmd_Activate_f ( gentity_t *ent );
extern qboolean		MyVisible ( gentity_t *self, gentity_t *other );
extern int			BotMP_Num_Defenders_Near ( vec3_t pos, gentity_t *bot );
extern int			BotMP_Find_POP_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
extern int			BotMP_Engineer_Find_POP_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
#ifdef __BOT_AAS__
extern int			BotMP_Medic_Find_POP_Goal_EntityNum
					(
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum,
						bot_state_t *bs
					);
extern int			BotMP_Medic_Scan_For_Revives
					(
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum,
						bot_state_t *bs
					);
#else //!__BOT_AAS__
extern int			BotMP_Medic_Find_POP_Goal_EntityNum
					(
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum,
						gentity_t	*bot
					);
#endif
extern int			BotMP_FieldOps_Find_POP_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
extern int			BotMP_Soldier_Find_POP_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
extern void			BotMP_Update_Goal_Lists ( void );
extern int			CreatePathAStar ( gentity_t *bot, int from, int to, short int *pathlist );
extern int			OrgVisibleBox ( vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodeTo ( vec3_t origin, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNode ( gentity_t *bot, int r, int type, gentity_t *master );
extern int			AIMOD_NAVIGATION_FindCloseReachableNode ( gentity_t *bot, int r, int type );
extern int			AIMOD_NAVIGATION_FindFarReachableNode ( gentity_t *bot, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodesTo ( gentity_t *bot, vec3_t origin, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodes ( gentity_t *bot, int r, int type );
extern void			Check_VoiceChats ( gentity_t *bot );
extern void			AI_ResetJob ( gentity_t *bot );
extern int			JobType ( gentity_t *bot );
extern void			AIMOD_Player_Special_Needs_Clear ( int clientNum );
extern qboolean		nodes_loaded;
extern qboolean		NodesLoaded ( void );
extern void			AIMOD_AI_DropMed ( gentity_t *bot );
extern void			AIMOD_AI_DropAmmo ( gentity_t *bot );
extern int			number_of_nodes;
extern gentity_t	*Dynamite_Entity ( gentity_t *ent );
extern vec3_t		bot_eng_fix_points[MAX_CLIENTS];			// g_main.c
extern gentity_t	*foundplayer;								// Closest found player to us after player_close calls... // AIMOD_nodes.c
extern void			G_BotCheckForCursorHints ( gentity_t *ent );
extern int			no_mount_time[MAX_CLIENTS];					// Don't try to mount emplaced for so many secs...
extern int			next_satchel_available_time[MAX_CLIENTS];	// Next time covert can use a satchel... - g_main.c
extern vec3_t		snipe_points[256];
extern int			num_snipe_points;
extern qboolean		above_average_node_height ( int nodeNum );
extern int			BotMP_Defender_Find_POP_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
extern vmCvar_t		bot_forced_auto_waypoint;					// Forced random bots to auto-waypoint the map...
extern qboolean		MyInfront ( gentity_t *self, gentity_t *other );
extern qboolean		OnIce[MAX_CLIENTS];							// Set if bot is on ice or not.
extern qboolean		Can_Airstrike ( gentity_t *ent, vec3_t origin );				// AIMOD_navigation.c
extern qboolean		Can_Arty ( gentity_t *ent, vec3_t origin );						// AIMOD_navigation.c
extern qboolean		Teammate_SpashDamage_OK ( gentity_t *ent, vec3_t origin );		// AIMOD_navigation.c
extern void			Weapon_Artillery ( gentity_t *ent );							// g_weapon.c
extern gentity_t	*weapon_grenadelauncher_fire ( gentity_t *ent, int grenType );	// g_weapon.c
extern int			NodeVisible ( vec3_t org1, vec3_t org2, int ignore );
extern vmCvar_t		bot_skill;
extern float		anglemod ( float a );
extern vmCvar_t		bot_thinktime;

// In unique_goal.c
extern int			Find_Goal_Teammate ( gentity_t *bot );
extern int			Find_Goal_Enemy ( gentity_t *bot );

//
// Locals
//
vec3_t				last_position[MAX_CLIENTS];
qboolean			debug_mode = qfalse;
short int			BotGetNextNode ( gentity_t *bot /*, int *state*/ );				// below...
void				NowHeal ( gentity_t *bot );
vec3_t				last_vector[MAX_CLIENTS];
int					last_node_set_time[MAX_CLIENTS];
int					randmomization_time[MAX_CLIENTS];
int					next_drop[MAX_CLIENTS];
int					out_of_trouble[MAX_CLIENTS];
int					reset_time[MAX_CLIENTS];
int					next_dynamite_test[MAX_CLIENTS];
int					next_visibility_test[MAX_CLIENTS];
vec3_t				planted_origin[MAX_CLIENTS];									// The origin we dropped a satchel charge at!
int					next_turret_angle_change[MAX_CLIENTS];							// Next time to change angles while on a tank or mg42 or aagun.
qboolean			runaway_node[MAX_CLIENTS];
vec3_t				SP_Bot_Position[MAX_CLIENTS];
int					Unique_FindGoal ( gentity_t *bot );
qboolean			last_medic_check[MAX_CLIENTS];
int					last_medic_check_time[MAX_CLIENTS];
int					last_node_hit_time[MAX_CLIENTS];
int					Bot_Ignore_Use_Time[MAX_CLIENTS];
int					duck_time[MAX_CLIENTS];
qboolean			AIMOD_MOVEMENT_CanMove ( gentity_t *bot, int direction );		// Below...
short int			BotGetNextNode ( gentity_t *bot /*, int *state*/ );				// below...
void				Bot_Set_New_Path ( gentity_t *bot, usercmd_t *ucmd );			// below
void				AIMOD_MOVEMENT_AttackMove ( gentity_t *bot, usercmd_t *ucmd );	// below...
int					next_arty[MAX_CLIENTS];
int					next_grenade[MAX_CLIENTS];
int					after_enemy[MAX_CLIENTS];


//===========================================================================
//
// Bot visibility code...
//
//===========================================================================
//===========================================================================
// Routine      : OrgVisibleCurve

// Description  : Check visibility at same heights...
int
OrgVisibleCurve ( vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore )
{	// Check visibility at same heights...
	trace_t tr;
	vec3_t	evenorg1;
	VectorCopy( org1, evenorg1 );
	evenorg1[2] = org2[2];
	trap_Trace( &tr, evenorg1, mins, maxs, org2, ignore, MASK_SOLID );
	if ( tr.fraction == 1 && !tr.startsolid && !tr.allsolid )
	{
		trap_Trace( &tr, evenorg1, mins, maxs, org1, ignore, MASK_SOLID );
		if ( tr.fraction == 1 && !tr.startsolid && !tr.allsolid )
		{
			return ( 1 );
		}
	}

	return ( 0 );
}


//===========================================================================
// Routine      : NodeOrgVisible
// Description  : Special node visibility check for bots... Returns the values below...
// 0 = wall in way
// 1 = player or no obstruction
// 2 = useable door in the way.
// 3 = useable team door in the way.

// 4 = door entity in the way.
int
NodeOrgVisible ( gentity_t *bot, vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg;

	// Look from up a little for bots..
	VectorCopy( org1, newOrg );
	newOrg[2] += 32;
	trap_Trace( &tr, newOrg, NULL, NULL, org2, ignore, MASK_SOLID );
	if ( tr.fraction == 1 )
	{
		trap_Trace( &tr, newOrg, NULL, NULL, org2, ignore, MASK_PLAYERSOLID );
		if
		(
			tr.fraction != 1 &&
			tr.entityNum != ENTITYNUM_NONE &&
			!strcmp( g_entities[tr.entityNum].classname, "func_door")
		)
		{	// A door in the way.. See if we have access...
			if ( !g_entities[tr.entityNum].allowteams )
			{
				return ( 2 );
			}

			if ( (g_entities[tr.entityNum].allowteams & TEAM_ALLIES) && bot->client->sess.sessionTeam == TEAM_ALLIES )
			{
				return ( 3 );
			}

			if ( (g_entities[tr.entityNum].allowteams & ALLOW_AXIS_TEAM) && bot->client->sess.sessionTeam == TEAM_AXIS )
			{
				return ( 3 );
			}

			return ( 4 );
		}

		return ( 1 );
	}

	return ( 0 );
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Avoidance_Needed
// Description  : Returns the what is/isn't blocking us.
// Do we need to avoid something?
// 0 = Not blocked.
// 1 = Blocked by world.
// 2 = Blocked by player.
int
AIMOD_MOVEMENT_Avoidance_Needed ( gentity_t *bot )
{
	trace_t tr;
	vec3_t	newOrg;
	VectorCopy( nodes[bot->current_node].origin, newOrg );
	trap_Trace( &tr, bot->r.currentOrigin, NULL, NULL, newOrg, bot->s.number, MASK_PLAYERSOLID );
	if ( tr.fraction == 1 && tr.entityNum == ENTITYNUM_WORLD )
	{
		return ( 1 );
	}
	else if ( tr.fraction == 1 && g_entities[tr.entityNum].s.eType == ET_PLAYER )
	{
		return ( 2 );
	}

	return ( 0 );
}

extern vec3_t	botTraceMins;
extern vec3_t	botTraceMaxs;

//===========================================================================
// Routine      : AIMOD_MOVEMENT_ReachableBy
// Description  : Determine if a blocked goal vector is reachable.. Returns how to get there...
// Details:
//				-1 is Not reachable at all...
//				 0 is All clear to goal...
//				 1 is Jump...
//				 2 is Duck...
//				 3 is Strafe Left...
//				 4 is Strafe Right...
//
#define NOT_REACHABLE			- 1
#define REACHABLE				0
#define REACHABLE_JUMP			1
#define REACHABLE_DUCK			2
#define REACHABLE_STRAFE_LEFT	3
#define REACHABLE_STRAFE_RIGHT	4


/* */
int
AIMOD_MOVEMENT_ReachableBy ( gentity_t *bot, vec3_t goal )
{
	trace_t				trace;
	vec3_t /*v, v2,*/ eyes, mins, maxs, Org, forward, right, up, rightOrigin, leftOrigin;

	// First check direct movement...
	VectorCopy( bot->r.currentOrigin, Org );

	//VectorCopy(bot->r.mins,v);
	//    v[2] += 18; // Stepsize
	//	trap_Trace(&trace, Org, v, bot->r.maxs, goal, bot->s.number, MASK_SOLID|MASK_OPAQUE);
	trap_Trace( &trace, Org, botTraceMins, botTraceMaxs, goal, bot->s.number, MASK_SOLID | MASK_OPAQUE );
	if ( trace.fraction == 1.0 )
	{
		return ( REACHABLE );				// Yes we can see it
	}

	// Now look for jump access...
	VectorAdd( Org, playerMins, mins );
	VectorAdd( Org, playerMaxs, maxs );

	//	VectorCopy(bot->r.mins,v);
	//    v[2] += 18; // Stepsize
	VectorCopy( Org, eyes );
	eyes[2] += 32;

	//trap_Trace(&trace, eyes, v, bot->r.maxs, goal, bot->s.number, MASK_SOLID|MASK_OPAQUE);
	trap_Trace( &trace, eyes, botTraceMins, botTraceMaxs, goal, bot->s.number, MASK_SOLID | MASK_OPAQUE );
	if ( trace.fraction == 1.0 )
	{
		return ( REACHABLE_JUMP );			// Yes we can see it
	}

	// Now look for crouch access...
	//CROUCH_VIEWHEIGHT ??
	//	VectorCopy(bot->r.mins,v);
	//    v[2] += 18; // Stepsize
	//	VectorCopy(bot->r.maxs,v2);
	//    v2[2] *= 0.5; // Stepsize
	//trap_Trace(&trace, Org, v, v2, goal, bot->s.number, MASK_SOLID|MASK_OPAQUE);
	trap_Trace( &trace, eyes, botTraceMins, botTraceMaxs, goal, bot->s.number, MASK_SOLID | MASK_OPAQUE );
	if ( trace.fraction == 1.0 )
	{
		return ( REACHABLE_DUCK );			// Yes we can see it
	}

	// Now look for strafe access... Left or Right...
#ifdef __NPC__
	if ( bot->NPC_client )
	{
		AngleVectors( bot->NPC_client->ps.viewangles, forward, right, up );
	}
	else
	{
		AngleVectors( bot->client->ps.viewangles, forward, right, up );
	}

#else //!__NPC__
	AngleVectors( bot->client->ps.viewangles, forward, right, up );
#endif //__NPC__
	VectorMA( bot->r.currentOrigin, 64, right, rightOrigin );
	VectorMA( bot->r.currentOrigin, -64, right, leftOrigin );
	if ( NodeVisible( leftOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_left_timer = level.time + 201;
		VectorCopy( leftOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( NodeVisible( rightOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_right_timer = level.time + 201;
		VectorCopy( rightOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	// And strafing a bit further away...
	VectorMA( bot->r.currentOrigin, 128, right, rightOrigin );
	VectorMA( bot->r.currentOrigin, -128, right, leftOrigin );
	if ( NodeVisible( leftOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_left_timer = level.time + 401;
		VectorCopy( leftOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( NodeVisible( rightOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_right_timer = level.time + 401;
		VectorCopy( rightOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	// And strafing a bit further away...
	VectorMA( bot->r.currentOrigin, 192, right, rightOrigin );
	VectorMA( bot->r.currentOrigin, -192, right, leftOrigin );
	if ( NodeVisible( leftOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_left_timer = level.time + 601;
		VectorCopy( leftOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( NodeVisible( rightOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		bot->bot_strafe_right_timer = level.time + 601;
		VectorCopy( rightOrigin, bot->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	return ( NOT_REACHABLE );
}

//===========================================================================
// Routine      : AIMOD_MOVEMENT_CurrentNodeReachable
// Description  : Quick vis check of our current node...
extern int		ReachableBox ( vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore );
extern void		BOT_MakeLinkDangerous ( gentity_t *bot, int node, int linkNum );
extern vec3_t	botTraceMins;
extern vec3_t	botTraceMaxs;

//extern vec3_t	tankMins;
//extern vec3_t	tankMaxs;
//vec3_t	tankTraceMaxs = {142, 142, 78};
//vec3_t	tankTraceMins = {-142, -142, -24};
//vec3_t	tankTraceMaxs = {192, 192, 78};
//vec3_t	tankTraceMins = {-192, -192, -24};
//vec3_t	tankTraceMaxs = {64, 64, 0/*78*/};
//vec3_t	tankTraceMins = {-64, -64, 0/*-24*/};
//vec3_t	tankTraceMaxs = {48, 48, 0/*78*/};
//vec3_t	tankTraceMins = {-48, -48, 0/*-24*/};
extern vec3_t	tankTraceMins;
extern vec3_t	tankTraceMaxs;


/* */
qboolean
AIMOD_MOVEMENT_CurrentNodeReachable ( gentity_t *bot )
{							// Quick vis check of our current node...
	vec3_t	bot_origin;
	int		reachable = 0;
	if ( bot->bot_last_reachable_check == bot->current_node )
	{
		return ( qtrue );
	}

	bot->bot_last_reachable_check = bot->current_node;
	if ( !bot->current_node || !bot->next_node || !bot->longTermGoal )
	{
		return ( qtrue );	// Ignore until we have a path...
	}

	VectorCopy( bot->r.currentOrigin, bot_origin );
	bot_origin[2] += 4 /*16*/ ;

	// Trace to the next node..
#ifdef __VEHICLES__
	if ( bot->client && bot->client->ps.eFlags & EF_TANK )
	{
		reachable = ReachableBox( bot_origin, tankTraceMins, tankTraceMaxs, nodes[bot->current_node].origin,
								  bot->s.number );
	}
	else
#endif //__VEHICLES__
		reachable = ReachableBox( bot_origin, botTraceMins, botTraceMaxs, nodes[bot->current_node].origin, bot->s.number );

	//if (reachable == 0 /* Blocked */ || reachable == 2 /* Lava/Water/Slime*/ )
	if ( /*reachable == 0 || */ reachable == 2 /* Lava/Water/Slime*/ )
	{
		if ( reachable == 2 /* Lava/Water/Slime*/ )
		{					// Dangerous...
			int loop = 0;
			int link = -1;
			for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
			{				// Find the link we last used and mark it...
				if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
				{			// Mark it as Dangerous...
					BOT_MakeLinkDangerous( bot, bot->last_node, loop );
					break;
				}
			}
		}
		else
		{					// Blocked...
			int			loop = 0;
			int			link = -1;
			qboolean	onLadder = qfalse;
			if
			(
				bot->client &&
				(
					bot->client->ps.serverCursorHint == HINT_LADDER ||
					bot->client->ps.torsoAnim == BOTH_CLIMB ||
					bot->client->ps.legsAnim == BOTH_CLIMB
				)
			)
			{
				onLadder = qtrue;
			}

#ifdef __NPC__
			if
			(
				bot->NPC_client &&
				(
					bot->NPC_client->ps.serverCursorHint == HINT_LADDER ||
					bot->NPC_client->ps.torsoAnim == BOTH_CLIMB ||
					bot->NPC_client->ps.legsAnim == BOTH_CLIMB
				)
			)
			{
				onLadder = qtrue;
			}
#endif //__NPC__
			if ( !onLadder )
			{
				for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
				{			// Find the link we last used and mark it...
					if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
					{		// Mark it as Dangerous...
						BOT_MakeLinkUnreachable( bot, bot->last_node, loop );
						break;
					}
				}
			}
		}

		return ( qfalse );
	}

	return ( qtrue );
}

/*
//===========================================================================
// Routine      : AIMOD_MOVEMENT_CanMove
// Description  : Checks if bot can move (really just checking ground)
//              : Not an accurate check, but does a decent enough job
//              : and looks for lava/slime.
qboolean AIMOD_MOVEMENT_CanMove(gentity_t *bot, int direction)
{
    vec3_t forward, right;
    vec3_t offset,start,end;
    vec3_t angles;
    trace_t tr;

    // Now check to see if move will move us off an edge
    //-------------------------------------------------------
    VectorCopy(bot->s.angles,angles);
    
    if(direction == BOT_MOVE_LEFT)
        angles[1] += 90;
    else if(direction == BOT_MOVE_RIGHT)
        angles[1] -= 90;
    else if(direction == BOT_MOVE_BACK)
        angles[1] -=180;
    //-------------------------------------------------------
    
    // Set up the vectors
    //-------------------------------------------------------
    AngleVectors (angles, forward, right, NULL);
    
    VectorSet(offset, 36, 0, 24);
    G_ProjectSource (bot->s.origin, offset, forward, right, start);
        
    VectorSet(offset, 36, 0, -100);
    G_ProjectSource (bot->s.origin, offset, forward, right, end);
    
    //tr = gi.trace(start, NULL, NULL, end, bot, MASK_SOLID|MASK_OPAQUE);
	trap_Trace(&tr,start, NULL, NULL, end, bot->s.number, MASK_SOLID|MASK_OPAQUE);
    
    if(tr.fraction == 1.0 || tr.contents & (CONTENTS_LAVA|CONTENTS_SLIME))
    {
        if(debug_mode)
            G_Printf("%s: move blocked\n",bot->client->pers.netname);
        return qfalse;   
    }
    
    // Can Move
    return qtrue;
}
*/


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Visible_For_Attacking

// Description  : Is our enemy attackable???
qboolean
AIMOD_MOVEMENT_Visible_For_Attacking ( gentity_t *self, gentity_t *other )
{
	return ( MyVisible( self, other) );
}

//===========================================================================
//
// Bot user command code...
//
//===========================================================================
//===========================================================================
// Routine      : G_AdjustforStrafe
// Description  : Adjusts the moveDir to account for strafing...

/*void G_AdjustforStrafe(gentity_t *bot, vec3_t moveDir)
{
	vec3_t right;

	if(!bot->strafeDir || bot->strafeTime < level.time )
	{//no strafe
		return;
	}

	AngleVectors(bot->client->ps.viewangles, NULL, right, NULL);

	//flaten up/down
	right[2] = 0;

	if(bot->strafeDir == 2)
	{//strafing left
		VectorScale(right, -1, right);
	}

	//We assume that moveDir has been normalized before this function.
	VectorAdd(moveDir, right, moveDir);
	VectorNormalize(moveDir);
}*/


//===========================================================================
// Routine      : G_UcmdMoveForDir

// Description  : Set a valid ucmd move for the current move direction...
void
G_UcmdMoveForDir ( gentity_t *self, usercmd_t *cmd, vec3_t dir )
{
	vec3_t	forward, right, up;
	float	speed = 127.0f;
	if ( cmd->buttons & BUTTON_WALKING )
	{
		speed = 64.0f;
	}

#ifdef __NPC__
	if ( self->NPC_client )
	{
		AngleVectors( self->NPC_client->ps.viewangles, forward, right, up );
	}
	else
	{
		AngleVectors( self->client->ps.viewangles, forward, right, up );
	}

#else //!__NPC__
	AngleVectors( self->client->ps.viewangles, forward, right, up );
#endif //__NPC__
	dir[2] = 0;
	VectorNormalize( dir );
	cmd->forwardmove = DotProduct( forward, dir ) * speed;
	cmd->rightmove = DotProduct( right, dir ) * speed;

	//cmd->upmove = abs(forward[3] ) * dir[3] * speed;
}


//===========================================================================
//
// Bot misc code...
//
//===========================================================================
//===========================================================================
// Routine      : AIMOD_HaveAmmoForWeapon

// Description  : Does the bot have any ammo for this weapon...
qboolean
AIMOD_HaveAmmoForWeapon ( playerState_t *ps, weapon_t wep )
{
	weapon_t	cur_wep = wep;
	if ( wep == WP_MOBILE_MG42_SET )
	{
		cur_wep = WP_MOBILE_MG42;
	}

	if ( ps->ammoclip[cur_wep] <= 0 && ps->ammo[cur_wep] <= 0 )
	{
		return ( qfalse );
	}

	return ( qtrue );
}


//===========================================================================
// Routine      : AIMOD_NeedAmmoForWeapon

// Description  : Does the bot need any ammo for this weapon...
qboolean
AIMOD_NeedAmmoForWeapon ( playerState_t *ps, weapon_t wep )
{
	int maxammo = 0;
	if ( wep == WP_MOBILE_MG42_SET )
	{
		wep = WP_MOBILE_MG42;
	}

	if ( wep == WP_GRENADE_LAUNCHER || wep == WP_GRENADE_PINEAPPLE )
	{
		maxammo = BG_GrenadesForClass( ps->stats[STAT_PLAYER_CLASS], g_entities[ps->clientNum].client->sess.skill );
	}
	else if ( wep == WP_GARAND_SCOPE )
	{
		maxammo = BG_MaxAmmoForWeapon( WP_GARAND, g_entities[ps->clientNum].client->sess.skill );
	}
	else if ( wep == WP_K43_SCOPE )
	{
		maxammo = BG_MaxAmmoForWeapon( WP_K43, g_entities[ps->clientNum].client->sess.skill );
	}
	else if ( wep == WP_FG42SCOPE )
	{
		maxammo = BG_MaxAmmoForWeapon( WP_FG42, g_entities[ps->clientNum].client->sess.skill );
	}

	//	else if (wep == WP_KAR98)
	//		maxammo = BG_MaxAmmoForWeapon( WP_GPG40, g_entities[ps->clientNum].client->sess.skill );
	//	else if (wep == WP_CARBINE)
	//		maxammo = BG_MaxAmmoForWeapon( WP_M7, g_entities[ps->clientNum].client->sess.skill );
	else if ( wep == WP_MEDKIT || wep == WP_AMMO || wep == WP_PLIERS || wep == WP_KNIFE )
	{
		return ( qfalse );
	}
	else
	{
		maxammo = BG_MaxAmmoForWeapon( wep, g_entities[ps->clientNum].client->sess.skill );
	}

	if ( ps->ammo[wep] <= maxammo * 0.75 /*0.25*/ )
	{
		return ( qtrue );
	}

	return ( qfalse );
}


//===========================================================================
// Routine      : AIMOD_InformTeamOfArty

// Description  : Inform nearby teammates of an arty strike area...
void
AIMOD_InformTeamOfArty ( gentity_t *bot )
{	// Test.. See if we can not vischek as often by assigning enemies in a fast way...
	int loop;
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*ent = &g_entities[loop];
		if ( !ent )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}

		if ( ent->health > 0 )
		{
			continue;
		}

		if ( !OnSameTeam( bot, ent) )
		{
			continue;
		}

		if ( VectorDistance( bot->enemy->r.currentOrigin, ent->r.currentOrigin) > 256 )
		{
			continue;
		}

		ent->bot_runaway_time = level.time + 20000;
	}
}


//===========================================================================
// Routine      : AIMOD_InformTeamOfAirstrike

// Description  : Inform nearby teammates of an airstrike area...
void
AIMOD_InformTeamOfAirstrike ( gentity_t *bot )
{	// Test.. See if we can not vischek as often by assigning enemies in a fast way...
	int loop;
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*ent = &g_entities[loop];
		if ( !ent )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}

		if ( ent->health > 0 )
		{
			continue;
		}

		if ( !OnSameTeam( bot, ent) )
		{
			continue;
		}

		if ( VectorDistance( bot->enemy->r.currentOrigin, ent->r.currentOrigin) > 256 )
		{
			continue;
		}

		ent->bot_runaway_time = level.time + 12000;
	}
}


//===========================================================================
// Routine      : AIMOD_InformTeamOfGrenade

// Description  : Inform nearby teammates of a grenade...
void
AIMOD_InformTeamOfGrenade ( gentity_t *bot )
{	// Test.. See if we can not vischek as often by assigning enemies in a fast way...
	int loop;
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*ent = &g_entities[loop];
		if ( !ent )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}

		if ( ent->health > 0 )
		{
			continue;
		}

		if ( !OnSameTeam( bot, ent) )
		{
			continue;
		}

		if ( VectorDistance( bot->enemy->r.currentOrigin, ent->r.currentOrigin) > 256 )
		{
			continue;
		}

		ent->bot_runaway_time = level.time + 6000;
	}
}


//===========================================================================
// Routine      : AIMOD_InformTeamOfEnemy

// Description  : See if we can not vischek as often by assigning enemies in a fast way...
void
AIMOD_InformTeamOfEnemy ( gentity_t *bot )
{	// Test.. See if we can not vischek as often by assigning enemies in a fast way...
	/*	int loop;

	for (loop = 0;loop < MAX_CLIENTS;loop++)
	{
		gentity_t *ent = &g_entities[loop];

		if (!ent)
			continue;

		if (!ent->client)
			continue;

		if (ent->health > 0)
			continue;

		if (!OnSameTeam(bot, ent))
			continue;

		if (ent->client->sess.playerType == PC_SOLDIER)
			continue; // Engineers shouldn't share enemies, they have work to do.

		if (VectorDistance(bot->r.currentOrigin, ent->r.currentOrigin) > 512)
			continue;

		if (!ent->enemy 
			|| VectorDistance(ent->r.currentOrigin, ent->enemy->r.currentOrigin) < VectorDistance(ent->r.currentOrigin, bot->enemy->r.currentOrigin))
		continue;

		if (!MyVisible(ent, bot->enemy))
			continue;

		// OK.. Set their enemy to ours!
		ent->enemy = bot->enemy;
		ent->bot_enemy_visible_time = level.time + 100;
	}
*/
}


//===========================================================================
// Routine      : Is_Medic_Close

// Description  : Is there a medic close to us???
qboolean
Is_Medic_Close ( int targetNum )
{
	int			j;

	//qboolean ok = qfalse;
	gentity_t	*bot = &g_entities[targetNum];
	for ( j = 0; j < MAX_CLIENTS; j++ )
	{
		gentity_t	*test = &g_entities[j];
		if ( !test )
		{
			continue;
		}

		if ( !test->client )
		{
			continue;
		}

		if ( test->health <= 0 )
		{
			continue;
		}

		if ( test->health <= 0 )
		{
			continue;
		}

		if ( test->client->sess.sessionTeam == TEAM_SPECTATOR || test->client->ps.pm_flags & PMF_LIMBO )
		{
			continue;
		}

		if ( !OnSameTeam( test, bot) )
		{
			continue;
		}

		if ( test->client->sess.playerType != PC_MEDIC )
		{
			continue;
		}

		if ( VectorDistance( test->r.currentOrigin, bot->r.currentOrigin) > 1024 )
		{
			continue;
		}

		// Found a medic close by!
		return ( qtrue );
	}

	// None found...
	return ( qfalse );
}


//===========================================================================
//
// Bot view angles code...
//
//===========================================================================
//===========================================================================
// Routine      : BotSetViewAngle

// Description  : Set bot view angles over time... Called from BotSetViewAngles (below)...
float
BotSetViewAngle ( float angle, float ideal_angle, float speed )
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
BotSetViewAngles ( gentity_t *bot, float thinktime )
{	// Just keep bot->bot_ideal_view_angles correct and call this to make it realistic...
	float	diff, factor, maxchange, anglespeed, fulldiff;
	int		i;
#ifdef __NPC__
	if ( bot->NPC_client )
	{
		fulldiff = fabs( AngleDifference( bot->NPC_client->ps.viewangles[0], bot->bot_ideal_view_angles[0]) );
		fulldiff += fabs( AngleDifference( bot->NPC_client->ps.viewangles[1], bot->bot_ideal_view_angles[1]) );
		fulldiff += fabs( AngleDifference( bot->NPC_client->ps.viewangles[2], bot->bot_ideal_view_angles[2]) );
	}
	else
	{
		fulldiff = fabs( AngleDifference( bot->client->ps.viewangles[0], bot->bot_ideal_view_angles[0]) );
		fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[1], bot->bot_ideal_view_angles[1]) );
		fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[2], bot->bot_ideal_view_angles[2]) );
	}

#else //!__NPC__
	fulldiff = fabs( AngleDifference( bot->client->ps.viewangles[0], bot->bot_ideal_view_angles[0]) );
	fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[1], bot->bot_ideal_view_angles[1]) );
	fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[2], bot->bot_ideal_view_angles[2]) );
#endif //__NPC__
	thinktime = 50 * fulldiff;

	//	if (fulldiff >= 180)
	//		bot->bot_turn_wait_time = level.time + thinktime;
	if ( bot->bot_ideal_view_angles[PITCH] > 180 )
	{
		bot->bot_ideal_view_angles[PITCH] -= 360;
	}

	//
	factor = 0.15;
	maxchange = 240;
	maxchange *= thinktime;
	for ( i = 0; i < 2; i++ )
	{
#ifdef __NPC__
		if ( bot->NPC_client )
		{
			diff = fabs( AngleDifference( bot->NPC_client->ps.viewangles[i], bot->bot_ideal_view_angles[i]) );
			anglespeed = diff * factor;
			if ( anglespeed > maxchange )
			{
				anglespeed = maxchange;
			}

			bot->NPC_client->ps.viewangles[i] = BotSetViewAngle( bot->NPC_client->ps.viewangles[i],
																 bot->bot_ideal_view_angles[i], anglespeed );
		}
		else
		{
			diff = fabs( AngleDifference( bot->client->ps.viewangles[i], bot->bot_ideal_view_angles[i]) );
			anglespeed = diff * factor;
			if ( anglespeed > maxchange )
			{
				anglespeed = maxchange;
			}

			bot->client->ps.viewangles[i] = BotSetViewAngle( bot->client->ps.viewangles[i],
															 bot->bot_ideal_view_angles[i], anglespeed );
		}

#else //!__NPC__
		diff = fabs( AngleDifference( bot->client->ps.viewangles[i], bot->bot_ideal_view_angles[i]) );
		anglespeed = diff * factor;
		if ( anglespeed > maxchange )
		{
			anglespeed = maxchange;
		}

		bot->client->ps.viewangles[i] = BotSetViewAngle( bot->client->ps.viewangles[i], bot->bot_ideal_view_angles[i],
														 anglespeed );
#endif //__NPC__
	}

#ifdef __NPC__
	if ( bot->NPC_client )
	{
		if ( bot->NPC_client->ps.viewangles[PITCH] > 180 )
		{
			bot->NPC_client->ps.viewangles[PITCH] -= 360;
		}

		VectorCopy( bot->NPC_client->ps.viewangles, bot->s.angles );
	}
	else
	{
		if ( bot->client->ps.viewangles[PITCH] > 180 )
		{
			bot->client->ps.viewangles[PITCH] -= 360;
		}

		VectorCopy( bot->client->ps.viewangles, bot->s.angles );
	}

#else //!__NPC__
	if ( bot->client->ps.viewangles[PITCH] > 180 )
	{
		bot->client->ps.viewangles[PITCH] -= 360;
	}

	//trap_EA_View(bot->s.number, bot->client->ps.viewangles);
	VectorCopy( bot->client->ps.viewangles, bot->s.angles );
#endif //__NPC__
}


//===========================================================================
//
// Bot nodes and generic movement code...
//
//===========================================================================
//===========================================================================
// Routine      : AIMOD_SinglePlayerMovement

// Description  : Single Player (coop) movement
void
AIMOD_SinglePlayerMovement ( gentity_t *bot, usercmd_t *ucmd )
{

}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Fallback

// Description  : Fallback navigation. For emergency routing...
qboolean
AIMOD_MOVEMENT_Fallback ( gentity_t *bot, usercmd_t *ucmd )
{															// Fallback navigation. returns if it was used or not as qtrue/qfalse.
	if ( NodesLoaded() == qfalse || bot_forced_auto_waypoint.integer )
	{														// Random for unrouted maps.
		int		vis;
		vec3_t	forward, forward_pos;
		AngleVectors( bot->s.angles, forward, NULL, NULL );
		VectorMA( bot->r.currentOrigin, 96, forward, forward_pos );
		vis = NodeVisible( bot->r.currentOrigin, forward_pos, bot->s.number );

		//0 = wall in way
		//1 = player or no obstruction
		//2 = useable door in the way.
		//3 = door entity in the way.
		//if (VectorLength(bot->client->ps.velocity) < 64 )
		if ( !(vis == 1 /*|| vis == 2 || vis == 3*/ ) )
		{													// We havn't moved at all... Let's randomize things.
			vec3_t	temp;
			if ( Bot_Ignore_Use_Time[bot->s.clientNum] < level.time && no_mount_time[bot->s.clientNum] < level.time )
			{
				Cmd_Activate_f( bot );
			}

			VectorCopy( bot->s.angles, temp );
			temp[0] = bot->s.angles[0] = 0;
			temp[2] = bot->s.angles[2] = 0;

			//temp[1] = bot->s.angles[1] - rand()%360;
			temp[1] = Q_TrueRand( 0, 360 );
			bot->client->AImovetime = level.time + 2000;	// 10 seconds?
			VectorCopy( temp, bot->s.angles );
			ucmd->forwardmove = 120;
		}
		else
		{
			ucmd->forwardmove = 120;
		}

		if ( bot->waterlevel )
		{
			ucmd->upmove = 120;								// Try to always be on surface...
		}

		return ( qtrue );
	}

	return ( qfalse );
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Backup

// Description  : Fallback navigation. For emergency routing...
void
AIMOD_MOVEMENT_Backup ( gentity_t *bot, usercmd_t *ucmd )
{																// Fallback navigation. returns if it was used or not as qtrue/qfalse.
	int		vis;
	vec3_t	forward, forward_pos;
	AngleVectors( bot->s.angles, forward, NULL, NULL );
	VectorMA( bot->r.currentOrigin, 96, forward, forward_pos );
	vis = NodeVisible( bot->r.currentOrigin, forward_pos, bot->s.number );

	//0 = wall in way
	//1 = player or no obstruction
	//2 = useable door in the way.
	//3 = door entity in the way.
	if ( bot->client )
	{

		//if (VectorLength(bot->client->ps.velocity) < 64 )
		if ( !(vis == 1 /*|| vis == 2 || vis == 3*/ ) )
		{														// We havn't moved at all... Let's randomize things.
			vec3_t	temp;
			if ( Bot_Ignore_Use_Time[bot->s.clientNum] < level.time && no_mount_time[bot->s.clientNum] < level.time )
			{
				Cmd_Activate_f( bot );
			}

			VectorCopy( bot->s.angles, temp );
			temp[0] = bot->s.angles[0] = 0;
			temp[2] = bot->s.angles[2] = 0;

			//temp[1] = bot->s.angles[1] - rand()%360;
			temp[1] = Q_TrueRand( 0, 360 );
			bot->client->AImovetime = level.time + 2000;		// 10 seconds?
			VectorCopy( temp, bot->s.angles );
			ucmd->forwardmove = 120;
		}
		else
		{
			ucmd->forwardmove = 120;
		}

		if ( bot->waterlevel )
		{
			ucmd->upmove = 120;									// Try to always be on surface...
		}
	}

#ifdef __NPC__
	else if ( bot->NPC_client )
	{

		//if (VectorLength(bot->NPC_client->ps.velocity) < 64 )
		if ( !(vis == 1 /*|| vis == 2 || vis == 3*/ ) )
		{														// We havn't moved at all... Let's randomize things.
			vec3_t	temp;

			//			if ( Bot_Ignore_Use_Time[bot->s.number] < level.time && no_mount_time[bot->s.number] < level.time)
			//				Cmd_Activate_f( bot );
			VectorCopy( bot->s.angles, temp );
			temp[0] = bot->s.angles[0] = 0;
			temp[2] = bot->s.angles[2] = 0;
			temp[1] = Q_TrueRand( 0, 360 );
			bot->NPC_client->AImovetime = level.time + 2000;	// 10 seconds?
			VectorCopy( temp, bot->s.angles );
			ucmd->forwardmove = 120;
		}
		else
		{
			ucmd->forwardmove = 120;
		}

		if ( bot->waterlevel )
		{
			ucmd->upmove = 120;									// Try to always be on surface...
		}
	}
#endif //__NPC__
}


//#ifndef __BOT_PERFECTION__
//===========================================================================
// Routine      : AIMOD_MOVEMENT_From_Void

// Description  : Fallback navigation. For emergency routing...
void
AIMOD_MOVEMENT_From_Void ( gentity_t *bot, usercmd_t *ucmd )
{														// Forced fallback navigation (out of voids)...
	float	diff;
	vec3_t	fwdOrg;
	vec3_t	fwd;
	diff = fabs( AngleDifference( bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]) );
	if ( diff > 90.0f )
	{													// Delay large turns...
		BotSetViewAngles( bot, 100 );
		ucmd->forwardmove = 0;
		return;
	}

	if ( VectorLength( bot->client->ps.velocity) < 64 )
	{													// We havn't moved at all... Let's randomize things.
		vec3_t	temp;
		if ( Bot_Ignore_Use_Time[bot->s.clientNum] < level.time && no_mount_time[bot->s.clientNum] < level.time )
		{
			Cmd_Activate_f( bot );
		}

		VectorCopy( bot->s.angles, temp );
		temp[0] = bot->s.angles[0];
		temp[2] = bot->s.angles[2];
		temp[1] = bot->s.angles[1] + Q_TrueRand( 0, 360 );
		bot->client->AImovetime = level.time + 10000;	// 10 seconds?

		// Set up ideal view angles for this node...
		VectorCopy( temp, bot->bot_ideal_view_angles );
		bot->bot_ideal_view_angles[2] = 0;				// Always look straight.. Don't look up or down at nodes...
		diff = fabs( AngleDifference( bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]) );
		if ( diff > 90.0f )
		{												// Delay large turns...
			BotSetViewAngles( bot, 100 );
			ucmd->forwardmove = 0;
		}
		else
		{
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
			VectorCopy( bot->r.currentOrigin, last_vector[bot->s.clientNum] );
			AngleVectors( bot->s.angles, fwd, NULL, NULL );
			VectorCopy( bot->r.currentOrigin, fwdOrg );
			fwdOrg[0] += ( fwd[0] * 64 );
			fwdOrg[1] += ( fwd[1] * 64 );
			fwdOrg[2] += ( fwd[2] * 64 );
			if ( NodeVisible( bot->r.currentOrigin, fwdOrg, bot->s.number) >= 1 )
			{
				if ( ucmd->buttons & BUTTON_WALKING )
				{
					ucmd->forwardmove = 64;
				}
				else
				{
					ucmd->forwardmove = 127;
				}
			}
		}
	}
	else
	{
		if ( Bot_Ignore_Use_Time[bot->s.clientNum] < level.time && no_mount_time[bot->s.clientNum] < level.time )
		{
			Cmd_Activate_f( bot );
		}

		diff = fabs( AngleDifference( bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]) );
		if ( diff > 90.0f )
		{												// Delay large turns...
			BotSetViewAngles( bot, 100 );
			ucmd->forwardmove = 0;
		}
		else
		{
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
			VectorCopy( bot->r.currentOrigin, last_vector[bot->s.clientNum] );
			AngleVectors( bot->s.angles, fwd, NULL, NULL );
			VectorCopy( bot->r.currentOrigin, fwdOrg );
			fwdOrg[0] += ( fwd[0] * 64 );
			fwdOrg[1] += ( fwd[1] * 64 );
			fwdOrg[2] += ( fwd[2] * 64 );
			if ( NodeVisible( bot->r.currentOrigin, fwdOrg, bot->s.number) >= 1 )
			{
				if ( ucmd->buttons & BUTTON_WALKING )
				{
					ucmd->forwardmove = 64;
				}
				else
				{
					ucmd->forwardmove = 127;
				}
			}
		}
	}

	if ( bot->waterlevel )
	{
		ucmd->upmove = 120;								// Try to always be on surface...
	}
}


//#endif
//===========================================================================
// Routine      : AIMOD_MOVEMENT_Fallback2

// Description  : Fallback navigation. For emergency routing...
void
AIMOD_MOVEMENT_Fallback2 ( gentity_t *bot, usercmd_t *ucmd )
{														// Fallback navigation. For emergency routing...
	vec3_t	fwdOrg;
	vec3_t	fwd;
	if ( VectorLength( bot->client->ps.velocity) < 64 )
	{													// We havn't moved at all... Let's randomize things.
		vec3_t	temp;
		if ( Bot_Ignore_Use_Time[bot->s.clientNum] < level.time && no_mount_time[bot->s.clientNum] < level.time )
		{
			Cmd_Activate_f( bot );
		}

		VectorCopy( bot->s.angles, temp );
		temp[0] = bot->s.angles[0];
		temp[2] = bot->s.angles[2];
		temp[1] = bot->s.angles[1] - rand() % 360;
		bot->client->AImovetime = level.time + 10000;	// 10 seconds?
		VectorCopy( temp, bot->s.angles );
		AngleVectors( bot->s.angles, fwd, NULL, NULL );
		VectorCopy( bot->r.currentOrigin, fwdOrg );
		fwdOrg[0] += ( fwd[0] * 64 );
		fwdOrg[1] += ( fwd[1] * 64 );
		fwdOrg[2] += ( fwd[2] * 64 );
		if ( NodeVisible( bot->r.currentOrigin, fwdOrg, bot->s.number) >= 1 )
		{
			ucmd->forwardmove = 120;
		}
	}
	else
	{
		AngleVectors( bot->s.angles, fwd, NULL, NULL );
		VectorCopy( bot->r.currentOrigin, fwdOrg );
		fwdOrg[0] += ( fwd[0] * 64 );
		fwdOrg[1] += ( fwd[1] * 64 );
		fwdOrg[2] += ( fwd[2] * 64 );
		if ( NodeVisible( bot->r.currentOrigin, fwdOrg, bot->s.number) >= 1 )
		{
			ucmd->forwardmove = 120;
		}
	}

	if ( bot->waterlevel )
	{
		ucmd->upmove = 120;								// Try to always be on surface...
	}
}

/*
//===========================================================================
// Routine      : fast_find_runaway
// Description  : Find a node to run away to...
int fast_find_runaway ( vec3_t origin )
{
	int i = 0;

	for (i = 0;i < number_of_nodes;i+=rand()%4)
	{
		float dist = VectorDistance(nodes[i].origin, origin);
		
		if (dist > 800 && dist < 1000)
			break;
	}

	if (i >= number_of_nodes)
		return -1;

	return i;
}*/


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Defender_Find_Sniper_Spot

// Description  : Find a sniper position (defender version).
int
AIMOD_MOVEMENT_Defender_Find_Sniper_Spot ( gentity_t *bot )
{

	/*	int entNum = BotMP_Defender_Find_POP_Goal_EntityNum( bot, bot->current_target_entity, bot->current_target_entity, bot->r.currentOrigin, bot->client->sess.sessionTeam );
	gentity_t *ent = &g_entities[entNum];
	int best_point = -1;
	int second_best_point = -1;
	int third_best_point = -1;
	int loop = 0;
	float best_dist = 99999.9f;

	if (ent)
	{// We have a goal to defend near... Find a spot close to it...
		for (loop = 0; loop < num_snipe_points; loop++)
		{
			if (VectorDistance(snipe_points[best_point], ent->r.currentOrigin) < best_dist)
			{
				third_best_point = second_best_point;
				second_best_point = best_point;
				best_point = loop;
				best_dist = VectorDistance(snipe_points[best_point], bot->r.currentOrigin);
			}
		}
	}
	else
	{
		for (loop = 0; loop < num_snipe_points; loop++)
		{
			if (VectorDistance(snipe_points[best_point], bot->r.currentOrigin) < best_dist)
			{
				third_best_point = second_best_point;
				second_best_point = best_point;
				best_point = loop;
				best_dist = VectorDistance(snipe_points[best_point], bot->r.currentOrigin);
			}
		}
	}

	// Return what we found...
	if (third_best_point >= 0)
	{
		if (Q_TrueRand(0,2) == 2)
		{
			return third_best_point;
		}
		else if (Q_TrueRand(0,2) == 2)
		{
			return second_best_point;
		}
		else
		{
			return best_point;
		}
	}
	else if (second_best_point >= 0)
	{
		if (Q_TrueRand(0,1) == 1)
		{
			return second_best_point;
		}
		else
		{
			return best_point;
		}
	}
	else
	{
		return best_point;
	}*/
	if ( num_snipe_points >= 1 )
	{
		return ( Q_TrueRand( 0, num_snipe_points - 1) );
	}
	else
	{
		return ( -1 );
	}
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Find_Sniper_Spot

// Description  : Find a sniper position.
int
AIMOD_MOVEMENT_Find_Sniper_Spot ( gentity_t *bot )
{

	/*	int best_point = -1;
	int loop = 0;
	float best_height = -64000.0f;
	int entNum = -1;
	int best_target = -1;
	float best_dist = 99999.9f;

	if (bot->bot_snipe_node && bot->bot_snipe_node > 0 && VectorDistance(nodes[bot->bot_snipe_node].origin, bot->r.currentOrigin) < 1024)
	{// Keep heading to our current snipe node!
		VectorCopy(nodes[bot->bot_snipe_node].origin, bot->bot_sniper_spot);

		// Set up this single node in long term goal array...
		memset(bot->longTermGoalNodes, NODE_INVALID, 32);
		bot->longTermGoalNodes[0] = bot->bot_snipe_node;
		bot->longTermGoalNodes_total = 1;

		return bot->bot_snipe_node;
	}

	for (loop = 0; loop < num_snipe_points; loop++)
	{
		if (VectorDistance(snipe_points[best_point], bot->r.currentOrigin) < best_dist 
			&& Q_TrueRand(0,2) == 1 ) // Mix it up a little...
		{
			best_dist = VectorDistance(snipe_points[best_point], bot->r.currentOrigin);
			best_point = loop;
		}
	}

	if (best_point < 0)
	{
		for (loop = 0; loop < num_snipe_points; loop++)
		{// Force it this time to be the very closest...
			if (VectorDistance(snipe_points[best_point], bot->r.currentOrigin) < best_dist )
			{
				best_dist = VectorDistance(snipe_points[best_point], bot->r.currentOrigin);
				best_point = loop;
			}
		}
	}

	for (loop = 0; loop < number_of_nodes; loop++)
	{// Find closest node(s) to the objective...
		float dist = VectorDistance(nodes[loop].origin, snipe_points[best_point]);

		if ( dist < best_dist )
		{
			memset(bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES);

			best_dist = VectorDistance(nodes[loop].origin, snipe_points[best_point]);
			best_target = loop;
		}
	}

	if (best_target > 0) // We found a snipe point...
	{
		//G_Printf("^3BOT DEBUG^5: ^7%s^5 found a snipe node (#^7%i^5).\n", bot->client->pers.netname, best_target);
		VectorCopy(nodes[best_target].origin, bot->bot_sniper_spot);
		//bot->bot_snipe_node = best_target;
		//memset(bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES);

		// Set up this single node in long term goal array...
		//memset(bot->longTermGoalNodes, NODE_INVALID, 32);
		//bot->longTermGoalNodes[0] = best_target;
		//bot->longTermGoalNodes_total = 1;

		return best_target;
	}

	// Didn't find one!
	return AIMOD_MOVEMENT_Defender_Find_Sniper_Spot( bot );*/
	if ( num_snipe_points >= 1 )
	{
		return ( Q_TrueRand( 0, num_snipe_points - 1) );
	}
	else
	{
		return ( -1 );
	}
}

//===========================================================================
// Routine      : Unique_FindGoal
// Description  : Find a new goal entity and node.
extern int	AIMOD_NAVIGATION_FindAbsoluteClosestReachableNode ( gentity_t *bot, int r, int type );
extern int	BotMP_Sniper_Find_Goal_EntityNum
			(
				gentity_t	*bot,
				int			ignoreEnt,
				int			ignoreEnt2,
				vec3_t		current_org,
				int			teamNum
			);


/* */
int
Unique_FindGoal ( gentity_t *bot )
{							// Returns goal waypoint...
	int		entNum = -1;
	int		best_target = -1;
	float	best_dist = 99999.9f;
	bot->bot_sniping = qfalse;
	VectorSet( bot->bot_sniper_spot, 0, 0, 0 );

	// Init the bot constructible marker...
	bot->bot_goal_constructible = qfalse;

	// Init the bot destroyable marker...
	bot->bot_goal_destroyable = qfalse;
	best_target = -1;		// Initialize with each loop...
	best_dist = 99999.9f;	// Initialize with each loop...
	if ( bot->bot_defender )
	{						// Bot Objective Defenders goal finding...
#ifndef __BOT_NO_SNIPE_NODES__
		if
		(
			BG_IsScopedWeapon( bot->client->ps.weapon) ||
			bot->client->ps.weapon == WP_FG42 ||
			bot->client->ps.weapon == WP_K43 ||
			bot->client->ps.weapon == WP_GARAND ||
			bot->client->ps.weapon == WP_PANZERFAUST ||
			bot->client->ps.weapon == WP_FG42SCOPE ||
			bot->client->ps.weapon == WP_K43_SCOPE ||
			bot->client->ps.weapon == WP_GARAND_SCOPE
		)
		{
			entNum = BotMP_Sniper_Find_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
													   bot->client->sess.sessionTeam );
		}

		if ( !entNum )
#endif //__BOT_NO_SNIPE_NODES__

			// Find a standard defence point...
			entNum = BotMP_Defender_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity,
															 bot->r.currentOrigin, bot->client->sess.sessionTeam );
		if ( entNum >= 0 )
		{
			if (g_entities[entNum].num_in_range_nodes > 0)
				best_target = g_entities[entNum].in_range_nodes[0];
			else
				best_target = AIMOD_NAVIGATION_FindClosestReachableNode( &g_entities[entNum], NODE_DENSITY, NODEFIND_ALL,
																	 bot );
		}

		if ( best_target >= 0 )
		{					// Found a goal.. Use it...
			bot->current_target_entity = entNum;
			bot->goalentity = &g_entities[entNum];

			//			G_Printf("%s: found a goal for entity %i (best_target %i)! (current node %i)\n", bot->client->pers.netname, bot->current_node, entNum, best_target);
			// Return the best choice...
			return ( best_target );
		}

		//G_Printf("%s: failed to find a goal for entity %i! (current node %i)\n", bot->client->pers.netname, bot->current_node, entNum);
		// If we didn't find a goal, return -1.
		//return ( -1 );
		//return ( bot->current_node );
	}

	if ( bot->followtarget )
	{
		entNum = bot->followtarget->s.number;
	}
	else if ( bot->movetarget )
	{
		entNum = bot->movetarget->s.number;
	}
	else if ( bot->enemy )
	{
		entNum = bot->enemy->s.number;
	}

#ifdef __NPC__
	if
	(
		bot->client &&
		(
			BG_IsScopedWeapon( bot->client->ps.weapon) ||
			bot->client->ps.weapon == WP_FG42 ||
			bot->client->ps.weapon == WP_K43 ||
			bot->client->ps.weapon == WP_GARAND ||
			bot->client->ps.weapon == WP_PANZERFAUST ||
			bot->client->ps.weapon == WP_FG42SCOPE ||
			bot->client->ps.weapon == WP_K43_SCOPE ||
			bot->client->ps.weapon == WP_GARAND_SCOPE
		)
	)
	{
		entNum = BotMP_Sniper_Find_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												   bot->client->sess.sessionTeam );
	}
	else if
		(
			bot->NPC_client &&
			(
				BG_IsScopedWeapon( bot->NPC_client->ps.weapon) ||
				bot->NPC_client->ps.weapon == WP_FG42 ||
				bot->NPC_client->ps.weapon == WP_K43 ||
				bot->NPC_client->ps.weapon == WP_GARAND ||
				bot->NPC_client->ps.weapon == WP_PANZERFAUST ||
				bot->NPC_client->ps.weapon == WP_FG42SCOPE ||
				bot->NPC_client->ps.weapon == WP_K43_SCOPE ||
				bot->NPC_client->ps.weapon == WP_GARAND_SCOPE
			)
		)
	{
		entNum = BotMP_Sniper_Find_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												   bot->NPC_client->sess.sessionTeam );
	}
	else if ( bot->client && bot->client->sess.playerType == PC_ENGINEER )
	{
		entNum = BotMP_Engineer_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->client->sess.sessionTeam );
	}
	else if ( bot->NPC_client && bot->NPC_client->sess.playerType == PC_ENGINEER )
	{
		entNum = BotMP_Engineer_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->NPC_client->sess.sessionTeam );
	}
	else if
		(
			bot->client &&
			(bot->client->sess.playerType == PC_SOLDIER || bot->client->sess.playerType == PC_COVERTOPS)
		)
	{
		entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														bot->client->sess.sessionTeam );
	}
	else if
		(
			bot->NPC_client &&
			(bot->NPC_client->sess.playerType == PC_SOLDIER || bot->NPC_client->sess.playerType == PC_COVERTOPS)
		)
	{
		entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														bot->NPC_client->sess.sessionTeam );
	}
	else if ( bot->client && bot->client->sess.playerType == PC_FIELDOPS )
	{
		entNum = BotMP_FieldOps_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->client->sess.sessionTeam );
	}
	else if ( bot->NPC_client && bot->NPC_client->sess.playerType == PC_FIELDOPS )
	{
		entNum = BotMP_FieldOps_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->NPC_client->sess.sessionTeam );
	}
	else if ( bot->client && bot->client->sess.playerType == PC_MEDIC )
#ifdef __BOT_AAS__
		entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bot->current_target_entity, bot->r.currentOrigin,
													  bot->client->sess.sessionTeam, &botstates[bot->s.number] );
#else //!__BOT_AAS__
	entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bot->current_target_entity, bot->r.currentOrigin,
												  bot->client->sess.sessionTeam, bot );
#endif //__BOT_AAS__

	//	else if (bot->NPC_client && bot->NPC_client->sess.playerType == PC_MEDIC)
	//		entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bot->current_target_entity, bot->r.currentOrigin, bot->NPC_client->sess.sessionTeam, bot );
	else if ( bot->NPC_client )
	{
		entNum = BotMP_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												bot->NPC_client->sess.sessionTeam );
	}
	else
	{
		entNum = BotMP_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												bot->client->sess.sessionTeam );
	}

#else //!__NPC__
	if
	(
		BG_IsScopedWeapon( bot->client->ps.weapon) ||
		bot->client->ps.weapon == WP_FG42 ||
		bot->client->ps.weapon == WP_K43 ||
		bot->client->ps.weapon == WP_GARAND ||
		bot->client->ps.weapon == WP_PANZERFAUST ||
		bot->client->ps.weapon == WP_FG42SCOPE ||
		bot->client->ps.weapon == WP_K43_SCOPE ||
		bot->client->ps.weapon == WP_GARAND_SCOPE
	)
	{
		entNum = BotMP_Sniper_Find_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												   bot->client->sess.sessionTeam );
	}
	else if ( bot->client->sess.playerType == PC_ENGINEER )
	{
		entNum = BotMP_Engineer_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->client->sess.sessionTeam );
	}
	else if ( bot->client->sess.playerType == PC_SOLDIER || bot->client->sess.playerType == PC_COVERTOPS )
	{
		entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														bot->client->sess.sessionTeam );
	}
	else if ( bot->client->sess.playerType == PC_FIELDOPS )
	{
		entNum = BotMP_FieldOps_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->client->sess.sessionTeam );
	}
	else if ( bot->client->sess.playerType == PC_MEDIC )
#ifdef __BOT_AAS__
		entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bot->current_target_entity, bot->r.currentOrigin,
													  bot->client->sess.sessionTeam, &botstates[bot->s.number] );
#else //!__BOT_AAS__
	entNum = BotMP_Medic_Find_POP_Goal_EntityNum( entNum, bot->current_target_entity, bot->r.currentOrigin,
												  bot->client->sess.sessionTeam, bot );
#endif //__BOT_AAS__
	else
	{
		entNum = BotMP_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												bot->client->sess.sessionTeam );
	}
#endif //__NPC__
	if ( entNum < 0 )
	{
#ifdef __NPC__
		if ( bot->NPC_client )
		{
			entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity,
															bot->r.currentOrigin, bot->NPC_client->sess.sessionTeam );
		}
		else
#endif //__NPC__
			entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity,
															bot->r.currentOrigin, bot->client->sess.sessionTeam );
	}

	if ( entNum >= 0 )
	{
		if (g_entities[entNum].num_in_range_nodes > 0)
			best_target = g_entities[entNum].in_range_nodes[0];
		else
			best_target = AIMOD_NAVIGATION_FindClosestReachableNode( &g_entities[entNum], NODE_DENSITY, NODEFIND_ALL, bot );
	}

	if ( best_target >= 0 )
	{						// Found a goal.. Use it...
		bot->current_target_entity = entNum;
		bot->goalentity = &g_entities[entNum];

		// Return the best choice...
		return ( best_target );
	}

	//	G_Printf("%s: failed to find a goal for entity %i! (current node %i)\n", bot->client->pers.netname, entNum, bot->current_node);
	//	if (entNum > 0)
	//		G_Printf("Entity location %f %f %f.\n", g_entities[entNum].r.currentOrigin[0], g_entities[entNum].r.currentOrigin[1], g_entities[entNum].r.currentOrigin[2]);
	// If we got here, then we failed to find a path to a goal...
	return ( -1 );
}


//===========================================================================
// Routine      : BotGetNextNode
// Description  : if the bot has reached a node, this function selects the next node
//				  that he will go to, and returns it right now it's being developed,

//				  feel free to experiment
short int
BotGetNextNode ( gentity_t *bot /*, int *state*/ )
{
	short int	node = NODE_INVALID;

	//we should never call this in BOTSTATE_MOVE with no goal
	//setup the goal/path in HandleIdleState
	if ( bot->longTermGoal == NODE_INVALID )
	{
		return ( NODE_INVALID );
	}

	if ( bot->pathsize == 0 )					//if the bot is at the end of his path, this shouldn't have been called
	{
		bot->longTermGoal = NODE_INVALID;		//reset to having no goal
		return ( NODE_INVALID );
	}

	node = bot->pathlist[bot->pathsize - 1];	//pathlist is in reverse order
	bot->pathsize--;							//mark that we've moved another node
	return ( node );
}


//===========================================================================
// Routine      : BotEntityCloserOnFuturePath

// Description  : Is an entity closer to one of our current nodes then to us now?
qboolean
BotEntityCloserOnFuturePath ( gentity_t *bot, gentity_t *ent )
{					// Return qtrue if future nodes are closer to "ent" then the current one...
	int		pathsize = bot->pathsize;
	float	current_dist = VectorDistance( ent->r.currentOrigin, nodes[bot->current_node].origin );
	while ( pathsize > 0 )
	{
		if ( !nodes[bot->pathlist[pathsize - 1]].origin )
		{
			break;
		}

		if ( VectorDistance( ent->r.currentOrigin, nodes[bot->pathlist[pathsize - 1]].origin) < current_dist )
		{
			return ( qtrue );
		}

		pathsize--; //mark that we've moved another node
	}

	return ( qfalse );
}


/* */
float
TravelTime ( gentity_t *bot )
{

	/*float dist;
	float travel_time;

	if (bot->current_node)
	{
		dist = VectorDistance(bot->r.currentOrigin, nodes[bot->current_node].origin);
		travel_time = (dist*0.02)*1000;
		//G_Printf("Travel time set to %f (distance %f) for bot %s.\n", travel_time, dist, bot->client->pers.netname);
	}
	else
	{
		travel_time = 10000;
	}

	if (travel_time < 2000)
		travel_time = 2000;
		*/
	float	travel_time = 10000;
	return ( travel_time );
}

//===========================================================================
// Routine      : Bot_Set_New_Path
// Description  : Find a new path.
extern qboolean map_has_sequential_nodes;
extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );
extern vmCvar_t bot_minplayers;
extern vmCvar_t npc_minplayers;


/* */
void
Bot_Set_New_Path ( gentity_t *bot, usercmd_t *ucmd )
{

	// Try to reduce CPU time when theres a lot of bots/npcs in use...
#ifdef __NPC__
	if ( bot->NPC_client )
	{
		if ( npc_minplayers.integer > 192 )
		{
			if ( bot->last_astar_path_think > level.time - 8000 )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				//AIMOD_MOVEMENT_Backup(bot, ucmd);
				return;
			}
		}
		else if ( npc_minplayers.integer > 128 )
		{
			if ( bot->last_astar_path_think > level.time - 5000 )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				//AIMOD_MOVEMENT_Backup(bot, ucmd);
				return;
			}
		}
		else if ( npc_minplayers.integer > 64 )
		{
			if ( bot->last_astar_path_think > level.time - 4000 )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				//AIMOD_MOVEMENT_Backup(bot, ucmd);
				return;
			}
		}
		else if ( npc_minplayers.integer > 32 )
		{
			if ( bot->last_astar_path_think > level.time - 2000 )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				//AIMOD_MOVEMENT_Backup(bot, ucmd);
				return;
			}
		}
		else
		{
			if ( bot->last_astar_path_think > level.time - 1000 )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				//AIMOD_MOVEMENT_Backup(bot, ucmd);
				return;
			}
		}
	}
	else
#endif //__NPC__
	{
		if ( bot_minplayers.integer > 48 )
		{
			if ( bot->last_astar_path_think > level.time - 5000 )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				//AIMOD_MOVEMENT_Backup(bot, ucmd);
				return;
			}
		}
		else if ( bot_minplayers.integer > 32 )
		{
			if ( bot->last_astar_path_think > level.time - 4000 )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				//AIMOD_MOVEMENT_Backup(bot, ucmd);
				return;
			}
		}
		else if ( bot_minplayers.integer > 16 )
		{
			if ( bot->last_astar_path_think > level.time - 3000 )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				//AIMOD_MOVEMENT_Backup(bot, ucmd);
				return;
			}
		}
		else if ( bot_minplayers.integer > 8 )
		{
			if ( bot->last_astar_path_think > level.time - 2000 )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				//AIMOD_MOVEMENT_Backup(bot, ucmd);
				return;
			}
		}
		else
		{
			if ( bot->last_astar_path_think > level.time - 500 )
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;

				//AIMOD_MOVEMENT_Backup(bot, ucmd);
				return;
			}
		}
	}

	bot->last_astar_path_think = level.time;

	// Initialize...
	bot->shortTermGoal = -1;
#ifdef _DEBUG
	if ( bot->bot_last_good_move_time < level.time - 2000 )
	{

		//G_Printf("%s: Bot_Set_New_Path() called because of bot_last_good_move_time.\n", bot->client->pers.netname);
	}
	else if ( bot->node_timeout < 0 )
	{

		//G_Printf("%s: Bot_Set_New_Path() called because of node_timeout.\n", bot->client->pers.netname);
	}
	else if ( bot->current_node < 0 )
	{

		//G_Printf("%s: Bot_Set_New_Path() called because of bot->current_node < 0.\n", bot->client->pers.netname);
	}
	else if ( bot->longTermGoal < 0 )
	{

		//G_Printf("%s: Bot_Set_New_Path() called because of bot->longTermGoal < 0.\n", bot->client->pers.netname);
	}
	else if ( bot->next_node < 0 )
	{

		//G_Printf("%s: Bot_Set_New_Path() called because of bot->next_node < 0.\n", bot->client->pers.netname);
	}

	//	else if (!AIMOD_MOVEMENT_CurrentNodeReachable( bot ))
	//	{
	//		//G_Printf("%s: Bot_Set_New_Path() called because of !AIMOD_MOVEMENT_CurrentNodeReachable().\n", bot->client->pers.netname);
	//	}
	else if ( BAD_WP_Height( nodes[bot->current_node].origin, bot->r.currentOrigin) )
	{

		//G_Printf("%s: Bot_Set_New_Path() called because of BAD_WP_Height().\n", bot->client->pers.netname);
	}
	else
	{

		//G_Printf("%s: Bot_Set_New_Path() called because UNKNOWN!\n", bot->client->pers.netname);
		bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
	}

#else
	if
	(
		bot->bot_last_good_move_time < level.time - 2000 ||
		bot->node_timeout < 0 ||
		bot->current_node < 0 ||
		bot->longTermGoal < 0 ||
		bot->next_node < 0 ||
		!AIMOD_MOVEMENT_CurrentNodeReachable( bot)		/*|| BAD_WP_Height(nodes[bot->current_node].origin, bot->r.currentOrigin)*/
	)
	{

	}
	else
	{

		//G_Printf("%s: Bot_Set_New_Path() called because UNKNOWN!\n", bot->client->pers.netname);
		bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
	}
#endif //_DEBUG

	/*	if (bot->current_node && bot->longTermGoal && VectorDistance(bot->r.currentOrigin, nodes[bot->longTermGoal].origin) > 128)
	{// See if we can continue on our current goal first!
		memset(bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES);
		bot->pathsize = 0;
		bot->last_node = bot->current_node-1;
		bot->pathsize = CreatePathAStar(bot, bot->current_node, bot->longTermGoal, bot->pathlist);
		bot->next_node = BotGetNextNode(bot);
		bot->node_timeout = level.time + TravelTime( bot );

		if (bot->pathsize > 0)
			return; // We can!
	}*/
	memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
	bot->pathsize = 0;
	if ( bot->current_node < 0 )
	{													//bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNodes( bot, NODE_DENSITY, NODEFIND_ALL );
		bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
	}

	if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
	{													// Try to roam out of problems...
		bot->goalentity = NULL;
		bot->longTermGoal = Unique_FindGoal( bot );
	}
	else
	{													// Work out why we got here, and pick another goal based on the reason...
		if ( VectorDistance( nodes[bot->longTermGoal].origin, bot->r.currentOrigin) < 32 )
		{												// We found our current goal. Select another...
			bot->goalentity = NULL;
			bot->longTermGoal = Unique_FindGoal( bot );
		}
		else if
			(
				(bot->goalentity && bot->goalentity->client) &&
				(bot->goalentity->health <= 0 || (bot->goalentity->client->ps.pm_flags & PMF_LIMBO))
			)
		{												// If our goal entity is a player/bot, and they are dead... Select another goal...
			bot->goalentity = NULL;
			bot->longTermGoal = Unique_FindGoal( bot );
		}
		else if ( bot->goalentity && bot->goalentity->client && bot->goalentity->health > 0 )
		{												// If our goal entity is a player/bot, continue heading toward them...
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( bot->goalentity, NODE_DENSITY, NODEFIND_ALL,
																		   bot );
		}
	}

	// So we can offset thinking for fps...
	bot->bot_route_think_delay = level.time + 5000;
	if ( bot->longTermGoal < 0 )
	{													// Find an enemy to head for...
		int best_ent = Find_Goal_Enemy( bot );
		if ( best_ent >= 0 )
		{
			bot->goalentity = &g_entities[best_ent];
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( bot->goalentity->r.currentOrigin,
																			 NODE_DENSITY, NODEFIND_ALL );
		}
	}

	if ( bot->longTermGoal < 0 )
	{													// Just get us out of trouble...
		bot->goalentity = NULL;
		bot->longTermGoal = bot->current_node + 5;

		//G_Printf("%s: failed to find a goal! (current node %i)\n", bot->client->pers.netname, bot->current_node);
	}

	bot->last_node = bot->current_node - 1;
	bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
	bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
	bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Shorten_Path

// Description  : Quickly shorten the current path... Skip any extra nodes we can...
void
AIMOD_MOVEMENT_Shorten_Path ( gentity_t *bot )
{

	//#ifdef __BOT_SHORTEN_ROUTING__
	int pathsize = bot->pathsize;
	int original_size = bot->pathsize;
	int link;
	if ( !bot->current_node || !bot->next_node || !bot->longTermGoal || pathsize < 5 )
	{
		return; // No point shortening this path...
	}

	if ( bot->bot_wanderring )
	{
		return; // Don't shorten wanderring bot's paths...
	}

	// Don't do this continually on the same node...
	if ( bot->last_shorten_path_node == bot->current_node )
	{
		return;
	}

	//bot->last_shorten_path_node = bot->current_node;
	while ( pathsize > 0 )
	{
		if ( !nodes[bot->pathlist[pathsize - 1]].origin )
		{
			break;
		}

		for ( link = 0; link < nodes[bot->current_node].enodenum; link++ )
		{
			if ( !nodes[bot->current_node].links[link].targetNode )
			{
				break;
			}

			if ( nodes[bot->current_node].links[link].flags & PATH_BLOCKED )
			{	//if this path is blocked, skip it
				continue;
			}

			if ( nodes[bot->current_node].links[link].flags & PATH_DANGER )
			{	//if this path is dangerous, skip it
				continue;
			}

#ifdef __VEHICLES__
			if
			(
				bot->client &&
				(bot->client->ps.eFlags & EF_TANK) &&
				nodes[bot->current_node].links[link].flags & PATH_NOTANKS
			)
			{	//if this path is blocked, skip it
				continue;
			}
#endif //__VEHICLES__

			if ( nodes[bot->current_node].links[link].targetNode == bot->pathlist[pathsize - 1] )
			{

				//if (NodeVisible(bot->r.currentOrigin, nodes[nodes[bot->current_node].links[link].targetNode].origin, bot->s.number) >= 1)
				{
					bot->pathsize = pathsize - 1;
					bot->next_node = bot->pathlist[bot->pathsize - 1];	//pathlist is in reverse order

					//G_Printf("%s's path was shortened from %i nodes to %i nodes.\n", bot->client->pers.netname, original_size, bot->pathsize);
					// Path has been shortened...
					return;
				}
			}
		}

		pathsize--;														//mark that we've moved another node
	}

	//#endif //__BOT_SHORTEN_ROUTING__
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_ENG_ReadyToDynamite

// Description  : Is this engineer ready to drop some dynamite???
qboolean
AIMOD_MOVEMENT_ENG_ReadyToDynamite ( gentity_t *bot )
{
	usercmd_t	*ucmd = &bot->client->pers.cmd;
	if
	(
		bot->bot_goal_destroyable &&
		bot->bot_goal_entity &&
		VectorDistance( bot->bot_goal_position, bot->r.currentOrigin) < 256
	)
	{
		vec3_t	origin, mins, maxs;
		int		touch[MAX_GENTITIES];
		int		numListedEntities;
		int		i;
		VectorCopy( bot->bot_goal_entity->r.currentOrigin, origin );
		VectorAdd( origin, bot->bot_goal_entity->r.mins, mins );
		VectorAdd( origin, bot->bot_goal_entity->r.maxs, maxs );
		numListedEntities = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
		for ( i = 0; i < numListedEntities; i++ )
		{
			gentity_t	*hit = &g_entities[touch[i]];
			if ( hit == bot )
			{
				gentity_t	*dynamite = Dynamite_Entity( bot );
				if ( !dynamite && next_dynamite_test[bot->s.number] <= level.time )
				{
					vec3_t	angles;
					bot->beStillEng = level.time + 10000;
					bot->client->pers.cmd.weapon = WP_DYNAMITE;
					if ( next_satchel_available_time[bot->s.clientNum] < level.time )
					{

						//weapon_grenadelauncher_fire(bot, bot->client->ps.weapon);
						next_dynamite_test[bot->s.number] = level.time + ( 1000 / (bot->skillchoice + 1) );
						ucmd->buttons |= BUTTON_ATTACK;
						next_satchel_available_time[bot->s.clientNum] = level.time + 10000;
					}

					// Head for our object!
					VectorSubtract( bot->bot_goal_entity->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
					vectoangles( bot->move_vector, angles );
					VectorCopy( angles, bot->s.angles );
					if ( bot->bot_last_chat_time < level.time )
					{
						bot->bot_last_chat_time = level.time + 10000;
						G_Voice( bot, NULL, SAY_TEAM, va( "CoverMe"), qfalse );
					}
				}

				return ( qtrue );
			}
		}
	}

	return ( qfalse );
}

#ifdef __ETE__
vmCvar_t	g_realmedic;
vmCvar_t	g_noSelfHeal;
#endif //__ETE__

//===========================================================================
// Routine      : AIMOD_MOVEMENT_SpecialMoves

// Description  : Bot Special Movements (medic, fldops, cvt, etc...)
qboolean
AIMOD_MOVEMENT_SpecialMoves ( gentity_t *bot, usercmd_t *ucmd )
{
	vec3_t /*angles,*/ eyes;
	if ( !bot->skillchoice )
	{
		bot->skillchoice = rand() % 5;
	}

	if ( no_mount_time[bot->s.clientNum] > level.time )
	{
		bot->beStillEng = 0;
		bot->beStill = 0;
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{																								// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{																								// Release the emplaced gun we are on to do our job...
			//mg42_stopusing( bot );
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}
	}

	// Special Movements.
	VectorCopy( bot->r.currentOrigin, eyes );
	eyes[2] += 48;
	G_BotCheckForCursorHints( bot );
	if ( bot->client->ps.weapon == WP_DYNAMITE && next_dynamite_test[bot->s.number] <= level.time )
	{																									// Make sure we plant!
		gentity_t	*dynamite = Dynamite_Entity( bot );
		if ( dynamite == NULL )
		{
			next_dynamite_test[bot->s.number] = level.time + ( 1000 / (bot->skillchoice + 1) );
			ucmd->buttons |= BUTTON_ATTACK;
			bot->needed_node = -1;

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
			return ( qtrue );
		}
	}

	// Check for any generic ingame sounds we might need...
	Check_VoiceChats( bot );
	if
	(
		(bot->beStillEng > level.time || AIMOD_MOVEMENT_ENG_ReadyToDynamite( bot)) &&
		(bot->client->sess.playerType == PC_ENGINEER)
	)
	{																									// Got some building to do!
		gentity_t	*dynamite = Dynamite_Entity( bot );
		if ( dynamite != NULL )
		{																								// Dynamite Found. Head toward it and help plant.
			if ( dynamite->s.teamNum >= 4 )
			{																							// Dynamite not planted.
				bot->beStillEng = level.time + 2000;
				bot->beStill = level.time + 2000;
				if ( bot->client->ps.weapon != WP_PLIERS )
				{		
					if (bot->client->ps.weapon != WP_PLIERS)	
					{// Switch to plyers and use them!
						ucmd->weapon = WP_PLIERS;
						next_weapon_change[bot->s.clientNum] = level.time + 3000;
					}

					bot->needed_node = -1;
					if ( !(bot->client->ps.eFlags & EF_CROUCHING) )
					{

						//ucmd->upmove = -127; // Crouch?
						bot->client->ps.eFlags |= EF_CROUCHING;
						ucmd->upmove = -48;																// Crouch...
						bot->client->ps.pm_flags |= PMF_DUCKED;
					}

					// Set our last good move time to now...
					bot->bot_last_good_move_time = level.time;
					return ( qtrue );
				}

				if ( VectorDistance( dynamite->r.currentOrigin, bot->r.currentOrigin) > 48 )
				{
					ucmd->forwardmove = 120;
				}

				if ( !(bot->client->ps.eFlags & EF_CROUCHING) )
				{

					//ucmd->upmove = -127; // Crouch?
					bot->client->ps.eFlags |= EF_CROUCHING;
					ucmd->upmove = -48;																	// Crouch...
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}

				VectorSubtract( dynamite->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );

				//vectoangles( bot->move_vector, angles );
				//G_SetAngle( bot, angles);
				//VectorCopy( angles, bot->s.angles );	
				bot->move_vector[ROLL] = 0;
				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
				BotSetViewAngles( bot, 100 );

				//G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
				if ( bot->client->ps.weapon == WP_PLIERS )
				{
					ucmd->buttons |= BUTTON_ATTACK;
				}

				bot->needed_node = -1;

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;
				return ( qtrue );
			}
			else
			{																							// Planted already. Get away from here!
				next_dynamite_test[bot->s.number] = level.time + 2000;
				bot->client->ps.eFlags &= ~EF_CROUCHING;
				G_Voice( bot, NULL, SAY_TEAM, va( "FireInTheHole"), qfalse );
				bot->beStillEng = 0;
				bot->beStill = 0;
				if ( NodesLoaded() == qtrue )
				{
					bot->beStillEng = 0;
					if ( runaway_node[bot->s.clientNum] == qfalse )
					{
						Bot_Set_New_Path( bot, ucmd );
					}

					if ( bot->next_node != -1 )
					{
						VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );

						//vectoangles( bot->move_vector, angles );
						//VectorCopy( angles, bot->s.angles );
						bot->move_vector[ROLL] = 0;
						vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
						BotSetViewAngles( bot, 100 );
						G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
						VectorCopy( bot->r.currentOrigin, last_vector[bot->s.clientNum] );

						//ucmd->forwardmove = 120;
					}
					else
					{
						ucmd->forwardmove = -120;
					}
				}
				else
				{
					ucmd->forwardmove = -120;
				}

				if ( bot->client->ps.eFlags & EF_CROUCHING )
				{																						// We're still crouching.. Get up.
					ucmd->upmove = 127;																	// Jump up from crouch.
					bot->client->ps.eFlags &= ~EF_CROUCHING;
				}

				Set_Best_AI_weapon( bot );
				bot->needed_node = -1;

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;
				return ( qtrue );
			}
		}
		else
		{																								// No dynamite around. Use pliers to build something.
			if ( bot->bot_can_build )
			{
				if ( !(bot->client->ps.eFlags & EF_CROUCHING) )
				{
					ucmd->upmove = -127;																// Crouch?
					bot->client->ps.eFlags |= EF_CROUCHING;
				}

				if (bot->client->ps.weapon != WP_PLIERS)
				{
					ucmd->weapon = WP_PLIERS;
					next_weapon_change[bot->s.clientNum] = level.time + 3000;
				}

				ucmd->buttons |= BUTTON_ATTACK;
				bot->needed_node = -1;

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;
				return ( qtrue );
			}
			else
			{																							// Must have finished building... Init our goal and path...
				bot->longTermGoal = -1;
				bot->beStillEng = 0;
				Set_Best_AI_weapon( bot );
				return ( qfalse );
			}
		}
	}
	else
	{																									// Non-Engineers.
	}

	runaway_node[bot->s.clientNum] = qfalse;															// Initialize it!
	if ( (bot->client->ps.eFlags & EF_MOUNTEDTANK || bot->client->ps.eFlags & EF_AAGUN_ACTIVE) && bot->tankLink )
	{																									// Force no movement on emplaced/tank gun users... They stay on their gun until death!
		//bot->beStill = level.time + 2000;
		if ( next_turret_angle_change[bot->s.clientNum] < level.time )
		{																								// Now set angles randomly.
			bot->s.angles[YAW] += 5;																	//ang_mod;
			next_turret_angle_change[bot->s.clientNum] = level.time + 40;
		}

		VectorCopy( bot->tankLink->r.currentOrigin, bot->r.currentOrigin );
		bot->r.currentOrigin[2] += 96;
		VectorCopy( bot->tankLink->r.currentOrigin, bot->client->ps.origin );
		bot->client->ps.origin[2] += 96;
		if ( bot->beStill < level.time - 1000 )
		{
			bot->beStill = level.time + 20000;															// 20 secs on mover.
		}
		else if ( bot->beStill <= level.time && !bot->enemy )
		{
			G_LeaveTank( bot, qtrue );
		}

		bot->needed_node = -1;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}
	else if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
	{																									// Force no movement on emplaced/tank gun users... They stay on their gun until death!
		int ang_mod = rand() % 10;
		int ang_dir = rand() % 10;
		bot->beStill = level.time + 2000;
		if ( next_turret_angle_change[bot->s.clientNum] < level.time )
		{																								// Now set angles randomly.
			if ( ang_dir > 4 )
			{
				bot->s.angles[YAW] += ang_mod;
			}
			else
			{
				bot->s.angles[YAW] -= ang_mod;
			}

			next_turret_angle_change[bot->s.clientNum] = level.time + 40;
		}

		bot->needed_node = -1;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}

	if ( bot->beStill > level.time )
	{																									// Got something to do! Turret or tank gun?
		bot->needed_node = -1;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}

	if ( bot->bot_wait_for_supplies > level.time )
	{																									// Wait for medic/ammo packs...
		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}

	//
	// Now the actual JobTypes code...
	//
	if ( bot->movetarget && bot->movetarget->client )
	{
		if ( bot->movetarget->client->ps.pm_flags & PMF_LIMBO )
		{																								// Dead, doesn't need any attention. Reset job.
			if ( bot->movetarget )
			{
				AIMOD_Player_Special_Needs_Clear( bot->movetarget->s.clientNum );
			}

			AI_ResetJob( bot );
			bot->movetarget = NULL;
			Set_Best_AI_weapon( bot );
			return ( qfalse );
		}
	}

	if
	(
		JobType( bot) == BOT_JOB_NONE &&
		(bot->bot_goal_constructible || bot->bot_goal_destroyable) &&
		bot->current_target_entity >= 0 &&
		bot->longTermGoal != -1 &&
		VectorDistance( bot->r.currentOrigin, nodes[bot->longTermGoal].origin) < 96
	)
	{																									// If we have something to construct at this goal, then head directly toward it when we get close enough (until the bot auto-builds)..
		if (bot->client->ps.weapon != WP_PLIERS)
		{
			ucmd->weapon = WP_PLIERS;
			next_weapon_change[bot->s.clientNum] = level.time + 3000;
		}
	}
	else if
		(
			JobType( bot) == BOT_JOB_NONE &&
			bot->current_target_entity >= 0 &&
			bot->longTermGoal &&
			bot->longTermGoal < number_of_nodes &&
			VectorDistance( bot->r.currentOrigin, nodes[bot->longTermGoal].origin) < 96
		)
	{																									// At our target point... Touch it!
		if ( VectorDistance( bot->r.currentOrigin, nodes[bot->longTermGoal].origin) < 24 )
		{																								// Close enough.. Find a new goal...
			Set_Best_AI_weapon( bot );
			bot->longTermGoal = -1;
			return ( qfalse );
		}
		else
		{
			VectorSubtract( nodes[bot->longTermGoal].origin, bot->r.currentOrigin, bot->movedir );

			//vectoangles( bot->movedir, angles );
			//VectorCopy( angles, bot->s.angles );
			bot->movedir[ROLL] = 0;
			vectoangles( bot->movedir, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			G_UcmdMoveForDir( bot, ucmd, bot->movedir );

			//ucmd->forwardmove = 120;
		}

		return ( qtrue );
	}

	if ( JobType( bot) == BOT_JOB_NONE )
	{																									// Reset job.
		if ( bot->movetarget )
		{
			AIMOD_Player_Special_Needs_Clear( bot->movetarget->s.clientNum );
		}

		AI_ResetJob( bot );
		bot->movetarget = NULL;
		Set_Best_AI_weapon( bot );
		return ( qfalse );
	}

	if ( bot->movetarget && bot->movetarget->client && MyVisible( bot, bot->movetarget) )
	{
		if ( JobType( bot) == BOT_JOB_SYRINGE && bot->movetarget->health > 0 )
		{																								// Revive Job completed... Switch to medkit...
			ucmd->forwardmove = -120;																	// Step back.
			NowHeal( bot );																				// Will switch jobtype to medkit.
			bot->needed_node = -1;

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
		}

		if
		(
			JobType( bot) == BOT_JOB_SYRINGE &&
			VectorDistance( bot->movetarget->r.currentOrigin, bot->r.currentOrigin) < 48
		)
		{																								// Syringe.. We're there... Crouch and syringe them!
			vec3_t	eyes;
			BOT_GetMuzzlePoint( bot );
			VectorCopy( bot->bot_muzzlepoint, eyes );

			if (bot->client->ps.weapon != WP_MEDIC_SYRINGE)
			{
				ucmd->weapon = WP_MEDIC_SYRINGE;
				next_weapon_change[bot->s.clientNum] = level.time + 3000;
			}

			ucmd->upmove = -48;																			// Crouch...
			bot->client->ps.pm_flags |= PMF_DUCKED;
			VectorSubtract( bot->movetarget->r.currentOrigin, eyes, bot->move_vector );
			bot->move_vector[ROLL] = -90;
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			if ( bot->client->ps.weapon == WP_MEDIC_SYRINGE && !g_realmedic.integer )
			{																							// jaquboss, support for realmedic
				ucmd->buttons |= BUTTON_ATTACK;
			}

			if ( g_realmedic.integer )
			{
				ucmd->buttons |= BUTTON_ACTIVATE;														// this is correct
			}

			bot->needed_node = -1;

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
			return ( qtrue );
		}

		if ( JobType( bot) == BOT_JOB_MEDKIT && bot->movetarget->health > 90 )
		{																								// Healed already!... Give up on him.
			AIMOD_Player_Special_Needs_Clear( bot->movetarget->s.clientNum );
			AI_ResetJob( bot );
			bot->movetarget = NULL;
			bot->needed_node = -1;

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
			Set_Best_AI_weapon( bot );
			return ( qfalse );
		}

		if
		(
			JobType( bot) == BOT_JOB_AMMO &&
			((bot->movetarget->s.eFlags & EF_DEAD && bot->movetarget) || !ClientOutOfAmmo( bot->movetarget->s.clientNum))
		)
		{																								// Dead, doesn't need any ammo.
			AIMOD_Player_Special_Needs_Clear( bot->movetarget->s.clientNum );
			AI_ResetJob( bot );
			bot->movetarget = NULL;
			bot->needed_node = -1;

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
			Set_Best_AI_weapon( bot );
			return ( qfalse );
		}

		if ( VectorLength( bot->client->ps.velocity) < 24 )
		{																								// Jump over something in the way to our target!
			ucmd->upmove = 127;
		}

		if ( JobType( bot) == BOT_JOB_MEDKIT || JobType( bot) == BOT_JOB_AMMO )
		{																								// MedKit or Ammo.
			if ( VectorDistance( bot->movetarget->r.currentOrigin, bot->r.currentOrigin) < 64 /*80*/ )	// We're there... Give them AMMO or MEDKIT!
			{							// Need to be further away to throw packs.
				VectorSubtract( bot->movetarget->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );

				// Set up ideal view angles for this target...
				bot->move_vector[ROLL] = 0;
				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
				BotSetViewAngles( bot, 100 );
				if ( JobType( bot) == BOT_JOB_MEDKIT )
				{

					//bot->s.weapon = bot->client->pers.cmd.weapon = WP_MEDKIT;
					if (bot->client->ps.weapon != WP_MEDKIT)
					{
						ucmd->weapon = WP_MEDKIT;
						next_weapon_change[bot->s.clientNum] = level.time + 3000;
					}
				}
				else
				{

					//bot->s.weapon = bot->client->pers.cmd.weapon = WP_AMMO;
					if (bot->client->ps.weapon != WP_AMMO)
					{
						ucmd->weapon = WP_AMMO;
						next_weapon_change[bot->s.clientNum] = level.time + 2000;
					}
				}

				//ucmd->forwardmove = -64; // Step back.
				ucmd->upmove = -127;	// Crouch?
				if ( bot->movetarget->r.svFlags & SVF_BOT )
				{						// Tell our target to wait for packs...
					bot->movetarget->bot_wait_for_supplies = level.time + 2000;
				}

				if ( !next_drop[bot->s.number] )
				{
					next_drop[bot->s.number] = level.time - 1;
				}

				if
				(
					next_drop[bot->s.number] < level.time &&
					(bot->client->ps.weapon == WP_AMMO || bot->client->ps.weapon == WP_MEDKIT)
				)
				{
					next_drop[bot->s.number] = level.time + 800;
					if
					(
						(bot->client->ps.weapon == WP_MEDKIT && !g_realmedic.integer) ||
						bot->client->ps.weapon == WP_AMMO
					)
					{
						ucmd->buttons |= BUTTON_ATTACK;
					}

					if ( bot->client->ps.weapon == WP_MEDKIT && g_realmedic.integer )
					{
						ucmd->buttons |= BUTTON_ACTIVATE;
					}
				}

				bot->needed_node = -1;

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;
				return ( qtrue );
			}
		}

		VectorSubtract( bot->movetarget->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
		VectorCopy( bot->r.currentOrigin, last_vector[bot->s.clientNum] );
		bot->needed_node = -1;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}

	if ( bot->followtarget && VectorDistance( bot->followtarget->r.currentOrigin, bot->r.currentOrigin) < 128 )
	{

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return ( qtrue );				// Close enough, don't move!
	}
	else if ( bot->followtarget && MyVisible( bot, bot->followtarget) )
	{
		if ( VectorLength( bot->client->ps.velocity) < 24 )
		{								// Jump over something in the way to our target!
			ucmd->upmove = 127;
		}

		VectorSubtract( bot->followtarget->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		VectorCopy( bot->r.currentOrigin, last_vector[bot->s.clientNum] );
		ucmd->forwardmove = 120;
		bot->needed_node = -1;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}

	if ( bot->beStillEng > level.time && bot->client->ps.weapon == WP_PLIERS )
	{
		bot->needed_node = -1;

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		ucmd->buttons |= BUTTON_ATTACK;
		return ( qtrue );
	}

	// Need to continue normal movememnt... So return qfalse.
	return ( qfalse );
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Setup_Strafe

// Description  : If we need to strafe, then set it up with this!
void
AIMOD_MOVEMENT_Setup_Strafe ( gentity_t *bot, usercmd_t *ucmd )
{
	vec3_t	forward, right, up, rightOrigin, leftOrigin, temp;
	trace_t tr;
#ifdef __NPC__
	if ( bot->client )
	{
		AngleVectors( bot->client->ps.viewangles, forward, right, up );
		VectorMA( bot->r.currentOrigin, 64, right, rightOrigin );
		VectorMA( bot->r.currentOrigin, -64, right, leftOrigin );
	}
	else if ( bot->NPC_client )
	{
		AngleVectors( bot->NPC_client->ps.viewangles, forward, right, up );
		VectorMA( bot->r.currentOrigin, 64, right, rightOrigin );
		VectorMA( bot->r.currentOrigin, -64, right, leftOrigin );
	}
	else
	{
		AngleVectors( bot->s.angles, forward, right, up );
		VectorMA( bot->r.currentOrigin, 64, right, rightOrigin );
		VectorMA( bot->r.currentOrigin, -64, right, leftOrigin );
	}

#else //!__NPC__
	AngleVectors( bot->client->ps.viewangles, forward, right, up );
	VectorMA( bot->r.currentOrigin, 64, right, rightOrigin );
	VectorMA( bot->r.currentOrigin, -64, right, leftOrigin );
#endif //__NPC__
	if ( NodeVisible( rightOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		VectorCopy( rightOrigin, bot->bot_strafe_target_position );
		bot->bot_strafe_right_timer = level.time + 101;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 64;
		ucmd->upmove = 0;
	}
	else if ( NodeVisible( leftOrigin, nodes[bot->current_node].origin, bot->s.number) )
	{
		VectorCopy( leftOrigin, bot->bot_strafe_target_position );
		bot->bot_strafe_left_timer = level.time + 100;
		ucmd->forwardmove = 0;
		ucmd->rightmove = -64;
		ucmd->upmove = 0;
	}
	else
	{
		ucmd->upmove = 127;
		ucmd->forwardmove = 127;
	}

	// Prepare to set another player/bot's (our blocker) strafe dir...
	VectorCopy( bot->r.currentOrigin, temp );
	temp[0] += forward[0] * 64;
	temp[1] += forward[1] * 64;
	temp[2] += forward[2] * 64;
	trap_Trace( &tr, bot->r.currentOrigin, NULL, NULL, temp, -1, MASK_SHOT | MASK_OPAQUE );
#ifdef __NPC__
	if ( tr.entityNum && (g_entities[tr.entityNum].r.svFlags & SVF_BOT || g_entities[tr.entityNum].s.eType == ET_NPC) )
#else //!__NPC__
		if ( tr.entityNum && (tr.entityNum < MAX_CLIENTS && g_entities[tr.entityNum].r.svFlags & SVF_BOT) )
#endif //__NPC__
		{	// A bot is stopping us... Tell them to strafe other direction to us...
#ifdef __NPC__
			if ( g_entities[tr.entityNum].client && bot->bot_strafe_left_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
				VectorCopy( leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}
			else if ( g_entities[tr.entityNum].NPC_client && bot->bot_strafe_left_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].NPC_client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
				VectorCopy( leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}
			else if ( g_entities[tr.entityNum].client && bot->bot_strafe_right_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
				VectorCopy( rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}
			else if ( g_entities[tr.entityNum].NPC_client && bot->bot_strafe_right_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].NPC_client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
				VectorCopy( rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}

#else //!__NPC__
			if ( bot->bot_strafe_left_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
				VectorCopy( leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}
			else if ( bot->bot_strafe_right_timer >= level.time )
			{
				g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;
				AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
				VectorCopy( rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
			}
#endif //__NPC__
		}
}

extern float	VectorDistanceNoHeight ( vec3_t v1, vec3_t v2 );
extern float	HeightDistance ( vec3_t v1, vec3_t v2 );
extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );
extern qboolean HaveSequentialNodes ( void );


/* */
int
AIMOD_Movement_SniperClosestEnemy ( gentity_t *bot )
{
	int		client;
	float	closest = 99999.0f;
	int		closest_enemy = -1;
	for ( client = 0; client < MAX_CLIENTS; client++ )
	{
		gentity_t	*ent = &g_entities[client];
		if ( !ent )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}

		if ( VectorDistance( bot->r.currentOrigin, ent->r.currentOrigin) > closest )
		{
			continue;
		}

		if ( OnSameTeam( bot, ent) )
		{
			continue;
		}

		closest = VectorDistance( bot->r.currentOrigin, ent->r.currentOrigin );
		closest_enemy = client;
	}

	return ( closest_enemy );
}

int test_last_message[MAX_CLIENTS];


/* */
void
AIMOD_Movement_SniperMove ( gentity_t *bot, usercmd_t *ucmd )
{
/*	if
	(
		bot->weaponchoice > WP_KNIFE &&
		bot->client->ps.weapon != bot->weaponchoice &&
		bot->client->ps.weapon != WP_FG42SCOPE &&
		bot->client->ps.weapon != WP_K43_SCOPE &&
		bot->client->ps.weapon != WP_GARAND_SCOPE &&
		AIMOD_HaveAmmoForWeapon( &bot->client->ps, bot->weaponchoice)
	)
	{
		if (bot->client->ps.weapon != bot->weaponchoice)
		{
			ucmd->weapon = bot->weaponchoice;
			next_weapon_change[bot->s.clientNum] = level.time + 3000;
		}

		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if ( bot->client->ps.weapon == WP_FG42 )
	{
		if (bot->client->ps.weapon != WP_FG42SCOPE)
		{
			ucmd->weapon = WP_FG42SCOPE;
			next_weapon_change[bot->s.clientNum] = level.time + 3000;
		}

		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}
	else if ( bot->client->ps.weapon == WP_K43 )
	{
		if (bot->client->ps.weapon != WP_K43_SCOPE)
		{
			ucmd->weapon = WP_K43_SCOPE;
			next_weapon_change[bot->s.clientNum] = level.time + 3000;
		}

		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}
	else if ( bot->client->ps.weapon == WP_GARAND )
	{
		if (bot->client->ps.weapon != WP_GARAND_SCOPE)
		{
			ucmd->weapon = WP_GARAND_SCOPE;
			next_weapon_change[bot->s.clientNum] = level.time + 3000;
		}

		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	// No more ammo, but we can reload, so RELOAD !
	if
	(
		bot->client->ps.weapon == WP_FG42SCOPE ||
		bot->client->ps.weapon == WP_K43_SCOPE ||
		bot->client->ps.weapon == WP_GARAND_SCOPE
	)
	{
		if ( bot->client->ps.ammoclip[bot->client->ps.weapon] == 0 && bot->client->ps.ammo[bot->client->ps.weapon] )
		{														// Need an instant change here!
			if ( bot->client->ps.weapon == WP_FG42SCOPE )
			{
				if (bot->client->ps.weapon != WP_FG42)
				{
					ucmd->weapon = WP_FG42;
					next_weapon_change[bot->s.clientNum] = level.time + 3000;
				}
			}

			if ( bot->client->ps.weapon == WP_K43_SCOPE )
			{
				if (bot->client->ps.weapon != WP_K43)
				{
					ucmd->weapon = WP_K43;
					next_weapon_change[bot->s.clientNum] = level.time + 3000;
				}
			}

			if ( bot->client->ps.weapon == WP_GARAND_SCOPE )
			{
				if (bot->client->ps.weapon != WP_GARAND)
				{
					ucmd->weapon = WP_GARAND;
					next_weapon_change[bot->s.clientNum] = level.time + 3000;
				}
			}

			ucmd->wbuttons = WBUTTON_RELOAD;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;

			if (bot->bot_coverspot_time > level.time)
			{
				bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
				bot->client->ps.pm_flags |= PMF_DUCKED;
				ucmd->upmove = -127;
			}
			return;
		}
	}
*/
	/*if (VectorDistance(bot->goalentity->r.currentOrigin, bot->r.currentOrigin) > 24)
	{
		
#ifdef __BOT_STRAFE__
		if (bot->bot_strafe_right_timer >= level.time)
		{
			if (VectorDistance(bot->bot_strafe_target_position, bot->r.currentOrigin) > 8)
			{
				ucmd->forwardmove = 0;//64;
				ucmd->upmove = 0;
				ucmd->rightmove = 64;
				return;
			}
			else
			{
				bot->bot_strafe_right_timer = 0;
				ucmd->upmove = 127;
			}
		}

		if (bot->bot_strafe_left_timer >= level.time)
		{
			if (VectorDistance(bot->bot_strafe_target_position, bot->r.currentOrigin) > 8)
			{
				ucmd->forwardmove = 0;//64;
				ucmd->upmove = 0;
				ucmd->rightmove = -64;
				return;
			}
			else
			{
				bot->bot_strafe_left_timer = 0;
				ucmd->upmove = 127;
			}
		}

		if (bot->bot_duck_time > level.time)
		{
			if (!(bot->client->ps.pm_flags & PMF_DUCKED))
			{// For special duck nodes, make the bot duck down...
				ucmd->upmove = -48;
				bot->client->ps.pm_flags |= PMF_DUCKED;
			}
		
			ucmd->forwardmove = 127;
			return;
		}
		else if (bot->bot_strafe_left_time > level.time)
		{
			ucmd->rightmove = -48;
			return;
		}
		else if (bot->bot_strafe_right_time > level.time)
		{
			ucmd->rightmove = 48;
			return;
		}
		else if (bot->bot_jump_time > level.time)
		{
			ucmd->upmove = 127;
		}
		else if (bot->bot_last_good_move_time < level.time - 1000
			&& bot->bot_last_move_fix_time < level.time - 1000)
		{// The first type of route correction...
			int result = -1;

			// So we don't vis check too often...
			bot->bot_last_move_fix_time = level.time;

			result = AIMOD_MOVEMENT_ReachableBy(bot, bot->goalentity->r.currentOrigin);

			if (result == -1)
			{// -1 is Not reachable at all... Let's use some blind routing for a short time!
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				bot->longTermGoal = -1;
				bot->shortTermGoal = -1;
				return;
			}

			if (result == 1)
			{// 1 is Jump...
				ucmd->upmove = 127;
				//ucmd->forwardmove = 127;
				bot->bot_jump_time = level.time + 500;
				//return;
			}

			if (result == 2)
			{// 2 is Duck...
				if (!(bot->client->ps.pm_flags & PMF_DUCKED))
				{// For special duck nodes, make the bot duck down...
					ucmd->upmove = -48;
					bot->client->ps.pm_flags |= PMF_DUCKED;
					bot->bot_duck_time = level.time + 2000;
				}
			}

			if (result == 3)
			{// 3 is Strafe Left...
				ucmd->rightmove = -48;
				bot->bot_strafe_left_time = level.time + 1000;
				return;
			}

			if (result == 4)
			{// 4 is Strafe Right...
				ucmd->rightmove = 48;
				bot->bot_strafe_right_time = level.time + 1000;
				return;
			}
		}
#endif //__BOT_STRAFE__

		if (NodeVisible(bot->goalentity->r.currentOrigin, bot->r.currentOrigin, bot->s.number))
		{// Get closer!
			// Set up ideal view angles for this enemy...
			VectorSubtract (bot->goalentity->r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
			bot->move_vector[ROLL] = 0;
			vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
			BotSetViewAngles(bot, 100);

			ucmd->buttons |= BUTTON_WALKING;

			if (bot->bot_turn_wait_time <= level.time)
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );

			G_Printf("^2*** ^3BOT DEBUG^5: ^3%s^5's has found his sniper point. (geting closer)\n", bot->client->pers.netname);
			G_Printf("^2*** ^3BOT DEBUG^5: Distance is %f.\n", VectorDistance(bot->goalentity->r.currentOrigin, bot->r.currentOrigin));
			return;
		}
		else
		{
			if (VectorDistance(nodes[bot->longTermGoal].origin, bot->r.currentOrigin) < 24)
			{
				G_Printf("^2*** ^3BOT DEBUG^5: ^3%s^5's has found his sniper point. (at goal node)\n", bot->client->pers.netname);
				G_Printf("^2*** ^3BOT DEBUG^5: Distance from actual sniper point is %f.\n", VectorDistance(bot->goalentity->r.currentOrigin, bot->r.currentOrigin));
			}
		}
	}*/
	if ( test_last_message[bot->s.number] < level.time - 3000 )
	{
		if( bot_debug.integer >= BOT_DEBUG_REGULAR )
			G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^5's has found his sniper point. (currently sniping)\n",
				  bot->client->pers.netname );
		test_last_message[bot->s.number] = level.time;
	}

	if ( bot->enemy && bot->bot_enemy_visible_time > level.time && bot->enemy == bot->bot_last_visible_enemy )
	{															// Do attacking first...
		AIMOD_MOVEMENT_AttackMove( bot, ucmd );
		return;
	}
	else
	{
		trace_t tr;
		vec3_t	forward, right, up, fwdPoint;
		float	fulldiff = 0;
		fulldiff = fabs( AngleDifference( bot->client->ps.viewangles[0], bot->bot_ideal_view_angles[0]) );
		fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[1], bot->bot_ideal_view_angles[1]) );
		fulldiff += fabs( AngleDifference( bot->client->ps.viewangles[2], bot->bot_ideal_view_angles[2]) );
		if ( fulldiff < 24 )
		{
			int closest_enemy = AIMOD_Movement_SniperClosestEnemy( bot );
			if ( !closest_enemy )
			{
				AngleVectors( bot->client->ps.viewangles, forward, right, up );

				// Set up ideal view angles for this enemy...
				bot->move_vector[0] += Q_TrueRand( -5, 5 );
				bot->move_vector[1] += Q_TrueRand( -25, 25 );
				bot->move_vector[2] += Q_TrueRand( -5, 5 );
				VectorMA( bot->r.currentOrigin, 65500, forward, fwdPoint );
				trap_Trace( &tr, bot->r.currentOrigin, NULL, NULL, fwdPoint, bot->s.number, MASK_SHOT );
				if ( VectorDistance( bot->r.currentOrigin, tr.endpos) < 512 )
				{												// If looking at ground, look up a bit!
					bot->move_vector[PITCH] += 10;
				}

				if ( tr.surfaceFlags & SURF_SKY )
				{												// If looking at sky, look down a bit...
					bot->move_vector[PITCH] -= 10;
				}

				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			}
			else
			{
				vec3_t	enemy_dir, enemy_angles;
				float	enemydiff = 0;
				AngleVectors( bot->client->ps.viewangles, forward, right, up );

				// Set up ideal view angles for this enemy...
				bot->move_vector[0] += Q_TrueRand( -5, 5 );
				bot->move_vector[1] += Q_TrueRand( -25, 25 );
				bot->move_vector[2] += Q_TrueRand( -5, 5 );
				VectorMA( bot->r.currentOrigin, 65500, forward, fwdPoint );
				trap_Trace( &tr, bot->r.currentOrigin, NULL, NULL, fwdPoint, bot->s.number, MASK_SHOT );
				if ( VectorDistance( bot->r.currentOrigin, tr.endpos) < 512 )
				{												// If looking at ground, look up a bit!
					bot->move_vector[PITCH] += 10;
				}

				if ( tr.surfaceFlags & SURF_SKY )
				{												// If looking at sky, look down a bit...
					bot->move_vector[PITCH] -= 10;
				}

				VectorSubtract( g_entities[closest_enemy].r.currentOrigin, bot->r.currentOrigin, enemy_dir );
				vectoangles( enemy_dir, enemy_angles );
				enemydiff = fabs( AngleDifference( bot->bot_ideal_view_angles[0], enemy_angles[0]) );
				enemydiff += fabs( AngleDifference( bot->bot_ideal_view_angles[1], enemy_angles[1]) );
				enemydiff += fabs( AngleDifference( bot->bot_ideal_view_angles[2], enemy_angles[2]) );
				if ( enemydiff > 45 )
				{												// Look somewhat toward them...
					bot->move_vector[1] = enemy_angles[1] + Q_TrueRand( -25, 25 );
				}

				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			}
		}

		BotSetViewAngles( bot, 100 );
	}

	//bot->bot_stand_guard = qtrue;
	ucmd->forwardmove = 0;
	ucmd->rightmove = 0;
	ucmd->upmove = 0;
	bot->node_timeout = level.time + ( TravelTime( bot) * 2 );	// Duck moves take longer, so...

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;

	//G_AI_Voice( bot, NULL, SAY_ALL, "Cheer", qfalse, 1000+Q_TrueRand(0, 500) , AIVOICE_FUNTALK   );
	//G_Printf("\n\n%s found his sniper position!\n\n\n", bot->client->pers.netname);
	return;
}


/* */
qboolean
AIMOD_MOVEMENT_WaypointTouched ( gentity_t *bot, int touch_node, qboolean onLadder )
{
	vec3_t	node_origin;
	float	node_distance;
	float	heightdiff;
	VectorCopy( nodes[touch_node].origin, node_origin );
	node_distance = VectorDistance( bot->r.currentOrigin, node_origin );
	heightdiff = HeightDistance( bot->r.currentOrigin, node_origin );
	if ( onLadder && node_distance < 32 )
	{							// Need vertical checks on ladders...
		if ( node_distance - heightdiff < 4 )
		{
			return ( qtrue );
		}
	}

	if ( node_distance < 64 && nodes[touch_node].type == NODE_ICE )
	{							// Close enough when we are sliding...
		return ( qtrue );
	}

	if ( node_distance < 16 )
	{							// At the node...
		return ( qtrue );
	}

	if ( node_distance < 64 )
	{							// Check height...
		if ( node_distance - heightdiff < 16 )
		{
			return ( qtrue );	// At the node...
		}
	}

#ifdef __VEHICLES__

	/*	if (bot->client->ps.eFlags & EF_TANK)
	{
		if (node_distance < 96 )
		{
			vec3_t test_pos, forward;

			AngleVectors (bot->client->ps.viewangles, forward, NULL, NULL);
			VectorCopy (bot->r.currentOrigin, test_pos);
			VectorMA (test_pos, 32, forward, test_pos);

			if (VectorDistance(test_pos, nodes[touch_node].origin) > node_distance)
			{// Must have gone past the node... Accept it as a hit...
				return qtrue;
			}

		}
	}*/
#endif //__VEHICLES__
	return ( qfalse );
}

extern vmCvar_t bot_minplayers;
#ifdef __VEHICLES__
extern void		AIMOD_MOVEMENT_VehicleMove ( gentity_t *bot, usercmd_t *ucmd );
#endif //__VEHICLES__

vmCvar_t bot_cpu_use;

//special node visibility check for bot linkages..
//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.

//3 = team door entity in the way.
int
SP_NodeVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg;
	vec3_t	mins, maxs;

	VectorCopy( botTraceMins, mins );
	VectorCopy( botTraceMaxs, maxs );
	VectorCopy( org2, newOrg );

	newOrg[2] += 4; // Look from up a little...

	trap_Trace( &tr, org1, mins, maxs, newOrg, ignore, MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA /*MASK_WATER*/ );
	if ( tr.fraction == 1 )
	{
		return ( 1 );
	}

	if
	(
		tr.fraction != 1 &&
		tr.entityNum != ENTITYNUM_NONE &&
		(
			!strcmp( g_entities[tr.entityNum].classname, "func_door") || !strcmp( g_entities[tr.entityNum].classname,
																				  "func_door_rotating")
		)
	)
	{// A door in the way.. See if we have access...
		if ( !g_entities[tr.entityNum].allowteams )
		{
			return ( 2 );
		}

		return ( 3 );
	}

	return ( 0 );
}

//
// Cover Spot Code...
//

int		num_axis_cover_spots = 0;
vec3_t	axis_cover_spots[16550];
int		num_allied_cover_spots = 0;
vec3_t	allied_cover_spots[16550];

void SP_Axis_Cover_Spot ( gentity_t *ent )
{
	VectorCopy(ent->s.origin, axis_cover_spots[num_axis_cover_spots]);
	num_axis_cover_spots++;
	G_FreeEntity(ent);
}

void SP_Allied_Cover_Spot ( gentity_t *ent )
{
	VectorCopy(ent->s.origin, allied_cover_spots[num_allied_cover_spots]);
	num_allied_cover_spots++;
	G_FreeEntity(ent);
}

void
AIMOD_SaveCoverPoints ( void )
{
	fileHandle_t	f;
	int				i;
	vmCvar_t		mapname;
	char			map[64] = "";
	char			filename[60];

	G_Printf( "^3*** ^3%s: ^7Saving cover point table.\n", GAME_VERSION );
	strcpy( filename, "nodes/" );

	////////////////////
//	trap_Cvar_VariableStringBuffer( "g_scriptName", filename, sizeof(filename) );
//	if ( strlen( filename) > 0 )
//	{
//		trap_Cvar_Register( &mapname, "g_scriptName", "", CVAR_ROM );
//	}
//	else
//	{
		trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );
//	}

	Q_strcat( filename, sizeof(filename), mapname.string );

	///////////////////
	//try to open the output file, return if it failed
	trap_FS_FOpenFile( va( "%s.cpf", filename), &f, FS_WRITE );
	if ( !f )
	{
		G_Printf( "^1*** ^3ERROR^5: Error opening cover point file ^7%s.cpf^5!!!\n", filename );
		return;
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );	//get the map name
	strcpy( map, mapname.string );
									//write the map name
	trap_FS_Write( &num_axis_cover_spots, sizeof(short int), f );							//write the number of nodes in the map

	for ( i = 0; i < num_axis_cover_spots; i++ )											//loop through all the nodes
	{
		trap_FS_Write( &(axis_cover_spots[i]), sizeof(vec3_t), f );
	}

	trap_FS_Write( &num_allied_cover_spots, sizeof(short int), f );							//write the number of nodes in the map

	for ( i = 0; i < num_allied_cover_spots; i++ )											//loop through all the nodes
	{
		trap_FS_Write( &(allied_cover_spots[i]), sizeof(vec3_t), f );
	}

	trap_FS_FCloseFile( f );		

	G_Printf( "^3*** ^3%s: ^5Cover point table saved to file ^7%s.cpf^5.\n", GAME_VERSION, filename );
}

vmCvar_t fs_game;

/* */
qboolean
AIMOD_LoadCoverPoints ( void )
{
	fileHandle_t	f;
	int				i;
	char			filename[60];
	vmCvar_t		mapname;
	char			nm[64] = "";
	char			map[64] = "";
	char			mp[64] = "";

	i = 0;
	strcpy( filename, "nodes/" );

//	////////////////////
//	trap_Cvar_VariableStringBuffer( "g_scriptName", filename, sizeof(filename) );
//	if ( strlen( filename) > 0 )
//	{
//		trap_Cvar_Register( &mapname, "g_scriptName", "", CVAR_ROM );
//	}
//	else
//	{
		trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );
//	}

	Q_strcat( filename, sizeof(filename), mapname.string );

	///////////////////
	//open the node file for reading, return false on error
	trap_FS_FOpenFile( va( "%s.cpf", filename), &f, FS_READ );
	if ( !f )
	{
		trap_FS_FOpenFile( va( "/%s/%s.cpf", fs_game.string, filename), &f, FS_READ );
		if ( !f )
		{
			G_Printf( "^1*** ^3WARNING^5: Reading from ^7%s.cpf^3 failed^5!!!\n", filename );
			return qfalse;
		}
	}

	trap_FS_Read( &num_axis_cover_spots, sizeof(short int), f ); //read in the number of nodes in the map
	for ( i = 0; i < num_axis_cover_spots; i++ )					//loop through all the nodes
	{
		trap_FS_Read( &axis_cover_spots[i], sizeof(vec3_t), f );
	}

	trap_FS_Read( &num_allied_cover_spots, sizeof(short int), f ); //read in the number of nodes in the map
	for ( i = 0; i < num_allied_cover_spots; i++ )					//loop through all the nodes
	{
		trap_FS_Read( &allied_cover_spots[i], sizeof(vec3_t), f );
	}

	trap_FS_FCloseFile( f );							//close the file

	G_Printf( "^1*** ^3%s^5: Successfully loaded %i axis and %i allied\n", GAME_VERSION, num_axis_cover_spots, num_allied_cover_spots);
	G_Printf( "^1*** ^3%s^5: cover points from file ^7%s.cpf^5.\n", GAME_VERSION, filename );

	return qtrue;
}

qboolean coverspots_generated = qfalse;

void AIMOD_Generate_Cover_Spots ( void )
{
	if (!coverspots_generated)
	{// Need to make some from waypoint list if we can!
		coverspots_generated = qtrue;

		// Try loading from a file first (for speed)
		if (number_of_nodes > 0)
			if (AIMOD_LoadCoverPoints())
				return;

		if (number_of_nodes > 0)
		{
			int i = 0;

			for (i = 0; i < number_of_nodes; i++)
			{
				int j = 0;
				int num_found = 0;

				for (j = 0; j < number_of_nodes; j++)
				{
					if (VectorDistance(nodes[i].origin, nodes[j].origin) < 256)
					{
						qboolean	bad = qfalse;
						int			z = 0;

						for (z = 0; z < nodes[z].enodenum; z++)
						{
							if (nodes[i].links[z].targetNode == j)
							{
								bad = qtrue;
								break;
							}
						}

						if (!bad)
						{// We found a close node thats not a link! Must be blocked by something!
							vec3_t org1, org2;

							VectorCopy(nodes[i].origin, org1);
							VectorCopy(nodes[j].origin, org2);
							org1[2]+=16;
							org2[2]+=16;

							if (!NodeVisible(org1, org2, -1))
							{
								org1[2]+=128;//48;
								org2[2]+=16;//48;

								if (NodeVisible(org1, org2, -1) == 1)
								{
									num_found++;
								}
							}
						}
					}
				}

				if (num_found > 1)
				{
					VectorCopy(nodes[i].origin, axis_cover_spots[num_axis_cover_spots]);
					num_axis_cover_spots++;
					VectorCopy(nodes[i].origin, allied_cover_spots[num_allied_cover_spots]);
					num_allied_cover_spots++;
				}
			}

			if( bot_debug.integer >= BOT_DEBUG_REGULAR )
				G_Printf("^4*** ^3BOT DEBUG^5: Generated ^7%i^5 cover spots for"
					" each team.\n", num_allied_cover_spots);

			// Save them for fast loading next time!
			AIMOD_SaveCoverPoints();
		}
	}
}

extern vmCvar_t bot_coverspot_render;

/* */
void
BotCoverSpotRender ( void )
{
	int			i, n;
	int			inc_checker;
	int			bestindex;
	gentity_t	*plum;
	vec3_t		eyes;
	qboolean	showCoverPoints;
	int			gCPRenderTime = 0;
	int			gCPNum = 0;
	int			gCPRenderedFrame = 0;

	VectorCopy( g_entities[0].r.currentOrigin, eyes );
	eyes[2] += 32;
	
	if ( bot_coverspot_render.integer )
	{
		showCoverPoints = qtrue;
	}

	if ( !showCoverPoints )
	{
		return;
	}

	gCPNum = num_axis_cover_spots;

	bestindex = 0;
	if ( gCPRenderTime > level.time )
	{
		goto checkprint;
	}

	gCPRenderTime = level.time + 300;
	gCPRenderedFrame = 0;
	i = 0;
	inc_checker = 0;
	while ( i < gCPNum )
	{
		if ( axis_cover_spots[i] )
		{
			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, axis_cover_spots[i]) < 128 &&
				OrgVisible( axis_cover_spots[i], eyes, -1)
			)
			{
				vec3_t	up;
				VectorCopy( axis_cover_spots[i], up );
				up[2] += i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
//				plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
//				plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
//				plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
//				plum->s.weapon = 4;		// Mark this as a vehicle node for the tank icon.
//				plum->s.weapon = 0;

				n = 0;
				gCPRenderedFrame++;
			}
		}
		if ( gCPRenderedFrame > 64 )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	i = 0;									//gCPRenderedFrame;
	inc_checker = 0;						//gCPRenderedFrame;
	while ( i < gCPNum )
	{
		if ( gCPRenderedFrame > 64 /*128*/ )
		{
			break;
		}

		if ( axis_cover_spots[i] )
		{
			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, axis_cover_spots[i]) < 512 &&
				VectorDistance( g_entities[0].r.currentOrigin, axis_cover_spots[i]) >= 128 &&
				OrgVisible( axis_cover_spots[i], eyes, -1)
			)
			{
				vec3_t	up;
				VectorCopy( axis_cover_spots[i], up );
				up[2] += i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
//				plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
//				plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
//				plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
//				plum->s.weapon = 4;		// Mark this as a vehicle node for the tank icon.
//				plum->s.weapon = 0;

				n = 0;
				gCPRenderedFrame++;
			}
		}

		if ( gCPRenderedFrame > 64 /*128*/ )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	i = 0;									//gCPRenderedFrame;
	inc_checker = 0;						//gCPRenderedFrame;
	while ( i < gCPNum )
	{
		if ( gCPRenderedFrame > 64 /*128*/ )
		{
			break;
		}

		if ( axis_cover_spots[i] )
		{
			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, axis_cover_spots[i]) < 1024 &&
				VectorDistance( g_entities[0].r.currentOrigin, axis_cover_spots[i]) >= 512 &&
				OrgVisible( axis_cover_spots[i], eyes, -1)
			)
			{
				vec3_t	up;
				VectorCopy( axis_cover_spots[i], up );
				up[2] += i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
//				plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
//				plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
//				plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
//				plum->s.weapon = 4;		// Mark this as a vehicle node for the tank icon.
//				plum->s.weapon = 0;

				n = 0;
				gCPRenderedFrame++;
			}
		}

		if ( gCPRenderedFrame > 64 /*128*/ )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	if ( i >= gCPNum )
	{
		gCPRenderTime = level.time + 300;	//1500; //wait a bit after we finish doing the whole trail
		gCPRenderedFrame = 0;
	}

checkprint:
	{
	}
}

void AIMOD_Combat_Weapons_Check ( gentity_t *bot, usercmd_t *ucmd );

#ifdef __VEHICLES__
extern void AIMOD_MOVEMENT_VehicleAttack ( gentity_t *bot, usercmd_t *ucmd );
#endif //__VEHICLES__

//===========================================================================
// Routine      : AIMOD_MOVEMENT_CoverSpot_AttackMove

// Description  : Movement while attacking...
void
AIMOD_MOVEMENT_CoverSpot_AttackMove ( gentity_t *bot, usercmd_t *ucmd )
{
	gentity_t	*self = bot;
	float		firedelay;
	vec3_t		target;
	vec3_t		attackvector;
	float		dist;
	vec3_t		ang, dir;
	int			skill;
	qboolean	onLadder = qfalse;

	if (g_gametype.integer <= GT_COOP)
		bot->client->ps.ammo[bot->client->ps.weapon] = 32; // SP Bots always have ammo!

#ifdef __VEHICLES__
	if ( bot->client->ps.eFlags & EF_TANK )
	{
		AIMOD_MOVEMENT_VehicleAttack( bot, ucmd );
		return;
	}
#endif //__VEHICLES__
	
	//
	// Attack Code First!!!
	//
	if
	(
		bot->beStill > level.time &&
		!(bot->client && (bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK))
	)
	{
		return;
	}

	if ( g_gametype.integer >= GT_WOLF )
	{
		skill = bot_skill.integer;
	}
	else
	{
		skill = bot_skill.integer;
	}

	if ( skill <= 0 )
	{
		skill = 1;
	}

	if ( !bot->enemy || bot->bot_enemy_visible_time < level.time)
	{
		return;
	}

	VectorSubtract( bot->r.currentOrigin, bot->enemy->r.currentOrigin, dir );
	vectoangles( dir, ang );

	// Give them extra time to get to next node...
	bot->node_timeout = level.time + TravelTime( bot );

	// Set up ideal view angles for this enemy...
	VectorSubtract( bot->enemy->r.currentOrigin, bot->r.currentOrigin, attackvector );
	vectoangles( attackvector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );
	dist = VectorLength( attackvector );
	Set_Best_AI_weapon( bot );
	AIMOD_Combat_Weapons_Check( bot, ucmd );
	
	if ( bot->client->ps.ammoclip[bot->client->ps.weapon] == 0 && bot->client->ps.ammo[bot->client->ps.weapon] > 0 )
	{
		int weaponChoice = bot->client->ps.weapon;

		if ( bot->client->ps.weapon == WP_FG42SCOPE )
		{
			weaponChoice = WP_FG42;
		}

		if ( bot->client->ps.weapon == WP_K43_SCOPE )
		{
			weaponChoice = WP_K43;
		}

		if ( bot->client->ps.weapon == WP_GARAND_SCOPE )
		{
			weaponChoice = WP_GARAND;
		}

		if ( bot->client->ps.weapon == WP_GPG40 )
		{
			weaponChoice = WP_KAR98;
		}

		if ( bot->client->ps.weapon == WP_M7 )
		{
			weaponChoice = WP_CARBINE;
		}

		if (bot->client->ps.weapon != weaponChoice)
			ucmd->weapon = weaponChoice;

		ucmd->wbuttons = WBUTTON_RELOAD;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		if (bot->bot_coverspot_time > level.time)
		{
			bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
			bot->client->ps.pm_flags |= PMF_DUCKED;
			ucmd->upmove = -127;
		}
		return;
	}

	if ( ucmd->wbuttons == WBUTTON_RELOAD )
	{															// Reloading!
		int weaponChoice = bot->client->ps.weapon;

		if ( bot->client->ps.weapon == WP_FG42SCOPE )
		{
			weaponChoice = WP_FG42;
		}

		if ( bot->client->ps.weapon == WP_K43_SCOPE )
		{
			weaponChoice = WP_K43;
		}

		if ( bot->client->ps.weapon == WP_GARAND_SCOPE )
		{
			weaponChoice = WP_GARAND;
		}

		if ( bot->client->ps.weapon == WP_GPG40 )
		{
			weaponChoice = WP_KAR98;
		}

		if ( bot->client->ps.weapon == WP_M7 )
		{
			weaponChoice = WP_CARBINE;
		}

		if (bot->client->ps.weapon != weaponChoice)
			ucmd->weapon = weaponChoice;

		ucmd->upmove = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;

		if (bot->bot_coverspot_time > level.time)
		{
			bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
			bot->client->ps.pm_flags |= PMF_DUCKED;
			ucmd->upmove = -127;
		}
		return;
	}

	if (ucmd->weapon == WP_SMOKE_MARKER && bot->client->ps.weapon != WP_SMOKE_MARKER)
	{															// Wait until the animation has been completed...
		ucmd->upmove = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		return;
	}

	if ( ucmd->weapon && ucmd->weapon != bot->client->ps.weapon )
	{															// Wait until the animation has been completed...
		ucmd->upmove = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		return;
	}

	if (bot->client->ps.eFlags & EF_PRONE)
		bot->client->ps.eFlags &= ~EF_PRONE;

	if (bot->client->ps.pm_flags & PMF_DUCKED)
		bot->client->ps.pm_flags &= ~PMF_DUCKED;

	if ( bot->client->ps.weapon == WP_KNIFE )
	{															// Bots with a knife should always run at the enemy.
		ucmd->forwardmove = 120;
		if ( VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 64 )
		{
			return;												// And don't try to stab until we are close enough...
		}
	}
	else
	{
/*		if (bot->bot_coverspot_time > level.time)
		{
			if (bot->client->ps.eFlags & EF_PRONE)
				bot->client->ps.eFlags &= ~EF_PRONE;

			if (bot->client->ps.pm_flags & PMF_DUCKED)
				bot->client->ps.pm_flags &= ~PMF_DUCKED;

			bot->bot_coverspot_time = level.time + 20000;

			if (bot->bot_coverspot_staydown_timer > level.time)
			{// Stay down for a bit longer!
				bot->client->ps.pm_flags |= PMF_DUCKED;
				ucmd->upmove = -127;
				return;
			}
			else
			{// Should we duck behind our cover for a while???
				if (Q_TrueRand(0, 20000) > 19900)
				{// Pick how long to stay down for!
					bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
					bot->client->ps.pm_flags |= PMF_DUCKED;
					ucmd->upmove = -127;
					return;
				}
			}
	
		}*/
/*		else if ( bot->skillchoice >= 4 && dist > 800 )
		{														// Elite bots.
			if ( !(bot->client->ps.eFlags & EF_PRONE) && !(bot->client->ps.pm_flags & PMF_DUCKED) )
			{													// We should only vischeck this once at the first attack time...
				ucmd->upmove = -127;							// Crouch?

				// Only go prone if we can still hit them from down there...
				if ( visible( bot, bot->enemy) )
				{
					bot->client->ps.eFlags |= EF_PRONE;
				}
				else
				{
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}

				//	ucmd->upmove = 0; // jaquboss - reseting this will make bots go up again
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
			}
		}
		else if ( bot->skillchoice >= 2 && dist > 1024 )
		{														// Good bots.
			if ( !(bot->client->ps.eFlags & EF_PRONE) && !(bot->client->ps.pm_flags & PMF_DUCKED) )
			{													// We should only vischeck this once at the first attack time...
				ucmd->upmove = -127;							// Crouch?

				// Only go prone if we can still hit them from down there...
				if ( visible( bot, bot->enemy) )
				{
					bot->client->ps.eFlags |= EF_PRONE;
				}
				else
				{
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}

				//	ucmd->upmove = 0; // jaquboss - reseting this will make bots go up again
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
			}
		}
		else if ( dist > 1024 && !(bot->client->ps.pm_flags & PMF_DUCKED) )
		{														// Use long aim ! DUCK ! IF you don't have the flag ! That is
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;

			//	ucmd->upmove = 0; // jaquboss - reseting this will make bots go up again
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
		}*/
	}

	bot->node_timeout = level.time + TravelTime( bot );			//6000;//4000;
	bot->bot_last_good_move_time = level.time;

	// Set the attack
	//-------------------------------------------------------
	firedelay = ( random() - 0.5 ) * 5;

	// Aim
	//-------------------------------------------------------
	VectorCopy( bot->enemy->r.currentOrigin, target );

	//-------------------------------------------------------
	// Modify attack angles based on accuracy
	// added skill level
	//-------------------------------------------------------
	if ( bot->skillchoice == 0 )
	{

		// Can't hit a good strafing player
		int randm;
		randm = Q_TrueRand( 0, 1 );
		if ( randm == 0 )
		{
			target[0] += ( random() - 0.5 ) * 20;
		}
		else
		{
			target[0] -= ( random() - 0.5 ) * 20;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[1] += ( random() - 0.5 ) * 20;
		}
		else
		{
			target[1] -= ( random() - 0.5 ) * 20;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[2] += ( random() - 0.5 ) * 20;
		}
		else
		{
			target[2] -= ( random() - 0.5 ) * 20;
		}
	}
	else if ( bot->skillchoice == 1 )
	{

		// Will get ya every now and then
		int randm;
		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[0] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[0] -= ( random() - 0.5 ) * 10;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[1] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[1] -= ( random() - 0.5 ) * 10;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[2] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[2] -= ( random() - 0.5 ) * 10;
		}
	}
	else if ( bot->skillchoice == 2 )
	{

		// Good competetion for some of the best players
		int randm;
		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[0] += ( random() - 0.5 ) * 5;
		}
		else
		{
			target[0] -= ( random() - 0.5 ) * 5;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[1] += ( random() - 0.5 ) * 5;
		}
		else
		{
			target[1] -= ( random() - 0.5 ) * 5;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[2] += ( random() - 0.5 ) * 5;
		}
		else
		{
			target[2] -= ( random() - 0.5 ) * 5;
		}
	}
	else if ( bot->skillchoice >= 3 )
	{

		// Never misses, unloads on ya!
		//target[0] = target[0];
		//target[1] = target[1];
	}

	//-------------------
	if ( bot->client && (bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK) )
	{															// Randomize attack dir a little more while on an MG42 or Tank...
		int randm;
		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[0] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[0] -= ( random() - 0.5 ) * 10;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[1] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[1] -= ( random() - 0.5 ) * 10;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[2] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[2] -= ( random() - 0.5 ) * 10;
		}
	}

	// If they are not standing, we need to lower our shots a little!
	if ( bot->enemy->client && bot->client->ps.eFlags & EF_PRONE && bot->enemy->client->ps.eFlags & EF_PRONE )
	{
		target[2] -= Q_TrueRand( 0, 16 );;
	}
	else if ( bot->enemy->client && bot->enemy->client->ps.eFlags & EF_PRONE )
	{
		target[2] -= Q_TrueRand( 48, 62 );
	}
	else if ( bot->enemy->client && bot->enemy->client->ps.pm_flags & PMF_DUCKED )
	{
		target[2] -= Q_TrueRand( 32, 48 );
	}

	// Set direction
	//-------------------------------------------------------
	VectorSubtract( target, bot->r.currentOrigin, bot->attack_vector );
	if ( bot->client->ps.weapon == WP_SMOKE_MARKER )
	{
		bot->attack_vector[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfAirstrike( bot );
	}

	if ( bot->client->ps.weapon == WP_GPG40 || bot->client->ps.weapon == WP_M7 )
	{
		bot->attack_vector[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfGrenade( bot );
	}

	if ( bot->client->ps.weapon == WP_GRENADE_LAUNCHER || bot->client->ps.weapon == WP_GRENADE_PINEAPPLE )
	{
		bot->attack_vector[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfGrenade( bot );
	}

	vectoangles( bot->attack_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );

	// Added Skill Level Settings for Attack
	if ( VectorDistance( bot->bot_ideal_view_angles, bot->client->ps.viewangles) <= 16 )
	{
		if ( bot->skillchoice == 0 )
		{
			if ( firedelay < 0.10 )
			{
				ucmd->buttons = BUTTON_ATTACK;
			}
		}
		else if ( bot->skillchoice == 1 )
		{
			if ( firedelay < 0.20 )
			{
				ucmd->buttons = BUTTON_ATTACK;
			}
		}
		else if ( bot->skillchoice == 2 )
		{
			if ( firedelay < 0.40 )
			{
				ucmd->buttons = BUTTON_ATTACK;
			}
		}
		else if ( bot->skillchoice >= 3 )
		{
			ucmd->buttons = BUTTON_ATTACK;
		}
	}

	//-------------------------------------------------------
	if
	(
		bot->client->ps.weapon == WP_MEDKIT ||
		bot->client->ps.weapon == WP_AMMO ||
		bot->client->ps.weapon == WP_PLIERS ||
		bot->client->ps.weapon == WP_DYNAMITE
	)
	{
		ucmd->buttons = 0;
	}

	bot->teamPauseTime = level.time;

	// Prototype team enemy handling...
	//	AIMOD_InformTeamOfEnemy(bot);
/*	if ( (bot->client->ps.eFlags & EF_PRONE) || (bot->client->ps.pm_flags & PMF_DUCKED) )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}*/

#ifdef __VEHICLES__
	if ( bot->client->ps.eFlags & EF_TANK )
	{															// Don't move while firing in a tank...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}
#endif //__VEHICLES__

	// Cover spots!
/*	if (bot->bot_coverspot_time > level.time)
	{
		if (VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) < 16)
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
		}
	}*/

	//
	// End of Attack Code!!!
	//
	//
	// Now the movement code!
	//
	if ( bot->client->ps.eFlags & EF_ZOOMING )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if
	(
		bot->client->ps.weapon == WP_FG42SCOPE ||
		bot->client->ps.weapon == WP_K43_SCOPE ||
		bot->client->ps.weapon == WP_GARAND_SCOPE
	)
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if ( bot->client->ps.weapon == WP_MOBILE_MG42_SET )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

#ifdef __WEAPON_AIM__
	if ( bot->client->ps.eFlags & EF_AIMING )
	{
		ucmd->buttons |= BUTTON_WALKING;
	}
	else
	{
		ucmd->buttons &= ~BUTTON_WALKING;
	}
#endif //__WEAPON_AIM__

	// Set up ideal angles for this node...
	ucmd->buttons &= ~BUTTON_WALKING; // We want to RUN to cover spots! :)
	
	VectorSubtract( bot->bot_coverspot, bot->r.currentOrigin, bot->move_vector );
	bot->move_vector[ROLL] = 0;
	//vectoangles( bot->move_vector, bot->movedir );
	
	G_UcmdMoveForDir( bot, ucmd, bot->move_vector/*bot->movedir*/ );

	// Do we need to jump?
	if (VectorLength(bot->client->ps.velocity) < 16)
		ucmd->upmove = 127;

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;
}

qboolean
AIMOD_SP_MOVEMENT_Someone_At_Coverpoint ( vec3_t coverpoint )
{
	int i = 0;

	for (i = 0; i < g_maxclients.integer; i++)
	{
		gentity_t *ent = &g_entities[i];

		if (!ent)
			continue;

		if (!ent->client)
			continue;

		if (ent->client->ps.stats[STAT_HEALTH] <= 0)
			continue;

		if (VectorDistance(coverpoint, ent->r.currentOrigin) < 64)
			return qtrue;

		if (ent->bot_coverspot_time && ent->bot_coverspot_time > level.time)
		{
			if (VectorDistance(coverpoint, ent->bot_coverspot) < 64)
				return qtrue;
		}
	}

	return qfalse;
}

qboolean
AIMOD_SP_MOVEMENT_Find_Cover_Spot ( gentity_t *bot, usercmd_t *ucmd )
{
	if (!bot->enemy)
		return qfalse;

	if (num_axis_cover_spots <= 0 && bot->client->sess.sessionTeam == TEAM_AXIS)
		return qfalse;

	if (num_allied_cover_spots <= 0 && bot->client->sess.sessionTeam == TEAM_ALLIES)
		return qfalse;

	if (bot->bot_coverspot_next_check_time > level.time)
		return qfalse;

	bot->bot_coverspot_next_check_time = level.time + 1000; // Max of 1 check every second!

	if (bot->client->sess.sessionTeam == TEAM_AXIS)
	{// Axis bot...
		int		i = 0;
		int		num_spots = 0;
		vec3_t	spots[1024];

		// Create a short list of spots this bot can use...
		for (i = 0; i < num_axis_cover_spots; i++)
		{
			if (VectorDistance(bot->r.currentOrigin, axis_cover_spots[i]) <= 1024
				&& HeightDistance(bot->r.currentOrigin, axis_cover_spots[i]) < 24 )
			{// Close enough to add to the "good" list...
				if (SP_NodeVisible(bot->r.currentOrigin, axis_cover_spots[i], bot->s.number))
				{// Only if the spot is reachable!
					vec3_t upspot, upspot2, upspot3;

					VectorCopy(axis_cover_spots[i], upspot);
					upspot[2]+=48;
					VectorCopy(bot->enemy->r.currentOrigin, upspot2);
					upspot2[2]+=48;
					VectorCopy(axis_cover_spots[i], upspot3);
					upspot3[2]+=16;

					if (!SP_NodeVisible(upspot2, upspot, bot->enemy->s.number))
					{// Make sure we can shoot over it!

					}
					else if (!SP_NodeVisible(bot->enemy->r.currentOrigin, upspot3/*axis_cover_spots[i]*/, bot->enemy->s.number))
					{// And if the enemy can not see it!
						VectorCopy(axis_cover_spots[i], spots[num_spots]);
						num_spots++;
					}
				}
			}
		}

		// Pick the best spot!
		if (num_spots > 0)
		{
			int		closest = -1;
			float	closest_dist = 99999.9f;

			// Find the closest one!
			for (i = 0; i < num_spots; i++)
			{
				float	dist = VectorDistance(bot->r.currentOrigin, spots[i]);
				int		num = 0;
				vec3_t	mins, maxs;
				int		touch[MAX_GENTITIES];

				VectorCopy(spots[i], mins);
				mins[0]-=96;
				mins[1]-=96;
				mins[2]-=96;
				VectorCopy(spots[i], maxs);
				maxs[0]+=96;
				maxs[1]+=96;
				maxs[2]+=96;
				
				num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

				if (num <= 0 && dist < closest_dist && !AIMOD_SP_MOVEMENT_Someone_At_Coverpoint(spots[i]))
				{// Found a better one!
					closest_dist = dist;
					closest = i;
				}
			}

			if (closest < 0)
				return qfalse;

			// Set it up as our destination...
			VectorCopy(spots[closest], bot->bot_coverspot);
			bot->bot_coverspot_time = level.time + 30000;

			return qtrue;
		}
	}
	else
	{// Allied bot...
		int		i = 0;
		int		num_spots = 0;
		vec3_t	spots[1024];

		// Create a short list of spots this bot can use...
		for (i = 0; i < num_allied_cover_spots; i++)
		{
			if (VectorDistance(bot->r.currentOrigin, allied_cover_spots[i]) <= 1024
				&& HeightDistance(bot->r.currentOrigin, allied_cover_spots[i]) < 24 )
			{// Close enough to add to the "good" list...
				if (SP_NodeVisible(bot->r.currentOrigin, allied_cover_spots[i], bot->s.number))
				{// Only if the spot is reachable!
					vec3_t upspot, upspot2, upspot3;

					VectorCopy(allied_cover_spots[i], upspot);
					upspot[2]+=48;
					VectorCopy(bot->enemy->r.currentOrigin, upspot2);
					upspot2[2]+=48;
					VectorCopy(allied_cover_spots[i], upspot3);
					upspot3[2]+=16;

					if (!SP_NodeVisible(upspot2, upspot, bot->enemy->s.number))
					{// Make sure we can shoot over it!

					}
					else if (!SP_NodeVisible(bot->enemy->r.currentOrigin, upspot3/*allied_cover_spots[i]*/, bot->enemy->s.number))
					{// And if the enemy can not see it!
						VectorCopy(allied_cover_spots[i], spots[num_spots]);
						num_spots++;
					}
				}
			}
		}

		// Pick the best spot!
		if (num_spots > 0)
		{
			int		closest = -1;
			float	closest_dist = 99999.9f;

			// Find the closest one!
			for (i = 0; i < num_spots; i++)
			{
				float	dist = VectorDistance(bot->r.currentOrigin, spots[i]);
				int		num = 0;
				vec3_t	mins, maxs;
				int		touch[MAX_GENTITIES];

				VectorCopy(spots[i], mins);
				mins[0]-=96;
				mins[1]-=96;
				mins[2]-=96;
				VectorCopy(spots[i], maxs);
				maxs[0]+=96;
				maxs[1]+=96;
				maxs[2]+=96;
				
				num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

				if (num <= 0 && dist < closest_dist && !AIMOD_SP_MOVEMENT_Someone_At_Coverpoint(spots[i]))
				{// Found a better one!
					closest_dist = dist;
					closest = i;
				}
			}

			if (closest < 0)
				return qfalse;

			// Set it up as our destination...
			VectorCopy(spots[closest], bot->bot_coverspot);
			bot->bot_coverspot_time = level.time + 30000;

			return qtrue;
		}
	}

	return qfalse;
}

void AIMOD_SP_Debug_GotoCP ( gentity_t *ent, int number )
{
	G_SetOrigin(ent, axis_cover_spots[number]);
	VectorCopy(axis_cover_spots[number], ent->client->ps.origin);
}

qboolean
AIMOD_SP_MOVEMENT_Is_CoverSpot_Weapon ( int weapon )
{
	qboolean retval = qtrue;

	switch (weapon)
	{
		case WP_NONE:
		case WP_KNIFE:
		case WP_MEDIC_SYRINGE:
		case WP_AMMO:
		case WP_ARTY:
		case WP_DYNAMITE:
		case WP_MAPMORTAR:
		case VERYBIGEXPLOSION:
		case WP_MEDKIT:
		case WP_BINOCULARS:
		case WP_PLIERS:
		case WP_LANDMINE:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
//		case WP_TRIPMINE:
		case WP_DUMMY_MG42:
		case WP_MORTAR:
//		case WP_LOCKPICK:
		case WP_MORTAR_SET:
		case WP_MEDIC_ADRENALINE:
		case WP_MOBILE_MG42_SET:
			retval = qfalse;
			break;
		default:
			retval = qtrue;
			break;
	}

	return (retval);
}

extern vmCvar_t bot_coverspot_enable;

//===========================================================================
// Routine      : AIMOD_SP_MOVEMENT_CoverSpot_Movement

// Description  : Single Player AI cover spot usage...
void
AIMOD_SP_MOVEMENT_CoverSpot_Movement ( gentity_t *bot, usercmd_t *ucmd )
{
	if (!bot_coverspot_enable.integer)
	{// Coverspots disabled! Normal attack code..
		if (number_of_nodes > 0)
			AIMOD_MOVEMENT_AttackMove( bot, ucmd );
		else
			AIMOD_MOVEMENT_Attack( bot, ucmd );

		return;
	}

	if (bot->bot_coverspot_time > level.time)
	{// We have one! Move to it if we need to!
		vec3_t		upspot, upspot2, upspot3;
		qboolean	invalid = qfalse;

		if (bot->enemy && bot->bot_coverspot_enemy)
		{// Still valid check!
			VectorCopy(bot->bot_coverspot, upspot);
			upspot[2]+=48;
			VectorCopy(bot->enemy->r.currentOrigin, upspot2);
			upspot2[2]+=48;
			VectorCopy(bot->bot_coverspot, upspot3);
			upspot3[2]+=16;

			if (!SP_NodeVisible(upspot2, upspot, bot->enemy->s.number))
			{// Make sure we can shoot over it!	

			}
			else if (!SP_NodeVisible(bot->enemy->r.currentOrigin, upspot3, bot->enemy->s.number))
			{// And if the enemy can not see it!
				
			}
			else
			{
				invalid = qtrue;
			}
		}

		if (!bot->bot_coverspot_enemy || !bot->enemy || bot->enemy != bot->bot_coverspot_enemy)
		{// No longer have an enemy!
			// Initialize! Will find a new spot next frame!
			bot->bot_coverspot_time = 0;
			VectorSet(bot->bot_coverspot, 0, 0, 0);
#ifdef __BOT_COVERSPOT_DEBUG__
			bot->s.dl_intensity = 0; // Debug sprite above head!
#endif //__BOT_COVERSPOT_DEBUG__
		}
		else if (invalid && VectorDistance(bot->bot_coverspot_enemy->r.currentOrigin, bot->bot_coverspot_enemy_position) > 16 )
		{// He has moved!!!
			// Initialize! Will find a new spot next frame!
			bot->bot_coverspot_time = 0;
			VectorSet(bot->bot_coverspot, 0, 0, 0);
#ifdef __BOT_COVERSPOT_DEBUG__
			bot->s.dl_intensity = 0; // Debug sprite above head!
#endif //__BOT_COVERSPOT_DEBUG__
		}
		if (bot->bot_coverspot_enemy->health <= 0 || bot->bot_coverspot_enemy->client->ps.stats[STAT_HEALTH] <= 0)
		{// He is dead!!!
			// Initialize! Will find a new spot next frame!
			bot->bot_coverspot_time = 0;
			VectorSet(bot->bot_coverspot, 0, 0, 0);
#ifdef __BOT_COVERSPOT_DEBUG__
			bot->s.dl_intensity = 0; // Debug sprite above head!
#endif //__BOT_COVERSPOT_DEBUG__
		}
		else if (VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) > 8/*16*/)
		{// Set up ideal view angles for this destination & move...
			//AIMOD_MOVEMENT_CoverSpot_AttackMove(bot, ucmd);
			VectorSubtract(bot->bot_coverspot, bot->r.currentOrigin, bot->move_vector);
			bot->move_vector[ROLL] = 0;
			vectoangles( bot->move_vector, bot->s.angles );
			ucmd->buttons |= BUTTON_SPRINT;
			ucmd->forwardmove = 127;
			ucmd->upmove = 0;
			ucmd->rightmove = 0;
#ifdef __BOT_COVERSPOT_DEBUG__
			bot->s.dl_intensity = 999; // Debug sprite above head!
#endif //__BOT_COVERSPOT_DEBUG__
		}
		else
		{// We are already there!
			//G_Printf("Bot %s found his cover spot!\n", bot->client->pers.netname);
			//G_Printf("Current pos %f %f %f.\n", bot->r.currentOrigin[0], bot->r.currentOrigin[1], bot->r.currentOrigin[2]);
			//G_Printf("Cover pos %f %f %f.\n", bot->bot_coverspot[0], bot->bot_coverspot[1], bot->bot_coverspot[2]);
			ucmd->buttons &= ~BUTTON_SPRINT;
			AIMOD_MOVEMENT_Attack( bot, ucmd );
#ifdef __BOT_COVERSPOT_DEBUG__
			bot->s.dl_intensity = 1000; // Debug sprite above head!
#endif //__BOT_COVERSPOT_DEBUG__
		}
	}
	else
	{
		if (AIMOD_SP_MOVEMENT_Is_CoverSpot_Weapon(bot->client->ps.weapon) 
			&& AIMOD_SP_MOVEMENT_Find_Cover_Spot( bot, ucmd ))
		{
			bot->bot_coverspot_enemy = bot->enemy;
			VectorCopy(bot->bot_coverspot_enemy->r.currentOrigin, bot->bot_coverspot_enemy_position);
#ifdef __BOT_COVERSPOT_DEBUG__
			//G_Printf("Bot %s found a cover spot!\n", bot->client->pers.netname);
			//G_Printf("Current pos %f %f %f.\n", bot->r.currentOrigin[0], bot->r.currentOrigin[1], bot->r.currentOrigin[2]);
			//G_Printf("Cover pos %f %f %f.\n", bot->bot_coverspot[0], bot->bot_coverspot[1], bot->bot_coverspot[2]);
			//G_Printf("Distance %f. HeightDiff %f.\n", VectorDistance(bot->r.currentOrigin, bot->bot_coverspot), HeightDistance(bot->r.currentOrigin, bot->bot_coverspot));
			bot->s.dl_intensity = 999; // Debug sprite above head!
#endif //__BOT_COVERSPOT_DEBUG__
		}
		else
		{
			bot->bot_coverspot_enemy = NULL;
			ucmd->buttons &= ~BUTTON_SPRINT;

			//G_Printf("Bot %s failed to find a cover spot!\n", bot->client->pers.netname);
			if (number_of_nodes > 0)
				AIMOD_MOVEMENT_AttackMove( bot, ucmd );
			else
				AIMOD_MOVEMENT_Attack( bot, ucmd );

			// Initialize!
			bot->bot_coverspot_time = 0;
			VectorSet(bot->bot_coverspot, 0, 0, 0);
#ifdef __BOT_COVERSPOT_DEBUG__
			bot->s.dl_intensity = 0; // Debug sprite above head!
#endif //__BOT_COVERSPOT_DEBUG__
		}
	}
}

//===========================================================================
// Routine      : AIMOD_SP_MOVEMENT_Pick_New_Destination

// Description  : Single Player AI Destination Selection
qboolean
AIMOD_SP_MOVEMENT_Pick_New_Destination ( gentity_t *bot, usercmd_t *ucmd )
{
	qboolean	good = qfalse;
	int			tries = 0;

	while (!good)
	{
		vec3_t up_pos, down_pos, good_pos;
		trace_t tr;
		int max_tries = (12*(bot_cpu_use.integer/100));

		if (max_tries < 1)
			max_tries = 1;

		if (tries > max_tries)
		{// Try again next frame!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->bot_sniping_time = level.time + 1000;
			return qfalse; 
		}

		tries++;

		VectorCopy(bot->bot_sp_spawnpos, bot->bot_sp_destination);
		bot->bot_sp_destination[0] += (512-(64*Q_TrueRand(0, 16)));
		bot->bot_sp_destination[1] += (512-(64*Q_TrueRand(0, 16)));

		VectorCopy(bot->bot_sp_destination, up_pos);
		up_pos[2]+=256;
		VectorCopy(bot->bot_sp_destination, down_pos);
		down_pos[2]-=256;

		trap_Trace( &tr, up_pos, NULL, NULL, down_pos, -1, MASK_SHOT | MASK_WATER );
			
		if ( tr.startsolid || 
			tr.allsolid  ||
			tr.contents & CONTENTS_STRUCTURAL ||
			tr.contents & CONTENTS_DETAIL ||
			tr.contents & CONTENTS_NODROP ||
			tr.contents & CONTENTS_STRUCTURAL ||
			tr.contents & CONTENTS_DONOTENTER_LARGE ||
			tr.contents & CONTENTS_DONOTENTER ||
			tr.contents & CONTENTS_MONSTERCLIP ||
			tr.contents & CONTENTS_PLAYERCLIP ||
			tr.contents & CONTENTS_MOVER ||
			tr.contents & CONTENTS_MISSILECLIP ||
			tr.contents & CONTENTS_SLIME ||
			tr.contents & CONTENTS_LAVA )
		{// Not a good end spot!
			continue;
		}

		VectorCopy(bot->bot_sp_destination, bot->bot_sp_destination);
		bot->bot_sp_destination[2]=tr.endpos[2]+1;

		VectorCopy(bot->bot_sp_destination, good_pos);
		good_pos[2]=tr.endpos[2]+8/*16*/;
			
		if ( HeightDistance(bot->r.currentOrigin, good_pos) < 64 
			&& SP_NodeVisible(bot->r.currentOrigin, good_pos, bot->s.number) )
		{
			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			good = qtrue;
			//G_Printf("%s - Position good!\n", bot->client->pers.netname);

			bot->bot_sniping_time = 0;
			bot->bot_stand_guard = 0;
			return qtrue;
		}
	}

	return qfalse;
}

//===========================================================================
// Routine      : AIMOD_SP_MOVEMENT_Move

// Description  : Single Player AI Movement (following per-bot node paths)
void
AIMOD_SP_MOVEMENT_Move ( gentity_t *bot, usercmd_t *ucmd )
{
	if ( !bot->bot_initialized )
	{// Initialization...
		VectorCopy(bot->bot_sp_spawnpos, bot->r.currentOrigin);
		G_SetOrigin(bot, bot->r.currentOrigin);
		bot->bot_initialized = qtrue;

		// Initialize move destination...
		VectorCopy(bot->r.currentOrigin, bot->bot_sp_destination);

		// UQ1: Not using these yet... Maybe not needed...
//		for ( i = -512; i <= 512; i+=128 )
//		{// Now we need to generate some waypoints...
//			for ( j = -512; j <= 512; j+=128 )
//			bot->bot_sp_waypoints	
//		}
	}

	if ( HeightDistance(bot->r.currentOrigin, bot->bot_sp_spawnpos) > 64 )
	{// Must have fallen down somewhere, set a new spawn pos...
		VectorCopy(bot->r.currentOrigin, bot->bot_sp_spawnpos);
	}

	if (bot->enemy)
	{// If this bot has an enemy, check cover spots!
		if ( Have_Close_Enemy( bot) &&
			bot->bot_enemy_visible_time > level.time &&
			bot->enemy == bot->bot_last_visible_enemy &&
			bot->bot_runaway_time < level.time )
		{	// Do attacking first...
			AIMOD_MOVEMENT_Attack( bot, ucmd );
			return;
		}
		else
		{// Use cover spots!
			AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
			return;
		}
	}

	// Actual movement here...
	if ( bot->bot_stand_guard > level.time )
	{// Stand guard here for a moment...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		bot->bot_sniping_time = level.time + 1000;
		return;
	}
	else if ( bot->bot_sniping_time > level.time )
	{// Ready to find a new destination...
		if (!AIMOD_SP_MOVEMENT_Pick_New_Destination( bot, ucmd ))
			return;
	}
	else if (VectorDistanceNoHeight(bot->r.currentOrigin, bot->bot_sp_destination) <= 16)
	{// Hit current waypoint...
		bot->bot_stand_guard = level.time + Q_TrueRand(2000, 6000);
		bot->bot_sniping_time = 0;
		//G_Printf("%s - Hit destination!\n", bot->client->pers.netname);
		return;
	}
	else if (VectorLength(bot->client->ps.velocity) < 12)
	{// Stuck against a wall or something, find a new destination...
		if (SP_NodeVisible(bot->r.currentOrigin, bot->bot_sp_destination, bot->s.number) >= 2)
		{// A door, open it!
			if ( Bot_Ignore_Use_Time[bot->s.clientNum] < level.time )
			{// Only use the door once each 2 secs max...
				Bot_Ignore_Use_Time[bot->s.clientNum] = level.time + 2000;
				Cmd_Activate_f( bot );
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;
			}
		}
		else if (!AIMOD_SP_MOVEMENT_Pick_New_Destination( bot, ucmd ))
			return;
	}

	if (!SP_NodeVisible(bot->r.currentOrigin, bot->bot_sp_destination, bot->s.number))
	{// Stop bots running into walls!
		//ucmd->forwardmove = 0;
		//ucmd->rightmove = 0;
		//ucmd->upmove = 0;
		//return;

		if (!AIMOD_SP_MOVEMENT_Pick_New_Destination( bot, ucmd ))
			return;
	}

	// Set up ideal view angles for this destination & move...
	VectorSubtract( bot->bot_sp_destination, bot->r.currentOrigin, bot->move_vector );
	bot->move_vector[ROLL] = 0;
	vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );
	ucmd->buttons |= BUTTON_WALKING;
	G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
}

//===========================================================================
// Routine      : AIMOD_MOVEMENT_Move

// Description  : Main movement (following node paths)
void
AIMOD_MOVEMENT_Move ( gentity_t *bot, usercmd_t *ucmd )
{
	qboolean	onLadder = qfalse;
	qboolean	ladderMove = qfalse;

	if ( g_gametype.integer <= GT_COOP )
	{
		AIMOD_SP_MOVEMENT_Move( bot, ucmd );
		return;
	}

	//BotDropToFloor( bot );
#ifdef __VEHICLES__
	if ( bot->client->ps.eFlags & EF_TANK )
	{
		AIMOD_MOVEMENT_VehicleMove( bot, ucmd );
		return;
	}
#endif //__VEHICLES__
	if ( bot->bot_stand_guard )
	{// A bot that is standing guard...
		// Initialize move destination...
		VectorCopy(bot->r.currentOrigin, bot->bot_sp_destination);
		bot->bot_initialized = qtrue;

		VectorCopy(bot->r.currentOrigin, bot->bot_sp_spawnpos);
		AIMOD_SP_MOVEMENT_Move( bot, ucmd );
		return;
	}

	if ( bot->bot_last_position_check < level.time - 4000 )
	{		// Check if we have moved in the last 10 seconds... (Are we stuck somewhere?)
		if
		(
			bot->goalentity &&
			!Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot")) &&
			VectorDistance( bot->goalentity->r.currentOrigin, bot->r.currentOrigin) < 256
		)
		{

		}
		else if ( VectorDistance( bot->bot_last_position, bot->r.currentOrigin) <= 64 )
		{	// We are stuck... Find a new route!
			bot->current_node = -1;
			bot->longTermGoal = -1;
			Bot_Set_New_Path( bot, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->bot_last_position_check = level.time;
			VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
			return;
		}

		bot->bot_last_position_check = level.time;
		VectorCopy( bot->r.currentOrigin, bot->bot_last_position );
	}

	if
	(
		nodes_loaded &&
		bot->goalentity &&
		!Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot")) //&& VectorDistance(bot->goalentity->r.currentOrigin, bot->r.currentOrigin) < 256
		&&
		VectorDistance( nodes[bot->longTermGoal].origin, bot->r.currentOrigin) < 24 //&& NodeVisible(bot->goalentity->r.currentOrigin, bot->r.currentOrigin, bot->s.number)
	)
	{														// A sniper... Make him look around in snipe mode for enemies...
		AIMOD_Movement_SniperMove( bot, ucmd );
		if 	(	(test_last_message[bot->s.number] < level.time - 3000) &&
				(bot_debug.integer >= BOT_DEBUG_REGULAR) )
		{
			G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^5's has found his sniper point.\n", bot->client->pers.netname );
			test_last_message[bot->s.number] = level.time;
		}

		return;
	}
	else if ( nodes_loaded && bot->goalentity && !Q_stricmp( bot->goalentity->classname, va( "bot_sniper_spot")) )
	{
		if	(	(test_last_message[bot->s.number] < level.time - 3000 ) &&
				( bot_debug.integer >= BOT_DEBUG_REGULAR ) )
		{
			G_Printf( "^2*** ^3BOT DEBUG^5: ^3%s^5's distance from his sniper point is ^7%f^5.\n",
					  bot->client->pers.netname, VectorDistance( bot->goalentity->r.currentOrigin, bot->r.currentOrigin) );
			test_last_message[bot->s.number] = level.time;
		}
	}

	if ( nodes_loaded && bot->current_node > 0 )
	{
		if ( !AIMOD_MOVEMENT_CurrentNodeReachable( bot) )
		{													// If our current node is not visible, then we need to try something else...
			bot->current_node = -1;
			Bot_Set_New_Path( bot, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
		}
		else
		{
			if ( bot->pathsize > 10 )
			{
				AIMOD_MOVEMENT_Shorten_Path( bot );
			}
		}
	}

	if ( bot->client->ps.weapon == WP_MOBILE_MG42_SET )
	{
		return;
	}

	if ( bot->health <= 0 )
	{
		return;
	}

	if ( bot->client->ps.pm_flags & PMF_LIMBO )
	{
		return;
	}

	if ( !bot->r.currentOrigin )
	{
		return;
	}

#ifdef __BOT_AAS__
	if ( trap_AAS_Initialized() )
	{														// If aas data is loaded, use it instead of nodes...
		return;
	}
#endif //__BOT_AAS__

	if ( bot->client->sess.playerType != PC_SOLDIER )
	{														// Soldiers shouldn't need special moves...
		if ( AIMOD_MOVEMENT_SpecialMoves( bot, ucmd) )
		{
			return;
		}
	}

#ifdef __WEAPON_AIM__
	if ( bot->client->ps.eFlags & EF_AIMING )
	{
		ucmd->buttons |= BUTTON_WALKING;
	}
	else
	{
		ucmd->buttons &= ~BUTTON_WALKING;
	}
#endif //__WEAPON_AIM__

	if ( g_gametype.integer > GT_COOP &&
		bot->enemy &&
		bot->beStillEng < level.time &&
		bot->beStill < level.time &&
		bot->bot_enemy_visible_time > level.time &&
		bot->enemy == bot->bot_last_visible_enemy &&
		bot->bot_runaway_time < level.time )
	{// If this bot has an enemy, check cover spots!
		if ( Have_Close_Enemy( bot) )
		{	// Do attacking first...
			AIMOD_MOVEMENT_Attack( bot, ucmd );
			return;
		}
		else
		{// Use cover spots!
			AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
			return;
		}
	}
	else if
	(
		bot->enemy &&
		bot->beStillEng < level.time &&
		bot->beStill < level.time &&
		bot->bot_enemy_visible_time > level.time &&
		bot->enemy == bot->bot_last_visible_enemy &&
		bot->bot_runaway_time < level.time
	)
	{														// Do attacking first...
		AIMOD_MOVEMENT_AttackMove( bot, ucmd );
		return;
	}

	// ---------------------------------------------
	// OK... First the movement for non-routed maps.
	// ---------------------------------------------
	if ( !Bot_Ignore_Use_Time[bot->s.clientNum] )
	{
		Bot_Ignore_Use_Time[bot->s.clientNum] = 0;
	}

#ifdef __SUPREMACY__
	if ( nodes_loaded && bot->current_node )
	{														// We have a routed map.. Do some flag capture checks...
		if
		(
			bot->client->ps.stats[STAT_FLAG_CAPTURE] > 0 &&
			g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != bot->client->sess.sessionTeam &&
			bot->current_node > 0
		)
		{
			/*if
			(
				!BotEntityCloserOnFuturePath( bot, &g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]]) &&
				!Have_Close_Enemy( bot)
			)
			{												// If we're capturing a flag in POP gametype don't move, just defend current pos...
				bot->bot_last_good_move_time = level.time;
				bot->node_timeout = level.time + TravelTime( bot );
				return;										// Don't move while capturing a flag...
			}*/

			vec3_t org;

			VectorCopy(g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.origin, org);
			org[0]+=64;
			org[1]+=64;

			// Initialize move destination...
			//VectorCopy(bot->r.currentOrigin, bot->bot_sp_destination);
			bot->bot_initialized = qtrue;

			VectorCopy(org, bot->bot_sp_spawnpos);
			AIMOD_SP_MOVEMENT_Move( bot, ucmd );
			return;
		}

		if
		(
			bot->enemy &&
			bot->beStillEng < level.time &&
			bot->beStill < level.time &&
			bot->bot_enemy_visible_time > level.time &&
			bot->enemy == bot->bot_last_visible_enemy &&
			bot->bot_runaway_time < level.time
		)
		{													// Do attacking first...
			AIMOD_MOVEMENT_AttackMove( bot, ucmd );
			return;
		}
	}
	else
	{
		if
		(
			bot->client->ps.stats[STAT_FLAG_CAPTURE] > 0 &&
			g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != bot->client->sess.sessionTeam
		)
		{
			/*if ( !Have_Close_Enemy( bot) )
			{												// If we're capturing a flag in POP gametype don't move, just defend current pos...
				bot->bot_last_good_move_time = level.time;
				bot->node_timeout = level.time + TravelTime( bot );
				return;										// Don't move while capturing a flag...
			}*/

			vec3_t org;

			VectorCopy(g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.origin, org);
			org[0]+=64;
			org[1]+=64;

			// Initialize move destination...
			//VectorCopy(bot->r.currentOrigin, bot->bot_sp_destination);
			bot->bot_initialized = qtrue;

			VectorCopy(org, bot->bot_sp_spawnpos);
			AIMOD_SP_MOVEMENT_Move( bot, ucmd );
			return;
		}

		if
		(
			bot->enemy &&
			bot->beStillEng < level.time &&
			bot->beStill < level.time &&
			bot->bot_enemy_visible_time > level.time &&
			bot->enemy == bot->bot_last_visible_enemy &&
			bot->bot_runaway_time < level.time
		)
		{													// Do attacking first...
			AIMOD_MOVEMENT_AttackMove( bot, ucmd );
			return;
		}
	}
#endif //__SUPREMACY__
	if ( AIMOD_MOVEMENT_Fallback( bot, ucmd) )
	{
		return;
	}

	if
	(
		bot->client->ps.serverCursorHint == HINT_LADDER ||
		bot->client->ps.torsoAnim == BOTH_CLIMB ||
		bot->client->ps.legsAnim == BOTH_CLIMB
	)
	{
		onLadder = qtrue;
	}

	if ( onLadder )
	{														// Always go up/down!
		if ( VectorDistance( nodes[bot->current_node].origin, bot->r.currentOrigin) < 16 )
		{
			bot->last_node = bot->current_node;
			bot->current_node = bot->next_node;
			bot->next_node = BotGetNextNode( bot );
		}

		ucmd->forwardmove = 127;

		// Set up ideal view angles for this enemy...
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		return;
	}

	// ----------------------------------------
	// OK... Now the movement for routed maps.
	// ----------------------------------------
	if ( bot->current_node < 0 )
	{
		bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		if ( bot->current_node < 0 )
		{
			bot->bot_random_move_time = level.time + 500;
		}
	}

	if (!SP_NodeVisible(bot->r.currentOrigin, nodes[bot->current_node].origin, bot->s.number))
	{// Stop bots running into walls!
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if ( bot->bot_random_move_time > level.time )
	{														// Get out of trouble with non-existant waypoint!
		AIMOD_MOVEMENT_Backup( bot, ucmd );
		return;
		// Initialize move destination...
		//VectorCopy(bot->r.currentOrigin, bot->bot_sp_destination);
		//bot->bot_initialized = qtrue;

		//VectorCopy(bot->r.currentOrigin, bot->bot_sp_spawnpos);
		//AIMOD_SP_MOVEMENT_Move( bot, ucmd );
		//return;
	}

	if ( !onLadder && BAD_WP_Height( nodes[bot->current_node].origin, bot->r.currentOrigin) && !onLadder )
	{														// Height diff is greater then plane distance... Block it and skip it..
		int loop = 0;
		int link = -1;
		for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
		{													// Find the link we last used and mark it...
			if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
			{												// Mark it as blocked...
				BOT_MakeLinkUnreachable( bot, bot->last_node, loop );
				break;
			}
		}

		//bot->current_node = -1;
		//bot->next_node = -1;
		//bot->last_node = -1;
		Bot_Set_New_Path( bot, ucmd );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

#ifdef __BOT_STRAFE__
	if ( bot->bot_strafe_right_timer >= level.time )
	{
		if ( VectorDistance( bot->bot_strafe_target_position, bot->r.currentOrigin) > 8 )
		{
			ucmd->forwardmove = 0;							//64;
			ucmd->upmove = 0;
			ucmd->rightmove = 64;
			return;
		}
		else
		{
			bot->bot_strafe_right_timer = 0;
			ucmd->upmove = 127;
		}
	}

	if ( bot->bot_strafe_left_timer >= level.time )
	{
		if ( VectorDistance( bot->bot_strafe_target_position, bot->r.currentOrigin) > 8 )
		{
			ucmd->forwardmove = 0;							//64;
			ucmd->upmove = 0;
			ucmd->rightmove = -64;
			return;
		}
		else
		{
			bot->bot_strafe_left_timer = 0;
			ucmd->upmove = 127;
		}
	}

	if ( bot->bot_duck_time > level.time )
	{
		if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
		{													// For special duck nodes, make the bot duck down...
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;
		}

		ucmd->forwardmove = 127;
		return;
	}
	else if ( bot->bot_strafe_left_time > level.time )
	{
		ucmd->rightmove = -48;
		return;
	}
	else if ( bot->bot_strafe_right_time > level.time )
	{
		ucmd->rightmove = 48;
		return;
	}
	else if ( bot->bot_jump_time > level.time )
	{
		ucmd->upmove = 127;

		//ucmd->forwardmove = 127;
		//return;
	}
	else if
		(
			bot->bot_last_good_move_time < level.time - 1000 &&
			bot->bot_last_move_fix_time < level.time - 1000 &&
			!onLadder
		)
	{														// The first type of route correction...
		int result = -1;

		// So we don't vis check too often...
		bot->bot_last_move_fix_time = level.time;
		result = AIMOD_MOVEMENT_ReachableBy( bot, nodes[bot->current_node].origin );
		if ( result == -1 )
		{													// -1 is Not reachable at all... Let's use some blind routing for a short time!
			/*bot->bot_blind_routing_time = level.time + 6000;
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
			ucmd->forwardmove = 0;
			ucmd->upmove = 0;
			ucmd->rightmove = 0;
			Bot_Set_New_Path(bot, ucmd);*/

			//bot->bot_last_move_fix_time = level.time;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;

			//bot->next_node = -1;
			//bot->current_node = -1;
			//bot->last_node = -1;
			bot->longTermGoal = -1;
			bot->shortTermGoal = -1;
			return;
		}

		if ( result == 1 )
		{													// 1 is Jump...
			ucmd->upmove = 127;

			//ucmd->forwardmove = 127;
			bot->bot_jump_time = level.time + 500;

			//return;
		}

		if ( result == 2 )
		{													// 2 is Duck...
			if ( !(bot->client->ps.pm_flags & PMF_DUCKED) )
			{												// For special duck nodes, make the bot duck down...
				ucmd->upmove = -48;
				bot->client->ps.pm_flags |= PMF_DUCKED;
				bot->bot_duck_time = level.time + 2000;
			}
		}

		if ( result == 3 )
		{													// 3 is Strafe Left...
			ucmd->rightmove = -48;
			bot->bot_strafe_left_time = level.time + 1000;
			return;
		}

		if ( result == 4 )
		{													// 4 is Strafe Right...
			ucmd->rightmove = 48;
			bot->bot_strafe_right_time = level.time + 1000;
			return;
		}
	}
#endif //__BOT_STRAFE__
	if
	(
		bot->longTermGoal < 0 ||
		bot->longTermGoal > number_of_nodes ||
		bot->current_node < 0 ||
		bot->node_timeout <= level.time ||
		bot->bot_last_good_move_time < level.time - 2000
	)
	{														// We hit a problem in route, or don't have one yet.. Find a new goal...
		if
		(
			!onLadder &&
			!(
				bot->longTermGoal < 0 ||
				bot->longTermGoal > number_of_nodes ||
				bot->current_node < 0 ||
				bot->next_node < 0
			) &&
			(bot->node_timeout <= level.time || bot->bot_last_good_move_time < level.time - 2000)
		)
		{													// Bad current path link.. Mark it as blocked...
			int loop = 0;
			int link = -1;
			for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
			{												// Find the link we last used and mark it...
				if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
				{											// Mark it as blocked...
					BOT_MakeLinkUnreachable( bot, bot->last_node, loop );
					break;
				}
			}

			if
			(
				(bot->client->sess.sessionTeam == TEAM_AXIS && numAxisOnlyNodes > 0) ||
				(bot->client->sess.sessionTeam == TEAM_ALLIES && numAlliedOnlyNodes > 0)
			)
			{
				bot->pathsize = CreateTeamRoute( bot, bot->pathlist );
				if ( bot->pathsize > 0 )
				{

					//G_Printf("!!! %s is attempting to use team nodes !!!\n", bot->client->pers.netname);
					bot->next_node = BotGetNextNode( bot ); //move to this node first, since it's where our path starts from
					bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
					bot->bot_last_good_move_time = level.time;
					return;
				}
				else
				{
					bot->longTermGoal = -1;
					bot->shortTermGoal = -1;
					return;
				}
			}
		}

		Bot_Set_New_Path( bot, ucmd );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		bot->node_timeout = level.time + TravelTime( bot );				//6000;//4000;
		bot->bot_last_good_move_time = level.time;
		return;
	}

	if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
	{			// Try to roam out of problems...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		bot->longTermGoal = -1;
		bot->shortTermGoal = -1;
		return; // next think...
	}

	if ( nodes[bot->current_node].type & NODE_DUCK && !(bot->client->ps.pm_flags & PMF_DUCKED) )
	{			// For special duck nodes, make the bot duck down...
		ucmd->upmove = -48;
		bot->client->ps.pm_flags |= PMF_DUCKED;
	}
	else if ( nodes[bot->last_node].type & NODE_DUCK && !(bot->client->ps.pm_flags & PMF_DUCKED) )
	{			// For special duck nodes, make the bot duck down...
		ucmd->upmove = -48;
		bot->client->ps.pm_flags |= PMF_DUCKED;
	}
	else if
		(
			!(nodes[bot->current_node].type & NODE_DUCK) &&
			!(nodes[bot->last_node].type & NODE_DUCK) &&
			!bot->enemy &&
			(bot->client->ps.eFlags & EF_CROUCHING)
		)
	{			// Get up?
		ucmd->upmove = 127;
		bot->client->ps.eFlags &= ~EF_CROUCHING;
	}
	else if ( !bot->enemy && (bot->client->ps.eFlags & EF_PRONE) )
	{			// Get up?
		ucmd->upmove = 127;
		bot->client->ps.eFlags &= ~EF_PRONE;
	}

	// Now we need to check if we have hit our long term node...
	if ( bot->current_node == bot->longTermGoal && AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
	{			// We're there! Find a new goal... FIXME: We should actually do something at most goals..
		if ( bot->followtarget && VectorDistance( bot->followtarget->r.currentOrigin, bot->r.currentOrigin) < 256 )
		{		// A following bot, at it's owner... Just keep standing with him for now...
			//FIXME: Make them move around just a little near their owner... Maybe some anims/sounds??? - Unique1
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;

			//			bot->longTermGoal = -1;
			//			bot->shortTermGoal = -1;
			bot->node_timeout = level.time + ( TravelTime( bot) * 2 );		// Duck moves take longer, so...

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
			return;															// Think again soon!
		}

		if
		(
			bot->bot_defender &&
			!(bot->bot_goal_constructible && bot->client->sess.sessionTeam == level.attackingTeam)
		)
		{
			if ( !(BotMP_Num_Defenders_Near( bot->bot_goal_position, bot) > 2) )
			{
				bot->bot_stand_guard = qtrue;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				bot->node_timeout = level.time + ( TravelTime( bot) * 2 );	// Duck moves take longer, so...

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;
				return;
			}
		}

		if ( bot->goalentity && bot->goalentity->s.eType == ET_DEFENCEPOINT_HINT )
		{
			if ( !(BotMP_Num_Defenders_Near( bot->goalentity->s.origin, bot) > (bot_minplayers.integer * 0.35)) )
			{
				bot->bot_stand_guard = qtrue;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				bot->node_timeout = level.time + ( TravelTime( bot) * 2 );	// Duck moves take longer, so...

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;
				return;
			}
		}

		if ( bot->longterm_pathsize > 0 )
		{
			int i;
			bot->pathsize = bot->longterm_pathsize;
			for ( i = 0; i < bot->pathsize; i++ )
			{
				bot->pathlist[i] = bot->longterm_pathlist[i];
			}

			bot->current_node = bot->shortTermGoal;
			bot->next_node = BotGetNextNode( bot );
			bot->longterm_pathsize = -1;
			bot->shortTermGoal = -1;
		}
		else
		{
			Bot_Set_New_Path( bot, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->node_timeout = level.time + TravelTime( bot );				//6000;//4000;
			bot->bot_last_good_move_time = level.time;
			return;
		}
	}

	// Now we need to check if we have hit our current node...
	if ( AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
	{	// At current node.. Pick next in the list...
		bot->last_node = bot->current_node;
		bot->current_node = bot->next_node;
		bot->next_node = BotGetNextNode( bot );
		bot->node_timeout = level.time + TravelTime( bot );
		if ( bot->current_node > number_of_nodes )
		{
			bot->current_node = -1;
			bot->next_node = -1;
			return;
		}

		if ( nodes[bot->current_node].type & NODE_DUCK )
		{
			bot->node_timeout = level.time + ( TravelTime( bot) * 2 );	// Duck moves take longer, so...
		}

#ifdef __VEHICLES__
		if ( bot->client->ps.eFlags & EF_TANK )
		{
			bot->node_timeout = level.time + ( TravelTime( bot) * 4 );	// Tank angles take longer, so...
		}
#endif //__VEHICLES__

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		bot->node_timeout = level.time + TravelTime( bot );
	}

	if ( nodes[bot->current_node].origin[2] > bot->r.currentOrigin[2] && !onLadder )
	{	// Do we need to jump to our next node?
		vec3_t	temp, temp2;
		VectorCopy( nodes[bot->current_node].origin, temp );
		temp[2] = bot->r.currentOrigin[2];
		VectorCopy( nodes[bot->current_node].origin, temp2 );
		temp2[2] = bot->r.currentOrigin[2] + 16;
		if ( nodes[bot->current_node].origin[2] - bot->r.currentOrigin[2] > 16	 /*&& VectorLength(bot->client->ps.velocity) < 24*/ )
		{
			ucmd->upmove = 127;
		}
	}

#ifdef __BOT_STRAFE__
	if
	(
		bot->bot_strafe_right_timer < level.time &&
		bot->bot_strafe_left_timer < level.time &&
		VectorLength( bot->client->ps.velocity) < 24 &&
		!(nodes[bot->current_node].type & NODE_DUCK) &&
		!(nodes[bot->last_node].type & NODE_DUCK) &&
		!onLadder
#ifdef __VEHICLES__
		&&
		!(bot->client->ps.eFlags & EF_TANK)
#endif //__VEHICLES__
	)
	{	// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
		AIMOD_MOVEMENT_Setup_Strafe( bot, ucmd );
	}

	if ( bot->bot_strafe_right_timer >= level.time || bot->bot_strafe_left_timer >= level.time )
	{
		return;
	}
#endif //__BOT_STRAFE__

	// Set up ideal view angles for this enemy...
	VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
	bot->move_vector[ROLL] = 0;
	vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );

	//	if (JobType(bot) <= BOT_JOB_NONE)
	//		ucmd->buttons |= BUTTON_WALKING;
	if ( bot->bot_turn_wait_time <= level.time )
	{
		G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
	}

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;

	//G_Printf("%s (%i) is moving.\n", bot->client->pers.netname, bot->s.number);
	return;
}


//===========================================================================
// Routine      : AIMOD_Combat_Weapons_Check

// Description  : Check bot's weapon during attacks...
void
AIMOD_Combat_Weapons_Check ( gentity_t *bot, usercmd_t *ucmd )
{
/*	if
	(
		bot->client->ps.weapon == WP_FG42 &&
		(VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024 || (bot->bot_coverspot_time > level.time && VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) < 8))
	)
	{
		if ( COM_BitCheck( bot->client->ps.weapons, WP_FG42SCOPE) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_FG42SCOPE) )
		{
			if (bot->client->ps.weapon != WP_FG42SCOPE)
			{
				ucmd->weapon = WP_FG42SCOPE;
				next_weapon_change[bot->s.clientNum] = level.time + 3000;
			}
		}
	}
	else if
		(
			bot->client->ps.weapon == WP_K43 &&
			(VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024 || (bot->bot_coverspot_time > level.time && VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) < 8))
		)
	{
		if ( COM_BitCheck( bot->client->ps.weapons, WP_K43_SCOPE) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_K43_SCOPE) )
		{
			if (bot->client->ps.weapon != WP_K43_SCOPE)
			{
				ucmd->weapon = WP_K43_SCOPE;
				next_weapon_change[bot->s.clientNum] = level.time + 3000;
			}
		}
	}
	else if
		(
			bot->client->ps.weapon == WP_GARAND &&
			(VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024 || (bot->bot_coverspot_time > level.time && VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) < 8))
		)
	{
		if ( COM_BitCheck( bot->client->ps.weapons, WP_GARAND_SCOPE) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_GARAND_SCOPE) )
		{
			if (bot->client->ps.weapon != WP_GARAND_SCOPE)
			{
				ucmd->weapon = WP_GARAND_SCOPE;
				next_weapon_change[bot->s.clientNum] = level.time + 3000;
			}
		}
	}

	if
	(
		COM_BitCheck( bot->client->ps.weapons, WP_SMOKE_MARKER) &&
		next_weapon_change[bot->s.clientNum] < level.time &&
		bot->enemy &&
		bot->client->sess.playerType == PC_FIELDOPS &&
		VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) < 512
	)
	{	// Switch to a grenade if close to an enemy...
		if (bot->client->ps.weapon != WP_SMOKE_MARKER)
		{
			ucmd->weapon = WP_SMOKE_MARKER;
			next_weapon_change[bot->s.clientNum] = level.time + 3000;
		}
	}

	// No more ammo, but we can reload, so RELOAD !
	if
	(
		next_weapon_change[bot->s.clientNum] < level.time &&
		(bot->client->ps.weapon == WP_FG42SCOPE ||
		bot->client->ps.weapon == WP_K43_SCOPE ||
		bot->client->ps.weapon == WP_GARAND_SCOPE)
	)
	{
		if ( bot->client->ps.ammoclip[bot->client->ps.weapon] == 0 && bot->client->ps.ammo[bot->client->ps.weapon] > 0 )
		{
			if ( bot->client->ps.weapon == WP_FG42SCOPE )
			{
				if (bot->client->ps.weapon != WP_FG42)
				{
					ucmd->weapon = WP_FG42;
					next_weapon_change[bot->s.clientNum] = level.time + 3000;
				}
			}

			if ( bot->client->ps.weapon == WP_K43_SCOPE )
			{
				if (bot->client->ps.weapon != WP_K43)
				{
					ucmd->weapon = WP_K43;
					next_weapon_change[bot->s.clientNum] = level.time + 3000;
				}
			}

			if ( bot->client->ps.weapon == WP_GARAND_SCOPE )
			{
				if (bot->client->ps.weapon != WP_GARAND)
				{
					ucmd->weapon = WP_GARAND;
					next_weapon_change[bot->s.clientNum] = level.time + 3000;
				}
			}

			ucmd->wbuttons = WBUTTON_RELOAD;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;

			if (bot->bot_coverspot_time > level.time)
			{
				bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
				bot->client->ps.pm_flags |= PMF_DUCKED;
				ucmd->upmove = -127;
			}

			next_weapon_change[bot->s.clientNum] = level.time + 3000;
			return;
		}
	}*/
}


//===========================================================================
// Routine      : AIMOD_AttackMovement_Fallback

// Description  : Fallback movement while attacking...
qboolean
AIMOD_AttackMovement_Fallback ( gentity_t *bot, usercmd_t *ucmd )
{															// Fallback navigation. returns if it was used or not as qtrue/qfalse.
	vec3_t	dir;
	if ( NodesLoaded() == qfalse || bot_forced_auto_waypoint.integer )
	{														// Random for unrouted maps.
		if ( VectorLength( bot->client->ps.velocity) < 64 )
		{													// We havn't moved at all... Let's randomize things.
			vec3_t	temp;
			if ( Bot_Ignore_Use_Time[bot->s.clientNum] < level.time && no_mount_time[bot->s.clientNum] < level.time )
			{
				Cmd_Activate_f( bot );
			}

			VectorCopy( bot->move_vector, temp );
			temp[0] = bot->move_vector[0];
			temp[2] = bot->move_vector[2];
			temp[1] = bot->move_vector[1] - rand() % 360;
			bot->client->AImovetime = level.time + 10000;	// 10 seconds?
			VectorCopy( temp, bot->move_vector );
			VectorSubtract( temp, bot->r.currentOrigin, dir );
			if ( bot /*VectorNormalize( dir ) > 32*/ )
			{
				if ( bot->bot_turn_wait_time <= level.time )
				{
					G_UcmdMoveForDir( bot, ucmd, dir );
				}
			}
			else
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
			}
		}
		else
		{
			VectorSubtract( bot->move_vector, bot->r.currentOrigin, dir );
			if ( bot /*VectorNormalize( dir ) > 32*/ )
			{
				if ( bot->bot_turn_wait_time <= level.time )
				{
					G_UcmdMoveForDir( bot, ucmd, dir );
				}
			}
			else
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
			}
		}

		if ( bot->waterlevel )
		{
			ucmd->upmove = 120;								// Try to always be on surface...
		}

		return ( qtrue );
	}

	return ( qfalse );
}

#ifndef __BOT_PERFECTION__


//===========================================================================
// Routine      : AIMOD_AttackMovement_From_Void

// Description  : Attack movement while in a void...
void
AIMOD_AttackMovement_From_Void ( gentity_t *bot, usercmd_t *ucmd )
{														// Forced fallback navigation (out of voids)...
	vec3_t	dir;
	if ( VectorLength( bot->client->ps.velocity) < 64 )
	{													// We havn't moved at all... Let's randomize things.
		vec3_t	temp;
		if ( Bot_Ignore_Use_Time[bot->s.clientNum] < level.time && no_mount_time[bot->s.clientNum] < level.time )
		{
			Cmd_Activate_f( bot );
		}

		VectorCopy( bot->move_vector, temp );
		temp[0] = bot->move_vector[0];
		temp[2] = bot->move_vector[2];
		temp[1] = bot->move_vector[1] - rand() % 360;
		bot->client->AImovetime = level.time + 10000;	// 10 seconds?
		VectorCopy( temp, bot->move_vector );
		VectorSubtract( temp, bot->r.currentOrigin, dir );
		if ( bot /*VectorNormalize( dir ) > 32*/ )
		{
			if ( bot->bot_turn_wait_time <= level.time )
			{
				G_UcmdMoveForDir( bot, ucmd, dir );
			}
		}
		else
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
		}
	}
	else
	{
		VectorSubtract( bot->move_vector, bot->r.currentOrigin, dir );
		if ( bot /*VectorNormalize( dir ) > 32*/ )
		{
			if ( bot->bot_turn_wait_time <= level.time )
			{
				G_UcmdMoveForDir( bot, ucmd, dir );
			}
		}
		else
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
		}
	}

	if ( bot->waterlevel )
	{
		ucmd->upmove = 120;								// Try to always be on surface...
	}
}
#endif

//===========================================================================
// Routine      : AIMOD_MOVEMENT_AttackMove

// Description  : Movement while attacking...
void
AIMOD_MOVEMENT_AttackMove ( gentity_t *bot, usercmd_t *ucmd )
{
	gentity_t	*self = bot;
	float		firedelay;
	vec3_t		target;
	vec3_t		attackvector;
	float		dist;
	vec3_t		ang, dir;
	int			skill;
	qboolean	onLadder = qfalse;

	if (g_gametype.integer <= GT_COOP)
		bot->client->ps.ammo[bot->client->ps.weapon] = 32; // SP Bots always have ammo!

	// Cover spots!
//	if (bot->bot_coverspot_time > level.time)
//	{
//		if (VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) > 16)
//		{
//			AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
//			return;
//		}
//	}

#ifdef __VEHICLES__
	if ( bot->client->ps.eFlags & EF_TANK )
	{
		AIMOD_MOVEMENT_VehicleAttack( bot, ucmd );
		return;
	}
#endif //__VEHICLES__
	if ( bot->bot_stand_guard )
	{															// A bot that is standing guard...
		if
		(
			bot->beStillEng < level.time &&
			bot->beStill < level.time &&
			bot->bot_enemy_visible_time > level.time &&
			bot->enemy == bot->bot_last_visible_enemy &&
			bot->bot_runaway_time < level.time
		)
		{
			if ( bot->enemy /*Have_Close_Enemy(bot)*/ )
			{													// Do attacking first...
				AIMOD_MOVEMENT_Attack( bot, ucmd );
			}
			else
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				bot->node_timeout = level.time + 6000;			// Duck moves take longer, so...

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;
			}
		}
		else
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->node_timeout = level.time + 6000;				// Duck moves take longer, so...

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
		}

		return;
	}

	//	if (bot->client->sess.playerType != PC_SOLDIER) // Soldiers shouldn't need special moves...
	//		if (AIMOD_MOVEMENT_SpecialMoves(bot, ucmd))
	//			return;
	//
	// Attack Code First!!!
	//
	if
	(
		bot->beStill > level.time &&
		!(bot->client && (bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK))
	)
	{
		return;
	}

	if ( g_gametype.integer >= GT_WOLF )
	{
		skill = bot_skill.integer;
	}
	else
	{
		skill = bot_skill.integer;
	}

	if ( skill <= 0 )
	{
		skill = 1;
	}

	if ( !bot->enemy )
	{
		AIMOD_MOVEMENT_Move( bot, ucmd );
		return;
	}

	VectorSubtract( bot->r.currentOrigin, bot->enemy->r.currentOrigin, dir );
	vectoangles( dir, ang );
	if ( bot->bot_enemy_visible_time < level.time /*|| bot->enemy != bot->bot_last_visible_enemy*/ )
	{
		AIMOD_MOVEMENT_Move( bot, ucmd );
		return;													// Shouldn't have got this far...
	}

	// Give them extra time to get to next node...
	bot->node_timeout = level.time + TravelTime( bot );

	// Set up ideal view angles for this enemy...
	VectorSubtract( bot->enemy->r.currentOrigin, bot->r.currentOrigin, attackvector );
	vectoangles( attackvector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );
	dist = VectorLength( attackvector );
	Set_Best_AI_weapon( bot );
	AIMOD_Combat_Weapons_Check( bot, ucmd );
	
	if ( bot->client->ps.ammoclip[bot->client->ps.weapon] == 0 && bot->client->ps.ammo[bot->client->ps.weapon] > 0 )
	{
		int weaponChoice = bot->client->ps.weapon;

		if ( bot->client->ps.weapon == WP_FG42SCOPE )
		{
			weaponChoice = WP_FG42;
		}

		if ( bot->client->ps.weapon == WP_K43_SCOPE )
		{
			weaponChoice = WP_K43;
		}

		if ( bot->client->ps.weapon == WP_GARAND_SCOPE )
		{
			weaponChoice = WP_GARAND;
		}

		if ( bot->client->ps.weapon == WP_GPG40 )
		{
			weaponChoice = WP_KAR98;
		}

		if ( bot->client->ps.weapon == WP_M7 )
		{
			weaponChoice = WP_CARBINE;
		}

		if (bot->client->ps.weapon != weaponChoice)
			ucmd->weapon = weaponChoice;

		ucmd->wbuttons = WBUTTON_RELOAD;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		if (bot->bot_coverspot_time > level.time)
		{
			bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
			bot->client->ps.pm_flags |= PMF_DUCKED;
			ucmd->upmove = -127;
		}
		return;
	}

	if ( ucmd->wbuttons == WBUTTON_RELOAD )
	{															// Reloading!
		int weaponChoice = bot->client->ps.weapon;

		if ( bot->client->ps.weapon == WP_FG42SCOPE )
		{
			weaponChoice = WP_FG42;
		}

		if ( bot->client->ps.weapon == WP_K43_SCOPE )
		{
			weaponChoice = WP_K43;
		}

		if ( bot->client->ps.weapon == WP_GARAND_SCOPE )
		{
			weaponChoice = WP_GARAND;
		}

		if ( bot->client->ps.weapon == WP_GPG40 )
		{
			weaponChoice = WP_KAR98;
		}

		if ( bot->client->ps.weapon == WP_M7 )
		{
			weaponChoice = WP_CARBINE;
		}

		if (bot->client->ps.weapon != weaponChoice)
			ucmd->weapon = weaponChoice;

		ucmd->upmove = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;

		if (bot->bot_coverspot_time > level.time)
		{
			bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
			bot->client->ps.pm_flags |= PMF_DUCKED;
			ucmd->upmove = -127;
		}
		return;
	}

	if (														/*(ucmd->weapon == WP_GRENADE_LAUNCHER && bot->client->ps.weapon != WP_GRENADE_LAUNCHER)
		|| (ucmd->weapon == WP_GRENADE_PINEAPPLE && bot->client->ps.weapon != WP_GRENADE_LAUNCHER) 
		||*/
			(ucmd->weapon == WP_SMOKE_MARKER && bot->client->ps.weapon != WP_SMOKE_MARKER)
		)
	{															// Wait until the animation has been completed...
		ucmd->upmove = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		return;
	}

	if ( ucmd->weapon && ucmd->weapon != bot->client->ps.weapon )
	{															// Wait until the animation has been completed...
		ucmd->upmove = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		return;
	}

	if ( bot->client->ps.weapon == WP_KNIFE )
	{															// Bots with a knife should always run at the enemy.
		ucmd->forwardmove = 120;
		if ( VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 64 )
		{
			return;												// And don't try to stab until we are close enough...
		}
	}
	else
	{
		if (bot->bot_coverspot_time > level.time)
		{
			if (bot->client->ps.eFlags & EF_PRONE)
				bot->client->ps.eFlags &= ~EF_PRONE;

			if (bot->client->ps.pm_flags & PMF_DUCKED)
				bot->client->ps.pm_flags &= ~PMF_DUCKED;

			bot->bot_coverspot_time = level.time + 20000;

			if (bot->bot_coverspot_staydown_timer > level.time)
			{// Stay down for a bit longer!
				bot->client->ps.pm_flags |= PMF_DUCKED;
				ucmd->upmove = -127;
				return;
			}
			else
			{// Should we duck behind our cover for a while???
				if (Q_TrueRand(0, 20000) > 19900)
				{// Pick how long to stay down for!
					bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
					bot->client->ps.pm_flags |= PMF_DUCKED;
					ucmd->upmove = -127;
					return;
				}
			}
	
		}
		else if ( bot->skillchoice >= 4 && dist > 800 /*600*/ )
		{														// Elite bots.
			if ( !(bot->client->ps.eFlags & EF_PRONE) && !(bot->client->ps.pm_flags & PMF_DUCKED) )
			{													// We should only vischeck this once at the first attack time...
				ucmd->upmove = -127;							// Crouch?

				// Only go prone if we can still hit them from down there...
				if ( visible( bot, bot->enemy) )
				{
					bot->client->ps.eFlags |= EF_PRONE;
				}
				else
				{
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}

				//	ucmd->upmove = 0; // jaquboss - reseting this will make bots go up again
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
			}
		}
		else if ( bot->skillchoice >= 2 && dist > 1024 /*800*/ )
		{														// Good bots.
			if ( !(bot->client->ps.eFlags & EF_PRONE) && !(bot->client->ps.pm_flags & PMF_DUCKED) )
			{													// We should only vischeck this once at the first attack time...
				ucmd->upmove = -127;							// Crouch?

				// Only go prone if we can still hit them from down there...
				if ( visible( bot, bot->enemy) )
				{
					bot->client->ps.eFlags |= EF_PRONE;
				}
				else
				{
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}

				//	ucmd->upmove = 0; // jaquboss - reseting this will make bots go up again
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
			}
		}
		else if ( dist > 1024 /*800*/ /*256*/ && !(bot->client->ps.pm_flags & PMF_DUCKED) )
		{														// Use long aim ! DUCK ! IF you don't have the flag ! That is
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;

			//	ucmd->upmove = 0; // jaquboss - reseting this will make bots go up again
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
		}
	}

	bot->node_timeout = level.time + TravelTime( bot );			//6000;//4000;
	bot->bot_last_good_move_time = level.time;

	// Set the attack
	//-------------------------------------------------------
	firedelay = ( random() - 0.5 ) * 5;

	// Aim
	//-------------------------------------------------------
	VectorCopy( bot->enemy->r.currentOrigin, target );

	//-------------------------------------------------------
	// Modify attack angles based on accuracy
	// added skill level
	//-------------------------------------------------------
	if ( bot->skillchoice == 0 )
	{

		// Can't hit a good strafing player
		int randm;
		randm = Q_TrueRand( 0, 1 );
		if ( randm == 0 )
		{
			target[0] += ( random() - 0.5 ) * 20;
		}
		else
		{
			target[0] -= ( random() - 0.5 ) * 20;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[1] += ( random() - 0.5 ) * 20;
		}
		else
		{
			target[1] -= ( random() - 0.5 ) * 20;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[2] += ( random() - 0.5 ) * 20;
		}
		else
		{
			target[2] -= ( random() - 0.5 ) * 20;
		}
	}
	else if ( bot->skillchoice == 1 )
	{

		// Will get ya every now and then
		int randm;
		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[0] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[0] -= ( random() - 0.5 ) * 10;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[1] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[1] -= ( random() - 0.5 ) * 10;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[2] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[2] -= ( random() - 0.5 ) * 10;
		}
	}
	else if ( bot->skillchoice == 2 )
	{

		// Good competetion for some of the best players
		int randm;
		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[0] += ( random() - 0.5 ) * 5;
		}
		else
		{
			target[0] -= ( random() - 0.5 ) * 5;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[1] += ( random() - 0.5 ) * 5;
		}
		else
		{
			target[1] -= ( random() - 0.5 ) * 5;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[2] += ( random() - 0.5 ) * 5;
		}
		else
		{
			target[2] -= ( random() - 0.5 ) * 5;
		}
	}
	else if ( bot->skillchoice >= 3 )
	{

		// Never misses, unloads on ya!
		//target[0] = target[0];
		//target[1] = target[1];
	}

	//-------------------
	if ( bot->client && (bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK) )
	{															// Randomize attack dir a little more while on an MG42 or Tank...
		int randm;
		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[0] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[0] -= ( random() - 0.5 ) * 10;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[1] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[1] -= ( random() - 0.5 ) * 10;
		}

		randm = Q_TrueRand( 0, 1 );
		if ( rand == 0 )
		{
			target[2] += ( random() - 0.5 ) * 10;
		}
		else
		{
			target[2] -= ( random() - 0.5 ) * 10;
		}
	}

	// If they are not standing, we need to lower our shots a little!
	if ( bot->enemy->client && bot->client->ps.eFlags & EF_PRONE && bot->enemy->client->ps.eFlags & EF_PRONE )
	{
		target[2] -= Q_TrueRand( 0, 16 );;
	}
	else if ( bot->enemy->client && bot->enemy->client->ps.eFlags & EF_PRONE )
	{
		target[2] -= Q_TrueRand( 48, 62 );
	}
	else if ( bot->enemy->client && bot->enemy->client->ps.pm_flags & PMF_DUCKED )
	{
		target[2] -= Q_TrueRand( 32, 48 );
	}

	// Set direction
	//-------------------------------------------------------
	VectorSubtract( target, bot->r.currentOrigin, bot->attack_vector );
	if ( bot->client->ps.weapon == WP_SMOKE_MARKER )
	{
		bot->attack_vector[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfAirstrike( bot );
	}

	if ( bot->client->ps.weapon == WP_GPG40 || bot->client->ps.weapon == WP_M7 )
	{
		bot->attack_vector[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfGrenade( bot );
	}

	if ( bot->client->ps.weapon == WP_GRENADE_LAUNCHER || bot->client->ps.weapon == WP_GRENADE_PINEAPPLE )
	{
		bot->attack_vector[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfGrenade( bot );
	}

	vectoangles( bot->attack_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );

	// Added Skill Level Settings for Attack
	if ( VectorDistance( bot->bot_ideal_view_angles, bot->client->ps.viewangles) <= 16 )
	{
		if ( bot->skillchoice == 0 )
		{
			if ( firedelay < 0.10 )
			{
				ucmd->buttons = BUTTON_ATTACK;
			}
		}
		else if ( bot->skillchoice == 1 )
		{
			if ( firedelay < 0.20 )
			{
				ucmd->buttons = BUTTON_ATTACK;
			}
		}
		else if ( bot->skillchoice == 2 )
		{
			if ( firedelay < 0.40 )
			{
				ucmd->buttons = BUTTON_ATTACK;
			}
		}
		else if ( bot->skillchoice >= 3 )
		{
			ucmd->buttons = BUTTON_ATTACK;
		}
	}

	//-------------------------------------------------------
	if
	(
		bot->client->ps.weapon == WP_MEDKIT ||
		bot->client->ps.weapon == WP_AMMO ||
		bot->client->ps.weapon == WP_PLIERS ||
		bot->client->ps.weapon == WP_DYNAMITE
	)
	{
		ucmd->buttons = 0;
	}

	bot->teamPauseTime = level.time;

	// Prototype team enemy handling...
	//	AIMOD_InformTeamOfEnemy(bot);
	if ( (bot->client->ps.eFlags & EF_PRONE) || (bot->client->ps.pm_flags & PMF_DUCKED) )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

#ifdef __VEHICLES__
	if ( bot->client->ps.eFlags & EF_TANK )
	{															// Don't move while firing in a tank...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}
#endif //__VEHICLES__

	// Cover spots!
	if (bot->bot_coverspot_time > level.time)
	{
		if (VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) <= 16)
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
		}
	}

	//
	// End of Attack Code!!!
	//
	//
	// Now the movement code!
	//
	if ( bot->client->ps.eFlags & EF_ZOOMING )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if
	(
		bot->client->ps.weapon == WP_FG42SCOPE ||
		bot->client->ps.weapon == WP_K43_SCOPE ||
		bot->client->ps.weapon == WP_GARAND_SCOPE
	)
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if ( bot->client->ps.weapon == WP_MOBILE_MG42_SET )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

#ifdef __BOT_AAS__
	if ( trap_AAS_Initialized() )
	{															// If aas data is loaded, use it instead of nodes...
		return;
	}
#endif //__BOT_AAS__
#ifdef __WEAPON_AIM__
	if ( bot->client->ps.eFlags & EF_AIMING )
	{
		ucmd->buttons |= BUTTON_WALKING;
	}
	else
	{
		ucmd->buttons &= ~BUTTON_WALKING;
	}
#endif //__WEAPON_AIM__

	// ---------------------------------------------
	// OK... First the movement for non-routed maps.
	// ---------------------------------------------
	if ( !Bot_Ignore_Use_Time[bot->s.clientNum] )
	{
		Bot_Ignore_Use_Time[bot->s.clientNum] = 0;
	}

	if ( AIMOD_AttackMovement_Fallback( bot, ucmd) )
	{
		return;
	}

	if ( bot->client->ps.serverCursorHint == HINT_LADDER )
	{
		onLadder = qtrue;
	}

	// ----------------------------------------
	// OK... Now the movement for routed maps.
	// ----------------------------------------
#ifdef __BOT_STRAFE__
	if ( bot->bot_strafe_right_timer >= level.time )
	{
		if ( VectorDistance( bot->bot_strafe_target_position, bot->r.currentOrigin) > 8 )
		{
			ucmd->forwardmove = 0;								//64;
			ucmd->upmove = 0;
			ucmd->rightmove = 64;
			return;
		}
		else
		{
			bot->bot_strafe_right_timer = 0;
			ucmd->upmove = 127;
		}
	}

	if ( bot->bot_strafe_left_timer >= level.time )
	{
		if ( VectorDistance( bot->bot_strafe_target_position, bot->r.currentOrigin) > 8 )
		{
			ucmd->forwardmove = 0;								//64;
			ucmd->upmove = 0;
			ucmd->rightmove = -64;
			return;
		}
		else
		{
			bot->bot_strafe_left_timer = 0;
			ucmd->upmove = 127;
		}
	}
#endif //__BOT_STRAFE__
	if
	(
		bot->longTermGoal < 0 ||
		bot->longTermGoal > number_of_nodes ||
		bot->current_node < 0 ||
		bot->node_timeout <= level.time ||
		bot->bot_last_good_move_time < level.time - 2000
	)
	{															// We hit a problem in route, or don't have one yet.. Find a new goal...
		if ( !bot->bot_stand_guard )
		{

			//bot->current_node = -1;
			//bot->next_node = -1;
			//bot->last_node = -1;
			bot->longTermGoal = -1;
			bot->shortTermGoal = -1;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
		}

		return;													// next think...
	}

	if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
	{															// Try to roam out of problems...
		//bot->current_node = -1;
		//bot->next_node = -1;
		//bot->last_node = -1;
		bot->longTermGoal = -1;
		bot->shortTermGoal = -1;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;													// next think...
	}

	if ( nodes[bot->current_node].type & NODE_DUCK && !(bot->client->ps.pm_flags & PMF_DUCKED) )
	{															// For special duck nodes, make the bot duck down...
		ucmd->upmove = -48;
		bot->client->ps.pm_flags |= PMF_DUCKED;
	}
	else if ( nodes[bot->last_node].type & NODE_DUCK && !(bot->client->ps.pm_flags & PMF_DUCKED) )
	{															// For special duck nodes, make the bot duck down...
		ucmd->upmove = -48;
		bot->client->ps.pm_flags |= PMF_DUCKED;
	}
	else if
		(
			!(nodes[bot->current_node].type & NODE_DUCK) &&
			!(nodes[bot->last_node].type & NODE_DUCK) &&
			!bot->enemy &&
			(bot->client->ps.eFlags & EF_CROUCHING)
		)
	{															// Get up?
		ucmd->upmove = 127;
		bot->client->ps.eFlags &= ~EF_CROUCHING;
	}
	else if ( !bot->enemy && (bot->client->ps.eFlags & EF_PRONE) )
	{															// Get up?
		ucmd->upmove = 127;
		bot->client->ps.eFlags &= ~EF_PRONE;
	}

	// Now we need to check if we have hit our long term node...
	if ( bot->current_node == bot->longTermGoal && AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
	{															// We're there! Find a new goal... FIXME: We should actually do something at most goals..
		if ( bot->followtarget && VectorDistance( bot->followtarget->r.currentOrigin, bot->r.currentOrigin) < 256 )
		{														// A following bot, at it's owner... Just keep standing with him for now...
			//FIXME: Make them move around just a little near their owner... Maybe some anims/sounds??? - Unique1
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			bot->node_timeout = level.time + TravelTime( bot ); // Duck moves take longer, so...

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
			return;												// Think again soon!
		}

		if
		(
			bot->bot_defender &&
			!(bot->bot_goal_constructible && bot->client->sess.sessionTeam == level.attackingTeam)
		)
		{
			if ( !(BotMP_Num_Defenders_Near( bot->bot_goal_position, bot) > 2) )
			{
				bot->bot_stand_guard = qtrue;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				bot->node_timeout = level.time + ( TravelTime( bot) * 2 );	// Duck moves take longer, so...

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;
				return;
			}
		}

		if ( bot->longterm_pathsize > 0 )
		{
			int i;
			bot->pathsize = bot->longterm_pathsize;
			for ( i = 0; i < bot->pathsize; i++ )
			{
				bot->pathlist[i] = bot->longterm_pathlist[i];
			}

			bot->current_node = bot->shortTermGoal;
			bot->next_node = BotGetNextNode( bot );
			bot->longterm_pathsize = -1;
			bot->shortTermGoal = -1;
		}
		else
		{

			//bot->current_node = -1;
			//bot->next_node = -1;
			//bot->last_node = -1;
			bot->longTermGoal = -1;
			bot->shortTermGoal = -1;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;

			//Bot_Set_New_Path(bot, ucmd);
		}
	}

	// Now we need to check if we have hit our current node... Normally ignore heights...
	if ( AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
	{	// At current node.. Pick next in the list...
		bot->last_node = bot->current_node;
		bot->current_node = bot->next_node;
		bot->next_node = BotGetNextNode( bot );
		bot->node_timeout = level.time + TravelTime( bot );
		if ( bot->current_node > number_of_nodes )
		{

			//bot->current_node = -1;
			//bot->next_node = -1;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
		}

		if ( nodes[bot->current_node].type & NODE_DUCK )
		{
			bot->node_timeout = level.time + ( TravelTime( bot) * 2 );	// Duck moves take longer, so...
		}

#ifdef __VEHICLES__
		if ( bot->client->ps.eFlags & EF_TANK )
		{
			bot->node_timeout = level.time + ( TravelTime( bot) * 4 );	// Tank angles take longer, so...
		}
#endif //__VEHICLES__

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
	}

	if ( nodes[bot->current_node].origin[2] > bot->r.currentOrigin[2] && !onLadder )
	{
		if ( nodes[bot->current_node].origin[2] - bot->r.currentOrigin[2] > 64 )
		{																// This link is ridiculous... Disable it!
			int loop = 0;
			for ( loop = 0; loop < nodes[bot->last_node].enodenum; loop++ )
			{															// Find the link we last used and mark it...
				if ( nodes[bot->last_node].links[loop].targetNode == bot->current_node )
				{														// Mark it as blocked...
					BOT_MakeLinkUnreachable( bot, bot->last_node, loop );

					//Bot_Set_New_Path(bot, ucmd);
					//return;
					//bot->current_node = -1;
					//bot->next_node = -1;
					//bot->last_node = -1;
					bot->longTermGoal = -1;
					bot->shortTermGoal = -1;
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					return;

					//break;
				}
			}
		}
		else if
			(
				nodes[bot->current_node].origin[2] - bot->r.currentOrigin[2] > 24 /*16*/ &&
				VectorLength( bot->client->ps.velocity) < 24
			)
		{
			ucmd->upmove = 127;
		}
	}

#ifdef __BOT_STRAFE__
	if
	(
		bot->bot_strafe_right_timer < level.time &&
		bot->bot_strafe_left_timer < level.time &&
		VectorLength( bot->client->ps.velocity) < 24 &&
		!(nodes[bot->current_node].type & NODE_DUCK) &&
		!(nodes[bot->last_node].type & NODE_DUCK) &&
		!onLadder
#ifdef __VEHICLES__
		&&
		!(bot->client->ps.eFlags & EF_TANK)
#endif //__VEHICLES__
	)
	{	// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
		AIMOD_MOVEMENT_Setup_Strafe( bot, ucmd );
	}

	if ( bot->bot_strafe_right_timer >= level.time || bot->bot_strafe_left_timer >= level.time )
	{
		return;
	}
#endif //__BOT_STRAFE__

	// Set up ideal angles for this node...
	//VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, dir );
	if ( JobType( bot) <= BOT_JOB_NONE )
	{
		ucmd->buttons |= BUTTON_WALKING;
	}

	VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
	bot->move_vector[ROLL] = 0;
	vectoangles( bot->move_vector, bot->movedir );
	
	if ( bot->bot_turn_wait_time <= level.time )
	{
		G_UcmdMoveForDir( bot, ucmd, bot->movedir );
	}

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_Attack
// Description  : Attack movement
//              : Just basic move and avoidance

// TODO         : Change routine for more advanced attack movement
void
AIMOD_MOVEMENT_Attack ( gentity_t *bot, usercmd_t *ucmd )
{
	float /*c,*/ firedelay;
	vec3_t		target;
	vec3_t		angles;
	vec3_t		attackvector;
	float		dist, diff;
	vec3_t		ang, dir;
	int			skill;

	if (g_gametype.integer <= GT_COOP)
		bot->client->ps.ammo[bot->client->ps.weapon] = 32; // SP Bots always have ammo!

	// Cover spots!
//	if (bot->bot_coverspot_time > level.time)
//	{
//		if (VectorDistance(bot->bot_coverspot, bot->r.currentOrigin) > 16)
//		{
//			AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
//			return;
//		}
//	}

#ifdef __VEHICLES__
	if ( bot->client->ps.eFlags & EF_TANK )
	{
		AIMOD_MOVEMENT_VehicleAttack( bot, ucmd );
		return;
	}
#endif //__VEHICLES__

	// Make sure we continue along our current route after the attack is over...
	bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
	bot->bot_last_good_move_time = level.time;
	if
	(
		bot->beStill > level.time &&
		!(bot->client && (bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK))
	)
	{
		return;
	}

	if ( g_gametype.integer >= GT_WOLF )
	{
		skill = bot_skill.integer;
	}
	else
	{
		skill = bot_skill.integer;
	}

	if ( skill <= 0 )
	{
		skill = 1;
	}

	if ( !bot->enemy )
	{

		//G_Printf("%s can't attack with no enemy!\n", bot->client->pers.netname);
		ucmd->buttons = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	// Give them extra time to get to next node...
	bot->node_timeout = level.time + TravelTime( bot );
	VectorSubtract( bot->r.currentOrigin, bot->enemy->r.currentOrigin, dir );
	VectorNormalize( dir );
	vectoangles( dir, ang );
	if ( bot->bot_enemy_visible_time < level.time /* || bot->enemy != bot->bot_last_visible_enemy*/ )
	{

		//G_Printf("%s can't attack because of vis time or last_enemy!\n", bot->client->pers.netname);
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;											// Shouldn't have got this far...
	}

	// Set up ideal view angles for this enemy...
	VectorSubtract( bot->enemy->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
	vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
	if ( bot->client && (bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK) )
	{													// Don't do this while on an MG42 or Tank...
		diff = fabs( AngleDifference( bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]) );
		if ( diff > 15.f )
		{												// Look toward the player over frames... Not too quick for realism and fairness... Based on bot skill...
			BotSetViewAngles( bot, 100 );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
		}
		else
		{
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
		}
	}
	else
	{													// Slowly set view angles...
		diff = fabs( AngleDifference( bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]) );
		if ( diff > 15.f )
		{												// Look toward the player over frames... Not too quick for realism and fairness... Based on bot skill...
			BotSetViewAngles( bot, 100 );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
		}

		if
		(
			bot->enemy->client &&
			VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > (1200 + (bot->skillchoice * 50)) &&
			bot->weaponchoice != WP_K43 &&
			bot->weaponchoice != WP_K43_SCOPE &&
			bot->weaponchoice != WP_FG42 &&
			bot->weaponchoice != WP_FG42SCOPE /*&& !(bot->client->ps.eFlags & EF_AIMING)*/ &&
			!(bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK) &&
			!(bot->enemy->client->ps.eFlags & EF_MG42_ACTIVE || bot->enemy->client->ps.eFlags & EF_MOUNTEDTANK)
		)
		{
			if ( next_arty[bot->s.clientNum] < level.time && bot->client->ps.stats[STAT_PLAYER_CLASS] == PC_FIELDOPS )
			{
				if
				(
					VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 768 &&
					Can_Arty( bot, bot->enemy->r.currentOrigin)
				)
				{
					VectorSubtract( bot->enemy->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
					bot->move_vector[2] -= VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 100;
					vectoangles( bot->move_vector, angles );
					VectorCopy( angles, bot->s.angles );
					Weapon_Artillery( bot );

					//G_Printf("BOT DEBUG: Bot %s is calling an artillery strike!\n", bot->client->pers.netname);
					next_arty[bot->s.clientNum] = level.time + ( 50000 / skill );
					AIMOD_InformTeamOfArty( bot );
					return;
				}
				else
				{
					bot->enemy = NULL;
					AI_ResetJob( bot );
					return;								// Too far away??? Snipers only?
				}
			}
			else
			{
				bot->enemy = NULL;
				AI_ResetJob( bot );
				return;									// Too far away??? Snipers only?
			}
		}

#ifdef __NPC__
		else if
			(
				bot->enemy->NPC_client &&
				VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > (1200 + (bot->skillchoice * 50)) &&
				bot->weaponchoice != WP_K43 &&
				bot->weaponchoice != WP_K43_SCOPE &&
				bot->weaponchoice != WP_FG42 &&
				bot->weaponchoice != WP_FG42SCOPE &&
				!(bot->client->ps.eFlags & EF_AIMING) &&
				!(bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK) &&
				!(
					bot->enemy->NPC_client->ps.eFlags & EF_MG42_ACTIVE ||
					bot->enemy->NPC_client->ps.eFlags & EF_MOUNTEDTANK
				)
			)
		{
			if ( next_arty[bot->s.clientNum] < level.time && bot->client->ps.stats[STAT_PLAYER_CLASS] == PC_FIELDOPS )
			{
				if
				(
					VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 768 &&
					Can_Arty( bot, bot->enemy->r.currentOrigin)
				)
				{
					VectorSubtract( bot->enemy->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
					bot->move_vector[2] -= VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 100;
					vectoangles( bot->move_vector, angles );
					VectorCopy( angles, bot->s.angles );
					Weapon_Artillery( bot );

					//G_Printf("BOT DEBUG: Bot %s is calling an artillery strike!\n", bot->client->pers.netname);
					next_arty[bot->s.clientNum] = level.time + ( 50000 / skill );
					AIMOD_InformTeamOfArty( bot );
					return;
				}
				else
				{
					bot->enemy = NULL;
					AI_ResetJob( bot );
					return;								// Too far away??? Snipers only?
				}
			}
			else
			{
				bot->enemy = NULL;
				AI_ResetJob( bot );
				return;									// Too far away??? Snipers only?
			}
		}
#endif //__NPC__
	}

	VectorSubtract( bot->r.currentOrigin, bot->enemy->r.currentOrigin, attackvector );
	dist = VectorLength( attackvector );
	AIMOD_Combat_Weapons_Check( bot, ucmd );

	if ( bot->client->ps.ammoclip[bot->client->ps.weapon] == 0 && bot->client->ps.ammo[bot->client->ps.weapon] > 0 )
	{
		int weaponChoice = bot->client->ps.weapon;

		if ( bot->client->ps.weapon == WP_FG42SCOPE )
		{
			weaponChoice = WP_FG42;
		}

		if ( bot->client->ps.weapon == WP_K43_SCOPE )
		{
			weaponChoice = WP_K43;
		}

		if ( bot->client->ps.weapon == WP_GARAND_SCOPE )
		{
			weaponChoice = WP_GARAND;
		}

		if ( bot->client->ps.weapon == WP_GPG40 )
		{
			weaponChoice = WP_KAR98;
		}

		if ( bot->client->ps.weapon == WP_M7 )
		{
			weaponChoice = WP_CARBINE;
		}

		if (bot->client->ps.weapon != weaponChoice)
			ucmd->weapon = weaponChoice;

		ucmd->wbuttons = WBUTTON_RELOAD;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		if (bot->bot_coverspot_time > level.time)
		{
			bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
			bot->client->ps.pm_flags |= PMF_DUCKED;
			ucmd->upmove = -127;
		}
		return;
	}

	if ( ucmd->wbuttons == WBUTTON_RELOAD )
	{															// Reloading!
		int weaponChoice = bot->client->ps.weapon;

		if ( bot->client->ps.weapon == WP_FG42SCOPE )
		{
			weaponChoice = WP_FG42;
		}

		if ( bot->client->ps.weapon == WP_K43_SCOPE )
		{
			weaponChoice = WP_K43;
		}

		if ( bot->client->ps.weapon == WP_GARAND_SCOPE )
		{
			weaponChoice = WP_GARAND;
		}

		if ( bot->client->ps.weapon == WP_GPG40 )
		{
			weaponChoice = WP_KAR98;
		}

		if ( bot->client->ps.weapon == WP_M7 )
		{
			weaponChoice = WP_CARBINE;
		}

		if (bot->client->ps.weapon != weaponChoice)
			ucmd->weapon = weaponChoice;

		ucmd->upmove = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;

		if (bot->bot_coverspot_time > level.time)
		{
			bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
			bot->client->ps.pm_flags |= PMF_DUCKED;
			ucmd->upmove = -127;
		}
		return;
	}

	if ( bot->client->ps.weapon == WP_KNIFE )
	{													// Bots with a knife should always run at the enemy.
		ucmd->forwardmove = 120;
		if ( VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 64 )
		{
			return;										// And don't try to stab until we are close enough...
		}
	}
	else
	{
		ucmd->upmove = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;

		if (bot->bot_coverspot_time > level.time)
		{
			if (bot->client->ps.eFlags & EF_PRONE)
				bot->client->ps.eFlags &= ~EF_PRONE;

			if (bot->client->ps.pm_flags & PMF_DUCKED)
				bot->client->ps.pm_flags &= ~PMF_DUCKED;

			bot->bot_coverspot_time = level.time + 20000;

			if (bot->bot_coverspot_staydown_timer > level.time)
			{// Stay down for a bit longer!
				bot->client->ps.pm_flags |= PMF_DUCKED;
				ucmd->upmove = -127;
				return;
			}
			else
			{// Should we duck behind our cover for a while???
				if (Q_TrueRand(0, 20000) > 19900)
				{// Pick how long to stay down for!
					bot->bot_coverspot_staydown_timer = level.time + Q_TrueRand(5000, 10000);
					bot->client->ps.pm_flags |= PMF_DUCKED;
					ucmd->upmove = -127;
					return;
				}
			}
	
		}
		else if ( bot->skillchoice >= 4 && dist > 600 )
		{												// Elite bots.
			if ( !(bot->client->ps.eFlags & EF_PRONE) && !(bot->client->ps.pm_flags & PMF_DUCKED) )
			{											// We should only vischeck this once at the first attack time...
				ucmd->upmove = -127;					// Crouch?

				// Only go prone if we can still hit them from down there...
				if ( visible( bot, bot->enemy) )
				{
					bot->client->ps.eFlags |= EF_PRONE;
				}
				else
				{
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}
			}
		}
		else if ( bot->skillchoice >= 2 && dist > 800 )
		{												// Good bots.
			if ( !(bot->client->ps.eFlags & EF_PRONE) && !(bot->client->ps.pm_flags & PMF_DUCKED) )
			{											// We should only vischeck this once at the first attack time...
				ucmd->upmove = -127;					// Crouch?

				// Only go prone if we can still hit them from down there...
				if ( visible( bot, bot->enemy) )
				{
					bot->client->ps.eFlags |= EF_PRONE;
				}
				else
				{
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}
			}
		}
		else if ( dist > 256 && !(bot->client->ps.pm_flags & PMF_DUCKED) )
		{												// Use long aim ! DUCK ! IF you don't have the flag ! That is
			ucmd->upmove = -48;
			bot->client->ps.pm_flags |= PMF_DUCKED;
		}
	}

	// Set the attack
	//-------------------------------------------------------
	firedelay = ( random() - 0.5 ) * 5;

	// Aim
	//-------------------------------------------------------
	VectorCopy( bot->enemy->r.currentOrigin, target );

	//-------------------------------------------------------
	// Modify attack angles based on accuracy
	// added skill level
	//-------------------------------------------------------

	/*    if (bot->skillchoice == 0)
    {
        // Can't hit a good strafing player
        int randm;

		randm = Q_TrueRand(0,1);
		
		if (randm == 0)
			target[0] += (random()-0.5) * 20;
		else
			target[0] -= (random()-0.5) * 20;

        randm = Q_TrueRand(0,1);
		
		if (rand == 0)
			target[1] += (random()-0.5) * 20;
		else
			target[1] -= (random()-0.5) * 20;

		randm = Q_TrueRand(0,1);

#ifdef __VEHICLES__
		if (!(bot->enemy->client->ps.eFlags & EF_TANK))
#endif //__VEHICLES__
		if (rand == 0)
			target[2] += (random()-0.5) * 20;
		else
			target[2] -= (random()-0.5) * 20;
    }
    else if (bot->skillchoice == 1)
    {
        // Will get ya every now and then
        int randm;

		randm = Q_TrueRand(0,1);
		
		if (rand == 0)
			target[0] += (random()-0.5) * 10;
		else
			target[0] -= (random()-0.5) * 10;

        randm = Q_TrueRand(0,1);
		
		if (rand == 0)
			target[1] += (random()-0.5) * 10;
		else
			target[1] -= (random()-0.5) * 10;

		randm = Q_TrueRand(0,1);
		
#ifdef __VEHICLES__
		if (!(bot->enemy->client->ps.eFlags & EF_TANK))
#endif //__VEHICLES__
		if (rand == 0)
			target[2] += (random()-0.5) * 10;
		else
			target[2] -= (random()-0.5) * 10;
    }
    else if (bot->skillchoice == 2)
    {
        // Good competetion for some of the best players
        int randm;

		randm = Q_TrueRand(0,1);
		
		if (rand == 0)
			target[0] += (random()-0.5) * 5;
		else
			target[0] -= (random()-0.5) * 5;

        randm = Q_TrueRand(0,1);
		
		if (rand == 0)
			target[1] += (random()-0.5) * 5;
		else
			target[1] -= (random()-0.5) * 5;

		randm = Q_TrueRand(0,1);
#ifdef __VEHICLES__
		if (!(bot->enemy->client->ps.eFlags & EF_TANK))
#endif //__VEHICLES__
		if (rand == 0)
			target[2] += (random()-0.5) * 5;
		else
			target[2] -= (random()-0.5) * 5;
    }
    else if (bot->skillchoice >= 3)
    {
        // Never misses, unloads on ya!
        target[0] = target[0];
        target[1] = target[1];
    }
    //-------------------

	if (bot->client
		&& (bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK))
	{// Randomize attack dir a little more while on an MG42 or Tank...
		int randm;

		randm = Q_TrueRand(0,1);
		
		if (rand == 0)
			target[0] += (random()-0.5) * 10;
		else
			target[0] -= (random()-0.5) * 10;

        randm = Q_TrueRand(0,1);
		
		if (rand == 0)
			target[1] += (random()-0.5) * 10;
		else
			target[1] -= (random()-0.5) * 10;

		randm = Q_TrueRand(0,1);
		
#ifdef __VEHICLES__
		if (!(bot->enemy->client->ps.eFlags & EF_TANK))
#endif //__VEHICLES__
		if (rand == 0)
			target[2] += (random()-0.5) * 10;
		else
			target[2] -= (random()-0.5) * 10;
	}
*/
#ifdef __VEHICLES__
	if ( (bot->client->ps.eFlags & EF_TANK) && (bot->enemy->client->ps.eFlags & EF_TANK) )
	{
		target[2] -= Q_TrueRand( 16, 32 );
	}
#endif //__VEHICLES__

	// Set direction
	//-------------------------------------------------------
	VectorSubtract( target, bot->r.currentOrigin, bot->attack_vector );
	vectoangles( bot->attack_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );
	if ( VectorDistance( bot->bot_ideal_view_angles, bot->client->ps.viewangles) > 16 )
	{													// Wait until we are looking toward them to fire...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		ucmd->buttons = 0;
		bot->bot_last_good_move_time = level.time;
		bot->node_timeout = level.time + TravelTime( bot );
		return;
	}

	// Added Skill Level Settings for Attack
#ifdef __VEHICLES__
	if ( bot->client->ps.eFlags & EF_TANK )
	{
		bot->tank_attack_time = level.time;
		if ( bot->skillchoice == 0 )
		{
			if ( firedelay < 0.10 )
			{
				if ( bot->enemy->client && bot->enemy->client->ps.eFlags & EF_TANK )
				{
					ucmd->buttons = BUTTON_ATTACK;		// Attacking another vehicle... Use turret.
				}
				else
				{
					ucmd->wbuttons = WBUTTON_ATTACK2;	// Attacking a humanoid... Use MG.
				}
			}
			else
			{

			}
		}
		else if ( bot->skillchoice == 1 )
		{
			if ( firedelay < 0.20 )
			{
				if ( bot->enemy->client && bot->enemy->client->ps.eFlags & EF_TANK )
				{
					ucmd->buttons = BUTTON_ATTACK;		// Attacking another vehicle... Use turret.
				}
				else
				{
					ucmd->wbuttons = WBUTTON_ATTACK2;	// Attacking a humanoid... Use MG.
				}
			}
			else
			{

			}
		}
		else if ( bot->skillchoice == 2 )
		{
			if ( firedelay < 0.40 )
			{
				if ( bot->enemy->client && bot->enemy->client->ps.eFlags & EF_TANK )
				{
					ucmd->buttons = BUTTON_ATTACK;		// Attacking another vehicle... Use turret.
				}
				else
				{
					ucmd->wbuttons = WBUTTON_ATTACK2;	// Attacking a humanoid... Use MG.
				}
			}
			else
			{

			}
		}
		else if ( bot->skillchoice >= 3 )
		{
			if ( bot->enemy->client && bot->enemy->client->ps.eFlags & EF_TANK )
			{
				ucmd->buttons = BUTTON_ATTACK;			// Attacking another vehicle... Use turret.
			}
			else
			{
				ucmd->wbuttons = WBUTTON_ATTACK2;		// Attacking a humanoid... Use MG.
			}
		}
	}
	else
#endif //__VEHICLES__
	{
		if (bot->bot_coverspot_time > level.time 
			&& (bot->bot_coverspot_staydown_timer > level.time || (bot->client->ps.pm_flags & PMF_DUCKED)))
			return; // Don't shoot the poor barrel/crate!!! :)

		if ( bot->skillchoice == 0 )
		{
			if ( firedelay < 0.10 )
			{
				ucmd->buttons = BUTTON_ATTACK;
			}
			else
			{

			}
		}
		else if ( bot->skillchoice == 1 )
		{
			if ( firedelay < 0.20 )
			{
				ucmd->buttons = BUTTON_ATTACK;
			}
			else
			{

			}
		}
		else if ( bot->skillchoice == 2 )
		{
			if ( firedelay < 0.40 )
			{
				ucmd->buttons = BUTTON_ATTACK;
			}
			else
			{

			}
		}
		else if ( bot->skillchoice >= 3 )
		{
			ucmd->buttons = BUTTON_ATTACK;
		}
	}

	//-------------------------------------------------------
	if ( bot->client->ps.weapon == WP_SMOKE_MARKER )
	{
		bot->move_vector[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfAirstrike( bot );
	}

	if ( bot->client->ps.weapon == WP_GPG40 || bot->client->ps.weapon == WP_M7 )
	{
		bot->move_vector[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfGrenade( bot );
	}

	if ( bot->client->ps.weapon == WP_GRENADE_LAUNCHER || bot->client->ps.weapon == WP_GRENADE_PINEAPPLE )
	{
		bot->move_vector[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfGrenade( bot );
	}

	vectoangles( bot->move_vector, angles );
	if ( bot->client->ps.weapon == WP_SMOKE_MARKER )
	{
		angles[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfAirstrike( bot );
	}

	if ( bot->client->ps.weapon == WP_GPG40 || bot->client->ps.weapon == WP_M7 )
	{
		angles[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfGrenade( bot );
	}

	if ( bot->client->ps.weapon == WP_GRENADE_LAUNCHER || bot->client->ps.weapon == WP_GRENADE_PINEAPPLE )
	{
		angles[2] += VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin ) / 10;
		AIMOD_InformTeamOfGrenade( bot );
	}

	VectorCopy( angles, bot->s.angles );

	//-------------------------------------------------------
	bot->teamPauseTime = level.time;

	// Prototype team enemy handling...
	//	AIMOD_InformTeamOfEnemy(bot);
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_ChangeBotAngle
// Description  : Make changes in angles a little more gradual, not so snappy

//              : but noticable. Modified from id code ChangeYaw.
void
AIMOD_MOVEMENT_ChangeBotAngle ( gentity_t *ent )
{
	float	ideal_yaw;
	float	ideal_pitch;
	float	current_yaw;
	float	current_pitch;
	float	move;
	float	speed;
	vec3_t	ideal_angle;

	// Normalize the move angle first
	//-------------------------------------------------------
	VectorNormalize( ent->move_vector );
	current_yaw = anglemod( ent->s.angles[YAW] );
	current_pitch = anglemod( ent->s.angles[PITCH] );
	vectoangles( ent->move_vector, ideal_angle );
	ideal_yaw = anglemod( ideal_angle[YAW] );
	ideal_pitch = anglemod( ideal_angle[PITCH] );

	//-------------------------------------------------------
	// Yaw
	//-------------------------------------------------------
	if ( current_yaw != ideal_yaw )
	{
		move = ideal_yaw - current_yaw;
		speed = ent->yaw_speed;
		if ( ideal_yaw > current_yaw )
		{
			if ( move >= 180 )
			{
				move = move - 360;
			}
		}
		else
		{
			if ( move <= -180 )
			{
				move = move + 360;
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

		ent->s.angles[YAW] = anglemod( current_yaw + move );
	}

	//-------------------------------------------------------
	// Pitch
	//-------------------------------------------------------
	if ( current_pitch != ideal_pitch )
	{
		move = ideal_pitch - current_pitch;
		speed = ent->yaw_speed;
		if ( ideal_pitch > current_pitch )
		{
			if ( move >= 180 )
			{
				move = move - 360;
			}
		}
		else
		{
			if ( move <= -180 )
			{
				move = move + 360;
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

		ent->s.angles[PITCH] = anglemod( current_pitch + move );
	}

	//-------------------------------------------------------
}
#endif
