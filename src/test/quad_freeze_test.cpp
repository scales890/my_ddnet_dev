#include <gtest/gtest.h>

#include <game/quad_freeze.h>

TEST(QuadFreeze, PointInAxisAlignedQuad)
{
	const vec2 aCorners[4] = {
		vec2(0.0f, 0.0f),
		vec2(32.0f, 0.0f),
		vec2(32.0f, 32.0f),
		vec2(0.0f, 32.0f),
	};

	EXPECT_TRUE(PointInQuad(vec2(16.0f, 16.0f), aCorners));
	EXPECT_FALSE(PointInQuad(vec2(40.0f, 16.0f), aCorners));
}

TEST(QuadFreeze, BoxOverlapsQuad)
{
	const vec2 aCorners[4] = {
		vec2(0.0f, 0.0f),
		vec2(32.0f, 0.0f),
		vec2(32.0f, 32.0f),
		vec2(0.0f, 32.0f),
	};

	EXPECT_TRUE(BoxOverlapsQuad(vec2(16.0f, 16.0f), vec2(14.0f, 14.0f), aCorners));
	EXPECT_FALSE(BoxOverlapsQuad(vec2(64.0f, 64.0f), vec2(14.0f, 14.0f), aCorners));
}

TEST(QuadFreeze, MovingFreezeQuadCandidateRequiresPositionEnvelope)
{
	CQuad Quad = {};
	Quad.m_PosEnv = -1;
	EXPECT_FALSE(IsMovingFreezeQuadCandidate(Quad));

	Quad.m_PosEnv = 0;
	EXPECT_TRUE(IsMovingFreezeQuadCandidate(Quad));
}

TEST(QuadFreeze, EnvelopeTimeFromTick)
{
	using namespace std::chrono_literals;
	const auto Time = EnvelopeTimeFromTick(150, 100, 50, 0.5);
	EXPECT_EQ(Time, 50 * 20ms + 10ms);
}
