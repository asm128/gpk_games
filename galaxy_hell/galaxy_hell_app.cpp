#include "gpk_galaxy_hell_app.h"
#include "gpk_grid_copy.h"
#include "gpk_raster_lh.h"
#include "gpk_storage.h"
#include "gpk_deflate.h"

::gpk::error_t					ghg::galaxyHellUpdate			(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::view_array<::gpk::SSysEvent> & systemEvents) {
	if(app.ActiveState == ::ghg::APP_STATE_Quit)
		return 1;

	for(uint32_t iEvent = 0; iEvent < systemEvents.size(); ++iEvent) {
		switch(systemEvents[iEvent].Type) {
		case gpk::SYSEVENT_RESIZE: {
			::gpk::SCoord2<uint16_t> newMetrics = *(const ::gpk::SCoord2<uint16_t>*)systemEvents[iEvent].Data.begin();
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
		case ::gpk::SYSEVENT_CHAR:
			if(systemEvents[iEvent].Data[0] >= 0x20 && systemEvents[iEvent].Data[0] <= 0x7F) {
				if(app.ActiveState == ::ghg::APP_STATE_Welcome)
					app.InputboxText.push_back(systemEvents[iEvent].Data[0]);
			}
			break;
		case ::gpk::SYSEVENT_KEY_DOWN:
			if(app.ActiveState == ::ghg::APP_STATE_Welcome) {
				switch(systemEvents[iEvent].Data[0]) {
				case VK_BACK:
					app.InputboxText.pop_back(0);
					break;
				case VK_RETURN: {
					::gpk::vcc trimmed{app.InputboxText};
					::gpk::trim(trimmed, trimmed);
					app.Players.push_back({::gpk::label(trimmed)});
					if(app.Game.Pilots.size())
						app.Game.Pilots[0].Name = app.Players[0].Name;
					else 
						app.Game.PilotCreate({app.Players[0].Name, ::ghg::PLAYER_COLORS[0]});

					app.Save(::ghg::SAVE_MODE_AUTO);
					break;
				}
				case VK_ESCAPE:
					app.InputboxText.clear();
					break;
				}
			}
			else if(systemEvents[iEvent].Data[0] == VK_ESCAPE) {
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
		case ::gpk::SYSEVENT_CLOSE:
		case ::gpk::SYSEVENT_DEACTIVATE: 
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

		::gpk::array_obj<::gpk::array_pod<char>>					fileNames					= {};
		::gpk::pathList(app.SavegameFolder, fileNames, app.ExtensionSaveAuto);
		if(fileNames.size())
			::ghg::solarSystemLoad(app.Game, fileNames[0]);

		fileNames					= {};
		::gpk::pathList(app.SavegameFolder, fileNames, app.ExtensionProfile);
		app.Players.resize(fileNames.size());
		for(uint32_t iPlayer = 0; iPlayer < fileNames.size(); ++iPlayer) {
			gpk_necall(app.Players[iPlayer].Load(fileNames[iPlayer]), "fileNames[iPlayer]: %s", fileNames[iPlayer].begin());
		}
		::gpk::tunerSetValue(*app.TunerPlayerCount, app.Game.PlayState.PlayerCount);
		for(uint32_t iPilot = 0; iPilot < app.Game.PlayState.PlayerCount; ++iPilot) {
			const ::gpk::vcc namePilot	= app.Game.Pilots[iPilot].Name;
			for(uint32_t iPlayer = 0; iPlayer < app.Game.PlayState.PlayerCount; ++iPlayer) {
				if(iPlayer >= app.Players.size())
					app.Players.push_back({namePilot});

				const ::gpk::vcc namePlayer	= app.Players[iPlayer].Name;
				if(namePilot == namePlayer) {
					::std::swap(app.Players[iPlayer], app.Players[iPilot]);
					break;
				}
			}
			for(uint32_t iPlayer = app.Game.PlayState.PlayerCount; iPlayer < app.Players.size(); ++iPlayer) {
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

	if(controllerPlayer.size() == 1 || (controllerPlayer.size() > 1 && app.Game.PlayState.PlayerCount == 1)) {
		controllerPlayer[0].Forward					= input.KeyboardCurrent.KeyState[VK_UP		] || input.KeyboardCurrent.KeyState['W'];
		controllerPlayer[0].Back					= input.KeyboardCurrent.KeyState[VK_DOWN	] || input.KeyboardCurrent.KeyState['S'];
		controllerPlayer[0].Left					= input.KeyboardCurrent.KeyState[VK_LEFT	] || input.KeyboardCurrent.KeyState['A'];
		controllerPlayer[0].Right					= input.KeyboardCurrent.KeyState[VK_RIGHT	] || input.KeyboardCurrent.KeyState['D'];
		controllerPlayer[0].Turbo					= input.KeyboardCurrent.KeyState[VK_RCONTROL] || input.KeyboardCurrent.KeyState[VK_CONTROL] || input.KeyboardCurrent.KeyState[VK_LSHIFT] || input.KeyboardCurrent.KeyState[VK_SHIFT];
	}
	else {
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
	}

	::ghg::solarSystemUpdate(app.Game, (false == inGame) ? 0 : lastTimeSeconds, *inputState, systemEvents);
	for(uint32_t iShip = 0; iShip < app.Game.ShipState.ShipOrbiterActionQueue.size(); ++iShip)
		if(iShip < app.Game.PlayState.PlayerCount) {
			for(uint32_t iEvent = 0; iEvent < app.Game.ShipState.ShipOrbiterActionQueue[iShip].size(); ++iEvent)
				if(app.Game.ShipState.ShipOrbiterActionQueue[iShip][iEvent] == ::ghg::SHIP_ACTION_spawn) {
					::gpk::array_pod<byte_t>			serialized;
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

	::gpk::ptr_obj<::ghg::TRenderTarget>		target	= {};
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

	::gpk::view_grid<::gpk::SColorBGRA>							targetPixels			= target->Color.View;
	::gpk::view_grid<uint32_t>									depthBuffer				= target->DepthStencil.View;
	switch(app.ActiveState) {
	default					: 
	case APP_STATE_Play		: 
		::ghg::solarSystemDraw(app.Game, app.Game.DrawCache, app.Game.LockUpdate);
		::gpk::ptr_obj<::ghg::TRenderTarget>		sourceRT	= {};
		sourceRT								= app.Game.DrawCache.RenderTarget;
		if(!sourceRT)
			break;

		::gpk::view_grid<::gpk::SColorBGRA>			cameraView			= sourceRT->Color.View;
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

::gpk::error_t					ghg::listFilesSavegame		(::ghg::SGalaxyHellApp & app, const ::gpk::vcc & saveGameFolder, ::gpk::array_obj<::gpk::vcc> & savegameFilenames) {
	::gpk::array_obj<::gpk::array_pod<char>>	fileNames;
	gpk_necall(::gpk::pathList(saveGameFolder, fileNames, app.ExtensionSave), "%s", saveGameFolder.begin());

	::gpk::array_obj<::gpk::vcc>				pathFileNames;
	for(uint32_t iFile = 0; iFile < fileNames.size(); ++iFile)
		pathFileNames.push_back(fileNames[iFile]);

	::gpk::array_obj<::gpk::vcc>				pathFileNamesSaveAuto;
	::gpk::array_obj<::gpk::vcc>				pathFileNamesSaveUser;
	::gpk::array_obj<::gpk::vcc>				pathFileNamesSaveStage;
	::gpk::filterPostfix(pathFileNames, app.ExtensionSaveAuto	, pathFileNamesSaveAuto);
	::gpk::filterPostfix(pathFileNames, app.ExtensionSaveUser	, pathFileNamesSaveUser);
	::gpk::filterPostfix(pathFileNames, app.ExtensionSaveStage	, pathFileNamesSaveStage);
		
	for(uint32_t iPath = 0; iPath < pathFileNamesSaveAuto .size(); ++iPath) savegameFilenames.push_back(::gpk::label(pathFileNamesSaveAuto [iPath]));
	for(uint32_t iPath = 0; iPath < pathFileNamesSaveUser .size(); ++iPath) savegameFilenames.push_back(::gpk::label(pathFileNamesSaveUser [iPath]));
	for(uint32_t iPath = 0; iPath < pathFileNamesSaveStage.size(); ++iPath) savegameFilenames.push_back(::gpk::label(pathFileNamesSaveStage[iPath]));
	return 0;
}

::gpk::error_t					ghg::listFilesProfile		(::ghg::SGalaxyHellApp & app, const ::gpk::vcc & saveGameFolder, ::gpk::array_obj<::gpk::vcc> & savegameFilenames) {
	::gpk::array_obj<::gpk::array_pod<char>>	fileNames;
	gpk_necall(::gpk::pathList(saveGameFolder, fileNames, app.ExtensionProfile), "%s", saveGameFolder.begin());

	::gpk::array_obj<::gpk::vcc>				pathFileNames;
	for(uint32_t iFile = 0; iFile < fileNames.size(); ++iFile)
		savegameFilenames.push_back(::gpk::label(::gpk::vcc{fileNames[iFile]}));
	return 0;
}