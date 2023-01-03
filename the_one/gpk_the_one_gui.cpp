#include "gpk_the_one.h"

static constexpr	::gpk::GUI_COLOR_MODE		GHG_MENU_COLOR_MODE		= ::gpk::GUI_COLOR_MODE_3D;
static constexpr	bool						BACKGROUND_3D_ONLY		= true;
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_CAMERA_SIZE		= {48, 48};
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_VIEWPORT_SIZE	= {128, 64};
static constexpr	::gpk::SCoord2<uint16_t>	WEAPON_BAR_SIZE			= {96, 16};

static	::gpk::error_t			guiSetupCommon				(::gpk::SGUI & gui, const ::gpk::SCoord2<float> & cursorPos) {
	gui.ThemeDefault				= ::gpk::ASCII_COLOR_DARKRED * 16 + 10;
	gui.SelectedFont				= 7;
	gui.CursorPos					= cursorPos;
	gui.Controls.Modes[0].NoBackgroundRect	= true;
	return 0;
}

static	::gpk::error_t			dialogCreateCommon			(::gpk::SDialog & dialog, const ::gpk::pobj<::gpk::SInput> & inputState, const ::gpk::SCoord2<float> & cursorPos) { 
	dialog							= {};
	dialog.Input					= inputState;
	::guiSetupCommon(*dialog.GUI, cursorPos);
	dialog.Update();
	return 0;
}

static	::gpk::error_t			guiContainerSetupDefaults	(::gpk::SGUI & gui, uint32_t iControl, uint32_t iParent) { 
	gui.Controls.Constraints[iControl].AttachSizeToControl		= {(int32_t)iControl, (int32_t)iControl};
	gui.Controls.Modes		[iControl].NoBackgroundRect			= true;
	gui.Controls.Modes		[iControl].Design					= true;
	::gpk::controlSetParent(gui, iControl, iParent);
	return 0; 
}

static	::gpk::error_t			guiSetupWelcome				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.Dialog; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupHome				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.Dialog; 
	constexpr int						BUTTON_HEIGHT				= 24;
	::gpk::SGUI							& gui						= *dialog.GUI;
	{
		uint32_t							firstControl				= ::gpk::guiSetupButtonList<::the1::UI_HOME>(gui, app.DialogPerState[::the1::APP_STATE_Home], {160, BUTTON_HEIGHT}, {0, int16_t(-BUTTON_HEIGHT * ::gpk::get_value_count<::the1::UI_HOME>() / 2)}, ::gpk::ALIGN_CENTER);
		gpk_necs(firstControl); 
		for(uint32_t iButton = firstControl; iButton < firstControl + ::gpk::get_value_count<::the1::UI_HOME>(); ++iButton)
			gui.Controls.Controls[iButton].Area.Offset.y += 0;

		for(uint32_t iButton = firstControl + ::the1::UI_HOME_Start; iButton < firstControl + ::gpk::get_value_count<::the1::UI_HOME>(); ++iButton)
			gui.Controls.Controls[iButton].Area.Offset.y += 36;
	}
	for(uint32_t iPlayer = 0; iPlayer < 2; ++iPlayer) {
		uint32_t							playerRoot					= app.MainGame.PlayerUI[iPlayer].DialogHome = ::gpk::controlCreate(gui);
		guiContainerSetupDefaults(gui, playerRoot, app.DialogPerState[::the1::APP_STATE_Home]);
		uint32_t							firstControl				= ::gpk::guiSetupButtonList<::the1::UI_PROFILE>(gui, playerRoot, {160, BUTTON_HEIGHT}, {0, 0}, iPlayer ? ::gpk::ALIGN_TOP_LEFT : ::gpk::ALIGN_TOP_RIGHT);
		gpk_necs(firstControl);
		gui.Controls.States[firstControl].Hidden = true;
		gui.Controls.States[firstControl].Disabled = true;
		gpk_necs(::gpk::inputBoxCreate(app.MainGame.PlayerUI[iPlayer].Name, gui, firstControl));
		app.MainGame.PlayerUI[iPlayer].Name.Edit(gui, false);
	}
	return 0; 
}

static	::gpk::error_t			guiSetupPlay				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.Dialog; 
	constexpr int						BUTTON_HEIGHT				= 24;
	::gpk::SGUI							& gui						= *dialog.GUI;
	uint32_t							firstControl				= ::gpk::guiSetupButtonList<::the1::UI_PLAY>(gui, app.DialogPerState[::the1::APP_STATE_Play], {160, BUTTON_HEIGHT}, {0, 0}, ::gpk::ALIGN_CENTER_BOTTOM);
	gpk_necs(firstControl); 
	for(uint32_t iPlayer = 0; iPlayer < 2; ++iPlayer) {
		uint32_t							playerRoot					= app.MainGame.PlayerUI[iPlayer].DialogPlay = ::gpk::controlCreate(gui);
		guiContainerSetupDefaults(gui, app.MainGame.PlayerUI[iPlayer].DialogPlay, app.DialogPerState[::the1::APP_STATE_Play]);
		uint32_t							firstControlPlayer			= ::gpk::guiSetupButtonList<::the1::UI_PROFILE>(gui, playerRoot, {160, BUTTON_HEIGHT}, {0, 0}, iPlayer ? ::gpk::ALIGN_TOP_LEFT : ::gpk::ALIGN_TOP_RIGHT);
		gpk_necs(firstControlPlayer); 
		gui.Controls.Modes[firstControlPlayer].FrameOut	= true;
		gui.Controls.States[firstControlPlayer].Disabled	= true;
	}
	return 0; 
}

static	::gpk::error_t			guiSetupShop				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.Dialog; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupProfile				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.Dialog; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupSettings			(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.Dialog; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupAbout				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.Dialog; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupLoad				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.Dialog; (void)dialog; return 0; }

::gpk::error_t					the1::guiSetup				(::the1::STheOne & app, const ::gpk::pobj<::gpk::SInput> & input) {
	gpk_necs(::dialogCreateCommon(app.Dialog, input, {}));
	::gpk::SGUI							& gui						= *app.Dialog.GUI;
	for(uint32_t iState = 0; iState < app.DialogPerState.size(); ++iState) {
		uint32_t							iControl;
		gpk_necs(iControl = app.DialogPerState[iState] = ::gpk::controlCreate(gui));
		guiContainerSetupDefaults(gui, iControl, app.Dialog.Root);
	}
	for(uint32_t iPlayer = 0; iPlayer < 2; ++iPlayer) {
		guiContainerSetupDefaults(gui, app.MainGame.PlayerUI[iPlayer].DialogHome = ::gpk::controlCreate(gui), app.DialogPerState[::the1::APP_STATE_Home]);
		guiContainerSetupDefaults(gui, app.MainGame.PlayerUI[iPlayer].DialogPlay = ::gpk::controlCreate(gui), app.DialogPerState[::the1::APP_STATE_Play]);
	}
	gpk_necs(::guiSetupWelcome	(app));
	gpk_necs(::guiSetupHome		(app));
	gpk_necs(::guiSetupPlay		(app));
	gpk_necs(::guiSetupShop		(app));
	gpk_necs(::guiSetupProfile	(app));
	gpk_necs(::guiSetupSettings	(app));
	gpk_necs(::guiSetupAbout	(app));
	gpk_necs(::guiSetupLoad		(app));
	return 0;
}

static	::gpk::error_t			guiUpdatePlay				(::the1::STheOne & app, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents) { (void)app; (void)sysEvents; return 0; }
static	::gpk::error_t			guiUpdateHome				(::the1::STheOne & app, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents) { (void)app; (void)sysEvents; 
	return 0; 
}

static	::gpk::error_t			guiHandlePlay				(::the1::STheOne & /*app*/, ::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::the1::STheOneGame & /*game*/) { return 0; }
static	::gpk::error_t			guiHandleHome				(::the1::STheOne & app, ::gpk::SGUI & gui, uint32_t idControl, ::the1::STheOneGame & /*game*/) { 
	switch((::the1::UI_HOME)idControl) {
	case ::the1::UI_HOME_Start: 
		::the1::poolGameReset(app.MainGame.Game, ::the1::POOL_GAME_MODE_8Ball);
	case ::the1::UI_HOME_Continue: 
		gui.Controls.States[app.DialogPerState[app.ActiveState]].Hidden = true;
		app.ActiveState = ::the1::APP_STATE_Play;
		break;
	case ::the1::UI_HOME_Exit: 
		app.ActiveState					= ::the1::APP_STATE_Quit;
		break;
	}
	return 0; 
}

::gpk::error_t					the1::guiUpdate				(::the1::STheOne & app, ::gpk::view1d<::gpk::SSysEvent> sysEvents) {
	::the1::APP_STATE					appState					= app.ActiveState; 
	::gpk::guiProcessInput(*app.Dialog.GUI, *app.Dialog.Input, sysEvents); 
	
	if(app.ActiveState == ::the1::APP_STATE_Play)
		::guiUpdatePlay(app, sysEvents);
	else 
		::guiUpdateHome(app, sysEvents);

	::gpk::SDialog						& dialog					= app.Dialog;
	dialog.Update();
	::gpk::SGUI							& gui						= *dialog.GUI;

	::gpk::apod<uint32_t>				controlsToProcess			= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	for(uint32_t iPlayer = 0; iPlayer < 2; ++iPlayer) {
		app.MainGame.PlayerUI[iPlayer].Name.Update(gui, sysEvents, controlsToProcess);
	}

	::the1::STheOneGame					& game						= app.MainGame;
	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		uint32_t							idControl			= controlsToProcess		[iControl];
		const ::gpk::SControlState			& controlState		= gui.Controls.States	[idControl];
		bool								handled				= false;
		if(controlState.Execute) {
			info_printf("Executed %u.", idControl);
			idControl						= idControl - (gui.Controls.Children[app.DialogPerState[app.ActiveState]][0]);
			switch(appState) {
			//case ::the1::APP_STATE_Load		: appState = (::the1::APP_STATE)::guiHandleLoad		(app, gui, idControl, game);			handled = true; break;
			//case ::the1::APP_STATE_Welcome	: appState = (::the1::APP_STATE)::guiHandleWelcome	(app, gui, idControl, game);			handled = true; break;
			case ::the1::APP_STATE_Home		: appState = (::the1::APP_STATE)::guiHandleHome		(app, gui, idControl, game);			handled = true; break;
			//case ::the1::APP_STATE_Profile	: appState = (::the1::APP_STATE)::guiHandleUser		(gui, idControl, game);				handled = true; break;
			//case ::the1::APP_STATE_Shop		: appState = (::the1::APP_STATE)::guiHandleShop		(gui, idControl, game);				handled = true; break;
			case ::the1::APP_STATE_Play		: appState = (::the1::APP_STATE)::guiHandlePlay		(app, gui, idControl, game);			handled = true; break;
			//case ::the1::APP_STATE_Settings	: appState = (::the1::APP_STATE)::guiHandleSetup	(gui, idControl, game);				handled = true; break;
			//case ::the1::APP_STATE_About	: appState = (::the1::APP_STATE)::guiHandleAbout	(gui, idControl, game);				handled = true; break;
			}
			if(handled)
				break;
		}
	}
	return appState;
}
