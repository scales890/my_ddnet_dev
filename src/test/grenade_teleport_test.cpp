#include <gtest/gtest.h>

#include <game/grenade_teleport.h>

static SKogGrenadeTeleInput BaseInput()
{
	SKogGrenadeTeleInput Input = {};
	Input.m_MapEnabled = true;
	Input.m_ActiveWeapon = WEAPON_GRENADE;
	Input.m_GrenadeTeleReady = true;
	return Input;
}

TEST(GrenadeTeleport, DisabledOrWrongWeaponIsNotApplicable)
{
	auto Input = BaseInput();
	Input.m_MapEnabled = false;
	EXPECT_EQ(KogGrenadeTeleEvaluate(Input), EKogGrenadeTeleResult::NOT_APPLICABLE);

	Input = BaseInput();
	Input.m_ActiveWeapon = WEAPON_GUN;
	EXPECT_EQ(KogGrenadeTeleEvaluate(Input), EKogGrenadeTeleResult::NOT_APPLICABLE);
}

TEST(GrenadeTeleport, FirstShotAllowedWithoutLiveGrenade)
{
	auto Input = BaseInput();
	Input.m_FirePress = true;
	EXPECT_EQ(KogGrenadeTeleEvaluate(Input), EKogGrenadeTeleResult::ALLOW_FIRE);
	EXPECT_FALSE(KogGrenadeTeleBlocksWeaponInput(EKogGrenadeTeleResult::ALLOW_FIRE));
}

TEST(GrenadeTeleport, HoldWithLiveGrenadeBlocksFire)
{
	auto Input = BaseInput();
	Input.m_HasOwnedLiveGrenade = true;
	EXPECT_EQ(KogGrenadeTeleEvaluate(Input), EKogGrenadeTeleResult::BLOCK_FIRE);
	EXPECT_TRUE(KogGrenadeTeleBlocksWeaponInput(EKogGrenadeTeleResult::BLOCK_FIRE));
}

TEST(GrenadeTeleport, PressWithLiveGrenadeTeleports)
{
	auto Input = BaseInput();
	Input.m_HasOwnedLiveGrenade = true;
	Input.m_FirePress = true;
	EXPECT_EQ(KogGrenadeTeleEvaluate(Input), EKogGrenadeTeleResult::TELEPORT);
	EXPECT_TRUE(KogGrenadeTeleBlocksWeaponInput(EKogGrenadeTeleResult::TELEPORT));
}

TEST(GrenadeTeleport, SameTickGrenadeCannotTeleportYet)
{
	auto Input = BaseInput();
	Input.m_HasOwnedLiveGrenade = true;
	Input.m_FirePress = true;
	Input.m_GrenadeTeleReady = false;
	EXPECT_EQ(KogGrenadeTeleEvaluate(Input), EKogGrenadeTeleResult::BLOCK_FIRE);
}

TEST(GrenadeTeleport, FrozenPressDoesNotTeleport)
{
	auto Input = BaseInput();
	Input.m_HasOwnedLiveGrenade = true;
	Input.m_FirePress = true;
	Input.m_Frozen = true;
	EXPECT_EQ(KogGrenadeTeleEvaluate(Input), EKogGrenadeTeleResult::BLOCK_FIRE);
}

TEST(GrenadeTeleport, TeleTriggeredBlocksDuplicateHandling)
{
	auto Input = BaseInput();
	Input.m_TeleTriggeredThisTick = true;
	EXPECT_EQ(KogGrenadeTeleEvaluate(Input), EKogGrenadeTeleResult::BLOCK_FIRE);
}

TEST(GrenadeTeleport, FullAutoDisabledForKogGrenade)
{
	EXPECT_TRUE(KogGrenadeTeleDisableFullAuto(true, WEAPON_GRENADE));
	EXPECT_FALSE(KogGrenadeTeleDisableFullAuto(true, WEAPON_GUN));
	EXPECT_FALSE(KogGrenadeTeleDisableFullAuto(false, WEAPON_GRENADE));
}
