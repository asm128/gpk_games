#include "gpk_pool_game_update.h"

static	::gpk::error_t	handleContactBall		(::d1p::SPoolGame & pool, const ::d1p::SArgsBall eventArgs, ::gpk::apobj<::d1p::SEventPool> & outputEvents) { 
	const ::d1p::SArgsBall::SContactBall	& eventData = eventArgs.Event.ContactBall; 
	info_printf("Ball: %i, Ball B : %i.", eventData.BallA, eventData.BallB); 
	::d1p::STurnInfo			& turnInfo				= pool.ActiveTurn();
	if(turnInfo.FirstContact || eventData.BallA) 
		return 0;

	::d1p::SArgsBall				firstContactEventArgs	= eventArgs;
	turnInfo.FirstContact	= eventData.BallB;
	firstContactEventArgs.Event.FirstContact.Ball	= eventData.BallB;
	gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_BALL_EVENT, ::d1p::BALL_EVENT_FirstContact, firstContactEventArgs));	// Report turn end for player

	if(0 == eventArgs.Turn && pool.FirstTurn())
		gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_MATCH_EVENT, ::d1p::MATCH_EVENT_Break, firstContactEventArgs));	// Report turn end for player
	else if(pool.MatchState.Flags.StrippedChosen) {
		const bool					isBallStripped		= eventData.BallB > 8;
		const bool					isTeamStripped		= turnInfo.Team == pool.MatchState.Flags.TeamStripped;
		if(8 == eventData.BallB) {
			if(false == pool.MatchState.PocketedAll(turnInfo.Team)) {
				gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_FOUL, turnInfo.Foul = ::d1p::FOUL_Wrong_ball_first, firstContactEventArgs));	// Report wrong ball
				turnInfo.Continues	= false;
			}
		}
		else if(isBallStripped != isTeamStripped) {
			gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_FOUL, turnInfo.Foul = ::d1p::FOUL_Wrong_ball_first, firstContactEventArgs));	// Report wrong ball
			turnInfo.Continues	= false;
		}
	}
	return 0;
}

static	::gpk::error_t	handleContactPocket		(::d1p::SPoolGame & pool, const ::d1p::SArgsBall eventArgs, ::gpk::apobj<::d1p::SEventPool> & outputEvents) { const ::d1p::SArgsBall::SContactPocket	& eventData = eventArgs.Event.ContactPocket	; (void)pool; (void)eventArgs; (void)outputEvents; info_printf("Ball: %i, Pocket : %i.", eventData.Ball,  eventData.Pocket	); return 0; }
static	::gpk::error_t	handleContactCushion	(::d1p::SPoolGame & pool, const ::d1p::SArgsBall eventArgs, ::gpk::apobj<::d1p::SEventPool> & outputEvents) { const ::d1p::SArgsBall::SContactCushion	& eventData = eventArgs.Event.ContactCushion; (void)pool; (void)eventArgs; (void)outputEvents; info_printf("Ball: %i, Cushion: %i.", eventData.Ball,  eventData.Cushion); return 0; }
static	::gpk::error_t	handleFall				(::d1p::SPoolGame & pool, const ::d1p::SArgsBall eventArgs, ::gpk::apobj<::d1p::SEventPool> & outputEvents) { const ::d1p::SArgsBall::SFall				& eventData = eventArgs.Event.Fall			; (void)pool; (void)eventArgs; (void)outputEvents; info_printf("Ball: %i.", eventData.Ball); return 0; }
static	::gpk::error_t	handleJump				(::d1p::SPoolGame & pool, const ::d1p::SArgsBall eventArgs, ::gpk::apobj<::d1p::SEventPool> & outputEvents) { const ::d1p::SArgsBall::SJump				& eventData = eventArgs.Event.Jump			; (void)pool; (void)eventArgs; (void)outputEvents; info_printf("Ball: %i.", eventData.Ball); return 0; }
static	::gpk::error_t	handleFirstContact		(::d1p::SPoolGame & pool, const ::d1p::SArgsBall eventArgs, ::gpk::apobj<::d1p::SEventPool> & outputEvents) { const ::d1p::SArgsBall::SFirstContact		& eventData = eventArgs.Event.FirstContact	; (void)pool; (void)eventArgs; (void)outputEvents; info_printf("Ball: %i.", eventData.Ball); return 0; }
static	::gpk::error_t	handlePocketed			(::d1p::SPoolGame & pool, const ::d1p::SArgsBall eventArgs, ::gpk::apobj<::d1p::SEventPool> & outputEvents) { const ::d1p::SArgsBall::SPocketed			& eventData = eventArgs.Event.Pocketed		; 
	info_printf("Ball: %i, Pocket : %i.", eventData.Ball,  eventData.Pocket); 
	::d1p::STurnInfo			& turnInfo				= pool.ActiveTurn();
	::d1p::SMatchState				& matchState			= pool.MatchState;
	::d1p::SMatchFlags				& matchFlags			= matchState.Flags;
	::d1p::STeamInfo				& teamInfo				= matchState.TeamInfo;

	switch(eventData.Ball) {
	case 0:
		gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_FOUL, turnInfo.Foul = ::d1p::FOUL_Cue_ball_scratch, {}));	// Report scratch
		turnInfo.Continues		= false;
		matchFlags.NotInHand		= 0;
		break;
	case 8: {
		if(false == pool.MatchState.PocketedAll(turnInfo.Team)) {
			const ::d1p::LOST			reason					= ::d1p::LOST_Eight_ball_pocketed;
			gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_MATCH_EVENT, ::d1p::MATCH_EVENT_Lost, reason));	// Report player won
		}
		else {
			if(turnInfo.PocketedAny()) {
				const ::d1p::LOST			reason					= ::d1p::LOST_Eight_ball_not_on_last_stroke;
				gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_MATCH_EVENT, ::d1p::MATCH_EVENT_Lost, reason));	// Report player won
			}
			else {
				const uint8_t				lastPocket				= turnInfo.Team ? teamInfo.LastPocketTeam1 : teamInfo.LastPocketTeam0;
				if(lastPocket == eventData.Pocket)
					gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_MATCH_EVENT, ::d1p::MATCH_EVENT_Won, {}));	// Report player won
				else {
					const ::d1p::LOST			reason					= ::d1p::LOST_Eight_ball_wrong_pocket;
					gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_MATCH_EVENT, ::d1p::MATCH_EVENT_Lost, reason));	// Report player won
				}
			}
		}
		turnInfo.Continues		= false;
		matchFlags.GameOver		= true;
		break;
	}
	default:
		const bool					isStrippedBall			= eventData.Ball > 8;
		const bool					noFoul					= ::d1p::FOUL_None == turnInfo.Foul;
		if(0 == matchFlags.StrippedChosen) {
			matchFlags.StrippedChosen	= 1;
			const uint8_t				teamStripped			= uint8_t(isStrippedBall ? turnInfo.Team : (turnInfo.Team ? 0 : 1));
			matchFlags.TeamStripped	= teamStripped;
			gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_MATCH_EVENT, ::d1p::MATCH_EVENT_StrippedAssigned, teamStripped));	// Report player won
			turnInfo.Continues		= noFoul;
		}
		else {
			const bool					isStrippedTeam			= matchFlags.TeamStripped == turnInfo.Team;
			const bool					isTeamBall				= isStrippedTeam == isStrippedBall;
			if(isTeamBall && pool.MatchState.PocketedAll(turnInfo.Team)) {
				if(turnInfo.Team)	
					teamInfo.LastPocketTeam1	= eventData.Pocket;
				else					
					teamInfo.LastPocketTeam0	= eventData.Pocket;
			}
			turnInfo.Continues		= noFoul && one_if(turnInfo.Continues || isTeamBall);
		}
		break;
	}
	turnInfo.SetPocketed(eventData.Ball);
	gpk_necs(pool.BallEventHistory.push_back(eventArgs));
	return 0; 
}

static	::gpk::error_t	handleBALL_EVENT		(::d1p::SPoolGame & pool, const ::gpk::SEventView<::d1p::BALL_EVENT> & ballEvent, ::gpk::apobj<::d1p::SEventPool> & outputEvents) { 
	info_printf("%s", ::gpk::get_value_namep(ballEvent.Type));
	const ::d1p::SArgsBall		& argsBall				= *(const ::d1p::SArgsBall*)ballEvent.Data.begin(); 
	switch(ballEvent.Type) { 
	case d1p::BALL_EVENT_ContactBall	: return handleContactBall		(pool, argsBall, outputEvents);
	case d1p::BALL_EVENT_ContactPocket	: return handleContactPocket	(pool, argsBall, outputEvents);
	case d1p::BALL_EVENT_ContactCushion	: return handleContactCushion	(pool, argsBall, outputEvents);
	case d1p::BALL_EVENT_Pocketed		: return handlePocketed			(pool, argsBall, outputEvents);
	case d1p::BALL_EVENT_Fall			: return handleFall				(pool, argsBall, outputEvents);
	case d1p::BALL_EVENT_Jump			: return handleJump				(pool, argsBall, outputEvents);
	case d1p::BALL_EVENT_FirstContact	: return handleFirstContact		(pool, argsBall, outputEvents);
	default:
		gpk_warning_unhandled_event(ballEvent); 
		break;
	} 
	return 0; 
}

static	::gpk::error_t	handleMATCH_EVENT		(::d1p::SPoolGame & pool, const ::gpk::SEventView<::d1p::MATCH_EVENT> & childEvent, ::gpk::apobj<::gpk::SEvent<::d1p::POOL_EVENT>> & outputEvents) { 
	info_printf("%s", ::gpk::get_value_namep(childEvent.Type));
	switch(childEvent.Type) { 
	case d1p::MATCH_EVENT_StrippedAssigned	: break;
	case d1p::MATCH_EVENT_Break				: break;
	case d1p::MATCH_EVENT_MatchEnd			: break;
	case d1p::MATCH_EVENT_MatchStart			: {
		const ::d1p::STurnInfo	newTurn					= {{(uint64_t)::gpk::timeCurrentInMs()}, 0, pool.ActivePlayer(), pool.ActiveTeam()};
		gpk_necs(pool.TurnHistory.push_back(newTurn));
		gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_MATCH_EVENT, ::d1p::MATCH_EVENT_TurnStart, newTurn));
		break;
	}
	case d1p::MATCH_EVENT_TurnStart			: break;
	case d1p::MATCH_EVENT_TurnEnd			: 
		::d1p::debugPrintTurnInfo  (pool.ActiveTurn());
		::d1p::debugPrintMatchState(pool.MatchState);
		gpk_necs(pool.AdvanceTurn(outputEvents)); 
		break;
	case d1p::MATCH_EVENT_Lost				:
	case d1p::MATCH_EVENT_Won				:
		gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_MATCH_EVENT, ::d1p::MATCH_EVENT_MatchEnd, pool.MatchState));	// Report turn end for player
		break;
	default: 
		gpk_warning_unhandled_event(childEvent); 
		break; 
	} 
	return 0; 
}


static	::gpk::error_t	endTurn					(::d1p::SPoolGame & pool, ::gpk::apobj<::d1p::SEventPool> & outputEvents) { // we need to change the state to be inactive
	::d1p::SMatchState			& matchState			= pool.MatchState;
	::d1p::SMatchFlags			& turnState				= matchState.Flags;
	turnState.PhysicsActive	= false;
	{
		::d1p::STurnInfo			& activeTurn			= pool.ActiveTurn();
		activeTurn.Time.Ended	= ::gpk::timeCurrentInMs();
		gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_MATCH_EVENT, ::d1p::MATCH_EVENT_TurnEnd, activeTurn));	// Report turn end for player
	}
	return 1;
}

// Checks if the balls are still moving
static	::gpk::error_t	endTurnIfNeeded			(::d1p::SPoolGame & pool, ::gpk::apobj<::d1p::SEventPool> & outputEvents) {
	::d1p::SMatchState			& matchState			= pool.MatchState;
	bool						playStillActive			= false;
	for(uint32_t iBall = 0; iBall < matchState.CountBalls && false == playStillActive; ++iBall) 
		playStillActive			= playStillActive || pool.Engine.IsPhysicsActive(pool.Entities.Balls[iBall]);

	::d1p::SMatchFlags			& turnState				= matchState.Flags;
	if(playStillActive) { // physics still active, wait before ending the turn
		turnState.PhysicsActive = playStillActive;
		return 0;
	}

	if(turnState.PhysicsActive) 
		return ::endTurn(pool, outputEvents);

	return 0;
}

static	::gpk::error_t	matchStart				(::d1p::SPoolGame & pool, ::gpk::apobj<::d1p::SEventPool> & outputEvents) {
	gpk_necs(::d1p::poolGamePhysicsUpdate(pool, outputEvents, 2));
	outputEvents.clear();

	::d1p::SMatchState		& matchState			= pool.MatchState;
	matchState.TimeStart	= ::gpk::timeCurrentInMs();
	matchState.TotalSeconds	= 0;
	gpk_necs(::gpk::eventEnqueueChild(outputEvents, ::d1p::POOL_EVENT_MATCH_EVENT, ::d1p::MATCH_EVENT_MatchStart, matchState));	// This will likely trigger a TurnStart event.
	return 0;
}

::gpk::error_t			d1p::evaluateTurnProgress(::d1p::SPoolGame & pool, ::gpk::apobj<::d1p::SEventPool> & outputEvents, uint32_t eventOffset) { 
	if(pool.MatchState.Flags.GameOver)
		return 0;

	bool						processEvents;
	if(pool.TurnHistory.size())
		gpk_necs(processEvents = ::endTurnIfNeeded(pool, outputEvents));
	else {	// this means that we need to start the match as the game has just reset and there is no active turn record for the current player.
		gpk_necs(::matchStart(pool, outputEvents));	// it is also a good opportunity to generate events reporting the start of the game.
		processEvents			= true;
	}

	if(false == processEvents && false == pool.MatchState.Flags.PhysicsActive)
		return 0;

	outputEvents.for_each([&pool, &outputEvents](const ::gpk::pobj<::d1p::SEventPool> & _eventToProcess) { 
		const ::d1p::SEventPool		& eventToProcess		= *_eventToProcess;
		info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));
		switch(eventToProcess.Type) {
		case ::d1p::POOL_EVENT_BALL_EVENT : return ::d1p::extractAndHandle<::d1p::BALL_EVENT >(eventToProcess, [&pool, &outputEvents](auto ev){ gpk_necs(::handleBALL_EVENT (pool, ev, outputEvents)); return 0; } ); 
		case ::d1p::POOL_EVENT_MATCH_EVENT: return ::d1p::extractAndHandle<::d1p::MATCH_EVENT>(eventToProcess, [&pool, &outputEvents](auto ev){ gpk_necs(::handleMATCH_EVENT(pool, ev, outputEvents)); return 0; } ); 
		default: 
			gpk_warning_unhandled_event(eventToProcess); 
			return 0;
		}
	}, eventOffset);

	return 0;
}