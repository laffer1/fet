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

/*

NAVIGATION FUNCTION LISTING

AIMOD_NAVIGATION_FindCost
AIMOD_NAVIGATION_FindCloseReachableNode
AIMOD_NAVIGATION_FindClosestReachableNode
AIMOD_NAVIGATION_SetGoal
AIMOD_NAVIGATION_SetFlagGoal
AIMOD_NAVIGATION_FindFlagNode

*/

/*
  _______              .__              __  .__               
  \      \ _____ ___  _|__| _________ _/  |_|__| ____   ____  
  /   |   \\__  \\  \/ /  |/ ___\__  \\   __\  |/  _ \ /    \ 
 /    |    \/ __ \\   /|  / /_/  > __ \|  | |  (  <_> )   |  \
 \____|__  (____  /\_/ |__\___  (____  /__| |__|\____/|___|  /
         \/     \/       /_____/     \/                    \/ 
    bot_navigation.c
    Fusion Navigation
================================================================
Should y */
#include "../game/g_local.h"

//#include "bot.h"
qboolean		OnIce[MAX_CLIENTS];
int				exitnode[MAX_CLIENTS];

// Total number of nodes that are items
//-------------------------------------------------------
int				number_of_itemnodes;

// Total number of nodes
//-------------------------------------------------------
int				number_of_nodes = 0;

// array for node data
//-------------------------------------------------------
extern node_t	nodes[MAX_NODES];
extern qboolean OriginWouldTelefrag ( vec3_t origin );	// g_client.c - UQ1
extern int		AIMOD_NAVIGATION_FindClosestReachableNodes ( gentity_t *bot, int r, int type );


/* */
qboolean
TooManyBots ( gentity_t *ent, vec3_t origin )
{
	int		e = 0;
	int		numListedEntities = 0;
	int		entityList[MAX_GENTITIES];
	int		count = 0;
	vec3_t	mins, maxs;
	VectorSet( mins, -800, -800, -60 );
	VectorSet( maxs, 800, 800, 60 );

	//VectorSet(mins, -1400, -1400, -60);
	//VectorSet(maxs, 1400, 1400, 60);
	numListedEntities = trap_EntitiesInBox( ent->r.mins, ent->r.maxs, entityList, MAX_GENTITIES );
	for ( e = 0; e < numListedEntities; e++ )
	{
		gentity_t	*tempent = &g_entities[entityList[e]];
		if ( !tempent )
		{
			continue;
		}

		if ( !tempent->client || !tempent->health )
		{
			continue;
		}

		if ( tempent->s.teamNum == TEAM_AXIS && tempent->health > 0 && tempent->r.svFlags & SVF_BOT )
		{
			count++;
		}
	}

	if ( count > 2 )
	{
		return ( qtrue );
	}
	else
	{
		return ( qfalse );
	}
}

extern qboolean G_AvailableAirstrikes ( gentity_t *ent );	// g_weapon.c


/* */
qboolean
Can_Airstrike ( gentity_t *ent, vec3_t origin )
{
	int		e = 0;
	int		numListedEntities = 0;
	int		entityList[MAX_GENTITIES];
	int		count = 0;
	vec3_t	mins, maxs;
	if ( VectorDistance( ent->r.currentOrigin, origin) < 300 )
	{
		return ( qfalse );
	}

	if ( VectorDistance( ent->r.currentOrigin, origin) > 700 )
	{
		return ( qfalse );
	}

	/*	if (!G_AvailableAirstrikes(ent))
	{
		//G_Printf("No available airstrikes!\n");
		return qfalse;
	}*/
	count = 0;
	e = 0;
	numListedEntities = 0;
	VectorCopy( origin, mins );
	mins[0] -= 500;
	mins[1] -= 500;
	mins[2] -= 500;
	VectorCopy( origin, maxs );
	mins[0] += 500;
	mins[1] += 500;
	mins[2] += 500;
	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
	for ( e = 0; e < numListedEntities; e++ )
	{
		gentity_t	*tempent = &g_entities[entityList[e]];
		if ( !tempent )
		{
			continue;
		}

		if ( !tempent->client || !tempent->health )
		{
			continue;
		}

		//		if (tempent == ent)
		//			continue;
		if ( tempent->s.teamNum == ent->s.teamNum )
		{
			return ( qfalse );	// Teammate is there! Don't fire!
		}
	}

	return ( qtrue );
}


/* */
qboolean
Can_Arty ( gentity_t *ent, vec3_t origin )
{
	int		e = 0;
	int		numListedEntities = 0;
	int		entityList[MAX_GENTITIES];

	//	int count = 0;
	vec3_t	mins, maxs;
	if ( VectorDistance( ent->r.currentOrigin, origin) < 900 )
	{
		return ( qfalse );
	}

	VectorCopy( origin, mins );
	mins[0] -= 500;
	mins[1] -= 500;
	mins[2] -= 500;
	VectorCopy( origin, maxs );
	mins[0] += 500;
	mins[1] += 500;
	mins[2] += 500;
	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
	for ( e = 0; e < numListedEntities; e++ )
	{
		gentity_t	*tempent = &g_entities[entityList[e]];
		if ( !tempent )
		{
			continue;
		}

		if ( !tempent->client || !tempent->health )
		{
			continue;
		}

		//		if (tempent == ent)
		//			continue;
		if ( tempent->s.teamNum == ent->s.teamNum && tempent->health > 0 )
		{
			return ( qfalse );
		}
	}

	// No friendlys found! Fire Away!
	return ( qtrue );
}


/* */
qboolean
Teammate_SpashDamage_OK ( gentity_t *ent, vec3_t origin )
{
	int		e = 0;
	int		numListedEntities = 0;
	int		entityList[MAX_GENTITIES];

	//	int count = 0;
	vec3_t	mins, maxs;
	if ( VectorDistance( ent->r.currentOrigin, origin) < 300 )
	{
		return ( qfalse );
	}

	VectorCopy( origin, mins );
	mins[0] -= 150;
	mins[1] -= 150;
	mins[2] -= 150;
	VectorCopy( origin, maxs );
	mins[0] += 150;
	mins[1] += 150;
	mins[2] += 150;
	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
	for ( e = 0; e < numListedEntities; e++ )
	{
		gentity_t	*tempent = &g_entities[entityList[e]];
		if ( !tempent )
		{
			continue;
		}

		if ( !tempent->client || !tempent->health )
		{
			continue;
		}

		//		if (tempent == ent)
		//			continue;
		if ( tempent->s.teamNum == ent->s.teamNum && tempent->health > 0 )
		{
			return ( qfalse );
		}
	}

	// No friendlys found! Fire Away!
	return ( qtrue );
}


/* */
qboolean
OriginVisible ( gentity_t *self, vec3_t origin )
{
	trace_t tr;
	vec3_t	eyes;
	vec3_t	origin2;
	VectorCopy( origin, origin2 );
	origin2[2] += 48;
	VectorCopy( self->r.currentOrigin, eyes );
	eyes[2] += 48;
	trap_Trace( &tr, eyes, NULL, NULL, origin2, self->s.number, MASK_SHOT );
	if ( tr.fraction == 1 )
	{
		return ( qtrue );
	}

	VectorCopy( origin, origin2 );
	origin2[2] += 64;
	VectorCopy( self->r.currentOrigin, eyes );
	eyes[2] += 64;
	trap_Trace( &tr, eyes, NULL, NULL, origin2, self->s.number, MASK_SHOT );
	if ( tr.fraction == 1 )
	{
		return ( qtrue );
	}

	/*VectorCopy(origin, origin2);
	origin2[2]+=96;

	VectorCopy(self->r.currentOrigin, eyes);
	eyes[2]+=96;

	trap_Trace (&tr, eyes, NULL, NULL, origin2, self->s.number, MASK_SHOT);

	if (tr.fraction == 1)
		return qtrue;*/
	return ( qfalse );
}


/* */
qboolean
Visible_to_player ( vec3_t origin )
{
	int loop = 0;
	while ( loop < MAX_CLIENTS )
	{					// Check for visible players.
		gentity_t	*ent = &g_entities[loop];
		if ( !ent )
		{				// An entity...
			if ( !ent->client )
			{			// An client structure...
				if ( !ent->is_bot )
				{		// Not a bot.
					if ( level.clients[loop].pers.connected == CON_CONNECTED )
					{	// Connected clients only.
						if ( OriginVisible( ent, origin) /*|| trap_InPVS( origin, ent->r.currentOrigin )*/ )
						{
							return ( qtrue );
						}
					}
				}
			}
		}

		loop++;
	}

	return ( qfalse );
}

char		temp_player_name[36];
gentity_t	*foundplayer;


/* */
qboolean
player_close ( vec3_t origin )
{
	int			loop = 0;
	qboolean	player_found = qfalse;
	while ( loop < MAX_CLIENTS )
	{									// Check for visible players.
		gentity_t	*ent = &g_entities[loop];
		if ( !(ent->r.svFlags & SVF_BOT) )
		{								// Not a bot.
			if ( level.clients[loop].pers.connected == CON_CONNECTED )
			{							// Connected clients only.
				player_found = qtrue;	// Mark that we have a real player connected somewhere...

				//if (VectorDistance(origin, ent->r.currentOrigin) < 2000)
				//if (VectorDistance(origin, ent->r.currentOrigin) < 2000)
				//if (VectorDistance(origin, ent->r.currentOrigin) < 3000)
				if ( VectorDistance( origin, ent->r.currentOrigin) < 4000 )

				//if (VectorDistance(origin, ent->r.currentOrigin) < 5000)
				{

					//if (trap_InPVS( origin, ent->r.currentOrigin ))
					//{
					//G_Printf("Player found... %s. \n", level.clients[loop].pers.netname);
					strcpy( temp_player_name, level.clients[loop].pers.netname );
					foundplayer = &g_entities[loop];
					return ( qtrue );

					//}
				}
			}
		}

		loop++;
	}

	if ( player_found )
	{									// Players found, but they arn't nearby... Find a new position.
		//G_Printf("Players found, but they arn't nearby... Find a new position.\n");
		return ( qfalse );
	}
	else
	{									// No actual players.. Let the bot be...
		//G_Printf("No actual players.. Let the bot be...\n");
		return ( qtrue );
	}
}


/* */
qboolean
player_too_close ( vec3_t origin )
{
	int			loop = 0;
	qboolean	player_found = qfalse;
	while ( loop < MAX_CLIENTS )
	{									// Check for visible players.
		gentity_t	*ent = &g_entities[loop];
		if ( !(ent->r.svFlags & SVF_BOT) )
		{								// Not a bot.
			if ( level.clients[loop].pers.connected == CON_CONNECTED )
			{							// Connected clients only.
				player_found = qtrue;	// Mark that we have a real player connected somewhere...

				//if (VectorDistance(origin, ent->r.currentOrigin) < 2000)
				//if (VectorDistance(origin, ent->r.currentOrigin) < 1000 /*|| trap_InPVS( origin, ent->r.currentOrigin )*/)
				if ( VectorDistance( origin, ent->r.currentOrigin) < 1500 )
				{

					//G_Printf("Player found... %s. \n", level.clients[loop].pers.netname);
					strcpy( temp_player_name, level.clients[loop].pers.netname );
					return ( qtrue );
				}
			}
		}

		loop++;
	}

	return ( qfalse );
}


/* */
int
player_total ( void )
{
	int loop = 0;
	int total = 0;
	while ( loop < MAX_CLIENTS )
	{											// Check for visible players.
		gentity_t	*ent = &g_entities[loop];
		if ( !(ent->r.svFlags & SVF_BOT) )
		{										// Not a bot.
			if ( level.clients[loop].pers.connected == CON_CONNECTED )
			{									// Connected clients only.
				total++;
			}
		}

		loop++;
	}

	return ( total );
}

int			random_number = 0;
extern int	Q_irand ( int value1, int value2 ); // UQ added - q_math.c


/* */
qboolean
OriginWouldTelefrag ( vec3_t origin )
{
	int			i, num;
	int			touch[MAX_GENTITIES];
	gentity_t	*hit;
	vec3_t		mins, maxs;
	VectorAdd( origin, playerMins, mins );
	VectorAdd( origin, playerMaxs, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
	for ( i = 0; i < num; i++ )
	{
		hit = &g_entities[touch[i]];
		if ( hit->client && hit->client->ps.stats[STAT_HEALTH] > 0 )
		{
			return ( qtrue );
		}
	}

	return ( qfalse );
}


/* */
void
Select_SP_Spawnpoint ( gentity_t *ent )
{															// Spawn or Reposition bot in CO-OP gametypes.
	vec3_t		origin, origin2, origin3;
	int			tries = 0;
	int			choice = Q_TrueRand( 1, number_of_nodes );	///10;

	//int choice = rand()%10;
	vmCvar_t	mapname;

	//choice+=random_number;
	//random_number = choice;
	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );
	if ( nodes[choice].origin )
	{
		VectorCopy( nodes[choice].origin, origin );
	}
	else
	{
		VectorSet( origin, 0, 0, 0 );
	}

	if ( nodes[choice + 10].origin )
	{
		VectorCopy( nodes[choice + 10].origin, origin2 );
	}
	else
	{
		VectorSet( origin2, 0, 0, 0 );
	}

	if ( nodes[choice - 10].origin )
	{
		VectorCopy( nodes[choice - 10].origin, origin3 );
	}
	else
	{
		VectorSet( origin3, 0, 0, 0 );
	}

	while
	(
		(
			!origin ||
			!origin2 ||
			!origin3 ||
			OriginWouldTelefrag( origin) ||
			(origin[0] == 0 && origin[1] == 0 && origin[2] == 0) ||
			(origin2[0] == 0 && origin2[1] == 0 && origin2[2] == 0) ||
			(origin3[0] == 0 && origin3[1] == 0 && origin3[2] == 0) ||
			TooManyBots( ent, origin)
		) ||
		Visible_to_player( origin) //		|| Visible_to_player (origin2)
		//		|| Visible_to_player (origin3)
		||
		!player_close( origin) ||
		player_too_close( origin) &&
		tries < 3000
	)
	{														// Try again.. And again.. etc..
		choice = Q_TrueRand( 1, number_of_nodes );			///10;	

		//choice = rand()%10;
		//choice+=random_number;
		//random_number = choice;
		if ( nodes[choice].origin )
		{
			VectorCopy( nodes[choice].origin, origin );
		}
		else
		{
			VectorSet( origin, 0, 0, 0 );
		}

		if ( nodes[choice + 10].origin )
		{
			VectorCopy( nodes[choice + 10].origin, origin2 );
		}
		else
		{
			VectorSet( origin2, 0, 0, 0 );
		}

		if ( nodes[choice - 10].origin )
		{
			VectorCopy( nodes[choice - 10].origin, origin3 );
		}
		else
		{
			VectorSet( origin3, 0, 0, 0 );
		}

		tries++;
	}

	if ( !ent->client->ps.viewangles )
	{
		VectorSet( ent->client->ps.viewangles, 0, 0, 0 );
	}

	if
	(
		(
			!origin ||
			!origin2 ||
			!origin3 ||
			OriginWouldTelefrag( origin) ||
			(origin[0] == 0 && origin[1] == 0 && origin[2] == 0) ||
			(origin2[0] == 0 && origin2[1] == 0 && origin2[2] == 0) ||
			(origin3[0] == 0 && origin3[1] == 0 && origin3[2] == 0) ||
			TooManyBots( ent, origin)
		) ||
		Visible_to_player( origin) //		|| Visible_to_player (origin2)
		//		|| Visible_to_player (origin3)
		||
		!player_close( origin) ||
		player_too_close( origin)
	)
	{														// A bad one.. Wait until next think instead...
	}
	else
	{														// Should be OK to spawn.
		origin[2] += 1;
		VectorCopy( origin, ent->client->ps.origin );

		// use the precise origin for linking
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
	}
}

void			UNIQUE_FALLBACK_NAVIGATION ( gentity_t *bot );
extern qboolean nodes_loaded;


/*void AIMOD_Special_Nodes( int node, trace_t *tr2 ) 
{
	vec3_t		forward, offset, end, angles;
	trace_t		*tr, tr3;
	float		dist;
	float		chMaxDist = 96;
	gentity_t	*checkEnt, *traceEnt = 0;
	int			numOfIgnoredEnts = 0;
	vec3_t		vector;
	vec3_t  maxs, mins;

	VectorSet (mins, 0, 0, 0);
	VectorSet (maxs, 16, 16, 16);

//	VectorSubtract( nodes[node-1].origin, nodes[node].origin, vector );
	VectorSubtract( nodes[node].origin, nodes[node-1].origin, vector );
	vectoangles( vector, angles );
	//VectorSubtract( nodes[node].origin, nodes[node-1].origin, angles );
	//VectorSubtract( nodes[node-1].origin, nodes[node].origin, angles );

	//AngleVectors (angles, forward, right, up);
	AngleVectors (angles, forward, NULL, NULL);

	VectorCopy( nodes[node].origin, offset );
	offset[2] += 16;//DEFAULT_VIEWHEIGHT;

	VectorMA( offset, chMaxDist, forward, end );

	//trace_contents = (CONTENTS_TRIGGER|CONTENTS_SOLID|CONTENTS_MISSILECLIP|CONTENTS_BODY|CONTENTS_CORPSE);
	trap_Trace(&tr3, offset, mins, maxs, end, -1 , MASK_ALL);
	tr = &tr3;

	//trace_contents = (CONTENTS_TRIGGER|CONTENTS_SOLID|CONTENTS_MISSILECLIP|CONTENTS_BODY|CONTENTS_CORPSE);
	//trap_Trace( tr, offset, NULL, NULL, end, -1, trace_contents );

	dist = VectorDistanceSquared( offset, tr->endpos );

//	if (!tr->entityNum)
//		return;

//	if( tr->fraction == 1 ) {
//		return;
//	}

	traceEnt = &g_entities[tr->entityNum];

//	if (!traceEnt)
//		return;

	//if( tr->entityNum == ENTITYNUM_WORLD )
	if ((tr->contents & CONTENTS_WATER) || (tr->surfaceFlags & SURF_LADDER) )
	{
		if ((tr->contents & CONTENTS_WATER)) {
			nodes[node].type = NODE_WATER;
			G_Printf("Node %i marked as a WATER node!\n", node);
		} else if( (tr->surfaceFlags & SURF_LADDER) ) { // ladder
			nodes[node].type = NODE_LADDER;
			G_Printf("Node %i marked as a LADDER node!\n", node);
		}
	} else {
		checkEnt = traceEnt;

		if(checkEnt) {

			// TDF This entire function could be the poster boy for converting to OO programming!!!
			// I'm making this into a switch in a vain attempt to make this readable so I can find which
			// brackets don't match!!!

			switch (checkEnt->s.eType) 
			{
				case ET_GENERAL:
					break;
				case ET_MG42_BARREL:
				case ET_AAGUN:
					nodes[node].type = NODE_MG42;
					G_Printf("Node %i marked as an MG42 node!\n", node);
					break;
				case ET_WOLF_OBJECTIVE:
				{
					nodes[node].type = NODE_DYNAMITE;
					G_Printf("Node %i marked as a DYNAMITE node!\n", node);
					break;
				}
				case ET_CONSTRUCTIBLE_INDICATOR:
				{
					nodes[node].type = NODE_DYNAMITE;
					G_Printf("Node %i marked as a DYNAMITE node!\n", node);
					break;
				}
				case ET_CONSTRUCTIBLE_MARKER:
				{
					nodes[node].type = NODE_DYNAMITE;
					G_Printf("Node %i marked as a DYNAMITE node!\n", node);
					break;
				}
				case ET_CONSTRUCTIBLE:
				{
					if( checkEnt->parent && checkEnt->parent->s.eType == ET_OID_TRIGGER ) 
					{
						if(checkEnt->parent->spawnflags & ALLIED_OBJECTIVE || checkEnt->parent->spawnflags & AXIS_OBJECTIVE)
						{
							nodes[node].type = NODE_DYNAMITE;
							G_Printf("Node %i marked as a DYNAMITE node!\n", node);
						}
					}
					break;
				}
				case ET_EXPLOSIVE:
				{
					//G_Printf("Explosive found...\n");
					if( checkEnt->spawnflags & EXPLOSIVE_TANK ) 
					{
						//G_Printf("Tank not marked...\n");
						//hintType = HINT_TANK;
					} else {
						switch( checkEnt->constructibleStats.weaponclass ) {
						case 0:
							nodes[node].type = NODE_DYNAMITE;
							G_Printf("Node %i marked as a DYNAMITE node!\n", node);
							break;
						case 1:
							//hintType = HINT_SATCHELCHARGE;
							nodes[node].type = NODE_DYNAMITE;
							G_Printf("Node %i marked as a DYNAMITE node!\n", node);
							break;
						case 2:
							if( checkEnt->parent && checkEnt->parent->s.eType == ET_OID_TRIGGER ) {
								if(checkEnt->parent->spawnflags & ALLIED_OBJECTIVE
									|| checkEnt->parent->spawnflags & AXIS_OBJECTIVE)
								{
									nodes[node].type = NODE_DYNAMITE;
									G_Printf("Node %i marked as a DYNAMITE node!\n", node);
								}
							}
							break;

						default:
							if( checkEnt->parent && checkEnt->parent->s.eType == ET_OID_TRIGGER ) {
								if(checkEnt->parent->spawnflags & ALLIED_OBJECTIVE
									|| checkEnt->parent->spawnflags & AXIS_OBJECTIVE)
								{
									nodes[node].type = NODE_DYNAMITE;
									G_Printf("Node %i marked as a DYNAMITE node!\n", node);
								}
							}
							break;
						}
					}

					break;
				}
				default:
					break;
			}
		}
	} 
}*/
float
AIMOD_Angle_Diff ( vec3_t viewangles, vec3_t angles )
{
	int		i;
	float	diff = 0.0f, angle;
	for ( i = 0; i < 2; i++ )
	{
		angle = AngleMod( viewangles[i] );
		angles[i] = AngleMod( angles[i] );
		diff = angles[i] - angle;
		if ( angles[i] > angle )
		{
			if ( diff > 180.0 )
			{
				diff -= 360.0;
			}
		}
		else
		{
			if ( diff < -180.0 )
			{
				diff += 360.0;
			}
		}
	}

	return ( diff );
}

extern int		last_node_hit_time[MAX_CLIENTS];
int				no_mount_time[MAX_CLIENTS]; // Don't try to mount emplaced for so many secs...
extern int		CreatePathAStar ( gentity_t *bot, int from, int to, short int *pathlist );
extern int		OrgVisibleBox ( vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore );
short int		BotGetNextNode ( gentity_t *bot /*, int *state*/ ); // below...
extern int		Unique_FindGoal ( gentity_t *bot );
extern qboolean MyVisible ( gentity_t *self, gentity_t *other );


/* */
void
set_follow_route ( gentity_t *ent )
{
	int			loop = 0;
	int			best_bot = -1;
	int			second_best_bot = -1;
	int			third_best_bot = -1;
	float		best_dist = 2000.0f;
	qboolean	skip = qfalse;
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*bot = &g_entities[loop];
		if ( bot && (bot->r.svFlags & SVF_BOT) )
		{
			if ( OnSameTeam( ent, bot) )
			{
				float	dist = VectorDistance( bot->r.currentOrigin, ent->r.currentOrigin );
				if ( dist < best_dist )
				{
					best_dist = dist;
					if ( best_bot >= 0 )
					{
						if ( second_best_bot >= 0 )
						{
							third_best_bot = second_best_bot;
						}

						second_best_bot = best_bot;
					}

					best_bot = loop;
				}
			}
		}
	}

	if ( best_bot >= 0 )
	{
		gentity_t	*bot = &g_entities[best_bot];
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{											// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{											// Release the emplaced gun we are on to do our job...
			//mg42_stopusing( bot );
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
		if ( !bot->current_node )
		{
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		}

		bot->movetarget = ent;

		//bot->longTermGoal = Unique_FindGoal( bot );
		bot->current_target_entity = best_bot;
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, bot );

		// So we can offset thinking for fps...
		bot->bot_route_think_delay = level.time + 1000;
		if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
		{											// We can't get there... Sorry!
			skip = qtrue;							// next think...
		}

		if ( !skip )
		{
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot ); //move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + 2000;

			// All is OK.. have a route to them.. Let them know!
			G_Voice( bot, NULL, SAY_TEAM, va( "Affirmative"), qfalse );
		}
	}

	skip = qfalse;
	if ( second_best_bot >= 0 )
	{
		gentity_t	*bot = &g_entities[second_best_bot];
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{											// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{											// Release the emplaced gun we are on to do our job...
			//mg42_stopusing( bot );
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
		if ( !bot->current_node )
		{
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		}

		bot->movetarget = ent;

		//bot->longTermGoal = Unique_FindGoal( bot );
		bot->current_target_entity = second_best_bot;
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, bot );

		// So we can offset thinking for fps...
		bot->bot_route_think_delay = level.time + 1000;
		if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
		{											// We can't get there... Sorry!
			skip = qtrue;							// next think...
		}

		if ( !skip )
		{
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot ); //move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + 2000;

			// All is OK.. have a route to them.. Let them know!
			G_Voice( bot, NULL, SAY_TEAM, va( "Affirmative"), qfalse );
		}
	}

	skip = qfalse;
	if ( third_best_bot >= 0 )
	{
		gentity_t	*bot = &g_entities[third_best_bot];
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{											// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{											// Release the emplaced gun we are on to do our job...
			//mg42_stopusing( bot );
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
		if ( !bot->current_node )
		{
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		}

		bot->movetarget = ent;

		//bot->longTermGoal = Unique_FindGoal( bot );
		bot->current_target_entity = third_best_bot;
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, bot );

		// So we can offset thinking for fps...
		bot->bot_route_think_delay = level.time + 1000;
		if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
		{											// We can't get there... Sorry!
			bot->movetarget = NULL;
			G_Voice( bot, NULL, SAY_TEAM, va( "Sorry"), qfalse );
			return;									// next think...
		}

		bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
		bot->next_node = BotGetNextNode( bot );		//move to this node first, since it's where our path starts from
		bot->node_timeout = level.time + 2000;

		// All is OK.. have a route to them.. Let them know!
		G_Voice( bot, NULL, SAY_TEAM, va( "Affirmative"), qfalse );
	}
}


/* */
void
set_engineer_route ( gentity_t *ent )
{
	int			loop = 0;
	int			best_eng = -1;
	int			second_best_eng = -1;
	float		best_dist = 2000.0f;
	qboolean	skip = qfalse;
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*bot = &g_entities[loop];
		if ( bot && (bot->r.svFlags & SVF_BOT) )
		{
			if ( OnSameTeam( ent, bot) && bot->client->sess.playerType == PC_ENGINEER )
			{
				float	dist = VectorDistance( bot->r.currentOrigin, ent->r.currentOrigin );
				if ( dist < best_dist )
				{
					best_dist = dist;
					if ( best_eng >= 0 )
					{
						second_best_eng = best_eng;
					}

					best_eng = loop;
				}
			}
		}
	}

	if ( best_eng >= 0 )
	{
		gentity_t	*bot = &g_entities[best_eng];
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{											// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{											// Release the emplaced gun we are on to do our job...
			//mg42_stopusing( bot );
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
		if ( !bot->current_node )
		{
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		}

		bot->movetarget = ent;

		//bot->longTermGoal = Unique_FindGoal( bot );
		bot->current_target_entity = best_eng;
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, bot );

		// So we can offset thinking for fps...
		bot->bot_route_think_delay = level.time + 1000;
		if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
		{											// We can't get there... Sorry!
			skip = qtrue;							// next think...
		}

		if ( !skip )
		{
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot ); //move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + 2000;

			// All is OK.. have a route to them.. Let them know!
			G_Voice( bot, NULL, SAY_TEAM, va( "IamEngineer"), qfalse );
		}
	}

	skip = qfalse;
	if ( second_best_eng >= 0 )
	{
		gentity_t	*bot = &g_entities[second_best_eng];
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{											// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{											// Release the emplaced gun we are on to do our job...
			//mg42_stopusing( bot );
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
		if ( !bot->current_node )
		{
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		}

		bot->movetarget = ent;

		//bot->longTermGoal = Unique_FindGoal( bot );
		bot->current_target_entity = second_best_eng;
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, bot );

		// So we can offset thinking for fps...
		bot->bot_route_think_delay = level.time + 1000;
		if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
		{											// We can't get there... Sorry!
			bot->movetarget = NULL;
			G_Voice( bot, NULL, SAY_TEAM, va( "Sorry"), qfalse );
			return;									// next think...
		}

		bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
		bot->next_node = BotGetNextNode( bot );		//move to this node first, since it's where our path starts from
		bot->node_timeout = level.time + 2000;

		// All is OK.. have a route to them.. Let them know!
		G_Voice( bot, NULL, SAY_TEAM, va( "IamEngineer"), qfalse );
	}
}


/* */
void
set_medic_route ( gentity_t *ent )
{
	int			loop = 0;
	int			best_medic = -1;
	int			second_best_medic = -1;
	float		best_dist = 2000.0f;
	qboolean	skip = qfalse;
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*bot = &g_entities[loop];
		if ( bot && (bot->r.svFlags & SVF_BOT) )
		{
			if ( OnSameTeam( ent, bot) && bot->client->sess.playerType == PC_MEDIC )
			{
				float	dist = VectorDistance( bot->r.currentOrigin, ent->r.currentOrigin );
				if ( dist < best_dist )
				{
					best_dist = dist;
					if ( best_medic >= 0 )
					{
						second_best_medic = best_medic;
					}

					best_medic = loop;
				}
			}
		}
	}

	if ( best_medic >= 0 )
	{
		gentity_t	*bot = &g_entities[best_medic];
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{											// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{											// Release the emplaced gun we are on to do our job...
			//mg42_stopusing( bot );
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
		if ( !bot->current_node )
		{
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		}

		bot->movetarget = ent;

		//bot->longTermGoal = Unique_FindGoal( bot );
		bot->current_target_entity = best_medic;
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, bot );

		// So we can offset thinking for fps...
		bot->bot_route_think_delay = level.time + 1000;
		if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
		{											// We can't get there... Sorry!
			skip = qtrue;							// next think...
		}

		if ( !skip )
		{
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot ); //move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + 2000;

			// All is OK.. have a route to them.. Let them know!
			G_Voice( bot, NULL, SAY_TEAM, va( "IamMedic"), qfalse );
		}
	}

	skip = qfalse;
	if ( second_best_medic >= 0 )
	{
		gentity_t	*bot = &g_entities[second_best_medic];
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{											// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{											// Release the emplaced gun we are on to do our job...
			//mg42_stopusing( bot );
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
		if ( !bot->current_node )
		{
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		}

		bot->movetarget = ent;

		//bot->longTermGoal = Unique_FindGoal( bot );
		bot->current_target_entity = second_best_medic;
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, bot );

		// So we can offset thinking for fps...
		bot->bot_route_think_delay = level.time + 1000;
		if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
		{											// We can't get there... Sorry!
			bot->movetarget = NULL;
			G_Voice( bot, NULL, SAY_TEAM, va( "Sorry"), qfalse );
			return;									// next think...
		}

		bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
		bot->next_node = BotGetNextNode( bot );		//move to this node first, since it's where our path starts from
		bot->node_timeout = level.time + 2000;

		// All is OK.. have a route to them.. Let them know!
		G_Voice( bot, NULL, SAY_TEAM, va( "IamMedic"), qfalse );
	}
}


/* */
void
set_fieldops_route ( gentity_t *ent )
{
	int			loop = 0;
	int			best_fieldops = -1;
	int			second_best_fieldops = -1;
	float		best_dist = 2000.0f;
	qboolean	skip = qfalse;
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*bot = &g_entities[loop];
		if ( bot && (bot->r.svFlags & SVF_BOT) )
		{
			if ( OnSameTeam( ent, bot) && g_entities[loop].client->sess.playerType == PC_FIELDOPS )
			{
				float	dist = VectorDistance( bot->r.currentOrigin, ent->r.currentOrigin );
				if ( dist < best_dist )
				{
					best_dist = dist;
					if ( best_fieldops >= 0 )
					{
						second_best_fieldops = best_fieldops;
					}

					best_fieldops = loop;
				}
			}
		}
	}

	if ( best_fieldops >= 0 )
	{
		gentity_t	*bot = &g_entities[best_fieldops];
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{											// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{											// Release the emplaced gun we are on to do our job...
			//mg42_stopusing( bot );
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
		if ( !bot->current_node )
		{
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		}

		bot->movetarget = ent;

		//bot->longTermGoal = Unique_FindGoal( bot );
		bot->current_target_entity = best_fieldops;
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, bot );

		// So we can offset thinking for fps...
		bot->bot_route_think_delay = level.time + 1000;
		if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
		{											// We can't get there... Sorry!
			skip = qtrue;							// next think...
		}

		if ( !skip )
		{
			bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
			bot->next_node = BotGetNextNode( bot ); //move to this node first, since it's where our path starts from
			bot->node_timeout = level.time + 2000;

			// All is OK.. have a route to them.. Let them know!
			G_Voice( bot, NULL, SAY_TEAM, va( "IamFieldOps"), qfalse );
		}
	}

	skip = qfalse;
	if ( second_best_fieldops >= 0 )
	{
		gentity_t	*bot = &g_entities[second_best_fieldops];
		if ( bot->client->ps.eFlags & EF_MOUNTEDTANK )
		{											// Release the emplaced gun we are on to do our job...
			G_LeaveTank( bot, qtrue );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		if ( bot->client->ps.eFlags & EF_MG42_ACTIVE )
		{											// Release the emplaced gun we are on to do our job...
			//mg42_stopusing( bot );
			Cmd_Activate_f( bot );
			no_mount_time[bot->s.clientNum] = level.time + 6000;
		}

		memset( bot->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
		if ( !bot->current_node )
		{
			bot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( bot, NODE_DENSITY, NODEFIND_ALL, bot );
		}

		bot->movetarget = ent;

		//bot->longTermGoal = Unique_FindGoal( bot );
		bot->current_target_entity = second_best_fieldops;
		bot->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, bot );

		// So we can offset thinking for fps...
		bot->bot_route_think_delay = level.time + 1000;
		if ( bot->longTermGoal < 0 || bot->longTermGoal > number_of_nodes )
		{											// We can't get there... Sorry!
			bot->movetarget = NULL;
			G_Voice( bot, NULL, SAY_TEAM, va( "Sorry"), qfalse );
			return;									// next think...
		}

		bot->pathsize = CreatePathAStar( bot, bot->current_node, bot->longTermGoal, bot->pathlist );
		bot->next_node = BotGetNextNode( bot );		//move to this node first, since it's where our path starts from
		bot->node_timeout = level.time + 2000;

		// All is OK.. have a route to them.. Let them know!
		G_Voice( bot, NULL, SAY_TEAM, va( "IamFieldOps"), qfalse );
	}
}


/* */
int
EntMove ( gentity_t *self, vec3_t moveTo, float stepSize )
{
	trace_t tr;
	vec3_t	stepTo; //, tempTo;
	vec3_t	stepSub;
	vec3_t	stepGoal;
	int		didMove = 0;

	/*	if (self->s.eFlags & EF_DEAD)
	{// Remove them for memory.
		G_FreeEntity(self);
		return 2;
	}

	if (self->s.groundEntityNum == ENTITYNUM_NONE)
	{
		return 2;
	}*/

	/*if ( self->s.owner >= 0 && self->s.owner <= level.maxclients && ExampleAnimEntAlignment(self) == 99)
	{
		//VectorCopy(g_entities[self->s.owner].r.currentOrigin, stepTo);
		VectorCopy(g_entities[self->s.owner].r.currentOrigin, tempTo);
		tempTo[0]+=50;
		tempTo[1]-=50;
		VectorCopy(tempTo, stepTo);

		if (self->health < 9999)
		{
			self->health = 9999;
			self->client->ps.stats[STAT_MAX_HEALTH] = 9999;
		}
	}
	else
	{*/
	VectorCopy( moveTo, stepTo );

	//}
	stepTo[2] = self->r.currentOrigin[2];
	VectorSubtract( stepTo, self->r.currentOrigin, stepSub );
	if ( VectorLength( stepSub) < 32 )
	{
		return ( 2 );
	}

	VectorNormalize( stepSub );

	/*if ( self->s.owner >= 0 && self->s.owner <= level.maxclients && ExampleAnimEntAlignment(self) == 99 )
	{*/
	stepGoal[0] = self->r.currentOrigin[0] + stepSub[0] * ( stepSize * 3 );
	stepGoal[1] = self->r.currentOrigin[1] + stepSub[1] * ( stepSize * 3 );
	stepGoal[2] = self->r.currentOrigin[2] + stepSub[2] * ( stepSize * 3 );

	/*}
	else
	{
		stepGoal[0] = self->r.currentOrigin[0] + stepSub[0]*stepSize;
		stepGoal[1] = self->r.currentOrigin[1] + stepSub[1]*stepSize;
		stepGoal[2] = self->r.currentOrigin[2] + stepSub[2]*stepSize;
	}*/
	trap_Trace( &tr, self->r.currentOrigin, self->r.mins, self->r.maxs, stepGoal, self->s.number, self->clipmask );
	if ( !tr.startsolid && !tr.allsolid && tr.fraction )
	{
		vec3_t	vecSub;
		VectorSubtract( self->r.currentOrigin, tr.endpos, vecSub );
		if ( VectorLength( vecSub) > (stepSize / 2) )
		{
			self->r.currentOrigin[0] = tr.endpos[0];
			self->r.currentOrigin[1] = tr.endpos[1];
			self->s.pos.trBase[0] = tr.endpos[0];
			self->s.pos.trBase[1] = tr.endpos[1];
			self->s.origin[0] = tr.endpos[0];
			self->s.origin[1] = tr.endpos[1];
			trap_LinkEntity( self );
			didMove = 1;
		}
	}

	if ( didMove != 1 )
	{				//stair check
		vec3_t	trFrom;
		vec3_t	trTo;
		vec3_t	trDir;
		vec3_t	vecMeasure;
		VectorCopy( tr.endpos, trFrom );
		trFrom[2] += 16;
		VectorSubtract( /*tr.endpos*/ stepGoal, self->r.currentOrigin, trDir );
		VectorNormalize( trDir );
		trTo[0] = tr.endpos[0] + trDir[0] * 2;
		trTo[1] = tr.endpos[1] + trDir[1] * 2;
		trTo[2] = tr.endpos[2] + trDir[2] * 2;
		trTo[2] += 16;
		VectorSubtract( trFrom, trTo, vecMeasure );
		if ( VectorLength( vecMeasure) > 1 )
		{
			trap_Trace( &tr, trFrom, self->r.mins, self->r.maxs, trTo, self->s.number, self->clipmask );
			if ( !tr.startsolid && !tr.allsolid && tr.fraction == 1 )
			{		//clear trace here, probably up a step
				vec3_t	trDown;
				vec3_t	trUp;
				VectorCopy( tr.endpos, trUp );
				VectorCopy( tr.endpos, trDown );
				trDown[2] -= 16;
				trap_Trace( &tr, trFrom, self->r.mins, self->r.maxs, trTo, self->s.number, self->clipmask );
				if ( !tr.startsolid && !tr.allsolid )
				{	//plop us down on the step after moving up
					VectorCopy( tr.endpos, self->r.currentOrigin );
					VectorCopy( tr.endpos, self->s.pos.trBase );
					VectorCopy( tr.endpos, self->s.origin );
					trap_LinkEntity( self );
					didMove = 1;
				}
			}
		}
	}

	return ( didMove );
}

extern int	JobType ( gentity_t *bot );
extern void LostBot_Set ( gentity_t *bot );


/* */
void
UNIQUE_FALLBACK_NAVIGATION ( gentity_t *bot )
{
	vec3_t /*b_angle, fwd,*/ trto, trto2;	//, mins, maxs;

	//	trace_t		tr;
	int						choice;
	usercmd_t				*ucmd;
	return;
	ucmd = &bot->client->pers.cmd;
	if ( bot->client->AImovetime > level.time )
	{

		//EntMove(bot, bot->client->AIdestination, 300);
		//AIMOD_MOVEMENT_Move( bot, ucmd );
		return;
	}

	bot->client->AImovetime = level.time + 1500;
	trto[2] = bot->r.currentOrigin[2];
	choice = rand() % 4;
	if ( choice == 1 )
	{										// Reverse Directions
		trto[0] = bot->r.currentOrigin[0] + ( choice * 64 );
		trto[1] = bot->r.currentOrigin[1] - ( choice * 64 );
	}
	else if ( choice == 2 )
	{										// Reverse Directions
		trto[0] = bot->r.currentOrigin[0] + ( choice * 64 );
		trto[1] = bot->r.currentOrigin[1] - ( choice * 64 );
	}
	else if ( choice == 3 )
	{										// Reverse Directions
		trto[0] = bot->r.currentOrigin[0] - ( choice * 64 );
		trto[1] = bot->r.currentOrigin[1] - ( choice * 64 );
	}
	else
	{										// Normal Directions
		trto[0] = bot->r.currentOrigin[0] + ( choice * 64 );
		trto[1] = bot->r.currentOrigin[1] + ( choice * 64 );
	}

	VectorSubtract( bot->r.currentOrigin, trto, trto2 );
	LostBot_Set( bot );
}

//===========================================================================
// AVD Copied this, made some alterations
// Routine      : AIMOD_NAVIGATION_FindCloseReachableNode
// Description  : Find a close node to the player within given distance
//              : Faster than looking for the closest node, but not very accurate
int				AIMOD_NAVIGATION_FindClosestReachableNode ( gentity_t *bot, int r, int type, gentity_t *master );
int				AIMOD_NAVIGATION_FindClosestReachableNodes ( gentity_t *bot, int r, int type );
int				AIMOD_NAVIGATION_FindFarReachableNode ( gentity_t *bot, int r, int type );
extern qboolean AICast_InFieldOfVision ( vec3_t viewangles, float fov, vec3_t angles );


/* */
int
AIMOD_NAVIGATION_FindFastFarthestReachableNode ( gentity_t *bot, int r, int type )
{
	int		i;
	float	closest;
	float	dist;
	int		node = -1;
	vec3_t	v;
	trace_t tr;
	float	rng;
	vec3_t	maxs, mins;

	//	int			ai;
	VectorCopy( bot->client->ps.mins, mins );
	VectorCopy( bot->client->ps.maxs, maxs );

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );
	closest = 99999;	//rng + 1;			// Cuz if we found none in the range, we just found none .
	for ( i = 0; i < number_of_nodes; i += rand() % 15 )
	{					// Incrument by up to 15 for speed.
		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
		dist = VectorLength( v );
		if (			/*nodes[i].origin[2] - bot->r.currentOrigin[2] < 128//48*/
				/*&& nodes[i].origin[2] < bot->r.currentOrigin[2] + 128//48*/dist > closest &&
				dist > 800
			)
		{
			vec3_t	eyes;
			VectorCopy( bot->r.currentOrigin, eyes );
			eyes[2] += 16;
			trap_Trace( &tr, eyes, mins, maxs, nodes[i].origin, bot->s.number, MASK_SHOT | MASK_OPAQUE );

			//trap_Trace(&tr, bot->r.currentOrigin, mins, maxs, nodes[i].origin, bot->s.number , MASK_SHOT|MASK_OPAQUE);
			if ( tr.fraction == 1.0 )
			{
				node = i;
				closest = dist;
			}
		}
	}

	return ( node );
}


/* */
int
AIMOD_NAVIGATION_FindFarthestReachableNode ( gentity_t *bot, int r, int type )
{
	int		i;
	float	closest;
	float	dist;
	int		node = -1;
	vec3_t	v;
	trace_t tr;
	float	rng;
	vec3_t	maxs, mins;

	//	int			ai;
	int		quick = AIMOD_NAVIGATION_FindFastFarthestReachableNode( bot, r, type );
	if ( quick != INVALID )
	{																// Return the fast one.. Should create more randomcy and freedom of movement.
		return ( quick );
	}

	VectorCopy( bot->client->ps.mins, mins );
	VectorCopy( bot->client->ps.maxs, maxs );

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );
	closest = 99999;												//rng + 1;			// Cuz if we found none in the range, we just found none .
	for ( i = 0; i < number_of_nodes; i += Q_TrueRand( 1, 4) /*i++*/ )
	{
		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
		dist = VectorLength( v );
		if ( dist > closest && dist > 600 )
		{
			vec3_t	eyes;
			VectorCopy( bot->r.currentOrigin, eyes );
			eyes[2] += 16;
			trap_Trace( &tr, eyes, mins, maxs, nodes[i].origin, bot->s.number, MASK_SHOT | MASK_OPAQUE );

			//trap_Trace(&tr, bot->r.currentOrigin, mins, maxs, nodes[i].origin, bot->s.number , MASK_SHOT|MASK_OPAQUE);
			if ( tr.fraction == 1.0 )
			{
				node = i;
				closest = dist;
			}
		}
	}

	return ( node );
}

extern qboolean OnIce[MAX_CLIENTS];									// Set if bot is on ice or not.
extern int		exitnode[MAX_CLIENTS];								// The way outa here!
extern qboolean AI_PM_SlickTrace ( vec3_t point, int clientNum );	// bg_pmove.c - Qtrue = On Ice. Qfalse = Off Ice.
extern int		NodeVisible ( vec3_t org1, vec3_t org2, int ignore );
extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );


//===========================================================================
// Routine      : AIMOD_NAVIGATION_FindAbsoluteClosestReachableNode

// Description  : Find the closest node to the player within a certain range
int
AIMOD_NAVIGATION_FindAbsoluteClosestReachableNode ( gentity_t *bot, int r, int type )
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
	int		upto = 0;
	if ( OnIce[bot->s.number] && exitnode[bot->s.number] > 0 )
	{															// When on ice, find a far node to get off it!
		bot->exit_node = exitnode[bot->s.number];
		return ( bot->exit_node );
	}

	VectorCopy( playerMins, mins );
	VectorCopy( playerMaxs, maxs );

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );
	closest = rng + 1;											// Cuz if we found none in the range, we just found none .
	nonLOSclosest = rng + 1;									// Cuz if we found none in the range, we just found none .
	nonFOVclosest = rng + 1;									// Cuz if we found none in the range, we just found none .

	//
	// Will attempt a close only check first with vis checks...
	//
	closest = 128.0f;											//64.0f;
	for ( i = 0; i < number_of_nodes; i += Q_TrueRand( 1, 4) )	//i++ ) // Randomize a little???
	{
		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
		dist = VectorLength( v );
		if ( dist < closest && i != bot->current_node && i != bot->next_node )
		{
			if ( !(nodes[i].type & NODE_ICE) )
			{													// Not on ice (slick)...
				node = i;
				closest = dist;
			}
		}
	}

	return ( node );
}


//===========================================================================
// Routine      : AIMOD_NAVIGATION_FindClosestReachableNode

// Description  : Find the closest node to the player within a certain range
int
AIMOD_NAVIGATION_FindClosestReachableNode ( gentity_t *bot, int r, int type, gentity_t *master )
{
#ifndef __NODE_SORT__
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
	int		upto = 0;
	if ( OnIce[bot->s.number] && exitnode[bot->s.number] > 0 )
	{															// When on ice, find a far node to get off it!
		bot->exit_node = exitnode[bot->s.number];
		return ( bot->exit_node );
	}

	VectorCopy( playerMins, mins );
	VectorCopy( playerMaxs, maxs );

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );
	closest = rng + 1;											// Cuz if we found none in the range, we just found none .
	nonLOSclosest = rng + 1;									// Cuz if we found none in the range, we just found none .
	nonFOVclosest = rng + 1;									// Cuz if we found none in the range, we just found none .

	//
	// Will attempt a close only check first with vis checks...
	//
#ifdef __VEHICLES__
	if ( master->client && (master->client->ps.eFlags & EF_TANK) )
	{
		closest = 512.0f;										//64.0f;
	}
	else
#endif //__VEHICLES__
		closest = 128.0f;										//64.0f;

	//closest = 128.0f;
	//closest = 256.0f;
	for ( i = 0; i < number_of_nodes; i += Q_TrueRand( 1, 4) )	//i++ ) // Randomize a little???
	{
		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
		dist = VectorLength( v );
		if
		(
			dist < closest &&
			i != bot->current_node &&
			i != bot->next_node //&& nodes[i].origin[2] - bot->r.currentOrigin[2] < 32 )  // Height check... never a node above us too far..
			&&
			!BAD_WP_Height( bot->r.currentOrigin, nodes[i].origin)
		)
		{
#ifdef __VEHICLES__
			if ( master->client && (master->client->ps.eFlags & EF_TANK) )
			{
				if ( master->client && (master->client->ps.eFlags & EF_TANK) && !(nodes[i].type & NODE_LAND_VEHICLE) )
				{
					continue;									// Tank driving bots should only use tank nodes!
				}
			}
			else if ( master->client && !(master->client->ps.eFlags & EF_TANK) )
			{
				if ( master->client && (master->client->ps.eFlags & EF_TANK) && (nodes[i].type & NODE_LAND_VEHICLE) )
				{
					continue;									// Non-Tank driving bots should NOT use tank nodes!
				}
			}
#endif //__VEHICLES__
			if ( !(nodes[i].type & NODE_ICE) )
			{													// Not on ice (slick)...
#ifdef __VEHICLES__
				if ( master->client && (master->client->ps.eFlags & EF_TANK) )
				{
					int			j = 0;
					qboolean	bad = qtrue;
					for ( j = 0; j < nodes[i].enodenum; j++ )
					{
						if ( !(nodes[i].links[j].flags & PATH_NOTANKS) )
						{
							bad = qfalse;
							break;
						}
					}

					if ( bad )
					{
						continue;
					}
				}
#endif //__VEHICLES__

				//if (OrgVisible(bot->r.currentOrigin, nodes[i].origin, bot->s.number))

				/*if (dist > 16)
				{// Check visibility, just in case!
					if (NodeVisible(bot->r.currentOrigin, nodes[i].origin, bot->s.number) >= 1)
					{// Never go onto ice!
						secondbest = node;
						node = i;
						closest = dist;
					}
				}
				else*/
				{												// Assume it is visible...
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

#else //__NODE_SORT__
	// UQ1: This is a bit slower then I would have hoped :(
	int i;
	int upto = 0;
	int best_nodes[MAX_NODES];
	int best_node = -1;
	int random_selection = -1;
	int selectable_nodes[MAX_NODES];
	int selection_upto = 0;

	//ent->in_range_nodes[ent->num_in_range_nodes] = best_nodes[i];
	//ent->num_in_range_nodes++;
	if ( bot->num_in_range_nodes > 0
#ifdef __VEHICLES__
	&& !(master->client && (master->client->ps.eFlags & EF_TANK))
#endif //__VEHICLES__		
	)
	{															// Already have some for this goal/objective...
		if ( bot->num_in_range_nodes >= 16 )
		{
			best_node = bot->in_range_nodes[Q_TrueRand( 0, 15 )];
		}
		else if ( bot->num_in_range_nodes >= 8 )
		{
			best_node = bot->in_range_nodes[Q_TrueRand( 0, 7 )];
		}
		else if ( bot->num_in_range_nodes >= 4 )
		{
			best_node = bot->in_range_nodes[Q_TrueRand( 0, 3 )];
		}
		else
		{
			best_node = bot->in_range_nodes[Q_TrueRand( 0, bot->num_in_range_nodes - 1 )];
		}

		return ( best_node );
	}

	best_nodes[0] = -1;
	for ( i = 0; i < number_of_nodes; i += Q_TrueRand( 1, 4) )
	{
		float	best_weight = 256.0f;//128.0f;
		float	weight = VectorDistance( bot->r.currentOrigin, nodes[i].origin );
#ifdef __VEHICLES__
		if ( master->client && (master->client->ps.eFlags & EF_TANK) )
		{
			best_weight = 512.0f;								//64.0f;
		}
#endif //__VEHICLES__
		if ( weight > best_weight )
		{
			continue;
		}

		if ( BAD_WP_Height( bot->r.currentOrigin, nodes[i].origin) )
		{
			continue;
		}

#ifdef __VEHICLES__
		if ( master->client && (master->client->ps.eFlags & EF_TANK) )
		{
			if ( master->client && (master->client->ps.eFlags & EF_TANK) && !(nodes[i].type & NODE_LAND_VEHICLE) )
			{
				continue;										// Tank driving bots should only use tank nodes!
			}
		}
		else if ( master->client && !(master->client->ps.eFlags & EF_TANK) )
		{
			if ( master->client && (master->client->ps.eFlags & EF_TANK) && (nodes[i].type & NODE_LAND_VEHICLE) )
			{
				continue;										// Non-Tank driving bots should NOT use tank nodes!
			}
		}
#endif //__VEHICLES__
		selectable_nodes[selection_upto] = i;
		selection_upto++;
	}

	for ( i = 0; i < selection_upto; i++ )
	{
		int		j;
		float	best_weight = 128.0f;
		float	weight = VectorDistance( bot->r.currentOrigin, nodes[selectable_nodes[i]].origin );
#ifdef __VEHICLES__
		if ( master->client && (master->client->ps.eFlags & EF_TANK) )
		{
			best_weight = 512.0f;								//64.0f;
		}
#endif //__VEHICLES__
		for ( j = 0; j < selection_upto; j++ )
		{
			qboolean	already_in_list = qfalse;
			int			z;
			float		weight2 = VectorDistance( bot->r.currentOrigin, nodes[selectable_nodes[j]].origin );
			if ( weight2 > best_weight )
			{
				continue;
			}

			for ( z = 0; z < upto; z++ )
			{
				if ( best_nodes[z] == selectable_nodes[j] )
				{
					already_in_list = qtrue;
					break;
				}
			}

			if ( !already_in_list && weight2 < best_weight )
			{													// Best so far...
				best_node = selectable_nodes[j];
				best_weight = weight2;
			}
		}

		best_nodes[upto] = best_node;
		upto++;
	}

	for ( i = 0; i < upto; i++ )
	{															// Return the closest one that is visible, checking visibilities only as far as needed for CPU time saving...
#ifdef __VEHICLES__
		if ( master->client && (master->client->ps.eFlags & EF_TANK) && bot != master )
		{														// Tanks don't need to actually be visible to the objective!
			return ( best_nodes[i] );
		}
		else													
#endif //__VEHICLES__
		if ( NodeVisible( bot->r.currentOrigin, nodes[best_nodes[i]].origin, bot->s.number) >= 1 )
		{

			//if (rand()%100 < 50 && upto > 1 && i < upto-4)
			//	continue;
			//if (bot && bot->client)
			//	G_Printf("%s - Visibility checked %i nodes from a best nodes total of %i.\n", bot->client->pers.netname, i+1, upto);
			return ( best_nodes[i] );
		}
	}

	// None found! Pick one of the first ones randomly!

	/*	if (upto-1 <= 0)
	{
		G_Printf("[NO VISIBLE NODES] Chose node -1 from a best nodes total of %i.\n", upto);
		return -1;
	}
	else if (upto-1 <= 10)
		random_selection = Q_TrueRand(0, upto-1);
	else
		random_selection = Q_TrueRand(0, 10);

	G_Printf("[NO VISIBLE NODES] %s!!!\n", bot->client->pers.netname);
	return best_nodes[random_selection];*/
	if ( !best_nodes[0] )
	{
		best_nodes[0] = -1;
	}

	if ( master && master->client )
	{

		//G_Printf("[NO VISIBLE NODES] %s!!! (Returning node %i)\n", bot->client->pers.netname, -1/*best_nodes[0]*/);
		master->bot_random_move_time = level.time + 500;
	}

	return ( -1 /*best_nodes[0]*/ );
#endif //__NODE_SORT__
}


//===========================================================================
// Routine      : AIMOD_NAVIGATION_FindClosestReachableNodeWithIgnore

// Description  : Find the closest node to the player within a certain range
int
AIMOD_NAVIGATION_FindClosestReachableNodeWithIgnore ( gentity_t *bot, int ignore, int r, int type )
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
	if ( OnIce[bot->s.number] && exitnode[bot->s.number] > 0 )
	{							// When on ice, find a far node to get off it!
		bot->exit_node = exitnode[bot->s.number];
		return ( bot->exit_node );
	}

	VectorCopy( playerMins, mins );
	VectorCopy( playerMaxs, maxs );

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );
	closest = rng + 1;			// Cuz if we found none in the range, we just found none .
	nonLOSclosest = rng + 1;	// Cuz if we found none in the range, we just found none .
	nonFOVclosest = rng + 1;	// Cuz if we found none in the range, we just found none .

	//
	// Will attempt a close only check first with vis checks...
	//
	//closest = 128.0f;
	closest = 256.0f;
	for ( i = 0; i < number_of_nodes; i += Q_TrueRand( 1, 4) /*i++*/ )	// Randomize a little???
	{
		if ( i == ignore )
		{
			continue;
		}

		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
		dist = VectorLength( v );
		if
		(
			dist < closest &&
			dist < 512 &&
			i != bot->current_node &&
			i != bot->next_node //&& nodes[i].origin[2] - bot->r.currentOrigin[2] < 32 )  // Height check... never a node above us too far..
			&&
			!BAD_WP_Height( bot->r.currentOrigin, nodes[i].origin)
		)
		{
			if ( !(nodes[i].type & NODE_ICE) )
			{															// Not on ice (slick)...
#ifdef __VEHICLES__
				if ( bot->client && (bot->client->ps.eFlags & EF_TANK) )
				{
					int			j = 0;
					qboolean	bad = qtrue;
					for ( j = 0; j < nodes[i].enodenum; j++ )
					{
						if ( !(nodes[i].links[j].flags & PATH_NOTANKS) )
						{
							bad = qfalse;
							break;
						}
					}

					if ( bad )
					{
						continue;
					}
				}
#endif //__VEHICLES__

				//if (OrgVisible(bot->r.currentOrigin, nodes[i].origin, bot->s.number))
				//if (NodeVisible(bot->r.currentOrigin, nodes[i].origin, bot->s.number) >= 1)
				{														// Never go onto ice!
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


/* 
 * Swap function for sorting.
 */
void
swap ( int a, int b )
{
	int tmp;	// Exchange temp value.
	tmp = a;
	a = b;
	b = tmp;
}


/* 
 * Partition function for the recursive quicksort.  It takes a pointer
 * to the start of the array, and its size, and partitions it, returning a
 * the location of the split point.
 */
int
split ( int arr[], int size )
{
	int splitval = arr[0];	// Use first value as split value.
	int left = 1;			// Left (low) end scanner.
	int right = size - 1;	// Right (high) end scanner.
	while ( 1 )
	{

		// Scan tward middle until you find a items which are out
		// of place at each end.
		while ( left <= right && arr[left] <= splitval )
		{
			left++;
		}

		while ( splitval < arr[right] )
		{
			right--;
		}

		// If they passed each other, we are done.  Otherwise,
		// swap the elements and try again.
		if ( left > right )
		{
			break;
		}
		else
		{
			swap( arr[left], arr[right] );
		}
	}

	/* Move the pivot into place, and return. */
	swap( arr[0], arr[right] );
	return ( right );
}


/* 
 * Recursive quicksort.  It takes a pointer to the data and a size,
 * and it sorts the data.
 */
void
sort ( int data[], int size )
{
	if ( size > 1 )
	{

		/* Split the array, and recursively sort each part. */
		int splitloc = split( data, size );
		sort( data, splitloc );
		sort( data + splitloc + 1, size - splitloc - 1 );
	}
}


//===========================================================================
// Routine      : AIMOD_NAVIGATION_FindClosestReachableNodes

// Description  : Find the closest nodes to the player within a certain range
int
AIMOD_NAVIGATION_FindClosestReachableNodes ( gentity_t *bot, int r, int type )
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
	int		close_list[MAX_NODES];
	int		close_total = 0;
	float	farthest_dist = 0.0f;
	int		farthest_node = -1;
	return ( AIMOD_NAVIGATION_FindClosestReachableNode( bot, r, type, bot) );
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
	closest = 256.0f;						//127.0f;
	for ( i = 0; i < number_of_nodes; i++ ) // Randomize a little???
	{
		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
		dist = VectorLength( v );
		if
		(
			dist < closest &&
			dist < 512 &&
			i != bot->current_node &&
			i != bot->next_node &&
			nodes[i].origin[2] - bot->r.currentOrigin[2] < 32
		)									// Height check... never a node above us too far..
		{
			vec3_t	eye1, eye2;
			VectorCopy( bot->r.currentOrigin, eye1 );
			eye1[2] += 16;
			VectorCopy( nodes[i].origin, eye2 );
			eye2[2] += 16;

			//if ( !(nodes[i].type & NODE_ICE) )
			{								// Not on ice (slick)...
#ifdef __VEHICLES__
				if ( bot->client && (bot->client->ps.eFlags & EF_TANK) )
				{
					int			j = 0;
					qboolean	bad = qtrue;
					for ( j = 0; j < nodes[i].enodenum; j++ )
					{
						if ( !(nodes[i].links[j].flags & PATH_NOTANKS) )
						{
							bad = qfalse;
							break;
						}
					}

					if ( bad )
					{
						continue;
					}
				}
#endif //__VEHICLES__
				if ( OrgVisible( eye1, eye2, bot->s.number) )
				{							// Never go onto ice!
					close_list[close_total] = i;
					close_total++;
					node = i;
					closest = dist;
				}
			}
		}
	}

	// Now sort the totals list...
	sort( close_list, close_total );
	bot->current_nodes_total = 0;

	//memset(&bot->current_nodes, NODE_INVALID, sizeof(bot->current_nodes));
	// Now assign the best 32 nodes to our current list...
	for ( i = 0; i < 32 && i < close_total; i++ )
	{
		bot->current_nodes[i] = close_list[i];
		bot->current_nodes_total++;
	}

	bot->current_node = node;

	// Returns the best choice...
	return ( node );
}


//===========================================================================
// Routine      : AIMOD_NAVIGATION_FindClosestReachableNodesTo

// Description  : Find the closest nodes to the given origin within a certain range
int
AIMOD_NAVIGATION_FindClosestReachableNodesTo ( gentity_t *bot, vec3_t origin, int r, int type )
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
	int		close_list[MAX_NODES];
	int		close_total = 0;
	float	farthest_dist = 0.0f;
	int		farthest_node = -1;
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
	closest = 512.0f;						//256.0f;//127.0f;
	for ( i = 0; i < number_of_nodes; i++ ) // Randomize a little???
	{
		VectorSubtract( nodes[i].origin, origin, v );
		dist = VectorLength( v );
		if
		(
			dist < closest &&
			dist < 512 &&
			i != bot->current_node &&
			i != bot->next_node				/*&& nodes[i].origin[2] - origin[2] < 32*/
		)									// Height check... never a node above us too far..
		{
			vec3_t	eye1, eye2;
			VectorCopy( origin, eye1 );
			eye1[2] += 16;
			VectorCopy( nodes[i].origin, eye2 );
			eye2[2] += 16;

			//if ( !(nodes[i].type & NODE_ICE) )
			{								// Not on ice (slick)...
				//if (OrgVisible(eye1, eye2, bot->s.number))
				//if (OrgVisible(origin, nodes[i].origin, bot->s.number))
				{							// Never go onto ice!
					close_list[close_total] = i;
					close_total++;
					node = i;
					closest = dist;
				}
			}
		}
	}

	// Now sort the totals list...
	sort( close_list, close_total );
	bot->longTermGoalNodes_total = 0;

	//memset(&bot->longTermGoalNodes, NODE_INVALID, sizeof(bot->longTermGoalNodes));
	// Now assign the best 32 nodes to our current list...
	for ( i = 0; i < 32 && i < close_total; i++ )
	{
		bot->longTermGoalNodes[i] = close_list[i];
		bot->longTermGoalNodes_total++;
	}

	bot->longTermGoal = node;

	//G_Printf("longTermGoalNodes_total is %i.\n", bot->longTermGoalNodes_total);
	// Returns the best choice...
	return ( node );
}


//===========================================================================
// Routine      : AIMOD_NAVIGATION_FindClosestReachableNodeTo

// Description  : Find the closest node to the origin within a certain range
int
AIMOD_NAVIGATION_FindClosestReachableNodeTo ( vec3_t origin, int r, int type )
{
	int		i;
	float	closest;
	float	nonLOSclosest;
	float	nonFOVclosest;
	float	dist;
	int		node = -1;

	//	int         nonLOSnode=-1;
	//	int         nonFOVnode=-1;
	vec3_t	v;
	float	rng;
	vec3_t	maxs, mins;

	//	int firstchoice = INVALID;
	//	int		choice = rand()%10;
	VectorCopy( playerMins, mins );
	VectorCopy( playerMaxs, maxs );

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );
	closest = rng + 1;																// Cuz if we found none in the range, we just found none .
	nonLOSclosest = rng + 1;														// Cuz if we found none in the range, we just found none .
	nonFOVclosest = rng + 1;														// Cuz if we found none in the range, we just found none .
	for ( i = 0; i < number_of_nodes; i += Q_TrueRand( 1, 3) /*i++*/ )				// Randomize a little???
	{
		VectorSubtract( nodes[i].origin, origin, v );
		dist = VectorLength( v );
		if ( dist < closest && dist < 512 && nodes[i].origin[2] - origin[2] < 32 )	// Height check... never a node above us too far..
		{

			//if (OrgVisible(bot->r.currentOrigin, nodes[i].origin, bot->s.number))
			//if (!AI_PM_SlickTrace( nodes[i].origin, bot->s.number ))
			if ( !(nodes[i].type & NODE_ICE) )
			{									// Not on ice (slick)...
				//if (OrgVisible(origin, nodes[i].origin, -1))
				{								// Never go onto ice!
					node = i;
					closest = dist;
				}
			}
		}
	}

	return ( node );
}

int			last_node_check[MAX_CLIENTS];
extern int	last_node_set_time[MAX_CLIENTS];	// AIMOD_movement.c
extern int	last_node_hit_time[MAX_CLIENTS];	// AIMOD_movement.c


/*
int AIMOD_FindIceExitRoute(int clientNum)
{
    int         i;
    float       closest;
    float       dist;
    int         node=-1;
    vec3_t      v;
    float       rng;
    vec3_t      maxs, mins;
//	int			ai;
	trace_t		tr;//, tr2;
	vec3_t		eyes;
	gentity_t	*bot = &g_entities[clientNum];
	int			r = NODE_DENSITY;
	int			type = NODEFIND_ALL;

    VectorCopy( bot->client->ps.mins, mins );
    VectorCopy( bot->client->ps.maxs, maxs );
    
	// Stepsize
	mins[2] += 18; 
    // Duck slightly
	maxs[2] -= 16;

    // Square Range for distance comparison (eliminated sqrt)
    rng = (float)(r * r); 
	closest = rng + 1;			// Cuz if we found none in the range, we just found none .

	VectorCopy(bot->r.currentOrigin, eyes);
	eyes[2] += 16;

    for( i = 0; i < number_of_nodes; i++)
    {       
		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
		dist = VectorLength( v );
        
		if ( dist < closest ) 
		{
			trap_Trace(&tr, bot->r.currentOrigin, mins, maxs, nodes[i].origin, bot->s.number, MASK_ALL);

			if (tr.fraction < 0.95 
				&& !AI_PM_SlickTrace( nodes[i].origin, bot->s.clientNum ))
			{
				node = i;
				closest = dist;
			}
		}
	}

    return node;
}
*/
int
AIMOD_NAVIGATION_FindRunawayReachableNode ( gentity_t *bot, int r, int type )
{
	int		i;
	float	closest;
	float	nonLOSclosest;
	float	nonFOVclosest;
	float	dist;
	int		node = -1;

	//	int         nonLOSnode=-1;
	//	int         nonFOVnode=-1;
	vec3_t	v;
	float	rng;
	vec3_t	maxs, mins;

	//	int			firstchoice = INVALID;
	vec3_t	eyes;

	//	int			choice = rand()%10;
	if ( OnIce[bot->s.clientNum] && exitnode[bot->s.clientNum] > 0 )
	{							// When on ice, find a far node to get off it!
		bot->exit_node = exitnode[bot->s.clientNum];
		return ( bot->exit_node );
	}

	VectorCopy( bot->client->ps.mins, mins );
	VectorCopy( bot->client->ps.maxs, maxs );

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );

	//closest = rng + 1;			// Cuz if we found none in the range, we just found none .
	closest = 400.0f;
	nonLOSclosest = rng + 1;	// Cuz if we found none in the range, we just found none .
	nonFOVclosest = rng + 1;	// Cuz if we found none in the range, we just found none .
	VectorCopy( bot->r.currentOrigin, eyes );
	eyes[2] += 16;
	if ( node == -1 )
	{							// Find another node.
		for ( i = 0; i < number_of_nodes; i++ /*i+=rand()%4*/ /*i++*/ )
		{
			VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );
			dist = VectorLength( v );
			if ( dist > closest && dist < 1800 //&& dist < 256
			&& i != bot->current_node && i != bot->next_node )
			{
				vec3_t	eyes;
				VectorCopy( nodes[i].origin, eyes );
				eyes[2] += 16;
				if ( OrgVisible( eyes, bot->r.currentOrigin, bot->s.clientNum) )
				{
					node = i;
					closest = dist;
				}
			}
		}
	}

	return ( node );
}


/* */
int
AIMOD_NAVIGATION_FindClosestReachableNodeORIG ( gentity_t *bot, int r, int type )
{
	int		i;
	float	closest;
	float	dist;
	int		node = -1;
	vec3_t	v;
	trace_t tr;
	float	rng;
	vec3_t	maxs, mins;

	//	int			ai;
	VectorCopy( bot->client->ps.mins, mins );
	VectorCopy( bot->client->ps.maxs, maxs );

	// NO VISIBILITY CHECK FOR NOW !
	// For Ladders, do not worry so much about reachability
	//-------------------------------------------------------

	/*if(type == NODE_LADDER)
    {
        VectorCopy(vec3_origin,maxs);
        VectorCopy(vec3_origin,mins);
    }
    else
    {   */

	// Stepsize
	mins[2] += 18;

	// Duck slightly
	maxs[2] -= 16;

	//}
	//-------------------------------------------------------
	// Square Range for distance comparison (eliminated sqrt)
	rng = (float) ( r * r );
	closest = rng + 1;	// Cuz if we found none in the range, we just found none .
	for ( i = 0; i < number_of_nodes; i++ )
	{

		// Check Node Type
		//if(type == NODEFIND_ALL || type == nodes[i].type)
		//{
		VectorSubtract( nodes[i].origin, bot->r.currentOrigin, v );

		//dist = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
		dist = VectorLength( v );

		//G_Printf("Distance: %f\n", dist);
		if
		(
			nodes[i].origin[2] - bot->r.currentOrigin[2] < 48 &&
			nodes[i].origin[2] < bot->r.currentOrigin[2] + 48 &&
			dist < closest &&
			dist < rng
		)
		{

			//G_Printf("my: %f %f %f | node: %f %f %f\n", bot->r.currentOrigin[0], bot->r.currentOrigin[1], bot->r.currentOrigin[2], nodes[i].origin[0], nodes[i].origin[1], nodes[i].origin[2]);
			// make sure it is visible
			//-------------------------------------------------------
			trap_Trace( &tr, bot->r.currentOrigin, mins, maxs, nodes[i].origin, bot->s.number, MASK_SHOT | MASK_OPAQUE );
			if ( tr.fraction == 1.0 )
			{
				node = i;
				closest = dist;
			}

			//-------------------------------------------------------
		}

		// }
	}

	return ( node );
}

extern gentity_t	*G_FindDynamiteTargetForTeam ( gentity_t *trav, team_t team );


/* */
qboolean
NearbySpawn ( gentity_t *ent )
{
	int			i;
	gentity_t	*hit;
	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		hit = &g_entities[i];
		if ( VectorDistance( hit->r.currentOrigin, ent->r.currentOrigin) > 256 )
		{
			continue;
		}

		if ( !Q_stricmp( va( "team_CTF_bluespawn"), hit->classname) )
		{
			return ( qtrue );
		}

		if ( !Q_stricmp( va( "CTF_bluespawn"), hit->classname) )
		{
			return ( qtrue );
		}

		if ( !Q_stricmp( va( "team_CTF_redspawn"), hit->classname) )
		{
			return ( qtrue );
		}

		if ( !Q_stricmp( va( "CTF_redspawn"), hit->classname) )
		{
			return ( qtrue );
		}
	}

	return ( qfalse );
}


/* */
gentity_t *
Dynamite_Entity ( gentity_t *ent )
{	// Entity for closest Dynamite.
	// check if dynamite is in trigger_objective_info field
	int			i;
	gentity_t	*hit;
	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		hit = &g_entities[i];
		if ( VectorDistance( hit->r.currentOrigin, ent->r.currentOrigin) > 256 )
		{
			continue;
		}

		if ( !Q_stricmp( va( "weapon_dynamite"), hit->classname) || !Q_stricmp( va( "dynamite"), hit->classname) )
		{
			return ( hit );
		}

		//if ( hit->methodOfDeath == MOD_DYNAMITE )
		//	return hit;	
	}

	return ( NULL );
}

/*gentity_t *Dynamite_EntityOLD ( gentity_t *ent )
{// Entity for closest Dynamite.
	// check if dynamite is in trigger_objective_info field
	vec3_t		mins, maxs; 
	int			i,num,touch[MAX_GENTITIES];
	gentity_t	*hit;

	// NERVE - SMF - made this the actual bounding box of dynamite instead of range
	VectorSet (mins, -128, -128, -128);
	VectorSet (maxs, 128, 128, 128);
	VectorAdd(mins, ent->r.currentOrigin, mins);
	VectorAdd(maxs, ent->r.currentOrigin, maxs);
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for( i = 0; i < num; i++ ) {
		hit = &g_entities[touch[i]];
		
		if ( hit->methodOfDeath == BG_GetMOD(MOD_DYNAMITE) )
			return hit;		
	}

	return NULL;
}*/
#endif
