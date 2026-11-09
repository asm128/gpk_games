#include "lls_client.h"
#include "lls_event_args.h"

#include "gpk_event_screen.h"
#include "gpk_gui_control_list.h"

using ::gpk::get_value_namep, ::gpk::get_enum_namep, ::gpk::failed;
GPK_USING_TYPEINT();

stacxpr	uint16_t		LLS_MAX_PLAYERS		= 4;
stacxpr	::gpk::n2u1_t	PLAYER_UI_SIZE		= {320, 40};
stacxpr	::gpk::astatic<::gpk::bgra, LLS_MAX_PLAYERS>	PLAYER_COLORS	=
	{ 0xFF00FF00U
	, ::gpk::PURPLE
	, 0xFF0000FFU
	, 0xFFFFFF00U
	};

static	::gpk::error_t	guiSetupCommon		(::gpk::SGUI & gui) {
	gui.ColorModeDefault	= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault		= ::gpk::ASCII_COLOR_DARKRED * 16 + 10;
	gui.SelectedFont		= 7;

	::gpk::pobj<::gpk::SGUIColors>	colors;
	colors.create(*gui.Colors);
	gui.Colors				= colors;
	return 0;
}

static	::gpk::error_t	dialogCreateCommon	(::gpk::SDialog & dialog, const ::gpk::pobj<::gpk::SInput> & inputState, const ::gpk::n2f2_t & cursorPos) {
	dialog						= {};
	dialog.Input				= inputState;
	dialog.GUI->CursorPos		= cursorPos;
	dialog.GUI->Controls.Draw[0].NoClient	= true;
	dialog.GUI->Controls.Draw[0].NoBorder	= true;
	gpk_necs(::guiSetupCommon(*dialog.GUI));
	return dialog.Update();
}

static	::gpk::error_t	uiPlayerSetupHome	(::lls::SUIPlayer & uiPlayer, const ::gpk::pobj<::gpk::SInput> & input, const ::gpk::n2f2_t & cursorPos, uint32_t iPlayer, const ::gpk::vcsc_t & playerName) {
	::gpk::ALIGN				playerAlign		= {};
	::gpk::n2s1_t			playerPosition	= {};
	switch(iPlayer) {
	default:
	case 0: playerAlign = ::gpk::ALIGN_BOTTOM_LEFT;	break;
	case 1: playerAlign = ::gpk::ALIGN_TOP_RIGHT;		break;
	case 2: playerAlign = ::gpk::ALIGN_BOTTOM_RIGHT;	break;
	case 3: playerAlign = ::gpk::ALIGN_TOP_LEFT;		break;
	}

	::gpk::SDialog			& playerDialog		= uiPlayer.DialogHome;
	gpk_necs(::dialogCreateCommon(playerDialog, input, cursorPos));
	::gpk::SGUI				& playerGUI			= *playerDialog.GUI;
	::gpk::pobj<::gpk::a8bgra>	palette;
	palette.create(*playerGUI.Colors->Palette);
	playerGUI.Colors->Palette	= palette;
	playerGUI.ThemeDefault		= ::gpk::ASCII_COLOR_DARKGREY * 16 + 12;
	gpk_necs(::gpk::guiCreateControlList<::lls::UI_PROFILE>(playerGUI, playerDialog.Root, PLAYER_UI_SIZE, playerPosition, playerAlign, ::gpk::ALIGN_CENTER, uiPlayer.ControlsDialogHome));
	for(uint32_t iButton = 0, iStop = ::gpk::get_value_count<::lls::UI_PROFILE>() - 1, iOffset = playerDialog.Root + 1 + ::lls::UI_PROFILE_Name; iButton < iStop; ++iButton) {
		playerGUI.Controls.Placement[iOffset + iButton].Margin	= {};
		playerGUI.Controls.Draw[iOffset + iButton].NoClient		= true;
	}
	for(uint32_t iButton = 0, iStop = ::gpk::get_value_count<::lls::UI_PROFILE>() - 1, iOffset = playerDialog.Root + 1 + ::lls::UI_PROFILE_Score; iButton < iStop; ++iButton) {
		playerGUI.Controls.Placement[iOffset + iButton].Area.Offset.y	-= int16_t(16 * iButton);
		playerGUI.Controls.Placement[iOffset + iButton].Area.Size.y		= 24;
	}

	gpk_necs(::gpk::inputBoxCreate(uiPlayer.InputBox, playerGUI, playerDialog.Root));
	playerGUI.Controls.SetHidden(uiPlayer.InputBox.IdRoot, true);
	playerGUI.Controls.Placement[uiPlayer.InputBox.IdRoot].Align			= playerAlign;
	playerGUI.Controls.Placement[uiPlayer.InputBox.IdRoot].Area.Offset.x	= 240;

	gpk_necs(::gpk::controlFontSet(playerGUI, playerDialog.Root + 1 + ::lls::UI_PROFILE_Name, 10));
	gpk_necs(::gpk::controlTextSet(playerGUI, playerDialog.Root + 1 + ::lls::UI_PROFILE_Name, playerName));
	return 0;
}

static	::gpk::error_t	clientUISetup		(::lls::LLSClient & app, const ::gpk::pobj<::gpk::SInput> & input) {
	while(app.Game.Players.size() < LLS_MAX_PLAYERS) {
		sc_t					playerName[64]	= {};
		sprintf_s(playerName, "Player %u", app.Game.Players.size() + 1);
		::gpk::SPlayer			player			= {};
		player.Name				= ::gpk::label(playerName);
		player.Color			= PLAYER_COLORS[app.Game.Players.size()];
		player.Mode				= ::gpk::PLAYER_MODE_Human;
		gpk_necs(app.Game.Players.push_back(player));
	}

	gpk_necs(app.UIPlay.PlayerUI.resize(LLS_MAX_PLAYERS));
	for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer)
		gpk_necall(::uiPlayerSetupHome(app.UIPlay.PlayerUI[iPlayer], input, app.GUI->CursorPos, iPlayer, app.Game.Players[iPlayer].Name), "iPlayer: %u", iPlayer);
	return 0;
}

static	::gpk::error_t	uiPlayerUpdateHome	(::lls::SUIPlayer & uiPlayer, uint16_t iPlayer, const ::gpk::vcsc_t & playerName, const ::gpk::bgra & playerColor) {
	::gpk::SDialog			& playerDialog		= uiPlayer.DialogHome;
	::gpk::SGUI				& playerGUI			= *playerDialog.GUI;
	gpk_necs(::gpk::controlTextSet(playerGUI, playerDialog.Root + 1 + ::lls::UI_PROFILE_Score, ""));
	gpk_necs(::gpk::controlTextSet(playerGUI, playerDialog.Root + 1 + ::lls::UI_PROFILE_Name, playerName));
	{
		::gpk::SControlDraw		& controlDraw		= playerGUI.Controls.Draw[playerDialog.Root + 1 + ::lls::UI_PROFILE_Name];
		controlDraw.ColorTheme	= int16_t(3 + iPlayer);
		const ::gpk::SControlTheme	& theme			= (*playerGUI.Colors->ControlThemes)[(0 == controlDraw.ColorTheme) ? playerGUI.ThemeDefault : controlDraw.ColorTheme - 1];
		const ::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>	& colorCombo	= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL];
		(*playerGUI.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_TEXT_FACE]]	= playerColor;
		(*playerGUI.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BACKGROUND]]	= playerColor;
		playerGUI.Controls.Draw[uiPlayer.InputBox.IdText].ColorTheme	= controlDraw.ColorTheme;
		playerGUI.Controls.Draw[uiPlayer.InputBox.IdText].NoClient		= true;
	}
	return 0;
}

static	::gpk::error_t	processClientScreenEvent	(::lls::LLSClient & app, const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) {
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Create:
	case ::gpk::EVENT_SCREEN_Resize: {
		const ::gpk::n2u1_t		newMetrics		= *(const ::gpk::n2u1_t*)screenEvent.Data.begin();
		for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer)
			gpk_necs(::gpk::guiUpdateMetrics(*app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI, newMetrics, true));
		} break;
	}
	return 0;
}

static	::gpk::error_t	clientUIUpdate		(::lls::LLSClient & app, ::gpk::vpobj<::gpk::SEventSystem> systemEvents) {
	for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) {
		::lls::SUIPlayer		& uiPlayer			= app.UIPlay.PlayerUI[iPlayer];
		::gpk::SDialog			& dialog				= uiPlayer.DialogHome;
		::gpk::SGUI				& gui					= *dialog.GUI;
		::gpk::guiProcessInput(gui, *dialog.Input, systemEvents);
		gui.Controls.SetHidden(dialog.Root, false);
		gpk_necall(::uiPlayerUpdateHome(uiPlayer, (uint16_t)iPlayer, app.Game.Players[iPlayer].Name, app.Game.Players[iPlayer].Color), "iPlayer: %u", iPlayer);

		::gpk::acid				controlsToProcess	= {};
		gpk_necs(::gpk::guiGetProcessableControls(gui, controlsToProcess));
		const bool				wasEditing			= uiPlayer.InputBox.Editing;
		if(int32_t result = uiPlayer.InputBox.Update(gui, uiPlayer.InputBox.VirtualKeyboard, systemEvents, controlsToProcess)) {
			if(result == INT_MAX) {
				::gpk::vcsc_t			trimmed				= {uiPlayer.InputBox.Text.begin(), ::gpk::min(uiPlayer.InputBox.Text.size(), 16U)};
				::gpk::trim(trimmed);
				if(trimmed.size())
					app.Game.Players[iPlayer].Name	= ::gpk::label(trimmed);
				gpk_necs(uiPlayer.InputBox.Edit(gui, false));
			}
		}
		else {
			gpk_necs(::gpk::guiProcessControls(gui, controlsToProcess, [&](uint32_t iControl) {
				const uint32_t			offsetControl		= dialog.Root + 1;
				switch(iControl - offsetControl) {
				case ::lls::UI_PROFILE_Name:
					gpk_necs(app.UI.UserInput.Edit(*app.GUI, false));
					gpk_necs(uiPlayer.InputBox.SetText(gui, gui.Controls.Text[offsetControl + ::lls::UI_PROFILE_Name].Text));
					gpk_necs(uiPlayer.InputBox.Edit(gui, true));
					break;
				}
				return 0;
			}));
		}
		if(wasEditing && false == uiPlayer.InputBox.Editing) {
			gpk_necs(app.UI.UserInput.SetText(*app.GUI, {}));
			gpk_necs(app.UI.UserInput.Edit(*app.GUI, true));
		}
	}
	return 0;
}

::gpk::error_t		lls::llsClientUIDraw	(::lls::LLSClient & app, ::gpk::g8bgra targetPixels) {
	for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer)
		gpk_necs(::gpk::guiDraw(*app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI, targetPixels));
	return 0;
}

::gpk::error_t		lls::llsClientUpdate		(::lls::LLSClient & app, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents) { 
	if(0 == app.UIPlay.PlayerUI.size())
		gpk_necs(::clientUISetup(app, inputState));
	gpk_necs(systemEvents.for_each([&app](const ::gpk::pobj<::gpk::SEventSystem> & systemEvent) {
		if(systemEvent->Type == ::gpk::SYSTEM_EVENT_Screen)
			return ::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN>(*systemEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) { return ::processClientScreenEvent(app, screenEvent); });
		return 0;
	}));
	gpk_necs(::lls::llsUpdate(app, lastTimeSeconds, inputState, systemEvents, [&app](::gpk::pobj<::lls::EventLLS> & _eventToProcess, ::gpk::apobj<::lls::EventLLS> & outputEvents) {
		if(!_eventToProcess)
			return false;

		const ::lls::EventLLS	& eventToProcess		= *_eventToProcess;
		info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

		::gpk::error_t			result			= 0; 
		switch(eventToProcess.Type) {
		CASE_LLS_EVENT(result, app, WORLD_EVENT, outputEvents); 
		CASE_LLS_EVENT(result, app, WORLD_ADMIN, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_SAILING, outputEvents); 
		CASE_LLS_EVENT(result, app, ACTION_CHAR, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_ENGINES, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_AIRSHIP, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_WHEELED, outputEvents); 
		CASE_LLS_EVENT(result, app, CLIENT_ASKS, outputEvents); 
		CASE_LLS_EVENT(result, app, WORLD_SETUP, outputEvents); 
		CASE_LLS_EVENT(result, app, WORLD_VALUE, outputEvents); 
		default: 
			gpk_warning_unhandled_event(eventToProcess); 
			break;
		}
		return bool(result == 1);
	}));
	gpk_necs(::clientUIUpdate(app, systemEvents));
	return 0; 
}

::gpk::error_t	lls::handleACTION_CHAR		(::lls::LLSClient & app, const ::lls::EViewMe & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::lls::SArgsEvent	& eventArgs				= *(const ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::ACTION_CHAR_Walk	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Turn	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Rush	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Hide	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Dash	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Jump	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Duck	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Warp	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Pick	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Grab	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Wear	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Use		: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Spawn	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Shoot	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Throw	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Sleep	: gpk_warning_not_implemented(gameEvent); break;
	} 
	return 0; 
}

::gpk::error_t	lls::handleWORLD_ADMIN		(::lls::LLSClient & app, const ::lls::EViewAdminWorld & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::lls::SArgsEvent	& eventArgs				= *(const ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Create	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Rename	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Delete	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Locate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Rotate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Resize	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Reskin	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Relocate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Generate	: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	lls::handleWORLD_EVENT		(::lls::LLSClient & app, const ::lls::EViewWorld & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	const ::lls::SArgsEvent	& eventArgs				= *(const ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_EVENT_Rain			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Thunderstorm	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Earthquake	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Tornado		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Wave			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Lightning		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Wildfire		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Flood			: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::error_t	lls::handleCLIENT_ASKS		(::lls::LLSClient & app, const ::lls::EViewClient & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::CLIENT_ASKS_Join: 
	case ::lls::CLIENT_ASKS_Quit: 
		gpk_necs(::gpk::eventEnqueueChild(app.EventsToSend, ::lls::LLS_EVENT_CLIENT_ASKS, gameEvent.Type, gameEvent.Data));
		return 1; 
	} 
	return 0; 
}

::gpk::error_t	lls::handleWORLD_SETUP		(::lls::LLSClient & world, const ::lls::EViewWorldSetup & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_SETUP_Seed		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_SETUP_MaxPlayers: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_SETUP_BlockSize	: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}

::gpk::error_t	lls::handleWORLD_VALUE		(::lls::LLSClient & world, const ::lls::EViewWorldValue & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeCreated			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeOffset			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_DaylightOffsetMinutes	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_DaylightRatioExtra	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeLastSaved			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_WorldSize				: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeScale				: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}
