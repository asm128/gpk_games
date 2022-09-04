#include "gpk_galaxy_hell_app.h"
#include "gpk_storage.h"

static constexpr	::gpk::GUI_COLOR_MODE	GHG_MENU_COLOR_MODE = ::gpk::GUI_COLOR_MODE_3D;
static constexpr	bool					BACKGROUND_3D_ONLY = true;

static	::gpk::error_t			guiSetupCommon				(::gpk::SGUI & gui) {
	gui.ColorModeDefault			= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault				= ::gpk::ASCII_COLOR_DARKGREY * 16 + 8;
	gui.SelectedFont				= 7;
	return 0;
}

template<typename _tUIEnum>
static	::gpk::error_t			guiSetupButtonList			(::gpk::SGUI & gui, uint16_t buttonWidth, int16_t yOffset, ::gpk::ALIGN controlAlign) {
	for(uint16_t iButton = 0; iButton < ::gpk::get_value_count<_tUIEnum>(); ++iButton) {
		int32_t								idControl						= ::gpk::controlCreate(gui);
		::gpk::SControl						& control						= gui.Controls.Controls[idControl];
		control.Area					= {{0,(int16_t)(20*iButton) + yOffset}, {(int16_t)buttonWidth, 20}};
		control.Border					= {1, 1, 1, 1};//{10, 10, 10, 10};
		control.Margin					= {1, 1, 1, 1};
		control.Align					= controlAlign;
		::gpk::SControlText					& controlText					= gui.Controls.Text[idControl];
		controlText.Text				= ::gpk::get_value_label((_tUIEnum)iButton);
		controlText.Align				= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, idControl, -1);
	}
	return 0;
}

static	::gpk::error_t			guiSetupLoad				(::gpk::SGUI & gui) { guiSetupCommon(gui); guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); return 0; }
static	::gpk::error_t			guiSetupHome				(::gpk::SGUI & gui) { 
	guiSetupCommon(gui); 
	guiSetupButtonList<::ghg::UI_HOME>(gui, 120, int16_t(-20 * ::gpk::get_value_count<::ghg::UI_HOME>() / 2), ::gpk::ALIGN_CENTER); 
	return 0; 
}
static	::gpk::error_t			guiSetupUser				(::gpk::SGUI & gui) { guiSetupCommon(gui); guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); return 0; }
static	::gpk::error_t			guiSetupShop				(::gpk::SGUI & gui) { guiSetupCommon(gui); guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); return 0; }
static	::gpk::error_t			guiSetupPlay				(::gpk::SGUI & gui) { guiSetupCommon(gui); guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); return 0; }
static	::gpk::error_t			guiSetupBrief				(::gpk::SGUI & gui) { guiSetupCommon(gui); guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); return 0; }
static	::gpk::error_t			guiSetupStage				(::gpk::SGUI & gui) { guiSetupCommon(gui); guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); return 0; }
static	::gpk::error_t			guiSetupStats				(::gpk::SGUI & gui) { guiSetupCommon(gui); guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); return 0; }
static	::gpk::error_t			guiSetupStore				(::gpk::SGUI & gui) { guiSetupCommon(gui); guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); return 0; }
static	::gpk::error_t			guiSetupScore				(::gpk::SGUI & gui) { guiSetupCommon(gui); guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); return 0; }
static	::gpk::error_t			guiSetupAbout				(::gpk::SGUI & gui) { guiSetupCommon(gui); guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); return 0; }

::gpk::error_t					ghg::guiSetup					(::ghg::SGameUI & gameui) {
	guiSetupLoad	(gameui.GUIPerState[::ghg::APP_STATE_Load	]);
	guiSetupHome	(gameui.GUIPerState[::ghg::APP_STATE_Home	]);
	guiSetupUser	(gameui.GUIPerState[::ghg::APP_STATE_User	]);
	guiSetupShop	(gameui.GUIPerState[::ghg::APP_STATE_Shop	]);
	guiSetupPlay	(gameui.GUIPerState[::ghg::APP_STATE_Play	]);
	guiSetupBrief	(gameui.GUIPerState[::ghg::APP_STATE_Brief	]);
	guiSetupStage	(gameui.GUIPerState[::ghg::APP_STATE_Stage	]);
	guiSetupStats	(gameui.GUIPerState[::ghg::APP_STATE_Stats	]);
	guiSetupStore	(gameui.GUIPerState[::ghg::APP_STATE_Store	]);
	guiSetupScore	(gameui.GUIPerState[::ghg::APP_STATE_Score	]);
	guiSetupAbout	(gameui.GUIPerState[::ghg::APP_STATE_About	]);
	return 0;
}
static	::gpk::error_t			guiUpdateLoad				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Load; }
static	::gpk::error_t			guiUpdateHome				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & game, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { 
	switch((::ghg::UI_HOME)idControl) {
	case ::ghg::UI_HOME_Start: 
		::ghg::solarSystemReset(game);
		game.Paused						= false;
		return ::ghg::APP_STATE_Play;
	case ::ghg::UI_HOME_Continue: 
		game.Paused						= false;
		return ::ghg::APP_STATE_Play;
	case ::ghg::UI_HOME_Save: {
		::gpk::array_pod<byte_t>			serialized;
		game.Save(game, serialized);

		char fileName[4096] = {};
		sprintf_s(fileName, "./%llu.ghs", ::gpk::timeCurrentInMs());
		::gpk::fileFromMemory(fileName, serialized);
		game.Paused						= false;
		return ::ghg::APP_STATE_Play;
	}
	case ::ghg::UI_HOME_Load: {
		::gpk::array_pod<byte_t>			serialized;
		char								fileName[4096]		= {};
		sprintf_s(fileName, "./%llu.ghs", ::gpk::timeCurrentInMs());
		::gpk::fileToMemory(fileName, serialized);
		//game.Load(game, serialized);

		return ::ghg::APP_STATE_Play;
	}
	case ::ghg::UI_HOME_Exit: 
		game.Paused						= true;
		return ::ghg::APP_STATE_Quit;
	}
	return ::ghg::APP_STATE_Home; 
}

static	::gpk::error_t			guiUpdateUser				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_User; }
static	::gpk::error_t			guiUpdateShop				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Shop; }
static	::gpk::error_t			guiUpdatePlay				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & game, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { 
	if(idControl == (uint32_t)::ghg::UI_PLAY_Menu) {
		game.Paused								= true;
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Play; 
}

static	::gpk::error_t			guiUpdateBrief				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Brief; }
static	::gpk::error_t			guiUpdateStage				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Stage; }
static	::gpk::error_t			guiUpdateStats				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Stats; }
static	::gpk::error_t			guiUpdateStore				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Store; }
static	::gpk::error_t			guiUpdateScore				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Score; }
static	::gpk::error_t			guiUpdateAbout				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_About; }

::gpk::error_t					ghg::guiUpdate				(::ghg::SGameUI & gameui, ::ghg::SGalaxyHell & game, ::ghg::APP_STATE appState, const ::gpk::SInput & input, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents, const ::gpk::SCoord2<uint16_t> & screenMetrics) {
	for(uint32_t iAppState = 0; iAppState < ::ghg::APP_STATE_COUNT; ++iAppState) {
		::gpk::SGUI							& gui							= gameui.GUIPerState[iAppState];
		::gpk::guiProcessInput(gui, input, sysEvents); 
	}
	::gpk::SGUI							& gui							= gameui.GUIPerState[appState];
	::gpk::array_pod<uint32_t>			controlsToProcess	= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		uint32_t							idControl			= controlsToProcess		[iControl];
		const ::gpk::SControlState			& controlState		= gui.Controls.States	[idControl];
		bool								handled				= false;
		if(controlState.Execute) {
			info_printf("Executed %u.", idControl);
			switch(appState) {
			case ::ghg::APP_STATE_Load	: appState = (::ghg::APP_STATE)guiUpdateLoad	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Home	: appState = (::ghg::APP_STATE)guiUpdateHome	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_User	: appState = (::ghg::APP_STATE)guiUpdateUser	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Shop	: appState = (::ghg::APP_STATE)guiUpdateShop	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Play	: appState = (::ghg::APP_STATE)guiUpdatePlay	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Brief	: appState = (::ghg::APP_STATE)guiUpdateBrief	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Stage	: appState = (::ghg::APP_STATE)guiUpdateStage	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Stats	: appState = (::ghg::APP_STATE)guiUpdateStats	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Store	: appState = (::ghg::APP_STATE)guiUpdateStore	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Score	: appState = (::ghg::APP_STATE)guiUpdateScore	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_About	: appState = (::ghg::APP_STATE)guiUpdateAbout	(gui, idControl, game, screenMetrics); handled = true; break;
			}
			if(handled)
				break;
		}
	}
	return appState;
}

