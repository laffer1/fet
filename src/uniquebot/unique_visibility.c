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
#include "../game/g_local.h"

//
// This file a prototype is for faster visibility checking for bots and NPCs.
//
// This code should reduce the ammount of the CPU hit when we do any Bot/NPC
// related vis checking, by storing all possiblities for later use.
//
#ifdef __BOT__
#ifdef __BOT_FAST_VISIBILITY__
extern node_t	nodes[MAX_NODES];
typedef struct visibilitylink_s				// Visibility Link Structure
{
	short int	targetNode;					// Target Node
} visibilitylink_t;							// Visibility Link Typedef
typedef struct visibility_s					// Visibility Structure
{
	short int			enodenum;			// Mostly just number of players around this node
	visibilitylink_t	links[MAX_NODES];	// Store all visible links
} visibility_t;								// Visibility Typedef
visibility_t	visibilities[MAX_NODES];

//special node visibility check for bot linkages..
//0 = wall in way
//1 = player or no obstruction
//2 = useable door in the way.
//3 = door entity in the way.
extern int		NodeVisible ( vec3_t org1, vec3_t org2, int ignore );


/* */
void
AIMOD_VISIBILITY_CreateVisLinks ( int node )
{
	int loop = 0;
	int linknum = 0;
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		vec3_t	from, to;
		int		visCheck = 0;
		VectorCopy( nodes[node].origin, from );
		from[2] += 40;
		VectorCopy( nodes[loop].origin, to );
		to[2] += 40;
		visCheck = NodeVisible( from, to, -1 );

		//0 = wall in way
		//1 = player or no obstruction
		//2 = useable door in the way.
		//3 = door entity in the way.
		if ( visCheck == 1 )
		{
			visibilities[node].links[linknum].targetNode = loop;
			linknum++;
			if ( linknum > MAX_NODES )
			{
				linknum--;
				break;
			}
		}
	}

	visibilities[node].enodenum = linknum;
	G_Printf( "^4*** ^3%s^5: Created ^7%i^5 visibility links for node number ^7%i^5.\n", GAME_VERSION, linknum, node );
}


/* */
void
AIMOD_VISIBILITY_CreateAllVisLinks ( void )
{	// Call this procedure on map load for now...
	int loop = 0;
	for ( loop = 0; loop < number_of_nodes; loop++ )
	{
		AIMOD_VISIBILITY_CreateVisLinks( loop );
	}
}

/*///////////////////////////////////////////////////
AIMOD_VISIBILITY_SaveVisibilities
saves all the current nodes and links to a file
default filename is <mapname>.nod
This function depends on the file format for .nod files
and the node_t structure format
*/


///////////////////////////////////////////////////
void
AIMOD_VISIBILITY_SaveVisibilities ( void )
{
	fileHandle_t	f;
	int				i;
	short int		j;
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
	trap_FS_FOpenFile( va( "nodes/%s.vis", filename), &f, FS_WRITE );
	if ( !f )
	{
		G_Printf( "^1*** ^3ERROR^5: Error opening visibility file ^7nodes/%s.vis^5!!!\n", filename );
		return;
	}

	for ( i = 0; i < number_of_nodes; i++ )
	{
		for ( j = 0; j < MAX_NODELINKS; j++ )
		{
			visibilities[i].links[j].targetNode = INVALID;
		}
	}

	// Resolve visibilities.
	//-------------------------------------------------------
	AIMOD_VISIBILITY_CreateAllVisLinks();
	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM );	//get the map name
	strcpy( map, mapname.string );
	trap_FS_Write( &num_nodes, sizeof(short int), f );							//write the number of nodes in the map
	for ( i = 0; i < number_of_nodes; i++ )										//loop through all the nodes
	{

		//write all the node data to the file
		trap_FS_Write( &(visibilities[i].enodenum), sizeof(short int), f );
		for ( j = 0; j < visibilities[i].enodenum; j++ )
		{
			trap_FS_Write( &(visibilities[i].links[j].targetNode), sizeof(short int), f );
		}
	}

	trap_FS_FCloseFile( f );													//close the file
	G_Printf( "^1*** ^3%s^5: Successfully saved visibility file ^7nodes/%s.vis^5.\n", GAME_VERSION, filename );
}

/*/////////////////////////////////////////////
AIMOD_VISIBILITY_LoadVisibilities
Initializes the nodes for a map from a file
The file must be in a .pk3 file in the mod folder
This function is completely dependent on the file format for the node files
as well as the format of the node_t structure
*/


/////////////////////////////////////////////
void
AIMOD_VISIBILITY_LoadVisibilities ( void )
{
	fileHandle_t	f;
	int				i, j;
	char			filename[60];
	vmCvar_t		mapname;
	short int		numLinks;
	short int		target;
	char			nm[64] = "";
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
	//open the visibility file for reading, return false on error
	trap_FS_FOpenFile( va( "nodes/%s.vis", filename), &f, FS_READ );
	if ( !f )
	{
		if ( number_of_nodes > 0 )
		{	// Calculate them and save it!
			AIMOD_VISIBILITY_SaveVisibilities();
		}
		else
		{
			G_Printf( "^1*** ^3WARNING^5: Reading from ^7nodes/%s.vis^3 failed^5!!!\n", filename );
			G_Printf( "^1*** ^3       ^5  You need to make bot routes for this map.\n" );
			G_Printf( "^1*** ^3       ^5  Using built-in ET visibility checking.\n" );
		}

		return;
	}

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_ROM | CVAR_SERVERINFO );	//get the map name
	strcpy( mp, mapname.string );
	trap_FS_Read( &numberNodes, sizeof(short int), f );							//read in the number of nodes in the map
	for ( i = 0; i < numberNodes; i++ )											//loop through all the nodes
	{
		trap_FS_Read( &numLinks, sizeof(short int), f );
		visibilities[i].enodenum = numLinks;

		//loop through all of the links and read the data
		for ( j = 0; j < numLinks; j++ )
		{
			trap_FS_Read( &target, sizeof(short int), f );
			visibilities[i].links[j].targetNode = target;
		}
	}

	trap_FS_FCloseFile( f );													//close the file
	G_Printf( "^1*** ^3%s^5: Successfully loaded visibility file ^7nodes/%s.vis^5.\n", GAME_VERSION, filename );
}


/* */
qboolean
AIMOD_VISIBILITY_CheckVisibility ( int from, int to )
{
	int i = 0;
	for ( i = 0; i < visibilities[from].enodenum; i++ )
	{
		if ( to == visibilities[from].links[i].targetNode )
		{	// Is visible!
			return ( qtrue );
		}
	}

	return ( qfalse );
}


/* */
int
AIMOD_VISIBILITY_ClosestNode ( gentity_t *self )
{
	int		closest = -1;

	//float closestdist = 99999.9f;
	float	closestdist = 64.0f;
	int		i = 0;
	for ( i = 0; i < number_of_nodes; i++ )
	{
		float	dist = VectorDistance( nodes[i].origin, self->r.currentOrigin );

		//		if ( dist > 32/*64*/)
		//			continue;
		if ( dist >= closestdist )
		{
			continue;
		}

		closest = i;
		closestdist = dist;
	}

	return ( closest );
}
#endif //__BOT_FAST_VISIBILITY__
#endif //__BOT__
