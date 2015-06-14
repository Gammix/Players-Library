/*
	Stats Include (playerstats.inc)
	* Gives online player's stats, very easy code but useful (atleast to me)

 	Author: (creator)
	* Gammix

	(c) Copyright 2015
  	* This file is provided as is (no warranties).
*/

/*
FUNCTIONS:
native GetPlayerKills(playerid);
native GetPlayerDeaths(playerid);
native GetPlayerSpree(playerid);
native GetPlayerConnectedTime(playerid, &hours, &minutes, &seconds);
native GetPlayerHeadshots(playerid);
native GetPlayerWeaponShots(playerid);
native GetPlayerWeaponHits(playerid);

CALLBACKS:
public OnPlayerHeadshot(playerid, issuerid, weaponid)
*/

enum PlayerEnum
{
	P_KILLS,
	P_DEATHS,
	P_SPREE,
	P_HEADSHOTS,
	P_SHOTS,
	P_HITS
}
static gPlayer[MAX_PLAYERS][PlayerEnum];

public OnPlayerSpawn(playerid)
{
	gPlayer[playerid][P_SPREE] = 0;

	#if defined STATS_OnPlayerSpawn
       	return STATS_OnPlayerSpawn(playerid);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerSpawn
    #undef OnPlayerSpawn
#else
    #define _ALS_OnPlayerSpawn
#endif
#define OnPlayerSpawn STATS_OnPlayerSpawn
#if defined STATS_OnPlayerSpawn
    forward STATS_OnPlayerSpawn(playerid);
#endif

public OnPlayerDeath(playerid, killerid, reason)
{
	gPlayer[playerid][P_DEATHS] += 1;
	gPlayer[playerid][P_SPREE] = 0;

	if(killerid != INVALID_PLAYER_ID)
	{
	    gPlayer[killerid][P_KILLS] += 1;
	    gPlayer[killerid][P_SPREE] += 1;
	}

	#if defined STATS_OnPlayerDeath
       	return STATS_OnPlayerDeath(playerid, killerid, reason);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerDeath
    #undef OnPlayerDeath
#else
    #define _ALS_OnPlayerDeath
#endif
#define OnPlayerDeath STATS_OnPlayerDeath
#if defined STATS_OnPlayerDeath
    forward STATS_OnPlayerDeath(playerid, killerid, reason);
#endif

public OnPlayerTakeDamage(playerid, issuerid, Float: amount, weaponid, bodypart)
{
		//headshot system
	if(	(GetPlayerTeam(playerid) == NO_TEAM && GetPlayerTeam(issuerid) == NO_TEAM) ||
		GetPlayerTeam(issuerid) != GetPlayerTeam(playerid))
	{
	    if(bodypart == 9)
		{
			if(CallLocalFunction("OnPlayerHeadshot", "iii", playerid, issuerid, weaponid))
			{
	    		gPlayer[issuerid][P_HEADSHOTS] += 1;
	    	}
	    }
	}
	else gPlayer[issuerid][P_HITS] += 1;

    #if defined STATS_OnPlayerTakeDamage
       	return STATS_OnPlayerTakeDamage(playerid, issuerid, amount, weaponid, bodypart);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerTakeDamage
    #undef OnPlayerTakeDamage
#else
    #define _ALS_OnPlayerTakeDamage
#endif
#define OnPlayerTakeDamage STATS_OnPlayerTakeDamage
#if defined STATS_OnPlayerTakeDamage
    forward STATS_OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart);
#endif

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	if(weaponid != 0)
	{
	    gPlayer[playerid][P_SHOTS] += 1;
	}

	#if defined STATS_OnPlayerWeaponShot
       	return STATS_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, fX, fY, fZ);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerWeaponShot
    #undef OnPlayerWeaponShot
#else
    #define _ALS_OnPlayerWeaponShot
#endif
#define OnPlayerWeaponShot STATS_OnPlayerWeaponShot
#if defined STATS_OnPlayerWeaponShot
    forward STATS_OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
#endif

stock GetPlayerKills(playerid)
{
	return gPlayer[playerid][P_KILLS];
}

stock GetPlayerDeaths(playerid)
{
	return gPlayer[playerid][P_DEATHS];
}

stock GetPlayerSpree(playerid)
{
	return gPlayer[playerid][P_SPREE];
}

stock GetPlayerConnectedTime(playerid, &hours, &minutes, &seconds)
{
	new connected_time = NetStats_GetConnectedTime(playerid);
	seconds = (connected_time / 1000) % 60;
	minutes = (connected_time / (1000 * 60)) % 60;
	hours = (connected_time / (1000 * 60 * 60));
	return true;
}

stock GetPlayerHeadshots(playerid)
{
	return gPlayer[playerid][P_HEADSHOTS];
}

stock GetPlayerWeaponShots(playerid)
{
	return gPlayer[playerid][P_SHOTS];
}

stock GetPlayerWeaponHits(playerid)
{
	return gPlayer[playerid][P_HITS];
}

forward OnPlayerHeadshot(playerid, issuerid, weaponid);
