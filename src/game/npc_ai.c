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


/*
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
Unique1's Experimental NPC Code.
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
NPCVisible - Visibility tracing...
NPC_AI_inFOV - FOV checking...
NPC_Setup - Generic NPC setup code...
NPC_AI_Move - Calls to class based move code...
NPC_AI_Think - Calls to class based think code...
¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
*/
#include "g_local.h"
#ifdef __NPC__
extern vmCvar_t bot_thinktime;


/*
===========================================================================
NPCVisible
This is just a copy of the built-in 'visible' function
but accounting for the players' viewheight.  This lets the
bots see players slightly more often and in a more realistic way
===========================================================================
*/
qboolean
NPCVisible ( gentity_t *self, gentity_t *other )
{
	vec3_t		selfView, otherView;					//basically stores the locations of the players' eyes
	trace_t		tr;										//used in performing the collision info
	gentity_t	*traceEnt;								//holds the entity returned by the trace function
	VectorCopy( self->r.currentOrigin, selfView );		//copy the bot's origin to this variable
	selfView[2] += 48;									//add the bot's viewheight	
	VectorCopy( other->r.currentOrigin, otherView );	//copy the target's origin
	otherView[2] += 48;									//add the target's viewheight

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];				//set traceEnt to the entity the shot would hit
	if ( traceEnt == other )
	{													//if it would hit the player, return true
		return ( qtrue );
	}

	return ( qfalse );									//it would hit something other than the player (or nothing) so return false
}


/*
==============
NPC_AI_inFOV
==============
*/
qboolean
NPC_AI_inFOV ( vec3_t viewangles, float fov, vec3_t angles )
{
	int		i;
	float	diff, angle;
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

		if ( diff > 0 )
		{
			if ( diff > fov * 0.5 )
			{
				return ( qfalse );
			}
		}
		else
		{
			if ( diff < -fov * 0.5 )
			{
				return ( qfalse );
			}
		}
	}

	return ( qtrue );
}

// The basic NPC classtypes. Moved to q_shared.h for cgame access...

/*
enum {
	CLASS_HUMANOID,		// A standard player-style npc...
	CLASS_BOAT,			// Water vehicles...
	CLASS_LCVP,	// Water vehicles... This one is for spawning on...
	CLASS_DESTROEYR,	// Water vehicles... This one is for spawning on...
	CLASS_PLANE,		// Planes (flying)...
	CLASS_PLANE_BOMBER,	// Planes (flying)... This one is for bombers...
	CLASS_PLANE_LANDER,	// Planes (flying)... This one is for spawning from (paratroop drop planes)...
	CLASS_VEHICLE,		// Vehicle (Driveable or not)...
	CLASS_ARTILLERY,	// Non-Moveable artillery/etc platforms...
	CLASS_MAX			// Simply marks the end of the class types... (Pointless???)
};
*/
extern qboolean G_TryPushingEntity ( gentity_t *check, gentity_t *pusher, vec3_t move, vec3_t amove );


/*
============
G_NPCPush

This version is for planes.. It allows us to pass through map boundaries and fly away...
============
*/
qboolean
G_NPCPush ( gentity_t *pusher, vec3_t move, vec3_t amove, gentity_t **obstacle )
{
	int		i;
	vec3_t	mins, maxs;
	vec3_t	totalMins, totalMaxs;
	*obstacle = NULL;

	// mins/maxs are the bounds at the destination
	// totalMins / totalMaxs are the bounds for the entire move
	if
	(
		pusher->r.currentAngles[0] ||
		pusher->r.currentAngles[1] ||
		pusher->r.currentAngles[2] ||
		amove[0] ||
		amove[1] ||
		amove[2]
	)
	{
		float	radius;
		radius = RadiusFromBounds( pusher->r.mins, pusher->r.maxs );
		for ( i = 0; i < 3; i++ )
		{
			mins[i] = pusher->r.currentOrigin[i] - radius + move[i];
			maxs[i] = pusher->r.currentOrigin[i] + radius + move[i];
			totalMins[i] = pusher->r.currentOrigin[i] - radius;
			totalMaxs[i] = pusher->r.currentOrigin[i] + radius;
		}
	}
	else
	{
		for ( i = 0; i < 3; i++ )
		{
			mins[i] = pusher->r.absmin[i] + move[i];
			maxs[i] = pusher->r.absmax[i] + move[i];
		}

		VectorCopy( pusher->r.absmin, totalMins );
		VectorCopy( pusher->r.absmax, totalMaxs );
	}

	for ( i = 0; i < 3; i++ )
	{
		if ( move[i] > 0 )
		{
			totalMaxs[i] += move[i];
		}
		else
		{
			totalMins[i] += move[i];
		}
	}

	// move the pusher to it's final position
	VectorAdd( pusher->r.currentOrigin, move, pusher->r.currentOrigin );
	VectorAdd( pusher->r.currentAngles, amove, pusher->r.currentAngles );

	// movement was successfull
	return ( qtrue );
}

void	NPC_AI_Think ( gentity_t *NPC );	// below...


//===========================================================================
// Routine      : NPC_Setup

// Description  : Set up an NPC.
void
NPC_Setup ( gentity_t *NPC )
{
	NPC->s.eType = ET_NPC;
	G_SetOrigin( NPC, NPC->s.origin );
	VectorCopy( NPC->s.angles, NPC->s.apos.trBase );
	NPC->health = 100;
	NPC->think = NPC_AI_Think;
	NPC->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;

	//NPC->r.contents = CONTENTS_BODY;
	trap_LinkEntity( NPC );
	NPC->enemy = NULL;
}

//extern void NPC_AI_Vehicle_Move( gentity_t *NPC );
extern void NPC_AI_Air_Move ( gentity_t *NPC );
extern void NPC_AI_Boat_Move ( gentity_t *NPC );
extern void NPC_AI_Humanoid_Move ( gentity_t *NPC );


//===========================================================================
// Routine      : NPC_AI_Move

// Description  : Moves the NPC.
void
NPC_AI_Move ( gentity_t *NPC )
{
	if ( NPC->NPC_Class == CLASS_VEHICLE )
	{		// An NPC Tank... Testing...
		//		NPC_AI_Vehicle_Move( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_PLANE )
	{		// Planes type NPC routing here... Just head forward for now...
		NPC_AI_Air_Move( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_PLANE_BOMBER )
	{		// Bomber planes type NPC routing here... Just head forward for now... FIXME: Should link this into air strike code...
		NPC_AI_Air_Move( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_PLANE_LANDER )
	{		// Paratrooper planes type NPC routing here... Just head forward for now...
		NPC_AI_Air_Move( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_BOAT )
	{
		NPC_AI_Boat_Move( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_LCVP )
	{		// Planes type NPC routing here... Just head forward for now...
		NPC_AI_Boat_Move( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_DESTROYER )
	{		// Planes type NPC routing here... Just head forward for now...
		NPC_AI_Boat_Move( NPC );
	}
	else	//if ( NPC->NPC_Class == CLASS_HUMANOID)
	{		// Humanoid type NPC routing here... (Now default)
		NPC_AI_Humanoid_Move( NPC );
	}
}

//extern void NPC_AI_Vehicle_Think( gentity_t *NPC );
extern void NPC_AI_Air_Think ( gentity_t *NPC );
extern void NPC_AI_Boat_Think ( gentity_t *NPC );
extern void NPC_AI_Humanoid_Think ( gentity_t *NPC );


//===========================================================================
// Routine      : NPC_AI_Think

// Description  : Main think function for NPCs.
void
NPC_AI_Think ( gentity_t *NPC )
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

	if ( NPC->NPC_Class == CLASS_VEHICLE )
	{		// An NPC Tank... Testing...
		//		NPC_AI_Vehicle_Think( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_PLANE )
	{		// Planes type NPC routing here... Just head forward for now...
		NPC_AI_Air_Think( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_PLANE_BOMBER )
	{		// Bomber planes type NPC routing here... Just head forward for now... FIXME: Should link this into air strike code...
		NPC_AI_Air_Think( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_PLANE_LANDER )
	{		// Paratrooper planes type NPC routing here... Just head forward for now...
		NPC_AI_Air_Think( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_BOAT )
	{
		NPC_AI_Boat_Think( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_LCVP )
	{		// Planes type NPC routing here... Just head forward for now...
		NPC_AI_Boat_Think( NPC );
	}
	else if ( NPC->NPC_Class == CLASS_DESTROYER )
	{		// Planes type NPC routing here... Just head forward for now...
		NPC_AI_Boat_Think( NPC );
	}
	else	//if ( NPC->NPC_Class == CLASS_HUMANOID)
	{		// Humanoid type NPC routing here... (Now default)
		NPC_AI_Humanoid_Think( NPC );
	}
}
#endif //__NPC__
