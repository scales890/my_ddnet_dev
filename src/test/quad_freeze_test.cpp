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

TEST(QuadFreeze, PointInMapStorageOrderQuad)
{
	// CQuad stores corners as 0,1,2,3 where 2 and 3 are opposite edges.
	// Connecting 0-1-2-3 forms a bow-tie; the visual/collision polygon is 0-1-3-2.
	const vec2 aBowTie[4] = {
		vec2(0.0f, 0.0f),
		vec2(32.0f, 0.0f),
		vec2(0.0f, 32.0f),
		vec2(32.0f, 32.0f),
	};
	const vec2 aConvex[4] = {
		vec2(0.0f, 0.0f),
		vec2(32.0f, 0.0f),
		vec2(32.0f, 32.0f),
		vec2(0.0f, 32.0f),
	};

	EXPECT_FALSE(PointInQuad(vec2(16.0f, 16.0f), aBowTie));
	EXPECT_TRUE(PointInQuad(vec2(16.0f, 16.0f), aConvex));
}

TEST(QuadFreeze, MovingKogQuadLayerTypeFromName)
{
	EXPECT_EQ(MovingKogQuadLayerTypeFromName(""), EMovingKogQuadLayerType::NONE);
	EXPECT_EQ(MovingKogQuadLayerTypeFromName("Quads"), EMovingKogQuadLayerType::NONE);
	EXPECT_EQ(MovingKogQuadLayerTypeFromName(KOG_QUAD_LAYER_FREEZE), EMovingKogQuadLayerType::FREEZE);
	EXPECT_EQ(MovingKogQuadLayerTypeFromName(KOG_QUAD_LAYER_UNFREEZE), EMovingKogQuadLayerType::UNFREEZE);
}

TEST(QuadFreeze, MovingKogQuadCandidateRequiresLayerAndPositionEnvelope)
{
	CQuad Quad = {};
	EXPECT_FALSE(IsMovingKogQuadCandidate(Quad, EMovingKogQuadLayerType::NONE));
	EXPECT_FALSE(IsMovingKogQuadCandidate(Quad, EMovingKogQuadLayerType::FREEZE));

	Quad.m_PosEnv = 0;
	EXPECT_TRUE(IsMovingKogQuadCandidate(Quad, EMovingKogQuadLayerType::FREEZE));
	EXPECT_TRUE(IsMovingKogQuadCandidate(Quad, EMovingKogQuadLayerType::UNFREEZE));
}

TEST(QuadFreeze, EnvelopeTimeFromTick)
{
	using namespace std::chrono_literals;
	const auto Time = EnvelopeTimeFromTick(150, 100, 50, 0.5);
	EXPECT_EQ(Time, 50 * 20ms + 10ms);
}

TEST(QuadFreeze, EnvelopeTimeFromTickUsesMapAnchor)
{
	using namespace std::chrono_literals;
	const auto Time = EnvelopeTimeFromTick(250, 100, 50);
	EXPECT_EQ(Time, 150 * 20ms);
}
