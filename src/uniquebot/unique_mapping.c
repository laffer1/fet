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

//vec3_t	tankTraceMaxs = {64, 64, 78};
//vec3_t	tankTraceMins = {-64, -64, -24};
extern vec3_t	tankTraceMins;
extern vec3_t	tankTraceMaxs;

/*
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
MAPPING FUNCTION LISTING
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
AIMOD_MAPPING_FollowPath
AIMOD_MAPPING_GrapFired
AIMOD_MAPPING_CheckForLadder
AIMOD_MAPPING_PathMap
AIMOD_MAPPING_InitNodes
AIMOD_MAPPING_ShowNode
AIMOD_MAPPING_DrawPath
AIMOD_MAPPING_ShowPath
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
*/

/*
    _____                       .__                
   /     \ _____  ______ ______ |__| ____    ____  
  /  \ /  \\__  \ \____ \\____ \|  |/    \  / ___\ 
 /    Y    \/ __ \|  |_> >  |_> >  |   |  \/ /_/  >
 \____|__  (____  /   __/|   __/|__|___|  /\___  / 
         \/     \/|__|   |__|           \//_____/  

    uniquebot_mapping.c 
    Mapping routines for the FUSION bot.
================================================================
Should you decide to release a modified version of FUSIO*/
#include "../game/g_local.h"

//#include "uniquebot.h"
// Total number of nodes that are items
//-------------------------------------------------------
int				number_of_itemnodes;

// Total number of nodes
//-------------------------------------------------------
extern int		number_of_nodes;
int				number_conn = 0;

// For debugging paths
//-------------------------------------------------------
int				show_path_from = -1;
int				show_path_to = -1;

// For adding, editing paths etc
qboolean		touch_connections = qfalse;
qboolean		relocate_node = qfalse;

// array for node data
//-------------------------------------------------------
int				current_node = -1;			// Holds the currently selected node globally
node_t			nodes[MAX_NODES];
gentity_t		*visible_nodes[MAX_NODES];	// These are 'visible' node entities
gentity_t		*connections[500];			// That's an awful lot, but hey :)
int				enodecount = 0;
enode_t			enodes[200];
extern botai_t	botai;
void			AIMOD_MAPPING_TouchNode ( gentity_t *ent, gentity_t *other, trace_t *trace );
void			AIMOD_MAPPING_CreateVisibleNode ( int i );
//void			AIMOD_MAPPING_CalculateVisibleNodes ( void );
void                  AIMOD_MAPPING_CalculateVisibleNodes ( gentity_t *ent );
void			BotWaypointRender ( void );


// Node initialization stuff
void
AIMOD_MAPPING_InitMapNodes ( void )
{
	int i, j;
	number_of_nodes = 0;
	shownodes = qfalse;
	for ( i = 0; i < MAX_NODES; i++ )
	{
		nodes[i].enodenum = INVALID;
		for ( j = 0; j < MAX_NODELINKS; j++ )
		{
			nodes[i].links[j].cost = 999999;	// too high
			nodes[i].links[j].targetNode = INVALID;
		}
	}

	// Initialize the botai global struct
	botai.ctf = qtrue;							// AVD set this in loadnodes or something
	for ( i = 0; i < TEAM_NUM_TEAMS; i++ )
	{
		botai.flagstate[i].dropped = qfalse;
		botai.flagstate[i].valid = qtrue;
		botai.flagstate[i].carrier = INVALID;
		botai.flagstate[i].node = INVALID;
		botai.flagstate[i].entityNum = INVALID;
	}

	for ( i = 0; i < 200; i++ )
	{
		enodes[i].link_node = INVALID;
		enodes[i].num_routes = 0;
	}

	for ( i = 0; i < MAX_NODES; i++ )
	{
		visible_nodes[i] = NULL;
	}

	for ( i = 0; i < 500; i++ )
	{
		connections[i] = NULL;
	}
}

extern vec3_t	tankTraceMaxs;
extern vec3_t	tankTraceMins;

//vec3_t	playerMins = {-18, -18, -24};
//vec3_t	playerMaxs = {18, 18, 48};
//vec3_t	botTraceMins = {-32, -32, -1};
//vec3_t	botTraceMaxs = {32, 32, 32};
//vec3_t	botTraceMins = {-24, -24, -1};
//vec3_t	botTraceMaxs = {24, 24, 32};
vec3_t			botTraceMins = { -20, -20, -1 };
vec3_t			botTraceMaxs = { 20, 20, 32 };


//extern int OrgVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore);
//special node visibility check for bot linkages..
//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.

//3 = team door entity in the way.
int
NodeVisible ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg;
	vec3_t	mins, maxs;

	//VectorSet(mins, 0, 0, 0);
	//VectorSet(maxs, 16, 16, 32);
	VectorCopy( botTraceMins, mins );
	VectorCopy( botTraceMaxs, maxs );
	VectorCopy( org2, newOrg );

	//VectorCopy(org1, newOrg);
	//	newOrg[2]+=16; // Look from up a little...
	newOrg[2] += 4; // Look from up a little...

	//newOrg[2]+=32; // Look from up a little...
	//trap_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
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
	{				// A door in the way.. See if we have access...
		if ( !g_entities[tr.entityNum].allowteams )
		{
			return ( 2 );
		}

		return ( 3 );
	}

	return ( 0 );
}


/* */
int
NodeVisible2 ( vec3_t org1, vec3_t org2, int ignore )
{
	trace_t tr;
	vec3_t	newOrg;
	vec3_t	mins, maxs;

	//VectorSet(mins, 0, 0, 0);
	//VectorSet(maxs, 16, 16, 32);
	VectorCopy( botTraceMins, mins );
	VectorCopy( botTraceMaxs, maxs );
	VectorCopy( org2, newOrg );

	//VectorCopy(org1, newOrg);
	//	newOrg[2]+=16; // Look from up a little...
	//	newOrg[2]+=8; // Look from up a little...
	newOrg[2] += 4; // Look from up a little...

	//trap_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
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
	{				// A door in the way.. See if we have access...
		if ( !g_entities[tr.entityNum].allowteams )
		{
			return ( 2 );
		}

		return ( 3 );
	}

	return ( 0 );
}


//extern int OrgVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore);
//special node visibility check for bot linkages..
//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.

//3 = team door entity in the way.
int
TankNodeVisible ( vec3_t org1, vec3_t org2, vec3_t mins, vec3_t maxs, int ignore )
{
	trace_t tr;
	vec3_t	newOrg, newOrg2;
	VectorCopy( org1, newOrg2 );
	VectorCopy( org2, newOrg );

	//VectorCopy(org1, newOrg);
	//newOrg[2]+=16; // Look from up a little...
	newOrg[2] += 4;		// Look from up a little...

	//	newOrg2[2]+=16; // Look from up a little...
	newOrg2[2] += 4;	// Look from up a little...

	//trap_Trace(&tr, newOrg, mins, maxs, org2, ignore, MASK_SHOT);
	trap_Trace( &tr, newOrg2, mins, maxs, newOrg, ignore, MASK_SHOT | MASK_OPAQUE | CONTENTS_LAVA /*MASK_WATER*/ );
	if ( tr.fraction == 1 )
	{

		//trap_Trace(&tr, newOrg, NULL, NULL, org2, ignore, MASK_SHOT/*MASK_PLAYERSOLID*/);

		/*		if (tr.fraction != 1 && tr.entityNum != ENTITYNUM_NONE 
			&& (!strcmp(g_entities[tr.entityNum].classname, "func_door") || !strcmp(g_entities[tr.entityNum].classname, "func_door_rotating")) )
		{// A door in the way.. See if we have access...
			if (!g_entities[tr.entityNum].allowteams)
				return 2;

			return 3;
		}*/
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

extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );
int				upto_node = 0;
int				upto_link = 0;

//int last_upto_node = -1;
int				link_check_start_time = 0;


// UQ1: Spread this across many frames! Checks one link each frame...
void
AIMOD_MAPPING_Realtime_NodeLinks_Checking ( void )
{									// Call this each frame...
	vec3_t	tmp;
	int		visCheck;
	int		i;
	vec3_t	tankMinsSize, tankMaxsSize;
	VectorCopy( tankTraceMins, tankMinsSize );
	VectorCopy( tankTraceMaxs, tankMaxsSize );
	if ( number_of_nodes <= 0 )
	{
		return;
	}

	/*	if (last_upto_node != upto_node)
	{
		G_Printf("Realtime_NodeLinks_Checking: Up to node %i.\n", upto_node);
		last_upto_node = upto_node;
	}*/
	if ( upto_node == 0 && upto_link == 0 )
	{
		link_check_start_time = trap_Milliseconds();
	}

#ifdef __VEHICLES__
	for ( i = 0; i < 20; i++ )
#else //!__VEHICLES__
		for ( i = 0; i < 40; i++ )
#endif //__VEHICLES__
		{
#ifdef __VEHICLES__
			if ( nodes[i].type & NODE_LAND_VEHICLE )
			{
				continue;
			}
#endif //__VEHICLES__
			upto_link++;
			if ( upto_link >= nodes[upto_node].enodenum || nodes[upto_node].links[upto_link].targetNode < 0 )
			{
				upto_link = 0;
				upto_node++;
				if ( upto_node > number_of_nodes )
				{
					int cur_time = trap_Milliseconds();
					upto_node = 0;	// Back to the start next frame :)
					if( bot_debug.integer >= BOT_DEBUG_ALL )
						G_Printf( "All node links checked in %i seconds.\n", (cur_time - link_check_start_time) );
					link_check_start_time = cur_time;
				}
			}

			VectorCopy( nodes[nodes[upto_node].links[upto_link].targetNode].origin, tmp );
			if ( BAD_WP_Height( nodes[upto_node].origin, tmp) )
			{
				nodes[upto_node].links[upto_link].flags |= PATH_BLOCKED;
				nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
				return;
			}

			visCheck = NodeVisible( nodes[upto_node].origin, tmp, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 || visCheck == 2 || visCheck == 3 || upto_node == i + 1 )
			{						// Is OK!
				//			if (nodes[upto_node].links[upto_link].flags & PATH_BLOCKED)
				//			{
				nodes[upto_node].links[upto_link].flags &= ~PATH_BLOCKED;

				//			}
				//			else if (nodes[upto_node].links[upto_link].flags & PATH_NOTANKS)
				//			{
				//				nodes[upto_node].links[upto_link].flags = 0;
				//				nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
				//			}
				//			else
				//			{
				//				nodes[upto_node].links[upto_link].flags = 0;
				//			}
			}
			else
			{						// Is bad currently! Mark it!
				nodes[upto_node].links[upto_link].flags |= PATH_BLOCKED;
			}

			/*#ifdef __VEHICLES__
		visCheck = TankNodeVisible(nodes[upto_node].origin, tmp, tankMinsSize, tankMaxsSize, -1);

		if ( visCheck == 1 )
		{// Is OK!
			nodes[upto_node].links[upto_link].flags &= ~PATH_NOTANKS;
		}
		else
		{// Is bad currently! Mark it!
			nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
		}
#endif //__VEHICLES__*/
		}
}


// UQ1: Run on startup... Set initial blocking...
void
AIMOD_MAPPING_Initial_NodeLinks_Checking ( void )
{
	vec3_t	tmp;
	int		visCheck;
	vec3_t	tankMinsSize, tankMaxsSize;
	VectorCopy( tankTraceMins, tankMinsSize );
	VectorCopy( tankTraceMaxs, tankMaxsSize );
	if ( number_of_nodes <= 0 )
	{
		return;
	}

	G_Printf( "^1*** ^3%s^5: Running initial node link checks!\n", GAME_VERSION );
	while ( 1 )
	{
#ifdef __VEHICLES__
		if ( nodes[upto_node].type & NODE_LAND_VEHICLE )
		{
			upto_link = 0;
			upto_node++;
			if ( upto_node > number_of_nodes )
			{
				upto_node = 0;
				break;	// DONE!
			}
		}
#endif //__VEHICLES__
		upto_link++;
		if ( upto_link >= nodes[upto_node].enodenum || nodes[upto_node].links[upto_link].targetNode < 0 )
		{
			upto_link = 0;
			upto_node++;
			if ( upto_node > number_of_nodes )
			{
				upto_node = 0;
				break;	// DONE!
			}
		}

		VectorCopy( nodes[nodes[upto_node].links[upto_link].targetNode].origin, tmp );
		if ( BAD_WP_Height( nodes[upto_node].origin, tmp) )
		{
			nodes[upto_node].links[upto_link].flags |= PATH_BLOCKED;
			nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
			return;
		}

		visCheck = NodeVisible( nodes[upto_node].origin, tmp, -1 );

		//0 = wall in way
		//1 = player or no obstruction
		//2 = useable door in the way.
		//3 = door entity in the way.
		if
		(
			visCheck == 1 ||
			visCheck == 2 ||
			visCheck == 3 ||
			upto_node == nodes[upto_node].links[upto_link].targetNode - 1
		)
		{				// Is OK!
			//			if (nodes[upto_node].links[upto_link].flags & PATH_BLOCKED)
			//			{
			nodes[upto_node].links[upto_link].flags &= ~PATH_BLOCKED;

			//			}
			//			else if (nodes[upto_node].links[upto_link].flags & PATH_NOTANKS)
			//			{
			//				nodes[upto_node].links[upto_link].flags = 0;
			//				nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
			//			}
			//			else
			//			{
			//				nodes[upto_node].links[upto_link].flags = 0;
			//			}
		}
		else
		{				// Is bad currently! Mark it!
			nodes[upto_node].links[upto_link].flags |= PATH_BLOCKED;
		}

		/*#ifdef __VEHICLES__
		visCheck = TankNodeVisible(nodes[upto_node].origin, tmp, tankMinsSize, tankMaxsSize, -1);

		if ( visCheck == 1)
		{// Is OK!
			nodes[upto_node].links[upto_link].flags &= ~PATH_NOTANKS;
		}
		else
		{// Is bad currently! Mark it!
			nodes[upto_node].links[upto_link].flags |= PATH_NOTANKS;
		}
#endif //__VEHICLES__*/
	}

	G_Printf( "^1*** ^3%s^5: Initial node link checks completed!\n", GAME_VERSION );
}


/* */
int
AIMOD_MAPPING_CreateNodeLinks ( int node )
{
	int		loop = 0;
	int		linknum = 0;
	int		loop2 = 0;
	vec3_t	tankMinsSize, tankMaxsSize;
	VectorCopy( tankTraceMins, tankMinsSize );

	//tankMinsSize[2] = 1;
	VectorCopy( tankTraceMaxs, tankMaxsSize );
	if ( nodes[node].type & NODE_LAND_VEHICLE )
	{	// Ground Vehicle nodes...
		for ( loop = 0; loop < number_of_nodes; loop++ )
		{
			vec3_t	tmp;
			VectorCopy( nodes[node].origin, tmp );
			if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
			{
				continue;
			}

			if ( !(nodes[loop].type & NODE_LAND_VEHICLE) )
			{
				continue;
			}

			if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= 512 )
			{
				int visCheck = TankNodeVisible( nodes[loop].origin, tmp, tankMinsSize, tankMaxsSize, -1 );

				//0 = wall in way
				//1 = player or no obstruction
				//2 = useable door in the way.
				//3 = door entity in the way.
				if ( visCheck == 1 || loop == node + 1 || loop == node - 1 )
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = VectorDistance( nodes[loop].origin, nodes[node].origin );
					linknum++;
					if ( linknum > MAX_NODELINKS )
					{
						linknum--;
						break;
					}
				}
			}
		}

		nodes[node].enodenum = linknum;
		if ( linknum == 0 )
		{
			G_Printf( "^1*** ^3%s^5: Created ^7%i^5 links for ^3Land Vehicle^5 node number ^7%i^5!!!\n", GAME_VERSION,
					  linknum, node );
		}

		//		G_Printf("^4*** ^3%s^5: Created ^7%i^5 links for ^3Land Vehicle^5 node number ^7%i^5.\n", GAME_VERSION, linknum, node);
		return ( linknum );
	}

	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		vec3_t	tmp;
		VectorCopy( nodes[node].origin, tmp );

		//		tmp[2]+=16;
		//		tmp[2]+=4;
		if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
		{
			continue;
		}

		if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= 256 )
		{
			int visCheck = NodeVisible( nodes[loop].origin, tmp, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 || visCheck == 2 || visCheck == 3 || loop == node - 1 )
			{
				nodes[node].links[linknum].targetNode = loop;
				nodes[node].links[linknum].cost = VectorDistance( nodes[loop].origin, nodes[node].origin );
				if ( TankNodeVisible( nodes[loop].origin, tmp, tankMinsSize, tankMaxsSize, -1) != 1 )
				{
					nodes[node].links[linknum].flags |= PATH_NOTANKS;
				}

				linknum++;
				if ( linknum > MAX_NODELINKS )
				{
					linknum--;
					break;
				}
			}
		}
	}

	if ( linknum < 5 )
	{
		for ( loop = 0; loop < number_of_nodes; loop++ )
		{
			vec3_t	tmp;
			VectorCopy( nodes[node].origin, tmp );

			//			tmp[2]+=16;
			tmp[2] += 4;
			if ( BAD_WP_Height( nodes[node].origin, nodes[loop].origin) )
			{
				continue;
			}

			if ( VectorDistance( nodes[loop].origin, nodes[node].origin) <= 512 )
			{
				int visCheck = NodeVisible( nodes[loop].origin, tmp, -1 );

				//0 = wall in way
				//1 = player or no obstruction
				//2 = useable door in the way.
				//3 = door entity in the way.
				if ( visCheck == 1 || visCheck == 2 || visCheck == 3 || loop == node - 1 )
				{
					nodes[node].links[linknum].targetNode = loop;
					nodes[node].links[linknum].cost = VectorDistance( nodes[loop].origin, nodes[node].origin );
					if ( TankNodeVisible( nodes[loop].origin, tmp, tankMinsSize, tankMaxsSize, -1) != 1 )
					{
						nodes[node].links[linknum].flags |= PATH_NOTANKS;
					}

					linknum++;
					if ( linknum > MAX_NODELINKS )
					{
						linknum--;
						break;
					}
				}
			}
		}
	}

	nodes[node].enodenum = linknum;

	//	if (linknum == 0)
	//		G_Printf("^4*** ^3%s^5: Created ^7%i^5 links for node number ^7%i^5!!!\n", GAME_VERSION, linknum, node);
	//G_Printf("^4*** ^3%s^5: Created ^7%i^5 links for node number ^7%i^5.\n", GAME_VERSION, linknum, node);
	return ( linknum );
}


/* */
void
AIMOD_MAPPING_CreateSpecialNodeFlags ( int node )
{	// Need to check for duck (etc) nodes and mark them...
	trace_t tr;
	vec3_t	up, temp;
	VectorCopy( nodes[node].origin, temp );
	temp[2] += 1;
	nodes[node].type &= ~NODE_DUCK;
	VectorCopy( nodes[node].origin, up );
	up[2] += 16550;
	trap_Trace( &tr, nodes[node].origin, NULL, NULL, up, -1, MASK_SHOT | MASK_OPAQUE | MASK_WATER /*MASK_ALL*/ );	//MASK_SHOT|MASK_OPAQUE);
	if ( VectorDistance( nodes[node].origin, tr.endpos) <= 72 )
	{	// Could not see the up pos.. Need to duck to go here!
		nodes[node].type |= NODE_DUCK;
		G_Printf( "^4*** ^3%s^5: Node ^7%i^5 marked as a duck node.\n", GAME_VERSION, node );
	}

	if ( AI_PM_SlickTrace( nodes[node].origin, -1) )
	{	// This node is on slippery ice... Mark it...
		nodes[node].type |= NODE_ICE;
		G_Printf( "^4*** ^3%s^5: Node ^7%i^5 marked as an ice (slick) node.\n", GAME_VERSION, node );
	}
}

// AVD Adds a node on the position of clientNum
// -------------------------------------------
extern int	aw_num_nodes;


/* */
void
AIMOD_MAPPING_MakeLinks ( void )
{
	int		loop = 0;
	node_t	good_nodes[MAX_NODES];
	int		upto = 0;
	int		total_good_count = 0;
	int		bad_nodes = 0;
	if ( aw_num_nodes > 0 )
	{
		number_of_nodes = aw_num_nodes;
	}

	G_Printf( "^4*** ^3%s^5: Checking node linkages...\n", GAME_VERSION );
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{		// Do links...
		int link_count;
		link_count = AIMOD_MAPPING_CreateNodeLinks( loop );
		if ( link_count > 0 )
		{	// Make a good node list...
			/*			good_nodes[upto].enodenum = nodes[loop].enodenum;
			good_nodes[upto].objectNum[0] = nodes[loop].objectNum[0];
			good_nodes[upto].objectNum[1] = nodes[loop].objectNum[1];
			good_nodes[upto].objectNum[2] = nodes[loop].objectNum[2];
			good_nodes[upto].objEntity = nodes[loop].objEntity;
			good_nodes[upto].objFlags = nodes[loop].objFlags;
			good_nodes[upto].objTeam = nodes[loop].objTeam;
			good_nodes[upto].objType = nodes[loop].objType;*/
			VectorCopy( nodes[loop].origin, good_nodes[upto].origin );

			//good_nodes[upto].type = nodes[loop].type;
			upto++;
		}
		else
		{
			bad_nodes++;
		}
	}

	if ( bad_nodes > 0 )
	{
		G_Printf( "^4*** ^3%s^5: Removing ^7%i^5 bad nodes...\n", GAME_VERSION, bad_nodes );
		total_good_count = upto;
		number_of_nodes = 0;
		upto = 0;
		for ( loop = 0; loop < total_good_count; loop++ )
		{

			/*			nodes[loop].enodenum = good_nodes[loop].enodenum;
			nodes[loop].objectNum[0] = good_nodes[loop].objectNum[0];
			nodes[loop].objectNum[1] = good_nodes[loop].objectNum[1];
			nodes[loop].objectNum[2] = good_nodes[loop].objectNum[2];
			nodes[loop].objEntity = good_nodes[loop].objEntity;
			nodes[loop].objFlags = good_nodes[loop].objFlags;
			nodes[loop].objTeam = good_nodes[loop].objTeam;
			nodes[loop].objType = good_nodes[loop].objType;*/
			VectorCopy( good_nodes[loop].origin, nodes[loop].origin );

			//nodes[loop].type = good_nodes[loop].type;
			upto++;
		}

		number_of_nodes = total_good_count - 1;
		G_Printf( "^4*** ^3%s^5: ^7%i^5 bad nodes removed...\n", GAME_VERSION, bad_nodes );
		G_Printf( "^4*** ^3%s^5: Finalizing node linkages...\n", GAME_VERSION, bad_nodes );
	}
	else
	{
		G_Printf( "^4*** ^3%s^5: Node linkages were found to be OK!\n", GAME_VERSION );
		G_Printf( "^4*** ^3%s^5: Finalizing node linkages...\n", GAME_VERSION, bad_nodes );
	}

	for ( loop = 0; loop < number_of_nodes; loop++ )
	{		// Do links...
		int link_count = AIMOD_MAPPING_CreateNodeLinks( loop );

		// Also check if the node needs special flags...
		AIMOD_MAPPING_CreateSpecialNodeFlags( loop );
	}
}


/* */
void
AIMOD_MAPPING_AddNode ( int clientNum, int teamNum )
{
	int		i;
	vec3_t	v;
	if ( teamNum == 99 )
	{																// I will use 99 as a vehicle only node...
		nodes[number_of_nodes].type |= NODE_LAND_VEHICLE;
	}

	if ( teamNum == TEAM_ALLIES )
	{
		nodes[number_of_nodes].objTeam |= TEAM_ALLIES;
		nodes[number_of_nodes].type |= NODE_AXIS_UNREACHABLE;
	}
	else if ( teamNum == TEAM_AXIS )
	{
		nodes[number_of_nodes].type |= NODE_ALLY_UNREACHABLE;
		nodes[number_of_nodes].objTeam |= TEAM_AXIS;
	}

	if ( relocate_node )
	{
		if ( current_node == INVALID || current_node > MAX_NODES )
		{															// Safety comes first :-p
			return;
		}

		// Put node at new origin, create new path-costs
		VectorCopy( g_entities[clientNum].r.currentOrigin, nodes[current_node].origin );
		for ( i = 0; i < MAX_NODELINKS; i++ )
		{
			if ( nodes[current_node].links[i].targetNode != INVALID )
			{
				VectorSubtract( nodes[current_node].origin, nodes[nodes[current_node].links[i].targetNode].origin, v );
				nodes[current_node].links[i].cost = VectorLength( v );
			}
		}

		// Recreate visible entity if necessary
		if ( visible_nodes[current_node] != NULL )
		{
			G_FreeEntity( visible_nodes[current_node] );
			visible_nodes[current_node] = NULL;
		}

		// Do links...
		//AIMOD_MAPPING_CreateNodeLinks( current_node ); // Do this on save...
		AIMOD_MAPPING_CalculateVisibleNodes( &g_entities[clientNum] );
	}
	else if ( number_of_nodes < MAX_NODES )
	{																// Setup the node's origin for now...
		//		VectorCopy(g_entities[clientNum].r.currentOrigin, g_entities[clientNum].last_added_waypoint);
		VectorCopy( g_entities[clientNum].r.currentOrigin, nodes[number_of_nodes].origin );
		if ( !nodes[number_of_nodes].type )
		{
			nodes[number_of_nodes].type = NODE_MOVE;				// Default node type
		}

		// Create a connection between this new node and the previous one
		for ( i = 0; i < MAX_NODELINKS; i++ )
		{
			nodes[number_of_nodes].links[i].targetNode = INVALID;	// Set all links of the NEW node to invalid
		}

		// Do links...
		//AIMOD_MAPPING_CreateNodeLinks( current_node ); // Do this on save isntead...
		G_Printf( "^4*** ^3%s^5: Node %i created.\n", GAME_VERSION, number_of_nodes + 1 );
		current_node = number_of_nodes;
		number_of_nodes++;											// We just added a node
		AIMOD_MAPPING_CalculateVisibleNodes( &g_entities[clientNum] );
	}
	else
	{																// No longer able to add nodes
		G_Printf( "Maximum number of nodes reached.\n" );
	}
}


// -------------------------------------------
// -------------------------------------------
// AVD Creates entities for all the nodes

// -------------------------------------------
void
AIMOD_MAPPING_ShowNodes ( gentity_t *ent )
{
	if ( shownodes )
	{
		return;
	}

	G_Printf( "Node entities created.\n" );
	shownodes = qtrue;

	//AIMOD_MAPPING_CalculateVisibleNodes(ent);
	BotWaypointRender();
}


// -------------------------------------------
void
AIMOD_MAPPING_ShowConnection ( int from, int to )
{
	if ( number_conn < 500 && from != INVALID && to != INVALID && from != to )
	{
		connections[number_conn] = G_Spawn();
		connections[number_conn]->s.eType = ET_EVENTS + EV_RAILTRAIL;
		connections[number_conn]->classname = "node_connection";
		connections[number_conn]->eventTime = level.time;
		connections[number_conn]->r.eventTime = level.time;
		connections[number_conn]->freeAfterEvent = qfalse;
		G_SetOrigin( connections[number_conn], nodes[from].origin );
		trap_LinkEntity( connections[number_conn] );
		VectorCopy( nodes[to].origin, connections[number_conn]->s.origin2 );
		connections[number_conn]->s.otherEntityNum2 = 0;
		connections[number_conn]->s.dmgFlags = 0;
		number_conn++;
	}
}


/* */
void
AIMOD_MAPPING_SetConnectionCost ( int from, int to, int new_cost )
{
	int i;
	for ( i = 0; i < MAX_NODELINKS; i++ )
	{
		if ( nodes[from].links[i].targetNode == to )
		{
			if ( new_cost == 32000 )
			{
				nodes[from].links[i].targetNode = INVALID;
				nodes[from].links[i].cost = 0;
				G_Printf( "Removed link.\n" );
			}
			else
			{

				//G_Printf("Updated cost from %i to %i : %i  (was %f).\n", from+1, to+1, new_cost, nodes[from].links[i].cost);
				nodes[from].links[i].cost = (float) new_cost;
			}

			return;
		}
	}
}


// AVD Sets the type of a node
void
AIMOD_MAPPING_SetNodeType ( int node, int node_type )
{

	/*
	if (node == INVALID)
		node = current_node;

    nodes[node].type = node_type;

	G_Printf("Type of node %i set to %i.\n", node+1, node_type);
	*/
}


/* */
void
G_TestLine ( vec3_t start, vec3_t end, int color, int time )
{
	gentity_t	*te;
	te = G_TempEntity( start, EV_DEBUG_LINE /*EV_TESTLINE*/ );
	VectorCopy( start, te->s.origin );
	VectorCopy( start, te->s.pos.trBase );
	VectorCopy( end, te->s.origin2 );
	te->s.time2 = time;
	te->s.weapon = color;
	te->r.svFlags |= SVF_BROADCAST;
}


/* */
void
G_MarkGoalNode ( int node )
{
	gentity_t	*plum;
	plum = G_TempEntity( nodes[node].origin, EV_GOALNODE );
	plum->r.svFlags |= SVF_BROADCAST;
	plum->s.time = node;
}

int				gWPRenderTime = 0;
int				gWPRenderedFrame = 0;
int				gWPNum = 0;
int				gLastPrintedIndex = 0;
extern int		num_links[MAX_NODES];					// AIMOD_navigation.c
extern void		AIMOD_NAVIGATION_Set_NumLinks ( void ); // AIMOD_navigation.c
extern vmCvar_t bot_node_edit;
extern vmCvar_t bot_node_edit_tankonly;
qboolean		linksdone = qfalse;


/* */
void
BotWaypointRender ( void )
{
	int			i, n;
	int			inc_checker;
	int			bestindex;
	int			gotbestindex;
	float		bestdist;
	float		checkdist;
	gentity_t	*plum;
	gentity_t	*viewent;
	char		*flagstr;
	vec3_t		a;
	vec3_t		eyes;
	VectorCopy( g_entities[0].r.currentOrigin, eyes );
	eyes[2] += 32;
	if ( bot_node_edit.integer )
	{
		shownodes = qtrue;
	}

	if ( !shownodes )
	{
		return;
	}

	gWPNum = number_of_nodes;

	/*	if (!linksdone)
	{
		AIMOD_NAVIGATION_Set_NumLinks();
		linksdone = qtrue;
	}*/
	bestindex = 0;
	if ( gWPRenderTime > level.time )
	{
		goto checkprint;
	}

	gWPRenderTime = level.time + 300;		// 100
	gWPRenderedFrame = 0;
	i = 0;									//gWPRenderedFrame;
	inc_checker = 0;						//gWPRenderedFrame;
	while ( i < gWPNum )
	{
		if ( nodes[i].origin )
		{
			if ( bot_node_edit_tankonly.integer && !(nodes[i].type & NODE_LAND_VEHICLE) )
			{
				i++;
				continue;
			}

			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, nodes[i].origin) < 128 &&
				OrgVisible( nodes[i].origin, eyes, -1)
			)
			{
				vec3_t	up;
				VectorCopy( nodes[i].origin, up );
				up[2] += i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				if ( nodes[i].type == NODE_GOAL )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( nodes[i].type & NODE_JUMP || nodes[i].type & NODE_FASTHOP )
				{
					plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
				}
				else if ( nodes[i].type & NODE_DUCK )
				{
					plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
				}
				else if ( nodes[i].type & NODE_LAND_VEHICLE )
				{
					plum->s.weapon = 4;		// Mark this as a vehicle node for the tank icon.
				}
				else
				{
					plum->s.weapon = 0;
				}

				n = 0;
				gWPRenderedFrame++;

				/*{
					int z;

					for (z = 0; z < nodes[i].enodenum; z++)
					{
						DebugLine(nodes[i].origin, nodes[nodes[i].links[z].targetNode].origin, 1);
					}
				}*/
			}
		}

		/*		else
		{
			gWPRenderedFrame = 0;
			break;
		}*/

		//if ((i - inc_checker) > 4)
		if ( gWPRenderedFrame > 64 /*128*/ )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	i = 0;									//gWPRenderedFrame;
	inc_checker = 0;						//gWPRenderedFrame;
	while ( i < gWPNum )
	{
		if ( gWPRenderedFrame > 64 /*128*/ )
		{
			break;
		}

		if ( nodes[i].origin )
		{
			if ( bot_node_edit_tankonly.integer && !(nodes[i].type & NODE_LAND_VEHICLE) )
			{
				i++;
				continue;
			}

			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, nodes[i].origin) < 512 &&
				VectorDistance( g_entities[0].r.currentOrigin, nodes[i].origin) >= 128 &&
				OrgVisible( nodes[i].origin, eyes, -1)
			)
			{
				vec3_t	up;
				VectorCopy( nodes[i].origin, up );
				up[2] += i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				if ( nodes[i].type == NODE_GOAL )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( nodes[i].type & NODE_JUMP || nodes[i].type & NODE_FASTHOP )
				{
					plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
				}
				else if ( nodes[i].type & NODE_DUCK )
				{
					plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
				}
				else if ( nodes[i].type & NODE_LAND_VEHICLE )
				{
					plum->s.weapon = 4;		// Mark this as a vehicle node for the tank icon.
				}
				else
				{
					plum->s.weapon = 0;
				}

				n = 0;
				gWPRenderedFrame++;

				/*{
					int z;

					for (z = 0; z < nodes[i].enodenum; z++)
					{
						DebugLine(nodes[i].origin, nodes[nodes[i].links[z].targetNode].origin, 1);
					}
				}*/
			}
		}

		/*		else
		{
			gWPRenderedFrame = 0;
			break;
		}*/

		//if ((i - inc_checker) > 4)
		if ( gWPRenderedFrame > 64 /*128*/ )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	i = 0;									//gWPRenderedFrame;
	inc_checker = 0;						//gWPRenderedFrame;
	while ( i < gWPNum )
	{
		if ( gWPRenderedFrame > 64 /*128*/ )
		{
			break;
		}

		if ( nodes[i].origin )
		{
			if ( bot_node_edit_tankonly.integer && !(nodes[i].type & NODE_LAND_VEHICLE) )
			{
				i++;
				continue;
			}

			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, nodes[i].origin) < 1024 &&
				VectorDistance( g_entities[0].r.currentOrigin, nodes[i].origin) >= 512 &&
				OrgVisible( nodes[i].origin, eyes, -1)
			)
			{
				vec3_t	up;
				VectorCopy( nodes[i].origin, up );
				up[2] += i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				if ( nodes[i].type == NODE_GOAL )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( nodes[i].type & NODE_JUMP || nodes[i].type & NODE_FASTHOP )
				{
					plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
				}
				else if ( nodes[i].type & NODE_DUCK )
				{
					plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
				}
				else if ( nodes[i].type & NODE_LAND_VEHICLE )
				{
					plum->s.weapon = 4;		// Mark this as a vehicle node for the tank icon.
				}
				else
				{
					plum->s.weapon = 0;
				}

				n = 0;
				gWPRenderedFrame++;

				/*{
					int z;

					for (z = 0; z < nodes[i].enodenum; z++)
					{
						DebugLine(nodes[i].origin, nodes[nodes[i].links[z].targetNode].origin, 1);
					}
				}*/
			}
		}

		/*		else
		{
			gWPRenderedFrame = 0;
			break;
		}*/

		//if ((i - inc_checker) > 4)
		if ( gWPRenderedFrame > 64 /*128*/ )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	if ( i >= gWPNum )
	{
		gWPRenderTime = level.time + 300;	//1500; //wait a bit after we finish doing the whole trail
		gWPRenderedFrame = 0;
	}

checkprint:
	if ( !bot_node_edit.integer )
	{
		return;
	}

	viewent = &g_entities[0];				//only show info to the first client
	if ( !viewent || !viewent->client )
	{										//client isn't in the game yet?
		return;
	}

	bestdist = 256;							//max distance for showing point info
	gotbestindex = 0;
	i = 0;
	while ( i < gWPNum )
	{
		if ( nodes[i].origin /*&& nodes[i]->inuse*/ )
		{
			VectorSubtract( viewent->client->ps.origin, nodes[i].origin, a );
			checkdist = VectorLength( a );
			if ( checkdist < bestdist )
			{
				bestdist = checkdist;
				bestindex = i;
				gotbestindex = 1;
			}
		}

		i++;
	}

	if ( gotbestindex && bestindex != gLastPrintedIndex )
	{
		if ( nodes[bestindex].type == NODE_JUMP )
		{
			flagstr = "Jump";
		}
		else if ( nodes[bestindex].type == NODE_FASTHOP )
		{
			flagstr = "Fast-Hop";
		}
		else if ( nodes[bestindex].type == NODE_WATER )
		{
			flagstr = "Water";
		}
		else if ( nodes[bestindex].type == NODE_LADDER )
		{
			flagstr = "Ladder";
		}
		else if ( nodes[bestindex].type == NODE_OBJECTIVE )
		{
			flagstr = "Objective";
		}
		else if ( nodes[bestindex].type == NODE_BUILD )
		{
			flagstr = "Build";
		}
		else if ( nodes[bestindex].type == NODE_DYNAMITE )
		{
			flagstr = "Dynamite";
		}
		else if ( nodes[bestindex].type == NODE_MG42 )
		{
			flagstr = "MG-42";
		}
		else if ( nodes[bestindex].type == NODE_GOAL )
		{
			flagstr = "Goal Node";
		}
		else if ( nodes[bestindex].type & NODE_LAND_VEHICLE )
		{
			flagstr = "Land Vehicle Node";
		}
		else
		{
			flagstr = "None";
		}

		gLastPrintedIndex = bestindex;
		G_Printf( "^5Node ^7%i\n^5Flags - ^3%i^5 (^7%s^5)\n^5Origin - (^3%i %i %i^5)\n", (int) (bestindex),
				  (int) nodes[bestindex].type, flagstr, (int) (nodes[bestindex].origin[0]),
				  (int) (nodes[bestindex].origin[1]), (int) (nodes[bestindex].origin[2]) );
		plum = G_TempEntity( nodes[bestindex].origin, EV_SCOREPLUM );
		plum->r.svFlags |= SVF_BROADCAST;
		plum->s.time = bestindex;			//render it once
	}
	else if ( !gotbestindex )
	{
		gLastPrintedIndex = -1;
	}
}


/* */
void
BotRouteWaypointRender ( gentity_t *bot )
{											// Render this bot's route...
	int			i, n;
	int			inc_checker;
	int			bestindex;
	gentity_t	*plum;
	vec3_t		eyes;
	int			clientNum = bot->s.number;
	VectorCopy( g_entities[0].r.currentOrigin, eyes );
	eyes[2] += 32;
	bestindex = 0;
	if ( gWPRenderTime > level.time )
	{
		return;
	}

	gWPRenderTime = level.time + 300;
	gWPRenderedFrame = 0;
	i = 0;
	inc_checker = 0;
	while ( i < bot->pathsize )
	{
		if ( nodes[bot->pathlist[i]].origin )
		{
			if (							/*VectorDistance(g_entities[0].r.currentOrigin, nodes[bot->pathlist[i]].origin) < 128
				&&*/
					OrgVisible( nodes[bot->pathlist[i]].origin, eyes, -1)
				)
			{
				vec3_t	up;
				VectorCopy( nodes[bot->pathlist[i]].origin, up );
				up[2] += bot->pathlist[i] / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = bot->pathlist[i];
				if ( nodes[bot->pathlist[i]].type == NODE_GOAL )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( nodes[bot->pathlist[i]].type == NODE_JUMP || nodes[bot->pathlist[i]].type == NODE_FASTHOP )
				{
					plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
				}
				else
				{
					plum->s.weapon = 0;
				}

				n = 0;
				gWPRenderedFrame++;

				/*{
					int z;

					for (z = 0; z < nodes[i].enodenum; z++)
					{
						DebugLine(nodes[i].origin, nodes[nodes[i].links[z].targetNode].origin, 1);
					}
				}*/
			}
		}

		if ( gWPRenderedFrame > 256 /*64*/ )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	if ( i >= bot->pathsize )
	{
		gWPRenderTime = level.time + 300;	//1500; //wait a bit after we finish doing the whole trail
		gWPRenderedFrame = 0;
	}
}


/* */
void
BotWaypointRenderORIG ( void )
{
	int			i, n;
	int			inc_checker;
	int			bestindex;
	int			gotbestindex;
	float		bestdist;
	float		checkdist;
	gentity_t	*plum;
	gentity_t	*viewent;
	char		*flagstr;
	vec3_t		a;

	//	vec3_t eyes;
	//VectorCopy(g_entities[0].r.currentOrigin, eyes);
	//eyes[2]+=32;
	if ( bot_node_edit.integer )
	{
		shownodes = qtrue;
	}

	if ( !shownodes )
	{
		return;
	}

	gWPNum = number_of_nodes;

	/*	if (!linksdone)
	{
		AIMOD_NAVIGATION_Set_NumLinks();
		linksdone = qtrue;
	}*/
	bestindex = 0;
	if ( gWPRenderTime > level.time )
	{
		goto checkprint;
	}

	gWPRenderTime = level.time + 100;
	i = 0;
	inc_checker = 0;
	while ( i < gWPNum )
	{
		if ( nodes[i].origin /*&& nodes[i]->inuse*/ /*OrgVisible(nodes[i].origin, eyes, -1)*/ )
		{

			//if (VectorDistance(g_entities[0].r.currentOrigin, nodes[i].origin) < 1500)
			//{
			plum = G_TempEntity( nodes[i].origin, EV_SCOREPLUM );
			plum->r.svFlags |= SVF_BROADCAST;
			plum->s.time = i;
			if ( nodes[i].type == NODE_GOAL )
			{
				plum->s.weapon = 1;			// Mark this as a goal node for the yellow highlight.
			}
			else if ( nodes[i].type == NODE_JUMP || nodes[i].type == NODE_FASTHOP )
			{
				plum->s.weapon = 2;			// Mark this as a jump node for the red highlight.
			}
			else
			{
				plum->s.weapon = 0;
			}

			n = 0;
			gWPRenderedFrame++;

			//}
		}
		else
		{
			gWPRenderedFrame = 0;
			break;
		}

		if ( (i - inc_checker) > 4 )
		{
			break;							//don't render too many at once
		}

		i++;
	}

	if ( i >= gWPNum )
	{
		gWPRenderTime = level.time + 1500;	//wait a bit after we finish doing the whole trail
		gWPRenderedFrame = 0;
	}

checkprint:
	if ( !bot_node_edit.integer )
	{
		return;
	}

	viewent = &g_entities[0];				//only show info to the first client
	if ( !viewent || !viewent->client )
	{										//client isn't in the game yet?
		return;
	}

	bestdist = 256;							//max distance for showing point info
	gotbestindex = 0;
	i = 0;
	while ( i < gWPNum )
	{
		if ( nodes[i].origin /*&& nodes[i]->inuse*/ )
		{
			VectorSubtract( viewent->client->ps.origin, nodes[i].origin, a );
			checkdist = VectorLength( a );
			if ( checkdist < bestdist )
			{
				bestdist = checkdist;
				bestindex = i;
				gotbestindex = 1;
			}
		}

		i++;
	}

	if ( gotbestindex && bestindex != gLastPrintedIndex )
	{
		if ( nodes[bestindex].type == NODE_JUMP )
		{
			flagstr = "Jump";
		}
		else if ( nodes[bestindex].type == NODE_FASTHOP )
		{
			flagstr = "Fast-Hop";
		}
		else if ( nodes[bestindex].type == NODE_WATER )
		{
			flagstr = "Water";
		}
		else if ( nodes[bestindex].type == NODE_LADDER )
		{
			flagstr = "Ladder";
		}
		else if ( nodes[bestindex].type == NODE_OBJECTIVE )
		{
			flagstr = "Objective";
		}
		else if ( nodes[bestindex].type == NODE_BUILD )
		{
			flagstr = "Build";
		}
		else if ( nodes[bestindex].type == NODE_DYNAMITE )
		{
			flagstr = "Dynamite";
		}
		else if ( nodes[bestindex].type == NODE_MG42 )
		{
			flagstr = "MG-42";
		}
		else if ( nodes[bestindex].type == NODE_GOAL )
		{
			flagstr = "Goal Node";
		}
		else
		{
			flagstr = "None";
		}

		gLastPrintedIndex = bestindex;
		G_Printf( "^5Node ^7%i\n^5Flags - ^3%i^5 (^7%s^5)\n^5Origin - (^3%i %i %i^5)\n", (int) (bestindex),
				  (int) nodes[bestindex].type, flagstr, (int) (nodes[bestindex].origin[0]),
				  (int) (nodes[bestindex].origin[1]), (int) (nodes[bestindex].origin[2]) );
		plum = G_TempEntity( nodes[bestindex].origin, EV_SCOREPLUM );
		plum->r.svFlags |= SVF_BROADCAST;
		plum->s.time = bestindex;			//render it once
	}
	else if ( !gotbestindex )
	{
		gLastPrintedIndex = -1;
	}
}

char	*nodeclassname;


// AVD this... calculates the visible nodes... surprising huh... sucks tho', cuz it doesn't work well
void
AIMOD_MAPPING_CalculateVisibleNodes ( gentity_t *ent )
{
	BotWaypointRender();
}


/* */
void
AIMOD_MAPPING_CalculateVisibleNodesOLD ( gentity_t *ent )
{
	int		i;	//, /*j,*/ z;

	//	int min, max;//, diff, diffmax;
	//	int shown = 0;
	//	int vnodes[MAX_VISIBLENODES];
	float	distance = 0.0, total_distance = 0.0;
	int		num_visible = 0;

	//	vec3_t v;
	if ( current_node == INVALID )
	{
		current_node = 0;
	}

	if ( !shownodes /*|| current_node < 0 || current_node > MAX_NODES*/ )
	{
		return;
	}

	// Destroy ALL connections, bit cpu intensive, but hey

	/*	for (i = 0; i < number_conn; i++)
		if (connections[i] != NULL) {
			G_FreeEntity(connections[i]);
			connections[i] = NULL;
		}*/
	number_conn = 0;

	// Calculate all nodes in a certain range
	//	min = ( (current_node - 50 >= 0) ? (current_node - 50) : 0 );		// We're showing QUITE SOME nodes here :(
	//	max = ( (current_node + 50 < number_of_nodes) ? (current_node + 50) : number_of_nodes );
	// Run through all visible entities, destroy or create if necessary

	/*for (i = 0; i < number_of_nodes; i++)
	{
		if (i >= min && i <= max)
		{
			if (visible_nodes[i] == NULL)
				AIMOD_MAPPING_CreateVisibleNode(i);
			// Always recreate the connections for the node !
			for (z = 0; z < MAX_NODELINKS; z++)
				if (nodes[i].links[z].targetNode != INVALID)
					AIMOD_MAPPING_ShowConnection( i, nodes[i].links[z].targetNode );
		}
		else {
			if (visible_nodes[i] != NULL) {
				G_FreeEntity( visible_nodes[i] );
				visible_nodes[i] = NULL;
			}
		}
	}*/

	/*	if (current_node != INVALID)
		for (z = 0; z < MAX_NODELINKS; z++)
			if (nodes[current_node].links[z].targetNode != INVALID)
				AIMOD_MAPPING_ShowConnection( current_node, nodes[current_node].links[z].targetNode );
*/
	if ( nodeclassname )
	{
		for ( i = MAX_CLIENTS; i < MAX_GENTITIES; i++ )
		{
			if ( !Q_stricmp( g_entities[i].classname, "node") )
			{
				G_FreeEntity( &g_entities[i] );
			}
		}
	}

	// Show all nodes within a certain distance of the current_node
	for ( i = 0; i < number_of_nodes; i++ )
	{
		vec3_t	eyes;
		if ( num_visible > 500 )
		{
			break;
		}

		VectorCopy( ent->r.currentOrigin, eyes );
		eyes[2] += 16;
		distance = VectorDistance( ent->r.currentOrigin, nodes[i].origin );
		total_distance += distance;
		if ( distance < 1000.0 && OrgVisible( eyes, nodes[i].origin, -1) /*|| i == current_node*/ )
		{
			if ( visible_nodes[i] == NULL )
			{
				AIMOD_MAPPING_CreateVisibleNode( i );

				//nodeclassname = g_entities[i].classname;
			}

			num_visible++;
		}
		else if ( visible_nodes[i] != NULL )
		{
			G_FreeEntity( visible_nodes[i] );
			visible_nodes[i] = NULL;
		}
	}

	//G_Printf("Showing %i nodes, average distance was %f.\n", num_visible, (total_distance /(float) number_of_nodes) );
}


// AVD Destroy all node entities

// -------------------------------------------
void
AIMOD_MAPPING_HideNodes ( void )
{
	int i;
	if ( !shownodes )
	{
		return;
	}

	// Destroy all connections between the nodes
	for ( i = 0; i < number_conn; i++ )
	{
		if ( connections[i] != NULL )
		{
			G_FreeEntity( connections[i] );
		}
	}

	number_conn = 0;

	// Destroy all nodes
	for ( i = 0; i < number_of_nodes; i++ )
	{
		if ( visible_nodes[i] != NULL )
		{
			G_FreeEntity( visible_nodes[i] );
		}
	}

	shownodes = qfalse;
}


// -------------------------------------------
void
AIMOD_MAPPING_SetTouchConnection ( void )
{
	if ( touch_connections )
	{
		touch_connections = qfalse;
		G_Printf( "Create connection on node touch: ^1OFF.\n" );
	}
	else
	{
		touch_connections = qtrue;
		G_Printf( "Create connection on node touch: ^1ON.\n" );
	}
}


/* */
void
AIMOD_MAPPING_SetRelocateNode ( void )
{
	if ( relocate_node )
	{
		relocate_node = qfalse;
		G_Printf( "Relocate node: ^1OFF.\n" );
	}
	else
	{
		relocate_node = qtrue;
		G_Printf( "Relocate node: ^1ON.\n" );
	}
}


// AVD Touch and select nodes

// -------------------------------------------
void
AIMOD_MAPPING_TouchNode ( gentity_t *ent, gentity_t *other, trace_t *trace )
{
	int node_num = ent->current_node;
	if ( node_num != current_node )
	{	// Select this node
		// Create a connection on 'touch' of a node, reset after creating it
		if ( touch_connections )
		{
			AIMOD_MAPPING_SetTouchConnection();
		}

		current_node = node_num;
		AIMOD_MAPPING_CalculateVisibleNodes( ent );
		G_Printf( "Node %i selected.\n", current_node + 1 );
	}
}


// -------------------------------------------
// AVD Creates a new visible entity for the current node

// -------------------------------------------
void
AIMOD_MAPPING_CreateVisibleNode ( int i )
{

	// Add the node itself
	visible_nodes[i] = G_Spawn();
	visible_nodes[i]->active = qtrue;
	visible_nodes[i]->classname = "node";
	visible_nodes[i]->r.svFlags = /*SVF_USE_CURRENT_ORIGIN |*/ SVF_BROADCAST;
	visible_nodes[i]->s.modelindex = G_ModelIndex( "models/weapons2/grenade/blablabla.md3" );
	visible_nodes[i]->current_node = i; // This is the node number
	visible_nodes[i]->touch = AIMOD_MAPPING_TouchNode;
	visible_nodes[i]->clipmask = CONTENTS_SOLID | CONTENTS_MISSILECLIP;
	visible_nodes[i]->r.contents = CONTENTS_TRIGGER | CONTENTS_ITEM;

	// Body, so we can touch this node
	VectorSet( visible_nodes[i]->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );				//----(SA)	so items sit on the ground
	VectorCopy( visible_nodes[i]->r.mins, visible_nodes[i]->r.absmin );
	VectorSet( visible_nodes[i]->r.maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );	//----(SA)	so items sit on the ground
	VectorCopy( visible_nodes[i]->r.maxs, visible_nodes[i]->r.absmax );
	VectorCopy( nodes[i].origin, visible_nodes[i]->s.origin );
	G_SetOrigin( visible_nodes[i], visible_nodes[i]->s.origin );
	G_SetAngle( visible_nodes[i], visible_nodes[i]->s.angles );
	trap_LinkEntity( visible_nodes[i] );
}


// -------------------------------------------
// AVD Prints the list of nodes to a file

// -------------------------------------------
void
AIMOD_MAPPING_WriteNodes ( void )
{
	int				i, j;
	char			*buffptr;
	fileHandle_t	f;

	// Create the mapnode file
	trap_FS_FOpenFile( "nodelist.txt", &f, FS_WRITE );
	if ( !f )
	{
		G_Printf( "Failed to open '%s' for writing.\n", "nodelist.txt" );
		return;
	}

	// Run through all nodes and write them out
	for ( i = 0; i < number_of_nodes; i++ )
	{
		buffptr = va( "%i %i %i %i ", i, (int) nodes[i].origin[0], (int) nodes[i].origin[1], (int) nodes[i].origin[2] );

		// Write the node away
		for ( j = 0; j < MAX_NODELINKS; j++ )
		{
			if ( nodes[i].links[j].targetNode != INVALID )
			{
				strcat( buffptr, va( "%i %f ", nodes[i].links[j].targetNode, nodes[i].links[j].cost) );
			}
		}

		strcat( buffptr, "-1\n" );
		trap_FS_Write( buffptr, strlen( buffptr), f );
	}

	// Close the node file
	trap_FS_FCloseFile( f );
	G_Printf( "Node file created.\n" );
}

// -------------------------------------------
#endif
