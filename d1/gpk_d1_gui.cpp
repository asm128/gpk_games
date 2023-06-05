#include "gpk_d1.h"

stacxpr	bool			BACKGROUND_3D_ONLY			= true;
stacxpr	::gpk::n2u16	SHOOT_SLIDER_SIZE			= {24, 320};
stacxpr	::gpk::n2u16	MODULE_VIEWPORT_SIZE		= {128, 64};
stacxpr	::gpk::n2u16	WEAPON_BAR_SIZE				= {184, 24};

static	::gpk::error_t	guiSetupCommon				(::gpk::SGUI & gui, const ::gpk::n2f32 & cursorPos) {
	gui.ThemeDefault		= ::gpk::ASCII_COLOR_DARKRED * 16 + 10;
	gui.SelectedFont		= 7;
	gui.CursorPos			= cursorPos;
	return 0;
}

static	::gpk::error_t	dialogCreateCommon			(::gpk::SDialog & dialog, const ::gpk::pobj<::gpk::SInput> & inputState, const ::gpk::n2f32 & cursorPos) { 
	//dialog				= {};
	dialog.Input			= inputState;
	gpk_necs(::guiSetupCommon(*dialog.GUI, cursorPos));
	dialog.GUI->Controls.Modes[0].NoBackgroundRect	= true;
	gpk_necs(dialog.Update());
	return 0;
}

static	::gpk::error_t	guiContainerSetupDefaults	(::gpk::SGUI & gui, uint32_t iControl, uint32_t iParent) { 
	gui.Controls.Constraints[iControl].AttachSizeToControl	= {(int32_t)iControl, (int32_t)iControl};
	gui.Controls.Modes		[iControl].NoHoverEffect		= true;
	gui.Controls.Modes		[iControl].NoBackgroundRect		= true;
	gui.Controls.Modes		[iControl].Design				= true;
	gui.Controls.Controls	[iControl].Border				= {};
	gui.Controls.Controls	[iControl].Margin				= {};
	gpk_necs(::gpk::controlSetParent(gui, iControl, iParent));
	return 0; 
}

static	::gpk::error_t	guiSetupHome				(::d1::SD1UI & appUI, ::d1::SD1Game & appGame) { ::gpk::SDialog	& dialog = appUI.Dialog; 
	cnstxpr uint16_t			BUTTON_HEIGHT				= 24;
	cnstxpr uint16_t			BUTTON_WIDTH_SMALL			= 160;
	cnstxpr uint16_t			BUTTON_WIDTH_LARGE			= 184;
	::gpk::SGUI					& gui						= *dialog.GUI;
	uint32_t					firstControlHome			= appUI.FirstControl[::d1::APP_STATE_Home] = ::gpk::guiSetupButtonList<::d1::UI_HOME>(gui, appUI.DialogPerState[::d1::APP_STATE_Home], {BUTTON_WIDTH_SMALL, BUTTON_HEIGHT}, {0, int16_t(-BUTTON_HEIGHT * ::gpk::get_value_count<::d1::UI_HOME>() / 2)}, ::gpk::ALIGN_CENTER);
	gpk_necs(firstControlHome); 
	for(uint32_t iButton = firstControlHome; iButton < firstControlHome + ::gpk::get_value_count<::d1::UI_HOME>(); ++iButton) {
		gui.Controls.Controls[iButton].Area.Offset.y	+= 0;
	}

	for(uint32_t iButton = firstControlHome + ::d1::UI_HOME_Start; iButton < firstControlHome + ::gpk::get_value_count<::d1::UI_HOME>(); ++iButton) {
		gui.Controls.Controls[iButton].Area.Offset.y	+= 36 * 3;
		gui.Controls.Controls[iButton].Area.Size.x		= BUTTON_WIDTH_LARGE;
	}

	gpk_necs(::gpk::inputBoxCreate(appUI.NameEditBox, gui, firstControlHome));
	gpk_necs(appUI.NameEditBox.Edit(gui, false));
	{
		gpk_necs(::gpk::tunerCreate(dialog, appUI.TunerTableSize));
		gpk_necs(::gpk::controlSetParent(gui, appUI.TunerTableSize->IdGUIControl, appUI.DialogPerState[::d1::APP_STATE_Home]));
		appUI.TunerTableSize->ValueLimits.Min	= 0;
		appUI.TunerTableSize->ValueLimits.Max	= uint8_t(::gpk::get_value_count<::d1p::TABLE_SIZE>() - 1);
		appUI.TunerTableSize->FuncValueFormat	= [&appGame, &appUI](::gpk::vcc & string, uint8_t value, const ::gpk::SMinMax<uint8_t> &)			mutable { 
			appGame.StartState.StandardTableSize	= (::d1p::TABLE_SIZE)value;
			string					= ::gpk::get_value_descv(appGame.StartState.StandardTableSize); 
			return 0; 
		};
		appUI.TunerTableSize->FuncGetString		= [&appGame, &appUI](::gpk::vcc & string, uint8_t, const ::gpk::SMinMax<uint8_t> &)	mutable { 
			string		= {appUI.TunerTableSize->ValueString, (uint32_t)snprintf(appUI.TunerTableSize->ValueString, ::gpk::size(appUI.TunerTableSize->ValueString) - 2, "%s Table", ::gpk::toString(string).begin())}; 
			return 0; 
		};

		appUI.TunerTableSize->SetValue(::d1p::TABLE_SIZE_8_00_FOOT);

		::gpk::SControl				& control					= dialog.GUI->Controls.Controls[appUI.TunerTableSize->IdGUIControl];
		control.Area.Size.x		= BUTTON_WIDTH_LARGE;
		control.Area.Size.y		= 24;
		control.Area.Offset.y	= int16_t(dialog.GUI->Controls.Controls[firstControlHome + ::d1::UI_HOME_Start].Area.Offset.y - (dialog.GUI->Controls.Controls[firstControlHome + ::d1::UI_HOME_Start].Area.Size.y >> 1) - (control.Area.Size.y >> 1));
		control.Align			= ::gpk::ALIGN_CENTER;
	}

	for(uint32_t iTeam = 0; iTeam < appUI.TeamUI.size(); ++iTeam) {
		uint32_t					playerRoot					= appUI.TeamUI[iTeam].DialogPerState[::d1::APP_STATE_Home];
		gui.Controls.Controls[playerRoot].Align			= iTeam ? ::gpk::ALIGN_LEFT : ::gpk::ALIGN_RIGHT;

		uint32_t					firstControl				= appUI.TeamUI[iTeam].FirstControl[::d1::APP_STATE_Home] = ::gpk::guiSetupButtonList<::d1::UI_TEAM>(gui, playerRoot, WEAPON_BAR_SIZE, {0, 0}, iTeam ? ::gpk::ALIGN_TOP_RIGHT : ::gpk::ALIGN_TOP_LEFT);
		gpk_necs(firstControl);
		//gui.Controls.States[firstControl].Hidden		= true;
		//gui.Controls.States[firstControl].Disabled	= true;
	}
	return 0; 
}

static	::gpk::error_t	guiSetupPlay				(::d1::SD1UI & appUI, ::d1::SD1Game & appGame) { ::gpk::SDialog	& dialog = appUI.Dialog; 
	cnstxpr int					BUTTON_HEIGHT				= 24;
	::gpk::SGUI					& gui						= *dialog.GUI;
	const uint32_t				iDialog						= appUI.DialogPerState[::d1::APP_STATE_Play];
	uint32_t					firstControl				=  appUI.FirstControl[d1::APP_STATE_Play] = ::gpk::guiSetupButtonList<::d1::UI_PLAY>(gui, iDialog, {160, BUTTON_HEIGHT}, {0, 0}, ::gpk::ALIGN_CENTER_BOTTOM);
	gpk_necs(firstControl); 
	for(uint32_t iTeam = 0; iTeam < appUI.TeamUI.size(); ++iTeam) {
		uint32_t					playerRoot					= appUI.TeamUI[iTeam].DialogPerState[d1::APP_STATE_Play];
		gui.Controls.States	[playerRoot].Hidden	= true;

		uint32_t					firstControlPlayer			= appUI.TeamUI[iTeam].FirstControl[::d1::APP_STATE_Play] = ::gpk::guiSetupButtonList<::d1::UI_TEAM>(gui, playerRoot, WEAPON_BAR_SIZE, {0, 0}, iTeam ? ::gpk::ALIGN_TOP_LEFT : ::gpk::ALIGN_TOP_RIGHT);
		gpk_necs(firstControlPlayer); 
		//gui.Controls.Modes [firstControlPlayer].FrameOut	= true;
		//gui.Controls.States[firstControlPlayer].Disabled	= true;
		//gui.Controls.Modes [firstControlPlayer + 1].FrameOut	= true;
		//gui.Controls.States[firstControlPlayer + 1].Disabled	= true;
	}


	gpk_necs(::gpk::sliderCreate(dialog, appUI.ForceSlider));
	appUI.ForceSlider->Vertical		= true;
	appUI.ForceSlider->ValueLimits	= {0, 1000};
	gpk_necs(::gpk::controlSetParent(gui, appUI.ForceSlider->IdGUIControl, iDialog));

	cnstxpr uint16_t			offsetX						= 8;

	gui.Controls.Controls[appUI.ForceSlider->IdGUIControl].Border			= {};
	gui.Controls.Controls[appUI.ForceSlider->IdGUIControl].Area.Offset		= {offsetX + 36};
	gui.Controls.Controls[appUI.ForceSlider->IdGUIControl].Area.Size		= SHOOT_SLIDER_SIZE.Cast<int16_t>();
	gui.Controls.Controls[appUI.ForceSlider->IdGUIControl].Align			= ::gpk::ALIGN_CENTER_RIGHT;
	gui.Controls.Controls[appUI.ForceSlider->IdButton].Area.Size.y			= 8;
	//gui.Controls.Modes[app.ForceSlider->IdGUIControl].NoBackgroundRect	= true;
	gui.Controls.Modes[appUI.ForceSlider->IdGUIControl].NoHoverEffect		= true;
	appUI.ForceSliderRenderTarget.resize(gui.Controls.Controls[appUI.ForceSlider->IdGUIControl].Area.Size.Cast<uint32_t>()); 

	cnstxpr ::gpk::rgbaf		min							= ::gpk::GREEN;
	cnstxpr ::gpk::rgbaf		mid							= ::gpk::YELLOW;
	cnstxpr ::gpk::rgbaf		max							= ::gpk::RED;
	float						colorScale					= 1.0f / (appUI.ForceSliderRenderTarget.metrics().y / 2);
	const uint32_t				halfHeight					= appUI.ForceSliderRenderTarget.metrics().y / 2;
	const uint32_t				lastRow						= appUI.ForceSliderRenderTarget.metrics().y - 1;
	for(uint32_t y = 0; y < halfHeight; ++y)
		for(uint32_t x = 0; x < appUI.ForceSliderRenderTarget.metrics().x; ++x)
			appUI.ForceSliderRenderTarget.Color[lastRow - y][x] = ::gpk::interpolate_linear(min, mid, colorScale * y);

	for(uint32_t y = 0; y < halfHeight; ++y)
		for(uint32_t x = 0; x < appUI.ForceSliderRenderTarget.metrics().x; ++x)
			appUI.ForceSliderRenderTarget.Color[lastRow - (y + halfHeight)][x] = ::gpk::interpolate_linear(mid, max, colorScale * y);

	gui.Controls.Controls[appUI.ForceSlider->IdGUIControl].Image = appUI.ForceSliderRenderTarget.Color;

	static char					velocityString[32]			= {};
	appUI.ForceSlider->FuncValueFormat	= [&appGame](::gpk::vcc & string, int64_t value, const ::gpk::SMinMax<int64_t> & limits) mutable { 
		const float					newVelocity					= ::d1p::MAX_SHOOT_VELOCITY / limits.Max * (limits.Max - value);
		const float					currentVelocity				= appGame.Pool.ActiveStick().Velocity;
		if(newVelocity != currentVelocity) { // only generate the event if the value actually changed. this is because this may be called randomly to ensure the right value is in sync with the server.
			const ::d1p::SArgsPlayerInput	eventData					= {newVelocity, ::gpk::AXIS_ORIGIN};
			appGame.QueueStick.push_back({::d1p::PLAYER_INPUT_Force, ::gpk::vcu8{(const uint8_t*)&eventData, sizeof(eventData)}});
		}
		string					= ::gpk::vcc{(uint32_t)sprintf_s(velocityString, "%.02f m/s", newVelocity), velocityString}; 
		return 0; 
	};
	gpk_necs(::gpk::sliderSetValue(*appUI.ForceSlider, int64_t(appUI.ForceSlider->ValueLimits.Max - appGame.Pool.ActiveStick().Velocity * (appUI.ForceSlider->ValueLimits.Max / ::d1p::MAX_SHOOT_VELOCITY))));

	gui.Controls.Controls[firstControl + ::d1::UI_PLAY_Shoot].Align			= ::gpk::ALIGN_CENTER_RIGHT;
	gui.Controls.Controls[firstControl + ::d1::UI_PLAY_Shoot].Area.Offset.x	= offsetX;
	gui.Controls.Controls[firstControl + ::d1::UI_PLAY_Shoot].Area.Offset.y	= SHOOT_SLIDER_SIZE.y / 2 + 16;
	gui.Controls.Controls[firstControl + ::d1::UI_PLAY_Shoot].Area.Size.x	= SHOOT_SLIDER_SIZE.x * 4;

	return 0; 
}

::gpk::error_t			d1::guiSetup				(::d1::SD1UI & appUI, ::d1::SD1Game & appGame, const ::gpk::pobj<::gpk::SInput> & input) {
	gpk_necs(::dialogCreateCommon(appUI.Dialog, input, appUI.Dialog.GUI->CursorPos));
	gpk_necs(::guiContainerSetupDefaults(*appUI.Dialog.GUI, 0, (uint32_t)-1));

	::gpk::SGUI					& gui						= *appUI.Dialog.GUI;
	for(uint32_t iState = 0; iState < appUI.DialogPerState.size(); ++iState) {
		if(iState == ::d1::APP_STATE_Home || iState == ::d1::APP_STATE_Play) {
			uint32_t					iControl;
			gpk_necs(iControl = appUI.DialogPerState[iState] = ::gpk::controlCreate(gui));
			gpk_necs(guiContainerSetupDefaults(gui, iControl, appUI.Dialog.Root));
			gui.Controls.States[iControl].Hidden	= true;
			for(uint32_t iTeam = 0; iTeam < appUI.TeamUI.size(); ++iTeam)
				gpk_necs(::guiContainerSetupDefaults(gui, appUI.TeamUI[iTeam].DialogPerState[iState] = ::gpk::controlCreate(gui), appUI.Dialog.Root));
		}
	}
	gpk_necs(::guiSetupHome(appUI, appGame));
	gpk_necs(::guiSetupPlay(appUI, appGame));
	return 0;
}

static	::gpk::error_t	guiHandlePlay				(::d1::SD1 & app, ::gpk::SGUI & gui, uint32_t idControl, d1::APP_STATE appState) { 
	uint32_t					idDialog					= app.AppUI.DialogPerState[appState];
	const ::gpk::ai32			& children					= gui.Controls.Children[idDialog];
	idControl				= idControl - children[0];
	switch((::d1::UI_PLAY)idControl) {
	case ::d1::UI_PLAY_Menu: 
		appState				= (::d1::APP_STATE)app.StateSwitch(::d1::APP_STATE_Home);
		break;
	case ::d1::UI_PLAY_Shoot:
		app.MainGame.QueueStick.push_back({::d1p::PLAYER_INPUT_Shoot});
		break;
	}
	return appState; 
}

static	::gpk::error_t	guiHandleHome				(::d1::SD1 & app, ::gpk::SGUI & gui, uint32_t idControl, d1::APP_STATE appState) { 
	uint32_t					idDialog					= app.AppUI.DialogPerState[appState];
	const ::gpk::ai32			& children					= gui.Controls.Children[idDialog];
	idControl				= idControl - children[0];
	switch((::d1::UI_HOME)idControl) {
	case ::d1::UI_HOME_Start: 
		if(app.AppUI.TunerTableSize->ValueCurrent)
			app.MainGame.StartState.Board.Table = ::d1p::TABLE_SIZES[app.AppUI.TunerTableSize->ValueCurrent - app.AppUI.TunerTableSize->ValueLimits.Min - 1];
		gpk_necs(::d1p::poolGameReset(app.MainGame.Pool, app.MainGame.StartState));
	case ::d1::UI_HOME_Continue: 
		appState				= ::d1::APP_STATE_Play;
		break;
	case ::d1::UI_HOME_Exit: 
		app.Save(::d1::SAVE_MODE_AUTO);
		appState				= ::d1::APP_STATE_Quit;
		break;
	}
	return appState;
}

::d1::APP_STATE			d1::guiUpdate				(::d1::SD1 & app, ::gpk::view<::gpk::SSysEvent> sysEvents) {
	::gpk::SDialog				& dialog					= app.AppUI.Dialog;
	::gpk::guiProcessInput(*dialog.GUI, *dialog.Input, sysEvents); 
	
	dialog.Update();
	::gpk::SGUI					& gui						= *dialog.GUI;

	::gpk::au32					controlsToProcess			= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	app.AppUI.NameEditBox.Update(gui, sysEvents, controlsToProcess);
	::d1::APP_STATE				appState					= app.ActiveState; 
	::gpk::guiProcessControls(gui, [&app, &gui, &controlsToProcess, &appState](int32_t idControl) {
		bool						handled						= false;
		switch(appState) {
		case ::d1::APP_STATE_Home		: appState = (::d1::APP_STATE)::guiHandleHome		(app, gui, idControl, appState);	handled = true; break;
		case ::d1::APP_STATE_Play		: appState = (::d1::APP_STATE)::guiHandlePlay		(app, gui, idControl, appState);	handled = true; break;
		//case ::d1::APP_STATE_Load		: appState = (::d1::APP_STATE)::guiHandleLoad		(app, gui, idControl, game);		handled = true; break;
		//case ::d1::APP_STATE_Welcome	: appState = (::d1::APP_STATE)::guiHandleWelcome	(app, gui, idControl, game);		handled = true; break;
		//case ::d1::APP_STATE_Profile	: appState = (::d1::APP_STATE)::guiHandleUser		(gui, idControl, game);				handled = true; break;
		//case ::d1::APP_STATE_Shop		: appState = (::d1::APP_STATE)::guiHandleShop		(gui, idControl, game);				handled = true; break;
		//case ::d1::APP_STATE_Settings	: appState = (::d1::APP_STATE)::guiHandleSetup		(gui, idControl, game);				handled = true; break;
		//case ::d1::APP_STATE_About	: appState = (::d1::APP_STATE)::guiHandleAbout		(gui, idControl, game);				handled = true; break;
		}
		return appState;
	});

	::d1p::SPoolGame			& poolGame					= app.MainGame.Pool;
	{ // update match time
		const double				seconds						= poolGame.MatchState.TotalSeconds;
		const int					minutes						= int(seconds / 60);
		const int					hours						= int(minutes / 60);
		sprintf_s(app.AppUI.secdsbuffer.Storage, "%.2i:%.2i:%05.2f", hours % 24, minutes % 60, (int(seconds) % 60) + (seconds - int(seconds)));
		::gpk::controlTextSet(*app.AppUI.Dialog.GUI, app.AppUI.FirstControl[::d1::APP_STATE_Play] + ::d1::UI_PLAY_Time, app.AppUI.secdsbuffer.Storage);
	} // update turn time
	if(poolGame.TurnHistory.size()) {
		const double				seconds						= poolGame.ActiveTurn().Time.SecondsActive + poolGame.ActiveTurn().Time.SecondsAiming;
		const int					minutes						= int(seconds / 60);
		const int					hours						= int(minutes / 60);
		sprintf_s(app.AppUI.turnsbuffer.Storage, "%.2i:%.2i:%05.2f", hours % 24, minutes % 60, (int(seconds) % 60) + (seconds - int(seconds)));
		::gpk::controlTextSet(*app.AppUI.Dialog.GUI, app.AppUI.FirstControl[::d1::APP_STATE_Play] + ::d1::UI_PLAY_Turn, app.AppUI.turnsbuffer.Storage);
	}
	return appState;
}
