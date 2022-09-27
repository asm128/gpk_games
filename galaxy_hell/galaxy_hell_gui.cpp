#include "gpk_galaxy_hell_app.h"
#include "gpk_storage.h"
#include "gpk_raster_lh.h"
#include <gpk_grid_copy.h>

static constexpr	::gpk::GUI_COLOR_MODE		GHG_MENU_COLOR_MODE		= ::gpk::GUI_COLOR_MODE_3D;
static constexpr	bool						BACKGROUND_3D_ONLY		= true;
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_CAMERA_SIZE		= {64, 64};
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_VIEWPORT_SIZE	= {128, 64};
static constexpr	::gpk::SCoord2<uint16_t>	WEAPON_BAR_SIZE			= {96, 20};

static	::gpk::error_t			guiSetupCommon				(::gpk::SGUI & gui) {
	gui.ColorModeDefault			= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault				= ::gpk::ASCII_COLOR_DARKRED * 16 + 10;
	gui.SelectedFont				= 7;
	return 0;
}

static	::gpk::error_t			guiSetupButtonList			(::gpk::SGUI & gui, uint16_t buttonWidth, int16_t yOffset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::view_array<::gpk::vcc> buttonText) {
	for(uint16_t iButton = 0; iButton < buttonText.size(); ++iButton) {
		int32_t								idControl						= ::gpk::controlCreate(gui);
		::gpk::SControl						& control						= gui.Controls.Controls[idControl];
		control.Area					= {{0U, (int16_t)(20*iButton + yOffset)}, {(int16_t)buttonWidth, 20}};
		control.Border					= {1, 1, 1, 1};//{10, 10, 10, 10};
		control.Margin					= {1, 1, 1, 1};
		control.Align					= controlAlign;
		::gpk::SControlText					& controlText					= gui.Controls.Text[idControl];
		controlText.Text				= buttonText[iButton];
		controlText.Align				= textAlign;
		::gpk::controlSetParent(gui, idControl, -1);
	}
	return 0;
}

template<typename _tUIEnum>
static	::gpk::error_t			guiSetupButtonList			(::gpk::SGUI & gui, uint16_t buttonWidth, int16_t yOffset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign = ::gpk::ALIGN_CENTER) {
	::gpk::view_array<::gpk::vcc>		labels						= ::gpk::get_value_labels<_tUIEnum>();
	return ::guiSetupButtonList(gui, buttonWidth, yOffset, controlAlign, textAlign, labels);
}

//static	::gpk::error_t			guiSetupInit				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_LOAD>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupLoad				(::gpk::SDialog & dialog) { 
	gpk_necall(guiSetupButtonList<::ghg::UI_LOAD>(*dialog.GUI,  60, 0, ::gpk::ALIGN_CENTER), "%s", ""); 
	return 0;
}

static	::gpk::error_t			guiSetupHome				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_HOME>(*dialog.GUI, 160, int16_t(-20 * ::gpk::get_value_count<::ghg::UI_HOME>() / 2), ::gpk::ALIGN_CENTER); }
static	::gpk::error_t			guiSetupProfile				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupShop				(::gpk::SDialog & dialog) { 
	::gpk::SGUI							& gui						= *dialog.GUI;
	gpk_necall(guiSetupButtonList<::ghg::UI_SHOP>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT), "%s", "");
	::gpk::ptr_obj<::gpk::SDialogViewport>	viewport									= {};
	//int32_t									idViewport					=
	::gpk::viewportCreate(dialog, viewport);
	//viewport->Settings.DisplacementLockX	= false;;
	//viewport->Settings.DisplacementLockY	= false;;
	gui.Controls.Controls	[viewport->IdGUIControl	].Align					= ::gpk::ALIGN_CENTER;
	gui.Controls.Controls	[viewport->IdGUIControl	].Area.Offset			= {};
	gui.Controls.Controls	[viewport->IdGUIControl	].Area.Size				= {640, 480};
	gui.Controls.States		[viewport->IdClient		].ImageInvertY			= true;
	return 0;
}
static	::gpk::error_t			guiSetupPlay				(::ghg::SGalaxyHellApp & /*app*/, ::gpk::SDialog & dialog) { 
	guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  220, 0, ::gpk::ALIGN_TOP_RIGHT); 
	return 0;
}
static	::gpk::error_t			guiSetupBrief				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupStage				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupStats				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupStore				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupScore				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
//static	::gpk::error_t			guiSetupSetup				(::gpk::SGUI & gui) { return guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); }
static	::gpk::error_t			guiSetupSettings			(::gpk::SDialog & dialog) { 
	dialog.GUI->SelectedFont		= 7;
	::gpk::SGUI							& gui						= *dialog.GUI;
	guiSetupButtonList<::ghg::UI_SETTINGS>(gui,  160, 0, ::gpk::ALIGN_CENTER); 
	return 0;
}

static	::gpk::error_t			guiSetupAbout				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_CREDITS>(*dialog.GUI,  160, int16_t(-20 * ::gpk::get_value_count<::ghg::UI_CREDITS>() / 2), ::gpk::ALIGN_CENTER); }

static	::gpk::error_t			virtualKeyboardSetup		(::gpk::SDialog & dialog, ::gpk::SVirtualKeyboard & vk) { 
	::gpk::array_pod<uint16_t> keys;
	for(uint16_t i = 1; i < 255; ++i) {
		keys.push_back(i);
	}
	keys.push_back('!');
	keys.push_back('?');

	gpk_necs(::gpk::virtualKeyboardSetup(*dialog.GUI, vk, 32, ::gpk::view_const_uint16{keys}));
	dialog.GUI->Controls.States[vk.IdRoot].Hidden	= true;
	return 0;
}

static	::gpk::error_t			dialogCreateCommon			(::gpk::SDialog & dialogLoad, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::SCoord2<float> & cursorPos) { 
	dialogLoad						= {};
	dialogLoad.Input				= inputState;
	dialogLoad.GUI->CursorPos		= cursorPos;
	dialogLoad.GUI->Controls.Modes[0].NoBackgroundRect	= true;
	::guiSetupCommon(*dialogLoad.GUI);
	dialogLoad.Update();
	return 0;
}

::gpk::error_t					ghg::guiSetup				(::ghg::SGalaxyHellApp & app, const ::gpk::ptr_obj<::gpk::SInput> & input) {
	::dialogCreateCommon(app.DialogDesktop, input, {});

	for(uint32_t iGUI = 0; iGUI < ::gpk::size(app.DialogPerState); ++iGUI) {
		::dialogCreateCommon(app.DialogPerState[iGUI], input, {});
	};

	::guiSetupLoad		(app.DialogPerState[::ghg::APP_STATE_Load		]);
	::guiSetupHome		(app.DialogPerState[::ghg::APP_STATE_Home		]);
	::guiSetupShop		(app.DialogPerState[::ghg::APP_STATE_Shop		]);
	::guiSetupProfile	(app.DialogPerState[::ghg::APP_STATE_Profile	]);
	::guiSetupPlay		(app, app.DialogPerState[::ghg::APP_STATE_Play		]);
	::guiSetupBrief		(app.DialogPerState[::ghg::APP_STATE_Brief		]);
	::guiSetupStage		(app.DialogPerState[::ghg::APP_STATE_Stage		]);
	::guiSetupStats		(app.DialogPerState[::ghg::APP_STATE_Stats		]);
	::guiSetupStore		(app.DialogPerState[::ghg::APP_STATE_Store		]);
	::guiSetupScore		(app.DialogPerState[::ghg::APP_STATE_Score		]);
	::guiSetupSettings	(app.DialogPerState[::ghg::APP_STATE_Settings	]);
	::guiSetupAbout		(app.DialogPerState[::ghg::APP_STATE_About		]);

	::virtualKeyboardSetup(app.DialogDesktop, app.VirtualKeyboard);
	return 0;
}

static	::gpk::error_t			guiHandleLoad				(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & gui, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl < (gui.Controls.Text.size() - 2)) {
		gerror_if(0 > app.Load(gui.Controls.Text[idControl + 1].Text), "%s", gui.Controls.Text[idControl + 1].Text.begin());
	}
	return ::ghg::APP_STATE_Home; 
}

static	::gpk::error_t			dialogCreateLoad			(::gpk::SDialog & dialogLoad, ::gpk::view_array<::gpk::vcc> pathFileNames, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::SCoord2<float> & cursorPos) { 
	::dialogCreateCommon(dialogLoad, inputState, cursorPos);
	return ::guiSetupButtonList(*dialogLoad.GUI, 256, (int16_t)(-20 * pathFileNames.size() / 2), ::gpk::ALIGN_CENTER, ::gpk::ALIGN_LEFT, pathFileNames);
}

static	::gpk::error_t			guiHandleHome				(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & game) { 
	switch((::ghg::UI_HOME)idControl) {
	case ::ghg::UI_HOME_Start: 
		::ghg::solarSystemReset(game);
		game.PlayState.Paused			= false;
		return ::ghg::APP_STATE_Play;
	case ::ghg::UI_HOME_Continue: 
		game.PlayState.Paused			= false;
		return ::ghg::APP_STATE_Play;
	case ::ghg::UI_HOME_Save: {
		gerror_if(0 > app.Save(::ghg::SAVE_MODE_USER), "%s", "");

		break;
	}
	case ::ghg::UI_HOME_Load: {
		::gpk::array_obj<::gpk::vcc>	pathFileNames;
		app.FileNames				= {};
		::gpk::pathList(app.SavegameFolder, app.FileNames);
		for(uint32_t iFile = 0; iFile < app.FileNames.size(); ++iFile)  {
			const ::gpk::vcc				fileName			= app.FileNames[iFile];
			if(fileName.size() < app.ExtensionSaveAuto.size())
				continue;
			if(0 == strcmp(&fileName[fileName.size() - (uint32_t)app.ExtensionSaveAuto.size()], app.ExtensionSaveAuto.begin())) 
				pathFileNames.push_back(::gpk::label(fileName));
		}
		for(uint32_t iFile = 0; iFile < app.FileNames.size(); ++iFile)  {
			const ::gpk::vcc				fileName			= app.FileNames[iFile];
			if(fileName.size() < app.ExtensionSaveStage.size())
				continue;
			if(0 == strcmp(&fileName[fileName.size() - (uint32_t)app.ExtensionSaveStage.size()], app.ExtensionSaveStage.begin())) 
				pathFileNames.push_back(fileName);
		}
		for(uint32_t iFile = 0; iFile < app.FileNames.size(); ++iFile)  {
			const ::gpk::vcc				fileName			= app.FileNames[iFile];
			if(fileName.size() < app.ExtensionSaveUser.size())
				continue;
			if(0 == strcmp(&fileName[fileName.size() - (uint32_t)app.ExtensionSaveUser.size()], app.ExtensionSaveUser.begin())) 
				pathFileNames.push_back(fileName);
		}
		pathFileNames.push_back(::gpk::vcs{"Back"});
		dialogCreateLoad(app.DialogPerState[::ghg::APP_STATE_Load], pathFileNames, app.DialogPerState[::ghg::APP_STATE_Home].Input, app.DialogPerState[::ghg::APP_STATE_Home].GUI->CursorPos);
		return ::ghg::APP_STATE_Load;
	}
	case ::ghg::UI_HOME_Settings: {
		return ::ghg::APP_STATE_Settings;
	}
	case ::ghg::UI_HOME_Credits: {
		return ::ghg::APP_STATE_About;
	}
	case ::ghg::UI_HOME_Shop: {
		return ::ghg::APP_STATE_Shop;
	}
	case ::ghg::UI_HOME_Exit: 
		game.PlayState.Paused						= true;
		return ::ghg::APP_STATE_Quit;
	}
	return ::ghg::APP_STATE_Home; 
}

static	::gpk::error_t			guiHandleUser				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/) { return ::ghg::APP_STATE_Profile; }
static	::gpk::error_t			guiHandleShop				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (uint32_t)::ghg::UI_SHOP_Back) {
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Shop; 
}
static	::gpk::error_t			guiHandlePlay				(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (uint32_t)::ghg::UI_PLAY_Menu) {
		app.Save(::ghg::SAVE_MODE_AUTO);
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Play; 
}

static	::gpk::error_t			guiHandleBrief				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/) { return ::ghg::APP_STATE_Brief; }
static	::gpk::error_t			guiHandleStage				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/) { return ::ghg::APP_STATE_Stage; }
static	::gpk::error_t			guiHandleStats				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/) { return ::ghg::APP_STATE_Stats; }
static	::gpk::error_t			guiHandleStore				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/) { return ::ghg::APP_STATE_Store; }
static	::gpk::error_t			guiHandleScore				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/) { return ::ghg::APP_STATE_Score; }
static	::gpk::error_t			guiHandleSetup				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (uint32_t)::ghg::UI_SETTINGS_Audio) {
		return ::ghg::APP_STATE_Home;
	}
	else if(idControl == (uint32_t)::ghg::UI_SETTINGS_Graphics) {
		return ::ghg::APP_STATE_Home;
	}
	else if(idControl == (uint32_t)::ghg::UI_SETTINGS_Controller) {
		return ::ghg::APP_STATE_Home;
	}
	else if(idControl == (uint32_t)::ghg::UI_SETTINGS_Back) {
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Settings; 
}
static	::gpk::error_t			guiHandleAbout				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (uint32_t)::ghg::UI_CREDITS_Back || idControl == (uint32_t)::ghg::UI_CREDITS_Home) {
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_About; 
}

template<size_t _nStorageSize>
::gpk::error_t					sprintfTime					(const char *prefix, char (&dest)[_nStorageSize], double seconds) {
	uint32_t							timeHours					= (int)seconds / 3600;
	uint32_t							timeMinutes					= (int)seconds / 60 % 60;
	uint32_t							timeSeconds					= (int)seconds % 60;
	uint32_t							timeCents					= int (seconds * 10) % 10;
	sprintf_s(dest, "%s%i:%.2i:%.2i.%.2i", prefix, timeHours, timeMinutes, timeSeconds, timeCents);
	return 0;
}

static	::gpk::error_t			guiUpdatePlay				(::ghg::SGalaxyHellApp & app) { 
	::ghg::SGalaxyHell					& game						= app.Game;

	::gpk::SDialog						& dialog					= app.DialogPerState[::ghg::APP_STATE_Play];
	::gpk::SGUI							& gui						= *dialog.GUI;

	sprintf_s	(app.UIPlay.TextLevel		.Storage, "Level: %i", game.PlayState.Stage);
	sprintfTime	("Real Time: "		, app.UIPlay.TextTimeReal	.Storage, game.PlayState.TimeReal);
	sprintfTime	("Simulated Time: "	, app.UIPlay.TextTimeWorld	.Storage, game.PlayState.TimeWorld);
	sprintfTime	("Stage Time: "		, app.UIPlay.TextTimeStage	.Storage, game.PlayState.TimeRealStage);
	::gpk::controlTextSet(gui, 1 + ::ghg::UI_PLAY_Level		, ::gpk::vcs{app.UIPlay.TextLevel		.Storage});
	::gpk::controlTextSet(gui, 1 + ::ghg::UI_PLAY_TimeStage , ::gpk::vcs{app.UIPlay.TextTimeStage	.Storage});
	::gpk::controlTextSet(gui, 1 + ::ghg::UI_PLAY_TimeWorld , ::gpk::vcs{app.UIPlay.TextTimeWorld	.Storage});
	::gpk::controlTextSet(gui, 1 + ::ghg::UI_PLAY_TimeReal	, ::gpk::vcs{app.UIPlay.TextTimeReal	.Storage});

	if(0 == game.ShipState.ShipCores.size())
		return 0;

	if(::ghg::APP_STATE_Play != app.ActiveState)
		return 0;

	// Setup new viewports if necessary. This may happen if the ship acquires new modules while playing the stage
	if(app.UIPlay.PlayerUIs.size() != game.PlayState.PlayerCount) {
		app.UIPlay.PlayerUIs.clear();
		app.UIPlay.PlayerUIs.resize(game.PlayState.PlayerCount);
		for(uint32_t iPlayer = 0; iPlayer < game.PlayState.PlayerCount; ++iPlayer) {
			::ghg::SUIPlayer	& uiPlayer		= app.UIPlay.PlayerUIs[iPlayer];
			
			::gpk::SDialog		& playerDialog	= uiPlayer.Dialog;
			::dialogCreateCommon(playerDialog, app.DialogPerState[0].Input, app.DialogPerState[0].GUI->CursorPos);

			::gpk::SGUI			& playerGUI		= *playerDialog.GUI;
			::guiSetupButtonList<::ghg::UI_PLAYER>(playerGUI, 240, 0, iPlayer ? ::gpk::ALIGN_BOTTOM_RIGHT : ::gpk::ALIGN_BOTTOM_LEFT, ::gpk::ALIGN_LEFT);

			for(uint32_t iOrbiter = 0, countViewports = game.ShipState.ShipParts[iPlayer].size(); iOrbiter < countViewports; ++iOrbiter) {
				int32_t								indexVP				= uiPlayer.ModuleViewports.push_back({});
				uiPlayer.ModuleViewports[indexVP]->Viewport = ::gpk::controlCreate(playerGUI); // Orbiter
				::gpk::controlCreate(playerGUI); // Weapon Load
				::gpk::controlCreate(playerGUI); // Weapon Type

				::ghg::SUIPlayModuleViewport		& viewport			= *uiPlayer.ModuleViewports[indexVP];
				viewport.RenderTargetOrbiter.resize(::MODULE_CAMERA_SIZE);
				viewport.RenderTargetWeaponLoad.resize(::WEAPON_BAR_SIZE);
				viewport.RenderTargetWeaponType.resize(::WEAPON_BAR_SIZE);
				::gpk::SControl						& control			= playerGUI.Controls.Controls [viewport.Viewport];
				control.Image					= viewport.RenderTargetOrbiter.Color.View;
				playerGUI.Controls.Controls [viewport.Viewport + 1].Image = viewport.RenderTargetWeaponLoad.Color.View;
				playerGUI.Controls.Controls [viewport.Viewport + 2].Image = viewport.RenderTargetWeaponType.Color.View;
				control.ImageAlign				= playerGUI.Controls.Controls [viewport.Viewport + 1].ImageAlign	= playerGUI.Controls.Controls [viewport.Viewport + 2].ImageAlign	= ::gpk::ALIGN_CENTER;
				control.Align					= playerGUI.Controls.Controls [viewport.Viewport + 1].Align			= playerGUI.Controls.Controls [viewport.Viewport + 2].Align			= iPlayer ? ::gpk::ALIGN_CENTER_RIGHT : ::gpk::ALIGN_CENTER_LEFT;
				control.Area.Offset				= playerGUI.Controls.Controls [viewport.Viewport + 1].Area.Offset	= playerGUI.Controls.Controls [viewport.Viewport + 2].Area.Offset	= {0, int16_t(-(int16_t(countViewports * (MODULE_CAMERA_SIZE.y + 4)) >> 1) + (MODULE_CAMERA_SIZE.y + 4) * iOrbiter)};
				control.Area.Size				= MODULE_CAMERA_SIZE.Cast<int16_t>();
				control.Border					= {};
				control.Margin					= {};

				playerGUI.Controls.Controls [viewport.Viewport + 1].Area.Size	= playerGUI.Controls.Controls [viewport.Viewport + 2].Area.Size		= WEAPON_BAR_SIZE.Cast<int16_t>();	//MODULE_VIEWPORT_SIZE.Cast<int16_t>();
				playerGUI.Controls.Controls [viewport.Viewport + 1].Border		= playerGUI.Controls.Controls [viewport.Viewport + 2].Border		= {2, 2, 2, 2};
				playerGUI.Controls.Controls [viewport.Viewport + 1].Margin		= playerGUI.Controls.Controls [viewport.Viewport + 2].Margin		= {};
				playerGUI.Controls.Controls [viewport.Viewport + 1].Area.Offset.y	-= WEAPON_BAR_SIZE.y / 2 + 1;
				playerGUI.Controls.Controls [viewport.Viewport + 2].Area.Offset.y	+= WEAPON_BAR_SIZE.y / 2 + 1;

				playerGUI.Controls.Controls [viewport.Viewport + 1].Area.Offset.x	+= 
				playerGUI.Controls.Controls [viewport.Viewport + 2].Area.Offset.x	+= MODULE_CAMERA_SIZE.x + 1;

				playerGUI.Controls.Text		[viewport.Viewport + 1].Align			= ::gpk::ALIGN_CENTER; //iPlayer ? ::gpk::ALIGN_CENTER_RIGHT : ::gpk::ALIGN_CENTER_LEFT;
				playerGUI.Controls.Text		[viewport.Viewport + 2].Align			= ::gpk::ALIGN_CENTER; //iPlayer ? ::gpk::ALIGN_CENTER_RIGHT : ::gpk::ALIGN_CENTER_LEFT;

				playerGUI.Controls.Modes	[viewport.Viewport + 0].NoBackgroundRect		= true;
				playerGUI.Controls.Modes	[viewport.Viewport + 1].NoBackgroundRect		= true;
				playerGUI.Controls.Modes	[viewport.Viewport + 2].NoBackgroundRect		= true;

				::gpk::SMatrix4<float>						& matrixProjection				= viewport.MatrixProjection;
				matrixProjection.FieldOfView(::gpk::math_pi * .25, MODULE_CAMERA_SIZE.x / (double)MODULE_CAMERA_SIZE.y, 0.01, 500);
				::gpk::SMatrix4<float>						matrixViewport					= {};
				matrixViewport.ViewportLH(MODULE_CAMERA_SIZE.Cast<uint32_t>());
				matrixProjection						*= matrixViewport;


				const ::gpk::SCircle<float> circleLife		= {::gpk::min(MODULE_CAMERA_SIZE.x, MODULE_CAMERA_SIZE.y) * .5f - 6 * 0, viewport.RenderTargetOrbiter.Color.View.metrics().Cast<float>() * .5};
				const ::gpk::SCircle<float> circleDelay		= {::gpk::min(MODULE_CAMERA_SIZE.x, MODULE_CAMERA_SIZE.y) * .5f - 6 * 1, viewport.RenderTargetOrbiter.Color.View.metrics().Cast<float>() * .5};
				const ::gpk::SCircle<float> circleCooldown	= {::gpk::min(MODULE_CAMERA_SIZE.x, MODULE_CAMERA_SIZE.y) * .5f - 6 * 2, viewport.RenderTargetOrbiter.Color.View.metrics().Cast<float>() * .5};
		
				::ghg::gaugeBuildRadial(viewport.GaugeLife		, circleLife		, 32, 12);
			}
			playerDialog.Update();
		}
	}
	::ghg::SGalaxyHellDrawCache								& drawCache				= app.UIPlay.DrawCache;
	drawCache.LightPointsWorld.clear();
	drawCache.LightColorsWorld.clear();
	::ghg::getLightArraysFromShips(game.ShipState, drawCache.LightPointsWorld, drawCache.LightColorsWorld);
	drawCache.LightPointsModel.reserve(drawCache.LightPointsWorld.size());
	drawCache.LightColorsModel.reserve(drawCache.LightColorsWorld.size());
	for(uint32_t iPlayer = 0; iPlayer < game.PlayState.PlayerCount; ++iPlayer) {
		::ghg::SUIPlayer	& uiPlayer = app.UIPlay.PlayerUIs[iPlayer];
		sprintf_s(uiPlayer.TextScore			.Storage, "Score           : %llu"	, game.ShipState.ShipScores[iPlayer].Score			);
		sprintf_s(uiPlayer.TextHits				.Storage, "Hits            : %llu"	, game.ShipState.ShipScores[iPlayer].Hits			);
		sprintf_s(uiPlayer.TextDamageDone		.Storage, "Damage dealt    : %llu"	, game.ShipState.ShipScores[iPlayer].DamageDone		);
		sprintf_s(uiPlayer.TextDamageReceived	.Storage, "Damage received : %llu"	, game.ShipState.ShipScores[iPlayer].DamageReceived	);
		sprintf_s(uiPlayer.TextHitsSurvived		.Storage, "Hits survived   : %llu"	, game.ShipState.ShipScores[iPlayer].HitsSurvived	);
		sprintf_s(uiPlayer.TextOrbitersLost		.Storage, "Orbiters lost   : %u"	, game.ShipState.ShipScores[iPlayer].OrbitersLost	);
		sprintf_s(uiPlayer.TextKilledShips		.Storage, "Killed ships    : %u"	, game.ShipState.ShipScores[iPlayer].KilledShips	);
		sprintf_s(uiPlayer.TextKilledOrbiters	.Storage, "Killed orbiters : %u"	, game.ShipState.ShipScores[iPlayer].KilledOrbiters	);

		::gpk::controlTextSet(*uiPlayer.Dialog.GUI, 1 +::ghg::UI_PLAYER_Score			, uiPlayer.TextScore			.Storage);
		::gpk::controlTextSet(*uiPlayer.Dialog.GUI, 1 +::ghg::UI_PLAYER_Hits			, uiPlayer.TextHits				.Storage);
		::gpk::controlTextSet(*uiPlayer.Dialog.GUI, 1 +::ghg::UI_PLAYER_DamageDone		, uiPlayer.TextDamageDone		.Storage);
		::gpk::controlTextSet(*uiPlayer.Dialog.GUI, 1 +::ghg::UI_PLAYER_DamageReceived	, uiPlayer.TextDamageReceived	.Storage);
		::gpk::controlTextSet(*uiPlayer.Dialog.GUI, 1 +::ghg::UI_PLAYER_HitsSurvived	, uiPlayer.TextHitsSurvived		.Storage);
		::gpk::controlTextSet(*uiPlayer.Dialog.GUI, 1 +::ghg::UI_PLAYER_OrbitersLost	, uiPlayer.TextOrbitersLost		.Storage);
		::gpk::controlTextSet(*uiPlayer.Dialog.GUI, 1 +::ghg::UI_PLAYER_KilledShips		, uiPlayer.TextKilledShips		.Storage);
		::gpk::controlTextSet(*uiPlayer.Dialog.GUI, 1 +::ghg::UI_PLAYER_KilledOrbiters	, uiPlayer.TextKilledOrbiters	.Storage);

		::gpk::SDialog		& playerDialog	= uiPlayer.Dialog;
		::gpk::SGUI			& playerGUI		= *playerDialog.GUI;
		for(uint32_t iOrbiter = 0, countViewports = game.ShipState.ShipParts[iPlayer].size(); iOrbiter < countViewports; ++iOrbiter) {
			::ghg::SUIPlayModuleViewport		& viewport			= *uiPlayer.ModuleViewports[iOrbiter];

			const ::ghg::SOrbiter				& orbiter			= game.ShipState.Orbiters[game.ShipState.ShipParts[iPlayer][iOrbiter]];
			const ::ghg::SWeapon				& weapon			= game.ShipState.Weapons[orbiter.Weapon];

			gpk_necall(::gpk::controlTextSet(playerGUI, viewport.Viewport + 1, ::gpk::get_value_label(weapon.Load)), "%s", "");
			gpk_necall(::gpk::controlTextSet(playerGUI, viewport.Viewport + 2, ::gpk::get_value_label(weapon.Type)), "%s", "");

			const float							healthRatio			= ::gpk::clamp(orbiter.Health	/ float(orbiter.MaxHealth), 0.0f, 1.0f);
			const float							ratioOverheat		= (orbiter.Health > 0) ? ::gpk::clamp(weapon.Overheat	/ float(weapon.Cooldown), 0.0f, 1.0f) : 0;
			const float							ratioDelay			= (orbiter.Health > 0) ? ::gpk::clamp(weapon.Delay		/ float(weapon.MaxDelay), 0.0f, 1.0f) : 0;

			const ::gpk::SColorFloat colorLife		= ::gpk::interpolate_linear(::gpk::RED, ::gpk::GREEN, healthRatio);
			const ::gpk::SColorFloat colorDelay		= ::gpk::interpolate_linear(::gpk::GRAY * .5, ::gpk::YELLOW, ratioDelay);;
			const ::gpk::SColorFloat colorCooldown	= ::gpk::interpolate_linear(::gpk::LIGHTBLUE * ::gpk::CYAN, ::gpk::ORANGE * .5 + ::gpk::RED * .5, ratioOverheat);

			viewport.GaugeLife		.SetValue(healthRatio	);

			{
				::gpk::SMatrix4<float>									matrixView				= {};
				::gpk::SCamera											& camera				= viewport.Camera;
				camera.Target										= game.ShipState.Scene.Transforms[game.ShipState.EntitySystem.Entities[orbiter.Entity + 1].Transform].GetTranslation();
				camera.Position										= camera.Target;
				camera.Position.y									+= 7; //-= 20;
				camera.Up											= {1, 0, 0};
				matrixView.LookAt(camera.Position, camera.Target, camera.Up);
				matrixView											*= uiPlayer.ModuleViewports[iOrbiter]->MatrixProjection;

				::ghg::TRenderTarget				& renderTarget		= viewport.RenderTargetOrbiter;
				::gpk::view_grid<::gpk::SColorBGRA>	targetPixels		= renderTarget.Color		; 
				::gpk::view_grid<uint32_t>			depthBuffer			= renderTarget.DepthStencil	;
				//targetPixels.fill(tone);
				memset(targetPixels.begin(), 0, targetPixels.byte_count());
				memset(depthBuffer.begin(), -1, depthBuffer.byte_count());

				drawCache.PixelCoords.clear();
				uint32_t												pixelsDrawn				= 0;
				pixelsDrawn											+= ::ghg::drawShipOrbiter(game.ShipState, orbiter, matrixView, targetPixels, depthBuffer, drawCache);

				if(healthRatio		) ::ghg::gaugeImageUpdate(viewport.GaugeLife		, targetPixels, colorLife		, colorLife		, colorLife		);
			}
			 {
				::ghg::TRenderTarget				& renderTarget		= viewport.RenderTargetWeaponLoad;
				::gpk::view_grid<::gpk::SColorBGRA>	targetPixels		= renderTarget.Color		; 
				::gpk::view_grid<uint32_t>			depthBuffer			= renderTarget.DepthStencil	;
				memset(targetPixels.begin(), 0, targetPixels.byte_count());
				memset(depthBuffer.begin(), -1, depthBuffer.byte_count());

				if(ratioDelay) {
					drawCache.PixelCoords.clear();
					for(uint32_t iLine = 0; iLine < targetPixels.metrics().y; ++iLine)
						::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{{0, (int16_t)iLine}, {int16_t((targetPixels.metrics().x - 1) * ratioDelay), (int16_t)iLine}}, drawCache.PixelCoords);
				
					for(uint32_t iPixel = 0; iPixel < drawCache.PixelCoords.size(); ++iPixel) {
						const ::gpk::SCoord2<int16_t>		& pixelCoord = drawCache.PixelCoords[iPixel];
						const ::gpk::SCoord2<float>			floatCoord						= pixelCoord.Cast<float>();
						const double						distanceFromCenter				= fabs((floatCoord.y / (targetPixels.metrics().y)) - .5) * 2.0;
						::gpk::setPixel(targetPixels, pixelCoord, ::gpk::interpolate_linear(colorDelay, ::gpk::GRAY * .25, distanceFromCenter));
					}
				}
			}

			{
				::ghg::TRenderTarget				& renderTarget		= viewport.RenderTargetWeaponType;
				::gpk::view_grid<::gpk::SColorBGRA>	targetPixels		= renderTarget.Color		; 
				::gpk::view_grid<uint32_t>			depthBuffer			= renderTarget.DepthStencil	;
				memset(targetPixels.begin(), 0, targetPixels.byte_count());
				memset(depthBuffer.begin(), -1, depthBuffer.byte_count());
				if(ratioOverheat) {
					drawCache.PixelCoords.clear();
					for(uint32_t iLine = 0; iLine < targetPixels.metrics().y; ++iLine)
						::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{{0, (int16_t)iLine}, {int16_t((targetPixels.metrics().x - 1) * ratioOverheat), (int16_t)iLine}}, drawCache.PixelCoords);
				
					for(uint32_t iPixel = 0; iPixel < drawCache.PixelCoords.size(); ++iPixel) {
						const ::gpk::SCoord2<int16_t>		& pixelCoord = drawCache.PixelCoords[iPixel];
						const ::gpk::SCoord2<float>			floatCoord						= pixelCoord.Cast<float>();
						const double						distanceFromCenter				= fabs((floatCoord.y / (targetPixels.metrics().y)) - .5) * 2.0;
						::gpk::setPixel(targetPixels, drawCache.PixelCoords[iPixel], ::gpk::interpolate_linear(colorCooldown, ::gpk::GRAY * .25, distanceFromCenter));
					}
				}
			}
		}
	}

	return 0;
}

::gpk::error_t					ghg::guiUpdate				(::ghg::SGalaxyHellApp & gameApp, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents) {
	{
		::gpk::SDialog						& dialog					= gameApp.DialogDesktop;
		::gpk::SGUI							& gui						= *dialog.GUI;
		::gpk::SInput						& input						= *dialog.Input;
		::gpk::guiProcessInput(gui, input, sysEvents); 

		dialog.Update();
		::gpk::array_pod<uint32_t>			controlsToProcess			= {};
		::gpk::guiGetProcessableControls(gui, controlsToProcess);

		gameApp.VirtualKeyboard.Events.clear();
		for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
			uint32_t							idControl			= controlsToProcess		[iControl];
			const ::gpk::SControlState			& controlState		= gui.Controls.States	[idControl];
			//bool								handled				= false;
			if(controlState.Execute) {
				::gpk::virtualKeyboardHandleEvent(gameApp.VirtualKeyboard, idControl);
			}
		}
	}

	::ghg::SGalaxyHell					& game						= gameApp.Game;

	::ghg::APP_STATE					appState					= gameApp.ActiveState; 
	for(uint32_t iAppState = 0; iAppState < ::ghg::APP_STATE_COUNT; ++iAppState) {
		::gpk::SDialog						& dialog					= gameApp.DialogPerState[iAppState];
		::gpk::SGUI							& gui						= *dialog.GUI;
		::gpk::SInput						& input						= *dialog.Input;
		::gpk::guiProcessInput(gui, input, sysEvents); 
	}

	for(uint32_t iPlayer = 0; iPlayer < gameApp.UIPlay.PlayerUIs.size(); ++iPlayer) {
		::ghg::SUIPlayer					& uiPlayer					= gameApp.UIPlay.PlayerUIs[iPlayer];
		::gpk::guiProcessInput(*uiPlayer.Dialog.GUI, *uiPlayer.Dialog.Input, sysEvents);
	}
	::gpk::SDialog						& dialog					= gameApp.DialogPerState[appState];
	dialog.Update();
	::gpk::SGUI							& gui						= *dialog.GUI;
	::gpk::array_pod<uint32_t>			controlsToProcess			= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);

	{
		::gpk::mutex_guard lock(gameApp.Game.LockUpdate);
		::guiUpdatePlay(gameApp);
	}

	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		uint32_t							idControl			= controlsToProcess		[iControl];
		const ::gpk::SControlState			& controlState		= gui.Controls.States	[idControl];
		bool								handled				= false;
		if(controlState.Execute) {
			idControl = idControl - 1;
			info_printf("Executed %u.", idControl);
			switch(appState) {
			case ::ghg::APP_STATE_Load	: appState = (::ghg::APP_STATE)::guiHandleLoad	(gameApp, gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Home	: appState = (::ghg::APP_STATE)::guiHandleHome	(gameApp, gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Profile	: appState = (::ghg::APP_STATE)::guiHandleUser	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Shop	: appState = (::ghg::APP_STATE)::guiHandleShop	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Play	: appState = (::ghg::APP_STATE)::guiHandlePlay	(gameApp, gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Brief	: appState = (::ghg::APP_STATE)::guiHandleBrief	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Stage	: appState = (::ghg::APP_STATE)::guiHandleStage	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Stats	: appState = (::ghg::APP_STATE)::guiHandleStats	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Store	: appState = (::ghg::APP_STATE)::guiHandleStore	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Score	: appState = (::ghg::APP_STATE)::guiHandleScore	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Settings	: appState = (::ghg::APP_STATE)::guiHandleSetup	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_About	: appState = (::ghg::APP_STATE)::guiHandleAbout	(gui, idControl, game); handled = true; break;
			}
			if(handled)
				break;
		}
	}
	return appState;
}


::gpk::error_t					ghg::gaugeBuildRadial			(::ghg::SUIControlGauge & gauge, const ::gpk::SCircle<float> & gaugeMetrics, int16_t resolution, int16_t width) {
	const double						stepUnit						= (1.0 / resolution) * ::gpk::math_2pi;
	const ::gpk::SSphere<float>			sphereMetrics					= {gaugeMetrics.Radius, {gaugeMetrics.Center.x, gaugeMetrics.Center.y, .5f}};
	for(int16_t iStep = 0, stepCount = resolution; iStep < stepCount; ++iStep) {
		::gpk::SPairSinCos					sinCos							= {sin(iStep * stepUnit), -cos(iStep * stepUnit)};
		const double						finalRadius						= gaugeMetrics.Radius; //::gpk::interpolate_linear(gaugeMetrics.Radius, gaugeMetrics.Radius * .5, ::gpk::clamp(abs(sinCos.Cos), 0.0, 1.0)); //
 		const ::gpk::SCoord3<double>		relativePosSmall				=
			{ sinCos.Sin * (finalRadius - width)
			, sinCos.Cos * (finalRadius - width)
			};
		const ::gpk::SCoord3<double>		relativePos				=
			{ sinCos.Sin * finalRadius
	  		, sinCos.Cos * finalRadius
			};
		gauge.Vertices.push_back(sphereMetrics.Center + relativePosSmall.Cast<float>());
		gauge.Vertices.push_back(sphereMetrics.Center + relativePos.Cast<float>());
	}
	for(int16_t iStep = 0, stepCount = (int16_t)(resolution * 2) - 3; iStep < stepCount; ++iStep) {
		gauge.Indices.push_back({int16_t(0 + iStep), int16_t(1 + iStep), int16_t(2 + iStep)});
		gauge.Indices.push_back({int16_t(1 + iStep), int16_t(3 + iStep), int16_t(2 + iStep)});
	}
	const int16_t						iStep							= int16_t(gauge.Vertices.size() - 2);
	gauge.Indices.push_back({int16_t(0 + iStep), int16_t(1 + iStep), 0});
	gauge.Indices.push_back({int16_t(1 + iStep), 1, 0});
	gauge.MaxValue					= resolution;
	return 0;
}


::gpk::error_t					ghg::gaugeImageUpdate			(::ghg::SUIControlGauge & gauge, ::gpk::view_grid<::gpk::SColorBGRA> target, ::gpk::SColorFloat colorMin, ::gpk::SColorFloat colorMid, ::gpk::SColorFloat colorMax, ::gpk::SColorBGRA colorEmpty)  {
	static ::gpk::SImage<uint32_t>		dummyDepth;
	const ::gpk::SCoord3<float>			center3							= (gauge.Vertices[1] - gauge.Vertices[gauge.Vertices.size() / 2 + 1]) / 2 + gauge.Vertices[gauge.Vertices.size() / 2 + 1];
	const ::gpk::SCoord2<float>			center2							= {center3.x, center3.y};
	const double						radiusLarge						= (center3 - gauge.Vertices[1]).Length();
	const double						radiusSmall						= (center3 - gauge.Vertices[0]).Length();
	const double						radiusCenter					= (radiusLarge - radiusSmall) / 2 + radiusSmall;
	dummyDepth.resize(target.metrics(), 0xFFFFFFFFU);
	::gpk::array_pod<::gpk::SCoord2<int16_t>>			pixelCoords;
	::gpk::array_pod<::gpk::STriangleWeights<float>>	triangleWeights;
	for(uint32_t iTriangle = 0, triangleCount = gauge.Indices.size(); iTriangle < triangleCount; ++iTriangle) {
		const ::gpk::STriangle3<float>		triangleCoords					=
			{ gauge.Vertices[gauge.Indices[iTriangle].A]
			, gauge.Vertices[gauge.Indices[iTriangle].B]
			, gauge.Vertices[gauge.Indices[iTriangle].C]
			};
		const double						colorFactor						= ::gpk::min(1.0, iTriangle / (double)triangleCount);
		::gpk::SColorFloat					finalColor;
		const bool							isEmptyGauge					= ((iTriangle + 2) >> 1) >= (uint32_t)gauge.CurrentValue * 2;
		finalColor						= isEmptyGauge
			? ::gpk::SColorFloat(colorEmpty)
			: (colorFactor < .5)
				? ::gpk::interpolate_linear(colorMin, colorMid, (::gpk::min(1.0, iTriangle * 2.25 / (double)triangleCount)))
				: ::gpk::interpolate_linear(colorMid, colorMax, (colorFactor - .5) * 2)
				;
		pixelCoords.clear();
		::gpk::drawTriangle(target.metrics(), triangleCoords, pixelCoords, triangleWeights, dummyDepth);
		for(uint32_t iPixelCoords = 0; iPixelCoords < pixelCoords.size(); ++iPixelCoords) {
			const ::gpk::SCoord2<int16_t>		pixelCoord						= pixelCoords[iPixelCoords];
//#define GAUGE_NO_SHADING
#ifndef GAUGE_NO_SHADING
			const ::gpk::SCoord2<float>			floatCoord						= pixelCoord.Cast<float>();
			const double						distanceFromCenter				= (floatCoord - center2).Length();
			const double						distanceFromRadiusCenter		= fabs(distanceFromCenter - radiusCenter) / ((radiusLarge - radiusSmall) / 2);
			finalColor.a					= (float)(1 - distanceFromRadiusCenter);
#else
			(void)radiusCenter;
#endif		
			::gpk::SColorBGRA					& targetPixel					= target[pixelCoord.y][pixelCoord.x];
			finalColor.Clamp();
			targetPixel						= ::gpk::interpolate_linear(::gpk::SColorFloat{targetPixel}, finalColor, finalColor.a);
			targetPixel.a = uint8_t(finalColor.a * 255);
		}
	}
	return 0;
}
