#include "gpk_color.h"
#include "gpk_array_static.h"
#include "gpk_noise.h"

#ifndef GPK_POOL_GAME_BASE_H_098273498237423
#define GPK_POOL_GAME_BASE_H_098273498237423

namespace d1p
{
#pragma pack(push, 1)
	stacxpr	float		SCALE_FOOT_TO_METER	= 1.0f / 3.281f;
	stacxpr	float		SCALE_INCH_TO_METER	= 2.54f * .01f;

	struct STeamInfo {
		uint8_t				LastPocketTeam0		: 4;
		uint8_t				LastPocketTeam1		: 4;
	};

	struct SMatchFlags {
		uint8_t				TeamActive			: 1;
		uint8_t				TeamStripped		: 1;
		uint8_t				StrippedChosen		: 1;
		uint8_t				PhysicsActive		: 1;
		uint8_t				GameOver			: 1;
		uint8_t				NotInHand			: 1;
		uint8_t				InHandAnywhere		: 1;
		uint8_t				UnusedBit			: 1;
	};

	struct STableMetrics {
		::gpk::n2f			Slate				= {  9 * ::d1p::SCALE_FOOT_TO_METER, 4.5f * ::d1p::SCALE_FOOT_TO_METER};
		::gpk::n2f			PlayingSurface		= {100 * ::d1p::SCALE_INCH_TO_METER,   50 * ::d1p::SCALE_INCH_TO_METER};
		::gpk::n2f			Cushion				= {100 * ::d1p::SCALE_INCH_TO_METER,   50 * ::d1p::SCALE_INCH_TO_METER};
		float				Height				= 30 * ::d1p::SCALE_INCH_TO_METER;
		float				PocketRadius		= .05715f;	// Pocket Radius == Ball Diameter
	};

	GDEFINE_ENUM_TYPE (CUSHION_PROFILE, uint8_t);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, K66, 0);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, K55, 1);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, U23, 2);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, Century, 3);
	stacxpr	::gpk::n2f			CUSHION_SIZES[]		=
		{ {(1 + 1.0f / 8) * ::d1p::SCALE_INCH_TO_METER, (1 + 3.0f / 16.f) * ::d1p::SCALE_INCH_TO_METER}
		, {(1 + 1.0f / 4) * ::d1p::SCALE_INCH_TO_METER, (1 + 5.0f / 16.f) * ::d1p::SCALE_INCH_TO_METER}
		, {(0 + 1.0f / 1) * ::d1p::SCALE_INCH_TO_METER, (1 + 1.0f / 8.0f) * ::d1p::SCALE_INCH_TO_METER}
		, {(0 + 3.0f / 4) * ::d1p::SCALE_INCH_TO_METER, (1 + 1.0f / 4.0f) * ::d1p::SCALE_INCH_TO_METER}
		};

	GDEFINE_ENUM_TYPE(TABLE_SIZE, uint8_t);
	GDEFINE_ENUM_VALUED(TABLE_SIZE, CUSTOM		, 0, "Custom");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 7_00_FOOT	, 1, "7-Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 8_00_FOOT	, 2, "8-Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 8_50_FOOT	, 3, "8.5 Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 9_00_FOOT	, 4, "9-Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 9_50_FOOT	, 5, "9.5 Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 10_0_FOOT	, 6, "10-Foot");
	stacxpr	::d1p::STableMetrics	TABLE_SIZES	[]		=
	{	{ {::d1p::SCALE_FOOT_TO_METER * 7.0f, ::d1p::SCALE_FOOT_TO_METER * 3.5f}		// 7-foot
		, {::d1p::SCALE_INCH_TO_METER * 78.f, ::d1p::SCALE_INCH_TO_METER * 39.f}
		, ::d1p::CUSHION_SIZES[0]
		},
		{ {::d1p::SCALE_FOOT_TO_METER * 8.0f, ::d1p::SCALE_FOOT_TO_METER * 4.0f}		// 8-foot
		, {::d1p::SCALE_INCH_TO_METER * 92.f, ::d1p::SCALE_INCH_TO_METER * 46.f}
		, ::d1p::CUSHION_SIZES[0]
		},
		{ {::d1p::SCALE_FOOT_TO_METER * 8.5f, ::d1p::SCALE_FOOT_TO_METER * 4.25f}		// 8.5-foot
		, {::d1p::SCALE_INCH_TO_METER * 97.75f, ::d1p::SCALE_INCH_TO_METER * 48.875f}
		, ::d1p::CUSHION_SIZES[0]
		},
		{ {::d1p::SCALE_FOOT_TO_METER *  9.0f, ::d1p::SCALE_FOOT_TO_METER * 4.5f}		// 9-foot
		, {::d1p::SCALE_INCH_TO_METER * 100.f, ::d1p::SCALE_INCH_TO_METER * 50.f}
		, ::d1p::CUSHION_SIZES[0]
		},
		{ {::d1p::SCALE_FOOT_TO_METER *  9.5f, ::d1p::SCALE_FOOT_TO_METER * 4.75f}	// 9.5-foot
		, {::d1p::SCALE_INCH_TO_METER * 105.55555f, ::d1p::SCALE_INCH_TO_METER * 52.777777f}
		, ::d1p::CUSHION_SIZES[0]
		},
		{ {::d1p::SCALE_FOOT_TO_METER *  10.f, ::d1p::SCALE_FOOT_TO_METER * 5.f}		// 10-foot
		, {::d1p::SCALE_INCH_TO_METER * 111.11111f, ::d1p::SCALE_INCH_TO_METER * 55.555555f}
		, ::d1p::CUSHION_SIZES[0]
		},
	};

	struct SPoolTable {
		STableMetrics		Dimensions			= TABLE_SIZES[1];
		float				BallRadius			= .05715f * .5f;	// meters
		uint16_t			BallWeight			= 165;	// grams
	};

	struct SPoolDamping {
		uint8_t				ClothDisplacement	= uint8_t(0xFFU * .6f);
		uint8_t				ClothRotation		= uint8_t(0xFFU * .01f);
		uint8_t				Collision			= uint8_t(0xFFU * .975f);
		uint8_t				Cushion				= uint8_t(0xFFU * .95f);
		uint8_t				Pocket				= uint8_t(0xFFU * .95f);
		uint8_t				Ground				= uint8_t(0xFFU * .5f);
	};

	struct SPoolPhysics {
		SPoolDamping		Damping				= {};
		uint16_t			Gravity				= 9800;	// mm/s
	};

	stacxpr	uint8_t		MAX_BALLS			= 32;

	stacxpr	::gpk::astatic<::gpk::bgra, ::d1p::MAX_BALLS>	BALL_COLORS_8_BALL	=
		{ ::gpk::WHITE
		, ::gpk::YELLOW
		, ::gpk::BLUE
		, ::gpk::RED
		, ::gpk::PURPLE
		, ::gpk::ORANGE
		, ::gpk::GREEN
		, ::gpk::DARKRED * .5f
		, ::gpk::BLACK
		, ::gpk::YELLOW
		, ::gpk::BLUE
		, ::gpk::RED
		, ::gpk::PURPLE
		, ::gpk::ORANGE
		, ::gpk::GREEN
		, ::gpk::DARKRED * .5f
		};

	GDEFINE_ENUM_TYPE(POOL_GAME_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 8Ball, 0);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 9Ball, 1);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 10Ball, 2);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, Test2Balls, 3);

	inlcxpr	bool		pocketedAll			(const uint64_t pocketed, uint8_t iTeam)	{ return 0xFE == ((pocketed >> (iTeam * 8)) & 0xFE); }
	inlcxpr	bool		pocketedAny			(const uint64_t pocketed, uint8_t iTeam)	{ return ((pocketed >> (iTeam * 8)) & 0xFE); }
	inlcxpr	bool		isPocketed			(const uint64_t pocketed, uint8_t iBall)	{ return pocketed & (1ULL << iBall); }
	inlcxpr	uint64_t	setPocketed			(uint32_t & pocketed, uint8_t iBall)		{ return pocketed |= (1ULL << iBall); }

	struct SMatchState {
		uint64_t			Seed				= (uint64_t)::gpk::timeCurrentInUs();
		uint64_t			TimeStart			= {};
		double				TotalSeconds		= 0;
		::d1p::SPoolTable	Table				= {};
		::d1p::SPoolPhysics	Physics				= {};
		uint32_t			Pocketed			= 0;

		POOL_GAME_MODE		Mode				= ::d1p::POOL_GAME_MODE_8Ball;
		uint8_t				CountBalls			= ::d1p::MAX_BALLS;
		::d1p::SMatchFlags	Flags				= {};
		::d1p::STeamInfo	TeamInfo			= {};

		inlcxpr	bool		PocketedAll			(uint8_t iTeam)	const	{ return pocketedAll(Pocketed, iTeam); }
		inlcxpr	bool		PocketedAny			(uint8_t iTeam)	const	{ return pocketedAny(Pocketed, iTeam); }
		inlcxpr	bool		IsPocketed			(uint8_t iBall)	const	{ return isPocketed	(Pocketed, iBall); }
		uint64_t			SetPocketed			(uint8_t iBall)			{ return setPocketed(Pocketed, iBall); }
		::gpk::error_t		Reset				(uint8_t firstTeam = 0, uint64_t seed = ::gpk::timeCurrentInUs() ^ ::gpk::noise1DBase(::gpk::timeCurrentInUs()))	{
			Seed				= seed;
			TotalSeconds		= 0;
			TeamInfo			= {};
			Flags				= {};
			Flags.TeamActive	= firstTeam;
			TimeStart			= ::gpk::timeCurrentInMs();
			Pocketed			= 0;
			return 0;
		}
	};

	cnstxpr	uint32_t		MATCH_STATE_SIZE	= sizeof(SMatchState);

#pragma pack(pop)

} // namespace

#endif // GPK_POOL_GAME_BASE_H_098273498237423
