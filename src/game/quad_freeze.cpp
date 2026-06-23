#include "quad_freeze.h"

#include <base/math.h>

#include <engine/map.h>

#include <cmath>

using namespace std::chrono_literals;

static void RotateQuadPoint(const CPoint &Center, CPoint &Point, float Rotation)
{
	const int x = Point.x - Center.x;
	const int y = Point.y - Center.y;
	Point.x = (int)(x * std::cos(Rotation) - y * std::sin(Rotation) + Center.x);
	Point.y = (int)(x * std::sin(Rotation) + y * std::cos(Rotation) + Center.y);
}

bool IsMovingFreezeQuadCandidate(const CQuad &Quad)
{
	return Quad.m_PosEnv >= 0;
}

void GetAnimatedQuadCorners(const CQuad &Quad, IMap *pMap, CMapBasedEnvelopePointAccess &EnvelopePoints, std::chrono::nanoseconds EnvelopeTime, vec2 aCorners[4])
{
	vec2 Offset(0.0f, 0.0f);
	float Rotation = 0.0f;

	if(Quad.m_PosEnv >= 0 && pMap)
	{
		int EnvelopeStart, EnvelopeNum;
		pMap->GetType(MAPITEMTYPE_ENVELOPE, &EnvelopeStart, &EnvelopeNum);
		if(Quad.m_PosEnv < EnvelopeNum)
		{
			const CMapItemEnvelope *pItem = static_cast<CMapItemEnvelope *>(pMap->GetItem(EnvelopeStart + Quad.m_PosEnv));
			if(pItem->m_Channels > 0)
			{
				EnvelopePoints.SetPointsRange(pItem->m_StartPoint, pItem->m_NumPoints);
				if(EnvelopePoints.NumPoints() > 0)
				{
					ColorRGBA Position(0.0f, 0.0f, 0.0f, 0.0f);
					const size_t Channels = std::min<size_t>(3, pItem->m_Channels);
					EvalEnvelope(&EnvelopePoints, EnvelopeTime + std::chrono::milliseconds(Quad.m_PosEnvOffset), Position, Channels);
					Offset = vec2(Position.r, Position.g);
					Rotation = Position.b / 180.0f * pi;
				}
			}
		}
	}

	const CPoint &Center = Quad.m_aPoints[4];
	for(int i = 0; i < 4; i++)
	{
		CPoint Point = Quad.m_aPoints[i];
		if(Rotation != 0.0f)
			RotateQuadPoint(Center, Point, Rotation);
		aCorners[i] = vec2(fx2f(Point.x), fx2f(Point.y)) + Offset;
	}
}

bool PointInQuad(vec2 Point, const vec2 aCorners[4])
{
	int Positive = 0;
	int Negative = 0;
	for(int i = 0; i < 4; i++)
	{
		const vec2 A = aCorners[i];
		const vec2 B = aCorners[(i + 1) % 4];
		const float Cross = (B.x - A.x) * (Point.y - A.y) - (B.y - A.y) * (Point.x - A.x);
		if(Cross > 0.0f)
			Positive++;
		else if(Cross < 0.0f)
			Negative++;
	}
	return Positive == 0 || Negative == 0;
}

bool BoxOverlapsQuad(vec2 Center, vec2 HalfSize, const vec2 aCorners[4])
{
	const vec2 aBoxPoints[5] = {
		Center + vec2(-HalfSize.x, -HalfSize.y),
		Center + vec2(HalfSize.x, -HalfSize.y),
		Center + vec2(HalfSize.x, HalfSize.y),
		Center + vec2(-HalfSize.x, HalfSize.y),
		Center,
	};

	for(const vec2 &Point : aBoxPoints)
	{
		if(PointInQuad(Point, aCorners))
			return true;
	}

	for(const vec2 &Corner : aCorners)
	{
		if(std::abs(Corner.x - Center.x) <= HalfSize.x && std::abs(Corner.y - Center.y) <= HalfSize.y)
			return true;
	}

	return false;
}

std::chrono::nanoseconds EnvelopeTimeFromTick(int CurrentTick, int RoundStartTick, int TickSpeed, double IntraTick)
{
	const std::chrono::nanoseconds NanosPerTick = std::chrono::nanoseconds(1s) / TickSpeed;
	const int EnvelopeTick = CurrentTick - RoundStartTick;
	return EnvelopeTick * NanosPerTick + std::chrono::duration_cast<std::chrono::nanoseconds>(IntraTick * NanosPerTick);
}
