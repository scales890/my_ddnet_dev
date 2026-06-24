#ifndef GAME_QUAD_FREEZE_H
#define GAME_QUAD_FREEZE_H

#include <base/vmath.h>

#include <game/envelope_eval.h>
#include <game/mapitems.h>

#include <chrono>
#include <vector>

class IMap;

enum class EMovingKogQuadLayerType
{
	NONE,
	FREEZE,
	UNFREEZE,
};

constexpr const char *KOG_QUAD_LAYER_FREEZE = "QFr";
constexpr const char *KOG_QUAD_LAYER_UNFREEZE = "QUnFr";

EMovingKogQuadLayerType MovingKogQuadLayerTypeFromName(const char *pLayerName);
bool IsMovingKogQuadCandidate(const CQuad &Quad, EMovingKogQuadLayerType LayerType);

void GetAnimatedQuadCorners(const CQuad &Quad, IMap *pMap, CMapBasedEnvelopePointAccess &EnvelopePoints, std::chrono::nanoseconds EnvelopeTime, vec2 aCorners[4]);
std::chrono::nanoseconds GetQuadPositionEnvelopeMaxTime(const CQuad &Quad, IMap *pMap, CMapBasedEnvelopePointAccess &EnvelopePoints);

bool PointInQuad(vec2 Point, const vec2 aCorners[4]);
bool PointInQuadAabb(vec2 Point, float MinX, float MinY, float MaxX, float MaxY);

bool BoxOverlapsQuad(vec2 Center, vec2 HalfSize, const vec2 aCorners[4]);

std::chrono::nanoseconds EnvelopeTimeFromTick(int CurrentTick, int SyncAnchorTick, int TickSpeed, double IntraTick = 0.0);

#endif
