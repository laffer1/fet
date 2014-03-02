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
#define USE_NOTHING 0
extern vmCvar_t bot_thinktime;
extern int		AIMOD_NAVIGATION_FindClosestReachableNodeTo ( vec3_t origin, int r, int type );
extern void		Bullet_Fire ( gentity_t *ent, float spread, int damage, qboolean distance_falloff );
extern qboolean NPCVisible ( gentity_t *self, gentity_t *other );
extern qboolean NPC_NodesLoaded ( void );

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
#define LUGER_SPREAD				BG_GetWeaponSpread( WP_LUGER )
#define LUGER_DAMAGE				BG_GetWeaponDamage( WP_LUGER )			// JPW
#define SILENCER_DAMAGE				BG_GetWeaponDamage( WP_SILENCER )
#define SILENCER_SPREAD				BG_GetWeaponSpread( WP_SILENCER )
#define AKIMBO_LUGER_DAMAGE			BG_GetWeaponDamage( WP_AKIMBO_LUGER )
#define AKIMBO_LUGER_SPREAD			BG_GetWeaponSpread( WP_AKIMBO_LUGER )
#define AKIMBO_SILENCEDLUGER_DAMAGE BG_GetWeaponDamage( WP_AKIMBO_SILENCEDLUGER )
#define AKIMBO_SILENCEDLUGER_SPREAD BG_GetWeaponSpread( WP_AKIMBO_SILENCEDLUGER )
#define COLT_SPREAD					BG_GetWeaponSpread( WP_COLT )
#define COLT_DAMAGE					BG_GetWeaponDamage( WP_COLT )			// JPW
#define SILENCED_COLT_DAMAGE		BG_GetWeaponDamage( WP_SILENCED_COLT )
#define SILENCED_COLT_SPREAD		BG_GetWeaponSpread( WP_SILENCED_COLT )
#define AKIMBO_COLT_DAMAGE			BG_GetWeaponDamage( WP_AKIMBO_COLT )
#define AKIMBO_COLT_SPREAD			BG_GetWeaponSpread( WP_AKIMBO_COLT )
#define AKIMBO_SILENCEDCOLT_DAMAGE	BG_GetWeaponDamage( WP_AKIMBO_SILENCEDCOLT )
#define AKIMBO_SILENCEDCOLT_SPREAD	BG_GetWeaponSpread( WP_AKIMBO_SILENCEDCOLT )
#define MP40_SPREAD					BG_GetWeaponSpread( WP_MP40 )
#define MP40_DAMAGE					BG_GetWeaponDamage( WP_MP40 )			// JPW
#define THOMPSON_SPREAD				BG_GetWeaponSpread( WP_THOMPSON )
#define THOMPSON_DAMAGE				BG_GetWeaponDamage( WP_THOMPSON )		// JPW
#define STEN_SPREAD					BG_GetWeaponSpread( WP_STEN )
#define STEN_DAMAGE					BG_GetWeaponDamage( WP_STEN )			// JPW
#define GARAND_SPREAD				BG_GetWeaponSpread( WP_GARAND )
#define GARAND_DAMAGE				BG_GetWeaponDamage( WP_GARAND )			// JPW
#define KAR98_SPREAD				BG_GetWeaponSpread( WP_KAR98 )
#define KAR98_DAMAGE				BG_GetWeaponDamage( WP_KAR98 )
#define CARBINE_SPREAD				BG_GetWeaponSpread( WP_CARBINE )
#define CARBINE_DAMAGE				BG_GetWeaponDamage( WP_CARBINE )
#define KAR98_GREN_DAMAGE			BG_GetWeaponDamage( WP_GREN_KAR98 )
#define MOBILE_MG42_SPREAD			BG_GetWeaponSpread( WP_MOBILE_MG42 )
#define MOBILE_MG42_DAMAGE			BG_GetWeaponDamage( WP_MOBILE_MG42 )
#define FG42_SPREAD					BG_GetWeaponSpread( WP_FG42 )
#define FG42_DAMAGE					BG_GetWeaponDamage( WP_FG42 )			// JPW
#define FG42SCOPE_SPREAD			BG_GetWeaponSpread( WP_FG42SCOPE )
#define FG42SCOPE_DAMAGE			BG_GetWeaponDamage( WP_FG42SCOPE )		// JPW
#define K43_SPREAD					BG_GetWeaponSpread( WP_K43 )
#define K43_DAMAGE					BG_GetWeaponDamage( WP_K43 )
#define GARANDSCOPE_SPREAD			BG_GetWeaponSpread( WP_GARAND_SCOPE )
#define GARANDSCOPE_DAMAGE			BG_GetWeaponDamage( WP_GARAND_SCOPE )
#define K43SCOPE_SPREAD				BG_GetWeaponSpread( WP_K43_SCOPE )
#define K43SCOPE_DAMAGE				BG_GetWeaponDamage( WP_K43_SCOPE )
#ifdef __RW__
#define MOBILE_M1919A4_SPREAD	BG_GetWeaponSpread( WP_M1919A4 )
#define MOBILE_M1919A4_DAMAGE	BG_GetWeaponDamage( WP_M1919A4 )
#ifdef __ETE__
#define GREASE_SPREAD			BG_GetWeaponSpread( WP_GREASE )
#define GREASE_DAMAGE			BG_GetWeaponDamage( WP_GREASE )				// JPW
#define MOBILE_MG34_SPREAD		BG_GetWeaponSpread( WP_MOBILE_MG34 )
#define MOBILE_MG34_DAMAGE		BG_GetWeaponDamage( WP_MOBILE_MG34 )
#define MOBILE_M1919A6_SPREAD	BG_GetWeaponSpread( WP_M1919A6 )
#define MOBILE_M1919A6_DAMAGE	BG_GetWeaponDamage( WP_M1919A6 )
#define G43_SPREAD				BG_GetWeaponSpread( WP_G43 )
#define G43_DAMAGE				BG_GetWeaponDamage( WP_G43 )				// JPW
#endif
#define M1CARBINE_SPREAD		BG_GetWeaponSpread( WP_M1CARBINE )
#define M1CARBINE_DAMAGE		BG_GetWeaponDamage( WP_M1CARBINE )
#define M1CARBINE_SCOPE_SPREAD	BG_GetWeaponSpread( WP_M1CARBINE_AIM )
#define M1CARBINE_SCOPE_DAMAGE	BG_GetWeaponDamage( WP_M1CARBINE_AIM )
#define BAR_SPREAD				BG_GetWeaponSpread( WP_BAR )
#define BAR_DAMAGE				BG_GetWeaponDamage( WP_BAR )				// JPW
#define BAR_AIM_SPREAD			BG_GetWeaponSpread( WP_BAR_AIM )
#define BAR_AIM_DAMAGE			BG_GetWeaponDamage( WP_BAR_AIM )			// JPW
#define K98_AIM_SPREAD			BG_GetWeaponSpread( WP_K98_AIM )
#define K98_AIM_DAMAGE			BG_GetWeaponDamage( WP_K98_AIM )			// JPW
#define K98_SPREAD				BG_GetWeaponSpread( WP_K98 )
#define K98_DAMAGE				BG_GetWeaponDamage( WP_K98 )				// JPW
#define M1GARAND_AIM_SPREAD		BG_GetWeaponSpread( WP_M1GARAND_AIM )
#define M1GARAND_AIM_DAMAGE		BG_GetWeaponDamage( WP_M1GARAND_AIM )		// JPW
#define M1GARAND_SPREAD			BG_GetWeaponSpread( WP_M1GARAND )
#define M1GARAND_DAMAGE			BG_GetWeaponDamage( WP_M1GARAND )			// JPW
#define MP44_AIM_SPREAD			BG_GetWeaponSpread( WP_MP44_AIM )
#define MP44_AIM_DAMAGE			BG_GetWeaponDamage( WP_MP44_AIM )			// JPW
#define MP44_SPREAD				BG_GetWeaponSpread( WP_MP44 )
#define MP44_DAMAGE				BG_GetWeaponDamage( WP_MP44 )				// JPW
#define MP40_AIM_SPREAD			BG_GetWeaponSpread( WP_MP40_AIM )
#define MP40_AIM_DAMAGE			BG_GetWeaponDamage( WP_MP40_AIM )			// JPW
#define THOMPSON_AIM_SPREAD		BG_GetWeaponSpread( WP_THOMPSON_AIM )
#define THOMPSON_AIM_DAMAGE		BG_GetWeaponDamage( WP_THOMPSON_AIM )		// JPW
#endif
#define LUGER_DIST					BG_GetWeaponDistance( WP_LUGER )
#define SILENCER_DIST				BG_GetWeaponDistance( WP_SILENCER )
#define AKIMBO_LUGER_DIST			BG_GetWeaponDistance( WP_AKIMBO_LUGER )
#define AKIMBO_SILENCEDLUGER_DIST	BG_GetWeaponDistance( WP_AKIMBO_SILENCEDLUGER )
#define COLT_DIST					BG_GetWeaponDistance( WP_COLT )			// JPW
#define SILENCED_COLT_DIST			BG_GetWeaponDistance( WP_SILENCED_COLT )
#define AKIMBO_COLT_DIST			BG_GetWeaponDistance( WP_AKIMBO_COLT )
#define AKIMBO_SILENCEDCOLT_DIST	BG_GetWeaponDistance( WP_AKIMBO_SILENCEDCOLT )
#define MP40_DIST					BG_GetWeaponDistance( WP_MP40 )			// JPW
#define THOMPSON_DIST				BG_GetWeaponDistance( WP_THOMPSON )		// JPW
#define STEN_DIST					BG_GetWeaponDistance( WP_STEN )			// JPW
#define GARAND_DIST					BG_GetWeaponDistance( WP_GARAND )		// JPW
#define KAR98_DIST					BG_GetWeaponDistance( WP_KAR98 )
#define CARBINE_DIST				BG_GetWeaponDistance( WP_CARBINE )
#define KAR98_GREN_DIST				BG_GetWeaponDistance( WP_GREN_KAR98 )
#define MOBILE_MG42_DIST			BG_GetWeaponDistance( WP_MOBILE_MG42 )
#define FG42_DIST					BG_GetWeaponDistance( WP_FG42 )			// JPW
#define FG42SCOPE_DIST				BG_GetWeaponDistance( WP_FG42SCOPE )	// JPW
#define K43_DIST					BG_GetWeaponDistance( WP_K43 )
#define GARANDSCOPE_DIST			BG_GetWeaponDistance( WP_GARAND_SCOPE )
#define K43SCOPE_DIST				BG_GetWeaponDistance( WP_K43_SCOPE )
#define MOBILE_M1919A4_DIST			BG_GetWeaponDistance( WP_M1919A4 )
#ifdef __RW__
#ifdef __ETE__
#define G43_AIM_SPREAD		BG_GetWeaponSpread( WP_G43_AIM )
#define G43_AIM_DAMAGE		BG_GetWeaponDamage( WP_G43_AIM )				// JPW
#define G43_DIST			BG_GetWeaponDistance( WP_G43 )					// JPW
#define G43_AIM_DIST		BG_GetWeaponDistance( WP_G43_AIM )				// JPW
#define G43_AIM_SPREAD		BG_GetWeaponSpread( WP_G43_AIM )
#define GREASE_DIST			BG_GetWeaponDistance( WP_GREASE )				// JPW
#define GREASE_AIM_SPREAD	BG_GetWeaponSpread( WP_GREASE_AIM )
#define GREASE_AIM_DAMAGE	BG_GetWeaponDamage( WP_GREASE_AIM )				// JPW
#define GREASE_AIM_DIST		BG_GetWeaponDistance( WP_GREASE_AIM )			// JPW
#define MOBILE_MG34_DIST	BG_GetWeaponDistance( WP_MOBILE_MG34 )
#define MOBILE_M1919A6_DIST BG_GetWeaponDistance( WP_M1919A6 )
#endif
#define M1CARBINE_DIST			BG_GetWeaponDistance( WP_M1CARBINE )
#define M1CARBINE_SCOPE_DIST	BG_GetWeaponDistance( WP_M1CARBINE_AIM )
#define BAR_DIST				BG_GetWeaponDistance( WP_BAR )
#define BAR_AIM_DIST			BG_GetWeaponDistance( WP_BAR_AIM )
#define K98_AIM_DIST			BG_GetWeaponDistance( WP_K98_AIM )
#define K98_DIST				BG_GetWeaponDistance( WP_K98 )				// JPW
#define M1GARAND_AIM_DIST		BG_GetWeaponDistance( WP_M1GARAND_AIM )
#define M1GARAND_DIST			BG_GetWeaponDistance( WP_M1GARAND )			// JPW
#define MP44_AIM_DIST			BG_GetWeaponDistance( WP_MP44_AIM )			// JPW
#define MP44_DIST				BG_GetWeaponDistance( WP_MP44 )
#define MP40_AIM_DIST			BG_GetWeaponDistance( WP_MP40_AIM )
#define THOMPSON_AIM_DIST		BG_GetWeaponDistance( WP_THOMPSON_AIM )		// JPW
#endif

qboolean			node_spawned_at[MAX_NODES];

// UQ1: .npc files need to match this!!!
stringID_table_t	NPC_ClassTable[] =
{
	ENUM2STRING( PC_SOLDIER ),
	ENUM2STRING( PC_MEDIC ),
	ENUM2STRING( PC_ENGINEER ),
	ENUM2STRING( PC_FIELDOPS ),
	ENUM2STRING( PC_COVERTOPS ),
};
stringID_table_t	NPC_WeaponTable[] =
{									// This MUST sync with bg_public.h 's list...
	ENUM2STRING( WP_NONE ),

	// 0
	ENUM2STRING( WP_KNIFE ),

	// 1
	ENUM2STRING( WP_LUGER ),

	// 2
	ENUM2STRING( WP_MP40 ),

	// 3
	ENUM2STRING( WP_GRENADE_LAUNCHER ),

	// 4
	ENUM2STRING( WP_PANZERFAUST ),

	// 5
	ENUM2STRING( WP_FLAMETHROWER ),

	// 6
	ENUM2STRING( WP_COLT ),

	// 7	// equivalent american weapon to german luger
	ENUM2STRING( WP_THOMPSON ),

	// 8	// equivalent american weapon to german mp40
	ENUM2STRING( WP_GRENADE_PINEAPPLE ),

	// 9
	ENUM2STRING( WP_STEN ),

	// 10	// silenced sten sub-machinegun
	ENUM2STRING( WP_MEDIC_SYRINGE ),

	// 11	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
	ENUM2STRING( WP_AMMO ),

	// 12	// JPW NERVE likewise
	ENUM2STRING( WP_ARTY ),

	// 13
	ENUM2STRING( WP_SILENCER ),

	// 14	// used to be sp5
	ENUM2STRING( WP_DYNAMITE ),

	// 15
	ENUM2STRING( WP_SMOKETRAIL ),

	// 16
	ENUM2STRING( WP_MAPMORTAR ),

	// 17	
	ENUM2STRING( VERYBIGEXPLOSION ),

	// 18	// explosion effect for airplanes
	ENUM2STRING( WP_MEDKIT ),

	// 19
	ENUM2STRING( WP_BINOCULARS ),

	// 20
	ENUM2STRING( WP_PLIERS ),

	// 21
	ENUM2STRING( WP_SMOKE_MARKER ),

	// 22	// Arnout: changed name to cause less confusion
	ENUM2STRING( WP_KAR98 ),

	// 23	// WolfXP weapons
	ENUM2STRING( WP_CARBINE ),

	// 24
	ENUM2STRING( WP_GARAND ),

	// 25
	ENUM2STRING( WP_LANDMINE ),

	// 26
	ENUM2STRING( WP_SATCHEL ),

	// 27
	ENUM2STRING( WP_SATCHEL_DET ),

	// 28
	//	ENUM2STRING( WP_TRIPMINE ),				// 29
	ENUM2STRING( WP_SMOKE_BOMB ),

	// 30
	ENUM2STRING( WP_MOBILE_MG42 ),

	// 31
	ENUM2STRING( WP_K43 ),

	// 32
	ENUM2STRING( WP_FG42 ),

	// 33
	ENUM2STRING( WP_DUMMY_MG42 ),

	// 34 // Gordon: for storing heat on mounted mg42s...
	ENUM2STRING( WP_MORTAR ),

	// 35
	//	ENUM2STRING( WP_LOCKPICK ),				// 36	// Mad Doc - TDF lockpick
	ENUM2STRING( WP_AKIMBO_COLT ),

	// 37
	ENUM2STRING( WP_AKIMBO_LUGER ),

	// 38
	// Gordon: ONLY secondaries below this mark ), as they are checked >= ENUM2STRING( WP_GPG40 && < ENUM2STRING( WP_NUM_WEAPONS
	ENUM2STRING( WP_GPG40 ),

	// 39
	ENUM2STRING( WP_M7 ),

	// 40
	ENUM2STRING( WP_SILENCED_COLT ),

	// 41
	ENUM2STRING( WP_GARAND_SCOPE ),

	// 42
	ENUM2STRING( WP_K43_SCOPE ),

	// 43
	ENUM2STRING( WP_FG42SCOPE ),

	// 44
	ENUM2STRING( WP_MORTAR_SET ),

	// 45
	ENUM2STRING( WP_MEDIC_ADRENALINE ),

	// 46
	ENUM2STRING( WP_AKIMBO_SILENCEDCOLT ),

	// 47
	ENUM2STRING( WP_AKIMBO_SILENCEDLUGER ),

	// 48
	ENUM2STRING( WP_MOBILE_MG42_SET ),

	// 49
	ENUM2STRING( WP_NUM_WEAPONS )	// WolfMP: 32 WolfXP: 50
	// NOTE: this cannot be larger than 64 for AI/player weapons!
};


/* */
int
NPC_ShortenASTARRoute ( short int *pathlist, int number )
{					// Wrote this to make the routes a little smarter (shorter)... No point looping back to the same places... Unique1
	short int	temppathlist[MAX_PATHLIST_NODES];
	int			count = 0;

	//	int count2 = 0;
	int			temp, temp2;
	int			link;
	for ( temp = number; temp >= 0; temp-- )
	{
		qboolean	shortened = qfalse;
		for ( temp2 = 0; temp2 < temp; temp2++ )
		{
			for ( link = 0; link < nodes[pathlist[temp]].enodenum; link++ )
			{
				if ( nodes[pathlist[temp]].links[link].targetNode == pathlist[temp2] )
				{	// Found a shorter route...
					if ( OrgVisible( nodes[pathlist[temp2]].origin, nodes[pathlist[temp]].origin, -1) )
					{
						temppathlist[count] = pathlist[temp2];
						temp = temp2;
						count++;
						shortened = qtrue;
					}
				}
			}
		}

		if ( !shortened )
		{
			temppathlist[count] = pathlist[temp];
			count++;
		}
	}

	pathlist = temppathlist;

	//G_Printf("ShortenASTARRoute: Path size reduced from %i to %i nodes...\n", number, count);
	return ( count );
}


/* */
int
NPC_CreateDumbRoute ( int from, int to, short int *pathlist )
{	// Create a route not using links...
	int count = 0;
	int number = 0;
	int upto = 0;
	if ( to < from )
	{
		if ( from - to > MAX_PATHLIST_NODES )
		{
			return ( -1 );
		}

		for ( upto = from; upto > to; upto-- )
		{
			pathlist[number] = upto;
			number++;
		}

		count = number;
	}
	else
	{
		if ( to - from > MAX_PATHLIST_NODES )
		{
			return ( -1 );
		}

		for ( upto = to; upto > from; upto-- )
		{
			pathlist[number] = upto;
			number++;
		}

		count = number;
	}

	return ( count - 1 );
}

extern int		GetFCost ( int to, int num, int parentNum, float *gcost );	// Below...
extern qboolean HaveSequentialNodes ( void );


/* */
int
NPC_CreatePathAStar ( gentity_t *NPC, int from, int to, short int *pathlist )
{

	//all the data we have to hold...since we can't do dynamic allocation, has to be MAX_NODES
	//we can probably lower this later - eg, the open list should never have more than at most a few dozen items on it
	short int	openlist[MAX_NODES + 1];												//add 1 because it's a binary heap, and they don't use 0 - 1 is the first used index
	float		gcost[MAX_NODES];
	int			fcost[MAX_NODES];
	char		list[MAX_NODES];														//0 is neither, 1 is open, 2 is closed - char because it's the smallest data type
	short int	parent[MAX_NODES];
	short int	numOpen = 0;
	short int	atNode, temp, newnode = -1;
	qboolean	found = qfalse;
	int			count = -1;
	float		gc;
	int			i, u, v, m;
	vec3_t		vec;
#ifdef __NPC_FAST_ROUTING__
	if ( g_gametype.integer != GT_COOP && g_gametype.integer != GT_SINGLE_PLAYER )
	{																					// In COOP, we always want shortest routes possible...
		count = NPC_CreateDumbRoute( from, to, pathlist );
		if ( count > 0 )
		{

			//count = NPC_ShortenASTARRoute(pathlist, count);
			return ( count );
		}
	}
#endif //__NPC_FAST_ROUTING__

	//clear out all the arrays
	memset( openlist, 0, sizeof(short int) * (MAX_NODES + 1) );
	memset( fcost, 0, sizeof(int) * MAX_NODES );
	memset( list, 0, sizeof(char) * MAX_NODES );
	memset( parent, 0, sizeof(short int) * MAX_NODES );
	memset( gcost, -1, sizeof(float) * MAX_NODES );

	//make sure we have valid data before calculating everything
	if ( (from == NODE_INVALID) || (to == NODE_INVALID) || (from >= MAX_NODES) || (to >= MAX_NODES) || (from == to) )
	{
		return ( -1 );
	}

	openlist[1] = from;																	//add the starting node to the open list
	numOpen++;
	gcost[from] = 0;																	//its f and g costs are obviously 0
	fcost[from] = 0;
	while ( 1 )
	{
		if ( numOpen != 0 )																//if there are still items in the open list
		{

			//pop the top item off of the list
			atNode = openlist[1];
			list[atNode] = 2;															//put the node on the closed list so we don't check it again
			numOpen--;
			openlist[1] = openlist[numOpen + 1];										//move the last item in the list to the top position
			v = 1;

			//this while loop reorders the list so that the new lowest fcost is at the top again
			while ( 1 )
			{
				u = v;
				if ( (2 * u + 1) < numOpen )											//if NPCh children exist
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
					if ( (2 * u) < numOpen )											//if only one child exists
					{
						if ( fcost[openlist[u]] >= fcost[openlist[2 * u]] )
						{
							v = 2 * u;
						}
					}
				}

				if ( u != v )															//if they're out of order, swap this item with its parent
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

			for ( i = 0; i < nodes[atNode].enodenum; i++ )								//loop through all the links for this node
			{
				newnode = nodes[atNode].links[i].targetNode;
				if ( nodes[atNode].links[i].flags & PATH_BLOCKED )
				{																		//if this path is blocked, skip it
					continue;
				}

				//skip any unreachable nodes
				if ( (nodes[newnode].type & NODE_ALLY_UNREACHABLE) && (NPC->s.teamNum == TEAM_ALLIES) )
				{
					continue;
				}

				if ( (nodes[newnode].type & NODE_AXIS_UNREACHABLE) && (NPC->s.teamNum == TEAM_AXIS) )
				{
					continue;
				}

				if ( list[newnode] == 2 )
				{																		//if this node is on the closed list, skip it
					continue;
				}

				if ( list[newnode] != 1 )												//if this node is not already on the open list
				{
					openlist[++numOpen] = newnode;										//add the new node to the open list
					list[newnode] = 1;
					parent[newnode] = atNode;											//record the node's parent
					if ( newnode == to )
					{																	//if we've found the goal, don't keep computing paths!
						break;															//this will break the 'for' and go all the way to 'if (list[to] == 1)'
					}

					fcost[newnode] = GetFCost( to, newnode, parent[newnode], gcost );	//store it's f cost value

					//this loop re-orders the heap so that the lowest fcost is at the top
					m = numOpen;
					while ( m != 1 )													//while this item isn't at the top of the heap already
					{
						if ( fcost[openlist[m]] <= fcost[openlist[m / 2]] )				//if it has a lower fcost than its parent
						{
							temp = openlist[m / 2];
							openlist[m / 2] = openlist[m];
							openlist[m] = temp;											//swap them
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
					VectorSubtract( nodes[newnode].origin, nodes[atNode].origin, vec );
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
								fcost[newnode] = GetFCost( to, newnode, parent[newnode], gcost );

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
		count = 0;
		temp = to;						//start at the end point
		while ( temp != from )			//travel along the path (backwards) until we reach the starting point
		{
			pathlist[count++] = temp;	//add the node to the pathlist and increment the count
			temp = parent[temp];		//move to the parent of this node to continue the path
		}

		pathlist[count++] = from;		//add the beginning node to the end of the pathlist
	}

	if ( !found && HaveSequentialNodes() )
	{
		count = NPC_CreateDumbRoute( from, to, pathlist );
	}

	if ( count > 0 )
	{
		return ( count );
	}

	return ( count );					//return the number of nodes in the path, -1 if not found
}

extern int	BotMP_Find_POP_Goal_EntityNum ( gentity_t *bot, int ignoreEnt, int ignoreEnt2, vec3_t current_org );	// ai_dmgoal_mp.c
extern int	BotMP_Engineer_Find_POP_Goal_EntityNum
			(
				gentity_t	*bot,
				int			ignoreEnt,
				int			ignoreEnt2,
				vec3_t		current_org,
				int			teamNum
			);
extern int	BotMP_FieldOps_Find_POP_Goal_EntityNum
			(
				gentity_t	*bot,
				int			ignoreEnt,
				int			ignoreEnt2,
				vec3_t		current_org,
				int			teamNum
			);
extern int	BotMP_Soldier_Find_POP_Goal_EntityNum
			(
				gentity_t	*bot,
				int			ignoreEnt,
				int			ignoreEnt2,
				vec3_t		current_org,
				int			teamNum
			);
extern int	BotMP_Sniper_Find_Goal_EntityNum
			(
				gentity_t	*bot,
				int			ignoreEnt,
				int			ignoreEnt2,
				vec3_t		current_org,
				int			teamNum
			);
extern void UpdateAttackerDirection ( void );


/* */
int
NPC_FindGoal ( gentity_t *NPC )
{							// Returns goal waypoint...
	int		entNum = -1;
	int		best_target = -1;
	float	best_dist = 99999.9f;

	// Update direction for defence bots/NPCs...
	UpdateAttackerDirection();
	best_target = -1;		// Initialize with each loop...
	best_dist = 99999.9f;	// Initialize with each loop...
	if ( NPC->playerType == PC_ENGINEER )
	{
		entNum = BotMP_Engineer_Find_POP_Goal_EntityNum( NPC, NPC->s.number, NPC->current_target_entity,
														 NPC->r.currentOrigin, NPC->s.teamNum );
	}
	else if ( NPC->playerType == PC_SOLDIER )
	{
		entNum = BotMP_Soldier_Find_POP_Goal_EntityNum( NPC, NPC->s.number, NPC->current_target_entity,
														NPC->r.currentOrigin, NPC->s.teamNum );
	}
	else if ( NPC->playerType == PC_FIELDOPS )
	{
		entNum = BotMP_FieldOps_Find_POP_Goal_EntityNum( NPC, NPC->s.number, NPC->current_target_entity,
														 NPC->r.currentOrigin, NPC->s.teamNum );
	}
	else
	{
		entNum = BotMP_Find_POP_Goal_EntityNum( NPC, NPC->s.number, NPC->current_target_entity, NPC->r.currentOrigin );
	}

	if ( entNum >= 0 )
	{
		int loop = 0;
		for ( loop = 0; loop < number_of_nodes; loop++ )
		{					// Find closest node(s) to the objective...
			float	dist = VectorDistance( nodes[loop].origin, g_entities[entNum].r.currentOrigin );
			if ( dist <= 256 && dist < best_dist )
			{
				memset( NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
				best_dist = VectorDistance( nodes[loop].origin, g_entities[entNum].r.currentOrigin );
				best_target = loop;
			}
		}
	}

	if ( best_target >= 0 )
	{						// Found a goal.. Use it...
		NPC->current_target_entity = entNum;
		return ( best_target );
	}

	// If we got here, then we failed to find a path to a goal...
	return ( -1 );
}

extern vmCvar_t		npc_simplenav;
extern short int	BotGetNextNode ( gentity_t *bot /*, int *state*/ ); // unique_movement.c

//===========================================================================
// Routine      : NPC_MOVEMENT_ReachableBy
// Description  : Determine if a blocked goal vector is reachable.. Returns how to get there...
#define NOT_REACHABLE			- 1
#define REACHABLE				0
#define REACHABLE_JUMP			1
#define REACHABLE_DUCK			2
#define REACHABLE_STRAFE_LEFT	3
#define REACHABLE_STRAFE_RIGHT	4


/* */
int
NPC_MOVEMENT_ReachableBy ( gentity_t *NPC, vec3_t goal )
{
	trace_t trace;
	vec3_t	v, v2, eyes, mins, maxs, Org, forward, right, up, rightOrigin, leftOrigin;

	// First check direct movement...
	VectorCopy( NPC->r.currentOrigin, Org );
	VectorCopy( NPC->r.mins, v );
	v[2] += 18;								// Stepsize
	trap_Trace( &trace, Org, v, NPC->r.maxs, goal, NPC->s.number, MASK_SOLID | MASK_OPAQUE );
	if ( trace.fraction == 1.0 )
	{
		return ( REACHABLE );				// Yes we can see it
	}

	// Now look for jump access...
	VectorAdd( Org, playerMins, mins );
	VectorAdd( Org, playerMaxs, maxs );
	VectorCopy( NPC->r.mins, v );
	v[2] += 18;								// Stepsize
	VectorCopy( Org, eyes );
	eyes[2] += 32;
	trap_Trace( &trace, eyes, v, NPC->r.maxs, goal, NPC->s.number, MASK_SOLID | MASK_OPAQUE );
	if ( trace.fraction == 1.0 )
	{
		return ( REACHABLE_JUMP );			// Yes we can see it
	}

	// Now look for crouch access...
	//CROUCH_VIEWHEIGHT ??
	VectorCopy( NPC->r.mins, v );
	v[2] += 18;								// Stepsize
	VectorCopy( NPC->r.maxs, v2 );
	v2[2] *= 0.5;							// Stepsize
	trap_Trace( &trace, Org, v, v2, goal, NPC->s.number, MASK_SOLID | MASK_OPAQUE );
	if ( trace.fraction == 1.0 )
	{
		return ( REACHABLE_DUCK );			// Yes we can see it
	}

	// Now look for strafe access... Left or Right...
	AngleVectors( NPC->NPC_client->ps.viewangles, forward, right, up );
	VectorMA( NPC->r.currentOrigin, 64, right, rightOrigin );
	VectorMA( NPC->r.currentOrigin, -64, right, leftOrigin );
	if ( OrgVisible( leftOrigin, nodes[NPC->current_node].origin, NPC->s.number) )
	{
		NPC->bot_strafe_left_timer = level.time + 201;
		VectorCopy( leftOrigin, NPC->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( OrgVisible( rightOrigin, nodes[NPC->current_node].origin, NPC->s.number) )
	{
		NPC->bot_strafe_right_timer = level.time + 201;
		VectorCopy( rightOrigin, NPC->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	// And strafing a bit further away...
	VectorMA( NPC->r.currentOrigin, 128, right, rightOrigin );
	VectorMA( NPC->r.currentOrigin, -128, right, leftOrigin );
	if ( OrgVisible( leftOrigin, nodes[NPC->current_node].origin, NPC->s.number) )
	{
		NPC->bot_strafe_left_timer = level.time + 401;
		VectorCopy( leftOrigin, NPC->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( OrgVisible( rightOrigin, nodes[NPC->current_node].origin, NPC->s.number) )
	{
		NPC->bot_strafe_right_timer = level.time + 401;
		VectorCopy( rightOrigin, NPC->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	// And strafing a bit further away...
	VectorMA( NPC->r.currentOrigin, 192, right, rightOrigin );
	VectorMA( NPC->r.currentOrigin, -192, right, leftOrigin );
	if ( OrgVisible( leftOrigin, nodes[NPC->current_node].origin, NPC->s.number) )
	{
		NPC->bot_strafe_left_timer = level.time + 601;
		VectorCopy( leftOrigin, NPC->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_LEFT );	// Yes we can see it
	}

	if ( OrgVisible( rightOrigin, nodes[NPC->current_node].origin, NPC->s.number) )
	{
		NPC->bot_strafe_right_timer = level.time + 601;
		VectorCopy( rightOrigin, NPC->bot_strafe_target_position );
		return ( REACHABLE_STRAFE_RIGHT );	// Yes we can see it
	}

	return ( NOT_REACHABLE );
}

extern void MuzzleRayTracing ( gentity_t *, int );
extern void ClientEvents ( gentity_t *ent, int oldEventSequence );
extern void ClientImpacts ( gentity_t *ent, pmove_t *pm );


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
G_StoreNPCPosition ( gentity_t *ent )
{
	int				currentTime;
	clientMarker_t	*pMarker;

	if (!ent->NPC_client || ent->s.eType != ET_NPC)
		return;

	if
	(
		ent->inuse &&
		(ent->NPC_client->sess.sessionTeam == TEAM_AXIS || ent->NPC_client->sess.sessionTeam == TEAM_ALLIES) &&
		ent->r.linked &&
		ent->health > 0 &&
		(ent->NPC_client->ps.pm_flags & PMF_LIMBO) == 0 &&
		ent->NPC_client->ps.pm_type == PM_NORMAL
	)
	{
		pMarker = ent->NPC_client->clientMarkers + ent->NPC_client->topMarker;

		// new frame, mark the old marker's time as the end of the last frame
		if ( pMarker->time < level.time )
		{
			pMarker->time = level.previousTime;
			ent->NPC_client->topMarker =
					(
						ent->NPC_client->topMarker == MAX_CLIENT_MARKERS -
						1
					) ? 0 : ent->NPC_client->topMarker +
					1;
			pMarker = ent->NPC_client->clientMarkers + ent->NPC_client->topMarker;
		}

		currentTime = level.previousTime + trap_Milliseconds() - 50/*bot_thinktime.integer*/; //level.FRAMETIME;
		if ( currentTime > level.time )
		{		// owwie, we just went into the next frame... let's push them back
			currentTime = level.time;
		}

		VectorCopy( ent->r.mins, pMarker->mins );
		VectorCopy( ent->r.maxs, pMarker->maxs );
		VectorCopy( ent->r.currentOrigin, pMarker->origin );
		pMarker->servertime = level.time;
		pMarker->time = currentTime;
#ifdef UNLAGGED // ??? Must be fixed ???
		pMarker->eFlags = ent->NPC_client->ps.eFlags;
		pMarker->pmf_flags = ent->NPC_client->ps.pm_flags;
#endif
	}
}


/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void
BG_NPCStateToEntityState ( playerState_t *ps, entityState_t *s, qboolean snap )
{
	int i;
	ps->clientNum = s->number;
	s->eType = ET_NPC;

	//s->number = ps->clientNum;
	s->pos.trType = TR_INTERPOLATE;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap )
	{
		SnapVector( s->pos.trBase );
	}

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap )
	{
		SnapVector( s->apos.trBase );
	}

	if ( ps->movementDir > 128 )
	{
		s->angles2[YAW] = (float) ps->movementDir - 256;
	}
	else
	{
		s->angles2[YAW] = ps->movementDir;
	}

	//	s->modelindex2	= ps->persistant[PERS_HWEAPON_USE];
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;

	//	s->clientNum	= ps->clientNum;	// ET_PLAYER looks here instead of at number
	// so corpses can also reference the proper config
	// Ridah, let clients know if this person is using a mounted weapon
	// so they don't show any client muzzle flashes
	if ( ps->eFlags & EF_MOUNTEDTANK )
	{
		ps->eFlags &= ~EF_MG42_ACTIVE;
		ps->eFlags &= ~EF_AAGUN_ACTIVE;
	}
	else
	{

		//		SETUP_MOUNTEDGUN_STATUS( ps );
	}

	s->eFlags = ps->eFlags;
	ps->stats[STAT_HEALTH] = g_entities[s->number].health;

	/*	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}*/

	// from MP
	if ( ps->externalEvent )
	{
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	}
	else if ( ps->entityEventSequence < ps->eventSequence )
	{
		int seq;
		if ( ps->entityEventSequence < ps->eventSequence - MAX_EVENTS )
		{
			ps->entityEventSequence = ps->eventSequence - MAX_EVENTS;
		}

		seq = ps->entityEventSequence & ( MAX_EVENTS - 1 );
		s->event = ps->events[seq] | ( (ps->entityEventSequence & 3) << 8 );
		s->eventParm = ps->eventParms[seq];
		ps->entityEventSequence++;
	}

	// end
	// Ridah, now using a circular list of events for all entities
	// add any new events that have been added to the playerState_t
	// (possibly overwriting entityState_t events)
	for ( i = ps->oldEventSequence; i != ps->eventSequence; i++ )
	{
		s->events[s->eventSequence & ( MAX_EVENTS - 1 )] = ps->events[i & ( MAX_EVENTS - 1 )];
		s->eventParms[s->eventSequence & ( MAX_EVENTS - 1 )] = ps->eventParms[i & ( MAX_EVENTS - 1 )];
		s->eventSequence++;
	}

	ps->oldEventSequence = ps->eventSequence;
	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;
	s->powerups = 0;
	for ( i = 0; i < MAX_POWERUPS; i++ )
	{
		if ( ps->powerups[i] )
		{
			s->powerups |= 1 << i;
		}
	}

#ifdef __LEANING__
	s->constantLight = ps->leanf * 1000;	// Does really *1000 make it better?
#endif
	s->nextWeapon = ps->nextWeapon;			// Ridah

	//	s->loopSound = ps->loopSound;
	//	s->teamNum = ps->teamNum;
#ifdef __RW__
	s->curweapAnim = ps->weapAnim;
	s->aiState = AISTATE_COMBAT;
#else
	s->aiState = ps->aiState;				// xkan, 1/10/2003
#endif
}


/*
========================
BG_NPCStateToEntityStateExtraPolate

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void
BG_NPCStateToEntityStateExtraPolate ( playerState_t *ps, entityState_t *s, int time, qboolean snap )
{
	int i;
	ps->clientNum = s->number;
	s->eType = ET_NPC;

	//s->number = ps->clientNum;
	s->pos.trType = TR_LINEAR_STOP;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap )
	{
		SnapVector( s->pos.trBase );
	}

	// set the trDelta for flag direction and linear prediction
	VectorCopy( ps->velocity, s->pos.trDelta );

	// set the time for linear prediction
	s->pos.trTime = time;

	// set maximum extra polation time
	s->pos.trDuration = 50;			// 1000 / sv_fps (default = 20)
	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap )
	{
		SnapVector( s->apos.trBase );
	}

	//G_Printf("Movedir is %i.\n",ps->movementDir);
	s->angles2[YAW] = ps->movementDir;

	//	s->modelindex2	= ps->persistant[PERS_HWEAPON_USE];
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;

	//	s->clientNum = ps->clientNum;		// ET_PLAYER looks here instead of at number
	// so corpses can also reference the proper config
	if ( ps->eFlags & EF_MOUNTEDTANK )
	{
		ps->eFlags &= ~EF_MG42_ACTIVE;
		ps->eFlags &= ~EF_AAGUN_ACTIVE;
	}
	else
	{

		//SETUP_MOUNTEDGUN_STATUS( ps );
	}

	s->eFlags = ps->eFlags;
	ps->stats[STAT_HEALTH] = g_entities[s->number].health;

	/*	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}*/
	if ( ps->externalEvent )
	{
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	}
	else if ( ps->entityEventSequence < ps->eventSequence )
	{
		int seq;
		if ( ps->entityEventSequence < ps->eventSequence - MAX_EVENTS )
		{
			ps->entityEventSequence = ps->eventSequence - MAX_EVENTS;
		}

		seq = ps->entityEventSequence & ( MAX_EVENTS - 1 );
		s->event = ps->events[seq] | ( (ps->entityEventSequence & 3) << 8 );
		s->eventParm = ps->eventParms[seq];
		ps->entityEventSequence++;
	}

	// Ridah, now using a circular list of events for all entities
	// add any new events that have been added to the playerState_t
	// (possibly overwriting entityState_t events)
	for ( i = ps->oldEventSequence; i != ps->eventSequence; i++ )
	{
		s->events[s->eventSequence & ( MAX_EVENTS - 1 )] = ps->events[i & ( MAX_EVENTS - 1 )];
		s->eventParms[s->eventSequence & ( MAX_EVENTS - 1 )] = ps->eventParms[i & ( MAX_EVENTS - 1 )];
		s->eventSequence++;
	}

	ps->oldEventSequence = ps->eventSequence;
	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;
	s->powerups = 0;
	for ( i = 0; i < MAX_POWERUPS; i++ )
	{
		if ( ps->powerups[i] )
		{
			s->powerups |= 1 << i;
		}
	}

	s->nextWeapon = ps->nextWeapon; // Ridah

	//	s->teamNum = ps->teamNum;
#ifdef __RW__
	s->curweapAnim = ps->weapAnim;
	s->aiState = AISTATE_COMBAT;	// xkan, 1/10/2003
#else
	s->aiState = ps->aiState;		// xkan, 1/10/2003
#endif
}


/*
==================
SetNPCClientViewAngle

==================
*/
void
SetNPCClientViewAngle ( gentity_t *ent, vec3_t angle )
{
	int i;

	// set the delta angle
	for ( i = 0; i < 3; i++ )
	{
		int cmdAngle;
		cmdAngle = ANGLE2SHORT( angle[i] );
		ent->NPC_client->ps.delta_angles[i] = cmdAngle - ent->NPC_client->pers.cmd.angles[i];
	}

	VectorCopy( angle, ent->s.angles );
	VectorCopy( ent->s.angles, ent->NPC_client->ps.viewangles );
}


/* */
void
SetNPCClientViewAnglePitch ( gentity_t *ent, vec_t angle )
{
	int cmdAngle;
	cmdAngle = ANGLE2SHORT( angle );
	ent->NPC_client->ps.delta_angles[PITCH] = cmdAngle - ent->NPC_client->pers.cmd.angles[PITCH];
	ent->s.angles[PITCH] = 0;
	VectorCopy( ent->s.angles, ent->NPC_client->ps.viewangles );
}


/*
==============
NPCSetViewAngle
==============
*/
float
NPCSetViewAngle ( float angle, float ideal_angle, float speed )
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


/*
==============
NPCSetViewAngles
==============
*/
void
NPCSetViewAngles ( gentity_t *NPC, float thinktime )
{	// Just keep NPC->bot_ideal_view_angles correct and call this to make it realistic...
	float	diff, factor, maxchange, anglespeed;
	int		i;
	if ( NPC->bot_ideal_view_angles[PITCH] > 180 )
	{
		NPC->bot_ideal_view_angles[PITCH] -= 360;
	}

	//
	factor = 0.15;
	maxchange = 240;
	maxchange *= thinktime;
	for ( i = 0; i < 2; i++ )
	{
		diff = fabs( AngleDifference( NPC->NPC_client->ps.viewangles[i], NPC->bot_ideal_view_angles[i]) );
		anglespeed = diff * factor;
		if ( anglespeed > maxchange )
		{
			anglespeed = maxchange;
		}

		NPC->NPC_client->ps.viewangles[i] = NPCSetViewAngle( NPC->NPC_client->ps.viewangles[i],
															 NPC->bot_ideal_view_angles[i], anglespeed );
	}

	if ( NPC->NPC_client->ps.viewangles[PITCH] > 180 )
	{
		NPC->NPC_client->ps.viewangles[PITCH] -= 360;
	}

	//trap_EA_View(NPC->s.number, NPC->NPC_client->ps.viewangles);
	//VectorCopy(NPC->NPC_client->ps.viewangles, NPC->s.angles);
	SetNPCClientViewAngle( NPC, NPC->NPC_client->ps.viewangles );
}


/* */
static entityState_t *
G_NPCGetEntityState ( int entityNum )
{
	if ( entityNum < 0 || entityNum >= MAX_GENTITIES )
	{
		return ( NULL );
	}

	return ( &g_entities[entityNum].s );
}

void	NPCThink ( int entityNum ); // below...


/* */
void
NPCThink_real ( gentity_t *ent )
{
	int			msec, oldEventSequence, monsterslick = 0;
	pmove_t		pm;
	usercmd_t	*ucmd;
	gclient_t	*client = ent->NPC_client;

	if (ent->s.eType != ET_NPC || !ent->NPC_client)
		return;

#ifdef __RW__
	if ( client->ps.weapon != WP_KNIFE2 )
	{
		client->ps.extFlags &= ~EX_DEFUSING;
	}
#endif

	/*
	if( ent->s.dl_intensity == TAG_HOOK || ent->s.dl_intensity == TAG_HOOK_DRIVER || ent->s.dl_intensity == TAG_HOOK_GUNNER1 || ent->s.dl_intensity == TAG_HOOK_GUNNER2 ) {
		client->pmext.centerangles[YAW] = g_entities[ent->s.otherEntityNum].r.currentAngles[ YAW ];
		client->pmext.centerangles[PITCH] = g_entities[ent->s.otherEntityNum].r.currentAngles[ YAW ];
	}
*/
	if ( (ent->s.eFlags & EF_MOUNTEDTANK) && ent->tagParent )
	{
		client->pmext.centerangles[YAW] = ent->tagParent->r.currentAngles[YAW];
		client->pmext.centerangles[PITCH] = ent->tagParent->r.currentAngles[PITCH];
	}

#ifdef _DEBUG
	if ( g_debugBullets.integer > 1 )
	{
		if ( !ent->NPC_client->ps.persistant[PERS_HWEAPON_USE] )
		{
			MuzzleRayTracing( ent, ent->s.weapon );
		}
	}
#endif

	// mark the time, so the connection sprite can be removed
	ucmd = &ent->NPC_client->pers.cmd;
	ent->NPC_client->ps.identifyClient = ucmd->identClient; // NERVE - SMF

	// sanity check the command time to prevent speedup cheating
	if ( ucmd->serverTime > level.time + 200 )
	{
		ucmd->serverTime = level.time + 200;

		//		G_Printf("serverTime <<<<<\n" );
	}

	if ( ucmd->serverTime < level.time - 1000 )
	{
		ucmd->serverTime = level.time - 1000;

		//		G_Printf("serverTime >>>>>\n" );
	}

	//G_Printf("NPC %i - Forward: %f. Right %f.\n", ent->s.number, ucmd->forwardmove, ucmd->rightmove);
	msec = ucmd->serverTime - client->ps.commandTime;

	// following others may result in bad times, but we still want
	// to check for follow toggles
	if ( msec > 200 )
	{
		msec = 200;
	}

	if ( pmove_fixed.integer || client->pers.pmoveFixed )
	{
		ucmd->serverTime = ( (ucmd->serverTime + pmove_msec.integer - 1) / pmove_msec.integer ) * pmove_msec.integer;
	}

	if
	(
		!(ucmd->flags & 0x01) ||
		ucmd->forwardmove ||
		ucmd->rightmove ||
		ucmd->upmove ||
		ucmd->wbuttons ||
		ucmd->doubleTap
	)
	{
		ent->r.svFlags &= ~( SVF_SELF_PORTAL_EXCLUSIVE | SVF_SELF_PORTAL );
	}

	if ( level.match_pause != PAUSE_NONE )
	{
		ucmd->buttons = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		ucmd->wbuttons = 0;
		ucmd->doubleTap = 0;

		// freeze player (RELOAD_FAILED still allowed to move/look)
		if ( level.match_pause != PAUSE_NONE )
		{
			client->ps.pm_type = PM_FREEZE;
		}
	}
	else if ( client->noclip )
	{
		client->ps.pm_type = PM_NOCLIP;
	}
	else if ( client->ps.stats[STAT_HEALTH] <= 0 )
	{
		client->ps.pm_type = PM_DEAD;
	}
	else
	{
		client->ps.pm_type = PM_NORMAL;
	}

#ifdef __RW__
	client->ps.pm_flags2 = 0;
#else
	client->ps.aiState = AISTATE_COMBAT;
#endif
	client->ps.gravity = g_gravity.value;

	// set speed
	//client->ps.speed = g_speed.value;
#define MOVEMENT_SPEED	235
	client->ps.speed = MOVEMENT_SPEED + g_speed.value;
	if ( client->speedScale )
	{														// Goalitem speed scale
		client->ps.speed *= ( client->speedScale * 0.01 );
	}

	// set up for pmove
	oldEventSequence = client->ps.eventSequence;
	client->currentAimSpreadScale = (float) client->ps.aimSpreadScale / 255.0;
	memset( &pm, 0, sizeof(pm) );
	pm.ps = &client->ps;
	pm.ps->pm_type = PM_NORMAL;
	pm.ps->runSpeedScale = 0.8f;
	VectorCopy( ent->r.mins, pm.mins );
	VectorCopy( ent->r.maxs, pm.maxs );
	pm.isNPC = qtrue;
	pm.entityNum = ent->s.number;

	//	pm.ps->clientNum = ent->s.number;
	//	ent->s.clientNum = ent->s.number;
	pm.pmext = &client->pmext;
	if ( ent->playerType < 0 )
	{
		ent->playerType = PC_SOLDIER;
	}

	pm.character = client->pers.character = ent->character = BG_GetCharacter( ent->s.teamNum, ent->playerType );;
	ent->s.weapon = ucmd->weapon = ent->NPC_client->ps.weapon = ent->weaponchoice;
	pm.cmd = *ucmd;
	pm.oldcmd = client->pers.oldcmd;

	// MrE: always use capsule for AI and player
	pm.trace = trap_TraceCapsule;
	if ( pm.ps->pm_type == PM_DEAD )
	{
		pm.tracemask = MASK_PLAYERSOLID &~CONTENTS_BODY;

		// DHM-Nerve added:: EF_DEAD is checked for in Pmove functions, but wasn't being set
		//              until after Pmove
		pm.ps->eFlags |= EF_DEAD;

		// dhm-Nerve end
	}
	else if ( pm.ps->pm_type == PM_SPECTATOR )
	{
		pm.trace = trap_TraceCapsuleNoEnts;
	}
	else
	{
		pm.tracemask = MASK_PLAYERSOLID;
	}

	//DHM - Nerve :: We've gone back to using normal bbox traces
	//pm.trace = trap_Trace;
	pm.pointcontents = trap_PointContents;
	pm.getentitystate = G_NPCGetEntityState;
	pm.debugLevel = g_debugMove.integer;
	pm.noFootsteps = qfalse;
	pm.pmove_fixed = pmove_fixed.integer | client->pers.pmoveFixed;
	pm.pmove_msec = pmove_msec.integer;
	pm.noWeapClips = qfalse;
	VectorCopy( client->ps.origin, client->oldOrigin );

	// NERVE - SMF
	pm.gametype = g_gametype.integer;
	pm.ltChargeTime = level.lieutenantChargeTime[ent->s.teamNum - 1];
	pm.soldierChargeTime = level.soldierChargeTime[ent->s.teamNum - 1];
	pm.engineerChargeTime = level.engineerChargeTime[ent->s.teamNum - 1];
	pm.medicChargeTime = level.medicChargeTime[ent->s.teamNum - 1];

	// -NERVE - SMF
	pm.skill = client->sess.skill;
	client->pmext.airleft = ent->NPC_client->airOutTime - level.time;
	pm.covertopsChargeTime = level.covertopsChargeTime[client->sess.sessionTeam - 1];
	pm.leadership = qfalse;

	// Gordon: bit hacky, stop the slight lag from client -> server even on locahost, switching back to the weapon you were holding
	//			and then back to what weapon you should have, became VERY noticible for the kar98/carbine + gpg40, esp now i've added the
	//			animation locking
	if ( level.time - client->pers.lastSpawnTime < 1000 )
	{
		pm.cmd.weapon = client->ps.weapon;
	}

	pm.ps->clientNum = ent->s.number;
	monsterslick = Pmove( &pm );

	// Gordon: thx to bani for this
	// ikkyo - fix leaning players bug
	VectorCopy( client->ps.velocity, ent->s.pos.trDelta );
	SnapVector( ent->s.pos.trDelta );

	// end
	// server cursor hints
	// TAT 1/10/2003 - bots don't need to check for cursor hints
	ent->s.animMovetype = 0;

	// DHM - Nerve :: Set animMovetype to 1 if ducking
	if ( ent->NPC_client->ps.pm_flags & PMF_DUCKED )
	{
		ent->s.animMovetype |= 1;
	}

	// save results of pmove
	if ( ent->NPC_client->ps.eventSequence != oldEventSequence )
	{
		ent->eventTime = level.time;
		ent->r.eventTime = level.time;
	}

	if ( g_smoothClients.integer )
	{
		BG_NPCStateToEntityStateExtraPolate( &ent->NPC_client->ps, &ent->s, ent->NPC_client->ps.commandTime, qtrue );
	}
	else
	{
		BG_NPCStateToEntityState( &ent->NPC_client->ps, &ent->s, qtrue );
	}

	if ( !(ent->NPC_client->ps.eFlags & EF_FIRING) )
	{
		client->fireHeld = qfalse;							// for grapple
	}

	// use the snapped origin for linking so it matches client predicted versions
	VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );
	VectorCopy( pm.mins, ent->r.mins );
	VectorCopy( pm.maxs, ent->r.maxs );
	ent->waterlevel = pm.waterlevel;
	ent->watertype = pm.watertype;

	// link entity now, after any personal teleporters have been used
	trap_LinkEntity( ent );
	if ( !ent->NPC_client->noclip )
	{
		G_TouchTriggers( ent );
	}

	// NOTE: now copy the exact origin over otherwise clients can be snapped into solid
	VectorCopy( ent->NPC_client->ps.origin, ent->r.currentOrigin );

	if (ent->s.eType != ET_NPC || !ent->NPC_client)
		return;

	// store the client's current position for antilag traces
	G_StoreNPCPosition( ent );

	// touch other objects
	ClientImpacts( ent, &pm );

	// save results of triggers and client events
	if ( ent->NPC_client->ps.eventSequence != oldEventSequence )
	{
		ent->eventTime = level.time;
	}

	// swap and latch button actions
	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons = client->buttons &~client->oldbuttons;

	//	client->latched_buttons |= client->buttons & ~client->oldbuttons;	// FIXME:? (SA) MP method (causes problems for us.  activate 'sticks')
	//----(SA)	added
	client->oldwbuttons = client->wbuttons;
	client->wbuttons = ucmd->wbuttons;
	client->latched_wbuttons = client->wbuttons &~client->oldwbuttons;

	//	client->latched_wbuttons |= client->wbuttons & ~client->oldwbuttons;	// FIXME:? (SA) MP method
	// Rafael - Activate
	// Ridah, made it a latched event (occurs on keydown only)
	if ( client->latched_buttons & BUTTON_ACTIVATE )
	{
		Cmd_Activate_f( ent );
	}
}


/*
==================
NPCThink

A new command has arrived from the client
==================
*/
void
NPCThink ( int entityNum )
{
	gentity_t	*ent;
	ent = g_entities + entityNum;
	ent->NPC_client->pers.oldcmd = ent->NPC_client->pers.cmd;

	// mark the time we got info, so we can display the
	// phone jack if they don't get any for a while
	//	ent->NPC_client->lastCmdTime = level.time;
#ifdef ALLOW_GSYNC
	if ( !g_synchronousClients.integer )
#endif // ALLOW_GSYNC
	{
		NPCThink_real( ent );
	}
}


/*
==================
LookAtEnemy
==================
*/
void
LookAtEnemy ( gentity_t *NPC )
{
	vec3_t		dir, angles, flat_angles;
	usercmd_t	*ucmd = &NPC->NPC_client->pers.cmd;
	VectorSubtract( NPC->enemy->r.currentOrigin, NPC->r.currentOrigin, dir );
	vectoangles( dir, angles );
	VectorCopy( angles, flat_angles );

	/*	if (Q_TrueRand(0,1) == 0)
		flat_angles[YAW] += Q_TrueRand(0, 5-NPC->skillchoice);
	else
		flat_angles[YAW] -= Q_TrueRand(0, 5-NPC->skillchoice);

	if (Q_TrueRand(0,1) == 0)
		flat_angles[PITCH] += Q_TrueRand(0, 5-NPC->skillchoice);
	else
		flat_angles[PITCH] -= Q_TrueRand(0, 5-NPC->skillchoice);*/
	if ( NPC->skillchoice == 0 )
	{

		// Can't hit a good strafing player
		flat_angles[0] += ( random() - 0.5 ) * 20;
		flat_angles[1] += ( random() - 0.5 ) * 20;
	}
	else if ( NPC->skillchoice == 1 )
	{

		// Will get ya every now and then
		flat_angles[0] += ( random() - 0.5 ) * 10;
		flat_angles[1] += ( random() - 0.5 ) * 10;
	}
	else if ( NPC->skillchoice == 2 )
	{

		// Good competetion for some of the best players
		flat_angles[0] += ( random() - 0.5 ) * 5;
		flat_angles[1] += ( random() - 0.5 ) * 5;
	}
	else if ( NPC->skillchoice >= 3 )
	{

		// Never misses, unloads on ya!
		flat_angles[0] = flat_angles[0];
		flat_angles[1] = flat_angles[1];
	}

	flat_angles[ROLL] = 0;

	//G_SetAngle(NPC, flat_angles);
	VectorCopy( angles, NPC->bot_ideal_view_angles );
	NPCSetViewAngles( NPC, 100 );
	VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
	ucmd->angles[PITCH] = ANGLE2SHORT( NPC->s.angles[PITCH] );
	ucmd->angles[YAW] = ANGLE2SHORT( NPC->s.angles[YAW] );
	ucmd->angles[ROLL] = ANGLE2SHORT( NPC->s.angles[ROLL] );
}


/* */
int
Count_NPC_Players ( void )
{
	int loop = MAX_CLIENTS;
	int npccount = 0;
	while ( loop < MAX_GENTITIES )
	{
		if
		(
			g_entities[loop].s.eType == ET_NPC &&
			g_entities[loop].NPC_Class != CLASS_PLANE &&
			g_entities[loop].NPC_Class != CLASS_PLANE_BOMBER &&
			g_entities[loop].NPC_Class != CLASS_LCVP &&
			g_entities[loop].NPC_Class != CLASS_DESTROYER &&
			g_entities[loop].NPC_Class != CLASS_VEHICLE
		)
		{
			npccount++;
		}

		loop++;
	}

	return ( npccount );
}

int mounts_added = 0;


/* */
int
mg42_count ( void )
{
	int entnum;
	int count = 0;
	for ( entnum = MAX_CLIENTS; entnum < MAX_GENTITIES; entnum++ )
	{
		gentity_t	*ent = &g_entities[entnum];
		if ( !ent )
		{
			continue;
		}

		if ( ent->s.eType != ET_MG42_BARREL )
		{
			continue;
		}

		if ( ent->NPC_Class != CLASS_HUMANOID )
		{
			continue;
		}

		count++;
	}

	return ( count );
}


/* */
qboolean
mg42_scan ( int node )
{
	int entnum;
	for ( entnum = MAX_CLIENTS; entnum < MAX_GENTITIES; entnum++ )
	{
		gentity_t	*ent = &g_entities[entnum];
		if ( !ent )
		{
			continue;
		}

		if ( ent->s.eType != ET_MG42_BARREL )
		{
			continue;
		}

		if ( VectorDistance( nodes[node].origin, ent->s.origin) < 128 )
		{
			return ( qtrue );
		}
	}

	return ( qfalse );
}


/* */
gentity_t *
NPC_FindMG42 ( gentity_t *NPC )
{
	gentity_t	*mg42 = NULL;
	gentity_t	*best = NULL;
	int			entity = 0;
	float		best_dist = 999.9f;
	for ( entity = MAX_CLIENTS; entity < MAX_GENTITIES; entity++ )
	{
		mg42 = &g_entities[entity];
		if ( !mg42 )
		{
			continue;
		}

		if ( mg42->s.eType != ET_MG42_BARREL )
		{
			continue;
		}

		if ( VectorDistance( mg42->r.currentOrigin, NPC->r.currentOrigin) > best_dist )
		{
			continue;
		}

		best_dist = VectorDistance( mg42->r.currentOrigin, NPC->r.currentOrigin );
		best = mg42;
	}

	//G_Printf("NPC %i is mounted on entity %i!\n", NPC->s.number, best->s.number);
	return ( best );
}


/* */
qboolean
above_average_node_height ( int nodeNum )
{		// ... From a random sample ...
	int node;
	int test;
	int bads_found = 0;
	for ( test = 0; test < 64; test++ )
	{	// 16 tests... (quick)...
		node = Q_TrueRand( 0, number_of_nodes );
		if ( nodes[node].origin[2] >= nodes[nodeNum].origin[2] )
		{
			bads_found++;
		}
	}

	if ( bads_found > 8 /*18*/ )
	{
		return ( qfalse );
	}

	return ( qtrue );
}


/* */
qboolean
spawnpoint_close_to_bad_object ( int nodeNum )
{
	int entnum;
	for ( entnum = 64; entnum < MAX_GENTITIES; entnum++ )
	{
		gentity_t	*ent = &g_entities[entnum];
		if ( !ent )
		{
			continue;
		}

		if ( ent->s.eType == ET_MOVER )
		{
			if ( VectorDistance( ent->r.currentOrigin, nodes[nodeNum].origin) < 340 )
			{
				return ( qtrue );
			}
		}

		if ( ent->classname == "team_CTF_blueplayer" )
		{
			if ( VectorDistance( ent->s.origin, nodes[nodeNum].origin) < 1024 )
			{
				return ( qtrue );
			}
		}
	}

	return ( qfalse );
}

extern void BodySink ( gentity_t *ent );


/*
=============
NPC_CopyToBodyQue

An NPC is dead, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/
/*gentity_t **/
void
NPC_CopyToBodyQue ( gentity_t *ent )
{
	gentity_t	*body;
	int			contents, i;
	trap_UnlinkEntity( ent );



	// if client is in a nodrop area, don't leave the body
	contents = trap_PointContents( ent->r.currentOrigin, -1 );
	if ( contents & CONTENTS_NODROP )
	{
// we have to do this here
	G_FadeItems( ent, MOD_SATCHEL );
	CalculateRanks();
	G_FreeEntity( ent );
// done 

		return;// ( NULL );
	}

	// grab a body que and cycle to the next one
	body = level.bodyQue[level.bodyQueIndex];
	level.bodyQueIndex = ( level.bodyQueIndex + 1 ) % BODY_QUEUE_SIZE;

	// Gordon: um, what on earth was this here for?
	//	trap_UnlinkEntity (body);
	body->s = ent->s;
	body->s.eFlags = EF_DEAD;	// clear EF_TALK, etc

	/*	if (ent->client->ps.eFlags & EF_HEADSHOT) {
		body->s.eFlags |= EF_HEADSHOT;			// make sure the dead body draws no head (if killed that way)
	}*/

	/*#ifdef __RW__

#ifdef __ETE__
	memcpy(body->LeftAmmosBank, ent->client->ps.ammo, sizeof(ent->client->ps.ammo));
	memcpy(body->LeftCurrentClip, ent->client->ps.ammoclip, sizeof(ent->client->ps.ammoclip));
#else
	memcpy(body->LeftAmmo, ent->client->ps.Ammo, sizeof(ent->client->ps.Ammo));
	memcpy(body->LeftAmmoClip, ent->client->ps.AmmoClip, sizeof(ent->client->ps.AmmoClip));
	if (ent->client->ps.powerups[PW_MASKOFGAS])
		body->s.powerups |= 1 << PW_MASKOFGAS;
	if (ent->client->ps.powerups[PW_NAKED])
		body->s.powerups |= 1 << PW_NAKED;
#endif

	memcpy(&body->LeftWeapons, &ent->client->ps.weapons, sizeof(ent->client->ps.weapons));
	body->LeftPrimaryWeapon = G_GetPrimaryWeaponForClient(ent->client);
#endif*/
	body->s.eType = ET_CORPSE;
	body->classname = "corpse";
	body->s.powerups = 0;		// clear powerups
	body->s.loopSound = 0;		// clear lava burning
	body->s.number = body - g_entities;
	body->timestamp = level.time;
	body->physicsObject = qtrue;
	body->physicsBounce = 0;	// don't bounce
	if ( body->s.groundEntityNum == ENTITYNUM_NONE )
	{
		body->s.pos.trType = TR_GRAVITY;
		body->s.pos.trTime = level.time;

		//VectorCopy(ent->client->ps.velocity, body->s.pos.trDelta);
		VectorSet( body->s.pos.trDelta, 0, 0, 0 );
	}
	else
	{
		body->s.pos.trType = TR_STATIONARY;
	}

	body->s.event = 0;

	// DHM - Clear out event system
	for ( i = 0; i < MAX_EVENTS; i++ )
	{
		body->s.events[i] = 0;
	}

	body->s.eventSequence = 0;

	// DHM - Nerve
	// change the animation to the last-frame only, so the sequence
	// doesn't repeat anew for the body
	switch ( body->s.legsAnim &~ANIM_TOGGLEBIT )
	{
		case BOTH_DEATH1:
		case BOTH_DEAD1:
		default:
			body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD1;
			break;
		case BOTH_DEATH2:
		case BOTH_DEAD2:
			body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD2;
			break;
		case BOTH_DEATH3:
		case BOTH_DEAD3:
			body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD3;
			break;
	}


#ifndef __RW__
	body->r.svFlags = ent->r.svFlags &~SVF_BOT;
#endif
	VectorCopy( ent->r.mins, body->r.mins );
	VectorCopy( ent->r.maxs, body->r.maxs );
	VectorCopy( ent->r.absmin, body->r.absmin );
	VectorCopy( ent->r.absmax, body->r.absmax );

	// ydnar: bodies have lower bounding box
	body->r.maxs[2] = 0;
	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	// DHM - Nerve :: allow bullets to pass through bbox
	// Gordon: need something to allow the hint for covert ops
	body->r.contents = CONTENTS_CORPSE;
	body->r.ownerNum = ent->r.ownerNum;
	BODY_TEAM( body ) = ent->s.teamNum;
	BODY_CLASS( body ) = ent->playerType;
	BODY_CHARACTER( body ) = ent->characterIndex;
	BODY_VALUE( body ) = 0;
	body->s.time2 = 0;
	body->activator = NULL;
	body->nextthink = level.time + BODY_TIME( ent->s.teamNum );
	body->think = BodySink;
	body->die = body_die;
#if defined( __RW__ ) && !defined( __ETE__ )
	BODY_VALUE( body ) = ent->nakedHealth;
#endif

	// don't take more damage if already gibbed
	if ( ent->health <= GIB_HEALTH )
	{
		body->takedamage = qfalse;
	}
	else
	{
		body->takedamage = qtrue;
	}

#ifdef __RW__
	body->r.contents |= CONTENTS_FLAME;
#endif
	VectorCopy( body->s.pos.trBase, body->r.currentOrigin );
	trap_LinkEntity( body );

// we have to do this here
	G_FadeItems( ent, MOD_SATCHEL );
	CalculateRanks();
	G_FreeEntity( ent );
// done 


	return;// ( body );
}


//===========================================================================
// Routine      : NPC_die

// Description  : When an NPC dies, use this instead of the player_die function... We have no client info...
void
NPC_die ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	qboolean				nogib = qtrue;
	gitem_t					*item = NULL;
	qboolean				killedintank = qfalse;

	if ( self->tankLink )
	{
		G_LeaveTank( self, qfalse );
		killedintank = qtrue;
	}

	if ( g_gamestate.integer == GS_INTERMISSION )
	{
		return;
	}

	G_AddEvent( self, EV_STOPSTREAMINGSOUND, 0 );


	self->enemy = attacker;
	/* // npcs may drop something but this wont work like it is now
	if ( item )
	{
		vec3_t		launchvel = { 0, 0, 0 };
		gentity_t	*flag = LaunchItem( item, self->r.currentOrigin, launchvel, self->s.number );
		flag->s.modelindex2 = self->s.otherEntityNum2;	// JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
		flag->message = self->message;					// DHM - Nerve :: also restore item name

		// Clear out player's temp copies
		self->s.otherEntityNum2 = 0;
		self->message = NULL;
	}*/

	self->takedamage = qtrue;							// can still be gibbed
	self->r.contents = CONTENTS_CORPSE;

	//self->s.angles[2] = 0;
	self->s.powerups = 0;
	self->s.loopSound = 0;
	trap_LinkEntity( self );
	if ( nogib )
	{
		int	anim;

		// normal death
		// for the no-blood option, we need to prevent the health
		// from going to gib level
		if ( self->health <= GIB_HEALTH )
		{
			self->health = GIB_HEALTH + 1;
		}

		anim = BG_GetAnimScriptEvent_NPC ( ANIM_ET_DMDEATH );

		self->NPC_client->torsoDeathAnim = self->s.torsoAnim = anim;
		self->NPC_client->legsDeathAnim = self->s.legsAnim = anim;

	//	G_AddEvent( self, EV_DEATH1 + 1, killer ); // note we have to do own death event for npcs

		// the body can still be gibbed
		self->die = body_die;

		// Make a dead body...
		self->think =  NPC_CopyToBodyQue;//( self );
		if ( killedintank )
		self->nextthink = level.time + 50;
			else 
		self->nextthink = level.time + 2000;

	} else {
		GibEntity( self, 0 ); // do a fancy gib
		G_FreeEntity( self );

	}

}


/* */
void
SetNPCWeapon ( gentity_t *NPC )
{

	// The new way to spawn bot weapons.. Look up ammo table instead..
	// Should allow us to add more weapons easier! Maybe we should use this for normal clients too???
	int					pc = PC_SOLDIER;
	int					loop = 0;
	int					primaryCount = 0;

	//	int					nadesCount		= 0;
	bg_playerclass_t	*classInfo = NULL;
	if ( g_gametype.integer == GT_COOP || g_gametype.integer == GT_SINGLE_PLAYER )
	{	// Loaded from .npc script...
		NPC->s.weapon = NPC->weaponchoice;
		return;
	}

	// Let's make sure the bots actually have a class first...
	pc = NPC->playerType;
	classInfo = BG_GetPlayerClassInfo( NPC->s.teamNum, pc );

	// And now find a main weapon we can use...
	while ( classInfo->classWeapons[loop] != WP_NONE && loop < MAX_WEAPS_PER_CLASS )
	{
		primaryCount++;
		loop++;
	}

	loop = 0;

	/*	// And now find a nade type we can use...
	while (classInfo->classWeapons[4][loop].weapon != WP_NONE && loop < MAX_WEAPS_PER_CLASS)
	{
		nadesCount++;
		loop++;
	}*/

	// Now select a weapon from the allowed list randomly...
	if ( primaryCount > 0 )
	{
		NPC->s.weapon = classInfo->classWeapons[Q_TrueRand( 0, primaryCount - 1 )];
	}

	// Now select a nade from the allowed list randomly...

	/*	if (nadesCount > 0)
		client->sess.playerGrenade = classInfo->classWeapons[4][Q_TrueRand(0, nadesCount - 1)].weapon;
*/
	NPC->weaponchoice = NPC->s.weapon;
}

extern gentity_t	*SelectSpectatorSpawnPoint ( vec3_t origin, vec3_t angles );
#ifdef __SUPREMACY__
extern gentity_t	*NPCSelectPOPSpawnpoint ( gentity_t *ent );
extern int			redtickets;
extern int			bluetickets;
#endif //__SUPREMACY__


//===========================================================================
// Routine      : NPC_Spawn

// Description  : Spawn an NPC.
void
NPC_Spawn ( void )
{
	gentity_t	*NPC = G_Spawn();
	gentity_t	*spawnPoint = NULL;
	vec3_t		spawn_origin, spawn_angles;
	NPC->s.teamNum = PickTeam( -1 );
	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	if ( NPC->s.teamNum != TEAM_AXIS && NPC->s.teamNum != TEAM_ALLIES )
	{
		spawnPoint = SelectSpectatorSpawnPoint( spawn_origin, spawn_angles );
	}
	else
	{														// FIXME: Add parachute support...
#ifdef __SUPREMACY__
		if
		(
			!level.intermissiontime &&
			(g_gametype.integer == GT_SUPREMACY || g_gametype.integer == GT_SUPREMACY_CAMPAIGN)
		)
		{
			spawnPoint = NPCSelectPOPSpawnpoint( NPC );
			if ( !spawnPoint )
			{
				return;
			}

			if ( NPC->s.teamNum == TEAM_AXIS )
			{												// Decrease the tickets for this team...
				redtickets--;
			}
			else
			{												// Decrease the tickets for this team...
				bluetickets--;
			}

			trap_SendServerCommand( -1, va( "tkt %i %i", redtickets, bluetickets) );
		}
		else
#endif //__SUPREMACY__
			spawnPoint = SelectCTFSpawnPoint( NPC->s.teamNum, TEAM_ACTIVE, spawn_origin, spawn_angles, 0 );
	}

	VectorCopy( spawnPoint->s.origin, NPC->s.origin );
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->playerType = Q_TrueRand( PC_SOLDIER, NUM_PLAYER_CLASSES - 1 ); //NPC_SOLDIER;

	//NPC->s.modelindex2 = NPC->playerType;
	NPC->s.modelindex2 = CLASS_HUMANOID;

	//NPC->s.density = NPC->playerType;
	//NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );

	//if (!NPC->NPC_client)
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

		// JPW NERVE -- zero out all ammo counts
		//memset(client->ps.Ammo, 0, sizeof(client->ps.Ammo));
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;
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
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = USE_NOTHING;

		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;

		//		client->ps.persistant[PERS_CHOOSED_WEAPON] = (client->sess.playerWeapon2 << 8) | client->sess.playerWeapon;
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

		// run the presend to set anything else
		//ClientEndFrame(ent);
		// clear entity state values
		BG_NPCStateToEntityState( &client->ps, &ent->s, qtrue );

		// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=569
		//G_ResetMarkers(ent);
		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	if ( NPC_NodesLoaded() )
	{
		if ( Q_irand( 0, 10) <= 1 )
		{										// Occasionally, one should also be an attacker...
			NPC->NPC_Humanoid_Type |= HUMANOID_TYPE_FODDER_ATTACKER;
		}

		NPC->NPC_Humanoid_Type |= HUMANOID_TYPE_FODDER;
	}

	//G_Printf("NPC Spawned...\n");
}


/* */
static qboolean
NPC_LoadData_ParseError ( int handle, char *format, ... )
{
	int			line;
	char		filename[128];
	va_list		argptr;
	static char string[4096];
	va_start( argptr, format );
	Q_vsnprintf( string, sizeof(string), format, argptr );
	va_end( argptr );
	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine( handle, filename, &line );
	if ( filename[0] )
	{
		Com_Printf( S_COLOR_RED "NPC_LoadData: ERROR - %s, line %d: %s\n", filename, line, string );
	}
	else
	{
		Com_Printf( S_COLOR_RED "NPC_LoadData: ERROR - %s\n", string );
	}

	trap_PC_FreeSource( handle );
	return ( qfalse );
}


/* */
qboolean
NPC_String_Parse ( int handle, char *out, size_t size )
{
	pc_token_t	token;
	if ( !trap_PC_ReadToken( handle, &token) )
	{
		return ( qfalse );
	}

	Q_strncpyz( out, token.string, size );
	return ( qtrue );
}

#define ACTION_NONE			0
#define ACTION_RUNANDHIDE	1
#define ACTION_GOTOENTITY	2
stringID_table_t	NPC_ActionTable[] =
{
	ENUM2STRING( ACTION_NONE ),
	ENUM2STRING( ACTION_RUNANDHIDE ),
	ENUM2STRING( ACTION_GOTOENTITY ),
};


/* */
qboolean
NPC_LoadData ( gentity_t *NPC, const char *szFileName )
{
	pc_token_t	token;
	int			handle;

	//	int			total	= 1;
	char		string[MAX_TOKEN_CHARS];
	handle = trap_PC_LoadSource( va( "npcs/%s.npc", szFileName) );
	if ( !handle )
	{
		return ( qfalse );
	}

	if ( !trap_PC_ReadToken( handle, &token) || Q_stricmp( token.string, "{") )
	{
		return ( NPC_LoadData_ParseError( handle, "expected '{'") );
	}

	while ( 1 )
	{
		if ( !trap_PC_ReadToken( handle, &token) )
		{
			return ( NPC_LoadData_ParseError( handle, "unexpected eof") );
		}

		if ( !Q_stricmp( token.string, "}") )
		{
			break;
		}
		else if ( !Q_stricmp( token.string, "class") )
		{
			if ( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS) )
			{
				return ( NPC_LoadData_ParseError( handle, "expected class type") );
			}
			else
			{
				NPC->playerType = (int) GetIDForString( NPC_ClassTable, string );
#ifdef _DEBUG
				G_Printf( "^4*** ^3NPC DEBUG^5: LoadNPC (^3%s^5): ^7%s^5 (^7%i^5).\n", szFileName, string,
						  NPC->playerType );
#endif //_DEBUG
			}
		}
		else if ( !Q_stricmp( token.string, "skill") )
		{
			if ( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS) )
			{
				return ( NPC_LoadData_ParseError( handle, "expected skill level") );
			}
			else
			{
				NPC->skillchoice = atoi( string );

				//G_Printf("^4*** ^3NPC DEBUG^5: Skill: ^7%i^5.\n", NPC->skillchoice);
			}
		}
		else if ( !Q_stricmp( token.string, "weapon") )
		{
			if ( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS) )
			{
				return ( NPC_LoadData_ParseError( handle, "expected weapon choice") );
			}
			else
			{
				NPC->weaponchoice = (int) GetIDForString( NPC_WeaponTable, string );

				//G_Printf("^4*** ^3NPC DEBUG^5: Weapon: ^7%s^5.\n", string);
			}
		}
		else if ( !Q_stricmp( token.string, "patrol") )
		{
			if ( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS) )
			{
				return ( NPC_LoadData_ParseError( handle, "expected patrol true/false") );
			}
			else
			{

				//G_Printf("^4*** ^3NPC DEBUG^5: Patrol: ^7%s^5.\n", string);
				if ( !Q_stricmp( string, "true") )
				{
					NPC->npc_patrol = qtrue;
				}
				else
				{
					NPC->npc_patrol = qfalse;
				}
			}
		}
		else if ( !Q_stricmp( token.string, "mounted") )
		{
			if ( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS) )
			{
				return ( NPC_LoadData_ParseError( handle, "expected mounted true/false") );
			}
			else
			{

				//G_Printf("^4*** ^3NPC DEBUG^5: Mounted: ^7%s^5.\n", string);
				if ( !Q_stricmp( string, "true") )
				{
					NPC->npc_mount = qtrue;
				}
				else
				{
					NPC->npc_mount = qfalse;
				}
			}
		}
		else if ( !Q_stricmp( token.string, "hunter") )
		{
			if ( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS) )
			{
				return ( NPC_LoadData_ParseError( handle, "expected hunter true/false") );
			}
			else
			{

				//G_Printf("^4*** ^3NPC DEBUG^5: Hunter: ^7%s^5.\n", string);
				if ( !Q_stricmp( string, "true") )
				{
					NPC->npc_hunter = qtrue;
				}
				else
				{
					NPC->npc_hunter = qfalse;
				}
			}
		}
		else if ( !Q_stricmp( token.string, "alertentity") )
		{
			if ( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS) )
			{
				return ( NPC_LoadData_ParseError( handle, "expected alertentity <number>") );
			}
			else
			{
				NPC->target_ent = &g_entities[atoi( string )];
			}
		}
		else if ( !Q_stricmp( token.string, "alertevent") )
		{
			if ( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS) )
			{
				return ( NPC_LoadData_ParseError( handle, "expected alertevent type") );
			}
			else
			{
				NPC->action = (int) GetIDForString( NPC_ActionTable, string );
			}
		}
	}

	trap_PC_FreeSource( handle );
	return ( qtrue );
}

//===========================================================================
// Routine      : NPC_CoopSpawn
// Description  : Spawn an NPC for coop gametype.
extern void		G_RegisterPlayerClasses ( void );
extern qboolean characters_loaded;


/* */
void
NPC_CoopSpawn ( gentity_t *NPC )
{

	//	trace_t tr;
	//	vec3_t end;
	if ( g_gametype.integer > GT_COOP )
	{
		G_FreeEntity( NPC );
		return;
	}

	if ( !characters_loaded )
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	// Find the ground from our spawnpoint... (for SP_NPC_Position)

	/*	VectorCopy(NPC->s.origin, end);
	end[2]-=1024;
	trap_Trace( &tr, NPC->s.origin, NULL, NULL, end, NPC->s.number, MASK_SHOT );
	VectorCopy(tr.endpos, NPC->s.origin);*/
	NPC->s.teamNum = TEAM_AXIS;								//PickTeam(-1);
	if ( NPC->spawnflags & TEAM_ALLIES )
	{
		NPC->s.teamNum = TEAM_ALLIES;						//PickTeam(-1);
	}

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	// Load the NPC script for NPC->scriptName
	if ( !NPC->scriptName )
	{
		NPC->scriptName = "default";
	}

	if ( !NPC_LoadData( NPC, NPC->scriptName) )
	{
		G_Printf( "Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName );
		NPC->scriptName = "default";
		if ( !NPC_LoadData( NPC, NPC->scriptName) )
		{
			G_Printf( "Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName );
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy( NPC->s.origin, NPC->SP_NPC_Position );

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	//NPC->s.modelindex2 = NPC->playerType;
	//NPC->s.modelindex2 = NPC->playerType;
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );

	//if (!NPC->NPC_client)
	{														// Set up an NPC client structure...
		gclient_t	*client;
		gentity_t	*ent = NPC;
		NPC->NPC_client = G_Alloc( sizeof(*client) );
		client = NPC->NPC_client;
		memset( client, 0, sizeof(*client) );
		memset( &client->ps, 0, sizeof(client->ps) );
		memset( &client->ps.weapons, 0, sizeof(client->ps.weapons) );
		memset( client->ps.ammo, 0, sizeof(client->ps.ammo) );
		memset( client->ps.ammoclip, 0, sizeof(client->ps.ammoclip) );

		// JPW NERVE -- zero out all ammo counts
		//memset(client->ps.Ammo, 0, sizeof(client->ps.Ammo));
		client->pers.connected = CON_CONNECTED;
		client->pers.connectTime = level.time;				// DHM - Nerve
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
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = USE_NOTHING;

		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;

		//		client->ps.persistant[PERS_CHOOSED_WEAPON] = (client->sess.playerWeapon2 << 8) | client->sess.playerWeapon;
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
		client->latched_wbuttons = 0;						//----(SA)	added

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

		// run the presend to set anything else
		//ClientEndFrame(ent);
		// clear entity state values
		BG_NPCStateToEntityState( &client->ps, &ent->s, qtrue );

		// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=569
		//G_ResetMarkers(ent);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;
		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	//G_Printf("NPC Spawned...\n");
	BotDropToFloor( NPC );
}


/* */
void
NPC_CoopSoldier ( gentity_t *NPC )
{
	if ( g_gametype.integer > GT_COOP )
	{
		G_FreeEntity( NPC );
		return;
	}

	if ( !characters_loaded )
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;
	if ( NPC->spawnflags & TEAM_ALLIES )
	{
		NPC->s.teamNum = TEAM_ALLIES;
	}

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	// Load the NPC script for NPC->scriptName
	if ( !NPC->scriptName )
	{
		NPC->scriptName = "default";
	}

	if ( !NPC_LoadData( NPC, NPC->scriptName) )
	{
		G_Printf( "Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName );
		NPC->scriptName = "default";
		if ( !NPC_LoadData( NPC, NPC->scriptName) )
		{
			G_Printf( "Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName );
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy( NPC->s.origin, NPC->SP_NPC_Position );

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );
	{														// Set up an NPC client structure...
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
		client->pers.connectTime = level.time;				// DHM - Nerve
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
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = USE_NOTHING;

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
		client->latched_wbuttons = 0;						//----(SA)	added

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

	//G_Printf("NPC (%s) spawned.\n", NPC->scriptName);
	BotDropToFloor( NPC );
}


/* */
void
NPC_CoopVenomSoldier ( gentity_t *NPC )
{
	if ( g_gametype.integer > GT_COOP )
	{
		G_FreeEntity( NPC );
		return;
	}

	if ( !characters_loaded )
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;
	if ( NPC->spawnflags & TEAM_ALLIES )
	{
		NPC->s.teamNum = TEAM_ALLIES;
	}

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	// Load the NPC script for NPC->scriptName
	if ( !NPC->scriptName )
	{
		NPC->scriptName = "mp42";
	}

	if ( !NPC_LoadData( NPC, NPC->scriptName) )
	{
		G_Printf( "Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName );
		NPC->scriptName = "default";
		if ( !NPC_LoadData( NPC, NPC->scriptName) )
		{
			G_Printf( "Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName );
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy( NPC->s.origin, NPC->SP_NPC_Position );

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );
	{														// Set up an NPC client structure...
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
		client->pers.connectTime = level.time;				// DHM - Nerve
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
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = USE_NOTHING;

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
		client->latched_wbuttons = 0;						//----(SA)	added

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

	//G_Printf("NPC (%s) spawned.\n", NPC->scriptName);
	BotDropToFloor( NPC );
}


/* */
void
NPC_CoopBlackGuard ( gentity_t *NPC )
{
	if ( g_gametype.integer > GT_COOP )
	{
		G_FreeEntity( NPC );
		return;
	}

	if ( !characters_loaded )
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;
	if ( NPC->spawnflags & TEAM_ALLIES )
	{
		NPC->s.teamNum = TEAM_ALLIES;
	}

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	// Load the NPC script for NPC->scriptName
	if ( !NPC->scriptName )
	{
		NPC->scriptName = "fieldops";
	}

	if ( !NPC_LoadData( NPC, NPC->scriptName) )
	{
		G_Printf( "Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName );
		NPC->scriptName = "default";
		if ( !NPC_LoadData( NPC, NPC->scriptName) )
		{
			G_Printf( "Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName );
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy( NPC->s.origin, NPC->SP_NPC_Position );

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );
	{														// Set up an NPC client structure...
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
		client->pers.connectTime = level.time;				// DHM - Nerve
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
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = USE_NOTHING;

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
		client->latched_wbuttons = 0;						//----(SA)	added

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

	//G_Printf("NPC (%s) spawned.\n", NPC->scriptName);
	BotDropToFloor( NPC );
}


/* */
void
NPC_CoopEliteGuard ( gentity_t *NPC )
{
	if ( g_gametype.integer > GT_COOP )
	{
		G_FreeEntity( NPC );
		return;
	}

	if ( !characters_loaded )
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;
	if ( NPC->spawnflags & TEAM_ALLIES )
	{
		NPC->s.teamNum = TEAM_ALLIES;
	}

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	// Load the NPC script for NPC->scriptName
	if ( !NPC->scriptName )
	{
		NPC->scriptName = "akimbo";
	}

	if ( !NPC_LoadData( NPC, NPC->scriptName) )
	{
		G_Printf( "Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName );
		NPC->scriptName = "default";
		if ( !NPC_LoadData( NPC, NPC->scriptName) )
		{
			G_Printf( "Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName );
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy( NPC->s.origin, NPC->SP_NPC_Position );

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );
	{														// Set up an NPC client structure...
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
		client->pers.connectTime = level.time;				// DHM - Nerve
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
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = USE_NOTHING;

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
		client->latched_wbuttons = 0;						//----(SA)	added

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

	//G_Printf("NPC (%s) spawned.\n", NPC->scriptName);
	BotDropToFloor( NPC );
}


/* */
void
NPC_CoopHelga ( gentity_t *NPC )
{
	if ( g_gametype.integer > GT_COOP )
	{
		G_FreeEntity( NPC );
		return;
	}

	if ( !characters_loaded )
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;
	if ( NPC->spawnflags & TEAM_ALLIES )
	{
		NPC->s.teamNum = TEAM_ALLIES;
	}

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	// Load the NPC script for NPC->scriptName
	if ( !NPC->scriptName )
	{
		NPC->scriptName = "akimbo";
	}

	if ( !NPC_LoadData( NPC, NPC->scriptName) )
	{
		G_Printf( "Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName );
		NPC->scriptName = "default";
		if ( !NPC_LoadData( NPC, NPC->scriptName) )
		{
			G_Printf( "Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName );
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy( NPC->s.origin, NPC->SP_NPC_Position );

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );
	{														// Set up an NPC client structure...
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
		client->pers.connectTime = level.time;				// DHM - Nerve
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
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = USE_NOTHING;

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
		client->latched_wbuttons = 0;						//----(SA)	added

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

	//G_Printf("NPC (%s) spawned.\n", NPC->scriptName);
	BotDropToFloor( NPC );
}


/* */
void
NPC_CoopProtoSoldier ( gentity_t *NPC )
{
	if ( g_gametype.integer > GT_COOP )
	{
		G_FreeEntity( NPC );
		return;
	}

	if ( !characters_loaded )
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;
	if ( NPC->spawnflags & TEAM_ALLIES )
	{
		NPC->s.teamNum = TEAM_ALLIES;
	}

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	// Load the NPC script for NPC->scriptName
	if ( !NPC->scriptName )
	{
		NPC->scriptName = "panzer";
	}

	if ( !NPC_LoadData( NPC, NPC->scriptName) )
	{
		G_Printf( "Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName );
		NPC->scriptName = "default";
		if ( !NPC_LoadData( NPC, NPC->scriptName) )
		{
			G_Printf( "Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName );
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy( NPC->s.origin, NPC->SP_NPC_Position );

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );
	{														// Set up an NPC client structure...
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
		client->pers.connectTime = level.time;				// DHM - Nerve
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
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = USE_NOTHING;

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
		client->latched_wbuttons = 0;						//----(SA)	added

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

	//G_Printf("NPC (%s) spawned.\n", NPC->scriptName);
	BotDropToFloor( NPC );
}


/* */
void
NPC_CoopSuperSoldier ( gentity_t *NPC )
{
	if ( g_gametype.integer > GT_COOP )
	{
		G_FreeEntity( NPC );
		return;
	}

	if ( !characters_loaded )
	{
		BG_ClearAnimationPool();
		BG_ClearCharacterPool();
		BG_InitWeaponStrings();
		G_RegisterPlayerClasses();
	}

	NPC->s.teamNum = TEAM_AXIS;
	if ( NPC->spawnflags & TEAM_ALLIES )
	{
		NPC->s.teamNum = TEAM_ALLIES;
	}

	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	NPC_Setup( NPC );
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	// Load the NPC script for NPC->scriptName
	if ( !NPC->scriptName )
	{
		NPC->scriptName = "panzer";
	}

	if ( !NPC_LoadData( NPC, NPC->scriptName) )
	{
		G_Printf( "Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", NPC->scriptName );
		NPC->scriptName = "default";
		if ( !NPC_LoadData( NPC, NPC->scriptName) )
		{
			G_Printf( "Failed to load NPC script file npcs/%s.npc!\n", NPC->scriptName );
			return;
		}
	}

	// NPC->SP_NPC_Position is our spawn position.. NPC should return there between enemies...
	VectorCopy( NPC->s.origin, NPC->SP_NPC_Position );

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->s.modelindex2 = CLASS_HUMANOID;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );
	{														// Set up an NPC client structure...
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
		client->pers.connectTime = level.time;				// DHM - Nerve
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
		client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam = ent->s.teamNum;
		client->ps.persistant[PERS_HWEAPON_USE] = USE_NOTHING;

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
		client->latched_wbuttons = 0;						//----(SA)	added

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

	//G_Printf("NPC (%s) spawned.\n", NPC->scriptName);
	BotDropToFloor( NPC );
}

extern vec3_t	playerMins;
extern vec3_t	playerMaxs;


//===========================================================================
// Routine      : Debug_NPC_Spawn

// Description  : Spawn an NPC.
void
Debug_NPC_Spawn ( gentity_t *CallEnt )
{
	vec3_t		fwd;
	gentity_t	*NPC = G_Spawn();
	NPC->current_node = -1;
	NPC->last_node = 0;
	NPC->next_node = 0;
	VectorCopy( CallEnt->r.currentOrigin, NPC->s.origin );
	AngleVectors( CallEnt->client->ps.viewangles, fwd, NULL, NULL );
	NPC->s.origin[0] += ( fwd[0] * 64 );
	NPC->s.origin[1] += ( fwd[1] * 64 );
	NPC->s.origin[2] += ( fwd[2] * 64 );
	NPC_Setup( NPC );
	NPC->model = CallEnt->model;
	NPC->s.modelindex = CallEnt->s.modelindex;
	NPC->NPC_Class = CLASS_HUMANOID;
	NPC->s.modelindex2 = NPC->NPC_Class;					// For cgame access...

	//NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC_SOLDIER;
	NPC->s.teamNum = PickTeam( -1 );						//CallEnt->client->sess.sessionTeam;

	// Solids...
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;	//CONTENTS_SOLID;
	NPC->r.contents = CONTENTS_SOLID | CONTENTS_PLAYERCLIP; //CONTENTS_SOLID;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	NPC->s.pos.trType = TR_GRAVITY;
	VectorCopy( playerMins, NPC->r.mins );
	VectorCopy( playerMaxs, NPC->r.maxs );

	// Health/Damage...
	NPC->health = 120;
	NPC->takedamage = qtrue;
	NPC->die = NPC_die;

	// FIXME: Need to make these guys independant of a real player... Can't just copy the player then...
	NPC->playerType = Q_TrueRand( PC_SOLDIER, NUM_PLAYER_CLASSES - 1 ); //NPC_SOLDIER;

	//NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;
	//NPC->s.modelindex2 = NPC->playerType;
	NPC->character = BG_GetCharacter( NPC->s.teamNum, NPC->playerType );
	NPC->characterIndex = G_CharacterIndex( NPC->character->characterFile );
	SetNPCWeapon( NPC );

	//if (!NPC->NPC_client)
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

		// JPW NERVE -- zero out all ammo counts
		//memset(client->ps.Ammo, 0, sizeof(client->ps.Ammo));
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
		client->ps.persistant[PERS_HWEAPON_USE] = USE_NOTHING;

		// clear entity values
		client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;

		//		client->ps.persistant[PERS_CHOOSED_WEAPON] = (client->sess.playerWeapon2 << 8) | client->sess.playerWeapon;
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

		// run the presend to set anything else
		//ClientEndFrame(ent);
		// clear entity state values
		BG_NPCStateToEntityState( &client->ps, &ent->s, qtrue );

		// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=569
		//G_ResetMarkers(ent);
		client->ps.clientNum = NPC->s.clientNum = NPC->s.number;
		NPC->s.density = NPC->playerType = NPC->NPC_client->ps.stats[STAT_PLAYER_CLASS] = NPC->playerType;

		// 12 secs of blind routing to get them away from spawnpoint...
		NPC->bot_blind_routing_time = level.time + 12000;
	}

	//G_Printf("Debug NPC Spawned...\n");
}


/* */
void
COOP_NPC_SelectType ( gentity_t *NPC )
{
	int choice = Q_TrueRand( 0, 24 );
	switch ( choice )
	{
		case 0:
		case 1:
			NPC->scriptName = "akimbo";
			break;
		case 2:
			NPC->scriptName = "fieldops";
			break;
		case 3:
		case 4:
		case 5:
			NPC->scriptName = "flamer";
			break;
		case 6:
			NPC->scriptName = "grenader";
			{		// Find high spots for these...
				int nodeNum = Q_TrueRand( 0, number_of_nodes );
				while
				(
					node_spawned_at[nodeNum] ||
					spawnpoint_close_to_bad_object( nodeNum) ||
					!above_average_node_height( nodeNum)
				)
				{
					nodeNum = Q_TrueRand( 0, number_of_nodes );
				}

				VectorCopy( nodes[nodeNum].origin, NPC->s.origin );
				node_spawned_at[nodeNum] = qtrue;
			}
			break;
		case 7:
			NPC->scriptName = "medic";
			break;
		case 8:
		case 9:
		case 10:
			NPC->scriptName = "mp42";
			break;
		case 11:
		case 12:
			NPC->scriptName = "panzer";
			if ( Q_TrueRand( 0, 10) < 5 )
			{		// Find high spots for these...
				int nodeNum = Q_TrueRand( 0, number_of_nodes );
				while
				(
					node_spawned_at[nodeNum] ||
					spawnpoint_close_to_bad_object( nodeNum) ||
					!above_average_node_height( nodeNum)
				)
				{
					nodeNum = Q_TrueRand( 0, number_of_nodes );
				}

				VectorCopy( nodes[nodeNum].origin, NPC->s.origin );
				node_spawned_at[nodeNum] = qtrue;
			}
			break;
		case 13:
		case 14:
		case 16:
			NPC->scriptName = "sniper";
			{		// Find high spots for these...
				int nodeNum = Q_TrueRand( 0, number_of_nodes );
				while
				(
					node_spawned_at[nodeNum] ||
					spawnpoint_close_to_bad_object( nodeNum) ||
					!above_average_node_height( nodeNum)
				)
				{
					nodeNum = Q_TrueRand( 0, number_of_nodes );
				}

				VectorCopy( nodes[nodeNum].origin, NPC->s.origin );
				node_spawned_at[nodeNum] = qtrue;
			}
			break;
		case 15:
			if ( mg42_count() > mounts_added )
			{
				NPC->scriptName = "mounted";
				{	// Find high spots for these...
					int nodeNum = Q_TrueRand( 0, number_of_nodes );
					while ( node_spawned_at[nodeNum] || !mg42_scan( nodeNum) )
					{
						nodeNum = Q_TrueRand( 0, number_of_nodes );
					}

					VectorCopy( nodes[nodeNum].origin, NPC->s.origin );
					node_spawned_at[nodeNum] = qtrue;
				}

				mounts_added++;
			}
			else
			{		// An extra default... No spot to put another mg42 opperator...
				NPC->scriptName = "default";
			}
			break;
		default:
			NPC->scriptName = "default";
			break;
	}

	//NPC->scriptName = "mounted";
}

qboolean	coop_npcs_checked = qfalse;
extern int	Count_NPC_Players ( void );


/* */
void
COOP_NPC_Check ( void )
{										// Does this map require us to randomly pick spots for npcs from the bot node positions??? -- (Unsupported map)
	int nodeNum = 0;
	if ( number_of_nodes <= 20 )
	{
		return;
	}

	if ( coop_npcs_checked )
	{
		return;
	}
	else
	{
		coop_npcs_checked = qtrue;
	}

	if ( Count_NPC_Players() > 0 )
	{
		return;							// Map specified some NPCs... Don't need to generate them...
	}

	G_Printf( "^1*** ^3AIMod-ET^5: No COOP \"npc\" entities specified for this map...\n" );
	G_Printf( "           ^5 : AIMod-ET will be forced to generate spawnpoints from\n" );
	G_Printf( "           ^5 : random nodes! This is much less realistic!\n" );
	while ( Count_NPC_Players() < 96 )	// 96 NPCs randomly per map???
	{
		gentity_t	*NPC = G_Spawn();
		nodeNum = Q_TrueRand( 0, number_of_nodes );
		while ( node_spawned_at[nodeNum] || spawnpoint_close_to_bad_object( nodeNum) )
		{
			nodeNum = Q_TrueRand( 0, number_of_nodes );
		}

		node_spawned_at[nodeNum] = qtrue;
		VectorCopy( nodes[nodeNum].origin, NPC->s.origin );
		VectorSet( NPC->s.angles, 0, 0, 0 );
		COOP_NPC_SelectType( NPC );
		NPC_CoopSpawn( NPC );
	}
}

extern void NPC_AI_FODDER_Humanoid_Move ( gentity_t *NPC );


/* */
void
NPC_Backup_Move ( gentity_t *NPC, usercmd_t *ucmd )
{
	if ( !NPC->oldOrigin )
	{
		VectorCopy( NPC->r.currentOrigin, NPC->oldOrigin );
	}
	{
		float	velocity = VectorDistance( NPC->r.currentOrigin, NPC->oldOrigin );
		if ( velocity < 8 )
		{	// Randomize our direction.. If too slow...
			NPC->bot_ideal_view_angles[PITCH] = 0;
			NPC->bot_ideal_view_angles[YAW] = Q_TrueRand( -360, 360 );
			NPC->bot_ideal_view_angles[ROLL] = 0;
		}

		NPCSetViewAngles( NPC, 100 );
		VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
		ucmd->angles[PITCH] = ANGLE2SHORT( NPC->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( NPC->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( NPC->s.angles[ROLL] );
		ucmd->forwardmove = 127;
		ucmd->upmove = 0;
		ucmd->rightmove = 0;

		// Set origin ready for getting velocity next frame...
		VectorCopy( NPC->r.currentOrigin, NPC->oldOrigin );
		return;
	}
}


/* */
void
NPC_Blind_Move ( gentity_t *NPC, usercmd_t *ucmd )
{
	if ( NPC->current_node <= 0 || NPC->current_node >= number_of_nodes || NPC->node_timeout <= level.time )
	{			// Find a new node...
		if ( NPC->current_node <= 0 )
		{		// Default to forwards...
			NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
			NPC->last_node = NPC->current_node - 1;
			NPC->next_node = NPC->current_node + 1;
			NPC->node_timeout = level.time + 5000;
			NPC->reversed_route = qfalse;
		}
		else if ( NPC->current_node >= number_of_nodes )
		{		// Switch to reversed route...
			NPC->current_node = NPC->current_node - 1;
			NPC->last_node = NPC->current_node + 1;
			NPC->next_node = NPC->current_node - 1;
			NPC->node_timeout = level.time + 5000;
			NPC->reversed_route = qtrue;
		}
		else
		{		// Default to forwards...
			NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
			NPC->last_node = NPC->current_node - 1;
			NPC->next_node = NPC->current_node + 1;
			NPC->node_timeout = level.time + 5000;
			NPC->reversed_route = qfalse;
		}
	}

	if ( NPC->current_node <= 0 || NPC->current_node >= number_of_nodes )
	{
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return; // Next think...
	}

	if ( VectorDistance( nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 48 )
	{			// At current node.. Pick next in the list...
		if ( NPC->reversed_route )
		{
			NPC->last_node = NPC->current_node;
			NPC->current_node = NPC->next_node;
			NPC->next_node = NPC->current_node - 1;
			NPC->node_timeout = level.time + 5000;
			NPC->reversed_route = qtrue;
		}
		else
		{
			NPC->last_node = NPC->current_node;
			NPC->current_node = NPC->next_node;
			NPC->next_node = NPC->current_node + 1;
			NPC->node_timeout = level.time + 5000;
			NPC->reversed_route = qfalse;
		}
	}
}

/* */
void
NPC_Waypoint_SinglePlayer_Waypointing ( gentity_t *NPC, usercmd_t *ucmd )
{
	qboolean	onLadder = qfalse;
	qboolean	ladderMove = qfalse;
	if ( NPC->bot_stand_guard )
	{		// A bot that is standing guard...
		if
		(
			Have_Close_Enemy( NPC) &&
			NPC->beStillEng < level.time &&
			NPC->beStill < level.time &&
			NPC->bot_enemy_visible_time > level.time &&
			NPC->enemy == NPC->bot_last_visible_enemy &&
			NPC->bot_runaway_time < level.time
		)
		{	// Do attacking first...
			//AIMOD_MOVEMENT_Attack(NPC, ucmd);
			return;
		}
		else
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->node_timeout = level.time + ( TravelTime( NPC) * 2 );	// Duck moves take longer, so...

			// Set our last good move time to now...
			NPC->bot_last_good_move_time = level.time;
		}

		//G_Printf("NPC (%i) stand guard.\n", NPC->s.number);
		return;
	}

	if ( NPC->bot_last_position_check < level.time - 4000 )
	{														// Check if we have moved in the last 10 seconds... (Are we stuck somewhere?)
		if
		(
			NPC->goalentity &&
			!Q_stricmp( NPC->goalentity->classname, va( "bot_sniper_spot")) &&
			VectorDistance( NPC->goalentity->r.currentOrigin, NPC->r.currentOrigin) < 256
		)
		{

		}
		else if ( VectorDistance( NPC->bot_last_position, NPC->r.currentOrigin) <= 64 )
		{													// We are stuck... Find a new route!
			NPC->current_node = -1;
			NPC->longTermGoal = -1;
			//Bot_Set_New_Path( NPC, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->bot_last_position_check = level.time;
			VectorCopy( NPC->r.currentOrigin, NPC->bot_last_position );

			//G_Printf("NPC (%i) position check.\n", NPC->s.number);
			return;
		}

		NPC->bot_last_position_check = level.time;
		VectorCopy( NPC->r.currentOrigin, NPC->bot_last_position );
	}

	/*	if (nodes_loaded 
		&& NPC->goalentity
		&& !Q_stricmp( NPC->goalentity->classname, va("bot_sniper_spot") )
		&& VectorDistance(nodes[NPC->longTermGoal].origin, NPC->r.currentOrigin) < 24 )
	{// A sniper... Make him look around in snipe mode for enemies...
		AIMOD_Movement_SniperMove(NPC, ucmd);
		
		if (test_last_message[NPC->s.number] < level.time - 3000)
		{
			G_Printf("^2*** ^3BOT DEBUG^5: ^3%s^5's has found his sniper point.\n", NPC->NPC_client->pers.netname);
			test_last_message[NPC->s.number] = level.time;
		}

		return;
	}
	else if (nodes_loaded 
		&& NPC->goalentity
		&& !Q_stricmp(NPC->goalentity->classname, va("bot_sniper_spot")) )
	{
		if (test_last_message[NPC->s.number] < level.time - 3000)
		{
			G_Printf("^2*** ^3BOT DEBUG^5: ^3%s^5's distance from his sniper point is ^7%f^5.\n", NPC->NPC_client->pers.netname, VectorDistance(NPC->goalentity->r.currentOrigin, NPC->r.currentOrigin));
			test_last_message[NPC->s.number] = level.time;
		}
	}*/
	if ( nodes_loaded && NPC->current_node > 0 )
	{
		if ( !AIMOD_MOVEMENT_CurrentNodeReachable( NPC) )
		{													// If our current node is not visible, then we need to try something else...
			NPC->current_node = -1;
			//Bot_Set_New_Path( NPC, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;

			//G_Printf("NPC (%i) current node unreachable.\n", NPC->s.number);
			return;
		}
		else
		{

			//if (HaveSequentialNodes())
			if ( NPC->pathsize > 10 )
			{
				AIMOD_MOVEMENT_Shorten_Path( NPC );
			}
		}
	}

	//	if (NPC->NPC_client->ps.weapon == WP_MOBILE_MG42_SET)
	//		return;
	if ( NPC->health <= 0 )
	{
		return;
	}

	//	if (NPC->NPC_client->ps.pm_flags & PMF_LIMBO)
	//		return;
	if ( !NPC->r.currentOrigin )
	{
		return;
	}

	//	if (NPC->NPC_client->sess.playerType != PC_SOLDIER) // Soldiers shouldn't need special moves...
	//		if (AIMOD_MOVEMENT_SpecialMoves(NPC, ucmd))
	//			return;
#ifdef __WEAPON_AIM__
	if ( NPC->NPC_client->ps.eFlags & EF_AIMING )
	{
		ucmd->buttons |= BUTTON_WALKING;
	}
	else
	{
		ucmd->buttons &= ~BUTTON_WALKING;
	}
#endif //__WEAPON_AIM__
	if
	(
		NPC->enemy &&
		NPC->beStillEng < level.time &&
		NPC->beStill < level.time &&
		NPC->bot_enemy_visible_time > level.time &&
		NPC->enemy == NPC->bot_last_visible_enemy &&
		NPC->bot_runaway_time < level.time
	)
	{														// Do attacking first...
		//AIMOD_MOVEMENT_AttackMove(NPC, ucmd);
		//G_Printf("NPC (%i) attack.\n", NPC->s.number);
		return;
	}

	// ---------------------------------------------
	// OK... First the movement for non-routed maps.
	// ---------------------------------------------
	//	if ( !Bot_Ignore_Use_Time[NPC->s.clientNum] )
	//		Bot_Ignore_Use_Time[NPC->s.clientNum] = 0;
#ifdef __SUPREMACY__
	if ( nodes_loaded && NPC->current_node )
	{														// We have a routed map.. Do some flag capture checks...
		if
		(
			NPC->NPC_client->ps.stats[STAT_FLAG_CAPTURE] > 0 &&
			g_entities[NPC->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != NPC->NPC_client->sess.sessionTeam &&
			NPC->current_node > 0
		)
		{
			if
			(
				!BotEntityCloserOnFuturePath( NPC, &g_entities[NPC->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]]) &&
				!Have_Close_Enemy( NPC)
			)
			{												// If we're capturing a flag in POP gametype don't move, just defend current pos...
				NPC->bot_last_good_move_time = level.time;
				NPC->node_timeout = level.time + TravelTime( NPC );

				//G_Printf("NPC (%i) flag capture.\n", NPC->s.number);
				return;										// Don't move while capturing a flag...
			}
		}

		if
		(
			NPC->enemy &&
			NPC->beStillEng < level.time &&
			NPC->beStill < level.time &&
			NPC->bot_enemy_visible_time > level.time &&
			NPC->enemy == NPC->bot_last_visible_enemy &&
			NPC->bot_runaway_time < level.time
		)
		{													// Do attacking first...
			//AIMOD_MOVEMENT_AttackMove(NPC, ucmd);
			//G_Printf("NPC (%i) attack 2.\n", NPC->s.number);
			return;
		}
	}
	else
	{
		if
		(
			NPC->NPC_client->ps.stats[STAT_FLAG_CAPTURE] > 0 &&
			g_entities[NPC->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != NPC->NPC_client->sess.
				sessionTeam
		)
		{
			if ( !Have_Close_Enemy( NPC) )
			{												// If we're capturing a flag in POP gametype don't move, just defend current pos...
				NPC->bot_last_good_move_time = level.time;
				NPC->node_timeout = level.time + TravelTime( NPC );

				//G_Printf("NPC (%i) capture flag 2.\n", NPC->s.number);
				return;										// Don't move while capturing a flag...
			}
		}

		if
		(
			NPC->enemy &&
			NPC->beStillEng < level.time &&
			NPC->beStill < level.time &&
			NPC->bot_enemy_visible_time > level.time &&
			NPC->enemy == NPC->bot_last_visible_enemy &&
			NPC->bot_runaway_time < level.time
		)
		{													// Do attacking first...
			//AIMOD_MOVEMENT_AttackMove(NPC, ucmd);
			//G_Printf("NPC (%i) attack 3.\n", NPC->s.number);
			return;
		}
	}
#endif //__SUPREMACY__
	if
	(
		NPC->NPC_client->ps.serverCursorHint == HINT_LADDER ||
		NPC->NPC_client->ps.torsoAnim == BOTH_CLIMB ||
		NPC->NPC_client->ps.legsAnim == BOTH_CLIMB
	)
	{
		onLadder = qtrue;
	}

	if ( onLadder )
	{														// Always go up/down!
		if ( VectorDistance( nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 16 )
		{
			NPC->last_node = NPC->current_node;
			NPC->current_node = NPC->next_node;
			NPC->next_node = BotGetNextNode( NPC );
		}

		ucmd->forwardmove = 127;

		// Set up ideal view angles for this enemy...
		VectorSubtract( nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
		vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );

		//BotSetViewAngles(NPC, 100);
		NPCSetViewAngles( NPC, 100 );
		return;
	}

	// ----------------------------------------
	// OK... Now the movement for routed maps.
	// ----------------------------------------
	if ( NPC->current_node < 0 )
	{
		NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
		if ( NPC->current_node < 0 )
		{
			NPC->bot_random_move_time = level.time + 500;
		}
	}

	if ( NPC->bot_random_move_time > level.time )
	{														// Get out of trouble with non-existant waypoint!
		AIMOD_MOVEMENT_Backup( NPC, ucmd );
		return;
	}

	if ( !onLadder && BAD_WP_Height( nodes[NPC->current_node].origin, NPC->r.currentOrigin) && !onLadder )
	{														// Height diff is greater then plane distance... Block it and skip it..
		int loop = 0;
		int link = -1;
		for ( loop = 0; loop < nodes[NPC->last_node].enodenum; loop++ )
		{													// Find the link we last used and mark it...
			if ( nodes[NPC->last_node].links[loop].targetNode == NPC->current_node )
			{												// Mark it as blocked...
				BOT_MakeLinkUnreachable( NPC, NPC->last_node, loop );
				break;
			}
		}

		Bot_Set_New_Path( NPC, ucmd );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		//G_Printf("NPC (%i) BAD_WP_Height.\n", NPC->s.number);
		return;
	}

	//#ifdef __BOT_STRAFE__
	if ( NPC->bot_strafe_right_timer >= level.time )
	{
		if ( VectorDistance( NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8 )
		{
			ucmd->forwardmove = 0;							//64;
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

	if ( NPC->bot_strafe_left_timer >= level.time )
	{
		if ( VectorDistance( NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8 )
		{
			ucmd->forwardmove = 0;							//64;
			ucmd->upmove = 0;
			ucmd->rightmove = -64;
			return;
		}
		else
		{
			NPC->bot_strafe_left_timer = 0;
			ucmd->upmove = 127;
		}
	}

	if ( NPC->bot_duck_time > level.time )
	{
		if ( !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
		{													// For special duck nodes, make the bot duck down...
			//ucmd->upmove = -48;
			NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
		}

		ucmd->forwardmove = 127;
		return;
	}
	else if ( NPC->bot_strafe_left_time > level.time )
	{
		ucmd->rightmove = -48;
		return;
	}
	else if ( NPC->bot_strafe_right_time > level.time )
	{
		ucmd->rightmove = 48;
		return;
	}
	else if ( NPC->bot_jump_time > level.time )
	{
		ucmd->upmove = 127;

		//ucmd->forwardmove = 127;
		//return;
	}

	/*	else if (NPC->bot_last_good_move_time < level.time - 1000
		&& NPC->bot_last_move_fix_time < level.time - 1000 
		&& !onLadder )
	{// The first type of route correction...
		int result = -1;

		// So we don't vis check too often...
		NPC->bot_last_move_fix_time = level.time;

		result = AIMOD_MOVEMENT_ReachableBy(NPC, nodes[NPC->current_node].origin);

		if (result == -1)
		{// -1 is Not reachable at all... Let's use some blind routing for a short time!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->longTermGoal = -1;
			NPC->shortTermGoal = -1;
			//G_Printf("NPC (%i) strafe unreach.\n", NPC->s.number);
			return;
		}

		if (result == 1)
		{// 1 is Jump...
			ucmd->upmove = 127;
			NPC->bot_jump_time = level.time + 500;
		}

		if (result == 2)
		{// 2 is Duck...
			if (!(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
			{// For special duck nodes, make the bot duck down...
				//ucmd->upmove = -48;
				NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				NPC->bot_duck_time = level.time + 2000;
			}
		}

		if (result == 3)
		{// 3 is Strafe Left...
			ucmd->rightmove = -48;
			NPC->bot_strafe_left_time = level.time + 1000;
			return;
		}

		if (result == 4)
		{// 4 is Strafe Right...
			ucmd->rightmove = 48;
			NPC->bot_strafe_right_time = level.time + 1000;
			return;
		}
	}
#endif //__BOT_STRAFE__
*/
	if
	(
		NPC->longTermGoal < 0 ||
		NPC->longTermGoal > number_of_nodes ||
		NPC->current_node < 0 ||
		NPC->node_timeout <= level.time ||
		NPC->bot_last_good_move_time < level.time - 2000
	)
	{														// We hit a problem in route, or don't have one yet.. Find a new goal...
		if
		(
			!onLadder &&
			!(
				NPC->longTermGoal < 0 ||
				NPC->longTermGoal > number_of_nodes ||
				NPC->current_node < 0 ||
				NPC->next_node < 0
			) &&
			(NPC->node_timeout <= level.time || NPC->bot_last_good_move_time < level.time - 2000)
		)
		{													// Bad current path link.. Mark it as blocked...
			int loop = 0;
			int link = -1;
			for ( loop = 0; loop < nodes[NPC->last_node].enodenum; loop++ )
			{												// Find the link we last used and mark it...
				if ( nodes[NPC->last_node].links[loop].targetNode == NPC->current_node )
				{											// Mark it as blocked...
					BOT_MakeLinkUnreachable( NPC, NPC->last_node, loop );
					break;
				}
			}

			/*			if ( (NPC->NPC_client->sess.sessionTeam == TEAM_AXIS && numAxisOnlyNodes > 0)
				|| (NPC->NPC_client->sess.sessionTeam == TEAM_ALLIES && numAlliedOnlyNodes > 0) )
			{
				NPC->pathsize = CreateTeamRoute(NPC, NPC->pathlist);
			
				if (NPC->pathsize > 0)
				{
					NPC->next_node = BotGetNextNode(NPC);		//move to this node first, since it's where our path starts from
					NPC->node_timeout = level.time + TravelTime(NPC);//6000;//4000;
					NPC->bot_last_good_move_time = level.time;
					return;
				}
				else
				{
					NPC->longTermGoal = -1;
					NPC->shortTermGoal = -1;
					return;
				}
			}*/
		}

		//Bot_Set_New_Path( NPC, ucmd );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		NPC->node_timeout = level.time + TravelTime( NPC ); //6000;//4000;
		NPC->bot_last_good_move_time = level.time;

		//G_Printf("NPC (%i) node timeout.\n", NPC->s.number);
		return;
	}

	if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
	{														// Try to roam out of problems...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		NPC->longTermGoal = -1;
		NPC->shortTermGoal = -1;
		NPC->bot_random_move_time = level.time + 500;

		//G_Printf("NPC (%i) bad longterm goal.\n", NPC->s.number);
		return;												// next think...
	}

	if ( nodes[NPC->current_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
	{														// For special duck nodes, make the bot duck down...
		//ucmd->upmove = -48;
		NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
	}
	else if ( nodes[NPC->last_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
	{														// For special duck nodes, make the bot duck down...
		//ucmd->upmove = -48;
		NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
	}
	else if
		(
			!(nodes[NPC->current_node].type & NODE_DUCK) &&
			!(nodes[NPC->last_node].type & NODE_DUCK) &&
			!NPC->enemy &&
			(NPC->NPC_client->ps.eFlags & EF_CROUCHING)
		)
	{														// Get up?
		ucmd->upmove = 127;
		NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
	}
	else if ( !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_PRONE) )
	{														// Get up?
		ucmd->upmove = 127;
		NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
	}

	// Now we need to check if we have hit our long term node...
	if ( NPC->current_node == NPC->longTermGoal && AIMOD_MOVEMENT_WaypointTouched( NPC, NPC->current_node, onLadder) )
	{														// We're there! Find a new goal... FIXME: We should actually do something at most goals..
		if ( NPC->followtarget && VectorDistance( NPC->followtarget->r.currentOrigin, NPC->r.currentOrigin) < 256 )
		{													// A following bot, at it's owner... Just keep standing with him for now...
			//FIXME: Make them move around just a little near their owner... Maybe some anims/sounds??? - Unique1
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->node_timeout = level.time + ( TravelTime( NPC) * 2 );		// Duck moves take longer, so...

			// Set our last good move time to now...
			NPC->bot_last_good_move_time = level.time;

			//G_Printf("NPC (%i) LTG waypoint touched.\n", NPC->s.number);
			return;															// Think again soon!
		}

		if
		(
			NPC->bot_defender &&
			!(NPC->bot_goal_constructible && NPC->NPC_client->sess.sessionTeam == level.attackingTeam)
		)
		{

			//			if (!(BotMP_Num_Defenders_Near ( NPC->bot_goal_position, NPC ) > 2) )
			{
				NPC->bot_stand_guard = qtrue;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				NPC->node_timeout = level.time + ( TravelTime( NPC) * 2 );	// Duck moves take longer, so...

				// Set our last good move time to now...
				NPC->bot_last_good_move_time = level.time;

				//G_Printf("NPC (%i) defender.\n", NPC->s.number);
				return;
			}
		}

		if ( NPC->goalentity && NPC->goalentity->s.eType == ET_DEFENCEPOINT_HINT )
		{

			//			if (!(BotMP_Num_Defenders_Near ( NPC->goalentity->s.origin, NPC ) > (bot_minplayers.integer*0.35)) )
			{
				NPC->bot_stand_guard = qtrue;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				NPC->node_timeout = level.time + ( TravelTime( NPC) * 2 );	// Duck moves take longer, so...

				// Set our last good move time to now...
				NPC->bot_last_good_move_time = level.time;

				//G_Printf("NPC (%i) defence point.\n", NPC->s.number);
				return;
			}
		}

		if ( NPC->longterm_pathsize > 0 )
		{
			int i;
			NPC->pathsize = NPC->longterm_pathsize;
			for ( i = 0; i < NPC->pathsize; i++ )
			{
				NPC->pathlist[i] = NPC->longterm_pathlist[i];
			}

			NPC->current_node = NPC->shortTermGoal;
			NPC->next_node = BotGetNextNode( NPC );
			NPC->longterm_pathsize = -1;
			NPC->shortTermGoal = -1;
		}
		else
		{
			Bot_Set_New_Path( NPC, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->node_timeout = level.time + TravelTime( NPC );				//6000;//4000;
			NPC->bot_last_good_move_time = level.time;
			return;
		}
	}

	// Now we need to check if we have hit our current node...
	if ( AIMOD_MOVEMENT_WaypointTouched( NPC, NPC->current_node, onLadder) )
	{	// At current node.. Pick next in the list...
		NPC->last_node = NPC->current_node;
		NPC->current_node = NPC->next_node;
		NPC->next_node = BotGetNextNode( NPC );
		NPC->node_timeout = level.time + TravelTime( NPC );
		if ( NPC->current_node > number_of_nodes )
		{
			NPC->current_node = -1;
			NPC->next_node = -1;

			//G_Printf("NPC (%i) current wp touched.\n", NPC->s.number);
			return;
		}

		if ( nodes[NPC->current_node].type & NODE_DUCK )
		{
			NPC->node_timeout = level.time + ( TravelTime( NPC) * 2 );	// Duck moves take longer, so...
		}

		// Set our last good move time to now...
		NPC->bot_last_good_move_time = level.time;
		NPC->node_timeout = level.time + TravelTime( NPC );
	}

	if ( nodes[NPC->current_node].origin[2] > NPC->r.currentOrigin[2] && !onLadder )
	{	// Do we need to jump to our next node?
		vec3_t	temp, temp2;
		VectorCopy( nodes[NPC->current_node].origin, temp );
		temp[2] = NPC->r.currentOrigin[2];
		VectorCopy( nodes[NPC->current_node].origin, temp2 );
		temp2[2] = NPC->r.currentOrigin[2] + 16;
		if ( nodes[NPC->current_node].origin[2] - NPC->r.currentOrigin[2] > 16	 /*&& VectorLength(NPC->NPC_client->ps.velocity) < 24*/ )
		{
			ucmd->upmove = 127;
		}
	}

#ifdef __BOT_STRAFE__
	if
	(
		NPC->bot_strafe_right_timer < level.time &&
		NPC->bot_strafe_left_timer < level.time &&
		VectorLength( NPC->NPC_client->ps.velocity) < 24 &&
		!(nodes[NPC->current_node].type & NODE_DUCK) &&
		!(nodes[NPC->last_node].type & NODE_DUCK) &&
		!onLadder
	)
	{	// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
		AIMOD_MOVEMENT_Setup_Strafe( NPC, ucmd );
	}

	if ( NPC->bot_strafe_right_timer >= level.time || NPC->bot_strafe_left_timer >= level.time )
	{
		return;
	}
#endif //__BOT_STRAFE__

	// Set up ideal view angles for this enemy...
	VectorSubtract( nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
	NPC->move_vector[ROLL] = 0;
	vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );

	//BotSetViewAngles(NPC, 100);
	NPCSetViewAngles( NPC, 100 );
	if ( NPC->bot_turn_wait_time <= level.time )
	{
		G_UcmdMoveForDir( NPC, ucmd, NPC->move_vector );
	}

	// Set our last good move time to now...
	NPC->bot_last_good_move_time = level.time;

	//G_Printf("NPC %i - Forward: %f. Right %f.\n", NPC->s.number, ucmd->forwardmove, ucmd->rightmove);
}

/* */
void
NPC_SinglePlayer_Move ( gentity_t *NPC, usercmd_t *ucmd )
{
	if ( NPC->npc_mount )
	{
		if ( !(NPC->NPC_client->ps.eFlags & EF_MG42_ACTIVE) )
		{
			NPC->npc_mg42 = NPC_FindMG42( NPC );
			if ( NPC->npc_mg42 )
			{
				NPC->npc_mount = qfalse;
			}
		}
	}

	if ( NPC->npc_mg42 )
	{												// We have an MG42 to mount ourselves on.. Do it!
		if ( NPC->NPC_client->ps.eFlags & EF_MG42_ACTIVE )
		{											// We're mounted already...
		}
		else if ( !(NPC->NPC_client->ps.eFlags & EF_MG42_ACTIVE) )
		{											// Use our mounted gun!
			gentity_t	*gun = NPC->npc_mg42;
			gclient_t	*cl = NPC->NPC_client;

			VectorSubtract( gun->r.currentOrigin, NPC->r.currentOrigin, NPC->move_vector );
			vectoangles( NPC->move_vector, NPC->s.angles );

			// Zero out velocity
			VectorCopy( vec3_origin, NPC->NPC_client->ps.velocity );
			VectorCopy( vec3_origin, NPC->s.pos.trDelta );
			gun->active = qtrue;
			NPC->active = qtrue;
			gun->r.ownerNum = NPC->s.number;
			VectorCopy( gun->s.angles, gun->TargetAngles );
			gun->s.otherEntityNum = NPC->s.number;
			cl->pmext.harc = gun->harc;
			cl->pmext.varc = gun->varc;
			VectorCopy( gun->s.angles, cl->pmext.centerangles );
			cl->pmext.centerangles[PITCH] = AngleNormalize180( cl->pmext.centerangles[PITCH] );
			cl->pmext.centerangles[YAW] = AngleNormalize180( cl->pmext.centerangles[YAW] );
			cl->pmext.centerangles[ROLL] = AngleNormalize180( cl->pmext.centerangles[ROLL] );
			NPC->backupWeaponTime = NPC->NPC_client->ps.weaponTime;
			NPC->NPC_client->ps.weaponTime = gun->backupWeaponTime;

			// Woop we got through, let's use the entity
			NPC->npc_mg42->use( NPC->npc_mg42, NPC, 0 );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->NPC_client->ps.eFlags |= EF_MG42_ACTIVE;
		}

		return;
	}

	if ( NPC->npc_hunter && NPC->enemy )
	{												// Find and kill them!
		if
		(
			NPC->longTermGoal < 0 ||
			NPC->longTermGoal > number_of_nodes ||
			NPC->current_node < 0 ||
			NPC->next_node < 0 ||
			NPC->node_timeout <= level.time ||
			VectorDistance(NPC->enemy->r.currentOrigin, nodes[NPC->longTermGoal].origin) > 256
		)
		{											// We hit a problem in route, or don't have one yet.. Find a new goal...
			memset( NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
			NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
			NPC->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( NPC->enemy->r.currentOrigin, NODE_DENSITY,
																			 NODEFIND_ALL );
			if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
			{										// Try to roam out of problems...
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;								// next think...
			}

			NPC->pathsize = NPC_CreatePathAStar( NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist );
			NPC->next_node = BotGetNextNode( NPC ); //move to this node first, since it's where our path starts from
			NPC->node_timeout = level.time + 5000;
		}

		NPC_Waypoint_SinglePlayer_Waypointing( NPC, ucmd );

		return;
	}

	if ( NPC->npc_hunter && !NPC->enemy && VectorDistance( NPC->SP_NPC_Position, NPC->r.currentOrigin) > 256 )
	{												// Return to our marker!
		if
		(
			NPC->longTermGoal < 0 ||
			NPC->longTermGoal > number_of_nodes ||
			NPC->current_node < 0 ||
			NPC->next_node < 0 ||
			NPC->node_timeout <= level.time
		)
		{											// We hit a problem in route, or don't have one yet.. Find a new goal...
			memset( NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
			NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
			NPC->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( NPC->SP_NPC_Position, NODE_DENSITY,
																			 NODEFIND_ALL );
			if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
			{										// Try to roam out of problems...
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;								// next think...
			}

			NPC->pathsize = NPC_CreatePathAStar( NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist );
			NPC->next_node = BotGetNextNode( NPC ); //move to this node first, since it's where our path starts from
			NPC->node_timeout = level.time + 5000;
		}

		NPC_Waypoint_SinglePlayer_Waypointing( NPC, ucmd );

		return;
	}

	if ( NPC->npc_patrol )
	{												// We're a patroling NPC!
		if
		(
			NPC->longTermGoal < 0 ||
			NPC->longTermGoal > number_of_nodes ||
			NPC->current_node < 0 ||
			NPC->next_node < 0 ||
			NPC->node_timeout <= level.time
		)
		{											// We hit a problem in route, or don't have one yet.. Find a new goal...
			int		best_node = -1;
			float	best_dist = 1024.0f;
			int		node = 0;
			memset( NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
			NPC->current_node = NPC->npc_patrol_start_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC,
																										NODE_DENSITY, NODEFIND_ALL,
																										NPC );
			for ( node = 0; node < number_of_nodes; node+=Q_TrueRand(1,4) )
			{										// Find a good node to patrol to/from...
				float	node_dist = VectorDistance( NPC->r.currentOrigin, nodes[node].origin );
				if ( node_dist < best_dist && node_dist > 512 )
				{
					best_dist = node_dist;
					best_node = node;
				}
			}

			if ( best_node != -1 )
			{
				NPC->longTermGoal = NPC->npc_patrol_end_node = best_node;
			}
			else
			{										// We didnt find a good node to patrol to/from... Just stand there...
				NPC->npc_patrol = qfalse;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;
			}

			if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
			{										// Try to roam out of problems...
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;								// next think...
			}

			NPC->pathsize = NPC_CreatePathAStar( NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist );
			NPC->next_node = BotGetNextNode( NPC ); //move to this node first, since it's where our path starts from
			NPC->node_timeout = level.time + 5000;
		}

		NPC_Waypoint_SinglePlayer_Waypointing( NPC, ucmd );

		return;
	}
}

/* */
void
NPC_SinglePlayer_Move_OLD ( gentity_t *NPC, usercmd_t *ucmd )
{
	vec3_t	testpos;
	float	diff;
	VectorCopy( NPC->r.currentOrigin, testpos );
	testpos[2] = NPC->SP_NPC_Position[2];
	if ( NPC->npc_mount )
	{
		if ( !(NPC->NPC_client->ps.eFlags & EF_MG42_ACTIVE) )
		{
			NPC->npc_mg42 = NPC_FindMG42( NPC );
			if ( NPC->npc_mg42 )
			{
				NPC->npc_mount = qfalse;
			}
		}
	}

	if ( NPC->npc_mg42 )
	{												// We have an MG42 to mount ourselves on.. Do it!
		if ( NPC->NPC_client->ps.eFlags & EF_MG42_ACTIVE )
		{											// We're mounted already...
		}
		else if ( !(NPC->NPC_client->ps.eFlags & EF_MG42_ACTIVE) )
		{											// Use our mounted gun!
			//					gentity_t *base = &g_entities[NPC->npc_mg42->mg42BaseEnt];
			gentity_t	*gun = NPC->npc_mg42;
			gclient_t	*cl = NPC->NPC_client;

			//					vec3_t	point, forward;

			/*AngleVectors (gun->s.apos.trBase, forward, NULL, NULL);
					VectorMA (gun->r.currentOrigin, -36, forward, point);
					point[2] = NPC->r.currentOrigin[2];

					// Save initial position
					VectorCopy(point, NPC->TargetAngles);*/
			VectorSubtract( gun->r.currentOrigin, NPC->r.currentOrigin, NPC->move_vector );
			vectoangles( NPC->move_vector, NPC->s.angles );

			//G_SetOrigin(NPC, point);
			// Zero out velocity
			VectorCopy( vec3_origin, NPC->NPC_client->ps.velocity );
			VectorCopy( vec3_origin, NPC->s.pos.trDelta );
			gun->active = qtrue;
			NPC->active = qtrue;
			gun->r.ownerNum = NPC->s.number;
			VectorCopy( gun->s.angles, gun->TargetAngles );
			gun->s.otherEntityNum = NPC->s.number;
			cl->pmext.harc = gun->harc;
			cl->pmext.varc = gun->varc;
			VectorCopy( gun->s.angles, cl->pmext.centerangles );
			cl->pmext.centerangles[PITCH] = AngleNormalize180( cl->pmext.centerangles[PITCH] );
			cl->pmext.centerangles[YAW] = AngleNormalize180( cl->pmext.centerangles[YAW] );
			cl->pmext.centerangles[ROLL] = AngleNormalize180( cl->pmext.centerangles[ROLL] );
			NPC->backupWeaponTime = NPC->NPC_client->ps.weaponTime;
			NPC->NPC_client->ps.weaponTime = gun->backupWeaponTime;

			//					NPC->NPC_client->ps.weaponsHeat[WEAPON_OVERHEAT_SLOT(WP_DUMMY_MG42)] = gun->mg42weapHeat;
			// Woop we got through, let's use the entity
			NPC->npc_mg42->use( NPC->npc_mg42, NPC, 0 );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->NPC_client->ps.eFlags |= EF_MG42_ACTIVE;
		}

		return;
	}

	if ( NPC->npc_hunter && NPC->enemy )
	{												// Find and kill them!
		if
		(
			NPC->enemy->health <= 0 ||
			(NPC->enemy->client->ps.pm_flags & PMF_LIMBO) ||
			OnSameTeam( NPC, NPC->enemy) ||
			NPC->s.teamNum == TEAM_SPECTATOR
		)
		{											// They are dead... Don't move... Initialize goal so they head back to their spawn position...
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->enemy = NULL;
			NPC->longTermGoal = -1;
			return;									// next think...
		}

		if ( NPC->bot_strafe_right_timer >= level.time )
		{
			if ( VectorDistance( NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8 )
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

		if ( NPC->bot_strafe_left_timer >= level.time )
		{
			if ( VectorDistance( NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8 )
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
		}

		if
		(
			NPC->longTermGoal < 0 ||
			NPC->longTermGoal > number_of_nodes ||
			NPC->current_node < 0 ||
			NPC->next_node < 0 ||
			NPC->node_timeout <= level.time
		)
		{											// We hit a problem in route, or don't have one yet.. Find a new goal...
			memset( NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
			NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
			NPC->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( NPC->enemy->r.currentOrigin, NODE_DENSITY,
																			 NODEFIND_ALL );
			if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
			{										// Try to roam out of problems...
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;								// next think...
			}

			NPC->pathsize = NPC_CreatePathAStar( NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist );
			NPC->next_node = BotGetNextNode( NPC ); //move to this node first, since it's where our path starts from
			NPC->node_timeout = level.time + 5000;
		}

		if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
		{											// Try to roam out of problems...
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;									// next think...
		}

		if ( VectorDistance( nodes[NPC->longTermGoal].origin, NPC->r.currentOrigin) < 48 )
		{											// We're there! Find a new goal...
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;									// Next think...
		}

		if ( NPC->current_node <= 0 || NPC->current_node >= number_of_nodes )
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;									// Next think...
		}

		if ( VectorDistance( nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 48 )
		{											// At current node.. Pick next in the list...
			NPC->last_node = NPC->current_node;
			NPC->current_node = NPC->next_node;
			NPC->next_node = BotGetNextNode( NPC );
			NPC->node_timeout = level.time + 5000;
		}

		if
		(
			NPC->enemy &&
			NPC->action > level.time - 100 &&
			VectorDistance( NPC->r.currentOrigin, NPC->enemy->r.currentOrigin) < 256
		)
		{											// We're close enough to our enemy now... Don't move any more...
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;									// Next think...
		}

		if ( NPC->bot_last_good_move_time < level.time - 1000 && NPC->bot_last_move_fix_time < level.time - 2000 )
		{
			int result = -1;

			// So we don't vis check too often...
			NPC->bot_last_move_fix_time = level.time;
			result = NPC_MOVEMENT_ReachableBy( NPC, nodes[NPC->current_node].origin );
			if ( result == NOT_REACHABLE )
			{										// -1 is Not reachable at all... Reset our long term goal so it finds a new path next think!
				NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
				NPC->longTermGoal = 0;
				ucmd->forwardmove = 0;
				ucmd->upmove = 0;
				ucmd->rightmove = 0;
				return;
			}

			if ( result == REACHABLE_JUMP )
			{										// 1 is Jump...
				ucmd->upmove = 127;
				ucmd->forwardmove = 127;
				NPC->NPC_client->ps.velocity[2] = 256;
				return;
			}

			if ( result == REACHABLE_DUCK )
			{										// 2 is Duck...
				if ( !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
				{									// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
			}

			if ( result == REACHABLE_STRAFE_LEFT )
			{										// 3 is Strafe Left...
				ucmd->rightmove = -48;
				return;
			}

			if ( result == REACHABLE_STRAFE_RIGHT )
			{										// 4 is Strafe Right...
				ucmd->rightmove = 48;
				return;
			}
		}

		if ( NPC->last_node < 0 || NPC->last_node >= number_of_nodes )
		{
			if ( NPC->current_node >= 0 && NPC->current_node <= number_of_nodes )
			{
				NPC->last_node = NPC->current_node - 1;
			}
			else
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;								// Next think...
			}
		}

		if
		(
			nodes[NPC->current_node].type & NODE_JUMP ||
			nodes[NPC->current_node].origin[2] - NPC->r.currentOrigin[2] > 8
		)
		{											// For special jump nodes, make the bot jump...
			ucmd->upmove = 127;
		}
		else if ( nodes[NPC->current_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
		{											// For special duck nodes, make the bot duck down...
			//ucmd->upmove = -48;
			NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
		}
		else if ( nodes[NPC->last_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
		{											// For special duck nodes, make the bot duck down...
			//ucmd->upmove = -48;
			NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
		}
		else if
			(
				!(nodes[NPC->current_node].type & NODE_DUCK) &&
				!(nodes[NPC->last_node].type & NODE_DUCK) &&
				!NPC->enemy &&
				(NPC->NPC_client->ps.eFlags & EF_CROUCHING)
			)
		{											// Get up?
			ucmd->upmove = 127;
			NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
		}
		else if ( !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_PRONE) )
		{											// Get up?
			ucmd->upmove = 127;
			NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
		}

		if
		(
			NPC->bot_strafe_right_timer < level.time &&
			NPC->bot_strafe_left_timer < level.time &&
			(
				(
					VectorLength( NPC->NPC_client->ps.velocity) < 24 &&
					!(nodes[NPC->current_node].type & NODE_DUCK) &&
					!(nodes[NPC->last_node].type & NODE_DUCK)
				)									/*|| NPC->bot_last_good_move_time < level.time - 3000*/
			)
		)
		{											// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
			vec3_t	forward, right, up, rightOrigin, leftOrigin, temp;
			trace_t tr;
			AngleVectors( NPC->NPC_client->ps.viewangles, forward, right, up );
			VectorMA( NPC->r.currentOrigin, 64, right, rightOrigin );
			VectorMA( NPC->r.currentOrigin, -64, right, leftOrigin );
			if ( OrgVisible( rightOrigin, nodes[NPC->current_node].origin, NPC->s.number) )
			{
				VectorCopy( rightOrigin, NPC->bot_strafe_target_position );
				NPC->bot_strafe_right_timer = level.time + 101;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 64;
				ucmd->upmove = 0;
			}
			else if ( OrgVisible( leftOrigin, nodes[NPC->current_node].origin, NPC->s.number) )
			{
				VectorCopy( leftOrigin, NPC->bot_strafe_target_position );
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
			VectorCopy( NPC->r.currentOrigin, temp );
			temp[0] += forward[0] * 64;
			temp[1] += forward[1] * 64;
			temp[2] += forward[2] * 64;
			trap_Trace( &tr, NPC->r.currentOrigin, NULL, NULL, temp, -1, MASK_SHOT | MASK_OPAQUE );
			if ( tr.entityNum && (tr.entityNum < MAX_CLIENTS && g_entities[tr.entityNum].r.svFlags & SVF_BOT) )
			{										// A bot is stopping us... Tell them to strafe other direction to us...
				if ( NPC->bot_strafe_left_timer >= level.time )
				{
					g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;
					AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
					VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
					VectorCopy( leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
				}
				else if ( NPC->bot_strafe_right_timer >= level.time )
				{
					g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;
					AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
					VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
					VectorCopy( rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
				}
			}
		}

		if ( NPC->bot_strafe_right_timer >= level.time || NPC->bot_strafe_left_timer >= level.time )
		{
			return;
		}

		// Set up ideal view angles for this node...
		VectorSubtract( nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
		vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );
		NPC->bot_ideal_view_angles[2] = 0;			// Always look straight.. Don't look up or down at nodes...
		diff = fabs( AngleDifference( NPC->NPC_client->ps.viewangles[YAW], NPC->bot_ideal_view_angles[YAW]) );
		if ( diff > 90.0f )
		{											// Delay large turns...
			NPCSetViewAngles( NPC, 500 );
			VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
			ucmd->angles[PITCH] = ANGLE2SHORT( NPC->s.angles[PITCH] );
			ucmd->angles[YAW] = ANGLE2SHORT( NPC->s.angles[YAW] );
			ucmd->angles[ROLL] = ANGLE2SHORT( NPC->s.angles[ROLL] );
		}
		else
		{
			VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
			ucmd->angles[PITCH] = ANGLE2SHORT( NPC->s.angles[PITCH] );
			ucmd->angles[YAW] = ANGLE2SHORT( NPC->s.angles[YAW] );
			ucmd->angles[ROLL] = ANGLE2SHORT( NPC->s.angles[ROLL] );
			ucmd->forwardmove = 127;
			NPC->bot_last_good_move_time = level.time;
		}

		return;
	}

	if ( NPC->npc_hunter && !NPC->enemy && VectorDistance( NPC->SP_NPC_Position, NPC->r.currentOrigin) > 256 )
	{												// Return to our marker!
		if ( NPC->bot_strafe_right_timer >= level.time )
		{
			if ( VectorDistance( NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8 )
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

		if ( NPC->bot_strafe_left_timer >= level.time )
		{
			if ( VectorDistance( NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8 )
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
		}

		if
		(
			NPC->longTermGoal < 0 ||
			NPC->longTermGoal > number_of_nodes ||
			NPC->current_node < 0 ||
			NPC->next_node < 0 ||
			NPC->node_timeout <= level.time
		)
		{											// We hit a problem in route, or don't have one yet.. Find a new goal...
			memset( NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
			NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
			NPC->longTermGoal = AIMOD_NAVIGATION_FindClosestReachableNodeTo( NPC->SP_NPC_Position, NODE_DENSITY,
																			 NODEFIND_ALL );
			if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
			{										// Try to roam out of problems...
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;								// next think...
			}

			NPC->pathsize = NPC_CreatePathAStar( NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist );
			NPC->next_node = BotGetNextNode( NPC ); //move to this node first, since it's where our path starts from
			NPC->node_timeout = level.time + 5000;
		}

		if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
		{											// Try to roam out of problems...
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;									// next think...
		}

		if ( VectorDistance( nodes[NPC->longTermGoal].origin, NPC->r.currentOrigin) < 48 )
		{											// We're there! Find a new goal...
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;									// Next think...
		}

		if ( NPC->current_node <= 0 || NPC->current_node >= number_of_nodes )
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;									// Next think...
		}

		if ( VectorDistance( nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 48 )
		{											// At current node.. Pick next in the list...
			NPC->last_node = NPC->current_node;
			NPC->current_node = NPC->next_node;
			NPC->next_node = BotGetNextNode( NPC );
			NPC->node_timeout = level.time + 5000;
		}

		if
		(
			NPC->enemy &&
			NPC->action > level.time - 100 &&
			VectorDistance( NPC->r.currentOrigin, NPC->SP_NPC_Position) < 256
		)
		{											// We're close enough to our spawn position now... Don't move any more...
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;									// Next think...
		}

		if ( NPC->bot_last_good_move_time < level.time - 1000 && NPC->bot_last_move_fix_time < level.time - 2000 )
		{
			int result = -1;

			// So we don't vis check too often...
			NPC->bot_last_move_fix_time = level.time;
			result = NPC_MOVEMENT_ReachableBy( NPC, nodes[NPC->current_node].origin );
			if ( result == NOT_REACHABLE )
			{										// -1 is Not reachable at all... Reset our long term goal so it finds a new path next think!
				NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
				NPC->longTermGoal = 0;
				ucmd->forwardmove = 0;
				ucmd->upmove = 0;
				ucmd->rightmove = 0;
				return;
			}

			if ( result == REACHABLE_JUMP )
			{										// 1 is Jump...
				ucmd->upmove = 127;
				ucmd->forwardmove = 127;
				NPC->NPC_client->ps.velocity[2] = 256;
				return;
			}

			if ( result == REACHABLE_DUCK )
			{										// 2 is Duck...
				if ( !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
				{									// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
			}

			if ( result == REACHABLE_STRAFE_LEFT )
			{										// 3 is Strafe Left...
				ucmd->rightmove = -48;
				return;
			}

			if ( result == REACHABLE_STRAFE_RIGHT )
			{										// 4 is Strafe Right...
				ucmd->rightmove = 48;
				return;
			}
		}

		if ( NPC->last_node < 0 || NPC->last_node >= number_of_nodes )
		{
			if ( NPC->current_node >= 0 && NPC->current_node <= number_of_nodes )
			{
				NPC->last_node = NPC->current_node - 1;
			}
			else
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;								// Next think...
			}
		}

		if
		(
			nodes[NPC->current_node].type & NODE_JUMP ||
			nodes[NPC->current_node].origin[2] - NPC->r.currentOrigin[2] > 8
		)
		{											// For special jump nodes, make the bot jump...
			ucmd->upmove = 127;
		}
		else if ( nodes[NPC->current_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
		{											// For special duck nodes, make the bot duck down...
			//ucmd->upmove = -48;
			NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
		}
		else if ( nodes[NPC->last_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
		{											// For special duck nodes, make the bot duck down...
			//ucmd->upmove = -48;
			NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
		}
		else if
			(
				!(nodes[NPC->current_node].type & NODE_DUCK) &&
				!(nodes[NPC->last_node].type & NODE_DUCK) &&
				!NPC->enemy &&
				(NPC->NPC_client->ps.eFlags & EF_CROUCHING)
			)
		{											// Get up?
			ucmd->upmove = 127;
			NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
		}
		else if ( !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_PRONE) )
		{											// Get up?
			ucmd->upmove = 127;
			NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
		}

		/*
				if (NPC->bot_strafe_right_timer < level.time && NPC->bot_strafe_left_timer < level.time
					&& ( ( VectorLength(NPC->NPC_client->ps.velocity) < 24 && !( nodes[NPC->current_node].type & NODE_DUCK ) && !( nodes[NPC->last_node].type & NODE_DUCK ) ) ) )
				{// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
					vec3_t forward, right, up, rightOrigin, leftOrigin, temp;
					trace_t tr;

					AngleVectors( NPC->NPC_client->ps.viewangles, forward, right, up );
					VectorMA( NPC->r.currentOrigin, 64, right, rightOrigin );
					VectorMA( NPC->r.currentOrigin, -64, right, leftOrigin );

					if (OrgVisible(rightOrigin, nodes[NPC->current_node].origin, NPC->s.number))
					{
						VectorCopy(rightOrigin, NPC->bot_strafe_target_position);
						NPC->bot_strafe_right_timer = level.time + 101;
						ucmd->forwardmove = 0;
						ucmd->rightmove = 64;
						ucmd->upmove = 0;
					}
					else if (OrgVisible(leftOrigin, nodes[NPC->current_node].origin, NPC->s.number))
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
		VectorSubtract( nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
		vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );
		NPC->bot_ideal_view_angles[2] = 0;			// Always look straight.. Don't look up or down at nodes...
		diff = fabs( AngleDifference( NPC->NPC_client->ps.viewangles[YAW], NPC->bot_ideal_view_angles[YAW]) );
		if ( diff > 90.0f )
		{											// Delay large turns...
			NPCSetViewAngles( NPC, 500 );
			VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
			ucmd->angles[PITCH] = ANGLE2SHORT( NPC->s.angles[PITCH] );
			ucmd->angles[YAW] = ANGLE2SHORT( NPC->s.angles[YAW] );
			ucmd->angles[ROLL] = ANGLE2SHORT( NPC->s.angles[ROLL] );
		}
		else
		{
			VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
			ucmd->angles[PITCH] = ANGLE2SHORT( NPC->s.angles[PITCH] );
			ucmd->angles[YAW] = ANGLE2SHORT( NPC->s.angles[YAW] );
			ucmd->angles[ROLL] = ANGLE2SHORT( NPC->s.angles[ROLL] );
			if ( ucmd->upmove > 0 )					// We need to jump something...
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
		}

		return;
	}

	if ( NPC->npc_patrol )
	{												// We're a patroling NPC!
		if ( NPC->bot_strafe_right_timer >= level.time )
		{
			if ( VectorDistance( NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8 )
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

		if ( NPC->bot_strafe_left_timer >= level.time )
		{
			if ( VectorDistance( NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8 )
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
		}

		if
		(
			NPC->longTermGoal < 0 ||
			NPC->longTermGoal > number_of_nodes ||
			NPC->current_node < 0 ||
			NPC->next_node < 0 ||
			NPC->node_timeout <= level.time
		)
		{											// We hit a problem in route, or don't have one yet.. Find a new goal...
			int		best_node = -1;
			float	best_dist = 1024.0f;
			int		node = 0;
			memset( NPC->pathlist, NODE_INVALID, MAX_PATHLIST_NODES );
			NPC->current_node = NPC->npc_patrol_start_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC,
																										NODE_DENSITY, NODEFIND_ALL,
																										NPC );
			for ( node = 0; node < number_of_nodes; node++ )
			{										// Find a good node to patrol to/from...
				float	node_dist = VectorDistance( NPC->r.currentOrigin, nodes[node].origin );
				if ( node_dist < best_dist && node_dist > 512 )
				{
					best_dist = node_dist;
					best_node = node;
				}
			}

			if ( best_node != -1 )
			{
				NPC->longTermGoal = NPC->npc_patrol_end_node = best_node;
			}
			else
			{										// We didnt find a good node to patrol to/from... Just stand there...
				NPC->npc_patrol = qfalse;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;
			}

			if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
			{										// Try to roam out of problems...
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;								// next think...
			}

			NPC->pathsize = NPC_CreatePathAStar( NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist );
			NPC->next_node = BotGetNextNode( NPC ); //move to this node first, since it's where our path starts from
			NPC->node_timeout = level.time + 5000;
		}

		if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
		{											// Try to roam out of problems...
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;									// next think...
		}

		if ( VectorDistance( nodes[NPC->longTermGoal].origin, NPC->r.currentOrigin) < 48 )
		{											// We're there! Head back to our original point again...
			NPC->longTermGoal = NPC->npc_patrol_start_node;
			if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
			{										// Try to roam out of problems...
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;								// next think...
			}

			NPC->pathsize = NPC_CreatePathAStar( NPC, NPC->current_node, NPC->longTermGoal, NPC->pathlist );
			NPC->next_node = BotGetNextNode( NPC ); //move to this node first, since it's where our path starts from
			NPC->node_timeout = level.time + 5000;
			return;									// Next think...
		}

		if ( NPC->current_node <= 0 || NPC->current_node >= number_of_nodes )
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;									// Next think...
		}

		if ( VectorDistance( nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 48 )
		{											// At current node.. Pick next in the list...
			NPC->last_node = NPC->current_node;
			NPC->current_node = NPC->next_node;
			NPC->next_node = BotGetNextNode( NPC );
			NPC->node_timeout = level.time + 5000;
		}

		if ( NPC->bot_last_good_move_time < level.time - 1000 && NPC->bot_last_move_fix_time < level.time - 2000 )
		{
			int result = -1;

			// So we don't vis check too often...
			NPC->bot_last_move_fix_time = level.time;
			result = NPC_MOVEMENT_ReachableBy( NPC, nodes[NPC->current_node].origin );
			if ( result == NOT_REACHABLE )
			{										// -1 is Not reachable at all... Reset our long term goal so it finds a new path next think!
				NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
				NPC->longTermGoal = -1;
				ucmd->forwardmove = 0;
				ucmd->upmove = 0;
				ucmd->rightmove = 0;
				return;
			}

			if ( result == REACHABLE_JUMP )
			{										// 1 is Jump...
				ucmd->upmove = 127;
				ucmd->forwardmove = 127;
				NPC->NPC_client->ps.velocity[2] = 256;
				return;
			}

			if ( result == REACHABLE_DUCK )
			{										// 2 is Duck...
				if ( !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
				{									// For special duck nodes, make the bot duck down...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}
			}

			if ( result == REACHABLE_STRAFE_LEFT )
			{										// 3 is Strafe Left...
				ucmd->rightmove = -48;
				return;
			}

			if ( result == REACHABLE_STRAFE_RIGHT )
			{										// 4 is Strafe Right...
				ucmd->rightmove = 48;
				return;
			}
		}

		if ( NPC->last_node < 0 || NPC->last_node >= number_of_nodes )
		{
			if ( NPC->current_node >= 0 && NPC->current_node <= number_of_nodes )
			{
				NPC->last_node = NPC->current_node - 1;
			}
			else
			{
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				return;								// Next think...
			}
		}

		if
		(
			nodes[NPC->current_node].type & NODE_JUMP ||
			nodes[NPC->current_node].origin[2] - NPC->r.currentOrigin[2] > 8
		)
		{											// For special jump nodes, make the bot jump...
			ucmd->upmove = 127;
		}
		else if ( nodes[NPC->current_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
		{											// For special duck nodes, make the bot duck down...
			//ucmd->upmove = -48;
			NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
		}
		else if ( nodes[NPC->last_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
		{											// For special duck nodes, make the bot duck down...
			//ucmd->upmove = -48;
			NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
		}
		else if
			(
				!(nodes[NPC->current_node].type & NODE_DUCK) &&
				!(nodes[NPC->last_node].type & NODE_DUCK) &&
				!NPC->enemy &&
				(NPC->NPC_client->ps.eFlags & EF_CROUCHING)
			)
		{											// Get up?
			ucmd->upmove = 127;
			NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
		}
		else if ( !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_PRONE) )
		{											// Get up?
			ucmd->upmove = 127;
			NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
		}

		if
		(
			NPC->bot_strafe_right_timer < level.time &&
			NPC->bot_strafe_left_timer < level.time &&
			(
				(
					VectorLength( NPC->NPC_client->ps.velocity) < 24 &&
					!(nodes[NPC->current_node].type & NODE_DUCK) &&
					!(nodes[NPC->last_node].type & NODE_DUCK)
				)									/*|| NPC->bot_last_good_move_time < level.time - 3000*/
			)
		)
		{											// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
			vec3_t	forward, right, up, rightOrigin, leftOrigin, temp;
			trace_t tr;
			AngleVectors( NPC->NPC_client->ps.viewangles, forward, right, up );
			VectorMA( NPC->r.currentOrigin, 64, right, rightOrigin );
			VectorMA( NPC->r.currentOrigin, -64, right, leftOrigin );
			if ( OrgVisible( rightOrigin, nodes[NPC->current_node].origin, NPC->s.number) )
			{
				VectorCopy( rightOrigin, NPC->bot_strafe_target_position );
				NPC->bot_strafe_right_timer = level.time + 101;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 64;
				ucmd->upmove = 0;
			}
			else if ( OrgVisible( leftOrigin, nodes[NPC->current_node].origin, NPC->s.number) )
			{
				VectorCopy( leftOrigin, NPC->bot_strafe_target_position );
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
			VectorCopy( NPC->r.currentOrigin, temp );
			temp[0] += forward[0] * 64;
			temp[1] += forward[1] * 64;
			temp[2] += forward[2] * 64;
			trap_Trace( &tr, NPC->r.currentOrigin, NULL, NULL, temp, -1, MASK_SHOT | MASK_OPAQUE );
			if ( tr.entityNum && (tr.entityNum < MAX_CLIENTS && g_entities[tr.entityNum].r.svFlags & SVF_BOT) )
			{										// A bot is stopping us... Tell them to strafe other direction to us...
				if ( NPC->bot_strafe_left_timer >= level.time )
				{
					g_entities[tr.entityNum].bot_strafe_left_timer = level.time + 200;
					AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
					VectorMA( g_entities[tr.entityNum].r.currentOrigin, -64, right, leftOrigin );
					VectorCopy( leftOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
				}
				else if ( NPC->bot_strafe_right_timer >= level.time )
				{
					g_entities[tr.entityNum].bot_strafe_right_timer = level.time + 200;
					AngleVectors( g_entities[tr.entityNum].client->ps.viewangles, forward, right, up );
					VectorMA( g_entities[tr.entityNum].r.currentOrigin, 64, right, rightOrigin );
					VectorCopy( rightOrigin, g_entities[tr.entityNum].bot_strafe_target_position );
				}
			}
		}

		if ( NPC->bot_strafe_right_timer >= level.time || NPC->bot_strafe_left_timer >= level.time )
		{
			return;
		}

		// Set up ideal view angles for this node...
		VectorSubtract( nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
		vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );
		NPC->bot_ideal_view_angles[2] = 0;			// Always look straight.. Don't look up or down at nodes...
		diff = fabs( AngleDifference( NPC->NPC_client->ps.viewangles[YAW], NPC->bot_ideal_view_angles[YAW]) );
		if ( diff > 90.0f )
		{											// Delay large turns...
			NPCSetViewAngles( NPC, 500 );
			VectorCopy( NPC->NPC_client->ps.viewangles, NPC->s.angles );
			ucmd->angles[PITCH] = ANGLE2SHORT( NPC->s.angles[PITCH] );
			ucmd->angles[YAW] = ANGLE2SHORT( NPC->s.angles[YAW] );
			ucmd->angles[ROLL] = ANGLE2SHORT( NPC->s.angles[ROLL] );
		}
		else
		{
			VectorCopy( NPC->bot_ideal_view_angles, NPC->s.angles );
			ucmd->angles[PITCH] = ANGLE2SHORT( NPC->s.angles[PITCH] );
			ucmd->angles[YAW] = ANGLE2SHORT( NPC->s.angles[YAW] );
			ucmd->angles[ROLL] = ANGLE2SHORT( NPC->s.angles[ROLL] );
			if ( ucmd->upmove > 0 )					// We need to jump something...
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
		}

		return;
	}

	return;
}


/* */
void
NPC_Waypoint_Move ( gentity_t *NPC, usercmd_t *ucmd )
{
	qboolean	onLadder = qfalse;
	qboolean	ladderMove = qfalse;
	if ( NPC->bot_stand_guard )
	{		// A bot that is standing guard...
		if
		(
			Have_Close_Enemy( NPC) &&
			NPC->beStillEng < level.time &&
			NPC->beStill < level.time &&
			NPC->bot_enemy_visible_time > level.time &&
			NPC->enemy == NPC->bot_last_visible_enemy &&
			NPC->bot_runaway_time < level.time
		)
		{	// Do attacking first...
			//AIMOD_MOVEMENT_Attack(NPC, ucmd);
			return;
		}
		else
		{
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->node_timeout = level.time + ( TravelTime( NPC) * 2 );	// Duck moves take longer, so...

			// Set our last good move time to now...
			NPC->bot_last_good_move_time = level.time;
		}

		//G_Printf("NPC (%i) stand guard.\n", NPC->s.number);
		return;
	}

	if ( NPC->bot_last_position_check < level.time - 4000 )
	{														// Check if we have moved in the last 10 seconds... (Are we stuck somewhere?)
		if
		(
			NPC->goalentity &&
			!Q_stricmp( NPC->goalentity->classname, va( "bot_sniper_spot")) &&
			VectorDistance( NPC->goalentity->r.currentOrigin, NPC->r.currentOrigin) < 256
		)
		{

		}
		else if ( VectorDistance( NPC->bot_last_position, NPC->r.currentOrigin) <= 64 )
		{													// We are stuck... Find a new route!
			NPC->current_node = -1;
			NPC->longTermGoal = -1;
			Bot_Set_New_Path( NPC, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->bot_last_position_check = level.time;
			VectorCopy( NPC->r.currentOrigin, NPC->bot_last_position );

			//G_Printf("NPC (%i) position check.\n", NPC->s.number);
			return;
		}

		NPC->bot_last_position_check = level.time;
		VectorCopy( NPC->r.currentOrigin, NPC->bot_last_position );
	}

	/*	if (nodes_loaded 
		&& NPC->goalentity
		&& !Q_stricmp( NPC->goalentity->classname, va("bot_sniper_spot") )
		&& VectorDistance(nodes[NPC->longTermGoal].origin, NPC->r.currentOrigin) < 24 )
	{// A sniper... Make him look around in snipe mode for enemies...
		AIMOD_Movement_SniperMove(NPC, ucmd);
		
		if (test_last_message[NPC->s.number] < level.time - 3000)
		{
			G_Printf("^2*** ^3BOT DEBUG^5: ^3%s^5's has found his sniper point.\n", NPC->NPC_client->pers.netname);
			test_last_message[NPC->s.number] = level.time;
		}

		return;
	}
	else if (nodes_loaded 
		&& NPC->goalentity
		&& !Q_stricmp(NPC->goalentity->classname, va("bot_sniper_spot")) )
	{
		if (test_last_message[NPC->s.number] < level.time - 3000)
		{
			G_Printf("^2*** ^3BOT DEBUG^5: ^3%s^5's distance from his sniper point is ^7%f^5.\n", NPC->NPC_client->pers.netname, VectorDistance(NPC->goalentity->r.currentOrigin, NPC->r.currentOrigin));
			test_last_message[NPC->s.number] = level.time;
		}
	}*/
	if ( nodes_loaded && NPC->current_node > 0 )
	{
		if ( !AIMOD_MOVEMENT_CurrentNodeReachable( NPC) )
		{													// If our current node is not visible, then we need to try something else...
			NPC->current_node = -1;
			Bot_Set_New_Path( NPC, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;

			//G_Printf("NPC (%i) current node unreachable.\n", NPC->s.number);
			return;
		}
		else
		{

			//if (HaveSequentialNodes())
			if ( NPC->pathsize > 10 )
			{
				AIMOD_MOVEMENT_Shorten_Path( NPC );
			}
		}
	}

	//	if (NPC->NPC_client->ps.weapon == WP_MOBILE_MG42_SET)
	//		return;
	if ( NPC->health <= 0 )
	{
		return;
	}

	//	if (NPC->NPC_client->ps.pm_flags & PMF_LIMBO)
	//		return;
	if ( !NPC->r.currentOrigin )
	{
		return;
	}

	//	if (NPC->NPC_client->sess.playerType != PC_SOLDIER) // Soldiers shouldn't need special moves...
	//		if (AIMOD_MOVEMENT_SpecialMoves(NPC, ucmd))
	//			return;
#ifdef __WEAPON_AIM__
	if ( NPC->NPC_client->ps.eFlags & EF_AIMING )
	{
		ucmd->buttons |= BUTTON_WALKING;
	}
	else
	{
		ucmd->buttons &= ~BUTTON_WALKING;
	}
#endif //__WEAPON_AIM__
	if
	(
		NPC->enemy &&
		NPC->beStillEng < level.time &&
		NPC->beStill < level.time &&
		NPC->bot_enemy_visible_time > level.time &&
		NPC->enemy == NPC->bot_last_visible_enemy &&
		NPC->bot_runaway_time < level.time
	)
	{														// Do attacking first...
		//AIMOD_MOVEMENT_AttackMove(NPC, ucmd);
		//G_Printf("NPC (%i) attack.\n", NPC->s.number);
		return;
	}

	// ---------------------------------------------
	// OK... First the movement for non-routed maps.
	// ---------------------------------------------
	//	if ( !Bot_Ignore_Use_Time[NPC->s.clientNum] )
	//		Bot_Ignore_Use_Time[NPC->s.clientNum] = 0;
#ifdef __SUPREMACY__
	if ( nodes_loaded && NPC->current_node )
	{														// We have a routed map.. Do some flag capture checks...
		if
		(
			NPC->NPC_client->ps.stats[STAT_FLAG_CAPTURE] > 0 &&
			g_entities[NPC->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != NPC->NPC_client->sess.sessionTeam &&
			NPC->current_node > 0
		)
		{
			if
			(
				!BotEntityCloserOnFuturePath( NPC, &g_entities[NPC->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]]) &&
				!Have_Close_Enemy( NPC)
			)
			{												// If we're capturing a flag in POP gametype don't move, just defend current pos...
				NPC->bot_last_good_move_time = level.time;
				NPC->node_timeout = level.time + TravelTime( NPC );

				//G_Printf("NPC (%i) flag capture.\n", NPC->s.number);
				return;										// Don't move while capturing a flag...
			}
		}

		if
		(
			NPC->enemy &&
			NPC->beStillEng < level.time &&
			NPC->beStill < level.time &&
			NPC->bot_enemy_visible_time > level.time &&
			NPC->enemy == NPC->bot_last_visible_enemy &&
			NPC->bot_runaway_time < level.time
		)
		{													// Do attacking first...
			//AIMOD_MOVEMENT_AttackMove(NPC, ucmd);
			//G_Printf("NPC (%i) attack 2.\n", NPC->s.number);
			return;
		}
	}
	else
	{
		if
		(
			NPC->NPC_client->ps.stats[STAT_FLAG_CAPTURE] > 0 &&
			g_entities[NPC->NPC_client->ps.stats[STAT_CAPTURE_ENTITYNUM]].s.modelindex != NPC->NPC_client->sess.
				sessionTeam
		)
		{
			if ( !Have_Close_Enemy( NPC) )
			{												// If we're capturing a flag in POP gametype don't move, just defend current pos...
				NPC->bot_last_good_move_time = level.time;
				NPC->node_timeout = level.time + TravelTime( NPC );

				//G_Printf("NPC (%i) capture flag 2.\n", NPC->s.number);
				return;										// Don't move while capturing a flag...
			}
		}

		if
		(
			NPC->enemy &&
			NPC->beStillEng < level.time &&
			NPC->beStill < level.time &&
			NPC->bot_enemy_visible_time > level.time &&
			NPC->enemy == NPC->bot_last_visible_enemy &&
			NPC->bot_runaway_time < level.time
		)
		{													// Do attacking first...
			//AIMOD_MOVEMENT_AttackMove(NPC, ucmd);
			//G_Printf("NPC (%i) attack 3.\n", NPC->s.number);
			return;
		}
	}
#endif //__SUPREMACY__
	if
	(
		NPC->NPC_client->ps.serverCursorHint == HINT_LADDER ||
		NPC->NPC_client->ps.torsoAnim == BOTH_CLIMB ||
		NPC->NPC_client->ps.legsAnim == BOTH_CLIMB
	)
	{
		onLadder = qtrue;
	}

	if ( onLadder )
	{														// Always go up/down!
		if ( VectorDistance( nodes[NPC->current_node].origin, NPC->r.currentOrigin) < 16 )
		{
			NPC->last_node = NPC->current_node;
			NPC->current_node = NPC->next_node;
			NPC->next_node = BotGetNextNode( NPC );
		}

		ucmd->forwardmove = 127;

		// Set up ideal view angles for this enemy...
		VectorSubtract( nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
		vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );

		//BotSetViewAngles(NPC, 100);
		NPCSetViewAngles( NPC, 100 );
		return;
	}

	// ----------------------------------------
	// OK... Now the movement for routed maps.
	// ----------------------------------------
	if ( NPC->current_node < 0 )
	{
		NPC->current_node = AIMOD_NAVIGATION_FindClosestReachableNode( NPC, NODE_DENSITY, NODEFIND_ALL, NPC );
		if ( NPC->current_node < 0 )
		{
			NPC->bot_random_move_time = level.time + 500;
		}
	}

	if ( NPC->bot_random_move_time > level.time )
	{														// Get out of trouble with non-existant waypoint!
		AIMOD_MOVEMENT_Backup( NPC, ucmd );
		return;
	}

	if ( !onLadder && BAD_WP_Height( nodes[NPC->current_node].origin, NPC->r.currentOrigin) && !onLadder )
	{														// Height diff is greater then plane distance... Block it and skip it..
		int loop = 0;
		int link = -1;
		for ( loop = 0; loop < nodes[NPC->last_node].enodenum; loop++ )
		{													// Find the link we last used and mark it...
			if ( nodes[NPC->last_node].links[loop].targetNode == NPC->current_node )
			{												// Mark it as blocked...
				BOT_MakeLinkUnreachable( NPC, NPC->last_node, loop );
				break;
			}
		}

		Bot_Set_New_Path( NPC, ucmd );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;

		//G_Printf("NPC (%i) BAD_WP_Height.\n", NPC->s.number);
		return;
	}

	//#ifdef __BOT_STRAFE__
	if ( NPC->bot_strafe_right_timer >= level.time )
	{
		if ( VectorDistance( NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8 )
		{
			ucmd->forwardmove = 0;							//64;
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

	if ( NPC->bot_strafe_left_timer >= level.time )
	{
		if ( VectorDistance( NPC->bot_strafe_target_position, NPC->r.currentOrigin) > 8 )
		{
			ucmd->forwardmove = 0;							//64;
			ucmd->upmove = 0;
			ucmd->rightmove = -64;
			return;
		}
		else
		{
			NPC->bot_strafe_left_timer = 0;
			ucmd->upmove = 127;
		}
	}

	if ( NPC->bot_duck_time > level.time )
	{
		if ( !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
		{													// For special duck nodes, make the bot duck down...
			//ucmd->upmove = -48;
			NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
		}

		ucmd->forwardmove = 127;
		return;
	}
	else if ( NPC->bot_strafe_left_time > level.time )
	{
		ucmd->rightmove = -48;
		return;
	}
	else if ( NPC->bot_strafe_right_time > level.time )
	{
		ucmd->rightmove = 48;
		return;
	}
	else if ( NPC->bot_jump_time > level.time )
	{
		ucmd->upmove = 127;

		//ucmd->forwardmove = 127;
		//return;
	}

	/*	else if (NPC->bot_last_good_move_time < level.time - 1000
		&& NPC->bot_last_move_fix_time < level.time - 1000 
		&& !onLadder )
	{// The first type of route correction...
		int result = -1;

		// So we don't vis check too often...
		NPC->bot_last_move_fix_time = level.time;

		result = AIMOD_MOVEMENT_ReachableBy(NPC, nodes[NPC->current_node].origin);

		if (result == -1)
		{// -1 is Not reachable at all... Let's use some blind routing for a short time!
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->longTermGoal = -1;
			NPC->shortTermGoal = -1;
			//G_Printf("NPC (%i) strafe unreach.\n", NPC->s.number);
			return;
		}

		if (result == 1)
		{// 1 is Jump...
			ucmd->upmove = 127;
			NPC->bot_jump_time = level.time + 500;
		}

		if (result == 2)
		{// 2 is Duck...
			if (!(NPC->NPC_client->ps.pm_flags & PMF_DUCKED))
			{// For special duck nodes, make the bot duck down...
				//ucmd->upmove = -48;
				NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				NPC->bot_duck_time = level.time + 2000;
			}
		}

		if (result == 3)
		{// 3 is Strafe Left...
			ucmd->rightmove = -48;
			NPC->bot_strafe_left_time = level.time + 1000;
			return;
		}

		if (result == 4)
		{// 4 is Strafe Right...
			ucmd->rightmove = 48;
			NPC->bot_strafe_right_time = level.time + 1000;
			return;
		}
	}
#endif //__BOT_STRAFE__
*/
	if
	(
		NPC->longTermGoal < 0 ||
		NPC->longTermGoal > number_of_nodes ||
		NPC->current_node < 0 ||
		NPC->node_timeout <= level.time ||
		NPC->bot_last_good_move_time < level.time - 2000
	)
	{														// We hit a problem in route, or don't have one yet.. Find a new goal...
		if
		(
			!onLadder &&
			!(
				NPC->longTermGoal < 0 ||
				NPC->longTermGoal > number_of_nodes ||
				NPC->current_node < 0 ||
				NPC->next_node < 0
			) &&
			(NPC->node_timeout <= level.time || NPC->bot_last_good_move_time < level.time - 2000)
		)
		{													// Bad current path link.. Mark it as blocked...
			int loop = 0;
			int link = -1;
			for ( loop = 0; loop < nodes[NPC->last_node].enodenum; loop++ )
			{												// Find the link we last used and mark it...
				if ( nodes[NPC->last_node].links[loop].targetNode == NPC->current_node )
				{											// Mark it as blocked...
					BOT_MakeLinkUnreachable( NPC, NPC->last_node, loop );
					break;
				}
			}

			/*			if ( (NPC->NPC_client->sess.sessionTeam == TEAM_AXIS && numAxisOnlyNodes > 0)
				|| (NPC->NPC_client->sess.sessionTeam == TEAM_ALLIES && numAlliedOnlyNodes > 0) )
			{
				NPC->pathsize = CreateTeamRoute(NPC, NPC->pathlist);
			
				if (NPC->pathsize > 0)
				{
					NPC->next_node = BotGetNextNode(NPC);		//move to this node first, since it's where our path starts from
					NPC->node_timeout = level.time + TravelTime(NPC);//6000;//4000;
					NPC->bot_last_good_move_time = level.time;
					return;
				}
				else
				{
					NPC->longTermGoal = -1;
					NPC->shortTermGoal = -1;
					return;
				}
			}*/
		}

		Bot_Set_New_Path( NPC, ucmd );
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		NPC->node_timeout = level.time + TravelTime( NPC ); //6000;//4000;
		NPC->bot_last_good_move_time = level.time;

		//G_Printf("NPC (%i) node timeout.\n", NPC->s.number);
		return;
	}

	if ( NPC->longTermGoal < 0 || NPC->longTermGoal > number_of_nodes )
	{														// Try to roam out of problems...
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		NPC->longTermGoal = -1;
		NPC->shortTermGoal = -1;
		NPC->bot_random_move_time = level.time + 500;

		//G_Printf("NPC (%i) bad longterm goal.\n", NPC->s.number);
		return;												// next think...
	}

	if ( nodes[NPC->current_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
	{														// For special duck nodes, make the bot duck down...
		//ucmd->upmove = -48;
		NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
	}
	else if ( nodes[NPC->last_node].type & NODE_DUCK && !(NPC->NPC_client->ps.pm_flags & PMF_DUCKED) )
	{														// For special duck nodes, make the bot duck down...
		//ucmd->upmove = -48;
		NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
	}
	else if
		(
			!(nodes[NPC->current_node].type & NODE_DUCK) &&
			!(nodes[NPC->last_node].type & NODE_DUCK) &&
			!NPC->enemy &&
			(NPC->NPC_client->ps.eFlags & EF_CROUCHING)
		)
	{														// Get up?
		ucmd->upmove = 127;
		NPC->NPC_client->ps.eFlags &= ~EF_CROUCHING;
	}
	else if ( !NPC->enemy && (NPC->NPC_client->ps.eFlags & EF_PRONE) )
	{														// Get up?
		ucmd->upmove = 127;
		NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
	}

	// Now we need to check if we have hit our long term node...
	if ( NPC->current_node == NPC->longTermGoal && AIMOD_MOVEMENT_WaypointTouched( NPC, NPC->current_node, onLadder) )
	{														// We're there! Find a new goal... FIXME: We should actually do something at most goals..
		if ( NPC->followtarget && VectorDistance( NPC->followtarget->r.currentOrigin, NPC->r.currentOrigin) < 256 )
		{													// A following bot, at it's owner... Just keep standing with him for now...
			//FIXME: Make them move around just a little near their owner... Maybe some anims/sounds??? - Unique1
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->node_timeout = level.time + ( TravelTime( NPC) * 2 );		// Duck moves take longer, so...

			// Set our last good move time to now...
			NPC->bot_last_good_move_time = level.time;

			//G_Printf("NPC (%i) LTG waypoint touched.\n", NPC->s.number);
			return;															// Think again soon!
		}

		if
		(
			NPC->bot_defender &&
			!(NPC->bot_goal_constructible && NPC->NPC_client->sess.sessionTeam == level.attackingTeam)
		)
		{

			//			if (!(BotMP_Num_Defenders_Near ( NPC->bot_goal_position, NPC ) > 2) )
			{
				NPC->bot_stand_guard = qtrue;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				NPC->node_timeout = level.time + ( TravelTime( NPC) * 2 );	// Duck moves take longer, so...

				// Set our last good move time to now...
				NPC->bot_last_good_move_time = level.time;

				//G_Printf("NPC (%i) defender.\n", NPC->s.number);
				return;
			}
		}

		if ( NPC->goalentity && NPC->goalentity->s.eType == ET_DEFENCEPOINT_HINT )
		{

			//			if (!(BotMP_Num_Defenders_Near ( NPC->goalentity->s.origin, NPC ) > (bot_minplayers.integer*0.35)) )
			{
				NPC->bot_stand_guard = qtrue;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;
				ucmd->upmove = 0;
				NPC->node_timeout = level.time + ( TravelTime( NPC) * 2 );	// Duck moves take longer, so...

				// Set our last good move time to now...
				NPC->bot_last_good_move_time = level.time;

				//G_Printf("NPC (%i) defence point.\n", NPC->s.number);
				return;
			}
		}

		if ( NPC->longterm_pathsize > 0 )
		{
			int i;
			NPC->pathsize = NPC->longterm_pathsize;
			for ( i = 0; i < NPC->pathsize; i++ )
			{
				NPC->pathlist[i] = NPC->longterm_pathlist[i];
			}

			NPC->current_node = NPC->shortTermGoal;
			NPC->next_node = BotGetNextNode( NPC );
			NPC->longterm_pathsize = -1;
			NPC->shortTermGoal = -1;
		}
		else
		{
			Bot_Set_New_Path( NPC, ucmd );
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			NPC->node_timeout = level.time + TravelTime( NPC );				//6000;//4000;
			NPC->bot_last_good_move_time = level.time;
			return;
		}
	}

	// Now we need to check if we have hit our current node...
	if ( AIMOD_MOVEMENT_WaypointTouched( NPC, NPC->current_node, onLadder) )
	{	// At current node.. Pick next in the list...
		NPC->last_node = NPC->current_node;
		NPC->current_node = NPC->next_node;
		NPC->next_node = BotGetNextNode( NPC );
		NPC->node_timeout = level.time + TravelTime( NPC );
		if ( NPC->current_node > number_of_nodes )
		{
			NPC->current_node = -1;
			NPC->next_node = -1;

			//G_Printf("NPC (%i) current wp touched.\n", NPC->s.number);
			return;
		}

		if ( nodes[NPC->current_node].type & NODE_DUCK )
		{
			NPC->node_timeout = level.time + ( TravelTime( NPC) * 2 );	// Duck moves take longer, so...
		}

		// Set our last good move time to now...
		NPC->bot_last_good_move_time = level.time;
		NPC->node_timeout = level.time + TravelTime( NPC );
	}

	if ( nodes[NPC->current_node].origin[2] > NPC->r.currentOrigin[2] && !onLadder )
	{	// Do we need to jump to our next node?
		vec3_t	temp, temp2;
		VectorCopy( nodes[NPC->current_node].origin, temp );
		temp[2] = NPC->r.currentOrigin[2];
		VectorCopy( nodes[NPC->current_node].origin, temp2 );
		temp2[2] = NPC->r.currentOrigin[2] + 16;
		if ( nodes[NPC->current_node].origin[2] - NPC->r.currentOrigin[2] > 16	 /*&& VectorLength(NPC->NPC_client->ps.velocity) < 24*/ )
		{
			ucmd->upmove = 127;
		}
	}

#ifdef __BOT_STRAFE__
	if
	(
		NPC->bot_strafe_right_timer < level.time &&
		NPC->bot_strafe_left_timer < level.time &&
		VectorLength( NPC->NPC_client->ps.velocity) < 24 &&
		!(nodes[NPC->current_node].type & NODE_DUCK) &&
		!(nodes[NPC->last_node].type & NODE_DUCK) &&
		!onLadder
	)
	{	// We need to make our body fit around a doorway or something.. Node is visible, but half our body is stuck..
		AIMOD_MOVEMENT_Setup_Strafe( NPC, ucmd );
	}

	if ( NPC->bot_strafe_right_timer >= level.time || NPC->bot_strafe_left_timer >= level.time )
	{
		return;
	}
#endif //__BOT_STRAFE__

	// Set up ideal view angles for this enemy...
	VectorSubtract( nodes[NPC->current_node].origin, NPC->r.currentOrigin, NPC->move_vector );
	NPC->move_vector[ROLL] = 0;
	vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );

	//BotSetViewAngles(NPC, 100);
	NPCSetViewAngles( NPC, 100 );
	if ( NPC->bot_turn_wait_time <= level.time )
	{
		G_UcmdMoveForDir( NPC, ucmd, NPC->move_vector );
	}

	// Set our last good move time to now...
	NPC->bot_last_good_move_time = level.time;

	//G_Printf("NPC %i - Forward: %f. Right %f.\n", NPC->s.number, ucmd->forwardmove, ucmd->rightmove);
}


/* */
void
NPC_AI_Humanoid_Move ( gentity_t *NPC )
{

	/*	gentity_t *obstacle = NULL;
	
	G_SetMovedir (NPC->s.angles, NPC->movedir);

	NPC->s.pos.trDelta[0] = 120;
	NPC->s.pos.trDelta[1] = 120;
	NPC->s.pos.trDelta[2] = 120;
	NPC->instantVelocity[0] = 120;
	NPC->instantVelocity[1] = 120;
	NPC->instantVelocity[2] = 120;

	trap_SnapVector(NPC->instantVelocity);
	trap_SnapVector(NPC->s.pos.trDelta);*/
	if ( NPC_NodesLoaded() && (NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER) )
	{		// Humanoid fodder type NPC routing here...
		NPC_AI_FODDER_Humanoid_Move( NPC );

		//G_Printf("Using NPC_AI_FODDER_Humanoid_Move()\n");
		return;
	}
	else
	{		// Humanoid type NPC routing here... (Now default)
		usercmd_t	*ucmd = &NPC->NPC_client->pers.cmd;

		//		if( NPC->s.dl_intensity == TAG_HOOK || NPC->s.dl_intensity == TAG_HOOK_DRIVER || NPC->s.dl_intensity == TAG_HOOK_GUNNER1 || NPC->s.dl_intensity == TAG_HOOK_GUNNER2 )
		//			return; // No moving while riding an NPC...
#ifdef __WEAPON_AIM__
		if ( NPC->NPC_client->ps.eFlags & EF_AIMING )
		{
			ucmd->buttons |= BUTTON_WALKING;
		}
		else
		{
			ucmd->buttons &= ~BUTTON_WALKING;
		}
#endif //__WEAPON_AIM__
		if ( g_gametype.integer == GT_COOP || g_gametype.integer == GT_SINGLE_PLAYER )
		{	// Coop NPCs need their own routing...
			NPC_SinglePlayer_Move( NPC, ucmd );

			//G_Printf("Using NPC_SinglePlayer_Move()\n");
		}
		else if ( !nodes_loaded )
		{	// Randomize!
			NPC_Backup_Move( NPC, ucmd );

			//G_Printf("Using NPC_Backup_Move()\n");
		}

		/*else if ( npc_simplenav.integer || NPC->bot_blind_routing_time > level.time)
		{// Mindless routing for using large numbers of NPCs...
			NPC_Blind_Move(NPC, ucmd);
			G_Printf("Using NPC_Blind_Move()\n");
		}*/
		else
		{	// Standard navigation.. Uses pathfinding...
			NPC_Waypoint_Move( NPC, ucmd );

			//G_Printf("Using NPC_Waypoint_Move()\n");
			return;
		}
	}
}


//===========================================================================
// Routine      : NPC_AI_LocateEnemy

// Description  : Scans for enemy (very simple, just picks any visible enemy)
qboolean
NPC_AI_LocateEnemy_OLD ( gentity_t *NPC )
{
	int			i;
	gentity_t	*bestenemy = NULL;
	gentity_t	*tempent;
	float		bestweight = 2048.0f;
	float		weight, max_weight = 2048.0f;	//300.0;
	vec3_t		dist;
	if ( !BG_IsScopedWeapon( NPC->weaponchoice) )
	{
		max_weight = 1024.0f;
	}

	// Spread out the locating for FPS sake...

	/*	if (NPC->next_enemy_check > level.time)
	{
		//NPC->enemy = NULL;
		return qfalse;
	}

	NPC->next_enemy_check = level.time + 1000 + (Q_TrueRand(0, 2)*1000);
*/

	// Really, if we have the
	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		tempent = &g_entities[i];
		if ( !tempent )
		{
			continue;
		}

		// if slot is not used go next
		//if (!tempent->inuse)
		//	continue;

		/*if ( bestenemy != NULL ) 
		{
			NPC->lastEnemy = qtrue;
			NPC->enemy = bestenemy;
			return qtrue;
		}*/
		if ( tempent->client )
		{										// Players/Bots...
			if ( tempent->client->ps.pm_flags & PMF_LIMBO )
			{
				continue;
			}

			//if (NPC->enemy && tempent != NPC->enemy)
			//	continue;
			if ( OnSameTeam( NPC, tempent) )
			{
				continue;
			}

			if ( tempent->client->ps.stats[STAT_HEALTH] <= 0 )
			{
				continue;
			}

			if ( tempent->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
			{
				continue;						// Don't aim at spectators lol...
			}

			if ( tempent->client->ps.powerups[PW_REDFLAG] || tempent->client->ps.powerups[PW_BLUEFLAG] )
			{
				max_weight = 3000.0;
			}

			VectorSubtract( NPC->r.currentOrigin, tempent->r.currentOrigin, dist );
			weight = VectorLength( dist );

			/*if ( weight < 512 )
			{
				if( infront( NPC, tempent ) && (weight < max_weight ) ) 
				{
					if (weight < bestweight)
					{
						bestweight = weight;
						bestenemy = tempent;
					}
				}
			}
			else*/
			if ( /*infront( NPC, tempent ) &&*/ (weight < max_weight) )
			{
				if ( weight < bestweight )
				{
					if ( MyVisible( NPC, tempent) )
					{
						bestweight = weight;
						bestenemy = tempent;
					}
				}
			}
		}
		else if ( tempent->s.eType == ET_NPC )
		{										// NPCs.
			if ( tempent->health <= 0 )
			{
				continue;
			}

			//if (NPC->enemy && tempent != NPC->enemy)
			//	continue;
			if ( OnSameTeam( NPC, tempent) )
			{
				continue;
			}

			if ( tempent->health <= 0 )
			{
				continue;
			}

			VectorSubtract( NPC->r.currentOrigin, tempent->r.currentOrigin, dist );
			weight = VectorLength( dist );

			/*if ( weight < 512 )
			{
				if( infront( NPC, tempent ) && (weight < max_weight ) ) 
				{
					if (weight < bestweight)
					{
						bestweight = weight;
						bestenemy = tempent;
					}
				}
			}
			else*/
			if ( /*infront( NPC, tempent ) &&*/ (weight < max_weight) )
			{
				if ( weight < bestweight )
				{
					if ( MyVisible( NPC, tempent) )
					{
						bestweight = weight;
						bestenemy = tempent;
					}
				}
			}
		}
	}

	// If best enemy, set up
	//-------------------------------------------------------
	if ( bestenemy != NULL )
	{
		NPC->lastEnemy = qtrue;
		NPC->enemy = bestenemy;
		return ( qtrue );
	}

	return ( qfalse );
}


/* */
qboolean
NPC_AI_LocateEnemy ( gentity_t *bot )
{
	int			i;
	int			last_attack_time;
	gentity_t	*bestenemy = NULL;
	gentity_t	*tempent;
	float		bestweight = 99999;
	float		weight, max_weight = 2000.0;
	gentity_t	*possible_enemies[MAX_GENTITIES];
	gentity_t	*best_enemies[MAX_GENTITIES];
	int			upto = 0;
	int			upto2 = 0;
	if ( bot->NPC_client->attacker && (bot->NPC_client->attacker->client || bot->NPC_client->attacker->NPC_client) )
	{					// We might be able to skip the thinking and just use the person/bot that attacked us as enemy...
		gclient_t	*attacker_client;
		if ( bot->NPC_client->attacker->client )
		{
			attacker_client = bot->NPC_client->attacker->client;
		}
		else
		{
			attacker_client = bot->NPC_client->attacker->NPC_client;
		}

		if
		(
			attacker_client->ps.stats[STAT_HEALTH] > 0 &&
			!OnSameTeam( bot, bot->NPC_client->attacker) &&
			attacker_client->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR
		)
		{				// Attacker validity checks...
			if ( MyVisible( bot, bot->NPC_client->attacker) && bot->NPC_client->attacker->health > 0 )
			{			// Make sure that we only use attacker if he's still visible...
				bot->enemy = bot->NPC_client->attacker;
				bot->NPC_client->attacker = NULL;
				return ( qtrue );
			}
		}
	}

	last_attack_time = ( 3000 );
	for ( i = 0; i < MAX_GENTITIES; i++ )
	{					// Cycle through client slots looking for valid enemies...
		gclient_t	*client;
		tempent = &g_entities[i];

		// if slot is not used go next
		if ( !tempent->inuse )
		{
			continue;
		}

		if ( tempent->client )
		{
			client = tempent->client;
		}
		else
		{
			client = tempent->NPC_client;
		}

		if ( !client )
		{
			continue;
		}

		if ( client->ps.stats[STAT_PLAYER_CLASS] == PC_FIELDOPS || client->ps.stats[STAT_PLAYER_CLASS] == PC_COVERTOPS )
		{
			max_weight = 2000.0;
		}
		else
		{
			max_weight = 1200.0;
		}

		if
		(
			bot->NPC_client->ps.weapon == WP_FG42 ||
			bot->NPC_client->ps.weapon == WP_GARAND ||
			bot->NPC_client->ps.weapon == WP_K43 ||
			bot->NPC_client->ps.weapon == WP_FG42SCOPE ||
			bot->NPC_client->ps.weapon == WP_GARAND_SCOPE ||
			bot->NPC_client->ps.weapon == WP_K43_SCOPE
		)
		{
			max_weight = 9000.0;
		}

		if ( OnSameTeam( bot, tempent) )
		{
			continue;
		}

		if ( client->ps.stats[STAT_HEALTH] <= 0 || tempent->health <= 0 )
		{
			continue;
		}

		if ( client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
		{
			continue;	// Don't aim at spectators lol...
		}

		if ( client->ps.powerups[PW_REDFLAG] || client->ps.powerups[PW_BLUEFLAG] )
		{
			max_weight = 3000.0;
		}

		//#ifdef __VEHICLES__
		if ( (client->ps.eFlags & EF_TANK) )
		{				// Only these should try to shoot at tanks!
			if
			(
				!(bot->NPC_client->ps.eFlags & EF_TANK) &&
				bot->NPC_client->ps.weapon != WP_PANZERFAUST &&
				bot->NPC_client->ps.weapon != WP_KAR98 &&
				bot->NPC_client->ps.weapon != WP_CARBINE &&
				bot->NPC_client->ps.weapon != WP_GRENADE_LAUNCHER &&
				bot->NPC_client->ps.weapon != WP_GRENADE_PINEAPPLE
			)
			{
				continue;
			}
		}

		//#endif //__VEHICLES__
		weight = VectorDistance( bot->r.currentOrigin, tempent->r.currentOrigin );
		if ( weight < max_weight )
		{
			possible_enemies[upto] = tempent;
			upto++;
		}
	}

	bestweight = 99999;
	if ( upto == 1 )
	{					// Only have 1 possibile...
		best_enemies[0] = possible_enemies[0];
		upto2 = upto;
	}
	else
	{					// Sort the possible list...
		for ( i = 0; i < upto; i++ )
		{
			int			j;
			float		best_weight = 99999;
			gentity_t	*best_client = NULL;
			tempent = possible_enemies[i];
			weight = VectorDistance( bot->r.currentOrigin, tempent->r.currentOrigin );
			for ( j = 0; j < upto; j++ )
			{
				qboolean	already_in_list = qfalse;
				int			z;
				gentity_t	*tempent2 = possible_enemies[j];
				float		weight2 = VectorDistance( bot->r.currentOrigin, tempent2->r.currentOrigin );
				for ( z = 0; z < upto2; z++ )
				{
					if ( best_enemies[z] == tempent2 )
					{
						already_in_list = qtrue;
						break;
					}
				}

				if ( !already_in_list && weight2 < best_weight )
				{		// Best so far...
					best_client = tempent2;
					best_weight = weight2;
				}
			}

			best_enemies[upto2] = best_client;
			upto2++;
		}
	}

	bestweight = 99999;
	for ( i = 0; i < upto2; i++ )
	{					// Return the closest one that is visible, checking visibilities only as far as needed for CPU time saving...
		tempent = possible_enemies[i];
		weight = VectorDistance( bot->r.currentOrigin, tempent->r.currentOrigin );
		if ( MyVisible( bot, tempent) )
		{
			bot->lastEnemy = qtrue;
			bot->enemy = tempent;
			return ( qtrue );
		}
	}

	bot->enemy = NULL;
	return ( qfalse );
}


/*
int nextchat[MAX_CLIENTS];
// Check for any generic ingame sounds we might need...
void NPC_Check_VoiceChats (gentity_t *NPC)
{
	int clientNum = NPC->s.clientNum;
	int testclient = 0;

	if (!nextchat[NPC->s.clientNum])// Initialize!
		nextchat[NPC->s.clientNum] = level.time + Q_TrueRand(1000, 10000);

	if (nextchat[NPC->s.clientNum] <= 0)// Initialize!
		nextchat[NPC->s.clientNum] = level.time + Q_TrueRand(1000, 10000);

	while (	testclient < MAX_CLIENTS )
	{
		if (testclient == NPC->s.clientNum)
		{
			testclient++;
			continue;
		}

		if (VectorDistance(g_entities[testclient].r.currentOrigin, NPC->r.currentOrigin) <= 48)
		{// Too close.. Back Off Man!!!
			if (nextchat[NPC->s.clientNum] < level.time)
			{
				nextchat[NPC->s.clientNum] = level.time + Q_TrueRand(8000, 30000);

				if (rand()%10 < 6)//level.time < 5000)
					G_Voice( NPC, NULL, SAY_TEAM, "LetsGo", qfalse );
				else
					G_Voice( NPC, NULL, SAY_TEAM, "Move", qfalse );
			
				break;
			}
		}

		if (g_entities[testclient].is_NPC)
		{
			if (g_entities[testclient].current_node == NPC->current_node)
			{// Too close.. Back Off ManNPC!!!
				if (nextchat[NPC->s.clientNum] < level.time)
				{
					nextchat[NPC->s.clientNum] = level.time + Q_TrueRand(8000, 30000);
					//G_Voice( NPC, NULL, SAY_TEAM, "Move", qfalse );
					if (rand()%10 < 6)//level.time < 5000)
						G_Voice( NPC, NULL, SAY_TEAM, "LetsGo", qfalse );
					else
						G_Voice( NPC, NULL, SAY_TEAM, "Move", qfalse );
					break;
				}
			}
		}

		testclient++;
	}
}
*/
qboolean
NPC_Coop_Player_Within_Range ( gentity_t *NPC )
{	// In coop, let's save CPU time by only enabling NPCs that are withing range of a player...
	int i;
	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		gentity_t	*ent = &g_entities[i];
		if ( !ent || !ent->client )
		{
			continue;
		}

		if ( VectorDistance( NPC->r.currentOrigin, ent->r.currentOrigin) > 2048 )
		{

			/*if (VectorDistance(NPC->r.currentOrigin, ent->r.currentOrigin) < 4096)
			{
				if (!MyVisible(ent, NPC))
					continue;
			}
			else*/
			continue;
		}

		return ( qtrue );
	}

	return ( qfalse );
}

qboolean	NPC_Coop_Check_Wait ( gentity_t *NPC );
extern void NPC_AI_Think ( gentity_t *NPC );


/* */
qboolean
NPC_Coop_Check_InUse ( gentity_t *NPC )
{
	qboolean	activate = NPC_Coop_Player_Within_Range( NPC );
	if ( !activate )
	{

		//if (NPC->active)
		//	G_Printf("NPC %i deactivated.\n", NPC->s.number);
		NPC->think = NPC_Coop_Check_Wait;
		NPC->active = qfalse;
		return ( qfalse );
	}
	else
	{

		//if (!NPC->active)
		//	G_Printf("NPC %i activated.\n", NPC->s.number);
		NPC->think = NPC_AI_Think;
		NPC->active = qtrue;
		return ( qtrue );
	}
}


/* */
qboolean
NPC_Coop_Check_Wait ( gentity_t *NPC )
{
	if ( level.intermissiontime )
	{	// Don't think in level intermission time.
		return ( qfalse );
	}

	if ( NPC->nextthink )
	{
		if ( NPC->nextthink > level.time )
		{
			return ( NPC->active );
		}
	}

	if ( !NPC_Coop_Check_InUse( NPC) )
	{
		NPC->nextthink = level.time + 1000;
		return ( qtrue );
	}
	else
	{
		return ( qfalse );
	}
}


//===========================================================================
// Routine      : NPC_AI_Humanoid_Think

// Description  : Main think function for humanoid NPCs.
void
NPC_AI_Humanoid_Think ( gentity_t *NPC )
{
	if ( level.intermissiontime )
	{													// Don't think in level intermission time.
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

	if ( NPC->NPC_Class == CLASS_HUMANOID && g_gametype.integer < GT_WOLF )
	{													// Single player or coop...
		if ( NPC_Coop_Check_Wait( NPC) )
		{
			return;
		}
	}

	/*	if( NPC->NPC_Class == CLASS_HUMANOID 
		&& (NPC->s.dl_intensity == TAG_HOOK || NPC->s.dl_intensity == TAG_HOOK_DRIVER || NPC->s.dl_intensity == TAG_HOOK_GUNNER1 || NPC->s.dl_intensity == TAG_HOOK_GUNNER2) )
	{
		if ( !Q_stricmp(NPC->classname,  "freed") )
		{
			NPC->s.dl_intensity = TAG_UNHOOK;
			
			if (NPC->NPC_client)
			{
				NPC->NPC_client->ps.eFlags &= ~EF_TAGCONNECT;
				NPC->NPC_client->ps.stats[STAT_TAG_CONNECT_INFO] = TAG_UNHOOK;
			}

			G_FreeEntity(NPC);
			NPC->s.eType = 0;
			return;
		}

		if ( !Q_stricmp(g_entities[NPC->s.otherEntityNum].classname,  "freed") )
		{
			NPC->s.dl_intensity = TAG_UNHOOK;
			
			if (NPC->NPC_client)
			{
				NPC->NPC_client->ps.eFlags &= ~EF_TAGCONNECT;
				NPC->NPC_client->ps.stats[STAT_TAG_CONNECT_INFO] = TAG_UNHOOK;
			}

			G_FreeEntity(NPC);
			NPC->s.eType = 0;
			return;
		}

		G_SetAngle(NPC, g_entities[NPC->s.otherEntityNum].r.currentAngles);
		VectorCopy(g_entities[NPC->s.otherEntityNum].r.currentAngles, NPC->r.currentAngles);
		VectorCopy(g_entities[NPC->s.otherEntityNum].r.currentAngles, NPC->s.angles);
		VectorCopy(g_entities[NPC->s.otherEntityNum].r.currentAngles, NPC->NPC_client->ps.viewangles);
		return; // Don't need to think if we're attached!
	}*/

	//	if (NPC->health)
	//	{
	if ( NPC->health <= 0 )
	{													// Free the NPC's entity... It's dead!
		if ( NPC->NPC_Class == CLASS_HUMANOID )
		{
			NPC->die;
		}
		else
		{
			G_FreeEntity( NPC );
		}
	}

	//	}
	if
	(
		NPC->NPC_Class == CLASS_HUMANOID &&
		!(
			(NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER) && !
				(
					(NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER_ATTACKER) ||
					(NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER_DEFENDER)
				)
		)
	)
	{													// Humanoid NPCs can attack...
		gentity_t	*oldenemy = NPC->enemy;
		if ( NPC->enemy )
		{
			if ( NPC->bot_enemy_visible_time >= level.time && !OnSameTeam( NPC, NPC->enemy) )
			{
				NPC->action = level.time;
			}
			else
			{
				if ( MyVisible( NPC, NPC->enemy) )
				{
					NPC->action = level.time;
					NPC->bot_enemy_visible_time = level.time + 1000;
				}
			}
		}
		else if ( !NPC->enemy || NPC->enemy->health <= 0 )
		{
			NPC->enemy = NULL;
			if ( NPC->bot_next_enemy_scan_time < level.time )
			{
				if ( NPC_AI_LocateEnemy( NPC) )
				{
					NPC->action = level.time;
					NPC->bot_enemy_visible_time = level.time + 1000;
				}
			}

			NPC->bot_next_enemy_scan_time = level.time + 1000;
		}

#ifdef __WEAPON_AIM__
		if ( NPC->enemy && BG_Weapon_Is_Aimable( NPC->s.weapon) )
		{
			if
			(
				VectorDistance( NPC->r.currentOrigin, NPC->enemy->r.currentOrigin) > 1024 &&
				visible( NPC, NPC->enemy)
			)
			{
				NPC->NPC_client->ps.eFlags |= EF_PRONE;
			}
			else if ( VectorDistance( NPC->r.currentOrigin, NPC->enemy->r.currentOrigin) > 512 )
			{
				NPC->NPC_client->ps.eFlags |= EF_AIMING;
			}
		}

		if
		(
			NPC->NPC_client &&
			(!NPC->enemy || VectorDistance( NPC->r.currentOrigin, NPC->enemy->r.currentOrigin) < 512)
		)
		{
			NPC->NPC_client->ps.eFlags &= ~EF_AIMING;
			NPC->NPC_client->ps.eFlags &= ~EF_PRONE;
		}
#endif //__WEAPON_AIM__
		if ( NPC->enemy )
		{												// Look at them and attack!
			if ( NPC->action > level.time - 300 )
			{											// Enemy is visible.. Fire!
				usercmd_t	*ucmd = &NPC->NPC_client->pers.cmd;
				ucmd->forwardmove = 0;
				ucmd->rightmove = 0;

				//ucmd->upmove = 0;
				if ( !NPC->npc_mg42 )
				{

					// Duck for attacking...
					//ucmd->upmove = -48;
					NPC->NPC_client->ps.pm_flags |= PMF_DUCKED;
				}

				//LookAtEnemy(NPC);
				VectorSubtract( NPC->enemy->r.currentOrigin, NPC->r.currentOrigin, NPC->move_vector );
				vectoangles( NPC->move_vector, NPC->bot_ideal_view_angles );

				//BotSetViewAngles(NPC, 100);
				NPCSetViewAngles( NPC, 100 );
				if
				(
					g_gametype.integer < GT_WOLF &&
					oldenemy != NPC->enemy &&
					NPC->npc_next_talk <= level.time &&
					NPC->next_fire_time < level.time	/* - 300*/
				)
				{										// Yell "HALT!", etc First and animate the bot..
					int animchoice = rand() % 2;

					//gentity_t *te = NULL;
					if
					(
						NPC->enemy->health <= 0 ||
						(NPC->enemy->client->ps.pm_flags & PMF_LIMBO) ||
						OnSameTeam( NPC, NPC->enemy)	/*|| NPC->enemy->s.teamNum == TEAM_SPECTATOR*/
					)
					{									// They are dead... Don't move... Initialize goal so they head back to their spawn position...
						ucmd->forwardmove = 0;
						ucmd->rightmove = 0;
						ucmd->upmove = 0;
						NPC->enemy = NULL;
						NPC->longTermGoal = -1;
						NPC_AI_Move( NPC );
						NPCThink( NPC->s.number );
						return;							// next think...
					}

					// sound
					G_AddEvent( NPC, EV_GENERAL_SOUND, G_SoundIndex( va( "sound/coop/halt%i.wav", rand() % 18)) );
					NPC->npc_next_talk = level.time + ( 1000 * Q_TrueRand( 10, 30) );	// 10->30 secs between talk for each bot.

					// FIXME: Better animation choices...
					if ( animchoice == 1 )
					{
						NPC->NPC_client->ps.pm_time = BG_AnimScriptEvent( &NPC->NPC_client->ps,
																		  NPC->NPC_client->pers.character->animModelInfo,
																		  30, qfalse, qtrue );
					}
					else if ( animchoice == 2 )
					{
						NPC->NPC_client->ps.pm_time = BG_AnimScriptEvent( &NPC->NPC_client->ps,
																		  NPC->NPC_client->pers.character->animModelInfo,
																		  32, qfalse, qtrue );
					}
					else
					{
						NPC->NPC_client->ps.pm_time = BG_AnimScriptEvent( &NPC->NPC_client->ps,
																		  NPC->NPC_client->pers.character->animModelInfo,
																		  35, qfalse, qtrue );
					}

					NPC->action = level.time;
				}
				else
				{																// Fire when ready!
					if ( NPC->next_fire_time < level.time )
					{
						if
						(
							NPC->enemy->health <= 0 ||
							(NPC->enemy->client && NPC->enemy->client->ps.pm_flags & PMF_LIMBO) ||
							OnSameTeam( NPC, NPC->enemy) ||
							NPC->enemy->s.teamNum == TEAM_SPECTATOR
						)
						{														// They are dead... Don't move... Initialize goal so they head back to their spawn position...
							ucmd->forwardmove = 0;
							ucmd->rightmove = 0;
							ucmd->upmove = 0;
							NPC->enemy = NULL;
							NPC->longTermGoal = -1;
							NPC_AI_Move( NPC );
							NPCThink( NPC->s.number );
							return;												// next think...
						}

						if ( VectorDistance( NPC->bot_ideal_view_angles, NPC->NPC_client->ps.viewangles) < 2 )
						{														// Wait until we are looking at them!
							FireWeapon( NPC );
							G_AddEvent( NPC, EV_FIRE_WEAPON, 0 );
						}

						NPC->next_fire_time = level.time + GetAmmoTableData( NPC->s.weapon )->nextShotTime;
						NPC->action = level.time;
					}
				}

				NPCThink( NPC->s.number );
				return;
			}
			else																// not visible.. go back to moving...
			{
#ifdef __WEAPON_AIM__
				if ( BG_Weapon_Is_Aimable( NPC->s.weapon) )
				{
					if ( NPC->NPC_client->ps.eFlags & EF_AIMING )
					{
						NPC->NPC_client->ps.eFlags &= ~EF_AIMING;

						//NPC->s.eFlags |= EF_AIMING;
					}
				}
#endif //__WEAPON_AIM__
				if ( NPC->enemy && NPC->enemy->s.eType == ET_NPC )
				{
					if ( NPC->enemy->health <= 0								 /*|| OnSameTeam(NPC, NPC->enemy) || NPC->enemy->s.teamNum == TEAM_SPECTATOR*/ )
					{															// They are dead... Don't move... Initialize goal so they head back to their spawn position...
						NPC->enemy = NULL;
						NPC->longTermGoal = -1;
					}
				}
				else
				{
					if
					(
						NPC->enemy &&
						(
							NPC->enemy->health <= 0 ||
							(NPC->enemy->client->ps.pm_flags & PMF_LIMBO) ||	/*OnSameTeam(NPC, NPC->enemy) ||*/
							NPC->enemy->client->sess.sessionTeam == TEAM_SPECTATOR
						)
					)
					{															// They are dead... Don't move... Initialize goal so they head back to their spawn position...
						NPC->enemy = NULL;
						NPC->longTermGoal = -1;
					}
				}

				if
				(
					g_gametype.integer != GT_COOP &&
					g_gametype.integer != GT_SINGLE_PLAYER &&
					NPC->action < level.time - 10000
				)
				{
					NPC->enemy = NULL;											// Reset our enemy if we havn't found him in a while...
				}

				NPC_AI_Move( NPC );
			}
		}
		else
		{
			NPC_AI_Move( NPC );
		}

		NPCThink( NPC->s.number );
	}
	else if ( NPC->NPC_Class == CLASS_HUMANOID && (NPC->NPC_Humanoid_Type & HUMANOID_TYPE_FODDER) )
	{	// A humanoid fodder NPC...
		NPC_AI_Move( NPC );
		NPCThink( NPC->s.number );
	}
	else
	{	// Not a humanoid...
		NPC_AI_Move( NPC );
	}

	if ( g_gametype.integer < GT_WOLF && NPC->npc_next_idle_talk < level.time && NPC->s.teamNum == TEAM_AXIS )
	{	// Idle NPC talking...
		// sound
		//		G_AddEvent( NPC, EV_GENERAL_SOUND, G_SoundIndex( va("sound/idle/idle%i.wav", rand()%28)) );
		//		NPC->npc_next_idle_talk = level.time + 30000 + (Q_TrueRand(0, 30) * 1000); // 30->60 secs at least between talk for each bot.
	}

	// think again baby
	NPC->nextthink = level.time + bot_thinktime.integer /*FRAMETIME*/ ;
}
#endif //__NPC__
