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
#ifdef __VEHICLES__
#include "../game/g_local.h"
#include "../game/botlib.h"
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_char.h"
#include "../game/be_ai_chat.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../game/be_ai_weap.h"
#include "../botai/botai.h"
#include "../botai/inv.h"

//
#include "../botai/ai_main.h"
#include "../botai/ai_dmq3.h"
#include "../botai/ai_cmd.h"
#include "../botai/ai_team.h"

//
#include "../botai/ai_dmnet_mp.h"

// Key AI constants
#include "../botai/ai_distances.h"
#include "../botai/chars.h"
extern vmCvar_t		bot_cpu_use;
extern float		TravelTime ( gentity_t *bot );
extern void			ClientThink_real ( gentity_t *ent );
extern void			AIMOD_MOVEMENT_Attack ( gentity_t *bot, usercmd_t *ucmd );
extern void			AIMOD_MOVEMENT_AttackMove ( gentity_t *bot, usercmd_t *ucmd );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodes ( gentity_t *bot, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodesTo ( gentity_t *bot, vec3_t origin, int r, int type );
extern int			AIMOD_NAVIGATION_FindClosestReachableNodeTo ( vec3_t origin, int r, int type );
extern qboolean		BAD_WP_Height ( vec3_t start, vec3_t end );
extern qboolean		AIMOD_MOVEMENT_CurrentNodeReachable ( gentity_t *bot );
extern qboolean		AIMOD_MOVEMENT_WaypointTouched ( gentity_t *bot, int touch_node, qboolean onLadder );
extern short int	BotGetNextNode ( gentity_t *bot /*, int *state*/ ); // below...
extern void			BotSetViewAngles ( gentity_t *bot, float thinktime );
extern void			G_UcmdMoveForDir ( gentity_t *self, usercmd_t *cmd, vec3_t dir );
extern int			Unique_FindGoal ( gentity_t *bot );
extern int			CreatePathAStar ( gentity_t *bot, int from, int to, short int *pathlist );

// In unique_goal.c
extern int			Find_Goal_Teammate ( gentity_t *bot );
extern int			Find_Goal_Enemy ( gentity_t *bot );
extern int			BotMP_Sniper_Find_Goal_EntityNum
					(
						gentity_t	*bot,
						int			ignoreEnt,
						int			ignoreEnt2,
						vec3_t		current_org,
						int			teamNum
					);
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
extern void			BotDropToFloor ( gentity_t *ent );
qboolean			have_land_vehicle_waypoints = qfalse;
qboolean			land_vehicle_waypoints_checked = qfalse;


/* */
qboolean
BOT_HaveVehicleWaypoints ( void )
{
	int i;
	if ( land_vehicle_waypoints_checked )
	{
		return ( have_land_vehicle_waypoints );
	}

	land_vehicle_waypoints_checked = qtrue;
	for ( i = 0; i < number_of_nodes; i++ )
	{
		if ( nodes[i].type & NODE_LAND_VEHICLE )
		{
			have_land_vehicle_waypoints = qtrue;
			return ( qtrue );
		}
	}

	return ( qfalse );
}


//===========================================================================
// Routine      : Vehicle_FindGoal

// Description  : Find a new goal entity and node.
int
Vehicle_FindGoal ( gentity_t *bot )
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
	if ( bot->enemy )
	{
		entNum = bot->enemy->s.number;
	}

	entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
													bot->client->sess.sessionTeam );
	if ( entNum < 0 )
	{
		entNum = BotMP_Engineer_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->client->sess.sessionTeam );
	}

	if ( entNum < 0 )
	{
		entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														bot->client->sess.sessionTeam );
	}

	if ( entNum < 0 )
	{
		entNum = BotMP_FieldOps_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
														 bot->client->sess.sessionTeam );
	}

	if ( entNum < 0 )
	{
		entNum = BotMP_Find_POP_Goal_EntityNum( bot, entNum, bot->current_target_entity, bot->r.currentOrigin,
												bot->client->sess.sessionTeam );
	}

	if ( entNum >= 0 )
	{
		int loop = 0;
		for ( loop = 0; loop < number_of_nodes; loop++ )
		{					// Find closest node(s) to the objective...
			float	dist;
			if ( !(nodes[loop].type & NODE_LAND_VEHICLE) )
			{
				continue;
			}

			if ( bot->bot_goal_constructible || bot->bot_goal_destroyable )
			{
				dist = VectorDistance( nodes[loop].origin, bot->bot_goal_position );
			}
			else
			{
				dist = VectorDistance( nodes[loop].origin, g_entities[entNum].r.currentOrigin );
			}

			if ( dist < best_dist )
			{
				memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
				if ( bot->bot_goal_constructible || bot->bot_goal_destroyable )
				{
					best_dist = VectorDistance( nodes[loop].origin, bot->bot_goal_position );
				}
				else
				{
					best_dist = VectorDistance( nodes[loop].origin, g_entities[entNum].r.currentOrigin );
				}

				best_target = loop;
				if ( dist <= 64 )
				{			// Close enough.. Use it straight away...
					bot->current_target_entity = entNum;
					return ( best_target );
				}
			}
		}
	}

	if ( best_target >= 0 )
	{						// Found a goal.. Use it...
		bot->current_target_entity = entNum;
		bot->goalentity = &g_entities[entNum];

		// Return the best choice...
		return ( best_target );
	}

	// If we got here, then we failed to find a path to a goal...
	return ( -1 );
}


/* */
void
Bot_Set_New_Vehicle_Path ( gentity_t *bot, usercmd_t *ucmd )
{
	if ( bot->last_astar_path_think > level.time - 2000 )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	bot->last_astar_path_think = level.time;

	// Initialize...
	bot->shortTermGoal = -1;
	if
	(
		bot->bot_last_good_move_time < level.time - 2000 ||
		bot->node_timeout < 0 ||
		bot->current_node < 0 ||
		bot->longTermGoal < 0 ||
		bot->next_node < 0 ||
		!AIMOD_MOVEMENT_CurrentNodeReachable( bot) ||
		BAD_WP_Height( nodes[bot->current_node].origin, bot->r.currentOrigin)
	)
	{

	}
	else
	{
		bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
	}

	memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
	bot->pathsize = 0;
	bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
	bot->goalentity = NULL;
	bot->longTermGoal = Vehicle_FindGoal( bot );
	if ( bot->goalentity && bot->goalentity->client && bot->goalentity->health > 0 )
	{													// If our goal entity is a player/bot, continue heading toward them...
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( bot->goalentity, NODE_DENSITY, NODEFIND_ALL, bot );
	}

	if ( bot->longTermGoal < 0 )
	{													// Find an enemy to head for...
		int best_ent = Find_Goal_Enemy( bot );
		if ( best_ent >= 0 )
		{
			bot->goalentity = &g_entities[best_ent];
			bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( bot->goalentity, NODE_DENSITY, NODEFIND_ALL,
																		   bot );
		}
	}

	if ( bot->longTermGoal < 0 )
	{													// Just get us out of trouble...
		bot->goalentity = NULL;
		bot->longTermGoal = bot->current_node + 5;
	}

	bot->last_node = bot->current_node - 1;
	bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
	bot->next_node = BotGetNextNode( bot );				//move to this node first, since it's where our path starts from
	bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;
}


//===========================================================================
// Routine      : Vehicle_BotEntityCloserOnFuturePath

// Description  : Is an entity closer to one of our current nodes then to us now?
qboolean
Vehicle_BotEntityCloserOnFuturePath ( gentity_t *bot, gentity_t *ent )
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


//===========================================================================
// Routine      : AIMOD_MOVEMENT_VehicleAttack
// Description  : Attack movement
//              : Just basic move and avoidance

// TODO         : Change routine for more advanced attack movement
void
AIMOD_MOVEMENT_VehicleAttack ( gentity_t *bot, usercmd_t *ucmd )
{
	float	firedelay;
	vec3_t	target;

	// Make sure we continue along our current route after the attack is over...
	bot->node_timeout = level.time + ( TravelTime( bot) * 4 );
	bot->bot_last_good_move_time = level.time;
	if ( !bot->enemy || bot->bot_enemy_visible_time < level.time )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;										// Shouldn't have got this far...
	}

	// Set the attack
	//-------------------------------------------------------
	firedelay = ( random() - 0.5 ) * 5;

	// Aim
	VectorCopy( bot->enemy->r.currentOrigin, target );
#ifdef __VEHICLES__

	//	if ((bot->client->ps.eFlags & EF_TANK) && (bot->enemy->client->ps.eFlags & EF_TANK))
	//		target[2]-=Q_TrueRand(8, 16);
#endif //__VEHICLES__

	// Set direction
	//-------------------------------------------------------
	VectorSubtract( target, bot->r.currentOrigin, bot->attack_vector );
	vectoangles( bot->attack_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );
	if ( VectorDistance( bot->bot_ideal_view_angles, bot->client->ps.viewangles) > 8 )
	{												// Wait until we are looking toward them to fire...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		ucmd->buttons = 0;
		return;
	}

	// Added Skill Level Settings for Attack
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

	bot->teamPauseTime = level.time;
}

vec3_t	visible_end;


/* */
int
PositionVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg;
	VectorCopy( org2, newOrg );
	newOrg[2] += 32;	// Look from up a little...
	trap_Trace( &tr, org1, NULL, NULL, newOrg, ignore, MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA /*MASK_WATER*/ );
	VectorCopy( tr.endpos, visible_end );
	if ( tr.startsolid || tr.allsolid )
	{
		return ( 4 );
	}

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
	{					// A door in the way.. See if we have access...
		if ( !g_entities[tr.entityNum].allowteams )
		{
			return ( 2 );
		}

		return ( 3 );
	}

	return ( 0 );
}


/* */
void
AIMOD_Vehicle_CheckNextNode ( gentity_t *bot )
{
	int		i, outcome;
	vec3_t	forward, forward_pos, back_pos, forward_pos2, back_pos2;
	if ( !bot->next_node || !bot->current_node )
	{
		bot->vehicle_pause_time = level.time + 2000;
		return;
	}

	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		gentity_t	*ent = &g_entities[i];
		if ( !ent || !ent->client )
		{
			continue;
		}

		if ( ent == bot )
		{
			continue;
		}

		if ( VectorDistance( ent->r.currentOrigin, nodes[bot->current_node].origin) < 256 )
		{	// Tell the tank to pause for a few seconds to let the entity move off...
			bot->vehicle_pause_time = level.time + 100;
		}

		//		else if (VectorDistance(ent->r.currentOrigin, nodes[bot->next_node].origin) < 256)
		//		{// Tell the tank to pause for a few seconds to let the entity move off...
		//			bot->vehicle_pause_time = level.time + 2000;
		//		}
	}

	// UQ1: Try to fix any times we are stuck in a solid...
	AngleVectors( bot->r.currentAngles, forward, NULL, NULL );
	VectorMA( bot->r.currentOrigin, 128, forward, forward_pos );
	VectorMA( bot->r.currentOrigin, 64, forward, forward_pos2 );
	VectorMA( bot->r.currentOrigin, -128, forward, back_pos );
	VectorMA( bot->r.currentOrigin, -64, forward, back_pos2 );
	outcome = PositionVisible( bot->r.currentOrigin, forward_pos, bot->s.number );
	if ( outcome == 4 )
	{		// In a solid! Cheat: Teleport direct to our node!
		vec3_t	good_dir, angle;
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, good_dir );
		vectoangles( good_dir, angle );
		AngleVectors( angle, forward, NULL, NULL );
		VectorMA( bot->r.currentOrigin, 8, forward, forward_pos );
		VectorCopy( forward_pos, bot->r.currentOrigin );
		VectorCopy( forward_pos, bot->s.origin );
		VectorCopy( forward_pos, bot->client->ps.origin );
	}
	else if ( outcome != 1 )
	{
		vec3_t	good_dir, angle;
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, good_dir );
		vectoangles( good_dir, angle );
		AngleVectors( angle, forward, NULL, NULL );
		VectorMA( bot->r.currentOrigin, 8, forward, forward_pos );
		VectorCopy( forward_pos, bot->r.currentOrigin );
		VectorCopy( forward_pos, bot->s.origin );
		VectorCopy( forward_pos, bot->client->ps.origin );
	}

	outcome = PositionVisible( bot->r.currentOrigin, back_pos, bot->s.number );
	if ( outcome == 4 )
	{		// In a solid! Cheat: Teleport direct to our node!
		vec3_t	good_dir, angle;
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, good_dir );
		vectoangles( good_dir, angle );
		AngleVectors( angle, forward, NULL, NULL );
		VectorMA( bot->r.currentOrigin, 8, forward, forward_pos );
		VectorCopy( forward_pos, bot->r.currentOrigin );
		VectorCopy( forward_pos, bot->s.origin );
		VectorCopy( forward_pos, bot->client->ps.origin );
	}
	else if ( outcome != 1 )
	{
		vec3_t	good_dir, angle;
		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, good_dir );
		vectoangles( good_dir, angle );
		AngleVectors( angle, forward, NULL, NULL );
		VectorMA( bot->r.currentOrigin, 8, forward, forward_pos );
		VectorCopy( forward_pos, bot->r.currentOrigin );
		VectorCopy( forward_pos, bot->s.origin );
		VectorCopy( forward_pos, bot->client->ps.origin );
	}
}


/* */
qboolean
AIMOD_Vehicle_PlayerInFront ( gentity_t *bot )
{
	int i;
	if ( !bot->next_node || !bot->current_node )
	{
		bot->vehicle_pause_time = level.time + 2000;
		return ( qfalse );
	}

	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		gentity_t	*ent = &g_entities[i];
		if ( !ent || !ent->client )
		{
			continue;
		}

		if ( ent == bot )
		{
			continue;
		}

		if ( VectorDistance( ent->r.currentOrigin, nodes[bot->current_node].origin) < 256 )
		{	// Tell the tank to pause for a few seconds to let the entity move off...
			bot->vehicle_pause_time = level.time + 100;
			return ( qtrue );
		}

		//		else if (VectorDistance(ent->r.currentOrigin, nodes[bot->next_node].origin) < 256)
		//		{// Tell the tank to pause for a few seconds to let the entity move off...
		//			bot->vehicle_pause_time = level.time + 2000;
		//			return qtrue;
		//		}
	}

	return ( qfalse );
}


//===========================================================================
// Routine      : AIMOD_MOVEMENT_VehicleMove

// Description  : Main movement (following node paths)
void
AIMOD_MOVEMENT_VehicleMove ( gentity_t *bot, usercmd_t *ucmd )
{
	qboolean	onLadder = qfalse;
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

	if ( nodes_loaded && bot->current_node > 0 )
	{
		if ( !AIMOD_MOVEMENT_CurrentNodeReachable( bot) )
		{				// If our current node is not visible, then we need to try something else...
			bot->current_node = -1;
			Bot_Set_New_Vehicle_Path( bot, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
		}
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
	{					// Do attacking first...
		AIMOD_MOVEMENT_VehicleAttack( bot, ucmd );
		return;
	}

	if ( bot->enemy || bot->bot_enemy_visible_time > level.time )
	{
		BotSetViewAngles( bot, 100 );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		AIMOD_MOVEMENT_VehicleAttack( bot, ucmd );
		return;
	}

	if ( bot->tank_attack_time > level.time - 8000 || bot->vehicle_pause_time > level.time )
	{					// Pause 8 secs after firing, or if theres a client in front of us!
		BotSetViewAngles( bot, 100 );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	AIMOD_Vehicle_CheckNextNode( bot );

	//BotDropToFloor( bot );
	// ---------------------------------------------
	// OK... First the movement for non-routed maps.
	// ---------------------------------------------
#ifdef __ETE__
	if ( nodes_loaded && bot->current_node )
	{					// We have a routed map.. Do some flag capture checks...
		if
		(
			bot->client->ps.stats[STAT_FLAG_CAPTURE] > 0 &&
			g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != bot->client->sess.sessionTeam &&
			bot->current_node > 0
		)
		{
			if
			(
				!Vehicle_BotEntityCloserOnFuturePath( bot, &g_entities[bot->client->ps.stats[STAT_CAPTURE_ENTITYNUM]]) &&
				!Have_Close_Enemy( bot)
			)
			{			// If we're capturing a flag in POP gametype don't move, just defend current pos...
				bot->bot_last_good_move_time = level.time;
				bot->node_timeout = level.time + TravelTime( bot );
				return; // Don't move while capturing a flag...
			}
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
		{				// Do attacking first...
			//AIMOD_MOVEMENT_AttackMove(bot, ucmd);
			AIMOD_MOVEMENT_Attack( bot, ucmd );
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
			if ( !Have_Close_Enemy( bot) )
			{			// If we're capturing a flag in POP gametype don't move, just defend current pos...
				bot->bot_last_good_move_time = level.time;
				bot->node_timeout = level.time + TravelTime( bot );
				return; // Don't move while capturing a flag...
			}
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
		{				// Do attacking first...
			//AIMOD_MOVEMENT_AttackMove(bot, ucmd);
			AIMOD_MOVEMENT_Attack( bot, ucmd );
			return;
		}
	}
#endif //__ETE__

	// ----------------------------------------
	// OK... Now the movement for routed maps.
	// ----------------------------------------
	if ( bot->current_node < 0 )
	{
		bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		if ( bot->current_node < 0 )
		{
			bot->bot_random_move_time = level.time + 1000;
		}
	}

	if ( bot->bot_last_good_move_time < level.time - 100 )
	{
		vec3_t	good_dir, angle, forward, forward_pos;

		// Now we need to check if we have hit our current node...
		if ( AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
		{				// At current node.. Pick next in the list...
			bot->last_node = bot->current_node;
			bot->current_node = bot->next_node;
			bot->next_node = BotGetNextNode( bot );
			bot->node_timeout = level.time + ( TravelTime( bot) * 4 );	//6000;//4000;
			if ( bot->current_node > number_of_nodes )
			{
				bot->current_node = -1;
				bot->next_node = -1;
				return;
			}

			// Set our last good move time to now...
			bot->bot_last_good_move_time = level.time;
			bot->node_timeout = level.time + TravelTime( bot );
		}

		if ( AIMOD_Vehicle_PlayerInFront( bot) )
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
		}

		VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, good_dir );
		vectoangles( good_dir, angle );
		AngleVectors( angle, forward, NULL, NULL );
		VectorMA( bot->r.currentOrigin, 8, forward, forward_pos );
		VectorCopy( forward_pos, bot->r.currentOrigin );
		VectorCopy( forward_pos, bot->s.origin );
		VectorCopy( forward_pos, bot->client->ps.origin );
		return;
	}

	if
	(
		bot->longTermGoal < 0 ||
		bot->longTermGoal > number_of_nodes ||
		bot->current_node < 0 ||
		bot->node_timeout <= level.time ||
		bot->bot_last_good_move_time < level.time - 500
	)
	{																// We hit a problem in route, or don't have one yet.. Find a new goal...
		Bot_Set_New_Vehicle_Path( bot, ucmd );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		bot->node_timeout = level.time + ( TravelTime( bot) * 4 );	//6000;//4000;
		bot->bot_last_good_move_time = level.time;

		/*
		if (bot->longTermGoal < 0)
			G_Printf("BOT VEHICLES: Bot paused because bot->longTermGoal < 0!\n");
		else if (bot->longTermGoal > number_of_nodes )
			G_Printf("BOT VEHICLES: Bot paused because bot->longTermGoal > number_of_nodes!\n");
		else if (bot->current_node < 0  )
			G_Printf("BOT VEHICLES: Bot paused because bot->current_node < 0!\n");
		else if (bot->node_timeout <= level.time )
			G_Printf("BOT VEHICLES: Bot paused because bot->node_timeout <= level.time!\n");
		else if (bot->bot_last_good_move_time < level.time - 2000 )
			G_Printf("BOT VEHICLES: Bot paused because bot->bot_last_good_move_time < level.time - 2000!\n");
*/
		return;
	}

	if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
	{																// Try to roam out of problems...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		bot->longTermGoal = -1;
		bot->shortTermGoal = -1;

		//G_Printf("BOT VEHICLES: Bot paused because he has no long term goal!\n");
		return;														// next think...
	}

	// Now we need to check if we have hit our long term node...
	if ( bot->current_node == bot->longTermGoal && AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
	{																// We're there! Find a new goal... FIXME: We should actually do something at most goals..
		Bot_Set_New_Vehicle_Path( bot, ucmd );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		bot->node_timeout = level.time + ( TravelTime( bot) * 4 );	//6000;//4000;
		bot->bot_last_good_move_time = level.time;

		//G_Printf("BOT VEHICLES: Bot paused because he hit long term goal!\n");
		return;
	}

	// Now we need to check if we have hit our current node...
	if ( AIMOD_MOVEMENT_WaypointTouched( bot, bot->current_node, onLadder) )
	{																// At current node.. Pick next in the list...
		bot->last_node = bot->current_node;
		bot->current_node = bot->next_node;
		bot->next_node = BotGetNextNode( bot );
		bot->node_timeout = level.time + ( TravelTime( bot) * 4 );	//6000;//4000;
		if ( bot->current_node > number_of_nodes )
		{
			bot->current_node = -1;
			bot->next_node = -1;
			return;
		}

		// Set our last good move time to now...
		bot->bot_last_good_move_time = level.time;
		bot->node_timeout = level.time + TravelTime( bot );
	}

	// Set up ideal view angles for this enemy...
	VectorSubtract( nodes[bot->current_node].origin, bot->r.currentOrigin, bot->move_vector );
	bot->move_vector[ROLL] = 0;
	vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
	BotSetViewAngles( bot, 100 );

	//if (bot->bot_turn_wait_time <= level.time)
	//	G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
	if ( VectorDistance( bot->client->ps.viewangles, bot->bot_ideal_view_angles) < 16 )
	{
		ucmd->forwardmove = 127;
	}

	// Set our last good move time to now...
	bot->bot_last_good_move_time = level.time;
}
#endif //__VEHICLES__
#endif //__BOT__
