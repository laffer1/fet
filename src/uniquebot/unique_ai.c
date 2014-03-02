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
#ifdef _WIN32

// UQ1: Trying some Win32 multithreading for AI...
#include <windows.h>
#include <process.h>
#include <mmsystem.h>
#include <stdio.h>
#include <conio.h>
DWORD	cthread[MAX_CLIENTS];
#endif //_WIN32

#include "../game/g_local.h"
#include "../game/q_shared.h"
#include "../game/botlib.h"
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_char.h"
#include "../game/be_ai_chat.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../game/be_ai_weap.h"
#include "../botai/botai.h"
#include "../botai/inv.h"

//
#include "../botai/ai_main.h"
#include "../botai/ai_dmq3.h"
#include "../botai/ai_cmd.h"
#include "../botai/ai_team.h"

//
#include "../botai/ai_dmnet_mp.h"

// Key AI constants
#include "../botai/ai_distances.h"
#include "../botai/chars.h"
int				myteam, otherteam;
extern vmCvar_t bot_cpu_use;

#ifdef __ETE__
vmCvar_t	g_realmedic;
vmCvar_t	g_noSelfHeal;
#endif //__ETE__

extern qboolean	BG_Weapon_Is_Aimable ( int weapon );

//===========================================================================
// Routine      : AIMOD_AI_Think
// Description  : Main think function for bot
extern float	TravelTime ( gentity_t *bot );
extern void		ClientThink_real ( gentity_t *ent );
void			AIMOD_AI_DropMed ( gentity_t *bot );
void			AIMOD_AI_DropAmmo ( gentity_t *bot );
void			AIMOD_AI_BackFromDrop ( gentity_t *bot );
int				AIMOD_AI_SetMedicAction ( gentity_t *bot, usercmd_t *ucmd );

//void AI_NewThink(gentity_t *bot);
extern void		AIMOD_MOVEMENT_Attack ( gentity_t *bot, usercmd_t *ucmd );
extern void		AIMOD_MOVEMENT_AttackMove ( gentity_t *bot, usercmd_t *ucmd );
extern int		AIMOD_NAVIGATION_FindClosestReachableNodes ( gentity_t *bot, int r, int type );
extern int		AIMOD_NAVIGATION_FindClosestReachableNodesTo ( gentity_t *bot, vec3_t origin, int r, int type );
extern int		AIMOD_NAVIGATION_FindClosestReachableNodeTo ( vec3_t origin, int r, int type );
void			AIMOD_SP_MOVEMENT_CoverSpot_Movement ( gentity_t *bot, usercmd_t *ucmd );
extern qboolean BAD_WP_Height ( vec3_t start, vec3_t end );
qboolean		AIMOD_AI_Think ( gentity_t *bot );
int				next_weapon_change[MAX_CLIENTS];
qboolean		need_medic[MAX_CLIENTS];
qboolean		need_ammo[MAX_CLIENTS];
int				AI_Medic_Time[MAX_CLIENTS];
int				AI_Job_Action[MAX_CLIENTS];
int				need_time[MAX_CLIENTS];
extern void		AI_ResetJob ( gentity_t *bot );
extern char		temp_player_name[36];	// AIMOD_navigation.c


/* */
void
AI_Needs_Medic ( int clientNum )
{
	need_medic[clientNum] = qtrue;
	need_time[clientNum] = level.time;
}


/* */
void
AI_Needs_Ammo ( int clientNum )
{
	need_ammo[clientNum] = qtrue;
	need_time[clientNum] = level.time;
}

extern void G_Voice ( gentity_t *ent, gentity_t *target, int mode, const char *id, qboolean voiceonly );


/* */
void
AIMOD_Player_Special_Needs_Clear ( int clientNum )
{
	if ( ClientOutOfAmmo( clientNum) == qfalse )
	{
		need_ammo[clientNum] = qfalse;
	}

	if ( g_entities[clientNum].health >= g_entities[clientNum].client->pers.maxHealth )
	{
		need_medic[clientNum] = qfalse;
	}

	need_time[clientNum] = 0;
}

int				JobType ( gentity_t *bot );
extern qboolean Teammate_SpashDamage_OK ( gentity_t *ent, vec3_t origin );	// g_navigation.c
extern qboolean AIMOD_HaveAmmoForWeapon ( playerState_t *ps, weapon_t wep );
extern qboolean AIMOD_NeedAmmoForWeapon ( playerState_t *ps, weapon_t wep );


/* */
qboolean
AIMod_ClientNeedsAmmo ( int client )
{
	gentity_t	*bot = &g_entities[client];

	//	qboolean needsammo = AIMOD_NeedAmmoForWeapon(&bot->client->ps, bot->weaponchoice);
	//	if (!needsammo || bot->weaponchoice <= 0)
	qboolean	needsammo = AIMOD_NeedAmmoForWeapon( &bot->client->ps, bot->client->ps.weapon );
	return ( needsammo );
}


/* */
qboolean
ClientOutOfAmmo ( int client )
{
	gentity_t	*bot = &g_entities[client];

	//	qboolean needsammo = AIMOD_HaveAmmoForWeapon(&bot->client->ps, bot->weaponchoice);
	//	if (!needsammo || bot->weaponchoice <= 0)
	qboolean	needsammo = !AIMOD_HaveAmmoForWeapon( &bot->client->ps, bot->client->ps.weapon );
	return ( needsammo );
}

extern int	next_drop[MAX_CLIENTS];
extern void		BotSetViewAngles ( gentity_t *bot, float thinktime );

/* */
void
Set_Best_AI_weapon ( gentity_t *bot )
{
	gclient_t	*client;
	int			clientNum;
	int			pc;
	usercmd_t	*ucmd;
	int			weaponChoice;

	if ( !bot )
	{
		return;
	}

	if ( !bot->client )
	{
		return;
	}

	if ( bot->health <= 0 )
	{
		return;
	}

	if ((bot->beStill > level.time || bot->beStillEng > level.time) && bot->client->ps.weapon == WP_PLIERS)
	{// We probebly have a job to do!
		return;
	}

	client = bot->client;
	clientNum = bot->client->ps.clientNum;
	pc = bot->client->sess.playerType;
	ucmd = &bot->client->pers.cmd;

	if
	(
		bot->client->sess.playerType == PC_FIELDOPS &&
		AIMOD_NeedAmmoForWeapon( &bot->client->ps, bot->weaponchoice)
	)
	{														// Heal self first!
		vec3_t	lookAngles;
		if (bot->client->ps.weapon != WP_AMMO)
		{
			weaponChoice = WP_AMMO;
			next_weapon_change[bot->s.clientNum] = level.time + 3000;
		}

		if (bot->client->ps.weapon != weaponChoice)
			ucmd->weapon = weaponChoice;

		// Set up ideal view angles for this target...
		VectorCopy( bot->client->ps.viewangles, lookAngles );
		lookAngles[ROLL] = lookAngles[ROLL] - 65;
		vectoangles( lookAngles, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		if (bot->enemy)
		{
			ucmd->upmove = -48;								// Crouch...
			bot->client->ps.pm_flags |= PMF_DUCKED;
		}
		if ( !next_drop[bot->s.number] )
		{
			next_drop[bot->s.number] = level.time - 1;
		}

		if ( next_drop[bot->s.number] < level.time )
		{
			next_drop[bot->s.number] = level.time + 800;
			if ( bot->client->ps.weapon == WP_AMMO )
			{
				ucmd->buttons = BUTTON_ATTACK;
			}
		}

		return;
	}


	if ( bot->client->sess.playerType == PC_MEDIC && bot->health < bot->client->ps.stats[STAT_MAX_HEALTH] * 0.75 
		&& !g_realmedic.integer // jaquboss
		&& !g_noSelfHeal.integer )
	{														// Heal self first!
		vec3_t	lookAngles;

		//bot->s.weapon = bot->client->pers.cmd.weapon = WP_MEDKIT;
		if (bot->client->ps.weapon != WP_MEDKIT)
		{
			weaponChoice = WP_MEDKIT;
			next_weapon_change[bot->s.clientNum] = level.time + 3000;
		}

		if (bot->client->ps.weapon != weaponChoice)
			ucmd->weapon = weaponChoice;

		// Set up ideal view angles for this target...
		VectorCopy( bot->client->ps.viewangles, lookAngles );
		lookAngles[ROLL] = lookAngles[ROLL] - 65;
		vectoangles( lookAngles, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		
		if (bot->enemy)
		{
			ucmd->upmove = -48;								// Crouch...
			bot->client->ps.pm_flags |= PMF_DUCKED;
		}

		if ( !next_drop[bot->s.number] )
		{
			next_drop[bot->s.number] = level.time - 1;
		}

		if ( next_drop[bot->s.number] < level.time )
		{
			next_drop[bot->s.number] = level.time + 800;
			ucmd->buttons = BUTTON_ATTACK;

			//FireWeapon( bot );
		}

		return;
	}

	if
	(
		bot->client->sess.playerType == PC_MEDIC &&
		bot->health < bot->client->ps.stats[STAT_MAX_HEALTH] * 0.95 &&
		!g_realmedic.integer // jaquboss
		&&
		!g_noSelfHeal.integer &&
		Q_TrueRand( 0, 10000) <= 1
	)
	{														// Heal self first!
		vec3_t	lookAngles;

		//bot->s.weapon = bot->client->pers.cmd.weapon = WP_MEDKIT;
		if (bot->client->ps.weapon != WP_MEDKIT)
		{
			weaponChoice = WP_MEDKIT;
			next_weapon_change[bot->s.clientNum] = level.time + 3000;
		}

		if (bot->client->ps.weapon != weaponChoice)
			ucmd->weapon = weaponChoice;

		// Set up ideal view angles for this target...
		VectorCopy( bot->client->ps.viewangles, lookAngles );
		lookAngles[ROLL] = lookAngles[ROLL] - 65;
		vectoangles( lookAngles, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		if ( !next_drop[bot->s.number] )
		{
			next_drop[bot->s.number] = level.time - 1;
		}

		if ( next_drop[bot->s.number] < level.time )
		{
			next_drop[bot->s.number] = level.time + 800;
			ucmd->buttons = BUTTON_ATTACK;
		}

		return;
	}

	if
	(
		JobType( bot) > BOT_JOB_NONE &&
		(
			bot->client->ps.weapon == WP_MEDIC_SYRINGE ||
			bot->client->ps.weapon == WP_MEDKIT ||
			bot->client->ps.weapon == WP_AMMO
		)
	)
	{															// We have a job to do.
		return;
	}

	if
	(
		(bot->bot_can_build && pc == PC_ENGINEER) ||
		(bot->client->ps.weapon == WP_PLIERS && bot->beStillEng > level.time)
	)
	{
		return;													// We have a job to do.
	}

	if
	(
		bot->client->ps.eFlags & EF_MOUNTEDTANK ||
		bot->client->ps.eFlags & EF_AAGUN_ACTIVE ||
		bot->client->ps.eFlags & EF_MG42_ACTIVE
	)
	{
		return;
	}


	if ( next_weapon_change[bot->s.clientNum] > level.time )
	{
		return;
	}

	next_weapon_change[bot->s.clientNum] = level.time + 3000;

	// No more ammo, but we can reload, so RELOAD !
	if
	(
		bot->client->ps.weapon == WP_FG42SCOPE ||
		bot->client->ps.weapon == WP_K43_SCOPE ||
		bot->client->ps.weapon == WP_GARAND_SCOPE ||
		bot->client->ps.weapon == WP_GPG40 ||
		bot->client->ps.weapon == WP_M7
	)
	{
		if ( bot->client->ps.ammoclip[bot->client->ps.weapon] == 0 && bot->client->ps.ammo[bot->client->ps.weapon] > 0 )
		{
			if ( bot->client->ps.weapon == WP_FG42SCOPE )
			{
				weaponChoice = WP_FG42;
			}

			if ( bot->client->ps.weapon == WP_K43_SCOPE )
			{
				weaponChoice = WP_K43;
			}

			if ( bot->client->ps.weapon == WP_GARAND_SCOPE )
			{
				weaponChoice = WP_GARAND;
			}

			if ( bot->client->ps.weapon == WP_GPG40 )
			{
				weaponChoice = WP_KAR98;
			}

			if ( bot->client->ps.weapon == WP_M7 )
			{
				weaponChoice = WP_CARBINE;
			}

			if (bot->client->ps.weapon != weaponChoice)
				ucmd->weapon = weaponChoice;

			ucmd->wbuttons = WBUTTON_RELOAD;
			ucmd->forwardmove = 0;
			ucmd->rightmove = 0;
			ucmd->upmove = 0;
			return;
		}
	}
	else if
		(
			bot->client->ps.ammoclip[bot->client->ps.weapon] == 0 &&
			bot->client->ps.ammo[bot->client->ps.weapon] > 0
		)
	{
		ucmd->wbuttons = WBUTTON_RELOAD;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		return;
	}

	if
	(
		(bot->client->ps.weapon == WP_GPG40 || bot->client->ps.weapon == WP_M7) &&
		bot->client->ps.ammoclip[bot->client->ps.weapon] != 0
	)
	{
		return;
	}

/*	if
	(
		bot->enemy &&
		bot->client->ps.weapon == WP_FG42 &&
		VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024
	)
	{
		weaponChoice = WP_FG42SCOPE;

		return;
	}
	else if
		(
			bot->enemy &&
			bot->client->ps.weapon == WP_K43 &&
			VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024
		)
	{
		weaponChoice = WP_K43_SCOPE;
		return;
	}
	else if
		(
			bot->enemy &&
			bot->client->ps.weapon == WP_GARAND &&
			VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024
		)
	{
		weaponChoice = WP_GARAND_SCOPE;
		return;
	}
	else if
		(
			bot->enemy &&
			bot->bot_sniper_spot &&
			VectorDistance( bot->r.currentOrigin, bot->bot_sniper_spot) < 24 &&
			bot->client->ps.weapon == WP_FG42 &&
			VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024
		)
	{
		weaponChoice = WP_FG42SCOPE;
		return;
	}
	else if
		(
			bot->enemy &&
			bot->bot_sniper_spot &&
			VectorDistance( bot->r.currentOrigin, bot->bot_sniper_spot) < 24 &&
			bot->client->ps.weapon == WP_K43 &&
			VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024
		)
	{
		weaponChoice = WP_K43_SCOPE;
		return;
	}
	else if
		(
			bot->enemy &&
			bot->bot_sniper_spot &&
			VectorDistance( bot->r.currentOrigin, bot->bot_sniper_spot) < 24 &&
			bot->client->ps.weapon == WP_GARAND &&
			VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024
		)
	{
		weaponChoice = WP_GARAND_SCOPE;
		return;
	}
*/
	if ( bot->bot_sniper_spot && VectorDistance( bot->r.currentOrigin, bot->bot_sniper_spot) < 24 )
	{
		return;
	}

	if
	(
		bot->enemy &&
		VectorDistance( bot->enemy->r.currentOrigin, bot->r.currentOrigin) >= 1024 &&
		(
			bot->client->ps.weapon == WP_FG42SCOPE ||
			bot->client->ps.weapon == WP_K43_SCOPE ||
			bot->client->ps.weapon == WP_GARAND_SCOPE
		)
	)
	{
		return;
	}

	need_ammo[clientNum] = qfalse;

	if ( bot->weaponchoice == WP_FLAMETHROWER )
	{															// Need to check range and maybe switch to pistol.
		//bot->client->ps.ammoclip[WP_FLAMETHROWER] = 200; // Flamers will always have ammo!
		if ( COM_BitCheck( bot->client->ps.weapons, WP_FLAMETHROWER) &&
			!AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_FLAMETHROWER) )
		{
			BG_AddAmmo( &bot->client->ps, WP_FLAMETHROWER, 200 );
		}

		if ( bot->enemy )
		{
			if
			(
				VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 240 /*300*/ ||
				!Teammate_SpashDamage_OK( bot, bot->enemy->r.currentOrigin)
			)
			{
				if
				(
					COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_COLT) &&
					AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_COLT)
				)
				{
					weaponChoice = WP_AKIMBO_COLT;
				}
				else if
					(
						COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_LUGER) &&
						AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_LUGER)
					)
				{
					weaponChoice = WP_AKIMBO_LUGER;
				}
				else if ( COM_BitCheck( bot->client->ps.weapons, WP_LUGER) &&
					AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_LUGER) )
				{
					weaponChoice = WP_LUGER;
				}
				else if ( COM_BitCheck( bot->client->ps.weapons, WP_COLT) &&
					AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_COLT) )
				{
					weaponChoice = WP_COLT;
				}
				else
				{
					weaponChoice = WP_KNIFE;
				}
			}
			else
			{
				if ( COM_BitCheck( bot->client->ps.weapons, WP_FLAMETHROWER) &&
					AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_FLAMETHROWER) )
				{
					weaponChoice = WP_FLAMETHROWER;
				}
				else if
				(
					COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_COLT) &&
					AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_COLT)
				)
				{
					weaponChoice = WP_AKIMBO_COLT;
				}
				else if
					(
						COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_LUGER) &&
						AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_LUGER)
					)
				{
					weaponChoice = WP_AKIMBO_LUGER;
				}
				else if ( COM_BitCheck( bot->client->ps.weapons, WP_LUGER) &&
					AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_LUGER) )
				{
					weaponChoice = WP_LUGER;
				}
				else if ( COM_BitCheck( bot->client->ps.weapons, WP_COLT) &&
					AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_COLT) )
				{
					weaponChoice = WP_COLT;
				}
				else
				{
					weaponChoice = WP_KNIFE;
				}
			}
		}
		else
		{
			if ( COM_BitCheck( bot->client->ps.weapons, WP_FLAMETHROWER) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_FLAMETHROWER) )
			{
				weaponChoice = bot->weaponchoice;
			}
			else if
			(
				COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_COLT) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_COLT)
			)
			{
				weaponChoice = WP_AKIMBO_COLT;
			}
			else if
				(
					COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_LUGER) &&
					AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_LUGER)
				)
			{
				weaponChoice = WP_AKIMBO_LUGER;
			}
			else if ( COM_BitCheck( bot->client->ps.weapons, WP_LUGER) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_LUGER) )
			{
				weaponChoice = WP_LUGER;
			}
			else if ( COM_BitCheck( bot->client->ps.weapons, WP_COLT) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_COLT) )
			{
				weaponChoice = WP_COLT;
			}
			else
			{
				weaponChoice = WP_KNIFE;
			}
		}
	}
	else if ( bot->weaponchoice == WP_PANZERFAUST )
	{															// Need to check special energy and maybe switch to pistol.
		//bot->client->ps.ammoclip[WP_PANZERFAUST] = 4; // Panzers will always have ammo!
		if ( !AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_PANZERFAUST) )
		{
			BG_AddAmmoClip( &bot->client->ps, WP_PANZERFAUST, 2 );
		}

		if ( !bot->enemy )
		{
			if ( COM_BitCheck( bot->client->ps.weapons, bot->weaponchoice) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, bot->weaponchoice) )
			{
				weaponChoice = bot->weaponchoice;
			}
			else if
			(
				COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_COLT) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_COLT)
			)
			{
				weaponChoice = WP_AKIMBO_COLT;
			}
			else if
				(
					COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_LUGER) &&
					AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_LUGER)
				)
			{
				weaponChoice = WP_AKIMBO_LUGER;
			}
			else if ( COM_BitCheck( bot->client->ps.weapons, WP_LUGER) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_LUGER) )
			{
				weaponChoice = WP_LUGER;
			}
			else if ( COM_BitCheck( bot->client->ps.weapons, WP_COLT) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_COLT) )
			{
				weaponChoice = WP_COLT;
			}
			else
			{
				weaponChoice = WP_KNIFE;
			}
		}
		else if
			(
				level.time -
					bot->client->ps.classWeaponTime > level.lieutenantChargeTime[bot->client->sess.sessionTeam - 1] &&
					(AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_PANZERFAUST)) &&
					Teammate_SpashDamage_OK( bot, bot->enemy->r.currentOrigin)
			)
		{
			weaponChoice = WP_PANZERFAUST;
		}
		else if ( bot->client->ps.weaponDelay > 0 )
		{														// Delay for panzer fire sequence.
		}
		else
		{
			if
			(
				COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_COLT) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_COLT)
			)
			{
				weaponChoice = WP_AKIMBO_COLT;
			}
			else if
				(
					COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_LUGER) &&
					AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_LUGER)
				)
			{
				weaponChoice = WP_AKIMBO_LUGER;
			}
			else if ( COM_BitCheck( bot->client->ps.weapons, WP_LUGER) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_LUGER) )
			{
				weaponChoice = WP_LUGER;
			}
			else if ( COM_BitCheck( bot->client->ps.weapons, WP_COLT) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_COLT) )
			{
				weaponChoice = WP_COLT;
			}
			else
			{
				weaponChoice = WP_KNIFE;
			}
		}
	}
	else
	{
		if ( bot->enemy 
			&& COM_BitCheck( bot->client->ps.weapons, WP_MOBILE_MG42_SET) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_MOBILE_MG42_SET)
			&& bot->client->ps.weapon == WP_MOBILE_MG42_SET)
		{
			weaponChoice = WP_MOBILE_MG42_SET;
		}
		else if
		(
			bot->enemy &&
			VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 400 &&
			COM_BitCheck( bot->client->ps.weapons, WP_K43_SCOPE) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_K43_SCOPE)
		)
		{														// Snipe!
			weaponChoice = WP_K43_SCOPE;
			bot->beStill = level.time + 2000;
		}
		else if
		(
			bot->enemy &&
			VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 400 &&
			COM_BitCheck( bot->client->ps.weapons, WP_FG42SCOPE) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_FG42SCOPE)
		)
		{														// Snipe!
			weaponChoice = WP_FG42SCOPE;

			bot->beStill = level.time + 2000;
		}
		else if
		(
			bot->enemy &&
			VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 400 &&
			COM_BitCheck( bot->client->ps.weapons, WP_GARAND_SCOPE) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_GARAND_SCOPE)
		)
		{														// Snipe!
			weaponChoice = WP_GARAND_SCOPE;

			bot->beStill = level.time + 2000;
		}
		else if ( COM_BitCheck( bot->client->ps.weapons, bot->weaponchoice) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, bot->weaponchoice) )
		{
			weaponChoice = bot->weaponchoice;
		}
		else if
			(
				COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_SILENCEDCOLT) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_SILENCEDCOLT)
			)
		{
			weaponChoice = WP_AKIMBO_SILENCEDCOLT;
		}
		else if
			(
				COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_SILENCEDLUGER) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_SILENCEDLUGER)
			)
		{
			weaponChoice = WP_AKIMBO_SILENCEDLUGER;
		}
		else if
			(
				COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_COLT) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_COLT)
			)
		{
			weaponChoice = WP_AKIMBO_COLT;
		}
		else if
			(
				COM_BitCheck( bot->client->ps.weapons, WP_AKIMBO_LUGER) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_AKIMBO_LUGER)
			)
		{
			weaponChoice = WP_AKIMBO_LUGER;
		}
		else if
			(
				COM_BitCheck( bot->client->ps.weapons, WP_SILENCER) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_SILENCER)
			)
		{
			weaponChoice = WP_SILENCER;
		}
		else if
			(
				COM_BitCheck( bot->client->ps.weapons, WP_SILENCED_COLT) &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_SILENCED_COLT)
			)
		{
			weaponChoice = WP_SILENCED_COLT;
		}
		else if ( COM_BitCheck( bot->client->ps.weapons, WP_LUGER) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_LUGER) )
		{
			weaponChoice = WP_LUGER;
		}
		else if ( COM_BitCheck( bot->client->ps.weapons, WP_COLT) &&
			AIMOD_HaveAmmoForWeapon( &bot->client->ps, WP_COLT) )
		{
			weaponChoice = WP_COLT;
		}
		else
		{
			weaponChoice = WP_KNIFE;
		}
	}

	if (bot->client->ps.weapon != weaponChoice)
		ucmd->weapon = weaponChoice;

}

int nextcheck[MAX_CLIENTS];


/* */
void
Check_Bot_Needs ( gentity_t *ent )
{
/*	gclient_t	*client = ent->client;
	int			clientNum = ent->s.clientNum;
	int			pc = client->sess.playerType;
	int			weapon;
	int			i;
	gentity_t	*bot = ent;
	if ( ent->health <= 0 )
	{
		return;
	}

	if ( !ent->is_bot || !(ent->r.svFlags & SVF_BOT) )
	{
		return;
	}

	if ( nextcheck[clientNum] > level.time )
	{
		return;
	}

	nextcheck[clientNum] = level.time + 5000 + ( Q_TrueRand( 0, 5000) );
	weapon = bot->weaponchoice;
	if ( pc == PC_FIELDOPS )
	{														// Give themself ammo.
		if ( client->ps.ammoclip[weapon] <= GetAmmoTableData( weapon)->maxclip * 0.6 )
		{

			// TTimo - add 8 pistol bullets
			if ( client->sess.sessionTeam == TEAM_AXIS )
			{
				weapon = WP_LUGER;
			}
			else
			{
				weapon = WP_COLT;
			}

			BG_AddAmmo( &client->ps, weapon, GetAmmoTableData( weapon)->maxammo );
			if ( client->ps.ammo[weapon] < GetAmmoTableData( weapon)->maxclip * 0.4 )
			{
				client->ps.ammo[weapon] = GetAmmoTableData( weapon )->maxammo;
			}

			// and some two-handed ammo
			for ( i = 0; i < MAX_WEAPS_IN_BANK_MP; i++ )
			{
				weapon = weapBanksMultiPlayer[3][i];
				if ( COM_BitCheck( client->ps.weapons, weapon) )
				{
					client->ps.ammo[weapon] = GetAmmoTableData( weapon )->maxammo;
					if ( GetAmmoTableData( weapon)->maxclip > client->ps.ammoclip[weapon] )
					{
						client->ps.ammoclip[weapon] = GetAmmoTableData( weapon )->maxclip;
					}
				}
			}
		}

		client->pers.cmd.forwardmove = 0;
		client->pers.cmd.rightmove = 0;
		client->pers.cmd.upmove = 0;
		return;
	}

	if
	(
		client->ps.ammo[bot->weaponchoice] == 0 &&
		client->ps.ammoclip[bot->weaponchoice] == 0 &&
		!need_ammo[clientNum]
	)
	{
		need_ammo[clientNum] = qtrue;
		G_Voice( bot, NULL, SAY_TEAM, "NeedAmmo", qfalse );
	}
	else
	{
		need_ammo[clientNum] = qfalse;
	}

	if ( ent->health < ent->client->pers.maxHealth * 0.45 )
	{														// Client is in need of ammo.
		if ( need_medic[clientNum] == qfalse )
		{
			G_Voice( ent, NULL, SAY_TEAM, "Medic", qfalse );
		}

		need_medic[clientNum] = qtrue;
	}
	else
	{
		need_medic[clientNum] = qfalse;
	}*/
}

extern bot_state_t	botstates[MAX_CLIENTS];
void				AIMOD_AI_FastThink ( gentity_t *bot );	// below...

void
SOD_AI_BotThink ( gentity_t *bot )
{
	usercmd_t	*ucmd;
	int			pc;
	gclient_t	*client;

	if ( level.intermissiontime )
	{							// Don't think in level intermission time.
		return;
	}

	if ( bot->health )
	{
		if ( bot->health <= 0 )
		{
			bot->s.eFlags |= EF_DEAD;
		}
	}

	if ( !bot->client )
	{
		return;
	}

	client = bot->client;

	if
	(
		bot->client &&
		(
			(bot->client->ps.eFlags & EF_AIMING) &&
			(bot->client->ps.eFlags & EF_ZOOMING) ||
			(bot->s.eFlags & EF_AIMING) &&
			(bot->s.eFlags & EF_ZOOMING)
		)
	)
	{
		bot->client->ps.eFlags &= ~EF_AIMING;
		bot->s.eFlags &= ~EF_AIMING;
	}

	if ( !AIMOD_AI_Think( bot) )
	{
		return;
	}

	if ( bot->client->sess.playerType )
	{
		pc = bot->client->sess.playerType;
	}

	ucmd = &bot->client->pers.cmd;

	// think again baby
	bot->nextthink = level.time + FRAMETIME;
	if ( bot->client->ps.persistant[PERS_SCORE] )
	{
		if ( bot->client->ps.persistant[PERS_SCORE] < 0 )
		{						// Bots should never go negative.
			bot->client->ps.persistant[PERS_SCORE] = 0;
		}
	}

	if ( bot->client->ps.pm_flags && bot->suicide_timeout )
	{
		if ( !(bot->client->ps.pm_flags & PMF_LIMBO) && bot->suicide_timeout == 0 )
		{
			AI_ResetJob( bot );
			AIMOD_Player_Special_Needs_Clear( bot->s.clientNum );
			AddScore( bot, 3 ); // Give them 3 extra score points for the suicide.
			G_Damage( bot, bot, bot, bot->r.currentAngles, bot->s.origin, 99999, DAMAGE_NO_PROTECTION, MOD_SUICIDE );
			bot->suicide_timeout = 1;
		}
	}

	Check_Bot_Needs( bot );		//AIMod medic/ammo calling.
}


/**
 * Voorkomt disconnects door "servercommand overflow"
 * wel met 50 bots crashte server bij level end
 * met 20 bots niet (in ctfmultidemo)
 * nog eens getest, crash lijkt opgelost te zijn.
 */
void
SOD_AI_parseServerCommand ( gentity_t *bot, qboolean show )
{
	char	buf[1024], *args;

	// catch the funky servercommand
	// ----------------------------------------------------------------
	while ( trap_BotGetServerCommand( bot->client->ps.clientNum, buf, sizeof(buf)) )
	{

		//have buf point to the command and args to the command arguments
		args = strchr( buf, ' ' );
		if ( !args )
		{
			continue;
		}

		*args++ = '\0';

		//remove color espace sequences from the arguments
		Q_CleanStr( args );
		if ( !Q_stricmp( buf, "cp ") )
		{

		}
		else if ( !Q_stricmp( buf, "cs") )
		{

		}
		else if ( !Q_stricmp( buf, "print") )
		{

		}
		else if ( !Q_stricmp( buf, "chat") )
		{

		}
		else if ( !Q_stricmp( buf, "tchat") )
		{

		}
		else if ( !Q_stricmp( buf, "scores") )
		{

		}
		else if ( !Q_stricmp( buf, "clientLevelShot") )
		{

		}

		if ( show )
		{
			G_Printf( "%s %s\n", buf, args );
		}
	}
}


/* */
void
SOD_AI_Soldier ( gentity_t *soldier )
{

}


/* */
void
SOD_AI_Engineer ( gentity_t *engineer )
{

} extern void	AIMOD_MOVEMENT_MoveToGoal ( gentity_t *bot, usercmd_t *ucmd );


/* */
void
AI_SetJob ( gentity_t *bot, int jobType )
{
	next_weapon_change[bot->s.clientNum] = level.time;
	bot->movetarget = NULL;
	AI_Job_Action[bot->s.clientNum] = jobType;
	AI_Medic_Time[bot->s.clientNum] = level.time + 4000;
}


/* */
void
AI_ResetJob ( gentity_t *bot )
{
	next_weapon_change[bot->s.clientNum] = level.time;
	bot->movetarget = NULL;
	AI_Job_Action[bot->s.clientNum] = BOT_JOB_NONE;
	AI_Medic_Time[bot->s.clientNum] = 0;
}


/* */
int
JobType ( gentity_t *bot )
{
	if ( !AI_Job_Action[bot->s.clientNum] )
	{
		AI_ResetJob( bot );
		return ( 0 );
	}

	return ( AI_Job_Action[bot->s.clientNum] );
}


/* */
void
NowHeal ( gentity_t *bot )
{
	AI_Job_Action[bot->s.clientNum] = BOT_JOB_MEDKIT;
}


/* */
void
LostBot_Set ( gentity_t *bot )
{
	AI_Job_Action[bot->s.clientNum] = BOT_JOB_LOST;
}


/* */
void
SOD_AI_Medic ( gentity_t *medic )
{
#ifndef __NO_MEDIC_WAYPOINTING__
	usercmd_t	*ucmd;
	int			action = 0;

	//if (trap_Milliseconds() - level.frameTime > MAX_BOT_THINKFRAME)
	//	return; // Try next frame!
	ucmd = &medic->client->pers.cmd;
	if ( medic->health <= 0 )
	{					// Medic is dead... Do nothing.
		return;
	}

	if ( medic->enemy ) // Deal with enimy first.
	{
		AI_ResetJob( medic );
		Set_Best_AI_weapon( medic );
		return;
	}
	else if ( medic->movetarget )
	{
		if
		(
			AI_Job_Action[medic->client->ps.clientNum] == BOT_JOB_SYRINGE &&
			medic->client->ps.weapon == WP_MEDIC_SYRINGE &&
			medic->movetarget->health > 0
		)
		{
			AI_ResetJob( medic );
			Set_Best_AI_weapon( medic );
			return;
		}
		else if
			(
				AI_Job_Action[medic->client->ps.clientNum] == BOT_JOB_MEDKIT &&
				(
					medic->movetarget->health >= medic->movetarget->client->ps.stats[STAT_MAX_HEALTH] -
					10 ||
					medic->movetarget->client->ps.pm_flags & PMF_LIMBO
				)
			)
		{
			AI_ResetJob( medic );
			Set_Best_AI_weapon( medic );
			return;
		}
		else
		{
			if
			(
				AI_Job_Action[medic->client->ps.clientNum] == BOT_JOB_SYRINGE &&
				VectorDistance( medic->r.currentOrigin, medic->movetarget->r.currentOrigin) < 96
			)			// Syringe.
			{
				if (medic->client->ps.weapon != WP_MEDIC_SYRINGE)
				{
					ucmd->weapon = WP_MEDIC_SYRINGE;
					next_weapon_change[medic->s.clientNum] = level.time + 2000;
				}
			}
			else if
				(
					AI_Job_Action[medic->client->ps.clientNum] == BOT_JOB_MEDKIT &&
					VectorDistance( medic->r.currentOrigin, medic->movetarget->r.currentOrigin) < 96
				)		// medpack
			{
				if (medic->client->ps.weapon != WP_MEDKIT)
				{
					ucmd->weapon = WP_MEDKIT;
					next_weapon_change[medic->s.clientNum] = level.time + 2000;
				}

				if ( medic->movetarget->r.svFlags & SVF_BOT )
				{		// Tell our target to wait for packs...
					medic->movetarget->bot_wait_for_supplies = level.time + 2000;
				}
			}
			else
			{
				Set_Best_AI_weapon( medic );
			}
		}
	}
	else
	{
		Set_Best_AI_weapon( medic );
	}

	action = AIMOD_AI_SetMedicAction( medic, ucmd );
	if ( action == 1 )	// Medkit Job...
	{
		AI_Job_Action[medic->client->ps.clientNum] = BOT_JOB_MEDKIT;	// Medkit Job...	
	}
	else if ( action == 2 )												// Syringe Job...
	{
		AI_Job_Action[medic->client->ps.clientNum] = BOT_JOB_SYRINGE;	// Syringe Job...
	}
	else
	{																// Noone to heal...
		AI_ResetJob( medic );
		return;
	}

	AI_Medic_Time[medic->client->ps.clientNum] = level.time + 4000; // Set a timer for this client.. 8 secs in syringe.
#endif //__NO_MEDIC_WAYPOINTING__
}


/* */
void
SOD_AI_Lt ( gentity_t *lt )
{
	usercmd_t	*ucmd;
	ucmd = &lt->client->pers.cmd;
	if ( lt->health <= 0 )
	{																// LT is dead... Do nothing.
		return;
	}

	if ( lt->enemy )												// Deal with enimy first.
	{
		AI_ResetJob( lt );
		Set_Best_AI_weapon( lt );
		return;
	}
	else if ( lt->movetarget )
	{
		if
		(
			AI_Job_Action[lt->client->ps.clientNum] == BOT_JOB_AMMO &&
			(lt->movetarget->client->ps.pm_flags & PMF_LIMBO)
		)
		{
			AI_ResetJob( lt );
			Set_Best_AI_weapon( lt );
			return;
		}
		else
		{
			if ( VectorDistance( lt->r.currentOrigin, lt->movetarget->r.currentOrigin) < 96 )
			{

				//Set_Best_AI_weapon (lt);
				if (lt->client->ps.weapon != WP_AMMO)
				{
					ucmd->weapon = WP_AMMO;
					next_weapon_change[lt->s.clientNum] = level.time + 2000;
				}
			}
		}
	}
	else
	{
		Set_Best_AI_weapon( lt );
	}

	if ( AIMOD_AI_SetLTAction( lt, ucmd) != 2 )						// Noone found to revive.
	{
		AI_ResetJob( lt );
		return;
	}

	AI_Job_Action[lt->client->ps.clientNum] = BOT_JOB_AMMO;			// ammo job.
	AI_Medic_Time[lt->client->ps.clientNum] = level.time + 4000;	// Set a timer for this client.. 8 secs in syringe.
}

extern qboolean MyVisible ( gentity_t *self, gentity_t *other );
extern qboolean MyObjectVisible ( gentity_t *self, gentity_t *other );
extern void		BotSetViewAngles ( gentity_t *bot, float thinktime );
extern qboolean AIMOD_MOVEMENT_SpecialMoves ( gentity_t *bot, usercmd_t *ucmd );
extern void		G_UcmdMoveForDir ( gentity_t *self, usercmd_t *cmd, vec3_t dir );


/* */
qboolean
QuickEngineerScan ( gentity_t *bot )
{															// Fast surrounding area check for visible constructables/destructables...
	int			loop = 0;
	int			best = -1;

	//float best_dist = 512.0f;
	float		best_dist = 1024.0f;						// At 75->100% bot_cpu_use, we can look up to 1024 units...
	usercmd_t	*ucmd = &bot->client->pers.cmd;
	int			type = -1;
	if ( bot_cpu_use.integer < 50 )
	{
		best_dist = 256.0f;									// Don't vischeck at this level!
	}
	else if ( bot_cpu_use.integer < 75 )
	{
		best_dist = 512.0f;									// Don't check as far at this level!
	}

	if ( bot->quick_job_target && bot->quick_job_type == BOT_QUICKJOB_ENGINEER )
	{														// Already have a target...
		float		diff;
		gentity_t	*bestent = bot->quick_job_target;
		best_dist = VectorDistance( bot->r.currentOrigin, bestent->r.currentOrigin );
		diff = fabs( AngleDifference( bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]) );
		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;
		if ( VectorDistance( bestent->r.currentOrigin, bot->r.currentOrigin) < 128 )
		{													// We are already there!!! Do our job!
			if ( type == BOTGOAL_CONSTRUCT )
			{												// A construction job...
				// Set up ideal view angles for this enemy...
				VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
				bot->move_vector[ROLL] = 0;
				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
				BotSetViewAngles( bot, 100 );
				if ( bot->bot_turn_wait_time <= level.time )
				{
					G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
				}

				if ( VectorLength( bot->client->ps.velocity) < 24 )
				{
					ucmd->upmove = 127;
				}

				if ( bot->client->ps.weapon != WP_DYNAMITE && bot->client->ps.weapon != WP_PLIERS )
				{											//bot->s.weapon = bot->client->pers.cmd.weapon = WP_PLIERS;
					if (bot->client->ps.weapon != WP_PLIERS)
					{
						ucmd->weapon = WP_PLIERS;
						next_weapon_change[bot->s.clientNum] = level.time + 3000;
					}
				}

				if ( !(bot->client->ps.eFlags & EF_CROUCHING) )
				{
					bot->client->ps.eFlags |= EF_CROUCHING;
					ucmd->upmove = -48;						// Crouch...
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}

				if ( bot->client->ps.weapon == WP_PLIERS )
				{											//FireWeapon(bot);
					ucmd->buttons |= BUTTON_ATTACK;
				}

				bot->needed_node = -1;

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;

				// set approximate ping and show values
				//-------------------------------------------------------
				bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

				//bot->client->ps.ping = 10;
				//-------------------------------------------------------
				// set bot's view angle
				//-------------------------------------------------------
				ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
				ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
				ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

				//-------------------------------------------------------
				// send command through id's code
				//-------------------------------------------------------
				ClientThink_real( bot );
				bot->nextthink = level.time + FRAMETIME;

				//-------------------------------------------------------
				// Use this with the fastthink procedure...
				bot->bot_last_ucmd = ucmd;
				return ( qtrue );
			}
			else if ( type == BOTGOAL_DESTROY )
			{												// A destruction job...
				if ( bot->client->ps.weapon != WP_DYNAMITE && bot->client->ps.weapon != WP_PLIERS )
				{											//bot->s.weapon = bot->client->pers.cmd.weapon = WP_DYNAMITE;
					if (bot->client->ps.weapon != WP_DYNAMITE)
					{
						ucmd->weapon = WP_DYNAMITE;
						next_weapon_change[bot->s.clientNum] = level.time + 3000;
					}
				}

				if ( !(bot->client->ps.eFlags & EF_CROUCHING) )
				{
					bot->client->ps.eFlags |= EF_CROUCHING;
					ucmd->upmove = -48;						// Crouch...
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;

				// AIMOD_MOVEMENT_SpecialMoves can handle this request... Just let it know we got an engie job...
				bot->beStillEng = level.time + 100;
				if ( AIMOD_MOVEMENT_SpecialMoves( bot, ucmd) )
				{
					return ( qtrue );
				}
			}
		}

		/*		if (diff > 90.0f)
		{// Delay large turns...
			BotSetViewAngles(bot, 100);
		}
		else
		{
			BotSetViewAngles(bot, 100);
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );

			if (ucmd->buttons & BUTTON_WALKING)
				ucmd->forwardmove = 64;
			else
				ucmd->forwardmove = 127;

			bot->bot_last_good_move_time = level.time;
		}*/

		// Set up ideal view angles for this enemy...
		VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		if ( bot->bot_turn_wait_time <= level.time )
		{
			G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
		}

		if ( VectorLength( bot->client->ps.velocity) < 24 )
		{
			ucmd->upmove = 127;
		}

		bot->quick_job_target = bestent;

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		//bot->client->ps.ping = 10;
		//-------------------------------------------------------
		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		return ( qtrue );
	}
	else if ( bot->quick_job_target )
	{
		return ( qfalse );
	}

	if ( bot->next_quick_job_scan > level.time )
	{
		return ( qfalse );
	}

	bot->next_quick_job_scan = level.time + ( ((6 - bot->skillchoice) * 1000) + Q_TrueRand( 0, 500) );
	for ( loop = MAX_CLIENTS; loop < MAX_GENTITIES; loop++ )
	{
		gentity_t	*test = &g_entities[loop];
		gentity_t	*constructible = NULL;
		if ( !test )
		{
			continue;
		}

		constructible = G_IsConstructible( bot->client->sess.sessionTeam, test );
		if ( constructible != NULL )
		{
			type = BOTGOAL_CONSTRUCT;
		}
		else if ( test->s.eType == ET_EXPLOSIVE )
		{
			if ( test->constructibleStats.weaponclass == 1 )
			{
				type = BOTGOAL_DESTROY;
			}
			else if ( test->constructibleStats.weaponclass == 2 )
			{
				if ( test->parent && test->parent->s.eType == ET_OID_TRIGGER )
				{
					if
					(
						(
							(bot->client->sess.sessionTeam == TEAM_AXIS) &&
							(test->parent->spawnflags & ALLIED_OBJECTIVE)
						) ||
						(
							(bot->client->sess.sessionTeam == TEAM_ALLIES) &&
							(test->parent->spawnflags & AXIS_OBJECTIVE)
						)
					)
					{
						type = BOTGOAL_DESTROY;
					}
					else
					{
						continue;
					}
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}
		else
		{
			continue;
		}

		if ( VectorDistance( bot->r.currentOrigin, test->r.currentOrigin) > best_dist )
		{
			continue;
		}

		if ( BAD_WP_Height( bot->r.currentOrigin, test->r.currentOrigin) )
		{
			continue;
		}

		//		if (best_dist > 256) // Try to conserve on visual traces... Only check if its a few frames away...
		if ( !MyObjectVisible( bot, test) )
		{
			continue;
		}

		best_dist = VectorDistance( bot->r.currentOrigin, test->r.currentOrigin );
		best = test->s.number;
	}

	if ( best > 0 )
	{
		float	diff;
		bot->quick_job_target = &g_entities[best];
		bot->quick_job_type = BOT_QUICKJOB_ENGINEER;

		// Set up ideal view angles for this node...
		VectorSubtract( g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		bot->bot_ideal_view_angles[2] = 0;					// Always look straight.. Don't look up or down at nodes...
		diff = fabs( AngleDifference( bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]) );
		if ( VectorDistance( g_entities[best].r.currentOrigin, bot->r.currentOrigin) < 128 )
		{													// We are already there!!! Do our job!
			if ( type == BOTGOAL_CONSTRUCT )
			{												// A construction job...
				/*if (diff > 90.0f)
				{// Delay large turns...
					BotSetViewAngles(bot, 100);
				}
				else
				{
					BotSetViewAngles(bot, 100);
					VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );

					if (ucmd->buttons & BUTTON_WALKING)
						ucmd->forwardmove = 64;
					else
						ucmd->forwardmove = 127;

					bot->bot_last_good_move_time = level.time;
				}*/

				// Set up ideal view angles for this enemy...
				VectorSubtract( g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
				bot->move_vector[ROLL] = 0;
				vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
				BotSetViewAngles( bot, 100 );
				if ( bot->bot_turn_wait_time <= level.time )
				{
					G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
				}

				if ( VectorLength( bot->client->ps.velocity) < 24 )
				{
					ucmd->upmove = 127;
				}

				if ( bot->client->ps.weapon != WP_DYNAMITE && bot->client->ps.weapon != WP_PLIERS )
				{											//bot->s.weapon = bot->client->pers.cmd.weapon = WP_PLIERS;
					if (bot->client->ps.weapon != WP_PLIERS)
					{
						ucmd->weapon = WP_PLIERS;
						next_weapon_change[bot->s.clientNum] = level.time + 3000;
					}
				}

				if ( !(bot->client->ps.eFlags & EF_CROUCHING) )
				{
					bot->client->ps.eFlags |= EF_CROUCHING;
					ucmd->upmove = -48;						// Crouch...
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}

				if ( bot->client->ps.weapon == WP_PLIERS )
				{											//FireWeapon(bot);
					ucmd->buttons |= BUTTON_ATTACK;
				}

				bot->needed_node = -1;

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;

				// set approximate ping and show values
				//-------------------------------------------------------
				bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

				//bot->client->ps.ping = 10;
				//-------------------------------------------------------
				// set bot's view angle
				//-------------------------------------------------------
				ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
				ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
				ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

				//-------------------------------------------------------
				// send command through id's code
				//-------------------------------------------------------
				ClientThink_real( bot );
				bot->nextthink = level.time + FRAMETIME;

				//-------------------------------------------------------
				// Use this with the fastthink procedure...
				bot->bot_last_ucmd = ucmd;
				return ( qtrue );
			}
			else if ( type == BOTGOAL_DESTROY )
			{												// A destruction job...
				if ( bot->client->ps.weapon != WP_DYNAMITE && bot->client->ps.weapon != WP_PLIERS )
				{											//bot->s.weapon = bot->client->pers.cmd.weapon = WP_DYNAMITE;
					if (bot->client->ps.weapon != WP_DYNAMITE)
					{
						ucmd->weapon = WP_DYNAMITE;
						next_weapon_change[bot->s.clientNum] = level.time + 3000;
					}
				}

				if ( !(bot->client->ps.eFlags & EF_CROUCHING) )
				{
					bot->client->ps.eFlags |= EF_CROUCHING;
					ucmd->upmove = -48;						// Crouch...
					bot->client->ps.pm_flags |= PMF_DUCKED;
				}

				// Set our last good move time to now...
				bot->bot_last_good_move_time = level.time;

				// AIMOD_MOVEMENT_SpecialMoves can handle this request... Just let it know we got an engie job...
				bot->beStillEng = level.time + 100;
				if ( AIMOD_MOVEMENT_SpecialMoves( bot, ucmd) )
				{
					return ( qtrue );
				}
			}
		}

		/*		if (diff > 90.0f)
		{// Delay large turns...
			BotSetViewAngles(bot, 100);
		}
		else
		{
			BotSetViewAngles(bot, 100);
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );

			if (ucmd->buttons & BUTTON_WALKING)
				ucmd->forwardmove = 64;
			else
				ucmd->forwardmove = 127;

			bot->bot_last_good_move_time = level.time;
		}*/

		// Set up ideal view angles for this enemy...
		VectorSubtract( g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		if ( bot->bot_turn_wait_time <= level.time )
		{
			G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
		}

		if ( VectorLength( bot->client->ps.velocity) < 24 )
		{
			ucmd->upmove = 127;
		}

		bot->quick_job_target = &g_entities[best];

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		//bot->client->ps.ping = 10;
		//-------------------------------------------------------
		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}
	else
	{
		bot->quick_job_target = NULL;
		bot->quick_job_type = BOT_QUICKJOB_NONE;
		return ( qfalse );
	}
}


/* */
qboolean
QuickHealthScan ( gentity_t *bot )
{															// Fast surrounding area check for visible health...
	int			loop = 0;
	int			best = -1;

	//float best_dist = 128.0f;
	float		best_dist = 1024.0f;						// At 75->100% bot_cpu_use, we can look up to 1024 units...
	usercmd_t	*ucmd = &bot->client->pers.cmd;
	if ( bot_cpu_use.integer < 50 )
	{
		best_dist = 256.0f;									// Don't vischeck at this level!
	}
	else if ( bot_cpu_use.integer < 75 )
	{
		best_dist = 512.0f;									// Don't check as far at this level!
	}

#ifdef __VEHICLES__
	if ( (bot->client->ps.eFlags & EF_TANK) )
	{
		return ( qfalse );
	}
#endif //__VEHICLES__
	if ( bot->quick_job_target && bot->quick_job_type == BOT_QUICKJOB_GETMEDKIT )
	{														// Already have a target...
		//		float diff;
		gentity_t	*bestent = bot->quick_job_target;
		best_dist = VectorDistance( bot->r.currentOrigin, bestent->r.currentOrigin );
		if ( bot->health >= bot->client->ps.stats[STAT_MAX_HEALTH] || !MyObjectVisible( bot, bot->quick_job_target) )
		{
			bot->quick_job_target = NULL;
			bot->quick_job_type = BOT_QUICKJOB_NONE;
			return ( qfalse );
		}

		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;

		// Set up ideal view angles for this node...

		/*VectorSubtract (g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
		vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
		bot->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

		diff = fabs(AngleDifference(bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]));
	
		if (diff > 90.0f)
		{// Delay large turns...
			BotSetViewAngles(bot, 100);
		}
		else
		{
			BotSetViewAngles(bot, 100);
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );

			if (ucmd->buttons & BUTTON_WALKING)
				ucmd->forwardmove = 64;
			else
				ucmd->forwardmove = 127;

			bot->bot_last_good_move_time = level.time;
		}*/

		// Set up ideal view angles for this enemy...
		VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		if ( bot->bot_turn_wait_time <= level.time )
		{
			G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
		}

		if ( VectorLength( bot->client->ps.velocity) < 24 )
		{
			ucmd->upmove = 127;
		}

		bot->quick_job_target = bestent;

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		//bot->client->ps.ping = 10;
		//-------------------------------------------------------
		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		return ( qtrue );
	}
	else if ( bot->quick_job_target )
	{
		return ( qfalse );
	}

	if ( bot->next_quick_job_scan > level.time )
	{
		return ( qfalse );
	}

	bot->next_quick_job_scan = level.time + ( ((6 - bot->skillchoice) * 1000) + Q_TrueRand( 0, 500) );
	for ( loop = MAX_CLIENTS; loop < MAX_GENTITIES; loop++ )
	{
		gentity_t	*test = &g_entities[loop];
		if ( !test )
		{
			continue;
		}

		if ( test->item )
		{
			if ( test->item->giType != IT_HEALTH )
			{
				continue;
			}
		}
		else if ( Q_stricmp( test->classname, "misc_cabinet_health") )
		{
			continue;
		}

		if ( VectorDistance( bot->r.currentOrigin, test->r.currentOrigin) > best_dist )
		{
			continue;
		}

		if ( BAD_WP_Height( bot->r.currentOrigin, test->r.currentOrigin) )
		{
			continue;
		}

		//if (best_dist > 256)
		if ( !MyObjectVisible( bot, test) )
		{
			continue;
		}

		best_dist = VectorDistance( bot->r.currentOrigin, test->r.currentOrigin );
		best = test->s.number;
	}

	if ( best > 0 )
	{

		//		float diff;
		bot->quick_job_target = &g_entities[best];
		bot->quick_job_type = BOT_QUICKJOB_GETMEDKIT;

		/*
		// Set up ideal view angles for this node...
		VectorSubtract (g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
		vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
		bot->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

		diff = fabs(AngleDifference(bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]));
	
		if (diff > 90.0f)
		{// Delay large turns...
			BotSetViewAngles(bot, 100);
		}
		else
		{
			BotSetViewAngles(bot, 100);
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );

			if (ucmd->buttons & BUTTON_WALKING)
				ucmd->forwardmove = 64;
			else
				ucmd->forwardmove = 127;

			bot->bot_last_good_move_time = level.time;
		}*/

		// Set up ideal view angles for this enemy...
		VectorSubtract( g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		if ( bot->bot_turn_wait_time <= level.time )
		{
			G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
		}

		if ( VectorLength( bot->client->ps.velocity) < 24 )
		{
			ucmd->upmove = 127;
		}

		bot->quick_job_target = &g_entities[best];
		bot->quick_job_type = BOT_QUICKJOB_GETMEDKIT;

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		//bot->client->ps.ping = 10;
		//-------------------------------------------------------
		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}
	else
	{
		bot->quick_job_target = NULL;
		bot->quick_job_type = BOT_QUICKJOB_NONE;
		return ( qfalse );
	}
}

extern int	next_drop[MAX_CLIENTS];


/* */
qboolean
QuickMedicScan ( gentity_t *bot )
{															// Fast surrounding area check for visible revive/heal targets...
	int			loop = 0;
	int			best = -1;
	float		best_dist = 1024.0f;						// At 75->100% bot_cpu_use, we can look up to 1024 units...
	usercmd_t	*ucmd = &bot->client->pers.cmd;
	if ( bot->client->sess.playerType != PC_MEDIC )
	{
		return ( qfalse );
	}

#ifdef __VEHICLES__
	if ( (bot->client->ps.eFlags & EF_TANK) )
	{
		return ( qfalse );
	}
#endif //__VEHICLES__
	if ( bot_cpu_use.integer < 50 )
	{
		best_dist = 256.0f;									// Don't vischeck at this level!
	}
	else if ( bot_cpu_use.integer < 75 )
	{
		best_dist = 512.0f;									// Don't check as far at this level!
	}

	if ( bot->movetarget )
	{
		return ( qfalse );									// UQ1: !!!Test!!!
	}

	if
	(
		bot->quick_job_target &&
		(bot->quick_job_type == BOT_QUICKJOB_GIVEMEDKIT || bot->quick_job_type == BOT_QUICKJOB_REVIVE)
	)
	{														// Already have a target...
		gentity_t	*bestent = bot->quick_job_target;
		best_dist = VectorDistance( bot->r.currentOrigin, bestent->r.currentOrigin );
		if ( bestent->client->ps.pm_flags & PMF_LIMBO )
		{
			bot->quick_job_target = NULL;
			bot->quick_job_type = BOT_QUICKJOB_NONE;
			return ( qfalse );
		}

		if ( bestent->health >= bestent->client->ps.stats[STAT_MAX_HEALTH] || !MyVisible( bot, bot->quick_job_target) )
		{
			bot->quick_job_target = NULL;
			bot->quick_job_type = BOT_QUICKJOB_NONE;
			return ( qfalse );
		}

		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;
		if ( bestent->health > 0 )
		{
			if (bot->client->ps.weapon != WP_MEDKIT)
			{
				ucmd->weapon = WP_MEDKIT;
				next_weapon_change[bot->s.clientNum] = level.time + 2000;
			}

			bestent->bot_wait_for_supplies = level.time + 2000;
			if ( bestent->bot_wait_for_supplies_end < level.time )
			{
				bestent->bot_wait_for_supplies_end = level.time + 8000;
			}
		}
		else
		{
			if (bot->client->ps.weapon != WP_MEDIC_SYRINGE)
			{
				ucmd->weapon = WP_MEDIC_SYRINGE;
				next_weapon_change[bot->s.clientNum] = level.time + 3000;
			}
		}

		if ( best_dist < 64 /*80*/ && bot->client->ps.weapon == WP_MEDKIT )
		{													// Medkit job...
			ucmd->upmove = -127;							// Crouch?

			// Set up ideal view angles for this target...
			VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
			bot->move_vector[ROLL] = 0;
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			ucmd->upmove = -48;								// Crouch...
			bot->client->ps.pm_flags |= PMF_DUCKED;
			if ( !next_drop[bot->s.number] )
			{
				next_drop[bot->s.number] = level.time - 1;
			}

			if ( next_drop[bot->s.number] < level.time )
			{
				next_drop[bot->s.number] = level.time + 800;
				if ( g_realmedic.integer )
				{
					ucmd->buttons = BUTTON_ACTIVATE;
				}
				else
				{
					ucmd->buttons = BUTTON_ATTACK;
				}

				//FireWeapon( bot );
			}

			bot->quick_job_target = bestent;
		}
		else if ( best_dist < 32 /*48*/ /*80*/ && bot->client->ps.weapon == WP_MEDIC_SYRINGE )
		{													// Revive job...
			//			vec3_t eyes;
			//BOT_GetMuzzlePoint( bot );
			//VectorCopy(bot->bot_muzzlepoint, eyes);
			//VectorSubtract (bestent->r.currentOrigin, eyes, bot->move_vector);
			//vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
			//VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
			// Set up ideal view angles for this target...
			VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
			bot->move_vector[ROLL] = -90;
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			if (bot->client->ps.weapon != WP_MEDIC_SYRINGE)
			{
				ucmd->weapon = WP_MEDIC_SYRINGE;
				next_weapon_change[bot->s.clientNum] = level.time + 3000;
			}

			ucmd->upmove = -48;								// Crouch...
			bot->client->ps.pm_flags |= PMF_DUCKED;
			ucmd->buttons = BUTTON_ATTACK;
			bot->quick_job_target = bestent;
		}
		else
		{													// Set up ideal view angles for this target... And move to it...
			//			float diff;

			/*VectorSubtract (bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
			vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
		
			if ( best_dist >= 64 )
				bot->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

			diff = fabs(AngleDifference(bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]));
	
			if (diff > 90.0f)
			{// Delay large turns...
				BotSetViewAngles(bot, 100);
			}
			else
			{
				BotSetViewAngles(bot, 100);
				VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
		
				if (ucmd->buttons & BUTTON_WALKING)
					ucmd->forwardmove = 64;
				else
					ucmd->forwardmove = 127;
	
				bot->bot_last_good_move_time = level.time;
			}

			if (VectorLength(bot->client->ps.velocity) < 24)
				ucmd->upmove = 127;*/

			// Set up ideal view angles for this enemy...
			VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
			bot->move_vector[ROLL] = 0;
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			if ( bot->bot_turn_wait_time <= level.time )
			{
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
			}

			if ( VectorLength( bot->client->ps.velocity) < 24 )
			{
				ucmd->upmove = 127;
			}

			bot->quick_job_target = bestent;
		}

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		//bot->client->ps.ping = 10;
		//-------------------------------------------------------
		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		return ( qtrue );
	}
	else if ( bot->quick_job_target )
	{
		return ( qfalse );
	}

	if ( bot->next_quick_job_scan > level.time )
	{
		return ( qfalse );
	}

	if (next_weapon_change[bot->s.clientNum] > level.time)
	{// Wait!!!
		return ( qfalse );
	}

	bot->next_quick_job_scan = level.time + ( ((6 - bot->skillchoice) * 1000) + Q_TrueRand( 0, 500) );
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*test = &g_entities[loop];
		if ( !test )
		{
			continue;
		}

		if ( !test->client || test->client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		if ( test == bot )
		{
			continue;
		}

		if ( test->bot_dont_give_supplies_end > level.time )
		{
			continue;
		}

		if ( test->bot_wait_for_supplies_end < level.time && test->bot_wait_for_supplies_end > level.time - 6000 )
		{
			test->bot_dont_give_supplies_end = level.time + 20000;
			continue;										// Given enough to this guy... Share the supplies!
		}

		if ( !OnSameTeam( bot, test) )
		{
			continue;
		}

		if ( test->health >= test->client->ps.stats[STAT_MAX_HEALTH] )
		{
			continue;										// They have full health.. Can't target them...
		}

		if ( test->client->ps.pm_flags & PMF_LIMBO )
		{
			continue;										// Can't revive someone who is in limbo, so ignore...
		}

		if ( best >= 0 && test->health > 0 && g_entities[best].health <= 0 )
		{
			continue;										// Revives first ALWAYS!
		}

		if ( VectorDistance( bot->r.currentOrigin, test->r.currentOrigin) > best_dist )
		{
			continue;
		}

		if ( BAD_WP_Height( bot->r.currentOrigin, test->r.currentOrigin) )
		{
			continue;
		}

#ifdef __VEHICLES__
		if ( (test->client->ps.eFlags & EF_TANK) )
		{
			continue;
		}
#endif //__VEHICLES__

		//if (best_dist > 256)
		if ( !MyVisible( bot, test) )
		{
			continue;
		}

		best_dist = VectorDistance( bot->r.currentOrigin, test->r.currentOrigin );
		best = test->s.number;
	}

	if ( best >= 0 )
	{
		gentity_t	*bestent = &g_entities[best];

		//		float diff;
		if ( bestent->health > 0 )
		{
			if (bot->client->ps.weapon != WP_MEDKIT)
			{
				ucmd->weapon = WP_MEDKIT;
				next_weapon_change[bot->s.clientNum] = level.time + 2000;
			}

			bestent->bot_wait_for_supplies = level.time + 2000;
			if ( bestent->bot_wait_for_supplies_end < level.time )
			{
				bestent->bot_wait_for_supplies_end = level.time + 8000;
			}
		}
		else
		{
			if (bot->client->ps.weapon != WP_MEDIC_SYRINGE)
			{
				ucmd->weapon = WP_MEDIC_SYRINGE;
				next_weapon_change[bot->s.clientNum] = level.time + 3000;
			}
		}

		if ( best_dist < 64 /*80*/ && bot->client->ps.weapon == WP_MEDKIT )
		{													// Medkit job...
			ucmd->upmove = -127;							// Crouch?

			// Set up ideal view angles for this target...
			VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
			bot->move_vector[ROLL] = 0;
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			ucmd->upmove = -127;							// Crouch?
			if ( !next_drop[bot->s.number] )
			{
				next_drop[bot->s.number] = level.time - 1;
			}

			if ( next_drop[bot->s.number] < level.time )
			{
				next_drop[bot->s.number] = level.time + 800;
				if ( g_realmedic.integer )
				{
					ucmd->buttons = BUTTON_ACTIVATE;
				}
				else
				{
					ucmd->buttons = BUTTON_ATTACK;
				}
			}

			bot->quick_job_target = bestent;
			bot->quick_job_type = BOT_QUICKJOB_GIVEMEDKIT;
		}
		else if ( best_dist < 32 /*48*/ /*80*/ && bot->client->ps.weapon == WP_MEDIC_SYRINGE )
		{													// Revive job...
			vec3_t	eyes;
			BOT_GetMuzzlePoint( bot );
			VectorCopy( bot->bot_muzzlepoint, eyes );
			VectorSubtract( bestent->r.currentOrigin, eyes, bot->move_vector );
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
			
			if (bot->client->ps.weapon != WP_MEDIC_SYRINGE)
			{
				ucmd->weapon = WP_MEDIC_SYRINGE;
				next_weapon_change[bot->s.clientNum] = level.time + 3000;
			}

			ucmd->upmove = -48;								// Crouch...
			bot->client->ps.pm_flags |= PMF_DUCKED;
			ucmd->buttons = BUTTON_ATTACK;
			bot->quick_job_target = bestent;
			bot->quick_job_type = BOT_QUICKJOB_REVIVE;
		}
		else
		{													// Set up ideal view angles for this target... And move to it...
			/*VectorSubtract (bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
			vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
		
			if ( best_dist >= 64 )
				bot->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

			diff = fabs(AngleDifference(bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]));
	
			if (diff > 90.0f)
			{// Delay large turns...
				BotSetViewAngles(bot, 100);
			}
			else
			{
				BotSetViewAngles(bot, 100);
				VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
		
				if (ucmd->buttons & BUTTON_WALKING)
					ucmd->forwardmove = 64;
				else
					ucmd->forwardmove = 127;
	
				bot->bot_last_good_move_time = level.time;
			}

			if (VectorLength(bot->client->ps.velocity) < 24)
				ucmd->upmove = 127;*/

			// Set up ideal view angles for this enemy...
			VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
			bot->move_vector[ROLL] = 0;
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			if ( bot->bot_turn_wait_time <= level.time )
			{
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
			}

			if ( VectorLength( bot->client->ps.velocity) < 24 )
			{
				ucmd->upmove = 127;
			}

			bot->quick_job_target = bestent;
			if ( bot->client->ps.weapon == WP_MEDKIT || ucmd->weapon == WP_MEDKIT )
			{
				bot->quick_job_type = BOT_QUICKJOB_GIVEMEDKIT;
			}
			else
			{
				bot->quick_job_type = BOT_QUICKJOB_REVIVE;
			}
		}

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		//bot->client->ps.ping = 10;
		//-------------------------------------------------------
		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}
	else
	{
		bot->quick_job_target = NULL;
		bot->quick_job_type = BOT_QUICKJOB_NONE;
		return ( qfalse );
	}
}

extern qboolean AIMOD_SP_MOVEMENT_Is_CoverSpot_Weapon ( int weapon );

/* */
qboolean
QuickFieldOpsScan ( gentity_t *bot )
{															// Fast surrounding area check for visible revive/heal targets...
	int			loop = 0;
	int			best = -1;
	float		best_dist = 1024.0f;						// At 75->100% bot_cpu_use, we can look up to 1024 units...
	usercmd_t	*ucmd = &bot->client->pers.cmd;
	if ( bot->client->sess.playerType != PC_FIELDOPS )
	{
		return ( qfalse );
	}

	if ( bot_cpu_use.integer < 50 )
	{
		best_dist = 256.0f;									// Don't vischeck at this level!
	}
	else if ( bot_cpu_use.integer < 75 )
	{
		best_dist = 512.0f;									// Don't check as far at this level!
	}

#ifdef __VEHICLES__
	if ( (bot->client->ps.eFlags & EF_TANK) )
	{
		return ( qfalse );
	}
#endif //__VEHICLES__

	if ( bot->quick_job_target && bot->quick_job_type == BOT_QUICKJOB_GIVEAMMO )
	{														// Already have a target...
		gentity_t	*bestent = bot->quick_job_target;
		best_dist = VectorDistance( bot->r.currentOrigin, bestent->r.currentOrigin );

		if (!AIMOD_SP_MOVEMENT_Is_CoverSpot_Weapon(bestent->client->ps.weapon))
		{
			bot->quick_job_target = NULL;
			bot->quick_job_type = BOT_QUICKJOB_NONE;
			return ( qfalse );
		}

		if
		(
			(bestent->client->ps.pm_flags & PMF_LIMBO) ||
			bestent->health <= 0 ||
			!MyVisible( bot, bot->quick_job_target)
		)
		{
			bot->quick_job_target = NULL;
			bot->quick_job_type = BOT_QUICKJOB_NONE;
			return ( qfalse );
		}

		if ( !AIMOD_NeedAmmoForWeapon( &bestent->client->ps, bestent->client->ps.weapon) )
		{
			bot->quick_job_target = NULL;
			bot->quick_job_type = BOT_QUICKJOB_NONE;
			return ( qfalse );
		}

		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;
		if (bot->client->ps.weapon != WP_AMMO)
		{
			ucmd->weapon = WP_AMMO;
			next_weapon_change[bot->s.clientNum] = level.time + 2000;
		}

		bestent->bot_wait_for_supplies = level.time + 2000;
		if ( bestent->bot_wait_for_supplies_end < level.time )
		{
			bestent->bot_wait_for_supplies_end = level.time + 8000;
		}

		if ( best_dist < 64 /*80*/ && bot->client->ps.weapon == WP_AMMO )
		{													// Medkit job...
			ucmd->upmove = -127;							// Crouch?

			// Set up ideal view angles for this target...
			VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
			bot->move_vector[ROLL] = 0;
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			ucmd->upmove = -127;							// Crouch?
			if ( !next_drop[bot->s.number] )
			{
				next_drop[bot->s.number] = level.time - 1;
			}

			if ( next_drop[bot->s.number] < level.time )
			{
				next_drop[bot->s.number] = level.time + 800;
				if ( bot->client->ps.weapon == WP_AMMO )
				{
					ucmd->buttons = BUTTON_ATTACK;
				}
			}

			bot->quick_job_target = bestent;
			bot->quick_job_type = BOT_QUICKJOB_GIVEAMMO;
		}
		else
		{													// Set up ideal view angles for this target... And move to it...
			//			float diff;

			/*			VectorSubtract (bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
			vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
		
			if ( best_dist >= 64 )
				bot->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

			diff = fabs(AngleDifference(bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]));
	
			if (diff > 90.0f)
			{// Delay large turns...
				BotSetViewAngles(bot, 100);
			}
			else
			{
				BotSetViewAngles(bot, 100);
				VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
		
				if (ucmd->buttons & BUTTON_WALKING)
					ucmd->forwardmove = 64;
				else
					ucmd->forwardmove = 127;
	
				bot->bot_last_good_move_time = level.time;
			}*/

			// Set up ideal view angles for this enemy...
			VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
			bot->move_vector[ROLL] = 0;
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			if ( bot->bot_turn_wait_time <= level.time )
			{
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
			}

			if ( VectorLength( bot->client->ps.velocity) < 24 )
			{
				ucmd->upmove = 127;
			}

			bot->quick_job_target = bestent;
			bot->quick_job_type = BOT_QUICKJOB_GIVEAMMO;
		}

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		//bot->client->ps.ping = 10;
		//-------------------------------------------------------
		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		return ( qtrue );
	}
	else if ( bot->quick_job_target )
	{
		return ( qfalse );
	}

	if ( bot->movetarget )
	{
		return ( qfalse );									// UQ1: !!!Test!!!
	}

	if ( bot->next_quick_job_scan > level.time )
	{
		return ( qfalse );
	}

	if (next_weapon_change[bot->s.clientNum] > level.time)
	{// Wait!!!
		return ( qfalse );
	}

	bot->next_quick_job_scan = level.time + ( ((6 - bot->skillchoice) * 1000) + Q_TrueRand( 0, 500) );
	for ( loop = 0; loop < MAX_CLIENTS; loop++ )
	{
		gentity_t	*test = &g_entities[loop];
		if ( !test )
		{
			continue;
		}

		if ( !test->client || test->client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		if ( test == bot )
		{
			continue;
		}

		if ( test->bot_dont_give_supplies_end > level.time )
		{
			continue;
		}

		if ( test->bot_wait_for_supplies_end < level.time && test->bot_wait_for_supplies_end > level.time - 6000 )
		{
			test->bot_dont_give_supplies_end = level.time + 20000;
			continue;										// Given enough to this guy... Share the supplies!
		}

		if ( !OnSameTeam( bot, test) )
		{
			continue;
		}

		if ( test->client->ps.pm_flags & PMF_LIMBO )
		{
			continue;										// Can't revive someone who is in limbo, so ignore...
		}

		if ( VectorDistance( bot->r.currentOrigin, test->r.currentOrigin) > best_dist )
		{
			continue;
		}

		if (!AIMOD_SP_MOVEMENT_Is_CoverSpot_Weapon(test->client->ps.weapon))
		{
			continue;
		}

		//if (!AIMod_ClientNeedsAmmo( bot->s.number ))
		if ( !AIMOD_NeedAmmoForWeapon( &test->client->ps, test->client->ps.weapon) /*ClientNeedsAmmo( bot->s.number )*/ )
		{
			continue;
		}

		if ( BAD_WP_Height( bot->r.currentOrigin, test->r.currentOrigin) )
		{
			continue;
		}

#ifdef __VEHICLES__
		if ( (test->client->ps.eFlags & EF_TANK) )
		{
			continue;
		}
#endif //__VEHICLES__

		//if (best_dist > 256)
		if ( !MyVisible( bot, test) )
		{
			continue;
		}

		best_dist = VectorDistance( bot->r.currentOrigin, test->r.currentOrigin );
		best = test->s.number;
	}

	if ( best >= 0 )
	{
		gentity_t	*bestent = &g_entities[best];

		//		float diff;
		bot->quick_job_target = bestent;
		bot->quick_job_type = BOT_QUICKJOB_GIVEAMMO;
		if (bot->client->ps.weapon != WP_AMMO)
		{
			ucmd->weapon = WP_AMMO;
			next_weapon_change[bot->s.clientNum] = level.time + 2000;
		}

		bestent->bot_wait_for_supplies = level.time + 2000;
		if ( bestent->bot_wait_for_supplies_end < level.time )
		{
			bestent->bot_wait_for_supplies_end = level.time + 8000;
		}

		if ( best_dist < 64 /*80*/ && bot->client->ps.weapon == WP_AMMO )
		{													// Medkit job...
			ucmd->upmove = -127;							// Crouch?

			// Set up ideal view angles for this target...
			VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
			bot->move_vector[ROLL] = 0;
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			ucmd->upmove = -127;							// Crouch?
			if ( !next_drop[bot->s.number] )
			{
				next_drop[bot->s.number] = level.time - 1;
			}

			if ( next_drop[bot->s.number] < level.time )
			{
				next_drop[bot->s.number] = level.time + 800;
				if ( bot->client->ps.weapon == WP_AMMO )
				{
					ucmd->buttons = BUTTON_ATTACK;
				}
			}

			bot->quick_job_target = bestent;
		}
		else
		{													// Set up ideal view angles for this target... And move to it...
			/*VectorSubtract (bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
			vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
		
			if ( best_dist >= 64 )
				bot->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

			diff = fabs(AngleDifference(bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]));
	
			if (diff > 90.0f)
			{// Delay large turns...
				BotSetViewAngles(bot, 100);
			}
			else
			{
				BotSetViewAngles(bot, 100);
				VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );
		
				if (ucmd->buttons & BUTTON_WALKING)
					ucmd->forwardmove = 64;
				else
					ucmd->forwardmove = 127;
	
				bot->bot_last_good_move_time = level.time;
			}

			if (VectorLength(bot->client->ps.velocity) < 24)
				ucmd->upmove = 127;*/

			// Set up ideal view angles for this enemy...
			VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
			bot->move_vector[ROLL] = 0;
			vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
			BotSetViewAngles( bot, 100 );
			if ( bot->bot_turn_wait_time <= level.time )
			{
				G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
			}

			if ( VectorLength( bot->client->ps.velocity) < 24 )
			{
				ucmd->upmove = 127;
			}
		}

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		//bot->client->ps.ping = 10;
		//-------------------------------------------------------
		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}
	else
	{
		bot->quick_job_target = NULL;
		bot->quick_job_type = BOT_QUICKJOB_NONE;
		return ( qfalse );
	}
}

extern qboolean G_CanPickupWeapon ( weapon_t weapon, gentity_t *ent );


/* */
qboolean
QuickAmmoScan ( gentity_t *bot )
{															// Fast surrounding area check for visible ammo...
	int			loop = 0;
	int			best = -1;

	//float best_dist = 128.0f;
	float		best_dist = 1024.0f;						// At 75->100% bot_cpu_use, we can look up to 1024 units...
	usercmd_t	*ucmd = &bot->client->pers.cmd;
	if ( bot_cpu_use.integer < 50 )
	{
		best_dist = 256.0f;									// Don't vischeck at this level!
	}
	else if ( bot_cpu_use.integer < 75 )
	{
		best_dist = 512.0f;									// Don't check as far at this level!
	}

#ifdef __VEHICLES__
	if ( (bot->client->ps.eFlags & EF_TANK) )
	{
		return ( qfalse );
	}
#endif //__VEHICLES__
	if ( bot->weaponchoice == WP_FLAMETHROWER && bot->client->ps.ammoclip[bot->weaponchoice] > 50 )
	{
		return ( qfalse );									// Flamers should only do this if they have not much ammo in their clip...
	}

	if ( bot->enemy && bot->enemy->health > 0 )
	{
		if ( bot->client->ps.ammo[bot->weaponchoice] != 0 && bot->client->ps.ammoclip[bot->weaponchoice] != 0 )
		{
			if ( bot->bot_enemy_visible_time > level.time /*MyVisible(bot, bot->enemy)*/ )
			{
				return ( qfalse );							// Don't look if we have an enemy to deal with!
			}
		}
	}

	if ( bot->quick_job_target && bot->quick_job_type == BOT_QUICKJOB_GETAMMO )
	{														// Already have a target...
		//		float diff;
		gentity_t	*bestent = bot->quick_job_target;
		best_dist = VectorDistance( bot->r.currentOrigin, bestent->r.currentOrigin );
		if
		(
			!AIMOD_NeedAmmoForWeapon( &bot->client->ps, bot->client->ps.weapon) ||
			!MyObjectVisible( bot, bot->quick_job_target)
		)
		{
			bot->quick_job_target = NULL;
			bot->quick_job_type = BOT_QUICKJOB_NONE;
			return ( qfalse );
		}

		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;

		// Set up ideal view angles for this node...

		/*VectorSubtract (g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
		vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
		
		if ( best_dist >= 64 )
			bot->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

		diff = fabs(AngleDifference(bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]));
	
		if (diff > 90.0f)
		{// Delay large turns...
			BotSetViewAngles(bot, 100);
		}
		else
		{
			BotSetViewAngles(bot, 100);
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );

			if (ucmd->buttons & BUTTON_WALKING)
				ucmd->forwardmove = 64;
			else
				ucmd->forwardmove = 127;

			bot->bot_last_good_move_time = level.time;
		}*/

		// Set up ideal view angles for this enemy...
		VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		if ( bot->bot_turn_wait_time <= level.time )
		{
			G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
		}

		if ( VectorLength( bot->client->ps.velocity) < 24 )
		{
			ucmd->upmove = 127;
		}

		if ( bestent->item )
		{													// Pickup item...
			if
			(
				bestent->item->giTag != bot->weaponchoice &&
				G_CanPickupWeapon( bestent->item->giTag, bot) &&
				best_dist < 64
			)
			{												// Close enough to a new weapon... Switch for ours...
				Cmd_Activate_f( bot );
			}

			bot->quick_job_target = &g_entities[best];
		}

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		return ( qtrue );
	}
	else if ( bot->quick_job_target )
	{
		return ( qfalse );
	}

	if ( bot->next_quick_job_scan > level.time )
	{
		return ( qfalse );
	}

	bot->next_quick_job_scan = level.time + ( ((6 - bot->skillchoice) * 1000) + Q_TrueRand( 0, 500) );
	for ( loop = MAX_CLIENTS; loop < MAX_GENTITIES; loop++ )
	{
		gentity_t	*test = &g_entities[loop];
		if ( !test )
		{
			continue;
		}

		if ( test->item )
		{
			if ( bot->client->ps.ammo[bot->weaponchoice] == 0 && bot->client->ps.ammoclip[bot->weaponchoice] == 0 )
			{
				if
				(
					test->item->giType != IT_AMMO &&
					!(
						test->item->giType == IT_WEAPON &&
						(test->item->giTag == bot->weaponchoice || G_CanPickupWeapon( test->item->giTag, bot))
					)
				)
				{
					continue;
				}
			}
			else if
				(
					test->item->giType != IT_AMMO &&
					!(test->item->giType == IT_WEAPON && test->item->giTag == bot->weaponchoice)
				)
			{
				continue;
			}
		}
		else if ( Q_stricmp( test->classname, "misc_cabinet_supply") )
		{
			continue;
		}

		if ( VectorDistance( bot->r.currentOrigin, test->r.currentOrigin) > best_dist )
		{
			continue;
		}

		if ( BAD_WP_Height( bot->r.currentOrigin, test->r.currentOrigin) )
		{
			continue;
		}

		//if (best_dist > 256)
		if ( !MyObjectVisible( bot, test) )
		{
			continue;
		}

		best_dist = VectorDistance( bot->r.currentOrigin, test->r.currentOrigin );
		best = test->s.number;
	}

	if ( best > 0 )
	{

		//		float diff;
		bot->quick_job_target = &g_entities[best];
		bot->quick_job_type = BOT_QUICKJOB_GETAMMO;

		// Set up ideal view angles for this node...

		/*VectorSubtract (g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
		vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
		
		if ( best_dist >= 64 )
			bot->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

		diff = fabs(AngleDifference(bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]));
	
		if (diff > 90.0f)
		{// Delay large turns...
			BotSetViewAngles(bot, 100);
		}
		else
		{
			BotSetViewAngles(bot, 100);
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );

			if (ucmd->buttons & BUTTON_WALKING)
				ucmd->forwardmove = 64;
			else
				ucmd->forwardmove = 127;

			bot->bot_last_good_move_time = level.time;
		}*/

		// Set up ideal view angles for this enemy...
		VectorSubtract( g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		if ( bot->bot_turn_wait_time <= level.time )
		{
			G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
		}

		if ( VectorLength( bot->client->ps.velocity) < 24 )
		{
			ucmd->upmove = 127;
		}

		if ( g_entities[best].item )
		{													// Pickup item...
			if
			(
				g_entities[best].item->giTag != bot->weaponchoice &&
				G_CanPickupWeapon( g_entities[best].item->giTag, bot) &&
				best_dist < 64
			)
			{												// Close enough to a new weapon... Switch for ours...
				Cmd_Activate_f( bot );
			}
		}

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}
	else
	{
		bot->quick_job_target = NULL;
		bot->quick_job_type = BOT_QUICKJOB_NONE;
		return ( qfalse );
	}
}


/* */
qboolean
QuickCorpseScan ( gentity_t *bot )
{															// Fast surrounding area check for visible corpse a covert ops can steal uniform from...
#ifdef __CORPSE_SCAN__
	int			loop = 0;
	int			best = -1;

	//float best_dist = 128.0f;
	float		best_dist = 1024.0f;						// At 75->100% bot_cpu_use, we can look up to 1024 units...
	usercmd_t	*ucmd = &bot->client->pers.cmd;
	if ( bot->client->ps.powerups[PW_OPS_DISGUISED] & (1 << PW_OPS_DISGUISED) )
	{
		bot->quick_job_target = NULL;
		bot->quick_job_type = BOT_QUICKJOB_NONE;
		bot->next_corpse_scan = level.time + 30000;
		return ( qfalse );
	}

#ifdef __VEHICLES__
	if ( (bot->client->ps.eFlags & EF_TANK) )
	{
		return ( qfalse );
	}
#endif //__VEHICLES__
	if ( bot_cpu_use.integer < 50 )
	{
		best_dist = 256.0f;									// Don't vischeck at this level!
	}
	else if ( bot_cpu_use.integer < 75 )
	{
		best_dist = 512.0f;									// Don't check as far at this level!
	}

	//if (bot->enemy && bot->enemy->health > 0 && MyVisible(bot, bot->enemy))
	//	return qfalse; // Don't look if we have an enemy to deal with!
	if ( bot->enemy && bot->enemy->health > 0 )
	{
		if ( bot->bot_enemy_visible_time > level.time )
		{
			return ( qfalse );								// Don't look if we have an enemy to deal with!
		}
	}

	if ( bot->quick_job_target && bot->quick_job_type == BOT_QUICKJOB_GETCORPSE )
	{														// Already have a target...
		//		float diff;
		gentity_t	*bestent = bot->quick_job_target;
		best_dist = VectorDistance( bot->r.currentOrigin, bestent->r.currentOrigin );
		if
		(
			bot->client->ps.powerups[PW_OPS_DISGUISED] & (1 << PW_OPS_DISGUISED) ||
			!MyObjectVisible( bot, bot->quick_job_target)
		)
		{
			bot->quick_job_target = NULL;
			bot->quick_job_type = BOT_QUICKJOB_NONE;
			bot->next_corpse_scan = level.time + 30000;
			return ( qfalse );
		}

		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;

		// Set up ideal view angles for this node...

		/*VectorSubtract (g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
		vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
		
		if (best_dist >= 64)
			bot->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

		diff = fabs(AngleDifference(bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]));
	
		if (diff > 90.0f)
		{// Delay large turns...
			BotSetViewAngles(bot, 100);
		}
		else
		{
			BotSetViewAngles(bot, 100);
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );

			if (ucmd->buttons & BUTTON_WALKING)
				ucmd->forwardmove = 64;
			else
				ucmd->forwardmove = 127;

			bot->bot_last_good_move_time = level.time;

			if (best_dist < 64)
			{
				G_Printf("%s is trying to pick up a uniform from a dead enemy!\n", bot->client->pers.netname);
				Cmd_Activate_f( bot );
			}
		}*/

		// Set up ideal view angles for this enemy...
		VectorSubtract( bestent->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		if ( bot->bot_turn_wait_time <= level.time )
		{
			G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
		}

		if ( VectorLength( bot->client->ps.velocity) < 24 )
		{
			ucmd->upmove = 127;
		}

		if ( best_dist < 64 )
		{
			G_Printf( "%s is trying to pick up a uniform from a dead enemy!\n", bot->client->pers.netname );
			Cmd_Activate_f( bot );
		}

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		//bot->client->ps.ping = 10;
		//-------------------------------------------------------
		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		return ( qtrue );
	}
	else if ( bot->quick_job_target )
	{
		return ( qfalse );
	}

	if ( bot->next_corpse_scan > level.time )
	{
		return ( qfalse );
	}

	bot->next_corpse_scan = level.time + ( ((6 - bot->skillchoice) * 10000) + Q_TrueRand( 0, 500) );
	for ( loop = MAX_CLIENTS; loop < MAX_GENTITIES; loop++ )
	{
		gentity_t	*test = &g_entities[loop];
		if ( !test )
		{
			continue;
		}

		if ( test->s.eType != ET_CORPSE )
		{
			continue;
		}

		if ( BODY_TEAM( test) >= 4 || BODY_TEAM( test) == bot->client->sess.sessionTeam )
		{
			continue;
		}

		if ( VectorDistance( bot->r.currentOrigin, test->r.currentOrigin) > best_dist )
		{
			continue;
		}

		if ( BAD_WP_Height( bot->r.currentOrigin, test->r.currentOrigin) )
		{
			continue;
		}

		//if (best_dist > 256)
		if ( !MyObjectVisible( bot, test) )
		{
			continue;
		}

		best_dist = VectorDistance( bot->r.currentOrigin, test->r.currentOrigin );
		best = test->s.number;
	}

	if ( best > 0 )
	{

		//		float diff;
		bot->quick_job_target = &g_entities[best];
		bot->quick_job_type = BOT_QUICKJOB_GETCORPSE;

		// Set up ideal view angles for this node...

		/*VectorSubtract (g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector);
		vectoangles (bot->move_vector, bot->bot_ideal_view_angles);
		
		if (best_dist >= 64)
			bot->bot_ideal_view_angles[2]=0; // Always look straight.. Don't look up or down at nodes...

		diff = fabs(AngleDifference(bot->client->ps.viewangles[YAW], bot->bot_ideal_view_angles[YAW]));
	
		if (diff > 90.0f)
		{// Delay large turns...
			BotSetViewAngles(bot, 100);
		}
		else
		{
			BotSetViewAngles(bot, 100);
			VectorCopy( bot->bot_ideal_view_angles, bot->s.angles );

			if (ucmd->buttons & BUTTON_WALKING)
				ucmd->forwardmove = 64;
			else
				ucmd->forwardmove = 127;

			bot->bot_last_good_move_time = level.time;

			if (best_dist < 64)
			{
				G_Printf("%s is trying to pick up a uniform from a dead enemy!\n", bot->client->pers.netname);
				Cmd_Activate_f( bot );
			}
		}*/

		// Set up ideal view angles for this enemy...
		VectorSubtract( g_entities[best].r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		bot->move_vector[ROLL] = 0;
		vectoangles( bot->move_vector, bot->bot_ideal_view_angles );
		BotSetViewAngles( bot, 100 );
		if ( bot->bot_turn_wait_time <= level.time )
		{
			G_UcmdMoveForDir( bot, ucmd, bot->move_vector );
		}

		if ( VectorLength( bot->client->ps.velocity) < 24 )
		{
			ucmd->upmove = 127;
		}

		if ( best_dist < 64 )
		{
			G_Printf( "%s is trying to pick up a uniform from a dead enemy!\n", bot->client->pers.netname );
			Cmd_Activate_f( bot );
		}

		// set approximate ping and show values
		//-------------------------------------------------------
		bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

		//bot->client->ps.ping = 10;
		//-------------------------------------------------------
		// set bot's view angle
		//-------------------------------------------------------
		ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
		ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
		ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

		//-------------------------------------------------------
		// send command through id's code
		//-------------------------------------------------------
		ClientThink_real( bot );
		bot->nextthink = level.time + FRAMETIME;

		//-------------------------------------------------------
		// Use this with the fastthink procedure...
		bot->bot_last_ucmd = ucmd;
		bot->node_timeout = level.time + TravelTime( bot ); //6000;//4000;
		bot->bot_last_good_move_time = level.time;
		return ( qtrue );
	}
	else
	{
		bot->quick_job_target = NULL;
		bot->quick_job_type = BOT_QUICKJOB_NONE;
		return ( qfalse );
	}

#else //!__CORPSE_SCAN__
	return ( qfalse );
#endif //__CORPSE_SCAN__
}


/*
==============
AIMOD_AI_inFOV
==============
*/
qboolean
AIMOD_AI_inFOV ( vec3_t viewangles, float fov, vec3_t angles )
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

extern qboolean Is_Move_Target_Client ( int targetNum );
extern int		need_time[MAX_CLIENTS];


/* */
int
AIMOD_AI_SetMedicAction ( gentity_t *bot, usercmd_t *ucmd )
{
	int			i;
	gentity_t	*bestrevive = NULL;
	gentity_t	*tempent;
	float		bestreviveweight = 99999;
	float		reviveweight;
	vec3_t		dist;
	if ( bot->enemy )
	{
		AI_ResetJob( bot );
		Set_Best_AI_weapon( bot );
		return ( 0 );
	}

#ifdef __VEHICLES__
	if ( (bot->client->ps.eFlags & EF_TANK) )
	{
		return ( 0 );
	}
#endif //__VEHICLES__

	// Run through all clients
	// Determine best enemy target, and best friendly target
	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		tempent = &g_entities[i];
		if ( !tempent )
		{
			continue;
		}

		// if slot is not used go next
		if ( i == bot->client->ps.clientNum )
		{
			continue;
		}

		if ( !tempent->inuse )
		{
			continue;
		}

		if ( !tempent->client )
		{
			continue;
		}

#ifdef __VEHICLES__
		if ( (tempent->client->ps.eFlags & EF_TANK) )
		{
			continue;
		}
#endif //__VEHICLES__
		if ( OnSameTeam( bot, tempent) )
		{		// Revive job...
			if ( (tempent->health <= 0) && !(tempent->client->ps.pm_flags & PMF_LIMBO) )
			{
				vec3_t	eyes;
				VectorCopy( bot->client->ps.origin, eyes );
				eyes[2] += 16;
				VectorSubtract( bot->client->ps.origin, tempent->client->ps.origin, dist );
				reviveweight = VectorLength( dist );
				if ( reviveweight < 1400 )
				{
					if ( reviveweight < bestreviveweight )
					{
						bestreviveweight = reviveweight;
						bestrevive = tempent;
					}
				}
			}
			else if
				(
					tempent->health < tempent->client->ps.stats[STAT_MAX_HEALTH] - 10 &&
					!(tempent->client->ps.pm_flags & PMF_LIMBO)
				)
			{	// Medkit job...
				if ( !bestrevive || (bestrevive && bestrevive->health > 0) )
				{
					vec3_t	eyes;
					VectorCopy( bot->client->ps.origin, eyes );
					eyes[2] += 16;
					VectorSubtract( bot->client->ps.origin, tempent->client->ps.origin, dist );
					reviveweight = VectorLength( dist );
					if ( reviveweight < 1400 )
					{
						if ( reviveweight < bestreviveweight )
						{
							bestreviveweight = reviveweight;
							bestrevive = tempent;
						}
					}
				}
			}
		}
	}

	if ( bestrevive != NULL )
	{
		bot->movetarget = bestrevive;
		AI_Job_Action[bot->s.clientNum] = BOT_JOB_SYRINGE;
		VectorSubtract( bot->movetarget->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		if ( bestrevive->health > 1 )
		{
			if (bot->client->ps.weapon != WP_MEDKIT)
				ucmd->weapon = WP_MEDKIT;
			return ( 1 );
		}
		else
		{
			if (bot->client->ps.weapon != WP_MEDIC_SYRINGE)
				ucmd->weapon = WP_MEDIC_SYRINGE;
			return ( 2 );
		}
	}

	AI_ResetJob( bot );
	Set_Best_AI_weapon( bot );
	return ( 0 );
}

int
AIMOD_AI_SetLTAction ( gentity_t *bot, usercmd_t *ucmd )
{
	int								i;
	gentity_t /**bestenemy = NULL,*/ *bestrevive = NULL;
	gentity_t						*tempent;
	float /*bestweight = 99999,*/ bestreviveweight = 99999;
	float							reviveweight;
	vec3_t							dist;

	//	qboolean revive = qfalse;
	if ( bot->enemy )
	{
		AI_ResetJob( bot );
		Set_Best_AI_weapon( bot );
		return ( 0 );
	}

#ifdef __VEHICLES__
	if ( (bot->client->ps.eFlags & EF_TANK) )
	{
		return ( 0 );
	}
#endif //__VEHICLES__
	if ( bot->movetarget )
	{
		bot->movetarget = NULL;
	}

	// Run through all clients
	// Determine best enemy target, and best friendly target
	for ( i = 0; i < level.maxclients; i++ )
	{
		tempent = &g_entities[i];
		if ( !tempent )
		{
			continue;
		}

		// if slot is not used go next
		if ( !tempent->inuse )
		{
			continue;
		}

		//if (bot->enemy && tempent != bot->enemy)
		//	continue;
		if ( i == bot->client->ps.clientNum )
		{
			continue;
		}

		if ( tempent->bot_dont_give_supplies_end > level.time )
		{
			continue;
		}

		if ( tempent->bot_wait_for_supplies_end < level.time && tempent->bot_wait_for_supplies_end > level.time - 6000 )
		{
			tempent->bot_dont_give_supplies_end = level.time + 20000;
			continue;								// Given enough to this guy... Share the supplies!
		}

#ifdef __VEHICLES__
		if ( (tempent->client->ps.eFlags & EF_TANK) )
		{
			continue;
		}
#endif //__VEHICLES__
		if ( OnSameTeam( bot, tempent) )
		{

			// We are looking for a friendly target
			if (									/*need_ammo[tempent->s.clientNum] == qtrue
				&&*/
					tempent->health >= 1 &&
					!(tempent->client->ps.pm_flags & PMF_LIMBO) &&
					ClientOutOfAmmo( i)
				)
			{
				vec3_t	eyes;
				VectorCopy( bot->r.currentOrigin, eyes );

				// We could revive this target !
				//if ( OrgVisible(eyes, tempent->r.currentOrigin, -1) )//visible(bot, tempent) )
				{
					VectorSubtract( bot->client->ps.origin, g_entities[i].client->ps.origin, dist );
					reviveweight = VectorLength( dist );
					if ( reviveweight < 1400 )		//MyInFOV(tempent, bot, 110, 90) || (reviveweight < 1400 ) )
					{
						if ( reviveweight < bestreviveweight )
						{
							bestreviveweight = reviveweight;
							bestrevive = tempent;
						}
					}
				}
			}
		}
	}

	if ( bestrevive != NULL )
	{
		bot->movetarget = bestrevive;
		AI_Job_Action[bot->s.clientNum] = BOT_JOB_AMMO;

		//if (bestrevive->r.svFlags & SVF_BOT) // Tell our target to wait for packs...
		bestrevive->bot_wait_for_supplies = level.time + 2000;
		if ( bestrevive->bot_wait_for_supplies_end < level.time )
		{
			bestrevive->bot_wait_for_supplies_end = level.time + 8000;
		}

		//Set_Best_AI_weapon(bot);
		if (bot->client->ps.weapon != WP_AMMO)
			ucmd->weapon = WP_AMMO;
		VectorSubtract( bot->movetarget->r.currentOrigin, bot->r.currentOrigin, bot->move_vector );
		return ( 2 );
	}

	AI_ResetJob( bot );
	Set_Best_AI_weapon( bot );
	return ( 0 );
}

botai_t				botai;
extern qboolean		Is_Medic_Close ( int targetNum );
int					next_talk[MAX_CLIENTS];
int					next_overall_talk_time = 0;
int					enemy_fire_time[MAX_CLIENTS];	// For fire delay.
int					next_idle_talk[MAX_CLIENTS];
int					next_overall_idle_talk_time = 0;
extern int			after_enemy[MAX_CLIENTS];
extern int			last_node_hit_time[MAX_CLIENTS];
int					bot_follow_time[MAX_CLIENTS];
int					bot_movetarget_time[MAX_CLIENTS];
extern int			CreatePathAStar ( gentity_t *bot, int from, int to, short int *pathlist );
extern short int	BotGetNextNode ( gentity_t *bot /*, int *state*/ );
extern qboolean		MyVisible ( gentity_t *self, gentity_t *other );
extern void			AIMOD_MOVEMENT_FastAttackMove ( gentity_t *bot, usercmd_t *ucmd );
extern int			last_medic_check_time[MAX_CLIENTS];


/* */
qboolean
Bot_MG42_Think ( gentity_t *bot, usercmd_t *ucmd )
{
	if
	(
		bot->client &&
		bot->health > 0 &&
		(bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK)
	)
	{	// Just attack if we can when on a Tank or MG42... Don't need to do any other thinking...
		if ( AIMOD_AI_LocateEnemy( bot) )
		{

			//G_Printf("MG42 user (%s) should be attacking!\n", bot->client->pers.netname);
			bot->bot_enemy_visible_time = level.time + 100;
			bot->bot_last_visible_enemy = bot->enemy;
			AIMOD_MOVEMENT_Attack( bot, ucmd );

			// set bot's view angle
			//-------------------------------------------------------
			bot->client->pers.cmd.angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
			bot->client->pers.cmd.angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
			bot->client->pers.cmd.angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

			//-------------------------------------------------------
			// send command through id's code
			//-------------------------------------------------------
			ClientThink_real( bot );

			//-------------------------------------------------------
			return ( qtrue );
		}
	}

	return ( qfalse );
}


qboolean		AIMOD_AI_LocateBetterEnemy ( gentity_t *bot );	// below..
void			Scan_Close_Enemies ( gentity_t *bot );			//below..
extern int		CreateDumbRoute ( int from, int to, short int *pathlist );
extern int		num_snipe_points;
extern vec3_t	snipe_points[256];
int				this_frame_time = 0;
int				num_this_frame = 1;


#ifdef __VEHICLES__
extern void AIMOD_MOVEMENT_VehicleAttack ( gentity_t *bot, usercmd_t *ucmd );
extern void AIMOD_MOVEMENT_VehicleMove ( gentity_t *bot, usercmd_t *ucmd );
#endif //__VEHICLES__


// AVD Think think mr bot !
qboolean
AIMOD_AI_Think ( gentity_t *bot )
{
	usercmd_t	*ucmd;
	int			testtime;
	char		buf[1024];									//, *args;
	qboolean	showServerCommand = qfalse;
	weapon_t	weapon;
	int			pc;											// Player Class.
	gentity_t	*oldenemy;
	int			time = trap_Milliseconds();
	int			i;

	// Debug sprites above bots heads.. Initialize each frame!
	bot->s.dl_intensity = 0;

	// delayed chats
	for ( i = 0; i < AIVOICE_MAX; i++ )
	{
		if ( bot->AIvoiceChat[i].delay < level.time && bot->AIvoiceChat[i].played )
		{
			G_Voice( bot, bot->AIvoiceChat[i].target, bot->AIvoiceChat[i].mode, bot->AIvoiceChat[i].id,
					 bot->AIvoiceChat[i].voiceonly );
			bot->AIvoiceChat[i].played = qfalse;
		}
	}

	if ( bot->s.number > MAX_CLIENTS || bot->s.number < 0 )
	{
		return ( qfalse );									// Stop non bots ever getting here...
	}

	// Okay, determine the team
	myteam = bot->client->ps.persistant[PERS_TEAM];			// this SHOULD be set for a bot
	otherteam = ( myteam == TEAM_ALLIES ? TEAM_AXIS : TEAM_ALLIES );
	weapon = bot->client->ps.weapon;
	pc = bot->client->sess.playerType;

	// fake handle server commands (to prevent server command overflow)
	while ( trap_BotGetServerCommand( bot->s.number, buf, sizeof(buf)) )
	{

	}

	// Set up client movement
	//-------------------------------------------------------
	VectorCopy( bot->client->ps.viewangles, bot->s.angles );
	VectorSet( bot->client->ps.delta_angles, 0, 0, 0 );
	ucmd = &bot->client->pers.cmd;

	//    bot->movetarget	= NULL;
	ucmd->forwardmove = 0;
	ucmd->upmove = 0;
	ucmd->rightmove = 0;
	ucmd->buttons &= ~BUTTON_ATTACK;						// Don't attack by default, it's just plain silly !
	ucmd->buttons &= ~BUTTON_SPRINT;						// Always sprint... naaaah :-)
	ucmd->wbuttons = 0;
	bot->client->ps.pm_flags &= ~PMF_DUCKED;
	if ( !(bot->client->ps.eFlags & EF_BOT) )
	{
		bot->client->ps.eFlags |= EF_BOT;
	}

	// here is a fix for a connection interrupted and also another identification of a bot applicable in pmove
	ucmd->flags |= 0x02;

	// edit dont work it seems that client cant get bots cmd
	//-------------------------------------------------------
	// Bot timers...
	if ( !bot->bot_last_frame_time )
	{
		bot->bot_last_frame_time = 0;
	}

	if ( bot->bot_last_frame_time > level.time /*|| bot->bot_last_frame_time == 0*/ )
	{														// Init all bot timers...
		bot->lastMedicVoiceChat = level.time - 1;
		bot->node_timeout = level.time + 1000;
		bot->next_move_time = level.time;
		bot->shooting_timeout = level.time - 1;
		bot->bot_route_think_delay = level.time - 1;
		bot->bot_last_good_move_time = level.time - 1;
		bot->bot_last_move_fix_time = level.time - 1;
		bot->bot_last_chat_time = level.time - 1;
		bot->bot_sniping_time = level.time - 1;
		bot->bot_runaway_time = level.time - 1;
		bot->bot_enemy_visible_time = level.time - 1;
		bot->bot_followtarget_visible_time = level.time - 1;
		bot->bot_movetarget_visible_time = level.time - 1;
		bot->bot_special_action_thinktime = level.time;
		bot->bot_strafe_left_timer = level.time - 1;
		bot->bot_strafe_right_timer = level.time - 1;
		bot->bot_blind_routing_time = level.time;
		last_medic_check_time[bot->s.number] = level.time;
		next_talk[bot->s.number] = level.time;
		enemy_fire_time[bot->s.number] = level.time;		// For fire delay.
		next_idle_talk[bot->s.number] = level.time + Q_TrueRand( 0, 25000 );
		after_enemy[bot->s.number] = level.time - 1;
		last_node_hit_time[bot->s.number] = level.time;
		bot_follow_time[bot->s.number] = level.time - 1;
		bot_movetarget_time[bot->s.number] = level.time - 1;
	}
	else
	{														// Set frame time.
		bot->bot_last_frame_time = level.time;
	}

	// End of bot timers...
	// AVD Send a dead bot into limbo about 300 ms before a respawn
	//-------------------------------------------------------
	// This gives a medic some time to revive him
	if ( bot->health <= 0 )
	{
		if ( bot->current_node && Is_Medic_Close( bot->s.number) )
		{

			// set approximate ping and show values
			//-------------------------------------------------------
			bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

			//-------------------------------------------------------
			// set bot's view angle
			//-------------------------------------------------------
			ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
			ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
			ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

			//-------------------------------------------------------
			// send command through id's code
			//-------------------------------------------------------
			ClientThink_real( bot );

			// We were able to do a full think this frame, so set the next think frame to the BOT frametime..
			if ( time - level.frameTime < MAX_BOT_THINKFRAME )
			{
				bot->nextthink = level.time + FRAMETIME;
			}

			//-------------------------------------------------------
			// Use this with the fastthink procedure...
			bot->bot_last_ucmd = ucmd;
			return ( qtrue );
		}

		// This should make sure the procedure for movement restarts
		bot->state = BOT_STATE_POSITION;
		bot->goal_node = INVALID;
		bot->next_node = INVALID;
		bot->current_node = INVALID;

		// We have a dead bot, wait with dropping it in limbo
		if ( !(bot->client->ps.pm_flags & PMF_LIMBO) )
		{
			// Not yet in limbo, MEDIC ! :-)
			if ( random() < 0.1 && bot->lastMedicVoiceChat + 5000 < level.time )
			{
				G_Voice( bot, NULL, SAY_TEAM, "medic", qtrue );
				G_Say( bot, NULL, SAY_TEAM, "Medic!" );
				bot->lastMedicVoiceChat = level.time;
			}

			if ( Is_Medic_Close( bot->s.number) )
			{												// Don't respawn if there is a medic close by that can revive us!
			}
			else if ( bot->client->sess.sessionTeam == TEAM_AXIS )
			{
				testtime = level.time % ( g_redlimbotime.integer - 300 );
				if ( testtime < bot->client->pers.lastReinforceTime )
				{
					if (bot->bot_no_respawn)
					{// In coop/sp we disconnect dead bots!
						trap_DropClient( bot->s.clientNum, " (bot) was removed on death.", 0 );
						return qtrue;
					}
					limbo( bot, qtrue );
					bot->client->pers.lastReinforceTime = 0;
				}
				else
				{
					bot->client->pers.lastReinforceTime = testtime;
				}
			}
			else if ( bot->client->sess.sessionTeam == TEAM_ALLIES )
			{
				testtime = level.time % ( g_bluelimbotime.integer - 300 );
				if ( testtime < bot->client->pers.lastReinforceTime )
				{
					if (bot->bot_no_respawn)
					{// In coop/sp we disconnect dead bots!
						trap_DropClient( bot->s.clientNum, " (bot) was removed on death.", 0 );
						return qtrue;
					}
					limbo( bot, qtrue );
					bot->client->pers.lastReinforceTime = 0;
				}
				else
				{
					bot->client->pers.lastReinforceTime = testtime;
				}
			}
		}

		// Else do some checks to see if it would be handy to change class etc
	}

	if ( Bot_MG42_Think( bot, ucmd) )
	{
		return ( qtrue );
	}

	// Only if we are still alive go running about etc :-)
	// Otherwise don't waste cpu time on this part
	if ( bot->health > 0 )
	{
		qboolean	getdown = qfalse;
		if ( bot->client->ps.weapon == WP_MOBILE_MG42 )
		{
			int loop;
			for ( loop = 0; loop < num_snipe_points; loop++ )
			{
				if ( VectorDistance( snipe_points[loop], bot->r.currentOrigin) < 64 )
				{
					getdown = qtrue;
					break;
				}
			}
		}

		if ( next_idle_talk[bot->s.number] == 0 )
		{													// Initialize them to start to talk at different random times.
			next_idle_talk[bot->s.number] = level.time + Q_TrueRand( 1, 30000 );
		}

/*		if
		(
			!bot->s.weapon ||
			bot->s.weapon == 0 ||
			!bot->client->ps.weapon ||
			bot->client->ps.weapon == 0 ||
			!bot->weaponchoice ||
			bot->weaponchoice == 0
		)
		{
			SetBotWeapons( bot );							// In case things get messed up...
		}*/

		oldenemy = bot->enemy;

		if
		(
			bot->bot_next_enemy_scan_time < level.time &&
			(!bot->enemy || bot->bot_enemy_visible_time < level.time || (bot->enemy && bot->enemy->health <= 0))
		)
		{

			//if (trap_Milliseconds() - level.frameTime < MAX_BOT_THINKFRAME)
			{
				if ( bot->enemy && bot->enemy->health > 0 && MyVisible( bot, bot->enemy) )
				{
					bot->bot_enemy_visible_time = level.time + ( 600 - (bot->skillchoice * 100) /*2400-(bot->skillchoice*400)*/ );
					bot->bot_last_visible_enemy = bot->enemy;
				}
				else if ( AIMOD_AI_LocateEnemy( bot) )
				{
					bot->bot_enemy_visible_time = level.time + ( 600 - (bot->skillchoice * 100) /*2400-(bot->skillchoice*400)*/ );
					bot->bot_last_visible_enemy = bot->enemy;
				}
				else
				{
					bot->bot_enemy_visible_time = 0;
					bot->bot_last_visible_enemy = NULL;
					bot->enemy = NULL;
				}

				bot->bot_next_enemy_scan_time = bot->bot_enemy_visible_time;

				//bot->bot_next_enemy_scan_time = level.time + (1200-(bot->skillchoice*200));//(2400-(bot->skillchoice*400)); // delay between enemy checks...
				//bot->bot_next_enemy_scan_time = level.time + (2400-(bot->skillchoice*400)); // delay between enemy checks...
			}
		}

#ifdef __RIFLE_GRENADES__
		if ( !bot->enemy || bot->bot_enemy_visible_time < level.time /*&& time - level.frameTime < MAX_BOT_THINKFRAME*/ )
		{													// Rifle grenades...
			// Switch to a rifle grenade between enemies...
			if ( bot->client->ps.weapon == WP_KAR98 && AIMOD_HaveAmmoForWeapon( &bot->client->ps, weapAlts[WP_KAR98]) )
			{
				if (bot->client->ps.weapon != weapAlts[WP_KAR98])
					ucmd->weapon = weapAlts[WP_KAR98];
			}

			if
			(
				bot->client->ps.weapon == WP_CARBINE &&
				AIMOD_HaveAmmoForWeapon( &bot->client->ps, weapAlts[WP_CARBINE])
			)
			{
				if (bot->client->ps.weapon != weapAlts[WP_CARBINE])
					ucmd->weapon = weapAlts[WP_CARBINE];
			}

			// If no ammo, switch back!
			if
			(
				bot->client->ps.weapon == weapAlts[WP_KAR98] &&
				!AIMOD_HaveAmmoForWeapon( &bot->client->ps, weapAlts[WP_KAR98])
			)
			{
				if (bot->client->ps.weapon != WP_KAR98)
					ucmd->weapon = WP_KAR98;
			}

			if
			(
				bot->client->ps.weapon == weapAlts[WP_CARBINE] &&
				!AIMOD_HaveAmmoForWeapon( &bot->client->ps, weapAlts[WP_CARBINE])
			)
			{
				if (bot->client->ps.weapon != WP_CARBINE)
					ucmd->weapon = WP_CARBINE;
			}

			// End switch to a rifle grenade between enemies...
		}
		else
#endif //__RIFLE_GRENADES__
		{
			if
			(
				getdown // at a snipe point.. Set up here!
				||
				(
					bot->client->ps.weapon == WP_MOBILE_MG42 &&
					bot->enemy &&
					(
						bot->bot_mg42_set_time <= level.time ||
						VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 512
					) /*&& MyVisible(bot, bot->enemy)*/ &&
					bot->bot_enemy_visible_time >= level.time
				)
			)
			{												// Move into MG42 set-up position...
				if ( bot->bot_mg42_set_time <= level.time && (bot->client->ps.eFlags & EF_PRONE) )
				{											// Ready to set it up now!
					bot->client->pers.cmd.weapon = WP_MOBILE_MG42_SET;
					bot->bot_mg42_set_staydown_time = level.time + 10000;
					if ( getdown )
					{
						bot->bot_mg42_set_staydown_time = level.time + 60000;
					}
				}
				else
				{											// Get on the floor ready to set up the mobile MG42... (Delayed setup based on skill level)...
					if ( !(bot->client->ps.eFlags & EF_PRONE) )
					{
						bot->client->ps.eFlags |= EF_PRONE;
						ucmd->upmove = -127;				// Crouch?
						bot->bot_mg42_set_time = level.time + ( 800 - (bot->skillchoice * 100) );
					}
				}
			}

#ifdef __WEAPON_AIM__
			else if ( BG_Weapon_Is_Aimable( bot->client->ps.weapon) )
			{
				if ( bot->enemy )
				{
					if ( VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) > 512 )
					{
						bot->client->ps.eFlags |= EF_AIMING;
					}
					else
					{
						bot->client->ps.eFlags &= ~EF_AIMING;
					}
				}
			}
#endif //__WEAPON_AIM__
		}

		/*		if (bot->enemy && (pc == PC_SOLDIER || pc == PC_ENGINEER) 
			&& VectorDistance(bot->enemy->r.currentOrigin, bot->r.currentOrigin) < 512)
		{// Switch to a grenade if close to an enemy...
			if (bot->client->sess.sessionTeam == TEAM_AXIS)
				bot->client->pers.cmd.weapon = WP_GRENADE_LAUNCHER;
			else
				bot->client->pers.cmd.weapon = WP_GRENADE_PINEAPPLE;
		}

		if (bot->enemy && pc == PC_FIELDOPS 
			&& VectorDistance(bot->enemy->r.currentOrigin, bot->r.currentOrigin) < 512)
		{// Switch to a grenade if close to an enemy...
			bot->client->pers.cmd.weapon = WP_SMOKE_MARKER;
		}*/
		if
		(
			!getdown &&
			(!bot->enemy || VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) < 512) &&
			bot->client->ps.weapon == WP_MOBILE_MG42_SET &&
			bot->bot_mg42_set_time <= level.time &&
			bot->bot_mg42_set_staydown_time < level.time
		)
		{													// Get up off the ground again...
			if ( bot->bot_mg42_set_time <= level.time && (bot->client->ps.eFlags & EF_PRONE) )
			{												// Get up off the floor after using the mobile MG42 set-up... (Delayed setup based on skill level)...
				ucmd->upmove = 128;
				bot->client->ps.eFlags &= ~EF_PRONE;
			}
			else
			{												// Ready to un-set it now!
				bot->client->pers.cmd.weapon = WP_MOBILE_MG42;
				bot->bot_mg42_set_time = level.time + ( 800 - (bot->skillchoice * 100) );
			}
		}
		else
#ifdef __WEAPON_AIM__
			if ( !bot->enemy || VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) < 512 )
			{
				bot->client->ps.eFlags &= ~EF_AIMING;
			}
#endif //__WEAPON_AIM__

#ifdef __VEHICLES__
		if ( bot->client->ps.eFlags & EF_TANK )
		{
			if ( bot->enemy && bot->bot_enemy_visible_time > level.time )
			{
				AIMOD_MOVEMENT_VehicleAttack( bot, ucmd );
			}
			else
			{
				AIMOD_MOVEMENT_VehicleMove( bot, ucmd );
			}

			// set approximate ping and show values
			//-------------------------------------------------------
			bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

			//-------------------------------------------------------
			// set bot's view angle
			//-------------------------------------------------------
			ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
			ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
			ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

			//-------------------------------------------------------
			// send command through id's code
			//-------------------------------------------------------
			ClientThink_real( bot );

			// We were able to do a full think this frame, so set the next think frame to the BOT frametime..
			if ( time - level.frameTime < MAX_BOT_THINKFRAME )
			{
				bot->nextthink = level.time + FRAMETIME;
			}

			//-------------------------------------------------------
			// Use this with the fastthink procedure...
			bot->bot_last_ucmd = ucmd;
			return ( qtrue );
		}
		else
#endif //__VEHICLES__
/*		if ((bot->enemy && bot->bot_coverspot_time > level.time)
			|| bot->bot_enemy_visible_time > level.time )
		{
			AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
		}
		else if
			(
				bot->enemy &&
				bot->bot_enemy_visible_time > level.time &&
				bot->bot_last_visible_enemy == bot->enemy
			)
		{			// Attack handling...
				if
				(
					bot->enemy->client &&
					VectorDistance( bot->r.currentOrigin, bot->enemy->r.currentOrigin) >
						(1200 + (bot->skillchoice * 200)) &&
					bot->weaponchoice != WP_K43 &&
					bot->weaponchoice != WP_K43_SCOPE &&
					bot->weaponchoice != WP_FG42 &&
					bot->weaponchoice != WP_FG42SCOPE &&
					!(bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK)
					&& !(
						bot->enemy->client->ps.eFlags & EF_MG42_ACTIVE ||
						bot->enemy->client->ps.eFlags & EF_MOUNTEDTANK
					)
				)
				{	// Out of range.. Keep following nodes!
					bot->enemy = NULL;
					bot->bot_enemy_visible_time = 0;
					bot->bot_last_visible_enemy = NULL;
					AI_ResetJob( bot );
					AIMOD_MOVEMENT_Move( bot, ucmd );
				}
			else if
				(
					bot->bot_enemy_visible_time > level.time &&
					bot->bot_runaway_time < level.time
				)
			{		// Attack
				AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );

				//if (bot->bot_coverspot_time < level.time)
				//	AIMOD_MOVEMENT_AttackMove( bot, ucmd );
			}
		}*/


/*
		if (bot->enemy && ucmd->buttons & BUTTON_ATTACK)
		{// Check we are actually close to hitting them!!!
			vec3_t	enemy_vec, enemy_angles;

			VectorSubtract( bot->enemy->r.currentOrigin, bot->r.currentOrigin, enemy_vec );
			vectoangles( enemy_vec, enemy_angles );
			
			if (VectorDistance(enemy_angles, bot->r.currentAngles) > 64)
				ucmd->buttons &= ~BUTTON_ATTACK;
			else if (!MyVisible(bot, bot->enemy))
				ucmd->buttons &= ~BUTTON_ATTACK;
		}*/

		//if (trap_Milliseconds() - level.frameTime < MAX_BOT_THINKFRAME)
		{
			if ( pc == PC_FIELDOPS )
			{			// Find someone to give ammo to...
				if ( QuickFieldOpsScan( bot) )
				{
					return ( qtrue );
				}
			}
			else if ( pc == PC_ENGINEER && bot->bot_enemy_visible_time < level.time )
			{
				if ( QuickEngineerScan( bot) )
				{
					return ( qtrue );
				}
			}

#ifdef __ADVANCED_BOT_CHECKS__
			else if ( pc == PC_MEDIC )
			{			// Find someone to revive/heal...
				if ( QuickMedicScan( bot) )
				{
					return ( qtrue );
				}
			}
			else if
				(
					pc != PC_MEDIC &&
					bot->health > 0 &&
					bot->health < bot->client->ps.stats[STAT_MAX_HEALTH] * 0.75 &&
					bot->bot_enemy_visible_time < level.time
				)
			{
				if ( QuickHealthScan( bot) )
				{
					return ( qtrue );
				}
			}
			else if
				(
					pc != PC_FIELDOPS &&
					pc != PC_ENGINEER &&
					(bot->weaponchoice != WP_FLAMETHROWER && bot->client->ps.ammo[bot->weaponchoice] < GetAmmoTableData( bot->weaponchoice)->maxammo * 0.30) &&
					bot->bot_enemy_visible_time < level.time
				)
			{
				if ( QuickAmmoScan( bot) )
				{
					return ( qtrue );
				}
			}
#endif //__ADVANCED_BOT_CHECKS__
#ifdef __CORPSE_SCAN__
			else if ( pc == PC_COVERTOPS && bot->bot_enemy_visible_time < level.time )
			{			// Find an enemy uniform...
				if ( QuickCorpseScan( bot) )
				{
					return ( qtrue );
				}
			}
#endif //__CORPSE_SCAN__
		}

		// No more ammo, but we can reload, so RELOAD !
		if ( bot->client->ps.ammoclip[weapon] == 0 && bot->client->ps.ammo[weapon] )
		{
			ucmd->wbuttons = WBUTTON_RELOAD;
		}

		else if ((bot->enemy && bot->bot_coverspot_time > level.time)
			|| bot->bot_enemy_visible_time > level.time )
		{
			AIMOD_SP_MOVEMENT_CoverSpot_Movement( bot, ucmd );
		}

		// If we are on the move, keep moving !
		else
		{				// Waypoint navigation...
			AIMOD_MOVEMENT_Move( bot, ucmd );
		}
	}

	// set approximate ping and show values
	//-------------------------------------------------------
	bot->client->ps.ping = 75 + floor( random() * 25 ) + 1;

	//bot->client->ps.ping = 10;
	//-------------------------------------------------------
	// set bot's view angle
	//-------------------------------------------------------
	ucmd->angles[PITCH] = ANGLE2SHORT( bot->s.angles[PITCH] );
	ucmd->angles[YAW] = ANGLE2SHORT( bot->s.angles[YAW] );
	ucmd->angles[ROLL] = ANGLE2SHORT( bot->s.angles[ROLL] );

	//-------------------------------------------------------
	// send command through id's code
	//-------------------------------------------------------
	ClientThink_real( bot );

	// We were able to do a full think this frame, so set the next think frame to the BOT frametime..
	if ( time - level.frameTime < MAX_BOT_THINKFRAME )
	{
		bot->nextthink = level.time + FRAMETIME;
	}

	//-------------------------------------------------------
	// Use this with the fastthink procedure...
	bot->bot_last_ucmd = ucmd;
	return ( qtrue );
}


/*
=================
findradius

Returns entities that have origins within a spherical area

findradius (origin, radius)
=================
*/
gentity_t *
findradius ( gentity_t *from, vec3_t org, float rad )
{
	vec3_t	eorg;
	int		j;
	if ( !from )
	{
		from = g_entities;
	}
	else
	{
		from++;
	}

	for ( ; from < &g_entities[level.num_entities]; from++ )
	{
		if ( !from->inuse )
		{
			continue;
		}

		if ( from->solid == SOLID_NOT )
		{
			continue;
		}

		for ( j = 0; j < 3; j++ )
		{
			eorg[j] = org[j] - ( from->s.origin[j] + (from->r.mins[j] + from->r.maxs[j]) * 0.5 );
		}

		if ( VectorLength( eorg) > rad )
		{
			continue;
		}

		return ( from );
	}

	return ( NULL );
}


/* */
void
G_ProjectSource ( vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result )
{
	result[0] = point[0] + forward[0] * distance[0] + right[0] * distance[1];
	result[1] = point[1] + forward[1] * distance[0] + right[1] * distance[1];
	result[2] = point[2] + forward[2] * distance[0] + right[2] * distance[1] + distance[2];
}


// AIMod stuff...
qboolean
isBot ( int clientNum )
{
	gentity_t	*ent = &g_entities[clientNum];
	if ( ent->r.svFlags & SVF_BOT )
	{
		return ( qtrue );
	}

	return ( qfalse );
}


/* */
void
Scan_AI_For_Followers ( int clientNum )
{					// Find a bot to follow a player.
	int													i;
	gentity_t											*bot = &g_entities[clientNum];
	gentity_t /**bestenemy = NULL,*/ *bestrevive = NULL, *bestrevive2 = NULL;
	gentity_t											*tempent;
	float /*bestweight = 99999, bestweight2 = 99999,*/ bestreviveweight = 99999, bestreviveweight2 = 99999;
	float												reviveweight;
	vec3_t												dist;

	//	qboolean revive = qfalse;
	// Run through all clients
	// Determine best enemy target, and best friendly target
	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		tempent = &g_entities[i];
		if ( !tempent )
		{
			continue;
		}

		// if slot is not used go next
		if ( !tempent->inuse )
		{
			continue;
		}

		if ( bot->enemy && tempent != bot->enemy )
		{
			continue;
		}

		if ( i == bot->client->ps.clientNum )
		{
			continue;
		}

		if ( !tempent->is_bot )
		{
			continue;
		}

		if ( !(tempent->r.svFlags & SVF_BOT) )
		{
			continue;
		}

		if ( OnSameTeam( bot, tempent) )
		{

			// We are looking for a friendly target
			if ( tempent->health > 0 && !(tempent->client->ps.pm_flags & PMF_LIMBO) )
			{
				vec3_t	eyes;
				VectorCopy( bot->r.currentOrigin, eyes );

				// We could revive this target !
				//if ( /*OrgVisible(eyes, tempent->r.currentOrigin, -1) )*/MyVisible(bot, tempent) )
				{
					VectorSubtract( bot->client->ps.origin, g_entities[i].client->ps.origin, dist );
					reviveweight = VectorLength( dist );
					if ( reviveweight < 1400 )
					{
						if ( reviveweight < bestreviveweight )
						{
							bestreviveweight2 = bestreviveweight;
							bestrevive2 = bestrevive;
							bestreviveweight = reviveweight;
							bestrevive = tempent;
						}
					}
				}
			}
		}
	}

	if ( bestrevive != NULL )
	{
		if ( !bestrevive->followtarget )
		{			// Not already following someone or doing a job....
			if ( rand() % 10 < 6 )
			{
				G_Voice( bestrevive, NULL, SAY_TEAM, va( "Affirmative"), qfalse );
			}
			else
			{
				G_Voice( bestrevive, NULL, SAY_TEAM, va( "CommandAcknowledged"), qfalse );
			}

			bestrevive->movetarget = bot;
			bestrevive->followtarget = bot;
			AI_Job_Action[bestrevive->s.clientNum] = BOT_JOB_FOLLOW;
#ifdef _DEBUG
			if( bot_debug.integer >= BOT_DEBUG_REGULAR )
				G_Printf( "^2*** ^4BOT DEBUG^2: ^7%s ^6will now follow ^7%s^6.\n",
					bestrevive->client->pers.netname, bot->client->pers.netname );
#endif //_DEBUG
			VectorSubtract( bestrevive->movetarget->r.currentOrigin, bestrevive->r.currentOrigin,
							bestrevive->move_vector );
			if ( bestrevive2 != NULL )
			{
				if ( !bestrevive2->followtarget )
				{	// Not already following someone or doing a job....
					if ( rand() % 10 < 6 )
					{
						G_Voice( bestrevive2, NULL, SAY_TEAM, va( "Affirmative"), qfalse );
					}
					else
					{
						G_Voice( bestrevive2, NULL, SAY_TEAM, va( "CommandAcknowledged"), qfalse );
					}

					bestrevive2->movetarget = bot;
					bestrevive2->followtarget = bot;
					AI_Job_Action[bestrevive2->s.clientNum] = BOT_JOB_FOLLOW;
#ifdef _DEBUG
					if( bot_debug.integer >= BOT_DEBUG_REGULAR )
						G_Printf( "^2*** ^4BOT DEBUG^2: ^7%s ^6will now follow ^7%s^6.\n",
							  bestrevive2->client->pers.netname, bot->client->pers.netname );
#endif //_DEBUG
					VectorSubtract( bestrevive2->movetarget->r.currentOrigin, bestrevive2->r.currentOrigin,
									bestrevive2->move_vector );
				}
			}
		}
		else
		{
			trap_SendServerCommand( bot - g_entities, "cp \"^3Sorry! ^7No available bots.\n\"" );
		}
	}
}

//===========================================================================
// Routine      : AIMOD_AI_LocateEnemy
// Description  : Scans for enemy (very simple, just picks any visible enemy)
#ifdef __NPC__
extern vmCvar_t npc_minplayers;
#endif //__NPC__

/* */
void
Scan_Close_Enemies ( gentity_t *bot )
{		// Fast scan of nearby entities...
	int			loop;
	gentity_t	*best = NULL;

	//float best_dist = 1024;
	float		best_dist = 512.0f;
	if ( bot->client && (bot->client->ps.eFlags & EF_MG42_ACTIVE || bot->client->ps.eFlags & EF_MOUNTEDTANK) )
	{	// Don't do this while on an MG42 or Tank...
		return;
	}

	for ( loop = 0; loop < MAX_GENTITIES; loop++ )
	{
		gentity_t	*ent = &g_entities[loop];
		if ( !ent )
		{
			continue;
		}

#ifdef __NPC__
		if ( ent->s.eType != ET_PLAYER && ent->s.eType != ET_NPC )
		{
			continue;
		}

		if ( ent->s.eType == ET_PLAYER && !ent->client )
		{
			continue;
		}

		if ( ent->s.eType == ET_NPC && ent->s.modelindex2 != CLASS_HUMANOID )
		{
			continue;
		}

#else //!__NPC__
		if ( ent->s.eType != ET_PLAYER )
		{
			continue;
		}

		if ( !ent->client )
		{
			continue;
		}
#endif //__NPC__
		if ( VectorDistance( ent->r.currentOrigin, bot->r.currentOrigin) >= best_dist )
		{
			continue;
		}

		if ( OnSameTeam( ent, bot) )
		{
			continue;
		}

		best_dist = VectorDistance( ent->r.currentOrigin, bot->r.currentOrigin );
		best = ent;
	}

	if ( best )
	{
		bot->enemy = best;
	}
}


/* */
qboolean
Have_Close_Enemy ( gentity_t *bot )
{
	Scan_Close_Enemies( bot );
	if ( bot->enemy )
	{
		return ( qtrue );
	}

	return ( qfalse );
}

#ifdef __NPC__
int			num_npcs_on_map = 0;
int			num_npcs_check_time = 0;
extern int	Count_NPC_Players ( void );


/* */
qboolean
NPCs_On_Map ( void )
{
	if ( num_npcs_check_time <= level.time )
	{
		num_npcs_on_map = Count_NPC_Players();
		num_npcs_check_time = level.time + 30000;
	}

	if ( num_npcs_on_map > 0 )
	{
		return ( qtrue );
	}

	return ( qfalse );
}
#endif //__NPC__

/* */
qboolean
AIMOD_AI_LocateEnemy ( gentity_t *bot )
{
	int			i;
	int			last_attack_time;
	gentity_t	*bestenemy = NULL;
	gentity_t	*tempent;
	float		bestweight = 99999;
	float		weight, max_weight = 2000.0;		//800.0;//300.0;

	//    vec3_t			dist;
	qboolean	ontherun = ( botai.flagstate[otherteam].carrier == bot->s.clientNum );
	gentity_t	*possible_enemies[MAX_GENTITIES];
	gentity_t	*best_enemies[MAX_GENTITIES];
	int			upto = 0;
	int			upto2 = 0;
	
	if ( bot->client->attacker && (bot->client->attacker->client 
#ifdef __NPC__
		|| bot->client->attacker->NPC_client
#endif //__NPC__
		) )
	{												// We might be able to skip the thinking and just use the person/bot that attacked us as enemy...
		gclient_t	*attacker_client;
		if ( bot->client->attacker->client )
		{
			attacker_client = bot->client->attacker->client;
		}
#ifdef __NPC__
		else
		{
			attacker_client = bot->client->attacker->NPC_client;
		}
#endif //__NPC__

		if
		(
			attacker_client->ps.stats[STAT_HEALTH] > 0 &&
			!OnSameTeam( bot, bot->client->attacker) &&
			attacker_client->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR
		)
		{											// Attacker validity checks...
			if ( bot->client->attacker->health > 0 )
			{										// Make sure that we only use attacker if he's still visible...
				if (MyVisible( bot, bot->client->attacker))
				{
					bot->enemy = bot->client->attacker;
					bot->client->attacker = NULL;
					return ( qtrue );
				}
			}
		}
	}

	last_attack_time = ( ontherun ? 1000 : 3000 );	// 1 second if on the run, 3 if not

	for ( i = 0; i < MAX_CLIENTS; i++ )
	{												// Cycle through client slots looking for valid enemies...
		tempent = &g_entities[i];

		if (!tempent)
			continue;

		if ( !tempent->client )
			continue;

		// if slot is not used go next
//		if ( !tempent->inuse )
//			continue;

		if ( tempent->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
			continue;								// Don't aim at spectators lol...

		if ( OnSameTeam( bot, tempent ) )
			continue;

		if ( tempent->client->ps.stats[STAT_HEALTH] <= 0 || tempent->health <= 0 )
			continue;

		if
		(
			bot->client->ps.stats[STAT_PLAYER_CLASS] == PC_FIELDOPS ||
			bot->client->ps.stats[STAT_PLAYER_CLASS] == PC_COVERTOPS
		)
		{
			max_weight = 5000.0f;//2000.0;
		}
		else
		{
			max_weight = 2400.0f;//1200.0;
		}

		if
		(
			bot->client->ps.weapon == WP_FG42 ||
			bot->client->ps.weapon == WP_GARAND ||
			bot->client->ps.weapon == WP_K43 ||
			bot->client->ps.weapon == WP_FG42SCOPE ||
			bot->client->ps.weapon == WP_GARAND_SCOPE ||
			bot->client->ps.weapon == WP_K43_SCOPE
		)
		{
			max_weight = 9000.0;
		}
		else if ( tempent->client->ps.powerups[PW_REDFLAG] || tempent->client->ps.powerups[PW_BLUEFLAG] )
		{
			max_weight = 8000.0;
		}

#ifdef __VEHICLES__
		if ( (tempent->client->ps.eFlags & EF_TANK) )
		{											// Only these should try to shoot at tanks!
			if
			(
				!(bot->client->ps.eFlags & EF_TANK) &&
				bot->client->ps.weapon != WP_PANZERFAUST &&
				bot->client->ps.weapon != WP_KAR98 &&
				bot->client->ps.weapon != WP_CARBINE &&
				bot->client->ps.weapon != WP_GRENADE_LAUNCHER &&
				bot->client->ps.weapon != WP_GRENADE_PINEAPPLE
			)
			{
				continue;
			}
		}
#endif //__VEHICLES__

		weight = VectorDistance( bot->r.currentOrigin, tempent->r.currentOrigin );
		if ( weight < max_weight )
		{
			possible_enemies[upto] = tempent;
			upto++;
		}
	}

	bestweight = 99999;
	if ( upto == 1 )
	{												// Only have 1 possibile...
		best_enemies[0] = possible_enemies[0];
		upto2 = upto;
	}
	else
	{												// Sort the possible list...
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
				{									// Best so far...
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
	{												// Return the closest one that is visible, checking visibilities only as far as needed for CPU time saving...
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


/* */
qboolean
AIMOD_AI_LocateBetterEnemy ( gentity_t *bot )
{
	int			i;
	int			last_attack_time;
	gentity_t	*bestenemy = NULL;
	gentity_t	*tempent;
	float		bestweight = 99999;
	float		weight, max_weight = 2000.0;		//800.0;//300.0;

	//    vec3_t			dist;
	qboolean	ontherun = ( botai.flagstate[otherteam].carrier == bot->s.clientNum );
	if ( bot->client->attacker && bot->client->attacker->client )
	{												// We might be able to skip the thinking and just use the person/bot that attacked us as enemy...
		if
		(
			bot->client->attacker->client->ps.stats[STAT_HEALTH] > 0 &&
			!OnSameTeam( bot, bot->client->attacker) &&
			bot->client->attacker->client->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR
		)
		{											// Attacker validity checks...
			if ( MyVisible( bot, bot->client->attacker) )
			{										// Make sure that we only use attacker if he's still visible...
				bot->enemy = bot->client->attacker;
				bot->client->attacker = NULL;
				return ( qtrue );
			}
		}
	}

	if ( bot->client->ps.stats[STAT_PLAYER_CLASS] == PC_FIELDOPS )
	{
		max_weight = 2000.0;
	}

	last_attack_time = ( ontherun ? 1000 : 3000 );	// 1 second if on the run, 3 if not
	for ( i = 0; i < MAX_CLIENTS; i++ )
	{												// Cycle through client slots looking for valid enemies...
		tempent = &g_entities[i];

		// if slot is not used go next
		if ( !tempent->inuse )
		{
			continue;
		}

		if ( !tempent->client )
		{
			continue;
		}

		if ( OnSameTeam( bot, tempent) )
		{
			continue;
		}

		if ( tempent->client->ps.stats[STAT_HEALTH] <= 0 )
		{											// Our attacker is dead, continue
			//			if (bot->client->attacker == tempent)
			//				bot->client->attacker = NULL;
			continue;
		}

		if ( tempent->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR )
		{
			continue;								// Don't aim at spectators lol...
		}

		if ( tempent->client->ps.powerups[PW_REDFLAG] || tempent->client->ps.powerups[PW_BLUEFLAG] )
		{
			max_weight = 3000.0;
		}

		//		else if (bot->current_node == NODE_FASTHOP && bot->next_node == NODE_FASTHOP)
		//			max_weight = 200;
		weight = VectorDistance( bot->r.currentOrigin, tempent->r.currentOrigin );
		if ( MyInFOV( tempent, bot, 110, 90) && (weight < max_weight && weight < bestweight) )
		{											// Only if in front and range is valid and better then previous best... Avoid vis checks as much as is possible...
			if ( bot->client->ps.stats[STAT_PLAYER_CLASS] == PC_FIELDOPS )
			{
				if ( !MyVisible( bot, tempent) )
				{
					continue;
				}
			}

			//if ( MyVisible(bot, tempent) )
			{
				bestweight = weight;
				bestenemy = tempent;

				/*if (VectorDistance(bot->r.currentOrigin, tempent->r.currentOrigin) < 512)
				{// Will do us.. Dont bother to vischeck further...
					bot->lastEnemy = qtrue;
					bot->enemy = bestenemy;
					return qtrue;
				}*/
			}
		}

		// If best enemy, set up
		//-------------------------------------------------------
		if ( bestenemy != NULL )
		{
			bot->lastEnemy = qtrue;
			bot->enemy = bestenemy;
			return ( qtrue );
		}
	}

#ifdef __NPC__
	if ( npc_minplayers.integer > 0 )
	{												// FIXME: npc_minplayers will not be the only time npcs can be enemies.. eg: map specific ones...
		vec3_t	dist;
		for ( i = MAX_CLIENTS; i < MAX_GENTITIES; i++ )
		{											// Now check NPCs...
			tempent = &g_entities[i];

			// if slot is not used go next
			if ( !tempent->s.eType != ET_NPC )
			{
				continue;
			}

			if ( tempent->health <= 0 )
			{
				continue;
			}

			if ( OnSameTeam( bot, tempent) )
			{
				continue;
			}

			//if ( MyVisible(bot, tempent) )
			{
				VectorSubtract( bot->r.currentOrigin, tempent->r.currentOrigin, dist );
				weight = VectorLength( dist );
				if ( MyInFOV( tempent, bot, 110, 90) && (weight < max_weight && weight < bestweight) )
				{
					if ( weight < bestweight )
					{
						if ( bot->client->ps.stats[STAT_PLAYER_CLASS] == PC_FIELDOPS )
						{
							if ( !MyVisible( bot, tempent) )
							{
								continue;
							}
						}

						//if ( MyVisible(bot, tempent) )
						{
							bestweight = weight;
							bestenemy = tempent;

							/*if (VectorDistance(bot->r.currentOrigin, tempent->r.currentOrigin) < 256)
							{// Will do us.. Dont bother to vischeck further...
								bot->lastEnemy = qtrue;
								bot->enemy = bestenemy;
								return qtrue;
							}*/
						}
					}
				}
			}

			// If best enemy, set up
			//-------------------------------------------------------
			if ( bestenemy != NULL )
			{
				bot->lastEnemy = qtrue;
				bot->enemy = bestenemy;
				return ( qtrue );
			}
		}
	}
#endif //__NPC__
	return ( qfalse );
}

int nextchat[MAX_CLIENTS];


// Check for any generic ingame sounds we might need...
void
Check_VoiceChats ( gentity_t *bot )
{
#ifdef __NOISY_BOTS__
	int clientNum = bot->s.clientNum;
	int testclient = 0;
	if ( !nextchat[bot->s.clientNum] )
	{					// Initialize!
		nextchat[bot->s.clientNum] = level.time + Q_TrueRand( 1000, 10000 );
	}

	if ( nextchat[bot->s.clientNum] <= 0 )
	{					// Initialize!
		nextchat[bot->s.clientNum] = level.time + Q_TrueRand( 1000, 10000 );
	}

	while ( testclient < MAX_CLIENTS )
	{
		if ( testclient == bot->s.clientNum )
		{
			testclient++;
			continue;
		}

		if ( VectorDistance( g_entities[testclient].r.currentOrigin, bot->r.currentOrigin) <= 48 )
		{				// Too close.. Back Off Man!!!
			if ( nextchat[bot->s.clientNum] < level.time )
			{
				nextchat[bot->s.clientNum] = level.time + Q_TrueRand( 8000, 30000 );
				if ( rand() % 10 < 6 )
				{		//level.time < 5000)
					G_Voice( bot, NULL, SAY_TEAM, "LetsGo", qfalse );
				}
				else
				{
					G_Voice( bot, NULL, SAY_TEAM, "Move", qfalse );
				}
				break;
			}
		}

		if ( g_entities[testclient].is_bot )
		{
			if ( g_entities[testclient].current_node == bot->current_node )
			{			// Too close.. Back Off ManBot!!!
				if ( nextchat[bot->s.clientNum] < level.time )
				{
					nextchat[bot->s.clientNum] = level.time + Q_TrueRand( 8000, 30000 );

					//G_Voice( bot, NULL, SAY_TEAM, "Move", qfalse );
					if ( rand() % 10 < 6 )
					{	//level.time < 5000)
						G_Voice( bot, NULL, SAY_TEAM, "LetsGo", qfalse );
					}
					else
					{
						G_Voice( bot, NULL, SAY_TEAM, "Move", qfalse );
					}
					break;
				}
			}
		}

		testclient++;
	}
#endif //__NOISY_BOTS__
}


/* */
void
AIMOD_AI_BotPain ( gentity_t *self, gentity_t *attacker, int damage, vec3_t point )
{
	G_Printf( "%s\n", attacker->client->pers.netname );
}

int next_drop_time[MAX_CLIENTS];
int drop_count[MAX_CLIENTS];


/* */
void
AIMOD_AI_DropMed ( gentity_t *bot )
{
	usercmd_t	*ucmd;
	ucmd = &bot->client->pers.cmd;
	if ( next_drop_time[bot->s.clientNum] <= level.time )
	{
		if (bot->client->ps.weapon != WP_MEDKIT)
			ucmd->weapon = WP_MEDKIT;

		FireWeapon( bot );
		next_drop_time[bot->s.clientNum] = level.time + 3000;	// 3 secs between each drop.
	}
	else
	{
		Set_Best_AI_weapon( bot );
	}
}


/* */
void
AIMOD_AI_DropAmmo ( gentity_t *bot )
{
	usercmd_t	*ucmd;
	ucmd = &bot->client->pers.cmd;
	if ( next_drop_time[bot->s.clientNum] <= level.time )
	{
		if (bot->client->ps.weapon != WP_AMMO)
			ucmd->weapon = WP_AMMO;

		FireWeapon( bot );
		next_drop_time[bot->s.clientNum] = level.time + 3000;	// 3 secs between each drop.
	}
	else
	{
		Set_Best_AI_weapon( bot );
	}
}


/* */
void
AIMOD_AI_BackFromDrop ( gentity_t *bot )
{
	usercmd_t	*ucmd;
	ucmd = &bot->client->pers.cmd;
	Set_Best_AI_weapon( bot );
}


/* */
qboolean
AI_PM_SlickTrace ( vec3_t point, int clientNum )
{
	trace_t trace;
	vec3_t	point2;
	VectorCopy( point, point2 );
	point2[2] = point2[2] - 0.25f;

	//pm->trace (&trace, point, playerMins, playerMaxs, point2, clientNum, pm->tracemask/*MASK_SHOT*/); // pm->tracemask
	trap_Trace( &trace, point, playerMins, playerMaxs, point2, clientNum, MASK_SHOT );
	if ( trace.surfaceFlags & SURF_SLICK )
	{
		return ( qtrue );
	}
	else
	{
		return ( qfalse );
	}
}


/*
=================
Count_Bot_Players

  Counts the number of bots currently on the server in the specific team
  Use a team of -1 to count all bots
=================
*/
int
Count_Bot_Players ( int team )
{
	int loop;
	int botcount = 0;

	for( loop = 0; loop < level.maxclients; loop++ )
	{
		if (
			g_entities[loop].inuse
			&& g_entities[loop].client != 0
			&& (g_entities[loop].r.svFlags & SVF_BOT)
			&& ( team == -1
				|| g_entities[loop].client->sess.sessionTeam == team
				)
		)
		{
			botcount++;
		}
		/*
		else if ( (g_entities[loop].r.svFlags & SVF_BOT) &&  )
		*/
	}

	return ( botcount );
}


int next_ready_check = 0;


/* */
void
Ready_Bot_Players ( void )
{
	int loop = 0;

	//	int botcount = 0;
	if ( next_ready_check > level.time )
	{
		return;
	}

	next_ready_check = level.time + 3000;
	while ( loop < level.maxclients )
	{
		if ( !g_entities[loop].client )
		{

		}
		else if ( !g_entities[loop].inuse )
		{

		}
		else if ( g_entities[loop].r.svFlags & SVF_BOT )
		{
			g_entities[loop].client->pers.ready = qtrue;
		}

		loop++;
	}
}

/*
======================
Count_Playerclass 

  Counts how much players there are in a given team with a specific playerclass
  Use team == -1 to count players from both teams
======================
*/
int
Count_Playerclass ( int playerclass, int team )
{
	int loop;
	int count = 0;

	for( loop = 0; loop < level.maxclients; loop++ )
	{
		if (
			g_entities[loop].inuse
			&& g_entities[loop].client != 0
			&& ( team == -1
				|| g_entities[loop].client->sess.sessionTeam == team
				)
			&& g_entities[loop].client->sess.playerType == playerclass
		)
		{
			count++;
		}
		/*
		else if ( (g_entities[loop].r.svFlags & SVF_BOT) &&  )
		*/
	}

	return ( count );
}

/* */
int
Count_Eng_Bot_Players ( void )
{
	int loop = 0;
	int botcount = 0;
	while ( loop < level.maxclients )
	{
		if ( !g_entities[loop].client )
		{

		}
		else if ( (g_entities[loop].r.svFlags & SVF_BOT) && g_entities[loop].client->sess.playerType == PC_ENGINEER )
		{
			botcount++;
		}

		loop++;
	}

	return ( botcount );
}


/* */
int
Count_Medic_Bot_Players ( void )
{
	int loop = 0;
	int botcount = 0;
	while ( loop < level.maxclients )
	{
		if ( !g_entities[loop].client )
		{

		}
		else if ( (g_entities[loop].r.svFlags & SVF_BOT) && g_entities[loop].client->sess.playerType == PC_MEDIC )
		{
			botcount++;
		}

		loop++;
	}

	return ( botcount );
}


/* */
int
Count_FldOps_Bot_Players ( void )
{
	int loop = 0;
	int botcount = 0;
	while ( loop < level.maxclients )
	{
		if ( !g_entities[loop].client )
		{

		}
		else if ( (g_entities[loop].r.svFlags & SVF_BOT) && g_entities[loop].client->sess.playerType == PC_FIELDOPS )
		{
			botcount++;
		}

		loop++;
	}

	return ( botcount );
}

/* */
int
Pick_Redundant_Bot ( int team )
{
	int clnum;

	clnum = level.maxclients;
	while
		(
			clnum >= 0
			&&	(
					! g_entities[clnum].client
					|| ! (g_entities[clnum].r.svFlags & SVF_BOT)
					|| g_entities[clnum].client->sess.sessionTeam != team
				)
		)
	{
		clnum--;
	}

	return clnum;
}



/* */
int
Pick_Bot_Kick ( void )
{
	int loop = 0;

	//	int botcount = 0;
	loop = level.maxclients;
	while ( loop >= 0 )
	{
		if ( !g_entities[loop].client )
		{

		}
		else if ( g_entities[loop].r.svFlags & SVF_BOT )
		{
			g_entities[loop].is_bot = qfalse;
			return ( loop );
		}

		loop--;
	}

	return ( -1 );
}


/* */
int
Count_Human_Players ( void )
{
	int loop;
	int botcount = 0;

	for( loop = 0; loop < level.maxclients; loop++ )
	{
		if (
			g_entities[loop].client != 0
			&& !(g_entities[loop].r.svFlags & SVF_BOT)
			&& level.clients[loop].pers.connected == CON_CONNECTED
			&&	(
					g_entities[loop].client->sess.sessionTeam == TEAM_AXIS
					|| g_entities[loop].client->sess.sessionTeam == TEAM_ALLIES
				)
		)
		{
			botcount++;
		}
	}

	return ( botcount );
}


/*
==========================
     /\
  __/  |
 |   | | I am a real boy and I will talk!
 |   |O| 
 |__ | | A voice Chat system
    \  | NOTE THIS IS ONLY COMMON TALK WHICH IS ONLY REPLIED NOTHING MORE DONE
     \/
==========================
*/
char	*ai_vchat_idstr[] = { "DynamitePlanted", "GreatShot", "Hi", "Bye", "Thanks", "DropFlag", NULL };
typedef enum
{
	AIVCHAT_DYNAMITEPLANTED,
	AIVCHAT_GREATSHOT,
	AIVCHAT_HI,
	AIVCHAT_BYE,
	AIVCHAT_THANKS,
	AIVCHAT_DROPFLAG,
} ai_vchat_id_t;


/* */
void
G_AI_Voice
(
	gentity_t		*bot,
	gentity_t		*target,
	int				mode,
	const char		*id,
	qboolean		voiceonly,
	int				delay,
	aiVoiceType_t	type
)
{
	bot->AIvoiceChat[type].delay = level.time + delay;
	bot->AIvoiceChat[type].target = target;
	bot->AIvoiceChat[type].mode = mode;
	bot->AIvoiceChat[type].id = id;
	bot->AIvoiceChat[type].voiceonly = voiceonly;
	bot->AIvoiceChat[type].played = qtrue;
}

/*
===============
AI_Should_Talk
===============
*/
extern vmCvar_t bot_minplayers;


/* */
qboolean
AI_Should_Talk ( void )
{	// Stop server command overflows when there are a lot of bots...
	qboolean	should_talk = qtrue;

	// See if we feel like talking or not... UQ1: Based on number of bots online...
	if ( bot_minplayers.integer >= 48 )
	{
		if ( rand() % 100 < 85 )
		{
			should_talk = qfalse;
		}
	}
	else if ( bot_minplayers.integer >= 32 )
	{
		if ( rand() % 100 < 75 )
		{
			should_talk = qfalse;
		}
	}
	else if ( bot_minplayers.integer >= 24 )
	{
		if ( rand() % 100 < 70 )
		{
			should_talk = qfalse;
		}
	}
	else if ( bot_minplayers.integer >= 16 )
	{
		if ( rand() % 100 < 50 )
		{
			should_talk = qfalse;
		}
	}
	else if ( bot_minplayers.integer >= 8 )
	{
		if ( rand() % 100 < 30 )
		{
			should_talk = qfalse;
		}
	}
	else
	{
		if ( rand() % 100 < 20 )
		{
			should_talk = qfalse;
		}
	}

	return ( should_talk );
}


/*
===============
AI_RecordVoiceChat
===============
*/
void
AI_RecordVoiceChat ( gentity_t *other, const char *id )
{
	int			i, j, vchat_id;
	gentity_t	*bot;
	if ( other->r.svFlags & SVF_BOT )
	{			// do not reply to bots please
		return;
	}

	vchat_id = -1;
	for ( i = 0; ai_vchat_idstr[i]; i++ )
	{
		if ( !Q_stricmp( id, ai_vchat_idstr[i]) )
		{
			vchat_id = i;
			break;
		}
	}

	if ( vchat_id == -1 )
	{
		return; // not a known vchat
	}

	for ( j = 0; j < MAX_CLIENTS; j++ )
	{
		if ( !g_entities[j].inuse )
		{
			continue;
		}

		if ( !g_entities[j].client )
		{
			continue;
		}

		bot = &g_entities[j];
		if ( bot->client->ps.clientNum == other->client->ps.clientNum )
		{
			continue;
		}

		if ( !(bot->r.svFlags & SVF_BOT) )
		{
			continue;
		}

		switch ( vchat_id )
		{
			case AIVCHAT_THANKS:

				// did we just helped them?
				// supporting revive, healing and giving ammo now
				if ( !OnSameTeam( bot, other) )
				{
					break;
				}

				if ( bot->last_helped_client != other->client->ps.clientNum )
				{
					break;
				}

				if ( bot->last_helped_time < level.time - 10000 )
				{
					break;
				}

				G_AI_Voice( bot, NULL, SAY_TEAM, "Welcome", qfalse, 1000 + Q_TrueRand( 0, 500), AIVOICE_TALK );
				break;
			case AIVCHAT_GREATSHOT:

				// did we just kill them?
				if ( other->health > 0 )
				{
					break;
				}

				if ( other->client->lasthurt_client != bot->client->ps.clientNum )
				{
					break;
				}

				if ( bot->client->lastKillTime < level.time - 10000 )
				{
					break;
				}

				if ( rand() % 100 < 40 )
				{
					G_AI_Voice( bot, NULL, SAY_ALL, "Cheer", qfalse, 1000 + Q_TrueRand( 0, 500), AIVOICE_FUNTALK );
				}
				else
				{
					G_AI_Voice( bot, NULL, SAY_ALL, "Thanks", qfalse, 1000 + Q_TrueRand( 0, 500), AIVOICE_FUNTALK );
				}
				break;
			case AIVCHAT_HI:
				if ( AI_Should_Talk() )
				{
					G_AI_Voice( bot, NULL, SAY_ALL, "Hi", qfalse, 1000 + Q_TrueRand( 0, 5000), AIVOICE_FUNTALK );
				}
				break;
			case AIVCHAT_BYE:
				if ( AI_Should_Talk() )
				{
					G_AI_Voice( bot, NULL, SAY_ALL, "Bye", qfalse, 1000 + Q_TrueRand( 0, 5000), AIVOICE_FUNTALK );
				}
				break;
		}
	}
}
#endif
