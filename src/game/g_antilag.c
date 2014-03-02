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

/*
=============
TimeShiftLerp (from g_unlagged.c)

Used below to interpolate between two previous vectors
Returns a vector "frac" times the distance between "start" and "end"
=============
*/
static void
TimeShiftLerp ( vec3_t start, vec3_t end, float frac, vec3_t result )
{

	// From CG_InterpolateEntityPosition in cg_ents.c:

	/*
	cent->lerpOrigin[0] = current[0] + f * ( next[0] - current[0] );
	cent->lerpOrigin[1] = current[1] + f * ( next[1] - current[1] );
	cent->lerpOrigin[2] = current[2] + f * ( next[2] - current[2] );

  Angles would be done in a different function, but here they are only the s.angles
  lerpAngles is copied over from ent->s.angles
	cent->lerpAngles[0] = LerpAngle( current[0], next[0], f );
	cent->lerpAngles[1] = LerpAngle( current[1], next[1], f );
	cent->lerpAngles[2] = LerpAngle( current[2], next[2], f );

*/

	// Making these exactly the same should avoid floating-point error
	result[0] = start[0] + frac * ( end[0] - start[0] );
	result[1] = start[1] + frac * ( end[1] - start[1] );
	result[2] = start[2] + frac * ( end[2] - start[2] );
}


/* */
qboolean
G_AntilagSafe ( gentity_t *ent )
{
	gclient_t	*client;

	// Kamikazee: If g_antilag is off, disable the whole anti-lag subsystem.
	//	It gives problems when anti-lag is off as certain states will not be saved.
	//	[TESTME] Might cause jerky gameplay when g_antilag is off
	if( ! g_antilag.integer )
		return qfalse;

	if ( !ent )
	{
		return ( qfalse );
	}

#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}

#else //!__NPC__
	client = ent->client;
#endif //__NPC__
	if ( !ent->inuse )
	{
		return ( qfalse );
	}

	if ( !ent->r.linked )
	{
		return ( qfalse );
	}

	if ( !ent->client )
	{
		return ( qfalse );
	}

	if ( client->sess.sessionTeam != TEAM_AXIS && client->sess.sessionTeam != TEAM_ALLIES )
	{
		return ( qfalse );
	}

	if ( ent->health <= 0 )
	{
		return ( qfalse );
	}

	if ( (client->ps.pm_flags & PMF_LIMBO) )
	{
		return ( qfalse );
	}

	// tjw: don't store clientMarkers for corpses, etc
	if ( !(client->ps.pm_type == PM_NORMAL) )
	{
		return ( qfalse );
	}

	// tjw: don't store clientMarkers for the player on
	//      the tank
	if ( (client->ps.eFlags & EF_MOUNTEDTANK) )
	{
		return ( qfalse );
	}

	return ( qtrue );
}


/* */
void
G_StoreClientPosition ( gentity_t *ent )
{
	int			top;
	gclient_t	*client;
#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}

#else //!__NPC__
	client = ent->client;
#endif //__NPC__
	if ( !G_AntilagSafe( ent) )
	{
		return;
	}

	client->topMarker++;
	if ( client->topMarker >= MAX_CLIENT_MARKERS )
	{
		client->topMarker = 0;
	}

	top = client->topMarker;
	VectorCopy( ent->r.mins, client->clientMarkers[top].mins );
	VectorCopy( ent->r.maxs, client->clientMarkers[top].maxs );

	// change to trbase instead of origin, correct for client (use s!!)
	VectorCopy( ent->s.pos.trBase, client->clientMarkers[top].origin );
	client->clientMarkers[top].time = level.time;

	//client->clientMarkers[top].servertime = level.time;
	//josh: more info needed for head / leg shots
	// tjw: It seems that s.apos.trBase and ps.viewangles are in sync and
	//      the only usable angles for determining the direction the client
	//      is pointing.  I don't know wtf r.currentAngles,
	//      s.angles, and s.angles2 are for.
	// josh: Yeah, Neil says they were just copying the angle into every
	//       angle they could think of "just in case". He was very unimpressed.
	VectorCopy( ent->s.apos.trBase, client->clientMarkers[top].viewangles );
	client->clientMarkers[top].eFlags = ent->s.eFlags;
	client->clientMarkers[top].pm_flags = client->ps.pm_flags;
	client->clientMarkers[top].viewheight = client->ps.viewheight;

	// forty - realistic hitboxes - Torso Markers
	client->clientMarkers[top].torsoOldFrameModel = ent->torsoFrame.oldFrameModel;
	client->clientMarkers[top].torsoFrameModel = ent->torsoFrame.frameModel;
	client->clientMarkers[top].torsoOldFrame = ent->torsoFrame.oldFrame;
	client->clientMarkers[top].torsoFrame = ent->torsoFrame.frame;
	client->clientMarkers[top].torsoOldFrameTime = ent->torsoFrame.oldFrameTime;
	client->clientMarkers[top].torsoFrameTime = ent->torsoFrame.frameTime;
	client->clientMarkers[top].torsoYawAngle = ent->torsoFrame.yawAngle;
	client->clientMarkers[top].torsoPitchAngle = ent->torsoFrame.pitchAngle;
	client->clientMarkers[top].torsoYawing = ent->torsoFrame.yawing;
	client->clientMarkers[top].torsoPitching = ent->torsoFrame.pitching;

	// forty - realistic hitboxes - Legs Markers
	client->clientMarkers[top].legsOldFrameModel = ent->legsFrame.oldFrameModel;
	client->clientMarkers[top].legsFrameModel = ent->legsFrame.frameModel;
	client->clientMarkers[top].legsOldFrame = ent->legsFrame.oldFrame;
	client->clientMarkers[top].legsFrame = ent->legsFrame.frame;
	client->clientMarkers[top].legsOldFrameTime = ent->legsFrame.oldFrameTime;
	client->clientMarkers[top].legsFrameTime = ent->legsFrame.frameTime;
	client->clientMarkers[top].legsYawAngle = ent->legsFrame.yawAngle;
	client->clientMarkers[top].legsPitchAngle = ent->legsFrame.pitchAngle;
	client->clientMarkers[top].legsYawing = ent->legsFrame.yawing;
	client->clientMarkers[top].legsPitching = ent->legsFrame.pitching;
}


/* */
void
G_AdjustSingleClientPosition ( gentity_t *ent, int time, gentity_t *debugger )
{
	int			i, j;
	gclient_t	*client;
#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}

#else //!__NPC__
	client = ent->client;
#endif //__NPC__

	// unlagged analogies: antilag i = unlagged j, antilag j = unlagged k
	if ( time > level.time )
	{
		time = level.time;
	}	// no lerping forward....

	if ( !G_AntilagSafe( ent) )
	{
		return;
	}

	// find a pair of markers which bound the requested time
	i = j = client->topMarker;
	do
	{
		if ( client->clientMarkers[i].time <= time )
		{
			break;
		}

		j = i;
		i--;
		if ( i < 0 )
		{
			i = MAX_CLIENT_MARKERS - 1;
		}
	}
	while ( i != client->topMarker );
	if ( i == j )
	{	// oops, no valid stored markers
		return;
	}

	// josh: from unlagged make sure it doesn't get backed-up twice
	if ( client->backupMarker.time != level.time )
	{
		VectorCopy( ent->r.currentOrigin, client->backupMarker.origin );
		VectorCopy( ent->r.mins, client->backupMarker.mins );
		VectorCopy( ent->r.maxs, client->backupMarker.maxs );

		//josh: for Head, Legs
		VectorCopy( client->ps.viewangles, client->backupMarker.viewangles );
		client->backupMarker.eFlags = client->ps.eFlags;
		client->backupMarker.pm_flags = client->ps.pm_flags;
		client->backupMarker.viewheight = client->ps.viewheight;

		// josh: This was missing, but needed or else no readjustment
		client->backupMarker.time = level.time;

		// forty - realistic hitboxes - Torso Markers
		client->backupMarker.torsoOldFrameModel = ent->torsoFrame.oldFrameModel;
		client->backupMarker.torsoFrameModel = ent->torsoFrame.frameModel;
		client->backupMarker.torsoOldFrame = ent->torsoFrame.oldFrame;
		client->backupMarker.torsoFrame = ent->torsoFrame.frame;
		client->backupMarker.torsoOldFrameTime = ent->torsoFrame.oldFrameTime;
		client->backupMarker.torsoFrameTime = ent->torsoFrame.frameTime;
		client->backupMarker.torsoYawAngle = ent->torsoFrame.yawAngle;
		client->backupMarker.torsoPitchAngle = ent->torsoFrame.pitchAngle;
		client->backupMarker.torsoYawing = ent->torsoFrame.yawing;
		client->backupMarker.torsoPitching = ent->torsoFrame.pitching;

		// forty - realistic hitboxes - Legs Markers
		client->backupMarker.legsOldFrameModel = ent->legsFrame.oldFrameModel;
		client->backupMarker.legsFrameModel = ent->legsFrame.frameModel;
		client->backupMarker.legsOldFrame = ent->legsFrame.oldFrame;
		client->backupMarker.legsFrame = ent->legsFrame.frame;
		client->backupMarker.legsOldFrameTime = ent->legsFrame.oldFrameTime;
		client->backupMarker.legsFrameTime = ent->legsFrame.frameTime;
		client->backupMarker.legsYawAngle = ent->legsFrame.yawAngle;
		client->backupMarker.legsPitchAngle = ent->legsFrame.pitchAngle;
		client->backupMarker.legsYawing = ent->legsFrame.yawing;
		client->backupMarker.legsPitching = ent->legsFrame.pitching;
	}

	if ( i != client->topMarker )
	{

		//float frac = ((float)(client->clientMarkers[j].time - time)) / (client->clientMarkers[j].time - client->clientMarkers[i].time);
		//josh: reversing the order to better match the client and prevent
		//roundoff error
		float	frac = (float) ( time - client->clientMarkers[i].time ) / (float)
			( client->clientMarkers[j].time - client->clientMarkers[i].time );

		//LerpPosition(client->clientMarkers[i].origin,		client->clientMarkers[j].origin,	frac,	ent->r.currentOrigin);
		//LerpPosition(client->clientMarkers[i].mins,		client->clientMarkers[j].mins,		frac,	ent->r.mins);
		//LerpPosition(client->clientMarkers[i].maxs,		client->clientMarkers[j].maxs,		frac,	ent->r.maxs);
		//josh: Using TimeShiftLerp since it follows the client exactly
		//meaning less roundoff error
		TimeShiftLerp( client->clientMarkers[i].origin, client->clientMarkers[j].origin, frac, ent->r.currentOrigin );
		TimeShiftLerp( client->clientMarkers[i].mins, client->clientMarkers[j].mins, frac, ent->r.mins );
		TimeShiftLerp( client->clientMarkers[i].maxs, client->clientMarkers[j].maxs, frac, ent->r.maxs );

		// These are for Head / Legs
		client->ps.viewangles[0] = LerpAngle( client->clientMarkers[i].viewangles[0],
											  client->clientMarkers[j].viewangles[0], frac );
		client->ps.viewangles[1] = LerpAngle( client->clientMarkers[i].viewangles[1],
											  client->clientMarkers[j].viewangles[1], frac );
		client->ps.viewangles[2] = LerpAngle( client->clientMarkers[i].viewangles[2],
											  client->clientMarkers[j].viewangles[2], frac );

		// josh: Set the ints to the closest ones in time since you can't
		//       lerp them.
		if ( (client->clientMarkers[j].time - time) < (time - client->clientMarkers[i].time) )
		{
			client->ps.eFlags = client->clientMarkers[j].eFlags;
			client->ps.pm_flags = client->clientMarkers[j].pm_flags;
			client->ps.viewheight = client->clientMarkers[j].viewheight;

			// forty - realistic hitboxes - Torso Markers
			ent->torsoFrame.oldFrameModel = client->clientMarkers[j].torsoOldFrameModel;
			ent->torsoFrame.frameModel = client->clientMarkers[j].torsoFrameModel;
			ent->torsoFrame.oldFrame = client->clientMarkers[j].torsoOldFrame;
			ent->torsoFrame.frame = client->clientMarkers[j].torsoFrame;
			ent->torsoFrame.oldFrameTime = client->clientMarkers[j].torsoOldFrameTime;
			ent->torsoFrame.frameTime = client->clientMarkers[j].torsoFrameTime;
			ent->torsoFrame.yawAngle = client->clientMarkers[j].torsoYawAngle;
			ent->torsoFrame.pitchAngle = client->clientMarkers[j].torsoPitchAngle;
			ent->torsoFrame.yawing = client->clientMarkers[j].torsoYawing;
			ent->torsoFrame.pitching = client->clientMarkers[j].torsoPitching;

			// forty - realistic hitboxes - Legs Markers
			ent->legsFrame.oldFrameModel = client->clientMarkers[j].legsOldFrameModel;
			ent->legsFrame.frameModel = client->clientMarkers[j].legsFrameModel;
			ent->legsFrame.oldFrame = client->clientMarkers[j].legsOldFrame;
			ent->legsFrame.frame = client->clientMarkers[j].legsFrame;
			ent->legsFrame.oldFrameTime = client->clientMarkers[j].legsOldFrameTime;
			ent->legsFrame.frameTime = client->clientMarkers[j].legsFrameTime;
			ent->legsFrame.yawAngle = client->clientMarkers[j].legsYawAngle;
			ent->legsFrame.pitchAngle = client->clientMarkers[j].legsPitchAngle;
			ent->legsFrame.yawing = client->clientMarkers[j].legsYawing;
			ent->legsFrame.pitching = client->clientMarkers[j].legsPitching;

			// forty - realistic hitboxes - time stamp for BuildHead/Leg
			ent->timeShiftTime = client->clientMarkers[j].time;
		}
		else
		{
			client->ps.eFlags = client->clientMarkers[i].eFlags;
			client->ps.pm_flags = client->clientMarkers[i].pm_flags;
			client->ps.viewheight = client->clientMarkers[i].viewheight;

			// forty - realistic hitboxes - Torso Markers
			ent->torsoFrame.oldFrameModel = client->clientMarkers[i].torsoOldFrameModel;
			ent->torsoFrame.frameModel = client->clientMarkers[i].torsoFrameModel;
			ent->torsoFrame.oldFrame = client->clientMarkers[i].torsoOldFrame;
			ent->torsoFrame.frame = client->clientMarkers[i].torsoFrame;
			ent->torsoFrame.oldFrameTime = client->clientMarkers[i].torsoOldFrameTime;
			ent->torsoFrame.frameTime = client->clientMarkers[i].torsoFrameTime;
			ent->torsoFrame.yawAngle = client->clientMarkers[i].torsoYawAngle;
			ent->torsoFrame.pitchAngle = client->clientMarkers[i].torsoPitchAngle;
			ent->torsoFrame.yawing = client->clientMarkers[i].torsoYawing;
			ent->torsoFrame.pitching = client->clientMarkers[i].torsoPitching;

			// forty - realistic hitboxes - Legs Markers
			ent->legsFrame.oldFrameModel = client->clientMarkers[i].legsOldFrameModel;
			ent->legsFrame.frameModel = client->clientMarkers[i].legsFrameModel;
			ent->legsFrame.oldFrame = client->clientMarkers[i].legsOldFrame;
			ent->legsFrame.frame = client->clientMarkers[i].legsFrame;
			ent->legsFrame.oldFrameTime = client->clientMarkers[i].legsOldFrameTime;
			ent->legsFrame.frameTime = client->clientMarkers[i].legsFrameTime;
			ent->legsFrame.yawAngle = client->clientMarkers[i].legsYawAngle;
			ent->legsFrame.pitchAngle = client->clientMarkers[i].legsPitchAngle;
			ent->legsFrame.yawing = client->clientMarkers[i].legsYawing;
			ent->legsFrame.pitching = client->clientMarkers[i].legsPitching;

			// forty - realistic hitboxes - time stamp for BuildHead/Leg
			ent->timeShiftTime = client->clientMarkers[i].time;
		}

		if ( debugger && debugger->client )
		{

			// print some debugging stuff exactly like what the client does
			// it starts with "Rec:" to let you know it backward-reconciled
			char	msg[2048];
			Com_sprintf( msg, sizeof(msg),
						 "print \"^1Rec: time: %d, j: %d, k: %d, origin: %0.2f %0.2f %0.2f\n^2frac: %0.4f, origin1: %0.2f %0.2f %0.2f, origin2: %0.2f %0.2f %0.2f\n^7level.time: %d, est time: %d, level.time delta: %d, est real ping: %d\n\"",
					 time, client->clientMarkers[i].time, client->clientMarkers[j].time, ent->r.currentOrigin[0],
						 ent->r.currentOrigin[1], ent->r.currentOrigin[2], frac, client->clientMarkers[i].origin[0],
						 client->clientMarkers[i].origin[1], client->clientMarkers[i].origin[2],
						 client->clientMarkers[j].origin[0], client->clientMarkers[j].origin[1],
						 client->clientMarkers[j].origin[2], level.time, level.time + debugger->client->frameOffset,
						 level.time - time, level.time + debugger->client->frameOffset - time );
			trap_SendServerCommand( debugger - g_entities, msg );
		}
	}
	else
	{
		VectorCopy( client->clientMarkers[j].origin, ent->r.currentOrigin );
		VectorCopy( client->clientMarkers[j].mins, ent->r.mins );
		VectorCopy( client->clientMarkers[j].maxs, ent->r.maxs );

		//// josh: BuildHead/Legs uses these
		VectorCopy( client->clientMarkers[j].viewangles, client->ps.viewangles );
		client->ps.eFlags = client->clientMarkers[j].eFlags;
		client->ps.pm_flags = client->clientMarkers[j].pm_flags;
		client->ps.viewheight = client->clientMarkers[j].viewheight;

		// forty - realistic hitboxes - Torso Markers
		ent->torsoFrame.oldFrameModel = client->clientMarkers[j].torsoOldFrameModel;
		ent->torsoFrame.frameModel = client->clientMarkers[j].torsoFrameModel;
		ent->torsoFrame.oldFrame = client->clientMarkers[j].torsoOldFrame;
		ent->torsoFrame.frame = client->clientMarkers[j].torsoFrame;
		ent->torsoFrame.oldFrameTime = client->clientMarkers[j].torsoOldFrameTime;
		ent->torsoFrame.frameTime = client->clientMarkers[j].torsoFrameTime;
		ent->torsoFrame.yawAngle = client->clientMarkers[j].torsoYawAngle;
		ent->torsoFrame.pitchAngle = client->clientMarkers[j].torsoPitchAngle;
		ent->torsoFrame.yawing = client->clientMarkers[j].torsoYawing;
		ent->torsoFrame.pitching = client->clientMarkers[j].torsoPitching;

		// forty - realistic hitboxes - Legs Markers
		ent->legsFrame.oldFrameModel = client->clientMarkers[j].legsOldFrameModel;
		ent->legsFrame.frameModel = client->clientMarkers[j].legsFrameModel;
		ent->legsFrame.oldFrame = client->clientMarkers[j].legsOldFrame;
		ent->legsFrame.frame = client->clientMarkers[j].legsFrame;
		ent->legsFrame.oldFrameTime = client->clientMarkers[j].legsOldFrameTime;
		ent->legsFrame.frameTime = client->clientMarkers[j].legsFrameTime;
		ent->legsFrame.yawAngle = client->clientMarkers[j].legsYawAngle;
		ent->legsFrame.pitchAngle = client->clientMarkers[j].legsPitchAngle;
		ent->legsFrame.yawing = client->clientMarkers[j].legsYawing;
		ent->legsFrame.pitching = client->clientMarkers[j].legsPitching;

		// forty - realistic hitboxes - time stamp for BuildHead/Leg
		ent->timeShiftTime = client->clientMarkers[j].time;
	}

	trap_LinkEntity( ent );
}


/* */
void
G_ReAdjustSingleClientPosition ( gentity_t *ent )
{
	gclient_t	*client;
	if ( !G_AntilagSafe( ent) )
	{
		return;
	}

#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}

#else //!__NPC__
	client = ent->client;
#endif //__NPC__
	if ( client->backupMarker.time == level.time )
	{
		VectorCopy( client->backupMarker.origin, ent->r.currentOrigin );
		VectorCopy( client->backupMarker.mins, ent->r.mins );
		VectorCopy( client->backupMarker.maxs, ent->r.maxs );

		//josh: Head, Legs stuff
		VectorCopy( client->backupMarker.viewangles, client->ps.viewangles );
		client->ps.eFlags = client->backupMarker.eFlags;
		client->ps.pm_flags = client->backupMarker.pm_flags;
		client->ps.viewheight = client->backupMarker.viewheight;

		//client->backupMarker.servertime =	0;
		client->backupMarker.time = 0;

		// forty - realistic hitboxes - Torso Markers
		ent->torsoFrame.oldFrameModel = client->backupMarker.torsoOldFrameModel;
		ent->torsoFrame.frameModel = client->backupMarker.torsoFrameModel;
		ent->torsoFrame.oldFrame = client->backupMarker.torsoOldFrame;
		ent->torsoFrame.frame = client->backupMarker.torsoFrame;
		ent->torsoFrame.oldFrameTime = client->backupMarker.torsoOldFrameTime;
		ent->torsoFrame.frameTime = client->backupMarker.torsoFrameTime;
		ent->torsoFrame.yawAngle = client->backupMarker.torsoYawAngle;
		ent->torsoFrame.pitchAngle = client->backupMarker.torsoPitchAngle;
		ent->torsoFrame.yawing = client->backupMarker.torsoYawing;
		ent->torsoFrame.pitching = client->backupMarker.torsoPitching;

		// forty - realistic hitboxes - Legs Markers
		ent->legsFrame.oldFrameModel = client->backupMarker.legsOldFrameModel;
		ent->legsFrame.frameModel = client->backupMarker.legsFrameModel;
		ent->legsFrame.oldFrame = client->backupMarker.legsOldFrame;
		ent->legsFrame.frame = client->backupMarker.legsFrame;
		ent->legsFrame.oldFrameTime = client->backupMarker.legsOldFrameTime;
		ent->legsFrame.frameTime = client->backupMarker.legsFrameTime;
		ent->legsFrame.yawAngle = client->backupMarker.legsYawAngle;
		ent->legsFrame.pitchAngle = client->backupMarker.legsPitchAngle;

		// forty - realistic hitboxes - time stamp for BuildHead/Leg
		ent->timeShiftTime = 0;
		trap_LinkEntity( ent );
	}
}


/* */
void
G_AdjustClientPositions ( gentity_t *ent, int time, qboolean forward )
{
	int			i;
	gentity_t	*list;

#ifdef __NPC__
	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		list = &g_entities[i];

		// unlagged doesn't adjust the firing client
		if ( list == ent )
		{
			continue;
		}

		if ( list->s.eType != ET_PLAYER && list->s.eType != ET_NPC )
		{
			continue;
		}

		if ( forward )
		{
			G_AdjustSingleClientPosition( list, time, NULL );
		}
		else
		{
			G_ReAdjustSingleClientPosition( list );
		}
	}

#else //!__NPC__
	for ( i = 0; i < level.numConnectedClients; i++, list++ )
	{
		list = g_entities + level.sortedClients[i];

		// unlagged doesn't adjust the firing client
		if ( list == ent )
		{
			continue;
		}

		if ( forward )
		{
			G_AdjustSingleClientPosition( list, time, NULL );
		}
		else
		{
			G_ReAdjustSingleClientPosition( list );
		}
	}
#endif //__NPC__
}


/* */
void
G_ResetMarkers ( gentity_t *ent )
{
	int			i, time;
	float		period;
	int			eFlags;
	gclient_t	*client;
#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}

#else //!__NPC__
	client = ent->client;
#endif //__NPC__
	period = sv_fps.value;
	if ( !period )
	{
		period = 50;
	}
	else
	{
		period = 1000.f / period;
	}

	eFlags = client->ps.eFlags;

	// tjw: don't save entity flags that are not allowed
	//      for clientMarkers
	if ( (eFlags & EF_MOUNTEDTANK) )
	{
		eFlags &= ~EF_MOUNTEDTANK;
	}

	client->topMarker = MAX_CLIENT_MARKERS - 1;
	for ( i = MAX_CLIENT_MARKERS - 1, time = level.time; i >= 0; i--, time -= period )
	{
		VectorCopy( ent->r.mins, client->clientMarkers[i].mins );
		VectorCopy( ent->r.maxs, client->clientMarkers[i].maxs );
		VectorCopy( ent->r.currentOrigin, client->clientMarkers[i].origin );
		client->clientMarkers[i].time = time;
		VectorCopy( client->ps.viewangles, client->clientMarkers[i].viewangles );
		client->clientMarkers[i].eFlags = eFlags;
		client->clientMarkers[i].pm_flags = client->ps.pm_flags;
		client->clientMarkers[i].viewheight = client->ps.viewheight;

		// forty - realistic hitboxes - Torso Markers
		client->clientMarkers[i].torsoOldFrameModel = ent->torsoFrame.oldFrameModel;
		client->clientMarkers[i].torsoFrameModel = ent->torsoFrame.frameModel;
		client->clientMarkers[i].torsoOldFrame = ent->torsoFrame.oldFrame;
		client->clientMarkers[i].torsoFrame = ent->torsoFrame.frame;
		client->clientMarkers[i].torsoOldFrameTime = ent->torsoFrame.oldFrameTime;
		client->clientMarkers[i].torsoFrameTime = ent->torsoFrame.frameTime;
		client->clientMarkers[i].torsoYawAngle = ent->torsoFrame.yawAngle;
		client->clientMarkers[i].torsoPitchAngle = ent->torsoFrame.pitchAngle;
		client->clientMarkers[i].torsoYawing = ent->torsoFrame.yawing;
		client->clientMarkers[i].torsoPitching = ent->torsoFrame.pitching;

		// forty - realistic hitboxes - Legs Markers
		client->clientMarkers[i].legsOldFrameModel = ent->legsFrame.oldFrameModel;
		client->clientMarkers[i].legsFrameModel = ent->legsFrame.frameModel;
		client->clientMarkers[i].legsOldFrame = ent->legsFrame.oldFrame;
		client->clientMarkers[i].legsFrame = ent->legsFrame.frame;
		client->clientMarkers[i].legsOldFrameTime = ent->legsFrame.oldFrameTime;
		client->clientMarkers[i].legsFrameTime = ent->legsFrame.frameTime;
		client->clientMarkers[i].legsYawAngle = ent->legsFrame.yawAngle;
		client->clientMarkers[i].legsPitchAngle = ent->legsFrame.pitchAngle;
		client->clientMarkers[i].legsYawing = ent->legsFrame.yawing;
		client->clientMarkers[i].legsPitching = ent->legsFrame.pitching;
	}

	// forty - realistic hitboxes - time stamp for BuildHead/Leg
	ent->timeShiftTime = 0;
}


/* */
void
G_AttachBodyParts ( gentity_t *ent )
{
	int			i;
	gentity_t	*list;
#ifdef __NPC__
	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		list = &g_entities[i];
		if ( !list )
		{
			continue;
		}

		if ( list->s.eType != ET_PLAYER && list->s.eType != ET_NPC )
		{
			continue;
		}

		if ( list->NPC_client )
		{

			// Gordon: ok lets test everything under the sun
			if
			(
				list->inuse &&
				(
					list->NPC_client->sess.sessionTeam == TEAM_AXIS ||
					list->NPC_client->sess.sessionTeam == TEAM_ALLIES
				) &&
				(list != ent) &&
				list->r.linked &&
				!(list->NPC_client->ps.eFlags & EF_TANK) &&
				!(list->NPC_client->ps.pm_flags & PMF_LIMBO) &&
				(list->NPC_client->ps.pm_type == PM_NORMAL || list->NPC_client->ps.pm_type == PM_DEAD)
			)
			{
				list->NPC_client->tempHead = G_BuildHead( list );
				list->NPC_client->tempLeg = G_BuildLeg( list );
				list->NPC_client->tempBody = G_BuildBody( list );
				list->NPC_client->tempGroin = G_BuildGroin( list );
			}
			else
			{
				list->NPC_client->tempHead = NULL;
				list->NPC_client->tempLeg = NULL;
				list->NPC_client->tempBody = NULL;
				list->NPC_client->tempGroin = NULL;
			}
		}
		else if ( list->client )// jaquboss, it seems it crashes in SP
		{
			if (!list->client)
				continue;

			// Gordon: ok lets test everything under the sun
			if
			(
				list->inuse &&
				(list->client->sess.sessionTeam == TEAM_AXIS || list->client->sess.sessionTeam == TEAM_ALLIES) &&
				(list != ent) &&
				list->r.linked &&
				!(list->client->ps.eFlags & EF_TANK) &&
				!(list->client->ps.pm_flags & PMF_LIMBO) &&
				(list->client->ps.pm_type == PM_NORMAL || list->client->ps.pm_type == PM_DEAD)
			)
			{
				list->client->tempHead = G_BuildHead( list );
				list->client->tempLeg = G_BuildLeg( list );
				list->client->tempBody = G_BuildBody( list );
				list->client->tempGroin = G_BuildGroin( list );
			}
			else
			{
				list->client->tempHead = NULL;
				list->client->tempLeg = NULL;
				list->client->tempBody = NULL;
				list->client->tempGroin = NULL;
			}
		}
	}

#else //!__NPC__
	for ( i = 0; i < level.numConnectedClients; i++, list++ )
	{
		list = g_entities + level.sortedClients[i];

		// Gordon: ok lets test everything under the sun
		if
		(
			list->inuse &&
			(list->client->sess.sessionTeam == TEAM_AXIS || list->client->sess.sessionTeam == TEAM_ALLIES) &&
			(list != ent) &&
			list->r.linked &&
			!(list->client->ps.eFlags & EF_TANK) &&
			!(list->client->ps.pm_flags & PMF_LIMBO) &&
			(list->client->ps.pm_type == PM_NORMAL || list->client->ps.pm_type == PM_DEAD)
		)
		{
			list->client->tempHead = G_BuildHead( list );
			list->client->tempLeg = G_BuildLeg( list );
			list->client->tempBody = G_BuildBody( list );
			list->client->tempGroin = G_BuildGroin( list );
		}
		else
		{
			list->client->tempHead = NULL;
			list->client->tempLeg = NULL;
			list->client->tempBody = NULL;
			list->client->tempGroin = NULL;
		}
	}
#endif //__NPC__
}


/* */
void
G_DettachBodyParts ( void )
{
	int			i;
	gentity_t	*list;
#ifdef __NPC__
	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		list = &g_entities[i];
		if ( !list )
		{
			continue;
		}

		if ( list->s.eType != ET_PLAYER && list->s.eType != ET_NPC )
		{
			continue;
		}

		if ( list->NPC_client )
		{
			if ( list->NPC_client->tempHead )
			{
				G_FreeEntity( list->NPC_client->tempHead );
			}

			if ( list->NPC_client->tempBody )
			{
				G_FreeEntity( list->NPC_client->tempBody );
			}

			if ( list->NPC_client->tempGroin )
			{
				G_FreeEntity( list->NPC_client->tempGroin );
			}

			if ( list->NPC_client->tempLeg )
			{
				G_FreeEntity( list->NPC_client->tempLeg );
			}
		}
		else if ( list->client )
		{
			if ( list->client->tempHead )
			{
				G_FreeEntity( list->client->tempHead );
			}

			if ( list->client->tempBody )
			{
				G_FreeEntity( list->client->tempBody );
			}

			if ( list->client->tempGroin )
			{
				G_FreeEntity( list->client->tempGroin );
			}

			if ( list->client->tempLeg )
			{
				G_FreeEntity( list->client->tempLeg );
			}
		}
	}

#else //!__NPC__
	for ( i = 0; i < level.numConnectedClients; i++, list++ )
	{
		list = g_entities + level.sortedClients[i];
		if ( list->client->tempHead )
		{
			G_FreeEntity( list->client->tempHead );
		}

		if ( list->client->tempBody )
		{
			G_FreeEntity( list->client->tempBody );
		}

		if ( list->client->tempGroin )
		{
			G_FreeEntity( list->client->tempGroin );
		}

		if ( list->client->tempLeg )
		{
			G_FreeEntity( list->client->tempLeg );
		}
	}
#endif //__NPC__
}


/* */
int
G_SwitchBodyPartEntity ( gentity_t *ent )
{
	if ( ent->s.eType == ET_TEMPHEAD )
	{
		return ( ent->parent - g_entities );
	}

	if ( ent->s.eType == ET_TEMPBODY )
	{
		return ( ent->parent - g_entities );
	}

	if ( ent->s.eType == ET_TEMPGROIN )
	{
		return ( ent->parent - g_entities );
	}

	if ( ent->s.eType == ET_TEMPLEGS )
	{
		return ( ent->parent - g_entities );
	}

	return ( ent - g_entities );
}

#define POSITION_READJUST	if ( res != results->entityNum )   \
	{														   \
		VectorSubtract( end, start, dir );					   \
		VectorNormalizeFast( dir );							   \
		VectorMA( results->endpos, -1, dir, results->endpos ); \
		results->entityNum = res;							   \
	}


// Run a trace with players in historical positions.
void
G_HistoricalTrace
(
	gentity_t		*ent,
	trace_t			*results,
	const vec3_t	start,
	const vec3_t	mins,
	const vec3_t	maxs,
	const vec3_t	end,
	int				passEntityNum,
	int				contentmask
)
{
	int			res;
	vec3_t		dir;
	gclient_t	*client;
#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}

#else //!__NPC__
	client = ent->client;
#endif //__NPC__
	if ( !g_antilag.integer || !client )
	{
		G_AttachBodyParts( ent );
		trap_Trace( results, start, mins, maxs, end, passEntityNum, contentmask );
		res = G_SwitchBodyPartEntity( &g_entities[results->entityNum] );

		POSITION_READJUST	G_DettachBodyParts ();
		return;
	}

	G_AdjustClientPositions( ent, client->pers.cmd.serverTime, qtrue );
	G_AttachBodyParts( ent );
	trap_Trace( results, start, mins, maxs, end, passEntityNum, contentmask );
	res = G_SwitchBodyPartEntity( &g_entities[results->entityNum] );

	POSITION_READJUST	G_DettachBodyParts ();
	G_AdjustClientPositions( ent, 0, qfalse );
}


/* */
void
G_HistoricalTraceBegin ( gentity_t *ent )
{
	gclient_t	*client;
#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}

#else //!__NPC__
	client = ent->client;
#endif //__NPC__
	G_AdjustClientPositions( ent, client->pers.cmd.serverTime, qtrue );
}


/* */
void
G_HistoricalTraceEnd ( gentity_t *ent )
{
	G_AdjustClientPositions( ent, 0, qfalse );
}


//bani - Run a trace without fixups (historical fixups will be done externally)
void
G_Trace
(
	gentity_t		*ent,
	trace_t			*results,
	const vec3_t	start,
	const vec3_t	mins,
	const vec3_t	maxs,
	const vec3_t	end,
	int				passEntityNum,
	int				contentmask
)
{
	int		res;
	vec3_t	dir;
	G_AttachBodyParts( ent );
	if ( g_realhead.integer > 1 );
	G_TempTraceIgnorePlayersAndBodies();
	trap_Trace( results, start, mins, maxs, end, passEntityNum, contentmask );
	res = G_SwitchBodyPartEntity( &g_entities[results->entityNum] );
	POSITION_READJUST if ( g_realhead.integer > 1 );
	G_ResetTempTraceIgnoreEnts();
	G_DettachBodyParts();
}


/* */
qboolean
G_SkipCorrectionSafe ( gentity_t *ent )
{
	gclient_t	*client;
	if ( !ent )
	{
		return ( qfalse );
	}

#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}

#else //!__NPC__
	client = ent->client;
#endif //__NPC__
	if ( !ent->inuse )
	{
		return ( qfalse );
	}

	if ( !ent->r.linked )
	{
		return ( qfalse );
	}

	if ( !ent->client )
	{
		return ( qfalse );
	}

	if ( client->sess.sessionTeam != TEAM_AXIS && client->sess.sessionTeam != TEAM_ALLIES )
	{
		return ( qfalse );
	}

	if
	(
		(client->ps.pm_flags & PMF_LIMBO) ||
		(client->ps.pm_flags & PMF_TIME_LOCKMOVE) ||
		(client->ps.pm_flags & PMF_TIME_LOCKPLAYER)
	)
	{
		return ( qfalse );
	}

	if ( ent->health <= 0 )
	{
		return ( qfalse );
	}

	if ( client->ps.pm_type != PM_NORMAL )
	{
		return ( qfalse );
	}

	// tjw: this causes segfault with new PM_TraceLegs() from
	//      2.60 because it wants to update pm.pmext which
	//      we don't bother passing.
	if ( (client->ps.eFlags & EF_PRONE) )
	{
		return ( qfalse );
	}

	if ( (client->ps.eFlags & EF_MOUNTEDTANK) )
	{
		return ( qfalse );
	}

	// tjw: perhaps 2 would be OK too?
	if ( ent->waterlevel > 1 )
	{
		return ( qfalse );
	}

	// tjw: don't bother with skip correction if the player
	//      isn't moving horizontally
	if ( !client->ps.velocity[0] && !client->ps.velocity[1] )
	{
		return ( qfalse );
	}

	return ( qtrue );
}


// G_PredictPmove
//
// Make use of the 'Pmove' functions to figure out where a player
// would have moved in the short amount of time indicated by frametime.
//
// After the Pmove is complete, copy the values to the player's entity
// state, but then copy the original player state values back to the
// player state so that the player's movements aren't effected in any way.
//
// The only Pmove functions used are PM_StepSlideMove() and
// PM_GroundTrace() since these are all that is needed for this
// short period of time.  This means that a lot of movement types
// cannot be predicted and no prediction should be done for them

// See G_SkipCorrectionSafe()
void
G_PredictPmove ( gentity_t *ent, float frametime )
{
	pmove_t		pm;
	vec3_t		origin;
	vec3_t		velocity;
	int			groundEntityNum;
	int			pm_flags;
	int			pm_time;
	int			eFlags;
	gclient_t	*client;
	if ( !G_SkipCorrectionSafe( ent) )
	{
		return;
	}

#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}

#else //!__NPC__
	client = ent->client;
#endif //__NPC__

	// tjw: backup all the playerState values that may
	//      be altered by PmovePredict()
	VectorCopy( client->ps.origin, origin );
	VectorCopy( client->ps.velocity, velocity );
	groundEntityNum = client->ps.groundEntityNum;
	pm_flags = client->ps.pm_flags;
	pm_time = client->ps.pm_time;
	eFlags = client->ps.eFlags;
	memset( &pm, 0, sizeof(pm) );
	pm.ps = &client->ps;
	pm.character = client->pers.character;
	pm.trace = trap_TraceCapsuleNoEnts;
	pm.pointcontents = trap_PointContents;
	pm.tracemask = MASK_PLAYERSOLID;
	VectorCopy( ent->r.mins, pm.mins );
	VectorCopy( ent->r.maxs, pm.maxs );
	pm.predict = qtrue;

	//pm.debugLevel = 1;
	PmovePredict( &pm, frametime );

	//G_Printf("origin: before %s after %s\n",
	//	vtos(ent->s.pos.trBase),
	//	vtos(client->ps.origin));
	// tjw: update entity state with the resulting player state
	VectorCopy( client->ps.origin, ent->s.pos.trBase );
	VectorCopy( client->ps.velocity, ent->s.pos.trDelta );
	ent->s.groundEntityNum = client->ps.groundEntityNum;
	ent->s.eFlags = client->ps.eFlags;

	// tjw: this needs to be updated in case g_antilag is not enabled
	VectorCopy( client->ps.origin, ent->r.currentOrigin );

	// josh: link them to stop them from skipping over mines and being
	//       unhittable when antilag is 0
	trap_LinkEntity( ent );

	// tjw: restore player state so that their next command
	//      will process as if nothing has happened.
	VectorCopy( origin, client->ps.origin );
	VectorCopy( velocity, client->ps.velocity );
	client->ps.groundEntityNum = groundEntityNum;
	client->ps.pm_flags = pm_flags;
	client->ps.pm_time = pm_time;
	client->ps.eFlags = eFlags;
}
