#include "gpk_color.h"
#include "gpk_array_static.h"
#include "gpk_noise.h"

#ifndef GPK_POOL_GAME_BASE_H_098273498237423
#define GPK_POOL_GAME_BASE_H_098273498237423

namespace d1
{
#pragma pack(push, 1)
	stacxpr	float				SCALE_FOOT_TO_METER	= 1.0f / 3.281f;
	stacxpr	float				SCALE_INCH_TO_METER	= 2.54f * .01f;

	struct SPoolTurnState {
		uint8_t						Active				: 1;
		uint8_t						TeamActive			: 1;
		uint8_t						TeamStripped		: 1;
		uint8_t						StrippedChosen		: 1;
		uint8_t						Paused				: 1;
		uint8_t						End					: 1;
		uint8_t						NotInHand			: 1;
		uint8_t						InHandAnywhere		: 1;
		uint8_t						LastPocketTeam0		: 4;
		uint8_t						LastPocketTeam1		: 4;
	};

	struct STableMetrics {
		::gpk::n2f					Slate				= {  9 * ::d1::SCALE_FOOT_TO_METER, 4.5f * ::d1::SCALE_FOOT_TO_METER};
		::gpk::n2f					PlayingSurface		= {100 * ::d1::SCALE_INCH_TO_METER, 50 * ::d1::SCALE_INCH_TO_METER};
		::gpk::n2f					Cushion				= {100 * ::d1::SCALE_INCH_TO_METER, 50 * ::d1::SCALE_INCH_TO_METER};
		float						Height				= 30 * ::d1::SCALE_INCH_TO_METER;
		float						PocketRadius		= .05715f;	// Pocket Radius == Ball Diameter 
	};

	GDEFINE_ENUM_TYPE (CUSHION_PROFILE, uint8_t);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, K66, 0);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, K55, 1);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, U23, 2);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, Century, 3);
	stacxpr	::gpk::n2f			CUSHION_SIZES[]		= 
		{ {(1 + 1.0f / 8) * ::d1::SCALE_INCH_TO_METER, (1 + 3.0f / 16.f) * ::d1::SCALE_INCH_TO_METER}
		, {(1 + 1.0f / 4) * ::d1::SCALE_INCH_TO_METER, (1 + 5.0f / 16.f) * ::d1::SCALE_INCH_TO_METER}
		, {(0 + 1.0f / 1) * ::d1::SCALE_INCH_TO_METER, (1 + 1.0f / 8.0f) * ::d1::SCALE_INCH_TO_METER}
		, {(0 + 3.0f / 4) * ::d1::SCALE_INCH_TO_METER, (1 + 1.0f / 4.0f) * ::d1::SCALE_INCH_TO_METER}
		};
	
	GDEFINE_ENUM_TYPE(TABLE_SIZE, uint8_t);
	GDEFINE_ENUM_VALUED(TABLE_SIZE, CUSTOM		, 0, "Custom");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 7_00_FOOT	, 1, "7-Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 8_00_FOOT	, 2, "8-Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 8_50_FOOT	, 3, "8.5 Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 9_00_FOOT	, 4, "9-Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 9_50_FOOT	, 5, "9.5 Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 10_0_FOOT	, 6, "10-Foot");
	stacxpr	::d1::STableMetrics	TABLE_SIZES	[]		=
	{	{ {::d1::SCALE_FOOT_TO_METER * 7.0f, ::d1::SCALE_FOOT_TO_METER * 3.5f}		// 7-foot
		, {::d1::SCALE_INCH_TO_METER * 78.f, ::d1::SCALE_INCH_TO_METER * 39.f}
		, ::d1::CUSHION_SIZES[0]
		},
		{ {::d1::SCALE_FOOT_TO_METER * 8.0f, ::d1::SCALE_FOOT_TO_METER * 4.0f}		// 8-foot
		, {::d1::SCALE_INCH_TO_METER * 92.f, ::d1::SCALE_INCH_TO_METER * 46.f}
		, ::d1::CUSHION_SIZES[0]
		},
		{ {::d1::SCALE_FOOT_TO_METER * 8.5f, ::d1::SCALE_FOOT_TO_METER * 4.25f}		// 8.5-foot
		, {::d1::SCALE_INCH_TO_METER * 97.75f, ::d1::SCALE_INCH_TO_METER * 48.875f}
		, ::d1::CUSHION_SIZES[0]
		},
		{ {::d1::SCALE_FOOT_TO_METER *  9.0f, ::d1::SCALE_FOOT_TO_METER * 4.5f}		// 9-foot
		, {::d1::SCALE_INCH_TO_METER * 100.f, ::d1::SCALE_INCH_TO_METER * 50.f}
		, ::d1::CUSHION_SIZES[0]
		},
		{ {::d1::SCALE_FOOT_TO_METER *  9.5f, ::d1::SCALE_FOOT_TO_METER * 4.75f}	// 9.5-foot
		, {::d1::SCALE_INCH_TO_METER * 105.55555f, ::d1::SCALE_INCH_TO_METER * 52.777777f}
		, ::d1::CUSHION_SIZES[0]
		},
		{ {::d1::SCALE_FOOT_TO_METER *  10.f, ::d1::SCALE_FOOT_TO_METER * 5.f}		// 10-foot
		, {::d1::SCALE_INCH_TO_METER * 111.11111f, ::d1::SCALE_INCH_TO_METER * 55.555555f}
		, ::d1::CUSHION_SIZES[0]
		},
	};

	struct SPoolTable {
		STableMetrics				Dimensions			= TABLE_SIZES[1];
		float						BallRadius			= .05715f * .5f;	// meters
		float						BallWeight			= 165;	// grams
	};

	struct SPoolPhysicsDamping {
		float						ClothDisplacement	= .6f;
		float						ClothRotation		= .01f;
		float						Collision			= .975f;
		float						Cushion				= .95f;
		float						Pocket				= .95f;
		float						Ground				= .5f;
	};

	struct SPoolPhysicsState {
		float						Gravity				= 9.8f;	// m/s
		SPoolPhysicsDamping			Damping				= {};
	};

	struct SStickControl {
		::gpk::n2f					Shift				= {};
		float						Angle				= 0;
		float						Pitch				= 0;
		float						Velocity			= 8;
	};

	struct SPoolStick {
		float						Radius				= .013f; //
	};

	stacxpr	uint8_t				MAX_BALLS			= 64;

	stacxpr	::gpk::astatic<::gpk::bgra, ::d1::MAX_BALLS>	BALL_COLORS_8_BALL	= 
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

	inlcxpr	bool				pocketedAll			(const uint64_t pocketed, uint8_t iTeam)	{ return 0xFE == ((pocketed >> (iTeam * 8)) & 0xFE); }
	inlcxpr	bool				pocketedAny			(const uint64_t pocketed, uint8_t iTeam)	{ return ((pocketed >> (iTeam * 8)) & 0xFE); }
	inlcxpr	bool				isPocketed			(const uint64_t pocketed, uint8_t iBall)	{ return pocketed & (1ULL << iBall); }
	inlcxpr	uint64_t			setPocketed			(uint64_t & pocketed, uint8_t iBall)		{ return pocketed |= (1ULL << iBall); }

	struct SPoolMatchState {
		uint64_t					Seed				= (uint64_t)::gpk::timeCurrentInUs();
		uint64_t					TimeStart			= {};
		double						TotalSeconds		= 0;
		uint64_t					Pocketed			= 0;

		POOL_GAME_MODE				Mode				= ::d1::POOL_GAME_MODE_8Ball;
		uint8_t						CountBalls			= ::d1::MAX_BALLS;
		::d1::SPoolTurnState		TurnState			= {};
		::d1::SPoolTable			Table				= {};
		::d1::SPoolPhysicsState		Physics				= {};

		inlcxpr	bool				PocketedAll			(uint8_t iTeam)	const	{ return pocketedAll(Pocketed, iTeam); }
		inlcxpr	bool				PocketedAny			(uint8_t iTeam)	const	{ return pocketedAny(Pocketed, iTeam); }
		inlcxpr	bool				IsPocketed			(uint8_t iBall)	const	{ return isPocketed	(Pocketed, iBall); }
		uint64_t					SetPocketed			(uint8_t iBall)			{ return setPocketed(Pocketed, iBall); }
		::gpk::error_t				Reset				(uint8_t firstTeam = 0, uint64_t seed = ::gpk::timeCurrentInUs() ^ ::gpk::noise1DBase(::gpk::timeCurrentInUs()))	{ 
			Seed						= seed;
			TotalSeconds				= 0;
			TurnState					= {};
			TurnState.TeamActive		= firstTeam;
			TimeStart					= ::gpk::timeCurrentInMs();
			Pocketed					= 0;
			return 0;
		}
	};

	cnstxpr	uint32_t			MATCH_STATE_SIZE	= sizeof(SPoolMatchState);

#pragma pack(pop)

} // namespace

#endif // GPK_POOL_GAME_BASE_H_098273498237423