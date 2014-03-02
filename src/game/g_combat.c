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
 * name:		g_combat.c
 *
 * desc:		
 *
*/
#include "g_local.h"
#include "../game/q_shared.h"
#include "../game/botlib.h" //bot lib interface
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../botai/botai.h" //bot ai interface
#include "../botai/ai_main.h"
#include "../botai/chars.h"
#include "../botai/ai_team.h"
#include "../botai/ai_dmq3.h"
#include "etpro_mdx.h"
extern void		BotRecordKill ( int client, int enemy );
extern void		BotRecordPain ( int client, int enemy, int mod );
extern void		BotRecordDeath ( int client, int enemy );
extern vec3_t	muzzleTrace;


/*
============
AddScore

Adds score to both the client and his team
============
*/
void
AddScore ( gentity_t *ent, int score )
{
	if ( !ent || !ent->client )
	{
		return;
	}

	// no scoring during pre-match warmup
	if ( g_gamestate.integer != GS_PLAYING )
	{
		return;
	}

	if
	(
		g_gametype.integer == GT_WOLF_LMS ||
		g_gametype.integer == GT_WOLF_DM ||
		g_gametype.integer == GT_WOLF_TDM ||
		g_gametype.integer == GT_WOLF_CTF ||
		g_noxp.integer > 0
	)
	{
		return;
	}

	ent->client->sess.game_points += score;
	CalculateRanks();
}


/*
============
AddKillScore

Adds score to both the client and his team, only used for playerkills, for lms
============
*/
void
AddKillScore ( gentity_t *ent, int score )
{
	if ( !ent || !ent->client )
	{
		return;
	}

	// no scoring during pre-match warmup
	if ( level.warmupTime )
	{
		return;
	}

	// someone already won
	if ( level.lmsWinningTeam )
	{
		return;
	}

	if
	(
		g_gametype.integer == GT_WOLF_CTF ||
		g_gametype.integer == GT_WOLF_LMS ||
		g_gametype.integer == GT_WOLF_DM ||
		g_gametype.integer == GT_WOLF_TDM ||
		g_noxp.integer > 0
	)
	{
		ent->client->ps.persistant[PERS_SCORE] += score;
		if ( g_gametype.integer != GT_WOLF_CTF )
		{
			level.teamScores[ent->client->ps.persistant[PERS_TEAM]] += score;
		}
	}

	ent->client->sess.game_points += score;
	trap_SendServerCommand( -1, va( "sc4 \"%i\"", level.teamScores[TEAM_AXIS]) );
	trap_SendServerCommand( -1, va( "sc5 \"%i\"", level.teamScores[TEAM_ALLIES]) );
	CalculateRanks();
}


///////// jaquboss
void
AddCaptureScore ( gentity_t *ent, int score )
{
	if ( !ent || !ent->client )
	{
		return;
	}

	// no scoring during pre-match warmup
	if ( level.warmupTime )
	{
		return;
	}

	if ( g_gametype.integer == GT_WOLF_CTF )
	{
		ent->client->ps.persistant[PERS_SCORE] += score;				// for player
		level.teamScores[ent->client->ps.persistant[PERS_TEAM]] += 1;	// one capture done ;-)
	}

	ent->client->sess.game_points += score;
	trap_SendServerCommand( -1, va( "sc4 \"%i\"", level.teamScores[TEAM_AXIS]) );
	trap_SendServerCommand( -1, va( "sc5 \"%i\"", level.teamScores[TEAM_ALLIES]) );
	CalculateRanks();
}


/*
================
DropCurrentWeapon

Drops selected weapon
================
*/
void
DropCurrentWeapon ( gentity_t *self )
{
	weapon_t	secondaryWeapon;
	weapon_t	primaryWeapon;
	primaryWeapon = G_GetPrimaryWeaponForClient( self->client );
	secondaryWeapon = G_GetSecondaryWeaponForClient( self->client );
	if ( self->client->ps.weapon == secondaryWeapon )
	{	// secondary weapon dropping
		if ( secondaryWeapon != WP_NONE )
		{
			G_DropWeapon( self, secondaryWeapon );
			BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_DROPWEAPON,
								qfalse, qtrue );
		}
	}

	if ( self->client->ps.weapon == primaryWeapon )
	{	// primary weapon dropping
		if ( primaryWeapon != WP_NONE )
		{
			G_DropWeapon( self, primaryWeapon );
			BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo, ANIM_ET_DROPWEAPON,
								qfalse, qtrue );
		}
	}

	// TODO KNIFE AND GRENADES?
}


/*
=================
TossClientItems

Toss the weapon and powerups for the killed player
=================
*/
void
TossClientItems ( gentity_t *self )
{
	weapon_t	primaryWeapon;
	weapon_t	secondaryWeapon;
	if ( g_gamestate.integer == GS_INTERMISSION )
	{
		return;
	}

	primaryWeapon = G_GetPrimaryWeaponForClient( self->client );
	secondaryWeapon = G_GetSecondaryWeaponForClient( self->client );
	if ( primaryWeapon )
	{

		// drop our primary weapon
		G_DropWeapon( self, primaryWeapon );
	}

	if ( secondaryWeapon )
	{

		// drop our secondary weapon
		G_DropWeapon( self, secondaryWeapon );
	}
}


/*
============================
Cmd_DropFlag_f

Drop carried objective
============================
*/
void
Cmd_DropFlag_f ( gentity_t *self )
{
	gitem_t *item = NULL;

	// drop flag regardless
	if ( self->client->ps.powerups[PW_REDFLAG] )
	{
		item = BG_FindItem( "Red Flag" );
		if ( !item )
		{
			item = BG_FindItem( "Objective" );
		}

		self->client->ps.powerups[PW_REDFLAG] = 0;
	}

	if ( self->client->ps.powerups[PW_BLUEFLAG] )
	{
		item = BG_FindItem( "Blue Flag" );
		if ( !item )
		{
			item = BG_FindItem( "Objective" );
		}

		self->client->ps.powerups[PW_BLUEFLAG] = 0;
	}

	if ( item )
	{
		vec3_t		launchvel = { 0, 0, 0 };
		gentity_t	*flag = LaunchItem( item, self->r.currentOrigin, launchvel, self->s.number );
		flag->s.modelindex2 = self->s.otherEntityNum2;	// JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
		flag->message = self->message;					// DHM - Nerve :: also restore item name

		// Clear out player's temp copies
		self->s.otherEntityNum2 = 0;
		self->message = NULL;
		self->client->dropObjectiveTime = level.time;
	}
}


/*
==================
LookAtKiller
==================
*/
void
LookAtKiller ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker )
{
	vec3_t	dir;
	vec3_t	angles;
	if ( attacker && attacker != self )
	{
		VectorSubtract( attacker->s.pos.trBase, self->s.pos.trBase, dir );
	}
	else if ( inflictor && inflictor != self )
	{
		VectorSubtract( inflictor->s.pos.trBase, self->s.pos.trBase, dir );
	}
	else
	{
		self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
		return;
	}

	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw( dir );
	angles[YAW] = vectoyaw( dir );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
}


/*
==================
GibEntity
==================
*/
void
GibEntity ( gentity_t *self, int killer )
{
	gentity_t	*other = &g_entities[killer];
	vec3_t		dir;
	VectorClear( dir );
	if ( other->inuse )
	{
		if ( other->client )
		{
			VectorSubtract( self->r.currentOrigin, other->r.currentOrigin, dir );
			VectorNormalize( dir );
		}
		else if ( !VectorCompare( other->s.pos.trDelta, vec3_origin) )
		{
			VectorNormalize2( other->s.pos.trDelta, dir );
		}
	}

	G_AddEvent( self, EV_GIB_PLAYER, DirToByte( dir) );
	self->takedamage = qfalse;
	self->s.eType = ET_INVISIBLE;
	self->r.contents = 0;
}


/*
================
TankExplode

Explode a tank 
================
*/
void
TankExplode ( gentity_t *ent )
{
	vec3_t	dir;
	vec3_t	origin;
	ent->takedamage = qfalse;
	ent->s.eType = ET_INVISIBLE;
	ent->r.contents = 0;
	ent->TankMarker = 0;

	// splash damage
	{
		vec3_t	origin;
		trace_t tr;
		VectorCopy( ent->r.currentOrigin, origin );
		origin[2] += 4;
		trap_Trace( &tr, origin, vec3_origin, vec3_origin, origin, ENTITYNUM_NONE, MASK_SHOT );
		G_RadiusDamage( origin, ent, ent, 55, 300, ent, MOD_EXPLOSIVE );
	}

	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin, qfalse, ent->s.effect2Time );
	SnapVector( origin );
	G_SetOrigin( ent, origin );

	// we don't have a valid direction, so just point straight up
	dir[0] = dir[1] = 0;
	dir[2] = 1;
	{
		gentity_t	*tent = G_TempEntity( ent->r.currentOrigin, EV_MISSILE_MISS_LARGE );
		tent->s.eventParm = DirToByte( dir );
	}
	{
		gentity_t	*tent = G_TempEntity( ent->r.currentOrigin, EV_SHAKE );
		tent->s.onFireStart = 1500;
		tent->r.svFlags |= SVF_BROADCAST;
	}
}


/*
==================
body_die
==================
*/
void
body_die ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	if ( self->health <= GIB_HEALTH /*&& !G_WeaponIsExplosive( meansOfDeath )*/ )
	{
		GibEntity( self, 0 );
	}
}

// these are just for logging, the client prints its own messages
char	*modNames[] =
{
	"MOD_UNKNOWN",
	"MOD_MACHINEGUN",
	"MOD_BROWNING",
	"MOD_MG42",
	"MOD_GRENADE",
	"MOD_ROCKET",

	// (SA) modified wolf weap mods
	"MOD_KNIFE",
	"MOD_LUGER",
	"MOD_COLT",
	"MOD_MP40",
	"MOD_THOMPSON",
	"MOD_STEN",
	"MOD_GARAND",
	"MOD_SNOOPERSCOPE",
	"MOD_SILENCER",		//----(SA)	
	"MOD_FG42",
	"MOD_FG42SCOPE",
	"MOD_PANZERFAUST",
	"MOD_GRENADE_LAUNCHER",
	"MOD_FLAMETHROWER",
	"MOD_GRENADE_PINEAPPLE",
	"MOD_CROSS",

	// end
	"MOD_MAPMORTAR",
	"MOD_MAPMORTAR_SPLASH",
	"MOD_KICKED",
	"MOD_GRABBER",
	"MOD_DYNAMITE",
	"MOD_AIRSTRIKE",	// JPW NERVE
	"MOD_SYRINGE",		// JPW NERVE
	"MOD_AMMO",			// JPW NERVE
	"MOD_ARTY",			// JPW NERVE
	"MOD_WATER",
	"MOD_SLIME",
	"MOD_LAVA",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TARGET_LASER",
	"MOD_TRIGGER_HURT",
	"MOD_EXPLOSIVE",
	"MOD_CARBINE",
	"MOD_KAR98",
	"MOD_GPG40",
	"MOD_M7",
	"MOD_LANDMINE",
	"MOD_SATCHEL",
	"MOD_TRIPMINE",
	"MOD_SMOKEBOMB",
	"MOD_MOBILE_MG42",
	"MOD_SILENCED_COLT",
	"MOD_GARAND_SCOPE",
	"MOD_CRUSH_CONSTRUCTION",
	"MOD_CRUSH_CONSTRUCTIONDEATH",
	"MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER",
	"MOD_K43",
	"MOD_K43_SCOPE",
	"MOD_MORTAR",
	"MOD_AKIMBO_COLT",
	"MOD_AKIMBO_LUGER",
	"MOD_AKIMBO_SILENCEDCOLT",
	"MOD_AKIMBO_SILENCEDLUGER",
	"MOD_SMOKEGRENADE",
	"MOD_AAGUN",
	"MOD_BROWNING2",
	"MOD_30CAL",
#ifdef NWEAPS
	"MOD_MP44",
	"MOD_BAR",
#endif

	// RF
	"MOD_SWAP_PLACES",

	// OSP -- keep these 2 entries last
	"MOD_SWITCHTEAM",
};

/*
==================
player_die
==================
*/
void	BotRecordTeamDeath ( int client );


/* */
void
player_die ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
	int			contents = 0, i, killer = ENTITYNUM_WORLD;
	char		*killerName = "<world>";
	qboolean	nogib = qtrue;
	gitem_t		*item = NULL;
	gentity_t	*ent;
	qboolean	killedintank = qfalse;
	int			deathID = 0;
	weapon_t	weap = BG_WeaponForMOD( meansOfDeath );

	//unlagged - backward reconciliation #2
	// make sure the body shows up in the client's current position
	G_ReAdjustSingleClientPosition( self );

	//unlagged - backward reconciliation #2
	if ( attacker == self )
	{
		if ( self->client )
		{
			self->client->pers.playerStats.suicides++;
			trap_PbStat( self - g_entities, "suicide",
						 va( "%d %d %d", self->client->sess.sessionTeam, self->client->sess.playerType, weap) );
		}
	}
	else if ( OnSameTeam( self, attacker) && g_gametype.integer != GT_WOLF_DM )
	{
		G_LogTeamKill( attacker, weap );
	}
	else
	{
		G_LogDeath( self, weap );
		G_LogKill( attacker, weap );
		if ( g_gamestate.integer == GS_PLAYING )
		{
			if ( attacker->client )
			{
				attacker->client->combatState |= ( 1 << COMBATSTATE_KILLEDPLAYER );
			}
		}

#ifdef __BOT__
		if ( self->r.svFlags & SVF_BOT && rand() % 100 > 50 && !(attacker->r.svFlags & SVF_BOT) )
		{														// great shot when killed by players, 50% chance
			G_AI_Voice( self, NULL, SAY_ALL, "GreatShot", qfalse, 500 + rand() % 1000, AIVOICE_FUNTALK );
		}
#endif
	}

	// RF, record this death in AAS system so that bots avoid areas which have high death rates
	if ( !OnSameTeam( self, attacker) || g_gametype.integer == GT_WOLF_DM )
	{
#ifndef NO_BOT_SUPPORT
		BotRecordTeamDeath( self->s.number );
#endif
		self->isProp = qfalse;									// were we teamkilled or not?
	}
	else
	{
		self->isProp = qtrue;
	}

	// if we got killed by a landmine, update our map
	if ( self->client && meansOfDeath == MOD_LANDMINE )
	{

		// if it's an enemy mine, update both teamlists
		mapEntityData_t *mEnt;
		if ( (mEnt = G_FindMapEntityData( &mapEntityData[0], inflictor - g_entities)) != NULL )
		{
			G_FreeMapEntityData( &mapEntityData[0], mEnt );
		}

		if ( (mEnt = G_FindMapEntityData( &mapEntityData[1], inflictor - g_entities)) != NULL )
		{
			G_FreeMapEntityData( &mapEntityData[1], mEnt );
		}
	}
	{
		mapEntityData_t			*mEnt;
		mapEntityData_Team_t	*teamList = self->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[1] : &
			mapEntityData[0];									// swapped, cause enemy team
		mEnt = G_FindMapEntityDataSingleClient( teamList, NULL, self->s.number, -1 );
		while ( mEnt )
		{
			if ( mEnt->type == ME_PLAYER_DISGUISED )
			{
				mapEntityData_t *mEntFree = mEnt;
				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, self->s.number, -1 );
				G_FreeMapEntityData( teamList, mEntFree );
			}
			else
			{
				mEnt = G_FindMapEntityDataSingleClient( teamList, mEnt, self->s.number, -1 );
			}
		}
	}

	if ( self->tankLink )
	{
		G_LeaveTank( self, qfalse );
		killedintank = qtrue;
	}

	if ( self->client->ps.pm_type == PM_DEAD || g_gamestate.integer == GS_INTERMISSION )
	{
		return;
	}

	// OSP - death stats handled out-of-band of G_Damage for external calls
	if ( meansOfDeath != MOD_SWAP_PLACES )
	{
		G_addStats( self, attacker, damage, meansOfDeath );
	}

	// OSP
	self->client->ps.pm_type = PM_DEAD;
	G_AddEvent( self, EV_STOPSTREAMINGSOUND, 0 );
	if ( attacker )
	{
		killer = attacker->s.number;
		killerName = ( attacker->client ) ? attacker->client->pers.netname : "<non-client>";
	}

	if ( attacker == 0 || killer < 0 || killer >= MAX_CLIENTS )
	{
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if ( g_gamestate.integer == GS_PLAYING )
	{
		char	*obit;
		if ( meansOfDeath < 0 || meansOfDeath >= sizeof(modNames) / sizeof(modNames[0]) )
		{
			obit = "<bad obituary>";
		}
		else
		{
			obit = modNames[meansOfDeath];
		}

		G_LogPrintf( "Kill: %i %i %i: %s killed %s by %s\n", killer, self->s.number, meansOfDeath, killerName,
					 self->client->pers.netname, obit );
	}

	// RF, record bot kills
#ifndef NO_BOT_SUPPORT
	if ( attacker->r.svFlags & SVF_BOT )
	{
		BotRecordKill( attacker->s.number, self->s.number );
	}
#endif
	if ( self->isHeadshot )
	{
		deathID = 1;
	}

	if ( self->isBackstabbed )
	{
		deathID = 2;
	}

#ifdef OMNIBOTS

	//////////////////////////////////////////////////////////////////////////
	// If the victim is a bot, send a death event.
	if ( self->r.svFlags & SVF_BOT )
	{
		BotUserData bud;
		const		numMods = sizeof( modNames ) / sizeof( modNames[0] );
		bud.DataType = dtString;
		bud.udata.m_String = "";
		if ( meansOfDeath >= 0 && meansOfDeath < numMods )
		{
			bud.udata.m_String = modNames[meansOfDeath];
		}

		Bot_Interface_SendEvent( MESSAGE_DEATH, self->s.number, attacker->s.number, 0, NULL );
	}

	// If the attacker is a bot, send a kill event.
	if ( attacker->r.svFlags & SVF_BOT )
	{
		BotUserData bud;
		const		numMods = sizeof( modNames ) / sizeof( modNames[0] );
		bud.DataType = dtString;
		bud.udata.m_String = "";
		if ( meansOfDeath >= 0 && meansOfDeath < numMods )
		{
			bud.udata.m_String = modNames[meansOfDeath];
		}

		Bot_Interface_SendEvent( MESSAGE_KILLEDSOMEONE, attacker->s.number, self->s.number, 0, NULL );
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// broadcast the death event to everyone
	ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
	ent->s.eventParm = meansOfDeath;
	ent->s.effect3Time = deathID;
	ent->s.otherEntityNum = self->s.number;
	ent->s.otherEntityNum2 = killer;
	ent->r.svFlags = SVF_BROADCAST;								// send to everyone
	self->enemy = attacker;
	self->client->ps.persistant[PERS_KILLED]++;

	// JPW NERVE -- if player is holding ticking grenade, drop it
	if ( (self->client->ps.grenadeTimeLeft) && (self->s.weapon != WP_DYNAMITE) && (self->s.weapon != WP_SATCHEL) )
	{															//&& (self->s.weapon != WP_TRIPMINE)) {
		vec3_t	launchvel, launchspot;
		launchvel[0] = crandom();
		launchvel[1] = crandom();
		launchvel[2] = random();
		VectorScale( launchvel, 160, launchvel );
		VectorCopy( self->r.currentOrigin, launchspot );
		launchspot[2] += 40;
		{

			// Gordon: fixes premature grenade explosion, ta bani ;)
			gentity_t	*m = fire_grenade( self, launchspot, launchvel, self->s.weapon /*, qtrue*/ );
			m->damage = 0;
		}
	}

	if ( attacker && attacker->client )
	{
		if ( attacker == self || (OnSameTeam( self, attacker) && g_gametype.integer != GT_WOLF_DM) )
		{

			// DHM - Nerve :: Complaint lodging
			if ( attacker != self && level.warmupTime <= 0 && g_gamestate.integer == GS_PLAYING )
			{
				if ( attacker->client->pers.localClient )
				{
					trap_SendServerCommand( self - g_entities, "complaint -4" );
				}
				else
				{
					if
					(
						meansOfDeath != MOD_CRUSH_CONSTRUCTION &&
						meansOfDeath != MOD_CRUSH_CONSTRUCTIONDEATH &&
						meansOfDeath != MOD_CRUSH_CONSTRUCTIONDEATH_NOATTACKER
					)
					{
						if ( g_complaintlimit.integer )
						{
							if
							(
								!(meansOfDeath == MOD_LANDMINE && g_disableComplaints.integer & TKFL_MINES) &&
								!(
									(meansOfDeath == MOD_ARTY || meansOfDeath == MOD_AIRSTRIKE) &&
									g_disableComplaints.integer & TKFL_AIRSTRIKE
								) &&
								!(meansOfDeath == MOD_MORTAR && g_disableComplaints.integer & TKFL_MORTAR)
							)
							{
								trap_SendServerCommand( self - g_entities, va( "complaint %i", attacker->s.number) );
								self->client->pers.complaintClient = attacker->s.clientNum;
#ifdef __BOT__
								if ( attacker->r.svFlags & SVF_BOT )
								{
									self->client->pers.complaintClient = -5;
								}
#endif //__BOT__
								self->client->pers.complaintEndTime = level.time + 20500;
							}
						}
					}
				}
			}

			// high penalty to offset medic heal
			if
			(
				g_gametype.integer == GT_WOLF_CTF ||
				g_gametype.integer == GT_WOLF_LMS ||
				g_noxp.integer > 0 ||
				g_gametype.integer == GT_WOLF_TDM
			)
			{
				AddKillScore( attacker, WOLF_FRIENDLY_PENALTY );
			}
		}
		else
		{

			// JPW NERVE -- mostly added as conveneience so we can tweak from the #defines all in one place
			AddScore( attacker, WOLF_FRAG_BONUS );
			if
			(
				g_gametype.integer == GT_WOLF_CTF ||
				g_gametype.integer == GT_WOLF_LMS ||
				g_noxp.integer > 0 ||
				g_gametype.integer == GT_WOLF_TDM ||
				g_gametype.integer == GT_WOLF_DM
			)
			{
				if ( level.firstbloodTeam == -1 )
				{
					level.firstbloodTeam = attacker->client->sess.sessionTeam;
				}

				AddKillScore( attacker, WOLF_FRAG_BONUS );
			}

			attacker->client->lastKillTime = level.time;
		}
	}
	else
	{
		AddScore( self, -1 );
		if
		(
			g_gametype.integer == GT_WOLF_CTF ||
			g_gametype.integer == GT_WOLF_LMS ||
			g_noxp.integer > 0 ||
			g_gametype.integer == GT_WOLF_TDM ||
			g_gametype.integer == GT_WOLF_DM
		)
		{
			AddKillScore( self, -1 );
		}
	}

	// Add team bonuses
	Team_FragBonuses( self, inflictor, attacker );

	// drop flag regardless
	if ( self->client->ps.powerups[PW_REDFLAG] )
	{
		item = BG_FindItem( "Red Flag" );
		if ( !item )
		{
			item = BG_FindItem( "Objective" );
		}

		self->client->ps.powerups[PW_REDFLAG] = 0;
	}

	if ( self->client->ps.powerups[PW_BLUEFLAG] )
	{
		item = BG_FindItem( "Blue Flag" );
		if ( !item )
		{
			item = BG_FindItem( "Objective" );
		}

		self->client->ps.powerups[PW_BLUEFLAG] = 0;
	}

	if ( item )
	{
		vec3_t		launchvel = { 0, 0, 0 };
		gentity_t	*flag = LaunchItem( item, self->r.currentOrigin, launchvel, self->s.number );
		flag->s.modelindex2 = self->s.otherEntityNum2;			// JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
		flag->message = self->message;							// DHM - Nerve :: also restore item name

		// Clear out player's temp copies
		self->s.otherEntityNum2 = 0;
		self->message = NULL;
	}

	// this is copied because of event thingies
	if ( !(self->client->ps.eFlags & EF_TANK) )
	{

		// send a fancy "MEDIC!" scream.  Sissies, ain' they?
		if ( self->client != NULL && g_gametype.integer != GT_WOLF_DM )
		{
			if
			(
				self->health > GIB_HEALTH &&
				self->waterlevel < 3 &&
				meansOfDeath != MOD_SUICIDE &&
				meansOfDeath != MOD_SWITCHTEAM &&
				meansOfDeath != MOD_WATER
			)
			{
				if ( self->health > FORCE_LIMBO_HEALTH )
				{
					G_AddEvent( self, EV_MEDIC_CALL, 0 );
#ifdef OMNIBOTS

					// ATM: only register the goal if the target isn't in water.
					if ( self->waterlevel <= 1 )
					{
						if ( self->client->sess.sessionTeam == TEAM_AXIS )
						{
							Bot_Util_AddGoal( (GameEntity *) self, ET_GOAL_FALLENTEAMMATE, (1 << ET_TEAM_AXIS), NULL,
											  NULL );
						}
						else if ( self->client->sess.sessionTeam == TEAM_ALLIES )
						{
							Bot_Util_AddGoal( (GameEntity *) self, ET_GOAL_FALLENTEAMMATE, (1 << ET_TEAM_ALLIES), NULL,
											  NULL );
						}
					}
#endif
				}												// death sounds here , todo ?
			}
		}
	}															// tank

	Cmd_Score_f( self );										// show scores

	// send updated scores to any clients that are following this one,
	// or they would get stale scoreboards
	for ( i = 0; i < level.numConnectedClients; i++ )
	{
		gclient_t	*client = &level.clients[level.sortedClients[i]];
		if ( client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		if ( client->sess.sessionTeam != TEAM_SPECTATOR )
		{
			continue;
		}

		if ( client->sess.spectatorClient == self->s.number )
		{
			Cmd_Score_f( g_entities + level.sortedClients[i] );
		}
	}

	self->takedamage = qtrue;									// can still be gibbed
	self->r.contents = CONTENTS_CORPSE;

	//self->s.angles[2] = 0;
	if ( g_realisticdisguise.integer != 1 )
	{
		self->s.powerups = 0;
	}

	self->s.loopSound = 0;
	LookAtKiller( self, inflictor, attacker );
	self->client->ps.viewangles[0] = 0;
	self->client->ps.viewangles[2] = 0;
	trap_LinkEntity( self );									// test it now!

	// don't allow respawn until the death anim is done
	// g_forcerespawn may force spawning at some later time
	if ( g_gametype.integer == GT_WOLF_DM && self->health > GIB_HEALTH )
	{
		self->client->respawnTime = level.timeCurrent + 2500;
		TossClientItems( self );								//drop items immidietly , so it doesnt look weird
	}
	else if	( g_latcheddeath.integer > 0 &&	self->health <= FORCE_LIMBO_HEALTH && self->health > GIB_HEALTH )
	{
		self->client->respawnTime = level.timeCurrent + 2200;	// todo find proper time
		TossClientItems( self );								//drop items immidietly , so it doesnt look weird
	}
	else
	{
		self->client->limboDropWeapon = 1;						// store this so it can be dropped in limbo
		self->client->respawnTime = level.timeCurrent + 800;
	}

	// remove powerups
	if ( g_realisticdisguise.integer != 1 )
	{
		memset( self->client->ps.powerups, 0, sizeof(self->client->ps.powerups) );
	}
	else
	{															// keep only disguise
		self->client->ps.powerups[PW_REDFLAG] = 0;
		self->client->ps.powerups[PW_BLUEFLAG] = 0;
		self->client->ps.powerups[PW_ADRENALINE] = 0;
	}

	// never gib in a nodrop
	// FIXME: contents is always 0 here
	if ( self->health <= GIB_HEALTH && !(contents & CONTENTS_NODROP) && G_WeaponIsExplosive( meansOfDeath) )
	{
		GibEntity( self, killer );
		nogib = qfalse;
	}

	if ( nogib )
	{
		int animResult;

		// normal death
		// for the no-blood option, we need to prevent the health
		// from going to gib level
		if ( self->health <= GIB_HEALTH )
		{
			self->health = GIB_HEALTH + 1;
		}

		if ( (self->health <= FORCE_LIMBO_HEALTH || g_gametype.integer == GT_WOLF_DM) && meansOfDeath != MOD_SUICIDE )
		{
			if ( self->client->ps.eFlags & EF_PRONE || self->client->ps.eFlags & EF_PRONE_MOVING )
			{
				animResult = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo,
												 ANIM_ET_FORCEDEATH_PRONE, qfalse, qtrue );
			}
			else if ( g_gametype.integer == GT_WOLF_DM )
			{
				animResult = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo,
												 ANIM_ET_DMDEATH, qfalse, qtrue );
			}
			else
			{
				animResult = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo,
												 ANIM_ET_FORCEDEATH, qfalse, qtrue );
			}
		}
		else
		{
			animResult /*self->client->ps.pm_time*/ = BG_AnimScriptEvent( &self->client->ps,
																		 self->client->pers.character->animModelInfo,
																		 ANIM_ET_DEATH, qfalse, qtrue );
		}

		if ( animResult < 0 )
		{
			animResult = BG_AnimScriptEvent( &self->client->ps, self->client->pers.character->animModelInfo,
											 ANIM_ET_DEATH, qfalse, qtrue );
		}

		// record the death animation to be used later on by the corpse
		self->client->torsoDeathAnim = self->client->ps.torsoAnim;
		self->client->legsDeathAnim = self->client->ps.legsAnim;
		G_AddEvent( self, EV_DEATH1 + 1, killer );				// todo!

		// the body can still be gibbed
		self->die = body_die;
	}

	if ( meansOfDeath == MOD_MACHINEGUN )
	{
		switch ( self->client->sess.sessionTeam )
		{
			case TEAM_AXIS:
				level.axisMG42Counter = level.time;
				break;
			case TEAM_ALLIES:
				level.alliesMG42Counter = level.time;
				break;
			default:
				break;
		}
	}

	G_FadeItems( self, MOD_SATCHEL );
	CalculateRanks();
	if ( self->client->ps.eFlags & EF_TANK )
	{
		limbo( self, qfalse );
		TankExplode( self );
	}
	else if ( killedintank /*Gordon: automatically go to limbo from tank*/ )
	{
		limbo( self, qfalse );									// but no corpse
	}
	else if ( (meansOfDeath == MOD_SUICIDE && g_gamestate.integer == GS_PLAYING) )
	{
		limbo( self, qtrue );
	}
	else if ( g_gametype.integer == GT_WOLF_LMS )
	{
		if ( !G_CountTeamMedics( self->client->sess.sessionTeam, qtrue) )
		{
			limbo( self, qtrue );
		}
	}
}


/* */
qboolean
IsHeadShotWeapon ( int mod )
{

	// players are allowed headshots from these weapons
	if
	(
		mod == MOD_LUGER ||
		mod == MOD_COLT ||
		mod == MOD_AKIMBO_COLT ||
		mod == MOD_AKIMBO_LUGER ||
		mod == MOD_AKIMBO_SILENCEDCOLT ||
		mod == MOD_AKIMBO_SILENCEDLUGER ||
		mod == MOD_MP40 ||
		mod == MOD_THOMPSON ||
		mod == MOD_STEN ||
		//			mod == MOD_PANZERFAUST ||
		mod == MOD_MOBILE_MG42 ||
		mod == MOD_30CAL ||
#ifdef NWEAPS
		mod == MOD_MP44 ||
		mod == MOD_BAR ||
#endif
		mod == MOD_GARAND ||
		mod == MOD_BROWNING ||
		mod == MOD_BROWNING2 ||
		mod == MOD_MACHINEGUN ||
		mod == MOD_KAR98 ||
		mod == MOD_K43 ||
		mod == MOD_K43_SCOPE ||
		mod == MOD_CARBINE ||
		mod == MOD_GARAND ||
		mod == MOD_GARAND_SCOPE ||
		mod == MOD_SILENCER ||
		mod == MOD_SILENCED_COLT ||
		mod == MOD_FG42 ||
		mod == MOD_FG42SCOPE
	)
	{
		return ( qtrue );
	}

	return ( qfalse );
}


/* */
gentity_t *
G_BuildHead ( gentity_t *ent )
{
	gentity_t		*head;
	orientation_t	or;								// DHM - Nerve
	gclient_t		*client;
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
	head = G_Spawn();

	// zinx - moved up here so realistic hitboxes can override
	VectorSet( head->r.mins, -6, -6, -2 );			// JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
	VectorSet( head->r.maxs, 6, 6, 10 );			// changed this z from 0 to 6
	if ( trap_GetTag( ent->s.number, 0, "tag_head", &or) )
	{
		G_SetOrigin( head, or.origin );
	}
	else if ( g_realhead.integer > 0 )
	{

		// zinx - realistic hitboxes
		grefEntity_t	refent;
		mdx_gentity_to_grefEntity( ent, &refent, ent->timeShiftTime ? ent->timeShiftTime : level.time );
		mdx_head_position( ent, &refent, or.origin );
		G_SetOrigin( head, or.origin );
		VectorSet( head->r.mins, -6, -6, -6 );
		VectorSet( head->r.maxs, 6, 6, 6 );
	}
	else
	{
		float	height, dest, leanheight;			// added leanheight , Jaquboss
		vec3_t	v, angles, forward, up, right;
		float	strafe;

		VectorSet(v, 0, 0, 0); // UQ1: Shut up compiler!

		if ( client->pers.cmd.rightmove > 0 )
		{
			if ( fabs( client->ps.velocity[0] / 25) > 10 || fabs( client->ps.velocity[1] / 25) > 10 )
			{
				strafe = (float) client->pers.cmd.rightmove / 11.0f;
			}
			else if ( fabs( client->ps.velocity[0] / 25) > 5 || fabs( client->ps.velocity[1] / 25) > 5 )
			{
				strafe = (float) client->pers.cmd.rightmove / 15.0f;
			}
			else if ( fabs( client->ps.velocity[0] / 25) > 2 || fabs( client->ps.velocity[1] / 25) > 2 )
			{
				strafe = (float) client->pers.cmd.rightmove / 20.0f;
			}
			else if ( fabs( client->ps.velocity[0] / 25) > 1 || fabs( client->ps.velocity[1] / 25) > 1 )
			{
				strafe = (float) client->pers.cmd.rightmove / 25.0f;
			}
			else if ( fabs( client->ps.velocity[0] / 25) >= 0.5f || fabs( client->ps.velocity[1] / 25) > 0.5f )
			{
				strafe = (float) client->pers.cmd.rightmove / 30.0f;
			}
			else
			{
				strafe = 0;
			}
		}
		else
		{
			if ( fabs( client->ps.velocity[0] / 25) > 10 || fabs( client->ps.velocity[1] / 25) > 10 )
			{
				strafe = (float) client->pers.cmd.rightmove / 10.0f;
			}
			else if ( fabs( client->ps.velocity[0] / 25) > 5 || fabs( client->ps.velocity[1] / 25) > 5 )
			{
				strafe = (float) client->pers.cmd.rightmove / 17.0f;
			}
			else if ( fabs( client->ps.velocity[0] / 25) > 2 || fabs( client->ps.velocity[1] / 25) > 2 )
			{
				strafe = (float) client->pers.cmd.rightmove / 22.5f;
			}
			else if ( fabs( client->ps.velocity[0] / 25) > 1 || fabs( client->ps.velocity[1] / 25) > 1 )
			{
				strafe = (float) client->pers.cmd.rightmove / 30.0f;
			}
			else if ( fabs( client->ps.velocity[0] / 25) > 0.5f || fabs( client->ps.velocity[1] / 25) > 0.5f )
			{
				strafe = (float) client->pers.cmd.rightmove / 35.0f;
			}
			else
			{
				strafe = 0;
			}
		}

		if ( client->ps.eFlags & EF_CROUCHING )
		{
			strafe *= 1.5f;
		}

		G_SetOrigin( head, ent->r.currentOrigin );
		if ( client->ps.eFlags & EF_PRONE )
		{
			height = client->ps.viewheight - 56;
		}
		else if ( client->ps.pm_flags & PMF_DUCKED )
		{											// closer fake offset for 'head' box when crouching
			height = client->ps.crouchViewHeight - 12;
		}
		else
		{
			height = client->ps.viewheight;
		}

		// NERVE - SMF - this matches more closely with WolfMP models
		VectorCopy( client->ps.viewangles, angles );
		if ( angles[PITCH] > 180 )
		{
			dest = ( -360 + angles[PITCH] ) * 0.75;
		}
		else
		{
			dest = angles[PITCH] * 0.75;
		}

		angles[PITCH] = dest;

		// tjw: the angles need to be clamped for prone
		//      or the head entity will be underground or
		//      far too tall
		if ( (client->ps.eFlags & EF_PRONE) )
		{
			if ( (client->ps.eFlags & EF_MOTION) )
			{
				angles[PITCH] = -15;
			}
			else
			{
				angles[PITCH] = -10;
			}
		}

		//angles[YAW] -=  client->ps.leanf*1.5f;
		AngleVectors( angles, forward, right, up );
		if ( client->ps.eFlags & EF_PRONE )
		{
			if ( (client->ps.eFlags & EF_MOTION) )
			{
				VectorScale( forward, 24, v );
			}
			else
			{
				VectorScale( forward, 28, v );
				VectorMA( v, 7, right, v );
			}
		}
		else if ( (client->ps.eFlags & EF_DEAD) )
		{

			// tjw: -28 is right for the head but it makes
			//      a small gap between the head and body
			//      that cannot be hit.  I think this is worse.
			//VectorScale(forward, -28, v);
			VectorScale( forward, -26, v );
			VectorMA( v, 5, right, v );
		}
		else
		{

			// tjw: when moving, the head is drawn
			//      down and forward
			if ( (client->ps.eFlags & EF_MOTION) )
			{
				VectorMA( v, strafe, right, v );
				if ( (client->ps.pm_flags & PMF_DUCKED) )
				{
					height += 2;
					VectorMA( v, 18, forward, v );
				}
				else
				{
					height -= 10;
					VectorMA( v, 10, forward, v );
				}
			}
			else
			{
				VectorScale( forward, 5, v );
			}

			VectorMA( v, 5, right, v );
		}

		VectorMA( v, 18, up, v );
		leanheight = ( client->ps.leanf / 1.5 );	// jaquboss , this is fine
		if ( leanheight > 0 )
		{
			leanheight *= -1.3;						// leaning height offset needs to be always down
		}
		else
		{
			leanheight *= 0.80;						// leaning left is not as far and this looks better
		}

		if ( client->ps.leanf < 0 )
		{
			VectorMA( v, client->ps.leanf * 0.85, right, v );
		}
		else
		{
			VectorMA( v, client->ps.leanf * 0.70, right, v );
		}

		VectorAdd( v, head->r.currentOrigin, head->r.currentOrigin );
		head->r.currentOrigin[2] += height / 2 + ( leanheight / 2 );	// jaquboss
		// -NERVE - SMF
	}

	VectorCopy( head->r.currentOrigin, head->s.origin );

	// tjw: this seems equiv to setting all to 0
	//VectorCopy (ent->r.currentAngles, head->s.angles);
	//VectorCopy (head->s.angles, head->s.apos.trBase);
	//VectorCopy (head->s.angles, head->s.apos.trDelta);
	// forty - realistic hitboxes
	//VectorSet (head->r.mins , -6, -6, -2); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
	//VectorSet (head->r.maxs , 6, 6, 10); // changed this z from 0 to 6
	head->clipmask = CONTENTS_SOLID;
	head->r.contents = CONTENTS_SOLID;
	head->parent = ent;
	head->s.eType = ET_TEMPHEAD;
	trap_LinkEntity( head );
	return ( head );
}


/* */
gentity_t *
G_BuildBody ( gentity_t *ent )
{
	gentity_t		*body;
	orientation_t	or; // DHM - Nerve
	grefEntity_t	refent;
	body = G_Spawn();
	mdx_gentity_to_grefEntity( ent, &refent, ent->timeShiftTime ? ent->timeShiftTime : level.time );
	mdx_body_position( ent, &refent, or.origin );
	if ( ent->s.eFlags & EF_PRONE )
	{
		or.origin[2] += 5;
	}

	G_SetOrigin( body, or.origin );
	VectorSet( body->r.mins, -10, -10, -12 );
	VectorSet( body->r.maxs, 10, 10, 3 );
	VectorCopy( body->r.currentOrigin, body->s.origin );
	body->clipmask = CONTENTS_SOLID;
	body->r.contents = CONTENTS_SOLID;
	body->parent = ent;
	body->s.eType = ET_TEMPBODY;
	trap_LinkEntity( body );
	return ( body );
}


/* */
gentity_t *
G_BuildGroin ( gentity_t *ent )
{
	gentity_t		*groin;
	orientation_t	or; // DHM - Nerve
	grefEntity_t	refent;
	groin = G_Spawn();
	mdx_gentity_to_grefEntity( ent, &refent, ent->timeShiftTime ? ent->timeShiftTime : level.time );
	mdx_groin_position( ent, &refent, or.origin );
	if ( ent->s.eFlags & EF_PRONE )
	{
		or.origin[2] += 5;
	}

	G_SetOrigin( groin, or.origin );
	if ( ent->s.eFlags & EF_PRONE )
	{
		VectorSet( groin->r.mins, -10, -10, -18 );
		VectorSet( groin->r.maxs, 10, 10, 2 );
	}
	else
	{
		VectorSet( groin->r.mins, -8, -8, -16 );
		VectorSet( groin->r.maxs, 8, 8, 1 );
	}

	VectorCopy( groin->r.currentOrigin, groin->s.origin );
	groin->clipmask = CONTENTS_SOLID;
	groin->r.contents = CONTENTS_SOLID;
	groin->parent = ent;
	groin->s.eType = ET_TEMPGROIN;
	trap_LinkEntity( groin );
	return ( groin );
}


/* */
gentity_t *
G_BuildLeg ( gentity_t *ent )
{
	gentity_t	*leg;
	vec3_t		flatforward, org;

	//orientation_t or;			// DHM - Nerve
#ifdef __NPC__
	if ( ent->client && !(ent->client->ps.eFlags & EF_PRONE) && g_realhead.integer == 0 )
	{
		return ( NULL );
	}

	if ( ent->NPC_client && !(ent->NPC_client->ps.eFlags & EF_PRONE) && g_realhead.integer == 0 )
	{
		return ( NULL );
	}

#else //!__NPC__
	if ( !(ent->client->ps.eFlags & EF_PRONE) && g_realhead.integer == 0 )
	{
		return ( NULL );
	}
#endif //__NPC__
	leg = G_Spawn();
	if ( g_realhead.integer > 0 )
	{

		// zinx - realistic hitboxes
		grefEntity_t	refent;
		mdx_gentity_to_grefEntity( ent, &refent, ent->timeShiftTime ? ent->timeShiftTime : level.time );
		mdx_legs_position( ent, &refent, org );
#ifdef __NPC__
		if ( ent->NPC_client )
		{
			org[2] += ent->NPC_client->pmext.proneLegsOffset;
		}
		else
		{
			org[2] += ent->client->pmext.proneLegsOffset;
		}

#else //!__NPC__
		org[2] += ent->client->pmext.proneLegsOffset;
#endif //__NPC__
		org[2] -= ( playerlegsProneMins[2] + playerlegsProneMaxs[2] ) * 0.5;
	}
	else
	{
#ifdef __NPC__
		if ( ent->NPC_client )
		{
			AngleVectors( ent->NPC_client->ps.viewangles, flatforward, NULL, NULL );
		}
		else
		{
			AngleVectors( ent->client->ps.viewangles, flatforward, NULL, NULL );
		}

#else //!__NPC__
		AngleVectors( ent->client->ps.viewangles, flatforward, NULL, NULL );
#endif //__NPC__
		flatforward[2] = 0;
		VectorNormalizeFast( flatforward );
#ifdef __NPC__
		if
		(
			(ent->client && (ent->client->ps.eFlags & EF_PRONE || ent->waterlevel >= 2)) ||
			(ent->NPC_client && (ent->NPC_client->ps.eFlags & EF_PRONE || ent->waterlevel >= 2))
		)
#else //!__NPC__
			if ( ent->client->ps.eFlags & EF_PRONE || ent->waterlevel >= 2 )
#endif //__NPC__
			{
				org[0] = ent->r.currentOrigin[0] + flatforward[0] * -32;
				org[1] = ent->r.currentOrigin[1] + flatforward[1] * -32;
			}
			else
			{
				org[0] = ent->r.currentOrigin[0] + flatforward[0] * 32;
				org[1] = ent->r.currentOrigin[1] + flatforward[1] * 32;
			}

#ifdef __NPC__
		if ( ent->NPC_client )
		{
			org[2] = ent->r.currentOrigin[2] + ent->NPC_client->pmext.proneLegsOffset;
		}
		else
		{
			org[2] = ent->r.currentOrigin[2] + ent->client->pmext.proneLegsOffset;
		}

#else //!__NPC__
		org[2] = ent->r.currentOrigin[2] + ent->client->pmext.proneLegsOffset;
#endif //__NPC__
	}

	G_SetOrigin( leg, org );
	VectorCopy( leg->r.currentOrigin, leg->s.origin );
#ifdef __NPC__
	if
	(
		(ent->client && (ent->client->ps.eFlags & EF_PRONE || ent->waterlevel >= 2)) ||
		(ent->NPC_client && (ent->NPC_client->ps.eFlags & EF_PRONE || ent->waterlevel >= 2))
	)
#else //!__NPC__
		if ( ent->client->ps.eFlags & EF_PRONE || ent->waterlevel >= 2 )
#endif //__NPC__
		{
			VectorCopy( playerlegsProneMins, leg->r.mins );
			VectorCopy( playerlegsProneMaxs, leg->r.maxs );
		}
		else
		{
			VectorSet( leg->r.mins, -10.5f, -10.5f, -22.f );
			if ( ent->s.eFlags & EF_CROUCHING )
			{
				VectorSet( leg->r.maxs, 10.5f, 10.5f, 4.4f );
			}
			else
			{
				VectorSet( leg->r.maxs, 10.5f, 10.5f, 9.4f );
			}
		}

	leg->clipmask = CONTENTS_SOLID;
	leg->r.contents = CONTENTS_SOLID;
	leg->parent = ent;
	leg->s.eType = ET_TEMPLEGS;
	trap_LinkEntity( leg );
	return ( leg );
}


/* */
qboolean
IsHeadShot ( gentity_t *targ, vec3_t dir, vec3_t point, int mod )
{
	gentity_t	*head;
	trace_t		tr;
	vec3_t		start, end;
	gentity_t	*traceEnt;

	// not a player or critter so bail
	if ( !(targ->client) )
	{
		return ( qfalse );
	}

	if ( targ->health <= 0 )
	{
		return ( qfalse );
	}

	if ( !IsHeadShotWeapon( mod) )
	{
		return ( qfalse );
	}

	head = G_BuildHead( targ );

	// trace another shot see if we hit the head
	VectorCopy( point, start );
	VectorMA( start, 64, dir, end );
	trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );
	traceEnt = &g_entities[tr.entityNum];
	if ( g_debugBullets.integer >= 3 )
	{								// show hit player head bb
		gentity_t	*tent;
		vec3_t		b1, b2;
		VectorCopy( head->r.currentOrigin, b1 );
		VectorCopy( head->r.currentOrigin, b2 );
		VectorAdd( b1, head->r.mins, b1 );
		VectorAdd( b2, head->r.maxs, b2 );
		tent = G_TempEntity( b1, EV_RAILTRAIL );
		VectorCopy( b2, tent->s.origin2 );
		tent->s.dmgFlags = 1;

		// show headshot trace
		// end the headshot trace at the head box if it hits
		if ( tr.fraction != 1 )
		{
			VectorMA( start, (tr.fraction * 64), dir, end );
		}

		tent = G_TempEntity( start, EV_RAILTRAIL );
		VectorCopy( end, tent->s.origin2 );
		tent->s.dmgFlags = 0;
	}

	if ( g_antilag.integer )
	{
		G_ReAdjustSingleClientPosition( targ );
	}

	G_FreeEntity( head );
	if ( traceEnt == head )
	{
		level.totalHeadshots++;		// NERVE - SMF
		return ( qtrue );
	}
	else
	{
		level.missedHeadshots++;	// NERVE - SMF
	}

	return ( qfalse );
}


/* */
qboolean
IsBodyShot ( gentity_t *targ, vec3_t dir, vec3_t point, int mod )
{
	gentity_t	*body;
	if ( !(targ->client) )
	{
		return ( qfalse );
	}

	if ( targ->health <= 0 )
	{
		return ( qfalse );
	}

	if ( !point )
	{
		return ( qfalse );
	}

	if ( !IsHeadShotWeapon( mod) )
	{
		return ( qfalse );
	}

	body = G_BuildBody( targ );
	if ( body )
	{
		gentity_t	*traceEnt;
		vec3_t		start, end;
		trace_t		tr;

		// trace another shot see if we hit the legs
		VectorCopy( point, start );
		VectorMA( start, 64, dir, end );
		trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );
		traceEnt = &g_entities[tr.entityNum];
		if ( g_debugBullets.integer >= 3 )
		{	// show hit player head bb
			gentity_t	*tent;
			vec3_t		b1, b2;
			VectorCopy( body->r.currentOrigin, b1 );
			VectorCopy( body->r.currentOrigin, b2 );
			VectorAdd( b1, body->r.mins, b1 );
			VectorAdd( b2, body->r.maxs, b2 );
			tent = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, tent->s.origin2 );
			tent->s.dmgFlags = 1;

			// show headshot trace
			// end the headshot trace at the head box if it hits
			if ( tr.fraction != 1 )
			{
				VectorMA( start, (tr.fraction * 64), dir, end );
			}

			tent = G_TempEntity( start, EV_RAILTRAIL );
			VectorCopy( end, tent->s.origin2 );
			tent->s.dmgFlags = 0;
		}

		if ( g_antilag.integer )
		{
			G_ReAdjustSingleClientPosition( targ );
		}

		G_FreeEntity( body );
		if ( traceEnt == body )
		{
			return ( qtrue );
		}
	}

	return ( qfalse );
}


/* */
qboolean
IsGroinShot ( gentity_t *targ, vec3_t dir, vec3_t point, int mod )
{
	gentity_t	*groin;
	if ( !(targ->client) )
	{
		return ( qfalse );
	}

	if ( targ->health <= 0 )
	{
		return ( qfalse );
	}

	if ( !point )
	{
		return ( qfalse );
	}

	if ( !IsHeadShotWeapon( mod) )
	{
		return ( qfalse );
	}

	groin = G_BuildGroin( targ );
	if ( groin )
	{
		gentity_t	*traceEnt;
		vec3_t		start, end;
		trace_t		tr;

		// trace another shot see if we hit the legs
		VectorCopy( point, start );
		VectorMA( start, 64, dir, end );
		trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );
		traceEnt = &g_entities[tr.entityNum];
		if ( g_debugBullets.integer >= 3 )
		{	// show hit player head bb
			gentity_t	*tent;
			vec3_t		b1, b2;
			VectorCopy( groin->r.currentOrigin, b1 );
			VectorCopy( groin->r.currentOrigin, b2 );
			VectorAdd( b1, groin->r.mins, b1 );
			VectorAdd( b2, groin->r.maxs, b2 );
			tent = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, tent->s.origin2 );
			tent->s.dmgFlags = 1;

			// show headshot trace
			// end the headshot trace at the head box if it hits
			if ( tr.fraction != 1 )
			{
				VectorMA( start, (tr.fraction * 64), dir, end );
			}

			tent = G_TempEntity( start, EV_RAILTRAIL );
			VectorCopy( end, tent->s.origin2 );
			tent->s.dmgFlags = 0;
		}

		if ( g_antilag.integer )
		{
			G_ReAdjustSingleClientPosition( groin );
		}

		G_FreeEntity( groin );
		if ( traceEnt == groin )
		{
			return ( qtrue );
		}
	}

	return ( qfalse );
}


/* */
qboolean
IsLegShot ( gentity_t *targ, vec3_t dir, vec3_t point, int mod )
{
	float		height;
	float		theight;
	gentity_t	*leg;
	if ( !(targ->client) )
	{
		return ( qfalse );
	}

	if ( targ->health <= 0 )
	{
		return ( qfalse );
	}

	if ( !point )
	{
		return ( qfalse );
	}

	if ( !IsHeadShotWeapon( mod) )
	{
		return ( qfalse );
	}

	leg = G_BuildLeg( targ );
	if ( leg )
	{
		gentity_t	*traceEnt;
		vec3_t		start, end;
		trace_t		tr;

		// trace another shot see if we hit the legs
		VectorCopy( point, start );
		VectorMA( start, 64, dir, end );
		trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );
		traceEnt = &g_entities[tr.entityNum];
		if ( g_debugBullets.integer >= 3 )
		{	// show hit player head bb
			gentity_t	*tent;
			vec3_t		b1, b2;
			VectorCopy( leg->r.currentOrigin, b1 );
			VectorCopy( leg->r.currentOrigin, b2 );
			VectorAdd( b1, leg->r.mins, b1 );
			VectorAdd( b2, leg->r.maxs, b2 );
			tent = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, tent->s.origin2 );
			tent->s.dmgFlags = 1;

			// show headshot trace
			// end the headshot trace at the head box if it hits
			if ( tr.fraction != 1 )
			{
				VectorMA( start, (tr.fraction * 64), dir, end );
			}

			tent = G_TempEntity( start, EV_RAILTRAIL );
			VectorCopy( end, tent->s.origin2 );
			tent->s.dmgFlags = 0;
		}

		if ( g_antilag.integer )
		{
			G_ReAdjustSingleClientPosition( targ );
		}

		G_FreeEntity( leg );
		if ( traceEnt == leg )
		{
			return ( qtrue );
		}
	}
	else
	{
		height = point[2] - targ->r.absmin[2];
		theight = targ->r.absmax[2] - targ->r.absmin[2];
		if ( height < (theight * 0.4f) )
		{
			return ( qtrue );
		}
	}

	return ( qfalse );
}


/* */
qboolean
IsArmShot ( gentity_t *targ, gentity_t *ent, vec3_t point, int mod )
{
	vec3_t	path, view;
	vec_t	dot;
	if ( !(targ->client) )
	{
		return ( qfalse );
	}

	if ( targ->health <= 0 )
	{
		return ( qfalse );
	}

	if ( !IsHeadShotWeapon( mod) )
	{
		return ( qfalse );
	}

	VectorSubtract( targ->client->ps.origin, point, path );
	path[2] = 0;
	AngleVectors( targ->client->ps.viewangles, view, NULL, NULL );
	view[2] = 0;
	VectorNormalize( path );
	dot = DotProduct( path, view );
	if ( dot > 0.4f || dot < -0.75f )
	{
		return ( qfalse );
	}

	return ( qtrue );
}


/* */
qboolean
checkLocation ( gentity_t *targ, vec3_t dir, vec3_t point, int mod )
{
	if ( dir )
	{
		VectorNormalize( dir );
	}

	if ( g_realhead.integer < 2 )
	{
		return ( qtrue );	// check whole box as damaging
	}

	if ( IsHeadShot( targ, dir, point, mod) )
	{
		return ( qtrue );
	}
	else if ( IsBodyShot( targ, dir, point, mod) )
	{
		return ( qtrue );
	}
	else if ( IsGroinShot( targ, dir, point, mod) )
	{
		return ( qtrue );
	}
	else if ( IsLegShot( targ, dir, point, mod) )
	{
		return ( qtrue );
	}

	return ( qfalse );
}

/*
============
G_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
	DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
	DAMAGE_NO_ARMOR			armor does not protect from this damage
	DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
	DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/
#ifdef BONE_HITTESTS


/* */
qboolean
G_PlayerBulletDamage
(
	gentity_t	*targ,
	gentity_t	*inflictor,
	gentity_t	*attacker,
	vec3_t		start,
	vec3_t		end,
	vec3_t		dir,
	int			damage,
	int			dflags,
	int			mod
)
{
	/*
	// Kamikazee: commented out to see if it gets used
	//	If not, don't bother with this code
	grefEntity_t			refent;
	int						hit_type = MDX_NONE;
	animScriptImpactPoint_t impactpoint = IMPACTPOINT_UNUSED;
	float					take;
	qboolean				wasAlive = qfalse;
	hitRegion_t				hr = HR_NUM_HITREGIONS;
	vec_t					*fraction = 0;
	if ( targ->health > 0 )
	{
		wasAlive = qtrue;
	}

	mdx_gentity_to_grefEntity( targ, &refent, targ->timeShiftTime ? targ->timeShiftTime : level.time );
	if ( mdx_hit_test( start, end, targ, &refent, &hit_type, fraction, &impactpoint) )
	{
		take = damage;
		if ( hit_type == MDX_NONE )
		{
			return ( qfalse );
		}
		else if ( hit_type == MDX_GUN )
		{
			return ( qfalse );
		}
		else if ( hit_type == MDX_HEAD )
		{
			hr = HR_HEAD;
			if ( attacker->client->sess.sessionTeam != targ->client->sess.sessionTeam )
			{
				attacker->client->ps.persistant[PERS_HEADSHOTS] += 1;
			}

			if ( take * (g_headshot_ratio.integer / 100) < g_headshot_min.integer )
			{												// head shots, all weapons, do minimum 50 points damage
				take = g_headshot_min.integer;
			}
			else
			{
				take *= ( g_headshot_ratio.integer / 100 ); // sniper rifles can do full-kill (and knock into limbo)
			}

			if ( dflags & DAMAGE_DISTANCEFALLOFF )
			{
				vec_t	dist;
				vec3_t	shotvec;
				float	scale;
				VectorSubtract( end, muzzleTrace, shotvec );
				dist = VectorLength( shotvec );

				// ~~~---______
				// zinx - start at 100% at 1500 units (and before),
				// and go to 20% at 2500 units (and after)
				// 1500 to 2500 -> 0.0 to 1.0
				scale = ( dist - 1500.f ) / ( 2500.f - 1500.f );

				// 0.0 to 1.0 -> 0.0 to 0.8
				scale *= 0.8f;

				// 0.0 to 0.8 -> 1.0 to 0.2
				scale = 1.0f - scale;

				// And, finally, cap it.
				if ( scale > 1.0f )
				{
					scale = 1.0f;
				}
				else if ( scale < 0.2f )
				{
					scale = 0.2f;
				}

				take *= scale;
			}

			if ( !(targ->client->ps.eFlags & EF_HEADSHOT) )
			{
				// Helmet gives us some protection from regular weapons, but not from snipers
				if
					(
						mod != MOD_K43_SCOPE
						&& mod != MOD_GARAND_SCOPE
					)
				{
					take *= g_HelmetPiercingFactor.value;
				}

				if( g_PopHelmets.integer != 0 )
				{
					// Only toss hat when enabled. Check when it should pop
					int rnd = rand() % 100;
					if ( rnd < g_helmet_popchance.integer )
					{
							// Pop!
							G_AddEvent( targ, EV_LOSE_HAT, DirToByte( dir) );
							targ->client->ps.eFlags |= EF_HEADSHOT;
					}
				}
			}
		}
		else if
			(
				hit_type == MDX_TORSO ||
				hit_type == MDX_ARM_L ||
				hit_type == MDX_ARM_R ||
				hit_type == MDX_LEG_L ||
				hit_type == MDX_LEG_R
			)
		{
			hr = HR_BODY;
			if ( dflags & DAMAGE_DISTANCEFALLOFF )
			{
				vec_t	dist;
				vec3_t	shotvec;
				float	scale;
				VectorSubtract( end, muzzleTrace, shotvec );
				dist = VectorLength( shotvec );

				// ~~~---______
				// zinx - start at 100% at 1500 units (and before),
				// and go to 20% at 2500 units (and after)
				// 1500 to 2500 -> 0.0 to 1.0
				scale = ( dist - 1500.f ) / ( 2500.f - 1500.f );

				// 0.0 to 1.0 -> 0.0 to 0.8
				scale *= 0.8f;

				// 0.0 to 0.8 -> 1.0 to 0.2
				scale = 1.0f - scale;

				// And, finally, cap it.
				if ( scale > 1.0f )
				{
					scale = 1.0f;
				}
				else if ( scale < 0.2f )
				{
					scale = 0.2f;
				}

				take *= scale;
			}

			if ( hit_type == MDX_ARM_L || hit_type == MDX_ARM_R )
			{
				take *= 0.75f;
				hr = HR_ARMS;
			}
			else if ( hit_type == MDX_LEG_L || hit_type == MDX_LEG_R )
			{
				take *= 0.5f;
				hr = HR_LEGS;
			}
		}

		if ( take )
		{																//1
			targ->health -= take;
			if ( targ->health <= GIB_HEALTH )
			{
				targ->health = GIB_HEALTH + 1;
			}

			targ->s.dl_intensity = targ->health;

			// JPW NERVE overcome previous chunk of code for making grenades work again
			// Arnout: only player entities! messes up ents like func_constructibles and func_explosives otherwise
			if ( ((targ->s.number < MAX_CLIENTS) && (take > 190)) && !(targ->r.svFlags & SVF_POW) )
			{
				targ->health = GIB_HEALTH - 1;
			}

			if ( targ->health <= 0 )
			{															//2
				if ( !wasAlive )
				{														//3
					targ->flags |= FL_NO_KNOCKBACK;

					// OSP - special hack to not count attempts for body gibbage
					if ( targ->client->ps.pm_type == PM_DEAD )
					{
						G_addStats( targ, attacker, take, mod );
					}

					if ( g_gametype.integer == GT_WOLF_DM && (targ->health > GIB_HEALTH) )
					{
						return;
					}

					if ( (targ->health < FORCE_LIMBO_HEALTH) && (targ->health > GIB_HEALTH) )
					{
						limbo( targ, qtrue );
					}

					// xkan, 1/13/2003 - record the time we died.
				}
				else
				{														//3 4
					targ->sound2to1 = mod;
					targ->sound2to3 = ( dflags & DAMAGE_RADIUS ) ? 1 : 0;
					if ( G_GetTeamFromEntity( inflictor) != G_GetTeamFromEntity( targ) )
					{
						G_AddKillSkillPoints( attacker, mod, hr, (dflags & DAMAGE_RADIUS) );
					}

					if ( targ->health < -999 )
					{
						targ->health = -999;
					}

					targ->enemy = attacker;
					targ->deathType = mod;
					if ( targ->die )
					{

						// Kill the entity.  Note that this funtion can set ->die to another
						// function pointer, so that next time die is applied to the dead body.
						if ( hit_type == MDX_HEAD )
						{
							targ->isHeadshot = qtrue;
						}
						else
						{
							targ->isHeadshot = qfalse;
						}

						targ->die( targ, inflictor, attacker, take, mod );
					}

					// if we freed ourselves in death function
					if ( !targ->inuse )
					{
						return ( qfalse );
					}
				}														//4
			}
			else
			{															//2 5
				// OSP - update weapon/dmg stats
				G_addStats( targ, attacker, take, mod );

				// OSP
			}															//5
		}																// 1

		targ->client->ps.stats[STAT_HEALTH] = targ->health;
		BG_UpdateConditionValue( targ->client->ps.clientNum, ANIM_COND_IMPACTPOINT, impactpoint, qtrue );
		if ( !targ->client->ps.eFlags & EF_PRONE )
		{
			BG_AnimScriptEvent( &targ->client->ps, targ->client->pers.character->animModelInfo, ANIM_ET_PAIN, qtrue,
								qfalse );
		}
	}
*/
	return ( qtrue );
}
#endif

/* */
void
G_Damage
(
	gentity_t	*targ,
	gentity_t	*inflictor,
	gentity_t	*attacker,
	vec3_t		dir,
	vec3_t		point,
	int			damage,
	int			dflags,
	int			mod
)
{
	gclient_t	*client;
	int			take;
	int			save;
	int			knockback;
	qboolean	headShot;
//	qboolean	groinShot;
//	qboolean	legShot;
//	qboolean	armShot;
	qboolean	wasAlive;
	qboolean	tank = qfalse;
	int			rnd;
	hitRegion_t hr = HR_NUM_HITREGIONS;

	if ( !targ->takedamage )
	{
		return;
	}

#ifdef SAVEGAME_SUPPORT
	if ( g_gametype.integer == GT_SINGLE_PLAYER && (g_reloading.integer || saveGamePending) )
	{
		return;
	}
#endif // SAVEGAME_SUPPORT

	// the intermission has allready been qualified for, so don't
	// allow any extra scoring
	if ( level.intermissionQueued || (g_gamestate.integer != GS_PLAYING && match_warmupDamage.integer == 0) )
	{
		return;
	}

	if ( !inflictor )
	{
		inflictor = &g_entities[ENTITYNUM_WORLD];
	}

	if ( !attacker )
	{
		attacker = &g_entities[ENTITYNUM_WORLD];
	}

	if
	(
		(targ->client && targ->client->ps.eFlags & EF_TANK) ||
		!Q_stricmp( targ->classname, "panzertank") ||
		!Q_stricmp( targ->classname, "churchilltank")
	)
	{
		tank = qtrue;
	}

	if ( tank && !G_WeaponIsExplosive( mod) )
	{
		return;
	}

	if ( tank )
	{
		switch ( mod )
		{
			case MOD_GRENADE_LAUNCHER:
			case MOD_GRENADE_PINEAPPLE:
			case MOD_MORTAR:
			case MOD_GPG40:
			case MOD_M7:
				damage *= 0.1;
				break;
			case MOD_LANDMINE:
			case MOD_PANZERFAUST:
			case MOD_TANKSHELL:
				damage *= 0.2;
				break;
			case MOD_AIRSTRIKE:
			case MOD_ARTY:
				damage *= 0.05;														// so players will not bother as much to spam airstrikes;
				break;
			case MOD_DYNAMITE:
				damage *= 0.5;
				break;
			default:
				damage *= 0.05;
				break;
		}
	}

	// Arnout: invisible entities can't be damaged
	if ( targ->entstate == STATE_INVISIBLE || targ->entstate == STATE_UNDERCONSTRUCTION )
	{
		return;
	}

#ifdef __BOT__
	if ( targ->client && targ->r.svFlags & SVF_BOT )
	{																				// We want bots to react to being shot by someone...
		if ( attacker->client )
		{
			if ( !OnSameTeam( targ, attacker) )
			{
				targ->client->attacker = attacker;

				/*				//if (!targ->enemy)
				{
					targ->enemy = attacker;
					AIMOD_InformTeamOfEnemy(targ);
				}*/
			}
		}

#ifdef __NPC__
		else if ( attacker->NPC_client )
		{
			if ( !OnSameTeam( targ, attacker) )
			{
				targ->client->attacker = attacker;

				//if (!targ->enemy)
				/*				{
					targ->enemy = attacker;
					AIMOD_InformTeamOfEnemy(targ);
				}*/
			}
		}
#endif //__NPC__
	}
#endif //__BOT__
#ifdef __NPC__
	if ( attacker->NPC_client )
	{
		if ( attacker->skillchoice == 5 )
		{
			damage *= 1.0f;
		}
		else if ( attacker->skillchoice == 4 )
		{
			damage *= 0.7f;
		}
		else if ( attacker->skillchoice == 3 )
		{
			damage *= 0.5f;
		}
		else if ( attacker->skillchoice == 2 )
		{
			damage *= 0.4f;
		}
		else if ( attacker->skillchoice == 1 )
		{
			damage *= 0.3f;
		}
		else
		{
			damage *= 0.2f;
		}
	}

	if ( targ->NPC_client )
	{																				// We want NPCs to react to being shot by someone...
		if ( attacker->client )
		{
			if ( !OnSameTeam( targ, attacker) )
			{
				targ->enemy = attacker;
			}
		}
		else if ( attacker->NPC_client )
		{
			if ( !OnSameTeam( targ, attacker) )
			{
				targ->NPC_client->attacker = attacker;

				//if (!targ->enemy)
				{
					targ->enemy = attacker;

					//AIMOD_InformTeamOfEnemy(targ);
				}
			}
		}
	}
#endif //__NPC__

	// xkan, 12/23/2002 - was the bot alive before applying any damage?
	wasAlive = ( targ->health > 0 );

	// Arnout: combatstate
	if ( targ->client && attacker && attacker->client && attacker != targ )
	{
		if ( g_gamestate.integer == GS_PLAYING )
		{
			if ( !OnSameTeam( attacker, targ) || g_gametype.integer == GT_WOLF_DM )
			{
				targ->client->combatState |= ( 1 << COMBATSTATE_DAMAGERECEIVED );
				attacker->client->combatState |= ( 1 << COMBATSTATE_DAMAGEDEALT );
			}
		}
	}

	// JPW NERVE
	if ( (targ->waterlevel >= 3) && (mod == MOD_FLAMETHROWER) )
	{
		return;
	}

	// jpw
	// shootable doors / buttons don't actually have any health
	if ( targ->s.eType == ET_MOVER && !(targ->isProp) && !targ->scriptName )
	{
		if ( targ->use && targ->moverState == MOVER_POS1 )
		{
			G_UseEntity( targ, inflictor, attacker );
		}

		return;
	}

	if ( targ->client && !attacker->client )
	{																				// set default Animation conditions..
		BG_UpdateConditionValue( targ->client->ps.clientNum, ANIM_COND_ENEMY_WEAPON, WP_NONE, qtrue );
		BG_UpdateConditionValue( targ->client->ps.clientNum, ANIM_COND_ENEMY_POSITION, POSITION_UNUSED, qtrue );
	}

	if ( targ->client && attacker->client )
	{
		vec3_t	pforward, eforward;
		AngleVectors( attacker->client->ps.viewangles, pforward, NULL, NULL );
		AngleVectors( targ->client->ps.viewangles, eforward, NULL, NULL );
		BG_UpdateConditionValue( targ->client->ps.clientNum, ANIM_COND_ENEMY_WEAPON, attacker->client->ps.weapon, qtrue );
		if ( mod == MOD_FLAMETHROWER )
		{
			targ->client->ps.stats[STAT_DMG] = 5;									// play pain animation..
		}

		if ( DotProduct( eforward, pforward) > 0.6f )
		{																			// from behind(-ish)
			BG_UpdateConditionValue( targ->client->ps.clientNum, ANIM_COND_ENEMY_POSITION, POSITION_BEHIND, qtrue );
		}
		else
		{
			BG_UpdateConditionValue( targ->client->ps.clientNum, ANIM_COND_ENEMY_POSITION, POSITION_UNUSED, qtrue );
		}

		/* else if ( DotProduct(eforward, pforward ) <= 1.0f){
			BG_UpdateConditionValue( targ->client->ps.clientNum, ANIM_COND_ENEMY_POSITION, POSITION_INFRONT, qtrue );
			// POSITION_RIGHT,
			// POSITION_LEFT,
		}*/
	}

	// TAT 11/22/2002
	//		In the old code, this check wasn't done for props, so I put that check back in to make props_statue properly work	
	// 4 means destructible
	if ( targ->s.eType == ET_MOVER && (targ->spawnflags & 4) && !targ->isProp )
	{
		if ( !G_WeaponIsExplosive( mod) )
		{
			return;
		}

		// check for team
		if ( G_GetTeamFromEntity( inflictor) == G_GetTeamFromEntity( targ) )
		{
			return;
		}
	}
	else if ( targ->s.eType == ET_EXPLOSIVE )
	{
		if ( targ->constructibleStats.weaponclass == 3 && mod != MOD_TANKSHELL )
		{																			// can be harmed only by a tank
			return;
		}

		if ( targ->parent && G_GetWeaponClassForMOD( mod) == 2 )
		{
			return;
		}

		if ( G_GetTeamFromEntity( inflictor) == G_GetTeamFromEntity( targ) )
		{
			return;
		}

		if
		(
			G_GetWeaponClassForMOD( mod) < targ->constructibleStats.weaponclass ||
			(targ->constructibleStats.weaponclass == 3 && mod == MOD_TANKSHELL)
		)
		{
			return;
		}
	}
	else if ( targ->s.eType == ET_MISSILE && targ->methodOfDeath == MOD_LANDMINE )
	{
		if ( targ->s.modelindex2 )
		{
			if ( G_WeaponIsExplosive( mod) )
			{
				mapEntityData_t *mEnt;
				if ( (mEnt = G_FindMapEntityData( &mapEntityData[0], targ - g_entities)) != NULL )
				{
					G_FreeMapEntityData( &mapEntityData[0], mEnt );
				}

				if ( (mEnt = G_FindMapEntityData( &mapEntityData[1], targ - g_entities)) != NULL )
				{
					G_FreeMapEntityData( &mapEntityData[1], mEnt );
				}

				if ( attacker && attacker->client )
				{
					AddScore( attacker, 1 );
				}

				G_ExplodeMissile( targ );
			}
		}

		return;
	}
	else if ( targ->s.eType == ET_CONSTRUCTIBLE )
	{
		if ( G_GetTeamFromEntity( inflictor) == G_GetTeamFromEntity( targ) )
		{
			return;
		}

		if ( G_GetWeaponClassForMOD( mod) < targ->constructibleStats.weaponclass )
		{
			return;
		}

		//bani - fix #238
		if ( mod == MOD_DYNAMITE )
		{
			if ( !(inflictor->etpro_misc_1 & 1) )
			{
				return;
			}
		}
	}

	client = targ->client;
	if ( client )
	{
		if ( client->noclip || client->ps.powerups[PW_INVULNERABLE] )
		{
			return;
		}
	}

	// check for godmode
	if ( targ->flags & FL_GODMODE )
	{
		return;
	}

	if ( !dir )
	{
		dflags |= DAMAGE_NO_KNOCKBACK;
	}
	else
	{
		VectorNormalize( dir );
	}

	knockback = damage;
	if ( knockback > 200 )
	{
		knockback = 200;
	}

	if ( targ->flags & FL_NO_KNOCKBACK )
	{
		knockback = 0;
	}

	if ( dflags & DAMAGE_NO_KNOCKBACK )
	{
		knockback = 0;
	}
	else if ( dflags & DAMAGE_HALF_KNOCKBACK )
	{
		knockback *= 0.5f;
	}

	// ydnar: set weapons means less knockback
	if
	(
		client &&
		(
			client->ps.weapon == WP_MORTAR_SET ||
			client->ps.weapon == WP_MOBILE_MG42_SET ||
			client->ps.weapon == WP_30CAL_SET
		)
	)
	{
		knockback *= 0.5;
	}

	if ( targ->client && g_friendlyFire.integer && OnSameTeam( targ, attacker) && g_gametype.integer != GT_WOLF_DM )
	{
		knockback = 0;
	}

	// figure momentum add, even if the damage won't be taken
	if ( knockback && targ->client )
	{
		vec3_t	kvel;
		float	mass;
		mass = 200;
		VectorScale( dir, g_knockback.value * (float) knockback / mass, kvel );
		VectorAdd( targ->client->ps.velocity, kvel, targ->client->ps.velocity );
		if
		(
			targ == attacker &&
			!(
				mod != MOD_ROCKET &&
				mod != MOD_GRENADE &&
				mod != MOD_GRENADE_LAUNCHER &&
				mod != MOD_DYNAMITE &&
				mod != MOD_GPG40 &&
				mod != MOD_M7 &&
				mod != MOD_LANDMINE
			)
		)
		{
			targ->client->ps.velocity[2] *= 0.25;
		}

		// set the timer so that the other client can't cancel
		// out the movement immediately
		if ( !targ->client->ps.pm_time )
		{
			int t;
			t = knockback * 2;
			if ( t < 50 )
			{
				t = 50;
			}

			if ( t > 200 )
			{
				t = 200;
			}

			targ->client->ps.pm_time = t;
			targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		}
	}

	// check for completely getting out of the damage
	if ( !(dflags & DAMAGE_NO_PROTECTION) )
	{

		// if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
		// if the attacker was on the same team
		if ( targ != attacker && OnSameTeam( targ, attacker) && g_gametype.integer != GT_WOLF_DM )
		{
			if ( (g_gamestate.integer != GS_PLAYING && match_warmupDamage.integer == 1) )
			{
				return;
			}
			else if ( !g_friendlyFire.integer )
			{

				// record "fake" pain - although the bot is not really hurt, his feeling has been hurt :-)
				// well at least he wants to shout "watch your fire".
#ifndef NO_BOT_SUPPORT
				if ( targ->s.number < level.maxclients && targ->r.svFlags & SVF_BOT )
				{
					BotRecordPain( targ->s.number, attacker->s.number, mod );
				}
#endif
				return;
			}
		}
	}

	// add to the attacker's hit counter
	if ( attacker->client && targ != attacker && targ->health > 0 && targ->client )
	{
		if ( OnSameTeam( targ, attacker) && g_gametype.integer != GT_WOLF_DM )
		{
			attacker->client->ps.persistant[PERS_HITS] -= damage;
		}
		else
		{
			attacker->client->ps.persistant[PERS_HITS] += damage;
		}
	}

	if ( damage < 1 )
	{
		damage = 1;
	}

	take = damage;
	take *= (float) ( g_damage_multi.value / 100 );
	save = 0;

	// adrenaline junkie!
	if ( targ->client && targ->client->ps.powerups[PW_ADRENALINE] )
	{
		take *= .5f;
	}

	// save some from flak jacket
	if
	(
		targ->client &&
		targ->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 4 &&
		targ->client->sess.playerType == PC_ENGINEER &&
		!(targ->client->ps.eFlags & EF_TANK)
	)
	{
		if
		(
			mod == MOD_GRENADE ||
			mod == MOD_GRENADE_LAUNCHER ||
			mod == MOD_ROCKET ||
			mod == MOD_GRENADE_PINEAPPLE ||
			mod == MOD_MAPMORTAR ||
			mod == MOD_MAPMORTAR_SPLASH ||
			mod == MOD_EXPLOSIVE ||
			mod == MOD_LANDMINE ||
			mod == MOD_GPG40 ||
			mod == MOD_M7 ||
			mod == MOD_SATCHEL ||
			mod == MOD_ARTY ||
			mod == MOD_AIRSTRIKE ||
			mod == MOD_DYNAMITE ||
			mod == MOD_MORTAR ||
			mod == MOD_PANZERFAUST ||
			mod == MOD_MAPMORTAR
		)
		{
			take -= take * .5f;
		}
	}

	rnd = rand() % 100;
	headShot = IsHeadShot( targ, dir, point, mod );
	if ( headShot )
	{
		if ( mod == MOD_K43_SCOPE || /*Jaquboss , this is to have sniper strong headshots*/ mod == MOD_GARAND_SCOPE )
		{
			take *= 2.0;
		}

		if ( mod == MOD_FG42SCOPE )
		{
			take *= 1.5;
		}

		if ( attacker->client && targ->client && attacker->client->sess.sessionTeam != targ->client->sess.sessionTeam )
		{
			attacker->client->ps.persistant[PERS_HEADSHOTS] += 1;
		}

		if ( take * (g_headshot_ratio.integer / 100) < g_headshot_min.integer )
		{																			// head shots, all weapons, do minimum 50 points damage
			take = g_headshot_min.integer;
		}
		else
		{
			take *= ( g_headshot_ratio.integer / 100 );								// sniper rifles can do full-kill (and knock into limbo)
		}

		if ( dflags & DAMAGE_DISTANCEFALLOFF )
		{
			vec_t	dist;
			vec3_t	shotvec;
			float	scale;
			VectorSubtract( point, muzzleTrace, shotvec );
			dist = VectorLength( shotvec );
#if DO_BROKEN_DISTANCEFALLOFF

			// ~~~___---___
			if ( dist > 1500.f )
			{
				if ( dist > 2500.f )
				{
					take *= 0.2f;
				}
				else
				{
					float	scale = 1.f - 0.2f * ( 1000.f / (dist - 1000.f) );
					take *= scale;
				}
			}

#else

			// ~~~---______
			// zinx - start at 100% at 1500 units (and before),
			// and go to 20% at 2500 units (and after)
			// 1500 to 2500 -> 0.0 to 1.0
			scale = ( dist - 1500.f ) / ( 2500.f - 1500.f );

			// 0.0 to 1.0 -> 0.0 to 0.8
			scale *= 0.8f;

			// 0.0 to 0.8 -> 1.0 to 0.2
			scale = 1.0f - scale;

			// And, finally, cap it.
			if ( scale > 1.0f )
			{
				scale = 1.0f;
			}
			else if ( scale < 0.2f )
			{
				scale = 0.2f;
			}

			take *= scale;
#endif
		}

/*
		// Kamikazee: What the hell is this doing here?
		// Legshot calculations within the headshot part???
		if ( g_realhead.integer > 1 )
		{
			groinShot = IsGroinShot( targ, dir, point, mod );
			if ( groinShot )
			{
				take *= 0.95f;
			}
		}

		legShot = IsLegShot( targ, dir, point, mod );
		if ( legShot && g_realhead.integer > 1 )
		{
			take *= .75f;
		}

		armShot = IsArmShot( targ, attacker, point, mod );
		if ( armShot )
		{
			take *= .75f;															// not so painfull
			if ( targ->client && !(targ->client->ps.eFlags & EF_ARMSHOT) )
			{																		// but it will be much worse now!
				targ->client->ps.eFlags |= EF_ARMSHOT;
			}
		}*/

		if ( targ->client && !(targ->client->ps.eFlags & EF_HEADSHOT) )
		{
			// Helmet gives us some protection from regular weapons, but not from snipers
			if ( mod != MOD_K43_SCOPE && mod != MOD_GARAND_SCOPE )
			{
				take *= g_HelmetPiercingFactor.value;
			}

			// Only toss hat when enabled
			if( g_PopHelmets.integer != 0 )
			{
				int rnd;
				// Check if helmet should pop.
				rnd = rand() % 100;
				if ( rnd < g_helmet_popchance.integer )
				{
					// Pop!
					G_AddEvent( targ, EV_LOSE_HAT, DirToByte( dir) );
					targ->client->ps.eFlags |= EF_HEADSHOT;
				}
			}
		}

		// OSP - Record the headshot
		if ( client && attacker && attacker->client
#ifndef DEBUG_STATS
		&& attacker->client->sess.sessionTeam != targ->client->sess.sessionTeam
#endif
		)
		{
			G_addStatsHeadShot( attacker, mod );
		}

		if ( g_debugBullets.integer )
		{
			trap_SendServerCommand( attacker - g_entities, "print \"Head Shot\n\"\n" );
		}

		G_LogRegionHit( attacker, HR_HEAD );
		hr = HR_HEAD;
	}
	else if ( IsLegShot( targ, dir, point, mod) )
	{
		G_LogRegionHit( attacker, HR_LEGS );
		hr = HR_LEGS;
		if ( g_debugBullets.integer )
		{
			trap_SendServerCommand( attacker - g_entities, "print \"Leg Shot\n\"\n" );
		}
	}
	else if ( g_realhead.integer > 1 && IsGroinShot( targ, dir, point, mod) )
	{
		G_LogRegionHit( attacker, HR_LEGS );
		hr = HR_GROIN;
		if ( g_debugBullets.integer )
		{
			trap_SendServerCommand( attacker - g_entities, "print \"Leg Shot\n\"\n" );
		}
	}
	else if ( IsArmShot( targ, attacker, point, mod) )
	{
		G_LogRegionHit( attacker, HR_ARMS );
		hr = HR_ARMS;
		if ( g_debugBullets.integer )
		{
			trap_SendServerCommand( attacker - g_entities, "print \"Arm Shot\n\"\n" );
		}
	}
	else if ( targ->client && targ->health > 0 && IsHeadShotWeapon( mod) )
	{
		G_LogRegionHit( attacker, HR_BODY );
		hr = HR_BODY;
		if ( g_debugBullets.integer )
		{
			trap_SendServerCommand( attacker - g_entities, "print \"Body Shot\n\"\n" );
		}
	}

#ifndef DEBUG_STATS
	if ( g_debugDamage.integer )
#endif
	{
		G_Printf( "client:%i health:%i damage:%i mod:%s\n", targ->s.number, targ->health, take, modNames[mod] );
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if ( client )
	{
		if ( attacker )
		{
			client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
		}
		else
		{
			client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
		}

		client->ps.stats[STAT_DMG] = 5;
		if ( hr == HR_GROIN )
		{
			client->ps.stats[STAT_DMG] = 6;

			//client->damage_blood += 100;
		}
		else if ( hr == HR_HEAD )
		{
			client->ps.stats[STAT_DMG] = 4;

			//client->damage_blood += 100;
		}
		else if ( hr == HR_BODY )
		{
			client->ps.stats[STAT_DMG] = 3;

			//client->damage_blood += 50;
		}
		else if ( hr == HR_ARMS )
		{
			client->ps.stats[STAT_DMG] = 2;

			//client->damage_blood += 30;
		}
		else if ( hr == HR_LEGS )
		{
			client->ps.stats[STAT_DMG] = 1;

			//client->damage_blood += 15;
		}

		client->damage_blood += take;
		client->damage_knockback += knockback;
		if ( dir )
		{
			VectorCopy( dir, client->damage_from );
			client->damage_fromWorld = qfalse;
		}
		else
		{
			VectorCopy( targ->r.currentOrigin, client->damage_from );
			client->damage_fromWorld = qtrue;
		}
	}

	// See if it's the player hurting the emeny flag carrier
	if ( targ->client )
	{

		// set the last client who damaged the target
		targ->client->lasthurt_client = attacker->s.number;
		targ->client->lasthurt_mod = mod;
	}

	// do the damage
	if ( take )
	{
		targ->health -= take;

		// Gordon: don't ever gib POWS
		if ( (targ->health <= 0) && (targ->r.svFlags & SVF_POW) )
		{
			targ->health = -1;
		}

		// Ridah, can't gib with bullet weapons (except VENOM)
		// Arnout: attacker == inflictor can happen in other cases as well! (movers trying to gib things)
		if ( targ->health <= GIB_HEALTH )
		{
			if ( !G_WeaponIsExplosive( mod) )
			{
				targ->health = GIB_HEALTH + 1;
			}
		}

		if ( targ->s.eType == ET_PLAYER || targ->s.eType == ET_CORPSE )
		{
			targ->s.dl_intensity = targ->health;
		}

		// JPW NERVE overcome previous chunk of code for making grenades work again
		// Arnout: only player entities! messes up ents like func_constructibles and func_explosives otherwise
		if ( ((targ->s.number < MAX_CLIENTS) && (take > 190)) && !(targ->r.svFlags & SVF_POW) )
		{
			targ->health = GIB_HEALTH - 1;
		}

		if ( targ->s.eType == ET_MOVER && !Q_stricmp( targ->classname, "script_mover") )
		{
			targ->s.dl_intensity = 255.f * ( targ->health / (float) targ->count );	// send it to the client
		}

		//G_Printf("health at: %d\n", targ->health);
		if ( targ->health <= 0 )
		{
			if ( client && !wasAlive )
			{
				targ->flags |= FL_NO_KNOCKBACK;

				// OSP - special hack to not count attempts for body gibbage
				if ( targ->client && targ->client->ps.pm_type == PM_DEAD )
				{
					G_addStats( targ, attacker, take, mod );
				}

				if ( g_gametype.integer == GT_WOLF_DM && (targ->health > GIB_HEALTH) )
				{
					return;
				}

				if ( (targ->health < FORCE_LIMBO_HEALTH) && (targ->health > GIB_HEALTH) )
				{
					limbo( targ, qtrue );
				}

				// xkan, 1/13/2003 - record the time we died.
				if ( !client->deathTime )
				{
					client->deathTime = level.time;
				}

				//bani - #389
				if ( targ->health <= GIB_HEALTH )
				{
					GibEntity( targ, 0 );
				}
			}
			else
			{
				targ->sound1to2 = hr;
				targ->sound2to1 = mod;
				targ->sound2to3 = ( dflags & DAMAGE_RADIUS ) ? 1 : 0;
				if ( client )
				{
					if ( G_GetTeamFromEntity( inflictor) != G_GetTeamFromEntity( targ) )
					{
						G_AddKillSkillPoints( attacker, mod, hr, (dflags & DAMAGE_RADIUS) );
					}
				}

				if ( targ->health < -999 )
				{
					targ->health = -999;
				}

				targ->enemy = attacker;
				targ->deathType = mod;

				// Ridah, mg42 doesn't have die func (FIXME)
				if ( targ->die )
				{

					// Kill the entity.  Note that this funtion can set ->die to another
					// function pointer, so that next time die is applied to the dead body.
					if ( targ->client )
					{
						if ( headShot )
						{
							targ->isHeadshot = qtrue;
						}
						else
						{
							targ->isHeadshot = qfalse;
						}

						if ( dflags & DAMAGE_BACKSTAB )
						{
							targ->isBackstabbed = qtrue;
						}
						else
						{
							targ->isBackstabbed = qfalse;
						}
					}

					targ->die( targ, inflictor, attacker, take, mod );

					// OSP - kill stats in player_die function
				}

				if
				(
					targ->s.eType == ET_MOVER &&
					!Q_stricmp( targ->classname, "script_mover") &&
					(targ->spawnflags & 8)
				)
				{
					return;															// reseructable script mover doesn't unlink itself but we don't want a second death script to be called
				}

				// if we freed ourselves in death function
				if ( !targ->inuse )
				{
					return;
				}

				// RF, entity scripting
				if ( targ->health <= 0 )
				{									// might have revived itself in death function
					if ( targ->r.svFlags & SVF_BOT )
					{
#ifndef NO_BOT_SUPPORT

						// See if this is the first kill of this bot
						if ( wasAlive )
						{
							Bot_ScriptEvent( targ->s.number, "death", "" );
						}
#endif
					}
					else if
						(
							(targ->s.eType != ET_CONSTRUCTIBLE && targ->s.eType != ET_EXPLOSIVE) ||
							(targ->s.eType == ET_CONSTRUCTIBLE && !targ->desstages)
						)
					{								// call manually if using desstages
						G_Script_ScriptEvent( targ, "death", "" );
					}
				}

				// RF, record bot death
#ifndef NO_BOT_SUPPORT
				if ( targ->s.number < level.maxclients && targ->r.svFlags & SVF_BOT )
				{
					BotRecordDeath( targ->s.number, attacker->s.number );
				}
#endif
			}
		}
		else if ( targ->pain )
		{
			if ( dir )
			{										// Ridah, had to add this to fix NULL dir crash
				VectorCopy( dir, targ->rotate );
				VectorCopy( point, targ->pos3 );	// this will pass loc of hit
			}
			else
			{
				VectorClear( targ->rotate );
				VectorClear( targ->pos3 );
			}

			targ->pain( targ, attacker, take, point );
		}
		else
		{

			// OSP - update weapon/dmg stats
			G_addStats( targ, attacker, take, mod );

			// OSP
		}

		// RF, entity scripting
		G_Script_ScriptEvent( targ, "pain", va( "%d %d", targ->health, targ->health + take) );
#ifndef NO_BOT_SUPPORT
		if ( targ->s.number < MAX_CLIENTS && (targ->r.svFlags & SVF_BOT) )
		{
			Bot_ScriptEvent( targ->s.number, "pain", va( "%d %d", targ->health, targ->health + take) );
		}
#endif

		// RF, record bot pain
#ifndef NO_BOT_SUPPORT
		if ( targ->s.number < level.maxclients && targ->r.svFlags & SVF_BOT )
		{
			BotRecordPain( targ->s.number, attacker->s.number, mod );
		}
#endif
#ifdef OMNIBOTS

		// notify omni-bot framework
		botUserData.m_DataType = dtEntity;
		botUserData.udata.m_Entity = (GameEntity *) attacker;
		Bot_Interface_SendEvent( PERCEPT_FEEL_PAIN, targ->s.number, 0, 0, &botUserData );
#endif

		// Ridah, this needs to be done last, incase the health is altered in one of the event calls
		if ( targ->client )
		{
			targ->client->ps.stats[STAT_HEALTH] = targ->health;
		}
	}
}


/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
void
G_RailTrail ( vec_t *start, vec_t *end )
{
	gentity_t	*temp = G_TempEntity( start, EV_RAILTRAIL );
	VectorCopy( end, temp->s.origin2 );
	temp->s.dmgFlags = 0;
}

#define MASK_CAN_DAMAGE ( CONTENTS_SOLID | CONTENTS_BODY )


/* */
qboolean
CanDamage ( gentity_t *targ, vec3_t origin )
{
	vec3_t	dest;
	trace_t tr;
	vec3_t	midpoint;
	vec3_t	offsetmins = { -16.f, -16.f, -16.f };
	vec3_t	offsetmaxs = { 16.f, 16.f, 16.f };

	// use the midpoint of the bounds instead of the origin, because
	// bmodels may have their origin is 0,0,0
	// Gordon: well, um, just check then...
	if ( targ->r.currentOrigin[0] || targ->r.currentOrigin[1] || targ->r.currentOrigin[2] )
	{
		VectorCopy( targ->r.currentOrigin, midpoint );
		if ( targ->s.eType == ET_MOVER )
		{
			midpoint[2] += 32;
		}
	}
	else
	{
		VectorAdd( targ->r.absmin, targ->r.absmax, midpoint );
		VectorScale( midpoint, 0.5, midpoint );
	}

	//	G_RailTrail( origin, dest );
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, midpoint, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1.0 )
	{
		return ( qtrue );
	}

	if ( &g_entities[tr.entityNum] == targ )
	{
		return ( qtrue );
	}

	if ( targ->client )
	{
		VectorCopy( targ->client->ps.mins, offsetmins );
		VectorCopy( targ->client->ps.maxs, offsetmaxs );
	}

	// this should probably check in the plane of projection,
	// rather than in world coordinate
	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ )
	{
		return ( qtrue );
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ )
	{
		return ( qtrue );
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ )
	{
		return ( qtrue );
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ )
	{
		return ( qtrue );
	}

	// =========================
	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ )
	{
		return ( qtrue );
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ )
	{
		return ( qtrue );
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ )
	{
		return ( qtrue );
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmins[2];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ )
	{
		return ( qtrue );
	}

	return ( qfalse );
}


/* */
void
G_AdjustedDamageVec ( gentity_t *ent, vec3_t origin, vec3_t v )
{
	int i;
	if ( !ent->r.bmodel )
	{
		VectorSubtract( ent->r.currentOrigin, origin, v );	// JPW NERVE simpler centroid check that doesn't have box alignment weirdness
	}
	else
	{
		for ( i = 0; i < 3; i++ )
		{
			if ( origin[i] < ent->r.absmin[i] )
			{
				v[i] = ent->r.absmin[i] - origin[i];
			}
			else if ( origin[i] > ent->r.absmax[i] )
			{
				v[i] = origin[i] - ent->r.absmax[i];
			}
			else
			{
				v[i] = 0;
			}
		}
	}
}


/*
============
G_RadiusDamage
============
*/
qboolean
G_RadiusDamage
(
	vec3_t		origin,
	gentity_t	*inflictor,
	gentity_t	*attacker,
	float		damage,
	float		radius,
	gentity_t	*ignore,
	int			mod
)
{
	float		points, dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			i, e;
	qboolean	hitClient = qfalse;
	float		boxradius;
	vec3_t		dest;
	trace_t		tr;
	vec3_t		midpoint;
	int			flags = DAMAGE_RADIUS;
	if ( mod == MOD_SATCHEL || mod == MOD_LANDMINE )
	{
		flags |= DAMAGE_HALF_KNOCKBACK;
	}

	if ( radius < 1 )
	{
		radius = 1;
	}

	boxradius = 1.41421356 * radius;	// radius * sqrt(2) for bounding box enlargement --

	// bounding box was checking against radius / sqrt(2) if collision is along box plane
	for ( i = 0; i < 3; i++ )
	{
		mins[i] = origin[i] - boxradius;
		maxs[i] = origin[i] + boxradius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
	for ( e = 0; e < level.num_entities; e++ )
	{
		g_entities[e].dmginloop = qfalse;
	}

	for ( e = 0; e < numListedEntities; e++ )
	{
		ent = &g_entities[entityList[e]];
		if ( ent == ignore )
		{
			continue;
		}

		if ( !ent->takedamage && (!ent->dmgparent || !ent->dmgparent->takedamage) )
		{
			continue;
		}

		G_AdjustedDamageVec( ent, origin, v );
		dist = VectorLength( v );
		if ( dist >= radius )
		{
			continue;
		}

		points = damage * ( 1.0 - dist / radius );
		if ( CanDamage( ent, origin) )
		{
			if ( ent->dmgparent )
			{
				ent = ent->dmgparent;
			}

			if ( ent->dmginloop )
			{
				continue;
			}

			if ( AccuracyHit( ent, attacker) )
			{
				hitClient = qtrue;
			}

			VectorSubtract( ent->r.currentOrigin, origin, dir );

			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;
			G_Damage( ent, inflictor, attacker, dir, origin, (int) points, flags, mod );
		}
		else
		{
			VectorAdd( ent->r.absmin, ent->r.absmax, midpoint );
			VectorScale( midpoint, 0.5, midpoint );
			VectorCopy( midpoint, dest );
			trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
			if ( tr.fraction < 1.0 )
			{
				VectorSubtract( dest, origin, dest );
				dist = VectorLength( dest );
				if ( dist < radius * 0.2f )
				{						// closer than 1/4 dist
					if ( ent->dmgparent )
					{
						ent = ent->dmgparent;
					}

					if ( ent->dmginloop )
					{
						continue;
					}

					if ( AccuracyHit( ent, attacker) )
					{
						hitClient = qtrue;
					}

					VectorSubtract( ent->r.currentOrigin, origin, dir );
					dir[2] += 24;
					G_Damage( ent, inflictor, attacker, dir, origin, (int) (points * 0.1f), flags, mod );
				}
			}
		}
	}

	return ( hitClient );
}


/*
============
etpro_RadiusDamage
mutation of G_RadiusDamage which lets us selectively damage only clients or only non clients
============
*/
qboolean
etpro_RadiusDamage
(
	vec3_t		origin,
	gentity_t	*inflictor,
	gentity_t	*attacker,
	float		damage,
	float		radius,
	gentity_t	*ignore,
	int			mod,
	qboolean	clientsonly
)
{
	float		points, dist;
	gentity_t	*ent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			i, e;
	qboolean	hitClient = qfalse;
	float		boxradius;
	vec3_t		dest;
	trace_t		tr;
	vec3_t		midpoint;
	int			flags = DAMAGE_RADIUS;
	if ( mod == MOD_SATCHEL || mod == MOD_LANDMINE )
	{
		flags |= DAMAGE_HALF_KNOCKBACK;
	}

	if ( radius < 1 )
	{
		radius = 1;
	}

	boxradius = 1.41421356 * radius;	// radius * sqrt(2) for bounding box enlargement --

	// bounding box was checking against radius / sqrt(2) if collision is along box plane
	for ( i = 0; i < 3; i++ )
	{
		mins[i] = origin[i] - boxradius;
		maxs[i] = origin[i] + boxradius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
	for ( e = 0; e < level.num_entities; e++ )
	{
		g_entities[e].dmginloop = qfalse;
	}

	for ( e = 0; e < numListedEntities; e++ )
	{
		ent = &g_entities[entityList[e]];
		if ( ent == ignore )
		{
			continue;
		}

		if ( !ent->takedamage && (!ent->dmgparent || !ent->dmgparent->takedamage) )
		{
			continue;
		}

		if ( clientsonly && !ent->client )
		{
			continue;
		}

		if ( !clientsonly && ent->client )
		{
			continue;
		}

		G_AdjustedDamageVec( ent, origin, v );
		dist = VectorLength( v );
		if ( dist >= radius )
		{
			continue;
		}

		points = damage * ( 1.0 - dist / radius );
		if ( CanDamage( ent, origin) )
		{
			if ( ent->dmgparent )
			{
				ent = ent->dmgparent;
			}

			if ( ent->dmginloop )
			{
				continue;
			}

			if ( AccuracyHit( ent, attacker) )
			{
				hitClient = qtrue;
			}

			VectorSubtract( ent->r.currentOrigin, origin, dir );

			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;
			G_Damage( ent, inflictor, attacker, dir, origin, (int) points, flags, mod );
		}
		else
		{
			VectorAdd( ent->r.absmin, ent->r.absmax, midpoint );
			VectorScale( midpoint, 0.5, midpoint );
			VectorCopy( midpoint, dest );
			trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
			if ( tr.fraction < 1.0 )
			{
				VectorSubtract( dest, origin, dest );
				dist = VectorLength( dest );
				if ( dist < radius * 0.2f )
				{						// closer than 1/4 dist
					if ( ent->dmgparent )
					{
						ent = ent->dmgparent;
					}

					if ( ent->dmginloop )
					{
						continue;
					}

					if ( AccuracyHit( ent, attacker) )
					{
						hitClient = qtrue;
					}

					VectorSubtract( ent->r.currentOrigin, origin, dir );
					dir[2] += 24;
					G_Damage( ent, inflictor, attacker, dir, origin, (int) (points * 0.1f), flags, mod );
				}
			}
		}
	}

	return ( hitClient );
}
