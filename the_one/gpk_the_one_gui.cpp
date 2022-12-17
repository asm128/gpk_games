#include "gpk_the_one.h"

static constexpr	::gpk::GUI_COLOR_MODE		GHG_MENU_COLOR_MODE		= ::gpk::GUI_COLOR_MODE_3D;
static constexpr	bool						BACKGROUND_3D_ONLY		= true;
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_CAMERA_SIZE		= {48, 48};
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_VIEWPORT_SIZE	= {128, 64};
static constexpr	::gpk::SCoord2<uint16_t>	WEAPON_BAR_SIZE			= {96, 16};

static	::gpk::error_t			guiSetupCommon				(::gpk::SGUI & gui) {
	gui.ColorModeDefault			= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault				= ::gpk::ASCII_COLOR_DARKRED * 16 + 10;
	gui.SelectedFont				= 7;
	return 0;
}

static	::gpk::error_t			dialogCreateCommon			(::gpk::SDialog & dialog, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::SCoord2<float> & cursorPos) { 
	dialog							= {};
	dialog.Input					= inputState;
	dialog.GUI->CursorPos			= cursorPos;
	dialog.GUI->Controls.Modes[0].NoBackgroundRect	= true;
	::guiSetupCommon(*dialog.GUI);
	dialog.Update();
	return 0;
}

static	::gpk::error_t			guiSetupWelcome				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.DialogPerState[::the1::APP_STATE_Welcome		]; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupHome				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.DialogPerState[::the1::APP_STATE_Home		]; 
	constexpr int						BUTTON_HEIGHT				= 24;
	gpk_necs(::gpk::guiSetupButtonList<::the1::UI_HOME>(*dialog.GUI, dialog.Root, {160, BUTTON_HEIGHT}, {0, int16_t(-BUTTON_HEIGHT * ::gpk::get_value_count<::the1::UI_HOME>() / 2)}, ::gpk::ALIGN_CENTER)); 
	for(uint32_t iButton = 0; iButton < ::gpk::get_value_count<::the1::UI_HOME>(); ++iButton)
		dialog.GUI->Controls.Controls[iButton + 1].Area.Offset.y += 0;

	for(uint32_t iButton = ::the1::UI_HOME_Start; iButton < ::gpk::get_value_count<::the1::UI_HOME>(); ++iButton)
		dialog.GUI->Controls.Controls[iButton + 1].Area.Offset.y += 36;

	(void)dialog; 
	return 0; 
}
static	::gpk::error_t			guiSetupPlay				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.DialogPerState[::the1::APP_STATE_Play		]; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupShop				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.DialogPerState[::the1::APP_STATE_Shop		]; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupProfile				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.DialogPerState[::the1::APP_STATE_Profile		]; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupSettings			(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.DialogPerState[::the1::APP_STATE_Settings	]; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupAbout				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.DialogPerState[::the1::APP_STATE_About		]; (void)dialog; return 0; }
static	::gpk::error_t			guiSetupLoad				(::the1::STheOne & app) { ::gpk::SDialog	& dialog = app.DialogPerState[::the1::APP_STATE_Load		]; (void)dialog; return 0; }

::gpk::error_t					the1::guiSetup				(::the1::STheOne & app, const ::gpk::ptr_obj<::gpk::SInput> & input) {
	es_if(errored(::dialogCreateCommon(app.Dialog, input, {})));

	for(uint32_t iGUI = 0; iGUI < app.DialogPerState.size(); ++iGUI)
		e_if(errored(::dialogCreateCommon(app.DialogPerState[iGUI], input, {})), "iGUI: %i", iGUI);

	es_if(errored(::guiSetupWelcome			(app)));
	es_if(errored(::guiSetupHome			(app)));
	es_if(errored(::guiSetupPlay			(app)));
	es_if(errored(::guiSetupShop			(app)));
	es_if(errored(::guiSetupProfile			(app)));
	es_if(errored(::guiSetupSettings		(app)));
	es_if(errored(::guiSetupAbout			(app)));
	es_if(errored(::guiSetupLoad			(app)));

	es_if(errored(::gpk::virtualKeyboardSetup437(*app.Dialog.GUI, app.VirtualKeyboard)));;
	return 0;
}

static	::gpk::error_t			guiUpdatePlay				(::the1::STheOne & app, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents) { (void)app; (void)sysEvents; return 0; }
static	::gpk::error_t			guiUpdateHome				(::the1::STheOne & app, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents) { (void)app; (void)sysEvents; return 0; }

static	::gpk::error_t			guiHandlePlay				(::the1::STheOne & /*app*/, ::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::the1::STheOneGame & /*game*/) { return 0; }
static	::gpk::error_t			guiHandleHome				(::the1::STheOne & /*app*/, ::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::the1::STheOneGame & /*game*/) { return 0; }

::gpk::error_t					the1::guiUpdate				(::the1::STheOne & app, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents) {
	{
		::gpk::SDialog						& dialog					= app.Dialog;
		::gpk::SGUI							& gui						= *dialog.GUI;
		::gpk::SInput						& input						= *dialog.Input;
		::gpk::guiProcessInput(gui, input, sysEvents); 
	}

	::the1::STheOneGame					& game						= app.MainGame;

	::the1::APP_STATE					appState					= app.ActiveState; 
	for(uint32_t iAppState = 0; iAppState < ::the1::APP_STATE_COUNT; ++iAppState) {
		::gpk::SDialog						& dialog					= app.DialogPerState[iAppState];
		::gpk::SGUI							& gui						= *dialog.GUI;
		::gpk::SInput						& input						= *dialog.Input;
		::gpk::guiProcessInput(gui, input, sysEvents); 
	}

	for(uint32_t iPlayer = 0; iPlayer < ::std::size(game.PlayerUI); ++iPlayer) {
		::the1::SPlayerUI					& uiPlayer					= game.PlayerUI[iPlayer];
		::gpk::guiProcessInput(*uiPlayer.DialogPlay.GUI, *uiPlayer.DialogPlay.Input, sysEvents);
		::gpk::guiProcessInput(*uiPlayer.DialogHome.GUI, *uiPlayer.DialogHome.Input, sysEvents);
	}
	
	if(app.ActiveState == ::the1::APP_STATE_Play)
		::guiUpdatePlay(app, sysEvents);
	else 
		::guiUpdateHome(app, sysEvents);

	::gpk::SDialog						& dialog					= app.DialogPerState[appState];
	dialog.Update();
	::gpk::SGUI							& gui						= *dialog.GUI;
	::gpk::array_pod<uint32_t>			controlsToProcess			= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);

	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		uint32_t							idControl			= controlsToProcess		[iControl];
		const ::gpk::SControlState			& controlState		= gui.Controls.States	[idControl];
		bool								handled				= false;
		if(controlState.Execute) {
			idControl = idControl - 1;
			info_printf("Executed %u.", idControl);
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
