#include "gpk_pool_game_event.h"

#include "gpk_engine.h"
#include "gpk_image.h"
#include "gpk_array_static.h"
#include "gpk_astatic_serialize.h"

#ifndef GPK_POOL_GAME_H_098273498237423
#define GPK_POOL_GAME_H_098273498237423

namespace d1p
{
	GPK_USING_TYPEINT();

#pragma pack(push, 1)
	struct SStickControl {
		::gpk::n2f2_t			Shift				= {};
		float					Angle				= 0;
		float					Pitch				= 0;
		float					Velocity			= 2;
	};

	struct SPoolStick {
		float					Radius				= .013f; //
	};

	struct STurnTimestamp {
		uint64_t				Start				= 0;
		uint64_t				Shoot				= 0;
		uint64_t				Ended				= 0;
		double					SecondsAiming		= 0;
		double					SecondsActive		= 0;
	};

	struct STurnInfo {
		STurnTimestamp			Time				= {};
		uint8_t					FirstContact		: 4;
		uint8_t					Player				: 4;
		uint8_t					Team				: 1;
		uint8_t					Continues			: 1;
		uint8_t					Reverse				: 1;
		uint8_t					Foul				: 5;
		uint16_t				Pocketed			= 0;
		::d1p::SStickControl	StickControl		= {};

		inlcxpr	bool			IsPocketed			(uint8_t iBall)	const	{ return ::d1p::isPocketed	(Pocketed, iBall); }
		inline	uint64_t		SetPocketed			(uint8_t iBall)			{ return ::d1p::setPocketed	(Pocketed, iBall); }
	};

	stacxpr	uint8_t			MAX_CUSHIONS		= 6;
	stacxpr	uint8_t			MAX_POCKETS			= 6;
	stacxpr	uint8_t			MAX_PLAYERS			= 6;
	stacxpr	float			MAX_SHOOT_VELOCITY	= 20;	// m/s
	struct SPoolEntityMap {
		::gpk::astu16<MAX_POCKETS>	Pockets			= {};
		::gpk::astu16<MAX_CUSHIONS>	Cushions		= {};
		::gpk::astu16<MAX_PLAYERS>	Sticks			= {};
		::gpk::astu16<MAX_BALLS>	Balls			= {};
		uint16_t					Table			= 0;
	};

	stacxpr	uint8_t			TEAM_COUNT			= 2;
	stacxpr	uint8_t			TEAM_SIZE			= (::d1p::MAX_PLAYERS >> 1);
	struct SPoolTeam {
		uint8_t					PlayerCount			= 0;
		uint8_t					CurrentPlayer		= 0;
		::gpk::astu8<TEAM_SIZE>	Players				= {};
	};

	struct SPoolPlayer {
		::d1p::SStickControl	StickControl		= {};
		::d1p::SPoolStick		StickPhysics		= {};
	};
#pragma pack(pop)
	// Extends the base engine to define pool-specific entity creation functions.
	struct SPoolEngine : public ::gpk::SEngine {
		::gpk::error_t						CreateTableCushion			();
	};

	struct SPoolGame {
		::d1p::SMatchState					MatchState					= {};
		::d1p::SPoolEntityMap				Entities;

		::d1p::SPoolEngine					Engine						= {}; 

		::gpk::apod<::d1p::STurnInfo>		TurnHistory					= {};
		::gpk::apod<::d1p::SArgsBall>		BallEventHistory			= {};
		::gpk::apod<::gpk::SContact>		LastFrameContactsBall		= {};
		::gpk::apod<::gpk::SContact>		LastFrameContactsCushion	= {};

		::gpk::astatic<::d1p::SPoolTeam				, TEAM_COUNT >	Teams			= {};
		::gpk::astatic<::gpk::bgra					, MAX_BALLS  >	BallColors		= BALL_COLORS_8_BALL;
		::gpk::astatic<::gpk::apod<::gpk::line3f32>	, MAX_BALLS  >	PositionDeltas	= {};
		::gpk::astatic<::d1p::SPoolPlayer			, MAX_PLAYERS>	Players			= {};

		::gpk::error_t						Save						(::gpk::au0_t & bytes)	const;
		::gpk::error_t						Load						(::gpk::vcu0_t & bytes);

		::gpk::error_t						AdvanceTurn					(::gpk::apobj<::d1p::SEventPool> & outputEvents);
		inline	::d1p::STurnInfo&			Turn						(const SArgsBall & eventArgs)			{ return TurnHistory[eventArgs.Turn]; }
		inline	const ::d1p::STurnInfo&		Turn						(const SArgsBall & eventArgs)	const	{ return TurnHistory[eventArgs.Turn]; }
		inlcxpr	bool						FirstTurn					()	const	{ return TurnHistory.size() == 1; }
		inline	::d1p::STurnInfo&			ActiveTurn					()			{ return TurnHistory[TurnHistory.size() - 1]; }
		inline	const ::d1p::STurnInfo&		ActiveTurn					()	const	{ return TurnHistory[TurnHistory.size() - 1]; }

		inlcxpr	uint8_t						ActiveTeam					()	const	{ return MatchState.Flags.TeamActive; }
		inline	uint8_t						ActivePlayer				()	const	{ 
			const ::d1p::SPoolTeam					& team						= Teams[ActiveTeam()]; 
			return team.Players[team.CurrentPlayer]; 
		}
		inline	uint16_t					ActiveStickEntity			()	const	{ return Entities.Sticks[ActivePlayer()]; }
		inline	::d1p::SStickControl&		ActiveStick					()			{ return Players[ActivePlayer()].StickControl; }
		inline	const ::d1p::SStickControl&	ActiveStick					()	const	{ return Players[ActivePlayer()].StickControl; }

		inline	uint8_t						EntityToPocket				(uint16_t iEntity)									const	{ return (uint8_t)::gpk::find(iEntity, ::gpk::vcu16{Entities.Pockets}); }
		inline	uint8_t						EntityToBall				(uint16_t iEntity)									const	{ return (uint8_t)::gpk::find(iEntity, ::gpk::vcu16{Entities.Balls  }); }
		inline	::gpk::error_t				BallToEntity				(uint8_t iBall)										const	{ return Entities.Balls[iBall]; }
		inline	::gpk::error_t				BallToRenderNode			(uint8_t iBall)										const	{ return Engine.Entities[Entities.Balls[iBall]].RenderNode; }
		inline	::gpk::error_t				BallToBody					(uint8_t iBall)										const	{ return Engine.Entities[Entities.Balls[iBall]].RigidBody; }

		inline	::gpk::error_t				GetPocketPosition			(uint8_t iPocket, ::gpk::n3f2_t & out_position)		const	{ return Engine.GetPosition(Entities.Pockets[iPocket], out_position); }
		inline	::gpk::error_t				GetBallPosition				(uint8_t iBall  , ::gpk::n3f2_t & out_position)		const	{ return Engine.GetPosition(Entities.Balls[iBall], out_position); }
		inline	::gpk::error_t				GetStickOrientation			(uint8_t iStick , ::gpk::quatf32 & out_orientation)	const	{ return Engine.GetOrientation(Entities.Sticks[iStick], out_orientation); }
		inline	::gpk::error_t				GetStickOrientation			(::gpk::quatf32 & out_orientation)					const	{ return Engine.GetOrientation(Entities.Sticks[ActivePlayer()], out_orientation); }
		inline	::gpk::error_t				SetBallPosition				(uint8_t iBall, const ::gpk::n3f2_t & in_position)			{ return Engine.SetPosition(Entities.Balls[iBall], in_position); }
		inline	::gpk::error_t				SetStickVelocity			(float velocity)											{ return ::gpk::error_t((ActiveStick().Velocity = ::gpk::clamped(velocity, 0.0f, ::d1p::MAX_SHOOT_VELOCITY)) * 1000); }
		inline	::gpk::error_t				AddStickVelocity			(float velocity)											{ 
			::d1p::SStickControl					& activeStick				= ActiveStick(); 
			return ::gpk::error_t((activeStick.Velocity = ::gpk::clamped(activeStick.Velocity + velocity, 0.0f, ::d1p::MAX_SHOOT_VELOCITY)) * 1000);
		}
		::gpk::error_t						ResetCueBall				()			{ 
			::d1p::SPoolBoard						& board						= MatchState.Board;
			const float								distanceFromCenter			= ::d1p::rackOriginX(board);
			Engine.SetOrientation	(BallToEntity(0), {0, 0, 0, 1});
			gpk_necs(SetBallPosition(0, {-distanceFromCenter, board.BallRadius, 0}));
			MatchState.Pocketed					= TBallField(MatchState.Pocketed & -2LL);
			return 0;
		}
	};

	::gpk::error_t						poolGameSave				(const ::d1p::SPoolGame & game, ::gpk::vcsc_t fileName);
	::gpk::error_t						poolGameLoad				(::d1p::SPoolGame & world,::gpk::vcsc_t filename);
	::gpk::error_t						poolGameReset				(::d1p::SPoolGame & pool);
	::gpk::error_t						poolGameReset				(::d1p::SPoolGame & pool, ::d1p::SMatchState & startState);
	::gpk::error_t						poolGameSetup				(::d1p::SPoolGame & pool);
	::gpk::error_t						poolGameUpdate				(::d1p::SPoolGame & pool, ::gpk::view<const ::d1p::SEventPlayer> inputEvents, ::gpk::apobj<::d1p::SEventPool> & outputEvents, double secondsElapsed);
	::gpk::error_t						poolGameDraw				(::d1p::SPoolGame & pool
		, ::gpk::rtbgra8d32		& backBuffer
		, const ::gpk::n3f2_t	& cameraPosition
		, const ::gpk::n3f2_t	& cameraTarget
		, const ::gpk::n3f2_t	& cameraUp
		, double				totalSeconds
		);
} // namespace

#endif // GPK_POOL_GAME_H_098273498237423

// Pool is played on a six pocket table. Modern pool tables generally range in size from 3.5 by 7 feet (1.07 by 2.13 m), to 4.5 by 9 feet (1.37 by 2.74 m).
// Under World Pool-Billiard Association (WPA) official equipment specifications, pool balls have a diameter of 2.25 inches (57.15 mm) and weigh from 5.5 to 6 oz (160 to 170 g) with a diameter of 2.25 inches (57 mm), 
// plus or minus 0.005 inches (0.13 mm). Modern coin-operated pool tables generally use one of three methods to distinguish and return the cue ball to the front of the table while the numbered balls 
// return to an inaccessible receptacle until paid for again: the cue ball is larger and heavier than the other balls, or denser and heavier, or has a magnetic core.
// Modern cue sticks are generally 58.5 inches (148.6 cm) long for pool while cues prior to 1980 were designed for straight pool and had an average length of 57.5 inches (146.1 cm). 
// By comparison, carom billiards cues are generally shorter with larger tips, and snooker cues longer with smaller tips.
//
// Source: https://en.wikipedia.org/wiki/Pool_(cue_sports)

