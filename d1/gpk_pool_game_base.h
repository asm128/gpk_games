//#define GPK_STDOUT_LOG_ENABLED
#include "gpk_color.h"
#include "gpk_array_static.h"
#include "gpk_noise.h"
#include "gpk_n3.h"
#include "gpk_enum.h"
#include "gpk_chrono.h"

#ifndef GPK_POOL_GAME_BASE_H_098273498237423
#define GPK_POOL_GAME_BASE_H_098273498237423

namespace d1p
{
	GPK_USING_TYPEINT();

#pragma pack(push, 1)
	stxp	float		SCALE_FOOT_TO_METER	= 1.0f / 3.281f;
	stxp	float		SCALE_INCH_TO_METER	= 2.54f * .01f;

	struct STeamInfo {
		u0_t 				LastPocketTeam0		: 4;
		u0_t 				LastPocketTeam1		: 4;
	};

	struct SMatchFlags {
		u0_t 				TeamActive			: 1;
		u0_t 				TeamStripped		: 1;
		u0_t 				StrippedChosen		: 1;
		u0_t 				PhysicsActive		: 1;
		u0_t 				GameOver			: 1;
		u0_t 				NotInHand			: 1;
		u0_t 				InHandAnywhere		: 1;
		u0_t 				UnusedBit			: 1;
	};

	struct SPoolTable {
		::gpk::n2f2_t		Slate				= {  9 * ::d1p::SCALE_FOOT_TO_METER, 4.5f * ::d1p::SCALE_FOOT_TO_METER};
		::gpk::n2f2_t		PlayingSurface		= {100 * ::d1p::SCALE_INCH_TO_METER,   50 * ::d1p::SCALE_INCH_TO_METER};
		::gpk::n2f2_t		Cushion				= {100 * ::d1p::SCALE_INCH_TO_METER,   50 * ::d1p::SCALE_INCH_TO_METER};
		float				Height				= 30 * ::d1p::SCALE_INCH_TO_METER;
		float				PocketRadius		= .05715f * 2;	// Pocket Radius == Ball Diameter

		inline	::gpk::n2f2_t	HalfSlate			()	const	noexcept	{ return Slate			* .5f; }
		inline	::gpk::n2f2_t	HalfPlayingSurface	()	const	noexcept	{ return PlayingSurface	* .5f; }
		
		bool					InPlayingSurface	(const ::gpk::n2f2_t & ballPosition)	const	noexcept	{
			const ::gpk::n2f2_t		halfSpace			= HalfPlayingSurface();
			return ballPosition.x > -halfSpace.x && ballPosition.x < halfSpace.x 
				&& ballPosition.y > -halfSpace.y && ballPosition.y < halfSpace.y
				;
		}

	};

	GDEFINE_ENUM_TYPE (CUSHION_PROFILE, u0_t );
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, K66, 0);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, K55, 1);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, U23, 2);
	GDEFINE_ENUM_VALUE(CUSHION_PROFILE, Century, 3);
	stxp	::gpk::n2f2_t	CUSHION_SIZES[]		=
		{ {(1 + 1.0f / 8) * ::d1p::SCALE_INCH_TO_METER, (1 + 3.0f / 16.f) * ::d1p::SCALE_INCH_TO_METER}
		, {(1 + 1.0f / 4) * ::d1p::SCALE_INCH_TO_METER, (1 + 5.0f / 16.f) * ::d1p::SCALE_INCH_TO_METER}
		, {(0 + 1.0f / 1) * ::d1p::SCALE_INCH_TO_METER, (1 + 1.0f / 8.0f) * ::d1p::SCALE_INCH_TO_METER}
		, {(0 + 3.0f / 4) * ::d1p::SCALE_INCH_TO_METER, (1 + 1.0f / 4.0f) * ::d1p::SCALE_INCH_TO_METER}
		};

	GDEFINE_ENUM_TYPE(TABLE_SIZE, u0_t );
	GDEFINE_ENUM_VALUED(TABLE_SIZE, CUSTOM		, 0, "Custom");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 7_00_FOOT	, 1, "7-Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 8_00_FOOT	, 2, "8-Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 8_50_FOOT	, 3, "8.5 Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 9_00_FOOT	, 4, "9-Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 9_50_FOOT	, 5, "9.5 Foot");
	GDEFINE_ENUM_VALUED(TABLE_SIZE, 10_0_FOOT	, 6, "10-Foot");
	stxp	SPoolTable	TABLE_SIZES	[]		=
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
	struct SPoolBoard {
		SPoolTable			Table				= TABLE_SIZES[1];
		float				BallRadius			= .05715f * .5f;	// meters

		float				CornerCut			()	const	{
			const double			ballDiameter		= BallRadius * 2.0;
			const double			pocketOpening		= ballDiameter * 2;
			const double			openingSquared		= pocketOpening * pocketOpening;
			const double			pocketCut			= sqrt(openingSquared * .5);
			return (float)pocketCut;
		}
		float				SideCut				()	const	{
			const double			ballDiameter		= BallRadius * 2.0;
			const double			pocketOpening		= ballDiameter * 2 + ballDiameter * .222222222;
			const double			pocketCut			= pocketOpening * .5;
			return (float)pocketCut;
		}
	};

	// The value between 0 and 1 we need for this gets compressed this way good enough for our purposes.
	struct SPoolDamping {
		u0_t 				ClothDisplacement	= u0_t (0xFFU * .6f);
		u0_t 				ClothRotation		= u0_t (0xFFU * .01f);
		u0_t 				Collision			= u0_t (0xFFU * .975f);
		u0_t 				Cushion				= u0_t (0xFFU * .95f);
		u0_t 				Pocket				= u0_t (0xFFU * .95f);
		u0_t 				Ground				= u0_t (0xFFU * .5f);
	};

	struct SPoolPhysics {
		SPoolDamping		Damping				= {};
		u1_t			Gravity				= 9800;	// mm/s
		u1_t			BallGrams			= 165;	// grams
	};


	sinx	float		rackOriginX			(const ::d1p::SPoolBoard & board)		{ return board.Table.PlayingSurface.x / 4; }
	sinx	float		headStringX			(const ::d1p::SPoolBoard & board)		{ return rackOriginX(board) * -1.0f; }

	stxp	u0_t 		MAX_BALLS			= 64;
	stxp	::gpk::astatic<::gpk::bgra, ::d1p::MAX_BALLS>	BALL_COLORS_8_BALL	=
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

	GDEFINE_ENUM_TYPE(BALL_TYPE, u0_t );
	GDEFINE_ENUM_VALUE(BALL_TYPE, Solid		, 0);
	GDEFINE_ENUM_VALUE(BALL_TYPE, Stripped	, 1);
	GDEFINE_ENUM_VALUE(BALL_TYPE, Cue		, 2);
	GDEFINE_ENUM_VALUE(BALL_TYPE, Black		, 3);
	typedef u1_t	TBallField;
	inxp	u0_t 		pocketedAny			(const TBallField pocketed, BALL_TYPE ballType)	{ return (pocketed >> (ballType * 8)) & 0xFE; }
	inxp	bool		pocketedAll			(const TBallField pocketed, BALL_TYPE ballType)	{ return 0xFE == pocketedAny(pocketed, ballType); }
	inxp	bool		isPocketed			(const TBallField pocketed, u0_t  iBall)		{ return pocketed & (1ULL << iBall); }
	inxp	TBallField	setPocketed			(TBallField & pocketed, u0_t  iBall)			{ return pocketed |= (1ULL << iBall); }
	inxp	BALL_TYPE 	ballToBallType		(u0_t  iBall)									{ 
		return (0 == iBall) ? BALL_TYPE_Cue 
			 : (8 == iBall) ? BALL_TYPE_Black 
			 : (7 >= iBall) ? BALL_TYPE_Solid 
			 : BALL_TYPE_Stripped
		;
	}
	inxp	u0_t 		pocketedCount		(const TBallField pocketed, BALL_TYPE iBallType)	{ 
		u0_t 					balls				= u0_t ((pocketed >> (iBallType * 8)) & 0xFE);
		u0_t 					ballCount			= 0;
		for(u2_t  iBall = 1; iBall < 8; ++iBall) {
			if((1 << iBall) & balls)
				++ballCount;
		}
		return ballCount; 
	}

	GDEFINE_ENUM_TYPE(POOL_GAME_MODE, u0_t );
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 8Ball, 0);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 9Ball, 1);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 10Ball, 2);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, Test2Balls, 3);
	struct SMatchState {
		u3_t 			Seed				= (u3_t )::gpk::timeCurrentInUs();
		u3_t 			TimeStart			= {};
		double				TotalSeconds		= 0;
		TBallField			Pocketed			= 0;
		::d1p::SPoolBoard	Board				= {};
		::d1p::SPoolPhysics	Physics				= {};

		POOL_GAME_MODE		Mode				= ::d1p::POOL_GAME_MODE_8Ball;
		u0_t 				CountBalls			= ::d1p::MAX_BALLS;
		::d1p::SMatchFlags	Flags				= {};
		::d1p::STeamInfo	TeamInfo			= {};

		inline	TBallField	SetPocketed			(u0_t  iBall)					{ return ::d1p::setPocketed	(Pocketed, iBall); }
		inxp	bool		IsPocketed			(u0_t  iBall)			const	{ return ::d1p::isPocketed	(Pocketed, iBall); }
		inxp	bool		BallTypeToTeam		(BALL_TYPE iBallType)	const	{ return (iBallType == ::d1p::BALL_TYPE_Stripped) ? Flags.TeamStripped : one_if(0 == Flags.TeamStripped); }
		inxp	u0_t 		BallToTeam			(u0_t  iBall)			const	{ return BallTypeToTeam(::d1p::ballToBallType(iBall)); }
		inxp	bool		PocketedAll			(u0_t  iTeam)			const	{ return ::d1p::pocketedAll	(Pocketed, TeamToBallType(iTeam)); }
		inline	u0_t 		PocketedCount		(u0_t  iTeam)			const	{ return ::d1p::pocketedCount	(Pocketed, TeamToBallType(iTeam)); }
		inxp	u0_t 		PocketedAny			(u0_t  iTeam)			const	{ return ::d1p::pocketedAny	(Pocketed, TeamToBallType(iTeam)); }
		inxp	BALL_TYPE 	TeamToBallType		(u0_t  iTeam)			const	{ return (iTeam == Flags.TeamStripped) ? ::d1p::BALL_TYPE_Stripped : ::d1p::BALL_TYPE_Solid; }

		::gpk::err_t		Reset				(u0_t  firstTeam = 0, u3_t  seed = ::gpk::timeCurrentInUs() ^ ::gpk::noise1DBase(::gpk::timeCurrentInUs()))	{
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

	cxpr	u2_t 	MATCH_STATE_SIZE	= sizeof(SMatchState);

#pragma pack(pop)

} // namespace

#endif // GPK_POOL_GAME_BASE_H_098273498237423
