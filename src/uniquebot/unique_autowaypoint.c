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
extern void		trap_Printff ( const char *fmt );
extern int		number_of_nodes;
extern node_t	nodes[MAX_NODES];
extern void		RemoveDoorsAndDestroyablesForSave ( void );
extern qboolean Load_AddNode ( vec3_t origin, int fl, short int *ents, int objFl );
extern void		AIMOD_NODES_SaveNodes ( void );
extern int		NodeVisible ( vec3_t org1, vec3_t org2, int ignore );
extern void		AIMOD_NODES_SetObjectiveFlags ( int node );
#define BOT_MOD_NAME	"aimod"
#define NOD_VERSION		1.0f
int				aw_num_nodes = 0;


/* */
void
AIMOD_NODES_SaveNodes_Autowaypointed ( void )
{
	fileHandle_t	f;
	int				i;
	short int		j;
	float			version = NOD_VERSION;										//version is 1.0 for now
	char			name[] = BOT_MOD_NAME;
	vmCvar_t		mapname;
	char			map[64] = "";
	char			filename[60];
	short			num_nodes = number_of_nodes;
	aw_num_nodes = number_of_nodes;
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
	//AIMOD_MAPPING_MakeLinks();
	num_nodes = aw_num_nodes;
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
	{
		short int	fix = 1;
		trap_FS_Write( &fix, sizeof(short int), f );							//write the version of this file
	}

	trap_FS_FCloseFile( f );													//close the file
	G_Printf( "^1*** ^3%s^5: Successfully saved node file ^7nodes/%s.bwp^5.\n", GAME_VERSION, filename );
}

#define MAX_TEMP_SIZE	MAX_MAP_SIZE * 8										//10//8

/*
#define area_scatter_distance 769
#define waypoint_scatter_distance 65
*/
#define area_scatter_distance		513
#define waypoint_scatter_distance	65

//
// The utilities for faster (then vector/float) integer maths...
//
typedef long int	intvec_t;
typedef intvec_t	intvec3_t[3];


/* */
void
intToVectorCopy ( const intvec3_t in, vec3_t out )
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}


/* */
void
intVectorCopy ( const intvec3_t in, intvec3_t out )
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}


/* */
intvec_t
intVectorLength ( const intvec3_t v )
{
	return ( sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2]) );
}


/* */
void
intVectorSubtract ( const intvec3_t veca, const intvec3_t vecb, intvec3_t out )
{
	out[0] = veca[0] - vecb[0];
	out[1] = veca[1] - vecb[1];
	out[2] = veca[2] - vecb[2];
}


/* */
long int
intVectorDistance ( intvec3_t v1, intvec3_t v2 )
{
	intvec3_t	dir;
	intVectorSubtract( v2, v1, dir );
	return ( intVectorLength( dir) );
}


//
// Now the actual number crunching and visualizations...

//
void
AIMod_GetMapBounts ( vec3_t mapMins, vec3_t mapMaxs )
{
	int		i;
	float	startx = -MAX_MAP_SIZE, starty = -MAX_MAP_SIZE, startz = -MAX_MAP_SIZE;
	float	highest_z_point = -MAX_MAP_SIZE;

	//
	// Z
	//
	i = 0;
	while ( startx < MAX_MAP_SIZE )
	{
		while ( starty < MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[2] += ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[2] < mapMins[2] )
			{
				mapMins[2] = tr.endpos[2];
				continue;
			}

			if ( tr.startsolid || tr.allsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA /*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				if ( tr.contents & CONTENTS_STRUCTURAL )
				{
					G_Printf( "CONTENTS_STRUCTURAL\n" );
				}

				if ( tr.contents & CONTENTS_DETAIL )
				{
					G_Printf( "CONTENTS_DETAIL\n" );
				}

				if ( tr.contents & CONTENTS_NODROP )
				{
					G_Printf( "CONTENTS_NODROP\n" );
				}

				if ( tr.contents & CONTENTS_STRUCTURAL )
				{
					G_Printf( "CONTENTS_STRUCTURAL\n" );
				}

				if ( tr.contents & CONTENTS_DONOTENTER_LARGE )
				{
					G_Printf( "CONTENTS_DONOTENTER_LARGE\n" );
				}

				if ( tr.contents & CONTENTS_DONOTENTER )
				{
					G_Printf( "CONTENTS_DONOTENTER\n" );
				}

				if ( tr.contents & CONTENTS_MONSTERCLIP )
				{
					G_Printf( "CONTENTS_MONSTERCLIP\n" );
				}

				if ( tr.contents & CONTENTS_PLAYERCLIP )
				{
					G_Printf( "CONTENTS_PLAYERCLIP\n" );
				}

				if ( tr.contents & CONTENTS_MOVER )
				{
					G_Printf( "CONTENTS_MOVER\n" );
				}

				if ( tr.contents & CONTENTS_MISSILECLIP )
				{
					G_Printf( "CONTENTS_MISSILECLIP\n" );
				}

				if ( tr.contents & CONTENTS_SLIME )
				{
					G_Printf( "CONTENTS_SLIME\n" );
				}

				if ( tr.contents & CONTENTS_LAVA )
				{
					G_Printf( "CONTENTS_LAVA\n" );
				}

				if ( tr.contents & CONTENTS_SOLID )
				{
					G_Printf( "CONTENTS_SOLID\n" );
				}

				continue;
			}

			starty += 256;
		}

		startx += 256;
		starty = -MAX_MAP_SIZE;
	}

	mapMins[2] += 16;
	startx = MAX_MAP_SIZE;
	starty = MAX_MAP_SIZE;
	startz = MAX_MAP_SIZE;
	i = 0;
	while ( startx > -MAX_MAP_SIZE )
	{
		while ( starty > -MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[2] -= ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[2] > mapMaxs[2] )
			{
				mapMaxs[2] = tr.endpos[2];
				continue;
			}

			if ( tr.startsolid || tr.allsolid )
			{
				continue;
			}

			starty -= 256;
		}

		startx -= 256;
		starty = MAX_MAP_SIZE;
	}

	mapMaxs[2] -= 16;

	//
	// X
	//
	startx = -MAX_MAP_SIZE;
	starty = -MAX_MAP_SIZE;
	startz = mapMins[2];
	i = 0;
	while ( startz < mapMaxs[2] )
	{
		while ( starty < MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[0] += ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[0] < mapMins[0] )
			{
				mapMins[0] = tr.endpos[0];
				continue;
			}

			if ( tr.startsolid || tr.allsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA /*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				continue;
			}

			starty += 256;
		}

		startz += 256;
		starty = -MAX_MAP_SIZE;
	}

	mapMins[0] += 16;
	startx = MAX_MAP_SIZE;
	starty = MAX_MAP_SIZE;
	startz = mapMaxs[2];
	i = 0;
	while ( startz > mapMins[2] )
	{
		while ( starty > -MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[0] -= ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[0] > mapMaxs[0] )
			{
				mapMaxs[0] = tr.endpos[0];
				continue;
			}

			if ( tr.startsolid || tr.allsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA /*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				continue;
			}

			starty -= 256;
		}

		startz -= 256;
		starty = MAX_MAP_SIZE;
	}

	mapMaxs[0] -= 16;

	//
	// Y
	//
	startx = -MAX_MAP_SIZE;
	starty = -MAX_MAP_SIZE;
	startz = mapMins[2];
	i = 0;
	while ( startz < mapMaxs[2] )
	{
		while ( startx < MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[1] += ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[1] < mapMins[1] )
			{
				mapMins[1] = tr.endpos[1];
				continue;
			}

			if ( tr.startsolid || tr.allsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA /*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				continue;
			}

			startx += 256;
		}

		startz += 256;
		startx = -MAX_MAP_SIZE;
	}

	mapMins[1] += 16;
	startx = MAX_MAP_SIZE;
	starty = MAX_MAP_SIZE;
	startz = mapMaxs[2];
	i = 0;
	while ( startz > mapMins[2] )
	{
		while ( startx > -MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[1] -= ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[1] > mapMaxs[1] )
			{
				mapMaxs[1] = tr.endpos[1];
				continue;
			}

			if ( tr.startsolid || tr.allsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA /*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				continue;
			}

			startx -= 256;
		}

		startz -= 256;
		startx = MAX_MAP_SIZE;
	}

	mapMaxs[1] -= 16;

	//
	// Refine Z Top Point to highest ground height!
	//
	startx = mapMaxs[0] - 32;
	starty = mapMaxs[1] - 32;
	startz = mapMaxs[2] - 32;
	highest_z_point = mapMins[2];
	i = 0;
	while ( startx > mapMins[2] )
	{
		while ( starty > mapMins[1] )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[2] -= ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.startsolid || tr.allsolid )
			{
				starty -= 128;
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA /*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				starty -= 128;
				continue;
			}

			if ( tr.endpos[2] > highest_z_point )
			{
				highest_z_point = tr.endpos[2];
				continue;
			}

			starty -= 64 /*128*/ ;
		}

		startx -= 64 /*128*/ ;
		starty = mapMaxs[1];
	}

	if ( highest_z_point <= mapMins[2] )
	{
		highest_z_point = mapMaxs[2] - 32;
	}

	if ( highest_z_point <= mapMins[2] + 128 )
	{
		highest_z_point = mapMaxs[2] - 32;
	}

	mapMaxs[2] = highest_z_point;
}

int			BotObjectives[MAX_GENTITIES];
int			num_bot_objectives;
int			BotGoals[MAX_GENTITIES];
int			num_bot_goals;
extern void BotMP_Update_Goal_Lists ( void );


/* */
long int
AIMod_Generate_Areas ( vec3_t mapMins, vec3_t mapMaxs, intvec3_t arealist[MAX_MAP_SIZE] )
{
	int			i;
	float		startx = -MAX_MAP_SIZE, starty = -MAX_MAP_SIZE, startz = -MAX_MAP_SIZE;
	int			areas = 0;
	int			temp_areas = 0;
	intvec3_t	temp_arealist[MAX_MAP_SIZE];
	BotMP_Update_Goal_Lists();

	//
	// OK.. Now we have the map bounds... Generate a set of temp nodes...
	//
	startx = mapMins[0];
	starty = mapMins[1];
	startz = mapMins[2];
	i = 0;
	trap_Printf( va( "Generating areas list.\n") );
	while ( startx < mapMaxs[0] )
	{
		while ( starty < mapMaxs[1] )
		{
			while ( startz < mapMaxs[2] )
			{
				trace_t tr;
				vec3_t	org1;
				vec3_t	org2;
				VectorSet( org1, startx, starty, startz );
				VectorSet( org2, startx, starty, startz );
				org2[2] -= ( 65536 * 2 );
				trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
				if ( tr.startsolid || tr.allsolid )
				{
					startz += area_scatter_distance * 2;
					continue;
				}

				if
				(
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
					tr.contents & CONTENTS_LAVA /*|| tr.contents & CONTENTS_SOLID*/
				)
				{
					startz += area_scatter_distance * 2;
					continue;
				}

				VectorSet( temp_arealist[i], startx, starty, startz );
				startz += area_scatter_distance;
				i++;
			}

			startz = mapMins[2];
			starty += area_scatter_distance;
		}

		starty = mapMins[1];
		startx += area_scatter_distance;
	}

	//
	// Now sort the nodes a bit...
	//
	temp_areas = i;
	temp_areas--;
	i = 0;
	trap_Printf( va( "Height-checking area list.\n") );
	for ( i = 0; i <= temp_areas; i++ )
	{											// Lower them all to ground level...
		trace_t tr;
		vec3_t	org1;
		vec3_t	org2;
		intToVectorCopy( temp_arealist[i], org1 );
		intToVectorCopy( temp_arealist[i], org2 );
		org2[2] -= ( 65536 * 2 );
		trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
		if ( tr.startsolid || tr.allsolid )
		{
			VectorSet( temp_arealist[i], 0, 0, 0 );
			continue;
		}

		if
		(
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
			tr.contents & CONTENTS_LAVA			/*|| tr.contents & CONTENTS_SOLID*/
		)
		{
			VectorSet( temp_arealist[i], 0, 0, 0 );
			continue;
		}

		if ( tr.endpos[2] + 32 < mapMins[2] )
		{
			VectorSet( temp_arealist[i], 0, 0, 0 );
			continue;
		}

		temp_arealist[i][2] = tr.endpos[2] + 32 /*8*/ ;
	}

	for ( i = 0; i <= temp_areas; i++ )
	{
		if ( temp_arealist[i][0] == 0 && temp_arealist[i][1] == 0 && temp_arealist[i][2] == 0 )
		{										// Removed this one...
			continue;
		}
		else
		{
			intVectorCopy( temp_arealist[i], arealist[areas] );

			//G_Printf("%i %i %i\n", arealist[areas][0], arealist[areas][1], arealist[areas][2]);
			areas++;
		}
	}

	for ( i = 0; i < num_bot_objectives; i++ )
	{
		vec3_t		org;
		gentity_t	*goal = &g_entities[BotObjectives[i]];
		VectorCopy( goal->s.origin, org );
		org[2] += 16;
		VectorCopy( org, arealist[areas] );
		areas++;
	}

	for ( i = 0; i < num_bot_goals; i++ )
	{
		vec3_t		org;
		gentity_t	*goal = &g_entities[BotGoals[i]];
		VectorCopy( goal->s.origin, org );
		org[2] += 16;
		VectorCopy( org, arealist[areas] );
		areas++;
	}

	for ( i = 0; i < MAX_GENTITIES; i++ )
	{											// Add spawnpoints...
		vec3_t		org;
		gentity_t	*goal = &g_entities[i];
		if ( !goal )
		{
			continue;
		}

		if
		(
			Q_stricmp( goal->classname, "CTF_redspawn") &&
			Q_stricmp( goal->classname, "CTF_bluespawn") &&
			Q_stricmp( goal->classname, "team_CTF_bluespawn") &&
			Q_stricmp( goal->classname, "team_CTF_redspawn")
		)
		{
			continue;
		}

		VectorCopy( goal->s.origin, org );
		org[2] += 16;
		VectorCopy( org, arealist[areas] );
		areas++;
	}

	return ( areas );
}


/* */
long int
AIMod_Generate_Temp_Waypoints ( vec3_t mapMins, vec3_t mapMaxs, intvec3_t nodelist[MAX_MAP_SIZE] )
{
	int			i;
	float		startx = -MAX_MAP_SIZE, starty = -MAX_MAP_SIZE, startz = -MAX_MAP_SIZE;
	int			nodes = 0;
	int			temp_nodes = 0;
	intvec3_t	temp_nodelist[MAX_MAP_SIZE];

	//
	// OK.. Now we have the map bounds... Generate a set of temp nodes...
	//
	startx = mapMins[0];
	starty = mapMins[1];
	startz = mapMins[2];
	i = 0;

	//trap_Printf(va("Generating temporary waypoints list.\n"));
	while ( startx < mapMaxs[0] )
	{
		while ( starty < mapMaxs[1] )
		{
			while ( startz < mapMaxs[2] )
			{
				trace_t tr;
				vec3_t	org1;
				vec3_t	org2;
				VectorSet( org1, startx, starty, startz );
				VectorSet( org2, startx, starty, startz );
				org2[2] -= ( 65536 * 2 );
				trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
				if ( tr.startsolid || tr.allsolid )
				{
					startz += waypoint_scatter_distance * 2;
					continue;
				}

				if
				(
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
					tr.contents & CONTENTS_LAVA /*|| tr.contents & CONTENTS_SOLID*/
				)
				{
					startz += waypoint_scatter_distance * 2;
					continue;
				}

				VectorSet( temp_nodelist[i], startx, starty, startz );
				startz += waypoint_scatter_distance * 2;
				i++;
			}

			startz = mapMins[2];
			starty += waypoint_scatter_distance;
		}

		starty = mapMins[1];
		startx += waypoint_scatter_distance;
	}

	//
	// Now sort the nodes a bit...
	//
	temp_nodes = i;
	temp_nodes--;
	i = 0;

	//trap_Printf(va("Height-checking temporary waypoints list.\n"));
	for ( i = 0; i <= temp_nodes; i++ )
	{											// Lower them all to ground level...
		trace_t tr;
		vec3_t	org1;
		vec3_t	org2;
		intToVectorCopy( temp_nodelist[i], org1 );
		intToVectorCopy( temp_nodelist[i], org2 );
		org2[2] -= ( 65536 * 2 );
		trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
		if ( tr.startsolid || tr.allsolid )
		{
			VectorSet( temp_nodelist[i], 0, 0, 0 );
			continue;
		}

		if
		(
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
			tr.contents & CONTENTS_LAVA			/*|| tr.contents & CONTENTS_SOLID*/
		)
		{
			VectorSet( temp_nodelist[i], 0, 0, 0 );
			continue;
		}

		if ( tr.endpos[2] + 32 < mapMins[2] )
		{
			VectorSet( temp_nodelist[i], 0, 0, 0 );
			continue;
		}

		temp_nodelist[i][2] = tr.endpos[2] + 32 /*8*/ ;
	}

	for ( i = 0; i <= temp_nodes; i++ )
	{
		if ( temp_nodelist[i][0] == 0 && temp_nodelist[i][1] == 0 && temp_nodelist[i][2] == 0 )
		{										// Removed this one...
			continue;
		}
		else
		{
			intVectorCopy( temp_nodelist[i], nodelist[nodes] );
			nodes++;
		}
	}

	nodes--;
	return ( nodes );
}


/* */
long int
AIMod_Generate_Temp_Waypoints_Closer ( vec3_t mapMins, vec3_t mapMaxs, intvec3_t nodelist[MAX_MAP_SIZE] )
{
	int			i;
	float		startx = -MAX_MAP_SIZE, starty = -MAX_MAP_SIZE, startz = -MAX_MAP_SIZE;
	int			nodes = 0;
	int			temp_nodes = 0;
	intvec3_t	temp_nodelist[MAX_MAP_SIZE];

	//
	// OK.. Now we have the map bounds... Generate a set of temp nodes...
	//
	startx = mapMins[0];
	starty = mapMins[1];
	startz = mapMins[2];
	i = 0;

	//trap_Printf(va("Generating temporary waypoints list.\n"));
	while ( startx < mapMaxs[0] )
	{
		while ( starty < mapMaxs[1] )
		{
			while ( startz < mapMaxs[2] )
			{
				trace_t tr;
				vec3_t	org1;
				vec3_t	org2;
				VectorSet( org1, startx, starty, startz );
				VectorSet( org2, startx, starty, startz );
				org2[2] -= ( 65536 * 2 );
				trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
				if ( tr.startsolid || tr.allsolid )
				{
					startz += waypoint_scatter_distance * 2;
					continue;
				}

				if
				(
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
					tr.contents & CONTENTS_LAVA /*|| tr.contents & CONTENTS_SOLID*/
				)
				{
					startz += waypoint_scatter_distance /**1.25*/ /*2*/ ;
					continue;
				}

				VectorSet( temp_nodelist[i], startx, starty, startz );
				startz += waypoint_scatter_distance /**1.25*/ /*2*/ ;
				i++;
			}

			startz = mapMins[2];
			starty += waypoint_scatter_distance * 0.75;
		}

		starty = mapMins[1];
		startx += waypoint_scatter_distance * 0.75;
	}

	//
	// Now sort the nodes a bit...
	//
	temp_nodes = i;
	temp_nodes--;
	i = 0;

	//trap_Printf(va("Height-checking temporary waypoints list.\n"));
	for ( i = 0; i <= temp_nodes; i++ )
	{											// Lower them all to ground level...
		trace_t tr;
		vec3_t	org1;
		vec3_t	org2;
		intToVectorCopy( temp_nodelist[i], org1 );
		intToVectorCopy( temp_nodelist[i], org2 );
		org2[2] -= ( 65536 * 2 );
		trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
		if ( tr.startsolid || tr.allsolid )
		{
			VectorSet( temp_nodelist[i], 0, 0, 0 );
			continue;
		}

		if
		(
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
			tr.contents & CONTENTS_LAVA			/*|| tr.contents & CONTENTS_SOLID*/
		)
		{
			VectorSet( temp_nodelist[i], 0, 0, 0 );
			continue;
		}

		if ( tr.endpos[2] + 32 < mapMins[2] )
		{
			VectorSet( temp_nodelist[i], 0, 0, 0 );
			continue;
		}

		temp_nodelist[i][2] = tr.endpos[2] + 32 /*8*/ ;
	}

	for ( i = 0; i <= temp_nodes; i++ )
	{
		if ( temp_nodelist[i][0] == 0 && temp_nodelist[i][1] == 0 && temp_nodelist[i][2] == 0 )
		{										// Removed this one...
			continue;
		}
		else
		{
			intVectorCopy( temp_nodelist[i], nodelist[nodes] );
			nodes++;
		}
	}

	nodes--;
	return ( nodes );
}


/* */
long int
AIMod_Generate_Temp_Waypoints_Closest ( vec3_t mapMins, vec3_t mapMaxs, intvec3_t nodelist[MAX_MAP_SIZE] )
{
	int			i;
	float		startx = -MAX_MAP_SIZE, starty = -MAX_MAP_SIZE, startz = -MAX_MAP_SIZE;
	int			nodes = 0;
	int			temp_nodes = 0;
	intvec3_t	temp_nodelist[MAX_MAP_SIZE];

	//
	// OK.. Now we have the map bounds... Generate a set of temp nodes...
	//
	startx = mapMins[0];
	starty = mapMins[1];
	startz = mapMins[2];
	i = 0;

	//trap_Printf(va("Generating temporary waypoints list.\n"));
	while ( startx < mapMaxs[0] )
	{
		while ( starty < mapMaxs[1] )
		{
			while ( startz < mapMaxs[2] )
			{
				trace_t tr;
				vec3_t	org1;
				vec3_t	org2;
				VectorSet( org1, startx, starty, startz );
				VectorSet( org2, startx, starty, startz );
				org2[2] -= ( 65536 * 2 );
				trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
				if ( tr.startsolid || tr.allsolid )
				{
					startz += waypoint_scatter_distance * 2;
					continue;
				}

				if
				(
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
					tr.contents & CONTENTS_LAVA /*|| tr.contents & CONTENTS_SOLID*/
				)
				{
					startz += waypoint_scatter_distance /**1.25*/ /*2*/ ;
					continue;
				}

				VectorSet( temp_nodelist[i], startx, starty, startz );
				startz += waypoint_scatter_distance /**1.25*/ /*2*/ ;
				i++;
			}

			startz = mapMins[2];
			starty += waypoint_scatter_distance * 0.65;
		}

		starty = mapMins[1];
		startx += waypoint_scatter_distance * 0.65;
	}

	//
	// Now sort the nodes a bit...
	//
	temp_nodes = i;
	temp_nodes--;
	i = 0;

	//trap_Printf(va("Height-checking temporary waypoints list.\n"));
	for ( i = 0; i <= temp_nodes; i++ )
	{											// Lower them all to ground level...
		trace_t tr;
		vec3_t	org1;
		vec3_t	org2;
		intToVectorCopy( temp_nodelist[i], org1 );
		intToVectorCopy( temp_nodelist[i], org2 );
		org2[2] -= ( 65536 * 2 );
		trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
		if ( tr.startsolid || tr.allsolid )
		{
			VectorSet( temp_nodelist[i], 0, 0, 0 );
			continue;
		}

		if
		(
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
			tr.contents & CONTENTS_LAVA			/*|| tr.contents & CONTENTS_SOLID*/
		)
		{
			VectorSet( temp_nodelist[i], 0, 0, 0 );
			continue;
		}

		if ( tr.endpos[2] + 32 < mapMins[2] )
		{
			VectorSet( temp_nodelist[i], 0, 0, 0 );
			continue;
		}

		temp_nodelist[i][2] = tr.endpos[2] + 32 /*8*/ ;
	}

	for ( i = 0; i <= temp_nodes; i++ )
	{
		if ( temp_nodelist[i][0] == 0 && temp_nodelist[i][1] == 0 && temp_nodelist[i][2] == 0 )
		{										// Removed this one...
			continue;
		}
		else
		{
			intVectorCopy( temp_nodelist[i], nodelist[nodes] );
			nodes++;
		}
	}

	nodes--;
	return ( nodes );
}


/* */
long int
AIMod_Prune_Nodes ( intvec3_t temp_nodelist[MAX_MAP_SIZE], long int nodes, intvec3_t out_nodes[MAX_MAP_SIZE] )
{
	int			i, j;
	qboolean	found = qfalse;
	long int	out_total = 0;
	int			pruned = 0;
	for ( i = 0; i <= nodes; i++ )
	{			// Find any that have the full number of neighbours, and prune all the extras around them...
		// Init found flag...
		int found_count = 0;
		int found_list[MAX_MAP_SIZE];
		found = qfalse;
		for ( j = 0; j <= nodes; j++ )
		{		// Lower them all to ground level...
			if ( i == j )
			{
				continue;
			}

			if
			(
				temp_nodelist[i][0] == temp_nodelist[j][0] &&
				temp_nodelist[i][1] == temp_nodelist[j][1] &&
				temp_nodelist[i][2] == temp_nodelist[j][2]
			)
			{	// Removed this one...
				VectorSet( temp_nodelist[j], 0, 0, 0 );
				continue;
			}

			if ( temp_nodelist[j][0] == 0 && temp_nodelist[j][1] == 0 && temp_nodelist[j][2] == 0 )
			{	// Removed this one...
				continue;
			}

			if ( intVectorDistance( temp_nodelist[i], temp_nodelist[j]) <= (16 /*waypoint_scatter_distance*/ ) )
			{
				found_list[found_count] = j;
				found_count++;
			}
		}

		if ( found_count >= 8 )
		{
			int z;
			for ( z = 0; z < found_count; z++ )
			{
				VectorSet( temp_nodelist[found_list[z]], 0, 0, 0 );
			}
		}
	}

	for ( i = 0; i <= nodes; i++ )
	{			// Sort them into the final array...
		// Init found flag...
		found = qfalse;
		for ( j = 0; j < out_total; j++ )
		{		// Lower them all to ground level...
			intvec3_t	nullNode;
			VectorSet( nullNode, 0, 0, 0 );
			if
			(
				(temp_nodelist[i][0] == 0 && temp_nodelist[i][1] == 0 && temp_nodelist[i][2] == 0) ||
				intVectorDistance( temp_nodelist[i], nullNode) <= 8
			)
			{	// Remove this one...
				found = qtrue;
				break;
			}

			if ( intVectorDistance( temp_nodelist[i], out_nodes[j]) <= waypoint_scatter_distance )
			{
				found = qtrue;
				break;
			}
		}

		if ( found )
		{
			pruned++;
			continue;
		}

		intVectorCopy( temp_nodelist[i], out_nodes[out_total] );
		out_total++;
	}

	out_total--;

	//trap_Printf(va("Pruned %i waypoints. There are %i sorted waypoints.\n", pruned, out_total));
	return ( out_total );
}


/* */
int
AIMod_Advanced_Prune_Nodes ( intvec3_t sort_nodes[MAX_MAP_SIZE], long int sort_total, vec3_t final_nodes[MAX_MAP_SIZE] )
{
	int			i, j;
	qboolean	found = qfalse;
	long int	final_total = 0;
	int			pruned = 0;
	for ( i = 0; i <= sort_total; i++ )
	{			// Sort them into the final array...
		int num_found = 0;

		// Init found flag...
		found = qfalse;
		for ( j = 0; j < sort_total; j++ )
		{		// Lower them all to ground level...
			vec3_t	org1;
			vec3_t	org2;
			vec3_t	org3;
			intToVectorCopy( sort_nodes[i], org1 );
			intToVectorCopy( sort_nodes[j], org2 );
			org2[2] += 16;
			intToVectorCopy( sort_nodes[i], org3 );
			org3[2] += 32;
			if ( intVectorDistance( sort_nodes[i], sort_nodes[j]) > (waypoint_scatter_distance * 4) )
			{
				continue;
			}

			if ( NodeVisible( org1, org3, -1) < 1 )
			{	// Height too low for a player...
				//trap_Printf(va("Height too low for waypoint at %f %f %f (%i). Pruned!\n", org1[0], org1[1], org1[2], i));
				found = qfalse;
				break;
			}

			if ( NodeVisible( org1, org2, -1) >= 1 )
			{
				num_found++;
				if ( num_found > 2 )
				{
					found = qtrue;
					break;
				}
			}
		}

		if ( !found )
		{

			//trap_Printf(va("No links for waypoint at %i %i %i (%i). Pruned!\n", sort_nodes[i][0], sort_nodes[i][1], sort_nodes[i][2], i));
			pruned++;
			continue;
		}

		intToVectorCopy( sort_nodes[i], final_nodes[final_total] );
		final_total++;
	}

	final_total--;

	//trap_Printf(va("Advanced pruned %i waypoints. There are %i final waypoints.\n", pruned, final_total));
	return ( final_total );
}

extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );
node_t			temporary_nodes[MAX_NODES];


/* */
int
GetWaypointFCost ( int to, int num, int parentNum, float *gcost )
{
	float	gc = 0;
	float	hc = 0;
	vec3_t	v;
	if ( gcost[num] == -1 )
	{
		if ( parentNum != -1 )
		{
			gc = gcost[parentNum];
			VectorSubtract( temporary_nodes[num].origin, temporary_nodes[parentNum].origin, v );
			gc += VectorLength( v );
		}

		gcost[num] = gc;
	}
	else
	{
		gc = gcost[num];
	}

	VectorSubtract( temporary_nodes[to].origin, temporary_nodes[num].origin, v );
	hc = VectorLength( v );
	return (int) ( gc + hc );
}


/* */
int
CreateWaypointPathAStar ( int from, int to, short int /***/ pathlist[MAX_NODES] )
{

	//all the data we have to hold...since we can't do dynamic allocation, has to be MAX_NODES
	//we can probably lower this later - eg, the open list should never have more than at most a few dozen items on it
	short int	openlist[MAX_NODES + 1];			//add 1 because it's a binary heap, and they don't use 0 - 1 is the first used index
	float		gcost[MAX_NODES];
	int			fcost[MAX_NODES];
	char		list[MAX_NODES];					//0 is neither, 1 is open, 2 is closed - char because it's the smallest data type
	short int	parent[MAX_NODES];
	short int	numOpen = 0;
	short int	atNode, temp, newnode = -1;
	qboolean	found = qfalse;
	int			count = -1;
	float		gc;
	int			i, u, v, m;
	vec3_t		vec;

	//clear out all the arrays
	memset( openlist, 0, sizeof(short int) * (MAX_NODES + 1) );
	memset( fcost, 0, sizeof(int) * MAX_NODES );
	memset( list, 0, sizeof(char) * MAX_NODES );
	memset( parent, 0, sizeof(short int) * MAX_NODES );
	memset( gcost, -1, sizeof(float) * MAX_NODES );

	//if ((from == NODE_INVALID) || (to == NODE_INVALID) || (from >= MAX_NODES) || (to >= MAX_NODES) || (from == to))
	//	return -1;
	openlist[1] = from;								//add the starting node to the open list
	numOpen++;
	gcost[from] = 0;								//its f and g costs are obviously 0
	fcost[from] = 0;
	while ( 1 )
	{
		if ( numOpen != 0 )							//if there are still items in the open list
		{

			//pop the top item off of the list
			atNode = openlist[1];
			list[atNode] = 2;						//put the node on the closed list so we don't check it again
			numOpen--;
			openlist[1] = openlist[numOpen + 1];	//move the last item in the list to the top position
			v = 1;

			//this while loop reorders the list so that the new lowest fcost is at the top again
			while ( 1 )
			{
				u = v;
				if ( (2 * u + 1) < numOpen )		//if both children exist
				{
					if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
					{
						v = 2 * u;
					}

					if ( fcost[openlist[v]] >= fcost[openlist[2 * u + 1]] )
					{
						v = 2 * u + 1;
					}
				}
				else
				{
					if ( (2 * u) < numOpen )		//if only one child exists
					{
						if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
						{
							v = 2 * u;
						}
					}
				}

				if ( u != v )						//if they're out of order, swap this item with its parent
				{
					temp = openlist[u];
					openlist[u] = openlist[v];
					openlist[v] = temp;
				}
				else
				{
					break;
				}
			}

			for ( i = 0; i < temporary_nodes[atNode].enodenum; i++ )							//loop through all the links for this node
			{
				newnode = temporary_nodes[atNode].links[i].targetNode;
				if ( BAD_WP_Height( temporary_nodes[newnode].origin, temporary_nodes[atNode].origin) )
				{																				// Height diff is greater then plane distance... Block it and skip it..
					continue;
				}

				//if (NodeVisible(temporary_nodes[atNode].origin, temporary_nodes[newnode].origin, -1) < 1)
				//	continue;
				//skip any unreachable nodes
				if ( list[newnode] == 2 )
				{																				//if this node is on the closed list, skip it
					continue;
				}

				if ( list[newnode] != 1 )														//if this node is not already on the open list
				{
					openlist[++numOpen] = newnode;												//add the new node to the open list
					list[newnode] = 1;
					parent[newnode] = atNode;													//record the node's parent
					if ( newnode == to )
					{																			//if we've found the goal, don't keep computing paths!
						break;																	//this will break the 'for' and go all the way to 'if (list[to] == 1)'
					}

					fcost[newnode] = GetWaypointFCost( to, newnode, parent[newnode], gcost );	//store it's f cost value

					//this loop re-orders the heap so that the lowest fcost is at the top
					m = numOpen;
					while ( m != 1 )															//while this item isn't at the top of the heap already
					{
						if ( fcost[openlist[m]] <= fcost[openlist[m / 2]] )						//if it has a lower fcost than its parent
						{
							temp = openlist[m / 2];
							openlist[m / 2] = openlist[m];
							openlist[m] = temp;													//swap them
							m /= 2;
						}
						else
						{
							break;
						}
					}
				}
				else										//if this node is already on the open list
				{
					gc = gcost[atNode];
					VectorSubtract( temporary_nodes[newnode].origin, temporary_nodes[atNode].origin, vec );
					gc += VectorLength( vec );				//calculate what the gcost would be if we reached this node along the current path
					if ( gc < gcost[newnode] )				//if the new gcost is less (ie, this path is shorter than what we had before)
					{
						parent[newnode] = atNode;			//set the new parent for this node
						gcost[newnode] = gc;				//and the new g cost
						for ( i = 1; i < numOpen; i++ )		//loop through all the items on the open list
						{
							if ( openlist[i] == newnode )	//find this node in the list
							{

								//calculate the new fcost and store it
								fcost[newnode] = GetWaypointFCost( to, newnode, parent[newnode], gcost );

								//reorder the list again, with the lowest fcost item on top
								m = i;
								while ( m != 1 )
								{
									if ( fcost[openlist[m]] < fcost[openlist[m / 2]] )	//if the item has a lower fcost than it's parent
									{
										temp = openlist[m / 2];
										openlist[m / 2] = openlist[m];
										openlist[m] = temp;								//swap them
										m /= 2;
									}
									else
									{
										break;
									}
								}
								break;													//exit the 'for' loop because we already changed this node
							}															//if
						}																//for
					}					//if (gc < gcost[newnode])
				}						//if (list[newnode] != 1) --> else
			}							//for (loop through links)
		}								//if (numOpen != 0)
		else
		{
			found = qfalse;				//there is no path between these nodes
			break;
		}

		if ( list[to] == 1 )			//if the destination node is on the open list, we're done
		{
			found = qtrue;
			break;
		}
	}									//while (1)

	if ( found == qtrue )				//if we found a path
	{

		//G_Printf("!!! Path found !!!\n");
		count = 0;
		temp = to;						//start at the end point
		while ( temp != from )			//travel along the path (backwards) until we reach the starting point
		{
			pathlist[count++] = temp;	//add the node to the pathlist and increment the count
			temp = parent[temp];		//move to the parent of this node to continue the path
		}

		pathlist[count++] = from;		//add the beginning node to the end of the pathlist
		return ( count );
	}

	/*
	G_Printf("No path between %i (%f %f %f) and %i (%f %f %f).\n"
												, from, temporary_nodes[from].origin[0], temporary_nodes[from].origin[1], temporary_nodes[from].origin[2]
												, to, temporary_nodes[to].origin[0], temporary_nodes[to].origin[1], temporary_nodes[to].origin[2]);
*/
	return ( -1 );
}


/* */
void
AIMOD_CreateWaypointLinks ( int node, int temp2_total )
{
	int loop = 0;
	int linknum = 0;
	for ( loop = 0; loop <= temp2_total; loop++ )
	{
		vec3_t	tmp;
		VectorCopy( temporary_nodes[node].origin, tmp );
		tmp[2] += 16;
		if ( BAD_WP_Height( temporary_nodes[node].origin, temporary_nodes[loop].origin) )
		{
			continue;
		}

		if ( VectorDistance( temporary_nodes[loop].origin, temporary_nodes[node].origin) <= 512 /*256*/ )
		{
			int visCheck = NodeVisible( temporary_nodes[loop].origin, tmp, -1 );

			//0 = wall in way
			//1 = player or no obstruction
			//2 = useable door in the way.
			//3 = door entity in the way.
			if ( visCheck == 1 || visCheck == 2 || visCheck == 3 )
			{
				temporary_nodes[node].links[linknum].targetNode = loop;
				temporary_nodes[node].links[linknum].cost = VectorDistance( temporary_nodes[loop].origin,
																			temporary_nodes[node].origin );
				linknum++;
				if ( linknum > MAX_NODELINKS )
				{
					linknum--;
					break;
				}
			}
		}
	}

	temporary_nodes[node].enodenum = linknum;

	//G_Printf("^4*** ^3%s^5: Created ^7%i^5 links for node number ^7%i^5.\n", GAME_VERSION, linknum, node);
}


/* */
void
AIMod_AutoWaypoint_NewMethod ( void )
{
	long int	nodes = 0;
	long int	i = 0;
	long int	j = 0;
	int			pruned = 0;
	intvec3_t	arealist[MAX_MAP_SIZE];
	intvec3_t	output_listb[MAX_MAP_SIZE];
	int			output_totalb = 0;
	vec3_t		output_list[MAX_MAP_SIZE];
	int			output_num = 0;
	float		startx = -MAX_MAP_SIZE, starty = -MAX_MAP_SIZE, startz = -MAX_MAP_SIZE;
	qboolean	incomplete = qtrue;
	vec3_t		mapMins, mapMaxs, mins, maxs;
	short int	objNum[3] = { 0, 0, 0 };
	int			start_time = trap_Milliseconds();
	int			upto_counter = 0;
	qboolean	found = qfalse;
	int			areas = 0;
	int			total_waypoints = 0;
	int			sort_total = 0;
	intvec3_t	sort_nodes[MAX_MAP_SIZE];
	qboolean	closest_retry = qfalse;
	qboolean	bigger_retry = qfalse;
	qboolean	retry_closer = qfalse;
	qboolean	retried = qfalse;
	qboolean	dump_nodes = qfalse;

	// These start off reversed so that we can set them using traces...
	VectorSet( mapMins, MAX_MAP_SIZE, MAX_MAP_SIZE, MAX_MAP_SIZE );
	VectorSet( mapMaxs, -MAX_MAP_SIZE, -MAX_MAP_SIZE, -MAX_MAP_SIZE );
	RemoveDoorsAndDestroyablesForSave();
	trap_Printf( va( "Finding map bounds.\n") );
	AIMod_GetMapBounts( mapMins, mapMaxs );
	trap_Printf( va( "Map bounds are %f %f %f to %f %f %f.\n", mapMins[0], mapMins[1], mapMins[2], mapMaxs[0],
				 mapMaxs[1], mapMaxs[2]) );
	areas = AIMod_Generate_Areas( mapMins, mapMaxs, arealist );
	trap_Printf( va( "Generated %i areas.\n", areas) );
	for ( i = 0; i <= areas; i++ )
	{
		int			area_wps_added = 0;
		int			temp_total = 0;
		int			temp2_total = 0;
		short int	pathlist[MAX_NODES];
		int			final_total = 0;
		vec3_t		final_nodes[MAX_MAP_SIZE];
		vec3_t		temp_nodes[MAX_MAP_SIZE];
		vec3_t		temp2_nodes[MAX_MAP_SIZE];
		intvec3_t	nodelist[MAX_MAP_SIZE /*MAX_TEMP_SIZE*/ ];
		/*if ( dump_nodes )
		{

			//VectorSet(mins, 0-(area_scatter_distance*0.45), 0-(area_scatter_distance*0.45), 0-(area_scatter_distance*0.45));
			//VectorSet(maxs, area_scatter_distance*0.45, area_scatter_distance*0.45, area_scatter_distance*0.45);
			//VectorSet(mins, 0-(area_scatter_distance*0.65), 0-(area_scatter_distance*0.65), 0-(area_scatter_distance*0.65));
			//VectorSet(maxs, area_scatter_distance*0.65, area_scatter_distance*0.65, area_scatter_distance*0.65);
			VectorSet( mins, 0 - (area_scatter_distance), 0 - (area_scatter_distance), 0 - (area_scatter_distance * 0.45) );
			VectorSet( maxs, area_scatter_distance, area_scatter_distance, area_scatter_distance * 0.45 );
			VectorAdd( mins, arealist[i], mins );
			VectorAdd( maxs, arealist[i], maxs );
			nodes = AIMod_Generate_Temp_Waypoints_Closer( mins, maxs, nodelist );
		}
		else if ( closest_retry )
		{

			//VectorSet(mins, 0-(area_scatter_distance*0.75), 0-(area_scatter_distance*0.75), 0-(area_scatter_distance*0.75));
			//VectorSet(maxs, area_scatter_distance*0.75, area_scatter_distance*0.75, area_scatter_distance*0.75);
			//VectorSet(mins, 0-(area_scatter_distance*0.85), 0-(area_scatter_distance*0.85), 0-(area_scatter_distance*0.85));
			//VectorSet(maxs, area_scatter_distance*0.85, area_scatter_distance*0.85, area_scatter_distance*0.85);
			//VectorSet(mins, 0-(area_scatter_distance), 0-(area_scatter_distance), 0-(area_scatter_distance));
			//VectorSet(maxs, area_scatter_distance, area_scatter_distance, area_scatter_distance);
			VectorSet( mins, 0 - (area_scatter_distance * 1.5), 0 - (area_scatter_distance), 0 - (area_scatter_distance) );
			VectorSet( maxs, area_scatter_distance, area_scatter_distance, area_scatter_distance );
			VectorAdd( mins, arealist[i], mins );
			VectorAdd( maxs, arealist[i], maxs );
			nodes = AIMod_Generate_Temp_Waypoints_Closest( mins, maxs, nodelist );
		}
		else if ( bigger_retry )
		{

			//VectorSet(mins, 0-(area_scatter_distance), 0-(area_scatter_distance), 0-(area_scatter_distance));
			//VectorSet(maxs, area_scatter_distance, area_scatter_distance, area_scatter_distance);
			VectorSet( mins, 0 - (area_scatter_distance * 2), 0 - (area_scatter_distance * 2), 0 - (area_scatter_distance) );
			VectorSet( maxs, area_scatter_distance * 2, area_scatter_distance * 2, area_scatter_distance );
			VectorAdd( mins, arealist[i], mins );
			VectorAdd( maxs, arealist[i], maxs );
			nodes = AIMod_Generate_Temp_Waypoints_Closer( mins, maxs, nodelist );
		}
		else if ( retry_closer )
		{

			//VectorSet(mins, 0-(area_scatter_distance*0.75), 0-(area_scatter_distance*0.75), 0-(area_scatter_distance*0.75));
			//VectorSet(maxs, area_scatter_distance*0.75, area_scatter_distance*0.75, area_scatter_distance*0.75);
			//VectorSet(mins, 0-(area_scatter_distance*0.85), 0-(area_scatter_distance*0.85), 0-(area_scatter_distance*0.85));
			//VectorSet(maxs, area_scatter_distance*0.85, area_scatter_distance*0.85, area_scatter_distance*0.85);
			VectorSet( mins, 0 - (area_scatter_distance), 0 - (area_scatter_distance), 0 - (area_scatter_distance) );
			VectorSet( maxs, area_scatter_distance, area_scatter_distance, area_scatter_distance );
			VectorAdd( mins, arealist[i], mins );
			VectorAdd( maxs, arealist[i], maxs );
			nodes = AIMod_Generate_Temp_Waypoints_Closer( mins, maxs, nodelist );
		}
		else*/
		{

			//VectorSet(mins, 0-(area_scatter_distance), 0-(area_scatter_distance), 0-(area_scatter_distance));
			//VectorSet(maxs, area_scatter_distance, area_scatter_distance, area_scatter_distance);
			VectorSet( mins, 0 - (area_scatter_distance * 1.5), 0 - (area_scatter_distance * 1.5),
					   0 - (area_scatter_distance * 1.5) );
			VectorSet( maxs, area_scatter_distance * 1.5, area_scatter_distance * 1.5, area_scatter_distance * 1.5 );
			VectorAdd( mins, arealist[i], mins );
			VectorAdd( maxs, arealist[i], maxs );
			nodes = AIMod_Generate_Temp_Waypoints( mins, maxs, nodelist );
		}

		//
		// Pruning the extras...
		//
		sort_total = AIMod_Prune_Nodes( nodelist, nodes, sort_nodes );

		//
		// Now visualize the sorted table and check the validity of each node...
		//
		temp_total = AIMod_Advanced_Prune_Nodes( sort_nodes, sort_total, temp_nodes );
		VectorCopy( arealist[i], temp2_nodes[temp2_total] );
		temp2_total++;
		for ( j = 0; j < temp_total; j++ )
		{
			vec3_t	nullNode;
			VectorSet( nullNode, 0, 0, 0 );
			if ( VectorDistance( temp_nodes[j], nullNode) <= 8 )
			{
				continue;
			}

			VectorCopy( temp_nodes[temp2_total], temporary_nodes[j].origin );
			temp2_total++;
		}

		if ( !dump_nodes )
		{

			//
			// Generate actual waypoint paths...
			//
			for ( j = 0; j < areas; j++ )
			{
				int		count = 0;
				float	best_dist = 9999;
				int		best_point = -1;
				int		k = 0;
				int		l = 0;
				vec3_t	areapoint;
				VectorCopy( arealist[j], areapoint );
				areapoint[2] += 16;
				VectorCopy( areapoint, temporary_nodes[temp2_total].origin );
				if ( i == j )
				{
					continue;
				}

				if ( intVectorDistance( arealist[j], arealist[i]) > area_scatter_distance * 1.5 )
				{
					continue;
				}

				// Make links..
				for ( k = 0; k <= temp2_total; k++ )
				{
					AIMOD_CreateWaypointLinks( k, temp2_total );
				}

				memset( pathlist, -1, sizeof(pathlist) );
				count = CreateWaypointPathAStar( 0, temp2_total, pathlist );
				if ( count < 0 )
				{
					continue;
				}

				for ( l = 0; l <= count; l++ )
				{
					VectorCopy( temporary_nodes[pathlist[l]].origin, final_nodes[final_total] );
					final_total++;
				}
			}
		}
		else											// (dump_nodes)
		{												// Dumping the waypoints without pathing... Desperation...
			int l;
			for ( l = 0; l <= temp_total; l++ )
			{
				VectorCopy( temp_nodes[l], final_nodes[final_total] );
				final_total++;
			}
		}

		// Add real waypoints to the actual waypoint array...
		for ( j = 0; j < final_total; j++ )
		{
			vec3_t	nullNode;
			VectorSet( nullNode, 0, 0, 0 );
			if ( VectorDistance( final_nodes[j], nullNode) <= 8 )
			{
				continue;
			}

			//trap_Printf(va("Added waypoint at %f %f %f.\n", final_nodes[i][0], final_nodes[i][1], final_nodes[i][2]));
			//Load_AddNode(final_nodes[j], 0, objNum, 0);			//add the node
			area_wps_added++;

			//total_waypoints++;
			VectorCopy( final_nodes[j], output_listb[output_totalb] );
			output_totalb++;
		}

/*		if ( dump_nodes )
		{
			trap_Printf( va( "Area %i/%i (dumping pruned table) added %i waypoints.\n", i, areas, area_wps_added) );
		}
		else if ( closest_retry )
		{
			trap_Printf( va( "Area %i/%i (closest waypoint retry) added %i waypoints.\n", i, areas, area_wps_added) );
		}
		else if ( bigger_retry )
		{
			trap_Printf( va( "Area %i/%i (large area waypoint retry) added %i waypoints.\n", i, areas, area_wps_added) );
		}
		else if ( retry_closer )
		{
			trap_Printf( va( "Area %i/%i (closer waypoint retry) added %i waypoints.\n", i, areas, area_wps_added) );
		}
		else
		{*/
			trap_Printf( va( "Area %i/%i added %i waypoints.\n", i, areas, area_wps_added) );
//		}

		//if (retried && dump_nodes)
		//	continue;
//		if ( !retried && area_wps_added <= 10 /*0*/ )
//		{
//			retry_closer = qtrue;
//			retried = qtrue;
//			dump_nodes = qfalse;
//			i--;
//		}

		/*else if (!retried && area_wps_added <= 10)
		{
			retried = qtrue;
			dump_nodes = qtrue;
			i--;
		}*/
//		else if ( retried && area_wps_added <= 10 /*20*/ /*0*/ && !bigger_retry )
//		{
//			bigger_retry = qtrue;
//			retried = qtrue;
//			dump_nodes = qfalse;
//			i--;
//		}
//		else if ( retried && area_wps_added <= 10 /*0*/ && !closest_retry )
//		{
//			closest_retry = qtrue;
//			retried = qtrue;
//			dump_nodes = qfalse;
//			i--;
//		}
//		else if ( closest_retry /*retried*/ && area_wps_added <= 10 && !dump_nodes )
//		{
//			retried = qtrue;
//			dump_nodes = qtrue;
//			i--;
//		}
//		else
		{
			dump_nodes = qfalse;
			bigger_retry = qfalse;
			retry_closer = qfalse;
			closest_retry = qfalse;
			retried = qfalse;
		}
	}

	//
	// Pruning the extras...
	//
	trap_Printf( va( "Pruning final waypoints list.\n") );
	sort_total = AIMod_Prune_Nodes( output_listb, output_totalb, sort_nodes );

	//
	// Now visualize the sorted table and check the validity of each node...
	//
	trap_Printf( va( "Visual pruning final waypoints list.\n") );
	output_num = AIMod_Advanced_Prune_Nodes( sort_nodes, sort_total, output_list );

	//trap_Printf(va("Visual pruning final waypoints list.\n"));
	//output_num = AIMod_Advanced_Prune_Nodes(output_listb, output_totalb, output_list);
	for ( i = 0; i <= output_num; i++ )
	{
		Load_AddNode( output_list[i], 0, objNum, 0 );	//add the node
		total_waypoints++;
	}

	/*	for (i = 0; i < output_totalb; i++)
	{
		vec3_t org;

		VectorCopy(output_listb[i], org);

		Load_AddNode(org, 0, objNum, 0);			//add the node
		total_waypoints++;
	}*/
	AIMOD_NODES_SaveNodes_Autowaypointed();

	//AIMOD_NODES_SaveNodes();
	trap_Printf( va( "Waypoint database created successfully in %f seconds with %i waypoints.\n",
				 (float) ((trap_Milliseconds() - start_time) / 1000), total_waypoints) );
}


/* */
void
AIMod_AutoWaypoint_Old ( void )
{
	long int	nodes = 0;
	long int	i = 0;
	long int	j = 0;
	int			pruned = 0;
	intvec3_t	nodelist[MAX_TEMP_SIZE];
	intvec3_t	sort_nodes[MAX_NODES];
	int			sort_total = 0;
	vec3_t		final_nodes[MAX_NODES];
	int			final_total = 0;
	float		startx = -MAX_MAP_SIZE, starty = -MAX_MAP_SIZE, startz = -MAX_MAP_SIZE;
	qboolean	incomplete = qtrue;
	vec3_t		mapMins, mapMaxs;
	float		highest_z_point = -MAX_MAP_SIZE;
	short int	objNum[3] = { 0, 0, 0 };
	int			start_time = trap_Milliseconds();
	int			upto_counter = 0;
	qboolean	found = qfalse;

	// These start off reversed so that we can set them using traces...
	VectorSet( mapMins, MAX_MAP_SIZE, MAX_MAP_SIZE, MAX_MAP_SIZE );
	VectorSet( mapMaxs, -MAX_MAP_SIZE, -MAX_MAP_SIZE, -MAX_MAP_SIZE );
	RemoveDoorsAndDestroyablesForSave();

	//
	// Z
	//
	i = 0;
	while ( startx < MAX_MAP_SIZE )
	{
		while ( starty < MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[2] += ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[2] < mapMins[2] )
			{
				mapMins[2] = tr.endpos[2];
				continue;
			}

			if ( tr.startsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA														/*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				continue;
			}

			starty += 256;
		}

		startx += 256;
		starty = -MAX_MAP_SIZE;
	}

	mapMins[2] += 16;
	startx = MAX_MAP_SIZE;
	starty = MAX_MAP_SIZE;
	startz = MAX_MAP_SIZE;
	i = 0;
	while ( startx > -MAX_MAP_SIZE )
	{
		while ( starty > -MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[2] -= ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[2] > mapMaxs[2] )
			{
				mapMaxs[2] = tr.endpos[2];
				continue;
			}

			if ( tr.startsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA														/*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				continue;
			}

			starty -= 256;
		}

		startx -= 256;
		starty = MAX_MAP_SIZE;
	}

	mapMaxs[2] -= 16;

	//
	// X
	//
	startx = -MAX_MAP_SIZE;
	starty = -MAX_MAP_SIZE;
	startz = mapMins[2];
	i = 0;
	while ( startz < mapMaxs[2] )
	{
		while ( starty < MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[0] += ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[0] < mapMins[0] )
			{
				mapMins[0] = tr.endpos[0];
				continue;
			}

			if ( tr.startsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA														/*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				continue;
			}

			starty += 256;
		}

		startz += 256;
		starty = -MAX_MAP_SIZE;
	}

	mapMins[0] += 16;
	startx = MAX_MAP_SIZE;
	starty = MAX_MAP_SIZE;
	startz = mapMaxs[2];
	i = 0;
	while ( startz > mapMins[2] )
	{
		while ( starty > -MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[0] -= ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[0] > mapMaxs[0] )
			{
				mapMaxs[0] = tr.endpos[0];
				continue;
			}

			if ( tr.startsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA														/*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				continue;
			}

			starty -= 256;
		}

		startz -= 256;
		starty = MAX_MAP_SIZE;
	}

	mapMaxs[0] -= 16;

	//
	// Y
	//
	startx = -MAX_MAP_SIZE;
	starty = -MAX_MAP_SIZE;
	startz = mapMins[2];
	i = 0;
	while ( startz < mapMaxs[2] )
	{
		while ( startx < MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[1] += ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[1] < mapMins[1] )
			{
				mapMins[1] = tr.endpos[1];
				continue;
			}

			if ( tr.startsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA														/*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				continue;
			}

			startx += 256;
		}

		startz += 256;
		startx = -MAX_MAP_SIZE;
	}

	mapMins[1] += 16;
	startx = MAX_MAP_SIZE;
	starty = MAX_MAP_SIZE;
	startz = mapMaxs[2];
	i = 0;
	while ( startz > mapMins[2] )
	{
		while ( startx > -MAX_MAP_SIZE )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[1] -= ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.endpos[1] > mapMaxs[1] )
			{
				mapMaxs[1] = tr.endpos[1];
				continue;
			}

			if ( tr.startsolid )
			{
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA														/*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				continue;
			}

			startx -= 256;
		}

		startz -= 256;
		startx = MAX_MAP_SIZE;
	}

	mapMaxs[1] -= 16;

	//
	// Refine Z Top Point to highest ground height!
	//
	startx = mapMaxs[0] - 32;
	starty = mapMaxs[1] - 32;
	startz = mapMaxs[2] - 32;
	highest_z_point = mapMins[2];
	i = 0;
	while ( startx > mapMins[2] )
	{
		while ( starty > mapMins[1] )
		{
			trace_t tr;
			vec3_t	org1;
			vec3_t	org2;
			VectorSet( org1, startx, starty, startz );
			VectorSet( org2, startx, starty, startz );
			org2[2] -= ( 65536 * 2 );
			trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
			if ( tr.startsolid )
			{
				starty -= 128;
				continue;
			}

			if
			(
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
				tr.contents & CONTENTS_LAVA														/*|| tr.contents & CONTENTS_SOLID*/
			)
			{
				starty -= 128;
				continue;
			}

			if ( tr.endpos[2] > highest_z_point )
			{
				highest_z_point = tr.endpos[2];
				continue;
			}

			starty -= 128;
		}

		startx -= 128;
		starty = mapMaxs[1];
	}

	if ( highest_z_point == mapMins[2] )
	{
		highest_z_point = mapMaxs[2];
	}

	mapMaxs[2] = highest_z_point;
	trap_Printf( va( "Map bounds are %f %f %f to %f %f %f.\n", mapMins[0], mapMins[1], mapMins[2], mapMaxs[0],
				 mapMaxs[1], mapMaxs[2]) );

	//
	// OK.. Now we have the map bounds... Generate a set of temp nodes...
	//
	startx = mapMins[0];
	starty = mapMins[1];
	startz = mapMins[2];
	i = 0;
	trap_Printf( va( "Generating temporary waypoints list.\n") );
	while ( startx < mapMaxs[0] )
	{
		while ( starty < mapMaxs[1] )
		{
			while ( startz < mapMaxs[2] )
			{
				trace_t tr;
				vec3_t	org1;
				vec3_t	org2;
				VectorSet( org1, startx, starty, startz );
				VectorSet( org2, startx, starty, startz );
				org2[2] -= ( 65536 * 2 );
				trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
				if ( tr.startsolid )
				{
					startz += waypoint_scatter_distance * 2;
					continue;
				}

				if
				(
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
					tr.contents & CONTENTS_LAVA													/*|| tr.contents & CONTENTS_SOLID*/
				)
				{
					startz += waypoint_scatter_distance * 2;
					continue;
				}

				VectorSet( nodelist[i], startx, starty, startz );
				startz += waypoint_scatter_distance * 2;
				i++;
			}

			startz = mapMins[2];
			starty += waypoint_scatter_distance;
		}

		starty = mapMins[1];
		startx += waypoint_scatter_distance;
	}

	//
	// Now sort the nodes a bit...
	//
	nodes = i;
	nodes--;
	trap_Printf( va( "Generated %i temporary waypoints.\n", nodes) );
	i = 0;
	trap_Printf( va( "Height-checking temporary waypoints list.\n") );
	for ( i = 0; i < nodes; i++ )
	{																							// Lower them all to ground level...
		trace_t tr;
		vec3_t	org1;
		vec3_t	org2;
		intToVectorCopy( nodelist[i], org1 );
		intToVectorCopy( nodelist[i], org2 );
		org2[2] -= ( 65536 * 2 );
		trap_Trace( &tr, org1, NULL, NULL, org2, -1, MASK_SHOT | MASK_WATER );
		if ( tr.startsolid )
		{
			VectorSet( nodelist[i], 0, 0, 0 );
			continue;
		}

		if
		(
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
			tr.contents & CONTENTS_LAVA															/*|| tr.contents & CONTENTS_SOLID*/
		)
		{
			VectorSet( nodelist[i], 0, 0, 0 );
			continue;
		}

		nodelist[i][2] = tr.endpos[2] + 32;
	}

	//
	// Pruning the extras...
	//
	trap_Printf( va( "Pruning waypoints list.\n") );
	for ( i = 0; i < nodes; i++ )
	{																							// Find any that have the full number of neighbours, and prune all the extras around them...
		// Init found flag...
		int found_count = 0;
		int found_list[MAX_MAP_SIZE];
		found = qfalse;
		upto_counter++;
		for ( j = 0; j < nodes; j++ )
		{																						// Lower them all to ground level...
			if ( i == j )
			{
				continue;
			}

			if
			(
				nodelist[i][0] == nodelist[j][0] &&
				nodelist[i][1] == nodelist[j][0] &&
				nodelist[i][2] == nodelist[j][0]
			)
			{																					// Removed this one...
				VectorSet( nodelist[j], 0, 0, 0 );
				continue;
			}

			if ( nodelist[j][0] == 0 && nodelist[j][1] == 0 && nodelist[j][2] == 0 )
			{																					// Removed this one...
				continue;
			}

			if ( intVectorDistance( nodelist[i], nodelist[j]) <= (waypoint_scatter_distance) )	//*1.5))
			{
				found_list[found_count] = j;
				found_count++;
			}
		}

		if ( found_count >= 6 )
		{
			int z;
			for ( z = 0; z < found_count; z++ )
			{
				VectorSet( nodelist[found_list[z]], 0, 0, 0 );
			}
		}
	}

	for ( i = 0; i < nodes; i++ )
	{													// Sort them into the final array...
		// Init found flag...
		found = qfalse;
		for ( j = 0; j < sort_total; j++ )
		{												// Lower them all to ground level...
			intvec3_t	nullNode;
			VectorSet( nullNode, 0, 0, 0 );
			if
			(
				(nodelist[i][0] == 0 && nodelist[i][1] == 0 && nodelist[i][2] == 0) ||
				intVectorDistance( nodelist[i], nullNode) <= 8
			)
			{											// Remove this one...
				found = qtrue;
				break;
			}

			if ( intVectorDistance( nodelist[i], sort_nodes[j]) <= waypoint_scatter_distance )
			{
				found = qtrue;
				break;
			}
		}

		if ( found )
		{
			pruned++;
			continue;
		}

		intVectorCopy( nodelist[i], sort_nodes[sort_total] );
		sort_total++;
	}

	sort_total--;
	trap_Printf( va( "Pruned %i waypoints. There are %i sorted waypoints.\n", pruned, sort_total) );

	//
	// Now visualize the sorted table and check the validity of each node...
	//
	pruned = 0;
	trap_Printf( va( "Advanced (visual) pruning waypoints list.\n") );
	for ( i = 0; i < sort_total; i++ )
	{													// Sort them into the final array...
		int num_found = 0;

		// Init found flag...
		found = qfalse;
		for ( j = 0; j < sort_total; j++ )
		{												// Lower them all to ground level...
			vec3_t	org1;
			vec3_t	org2;
			vec3_t	org3;
			intToVectorCopy( sort_nodes[i], org1 );
			intToVectorCopy( sort_nodes[j], org2 );
			org2[2] += 16;
			intToVectorCopy( sort_nodes[i], org3 );
			org3[2] += 32;
			if ( intVectorDistance( sort_nodes[i], sort_nodes[j]) > (waypoint_scatter_distance * 4) )
			{
				continue;
			}

			if ( NodeVisible( org1, org3, -1) < 1 )
			{											// Height too low for a player...
				//trap_Printf(va("Height too low for waypoint at %f %f %f (%i). Pruned!\n", org1[0], org1[1], org1[2], i));
				found = qfalse;
				break;
			}

			if ( NodeVisible( org1, org2, -1) >= 1 )
			{
				num_found++;
				if ( num_found > 2 )
				{
					found = qtrue;
					break;
				}
			}
		}

		if ( !found )
		{

			//trap_Printf(va("No links for waypoint at %i %i %i (%i). Pruned!\n", sort_nodes[i][0], sort_nodes[i][1], sort_nodes[i][2], i));
			pruned++;
			continue;
		}

		intToVectorCopy( sort_nodes[i], final_nodes[final_total] );
		final_total++;
	}

	final_total--;
	trap_Printf( va( "Advanced pruned %i waypoints. There are %i final waypoints.\n", pruned, final_total) );
	for ( i = 0; i < final_total; i++ )
	{
		vec3_t	nullNode;
		VectorSet( nullNode, 0, 0, 0 );
		if ( VectorDistance( final_nodes[i], nullNode) <= 8 )
		{
			continue;
		}

		//trap_Printf(va("Added waypoint at %f %f %f.\n", final_nodes[i][0], final_nodes[i][1], final_nodes[i][2]));
		Load_AddNode( final_nodes[i], 0, objNum, 0 );	//add the node
	}

	number_of_nodes--;
	trap_Printf( va( "Made a total of %i nodes.\n", final_total) );
	AIMOD_NODES_SaveNodes_Autowaypointed();

	//	AIMOD_NODES_SaveNodes();
	trap_Printf( va( "Waypoint database created successfully in %f seconds.\n",
				 (float) ((trap_Milliseconds() - start_time) / 1000)) );
}


/* */
void
AIMod_AutoWaypoint ( void )
{
	char	str[MAX_TOKEN_CHARS];

	int cheats, dedicated;

	cheats = trap_Cvar_VariableIntegerValue("sv_cheats");
	dedicated = trap_Cvar_VariableIntegerValue("dedicated");

	if( !cheats || !dedicated ) {
		G_Printf("^1You need to run a dedicated server with cheats on!\n");
		return;
	}

	if ( trap_Argc() < 2 )
	{
		G_Printf( "Usage:\n" );
		G_Printf( "/autowaypoint <method> or /aw <method>\n" );
		G_Printf( "Available methods are \"new\" and \"old\".\n" );
		return;
	}

	trap_Argv( 1, str, sizeof(str) );
	if ( Q_stricmp( str, "new") == 0 )
	{
		AIMod_AutoWaypoint_NewMethod();
	}
	else if ( Q_stricmp( str, "old") == 0 )
	{
		AIMod_AutoWaypoint_Old();
	}
}

extern int	BotObjectives[MAX_GENTITIES];
extern int	num_bot_objectives;
extern int	BotGoals[MAX_GENTITIES];
extern int	num_bot_goals;
extern int	CreatePathAStar ( gentity_t *bot, int from, int to, short int *pathlist );
extern int	AIMOD_NAVIGATION_FindClosestReachableNode ( gentity_t *bot, int r, int type, gentity_t *master );
extern int	AIMOD_NAVIGATION_FindClosestReachableNodeTo ( vec3_t origin, int r, int type );


/* */
void
AIMod_CheckMapPaths ( gentity_t *ent )
{
	short int	pathlist[MAX_NODES];
	int			pathsize;
	int			i;
	ent->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, ent );
	trap_Printf( va( "Finding bot objectives for %s at node number %i (%f %f %f).\n", ent->client->pers.netname,
				 ent->current_node, nodes[ent->current_node].origin[0], nodes[ent->current_node].origin[1],
				 nodes[ent->current_node].origin[2]) );
	for ( i = 0; i < num_bot_objectives; i++ )
	{
		gentity_t	*goal = &g_entities[BotObjectives[i]];
		ent->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( goal, NODE_DENSITY, NODEFIND_ALL, ent );
		pathsize = CreatePathAStar( ent, ent->current_node, ent->longTermGoal, pathlist );
		trap_Printf( va( "Objective %i (%s) pathsize is %i.\n", i, goal->classname, pathsize) );
	}

	trap_Printf( va( "Finding bot goals for %s at node number %i (%f %f %f).\n", ent->client->pers.netname,
				 ent->current_node, nodes[ent->current_node].origin[0], nodes[ent->current_node].origin[1],
				 nodes[ent->current_node].origin[2]) );
	for ( i = 0; i < num_bot_goals; i++ )
	{
		gentity_t	*goal = &g_entities[BotGoals[i]];
		ent->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNode( goal, NODE_DENSITY, NODEFIND_ALL, ent );
		pathsize = CreatePathAStar( ent, ent->current_node, ent->longTermGoal, pathlist );
		trap_Printf( va( "Goal %i (%s) pathsize is %i.\n", i, goal->classname, pathsize) );
	}

	trap_Printf( va( "Complete.\n") );
}


/* */
void
AIMod_ShowNodeLinks ( gentity_t *ent )
{
	int i;
	ent->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( ent, NODE_DENSITY, NODEFIND_ALL, ent );
	trap_Printf( va( "Displaying node links for node number %i (%f %f %f).\n", ent->current_node,
				 nodes[ent->current_node].origin[0], nodes[ent->current_node].origin[1],
				 nodes[ent->current_node].origin[2]) );
	for ( i = 0; i < nodes[ent->current_node].enodenum; i++ )
	{
		if
		(
			NodeVisible( nodes[ent->current_node].origin, nodes[nodes[ent->current_node].links[i].targetNode].origin,
						 ent->s.number) >= 1
		)
		{
			trap_Printf( va( "Visible: %i.", nodes[ent->current_node].links[i].targetNode) );
		}
		else
		{
			trap_Printf( va( "Not visible: %i!!!", nodes[ent->current_node].links[i].targetNode) );
		}
	}
}
#endif //__BOT__
