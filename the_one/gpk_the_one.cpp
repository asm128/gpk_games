#include "gpk_the_one.h"

static	::gpk::error_t				theOneSetup		(::the1::STheOne & app, const ::gpk::ptr_obj<::gpk::SInput> & inputState, the1::POOL_GAME_MODE mode = the1::POOL_GAME_MODE_8Ball) { 
	gpk_necs(::the1::poolGameSetup(app.MainGame.Game, mode));
	//for(uint32_t iGame = 0; iGame < app.TestGames.size(); ++iGame) {
	//	gpk_necs(::the1::poolGameSetup(app.TestGames[iGame].Game, the1::POOL_GAME_MODE_Test2Balls));
	//}
	::the1::SPlayerUI				& playerUI				= app.MainGame.PlayerUI[app.MainGame.CurrentPlayer];
	//::the1::SCamera				& camera				= playerUI.Cameras.Selected ? playerUI.Cameras.Balls[playerUI.Cameras.Selected - 1] : playerUI.Cameras.Free;

	for(uint32_t iBall = 0; iBall < app.MainGame.Game.StateCurrent.BallCount; ++iBall) {
		playerUI.Cameras.Balls[iBall].Position				= playerUI.Cameras.Free.Position;
		playerUI.Cameras.Balls[iBall].BallLockAtPosition	= true;
		playerUI.Cameras.Balls[iBall].BallLockAtTarget		= true;
		app.MainGame.Game.GetBallPosition(iBall, playerUI.Cameras.Balls[iBall].Target);
	}

	::the1::guiSetup(app, inputState);
	return 0; 
}


static	::gpk::error_t		updateInput				(::the1::STheOne & app, double secondsElapsed, ::gpk::view_array<const uint8_t> keyStates, const ::gpk::SCoord3<int16_t> mouseDeltas, ::gpk::view_array<const uint8_t> buttonStates) { 
	if(keyStates[VK_ADD]) 
		app.MainGame.TimeScale					+= (float)secondsElapsed;
	else if(keyStates[VK_SUBTRACT]) 
		app.MainGame.TimeScale					= ::gpk::max(0.f, app.MainGame.TimeScale - (float)secondsElapsed);
	else if(keyStates['T']) 
		app.MainGame.TimeScale					= 1;

	secondsElapsed				*= app.MainGame.TimeScale;

	bool							reverse					= keyStates[VK_SHIFT];
	float							scale					= 1.0f * (reverse ? -1 : 1);

	::the1::SPlayerUI				& playerUI				= app.MainGame.PlayerUI[app.MainGame.CurrentPlayer];

	if(keyStates[VK_CONTROL]) {
		if(keyStates['0']) 
			playerUI.Cameras.Selected	= reverse ? 0 : the1::MAX_BALLS + 1;
		else if(keyStates['9']) 
			playerUI.Cameras.Selected	= 1;
		else if(keyStates['8']) 
			playerUI.Cameras.Selected	= 9;
		else {
			if(keyStates[VK_MENU]) {
				const uint32_t offset = app.MainGame.Game.StateCurrent.BallCount / 2U;
				for(uint32_t iBall = offset; iBall < app.MainGame.Game.StateCurrent.BallCount; ++iBall) {
					 if(keyStates['1' + iBall - offset]) {
						 playerUI.Cameras.Selected	= iBall + 2;
						 break;
					 }
				}
			}
			else {
				for(uint32_t iBall = 0; iBall < app.MainGame.Game.StateCurrent.BallCount / 2U - 1; ++iBall) {
					 if(keyStates['1' + iBall]) {
						 playerUI.Cameras.Selected	= iBall + 2;
						 break;
					 }
				}
			}
		}
	}

	app.MainGame.Game.StateCurrent.Player[0].Stick.Angle	+= mouseDeltas.x * (1.0f / (float)::gpk::math_2pi) * .05f;

	if(keyStates[VK_ESCAPE] || buttonStates[1]) 
		app.MainGame.Game.StateCurrent.Player[0].Stick.Velocity = 0;
	else {
		if(keyStates[VK_RETURN] || buttonStates[0]) {
			if(app.MainGame.Game.StateCurrent.Player[0].Stick.Velocity > 5) 
				app.MainGame.Game.StateCurrent.Player[0].Stick.Velocity	= 5;
			else if(app.MainGame.Game.StateCurrent.Player[0].Stick.Velocity < 5) {
				app.MainGame.Game.StateCurrent.Player[0].Stick.Velocity	+= (float)secondsElapsed * 2.5f;
				app.MainGame.Game.Engine.SetHidden(app.MainGame.Game.StateCurrent.Player[0].Stick.Entity, false);
			}
		}
		else if(app.MainGame.Game.StateCurrent.Player[0].Stick.Velocity > 0) {
			::gpk::SCoord3<float>						velocity				= {app.MainGame.Game.StateCurrent.Player[0].Stick.Velocity, 0, 0}; //{70.0f + (rand() % 90), 0, 0};
			velocity.RotateY(app.MainGame.Game.StateCurrent.Player[0].Stick.Angle);
			app.MainGame.Game.Engine.SetVelocity(app.MainGame.Game.StateCurrent.Ball[0].Entity, velocity);
			//app.MainGame.Game.Engine.SetRotation(app.MainGame.Game.StartState.Balls[0].Entity, {0, (1.0f + (rand() % 10) * .5f) * -scale, 0});
			app.MainGame.Game.Engine.SetHidden(app.MainGame.Game.StateCurrent.Player[0].Stick.Entity, true);
			app.MainGame.Game.StateCurrent.Player[0].Stick.Velocity = 0;
		}
	}



	::the1::SCamera					& cameraSelected		
		= (playerUI.Cameras.Selected == 0					) ? playerUI.Cameras.Free 
		: (playerUI.Cameras.Selected > the1::MAX_BALLS		) ? playerUI.Cameras.Stick
		: playerUI.Cameras.Balls[playerUI.Cameras.Selected - 1] 
		;
	if(keyStates[VK_CONTROL]) {
		if(0 == playerUI.Cameras.Selected) {
				 if(keyStates['Z']) cameraSelected.Target.z += float(secondsElapsed * scale);
			else if(keyStates['X']) cameraSelected.Target.x += float(secondsElapsed * scale);
			else if(keyStates['Y']) cameraSelected.Target.y += float(secondsElapsed * scale);
		}
	}
	else{
			 if(keyStates['Z']) cameraSelected.Position.z += float(secondsElapsed * scale);
		else if(keyStates['X']) cameraSelected.Position.x += float(secondsElapsed * scale);
		else if(keyStates['Y']) cameraSelected.Position.y += float(secondsElapsed * scale);

			 if(keyStates['R']) ::the1::poolGameReset(app.MainGame.Game, app.MainGame.Game.StateCurrent.Mode);
		else if(keyStates['8']) ::the1::poolGameReset(app.MainGame.Game, ::the1::POOL_GAME_MODE_8Ball);
		else if(keyStates['9']) ::the1::poolGameReset(app.MainGame.Game, ::the1::POOL_GAME_MODE_9Ball);
		else if(keyStates['0']) ::the1::poolGameReset(app.MainGame.Game, ::the1::POOL_GAME_MODE_10Ball);
		else if(keyStates['2']) ::the1::poolGameReset(app.MainGame.Game, ::the1::POOL_GAME_MODE_Test2Balls);
	}

	return 0;
}

::gpk::error_t				the1::theOneUpdate		(::the1::STheOne & app, double secondsElapsed, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::view_array<::gpk::SSysEvent> & /*systemEvents*/) { 
	switch(app.ActiveState) {
	case ::the1::APP_STATE_Init		: {
		::theOneSetup(app, inputState);
		app.ActiveState				= ::the1::APP_STATE_Home;
	}
		break;
	//case ::the1::APP_STATE_Play		: {
	//	app.ActiveState				= ::the1::APP_STATE_Home;
	//	break;
	}
	::updateInput(app, secondsElapsed, inputState->KeyboardCurrent.KeyState, inputState->MouseCurrent.Deltas.Cast<int16_t>(), inputState->MouseCurrent.ButtonState);

	::the1::SPoolGame				& activeGame			= app.MainGame.Game;
	::the1::poolGameUpdate(activeGame, secondsElapsed * app.MainGame.TimeScale);

	::the1::SPlayerUI				& playerUI				= app.MainGame.PlayerUI[app.MainGame.CurrentPlayer];
	bool							playActive				= false;
	for(uint32_t iBall = 0; iBall < activeGame.StateCurrent.BallCount; ++iBall) {
		::the1::SCamera					& cameraBall			= playerUI.Cameras.Balls[iBall];
		if(0 == iBall) {
			activeGame.GetBallPosition(iBall, cameraBall.Target);
			cameraBall.Target			/= 2;		
		}
		else {
			activeGame.GetBallPosition(0, cameraBall.Position);
			activeGame.GetBallPosition(iBall, cameraBall.Target);
			auto							distance				=  cameraBall.Target - cameraBall.Position;
			auto							direction				=  ::gpk::SCoord3<float>{distance}.Normalize();
			cameraBall.Position			+= direction * -2.0f;
			cameraBall.Position.y		= 1.75f * .35f;
		}
		playActive = playActive || activeGame.Engine.Integrator.BodyFlags[activeGame.Engine.ManagedEntities.Entities[activeGame.StateCurrent.Ball[iBall].Entity].RigidBody].Active;
	}
	if(false == playActive)
		app.MainGame.Game.Engine.SetHidden(app.MainGame.Game.StateCurrent.Player[0].Stick.Entity, false);


	return 0; 
}

::gpk::error_t				the1::theOneDraw		(::the1::STheOne & app, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> & backBuffer, double totalSeconds) { 
	const ::the1::SPlayerUI			& playerUI				= app.MainGame.PlayerUI[app.MainGame.CurrentPlayer];
	const ::the1::SCamera			& cameraSelected		
		= (playerUI.Cameras.Selected == 0					) ? playerUI.Cameras.Free 
		: (playerUI.Cameras.Selected > the1::MAX_BALLS		) ? playerUI.Cameras.Stick
		: playerUI.Cameras.Balls[playerUI.Cameras.Selected - 1] 
		;
	::the1::poolGameDraw(app.MainGame.Game, backBuffer, cameraSelected.Position, cameraSelected.Target, {0, 1, 0}, totalSeconds);
	return 0; 
}

