#include "gpk_pool_game_update.h"

::gpk::error_t		d1::SPoolGame::AdvanceTurn	(::gpk::apobj<::d1::SEventPool> & outputEvents) { 
	::d1::SPoolTurnState	& turnState					= MatchState.TurnState;
	if(turnState.End)
		return 0;

	if(ActiveTurn().Continues) 
		ActiveTurn().Continues	= false;
	else {
		++Teams[ActiveTeam()].CurrentPlayer;
		++turnState.TeamActive;
		Teams[ActiveTeam()].CurrentPlayer %= Teams[ActiveTeam()].PlayerCount;
		gpk_necs(Engine.SetHidden(ActiveStickEntity(), false));
	}
	::d1::SPoolTurnInfo							newTurn						= {{(uint64_t)::gpk::timeCurrentInMs()}, 0, ActivePlayer(), ActiveTeam()};
	gpk_necs(TurnHistory.push_back(newTurn));
	return ::gpk::eventEnqueueChild(outputEvents, ::d1::POOL_EVENT_MATCH_EVENT, ::d1::MATCH_EVENT_TurnStart, newTurn);	// Report turn start for player
}

::gpk::error_t		d1::SPoolGame::Save						(::gpk::au8 & bytes)	const	{
	gpk_necs(::gpk::savePOD (bytes, MatchState		));
	gpk_necs(::gpk::saveView(bytes, ::gpk::view<const ::d1::SPoolTeam	>{Teams		}));
	gpk_necs(::gpk::saveView(bytes, ::gpk::view<const ::gpk::bgra		>{BallColors}));
	gpk_necs(::gpk::savePOD (bytes, Entities		));
	gpk_necs(::gpk::saveView(bytes, TurnHistory		));
	gpk_necs(::gpk::saveView(bytes, BallEventHistory));
	gpk_necs(Engine.Save(bytes));
	return 0;
}

::gpk::error_t		d1::SPoolGame::Load						(::gpk::vcu8 & bytes)			{
	gpk_necs(::gpk::loadPOD (bytes, MatchState		));
	gpk_necs(::gpk::loadView(bytes, Teams			));
	gpk_necs(::gpk::loadView(bytes, BallColors		));
	gpk_necs(::gpk::loadPOD (bytes, Entities		));
	gpk_necs(::gpk::loadView(bytes, TurnHistory		));
	gpk_necs(::gpk::loadView(bytes, BallEventHistory));
	gpk_necs(Engine.Load(bytes));
	return 0;
}


::gpk::error_t		d1::poolGameUpdate		(::d1::SPoolGame & pool, ::gpk::view<const ::d1::SEventPlayer> inputEvents, ::gpk::apobj<::d1::SEventPool> & outputEvents, double secondsElapsed) {
	pool.LastFrameContactsBall		.clear();
	pool.LastFrameContactsCushion	.clear();

	const uint32_t			eventOffset				= outputEvents.size();

	::gpk::SEngine			& engine				= pool.Engine;
	if(false == pool.MatchState.TurnState.Active) { // Always set the stick origin to the position of the cue ball
		::gpk::n3f				ballPosition			= {};
		pool.GetBallPosition(0, ballPosition);
		engine.SetPosition(pool.ActiveStickEntity(), ballPosition);
		gpk_necs(::d1::processInputEvents(pool, inputEvents, outputEvents));
		gpk_necs(engine.Update(secondsElapsed));
	}
	else {
		for(uint8_t iBall = 0; iBall < pool.MatchState.CountBalls; ++iBall)
			pool.GetBallPosition(iBall, pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].push_back({})].A);

		gpk_necs(::d1::poolGamePhysicsUpdate(pool, outputEvents, secondsElapsed));

		const float				radius					= pool.MatchState.Table.BallRadius;
		const float				diameter				= radius * 2;
		for(uint8_t iBall = 0; iBall < pool.MatchState.CountBalls; ++iBall) {
			::gpk::line3<float>		& delta					= pool.PositionDeltas[iBall][pool.PositionDeltas[iBall].size() - 1];
			pool.GetBallPosition(iBall, delta.B);
			if(engine.IsPhysicsActive(pool.Entities.Balls[iBall])) {
				::gpk::SBodyForces		& forces				= engine.Integrator.Forces[engine.Entities[pool.Entities.Balls[iBall]].RigidBody];
				const ::gpk::n3f		rotationResult			= (delta.B - delta.A) / diameter * ::gpk::math_2pi;
				forces.Rotation		+= {rotationResult.z, 0, -rotationResult.x};
			}
			if(pool.PositionDeltas[iBall].size() > 10) {
				for(uint32_t iDelta = 0; iDelta < pool.PositionDeltas[iBall].size(); ++iDelta) {
					if(false == engine.IsPhysicsActive(pool.Entities.Balls[iBall]))
						pool.PositionDeltas[iBall].remove_unordered(iDelta--);
				}
			}
		}
	}

	gpk_necs(::d1::evaluateTurnProgress(pool, outputEvents, eventOffset));

	return 0;
}

::gpk::error_t	d1::debugPrintMatchState	(const ::d1::SPoolMatchState & matchState) { 
	info_printf("Match state:"
		"\nSeed         : %llu"
		"\nTimeStart    : %llu"
		"\nTotalSeconds : %f"
		"\nMode         : %s"
		"\nCountBalls   : %u"
		, matchState.Seed
		, matchState.TimeStart
		, (float)matchState.TotalSeconds
		, ::gpk::get_value_namep(matchState.Mode)
		, matchState.CountBalls
		);
	return 0;
}

::gpk::error_t	d1::debugPrintTurnState		(const ::d1::SPoolTurnState & turnState) { 
	info_printf("Turn state:"
		"\nActive        : %i"
		"\nTeamActive    : %i"
		"\nTeamStripped  : %i"
		"\nStrippedChosen: %i"
		"\nPaused        : %i"
		"\nEnd           : %i"
		"\nNotInHand     : %i"
		"\nInHandAnywhere: %i"
		, (int)turnState.Active
		, (int)turnState.TeamActive
		, (int)turnState.TeamStripped
		, (int)turnState.StrippedChosen
		, (int)turnState.Paused
		, (int)turnState.End
		, (int)turnState.NotInHand
		, (int)turnState.InHandAnywhere
		);
	return 0;
}

::gpk::error_t	d1::debugPrintStickControl	(const ::d1::SStickControl & stickControl) { 
	info_printf("Stick control:"
		"\nShift     : {%f, %f}"
		"\nAngle     : %f"
		"\nPitch     : %f"
		"\nVelocity  : %f"
		, gpk_xy(stickControl.Shift)
		, stickControl.Angle		
		, stickControl.Pitch		
		, stickControl.Velocity	
		);
	return 0;
}

::gpk::error_t	d1::debugPrintTurnInfo		(const ::d1::SPoolTurnInfo & turnInfo) { 
	info_printf("Turn info:"
		"\nTimeStart   : %lli"
		"\nTimeShoot   : %lli"
		"\nTimeEnded   : %lli"
		"\nFirstContact: %i"
		"\nPlayer      : %i" 
		"\nTeam        : %i"
		"\nContinues   : %i" 
		"\nReverse     : %i" 
		"\nFoul        : %i"
		"\nOrientation : " QUAT_F32
		, turnInfo.Time.Start
		, turnInfo.Time.Shoot
		, turnInfo.Time.Ended
		, (int)turnInfo.FirstContact
		, (int)turnInfo.Player
		, (int)turnInfo.Team
		, (int)turnInfo.Continues
		, (int)turnInfo.Reverse
		, (int)turnInfo.Foul
		, gpk_xyzw(turnInfo.Orientation)
		);
	::d1::debugPrintStickControl(turnInfo.StickControl);
	return 0;
}
