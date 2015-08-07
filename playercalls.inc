/*

playercalls.inc
- Some usefull player callbacks and with interesting capabilities!
- For example: return 0 in a callback like OnPlayerPosChange, and the player can't move!
- Now pause detection, spot AFK players.

Author: (creator)
- Gammix

(c) Copyright 2015
- This file is provided as is (no warranties).

*/

/*

STOCKS:

native IsPlayerPaused(playerid);
native GetPlayerPausedTime(playerid);

//

CALLBACKS:

public OnPlayerPause(playerid);
public OnPlayerUnpause(playerid, time);
public OnPlayerPosChange(playerid, Float:newx, Float:newy, Float:newz, Float:oldx, Float:oldy, Float:oldz);
public OnPlayerFacingAngleChange(playerid, Float:newangle, Float:oldangle);
public OnPlayerWeaponChange(playerid, newweapon, oldweapon);
public OnPlayerHealthChange(playerid, Float:newhealth, Float:oldhealth);
public OnPlayerArmourChange(playerid, Float:newarmour, Float:oldarmour);
public OnPlayerVirtualWorldChange(playerid, newworld, oldworld);
public OnPlayerSkinChange(playerid, newskin, oldskin);
public OnPlayerPingChange(playerid, newping, oldping);
public OnPlayerAmmoChange(playerid, weaponid, newammo, oldammo);
public OnPlayerKillPlayer(playerid, killerid, weaponid);
public OnPlayerMoneyChange(playerid, newmoney, oldmoney);
public OnPlayerScoreChange(playerid, newscore, oldscore);

*/

#define PAUSE_CHECK_TIME 5 // This is the time after which the player will be considered as PAUSED.

enum
	PlayerCallEnum
{
	bool:P_ONPLAYERUPDATE,

	Float:P_HEALTH,
	Float:P_ARMOUR,

	Float:P_X,
	Float:P_Y,
	Float:P_Z,
	Float:P_ANGLE,

	P_WORLDID,

	P_SKIN,

	P_WEAPON,
	P_AMMO,

	P_MONEY,
	P_SCORE,

	P_PING,

	P_LAST_UPDATE,
	P_PAUSED_TIMER
}
static
	gPlayerCall[MAX_PLAYERS][PlayerCallEnum],
	bool:gHasCallback[12]
;

//internal function
stock static CALL_IsPlayerInClassSelection(playerid)
{
	new
		Float: health = 0.0
	;
	GetPlayerHealth(playerid, health);
	
	if(GetPlayerState(playerid) == PLAYER_STATE_WASTED && health > 0.0)
	{
		return true;
	}
	return false;
}

#if ! defined CALL_IsValidWeapon
	#define CALL_IsValidWeapon(%0) \
				(%0 < 47)
#endif

//

#if defined FILTESCRIPT

	public OnFilterScriptInit()
	{
		//for onplayerupdate only
		gHasCallback[0] = funcidx("OnPlayerUnpause") != -1;
		gHasCallback[1] = funcidx("OnPlayerPosChange") != -1;
		gHasCallback[2] = funcidx("OnPlayerFacingAngleChange") != -1;
		gHasCallback[3] = funcidx("OnPlayerWeaponChange") != -1;
		gHasCallback[4] = funcidx("OnPlayerHealthChange") != -1;
		gHasCallback[5] = funcidx("OnPlayerArmourChange") != -1;
		gHasCallback[6] = funcidx("OnPlayerVirtualWorldChange") != -1;
		gHasCallback[7] = funcidx("OnPlayerSkinChange") != -1;
		gHasCallback[8] = funcidx("OnPlayerPingChange") != -1;
		gHasCallback[9] = funcidx("OnPlayerAmmoChange") != -1;
		gHasCallback[10] = funcidx("OnPlayerMoneyChange") != -1;
		gHasCallback[11] = funcidx("OnPlayerScoreChange") != -1;

	    #if defined Calls_OnFilterScriptInit
        	return Calls_OnFilterScriptInit();
	    #else
	    	return 1;
	    #endif
	}
	#if defined _ALS_OnFilterScriptInit
	    #undef OnFilterScriptInit
	#else
	    #define _ALS_OnFilterScriptInit
	#endif
	#define OnFilterScriptInit Calls_OnFilterScriptInit
	#if defined Calls_OnFilterScriptInit
	    forward Calls_OnFilterScriptInit();
	#endif

#else

	public OnGameModeInit()
	{
		//for onplayerupdate only
		gHasCallback[0] = funcidx("OnPlayerUnpause") != -1;
		gHasCallback[1] = funcidx("OnPlayerPosChange") != -1;
		gHasCallback[2] = funcidx("OnPlayerFacingAngleChange") != -1;
		gHasCallback[3] = funcidx("OnPlayerWeaponChange") != -1;
		gHasCallback[4] = funcidx("OnPlayerHealthChange") != -1;
		gHasCallback[5] = funcidx("OnPlayerArmourChange") != -1;
		gHasCallback[6] = funcidx("OnPlayerVirtualWorldChange") != -1;
		gHasCallback[7] = funcidx("OnPlayerSkinChange") != -1;
		gHasCallback[8] = funcidx("OnPlayerPingChange") != -1;
		gHasCallback[9] = funcidx("OnPlayerAmmoChange") != -1;
		gHasCallback[10] = funcidx("OnPlayerMoneyChange") != -1;
		gHasCallback[11] = funcidx("OnPlayerScoreChange") != -1;

		#if defined Calls_OnGameModeInit
        	return Calls_OnGameModeInit();
	    #else
	    	return 1;
	    #endif
	}
	#if defined _ALS_OnGameModeInit
	    #undef OnGameModeInit
	#else
	    #define _ALS_OnGameModeInit
	#endif
	#define OnGameModeInit Calls_OnGameModeInit
	#if defined Calls_OnGameModeInit
	    forward Calls_OnGameModeInit();
	#endif

#endif

stock GetPlayerPausedTime(playerid)
{
	return GetPVarInt(playerid, "Calls_PausedTime");
}

stock IsPlayerPaused(playerid)
{
	return bool:GetPVarType(playerid, "Calls_Paused");
}

public OnPlayerConnect(playerid)
{
	if(GetPVarType(playerid, "Calls_OPU") == PLAYER_VARTYPE_NONE)
	{
    	gPlayerCall[playerid][P_ONPLAYERUPDATE] = true;
    	
		gPlayerCall[playerid][P_PAUSED_TIMER] = SetTimerEx("CALLS_OnPlayerPause", 1000, true, "i", playerid);
    }
    else
    {
    	gPlayerCall[playerid][P_ONPLAYERUPDATE] = true;
    }
    
	#if defined Calls_OnPlayerConnect
       	return Calls_OnPlayerConnect(playerid);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect Calls_OnPlayerConnect
#if defined Calls_OnPlayerConnect
    forward Calls_OnPlayerConnect(playerid);
#endif

public OnPlayerDisconnect(playerid, reason)
{
	if(GetPVarType(playerid, "Calls_OPU") != PLAYER_VARTYPE_NONE)
	{
		KillTimer(gPlayerCall[playerid][P_PAUSED_TIMER]);
	}
	gPlayerCall[playerid][P_ONPLAYERUPDATE] = false;

    #if defined Calls_OnPlayerDisconnect
       	return Calls_OnPlayerDisconnect(playerid, reason);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect Calls_OnPlayerDisconnect
#if defined Calls_OnPlayerDisconnect
    forward Calls_OnPlayerDisconnect(playerid, reason);
#endif

public OnPlayerSpawn(playerid)
{
	if(gPlayerCall[playerid][P_ONPLAYERUPDATE])
	{
		GetPlayerPos(playerid, gPlayerCall[playerid][P_X], gPlayerCall[playerid][P_Y], gPlayerCall[playerid][P_Z]);
		GetPlayerFacingAngle(playerid, gPlayerCall[playerid][P_ANGLE]);

		gPlayerCall[playerid][P_WORLDID] = GetPlayerVirtualWorld(playerid);

		GetPlayerHealth(playerid, gPlayerCall[playerid][P_HEALTH]);
		GetPlayerArmour(playerid, gPlayerCall[playerid][P_ARMOUR]);

		gPlayerCall[playerid][P_SKIN] = GetPlayerSkin(playerid);

		gPlayerCall[playerid][P_WEAPON] = GetPlayerWeapon(playerid);
		gPlayerCall[playerid][P_AMMO] = GetPlayerAmmo(playerid);

		gPlayerCall[playerid][P_MONEY] = GetPlayerMoney(playerid);
		gPlayerCall[playerid][P_SCORE] = GetPlayerScore(playerid);

		gPlayerCall[playerid][P_PING] = GetPlayerPing(playerid);
	}

	#if defined Calls_OnPlayerSpawn
       	return Calls_OnPlayerSpawn(playerid);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerSpawn
    #undef OnPlayerSpawn
#else
    #define _ALS_OnPlayerSpawn
#endif
#define OnPlayerSpawn Calls_OnPlayerSpawn
#if defined Calls_OnPlayerSpawn
    forward Calls_OnPlayerSpawn(playerid);
#endif

public OnPlayerUpdate(playerid)
{
	if(gPlayerCall[playerid][P_ONPLAYERUPDATE])
	{
		//OnPlayerUnpause
	    if(gHasCallback[0])
		{
			gPlayerCall[playerid][P_LAST_UPDATE] = GetTickCount();

		    if(IsPlayerPaused(playerid))
		    {
		        CallLocalFunction("OnPlayerUnpause", "ii", playerid, GetPlayerPausedTime(playerid));
		        CallRemoteFunction("OnPlayerUnpause", "ii", playerid, GetPlayerPausedTime(playerid));

				DeletePVar(playerid, "Calls_Paused");
				DeletePVar(playerid, "Calls_PausedTime");
		    }
		}

		//OnPlayerPosChange
		if(gHasCallback[1])
		{
			new
				Float:player_pos[3]
			;
			GetPlayerPos(playerid, player_pos[0], player_pos[1], player_pos[2]);
			
			if(	player_pos[0] != gPlayerCall[playerid][P_X] ||
				player_pos[1] != gPlayerCall[playerid][P_Y] ||
				player_pos[2] != gPlayerCall[playerid][P_Z])
			{
			    if(	(CallLocalFunction("OnPlayerPosChange", "iffffff", playerid, player_pos[0], player_pos[1], player_pos[2], gPlayerCall[playerid][P_X], gPlayerCall[playerid][P_Y], gPlayerCall[playerid][P_Z]) && CallRemoteFunction("OnPlayerPosChange", "iffffff", playerid, player_pos[0], player_pos[1], player_pos[2], gPlayerCall[playerid][P_X], gPlayerCall[playerid][P_Y], gPlayerCall[playerid][P_Z])) ||
					CallLocalFunction("OnPlayerPosChange", "iffffff", playerid, player_pos[0], player_pos[1], player_pos[2], gPlayerCall[playerid][P_X], gPlayerCall[playerid][P_Y], gPlayerCall[playerid][P_Z]) ||
					CallRemoteFunction("OnPlayerPosChange", "iffffff", playerid, player_pos[0], player_pos[1], player_pos[2], gPlayerCall[playerid][P_X], gPlayerCall[playerid][P_Y], gPlayerCall[playerid][P_Z]))
				{
					GetPlayerPos(playerid, gPlayerCall[playerid][P_X], gPlayerCall[playerid][P_Y], gPlayerCall[playerid][P_Z]);
				}
				else
				{
				    SetPlayerPos(playerid, gPlayerCall[playerid][P_X], gPlayerCall[playerid][P_Y], gPlayerCall[playerid][P_Z]);
				}
			}
		}

		//OnPlayerFacingAngleChange
		if(gHasCallback[2])
		{
			new
				Float:player_angle
			;
			GetPlayerFacingAngle(playerid, player_angle);
			
			if(player_angle != gPlayerCall[playerid][P_ANGLE])
			{
			    if(	(CallLocalFunction("OnPlayerFacingAngleChange", "iff", playerid, player_angle, gPlayerCall[playerid][P_ANGLE]) && CallRemoteFunction("OnPlayerFacingAngleChange", "iff", playerid, player_angle, gPlayerCall[playerid][P_ANGLE])) ||
					CallLocalFunction("OnPlayerFacingAngleChange", "iff", playerid, player_angle, gPlayerCall[playerid][P_ANGLE]) ||
					CallRemoteFunction("OnPlayerFacingAngleChange", "iff", playerid, player_angle, gPlayerCall[playerid][P_ANGLE]))
				{
					GetPlayerFacingAngle(playerid, gPlayerCall[playerid][P_ANGLE]);
				}
				else
				{
				    SetPlayerFacingAngle(playerid, gPlayerCall[playerid][P_ANGLE]);
				}
			}
		}

		//OnPlayerWeaponChange
		if(gHasCallback[3])
		{
			new
				player_weapon = GetPlayerWeapon(playerid)
			;
			
			if(player_weapon != gPlayerCall[playerid][P_WEAPON])
			{
			    if( (CallLocalFunction("OnPlayerWeaponChange", "iii", playerid, player_weapon, gPlayerCall[playerid][P_WEAPON]) && CallRemoteFunction("OnPlayerWeaponChange", "iii", playerid, player_weapon, gPlayerCall[playerid][P_WEAPON])) ||
					CallLocalFunction("OnPlayerWeaponChange", "iii", playerid, player_weapon, gPlayerCall[playerid][P_WEAPON]) ||
					CallRemoteFunction("OnPlayerWeaponChange", "iii", playerid, player_weapon, gPlayerCall[playerid][P_WEAPON]))
				{
					gPlayerCall[playerid][P_WEAPON] = player_weapon;
				}
				else
				{
				    SetPlayerArmedWeapon(playerid, gPlayerCall[playerid][P_WEAPON]);
				}
			}
		}

		//OnPlayerHealthChange
		if(gHasCallback[4])
		{
			new
				Float:player_health
			;
			GetPlayerHealth(playerid, player_health);
			
			if(player_health != gPlayerCall[playerid][P_HEALTH])
			{
			    if( (CallLocalFunction("OnPlayerHealthChange", "iff", playerid, player_health, gPlayerCall[playerid][P_HEALTH]) && CallRemoteFunction("OnPlayerHealthChange", "iff", playerid, player_health, gPlayerCall[playerid][P_HEALTH])) ||
					CallLocalFunction("OnPlayerHealthChange", "iff", playerid, player_health, gPlayerCall[playerid][P_HEALTH]) ||
					CallRemoteFunction("OnPlayerHealthChange", "iff", playerid, player_health, gPlayerCall[playerid][P_HEALTH]))
				{
					GetPlayerHealth(playerid, gPlayerCall[playerid][P_HEALTH]);
				}
				else
				{
				    SetPlayerHealth(playerid, gPlayerCall[playerid][P_HEALTH]);
				}
			}
		}

		//OnPlayerArmourChange
		if(gHasCallback[5])
		{
			new
				Float:player_armour
			;
			GetPlayerArmour(playerid, player_armour);
			
			if(player_armour != gPlayerCall[playerid][P_ARMOUR])
			{
			    if( (CallLocalFunction("OnPlayerArmourChange", "iff", playerid, player_armour, gPlayerCall[playerid][P_ARMOUR]) && CallRemoteFunction("OnPlayerArmourChange", "iff", playerid, player_armour, gPlayerCall[playerid][P_ARMOUR])) ||
					CallLocalFunction("OnPlayerArmourChange", "iff", playerid, player_armour, gPlayerCall[playerid][P_ARMOUR]) ||
					CallRemoteFunction("OnPlayerArmourChange", "iff", playerid, player_armour, gPlayerCall[playerid][P_ARMOUR]))
				{
					GetPlayerArmour(playerid, gPlayerCall[playerid][P_ARMOUR]);
				}
				else
				{
				    SetPlayerArmour(playerid, gPlayerCall[playerid][P_ARMOUR]);
				}
			}
		}

		//OnPlayerVirtualWorldChange
		if(gHasCallback[6])
		{
			new
				player_world = GetPlayerVirtualWorld(playerid)
			;
			
			if(player_world != gPlayerCall[playerid][P_WORLDID])
			{
			    if( (CallLocalFunction("OnPlayerVirtualWorldChange", "iii", playerid, player_world, gPlayerCall[playerid][P_WORLDID]) && CallRemoteFunction("OnPlayerVirtualWorldChange", "iii", playerid, player_world, gPlayerCall[playerid][P_WORLDID])) ||
					CallLocalFunction("OnPlayerVirtualWorldChange", "iii", playerid, player_world, gPlayerCall[playerid][P_WORLDID]) ||
					CallRemoteFunction("OnPlayerVirtualWorldChange", "iii", playerid, player_world, gPlayerCall[playerid][P_WORLDID]))
				{
					gPlayerCall[playerid][P_WORLDID] = player_world;
				}
				else
				{
				    SetPlayerVirtualWorld(playerid, gPlayerCall[playerid][P_WORLDID]);
				}
			}
		}

		//OnPlayerSkinChange
		if(gHasCallback[7])
		{
			new
				player_skin = GetPlayerSkin(playerid)
			;
			
			if(player_skin != gPlayerCall[playerid][P_SKIN])
			{
			    if(	(CallLocalFunction("OnPlayerSkinChange", "iii", playerid, player_skin, gPlayerCall[playerid][P_SKIN]) && CallRemoteFunction("OnPlayerSkinChange", "iii", playerid, player_skin, gPlayerCall[playerid][P_SKIN])) ||
					CallLocalFunction("OnPlayerSkinChange", "iii", playerid, player_skin, gPlayerCall[playerid][P_SKIN]) ||
					CallRemoteFunction("OnPlayerSkinChange", "iii", playerid, player_skin, gPlayerCall[playerid][P_SKIN]))
				{
					gPlayerCall[playerid][P_SKIN] = player_skin;
				}
				else
				{
				    SetPlayerSkin(playerid, gPlayerCall[playerid][P_SKIN]);
				}
			}
		}

		//OnPlayerPingChange
		if(gHasCallback[8])
		{
			new
				player_ping = GetPlayerPing(playerid)
			;
			
			if(player_ping != gPlayerCall[playerid][P_PING])
			{
			    CallLocalFunction("OnPlayerPingChange", "iii", playerid, player_ping, gPlayerCall[playerid][P_PING]);
			    CallRemoteFunction("OnPlayerPingChange", "iii", playerid, player_ping, gPlayerCall[playerid][P_PING]);

				gPlayerCall[playerid][P_PING] = player_ping;
			}
		}

		//OnPlayerAmmoChange
		if(gHasCallback[9])
		{
		    //OnPlayerAmmoChange
			new
		 		player_current_weapon = GetPlayerWeapon(playerid),
				player_ammo = GetPlayerAmmo(playerid)
			;
			
			if(player_ammo != gPlayerCall[playerid][P_AMMO])
			{
			    if(	(CallLocalFunction("OnPlayerAmmoChange", "iiii", playerid, player_current_weapon, player_ammo, gPlayerCall[playerid][P_AMMO]) && CallRemoteFunction("OnPlayerAmmoChange", "iiii", playerid, player_current_weapon, player_ammo, gPlayerCall[playerid][P_AMMO])) ||
					CallLocalFunction("OnPlayerAmmoChange", "iiii", playerid, player_current_weapon, player_ammo, gPlayerCall[playerid][P_AMMO]) ||
					CallRemoteFunction("OnPlayerAmmoChange", "iiii", playerid, player_current_weapon, player_ammo, gPlayerCall[playerid][P_AMMO]))
				{
					gPlayerCall[playerid][P_AMMO] = player_ammo;
				}
				else
				{
				    SetPlayerAmmo(playerid, player_current_weapon, gPlayerCall[playerid][P_AMMO]);
				}
			}
		}

		//OnPlayerMoneyChange
		if(gHasCallback[10])
		{
			new
				player_money = GetPlayerMoney(playerid)
			;
			
			if(player_money != gPlayerCall[playerid][P_MONEY])
			{
			    if( (CallLocalFunction("OnPlayerMoneyChange", "iii", playerid, player_money, gPlayerCall[playerid][P_MONEY]) && CallRemoteFunction("OnPlayerMoneyChange", "iii", playerid, player_money, gPlayerCall[playerid][P_MONEY])) ||
					CallLocalFunction("OnPlayerMoneyChange", "iii", playerid, player_money, gPlayerCall[playerid][P_MONEY]) ||
					CallRemoteFunction("OnPlayerMoneyChange", "iii", playerid, player_money, gPlayerCall[playerid][P_MONEY]))
				{
					gPlayerCall[playerid][P_MONEY] = player_money;
				}
				else
				{
				    ResetPlayerMoney(playerid);
				    GivePlayerMoney(playerid, gPlayerCall[playerid][P_MONEY]);
				}
			}
		}

		//OnPlayerScoreChange
		if(gHasCallback[11])
		{
			new
				player_score = GetPlayerScore(playerid)
			;
				
			if(player_score != gPlayerCall[playerid][P_SCORE])
			{
			    if(	(CallLocalFunction("OnPlayerScoreChange", "iii", playerid, player_score, gPlayerCall[playerid][P_SCORE]) && CallRemoteFunction("OnPlayerScoreChange", "iii", playerid, player_score, gPlayerCall[playerid][P_SCORE])) ||
					CallLocalFunction("OnPlayerScoreChange", "iii", playerid, player_score, gPlayerCall[playerid][P_SCORE]) ||
					CallRemoteFunction("OnPlayerScoreChange", "iii", playerid, player_score, gPlayerCall[playerid][P_SCORE]))
				{
					gPlayerCall[playerid][P_SCORE] = player_score;
				}
				else
				{
				    SetPlayerScore(playerid, gPlayerCall[playerid][P_SCORE]);
				}
			}
		}
	}
	
	#if defined Calls_OnPlayerUpdate
       	return Calls_OnPlayerUpdate(playerid);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerUpdate
    #undef OnPlayerUpdate
#else
    #define _ALS_OnPlayerUpdate
#endif
#define OnPlayerUpdate Calls_OnPlayerUpdate
#if defined Calls_OnPlayerUpdate
    forward Calls_OnPlayerUpdate(playerid);
#endif

public OnPlayerDeath(playerid, killerid, reason)
{
	if(gPlayerCall[playerid][P_ONPLAYERUPDATE])
	{
		//OnPlayerKillPlayer
		if(	killerid != INVALID_PLAYER_ID &&
			playerid != INVALID_PLAYER_ID &&
			! IsPlayerNPC(killerid) &&
			! IsPlayerNPC(playerid))
		{
		    reason = GetPlayerWeapon(killerid);
		    if(! CALL_IsValidWeapon(reason))
			{
				reason = 0;
			}
			
		    CallLocalFunction("OnPlayerKillPlayer", "iii", playerid, killerid, reason);
		    CallRemoteFunction("OnPlayerKillPlayer", "iii", playerid, killerid, reason);
		}
	}
	
	#if defined Calls_OnPlayerDeath
       	return Calls_OnPlayerDeath(playerid, killerid, reason);
	#else
	   	return 1;
	#endif
}
#if defined _ALS_OnPlayerDeath
    #undef OnPlayerDeath
#else
    #define _ALS_OnPlayerDeath
#endif
#define OnPlayerDeath Calls_OnPlayerDeath
#if defined Calls_OnPlayerDeath
    forward Calls_OnPlayerDeath(playerid, killerid, reason);
#endif

//internal callbacks
forward CALLS_OnPlayerPause(playerid);
public CALLS_OnPlayerPause(playerid)
{
    new
		CurrentTime = GetTickCount(),
		PlayerState = GetPlayerState(playerid)
	;

	if(! IsPlayerPaused(playerid))
 	{
  		if(! CALL_IsPlayerInClassSelection(playerid))
    	{
     		if((CurrentTime - gPlayerCall[playerid][P_LAST_UPDATE]) >= (PAUSE_CHECK_TIME * 1000))
       		{
         		if(	(PlayerState == PLAYER_STATE_ONFOOT) ||
				 	(PlayerState == PLAYER_STATE_DRIVER) ||
				 	(PlayerState == PLAYER_STATE_PASSENGER))
                {
              		CallLocalFunction("OnPlayerPause", "i", playerid);
				  	CallRemoteFunction("OnPlayerPause", "i", playerid);

					SetPVarInt(playerid, "Calls_Paused", 1);
					SetPVarInt(playerid, "Calls_Paused", 0);
                }
            }
        }
    }
    else
    {
		SetPVarInt(playerid, "Calls_Paused", GetPlayerPausedTime(playerid) + 1);
    }
    return 1;
}

forward OnPlayerPosChange(playerid, Float:newx, Float:newy, Float:newz, Float:oldx, Float:oldy, Float:oldz);
forward OnPlayerFacingAngleChange(playerid, Float:newangle, Float:oldangle);
forward OnPlayerWeaponChange(playerid, newweapon, oldweapon);
forward OnPlayerHealthChange(playerid, Float:newhealth, Float:oldhealth);
forward OnPlayerArmourChange(playerid, Float:newarmour, Float:oldarmour);
forward OnPlayerVirtualWorldChange(playerid, newworld, oldworld);
forward OnPlayerSkinChange(playerid, newskin, oldskin);
forward OnPlayerPingChange(playerid, newping, oldping);
forward OnPlayerAmmoChange(playerid, weaponid, newammo, oldammo);
forward OnPlayerKillPlayer(playerid, killerid, weaponid);
forward OnPlayerPause(playerid);
forward OnPlayerUnpause(playerid, time);
forward OnPlayerFakeKill(playerid, count);
forward OnPlayerMoneyChange(playerid, newmoney, oldmoney);
forward OnPlayerScoreChange(playerid, newscore, oldscore);

#undef PAUSE_CHECK_TIME
#undef CALL_IsValidWeapon
