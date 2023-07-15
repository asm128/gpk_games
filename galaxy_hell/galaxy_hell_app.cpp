#include "gpk_galaxy_hell_app.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"
#include "gpk_file.h"
#include "gpk_deflate.h"
#include "gpk_path.h"
#include "gpk_event_screen.h"

static	::gpk::error_t	processScreenEvent		(::ghg::SGalaxyHellApp & app, const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Close:
	case ::gpk::EVENT_SCREEN_Deactivate: 
		if(app.ActiveState != ::ghg::APP_STATE_Welcome) {
			app.Save(::ghg::SAVE_MODE_AUTO);
			app.ActiveState						= ::ghg::APP_STATE_Home;
		}
		break;
	case ::gpk::EVENT_SCREEN_Create:
	case ::gpk::EVENT_SCREEN_Resize: {
		::gpk::n2<uint16_t>			newMetrics				= *(const ::gpk::n2<uint16_t>*)screenEvent.Data.begin();
		::gpk::guiUpdateMetrics(*app.DialogDesktop.GUI, newMetrics, true);
		for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) {
			if(app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI) ::gpk::guiUpdateMetrics(*app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI, newMetrics, true);
			if(app.UIPlay.PlayerUI[iPlayer].DialogPlay.GUI) ::gpk::guiUpdateMetrics(*app.UIPlay.PlayerUI[iPlayer].DialogPlay.GUI, newMetrics, true);
		}

		app.RenderTargetPool.resize(16);
		for(uint32_t iRT = 0; iRT < 16; ++iRT) {
			app.RenderTargetPool[iRT]->resize(newMetrics);
		}

		app.Game.DrawCache.RenderTargetMetrics = newMetrics;
		double						currentRatio			= app.Game.DrawCache.RenderTargetMetrics.y / (double)app.Game.DrawCache.RenderTargetMetrics.x;
		double						targetRatioY			=  9 / 16.0;
		if(currentRatio >= targetRatioY)
			app.Game.DrawCache.RenderTargetMetrics.y = (uint16_t)(app.Game.DrawCache.RenderTargetMetrics.x * targetRatioY + .1f);
		else 
			app.Game.DrawCache.RenderTargetMetrics.x = (uint16_t)(app.Game.DrawCache.RenderTargetMetrics.y / targetRatioY + .1f);
		}
		break;
	}
	return 0;
}

static	::gpk::error_t	processTextEvent		(::ghg::SGalaxyHellApp & app, const ::gpk::SEventView<::gpk::EVENT_TEXT> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_TEXT_Char:
		if(screenEvent.Data[0] >= 0x20 && screenEvent.Data[0] <= 0x7F) {
			if(app.ActiveState == ::ghg::APP_STATE_Welcome)
				app.InputboxText.push_back(screenEvent.Data[0]);
		}
		break;
	}
	return 0;
}

static	::gpk::error_t	processKeyboardEvent	(::ghg::SGalaxyHellApp & app, const ::gpk::SEventView<::gpk::EVENT_KEYBOARD> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_KEYBOARD_Down:
		if(screenEvent.Data[0] == VK_ESCAPE) {
			if(app.ActiveState != ::ghg::APP_STATE_Home) {
				app.Save(::ghg::SAVE_MODE_AUTO);
				app.ActiveState				= ::ghg::APP_STATE_Home;
			}
			else { 
				bool editing = false;
				for(uint32_t iPlayer = 0; iPlayer < app.TunerPlayerCount->ValueCurrent; ++iPlayer) {
					if(app.UIPlay.PlayerUI[iPlayer].InputBox.Editing) {
						editing = true;
						break;
					}
				}
				if(!editing) {
					app.Game.PlayState.Paused	= false;
					app.ActiveState				= ::ghg::APP_STATE_Play;
				}
			}
		}
		break;
	}
	return 0;
}

static	::gpk::error_t	processSystemEvent		(::ghg::SGalaxyHellApp & app, const ::gpk::SSystemEvent & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen		: es_if(errored(::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN	>(sysEvent, [&app](auto ev) { return processScreenEvent		(app, ev); }))); break;
	case ::gpk::SYSTEM_EVENT_Text		: es_if(errored(::gpk::eventExtractAndHandle<::gpk::EVENT_TEXT		>(sysEvent, [&app](auto ev) { return processTextEvent		(app, ev); }))); break;
	case ::gpk::SYSTEM_EVENT_Keyboard	: es_if(errored(::gpk::eventExtractAndHandle<::gpk::EVENT_KEYBOARD	>(sysEvent, [&app](auto ev) { return processKeyboardEvent	(app, ev); }))); break;
	}
	return 0;
}

::gpk::error_t			ghg::galaxyHellUpdate	(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEventsNew) {
	rvis_if(1, app.ActiveState == ::ghg::APP_STATE_Quit);

	gpk_necs(systemEventsNew.for_each([&app](const ::gpk::pobj<::gpk::SSystemEvent> & sysEvent) { return ::processSystemEvent(app, *sysEvent); }));

	switch(app.ActiveState) {
	case APP_STATE_Init		: {
		::ghg::solarSystemSetup(app.Game, app.Game.DrawCache.RenderTargetMetrics);
		::ghg::guiSetup(app, inputState);

		::gpk::aobj<::gpk::apod<char>>					fileNames					= {};
		::gpk::pathList(app.SavegameFolder, fileNames, app.ExtensionSaveAuto);
		if(fileNames.size()) {
			if(errored(::ghg::solarSystemLoad(app.Game, fileNames[0]))) {
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
		app.TunerPlayerCount->SetValue((uint8_t)app.Game.PlayState.CountPlayers);
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

	::gpk::view<::ghg::SShipController>		controllerPlayer	= app.Game.ShipControllers;


	const ::gpk::SInput								& input				= *app.DialogDesktop.Input;
	if(controllerPlayer.size() && app.Game.PlayState.CountPlayers == 1) {
		controllerPlayer[0].Forward					= input.KeyboardCurrent.KeyState[VK_UP		] || input.KeyboardCurrent.KeyState['W'];
		controllerPlayer[0].Back					= input.KeyboardCurrent.KeyState[VK_DOWN	] || input.KeyboardCurrent.KeyState['S'];
		controllerPlayer[0].Left					= input.KeyboardCurrent.KeyState[VK_LEFT	] || input.KeyboardCurrent.KeyState['A'];
		controllerPlayer[0].Right					= input.KeyboardCurrent.KeyState[VK_RIGHT	] || input.KeyboardCurrent.KeyState['D'];
		controllerPlayer[0].Turbo					= input.KeyboardCurrent.KeyState[VK_RCONTROL] || input.KeyboardCurrent.KeyState[VK_CONTROL] || input.KeyboardCurrent.KeyState[VK_LSHIFT] || input.KeyboardCurrent.KeyState[VK_SHIFT];
		if(input.MouseCurrent.Deltas.x || input.MouseCurrent.Deltas.y || input.MouseCurrent.Deltas.z)
			controllerPlayer[0].PointerDeltas			= input.MouseCurrent.Deltas.i16();
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

	::ghg::solarSystemUpdate(app.Game, (false == inGame) ? 0 : lastTimeSeconds, *inputState, systemEventsNew);
	for(uint32_t iShip = 0; iShip < app.Game.ShipState.SpaceshipManager.ShipOrbiterActionQueue.size(); ++iShip)
		if(iShip < app.Game.PlayState.CountPlayers) {
			if(app.Game.ShipState.SpaceshipManager.ShipOrbiterActionQueue[iShip]) {
				::gpk::apod<::gpk::SHIP_ACTION> & actionQueue = *app.Game.ShipState.SpaceshipManager.ShipOrbiterActionQueue[iShip];
				for(uint32_t iEvent = 0; iEvent < actionQueue.size(); ++iEvent)
					if(actionQueue[iEvent] == ::gpk::SHIP_ACTION_Spawn) {
						app.Save(::ghg::SAVE_MODE_STAGE);
						actionQueue[iEvent] = (::gpk::SHIP_ACTION)-1;
					}
			}
		}
	//::ghg::overlayUpdate(app.Overlay, app.World.PlayState.Stage, app.World.ShipState.ShipCores.size() ? app.World.ShipState.ShipCores[0].Score : 0, app.World.PlayState.TimeWorld);

	app.ActiveState					= (::ghg::APP_STATE)::ghg::guiUpdate(app, systemEventsNew);
	return 0;
}

::gpk::error_t					ghg::galaxyHellDraw				(::ghg::SGalaxyHellApp & app, ::gpk::n2u16 renderTargetSize) {
	if(app.ActiveState < 2)
		return 0;

	::gpk::pobj<::ghg::TRenderTarget>		target	= {};
	{
		::std::lock_guard			rtLock(app.RenderTargetLockPool);
		if(app.RenderTargetPool.size()) {
			target					= app.RenderTargetPool[0];
			app.RenderTargetPool.remove_unordered(0);
		}
	}
	if(!target) {
		::std::lock_guard			rtLock(app.RenderTargetLockQueue);
		if(app.RenderTargetQueue.size() > 1) {
			target					= app.RenderTargetQueue[0];
			app.RenderTargetQueue.remove_unordered(0);
		}
	}

	target->resize(renderTargetSize.u32(), {0, 0, 0, 1}, 0xFFFFFFFFU);

	::gpk::g8bgra							targetPixels			= target->Color.View;
	::gpk::grid<uint32_t>									depthBuffer				= target->DepthStencil.View;
	switch(app.ActiveState) {
	default					: 
	case APP_STATE_Play		: 
		::ghg::solarSystemDraw(app.Game, app.Game.DrawCache, app.Game.LockUpdate);
		::gpk::pobj<::ghg::TRenderTarget>		sourceRT	= {};
		sourceRT								= app.Game.DrawCache.RenderTarget;
		if(!sourceRT)
			break;

		::gpk::g8bgra			cameraView			= sourceRT->Color.View;
		::gpk::n2i16						cameraViewMetrics	= cameraView.metrics().i16();
		::gpk::grid_copy(targetPixels, cameraView, ::gpk::n2<uint32_t>
				{ (targetPixels.metrics().x >> 1) - (cameraView.metrics().x >> 1)
				, (targetPixels.metrics().y >> 1) - (cameraView.metrics().y >> 1)
				}
			);

		const ::gpk::n2i16		targetCenter		= targetPixels.metrics().i16() / 2;
		const ::gpk::n2i16		cameraCenter		= cameraView.metrics().i16() / 2;
		::gpk::n2i16				cornerTopLeft		= targetCenter + ::gpk::n2i16{int16_t(-cameraCenter.x), int16_t(-cameraCenter.y)};
		::gpk::n2i16				cornerTopRight		= targetCenter + ::gpk::n2i16{int16_t( cameraCenter.x), int16_t(-cameraCenter.y)};
		::gpk::n2i16				cornerBottomLeft	= targetCenter + ::gpk::n2i16{int16_t(-cameraCenter.x), int16_t( cameraCenter.y)};
		::gpk::n2i16				cornerBottomRight	= targetCenter + ::gpk::n2i16{int16_t( cameraCenter.x), int16_t( cameraCenter.y)};

		cornerTopLeft		.InPlaceClamp({}, {(int16_t)(targetPixels.metrics().x - 1), (int16_t)(targetPixels.metrics().y - 1)});
		cornerTopRight		.InPlaceClamp({}, {(int16_t)(targetPixels.metrics().x - 1), (int16_t)(targetPixels.metrics().y - 1)});
		cornerBottomLeft	.InPlaceClamp({}, {(int16_t)(targetPixels.metrics().x - 1), (int16_t)(targetPixels.metrics().y - 1)});
		cornerBottomRight	.InPlaceClamp({}, {(int16_t)(targetPixels.metrics().x - 1), (int16_t)(targetPixels.metrics().y - 1)});

		::gpk::drawLine(targetPixels, ::gpk::line2i16{cornerTopLeft		, cornerTopRight	}, ::gpk::DARKGRAY);
		::gpk::drawLine(targetPixels, ::gpk::line2i16{cornerTopLeft		, cornerBottomLeft	}, ::gpk::DARKGRAY);
		::gpk::drawLine(targetPixels, ::gpk::line2i16{cornerTopRight	, cornerBottomRight	}, ::gpk::GRAY * 1.1);
		::gpk::drawLine(targetPixels, ::gpk::line2i16{cornerBottomLeft	, cornerBottomRight	}, ::gpk::GRAY * 1.1);

		//::ghg::overlayDraw(app.Overlay, app.World.DrawCache, app.World.PlayState.TimeWorld, depthBuffer, targetPixels);
		break;
	}

	{
		::std::lock_guard							lockUpdate			(app.Game.LockUpdate);
		::gpk::guiDraw(*app.DialogPerState[app.ActiveState].GUI, targetPixels);
	}
	if(app.ActiveState != ::ghg::APP_STATE_Play) {
		::std::lock_guard							lockUpdate			(app.Game.LockUpdate);
		::gpk::guiDraw(*app.DialogDesktop.GUI, targetPixels);
	}
	for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) {
		::std::lock_guard							lockUpdate			(app.Game.LockUpdate);
		::gpk::guiDraw((app.ActiveState == ::ghg::APP_STATE_Play)
			? *app.UIPlay.PlayerUI[iPlayer].DialogPlay.GUI
			: *app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI
			, targetPixels
		);
	}
	{
		::std::lock_guard			rtLock(app.RenderTargetLockQueue);
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