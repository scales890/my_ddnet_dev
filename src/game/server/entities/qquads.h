/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_QQUADS_H
#define GAME_SERVER_ENTITIES_QQUADS_H

#include <game/server/entity.h>

class CQquads : public CEntity
{
public:
	CQquads(CGameWorld *pGameWorld, int Type, vec2 Pos, bool Freeze, int PosEnv, int Layer = 0, int Number = 0);

	virtual void Reset() override;
	virtual void Tick() override;
	virtual void TickPaused() override;

private:
	int m_Type;
	int m_StartTick;
	bool m_Freeze;
};

#endif
