#include "gpk_d1.h"
#include "gpk_pool_game_update.h"

static	::gpk::error_t		theOneSetup				(::d1::SD1UI & appUI, ::d1::SD1Game & game, const ::gpk::pobj<::gpk::SInput> & inputState) { 
	gpk_necs(::d1::poolGameSetup(game.Pool));
	gpk_necs(::d1::guiSetup(appUI, game, inputState));

	for(uint32_t iPlayer = 0; iPlayer < game.Players.size(); ++iPlayer) {
		::d1::SPlayerCameras			& playerCameras			= game.Players[iPlayer].Cameras;
		for(uint8_t iBall = 0; iBall < game.Pool.MatchState.CountBalls; ++iBall) {
			playerCameras.Balls[iBall].Position					= playerCameras.Free.Position;
			playerCameras.Balls[iBall].BallLockAtPosition		= true;
			playerCameras.Balls[iBall].BallLockAtTarget			= true;
			game.Pool.GetBallPosition(iBall, playerCameras.Balls[iBall].Target);
		}
	}
	return 0;
}

static	::gpk::error_t		stickUpdateRotation		(::d1::SD1Game & clientGame, float actualSecondsElapsed, ::gpk::vcu8 keyStates, const ::gpk::n3i16 mouseDeltas, ::gpk::vcu8 buttonStates) {
	const bool						slow						= keyStates[VK_SHIFT];
	// The following aim unit constants should be grabbed from a settings struct.
	::gpk::n2f						rotation					= {};
	{
		const double					aimUnit						= 2.5;
		const double					aimScaled					= aimUnit * (slow ? .1 : 1);
		const ::gpk::n2f				rotationKeyValue			= {float(-1.0 / ::gpk::math_2pi * aimScaled), float(1.0 / ::gpk::math_2pi * aimScaled)};
		// Define a rotation value depending on 
			 if(keyStates[VK_UP		])	rotation.y	= actualSecondsElapsed * rotationKeyValue.y;
		else if(keyStates[VK_DOWN	])	rotation.y	= actualSecondsElapsed * -rotationKeyValue.y;
			 if(keyStates[VK_RIGHT	])	rotation.x	= actualSecondsElapsed * rotationKeyValue.x;
		else if(keyStates[VK_LEFT	])	rotation.x	= actualSecondsElapsed * -rotationKeyValue.x;
	}

	// Grab rotation from mouse deltas.
	if(buttonStates[1] && (mouseDeltas.x || mouseDeltas.y)) {
		const double					aimUnit						= 0.05;
		const double					aimScaled					= aimUnit * (slow ? .1 : 1);
		const ::gpk::n2f				rotationMouseValue			= {float(-1.0 / ::gpk::math_2pi * aimScaled), float(-1.0 / ::gpk::math_2pi * aimScaled)};
		if(buttonStates[0]) {
			if(mouseDeltas.y) 
				rotation.y					+= mouseDeltas.y * rotationMouseValue.y;
		}
		else if(mouseDeltas.x) 
			rotation.x					+= mouseDeltas.x * rotationMouseValue.x;
	}

	if(rotation.y || rotation.x) {
		::d1::SEventPlayer				newEvent						= {::d1::PLAYER_INPUT_Turn};
		if(rotation.y) {
			const ::d1::SArgsStickTurn		turnInfo						= {rotation.y, ::gpk::AXIS_Y_POSITIVE};
			newEvent.Data				= ::gpk::vcu8{(const uint8_t*)&turnInfo, sizeof(turnInfo)};
			clientGame.QueueStick.push_back(newEvent);
		}
		if(rotation.x) {
			const ::d1::SArgsStickTurn		turnInfo						= {rotation.x, ::gpk::AXIS_X_POSITIVE};
			newEvent.Data				= ::gpk::vcu8{(const uint8_t*)&turnInfo, sizeof(turnInfo)};
			clientGame.QueueStick.push_back(newEvent);
		}
	}
	return 0;
}

static	::gpk::error_t		stickCameraUpdate		(::d1::SD1UI & appUI, ::d1::SD1Game & clientGame, float actualSecondsElapsed, ::gpk::vcu8 keyStates, const ::gpk::n3i16 mouseDeltas, ::gpk::vcu8 buttonStates) {
	appUI, clientGame, actualSecondsElapsed, keyStates, mouseDeltas, buttonStates;
	return 0;
}

static	::gpk::error_t		stickUpdate				(::d1::SD1UI & appUI, ::d1::SD1Game & clientGame, float actualSecondsElapsed, ::gpk::vcu8 keyStates, const ::gpk::n3i16 mouseDeltas, ::gpk::vcu8 buttonStates) {
	const bool						slow					= keyStates[VK_SHIFT];
	if(keyStates[VK_CONTROL]) {
		gpk_necs(::stickCameraUpdate(appUI, clientGame, float(actualSecondsElapsed), keyStates, mouseDeltas, buttonStates));
		// Update force slider
		if(keyStates[VK_UP])
			::gpk::sliderSetValue(*appUI.ForceSlider, int64_t(appUI.ForceSlider->ValueCurrent - 1 * (slow ? 1.0f : 25.0f)));
		if(keyStates[VK_DOWN])
			::gpk::sliderSetValue(*appUI.ForceSlider, int64_t(appUI.ForceSlider->ValueCurrent + 1 * (slow ? 1.0f : 25.0f)));
	}
	else {
		gpk_necs(::stickUpdateRotation(clientGame, actualSecondsElapsed, keyStates, mouseDeltas, buttonStates));
		if(mouseDeltas.z) {	// Update force slider
			double							forceDelta				= mouseDeltas.z / double(WHEEL_DELTA) * (slow ? 1.0f : 25.0f);
			::gpk::sliderSetValue(*appUI.ForceSlider, int64_t(appUI.ForceSlider->ValueCurrent - forceDelta));
		}
	}
	return 0;
}


static	::gpk::error_t		cameraInputSelection	(::d1::SD1Game & clientGame, ::gpk::vcu8 keyStates) { 
	const bool						shift					= keyStates[VK_SHIFT];
	::d1::SPlayerCameras			& playerCameras			= clientGame.ActiveCameras();
	if(keyStates['0']) 
		playerCameras.Selected		= shift ? 0 : d1::MAX_BALLS + 1;	// Free or stick
	else if(keyStates['9']) 
		playerCameras.Selected		= 1;	// Cue ball
	else {
		::d1::SPoolGame					& poolGame				= clientGame.Pool;
		const uint32_t					offset					= poolGame.MatchState.CountBalls / 2U;
		if(shift) {	// stripped balls
			for(uint32_t iBall = offset; iBall < poolGame.MatchState.CountBalls; ++iBall) 
				if(keyStates['1' + iBall - offset]) {
					playerCameras.Selected		= iBall + 2;
					break;
				}
		}
		else {
			for(uint32_t iBall = 0; iBall < offset; ++iBall) 
				if(keyStates['1' + iBall]) {
					playerCameras.Selected		= iBall + 2;
					break;
				}
		}
	}
	return 0; 
}

static	::gpk::error_t		cameraInputTransform	(::d1::SD1Game & clientGame, double actualSecondsElapsed, ::gpk::vcu8 keyStates, const ::gpk::n3i16 mouseDeltas) { 
	const bool						slow					= keyStates[VK_SHIFT];
	::d1::SCamera					& cameraSelected		= clientGame.CameraSelected();
	{ // Update camera zoom
		float							zoomChange				= 0;
		if(!keyStates[VK_CONTROL]) { // update camera zoom
			constexpr float					zoomUnit				= 0.1f;
			if(keyStates[VK_ADD]) {
				zoomChange					= zoomUnit * (slow ? 0.1f : 1.0f);
			}
			else if(keyStates[VK_SUBTRACT]) {
				zoomChange					= -zoomUnit * (slow ? 0.1f : 1.0f);
			}
		}
		else if(mouseDeltas.z) {
			constexpr float					zoomUnit				= 0.001f;
			zoomChange					= mouseDeltas.z * zoomUnit * (slow ? 0.1f : 1.0f);
		}
		if(zoomChange) {
			cameraSelected.Zoom			+= zoomChange;
			cameraSelected.Zoom			= ::gpk::clamp(cameraSelected.Zoom, 0.1f, 6.0f);
		}
	}
	{ // Update camera position and target
		::d1::SPlayerCameras			& playerCameras			= clientGame.ActiveCameras();
		if(playerCameras.Selected <= d1::MAX_BALLS) { // update camera position and target
			float							scale					= 1.0f * (slow ? -1 : 1);
			if(keyStates[VK_CONTROL]) {
				if(0 == playerCameras.Selected) {
						 if(keyStates['Z']) cameraSelected.Target.z += float(actualSecondsElapsed * scale);
					else if(keyStates['X']) cameraSelected.Target.x += float(actualSecondsElapsed * scale);
					else if(keyStates['Y']) cameraSelected.Target.y += float(actualSecondsElapsed * scale);
				}
			}
			else {
					 if(keyStates['Z']) cameraSelected.Position.z += float(actualSecondsElapsed * scale);
				else if(keyStates['X']) cameraSelected.Position.x += float(actualSecondsElapsed * scale);
				else if(keyStates['Y']) cameraSelected.Position.y += float(actualSecondsElapsed * scale);
			}
		}
	}
	return 0; 
}

static	::gpk::error_t		cameraInputUpdate		(::d1::SD1Game & clientGame, double actualSecondsElapsed, ::gpk::vcu8 keyStates, const ::gpk::n3i16 mouseDeltas) { 
	gpk_necs(::cameraInputSelection(clientGame, keyStates));
	gpk_necs(::cameraInputTransform(clientGame, actualSecondsElapsed, keyStates, mouseDeltas));
	return 0;
}

static	::gpk::error_t		updateInput				(::d1::SD1UI & appUI, ::d1::SD1Game & clientGame, double actualSecondsElapsed, ::gpk::vcu8 keyStates, const ::gpk::n3i16 mouseDeltas, ::gpk::vcu8 buttonStates) { 
	double							secondsElapsed			= actualSecondsElapsed * clientGame.TimeScale;

	::d1::SPoolGame					& poolGame				= clientGame.Pool;

	if(keyStates[VK_CONTROL]) {
		if(keyStates[VK_ADD]) 
			clientGame.TimeScale		+= (float)secondsElapsed;
		else if(keyStates[VK_SUBTRACT]) 
			clientGame.TimeScale		= ::gpk::max(0.f, clientGame.TimeScale - (float)secondsElapsed);
		else if(keyStates['T']) 
			clientGame.TimeScale		= 1;
		else if(keyStates['R']) {
			gpk_necs(::d1::poolGameReset(poolGame));
		}
		else if(keyStates[VK_MENU]) {
				 if(keyStates['8']) { clientGame.StartState.Mode = ::d1::POOL_GAME_MODE_8Ball		; ::d1::poolGameReset(poolGame, clientGame.StartState);	}
			else if(keyStates['9']) { clientGame.StartState.Mode = ::d1::POOL_GAME_MODE_9Ball		; ::d1::poolGameReset(poolGame, clientGame.StartState);	}
			else if(keyStates['0']) { clientGame.StartState.Mode = ::d1::POOL_GAME_MODE_10Ball		; ::d1::poolGameReset(poolGame, clientGame.StartState);	}
			else if(keyStates['2']) { clientGame.StartState.Mode = ::d1::POOL_GAME_MODE_Test2Balls	; ::d1::poolGameReset(poolGame, clientGame.StartState);	}
		}
	}

	gpk_necs(::cameraInputUpdate(clientGame, float(actualSecondsElapsed), keyStates, mouseDeltas));

	if(false == poolGame.MatchState.TurnState.Active) // Update stick from user input controls only if the play is not active.
		gpk_necs(::stickUpdate(appUI, clientGame, float(actualSecondsElapsed), keyStates, mouseDeltas, buttonStates));

	return 0;
}

static	::gpk::error_t		processSystemEvent		(::d1::SD1 & app, const ::gpk::SSysEvent & eventToProcess) { 
	switch(eventToProcess.Type) {
	case gpk::SYSEVENT_WINDOW_RESIZE: {
		::gpk::n2u16					newMetrics				= *(const ::gpk::n2u16*)eventToProcess.Data.begin();
		gpk_necs(::gpk::guiUpdateMetrics(*app.AppUI.Dialog.GUI, newMetrics, true));
		break;
	}
	//case ::gpk::SYSEVENT_MOUSE_POSITION:
	case ::gpk::SYSEVENT_CHAR:
		if(eventToProcess.Data[0] >= 0x20 && eventToProcess.Data[0] <= 0x7F) {
			if(app.ActiveState == ::d1::APP_STATE_Home)
				app.AppUI.NameEditBox.Text.push_back(eventToProcess.Data[0]);
		}
		break;
	case ::gpk::SYSEVENT_KEY_DOWN:
		if(eventToProcess.Data[0] == VK_ESCAPE) {
			if(app.ActiveState != ::d1::APP_STATE_Home) {
				app.StateSwitch(::d1::APP_STATE_Home);
			}
			else { 
				if(!app.AppUI.NameEditBox.Editing) {
					app.MainGame.Pool.MatchState.TurnState.Paused			= false;
					app.StateSwitch(::d1::APP_STATE_Play);
				}
			}
		}
		break;
	case ::gpk::SYSEVENT_WINDOW_CLOSE:
	case ::gpk::SYSEVENT_WINDOW_DEACTIVATE: 
		if(app.ActiveState != ::d1::APP_STATE_Welcome) {
			app.StateSwitch(::d1::APP_STATE_Home);
		}
	}
	return 0;
};

static	::gpk::error_t		resetCameraBallCue			(::d1::SPoolGame & poolGame, ::d1::SCamera & cameraBall) {
	poolGame.GetBallPosition(0, cameraBall.Target);
	cameraBall.Target			/= 2;
	return 0;
}

static	::gpk::error_t		resetCameraBallRack			(::d1::SPoolGame & poolGame, uint8_t iBall, ::d1::SCamera & cameraBall) {
	poolGame.GetBallPosition(0, cameraBall.Position);
	poolGame.GetBallPosition(iBall, cameraBall.Target);
	::gpk::n3f						distance				= cameraBall.Target - cameraBall.Position;
	::gpk::n3f						direction				= ::gpk::n3f32{distance}.Normalize();
	cameraBall.Position			+= direction * -2.0f;
	cameraBall.Position.y		= 1.75f * .35f;
	return 0;
}


static	::gpk::error_t		resetBallCamera			(::d1::SD1Game & clientGame, uint8_t iBall, ::d1::SCamera & cameraBall) { 
	::d1::SPoolGame					& poolGame				= clientGame.Pool;
	return iBall 
		? ::resetCameraBallRack(poolGame, iBall, cameraBall)
		: ::resetCameraBallCue(poolGame, cameraBall)
		;
}
static	::gpk::error_t		refreshCameras			(::d1::SD1Game & clientGame, double secondsElapsed) { 
	::d1::SPoolGame					& poolGame				= clientGame.Pool;
	::d1::SPlayerCameras			& playerCameras			= clientGame.ActiveCameras();
	for(uint8_t iBall = 0; iBall < poolGame.MatchState.CountBalls; ++iBall) { // update ball cameras
		::d1::SCamera					& cameraBall			= playerCameras.Balls[iBall];
		::resetBallCamera(clientGame, iBall, cameraBall);
	}

	::d1::SCamera					& cameraSelected		= clientGame.CameraSelected();
	if(playerCameras.Selected > d1::MAX_BALLS) {
		if(poolGame.MatchState.TurnState.Active) { 
			cameraSelected.Target		*= .99;

			::gpk::n3f32					direction				= cameraSelected.Position - cameraSelected.Target;
			double							distance				= direction.Length();
			if(distance < poolGame.MatchState.Table.Dimensions.Slate.x * 1.25f) 
				cameraSelected.Position		= cameraSelected.Target + direction.Normalize() * (distance + secondsElapsed * poolGame.ActiveStick().Velocity);

			if(cameraSelected.Position.y < 1.25f)
				cameraSelected.Position.y	+= float(secondsElapsed);
		} 
		else 
			clientGame.ResetStickCamera();
	}
	return 0;
}

static	::gpk::error_t		handlePLAYER_INPUT		(::d1::SD1 & /*app*/, const ::gpk::SEventView<::d1::PLAYER_INPUT> & childEvent, ::gpk::apobj<::gpk::SEvent<::d1::POOL_EVENT>> & /*outputEvents*/) { 
	info_printf("%s", ::gpk::get_value_namep(childEvent.Type));
	switch(childEvent.Type) { 
	default: gpk_warning_unhandled_event(childEvent); break; 
	case d1::PLAYER_INPUT_Shoot	: break;
	case d1::PLAYER_INPUT_Move	: break;
	case d1::PLAYER_INPUT_Turn	: break;
	case d1::PLAYER_INPUT_Force	: break;
	case d1::PLAYER_INPUT_Ball	: break;
	} 
	return 0; 
}

static	::gpk::error_t		handleMATCH_CONTROL		(::d1::SD1 & /*app*/, const ::gpk::SEventView<::d1::MATCH_CONTROL> & childEvent, ::gpk::apobj<::gpk::SEvent<::d1::POOL_EVENT>> & /*outputEvents*/) { 
	info_printf("%s", ::gpk::get_value_namep(childEvent.Type));
	switch(childEvent.Type) { 
	default: gpk_warning_unhandled_event(childEvent); break; 
	case d1::MATCH_CONTROL_Start	: break;
	} 
	return 0; 
}

static	::gpk::error_t		handleMATCH_EVENT		(::d1::SD1 & app, const ::gpk::SEventView<::d1::MATCH_EVENT> & childEvent, ::gpk::apobj<::gpk::SEvent<::d1::POOL_EVENT>> & /*outputEvents*/) { 
	info_printf("%s", ::gpk::get_value_namep(childEvent.Type));
	::d1::SPoolGame					& pool					= app.MainGame.Pool;
	switch(childEvent.Type) { 
	default: gpk_warning_unhandled_event(childEvent); break; 
	case d1::MATCH_EVENT_Break				: info_printf(""); break;
	case d1::MATCH_EVENT_StrippedAssigned	: info_printf("Team: %i.", childEvent.Data[0]); break;
	case d1::MATCH_EVENT_TurnStart			: 
		::d1::debugPrintTurnState(pool.MatchState.TurnState);
		::d1::debugPrintTurnInfo (pool.ActiveTurn());
	case d1::MATCH_EVENT_TurnEnd			: 
		::d1::debugPrintTurnInfo(*(const ::d1::SPoolTurnInfo*)childEvent.Data.begin()); 
		break;
	case d1::MATCH_EVENT_MatchEnd			: 
	case d1::MATCH_EVENT_MatchStart			: 
		::d1::debugPrintMatchState(*(const ::d1::SPoolMatchState*)childEvent.Data.begin()); 
		break;
	case d1::MATCH_EVENT_Lost				: 
		info_printf("Reason: %s", ::gpk::get_value_namep(*(const ::d1::LOST*)childEvent.Data.begin())); 
	case d1::MATCH_EVENT_Won				: 
		break;
	} 
	return 0; 
}

static	::gpk::error_t		handleBALL_EVENT		(::d1::SD1 & /*app*/, const ::gpk::SEventView<::d1::BALL_EVENT> & childEvent, ::gpk::apobj<::gpk::SEvent<::d1::POOL_EVENT>> & /*outputEvents*/) { 
	info_printf("%s", ::gpk::get_value_namep(childEvent.Type));
	const ::d1::SArgsBall			& argsBall				= *(const ::d1::SArgsBall*)childEvent.Data.begin(); 
	switch(childEvent.Type) { 
	default: gpk_warning_unhandled_event(childEvent); break; 
	case d1::BALL_EVENT_ContactBall		: { const ::d1::SArgsBall::SContactBall		& eventData = argsBall.Event.ContactBall	; info_printf("Ball: %i, Ball B : %i.", eventData.BallA, eventData.BallB	); } break;
	case d1::BALL_EVENT_ContactPocket	: { const ::d1::SArgsBall::SContactPocket	& eventData = argsBall.Event.ContactPocket	; info_printf("Ball: %i, Pocket : %i.", eventData.Ball,  eventData.Pocket	); } break;
	case d1::BALL_EVENT_ContactCushion	: { const ::d1::SArgsBall::SContactCushion	& eventData = argsBall.Event.ContactCushion	; info_printf("Ball: %i, Cushion: %i.", eventData.Ball,  eventData.Cushion	); } break;
	case d1::BALL_EVENT_Pocketed		: { const ::d1::SArgsBall::SPocketed		& eventData = argsBall.Event.Pocketed		; info_printf("Ball: %i, Pocket : %i.", eventData.Ball,  eventData.Pocket	); } break;
	case d1::BALL_EVENT_Fall			: { const ::d1::SArgsBall::SJump			& eventData = argsBall.Event.Jump			; info_printf("Ball: %i.", eventData.Ball); } break;
	case d1::BALL_EVENT_Jump			: { const ::d1::SArgsBall::SFall			& eventData = argsBall.Event.Fall			; info_printf("Ball: %i.", eventData.Ball); } break;
	case d1::BALL_EVENT_FirstContact	: { const ::d1::SArgsBall::SFirstContact	& eventData = argsBall.Event.FirstContact	; info_printf("Ball: %i.", eventData.Ball); } break;
	} 
	return 0; 
}

static	::gpk::error_t		handleFOUL				(::d1::SD1 & /*app*/, const ::gpk::SEventView<::d1::FOUL> eventToProcess, ::gpk::apobj<::gpk::SEvent<::d1::POOL_EVENT>> & /*outputEvents*/) { 
	info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));
	switch(eventToProcess.Type) { 
	default: gpk_warning_unhandled_event(eventToProcess); break; 
	case d1::FOUL_Wrong_ball_first	: { 
		const ::d1::SArgsBall					& argsBall		= *(const ::d1::SArgsBall*)eventToProcess.Data.begin(); 
		const ::d1::SArgsBall::SFirstContact	& eventData		= argsBall.Event.FirstContact; 
		info_printf("Ball: %i.", eventData.Ball); 
		break;
	} 
	} 
	return 0; 
}


::gpk::error_t				d1::theOneUpdate		(::d1::SD1 & app, double secondsElapsed, const ::gpk::pobj<::gpk::SInput> & inputState, const ::gpk::view<::gpk::SSysEvent> & systemEvents) { 
	app.MainGame.FrameInfo.Frame(uint64_t(secondsElapsed * 1000000));

	const ::gpk::FSysEventConst		funcEvent				= [&app](const ::gpk::SSysEvent & sysEvent) { 
		e_if(errored(::processSystemEvent(app, sysEvent)), "Error while processing event '%s' (0x%X):", ::gpk::get_value_namep(sysEvent.Type), sysEvent.Type); 
		return 0; 
	};
	gpk_necs(systemEvents.for_each(funcEvent));

	switch(app.ActiveState) {
	case ::d1::APP_STATE_Quit		: return 1;
	case ::d1::APP_STATE_Welcome	: app.StateSwitch(::d1::APP_STATE_Home); break;	// APP_STATE_Welcome comes right after APP_STATE_Init.
	case ::d1::APP_STATE_Init		: {
		gpk_necs(::theOneSetup(app.AppUI, app.MainGame, inputState));

		::gpk::aobj<::gpk::apod<char>>	fileNames				= {};
		::gpk::pathList(app.FileStrings.SavegameFolder, fileNames, app.FileStrings.ExtensionSaveAuto);
		if(fileNames.size()) {
			if errored(app.Load(fileNames[0])) 
				gpk_necs(::d1::poolGameSetup(app.MainGame.Pool));
		}
		app.StateSwitch(::d1::APP_STATE_Welcome);
		break;
	} // APP_STATE_Init
	case ::d1::APP_STATE_Play		: {
		::d1::SD1Game				& clientGame			= app.MainGame;
		gpk_necs(::updateInput(app.AppUI, clientGame, secondsElapsed, inputState->KeyboardCurrent.KeyState, inputState->MouseCurrent.Deltas.Cast<int16_t>(), inputState->MouseCurrent.ButtonState));

		::d1::SPoolGame					& poolGame				= clientGame.Pool;
		if(false == poolGame.MatchState.TurnState.Active)//(poolGame.MatchState.TurnState.Status == ::d1::TURN_STATUS_Aiming)
			::gpk::sliderSetValue(*app.AppUI.ForceSlider, int64_t(app.AppUI.ForceSlider->ValueLimits.Max - poolGame.ActiveStick().Velocity * (app.AppUI.ForceSlider->ValueLimits.Max / ::d1::MAX_SHOOT_VELOCITY)));

		::gpk::apobj<::gpk::SEvent<::d1::POOL_EVENT>>	outputEvents;
		::d1::poolGameUpdate(poolGame, clientGame.QueueStick, outputEvents, secondsElapsed * clientGame.TimeScale);

		outputEvents.for_each([&app, &outputEvents](const ::gpk::pobj<::gpk::SEvent<::d1::POOL_EVENT>> & _eventToProcess) { 
			const ::gpk::SEvent<::d1::POOL_EVENT> & eventToProcess = *_eventToProcess;
			info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));
			switch(eventToProcess.Type) {
			case ::d1::POOL_EVENT_PLAYER_INPUT  : {	::d1::extractAndHandle<::d1::PLAYER_INPUT >(eventToProcess, [&app, &outputEvents](auto ev){ gpk_necs(handlePLAYER_INPUT (app, ev, outputEvents)); return 0; }); return; }
			case ::d1::POOL_EVENT_MATCH_CONTROL : {	::d1::extractAndHandle<::d1::MATCH_CONTROL>(eventToProcess, [&app, &outputEvents](auto ev){ gpk_necs(handleMATCH_CONTROL(app, ev, outputEvents)); return 0; }); return; }
			case ::d1::POOL_EVENT_MATCH_EVENT   : {	::d1::extractAndHandle<::d1::MATCH_EVENT  >(eventToProcess, [&app, &outputEvents](auto ev){ gpk_necs(handleMATCH_EVENT  (app, ev, outputEvents)); return 0; }); return; }
			case ::d1::POOL_EVENT_BALL_EVENT    : {	::d1::extractAndHandle<::d1::BALL_EVENT   >(eventToProcess, [&app, &outputEvents](auto ev){ gpk_necs(handleBALL_EVENT   (app, ev, outputEvents)); return 0; }); return; }
			case ::d1::POOL_EVENT_FOUL          : {	::d1::extractAndHandle<::d1::FOUL         >(eventToProcess, [&app, &outputEvents](auto ev){ gpk_necs(handleFOUL         (app, ev, outputEvents)); return 0; }); return; }
			default: 
				gpk_warning_unhandled_event(eventToProcess); 
				return;
			}
		});

		clientGame.QueueStick.clear();

		::refreshCameras(app.MainGame, secondsElapsed);
		clientGame.LightPos.RotateY(secondsElapsed);
		break;
	} // APP_STATE_Play
	} // switch

	const ::d1::APP_STATE			newState			= ::d1::guiUpdate(app, systemEvents);
	return app.StateSwitch(newState); 
}

::gpk::error_t				d1::theOneDraw		(::d1::SD1UI & appUI, ::d1::SD1Game & clientGame, ::gpk::rt<::gpk::bgra, uint32_t> & backBuffer, double totalSeconds, bool onlyGUI) { 
	if(false == onlyGUI) {
		const ::d1::SCamera				& cameraSelected	= clientGame.CameraSelected();
		gpk_necs(::d1::poolGameDraw(clientGame.Pool, backBuffer, cameraSelected.Position, cameraSelected.Target, {0, 1, 0}, totalSeconds));
	}
	gpk_necs(::gpk::guiDraw(*appUI.Dialog.GUI, backBuffer.Color.View));
	return 0; 
}
