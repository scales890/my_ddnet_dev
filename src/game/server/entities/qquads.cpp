/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "qquads.h"
#include "character.h"

#include <engine/shared/config.h>

#include <game/generated/protocol.h>
#include <game/mapitems.h>

#include <game/server/gamecontext.h>
#include <game/server/gamemodes/DDRace.h>

CQquads::CQquads(
	CGameWorld *pGameWorld,
	int Type,
	vec2 Pos,
	bool Freeze,
	int PosEnv,
	int Layer,
	int Number) :
	CEntity(pGameWorld, CGameWorld::ENTTYPE_QQUADS)
{
	m_Type = Type;
	m_Pos = Pos;
	m_StartTick = Server()->Tick();

	m_Layer = Layer;
	m_Number = Number;
	m_Freeze = Freeze;

	GameWorld()->InsertEntity(this);
}

void CQquads::Reset()
{
	m_MarkedForDestroy = true;
}

void CQquads::Tick()
{
	
}

void CQquads::TickPaused()
{
	++m_StartTick;
}