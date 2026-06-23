#include "envelope_eval.h"

#include <base/math.h>

#include <engine/map.h>
#include <engine/shared/map.h>

#include <game/mapitems_ex.h>

#include <algorithm>
#include <chrono>
#include <cmath>

using namespace std::chrono_literals;

int IEnvelopePointAccess::FindPointIndex(CFixedTime Time) const
{
	int Low = 0;
	int High = NumPoints() - 2;
	int FoundIndex = -1;

	while(Low <= High)
	{
		int Mid = Low + (High - Low) / 2;
		const CEnvPoint *pMid = GetPoint(Mid);
		const CEnvPoint *pNext = GetPoint(Mid + 1);
		if(Time >= pMid->m_Time && Time < pNext->m_Time)
		{
			FoundIndex = Mid;
			break;
		}
		else if(Time < pMid->m_Time)
		{
			High = Mid - 1;
		}
		else
		{
			Low = Mid + 1;
		}
	}
	return FoundIndex;
}

CMapBasedEnvelopePointAccess::CMapBasedEnvelopePointAccess(IMap *pMap)
{
	bool FoundBezierEnvelope = false;
	int EnvelopeStart, EnvelopeNum;
	pMap->GetType(MAPITEMTYPE_ENVELOPE, &EnvelopeStart, &EnvelopeNum);
	for(int EnvelopeIndex = 0; EnvelopeIndex < EnvelopeNum; EnvelopeIndex++)
	{
		CMapItemEnvelope *pEnvelope = static_cast<CMapItemEnvelope *>(pMap->GetItem(EnvelopeStart + EnvelopeIndex));
		if(pEnvelope->m_Version >= CMapItemEnvelope::VERSION_TEEWORLDS_BEZIER)
		{
			FoundBezierEnvelope = true;
			break;
		}
	}

	if(FoundBezierEnvelope)
	{
		m_pPoints = nullptr;
		m_pPointsBezier = nullptr;

		int EnvPointStart, FakeEnvPointNum;
		pMap->GetType(MAPITEMTYPE_ENVPOINTS, &EnvPointStart, &FakeEnvPointNum);
		if(FakeEnvPointNum > 0)
			m_pPointsBezierUpstream = static_cast<CEnvPointBezier_upstream *>(pMap->GetItem(EnvPointStart));
		else
			m_pPointsBezierUpstream = nullptr;

		m_NumPointsMax = pMap->GetItemSize(EnvPointStart) / sizeof(CEnvPointBezier_upstream);
	}
	else
	{
		int EnvPointStart, FakeEnvPointNum;
		pMap->GetType(MAPITEMTYPE_ENVPOINTS, &EnvPointStart, &FakeEnvPointNum);
		if(FakeEnvPointNum > 0)
			m_pPoints = static_cast<CEnvPoint *>(pMap->GetItem(EnvPointStart));
		else
			m_pPoints = nullptr;

		m_NumPointsMax = pMap->GetItemSize(EnvPointStart) / sizeof(CEnvPoint);

		int EnvPointBezierStart, FakeEnvPointBezierNum;
		pMap->GetType(MAPITEMTYPE_ENVPOINTS_BEZIER, &EnvPointBezierStart, &FakeEnvPointBezierNum);
		const int NumPointsBezier = pMap->GetItemSize(EnvPointBezierStart) / sizeof(CEnvPointBezier);
		if(FakeEnvPointBezierNum > 0 && m_NumPointsMax == NumPointsBezier)
			m_pPointsBezier = static_cast<CEnvPointBezier *>(pMap->GetItem(EnvPointBezierStart));
		else
			m_pPointsBezier = nullptr;

		m_pPointsBezierUpstream = nullptr;
	}

	SetPointsRange(0, m_NumPointsMax);
}

void CMapBasedEnvelopePointAccess::SetPointsRange(int StartPoint, int NumPoints)
{
	m_StartPoint = std::clamp(StartPoint, 0, m_NumPointsMax);
	m_NumPoints = std::clamp(NumPoints, 0, std::max(m_NumPointsMax - StartPoint, 0));
}

int CMapBasedEnvelopePointAccess::StartPoint() const
{
	return m_StartPoint;
}

int CMapBasedEnvelopePointAccess::NumPoints() const
{
	return m_NumPoints;
}

int CMapBasedEnvelopePointAccess::NumPointsMax() const
{
	return m_NumPointsMax;
}

const CEnvPoint *CMapBasedEnvelopePointAccess::GetPoint(int Index) const
{
	if(Index < 0 || Index >= m_NumPoints)
		return nullptr;
	if(m_pPoints != nullptr)
		return &m_pPoints[Index + m_StartPoint];
	if(m_pPointsBezierUpstream != nullptr)
		return &m_pPointsBezierUpstream[Index + m_StartPoint];
	return nullptr;
}

const CEnvPointBezier *CMapBasedEnvelopePointAccess::GetBezier(int Index) const
{
	if(Index < 0 || Index >= m_NumPoints)
		return nullptr;
	if(m_pPointsBezier != nullptr)
		return &m_pPointsBezier[Index + m_StartPoint];
	if(m_pPointsBezierUpstream != nullptr)
		return &m_pPointsBezierUpstream[Index + m_StartPoint].m_Bezier;
	return nullptr;
}

static float SolveBezier(float x, float p0, float p1, float p2, float p3)
{
	const double x3 = -p0 + 3.0 * p1 - 3.0 * p2 + p3;
	const double x2 = 3.0 * p0 - 6.0 * p1 + 3.0 * p2;
	const double x1 = -3.0 * p0 + 3.0 * p1;
	const double x0 = p0 - x;

	if(x3 == 0.0 && x2 == 0.0)
	{
		const double a = x1;
		const double b = x0;

		if(a == 0.0)
			return 0.0f;
		return -b / a;
	}
	else if(x3 == 0.0)
	{
		const double b = x1 / x2;
		const double c = x0 / x2;

		if(c == 0.0)
			return 0.0f;

		const double D = b * b - 4.0 * c;
		const double SqrtD = std::sqrt(D);

		const double t = (-b + SqrtD) / 2.0;

		if(0.0 <= t && t <= 1.0001)
			return t;
		return (-b - SqrtD) / 2.0;
	}
	else
	{
		const double a = x2 / x3;
		const double b = x1 / x3;
		const double c = x0 / x3;

		const double Substitute = a / 3.0;

		const double p = b / 3.0 - a * a / 9.0;
		const double q = (2.0 * a * a * a / 27.0 - a * b / 3.0 + c) / 2.0;

		const double D = q * q + p * p * p;

		if(D > 0.0)
		{
			const double s = std::sqrt(D);
			return std::cbrt(s - q) - std::cbrt(s + q) - Substitute;
		}
		else if(D == 0.0)
		{
			const double s = std::cbrt(-q);
			const double t = 2.0 * s - Substitute;

			if(0.0 <= t && t <= 1.0001)
				return t;
			return (-s - Substitute);
		}
		else
		{
			const double Phi = std::acos(-q / std::sqrt(-(p * p * p))) / 3.0;
			const double s = 2.0 * std::sqrt(-p);

			const double t1 = s * std::cos(Phi) - Substitute;

			if(0.0 <= t1 && t1 <= 1.0001)
				return t1;

			const double t2 = -s * std::cos(Phi + pi / 3.0) - Substitute;

			if(0.0 <= t2 && t2 <= 1.0001)
				return t2;
			return -s * std::cos(Phi - pi / 3.0) - Substitute;
		}
	}
}

void EvalEnvelope(const IEnvelopePointAccess *pPoints, std::chrono::nanoseconds TimeNanos, ColorRGBA &Result, size_t Channels)
{
	const int NumPoints = pPoints->NumPoints();
	if(NumPoints == 0)
	{
		return;
	}

	if(NumPoints == 1)
	{
		const CEnvPoint *pFirstPoint = pPoints->GetPoint(0);
		for(size_t c = 0; c < Channels; c++)
		{
			Result[c] = fx2f(pFirstPoint->m_aValues[c]);
		}
		return;
	}

	const CEnvPoint *pLastPoint = pPoints->GetPoint(NumPoints - 1);
	const int64_t MaxPointTime = (int64_t)pLastPoint->m_Time.GetInternal() * std::chrono::nanoseconds(1ms).count();
	if(MaxPointTime > 0)
		TimeNanos = std::chrono::nanoseconds(TimeNanos.count() % MaxPointTime);
	else
		TimeNanos = decltype(TimeNanos)::zero();

	const double TimeMillis = TimeNanos.count() / (double)std::chrono::nanoseconds(1ms).count();

	int FoundIndex = pPoints->FindPointIndex(CFixedTime(TimeMillis));
	if(FoundIndex == -1)
	{
		for(size_t c = 0; c < Channels; c++)
		{
			Result[c] = fx2f(pLastPoint->m_aValues[c]);
		}
		return;
	}

	const CEnvPoint *pCurrentPoint = pPoints->GetPoint(FoundIndex);
	const CEnvPoint *pNextPoint = pPoints->GetPoint(FoundIndex + 1);

	const CFixedTime Delta = pNextPoint->m_Time - pCurrentPoint->m_Time;
	if(Delta <= CFixedTime(0))
	{
		for(size_t c = 0; c < Channels; c++)
		{
			Result[c] = fx2f(pCurrentPoint->m_aValues[c]);
		}
		return;
	}

	float a = (float)(TimeMillis - pCurrentPoint->m_Time.GetInternal()) / Delta.GetInternal();

	switch(pCurrentPoint->m_Curvetype)
	{
	case CURVETYPE_STEP:
		a = 0.0f;
		break;

	case CURVETYPE_SLOW:
		a = a * a * a;
		break;

	case CURVETYPE_FAST:
		a = 1.0f - a;
		a = 1.0f - a * a * a;
		break;

	case CURVETYPE_SMOOTH:
		a = -2.0f * a * a * a + 3.0f * a * a;
		break;

	case CURVETYPE_BEZIER:
	{
		const CEnvPointBezier *pCurrentPointBezier = pPoints->GetBezier(FoundIndex);
		const CEnvPointBezier *pNextPointBezier = pPoints->GetBezier(FoundIndex + 1);
		if(pCurrentPointBezier == nullptr || pNextPointBezier == nullptr)
			break;
		for(size_t c = 0; c < Channels; c++)
		{
			const vec2 p0 = vec2(pCurrentPoint->m_Time.GetInternal(), fx2f(pCurrentPoint->m_aValues[c]));
			const vec2 p3 = vec2(pNextPoint->m_Time.GetInternal(), fx2f(pNextPoint->m_aValues[c]));

			const vec2 OutTang = vec2(pCurrentPointBezier->m_aOutTangentDeltaX[c].GetInternal(), fx2f(pCurrentPointBezier->m_aOutTangentDeltaY[c]));
			const vec2 InTang = vec2(pNextPointBezier->m_aInTangentDeltaX[c].GetInternal(), fx2f(pNextPointBezier->m_aInTangentDeltaY[c]));

			vec2 p1 = p0 + OutTang;
			vec2 p2 = p3 + InTang;

			p1.x = std::clamp(p1.x, p0.x, p3.x);
			p2.x = std::clamp(p2.x, p0.x, p3.x);

			a = std::clamp(SolveBezier(TimeMillis, p0.x, p1.x, p2.x, p3.x), 0.0f, 1.0f);

			Result[c] = bezier(p0.y, p1.y, p2.y, p3.y, a);
		}
		return;
	}

	case CURVETYPE_LINEAR: [[fallthrough]];
	default:
		break;
	}

	for(size_t c = 0; c < Channels; c++)
	{
		const float v0 = fx2f(pCurrentPoint->m_aValues[c]);
		const float v1 = fx2f(pNextPoint->m_aValues[c]);
		Result[c] = v0 + (v1 - v0) * a;
	}
}
