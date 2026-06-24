#include "grenade_teleport.h"

EKogGrenadeTeleResult KogGrenadeTeleEvaluate(const SKogGrenadeTeleInput &Input)
{
	if(!Input.m_MapEnabled || Input.m_ActiveWeapon != WEAPON_GRENADE)
		return EKogGrenadeTeleResult::NOT_APPLICABLE;

	if(Input.m_FirePress && !Input.m_Frozen)
	{
		if(Input.m_TeleTriggeredThisTick)
			return EKogGrenadeTeleResult::BLOCK_FIRE;
		if(Input.m_HasOwnedLiveGrenade && Input.m_GrenadeTeleReady)
			return EKogGrenadeTeleResult::TELEPORT;
	}

	if(Input.m_TeleTriggeredThisTick)
		return EKogGrenadeTeleResult::BLOCK_FIRE;

	if(Input.m_HasOwnedLiveGrenade)
		return EKogGrenadeTeleResult::BLOCK_FIRE;

	return EKogGrenadeTeleResult::ALLOW_FIRE;
}

bool KogGrenadeTeleBlocksWeaponInput(EKogGrenadeTeleResult Result)
{
	return Result == EKogGrenadeTeleResult::BLOCK_FIRE || Result == EKogGrenadeTeleResult::TELEPORT;
}

bool KogGrenadeTeleDisableFullAuto(bool MapEnabled, int ActiveWeapon)
{
	return MapEnabled && ActiveWeapon == WEAPON_GRENADE;
}
