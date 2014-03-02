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
 * ET <-> Omni-Bot interface header file.
 * 
 */
#ifdef OMNIBOTS
#ifndef __G_ETBOT_INTERFACE_H__
#define __G_ETBOT_INTERFACE_H__
#include "Omni-Bot_Types.h"
#include "Omni-Bot_Events.h"
#include "ET_Config.h"
#define NO_BOT_SUPPORT

// IMPORTANT: when changed this has to be copied manually to GAMEVERSION (g_local.h)
#define OMNIBOT_NAME		"Omni-Bot:etmain"
#define OMNIBOT_VERSION		"0.5 beta"
#define OMNIBOT_URL			"www.omni-bot.com"
#define OMNIBOT_MODNAME		"etmain"
#define OMNIBOT_MODVERSION	"2.60"
extern BotUserData	botUserData;
int					Bot_Interface_Init ( void );
int					Bot_Interface_Shutdown ( void );
void				Bot_Interface_SendEvent ( int _eid, int _dest, int _source, int _msdelay, BotUserData *_data );
void				Bot_Interface_SendGlobalEvent ( int _eid, int _source, int _msdelay, BotUserData *_data );
void				Bot_Interface_Update ( void );
int					Bot_Interface_ConsoleCommand ( void );
void				Bot_Interface_LogOutput ( const char *_txt );
void				Bot_Util_AddGoal
					(
						const GameEntity	_ent,
						int					_goaltype,
						int					_team,
						const char			*_tag,
						BotUserData			*_bud
					);
void				Bot_Util_SendSoundEvent ( int _client, int _sndtype, GameEntity _source );
void				Bot_Util_SendTrigger ( TriggerInfo *_triggerInfo );
#endif
#endif
