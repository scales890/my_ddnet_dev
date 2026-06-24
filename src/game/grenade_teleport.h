#ifndef GAME_GRENADE_TELEPORT_H
#define GAME_GRENADE_TELEPORT_H

#include <generated/protocol.h>

enum class EKogGrenadeTeleResult
{
	NOT_APPLICABLE,
	ALLOW_FIRE,
	BLOCK_FIRE,
	TELEPORT,
};

struct SKogGrenadeTeleInput
{
	bool m_MapEnabled;
	int m_ActiveWeapon;
	bool m_HasOwnedLiveGrenade;
	bool m_FirePress;
	bool m_Frozen;
	bool m_TeleTriggeredThisTick;
	bool m_GrenadeTeleReady;
};

EKogGrenadeTeleResult KogGrenadeTeleEvaluate(const SKogGrenadeTeleInput &Input);
bool KogGrenadeTeleBlocksWeaponInput(EKogGrenadeTeleResult Result);
bool KogGrenadeTeleDisableFullAuto(bool MapEnabled, int ActiveWeapon);

#endif
