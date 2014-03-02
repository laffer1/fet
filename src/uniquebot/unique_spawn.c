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

#include "../game/botlib.h"		//bot lib interface
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "../botai/botai.h"			//bot ai interface

//#include "../game/ai_cast.h"
//#include "../game/ai_cast_global.h"


//#include "uniquebot.h"
// -- Globals
int			number_of_bots = 0;

// ----------------------------------------------------------------
const char	*FALLBACKfirstNames[] =
{
	"Evil",
	"Dark",
	"Redneck",
	"Braindead",
	"Killer",
	"Last",
	"First",
	"John",
	"Light",
	"Fast",
	"Slow",
	"Nooby",
	"Professor",
	"Master",
	"Looser",
	"Owned",
	"Red",
	"Shadow",
	"Sneaky",
	"Broken",
	"Lost",
	"Hidden",
	"Silent",
	"The",
	"Da",
	"Your",
	"Laggy",
	"ET",
	"Angry",
	"Confused",
	"Lucky",
	"Bad",
	"Charlie",
	"Hungry",
	"Living",
	"Dead",
	"Johnny",
	"Defiant",
	"Green",
	"Lazy",
	"Mr",
	"Miss",
	"Mrs",
	"Young",
	"Old",
	"New",
	"Ancient",
	"Falling",
	"Fallen",
	"Darkened",
	"Idiot",
	"Moron",
	"Stupid",
	"Running",
	"Hiding",
	"Rising",
	"Jumping",
	"Retreating",
	"Commander",
	"Funny",
	"Faster",
	"Slower",
	"Grand",
	"Shallow",
	"Hollow",
	"Under",
	"Lesser",
	"Micro",
	"Macro",
	"Jack",
	"Dickie",
	"Dancing",
	"Lasting",
	"Fastest",
	"Slowest",
	"Needy",
	"Sniping",
	"Medic",
	"Hero",
	"Our",
	"My",
	"Holy",
	"The",
	"The",
	"The",
	"The",
	"Da",
	"Da",
	"Da",
	"Da",
	"Big",
	"Small",
	"Mad",
	"Crazy",
	"Black"
};	// 95
const char	*FALLBACKlastNames[] =
{
	"One",
	"Avenger",
	"Destroyer",
	"Killer",
	"Protector",
	"Assassin",
	"Defender",
	"Attacker",
	"Master",
	"Knight",
	"1",
	"Lagalot",
	"Spacefiller",
	"Player",
	"69",
	"101",
	"Hunter",
	"Jack",
	"Lamer",
	"Death",
	"Hidden",
	"Runner",
	"Skywalker",
	"Soul",
	"Hacker",
	"Hack",
	"Noob",
	"Missfire",
	"Gunfire",
	"Gunslinger",
	"Stabber",
	"Backstab",
	"Swiftkick",
	"Swift",
	"Slasher",
	"007",
	"Leadridden",
	"Leadrush",
	"Killer",
	"Jr",
	"Virus",
	"Trojen",
	"Guardmaster",
	"Attacker",
	"Battlemaster",
	"Kitty",
	"Wolf",
	"Fox",
	"Idiot",
	"Jack",
	"John",
	"Joe",
	"Mac",
	"Frontrunner",
	"Man",
	"Woman",
	"Dog",
	"Hog",
	"Fist",
	"Core",
	"Jackson",
	"Voss",
	"Vlad",
	"Voodoo",
	"Lost",
	"Doom",
	"Death",
	"Botman"
};	// 68
typedef struct
{
	char	Armericanfirstname[MAX_QPATH];
	char	Americanlastname[MAX_QPATH];
	char	Germanfirstname[MAX_QPATH];
	char	Germanlastname[MAX_QPATH];
} name_list_t;
name_list_t Name_List[4096];
qboolean	NOAmericanFirstNames = qfalse;
qboolean	NOGermanFirstNames = qfalse;
int			num_american_first_names = 0;
int			num_german_first_names = 0;
qboolean	NOAmericanLastNames = qfalse;
qboolean	NOGermanLastNames = qfalse;
int			num_american_last_names = 0;
int			num_german_last_names = 0;


/* */
void
Load_Bot_americanFirstNames ( void )
{				// Load bot first names from external file.
	char			*s, *t;
	int				len;
	fileHandle_t	f;
	char			*buf;
	char			*loadPath;

	//	int alt = 0;
	//	int num1 = 0, num2 = 0;
	int				num = 0;
	loadPath = va( "americanFirstNames.cfg\0" );
	len = trap_FS_FOpenFile( loadPath, &f, FS_READ );
	if ( !f )
	{
		G_Printf( "^1*** ^3WARNING^5: No (american) bot first names file (^7%s^5). Using default name list.\n", loadPath );
		NOAmericanFirstNames = qtrue;
		return;
	}

	if ( !len )
	{			//empty file
		trap_FS_FCloseFile( f );
		NOAmericanFirstNames = qtrue;
		return;
	}

	if ( (buf = G_Alloc( len + 1)) == 0 )
	{			//alloc memory for buffer
		NOAmericanFirstNames = qtrue;
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	//len = sizeof(buf);
	//G_Printf("Len is %i\n", len);
	G_Printf( "^4*** ^3%s^4: ^5Loading (american) bot first names from file ^7%s^5.\n", GAME_VERSION, loadPath );
	for ( t = s = buf; *t; /* */ )
	{
		num++;
		s = strchr( s, '\n' );
		if ( !s || num > len )
		{
			break;
		}

		while ( *s == '\n' )
		{
			*s++ = 0;
		}

		if ( *t )
		{
			if ( !Q_strncmp( "//", va( "%s", t), 2) == 0 && strlen( va( "%s", t)) > 0 )
			{	// Not a comment either... Record it in our list...
				Q_strncpyz( Name_List[num_american_first_names].Armericanfirstname, va( "%s", t), strlen( va( "%s", t)) );

				//G_Printf("(#%i) Added %s.\n", num_american_first_names, Name_List[num_american_first_names].Armericanfirstname);
				num_american_first_names++;
			}
		}

		t = s;
	}

	num_american_first_names--;
	G_Printf( "^4*** ^3%s^4: ^5There are ^7%i^5 (american) first names in the database.\n", GAME_VERSION,
			  num_american_first_names );

	//	Q_free(buf);
}


/* */
void
Load_Bot_americanLastNames ( void )
{				// Load bot last names from external file.
	char			*s, *t;
	int				len;
	fileHandle_t	f;
	char			*buf;
	char			*loadPath;

	//	int alt = 0;
	//	int num1 = 0, num2 = 0;
	int				num = 0;
	loadPath = va( "americanLastNames.cfg\0" );
	len = trap_FS_FOpenFile( loadPath, &f, FS_READ );
	if ( !f )
	{
		G_Printf( "^1*** ^3WARNING^5: No (american) bot last names file (^7%s^5). Using default name list.\n", loadPath );
		NOAmericanLastNames = qtrue;
		return;
	}

	if ( !len )
	{			//empty file
		trap_FS_FCloseFile( f );
		NOAmericanLastNames = qtrue;
		return;
	}

	if ( (buf = G_Alloc( len + 1)) == 0 )
	{			//alloc memory for buffer
		NOAmericanLastNames = qtrue;
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );
	G_Printf( "^4*** ^3%s^4: ^5Loading (american) bot last names from file ^7%s^5.\n", GAME_VERSION, loadPath );
	for ( t = s = buf; *t; /* */ )
	{
		num++;
		s = strchr( s, '\n' );
		if ( !s || num > len )
		{
			break;
		}

		while ( *s == '\n' )
		{
			*s++ = 0;
		}

		if ( *t )
		{
			if ( !Q_strncmp( "//", va( "%s", t), 2) == 0 && strlen( va( "%s", t)) > 0 )
			{	// Not a comment either... Record it in our list...
				Q_strncpyz( Name_List[num_american_last_names].Americanlastname, va( "%s", t), strlen( va( "%s", t)) );
				num_american_last_names++;
			}
		}

		t = s;
	}

	num_american_last_names--;
	G_Printf( "^4*** ^3%s^4: ^5There are ^7%i^5 (american) last names in the database.\n", GAME_VERSION,
			  num_american_last_names );

	//	Q_free(buf);
}


/* */
void
Load_Bot_germanFirstNames ( void )
{				// Load bot first names from external file.
	char			*s, *t;
	int				len;
	fileHandle_t	f;
	char			*buf;
	char			*loadPath;

	//	int alt = 0;
	//	int num1 = 0, num2 = 0;
	int				num = 0;
	loadPath = va( "germanFirstNames.cfg\0" );
	len = trap_FS_FOpenFile( loadPath, &f, FS_READ );
	if ( !f )
	{
		G_Printf( "^1*** ^3WARNING^5: No (german) bot first names file (^7%s^5). Using default name list.\n", loadPath );
		NOGermanFirstNames = qtrue;
		return;
	}

	if ( !len )
	{			//empty file
		trap_FS_FCloseFile( f );
		NOGermanFirstNames = qtrue;
		return;
	}

	if ( (buf = G_Alloc( len + 1)) == 0 )
	{			//alloc memory for buffer
		NOGermanFirstNames = qtrue;
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );
	G_Printf( "^4*** ^3%s^4: ^5Loading (german) bot first names from file ^7%s^5.\n", GAME_VERSION, loadPath );
	for ( t = s = buf; *t; /* */ )
	{
		num++;
		s = strchr( s, '\n' );
		if ( !s || num > len )
		{
			break;
		}

		while ( *s == '\n' )
		{
			*s++ = 0;
		}

		if ( *t )
		{
			if ( !Q_strncmp( "//", va( "%s", t), 2) == 0 && strlen( va( "%s", t)) > 0 )
			{	// Not a comment either... Record it in our list...
				Q_strncpyz( Name_List[num_german_first_names].Germanfirstname, va( "%s", t), strlen( va( "%s", t)) );
				num_german_first_names++;
			}
		}

		t = s;
	}

	num_german_first_names--;
	G_Printf( "^4*** ^3%s^4: ^5There are ^7%i^5 (german) first names in the database.\n", GAME_VERSION,
			  num_german_first_names );

	//	Q_free(buf);
}


/* */
void
Load_Bot_germanLastNames ( void )
{				// Load bot last names from external file.
	char			*s, *t;
	int				len;
	fileHandle_t	f;
	char			*buf;
	char			*loadPath;

	//	int alt = 0;
	//	int num1 = 0, num2 = 0;
	int				num = 0;
	loadPath = va( "germanLastNames.cfg\0" );
	len = trap_FS_FOpenFile( loadPath, &f, FS_READ );
	if ( !f )
	{
		G_Printf( "^1*** ^3WARNING^5: No (american) bot last names file (^7%s^5). Using default name list.\n", loadPath );
		NOGermanLastNames = qtrue;
		return;
	}

	if ( !len )
	{			//empty file
		trap_FS_FCloseFile( f );
		NOGermanLastNames = qtrue;
		return;
	}

	if ( (buf = G_Alloc( len + 1)) == 0 )
	{			//alloc memory for buffer
		NOGermanLastNames = qtrue;
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );
	G_Printf( "^4*** ^3%s^4: ^5Loading (german) bot last names from file ^7%s^5.\n", GAME_VERSION, loadPath );
	for ( t = s = buf; *t; /* */ )
	{
		num++;
		s = strchr( s, '\n' );
		if ( !s || num > len )
		{
			break;
		}

		while ( *s == '\n' )
		{
			*s++ = 0;
		}

		if ( *t )
		{
			if ( !Q_strncmp( "//", va( "%s", t), 2) == 0 && strlen( va( "%s", t)) > 0 )
			{	// Not a comment either... Record it in our list...
				Q_strncpyz( Name_List[num_german_last_names].Germanlastname, va( "%s", t), strlen( va( "%s", t)) );
				num_german_last_names++;
			}
		}

		t = s;
	}

	num_german_last_names--;
	G_Printf( "^4*** ^3%s^4: ^5There are ^7%i^5 (german) last names in the database.\n", GAME_VERSION,
			  num_german_last_names );

	//	Q_free(buf);
}


/* */
void
Load_Bot_FirstNames ( void )
{
	Load_Bot_americanFirstNames();
	Load_Bot_germanFirstNames();
}


/* */
void
Load_Bot_LastNames ( void )
{
	Load_Bot_americanLastNames();
	Load_Bot_germanLastNames();
}


/* */
char *
PickName ( int teamNum )
{			// Choose a random name by combining!
	//char *firstname = "", *lastname = "";
	char	firstname[256];
	char	lastname[256];

	//	int color1 = rand()%7;
	//	int color2 = rand()%7;
	if ( teamNum == TEAM_ALLIES )
	{
		if ( NOAmericanFirstNames || num_american_first_names <= 0 )
		{	// Use the hard-coded name list..
			int choice1 = rand() % 95;
			strcpy( firstname, va( "%s", FALLBACKfirstNames[choice1]) );
		}
		else
		{	// From external file..
			int choice1 = rand() % num_american_first_names;
			strcpy( firstname, va( "%s", Name_List[choice1].Armericanfirstname) );
		}
	}
	else
	{
		if ( NOGermanFirstNames || num_german_first_names <= 0 )
		{	// Use the hard-coded name list..
			int choice1 = rand() % 95;
			strcpy( firstname, va( "%s", FALLBACKfirstNames[choice1]) );
		}
		else
		{	// From external file..
			int choice1 = rand() % num_german_first_names;
			strcpy( firstname, va( "%s", Name_List[choice1].Germanfirstname) );
		}
	}

	if ( teamNum == TEAM_ALLIES )
	{
		if ( NOAmericanLastNames || num_american_last_names <= 0 )
		{	// Use the hard-coded name list..
			int choice2 = rand() % 68;
			strcpy( lastname, va( "%s", FALLBACKlastNames[choice2]) );
		}
		else
		{	// From external file..
			int choice2 = rand() % num_american_last_names;
			strcpy( lastname, va( "%s", Name_List[choice2].Americanlastname) );
		}
	}
	else
	{
		if ( NOGermanLastNames || num_german_last_names <= 0 )
		{	// Use the hard-coded name list..
			int choice2 = rand() % 68;
			strcpy( lastname, va( "%s", FALLBACKlastNames[choice2]) );
		}
		else
		{	// From external file..
			int choice2 = rand() % num_german_last_names;
			strcpy( lastname, va( "%s", Name_List[choice2].Germanlastname) );
		}
	}

	//G_Printf("!!!!!!!!!!! %s\n", va("^%i%s^%i%s", color1, firstname, color2, lastname));
	//return va("^%i%s^%i%s", color1, firstname, color2, lastname);
	return ( va( "%s %s", firstname, lastname) );
}

qboolean	BotMarker[MAX_CLIENTS];

/*
void AIMOD_SaveBotInfo( void )
{
    FILE        *pOut;
    
    if((pOut = fopen(va("botinfo.bi"), "wb" )) == NULL)
        return; // bail

	fwrite(&BotMarker,sizeof(BotMarker),1,pOut);
        
    fclose(pOut);
}

void AIMOD_LoadBotInfo( void )
{
    FILE        *pIn;

    if((pIn = fopen(va("botinfo.bi"), "rb" )) == NULL)
    {
        G_Printf("^1*** ^3%s^4: ^5 No botinfo file found...", GAME_VERSION);
        return; 
    }

	fread(&BotMarker,sizeof(BotMarker),1,pIn);

	fclose(pIn);
}*/


/**
 * Function to add a bot to a team, based on a bot profile.
 * @param team The team where we should put this bot, if no team is set, one will be picked.
 * @param profile Name of the profile for this bot.
 * @return qtrue if bot create succeeded, qfalse if failed
 */
qboolean
SOD_SPAWN_AddBotFromProfile ( char *team, char *profile )
{

	// Read a profile from file, create botstate from that profile
	return ( qfalse );
}


/**
 * Function to add a bot to a team, based on a bot name, and a simple bot skill number.
 * @param team The team where we should put this bot, if no team is set, one will be picked.
 * @param name Name of the bot.
 * @param skill Bot skill number between 0 and 3.
 * @return qtrue if bot create succeeded, qfalse if failed
 */
qboolean
SOD_SPAWN_AddBotShort ( char *team, char *name, int skill, char *userinfo )
{
	sodbot_t	botstate;
	botstate = SOD_SPAWN_ReadProfile( NULL );
	botstate.name = name;
	botstate.skill = skill;

	// Add the bot to the server
	return ( SOD_SPAWN_AddBot( &botstate, team, userinfo) );
}

/**
 * Creates a 'bot' entity and fills in a client slot.
 * @param bs Botstate for this bot.
 * @param team Team where the bot should go into.
 * @param load_userinfo is used by the SOD_SPAWN_LoadBots function.
 *  it reads the userinfo string out of the file and gives it to us :)
 * @return qtrue if bot connect succeeded, qfalse if failed
 */
/*
=================
Count_Bot_Players

  Counts the number of bots currently on the server in the specific team
  Use a team of -1 to count all bots
=================
*/
extern int	Count_Bot_Players ( int team );
extern int	nextchat[MAX_CLIENTS];


/* */
qboolean
SOD_SPAWN_AddBot ( sodbot_t *bs, char *team, char *load_userinfo )
{
	int			i, clientNum;
	gentity_t	*bot;
	char		userinfo[MAX_INFO_STRING];
	qboolean	firstTime;									// for loading after a mapchange bots;

	// See if we haven't reached max number of bots yet
	// ----------------------------------------------------------------
	number_of_bots = Count_Bot_Players( -1 );
	if ( number_of_bots >= MAX_BOTS )
	{
		G_Printf( S_COLOR_RED "Unable to add bot.  Maximum number of bots reached.\n" );
		return ( qfalse );
	}

	// Have the server allocate a client slot
	clientNum = trap_BotAllocateClient( 0 );

	/*if (!trap_BotAllocateClient(clientNum))
	{
		G_Printf( S_COLOR_RED "Unable to add bot.  All player slots are in use.\n" );
		G_Printf( S_COLOR_RED "Start server with more 'open' slots (or check setting of sv_maxclients cvar).\n" );
		return qfalse;
	}*/
	if ( clientNum == -1 )
	{
		G_Printf( S_COLOR_RED "Unable to add bot.  All player slots are in use.\n" );
		G_Printf( S_COLOR_RED "Start server with more 'open' slots (or check setting of sv_maxclients cvar).\n" );
		return ( qfalse );
	}

	// ----------------------------------------------------------------
	// Create the bot's userinfo
	// ----------------------------------------------------------------
	if ( load_userinfo == NULL )
	{														// if /addbot console command is used
		userinfo[0] = '\0';
		Info_SetValueForKey( userinfo, "name", bs->name );
		Info_SetValueForKey( userinfo, "rate", "25000" );
		Info_SetValueForKey( userinfo, "snaps", "20" );
		Info_SetValueForKey( userinfo, "skill", va( "%i", bs->skill) );
		Info_SetValueForKey( userinfo, "team", team );
		Info_SetValueForKey( userinfo, "ip", "127.0.0.1" ); // Bots behave like local clients, they are in fact !
	}

	// ----------------------------------------------------------------
	// Now set the bot entity specifics
	// ----------------------------------------------------------------
	bot = &g_entities[clientNum];
	bot->r.svFlags |= SVF_BOT;
	bot->client->ps.eFlags |= EF_BOT;
	bot->inuse = qtrue;
	bot->is_bot = qtrue;
	bot->scriptStatus.scriptEventIndex = -1;
	bot->s.number = bot - g_entities;
	bot->r.ownerNum = ENTITYNUM_NONE;
	bot->headshotDamageScale = 1.0;
	bot->client->pers.localClient = qtrue;
	bot->client->ps.persistant[PERS_BOT] = qtrue;

	// Set the team, or pick a team if none given
	if ( !team || !*team )
	{
		if ( g_gametype.integer == GT_COOP || g_gametype.integer == GT_SINGLE_PLAYER )
		{
			team = "red";
		}
		else if ( PickTeam( clientNum) == TEAM_AXIS )
		{
			team = "red";
		}
		else
		{
			team = "blue";
		}
	}

	if ( load_userinfo == NULL )
	{														// /addbot
		// Register the userinfo
		trap_SetUserinfo( clientNum, userinfo );
		firstTime = qtrue;
	}
	else
	{														// SOD_SPAWN_LoadBots
		trap_SetUserinfo( clientNum, load_userinfo );
		firstTime = qfalse;
	}

	// ----------------------------------------------------------------
	// Have it connect to the game as a normal client
	// ----------------------------------------------------------------
	// Dont set clientconnect isBot, it's a leftover from other stuff
	if ( ClientConnect( clientNum, firstTime, qtrue /*qfalse*/ ) )
	{
		return ( qfalse );									// Returns NULL if all went okay
	}

	ClientBegin( clientNum );								// Test
	sbots[number_of_bots++] = bot;

	// Okay, all went fine, set the final botstate
	for ( i = 0; i < MAX_BOTS; i++ )
	{
		if ( sodbotstates[i].clientNum == INVALID )
		{
			sodbotstates[i] = *bs;
			sodbotstates[i].clientNum = clientNum;
			bot->bs = &sodbotstates[i];
			break;
		}
	}

	// ----------------------------------------------------------------
	// Set the entity stuff like bounding box etc
	SOD_SPAWN_CreateBot( bot, team );

	// Say a nice hello to the other players

	/*	if ( random() < 0.13 ) {
		G_Voice( bot, NULL, SAY_ALL, va("hi"), qtrue);
		//G_Say( bot, NULL, SAY_ALL, va("Hi.\n") );
		if (rand()%2 <=1)
			G_Say( bot, NULL, SAY_ALL, va("Hi.") );
		else
			G_Say( bot, NULL, SAY_ALL, va("Hello.") );
	}*/

	// Kill them once to start things rolling... Needs to be fixed.
	//G_Damage( bot, bot, bot, bot->s.origin, bot->s.origin, 99999, DAMAGE_NO_PROTECTION, MOD_SUICIDE );
	//bot->suicide_timeout = level.time + 19500.0;
	bot->suicide_timeout = 0;								//level.time + 19500;
	bot->nextthink = level.time + FRAMETIME + rand() % 100;
	bot->client->AIbaseweapon = bot->client->ps.weapon;

	//	bot->client->ps.persistant[PERS_BOT] = 1;
	//	bot->client->saved_persistant[PERS_BOT] = bot->client->ps.persistant[PERS_BOT];
	//bot->classname = "bot";
#ifdef __BOT_AAS__
	if ( trap_AAS_Initialized() )
	{														// If aas is in use.. Set it up!
		BotAISetupClient( clientNum );
	}
#endif
	nextchat[bot->s.clientNum] = level.time + Q_TrueRand( 8000, 30000 );
	return ( qtrue );
}

void	SetBotWeapons ( gentity_t *bot );					// Below.

vmCvar_t		bot_skill;
qboolean		bots_added = qfalse;
int				sp_bot_max = 0;
vec3_t			sp_bot_positions[512];
int				sp_bot_type[512];
int				sp_bot_weapon[512];
int				sp_bot_skilllevel[512];

/**
 *
 */
void
SOD_SPAWN_CreateBot ( gentity_t *bot, char *team_str )
{
	vec3_t				mins = { -18, -18, -24 };			// uit g_client.c gehaald
	vec3_t				maxs = { 18, 18, 48 };				// idem

	//vec3_t              mins = {-16, -16, -24};
	//vec3_t              maxs = {16, 16, 32};
	int					index;
	vec3_t				spawn_origin, spawn_angles;
	gclient_t			*client;
	clientPersistant_t	saved;
	int					pc;									// fretn - player class
	VectorSet( spawn_origin, 0, 0, 0 );						// fretn - say no to compiler warings
	VectorSet( spawn_angles, 0, 0, 0 );						// fretn - say no to compiler warings
	index = bot - g_entities - 1;
	client = bot->client;

	// clear everything but the persistant data
	//-------------------------------------------------------
	saved = client->pers;
	memset( &client->sess, 0, sizeof(client->sess) );		// fretn
	memset( &client->pers, 0, sizeof(client->pers) );		// fretn
	memset( client, 0, sizeof(*client) );

	if (bot->botClassnum <= 0)
		pc = SOD_SPAWN_ChooseClass( TeamForName(team_str) );
	else 
	{
		bot->botClassnum = sp_bot_type[bot->bot_sp_indentifier];
		pc = bot->botClassnum;
	}

	bot->botClassnum = pc;

	client->pers = saved;
	client->ps.stats[STAT_PLAYER_CLASS] = pc;
	client->sess.latchPlayerType = pc;
	client->sess.playerType = pc;
	bot->takedamage = qtrue;
	bot->classname = "bot";
	bot->r.contents = CONTENTS_SOLID;
	bot->clipmask = MASK_PLAYERSOLID;
	bot->die = player_die;									// dunno of dat hier werkt
	bot->waterlevel = 0;
	bot->watertype = 0;
	bot->flags &= ~FL_NO_KNOCKBACK;
	bot->enemy = NULL;										// moet in botstate komen
	bot->r.svFlags |= SVF_BOT;
	VectorCopy( mins, bot->client->ps.mins );
	VectorCopy( maxs, bot->client->ps.maxs );
	VectorClear( bot->client->ps.velocity );

	// clear playerstate values
	//-------------------------------------------------------
	memset( &bot->client->ps, 0, sizeof(client->ps) );
	client->ps.origin[0] = spawn_origin[0] * 8;
	client->ps.origin[1] = spawn_origin[1] * 8;
	client->ps.origin[2] = spawn_origin[2] * 8;
	VectorCopy( spawn_origin, bot->s.origin );

	// Make sure off ground
	bot->s.origin[2] += 1;

	//-------------------------------------------------------
	bot->s.angles[PITCH] = 0;
	bot->s.angles[YAW] = spawn_angles[YAW];
	bot->s.angles[ROLL] = 0;
	VectorCopy( bot->s.angles, client->ps.viewangles );

	//-------------------------------------------------------
	// die limbo moet vlak voor setteam staan
	// dit voorkomt "killed himself" bij spawn.
	bot->client->ps.pm_flags |= PMF_LIMBO;
	SetTeam( bot, team_str, qtrue, -1, -1, qfalse );		// Test Unique1

	// deze functie meebie gebruiken in toekomst om soldiers easy te spawnen
	//void SetWolfData( gentity_t *ent, char *ptype, char *weap, char *grenade, char *skinnum );
	SetBotWeapons( bot );
	bot->weaponchoice = bot->client->ps.weapon;
	bot->client->AISpecialTime = 0;
	bot->client->AISpecialCount = 0;
	bot->think = SOD_AI_BotThink;							// I can think, I'm alive !

	//	bot->think = AIMOD_AI_Think;
	bot->nextthink = level.time + FRAMETIME + rand() % 100; // moet altijd frametime zijn

	// als je minder neemt kan het de server te veel
	// gaan belasten
	bot->is_bot = qtrue;

	// End of UQ1's Code.
}

#ifdef __OLDBOTWEAPONMETHOD__								// Old way of doing this... WHAT A MESS!!! What were you thinking splash damage???


/* */
void
SetBotWeapons ( gentity_t *bot )
{
	int			pc;
	gclient_t	*client = bot->client;
	if ( bot->client->ps.stats[STAT_PLAYER_CLASS] )
	{
		pc = bot->client->ps.stats[STAT_PLAYER_CLASS];
	}
	else
	{
		if ( bot->botClassnum )
		{
			pc = bot->botClassnum;
		}
		else
		{
			pc = SOD_SPAWN_ChooseClass();
		}
	}

	bot->client->ps.stats[STAT_PLAYER_CLASS] = pc;
	bot->client->sess.latchPlayerType = pc;
	bot->client->sess.playerType = pc;

	// Everyone except Medic and LT get some grenades
	if ( (pc != PC_FIELDOPS) && (pc != PC_MEDIC) )
	{					// JPW NERVE
		switch ( client->sess.sessionTeam )
		{				// was playerItem
			case TEAM_ALLIES:
				Weapon_BitSet( &client->ps, WP_GRENADE_PINEAPPLE );
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE), 4 + 4 * (pc == PC_SOLDIER) );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_GRENADE_PINEAPPLE)] = 4 + 4*(pc == PC_SOLDIER); // JPW NERVE
				break;
			case TEAM_AXIS:
				Weapon_BitSet( &client->ps, WP_GRENADE_LAUNCHER );
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_LAUNCHER), 4 + 4 * (pc == PC_SOLDIER) );

				//client->ps.ammo[BG_FindAmmoForWeapon(WP_GRENADE_LAUNCHER)] = 0;
				//client->ps.ammoclip[BG_FindClipForWeapon(WP_GRENADE_LAUNCHER)] = 4 + 4*(pc == PC_SOLDIER); // JPW NERVE
				break;
			default:
				Weapon_BitSet( &client->ps, WP_GRENADE_PINEAPPLE );
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE), 4 + 4 * (pc == PC_SOLDIER) );

				//client->ps.ammo[BG_FindAmmoForWeapon(WP_GRENADE_PINEAPPLE)] = 0;
				//client->ps.ammoclip[BG_FindClipForWeapon(WP_GRENADE_PINEAPPLE)] = 4 + 4*(pc == PC_SOLDIER); // JPW NERVE
				break;
		}
	}

	// JPW NERVE
	if ( pc == PC_MEDIC )
	{
		Weapon_BitSet( &client->ps, WP_MEDIC_SYRINGE );

		//client->ps.ammoclip[BG_FindClipForWeapon(WP_MEDIC_SYRINGE)] = 10;
		BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MEDIC_SYRINGE), 10 );

		// NERVE - SMF
		Weapon_BitSet( &client->ps, WP_MEDKIT );

		//client->ps.ammoclip[BG_FindClipForWeapon(WP_MEDKIT)] = 1;
		//client->ps.ammo[WP_MEDKIT] = 1;
		BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MEDKIT), 1 );
		switch ( client->sess.sessionTeam )
		{				// was playerItem
			case TEAM_ALLIES:
				Weapon_BitSet( &client->ps, WP_GRENADE_PINEAPPLE );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_GRENADE_PINEAPPLE)] = 1;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE), 1 );
				break;
			case TEAM_AXIS:
				Weapon_BitSet( &client->ps, WP_GRENADE_LAUNCHER );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_GRENADE_LAUNCHER)] = 1;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_LAUNCHER), 1 );
				break;
			default:
				Weapon_BitSet( &client->ps, WP_GRENADE_PINEAPPLE );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_GRENADE_PINEAPPLE)] = 1;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE), 1 );
				break;
		}
	}

	if ( pc == PC_FIELDOPS )
	{

		/*if (Q_TrueRand(0, 1) == 0)
			{// Shotgun...
				Weapon_BitSet( &client->ps,  WP_SHOTGUN );
				//client->ps.ammoclip[BG_FindAmmoForWeapon(WP_SHOTGUN)] = 2;
				AddWeaponToPlayer( client, WP_SHOTGUN, 16, 2, qtrue );
				bot->client->sess.playerWeapon = WP_SHOTGUN;
			}
			else*/
		{
			if ( client->sess.sessionTeam == TEAM_AXIS )
			{			// JPW NERVE
				Weapon_BitSet( &client->ps, WP_MP40 );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MP40), 32 );
				if ( pc == PC_SOLDIER )
				{
					BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MP40), 64 );
				}
				else
				{
					BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MP40), 32 );
				}

				bot->client->sess.playerWeapon = WP_MP40;
			}
			else
			{			// TEAM_ALLIES
				Weapon_BitSet( &client->ps, WP_THOMPSON );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_THOMPSON), 30 );
				if ( pc == PC_SOLDIER )
				{
					BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_THOMPSON), 60 );
				}
				else
				{
					BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_THOMPSON), 30 );
				}

				bot->client->sess.playerWeapon = WP_THOMPSON;
			}
		}

		bot->client->ps.weapon = bot->client->sess.playerWeapon;
		bot->s.weapon = bot->client->sess.playerWeapon;
		bot->client->AIbaseweapon = bot->client->sess.playerWeapon;
	}
	else if ( pc == PC_SOLDIER )
	{
		int choice = rand() % 4;
		switch ( choice )
		{
			case 1:		// WP_MAUSER, WP_SNIPERRIFLE
				Weapon_BitSet( &client->ps, WP_FLAMETHROWER );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_FLAMETHROWER)] = 100;
				//client->ps.ammo[BG_FindAmmoForWeapon(WP_FLAMETHROWER)] = 200;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_FLAMETHROWER), 200 );
				bot->client->sess.playerWeapon = WP_FLAMETHROWER;
				break;
			case 2:		// WP_PANZERFAUST
				Weapon_BitSet( &client->ps, WP_PANZERFAUST );

				//client->ps.ammo[BG_FindAmmoForWeapon(WP_PANZERFAUST)] = 4;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_PANZERFAUST), 4 );
				bot->client->sess.playerWeapon = WP_PANZERFAUST;
				break;
			case 3:		// WP_MOBILE_MG42
				Weapon_BitSet( &client->ps, WP_MOBILE_MG42 );

				//client->ps.ammoclip[BG_FindAmmoForWeapon(WP_MOBILE_MG42)] = 500;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MOBILE_MG42), 500 );
				bot->client->sess.playerWeapon = WP_MOBILE_MG42;
				break;

			/*case 4:		// WP_SHOTGUN
				Weapon_BitSet( &client->ps,  WP_SHOTGUN );
				//client->ps.ammoclip[BG_FindAmmoForWeapon(WP_SHOTGUN)] = 2;
				AddWeaponToPlayer( client, WP_SHOTGUN, 16, 2, qtrue );
				bot->client->sess.playerWeapon = WP_SHOTGUN;
				break;*/
			default:	// give MP40 if given invalid weapon number
				Weapon_BitSet( &client->ps, WP_MOBILE_MG42 );

				//client->ps.ammoclip[BG_FindAmmoForWeapon(WP_MOBILE_MG42)] = 500;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MOBILE_MG42), 500 );
				bot->client->sess.playerWeapon = WP_MOBILE_MG42;

				/*
				if (client->sess.sessionTeam == TEAM_AXIS) { // JPW NERVE
					Weapon_BitSet( &client->ps,  WP_MP40 );
					client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
					if (pc == PC_SOLDIER)
						client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += 64;
					else
						client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += 32;
					bot->client->sess.playerWeapon = WP_MP40;
				}
				else { // TEAM_ALLIES
					Weapon_BitSet( &client->ps,  WP_THOMPSON );
					client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
					if (pc == PC_SOLDIER)
						client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += 60;
					else
						client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += 30;
					bot->client->sess.playerWeapon = WP_THOMPSON;
				}*/
				break;
		}

		bot->client->ps.weapon = bot->client->sess.playerWeapon;
		bot->s.weapon = bot->client->sess.playerWeapon;
		bot->client->AIbaseweapon = bot->client->sess.playerWeapon;
	}

	/*		else if (pc == PC_COVERTOPS)
		{// Soldiers chose weapon randomly. (Except pistols and WP_NONE).
			int choice = rand()%3;

			switch( choice ) 
			{
				case 1:
					Weapon_BitSet( &client->ps,  WP_GARAND );
					client->ps.ammoclip[BG_FindClipForWeapon(WP_GARAND)] += 30;
					bot->client->sess.playerWeapon = WP_GARAND;
					break;
				case 2:
					Weapon_BitSet( &client->ps,  WP_K43 );
					client->ps.ammoclip[BG_FindClipForWeapon(WP_K43)] += 30;
					bot->client->sess.playerWeapon = WP_K43;
					break;
				case 3:
					Weapon_BitSet( &client->ps,  WP_FG42 );
					client->ps.ammoclip[BG_FindClipForWeapon(WP_FG42)] += 30;
					bot->client->sess.playerWeapon = WP_FG42;
					break;
				default:
					Weapon_BitSet( &client->ps,  WP_STEN );
					client->ps.ammoclip[BG_FindClipForWeapon(WP_STEN)] += 32;	
					bot->client->sess.playerWeapon = WP_STEN;
					break;
			}

			bot->client->ps.weapon = bot->client->sess.playerWeapon;
			bot->s.weapon = bot->client->sess.playerWeapon;
			bot->client->AIbaseweapon = bot->client->sess.playerWeapon;
		}*/
	else if ( pc == PC_SOLDIER )
	{					// Soldiers chose weapon randomly. (Except pistols and WP_NONE).
		int choice = rand() % 3;
		if ( client->sess.sessionTeam == TEAM_AXIS )
		{				// JPW NERVE
			if ( choice == 1 )
			{
				Weapon_BitSet( &client->ps, WP_MP40 );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MP40), 32 );
				bot->client->sess.playerWeapon = WP_MP40;
			}
			else
			{
				Weapon_BitSet( &client->ps, WP_KAR98 );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_KAR98)] += 30;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_KAR98), 30 );
				bot->client->sess.playerWeapon = WP_KAR98;
			}
		}
		else
		{				// TEAM_ALLIES
			if ( choice == 1 )
			{
				Weapon_BitSet( &client->ps, WP_THOMPSON );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 32;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_THOMPSON), 30 );
				bot->client->sess.playerWeapon = WP_THOMPSON;
			}
			else
			{
				Weapon_BitSet( &client->ps, WP_CARBINE );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_CARBINE)] += 30;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_CARBINE), 30 );
				bot->client->sess.playerWeapon = WP_CARBINE;
			}
		}

		bot->client->ps.weapon = bot->client->sess.playerWeapon;
		bot->s.weapon = bot->client->sess.playerWeapon;
		bot->client->AIbaseweapon = bot->client->sess.playerWeapon;
	}
	else if ( pc == PC_MEDIC )
	{					// medic or engineer gets assigned MP40 or Thompson with one magazine ammo
		if ( client->sess.sessionTeam == TEAM_AXIS )
		{				// axis
			Weapon_BitSet( &client->ps, WP_MP40 );

			//client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
			BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MP40), 32 );
			bot->client->sess.playerWeapon = WP_MP40;
		}
		else
		{				// allied
			Weapon_BitSet( &client->ps, WP_THOMPSON );

			//client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
			BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_THOMPSON), 30 );
			bot->client->sess.playerWeapon = WP_THOMPSON;
		}

		bot->client->ps.weapon = bot->client->sess.playerWeapon;
		bot->s.weapon = bot->client->sess.playerWeapon;
		bot->client->AIbaseweapon = bot->client->sess.playerWeapon;
	}
	else
	{					// Soldier Weapons.
		int choice = rand() % 4;
		switch ( choice )
		{
			case 1:		// WP_MAUSER, WP_SNIPERRIFLE
				Weapon_BitSet( &client->ps, WP_FLAMETHROWER );

				//client->ps.ammoclip[BG_FindClipForWeapon(WP_FLAMETHROWER)] = 100;
				//client->ps.ammo[BG_FindAmmoForWeapon(WP_FLAMETHROWER)] = 200;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_FLAMETHROWER), 200 );
				bot->client->sess.playerWeapon = WP_FLAMETHROWER;
				break;
			case 2:		// WP_PANZERFAUST
				Weapon_BitSet( &client->ps, WP_PANZERFAUST );

				//client->ps.ammo[BG_FindAmmoForWeapon(WP_PANZERFAUST)] = 4;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_PANZERFAUST), 200 );
				bot->client->sess.playerWeapon = WP_PANZERFAUST;
				break;
			case 3:		// WP_VENOM
				Weapon_BitSet( &client->ps, WP_MOBILE_MG42 );

				//client->ps.ammoclip[BG_FindAmmoForWeapon(WP_MOBILE_MG42)] = 500;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MOBILE_MG42), 500 );
				bot->client->sess.playerWeapon = WP_MOBILE_MG42;
				break;

			/*			case 4:		// WP_SHOTGUN
				Weapon_BitSet( &client->ps,  WP_SHOTGUN );
				//client->ps.ammoclip[BG_FindAmmoForWeapon(WP_SHOTGUN)] = 2;
				AddWeaponToPlayer( client, WP_SHOTGUN, 16, 2, qtrue );
				bot->client->sess.playerWeapon = WP_SHOTGUN;
				break;*/
			default:	// give MP40 if given invalid weapon number
				Weapon_BitSet( &client->ps, WP_MOBILE_MG42 );

				//client->ps.ammoclip[BG_FindAmmoForWeapon(WP_MOBILE_MG42)] = 500;
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_MOBILE_MG42), 500 );
				bot->client->sess.playerWeapon = WP_MOBILE_MG42;

				/*
				if (client->sess.sessionTeam == TEAM_AXIS) { // JPW NERVE
					Weapon_BitSet( &client->ps,  WP_MP40 );
					client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
					if (pc == PC_SOLDIER)
						client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += 64;
					else
						client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += 32;
					bot->client->sess.playerWeapon = WP_MP40;
				}
				else { // TEAM_ALLIES
					Weapon_BitSet( &client->ps,  WP_THOMPSON );
					client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
					if (pc == PC_SOLDIER)
						client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += 60;
					else
						client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += 30;
					bot->client->sess.playerWeapon = WP_THOMPSON;
				}*/
				break;
		}

		bot->client->ps.weapon = bot->client->sess.playerWeapon;
		bot->s.weapon = bot->client->sess.playerWeapon;
		bot->client->AIbaseweapon = bot->client->sess.playerWeapon;
	}

	client->sess.latchPlayerWeapon = bot->client->ps.weapon;
	bot->weaponchoice = bot->client->ps.weapon;
	ClientUserinfoChanged( bot->client->ps.clientNum );
}

#else
#ifdef __VEHICLES__
qboolean	TanksFound = qfalse;
qboolean	TanksChecked = qfalse;


/* */
qboolean
TanksOnMap ( void )
{
	int i;
	if ( TanksChecked )
	{
		return ( TanksFound );
	}

	TanksChecked = qtrue;
	for ( i = MAX_CLIENTS; i < MAX_GENTITIES; i++ )
	{
		gentity_t	*ent = &g_entities[i];
		if ( !ent )
		{
			continue;
		}

		if ( ent->s.eType == ET_VEHICLE )
		{
			TanksFound = qtrue;
			return ( qtrue );
		}
	}

	return ( qfalse );
}
#endif //__VEHICLES__

/* */
void
SetBotWeapons ( gentity_t *bot )
{
	// The new way to spawn bot weapons.. Look up ammo table instead..
	// Should allow us to add more weapons easier! Maybe we should use this for normal clients too???
	int					pc = PC_SOLDIER;
	gclient_t			*client = bot->client;
	int					loop = 0;
	int					primaryCount = 0;

	//	int					nadesCount		= 0;
	bg_playerclass_t	*classInfo = NULL;

	// Let's make sure the bots actually have a class first...
	if ( bot->client->ps.stats[STAT_PLAYER_CLASS] )
	{
		// STAT_PLAYER_CLASS has the value we want.. Use it...
		pc = bot->client->ps.stats[STAT_PLAYER_CLASS];
	}
	else
	{

		// STAT_PLAYER_CLASS not set.. We need to find another option...
		if ( bot->botClassnum )
		{

			// botClassnum was added by me as a backup... Use it if we have one...
			pc = bot->botClassnum;
		}
		else
		{

			// No class found anywhere.. Chose one...
			pc = SOD_SPAWN_ChooseClass( bot->client->sess.sessionTeam );
		}
	}

	classInfo = BG_GetPlayerClassInfo( client->sess.sessionTeam, pc );
	client->ps.stats[STAT_PLAYER_CLASS] = pc;
	client->sess.latchPlayerType = pc;
	client->sess.playerType = pc;

	// And now find a main weapon we can use...
	while ( classInfo->classWeapons[loop] != WP_NONE && loop < MAX_WEAPS_PER_CLASS )
	{
		primaryCount++;
		loop++;
	}

	loop = 0;

	// And now find a nade type we can use...

	/*	while (classInfo->classWeapons[4][loop] != WP_NONE && loop < MAX_WEAPS_PER_CLASS)
	{
		nadesCount++;
		loop++;
	}*/

	// Now select a weapon from the allowed list randomly...
	if ( sp_bot_weapon[bot->bot_sp_indentifier] > 0)
	{// Use a single player map specified weapon...
		client->sess.latchPlayerWeapon = client->sess.playerWeapon = sp_bot_weapon[bot->bot_sp_indentifier];
	}
	else if ( primaryCount > 0 )
	{
		client->sess.latchPlayerWeapon = client->sess.playerWeapon = classInfo->classWeapons[Q_TrueRand( 0, primaryCount - 1 )];
	}

	if ( sp_bot_weapon[bot->bot_sp_indentifier] > 0)
	{

	}
	else if ( pc == PC_SOLDIER 
#ifdef __VEHICLES__
		&& TanksOnMap() 
#endif //__VEHICLES__
		)
	{				// More panzers to kill the tanks!
		if ( rand() < 0.5 )
		{
			client->sess.playerWeapon = WP_PANZERFAUST;
		}
		else
		{
			if ( primaryCount > 0 )
			{
				while ( client->sess.playerWeapon == WP_MORTAR )
				{	// Bots can't use mortar.. Pick another!
					client->sess.latchPlayerWeapon = client->sess.playerWeapon = classInfo->classWeapons[Q_TrueRand( 0, primaryCount - 1 )];
				}
			}
		}
	}
	else
	{
		if ( primaryCount > 0 )
		{
			while ( client->sess.playerWeapon == WP_MORTAR )
			{		// Bots can't use mortar.. Pick another!
				client->sess.latchPlayerWeapon = client->sess.playerWeapon = classInfo->classWeapons[Q_TrueRand( 0, primaryCount - 1 )];
			}
		}
	}

	// Testing Mobile MG42...

	/*	pc = PC_SOLDIER;
	client->ps.stats[STAT_PLAYER_CLASS] = pc;
	client->sess.latchPlayerType = pc;
	client->sess.playerType = pc;
	client->sess.playerWeapon = WP_MOBILE_MG42;*/

	// Now select a nade from the allowed list randomly...

	/*	if (nadesCount > 0)
		client->sess.playerGrenade = classInfo->classWeapons[4][Q_TrueRand(0, nadesCount - 1)];*/
	client->sess.latchPlayerWeapon = bot->client->AIbaseweapon = bot->weaponchoice = client->sess.playerWeapon;

	//	client->sess.latchPlayerGrenade = client->sess.playerGrenade;
	// Everyone except Medic and LT get some grenades
	if ( (pc != PC_FIELDOPS) && (pc != PC_MEDIC) )
	{				// JPW NERVE
		switch ( client->sess.sessionTeam )
		{			// was playerItem
			case TEAM_ALLIES:
				COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE), 4 + 4 * (pc == PC_SOLDIER) );
				break;
			case TEAM_AXIS:
				COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_LAUNCHER), 4 + 4 * (pc == PC_SOLDIER) );
				break;
			default:
				COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE), 4 + 4 * (pc == PC_SOLDIER) );
				break;
		}
	}
	else
	{
		switch ( client->sess.sessionTeam )
		{			// was playerItem
			case TEAM_ALLIES:
				COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE), 1 );
				break;
			case TEAM_AXIS:
				COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_LAUNCHER), 1 );
				break;
			default:
				COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
				BG_AddAmmo( &client->ps, BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE), 1 );
				break;
		}
	}

	/*	if (client->sess.sessionTeam == TEAM_AXIS)
		client->sess.latchPlayerGrenade = WP_GRENADE_LAUNCHER;
	else
		client->sess.latchPlayerGrenade = WP_GRENADE_PINEAPPLE;*/

	// jaquboss, safety check this is because you change session info but do not send it, so you have soldier like looking cvops
	ClientUserinfoChanged( bot->client->ps.clientNum );
}
#endif

/**
 * Function to read in a bot profile from file, and return the bot struct.
 * @param profile Name of the profile for this bot.
 * @return An initialized bot struct, either by the profile, either by default.
 */
sodbot_t
SOD_SPAWN_ReadProfile ( char *profile )
{
	sodbot_t	botstate;

	// We should read the profile from file here
	// but for now let's just return a new, empty botstate
	botstate.skill = 0;
	botstate.name = "^7Unique^5Bot^3%i", rand() % 200;

	//botstate.name = PickName( g_entities[botstate.clientNum].client->sess.sessionTeam );
	//Q_strncpyz(name, va("%s", PickName()), sizeof(name));
	// Initialize botstate
	botstate.next_node = INVALID;
	botstate.current_node = INVALID;
	botstate.goal_node = INVALID;
	botstate.previous_node = INVALID;
	botstate.goal_entity = NULL;
	return ( botstate );
}


/**
 *
 */
void
SOD_SPAWN_BotList ( void )
{
	gentity_t	*e;
	int			i;
	e = &g_entities[0];
	G_Printf( " Nr : Name\n" );
	G_Printf( "------------------------\n" );
	for ( i = 0; i < MAX_CLIENTS; i++, e++ )
	{
		if ( e->is_bot )
		{
			G_Printf( " %i : %s\n", e->client->ps.clientNum, e->client->pers.netname );
		}
	}
}


/**
 *
 */
void
SOD_SPAWN_RemoveBot ( int clientNum )
{
	gentity_t	*bot;
	bot = &g_entities[clientNum];

	//	number_of_bots = Count_Bot_Players();

	/*	// Remove the bot pointer
	for (i = 0; i < number_of_bots; i++) {
		if ( sbots[i]->inuse && sbots[i]->s.clientNum == clientNum ) {
			sbots[i]				= sbots[--number_of_bots];		// Move the last one
			sbots[number_of_bots]	= NULL;
			break;
		}		
	}

	// Now free the botstate slot
	for (i = 0; i < MAX_BOTS; i++) {
		if ( sodbotstates[i].clientNum == clientNum ) {
			sodbotstates[i].clientNum = INVALID;
			break;
		}
	}*/
	trap_DropClient( clientNum, "was removed by the admin.", 0 );
	trap_BotFreeClient( clientNum );
	bot->bs = NULL;
	bot->inuse = qfalse;
	bot->is_bot = qfalse;

	//bot->think		= G_FreeEntity;
	bot->think = NULL;

	//bot->nextthink	= level.time + 1;
	bot->nextthink = level.time - 1;
}


/**
 *
 */
void
SOD_SPAWN_InitSodBot ( void )
{
	int i;
	number_of_bots = 0;
	memset( &sodbotstates, 0, sizeof(sodbotstates) );
	for ( i = 0; i < MAX_BOTS; i++ )
	{
		sbots[i] = NULL;
	}

	// Do some entity important stuff, just to be on the safe side
	for ( i = 0; i < 1024; i++ )
	{
		g_entities[i].bs = NULL;
		g_entities[i].is_bot = qfalse;
	}
}

/*
 * level.sodInfo.numWantedLieutenants ...
 */
extern vmCvar_t bot_minplayers;
extern int		Count_Eng_Bot_Players ( void );		// g_main.c
extern int		Count_Medic_Bot_Players ( void );	// g_main.c
extern int		Count_FldOps_Bot_Players ( void );	// g_main.c

/* */
int
SOD_SPAWN_ChooseClass_Standard ( int team )
{
	int choice = Q_TrueRand( 0, 100 );
	// See what classes we got on our team
	int num_eng = Count_Playerclass( PC_ENGINEER, team );
	int num_med = Count_Playerclass( PC_MEDIC, team );
	int num_fld = Count_Playerclass( PC_FIELDOPS, team );
	int num_cov = Count_Playerclass( PC_COVERTOPS, team );
	int ratio = TeamCount( -1, team ) / 5;
	int selclass;

	// Make sure that we got least one of each
	// If that's ok, divide it into 15% engies, 20% medics, 25% soldiers, 
	//	20% field ops and 20% covert ops
	if ( num_eng == 0 || (num_eng <= ratio && choice < 15 ) )
	{
		// We need an engineer! (Maybe check if dyno's are needed?)
		selclass = ( PC_ENGINEER ); 
	}
	else if ( num_med == 0 || (num_med <= ratio && choice - 15 < 20 ) )
	{
		// Medic!
		selclass = ( PC_MEDIC );
	}
	else if ( num_fld == 0 || (num_fld <= ratio && choice - 35 < 20 ) )
	{
		// Ammo please ;)
		selclass = ( PC_FIELDOPS );
	} else if ( num_cov == 0 || (num_cov <= ratio && choice - 55 < 20) )
	{
		// Add some sneaky ones
		selclass = ( PC_COVERTOPS );
	}
	else
	{
		// More soldiers over here!
		selclass = ( PC_SOLDIER );
	}
	if( bot_debug.integer >= BOT_DEBUG_REGULAR ) {
		G_Printf("Ratio = %i; Choice = %i; Engies: %i; Meds: %i; Fd ops: %i; Cov ops: %i\n",
			ratio, choice, num_eng, num_med, num_fld, num_cov );
		G_Printf("Selected class %i\n", selclass);
	}
	return selclass;
}


/* */
int
SOD_SPAWN_ChooseClass_Commandos ( void )
{								// No engineers needed for bots in these gametypes...
	//	int choice = rand()%3;
	// Kamikazee: code defunct, just add soldiers then...
/*	int num_med = Count_Bot_Players( PC_MEDIC );
	int num_fld = Count_Bot_Players( PC_FIELDOPS );
	if ( num_med < bot_minplayers.integer / 3 )
	{
		return ( PC_MEDIC );	// Plenty of medics!
	}
	else if ( num_fld < bot_minplayers.integer / 4 )
	{
		return ( PC_FIELDOPS ); // And some ammo givers ;)
	}
*/
	/*	if (choice == 1)
		return PC_COVERTOPS;		
	else*/

	// Twice as many soldiers!
	return ( PC_SOLDIER );
}


/* */
int
SOD_SPAWN_ChooseClassHelpers ( void )
{
	int choice = rand() % 3;
	if ( choice == 1 )
	{
		return ( PC_FIELDOPS );
	}
	else if ( choice == 2 )
	{
		return ( PC_SOLDIER );
	}
	else
	{
		return ( PC_MEDIC );
	}
}


/* */
int
SOD_SPAWN_ChooseClass ( int team )
{	
	// The main procedure you should call.
	//	if (g_gametype.integer < GT_WOLF)
	//		return SOD_SPAWN_ChooseClass_Commandos();
	//	else
	return ( SOD_SPAWN_ChooseClass_Standard( team ) );
}



/*
 * On map end save our bots in a temp file (.tmp).
 * TODO: File must be removed on server quit
 */
void
SOD_SPAWN_SaveBotsOLD ( void )
{
	gentity_t		*sodbot;
	int				i, count = 0;
	fileHandle_t	f;
	int				len;
	int				clientNum;
	char			userinfo[MAX_INFO_STRING];
	userinfo[0] = '\0';
	len = trap_FS_FOpenFile( "bots.tmp", &f, FS_WRITE );

	// Get number of bots
	//-------------------------------------------------------
	for ( i = level.maxclients; i > 0; i-- )
	{
		sodbot = g_entities + i;
		if ( sodbot->inuse && sodbot->is_bot )
		{
			count++;
		}
	}

	//-------------------------------------------------------
	trap_FS_Write( &count, sizeof(count), f );
	for ( i = level.maxclients; i > 0; i-- )
	{
		sodbot = g_entities + i;
		if ( sodbot->inuse && (sodbot->is_bot || sodbot->r.svFlags & SVF_BOT) )
		{
			clientNum = i;
			trap_FS_Write( &clientNum, sizeof(int), f );
			trap_GetUserinfo( clientNum, userinfo, sizeof(userinfo) );
			trap_FS_Write( userinfo, sizeof(char) * MAX_INFO_STRING, f );
		}
	}

	trap_FS_FCloseFile( f );

	/*gentity_t   *sodbot;
    int         i;

    for (i = level.maxclients; i > 0; i--)
    {
        sodbot = g_entities + i;

        if (sodbot->inuse && sodbot->is_bot)
            trap_DropClient( i, " (bot) was removed between maps.", 0 );
    }*/
}

/*
 * On map start, load our saved bots.
 */
extern void SOD_AI_BotThink ( gentity_t *bot );
void		AIMOD_SPAWN_PutClientInServer ( gentity_t *uniquebot, qboolean respawn, int team );


/* */
void
SOD_SPAWN_LoadBotsOLD ( void )
{

	//	return;
	char			userinfo[MAX_INFO_STRING];
	fileHandle_t	f;
	int				i, count, len;
	len = trap_FS_FOpenFile( "bots.tmp", &f, FS_READ );
	if ( len < 0 )
	{
		return;
	}

	trap_FS_Read( &count, sizeof(int), f );
	for ( i = 0; i < count; i++ )
	{
		int clientNum = -1;
		trap_FS_Read( &clientNum, sizeof(int), f );
		trap_FS_Read( &userinfo, sizeof(char) * MAX_INFO_STRING, f );
		if ( !SOD_SPAWN_AddBotShort( Info_ValueForKey( userinfo, "team"), NULL, 0, userinfo) )
		{
			G_Printf( S_COLOR_RED "SOD_SPAWN_LoadBots() failed\n" );
		}
		else
		{
			AIMOD_SPAWN_PutClientInServer( &g_entities[clientNum], qfalse,
										   g_entities[clientNum].client->sess.sessionTeam );

			//trap_DropClient( clientNum, " (bot) was removed between maps.", 0 );
			//G_Printf("Setting think for client %i.\n", clientNum);
			//g_entities[clientNum].think = SOD_AI_BotThink;
		}
	}

	trap_FS_FCloseFile( f );

	// empty file
	// so only after a map reload bots will be added

	/*	len = trap_FS_FOpenFile( "bots.tmp", &f, FS_WRITE );

	count = 0;
	trap_FS_Write(&count, sizeof(int), f);

	trap_FS_FCloseFile( f );*/

	//trap_FS_Delete("bots.tmp");
	// No fucking delete.... For fuck sake splash damage, theres on in the fucking UI code....
	trap_FS_Rename( "bots.tmp", "nofkindeletefunc.tmp" );
}


/* */
void
SOD_SPAWN_SaveBots ( void )
{
	gentity_t	*sodbot;
	int			i;
	for ( i = level.maxclients; i > 0; i-- )
	{
		sodbot = g_entities + i;

		if (g_gametype.integer <= GT_COOP && sodbot->inuse && sodbot->is_bot)
		    trap_DropClient( i, " (bot) was removed between maps.", 0 );
		else if ( sodbot && sodbot->inuse && sodbot->r.svFlags & SVF_BOT )
		{
			sodbot->client->ps.persistant[PERS_BOT] = qtrue;
		}
	}
}


/*
 * On map start, load our saved bots.
 */
void
SOD_SPAWN_LoadBots ( void )
{
	gentity_t	*sodbot;
	int			i;
	for ( i = level.maxclients; i > 0; i-- )
	{
		sodbot = g_entities + i;
		if ( sodbot && sodbot->client && sodbot->client->ps.persistant[PERS_BOT] )
		{
			sodbot->r.svFlags |= SVF_BOT;
			sodbot->is_bot = qtrue;
			sodbot->nextthink = level.time + 50;
			sodbot->pain = AIMOD_AI_BotPain;
			sodbot->die = player_die;
			sodbot->think = SOD_AI_BotThink;	// I can think, I'm alive !
			sodbot->nextthink = level.time + FRAMETIME + rand() % 100;
			sodbot->client->ps.eFlags |= EF_BOT;
		}
	}
}


//===========================================================================
// Routine      : AIMOD_SPAWN_PutClientInServer

// Description  : Just a modified version of id's code
void
AIMOD_SPAWN_PutClientInServer ( gentity_t *uniquebot, qboolean respawn, int team )
{
	vec3_t				mins = { -16, -16, -24 };
	vec3_t				maxs = { 16, 16, 32 };
	int					index;
	vec3_t				spawn_origin, spawn_angles;
	gclient_t			*client;
	int /*i, */ counter;
	clientPersistant_t	saved;

	//client_respawn_t    resp; // fretn
	// char                *s;
	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	//-------------------------------------------------------
	//SelectSpawnPoint (uniquebot, spawn_origin, spawn_angles);
	index = uniquebot - g_entities - 1;
	client = uniquebot->client;

	//memset (&resp, 0, sizeof(resp)); //fretn
	//-------------------------------------------------------
	// clear everything but the persistant data
	//-------------------------------------------------------
	saved = client->pers;
	memset( client, 0, sizeof(*client) );
	client->pers = saved;
	client->ps.stats[STAT_PLAYER_CLASS] = PC_SOLDIER;	// for the ammo !
	client->sess.latchPlayerType = PC_SOLDIER;
	client->sess.playerType = PC_SOLDIER;

	//client->resp = resp; // fretn
	//-------------------------------------------------------
	// copy some data from the client to the entity
	//-------------------------------------------------------
	//fretnFetchClientEntData (uniquebot);
	// clear entity values
	//-------------------------------------------------------
	//fretnuniquebot->groundentity = NULL;
	//fretnuniquebot->client = &game.clients[index];
	uniquebot->takedamage = qtrue;						// AVD 50 ???
	uniquebot->lastMedicVoiceChat = level.time;			//fretn

	//uniquebot->movetype = MOVETYPE_WALK;
	//uniquebot->viewheight = 24;
	uniquebot->classname = "bot";

	//uniquebot->mass = 200;
	uniquebot->solid = SOLID_BBOX;

	//uniquebot->deadflag = DEAD_NO;
	//uniquebot->air_finished = level.time + 12;
	uniquebot->r.contents = CONTENTS_SOLID;
	uniquebot->clipmask = MASK_PLAYERSOLID;

	//uniquebot->model = "players/male/tris.md2";
	uniquebot->pain = AIMOD_AI_BotPain;
	uniquebot->die = player_die;
	uniquebot->waterlevel = 0;
	uniquebot->watertype = 0;
	uniquebot->flags &= ~FL_NO_KNOCKBACK;

	//uniquebot->svFlags &= ~SVF_CORPSE;
	uniquebot->is_jumping = qfalse;

	//-------------------------------------------------------
	VectorCopy( mins, uniquebot->client->ps.mins );
	VectorCopy( maxs, uniquebot->client->ps.maxs );
	VectorClear( uniquebot->client->ps.velocity );

	// clear playerstate values
	//-------------------------------------------------------
	memset( &uniquebot->client->ps, 0, sizeof(client->ps) );
	VectorClear( spawn_origin );
	client->ps.origin[0] = spawn_origin[0] * 8;
	client->ps.origin[1] = spawn_origin[1] * 8;
	client->ps.origin[2] = spawn_origin[2] * 8;
	VectorCopy( spawn_origin, uniquebot->s.origin );

	// Make sure off ground
	uniquebot->s.origin[2] += 1;

	//-------------------------------------------------------
	// set the delta angle
	//-------------------------------------------------------
	// for (i=0 ; i<3 ; i++)
	//      client->ps.delta_angles[i] = ANGLE2SHORT(spawn_angles[i] - client->resp.cmd_angles[i]);
	VectorClear( spawn_angles );
	uniquebot->s.angles[PITCH] = 0;
	uniquebot->s.angles[YAW] = spawn_angles[YAW];
	uniquebot->s.angles[ROLL] = 0;
	VectorCopy( uniquebot->s.angles, client->ps.viewangles );

	//VectorCopy (uniquebot->s.angles, client->v_angle);
	//-------------------------------------------------------
	uniquebot->enemy = NULL;
	uniquebot->state = BOT_STATE_POSITION;

	// Set the current node
	//-------------------------------------------------------

	/*uniquebot->current_node = AIMOD_NAVIGATION_FindClosestReachableNode(uniquebot,NODE_DENSITY, NODEFIND_ALL);
    uniquebot->goal_node = uniquebot->current_node;
    uniquebot->next_node = uniquebot->current_node;
    uniquebot->next_move_time = level.time;     
    uniquebot->suicide_timeout = level.time + 15.0;*/

	//-------------------------------------------------------
	// If we are not respawning hold off for up to three seconds before releasing into game
	//-------------------------------------------------------
	if ( !respawn )
	{

		/*uniquebot->think = AIMOD_SPAWN_HoldSpawn;
        uniquebot->nextthink = level.time + 0.1;
        // Up to 3 seconds
        uniquebot->nextthink = level.time + random()*3.0;*/
	}
	else
	{
		if ( uniquebot->weaponchoice == 0 )
		{
			counter = rand() % 5;
		}
		else
		{
			counter = uniquebot->weaponchoice - 1;
		}
	}

	ClientUserinfoChanged( uniquebot->s.number );
}


/* */
void
AIMOD_SPAWN_SpawnBot ( char *team, char *name, int skill )
{
	gentity_t	*uniquebot;
	int			clientNum;
	char		userinfo[MAX_INFO_STRING];
	userinfo[0] = '\0';

	//clientNum = AIMOD_SPAWN_FindFreeClient();
	clientNum = trap_BotAllocateClient( 0 );			//	 <= dont use that crap
	if ( clientNum == -1 )
	{
		G_Printf( "^1*** ^3%s^4: ^5 Server is full, increase Maxclients.\n", GAME_VERSION );
		return;
	}

	// We found an empty client slot, go create a nice lil' backward running bot
	uniquebot = &g_entities[clientNum];
	if ( !uniquebot )
	{
		G_Printf( "^1*** ^3%s^4: ^5 Problem creating the bot, me is outta here !\n", GAME_VERSION );
		return;
	}

	Info_SetValueForKey( userinfo, "name", name );		// This be taking care of the bot's name
	Info_SetValueForKey( userinfo, "ip", "127.0.0.1" ); // Bots behave like local clients, they are in fact !
	Info_SetValueForKey( userinfo, "team", team );
	Info_SetValueForKey( userinfo, "rate", "25000" );
	Info_SetValueForKey( userinfo, "snaps", "20" );

	/*Info_SetValueForKey( userinfo, "model", "multi" );
	Info_SetValueForKey( userinfo, "head", "default" );*/
	trap_SetUserinfo( clientNum, userinfo );
	uniquebot->inuse = qtrue;
	uniquebot->is_bot = qtrue;
	uniquebot->scriptStatus.scriptEventIndex = -1;
	uniquebot->s.number = uniquebot - g_entities;
	uniquebot->r.ownerNum = ENTITYNUM_NONE;
	uniquebot->headshotDamageScale = 1.0;
	uniquebot->skillchoice = skill;
	uniquebot->r.svFlags |= SVF_BOT;
	uniquebot->client->pers.localClient = qtrue;

	// Now 'connect' the bot
	ClientConnect( clientNum, qtrue, qtrue /*qfalse*/ );

	//	uniquebot->think							= AIMOD_AI_Think;
	uniquebot->think = SOD_AI_BotThink;					// I can think, I'm alive !
	uniquebot->nextthink = level.time + FRAMETIME + rand() % 100;
	if ( !team || !*team )
	{

		//if( g_gametype.integer == GT_TEAM || g_gametype.integer == GT_CTF) {
		if ( PickTeam( clientNum) == TEAM_AXIS )
		{
			team = "red";
		}
		else
		{
			team = "blue";
		}

		/*}
		else {
			team = "red";
		}*/
	}

	if ( (team != NULL) && (strcmp( team, "blue") == 0) )
	{
		AIMOD_SPAWN_PutClientInServer( uniquebot, qfalse, TEAM_ALLIES );
	}
	else
	{
		AIMOD_SPAWN_PutClientInServer( uniquebot, qfalse, TEAM_AXIS );
	}

	//strcpy( uniquebot->client->pers.netname, name );
	// First register the userinfo
	//trap_LinkEntity(uniquebot);
	// Not entirely sure about this one
	trap_SetUserinfo( clientNum, userinfo );

	// Set the nodes to invalid
	uniquebot->current_node = INVALID;
	uniquebot->next_node = INVALID;
	uniquebot->goal_node = INVALID;

	//uniquebot->goal_node = AIMOD_Calculate_Goal_From_Nodenum(uniquebot->current_node);
	uniquebot->last_node = 10000;						// AVD hacked (badly) for sprint time

	//SetTeam( uniquebot, team);
	SetTeam( uniquebot, team, qfalse, -1, -1, qfalse ); // Test Unique1

	//limbo(uniquebot, qfalse);
	AIMOD_ITEMS_uniquebotAdded( uniquebot );
	if ( random() < 0.33 )
	{
		G_Voice( uniquebot, NULL, SAY_ALL, "hi", qtrue );

		//G_Say(uniquebot, NULL, SAY_ALL, va("Hi!\n"));
		G_Say( uniquebot, NULL, SAY_ALL, va( "Hi!") );
	}

	//	G_Say(uniquebot, NULL, SAY_ALL, "Hi!");
	//uniquebot->suicide_timeout = level.time + 15.0;
}


/* */
void
AIMOD_SPAWN_BotList ( void )
{
	gentity_t	*e;
	int			i;
	e = &g_entities[0];
	G_Printf( "N°: name\n" );
	G_Printf( "---------\n" );
	for ( i = 0; i < MAX_CLIENTS; i++, e++ )
	{
		if ( e->is_bot )
		{
			G_Printf( "%d : %s\n", e->client->ps.clientNum, e->client->pers.netname );
		}
	}
}


/* */
void
AIMOD_SPAWN_RemoveBot ( int clientNum )
{
	gentity_t	*bot;
	bot = &g_entities[clientNum];

	//ClientDisconnect( clientNum );
	trap_DropClient( clientNum, "was removed by the admin.", 0 );
	trap_BotFreeClient( clientNum );
	bot->inuse = qfalse;
	bot->is_bot = qfalse;
	bot->think = G_FreeEntity;
	bot->nextthink = level.time + 1;
	bot->client->ps.persistant[PERS_BOT] = qfalse;

	//G_FreeEntity( &g_entities[clientNum] );
}

// not used

/*int AIMOD_SPAWN_FindFreeClient(void) {
	// Variables
	gentity_t	*e;
	int i;
	static int maxclients;

	e = &g_entities[0];

	// Get maxclients for this server
	if (!maxclients)
		maxclients = trap_Cvar_VariableIntegerValue("sv_maxclients");

	for ( i = 0 ; i < maxclients && i < MAX_CLIENTS ; i++, e++) {
		if ( e->inuse )
			continue;
		return i;
	}

	return -1;
}*/

// Kamikazee: reserve a place in the specified team for the specified human
// client if that team contains auto-bots and teams would get unbalanced after
// the switch.
void
AIMOD_ReservePlace( int team )
{
	char	teamname[MAX_TOKEN_CHARS];
	// Auto-bots enabled?
	if( bot_dynamic.integer > 0 )
	{
		int clientNum;

		// See if there is a bot in the team
		clientNum = Pick_Redundant_Bot( team );
		if( clientNum > -1 )
		{
			Q_strncpyz( teamname, TeamName( OtherTeam( team ) ), MAX_TOKEN_CHARS);
			SetTeam( &g_entities[clientNum], teamname, qtrue, -1, -1, qfalse);
		}
	}
	return;
}



//
// Single Player / Coop AI....
//

vec3_t current_bot_position;

#ifndef __NPC__

/**
 * Creates a 'bot' entity and fills in a client slot.
 * @param bs Botstate for this bot.
 * @param team Team where the bot should go into.
 * @param load_userinfo is used by the SOD_SPAWN_LoadBots function.
 *  it reads the userinfo string out of the file and gives it to us :)
 * @return qtrue if bot connect succeeded, qfalse if failed
 */

/* */
qboolean
SP_SPAWN_AddBot ( sodbot_t *bs, char *team, int identifier, char *load_userinfo )
{
	int			i, clientNum;
	gentity_t	*bot;
	char		userinfo[MAX_INFO_STRING];
	qboolean	firstTime;									// for loading after a mapchange bots;

	// See if we haven't reached max number of bots yet
	// ----------------------------------------------------------------
	number_of_bots = Count_Bot_Players( -1 );
	if ( number_of_bots >= MAX_BOTS )
	{
		G_Printf( S_COLOR_RED "Unable to add bot.  Maximum number of bots reached.\n" );
		return ( qfalse );
	}

	// Have the server allocate a client slot
	clientNum = trap_BotAllocateClient( 0 );

	if ( clientNum == -1 )
	{
		G_Printf( S_COLOR_RED "Unable to add bot.  All player slots are in use.\n" );
		G_Printf( S_COLOR_RED "Start server with more 'open' slots (or check setting of sv_maxclients cvar).\n" );
		return ( qfalse );
	}

	// ----------------------------------------------------------------
	// Create the bot's userinfo
	// ----------------------------------------------------------------
	if ( load_userinfo == NULL )
	{														// if /addbot console command is used
		userinfo[0] = '\0';
		Info_SetValueForKey( userinfo, "name", bs->name );
		Info_SetValueForKey( userinfo, "rate", "25000" );
		Info_SetValueForKey( userinfo, "snaps", "20" );
		Info_SetValueForKey( userinfo, "skill", va( "%i", bs->skill) );
		Info_SetValueForKey( userinfo, "team", team );
		Info_SetValueForKey( userinfo, "ip", "127.0.0.1" ); // Bots behave like local clients, they are in fact !
	}

	// ----------------------------------------------------------------
	// Now set the bot entity specifics
	// ----------------------------------------------------------------
	bot = &g_entities[clientNum];
	bot->r.svFlags |= SVF_BOT;
	bot->client->ps.eFlags |= EF_BOT;
	bot->inuse = qtrue;
	bot->is_bot = qtrue;
	bot->scriptStatus.scriptEventIndex = -1;
	bot->s.number = bot - g_entities;
	bot->r.ownerNum = ENTITYNUM_NONE;
	bot->headshotDamageScale = 1.0;
	bot->client->pers.localClient = qtrue;
	bot->client->ps.persistant[PERS_BOT] = qtrue;

	bot->bot_sp_indentifier = identifier;
	bot->botClassnum = sp_bot_type[identifier];
	bot->weaponchoice = sp_bot_weapon[identifier];

	// Set it's single player spawn position based on the map's entity.
	VectorCopy(current_bot_position, bot->bot_sp_spawnpos);

	// Set the team, or pick a team if none given
	if ( !team || !*team )
	{
		if ( g_gametype.integer == GT_COOP || g_gametype.integer == GT_SINGLE_PLAYER )
		{
			team = "red";
		}
		else if ( PickTeam( clientNum) == TEAM_AXIS )
		{
			team = "red";
		}
		else
		{
			team = "blue";
		}
	}

	if ( load_userinfo == NULL )
	{														// /addbot
		// Register the userinfo
		trap_SetUserinfo( clientNum, userinfo );
		firstTime = qtrue;
	}
	else
	{														// SOD_SPAWN_LoadBots
		trap_SetUserinfo( clientNum, load_userinfo );
		firstTime = qfalse;
	}

	// ----------------------------------------------------------------
	// Have it connect to the game as a normal client
	// ----------------------------------------------------------------
	// Dont set clientconnect isBot, it's a leftover from other stuff
	if ( ClientConnect( clientNum, firstTime, qtrue /*qfalse*/ ) )
	{
		return ( qfalse );									// Returns NULL if all went okay
	}

	ClientBegin( clientNum );								// Test
	sbots[number_of_bots++] = bot;

	// Okay, all went fine, set the final botstate
	for ( i = 0; i < MAX_BOTS; i++ )
	{
		if ( sodbotstates[i].clientNum == INVALID )
		{
			sodbotstates[i] = *bs;
			sodbotstates[i].clientNum = clientNum;
			bot->bs = &sodbotstates[i];
			break;
		}
	}

	// ----------------------------------------------------------------
	// Set the entity stuff like bounding box etc
	SOD_SPAWN_CreateBot( bot, team );

	bot->suicide_timeout = 0;								//level.time + 19500;
	bot->nextthink = level.time + FRAMETIME + rand() % 100;
	bot->client->AIbaseweapon = bot->client->ps.weapon;

#ifdef __BOT_AAS__
	if ( trap_AAS_Initialized() )
	{														// If aas is in use.. Set it up!
		BotAISetupClient( clientNum );
	}
#endif
	nextchat[bot->s.clientNum] = level.time + Q_TrueRand( 8000, 30000 );

	return ( qtrue );
}

qboolean
SP_SPAWN_AddBotShort ( char *team, char *name, int skill, int identifier, char *userinfo )
{
	sodbot_t	botstate;
	botstate = SOD_SPAWN_ReadProfile( NULL );
	botstate.name = name;
	botstate.skill = skill;

	// Add the bot to the server
	return ( SP_SPAWN_AddBot( &botstate, team, identifier, userinfo) );
}

qboolean	more_bots_queued = qfalse;
int			more_bots_position = 0;

void SP_Check_AddBots ( void )
{// Need to spawn a bot.
	int	 iskill;
	char team[MAX_TOKEN_CHARS] = "none";
	char name[MAX_TOKEN_CHARS];
	int	 i = 0;

	if (bots_added)
		return;

	if (more_bots_queued)
	{
		for ( i = more_bots_position; i < sp_bot_max && level.numConnectedClients < g_maxclients.integer - 8 /* leave 8 slots for players*/; i++ )
		{// Add all the single player/coop bots...
//			if (team == "red")
				Q_strncpyz(name, va("%s", PickName(TEAM_AXIS)), sizeof(name));
//			else
//				Q_strncpyz(name, va("%s", PickName(TEAM_ALLIES)), sizeof(name));

			if (sp_bot_skilllevel[i] > 0)
				iskill = sp_bot_skilllevel[i];
			else if (bot_skill.integer >= 1)
				iskill = bot_skill.integer;
			else
				iskill = rand()%5;

			VectorCopy(sp_bot_positions[i], current_bot_position);

			if (SP_SPAWN_AddBotShort(team, name, iskill, i, NULL))
				G_Printf("^1*** ^3AIMod: ^5Spawned bot ^7%s^5 (ident %i) with skill level ^7%d^5 at %f %f %f.\n", name, i, iskill, sp_bot_positions[i][0], sp_bot_positions[i][1], sp_bot_positions[i][2]);
		}

		if (i >= g_maxclients.integer - 8)
		{// Too many NPC's on this map, need to spawn more later!
			more_bots_queued = qtrue;
			more_bots_position = i;
			return;
		}
		else
		{
			more_bots_queued = qfalse;
			bots_added = qtrue;
			return;
		}
	}

	if ( level.numConnectedClients < g_maxclients.integer - 8 )
	{
		for ( i = 0; i < sp_bot_max && level.numConnectedClients < g_maxclients.integer - 8 /* leave 8 slots for players*/; i++ )
		{// Add all the single player/coop bots...
//			if (team == "red")
				Q_strncpyz(name, va("%s", PickName(TEAM_AXIS)), sizeof(name));
//			else
//				Q_strncpyz(name, va("%s", PickName(TEAM_ALLIES)), sizeof(name));

			if (sp_bot_skilllevel[i] > 0)
				iskill = sp_bot_skilllevel[i];
			else if (bot_skill.integer >= 1)
				iskill = bot_skill.integer;
			else
				iskill = rand()%5;

			VectorCopy(sp_bot_positions[i], current_bot_position);

			if (SP_SPAWN_AddBotShort(team, name, iskill, i, NULL))
				G_Printf("^1*** ^3AIMod: ^5Spawned bot ^7%s^5 (ident %i) with skill level ^7%d^5 at %f %f %f.\n", name, i, iskill, sp_bot_positions[i][0], sp_bot_positions[i][1], sp_bot_positions[i][2]);
		}

		if (i >= g_maxclients.integer - 8)
		{// Too many NPC's on this map, need to spawn more later!
			more_bots_queued = qtrue;
			more_bots_position = i;
			return;
		}
		else
		{
			more_bots_queued = qfalse;
			bots_added = qtrue;
			return;
		}
	}
}

/*
#define PC_SOLDIER				0	//	shoot stuff
#define PC_MEDIC				1	//	heal stuff
#define PC_ENGINEER				2	//	build stuff
#define PC_FIELDOPS				3	//	bomb stuff
#define PC_COVERTOPS			4	//	sneak about ;o

  typedef enum {
	WP_NONE,				// 0
	WP_KNIFE,				// 1
	WP_LUGER,				// 2
	WP_MP40,				// 3
	WP_GRENADE_LAUNCHER,	// 4
	WP_PANZERFAUST,			// 5
	WP_FLAMETHROWER,		// 6

	WP_COLT,				// 7	// equivalent american weapon to german luger
	WP_THOMPSON,			// 8	// equivalent american weapon to german mp40
	WP_GRENADE_PINEAPPLE,	// 9
	WP_STEN,				// 10	// silenced sten sub-machinegun
	WP_MEDIC_SYRINGE,		// 11	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
	WP_AMMO,				// 12	// JPW NERVE likewise
	WP_ARTY,				// 13

	WP_SILENCER,			// 14	// used to be sp5
	WP_DYNAMITE,			// 15
	WP_SMOKETRAIL,			// 16
	WP_MAPMORTAR,			// 17	
	VERYBIGEXPLOSION,		// 18	// explosion effect for airplanes
	WP_MEDKIT,				// 19
	WP_BINOCULARS,			// 20

	WP_PLIERS,				// 21
	WP_SMOKE_MARKER,		// 22	// Arnout: changed name to cause less confusion
	WP_KAR98,				// 23	// WolfXP weapons
	WP_CARBINE,				// 24
	WP_GARAND,				// 25
	WP_LANDMINE,			// 26
	WP_SATCHEL,				// 27
	WP_SATCHEL_DET,			// 28
	WP_TRIPMINE,			// 29
	WP_SMOKE_BOMB,			// 30

	WP_MOBILE_MG42,			// 31
	WP_K43,					// 32
	WP_FG42,				// 33
	WP_DUMMY_MG42,          // 34 // Gordon: for storing heat on mounted mg42s...
	WP_MORTAR,				// 35
	WP_LOCKPICK,			// 36	// Mad Doc - TDF lockpick
	WP_AKIMBO_COLT,			// 37
	WP_AKIMBO_LUGER,		// 38

// Gordon: ONLY secondaries below this mark, as they are checked >= WP_GPG40 && < WP_NUM_WEAPONS

	WP_GPG40,				// 39

	WP_M7,					// 40
	WP_SILENCED_COLT,		// 41
	WP_GARAND_SCOPE,		// 42
	WP_K43_SCOPE,			// 43
	WP_FG42SCOPE,			// 44
	WP_MORTAR_SET,			// 45
	WP_MEDIC_ADRENALINE,	// 46
	WP_AKIMBO_SILENCEDCOLT,	// 47
	WP_AKIMBO_SILENCEDLUGER,// 48
	WP_MOBILE_MG42_SET,		// 49

	WP_NUM_WEAPONS			// WolfMP: 32 WolfXP: 50
							// NOTE: this cannot be larger than 64 for AI/player weapons!
} weapon_t;
*/

//
// Single Player NPC's - Emulated RTCW ones... Sorta crappy emulation atm!
//

void SP_AddBot ( gentity_t *ent )
{// Need to spawn a bot.
//	cast_state_t *cs;

//	AIChar_spawn( ent );
//	cs = AICast_GetCastState( ent->s.number );

//	G_Printf("cast weapon: %i.\n", cs->weaponNum);

	VectorCopy(ent->s.origin, sp_bot_positions[sp_bot_max]);
	if (strcmp(ent->classname, "ai_zombie") == 0)
	{
		sp_bot_type[sp_bot_max] = PC_SOLDIER;
		sp_bot_weapon[sp_bot_max] = WP_MP40;
	}
	else if (strcmp(ent->classname, "ai_soldier") == 0)
	{
		sp_bot_type[sp_bot_max] = PC_SOLDIER;
		sp_bot_weapon[sp_bot_max] = WP_MP40;
	}
	else if (strcmp(ent->classname, "ai_venom") == 0)
	{
		sp_bot_type[sp_bot_max] = PC_SOLDIER;
		sp_bot_weapon[sp_bot_max] = WP_MOBILE_MG42;
	}
	else if (strcmp(ent->classname, "ai_blackguard") == 0)
	{
		sp_bot_type[sp_bot_max] = PC_COVERTOPS;
		sp_bot_weapon[sp_bot_max] = WP_STEN;
	}
	else if (strcmp(ent->classname, "ai_eliteguard") == 0)
	{
		sp_bot_type[sp_bot_max] = PC_COVERTOPS;
		sp_bot_weapon[sp_bot_max] = WP_K43;
	}
	else if (strcmp(ent->classname, "ai_boss_helga") == 0)
	{
		sp_bot_type[sp_bot_max] = PC_FIELDOPS;
		sp_bot_weapon[sp_bot_max] = WP_MP40;
	}
	else if (strcmp(ent->classname, "ai_protosoldier") == 0)
	{
		sp_bot_type[sp_bot_max] = PC_SOLDIER;
		sp_bot_weapon[sp_bot_max] = WP_FLAMETHROWER;
	}
	else if (strcmp(ent->classname, "ai_supersoldier") == 0)
	{
		sp_bot_type[sp_bot_max] = PC_SOLDIER;
		sp_bot_weapon[sp_bot_max] = WP_PANZERFAUST;
	}
	else
	{
		sp_bot_type[sp_bot_max] = PC_SOLDIER;
		sp_bot_weapon[sp_bot_max] = WP_MP40;
	}

	sp_bot_max++;
	G_FreeEntity(ent);
}

//
// Single Player NPC's - The Real Ones
//

static qboolean NPC_LoadData_ParseError( int handle, char *format, ... )
{
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start( argptr, format );
	Q_vsnprintf( string, sizeof(string), format, argptr );
	va_end( argptr );

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine( handle, filename, &line );

	if (filename[0])
		Com_Printf( S_COLOR_RED "NPC_LoadData: ERROR - %s, line %d: %s\n", filename, line, string );
	else
		Com_Printf( S_COLOR_RED "NPC_LoadData: ERROR - %s\n", string );

	trap_PC_FreeSource( handle );

	return qfalse;
}

qboolean NPC_String_Parse(int handle, char *out, size_t size) 
{
	pc_token_t token;

	if( !trap_PC_ReadToken(handle, &token) )
		return qfalse;

	Q_strncpyz( out, token.string, size );
    return qtrue;
}

#define PC_SOLDIER				0	//	shoot stuff
#define PC_MEDIC				1	//	heal stuff
#define PC_ENGINEER				2	//	build stuff
#define PC_FIELDOPS				3	//	bomb stuff
#define PC_COVERTOPS			4	//	sneak about ;o

#define NUM_PLAYER_CLASSES		5

// UQ1: .npc files need to match this!!!
stringID_table_t NPC_ClassTable[] =
{
	ENUM2STRING( PC_SOLDIER ),
	ENUM2STRING( PC_MEDIC ),
	ENUM2STRING( PC_ENGINEER ),
	ENUM2STRING( PC_FIELDOPS ),
	ENUM2STRING( PC_COVERTOPS ),
};

stringID_table_t NPC_WeaponTable[] =
{// This MUST sync with bg_public.h 's list...
	ENUM2STRING( WP_NONE ),					// 0
	ENUM2STRING( WP_KNIFE ),				// 1
	ENUM2STRING( WP_LUGER ),				// 2
	ENUM2STRING( WP_MP40 ),					// 3
	ENUM2STRING( WP_GRENADE_LAUNCHER ),		// 4
	ENUM2STRING( WP_PANZERFAUST ),			// 5
	ENUM2STRING( WP_FLAMETHROWER ),			// 6

	ENUM2STRING( WP_COLT ),					// 7	// equivalent american weapon to german luger
	ENUM2STRING( WP_THOMPSON ),				// 8	// equivalent american weapon to german mp40
	ENUM2STRING( WP_GRENADE_PINEAPPLE ),	// 9
	ENUM2STRING( WP_STEN ),					// 10	// silenced sten sub-machinegun
	ENUM2STRING( WP_MEDIC_SYRINGE ),		// 11	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
	ENUM2STRING( WP_AMMO ),					// 12	// JPW NERVE likewise
	ENUM2STRING( WP_ARTY ),					// 13

	ENUM2STRING( WP_SILENCER ),				// 14	// used to be sp5
	ENUM2STRING( WP_DYNAMITE ),				// 15
	ENUM2STRING( WP_SMOKETRAIL ),			// 16
	ENUM2STRING( WP_MAPMORTAR ),			// 17	
	ENUM2STRING( VERYBIGEXPLOSION ),		// 18	// explosion effect for airplanes
	ENUM2STRING( WP_MEDKIT ),				// 19
	ENUM2STRING( WP_BINOCULARS ),			// 20

	ENUM2STRING( WP_PLIERS ),				// 21
	ENUM2STRING( WP_SMOKE_MARKER ),			// 22	// Arnout: changed name to cause less confusion
	ENUM2STRING( WP_KAR98 ),				// 23	// WolfXP weapons
	ENUM2STRING( WP_CARBINE ),				// 24
	ENUM2STRING( WP_GARAND ),				// 25
	ENUM2STRING( WP_LANDMINE ),				// 26
	ENUM2STRING( WP_SATCHEL ),				// 27
	ENUM2STRING( WP_SATCHEL_DET ),			// 28
//	ENUM2STRING( WP_TRIPMINE ),				// 29
	ENUM2STRING( WP_SMOKE_BOMB ),			// 30

	ENUM2STRING( WP_MOBILE_MG42 ),			// 31
	ENUM2STRING( WP_K43 ),					// 32
	ENUM2STRING( WP_FG42 ),					// 33
	ENUM2STRING( WP_DUMMY_MG42 ),			// 34 // Gordon: for storing heat on mounted mg42s...
	ENUM2STRING( WP_MORTAR ),				// 35
//	ENUM2STRING( WP_LOCKPICK ),				// 36	// Mad Doc - TDF lockpick
	ENUM2STRING( WP_AKIMBO_COLT ),			// 37
	ENUM2STRING( WP_AKIMBO_LUGER ),			// 38

// Gordon: ONLY secondaries below this mark ), as they are checked >= ENUM2STRING( WP_GPG40 && < ENUM2STRING( WP_NUM_WEAPONS

	ENUM2STRING( WP_GPG40 ),				// 39

	ENUM2STRING( WP_M7 ),					// 40
	ENUM2STRING( WP_SILENCED_COLT ),		// 41
	ENUM2STRING( WP_GARAND_SCOPE ),			// 42
	ENUM2STRING( WP_K43_SCOPE ),			// 43
	ENUM2STRING( WP_FG42SCOPE ),			// 44
	ENUM2STRING( WP_MORTAR_SET ),			// 45
	ENUM2STRING( WP_MEDIC_ADRENALINE ),		// 46
	ENUM2STRING( WP_AKIMBO_SILENCEDCOLT ),	// 47
	ENUM2STRING( WP_AKIMBO_SILENCEDLUGER ),	// 48
	ENUM2STRING( WP_MOBILE_MG42_SET ),		// 49
	ENUM2STRING( WP_NUM_WEAPONS	)			// WolfMP: 32 WolfXP: 50
											// NOTE: this cannot be larger than 64 for AI/player weapons!
};

#define ACTION_NONE			0
#define ACTION_RUNANDHIDE	1
#define ACTION_GOTOENTITY	2

stringID_table_t NPC_ActionTable[] =
{
	ENUM2STRING( ACTION_NONE ),
	ENUM2STRING( ACTION_RUNANDHIDE ),
	ENUM2STRING( ACTION_GOTOENTITY ),
};

qboolean NPC_LoadData(gentity_t *NPC, const char* szFileName)
{
	pc_token_t	token;
	int			handle;
//	int			total	= 1;
	char		string[MAX_TOKEN_CHARS];

	handle = trap_PC_LoadSource( va("npcs/%s.npc", szFileName) );

	if( !handle )
		return qfalse;

	if( !trap_PC_ReadToken( handle, &token ) || Q_stricmp( token.string, "{" ) ) 
	{
		return NPC_LoadData_ParseError( handle, "expected '{'" );
	}

	while (1)
	{
		if( !trap_PC_ReadToken( handle, &token )) 
		{
			return NPC_LoadData_ParseError( handle, "unexpected eof" );
		}

		if( !Q_stricmp( token.string, "}" ) ) 
		{
			break;
		}
		else if( !Q_stricmp( token.string, "class" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected class type" );
			}
			else
			{
				NPC->NPC_PlayerType = (int)GetIDForString( NPC_ClassTable, string );
#ifdef _DEBUG
				G_Printf("^4*** ^3NPC DEBUG^5: LoadNPC (^3%s^5): ^7%s^5 (^7%i^5).\n", szFileName, string, NPC->NPC_PlayerType);
#endif //_DEBUG
			}
		}
		else if( !Q_stricmp( token.string, "skill" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected skill level" );
			}
			else
			{
				NPC->skillchoice = atoi(string);
				//G_Printf("^4*** ^3NPC DEBUG^5: Skill: ^7%i^5.\n", NPC->skillchoice);
			}
		}
		else if( !Q_stricmp( token.string, "weapon" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected weapon choice" );
			}
			else
			{
				NPC->weaponchoice = (int)GetIDForString( NPC_WeaponTable, string );
				//G_Printf("^4*** ^3NPC DEBUG^5: Weapon: ^7%s^5.\n", string);
			}
		}
		else if( !Q_stricmp( token.string, "patrol" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected patrol true/false" );
			}
			else
			{
				//G_Printf("^4*** ^3NPC DEBUG^5: Patrol: ^7%s^5.\n", string);

				if( !Q_stricmp( string, "true" ) )
					NPC->NPC_Patroller = qtrue;
				else
					NPC->NPC_Patroller = qfalse;
			}
		}
		else if( !Q_stricmp( token.string, "mounted" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected mounted true/false" );
			}
			else
			{
				//G_Printf("^4*** ^3NPC DEBUG^5: Mounted: ^7%s^5.\n", string);

				if( !Q_stricmp( string, "true" ) )
					NPC->NPC_Mounted = qtrue;
				else
					NPC->NPC_Mounted = qfalse;
			}
		}
		else if( !Q_stricmp( token.string, "hunter" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected hunter true/false" );
			}
			else
			{
				//G_Printf("^4*** ^3NPC DEBUG^5: Hunter: ^7%s^5.\n", string);

				if( !Q_stricmp( string, "true" ) )
					NPC->NPC_Hunter = qtrue;
				else
					NPC->NPC_Hunter = qfalse;
			}
		}
		else if( !Q_stricmp( token.string, "alertentity" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected alertentity <number>" );
			}
			else
			{
				NPC->target_ent = &g_entities[atoi(string)];
			}
		}
		else if( !Q_stricmp( token.string, "alertevent" ) ) 
		{
			if( !NPC_String_Parse( handle, string, MAX_TOKEN_CHARS)) 
			{
				return NPC_LoadData_ParseError( handle, "expected alertevent type" );
			}
			else
			{
				NPC->action = (int)GetIDForString( NPC_ActionTable, string );
			}
		}
	}

	trap_PC_FreeSource( handle );

	return qtrue;
}

void SP_AddNPC ( gentity_t *ent )
{// Need to spawn a bot (NPC)...
	VectorCopy(ent->s.origin, sp_bot_positions[sp_bot_max]);

	// Load the NPC script for NPC->scriptName
	if (!ent->scriptName)
		ent->scriptName = "default";

	if (!NPC_LoadData(ent, ent->scriptName))
	{
		G_Printf("Failed to load NPC script file npcs/%s.npc. Trying default NPC script.\n", ent->scriptName);

		ent->scriptName = "default";
		
		if (!NPC_LoadData(ent, ent->scriptName))
		{
			G_Printf("Failed to load NPC script file npcs/%s.npc!\n", ent->scriptName);
			G_FreeEntity(ent);
			return;
		}
	}

	sp_bot_type[sp_bot_max] = ent->NPC_PlayerType;
	sp_bot_weapon[sp_bot_max] = ent->weaponchoice;
	//sp_bot_weapon[sp_bot_max] = ent->NPC_Patroller;
	//sp_bot_weapon[sp_bot_max] = NPC->NPC_Mounted;
	sp_bot_skilllevel[sp_bot_max] = ent->skillchoice;

	sp_bot_max++;
	G_FreeEntity(ent);
}
#endif //__NPC__

#endif
