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


#include "../game/g_local.h"
#include "../botai/ai_main.h"
#ifdef __NPC__
qboolean	npc_nodes_loaded = qfalse;
#define npc_MOD_NAME	"aimod"
#define NOD_VERSION		1.0f

// Total number of nodes
//-------------------------------------------------------
int				number_of_npc_nodes;

// array for node data
//-------------------------------------------------------
node_t			npc_nodes[MAX_NODES];

//special node visibility check for npc linkages..
//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.
//3 = team door entity in the way.
extern int		NodeVisible ( vec3_t org1, vec3_t org2, int ignore );

// Is the end point on a bad slope...
extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );

//===========================================================================
// Description  : NPC Node flags + Link Flags... In g_local.h

/*
#define NPC_NODE_INVALID				-1
#define NPC_NODE_MOVE					0       // Move Node
#define NPC_NODE_OBJECTIVE				1		// Objective position... Do something... (Unused in 0.1 for npcs)
#define NPC_NODE_ROUTE_BEGIN			2		// The beginning of an NPC route...
#define NPC_NODE_ROUTE_END				4		// The end of an NPC route...
#define NPC_NODE_JUMP					8		// Need a jump here...
#define NPC_NODE_DUCK					16		// Need to duck here...
#define NPC_NODE_WATER					32		// Node is in/on water...
#define NPC_NODE_ICE					64      // Node is on ice...
#define	NPC_NODE_LADDER					128		// Ladder Node
#define	NPC_NODE_MG42					256		// Node is at an mg42
#define	NPC_NODE_BUILD					512		// Need to build something here... (Unused in 0.1 for npcs)
#define	NPC_NODE_ALLY_UNREACHABLE		1024	// Axis only...
#define	NPC_NODE_AXIS_UNREACHABLE		2048	// Allied only...
*/


//===========================================================================
// Routine      : NPC_MAPPING_CreateNodeLinks

// Description  : Check if the next node is reachable for this node...
void
NPC_MAPPING_CreateNodeLinks ( int node )
{			// Fodder NPCs do not use links, they just follow a set path until they hit the end of it.. (Save CPU time!)
	vec3_t	tmp;
	VectorCopy( npc_nodes[node].origin, tmp );
	tmp[2] += 16;
	if ( node == 0 )
	{		// First map node.. Always begins a path...
		npc_nodes[node].type |= NPC_NODE_ROUTE_BEGIN;
	}

	if ( node + 1 >= number_of_npc_nodes )
	{		// End of this route...
		npc_nodes[node].type |= NPC_NODE_ROUTE_END;
		return;
	}

	if ( BAD_WP_Height( npc_nodes[node].origin, npc_nodes[node + 1].origin) )
	{		// End of this route...
		npc_nodes[node].type |= NPC_NODE_ROUTE_END;
		npc_nodes[node + 1].type |= NPC_NODE_ROUTE_BEGIN;
		return;
	}

	if ( VectorDistance( npc_nodes[node + 1].origin, npc_nodes[node].origin) <= 256 )
	{
		int visCheck = NodeVisible( npc_nodes[node + 1].origin, tmp, -1 );
		if ( !(visCheck == 1 || visCheck == 2 || visCheck == 3) )
		{	// End of this route...
			npc_nodes[node].type |= NPC_NODE_ROUTE_END;
			npc_nodes[node + 1].type |= NPC_NODE_ROUTE_BEGIN;
		}
	}
	else
	{		// End of this route...
		npc_nodes[node].type |= NPC_NODE_ROUTE_END;
		npc_nodes[node + 1].type |= NPC_NODE_ROUTE_BEGIN;
	}
}


//===========================================================================
// Routine      : NPC_MAPPING_CreateSpecialNodeFlags

// Description  : Set up special flags for this node...
void
NPC_MAPPING_CreateSpecialNodeFlags ( int node )
{		// Need to check for duck (etc) nodes and mark them...
	trace_t tr;
	vec3_t	up, temp;
	VectorCopy( npc_nodes[node].origin, temp );
	temp[2] += 1;
	npc_nodes[node].type &= ~NPC_NODE_DUCK;
	VectorCopy( npc_nodes[node].origin, up );
	up[2] += 16550;
	trap_Trace( &tr, npc_nodes[node].origin, NULL, NULL, up, -1, MASK_SHOT | MASK_OPAQUE | MASK_WATER );
	if ( VectorDistance( npc_nodes[node].origin, tr.endpos) <= 72 )
	{	// Could not see the up pos.. Need to duck to go here!
		npc_nodes[node].type |= NPC_NODE_DUCK;
		G_Printf( "^4*** ^3%s^5: Node ^7%i^5 marked as a duck waypoint.\n", GAME_VERSION, node );
	}

	if ( AI_PM_SlickTrace( npc_nodes[node].origin, -1) )
	{	// This node is on slippery ice... Mark it...
		npc_nodes[node].type |= NPC_NODE_ICE;
		G_Printf( "^4*** ^3%s^5: Node ^7%i^5 marked as an ice (slick) waypoint.\n", GAME_VERSION, node );
	}
}


//===========================================================================
// Routine      : NPC_MAPPING_MakeLinks

// Description  : Set up linkages for this node...
void
NPC_MAPPING_MakeLinks ( void )
{
	int loop = 0;
	G_Printf( "^4*** ^3%s^5: Creating NPC waypoint linkages...\n", GAME_VERSION );
	for ( loop = 0; loop < number_of_npc_nodes; loop++ )
	{	// Do links...
		NPC_MAPPING_CreateNodeLinks( loop );

		// Also check if the node needs special flags...
		NPC_MAPPING_CreateSpecialNodeFlags( loop );
	}
}


//===========================================================================
// Routine      : NPC_NodesLoaded

// Description  : Have we got loaded nodes?
qboolean
NPC_NodesLoaded ( void )
{
	if ( npc_nodes_loaded == qtrue )
	{
		return ( qtrue );
	}
	else
	{
		return ( qfalse );
	}
}


//===========================================================================
// Routine      : NPC_NODES_SetObjectiveFlags

// Description  : NPCs won't use this in 0.1
void
NPC_NODES_SetObjectiveFlags ( int node )
{	// Find objects near this node.
	int loop = 0;

	// Init them first...
	npc_nodes[node].objectNum[0] = npc_nodes[node].objectNum[1] = npc_nodes[node].objectNum[2] = ENTITYNUM_NONE;
	npc_nodes[node].objEntity = -1;
	npc_nodes[node].objFlags = -1;
	npc_nodes[node].objTeam = -1;
	npc_nodes[node].objType = -1;
}

int begin_nodes[MAX_NODES];
int num_begin_nodes = 0;


//===========================================================================
// Routine      : NPC_NODES_CheckBeginNode

// Description  : Set up begin node array.. (For faster selection)
void
NPC_NODES_CheckBeginNode ( int node )
{
	if ( npc_nodes[node].type & NPC_NODE_ROUTE_BEGIN )
	{
		begin_nodes[num_begin_nodes] = node;
		num_begin_nodes++;
	}
}


//===========================================================================
// Routine      : NPC_Load_AddNode

// Description  : creates a new waypoint/node with the specified values
qboolean
NPC_Load_AddNode ( vec3_t origin, int fl, short int *ents, int objFl )
{
	if ( number_of_npc_nodes + 1 > MAX_NODES )
	{
		return ( qfalse );
	}

	VectorCopy( origin, npc_nodes[number_of_npc_nodes].origin );	//set the node's position
	npc_nodes[number_of_npc_nodes].type = fl;						//set the flags (NODE_OBJECTIVE, for example)
	npc_nodes[number_of_npc_nodes].objectNum[0] = ents[0];			//set any map objects associated with this node
	npc_nodes[number_of_npc_nodes].objectNum[1] = ents[1];			//only applies to objects linked to the unreachable flag
	npc_nodes[number_of_npc_nodes].objectNum[2] = ents[2];
	npc_nodes[number_of_npc_nodes].objFlags = objFl;				//set the objective flags
	NPC_NODES_CheckBeginNode( number_of_npc_nodes );
	number_of_npc_nodes++;
	return ( qtrue );
}


//===========================================================================
// Routine      : NPC_NODES_LoadNodes
// Description  : Initializes the nodes for a map from a file
//              : The file must be in a .pk3 file in the mod folder
//              : This function is completely dependent on the file format for the node files

//              : as well as the format of the NPC_NODE_t structure
void
NPC_NODES_LoadNodes ( void )
{
	fileHandle_t	f;
	int				i;
	char			filename[60];
	vmCvar_t		mapname;
	short int		objNum[3] = { 0, 0, 0 },
	objFlags, numLinks;
	int				flags;
	vec3_t			vec;
	char			name[] = npc_MOD_NAME;
	char			nm[64] = "";
	float			version;
	char			map[64] = "";
	char			mp[64] = "";
	short int		numberNodes;
	i = 0;
	strcpy( filename, "nodes/" );

	////////////////////
	trap_Cvar_VariableStringBuffer( "g_scriptName", filename, sizeof(filename) );
	if ( strlen( filename) > 0 )
	{
		trap_Cvar_Register( &mapname, "g_scriptName", "", CVAR_ROM );
	}
	else
	{
		trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );
	}

	Q_strcat( filename, sizeof(filename), mapname.string );

	///////////////////
	//open the node file for reading, return false on error
	trap_FS_FOpenFile( va( "nodes/%s.nwp", filename), &f, FS_READ );
	if ( !f )
	{
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.nwp^3 failed^5!!!\n", filename );
		G_Printf( "^1*** ^3       ^5  You need to make npc routes for this map.\n" );
		G_Printf( "^1*** ^3       ^5  NPCs will move randomly for this map.\n" );
		return;
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_ROM | CVAR_SERVERINFO );	//get the map name
	strcpy( mp, mapname.string );
	trap_FS_Read( &nm, strlen( name) + 1, f );									//read in a string the size of the mod name (+1 is because all strings end in hex '00')
	if ( Q_stricmp( nm, name) != 0 )
	{
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.nwp^3 failed^5!!!\n", filename );
		G_Printf( "^1*** ^3       ^5  Incompatible or corrupt node file.\n" );
		G_Printf( "^1*** ^3       ^5  NPCs will move randomly for this map.\n" );
		trap_FS_FCloseFile( f );
		return;												//if the file doesn't start with the mod name, it's not a valid .nod file, so abort
	}

	trap_FS_Read( &version, sizeof(float), f );				//read and make sure the version is the same
	if ( version != NOD_VERSION )
	{
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.nwp^3 failed^5!!!\n", filename );
		G_Printf( "^1*** ^3       ^5  Old node file detected.\n" );
		G_Printf( "^1*** ^3       ^5  NPCs will move randomly for this map.\n" );
		trap_FS_FCloseFile( f );
		return;
	}

	trap_FS_Read( &map, strlen( mp) + 1, f );				//make sure the file is for the current map
	if ( Q_stricmp( map, mp) != 0 )
	{
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.nwp^3 failed^5!!!\n", filename );
		G_Printf( "^1*** ^3       ^5  Node file is not for this map!\n" );
		G_Printf( "^1*** ^3       ^5  NPCs will move randomly for this map.\n" );
		trap_FS_FCloseFile( f );
		return;
	}

	trap_FS_Read( &numberNodes, sizeof(short int), f );		//read in the number of nodes in the map
	for ( i = 0; i < numberNodes; i++ )						//loop through all the nodes
	{
		trap_FS_Read( &vec, sizeof(vec3_t), f );
		trap_FS_Read( &flags, sizeof(int), f );
		trap_FS_Read( objNum, sizeof(short int) * 3, f );
		trap_FS_Read( &objFlags, sizeof(short int), f );
		trap_FS_Read( &numLinks, sizeof(short int), f );
		NPC_Load_AddNode( vec, flags, objNum, objFlags );	//add the node

		// Set node objective flags..
		NPC_NODES_SetObjectiveFlags( i );
	}

	trap_FS_FCloseFile( f );								//close the file
	G_Printf( "^1*** ^3%s^5: Successfully loaded %i NPC waypoints from waypoint file ^7nodes/%s.nwp^5.\n", GAME_VERSION,
			  number_of_npc_nodes, filename );
	npc_nodes_loaded = qtrue;
	return;
}

extern void RemoveDoorsAndDestroyablesForSave ( void );


//===========================================================================
// Routine      : NPC_NODES_SaveNodes
// Description  : saves all the current nodes and links to a file
//              : default filename is <mapname>.nod
//              : This function depends on the file format for .nod files

//              : and the NPC_NODE_t structure format
void
NPC_NODES_SaveNodes ( void )
{
	fileHandle_t	f;
	int				i;
	float			version = NOD_VERSION;	//version is 1.0 for now
	char			name[] = npc_MOD_NAME;
	vmCvar_t		mapname;
	char			map[64] = "";
	char			filename[60];
	short			num_nodes = number_of_npc_nodes;
	strcpy( filename, "nodes/" );

	////////////////////
	trap_Cvar_VariableStringBuffer( "g_scriptName", filename, sizeof(filename) );
	if ( strlen( filename) > 0 )
	{
		trap_Cvar_Register( &mapname, "g_scriptName", "", CVAR_ROM );
	}
	else
	{
		trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );
	}

	Q_strcat( filename, sizeof(filename), mapname.string );

	///////////////////
	//try to open the output file, return if it failed
	trap_FS_FOpenFile( va( "nodes/%s.nwp", filename), &f, FS_WRITE );
	if ( !f )
	{
		G_Printf( "^1*** ^3ERROR^5: Error opening NPC waypoint file ^7nodes/%s.nwp^5!!!\n", filename );
		return;
	}

	RemoveDoorsAndDestroyablesForSave();

	// Resolve paths
	//-------------------------------------------------------
	NPC_MAPPING_MakeLinks();
	for ( i = 0; i < number_of_npc_nodes; i++ )
	{

		// Set node objective flags..
		NPC_NODES_SetObjectiveFlags( i );
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );	//get the map name
	strcpy( map, mapname.string );
	trap_FS_Write( &name, strlen( name) + 1, f );								//write the mod name to the file
	trap_FS_Write( &version, sizeof(float), f );								//write the version of this file
	trap_FS_Write( &map, strlen( map) + 1, f );									//write the map name
	trap_FS_Write( &num_nodes, sizeof(short int), f );							//write the number of nodes in the map
	if ( npc_nodes[number_of_npc_nodes].type & NPC_NODE_AXIS_UNREACHABLE )
	{
		npc_nodes[number_of_npc_nodes].objTeam |= TEAM_ALLIES;
	}
	else if ( npc_nodes[number_of_npc_nodes].type & NPC_NODE_ALLY_UNREACHABLE )
	{
		npc_nodes[number_of_npc_nodes].objTeam |= TEAM_AXIS;
	}

	for ( i = 0; i < number_of_npc_nodes; i++ )									//loop through all the nodes
	{
		trap_FS_Write( &(npc_nodes[i].origin), sizeof(vec3_t), f );
		trap_FS_Write( &(npc_nodes[i].type), sizeof(int), f );
		trap_FS_Write( &(npc_nodes[i].objectNum), sizeof(short int) * 3, f );
		trap_FS_Write( &(npc_nodes[i].objFlags), sizeof(short int), f );
		trap_FS_Write( &(npc_nodes[i].enodenum), sizeof(short int), f );
	}

	trap_FS_FCloseFile( f );													//close the file
	G_Printf( "^1*** ^3%s^5: Successfully saved NPC waypoint file ^7nodes/%s.nwp^5.\n", GAME_VERSION, filename );
}


//===========================================================================
// Routine      : NPC_FODDER_AddNode

// Description  : Create a new NPC Fodder waypoint...
void
NPC_FODDER_AddNode ( int clientNum, int teamNum, qboolean isCoverPoint )
{
	int i;
	if ( teamNum == TEAM_ALLIES )
	{
		npc_nodes[number_of_npc_nodes].objTeam |= TEAM_ALLIES;
		npc_nodes[number_of_npc_nodes].type |= NPC_NODE_AXIS_UNREACHABLE;
	}
	else if ( teamNum == TEAM_AXIS )
	{
		npc_nodes[number_of_npc_nodes].type |= NPC_NODE_ALLY_UNREACHABLE;
		npc_nodes[number_of_npc_nodes].objTeam |= TEAM_AXIS;
	}

	if ( isCoverPoint )
	{
		npc_nodes[number_of_npc_nodes].type |= NPC_NODE_COVER;
	}

	if ( number_of_npc_nodes < MAX_NODES )
	{																// Setup the node's origin for now...
		VectorCopy( g_entities[clientNum].r.currentOrigin, npc_nodes[number_of_npc_nodes].origin );
		if ( !npc_nodes[number_of_npc_nodes].type )
		{
			npc_nodes[number_of_npc_nodes].type = NPC_NODE_MOVE;	// Default node type
		}

		// Create a connection between this new node and the previous one
		for ( i = 0; i < MAX_NODELINKS; i++ )
		{
			npc_nodes[number_of_npc_nodes].links[i].targetNode = INVALID;	// Set all links of the NEW node to invalid
		}

		if ( !isCoverPoint )
		{
			G_Printf( "^4*** ^3%s^5: NPC waypoint %i created.\n", GAME_VERSION, number_of_npc_nodes + 1 );
		}
		else
		{
			G_Printf( "^4*** ^3%s^5: NPC (cover point) waypoint %i created.\n", GAME_VERSION, number_of_npc_nodes + 1 );
		}

		number_of_npc_nodes++;												// We just added a node
	}
	else
	{	// No longer able to add nodes
		G_Printf( "Unable to add waypoint! Maximum number of NPC waypoints reached!\n" );
	}
}


//===========================================================================
// Routine      : NPC_NAVIGATION_FindClosestReachableNodeWithIgnore

// Description  : Find a close begin node to the player within a certain range
int
NPC_FODDER_FindClosestReachableNodeWithIgnore ( gentity_t *npc, int ignore, int r, int type )
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
	closest = rng + 1;												// Cuz if we found none in the range, we just found none .
	nonLOSclosest = rng + 1;										// Cuz if we found none in the range, we just found none .
	nonFOVclosest = rng + 1;										// Cuz if we found none in the range, we just found none .

	//
	// Will attempt a close only check first with vis checks...
	//
	//closest = 128.0f;
	//closest = 256.0f;
	closest = 512.0f;
	for ( i = 0; i < num_begin_nodes; i++ /*i+=Q_TrueRand(1,3)*/ )	// Randomize a little???
	{
		if ( i == ignore )
		{
			continue;
		}

		VectorSubtract( npc_nodes[begin_nodes[i]].origin, npc->r.currentOrigin, v );
		dist = VectorLength( v );
		if
		(
			dist < closest &&
			dist < 512 &&
			begin_nodes[i] != npc->current_node &&
			begin_nodes[i] != npc->next_node &&
			!BAD_WP_Height( npc->r.currentOrigin, npc_nodes[begin_nodes[i]].origin)
		)
		{
			if ( NodeVisible( npc->r.currentOrigin, npc_nodes[begin_nodes[i]].origin, npc->s.number) >= 1 )
			{														// Never go onto ice!
				secondbest = node;
				node = begin_nodes[i];
				closest = dist;
			}
		}
	}

	if ( secondbest >= 0 && Q_TrueRand( 0, 2) == 2 )
	{
		return ( secondbest );
	}
	else if ( node >= 0 )
	{
		return ( node );
	}

	closest = 256.0f;

	// Didn't find one -- Must be somewhere in route... Search all nodes...
	for ( i = 0; i < number_of_npc_nodes; i++ /*i+=Q_TrueRand(1,3)*/ )	// Randomize a little???
	{
		if ( i == ignore )
		{
			continue;
		}

		VectorSubtract( npc_nodes[i].origin, npc->r.currentOrigin, v );
		dist = VectorLength( v );
		if
		(
			dist < closest &&
			dist < 512 &&
			i != npc->current_node &&
			i != npc->next_node &&
			!BAD_WP_Height( npc->r.currentOrigin, npc_nodes[i].origin)
		)
		{
			if ( !(npc_nodes[i].type & NPC_NODE_ICE) )
			{															// Not on ice (slick)...
				if ( NodeVisible( npc->r.currentOrigin, npc_nodes[i].origin, npc->s.number) >= 1 )
				{														// Never go onto ice!
					secondbest = node;
					node = i;
					closest = dist;
				}
			}
		}
	}

	if ( secondbest >= 0 && Q_TrueRand( 0, 2) == 2 )
	{
		return ( secondbest );
	}
	else
	{
		return ( node );
	}
}


/* */
int
NPC_FODDER_CreateRoute ( gentity_t *NPC, short int *pathlist )
{	// Create a route not using links...
	int number = 0;
	int upto = 0;
	int from = NPC_FODDER_FindClosestReachableNodeWithIgnore( NPC, NPC->current_node, NODE_DENSITY, NODEFIND_ALL );
	int i = from;
	int to = 0;
	NPC->current_node = from;

	// Find the end node...
	while ( !(npc_nodes[i].type & NPC_NODE_ROUTE_END) )
	{
		i++;
	}

	// Set "to" to our end point...
	to = i;
	for ( upto = to; upto > from; upto-- )
	{
		pathlist[number] = upto;
		number++;
	}

	NPC->longTermGoal = to;
	if ( from >= 0 )
	{
		G_Printf( "NPC: (%i) Created route from %i to %i.\n", NPC->s.number, from, to );
	}

	return ( number - 1 );
}


//===========================================================================
// Routine      : NPC_FODDER_GetNextNode
// Description  : if the npc has reached a node, this function selects the next node
//				  that he will go to, and returns it right now it's being developed,

//				  feel free to experiment
short int
NPC_FODDER_GetNextNode ( gentity_t *NPC )
{
	short int	node = NPC_NODE_INVALID;

	//we should never call this in npcSTATE_MOVE with no goal
	//setup the goal/path in HandleIdleState
	if ( NPC->longTermGoal == NPC_NODE_INVALID )
	{
		return ( NPC_NODE_INVALID );
	}

	if ( NPC->pathsize == 0 )					//if the npc is at the end of his path, this shouldn't have been called
	{
		NPC->longTermGoal = NPC_NODE_INVALID;	//reset to having no goal
		return ( NPC_NODE_INVALID );
	}

	node = NPC->pathlist[NPC->pathsize - 1];	//pathlist is in reverse order
	NPC->pathsize--;							//mark that we've moved another node
	return ( node );
}


//===========================================================================
// Routine      : NPC_UcmdMoveForDir

// Description  : Set a valid ucmd move for the current move direction...
void
NPC_UcmdMoveForDir ( gentity_t *self, usercmd_t *cmd, vec3_t dir )
{
	vec3_t	forward, right, up;
	float	speed = 127.0f;
	if ( cmd->buttons & BUTTON_WALKING )
	{
		speed = 64.0f;
	}

	AngleVectors( self->NPC_client->ps.viewangles, forward, right, up );
	dir[2] = 0;
	VectorNormalize( dir );
	cmd->forwardmove = DotProduct( forward, dir ) * speed;
	cmd->rightmove = DotProduct( right, dir ) * speed;

	//cmd->upmove = abs(forward[3] ) * dir[3] * speed;
}

extern void NPCSetViewAngles ( gentity_t *NPC, float thinktime );
extern void Cmd_Activate_f ( gentity_t *ent );


//===========================================================================
// Routine      : NPC_AI_FODDER_Humanoid_Move

// Description  : Humanoid fodder type NPC routing here...
void
NPC_AI_FODDER_Humanoid_Move ( gentity_t *NPC )
{
	gentity_t	*obstacle = NULL;
	usercmd_t	*ucmd = &NPC->NPC_client->pers.cmd;
	float		diff = 0;
	if ( !NPC_NodesLoaded() )
	{
		return;											// We have no NPC specific waypoints!!!
	}

	G_SetMovedir( NPC->s.angles, NPC->movedir );
	NPC->s.pos.trDelta[0] = 120;
	NPC->s.pos.trDelta[1] = 120;
	NPC->s.pos.trDelta[2] = 120;
	NPC->instantVelocity[0] = 120;
	NPC->instantVelocity[1] = 120;
	NPC->instantVelocity[2] = 120;
	trap_SnapVector( NPC->instantVelocity );
	trap_SnapVector( NPC->s.pos.trDelta );
	if
	(
		NPC->s.dl_intensity == TAG_HOOK ||
		NPC->s.dl_intensity == TAG_HOOK_DRIVER ||
		NPC->s.dl_intensity == TAG_HOOK_GUNNER1 ||
		NPC->s.dl_intensity == TAG_HOOK_GUNNER2
	)
	{
		return;											// No moving while riding an NPC...
	}

	// FODDER Should not require any strafing...

	/*if (NPC->bot_strafe_right_timer >= level.time)
	{
		if (VectorDistance(NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8)
		{
			ucmd->forwardmove = 64;
			ucmd->upmove = 0;
			ucmd->rightmove = 64;
			return;
		}
		else
		{
			NPC->bot_strafe_right_timer = 0;
			ucmd->upmove = 127;
		}
	}

	if (NPC->bot_strafe_left_timer >= level.time)
	{
		if (VectorDistance(NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8)
		{
			ucmd->forwardmove = 64;
			ucmd->upmove = 0;
			ucmd->rightmove = -64;
			return;
		}
		else
		{
			NPC->bot_strafe_left_timer = 0;
			ucmd->upmove = 127;
		}
	}*/
	if
	(
		NPC->longTermGoal < 0 ||
		NPC->longTermGoal > number_of_npc_nodes ||
		NPC->current_node < 0 ||
		NPC->next_node < 0 ||
		NPC->node_timeout <= level.time
	)
	{													// We hit a problem in route, or don't have one yet.. Find a new goal...
		memset( NPC->pathlist, NPC_NODE_INVALID, MAX_PATHLIST_NODES );
		NPC->pathsize = NPC_FODDER_CreateRoute( NPC, NPC->pathlist );
		if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_npc_nodes )
		{												// FIXME: Try to roam out of problems...
			return;										// next think...
		}

		NPC->next_node = NPC_FODDER_GetNextNode( NPC ); //move to this node first, since it's where our path starts from
		NPC->node_timeout = level.time + 5000;
	}

	if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_npc_nodes )
	{													// Try to roam out of problems...
		return;											// next think...
	}

	if ( VectorDistance( npc_nodes[NPC->longTermGoal].origin, NPC->r.currentOrigin) < 48 )
	{													// We're there! Find a new goal...
		memset( NPC->pathlist, NPC_NODE_INVALID, MAX_PATHLIST_NODES );
		NPC->pathsize = NPC_FODDER_CreateRoute( NPC, NPC->pathlist );
		if ( NPC->longTermGoal < 0 || NPC->longTermGoal > MAX_GENTITIES )
		{												// Try to roam out of problems...
			return;										// next think...
		}

		NPC->next_node = NPC_FODDER_GetNextNode( NPC ); //move to this node first, since it's where our path starts from
		NPC->node_timeout = level.time + 5000;
	}

	if ( NPC->current_node <= 0 || NPC->current_node >= number_of_npc_nodes )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;											// Next think...
	}

	if
	(
		npc_nodes[NPC->current_node].type & NPC_NODE_COVER ||
		(
			VectorDistance( npc_nodes[NPC->next_node].origin, NPC->r.currentOrigin) < 128 &&
			npc_nodes[NPC->next_node].type & NPC_NODE_COVER &&
			VectorLength( NPC->NPC_client->ps.velocity) < 16
		)
	)
	{
		float		cover_dist = VectorDistance( npc_nodes[NPC->current_node].origin, NPC->r.currentOrigin );
		qboolean	take_cover = qfalse;
		if
		(
			VectorDistance( npc_nodes[NPC->next_node].origin, NPC->r.currentOrigin) < 128 &&
			npc_nodes[NPC->next_node].type & NPC_NODE_COVER
		)
		{
			cover_dist = VectorDistance( npc_nodes[NPC->next_node].origin, NPC->r.currentOrigin );
		}

		if ( cover_dist < 16 || cover_dist >= NPC->NPC_CoverPoint_Last_Distance )
		{												// Use cover point...
			take_cover = qtrue;
		}

		// Set our last distance, in case theres a group of npcs seeking cover here...
		if
		(
			VectorDistance( npc_nodes[NPC->next_node].origin, NPC->r.currentOrigin) < 128 &&
			npc_nodes[NPC->next_node].type & NPC_NODE_COVER
		)
		{
			NPC->NPC_CoverPoint_Last_Distance = VectorDistance( npc_nodes[NPC->next_node].origin, NPC->r.currentOrigin );
		}
		else
		{
			NPC->NPC_CoverPoint_Last_Distance = VectorDistance( npc_nodes[NPC->current_node].origin,
																NPC->r.currentOrigin );
		}

		if ( NPC->NPC_CoverPoint )
		{												// Using cover...
			if ( NPC->NPC_CoverPoint_Timer > level.time )
			{											// We want to stay still!
				int			lookOver = Q_irand( 0, 1000 );
				qboolean	do_look = qfalse;
				if
				(
					lookOver <= 1 &&
					NPC->NPC_CoverPoint_LookUpTimer <= level.time &&
					(
						(NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER_ATTACKER) ||
						(NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER_DEFENDER)
					)
				)
				{										// Occasionally look over the cover we are behind, and maybe shoot someone...
					do_look = qtrue;
				}

				if ( NPC->enemy )
				{										// Set up ideal view angles for this enemy...
					VectorSubtract( NPC->enemy->r.currentOrigin, NPC->r.currentOrigin, NPC->move_vector );
					vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );
					NPC->bot_ideal_view_angles[2] = 0;	// Always look straight.. Don't look up or down at nodes...
					NPCSetViewAngles( NPC, 500 );
					VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
					ucmd->angles[PITCH] = ANGLE2SHORT( NPC->s.angles[PITCH] );
					ucmd->angles[YAW] = ANGLE2SHORT( NPC->s.angles[YAW] );
					ucmd->angles[ROLL] = ANGLE2SHORT( NPC->s.angles[ROLL] );
				}
				else
				{										// Set up ideal view angles for this node...
					VectorSubtract( npc_nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
					vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );
					NPC->bot_ideal_view_angles[2] = 0;	// Always look straight.. Don't look up or down at nodes...
					NPCSetViewAngles( NPC, 500 );
					VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
					ucmd->angles[PITCH] = ANGLE2SHORT( NPC->s.angles[PITCH] );
					ucmd->angles[YAW] = ANGLE2SHORT( NPC->s.angles[YAW] );
					ucmd->angles[ROLL] = ANGLE2SHORT( NPC->s.angles[ROLL] );
				}

				if ( NPC->NPC_CoverPoint_LookUpTimer > level.time )
				{
					NPC->NPC_client->ps.pm_flags &= ~PMF_DUCKED;
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					NPC->node_timeout = level.time + 5000;
					return;
				}
				else if ( do_look )
				{										// Occasionally look over the cover we are behind, and maybe shoot someone...
					NPC->NPC_CoverPoint_LookUpTimer = level.time + 2000 + Q_irand( 0, 3000 );
					NPC->NPC_client->ps.pm_flags &= ~PMF_DUCKED;
					ucmd->forwardmove = 0;
					ucmd->rightmove = 0;
					ucmd->upmove = 0;
					G_Printf( "NPC %i looking over it's cover!\n", NPC->s.number );
					NPC->node_timeout = level.time + 5000;
					return;
				}

				NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = -48;
				NPC->node_timeout = level.time + 5000;
				return;
			}
			else
			{											// Free to continue!
				NPC->NPC_CoverPoint = qfalse;
				NPC->NPC_client->ps.pm_flags &= ~PMF_DUCKED;
				NPC->last_node = NPC->current_node;
				NPC->current_node = NPC->next_node;
				NPC->next_node = NPC_FODDER_GetNextNode( NPC );
				NPC->node_timeout = level.time + 5000;
				G_Printf( "NPC is %i leaving cover point!\n", NPC->s.number );
			}
		}
		else if ( take_cover )
		{												// Set up our cover point timer....
			NPC->NPC_CoverPoint_Timer = level.time + Q_irand( 5000, 20000 );
			NPC->NPC_CoverPoint = qtrue;
			NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = -48;
			G_Printf( "NPC %i taking cover! (for %i secs)\n", NPC->s.number, (NPC->NPC_CoverPoint_Timer - level.time) / 1000 );
			NPC->node_timeout = level.time + 5000;
			return;
		}
	}

	//if (NodeVisible(NPC->r.currentOrigin, nodes[NPC->current_node].origin, NPC->s.number) == 2)
	if ( VectorLength( NPC->NPC_client->ps.velocity) < 16 )
	{													// A door...
		Cmd_Activate_f( NPC );
	}

	if ( VectorDistance( npc_nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 48 )
	{													// At current node.. Pick next in the list...
		NPC->last_node = NPC->current_node;
		NPC->current_node = NPC->next_node;
		NPC->next_node = NPC_FODDER_GetNextNode( NPC );
		NPC->node_timeout = level.time + 5000;
	}

	if
	(
		npc_nodes[NPC->current_node].type & NPC_NODE_JUMP ||
		npc_nodes[NPC->current_node].origin[2] - NPC->r.currentOrigin[2] > 8
	)
	{													// For special jump nodes, make the bot jump...
		ucmd->upmove = 127;
	}
	else if ( npc_nodes[NPC->current_node].type & NPC_NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
	{													// For special duck nodes, make the bot duck down...
		//ucmd->upmove = -48;
		NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
	}
	else if ( npc_nodes[NPC->last_node].type & NPC_NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
	{													// For special duck nodes, make the bot duck down...
		//ucmd->upmove = -48;
		NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
	}
	else if
		(
			!(npc_nodes[NPC->current_node].type & NPC_NODE_DUCK) &&
			!(npc_nodes[NPC->last_node].type & NPC_NODE_DUCK) &&
			!NPC->enemy &&
			(NPC->NPC_client->ps.eFlags & EF_CROUCHING)
		)
	{													// Get up?
		ucmd->upmove = 127;
		NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
	}
	else if ( !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_PRONE) )
	{													// Get up?
		ucmd->upmove = 127;
		NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
	}

	// FODDER Should not require any strafing...

	/*	if (NPC->bot_strafe_right_timer < level.time 
		&& NPC->bot_strafe_left_timer < level.time
		&& VectorLength(NPC->NPC_client->ps.velocity) < 24 
		&& !( npc_nodes[NPC->current_node].type & NPC_NODE_DUCK ) 
		&& !( npc_nodes[NPC->last_node].type & NPC_NODE_DUCK ) )
	{// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
		vec3_t forward, right, up, rightOrigin, leftOrigin, temp;
		trace_t tr;

		AngleVectors( NPC->NPC_client->ps.viewangles, forward, right, up );
		VectorMA( NPC->r.currentOrigin, 64, right, rightOrigin );
		VectorMA( NPC->r.currentOrigin, -64, right, leftOrigin );

		if (OrgVisible(rightOrigin, npc_nodes[NPC->current_node].origin, NPC->s.number))
		{
			VectorCopy(rightOrigin, NPC->bot_strafe_target_position);
			NPC->bot_strafe_right_timer = level.time + 101;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 64;
			ucmd->upmove = 0;
		}
		else if (OrgVisible(leftOrigin, npc_nodes[NPC->current_node].origin, NPC->s.number))
		{
			VectorCopy(leftOrigin, NPC->bot_strafe_target_position);
			NPC->bot_strafe_left_timer = level.time + 100;
			ucmd->forwardmove = 0;
			ucmd->rightmove = -64;
			ucmd->upmove = 0;
		}
		else
		{
			ucmd->upmove = 127;
			ucmd->forwardmove = 127;
			NPC->NPC_client->ps.velocity[2] = 256;
		}

		// Prepare to set another player/bot's (our blocker) strafe dir...
		VectorCopy(NPC->r.currentOrigin, temp);
		temp[0]+=forward[0]*64;
		temp[1]+=forward[1]*64;
		temp[2]+=forward[2]*64;

		trap_Trace(&tr, NPC->r.currentOrigin, NULL, NULL, temp, -1, MASK_SHOT|MASK_OPAQUE);

		if (tr.entityNum && (tr.entityNum < MAX_CLIENTS && g_entities[tr.entityNum].r.svFlags & SVF_BOT))
		{// A bot is stopping us... Tell them to strafe other direction to us...
			if (NPC->bot_strafe_left_timer >= level.time)
			{
				g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;

				AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
				VectorCopy(leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
			}
			else if (NPC->bot_strafe_right_timer >= level.time)
			{
				g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;	

				AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
				VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
				VectorCopy(rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position);
			}
		}
	}

	if (NPC->bot_strafe_right_timer >= level.time || NPC->bot_strafe_left_timer >= level.time)
	{
		return;
	}*/

	// Set up ideal view angles for this node...
	VectorSubtract( npc_nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
	vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );
	NPC->bot_ideal_view_angles[2] = 0;					// Always look straight.. Don't look up or down at nodes...

	// UQ1: Try using advanced movement... (look one way, shoot another)...

	/*	diff = fabs(AngleDifference(NPC->NPC_client->ps.viewangles[YAW], NPC->bot_ideal_view_angles[YAW]));

	if (diff > 90.0f)
	{// Delay large turns...
		NPCSetViewAngles(NPC, 500);
		VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
		ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
		ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
		ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
	}
	else
	{
		VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
		ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
		ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
		ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
					
		if (ucmd->upmove > 0) // We need to jump something...
		{
			ucmd->forwardmove = 127;
			ucmd->buttons &= ~BUTTON_WALKING;
		}
		else
		{
			ucmd->forwardmove = 64;
			ucmd->buttons |= BUTTON_WALKING;
		}

		NPC->bot_last_good_move_time = level.time;
	}*/

	/*VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
	ucmd->angles[PITCH] = ANGLE2SHORT(NPC->s.angles[PITCH]);
	ucmd->angles[YAW]	= ANGLE2SHORT(NPC->s.angles[YAW]);
	ucmd->angles[ROLL]	= ANGLE2SHORT(NPC->s.angles[ROLL]);
	ucmd->forwardmove = 127;
	NPC->bot_last_good_move_time = level.time;*/
	NPCSetViewAngles( NPC, 100 );
	NPC_UcmdMoveForDir( NPC, ucmd, NPC->move_vector );
	NPC->bot_last_good_move_time = level.time;

	//G_Printf("NPC: (%i) Moving (current node is %i).\n", NPC->s.number, NPC->current_node);
}

extern int		gWPRenderTime;
extern int		gWPRenderedFrame;
extern int		gWPNum;
extern int		gLastPrintedIndex;
extern vmCvar_t npc_node_edit;
qboolean		npc_shownodes = qfalse;


/* */
void
NPC_FODDER_WaypointRender ( void )
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
	if ( npc_node_edit.integer )
	{
		npc_shownodes = qtrue;
	}

	if ( !npc_shownodes )
	{
		return;
	}

	gWPNum = number_of_npc_nodes;
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
		if ( npc_nodes[i].origin )
		{
			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, npc_nodes[i].origin) < 128 &&
				OrgVisible( npc_nodes[i].origin, eyes, -1)
			)
			{
				vec3_t	up;
				VectorCopy( npc_nodes[i].origin, up );
				up[2] += i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				if ( npc_nodes[i].type == NPC_NODE_ROUTE_END )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( npc_nodes[i].type == NPC_NODE_JUMP )
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

					for (z = 0; z < npc_nodes[i].enodenum; z++)
					{
						DebugLine(npc_nodes[i].origin, npc_nodes[npc_nodes[i].links[z].targetNode].origin, 1);
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

		if ( npc_nodes[i].origin )
		{
			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, npc_nodes[i].origin) < 512 &&
				VectorDistance( g_entities[0].r.currentOrigin, npc_nodes[i].origin) >= 128 &&
				OrgVisible( npc_nodes[i].origin, eyes, -1)
			)
			{
				vec3_t	up;
				VectorCopy( npc_nodes[i].origin, up );
				up[2] += i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				if ( npc_nodes[i].type == NPC_NODE_ROUTE_END )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( npc_nodes[i].type == NPC_NODE_JUMP )
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

					for (z = 0; z < npc_nodes[i].enodenum; z++)
					{
						DebugLine(npc_nodes[i].origin, npc_nodes[npc_nodes[i].links[z].targetNode].origin, 1);
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

		if ( npc_nodes[i].origin )
		{
			if
			(
				VectorDistance( g_entities[0].r.currentOrigin, npc_nodes[i].origin) < 1024 &&
				VectorDistance( g_entities[0].r.currentOrigin, npc_nodes[i].origin) >= 512 &&
				OrgVisible( npc_nodes[i].origin, eyes, -1)
			)
			{
				vec3_t	up;
				VectorCopy( npc_nodes[i].origin, up );
				up[2] += i / 50;
				plum = G_TempEntity( up, EV_SCOREPLUM );
				plum->r.svFlags |= SVF_BROADCAST;
				plum->s.time = i;
				if ( npc_nodes[i].type == NPC_NODE_ROUTE_END )
				{
					plum->s.weapon = 1;		// Mark this as a goal node for the yellow highlight.
				}
				else if ( npc_nodes[i].type == NPC_NODE_JUMP )
				{
					plum->s.weapon = 2;		// Mark this as a jump node for the red highlight.
				}
				else if ( npc_nodes[i].type == NPC_NODE_DUCK )
				{
					plum->s.weapon = 3;		// Mark this as a duck node for the blue highlight.
				}
				else
				{
					plum->s.weapon = 0;
				}

				n = 0;
				gWPRenderedFrame++;

				/*{
					int z;

					for (z = 0; z < npc_nodes[i].enodenum; z++)
					{
						DebugLine(npc_nodes[i].origin, npc_nodes[npc_nodes[i].links[z].targetNode].origin, 1);
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
	if ( !npc_node_edit.integer )
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
		if ( npc_nodes[i].origin /*&& npc_nodes[i]->inuse*/ )
		{
			VectorSubtract( viewent->client->ps.origin, npc_nodes[i].origin, a );
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
		if ( npc_nodes[bestindex].type == NPC_NODE_JUMP )
		{
			flagstr = "Jump";
		}
		else if ( npc_nodes[bestindex].type == NPC_NODE_WATER )
		{
			flagstr = "Water";
		}
		else if ( npc_nodes[bestindex].type == NPC_NODE_LADDER )
		{
			flagstr = "Ladder";
		}
		else if ( npc_nodes[bestindex].type == NPC_NODE_OBJECTIVE )
		{
			flagstr = "Objective";
		}
		else if ( npc_nodes[bestindex].type == NPC_NODE_BUILD )
		{
			flagstr = "Build";
		}
		else if ( npc_nodes[bestindex].type == NPC_NODE_MG42 )
		{
			flagstr = "MG-42";
		}
		else if ( npc_nodes[bestindex].type == NPC_NODE_ROUTE_END )
		{
			flagstr = "Goal Node";
		}
		else
		{
			flagstr = "None";
		}

		gLastPrintedIndex = bestindex;
		G_Printf( "^5NPC Node ^7%i\n^5Flags - ^3%i^5 (^7%s^5)\n^5Origin - (^3%i %i %i^5)\n", (int) (bestindex),
				  (int) npc_nodes[bestindex].type, flagstr, (int) (npc_nodes[bestindex].origin[0]),
				  (int) (npc_nodes[bestindex].origin[1]), (int) (npc_nodes[bestindex].origin[2]) );
		plum = G_TempEntity( npc_nodes[bestindex].origin, EV_SCOREPLUM );
		plum->r.svFlags |= SVF_BROADCAST;
		plum->s.time = bestindex;			//render it once
	}
	else if ( !gotbestindex )
	{
		gLastPrintedIndex = -1;
	}
}
#endif //__NPC__
