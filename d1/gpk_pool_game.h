#include "gpk_pool_game_event.h"
#include "gpk_engine.h"
#include "gpk_image.h"
#include "gpk_array_static.h"
#include "gpk_astatic_serialize.h"

#ifndef GPK_POOL_GAME_H_098273498237423
#define GPK_POOL_GAME_H_098273498237423

namespace d1
{
	// Extends the base engine to define pool-specific entity creation functions.
	struct SPoolEngine : public ::gpk::SEngine {
		::gpk::error_t						CreateTableCushion			();
	};

#pragma pack(push, 1)
	struct SPoolTurnTimestamp {
		uint64_t							Start						= 0;
		uint64_t							Shoot						= 0;
		uint64_t							Ended						= 0;
	};

	struct SPoolTurnInfo {
		SPoolTurnTimestamp					Time						= {};
		uint8_t								FirstContact				: 4;
		uint8_t								Player						: 4;
		uint8_t								Team						: 1;
		uint8_t								Continues					: 1;
		uint8_t								Reverse						: 1;
		uint8_t								Foul						: 5;
		uint64_t							Pocketed					= 0;
		::gpk::quatf						Orientation					= {};
		::d1::SStickControl					StickControl				= {};

		inlcxpr	bool						PocketedAny					()				const	{ return pocketedAny(Pocketed, Team); }
		inlcxpr	bool						IsPocketed					(uint8_t iBall)	const	{ return isPocketed	(Pocketed, iBall); }
		inline	uint64_t					SetPocketed					(uint8_t iBall)			{ return setPocketed(Pocketed, iBall); }
	};

	stacxpr	uint8_t						MAX_CUSHIONS				= 6;
	stacxpr	uint8_t						MAX_POCKETS					= 6;
	stacxpr	uint8_t						MAX_PLAYERS					= 6;
	stacxpr	float						MAX_SHOOT_VELOCITY			= 20;	// m/s

	struct SPoolGameEntityMap {
		::gpk::astu16<::d1::MAX_POCKETS>	Pockets						= {};
		::gpk::astu16<::d1::MAX_CUSHIONS>	Cushions					= {};
		::gpk::astu16<::d1::MAX_PLAYERS>	Sticks						= {};
		::gpk::astu16<::d1::MAX_BALLS>		Balls						= {};
		uint16_t							Table						= 0;
	};

	struct SPoolTeam {
		uint8_t								PlayerCount					= 0;
		::gpk::astu8<::d1::MAX_PLAYERS / 2>	Players						= {};
		uint8_t								CurrentPlayer				= 0;
	};

	struct SPoolPlayer {
		::d1::SStickControl					StickControl				= {};
		::d1::SPoolStick					StickPhysics				= {};
	};
#pragma pack(pop)

	struct SPoolGame {
		::d1::SPoolMatchState				MatchState					= {};
		::d1::SPoolGameEntityMap			Entities;

		::gpk::SEngine						Engine						= {}; 

		::gpk::apod<::d1::SPoolTurnInfo>	TurnHistory					= {};
		::gpk::apod<::d1::SArgsBall>		BallEventHistory			= {};
		::gpk::apod<::gpk::SContact>		LastFrameContactsBall		= {};
		::gpk::apod<::gpk::SContact>		LastFrameContactsCushion	= {};

		::gpk::astatic<::d1::SPoolTeam				, 2>					Teams			= {};
		::gpk::astatic<::gpk::bgra					, ::d1::MAX_BALLS	>	BallColors		= BALL_COLORS_8_BALL;
		::gpk::astatic<::gpk::apod<::gpk::line3f32>	, ::d1::MAX_BALLS	>	PositionDeltas	= {};
		::gpk::astatic<::d1::SPoolPlayer			, ::d1::MAX_PLAYERS	>	Players			= {};

		::gpk::error_t						Save						(::gpk::au8 & bytes)	const;
		::gpk::error_t						Load						(::gpk::vcu8 & bytes);

		::gpk::error_t						AdvanceTurn					(::gpk::apobj<::d1::SEventPool> & outputEvents);

		inline	::d1::SPoolTurnInfo&		ActiveTurn					()			{ return TurnHistory[TurnHistory.size() - 1]; }
		inline	::d1::SStickControl&		ActiveStick					()			{ ::d1::SPoolTeam		& team = Teams[ActiveTeam()]; return Players[team.Players[team.CurrentPlayer]].StickControl; }
		inline	const ::d1::SPoolTurnInfo&	ActiveTurn					()	const	{ return TurnHistory[TurnHistory.size() - 1]; }
		inline	const ::d1::SStickControl&	ActiveStick					()	const	{ const ::d1::SPoolTeam	& team = Teams[ActiveTeam()]; return Players[team.Players[team.CurrentPlayer]].StickControl; }
		inlcxpr	bool						FirstTurn					()	const	{ return TurnHistory.size() == 1; }
		inlcxpr	uint8_t						ActiveTeam					()	const	{ return MatchState.TurnState.TeamActive; }
		inline	uint8_t						ActivePlayer				()	const	{ const ::d1::SPoolTeam	& team = Teams[ActiveTeam()]; return team.Players[team.CurrentPlayer]; }
		inline	uint16_t					ActiveStickEntity			()	const	{ return Entities.Sticks[MatchState.TurnState.TeamActive]; }
		inline	uint8_t						EntityToPocket				(uint16_t iEntity)								const	{ return (uint8_t)::gpk::find(iEntity, ::gpk::vcu16{Entities.Pockets}); }
		inline	uint8_t						EntityToBall				(uint16_t iEntity)								const	{ return (uint8_t)::gpk::find(iEntity, ::gpk::vcu16{Entities.Balls  }); }
		inline	::gpk::error_t				BallToEntity				(uint8_t iBall)									const	{ return Entities.Balls[iBall]; }
		inline	::gpk::error_t				BallToRenderNode			(uint8_t iBall)									const	{ return Engine.Entities[Entities.Balls[iBall]].RenderNode; }
		inline	::gpk::error_t				BallToBody					(uint8_t iBall)									const	{ return Engine.Entities[Entities.Balls[iBall]].RigidBody; }
		inline	::gpk::error_t				GetPocketPosition			(uint8_t iPocket, ::gpk::n3f & out_position)	const	{ return Engine.GetPosition(Entities.Pockets[iPocket], out_position); }
		inline	::gpk::error_t				GetBallPosition				(uint8_t iBall  , ::gpk::n3f & out_position)	const	{ return Engine.GetPosition(Entities.Balls  [iBall  ], out_position); }
		inline	::gpk::error_t				GetStickOrientation			(::gpk::quatf32 & out_orientation)				const	{ return Engine.GetOrientation(Entities.Sticks[MatchState.TurnState.TeamActive], out_orientation); }
		inline	::gpk::error_t				SetStickVelocity			(float velocity)										{ return ::gpk::error_t((ActiveStick().Velocity = ::gpk::clamp(velocity, 0.0f, ::d1::MAX_SHOOT_VELOCITY)) * 1000); }
		inline	::gpk::error_t				AddStickVelocity			(float velocity)										{ 
			::d1::SStickControl						& activeStick				= ActiveStick(); 
			return ::gpk::error_t((activeStick.Velocity = ::gpk::clamp(activeStick.Velocity + velocity, 0.0f, ::d1::MAX_SHOOT_VELOCITY)) * 1000);
		}
	};

	::gpk::error_t						poolGameSave				(const ::d1::SPoolGame & game, ::gpk::vcc fileName);
	::gpk::error_t						poolGameLoad				(::d1::SPoolGame & world,::gpk::vcc filename);
	::gpk::error_t						poolGameReset				(::d1::SPoolGame & pool);
	::gpk::error_t						poolGameReset				(::d1::SPoolGame & pool, ::d1::SPoolMatchState & startState);
	::gpk::error_t						poolGameSetup				(::d1::SPoolGame & pool);
	::gpk::error_t						poolGameUpdate				(::d1::SPoolGame & pool, ::gpk::view<const ::d1::SEventPlayer> inputEvents, ::gpk::apobj<::d1::SEventPool> & outputEvents, double secondsElapsed);
	::gpk::error_t						poolGameDraw				(::d1::SPoolGame & pool
		, ::gpk::rt<::gpk::bgra, uint32_t>	& backBuffer
		, const ::gpk::n3f					& cameraPosition
		, const ::gpk::n3f					& cameraTarget
		, const ::gpk::n3f					& cameraUp
		, double							totalSeconds
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

