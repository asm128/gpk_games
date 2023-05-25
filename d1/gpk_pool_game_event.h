#include "gpk_pool_game_base.h"

#include "gpk_event.h"
#include "gpk_axis.h"

#ifndef GPK_POOL_GAME_EVENT_H_098273498237423
#define GPK_POOL_GAME_EVENT_H_098273498237423

namespace d1
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(BALL_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(BALL_EVENT, ContactBall		, 0);
	GDEFINE_ENUM_VALUE(BALL_EVENT, ContactPocket	, 1);
	GDEFINE_ENUM_VALUE(BALL_EVENT, ContactCushion	, 2);
	GDEFINE_ENUM_VALUE(BALL_EVENT, Pocketed			, 3);
	GDEFINE_ENUM_VALUE(BALL_EVENT, Fall				, 4);
	GDEFINE_ENUM_VALUE(BALL_EVENT, Jump				, 5);
	GDEFINE_ENUM_VALUE(BALL_EVENT, FirstContact		, 6);

	typedef ::gpk::SEvent<BALL_EVENT>		SEventBall;
	
	struct SArgsBall {
		struct SContactBall		{ uint8_t BallA = 0; uint8_t BallB		= 0; };
		struct SContactPocket	{ uint8_t Ball  = 0; uint8_t Pocket		= 0; };
		struct SContactCushion	{ uint8_t Ball  = 0; uint8_t Cushion	= 0; };
		struct SPocketed		{ uint8_t Ball  = 0; uint8_t Pocket		= 0; };
		struct SJump			{ uint8_t Ball  = 0; };
		struct SFall			{ uint8_t Ball  = 0; };
		struct SFirstContact	{ uint8_t Ball  = 0; };

		double			Seconds				= 0;
		uint16_t		Turn				;
		//uint16_t		Type				;	// cast to BALL_EVENT
		union UEvent {
			SContactBall	ContactBall		;
			SContactPocket	ContactPocket	;
			SContactCushion	ContactCushion	;
			SPocketed		Pocketed		;
			SJump			Jump			;
			SFall			Fall			;
			SFirstContact	FirstContact	;
		}				Event;
	};

	GDEFINE_ENUM_TYPE(PLAYER_INPUT, uint8_t);
	GDEFINE_ENUM_VALUE(PLAYER_INPUT, Shoot		, 0);	
	GDEFINE_ENUM_VALUE(PLAYER_INPUT, Move		, 1);	
	GDEFINE_ENUM_VALUE(PLAYER_INPUT, Turn		, 2);	
	GDEFINE_ENUM_VALUE(PLAYER_INPUT, Force		, 3);	
	GDEFINE_ENUM_VALUE(PLAYER_INPUT, Ball		, 4);	
	typedef	::gpk::SEvent<PLAYER_INPUT>		SEventPlayer;

	struct SArgsStickMove	{ float Value = 0; ::gpk::AXIS Direction = ::gpk::AXIS_ORIGIN; }; // I put the members in this order because of floating point variable alignment
	struct SArgsStickTurn	{ float Value = 0; ::gpk::AXIS Direction = ::gpk::AXIS_ORIGIN; }; // I put the members in this order because of floating point variable alignment
	struct SArgsStickForce	{ float Value = 0; ::gpk::AXIS Direction = ::gpk::AXIS_ORIGIN; }; // I put the members in this order because of floating point variable alignment

	GDEFINE_ENUM_TYPE(MATCH_CONTROL, uint8_t);
	GDEFINE_ENUM_VALUE(MATCH_CONTROL, Start, 0); // SEvent::Data contains a ::d1::SPoolMatchState structure containing the start state of the match.
	typedef	::gpk::SEvent<MATCH_CONTROL>	SEventMatchControl;

	// 20. LOSS OF GAME.
	// A player loses the game if he commits any of the following infractions:
	// a. Fouls when pocketing the 8-ball (exception: see 8-Ball Pocketed On The Break).
	// b. Pockets the 8-ball on the same stroke as the last of his group of balls.
	// c. Jumps the 8-ball off the table at any time.
	// d. Pockets the 8-ball in a pocket other than the one designated.
	// e. Pockets the 8-ball when it is not the legal object ball.
	// Note: All infractions must be called before another shot is taken, or else it will be deemed that no infraction occurred. 
	GDEFINE_ENUM_TYPE(LOST, uint8_t);
	GDEFINE_ENUM_VALUE(LOST, Eight_ball_foul				, 0);	// a. Fouls when pocketing the 8-ball (exception: see 8-Ball Pocketed On The Break).
	GDEFINE_ENUM_VALUE(LOST, Eight_ball_not_on_last_stroke	, 1);	// b. Pockets the 8-ball on the same stroke as the last of his group of balls.
	GDEFINE_ENUM_VALUE(LOST, Eight_ball_off_the_table		, 2);	// c. Jumps the 8-ball off the table at any time.
	GDEFINE_ENUM_VALUE(LOST, Eight_ball_wrong_pocket		, 3);	// d. Pockets the 8-ball in a pocket other than the one designated.
	GDEFINE_ENUM_VALUE(LOST, Eight_ball_pocketed			, 4);	// e. Pockets the 8-ball when it is not the legal object ball.

	GDEFINE_ENUM_TYPE(MATCH_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(MATCH_EVENT, TurnEnd			, 0); // Arg is the player number
	GDEFINE_ENUM_VALUE(MATCH_EVENT, TurnStart		, 1); // Arg is the player number
	GDEFINE_ENUM_VALUE(MATCH_EVENT, MatchEnd		, 2); // 
	GDEFINE_ENUM_VALUE(MATCH_EVENT, MatchStart		, 3); // 
	GDEFINE_ENUM_VALUE(MATCH_EVENT, Break			, 4); // 
	GDEFINE_ENUM_VALUE(MATCH_EVENT, Lost			, 5); // 
	GDEFINE_ENUM_VALUE(MATCH_EVENT, Won				, 6); // 
	GDEFINE_ENUM_VALUE(MATCH_EVENT, StrippedAssigned, 7);
	typedef	::gpk::SEvent<MATCH_EVENT>		SEventMatchEvent;

	struct SArgsMatchEventLost {
		LOST			Reason			= LOST_Eight_ball_foul;
	};

	// Unsportsmanlike conduct is any intentional behavior that brings disrepute to the sport or which disrupts or changes the game to the extent that it cannot be played fairly. It includes
	// (a) distracting the opponent;
	// (b) changing the position of the balls in play other than by a shot;
	// (c) playing a shot by intentionally miscuing;
	// (d) continuing to play after a foul has been called or play has been suspended;
	// (e) practicing during a match;
	// (f) marking the table;
	// (g) delay of the game; and
	// (h) using equipment inappropriately.
	GDEFINE_ENUM_TYPE(FOUL, uint8_t);
	GDEFINE_ENUM_VALUE(FOUL, None									,  0);	// 
	GDEFINE_ENUM_VALUE(FOUL, Cue_ball_scratch						,  1);	// If the cue ball is pocketed or driven off the table, the shot is a foul.
	GDEFINE_ENUM_VALUE(FOUL, Cue_ball_off_the_table					,  2);	// If the cue ball is pocketed or driven off the table, the shot is a foul.
	GDEFINE_ENUM_VALUE(FOUL, Wrong_ball_first						,  3);	// In those games which require the first object ball struck to be a particular ball or one of a group of balls, it is a foul for the cue ball to first contact any other ball.
	GDEFINE_ENUM_VALUE(FOUL, No_rail_after_contact					,  4);	// If no ball is pocketed on a shot, the cue ball must contact an object ball, and after that contact at least one ball (cue ball or any object ball) must be driven to a rail, or the shot is a foul. (See 8.4 Driven to a Rail.)
	GDEFINE_ENUM_VALUE(FOUL, No_foot_on_floor						,  5);	// If the shooter does not have at least one foot touching the floor at the instant the tip contacts the cue ball, the shot is a foul.
	GDEFINE_ENUM_VALUE(FOUL, Ball_driven_off_the_table				,  6);	// It is a foul to drive an object ball off the table. Whether that ball is spotted depends on the rules of the game. (See 8.5 Driven off the Table.)
	GDEFINE_ENUM_VALUE(FOUL, Touched_ball							,  7);	// It is a foul to touch, move or change the path of any object ball except by the normal ball-to-ball contacts during shots. It is a foul to touch, move or change the path of the cue ball except when it is in hand or by the normal tip-to-ball forward stroke contact of a shot. The shooter is responsible for the equipment he controls at the table, such as chalk, bridges, clothing, his hair, parts of his body, and the cue ball when it is in hand, that may be involved in such fouls. If such a foul is accidental, it is a standard foul, but if it is intentional, it is 6.16 Unsportsmanlike Conduct.
	GDEFINE_ENUM_VALUE(FOUL, Double_hit_or_frozen_balls				,  8);	// 
	GDEFINE_ENUM_VALUE(FOUL, Push_shot								,  9);	// It is a foul to prolong tip-to-cue-ball contact beyond that seen in normal shots.
	GDEFINE_ENUM_VALUE(FOUL, Balls_still_moving						, 10);	// It is a foul to begin a shot while any ball in play is moving or spinning.
	GDEFINE_ENUM_VALUE(FOUL, Bad_cue_ball_placement					, 11);	// When the cue ball is in hand and restricted to the area behind the head string, it is a foul to play the cue ball from on or below the head string. If the shooter is uncertain whether the cue ball has been placed behind the head string, he may ask the referee for a determination.
	GDEFINE_ENUM_VALUE(FOUL, Bad_play_from_behind_the_head_string	, 12);	// When the cue ball is in hand behind the head string, and the first ball the cue ball contacts is also behind the head string, the shot is a foul unless the cue ball crosses the head string before that contact. If such a shot is intentional, it is unsportsmanlike conduct. The cue ball must either cross the head string or contact a ball in front of or on the head string or the shot is a foul, and the cue ball is in hand for the following player according to the rules of the specific game.
	GDEFINE_ENUM_VALUE(FOUL, Cue_stick_on_the_table					, 13);	// If the shooter uses his cue stick in order to align a shot by placing it on the table without having a hand on the stick, it is a foul.
	GDEFINE_ENUM_VALUE(FOUL, Playing_out_of_turn					, 14);	// It is a standard foul to unintentionally play out of turn. Normally, the balls will be played from the position left by the mistaken play. If a player intentionally plays out of turn, it should be treated like 6.16 Unsportsmanlike Conduct.
	GDEFINE_ENUM_VALUE(FOUL, Three_consecutive_fouls				, 15);	// If a player fouls three times without making an intervening legal shot, it is a serious foul. In games scored by the rack, such as nine ball, the fouls must be in a single rack. Some games such as eight ball do not include this rule. The referee must warn a shooter who is on two fouls when he comes to the table that he is on two fouls. Otherwise a possible third foul will be considered to be only the second.
	GDEFINE_ENUM_VALUE(FOUL, Slow_play								, 16);	// If the referee feels that a player is playing too slowly, he may advise that player to speed up his play. If the player does not speed up, the referee may impose a shot clock on that match that applies to both players. If the shooter exceeds the time limit specified for the tournament, a standard foul will be called and the incoming player is rewarded according to the rules applicable to the game being played. (Rule 6.16 Unsportsmanlike Conduct may also apply.)
	GDEFINE_ENUM_VALUE(FOUL, Unsportsmanlike_conduct				, 17);	// The normal penalty for unsportsmanlike conduct is the same as for a serious foul, but the referee may impose a penalty depending on his judgment of the conduct. Among other penalties possible are a warning; a standard-foul penalty, which will count as part of a three-foul sequence if applicable; a serious-foul penalty; loss of a rack, set or match; ejection from the competition possibly with forfeiture of all prizes, trophies and standings points.
	GDEFINE_ENUM_VALUE(FOUL, Eight_ball_scratch						, 18);	// The normal penalty for unsportsmanlike conduct is the same as for a serious foul, but the referee may impose a penalty depending on his judgment of the conduct. Among other penalties possible are a warning; a standard-foul penalty, which will count as part of a three-foul sequence if applicable; a serious-foul penalty; loss of a rack, set or match; ejection from the competition possibly with forfeiture of all prizes, trophies and standings points.
	typedef	::gpk::SEvent<FOUL>				SEventFoul;

	struct SArgsFoul {
		double			Seconds			= 0;
		uint16_t		BallA			= 0; 
		uint16_t		BallB			= 0;
		uint16_t		Pocket			= 0;
		uint16_t		Cushion			= 0;
	};

	GDEFINE_ENUM_TYPE (POOL_EVENT, uint8_t);
	GDEFINE_ENUM_VALUE(POOL_EVENT, PLAYER_INPUT	, 0);	// SEvent<>::Data contains a serialized SEvent<PLAYER_INPUT>   structure that requires to be loaded with SEvent<PLAYER_INPUT>  ::Load().
	GDEFINE_ENUM_VALUE(POOL_EVENT, MATCH_CONTROL, 1);	// SEvent<>::Data contains a serialized SEvent<MATCH_CONTROL> structure that requires to be loaded with SEvent<MATCH_CONTROL>::Load().
	GDEFINE_ENUM_VALUE(POOL_EVENT, MATCH_EVENT	, 2);	// SEvent<>::Data contains a serialized SEvent<MATCH_EVENT>   structure that requires to be loaded with SEvent<MATCH_EVENT>  ::Load().
	GDEFINE_ENUM_VALUE(POOL_EVENT, BALL_EVENT	, 3);	// SEvent<>::Data contains a serialized SEvent<BALL_EVENT>    structure that requires to be loaded with SEvent<BALL_EVENT>   ::Load().
	GDEFINE_ENUM_VALUE(POOL_EVENT, FOUL			, 4);	// SEvent<>::Data contains a serialized SEvent<FOUL>          structure that requires to be loaded with SEvent<FOUL>         ::Load().
	typedef	::gpk::SEvent<POOL_EVENT>		SEventPool;
#pragma pack(pop)

	template<typename _tChildEvent>
	stainli	::gpk::error_t		extractAndHandle		(const ::gpk::SEvent<::d1::POOL_EVENT> & parentEvent, const ::std::function<::gpk::error_t (const ::gpk::SEventView<_tChildEvent> &)> & funcHandleChild) {
		return ::gpk::extractAndHandle<::d1::POOL_EVENT, _tChildEvent>(parentEvent, funcHandleChild);
	}

} // namespace

// Event layers work as follows:
// Pool Game Event
//  |--- Stick input: Controls stick movement/angle, shoot speed and hitting the cue ball.
//  |--- Match control: Used to start the match.
//  |--- Ball events: Reports ball events during active play.


#endif // GPK_POOL_GAME_EVENT_H_098273498237423