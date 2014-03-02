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
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
NODES FUNCTION LISTING
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
AIMOD_NODES_SaveNodes
AIMOD_NODES_LoadNodes
WaypointThink
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
*/

/*
                   .___             
   ____   ____   __| _/____   ______
  /    \ /  _ \ / __ |/ __ \ /  ___/
 |   |  (  <_> ) /_/ \  ___/ \___ \ 
 |___|  /\____/\____ |\___  >____  >
      \/            \/    \/     \/ 

    uniquebot_nodes.c 
    Pathing routines for the FUSION bot.
================================================================
Should you decide to release a modified version of FUSION, you MUST
include the following text in the documentation and source of your 
modification*/
#include "../game/g_local.h"
#include "../botai/ai_main.h"
qboolean		nodes_loaded = qfalse;

// Flags*/
//-------------------------------------------------------
qboolean		newmap = qtrue;

// Total number of nodes that are items
//-------------------------------------------------------
int				number_of_itemnodes;

// Total number of nodes
//-------------------------------------------------------
int				number_of_nodes;

// array for node data
//-------------------------------------------------------
node_t			nodes[MAX_NODES];
node_convert_t	nodes_convert[MAX_NODES];


/* */
void
AIMOD_NODES_AddLinks ( void )
{

} extern void	AIMOD_MAPPING_CreateNodeConnection ( int from, int to );
extern void		AIMOD_MAPPING_SetConnectionCost ( int from, int to, int new_cost );

//===========================================================================
// Routine      : AIMOD_NODES_SaveNodes
// Description  : Save nodes to .nod file on disk
extern void		AIMOD_MAPPING_MakeLinks ( void );

//===========================================================================
// Routine      : NodesLoaded

// Description  : Have we got loaded nodes?
qboolean
NodesLoaded ( void )
{
	if ( nodes_loaded == qtrue )
	{
		return ( qtrue );
	}
	else
	{
		return ( qfalse );
	}
}

//===========================================================================
// Routine      : AIMOD_NODES_LoadNodes
// Description  : Loads nodes from .nod file on disk
extern void AIMOD_MAPPING_MakeLinks ( void );
extern void BotMP_Update_Goal_Lists ( void );
extern int	GoalType ( int entNum, int teamNum );
extern int	num_bot_objectives;
extern int	num_bot_goals;
extern int	BotObjectives[MAX_GENTITIES];
extern int	BotGoals[MAX_GENTITIES];


/* */
void
AIMOD_NODES_SetObjectiveFlags ( int node )
{										// Find objects near this node.
	int loop = 0;
	if ( nodes[node /*number_of_nodes*/ ].type & NODE_AXIS_UNREACHABLE )
	{
		nodes[node].objTeam |= TEAM_ALLIES;
	}
	else if ( nodes[node /*number_of_nodes*/ ].type & NODE_ALLY_UNREACHABLE )
	{
		nodes[node].objTeam |= TEAM_AXIS;
	}

	// Init them first...
	nodes[node].objectNum[0] = nodes[node].objectNum[1] = nodes[node].objectNum[2] = ENTITYNUM_NONE;
	nodes[node].objEntity = -1;
	nodes[node].objFlags = -1;
	if ( !nodes[node].objTeam )
	{
		nodes[node].objTeam = -1;
	}

	nodes[node].objType = -1;
	if ( num_bot_objectives <= 0 && num_bot_goals <= 0 )
	{
		return;							// Have no goals..
	}

	// Now link objectives...
	for ( loop = 0; loop < num_bot_objectives; loop++ )
	{

		//		int nameNum = 0;
		int type = 0;
		if ( VectorDistance( g_entities[BotObjectives[loop]].r.currentOrigin, nodes[node].origin) > 96 )
		{
			continue;					// Too far dont even think about it... lol..
		}

		// NOTE: ALLIES are opposite for BUILD/DESTROY goals.
		type = GoalType( BotObjectives[loop], TEAM_AXIS );
		if
		(
			type != BOTGOAL_CONSTRUCT &&
			type != BOTGOAL_DESTROY &&
			type != BOTGOAL_FLAG &&
			type != BOTGOAL_CHECKPOINT &&
			type != OBJECTIVE_CAPTURE &&
			type != BOTGOAL_POPFLAG
		)
		{
			continue;					// No point marking the others on nodes...
		}

		// We found an objective here...
		nodes[node].objectNum[0] = BotObjectives[loop];
		nodes[node].objEntity = BotObjectives[loop];
		nodes[node].objFlags = -1;		// Not currently used...
		if ( !nodes[node].objTeam )
		{
			nodes[node].objTeam = -1;	// Not currently used...
		}

		if ( type == BOTGOAL_CONSTRUCT )
		{
			nodes[node].objType = OBJECTIVE_BUILD;
		}
		else if ( type == BOTGOAL_DESTROY )
		{
			nodes[node].objType = OBJECTIVE_DYNAMITE;
		}
		else if ( type == BOTGOAL_FLAG || type == BOTGOAL_CHECKPOINT )
		{
			nodes[node].objType = OBJECTIVE_FLAG;
		}
		else if ( type == BOTGOAL_POPFLAG )
		{
			nodes[node].objType = OBJECTIVE_POPFLAG;
		}
		else if ( type == BOTGOAL_OBJECTIVE )
		{
			nodes[node].objType = OBJECTIVE_CAPTURE;
		}

		return;							// We're done here!
	}

	// Now link goals...
	for ( loop = 0; loop < num_bot_goals; loop++ )
	{

		//		int nameNum = 0;
		int type = 0;
		if ( VectorDistance( g_entities[BotGoals[loop]].r.currentOrigin, nodes[node].origin) > 96 )
		{
			continue;					// Too far dont even think about it... lol..
		}

		// NOTE: ALLIES are opposite for BUILD/DESTROY goals.
		type = GoalType( BotGoals[loop], TEAM_AXIS );
		if
		(
			type != BOTGOAL_CONSTRUCT &&
			type != BOTGOAL_DESTROY &&
			type != BOTGOAL_FLAG &&
			type != BOTGOAL_CHECKPOINT &&
			type != OBJECTIVE_CAPTURE &&
			type != BOTGOAL_POPFLAG
		)
		{
			continue;					// No point marking the others on nodes...
		}

		// We found an objective here...
		nodes[node].objectNum[0] = BotGoals[loop];
		nodes[node].objEntity = BotGoals[loop];
		nodes[node].objFlags = -1;		// Not currently used...
		if ( !nodes[node].objTeam )
		{
			nodes[node].objTeam = -1;	// Not currently used...
		}

		if ( type == BOTGOAL_CONSTRUCT )
		{
			nodes[node].objType = OBJECTIVE_BUILD;
		}
		else if ( type == BOTGOAL_DESTROY )
		{
			nodes[node].objType = OBJECTIVE_DYNAMITE;
		}
		else if ( type == BOTGOAL_FLAG || type == BOTGOAL_CHECKPOINT )
		{
			nodes[node].objType = OBJECTIVE_FLAG;
		}
		else if ( type == BOTGOAL_POPFLAG )
		{
			nodes[node].objType = OBJECTIVE_POPFLAG;
		}
		else if ( type == BOTGOAL_OBJECTIVE )
		{
			nodes[node].objType = OBJECTIVE_CAPTURE;
		}

		return;							// We're done here!
	}
}

// This was the old (pre-version 6) format flags... Using new ones, so we need conversion..
#define OLD_NODE_MOVE			0		// Move Node
#define OLD_NODE_OBJECTIVE		1
#define OLD_NODE_TARGET			2
#define OLD_NODE_TARGETSELECT	3
#define OLD_NODE_FASTHOP		4
#define OLD_NODE_GOAL			5
#define OLD_NODE_WATER			6
#define OLD_NODE_LADDER			7		// Ladder Node
#define OLD_NODE_MG42			8
#define OLD_NODE_DYNAMITE		9
#define OLD_NODE_BUILD			10
#define OLD_NODE_JUMP			11
#define OLD_NODE_DUCK			12


/* */
int
AIMOD_NODES_Convert_Old_Flags ( int node )
{
	int newtype = 0;
	if ( nodes[node].type == OLD_NODE_OBJECTIVE )
	{
		newtype |= NODE_OBJECTIVE;
	}

	if ( nodes[node].type == OLD_NODE_FASTHOP )
	{
		newtype |= NODE_FASTHOP;
	}

	if ( nodes[node].type == OLD_NODE_WATER )
	{
		newtype |= NODE_WATER;
	}

	if ( nodes[node].type == OLD_NODE_LADDER )
	{
		newtype |= NODE_LADDER;
	}

	if ( nodes[node].type == OLD_NODE_MG42 )
	{
		newtype |= NODE_MG42;
	}

	if ( nodes[node].type == OLD_NODE_DYNAMITE )
	{
		newtype |= NODE_DYNAMITE;
	}

	if ( nodes[node].type == OLD_NODE_BUILD )
	{
		newtype |= NODE_BUILD;
	}

	if ( nodes[node].type == OLD_NODE_JUMP )
	{
		newtype |= NODE_JUMP;
	}

	if ( nodes[node].type == OLD_NODE_DUCK )
	{
		newtype |= NODE_DUCK;
	}

	return ( newtype );
}


/*//////////////////////////////////////////////
AddNode
creates a new waypoint/node with the specified values
*/

/////////////////////////////////////////////
int numAxisOnlyNodes = 0;
int AxisOnlyFirstNode = -1;
int numAlliedOnlyNodes = 0;
int AlliedOnlyFirstNode = -1;


/* */
qboolean
Load_AddNode ( vec3_t origin, int fl, short int *ents, int objFl )
{

	//	int objTeam;
	//make sure we don't already have too many nodes
	//curently MAX_NODES is 8000
	if ( number_of_nodes + 1 > MAX_NODES )
	{
		return ( qfalse );
	}

	VectorCopy( origin, nodes[number_of_nodes].origin );	//set the node's position

	//nodes[number_of_nodes].num = number_of_nodes;				//set it's number
	nodes[number_of_nodes].type = fl;						//set the flags (NODE_OBJECTIVE, for example)
	nodes[number_of_nodes].objectNum[0] = ents[0];			//set any map objects associated with this node
	nodes[number_of_nodes].objectNum[1] = ents[1];			//only applies to objects linked to the unreachable flag
	nodes[number_of_nodes].objectNum[2] = ents[2];
	nodes[number_of_nodes].objFlags = objFl;				//set the objective flags
	if ( nodes[number_of_nodes].type & NODE_AXIS_UNREACHABLE )
	{
		if ( AlliedOnlyFirstNode < 0 )
		{
			AlliedOnlyFirstNode = number_of_nodes;
		}

		//G_Printf("Node %i is allied only!\n", number_of_nodes);
		nodes[number_of_nodes].objTeam |= TEAM_ALLIES;
		numAlliedOnlyNodes++;
	}

	if ( nodes[number_of_nodes].type & NODE_ALLY_UNREACHABLE )
	{
		if ( AxisOnlyFirstNode < 0 )
		{
			AxisOnlyFirstNode = number_of_nodes;
		}

		//G_Printf("Node %i is axis only!\n", number_of_nodes);
		nodes[number_of_nodes].objTeam |= TEAM_AXIS;
		numAxisOnlyNodes++;
	}

	//get some objective-related data out of the flags (this is based on the .wps format - see the forum)

	/*	nodes[number_of_nodes].objEntity = ((objFl & 0x0FFF0) >> 7);
	nodes[number_of_nodes].objType = ((objFl & 0x0000F) >> 2);
	
	objTeam = (objFl & 1);
	if (objTeam == 0)
		nodes[number_of_nodes].objTeam = TEAM_ALLIES;
	else if (objTeam == 1)
		nodes[number_of_nodes].objTeam = TEAM_AXIS;
*/
	number_of_nodes++;
	return ( qtrue );
}

/*////////////////////////////////////////////////
ConnectNodes
Connects 2 nodes and sets the flags for the path between them
/*/


///////////////////////////////////////////////
qboolean
ConnectNodes ( int from, int to, int flags )
{

	//check that we don't have too many connections from the 'from' node already
	if ( nodes[from].enodenum + 1 > MAX_NODELINKS )
	{
		return ( qfalse );
	}

	//check that we are creating a path between 2 valid nodes
	if ( (nodes[from].type == NODE_INVALID) || (to == NODE_INVALID) )
	{	//nodes[to].type is invalid on LoadNodes()
		return ( qfalse );
	}

	//update the individual nodes
	nodes[from].links[nodes[from].enodenum].targetNode = to;
	nodes[from].links[nodes[from].enodenum].flags = flags;
	nodes[from].enodenum++;
	return ( qtrue );
}

#define BOT_MOD_NAME	"aimod"
#define NOD_VERSION		1.0f


/* */
void
AIMOD_NODES_FixAASNodes ( void )
{
	int i;
	G_Printf( "^1*** ^3%s^5: Repairing loaded AAS node file.\n", GAME_VERSION );
	for ( i = 0; i < number_of_nodes; i++ )
	{
		trace_t tr;
		vec3_t	tempPos;
		VectorCopy( nodes[i].origin, tempPos );
		tempPos[2] -= 32000;
		trap_Trace( &tr, nodes[i].origin, NULL, NULL, tempPos, ENTITYNUM_NONE, MASK_SOLID );
		VectorCopy( tr.endpos, nodes[i].origin );
		nodes[i].origin[2] += 16;
	}

	G_Printf( "^1*** ^3%s^5: Loaded AAS node file repaired.\n", GAME_VERSION );
	AIMOD_NODES_SaveNodes();
}

/*/////////////////////////////////////////////
LoadNodes
Initializes the nodes for a map from a file
The file must be in a .pk3 file in the mod folder
This function is completely dependent on the file format for the node files
as well as the format of the node_t structure
*/

/////////////////////////////////////////////
qboolean	fix_aas_nodes = qfalse;

qboolean
AIMOD_NODES_LoadNodesFromPK3 ( fileHandle_t f, const char* mapname )
{
	short int		objNum[3] = { 0, 0, 0 },
	objFlags, numLinks;
	int				flags, i, j;
	vec3_t			vec;
	short int		target, fl2;
	char			name[] = BOT_MOD_NAME;
	char			nm[MAX_QPATH] = "";
	float			version;
	char			map[MAX_QPATH] = "";
	short int		numberNodes;

	if( f == 0 || mapname == 0 || *mapname == 0 ){
		G_Printf( "^1*** ^3ERROR^5: AIMOD_NODES_LoadNodesFromPK3 called with"
			" bad params\n" );
		return qfalse;
	}

	// Read in a string the size of the mod name
	// (+1 is because all strings end in hex '00')
	trap_FS_Read( &nm, strlen( name) + 1, f );
	if ( Q_stricmp( nm, name) != 0 )
	{
		// If the file doesn't start with the mod name, it's not a valid .nod
		// file, so abort
		G_Printf( "^1*** ^3WARNING^5: Incompatible or corrupt node file.\n", 
			mapname );
		return qfalse;
	}

	// Read and make sure the version is the same
	trap_FS_Read( &version, sizeof(float), f );
	if ( version != NOD_VERSION )
	{
		G_Printf( "^1*** ^3WARNING^5: Old node file detected.\n", mapname );
		return qfalse;
	}

	// Make sure the file is for the current map
	trap_FS_Read( &map, strlen( mapname ) + 1, f );
	if ( Q_stricmp( map, mapname) != 0 )
	{
		G_Printf( "^1*** ^3WARNING^5: Node file is not for this map!\n",
			mapname );
		//G_Printf( "^1*** ^3       ^5: %s != %s\n", mapname, map );
		return qfalse;
	}

	// Read in the number of nodes in the map
	trap_FS_Read( &numberNodes, sizeof(short int), f );
	// Loop through all the nodes
	for ( i = 0; i < numberNodes; i++ )
	{
		// Read in all the node info stored in the file
		trap_FS_Read( &vec, sizeof(vec3_t), f );
		trap_FS_Read( &flags, sizeof(int), f );
		trap_FS_Read( objNum, sizeof(short int) * 3, f );
		trap_FS_Read( &objFlags, sizeof(short int), f );
		trap_FS_Read( &numLinks, sizeof(short int), f );

		// Add the node
		Load_AddNode( vec, flags, objNum, objFlags );

		// Loop through all of the links and read the data
		for ( j = 0; j < numLinks; j++ )
		{
			trap_FS_Read( &target, sizeof(short int), f );
			trap_FS_Read( &fl2, sizeof(short int), f );
			// Add any links
			ConnectNodes( i, target, fl2 );
		}

		// Set node objective flags..
		AIMOD_NODES_SetObjectiveFlags( i );
	}

	trap_FS_Read( &fix_aas_nodes, sizeof(short int), f );

	G_Printf( "^1*** ^3%s^5: Successfully loaded %i waypoints from waypoint "
		"file ^7nodes/%s.bwp^5.\n", GAME_VERSION, number_of_nodes, mapname );

	nodes_loaded = qtrue;
	if ( fix_aas_nodes )
	{
		AIMOD_NODES_FixAASNodes();
	}

	return qtrue;

}

/* */
void
AIMOD_NODES_LoadNodes ( void )
{

	qboolean		success = qfalse;
	fileHandle_t	f;
	char			mapname[MAX_QPATH];

	// Set up bot goal entity lists...
	BotMP_Update_Goal_Lists();

	// Get map name
	trap_Cvar_VariableStringBuffer( "mapname", mapname, sizeof(mapname) );

	// try to open the node file in the pk3 for reading, return false on error
	trap_FS_FOpenFile( va( "nodes/%s.bwp", mapname), &f, FS_READ );
	if ( f )
	{
		success = AIMOD_NODES_LoadNodesFromPK3( f, mapname );
		//close the file
		trap_FS_FCloseFile( f );
	}
	else 
	{
		// No node file in a pk3 file found (is sv_pure on?)
		G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.bwp^3 failed^5!!"
			"!\n", mapname );
	}

	if( ! success ) {
		G_Printf( "^1*** ^3       ^5  You need to make new bot routes for "
			"this map.\n", mapname );
		G_Printf( "^1*** ^3       ^5  Bots will move randomly for this map.\n",
			mapname );
	}
	return;
}


/* */
void
RemoveDoorsAndDestroyablesForSave ( void )
{
	int i;
	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		gentity_t	*ent = &g_entities[i];
		if ( ent->s.eType == ET_INVISIBLE || ent->entstate == STATE_INVISIBLE || (ent->s.powerups & STATE_INVISIBLE) )
		{
			G_FreeEntity( &g_entities[i] );
			continue;
		}

		if ( !ent || !ent->classname || !ent->classname[0] )
		{
			continue;
		}

		if ( !strcmp( ent->classname, "func_door") || !strcmp( ent->classname, "func_door_rotating") )
		{
			G_FreeEntity( &g_entities[i] );
		}
		else if ( !strcmp( ent->classname, "func_explosive") )
		{
			G_FreeEntity( &g_entities[i] );
		}
	}
}


/* */
void
AIMOD_NODES_RemoveTeamNodes ( void )
{
	int i;
	for ( i = 0; i < number_of_nodes; i++ )
	{
		if ( nodes[i].type & NODE_AXIS_UNREACHABLE || nodes[i].type & NODE_ALLY_UNREACHABLE )
		{
			nodes[i].type = NODE_MOVE;
		}
	}
}

/*///////////////////////////////////////////////////
SaveNodes
saves all the current nodes and links to a file
default filename is <mapname>.nod
This function depends on the file format for .nod files
and the node_t structure format
*/


///////////////////////////////////////////////////
void
AIMOD_NODES_SaveNodes ( void )
{
	fileHandle_t	f;
	int				i;
	short int		j;
	float			version = NOD_VERSION;	//version is 1.0 for now
	char			name[] = BOT_MOD_NAME;
	vmCvar_t		mapname;
	char			map[64] = "";
	char			filename[60];
	short			num_nodes = number_of_nodes;
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
	trap_FS_FOpenFile( va( "nodes/%s.bwp", filename), &f, FS_WRITE );
	if ( !f )
	{
		G_Printf( "^1*** ^3ERROR^5: Error opening node file ^7nodes/%s.bwp^5!!!\n", filename );
		return;
	}

	for ( i = 0; i < num_nodes; i++ )
	{
		for ( j = 0; j < MAX_NODELINKS; j++ )
		{
			nodes[i].links[j].targetNode = INVALID;
			nodes[i].links[j].cost = 999999;
			nodes[i].links[j].flags = 0;
			nodes[i].objectNum[0] = nodes[i].objectNum[1] = nodes[i].objectNum[2] = ENTITYNUM_NONE;
		}
	}

	RemoveDoorsAndDestroyablesForSave();
	num_nodes = number_of_nodes;

	// Resolve paths
	//-------------------------------------------------------
	AIMOD_MAPPING_MakeLinks();
	num_nodes = number_of_nodes;
	for ( i = 0; i < num_nodes; i++ )
	{

		// Set node objective flags..
		AIMOD_NODES_SetObjectiveFlags( i );
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );	//get the map name
	strcpy( map, mapname.string );
	trap_FS_Write( &name, strlen( name) + 1, f );								//write the mod name to the file
	trap_FS_Write( &version, sizeof(float), f );								//write the version of this file
	trap_FS_Write( &map, strlen( map) + 1, f );									//write the map name
	trap_FS_Write( &num_nodes, sizeof(short int), f );							//write the number of nodes in the map
	if ( nodes[num_nodes].type & NODE_AXIS_UNREACHABLE )
	{
		nodes[num_nodes].objTeam |= TEAM_ALLIES;
	}
	else if ( nodes[num_nodes].type & NODE_ALLY_UNREACHABLE )
	{
		nodes[num_nodes].objTeam |= TEAM_AXIS;
	}

	for ( i = 0; i < num_nodes; i++ )											//loop through all the nodes
	{

		//write all the node data to the file
		//trap_FS_Write(&(nodes[i].num), sizeof (short int), f);
		trap_FS_Write( &(nodes[i].origin), sizeof(vec3_t), f );
		trap_FS_Write( &(nodes[i].type), sizeof(int), f );
		trap_FS_Write( &(nodes[i].objectNum), sizeof(short int) * 3, f );
		trap_FS_Write( &(nodes[i].objFlags), sizeof(short int), f );
		trap_FS_Write( &(nodes[i].enodenum), sizeof(short int), f );
		for ( j = 0; j < nodes[i].enodenum; j++ )
		{
			trap_FS_Write( &(nodes[i].links[j].targetNode), sizeof(short int), f );
			trap_FS_Write( &(nodes[i].links[j].flags), sizeof(short int), f );
		}
	}

	trap_FS_FCloseFile( f );													//close the file
	G_Printf( "^1*** ^3%s^5: Successfully saved node file ^7nodes/%s.bwp^5.\n", GAME_VERSION, filename );
}

vmCvar_t	bot_auto_waypoint;
extern void AIMOD_MAPPING_AddNode ( int clientNum, int teamNum );


/* */
void
WaypointThink ( gentity_t *ent )
{
	float		distance, min_distance;
	int			i;
	qboolean	force_node = qfalse;
	if ( ent->r.svFlags & SVF_BOT )
	{
		return;
	}

	if ( bot_auto_waypoint.integer )					// is auto waypoint on?
	{
		vec3_t	up;

		// find the distance from the last used waypoint
		distance = VectorDistance( ent->last_added_waypoint, ent->r.currentOrigin );
		if ( distance > 96 )
		{
			min_distance = 9999.0;

			// check that no other reachable waypoints are nearby...
			for ( i = 0; i < number_of_nodes; i++ )
			{
				distance = VectorDistance( nodes[i].origin, ent->r.currentOrigin );
				if ( distance < 256 )
				{
					if ( OrgVisible( ent->r.currentOrigin, nodes[i].origin, ent->client->ps.clientNum) )
					{
						if ( distance < min_distance )
						{
							min_distance = distance;
						}
					}
				}
			}

			// make sure nearest waypoint is far enough away...
			if ( min_distance >= 96 )
			{
				force_node = qtrue;
			}
		}

		VectorCopy( ent->r.currentOrigin, up );
		up[2] += 16;
		if ( force_node )
		{
			VectorCopy( ent->r.currentOrigin, ent->last_good_node_point );
			VectorCopy( ent->last_good_node_point, ent->last_added_waypoint );
			AIMOD_MAPPING_AddNode( ent->s.number, -1 ); // place a waypoint here
			return;
		}

		if ( OrgVisible( up, ent->last_added_waypoint, ent->s.number) )
		{												// this spot is better then our last selection..
			VectorCopy( ent->r.currentOrigin, ent->last_good_node_point );
		}

		if ( !OrgVisible( up, ent->last_added_waypoint, ent->s.number) )
		{												// We need to move the client back for a moment to make the point... So...
			vec3_t	original;
			VectorCopy( ent->r.currentOrigin, original );
			G_SetOrigin( ent, ent->last_good_node_point );
			VectorCopy( ent->last_good_node_point, ent->r.currentOrigin );
			VectorCopy( ent->last_good_node_point, ent->client->ps.origin );
			AIMOD_MAPPING_AddNode( ent->s.number, -1 ); // place a waypoint here
			G_SetOrigin( ent, original );
			VectorCopy( original, ent->r.currentOrigin );
			VectorCopy( original, ent->client->ps.origin );
			VectorCopy( ent->last_good_node_point, ent->last_added_waypoint );
			VectorCopy( original, ent->last_good_node_point );
		}
	}

	/*	float distance, min_distance;
	int i;
	qboolean force_node = qfalse;

	if (bot_auto_waypoint.integer)  // is auto waypoint on?
	{
		vec3_t up;

		// find the distance from the last used waypoint
		distance = VectorDistance(ent->last_added_waypoint, ent->r.currentOrigin);

		if (distance > 96)
		{
			min_distance = 9999.0;

	        // check that no other reachable waypoints are nearby...
			for (i=0; i < number_of_nodes; i++)
			{
				distance = VectorDistance(nodes[i].origin, ent->r.currentOrigin);

				if (distance < 256)
				{
					if (OrgVisible(ent->r.currentOrigin, nodes[i].origin, ent->client->ps.clientNum))
					{
						if (distance < min_distance)
							min_distance = distance;
					}
				}
			}

			// make sure nearest waypoint is far enough away...
			if (min_distance >= 96)
				 force_node = qtrue;
		}

		VectorCopy(ent->r.currentOrigin, up);
		up[2]+=16;

		if (OrgVisibleBox(up, ent->r.mins, ent->r.maxs, ent->last_added_waypoint, ent->s.number) || force_node )
		{// this spot is better then our last selection..
			VectorCopy(ent->r.currentOrigin, ent->last_good_node_point);
		}

		if (!ent->routing_pathsize || ent->routing_pathsize < 0 || ent->routing_pathsize > (MAX_PATHLIST_NODES/4)*1)
			ent->routing_pathsize = 0;

		if (!OrgVisibleBox(up, ent->r.mins, ent->r.maxs, ent->last_added_waypoint, ent->s.number) || force_node)
		{// Time to add our last_good_node_point as a node... // will delay this and make it work in chains ( > 1 players )
			VectorCopy(ent->last_added_waypoint, ent->routing_pathlist[ent->routing_pathsize]);
			ent->routing_pathsize++;

			if (ent->routing_pathsize > MAX_PATHLIST_NODES/4)
			{// Store our route...
				vec3_t stored_origin;
				int i = 0;

				VectorCopy(ent->r.currentOrigin, stored_origin);

				for (i = 0; i < ent->routing_pathsize; i++)
				{
					// We need to move the client back for a moment to make the point... So...
					G_SetOrigin(ent, ent->routing_pathlist[i]);
					VectorCopy(ent->routing_pathlist[i], ent->r.currentOrigin);
					VectorCopy(ent->routing_pathlist[i], ent->client->ps.origin);
					AIMOD_MAPPING_AddNode( ent->s.number );  // place a waypoint here
				}
				
				// And return straight away to our real position... As if we never moved hehehe...
				G_SetOrigin(ent, stored_origin);
				VectorCopy(stored_origin, ent->r.currentOrigin);
				VectorCopy(stored_origin, ent->client->ps.origin);
			}
		}
	}
*/
}
#endif
