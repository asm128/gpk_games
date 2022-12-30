#include "gpk_galaxy_hell_app.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"
#include "gpk_storage.h"
#include "gpk_deflate.h"

::gpk::error_t					ghg::galaxyHellUpdate			(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, const ::gpk::view_array<::gpk::SSysEvent> & systemEvents) {
	if(app.ActiveState == ::ghg::APP_STATE_Quit)
		return 1;

	for(uint32_t iEvent = 0; iEvent < systemEvents.size(); ++iEvent) {
		const ::gpk::SSysEvent				& eventToProcess				= systemEvents[iEvent];
		switch(eventToProcess.Type) {
		case gpk::SYSEVENT_WINDOW_RESIZE: {
			::gpk::SCoord2<uint16_t>			newMetrics						= *(const ::gpk::SCoord2<uint16_t>*)eventToProcess.Data.begin();
			::gpk::guiUpdateMetrics(*app.DialogDesktop.GUI, newMetrics.Cast<uint32_t>(), true);
			for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) {
				if(app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI) ::gpk::guiUpdateMetrics(*app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI, newMetrics.Cast<uint32_t>(), true);
				if(app.UIPlay.PlayerUI[iPlayer].DialogPlay.GUI) ::gpk::guiUpdateMetrics(*app.UIPlay.PlayerUI[iPlayer].DialogPlay.GUI, newMetrics.Cast<uint32_t>(), true);
			}

			app.RenderTargetPool.resize(16);
			for(uint32_t iRT = 0; iRT < 16; ++iRT) {
				app.RenderTargetPool[iRT]->resize(newMetrics);
			}

			app.Game.DrawCache.RenderTargetMetrics = newMetrics;
			double								currentRatio					= app.Game.DrawCache.RenderTargetMetrics.y / (double)app.Game.DrawCache.RenderTargetMetrics.x;
			double								targetRatioY					=  9 / 16.0;
			if(currentRatio >= targetRatioY)
				app.Game.DrawCache.RenderTargetMetrics.y = (uint16_t)(app.Game.DrawCache.RenderTargetMetrics.x * targetRatioY + .1f);
			else 
				app.Game.DrawCache.RenderTargetMetrics.x = (uint16_t)(app.Game.DrawCache.RenderTargetMetrics.y / targetRatioY + .1f);
			break;
		}
		//case ::gpk::SYSEVENT_MOUSE_POSITION:
		case ::gpk::SYSEVENT_CHAR:
			if(eventToProcess.Data[0] >= 0x20 && eventToProcess.Data[0] <= 0x7F) {
				if(app.ActiveState == ::ghg::APP_STATE_Welcome)
					app.InputboxText.push_back(eventToProcess.Data[0]);
			}
			break;
		case ::gpk::SYSEVENT_KEY_DOWN:
			if(eventToProcess.Data[0] == VK_ESCAPE) {
				if(app.ActiveState != ::ghg::APP_STATE_Home) {
					app.Save(::ghg::SAVE_MODE_AUTO);
					app.ActiveState				= ::ghg::APP_STATE_Home;
				}
				else { 
					bool editing = false;
					for(uint32_t iPlayer = 0; iPlayer < app.TunerPlayerCount->ValueCurrent; ++iPlayer) {
						if(app.UIPlay.PlayerUI[iPlayer].InputBox.Editing) {
							editing = true;
						}
					}
					if(!editing) {
						app.Game.PlayState.Paused	= false;
						app.ActiveState				= ::ghg::APP_STATE_Play;
					}
				}
			}
			break;
		case ::gpk::SYSEVENT_WINDOW_CLOSE:
		case ::gpk::SYSEVENT_WINDOW_DEACTIVATE: 
			if(app.ActiveState != ::ghg::APP_STATE_Welcome) {
				app.Save(::ghg::SAVE_MODE_AUTO);
				app.ActiveState						= ::ghg::APP_STATE_Home;
			}
		}
	}
	switch(app.ActiveState) {
	case APP_STATE_Init		: {
		::ghg::solarSystemSetup(app.Game, app.Game.DrawCache.RenderTargetMetrics);
		::ghg::guiSetup(app, inputState);

		::gpk::aobj<::gpk::apod<char>>					fileNames					= {};
		::gpk::pathList(app.SavegameFolder, fileNames, app.ExtensionSaveAuto);
		if(fileNames.size()) {
			if errored(::ghg::solarSystemLoad(app.Game, fileNames[0])) {
				app.Game.PilotsReset();
			}
		}
		fileNames					= {};
		::gpk::pathList(app.SavegameFolder, fileNames, app.ExtensionProfile);
		app.Players.resize(fileNames.size());

		for(uint32_t iPlayer = 0; iPlayer < fileNames.size(); ++iPlayer) {
			try {
				gpk_necall(app.Players[iPlayer].Load(fileNames[iPlayer]), "fileNames[iPlayer]: %s", fileNames[iPlayer].begin());
			}
			catch (const char * ) {
				app.Players[iPlayer] = {};
			}

		}
		::gpk::tunerSetValue(*app.TunerPlayerCount, app.Game.PlayState.CountPlayers);
		for(uint32_t iPilot = 0; iPilot < app.Game.PlayState.CountPlayers; ++iPilot) {
			const ::gpk::vcc namePilot	= app.Game.Pilots[iPilot].Name;
			for(uint32_t iPlayer = 0; iPlayer < app.Game.PlayState.CountPlayers; ++iPlayer) {
				if(iPlayer >= app.Players.size())
					app.AddNewPlayer(namePilot);

				const ::gpk::vcc namePlayer	= app.Players[iPlayer].Name;
				if(namePilot == namePlayer) {
					::std::swap(app.Players[iPlayer], app.Players[iPilot]);
					break;
				}
			}
			for(uint32_t iPlayer = app.Game.PlayState.CountPlayers; iPlayer < app.Players.size(); ++iPlayer) {
				const ::gpk::vcc namePlayer	= app.Players[iPlayer].Name;
				if(namePilot == namePlayer) {
					::std::swap(app.Players[iPlayer], app.Players[iPilot]);
					break;
				}
			}
		}
		app.ActiveState					= APP_STATE_Welcome;
		break;
	}
	case  APP_STATE_Welcome:
		if(app.Players.size())
			app.ActiveState					= APP_STATE_Home;
		else {
			//app.DialogDesktop.GUI->Controls.States[app.VirtualKeyboard.IdRoot].Hidden = false;
		}
		break;
	}	 

	const bool inGame = app.ActiveState == ::ghg::APP_STATE_Play;

	::gpk::view_array<::ghg::SShipController>		controllerPlayer	= app.Game.ShipControllers;


	const ::gpk::SInput								& input				= *app.DialogDesktop.Input;
	if(controllerPlayer.size() && app.Game.PlayState.CountPlayers == 1) {
		controllerPlayer[0].Forward					= input.KeyboardCurrent.KeyState[VK_UP		] || input.KeyboardCurrent.KeyState['W'];
		controllerPlayer[0].Back					= input.KeyboardCurrent.KeyState[VK_DOWN	] || input.KeyboardCurrent.KeyState['S'];
		controllerPlayer[0].Left					= input.KeyboardCurrent.KeyState[VK_LEFT	] || input.KeyboardCurrent.KeyState['A'];
		controllerPlayer[0].Right					= input.KeyboardCurrent.KeyState[VK_RIGHT	] || input.KeyboardCurrent.KeyState['D'];
		controllerPlayer[0].Turbo					= input.KeyboardCurrent.KeyState[VK_RCONTROL] || input.KeyboardCurrent.KeyState[VK_CONTROL] || input.KeyboardCurrent.KeyState[VK_LSHIFT] || input.KeyboardCurrent.KeyState[VK_SHIFT];
		if(input.MouseCurrent.Deltas.x || input.MouseCurrent.Deltas.y || input.MouseCurrent.Deltas.z)
			controllerPlayer[0].PointerDeltas			= input.MouseCurrent.Deltas.Cast<int16_t>();
		else 
			controllerPlayer[0].PointerDeltas			= {};
		//or(uint32_t i = 0; i < input.JoystickCurrent.size(); ++i) {
		//	controllerPlayer[0].Forward					= controllerPlayer[0].Forward	; //|| (input.JoystickCurrent[i].Deltas.y < 0);
		//	controllerPlayer[0].Back					= controllerPlayer[0].Back		; //|| (input.JoystickCurrent[i].Deltas.y > 0);
		//	controllerPlayer[0].Left					= controllerPlayer[0].Left		; //|| (input.JoystickCurrent[i].Deltas.x < 0);
		//	controllerPlayer[0].Right					= controllerPlayer[0].Right		; //|| (input.JoystickCurrent[i].Deltas.x > 0);
		//	controllerPlayer[0].Turbo					= controllerPlayer[0].Turbo		; //|| input.JoystickCurrent[i].ButtonState;
		//
	}
	else if(controllerPlayer.size() > 1) {
		controllerPlayer[0].Forward					= input.KeyboardCurrent.KeyState['W'];
		controllerPlayer[0].Back					= input.KeyboardCurrent.KeyState['S'];
		controllerPlayer[0].Left					= input.KeyboardCurrent.KeyState['A'];
		controllerPlayer[0].Right					= input.KeyboardCurrent.KeyState['D'];
		controllerPlayer[0].Turbo					= input.KeyboardCurrent.KeyState[VK_LSHIFT] || input.KeyboardCurrent.KeyState[VK_SHIFT];

		controllerPlayer[1].Forward					= input.KeyboardCurrent.KeyState[VK_UP		];
		controllerPlayer[1].Back					= input.KeyboardCurrent.KeyState[VK_DOWN	];
		controllerPlayer[1].Left					= input.KeyboardCurrent.KeyState[VK_LEFT	];
		controllerPlayer[1].Right					= input.KeyboardCurrent.KeyState[VK_RIGHT	];
		controllerPlayer[1].Turbo					= input.KeyboardCurrent.KeyState[VK_RCONTROL] || input.KeyboardCurrent.KeyState[VK_CONTROL];

		//for(uint32_t i = 0; i < input.JoystickCurrent.size() && i < controllerPlayer.size(); ++i) {
		//	controllerPlayer[i].Forward					= controllerPlayer[i].Forward	|| (input.JoystickCurrent[i].Deltas.y < 0);
		//	controllerPlayer[i].Back					= controllerPlayer[i].Back		|| (input.JoystickCurrent[i].Deltas.y > 0);
		//	controllerPlayer[i].Left					= controllerPlayer[i].Left		|| (input.JoystickCurrent[i].Deltas.x < 0);
		//	controllerPlayer[i].Right					= controllerPlayer[i].Right		|| (input.JoystickCurrent[i].Deltas.x > 0);
		//	controllerPlayer[i].Turbo					= controllerPlayer[i].Turbo		|| input.JoystickCurrent[i].ButtonState;
		//}
	}

	::ghg::solarSystemUpdate(app.Game, (false == inGame) ? 0 : lastTimeSeconds, *inputState, systemEvents);
	for(uint32_t iShip = 0; iShip < app.Game.ShipState.ShipOrbiterActionQueue.size(); ++iShip)
		if(iShip < app.Game.PlayState.CountPlayers) {
			for(uint32_t iEvent = 0; iEvent < app.Game.ShipState.ShipOrbiterActionQueue[iShip].size(); ++iEvent)
				if(app.Game.ShipState.ShipOrbiterActionQueue[iShip][iEvent] == ::ghg::SHIP_ACTION_spawn) {
					::gpk::apod<byte_t>			serialized;
					app.Save(::ghg::SAVE_MODE_STAGE);
					app.Game.ShipState.ShipOrbiterActionQueue[iShip][iEvent] = (::ghg::SHIP_ACTION)-1;
				}
		}
	//::ghg::overlayUpdate(app.Overlay, app.World.PlayState.Stage, app.World.ShipState.ShipCores.size() ? app.World.ShipState.ShipCores[0].Score : 0, app.World.PlayState.TimeWorld);

	app.ActiveState					= (::ghg::APP_STATE)::ghg::guiUpdate(app, systemEvents);
	return 0;
}

::gpk::error_t					ghg::galaxyHellDraw				(::ghg::SGalaxyHellApp & app, ::gpk::SCoord2<uint16_t> renderTargetSize) {
	if(app.ActiveState < 2)
		return 0;

	::gpk::pobj<::ghg::TRenderTarget>		target	= {};
	{
		::gpk::mutex_guard			rtLock(app.RenderTargetLockPool);
		if(app.RenderTargetPool.size()) {
			target					= app.RenderTargetPool[0];
			app.RenderTargetPool.remove_unordered(0);
		}
	}
	if(!target) {
		::gpk::mutex_guard			rtLock(app.RenderTargetLockQueue);
		if(app.RenderTargetQueue.size() > 1) {
			target					= app.RenderTargetQueue[0];
			app.RenderTargetQueue.remove_unordered(0);
		}
	}

	target->resize(renderTargetSize.Cast<uint32_t>(), {0, 0, 0, 1}, 0xFFFFFFFFU);

	::gpk::view2d<::gpk::SColorBGRA>							targetPixels			= target->Color.View;
	::gpk::view2d<uint32_t>									depthBuffer				= target->DepthStencil.View;
	switch(app.ActiveState) {
	default					: 
	case APP_STATE_Play		: 
		::ghg::solarSystemDraw(app.Game, app.Game.DrawCache, app.Game.LockUpdate);
		::gpk::pobj<::ghg::TRenderTarget>		sourceRT	= {};
		sourceRT								= app.Game.DrawCache.RenderTarget;
		if(!sourceRT)
			break;

		::gpk::view2d<::gpk::SColorBGRA>			cameraView			= sourceRT->Color.View;
		::gpk::SCoord2<int16_t>						cameraViewMetrics	= cameraView.metrics().Cast<int16_t>();
		::gpk::grid_copy(targetPixels, cameraView, ::gpk::SCoord2<uint32_t>
				{ (targetPixels.metrics().x >> 1) - (cameraView.metrics().x >> 1)
				, (targetPixels.metrics().y >> 1) - (cameraView.metrics().y >> 1)
				}
			);

		const ::gpk::SCoord2<int16_t>		targetCenter		= targetPixels.metrics().Cast<int16_t>() / 2;
		const ::gpk::SCoord2<int16_t>		cameraCenter		= cameraView.metrics().Cast<int16_t>() / 2;
		::gpk::SCoord2<int16_t>				cornerTopLeft		= targetCenter + ::gpk::SCoord2<int16_t>{int16_t(-cameraCenter.x), int16_t(-cameraCenter.y)};
		::gpk::SCoord2<int16_t>				cornerTopRight		= targetCenter + ::gpk::SCoord2<int16_t>{int16_t( cameraCenter.x), int16_t(-cameraCenter.y)};
		::gpk::SCoord2<int16_t>				cornerBottomLeft	= targetCenter + ::gpk::SCoord2<int16_t>{int16_t(-cameraCenter.x), int16_t( cameraCenter.y)};
		::gpk::SCoord2<int16_t>				cornerBottomRight	= targetCenter + ::gpk::SCoord2<int16_t>{int16_t( cameraCenter.x), int16_t( cameraCenter.y)};

		cornerTopLeft		.InPlaceClamp({}, {(int16_t)(targetPixels.metrics().x - 1), (int16_t)(targetPixels.metrics().y - 1)});
		cornerTopRight		.InPlaceClamp({}, {(int16_t)(targetPixels.metrics().x - 1), (int16_t)(targetPixels.metrics().y - 1)});
		cornerBottomLeft	.InPlaceClamp({}, {(int16_t)(targetPixels.metrics().x - 1), (int16_t)(targetPixels.metrics().y - 1)});
		cornerBottomRight	.InPlaceClamp({}, {(int16_t)(targetPixels.metrics().x - 1), (int16_t)(targetPixels.metrics().y - 1)});

		::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{cornerTopLeft		, cornerTopRight	}, ::gpk::DARKGRAY);
		::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{cornerTopLeft		, cornerBottomLeft	}, ::gpk::DARKGRAY);
		::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{cornerTopRight		, cornerBottomRight	}, ::gpk::GRAY * 1.1);
		::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{cornerBottomLeft	, cornerBottomRight	}, ::gpk::GRAY * 1.1);

		//::ghg::overlayDraw(app.Overlay, app.World.DrawCache, app.World.PlayState.TimeWorld, depthBuffer, targetPixels);
		break;
	}

	{
		::std::lock_guard<::std::mutex>							lockUpdate			(app.Game.LockUpdate);
		::gpk::guiDraw(*app.DialogPerState[app.ActiveState].GUI, targetPixels);
	}
	if(app.ActiveState != ::ghg::APP_STATE_Play) {
		::std::lock_guard<::std::mutex>							lockUpdate			(app.Game.LockUpdate);
		::gpk::guiDraw(*app.DialogDesktop.GUI, targetPixels);
	}
	for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) {
		::std::lock_guard<::std::mutex>							lockUpdate			(app.Game.LockUpdate);
		::gpk::guiDraw((app.ActiveState == ::ghg::APP_STATE_Play)
			? *app.UIPlay.PlayerUI[iPlayer].DialogPlay.GUI
			: *app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI
			, targetPixels
		);
	}
	{
		::gpk::mutex_guard			rtLock(app.RenderTargetLockQueue);
		app.RenderTargetQueue.push_back(target);
	}
	return 0;
}

::gpk::error_t					ghg::listFilesSavegame		(::ghg::SGalaxyHellApp & app, const ::gpk::vcc & saveGameFolder, ::gpk::aobj<::gpk::vcc> & savegameFilenames) {
	::gpk::aobj<::gpk::apod<char>>	fileNames;
	gerror_if(errored(::gpk::pathList(saveGameFolder, fileNames, app.ExtensionSave)), "%s", saveGameFolder.begin());

	::gpk::aobj<::gpk::vcc>				pathFileNames;
	for(uint32_t iFile = 0; iFile < fileNames.size(); ++iFile)
		pathFileNames.push_back(fileNames[iFile]);

	::gpk::aobj<::gpk::vcc>				pathFileNamesSaveAuto;
	::gpk::aobj<::gpk::vcc>				pathFileNamesSaveUser;
	::gpk::aobj<::gpk::vcc>				pathFileNamesSaveStage;
	::gpk::filterPostfix(pathFileNames, app.ExtensionSaveAuto	, pathFileNamesSaveAuto);
	::gpk::filterPostfix(pathFileNames, app.ExtensionSaveUser	, pathFileNamesSaveUser);
	::gpk::filterPostfix(pathFileNames, app.ExtensionSaveStage	, pathFileNamesSaveStage);
		
	for(uint32_t iPath = 0; iPath < pathFileNamesSaveAuto .size(); ++iPath) savegameFilenames.push_back(::gpk::label(pathFileNamesSaveAuto [iPath]));
	for(uint32_t iPath = 0; iPath < pathFileNamesSaveUser .size(); ++iPath) savegameFilenames.push_back(::gpk::label(pathFileNamesSaveUser [iPath]));
	for(uint32_t iPath = 0; iPath < pathFileNamesSaveStage.size(); ++iPath) savegameFilenames.push_back(::gpk::label(pathFileNamesSaveStage[iPath]));
	return 0;
}

::gpk::error_t					ghg::listFilesProfile		(::ghg::SGalaxyHellApp & app, const ::gpk::vcc & saveGameFolder, ::gpk::aobj<::gpk::vcc> & savegameFilenames) {
	::gpk::aobj<::gpk::apod<char>>	fileNames;
	gerror_if(errored(::gpk::pathList(saveGameFolder, fileNames, app.ExtensionProfile)), "%s", saveGameFolder.begin());

	::gpk::aobj<::gpk::vcc>				pathFileNames;
	for(uint32_t iFile = 0; iFile < fileNames.size(); ++iFile)
		savegameFilenames.push_back(::gpk::label(::gpk::vcc{fileNames[iFile]}));
	return 0;
}