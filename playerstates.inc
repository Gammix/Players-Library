/*
	Player states Include (playerstates.inc)
	* Gives player's current state, or status!

 	Author: (creator)
	* Gammix

	(c) Copyright 2015
  	* This file is provided as is (no warranties).
*/

/*
FUNCTIONS:
native IsPlayerSpawned(playerid);
native IsPlayerSpectating(playerid);
native IsPlayerDriver(playerid);
native IsPlayerInClassSelection(playerid);
native IsPlayerDead(playerid);
native IsPlayerInWater(playerid);
native IsPlayerAiming(playerid);
*/

stock IsPlayerSpawned(playerid)
{
	switch(GetPlayerState(playerid))
	{
	    case PLAYER_STATE_ONFOOT, PLAYER_STATE_DRIVER, PLAYER_STATE_PASSENGER, PLAYER_STATE_SPAWNED: return true;
	    default: return false;
	}
	return false;
}

stock IsPlayerSpectating(playerid)
{
	if(GetPlayerState(playerid) == PLAYER_STATE_SPECTATING) return true;
	return false;
}

stock IsPlayerDriver(playerid)
{
	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER) return true;
	return false;
}

stock IsPlayerInClassSelection(playerid)
{
	new Float: health = 0.0;
	GetPlayerHealth(playerid, health);
	if(GetPlayerState(playerid) == PLAYER_STATE_WASTED && health > 0.0) return true;
	return false;
}

stock IsPlayerDead(playerid)
{
	new Float: health = 0.0;
	GetPlayerHealth(playerid, health);
	if(GetPlayerState(playerid) == PLAYER_STATE_WASTED && health <= 0.0) return true;
	return false;
}

stock IsPlayerInWater(playerid)
{
	new anim = GetPlayerAnimationIndex(playerid);
 	if(((anim >=  1538) && (anim <= 1542)) || (anim == 1544) || (anim == 1250) || (anim == 1062)) return true;
  	return false;
}

stock IsPlayerAiming(playerid)
{
	new keys, updown, leftright;
	GetPlayerKeys(playerid, keys, updown, leftright);
	if(keys & KEY_AIM) return true;
 	return false;
}
