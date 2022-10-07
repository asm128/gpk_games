#include "gpk_galaxy_hell_app.h"
#include "gpk_storage.h"
#include "gpk_raster_lh.h"
#include <gpk_grid_copy.h>

static constexpr	::gpk::GUI_COLOR_MODE		GHG_MENU_COLOR_MODE		= ::gpk::GUI_COLOR_MODE_3D;
static constexpr	bool						BACKGROUND_3D_ONLY		= true;
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_CAMERA_SIZE		= {64, 64};
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_VIEWPORT_SIZE	= {128, 64};
static constexpr	::gpk::SCoord2<uint16_t>	WEAPON_BAR_SIZE			= {96, 20};

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

static	::gpk::error_t			guiSetupCommon				(::gpk::SGUI & gui) {
	gui.ColorModeDefault			= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault				= ::gpk::ASCII_COLOR_DARKRED * 16 + 10;
	gui.SelectedFont				= 7;
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

// -------------------------------- Set up player GUI for home screen
static ::gpk::error_t			uiPlayerSetupHome			(::ghg::SUIPlayer & uiPlayer, ::gpk::ptr_obj<::gpk::SInput> input, ::gpk::SCoord2<float> cursorPos, uint32_t iPlayer, const ::gpk::vcc playerName) {
	::gpk::ALIGN						playerAlign		;
	::gpk::SCoord2<int16_t>				playerPosition	;
	switch(iPlayer) {
	default:
	case 0: playerAlign = ::gpk::ALIGN_CENTER_LEFT;		break;
	case 1: playerAlign = ::gpk::ALIGN_CENTER_RIGHT;	break;
	case 2: playerAlign = ::gpk::ALIGN_CENTER_BOTTOM;	break;
	case 3: playerAlign = ::gpk::ALIGN_CENTER_TOP;		break;
	}
	switch(iPlayer) {
	default:
	case 0: playerPosition = {0, -10/2}; break;	// I don't know where this measure comes from
	case 1: playerPosition = {0, int16_t(-20 * (int16_t)::gpk::get_value_count<::ghg::UI_PLAYER_SCORE>())}; break;
	case 2: playerPosition = {120}; break;
	case 3: playerPosition = {-120}; break;
	}
	::gpk::SDialog				& playerDialog		= uiPlayer.DialogHome;
	gpk_necs(::dialogCreateCommon(playerDialog, input, cursorPos));
	::gpk::SGUI					& playerGUI			= *playerDialog.GUI;
	playerGUI.ThemeDefault	= ::gpk::ASCII_COLOR_DARKGREY * 16 + 12;
	//playerGUI.Controls.Controls[playerDialog.Root].ColorTheme = app.Game.Pilots. == ::gpk::ASCII_COLOR_;
	int32_t						profileMenuScore	= 
		::gpk::guiSetupButtonList<::ghg::UI_PLAYER_SCORE>(playerGUI, playerDialog.Root, {240, 20}, playerPosition, playerAlign, ::gpk::ALIGN_LEFT);

	switch(iPlayer) {
	default:
	case 0: playerPosition = {0, int16_t(-30 * (int16_t)::gpk::get_value_count<::ghg::UI_PROFILE>())}; break;
	case 1: playerPosition = {0, 10/2}; break;	// I don't know where this measure comes from
	case 2: playerPosition = {-120}; break;
	case 3: playerPosition = {120}; break;
	}
	//int32_t profileMenuOffset = 
		::gpk::guiSetupButtonList<::ghg::UI_PROFILE>(playerGUI, playerDialog.Root, {240, 30}, playerPosition, playerAlign, ::gpk::ALIGN_CENTER);
	for(uint32_t iButton = 1 + ::gpk::get_value_count<::ghg::UI_PLAYER_SCORE>(); iButton < ::gpk::get_value_count<::ghg::UI_PLAYER_SCORE>() + ::gpk::get_value_count<::ghg::UI_PROFILE>() + 1; ++iButton) {
		playerGUI.Controls.Controls		[iButton].Margin				= {}; 
		playerGUI.Controls.Controls		[iButton].Border				= {2, 2, 2, 2};
		playerGUI.Controls.Modes		[iButton].NoBackgroundRect		= true;
	}

	gpk_necs(::virtualKeyboardSetup(playerDialog, uiPlayer.VirtualKeyboard));
	gpk_necs(::gpk::controlSetParent(playerGUI, uiPlayer.VirtualKeyboard.IdRoot, 0));
	::gpk::SControl & vkControl = playerGUI.Controls.Controls[uiPlayer.VirtualKeyboard.IdRoot];
	switch(iPlayer) {
	default:
	case 0: playerPosition = playerGUI.Controls.Controls[profileMenuScore  + ::gpk::get_value_count<::ghg::UI_PLAYER_SCORE	>() - 1].Area.Offset; playerPosition.y += vkControl.Area.Size.y / 2 + 10; break;
	case 1: playerPosition = playerGUI.Controls.Controls[profileMenuScore].Area.Offset; playerPosition.y -= vkControl.Area.Size.y / 2 + 10; break;
	case 2: playerPosition = {}; playerPosition.x += 240 + vkControl.Area.Size.x / 2; break;
	case 3: playerPosition = {}; playerPosition.x -= 240 + vkControl.Area.Size.x / 2; break;
	}
	vkControl.Align			= playerAlign;
	vkControl.Area.Offset	= playerPosition;
	vkControl.ColorTheme	= ::gpk::ASCII_COLOR_DARKGREY;// * 16 + 8;

	gpk_necs(::gpk::controlFontSet(playerGUI, 1 + ::gpk::get_value_count<::ghg::UI_PLAYER_SCORE>() + ::ghg::UI_PROFILE_Name, 10));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 + ::gpk::get_value_count<::ghg::UI_PLAYER_SCORE>() + ::ghg::UI_PROFILE_Name, playerName));
	for(uint32_t iControl = 0; iControl < playerGUI.Controls.States.size(); ++iControl)
		playerGUI.Controls.States[iControl].Updated							= false;
	return 0;
}

// -------------------------------- Set up player GUI for in-stage display
static ::gpk::error_t			uiPlayerSetupPlay			(::ghg::SUIPlayer & uiPlayer, ::gpk::ptr_obj<::gpk::SInput> input, ::gpk::SCoord2<float> cursorPos, uint32_t iPlayer, const ::gpk::vcc pilotName, uint32_t nShipParts) {
	::gpk::SCoord2<int16_t>				playerPosition;
	::gpk::ALIGN						playerAlign		;
	switch(iPlayer) {
	default:
	case 0: playerAlign = ::gpk::ALIGN_CENTER_LEFT;		break;
	case 1: playerAlign = ::gpk::ALIGN_CENTER_RIGHT;	break;
	case 2: playerAlign = ::gpk::ALIGN_CENTER_BOTTOM;	break;
	case 3: playerAlign = ::gpk::ALIGN_CENTER_TOP;		break;
	}
	switch(iPlayer) {
	default:
	case 0: playerPosition = {int16_t(::MODULE_CAMERA_SIZE.x >> 1), int16_t(::MODULE_CAMERA_SIZE.y * +3)}; break;
	case 1: playerPosition = {int16_t(::MODULE_CAMERA_SIZE.x >> 1), int16_t(::MODULE_CAMERA_SIZE.y * -4)}; break;
	case 2: playerPosition = {}; break;
	case 3: playerPosition = {}; break;
	}
	::gpk::SDialog		& playerDialog		= uiPlayer.DialogPlay;
	::dialogCreateCommon(playerDialog, input, cursorPos);

	::gpk::SGUI					& playerGUI		= *playerDialog.GUI;
	playerGUI.SelectedFont	= 8;
	::gpk::guiSetupButtonList<::ghg::UI_PLAYER>(playerGUI, playerDialog.Root, 240, playerPosition, playerAlign, ::gpk::ALIGN_CENTER);
	for(uint32_t iButton = 1; iButton < ::gpk::get_value_count<::ghg::UI_PLAYER>() + 1; ++iButton) {
		playerGUI.Controls.Constraints[iButton].AttachSizeToText.x = true;
		playerGUI.Controls.Controls[iButton].Margin = {10, 1, 10, 1}; 
		playerGUI.Controls.Controls[iButton].Border = {};
		playerGUI.Controls.Modes[iButton].NoBackgroundRect = true;
	}
	::gpk::controlTextSet(playerGUI, 1 + ::ghg::UI_PLAYER_Name, pilotName);

		 if(iPlayer == 0)	{ playerGUI.Controls.Controls[1 + ::ghg::UI_PLAYER_Name].Area.Offset = {int16_t(::MODULE_CAMERA_SIZE.x >> 1), int16_t(::MODULE_CAMERA_SIZE.y * -4)};	}
	else if(iPlayer == 1)	{ playerGUI.Controls.Controls[1 + ::ghg::UI_PLAYER_Name].Area.Offset = {int16_t(::MODULE_CAMERA_SIZE.x >> 1), int16_t(::MODULE_CAMERA_SIZE.y * +3)};	}
	else if(iPlayer == 2)	{ 
		playerGUI.Controls.Controls[1 + ::ghg::UI_PLAYER_Score].Area.Offset = {int16_t(::WEAPON_BAR_SIZE.x * +3.5), int16_t(::MODULE_CAMERA_SIZE.y)};	
		playerGUI.Controls.Controls[1 + ::ghg::UI_PLAYER_Name ].Area.Offset = {int16_t(::WEAPON_BAR_SIZE.x * -4.5), int16_t(::MODULE_CAMERA_SIZE.y)};	
	}
	else { 
		playerGUI.Controls.Controls[1 + ::ghg::UI_PLAYER_Score].Area.Offset = {int16_t(::WEAPON_BAR_SIZE.x * -5), int16_t(::MODULE_CAMERA_SIZE.y)};	
		playerGUI.Controls.Controls[1 + ::ghg::UI_PLAYER_Name ].Area.Offset = {int16_t(::WEAPON_BAR_SIZE.x * +3.5), int16_t(::MODULE_CAMERA_SIZE.y)};		
	}

	for(uint32_t iOrbiter = 0, countViewports = nShipParts; iOrbiter < countViewports; ++iOrbiter) {
		int32_t								indexVP				= uiPlayer.ModuleViewports.push_back({});
		uiPlayer.ModuleViewports[indexVP]->Viewport = ::gpk::controlCreate(playerGUI); // Orbiter
		::gpk::controlCreate(playerGUI); // Weapon Load
		::gpk::controlCreate(playerGUI); // Weapon Type
		for(uint32_t iControl = 0; iControl < 3; ++iControl)
			::gpk::controlSetParent(playerGUI, uiPlayer.ModuleViewports[indexVP]->Viewport + iControl, playerDialog.Root); // Weapon Load

		::ghg::SUIPlayShipPartViewport		& viewport			= *uiPlayer.ModuleViewports[indexVP];
		viewport.RenderTargetOrbiter.resize(::MODULE_CAMERA_SIZE);
		viewport.RenderTargetWeaponLoad.resize(::WEAPON_BAR_SIZE);
		viewport.RenderTargetWeaponType.resize(::WEAPON_BAR_SIZE);
		::gpk::SControl						& controlOrbiter	= playerGUI.Controls.Controls [viewport.Viewport + 0];
		::gpk::SControl						& controlWeaponType	= playerGUI.Controls.Controls [viewport.Viewport + 2];
		::gpk::SControl						& controlWeaponLoad	= playerGUI.Controls.Controls [viewport.Viewport + 1];
		controlOrbiter		.Image				= viewport.RenderTargetOrbiter.Color.View;
		controlWeaponLoad	.Image				= viewport.RenderTargetWeaponLoad.Color.View;
		controlWeaponType	.Image				= viewport.RenderTargetWeaponType.Color.View;
		controlOrbiter.ImageAlign				= controlWeaponType.ImageAlign	= controlWeaponLoad.ImageAlign	= ::gpk::ALIGN_CENTER;

			 if(0 == iPlayer) controlOrbiter.Align = controlWeaponLoad.Align = controlWeaponType.Align = ::gpk::ALIGN_CENTER_LEFT	;
		else if(1 == iPlayer) controlOrbiter.Align = controlWeaponLoad.Align = controlWeaponType.Align = ::gpk::ALIGN_CENTER_RIGHT	;
		else if(2 == iPlayer) controlOrbiter.Align = controlWeaponLoad.Align = controlWeaponType.Align = ::gpk::ALIGN_CENTER_BOTTOM	;
		else if(3 == iPlayer) controlOrbiter.Align = controlWeaponLoad.Align = controlWeaponType.Align = ::gpk::ALIGN_CENTER_TOP	;
		if(1 >= iPlayer)	controlOrbiter.Area.Offset	= controlWeaponLoad.Area.Offset	= controlWeaponType.Area.Offset	= {0, int16_t(-(int16_t(countViewports * (MODULE_CAMERA_SIZE.y + 4)) >> 1) + (MODULE_CAMERA_SIZE.y + 4) * iOrbiter)};
		else	 			controlOrbiter.Area.Offset	= controlWeaponLoad.Area.Offset	= controlWeaponType.Area.Offset	= {int16_t(-(int16_t(countViewports * (WEAPON_BAR_SIZE.x + 4)) >> 1) + (WEAPON_BAR_SIZE.x + 4) * iOrbiter)};
		controlOrbiter.Area.Size				= MODULE_CAMERA_SIZE.Cast<int16_t>();
		controlOrbiter.Border					= {};
		controlOrbiter.Margin					= {};

		controlWeaponLoad.Area.Size		= controlWeaponType.Area.Size	= WEAPON_BAR_SIZE.Cast<int16_t>();	//MODULE_VIEWPORT_SIZE.Cast<int16_t>();
		controlWeaponLoad.Border		= controlWeaponType.Border		= {2, 2, 2, 2};
		controlWeaponLoad.Margin		= controlWeaponType.Margin		= {};

		if(1 >= iPlayer) {
			controlWeaponLoad.Area.Offset.y	-= WEAPON_BAR_SIZE.y / 2 + 1;
			controlWeaponType.Area.Offset.y	+= WEAPON_BAR_SIZE.y / 2 + 1;
			controlWeaponLoad.Area.Offset.x	+= 
			controlWeaponType.Area.Offset.x	+= MODULE_CAMERA_SIZE.x + 1;
		}
		else {
			controlWeaponLoad.Area.Offset.y	+= 1 * MODULE_CAMERA_SIZE.y + 2;
			controlWeaponType.Area.Offset.y	+= 1 * MODULE_CAMERA_SIZE.y + WEAPON_BAR_SIZE.y + 2;
		}

		playerGUI.Controls.Text		[viewport.Viewport + 1].Align			= ::gpk::ALIGN_CENTER; //iPlayer ? ::gpk::ALIGN_CENTER_RIGHT : ::gpk::ALIGN_CENTER_LEFT;
		playerGUI.Controls.Text		[viewport.Viewport + 2].Align			= ::gpk::ALIGN_CENTER; //iPlayer ? ::gpk::ALIGN_CENTER_RIGHT : ::gpk::ALIGN_CENTER_LEFT;

		playerGUI.Controls.Modes	[viewport.Viewport + 0].NoBackgroundRect		= 
		playerGUI.Controls.Modes	[viewport.Viewport + 1].NoBackgroundRect		= 
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
	return 0;
}

//static	::gpk::error_t			guiSetupInit				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_LOAD>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupLoad				(::gpk::SDialog & dialog) { 
	gpk_necall(::gpk::guiSetupButtonList<::ghg::UI_LOAD>(*dialog.GUI, dialog.Root, 60, 0, ::gpk::ALIGN_CENTER), "%s", ""); 
	return 0;
}

static	::gpk::error_t			guiSetupHome				(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialog) { 
	constexpr int BUTTON_HEIGHT = 24;
	gpk_necs(::gpk::guiSetupButtonList<::ghg::UI_HOME>(*dialog.GUI, dialog.Root, {160, BUTTON_HEIGHT}, {0, int16_t(-BUTTON_HEIGHT * ::gpk::get_value_count<::ghg::UI_HOME>() / 2)}, ::gpk::ALIGN_CENTER)); 
	for(uint32_t iButton = 0; iButton < ::gpk::get_value_count<::ghg::UI_HOME>(); ++iButton)
		dialog.GUI->Controls.Controls[iButton + 1].Area.Offset.y += 0;

	for(uint32_t iButton = ::ghg::UI_HOME_Start; iButton < ::gpk::get_value_count<::ghg::UI_HOME>(); ++iButton)
		dialog.GUI->Controls.Controls[iButton + 1].Area.Offset.y += 36;

	dialog.GUI->Controls.Text[::ghg::UI_HOME_Start + 1].Text = "Start new game";

	gpk_necs(::gpk::tunerCreate(dialog, app.TunerPlayerCount));
	app.TunerPlayerCount->ValueLimits.Min			= 1;
	app.TunerPlayerCount->ValueLimits.Max			= ::ghg::MAX_PLAYERS;
	app.TunerPlayerCount->FuncValueFormat			= [](::gpk::vcc & string, int64_t value) { 
		string = (value == 1) ? ::gpk::vcs("%lli Player") : ::gpk::vcs("%lli Players"); 
		return 0; 
	};
	::gpk::tunerSetValue(*app.TunerPlayerCount, 1);
	//tuner->IdGUIControl
	::gpk::SControl						& control					= dialog.GUI->Controls.Controls[app.TunerPlayerCount->IdGUIControl];
	control.Area.Size.x				= 160;
	control.Area.Size.y				= 24;
	control.Area.Offset.y			= int16_t(dialog.GUI->Controls.Controls[::ghg::UI_HOME_Start + 1].Area.Offset.y - (dialog.GUI->Controls.Controls[::ghg::UI_HOME_Start + 1].Area.Size.y >> 1) - (control.Area.Size.y >> 1));
	control.Align					= ::gpk::ALIGN_CENTER;

	app.UIPlay.PlayerUI.resize(::ghg::MAX_PLAYERS);	
	for(uint32_t iPlayer = 0; iPlayer < ::ghg::MAX_PLAYERS; ++iPlayer) {
		::ghg::SUIPlayer					& uiPlayer					= app.UIPlay.PlayerUI[iPlayer];
		gpk_necall(::uiPlayerSetupHome(uiPlayer, dialog.Input, dialog.GUI->CursorPos, iPlayer, ""/*app.Players[iPlayer].Name*/), "iPlayer: %i", iPlayer);
	}
	return 0;
}
static	::gpk::error_t			guiSetupProfile				(::gpk::SDialog & dialog) { return ::gpk::guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI, dialog.Root,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupShop				(::gpk::SDialog & dialog) { 
	::gpk::SGUI							& gui						= *dialog.GUI;
	gpk_necall(::gpk::guiSetupButtonList<::ghg::UI_SHOP>(gui, dialog.Root, 60, 0, ::gpk::ALIGN_BOTTOM_RIGHT), "%s", "");
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

static	::gpk::error_t			guiSetupWelcome				(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialog) { 
	::gpk::SGUI							& gui						= *dialog.GUI;
	gpk_necall(::gpk::guiSetupButtonList<::ghg::UI_WELCOME>(gui, dialog.Root, ::gpk::SCoord2<uint16_t>{128, 32}, ::gpk::SCoord2<int16_t>{0, 64}, ::gpk::ALIGN_CENTER), "%s", "");
	::gpk::viewportCreate(dialog, app.Inputbox);
	::gpk::ptr_obj<::gpk::SDialogViewport>	viewport									= app.Inputbox;
	gui.Controls.Controls	[viewport->IdGUIControl	].Align					= ::gpk::ALIGN_CENTER;
	gui.Controls.Controls	[viewport->IdGUIControl	].Area.Offset			= {};
	gui.Controls.Controls	[viewport->IdClient		].Area.Size				= {256, 20};
	gui.Controls.Text		[viewport->IdTitle		].Text					= "Enter your name:";
	::gpk::viewportAdjustSize(gui.Controls.Controls[viewport->IdGUIControl].Area.Size, gui.Controls.Controls[viewport->IdClient].Area.Size);
	gui.Controls.States		[viewport->IdClient		].ImageInvertY			= true;
	return 0;
}

static	::gpk::error_t			guiSetupPlay				(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialog) { 
	dialog.GUI->SelectedFont		= 6;

	::gpk::guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI, dialog.Root, {220, 18}, {}, ::gpk::ALIGN_TOP_RIGHT); 
	for(uint32_t iButton = 0; iButton < ::ghg::UI_PLAY_Level; ++iButton) {
		//dialog.GUI->Controls.Text	[iButton + 1].FontSelected		= ::gpk::ALIGN_TOP_LEFT;
	}
	for(uint32_t iButton = ::ghg::UI_PLAY_Level; iButton < ::gpk::get_value_count<::ghg::UI_PLAY>(); ++iButton) {
		dialog.GUI->Controls.Text		[iButton + 1].FontSelected		= 7;
		dialog.GUI->Controls.Controls	[iButton + 1].ColorTheme		= ::gpk::ASCII_COLOR_GREEN * 16 + 6;
		dialog.GUI->Controls.Controls	[iButton + 1].Align				= ::gpk::ALIGN_TOP_LEFT;
		dialog.GUI->Controls.Controls	[iButton + 1].Area.Offset.y		-= int16_t(18 - 2 * (iButton - 1));
		dialog.GUI->Controls.Controls	[iButton + 1].Area.Size.y		+= 2;
	}

	// Setup new viewports if necessary. This may happen if the ship acquires new modules while playing the stage
	app.UIPlay.PlayerUI.resize(::ghg::MAX_PLAYERS);	
	for(uint32_t iPlayer = 0; iPlayer < ::ghg::MAX_PLAYERS; ++iPlayer) {
		::ghg::SUIPlayer					& uiPlayer					= app.UIPlay.PlayerUI[iPlayer];
		gpk_necall(::uiPlayerSetupPlay(uiPlayer, app.DialogPerState[0].Input, app.DialogPerState[0].GUI->CursorPos, iPlayer, ""/*app.Game.Pilots[iPlayer].Name*/, ::ghg::MAX_ORBITER_COUNT), "iPlayer: %i", iPlayer);
	}
	return 0;
}

static	::gpk::error_t			guiSetupAbout				(::gpk::SDialog & dialog) { return ::gpk::guiSetupButtonList<::ghg::UI_CREDITS>(*dialog.GUI, -1, 160, int16_t(-20 * ::gpk::get_value_count<::ghg::UI_CREDITS>() / 2), ::gpk::ALIGN_CENTER); }
static	::gpk::error_t			guiSetupSettings			(::gpk::SDialog & dialog) { 
	::gpk::SGUI							& gui						= *dialog.GUI;
	::gpk::guiSetupButtonList<::ghg::UI_SETTINGS>(gui, -1, {160,20}, {}, ::gpk::ALIGN_CENTER); 
	return 0;
}

::gpk::error_t					ghg::guiSetup				(::ghg::SGalaxyHellApp & app, const ::gpk::ptr_obj<::gpk::SInput> & input) {
	::dialogCreateCommon(app.DialogDesktop, input, {});

	for(uint32_t iGUI = 0; iGUI < app.DialogPerState.size(); ++iGUI) {
		::dialogCreateCommon(app.DialogPerState[iGUI], input, {});
	};

	::guiSetupLoad		(app.DialogPerState[::ghg::APP_STATE_Load			]);
	::guiSetupWelcome	(app, app.DialogPerState[::ghg::APP_STATE_Welcome	]);
	::guiSetupHome		(app, app.DialogPerState[::ghg::APP_STATE_Home		]);
	::guiSetupPlay		(app, app.DialogPerState[::ghg::APP_STATE_Play		]);
	::guiSetupShop		(app.DialogPerState[::ghg::APP_STATE_Shop			]);
	::guiSetupProfile	(app.DialogPerState[::ghg::APP_STATE_Profile		]);
	::guiSetupSettings	(app.DialogPerState[::ghg::APP_STATE_Settings		]);
	::guiSetupAbout		(app.DialogPerState[::ghg::APP_STATE_About			]);

	::virtualKeyboardSetup(app.DialogDesktop, app.VirtualKeyboard);
	return 0;
}

static	::gpk::error_t			guiHandleLoad				(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & gui, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl < (gui.Controls.Text.size() - 2)) {
		gerror_if(0 > ::ghg::solarSystemLoad(app.Game, gui.Controls.Text[idControl + 1].Text), "%s", gui.Controls.Text[idControl + 1].Text.begin());
	}
	return ::ghg::APP_STATE_Home; 
}

static	::gpk::error_t			dialogCreateLoad			(::gpk::SDialog & dialogLoad, ::gpk::view_array<::gpk::vcc> pathFileNames, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::SCoord2<float> & cursorPos) { 
	::dialogCreateCommon(dialogLoad, inputState, cursorPos);
	return ::gpk::guiSetupButtonList(*dialogLoad.GUI, pathFileNames, dialogLoad.Root, 256, (int16_t)(-20 * pathFileNames.size() / 2), ::gpk::ALIGN_CENTER, ::gpk::ALIGN_LEFT);
}

static	::gpk::error_t			guiHandleHome				(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & game) { 
	switch((::ghg::UI_HOME)idControl) {
	case ::ghg::UI_HOME_Start: 
		::ghg::solarSystemReset(game);
		game.PlayState.PlayerCount = (uint32_t)app.TunerPlayerCount->ValueCurrent;
		for(uint32_t iPlayer = app.Players.size(); iPlayer < app.Game.PlayState.PlayerCount; ++iPlayer) {
			char text [64] = {};
			sprintf_s(text, "Player %i", app.Players.size() + 1);
			app.Players.push_back({::gpk::label(text)});
		}
		for(uint32_t iPlayer = 0; iPlayer < app.Players.size(); ++iPlayer) {
			if(iPlayer >= app.Game.Pilots.size())
				app.Game.PilotCreate({app.Players[iPlayer].Name, ::ghg::PLAYER_COLORS[iPlayer]});
			else
				app.Game.Pilots[iPlayer].Name = app.Players[iPlayer].Name;
		}
		game.PlayState.Paused			= false;
		return ::ghg::APP_STATE_Play;
	case ::ghg::UI_HOME_Continue: 
		game.PlayState.Paused			= false;
		return ::ghg::APP_STATE_Play;
	case ::ghg::UI_HOME_Save:
		gerror_if(0 > app.Save(::ghg::SAVE_MODE_USER), "%s", "");
		break;
	case ::ghg::UI_HOME_Load: {
		::gpk::array_obj<::gpk::vcc>	pathFileNames;
		::ghg::listFilesSavegame(app, app.SavegameFolder, pathFileNames);
		pathFileNames.push_back(::gpk::vcs{"Back"});
		dialogCreateLoad(app.DialogPerState[::ghg::APP_STATE_Load], pathFileNames, app.DialogPerState[::ghg::APP_STATE_Home].Input, app.DialogPerState[::ghg::APP_STATE_Home].GUI->CursorPos);
		return ::ghg::APP_STATE_Load;
	}
	case ::ghg::UI_HOME_Settings: return ::ghg::APP_STATE_Settings; 
	case ::ghg::UI_HOME_Credits: return ::ghg::APP_STATE_About; 
	case ::ghg::UI_HOME_Shop: return ::ghg::APP_STATE_Shop; 
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

static	::gpk::error_t			guiHandleWelcome			(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (uint32_t)::ghg::UI_WELCOME_Confirm && app.InputboxText.size()) {
		app.Players.push_back({::gpk::label(::gpk::vcc{app.InputboxText})});
		if(app.Game.Pilots.size())
			app.Game.Pilots[0] = {app.Players[0].Name, ::ghg::PLAYER_COLORS[0]};
		else
			app.Game.PilotCreate({app.Players[0].Name, ::ghg::PLAYER_COLORS[0]});

		app.Save(::ghg::SAVE_MODE_AUTO);
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Welcome; 
}
static	::gpk::error_t			guiHandleSetup				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (uint32_t)::ghg::UI_SETTINGS_Audio) {
		return ::ghg::APP_STATE_Home;
	}
	else if(idControl == (uint32_t)::ghg::UI_SETTINGS_Graphics) {
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

static	::gpk::error_t			uiPlayerUpdateHome			(::ghg::SUIPlayer & uiPlayer, uint16_t iPlayer, const ::gpk::vcc playerName, const ::gpk::SColorBGRA & shipColor, const ::ghg::SShipScore & shipScore) { 
	sprintf_s(uiPlayer.TextScore			.Storage, "Score           : %llu"	, shipScore.Score			);
	sprintf_s(uiPlayer.TextHits				.Storage, "Hits            : %llu"	, shipScore.Hits			);
	sprintf_s(uiPlayer.TextShots			.Storage, "Shots           : %llu"	, shipScore.Shots			);
	sprintf_s(uiPlayer.TextBullets			.Storage, "Bullets         : %llu"	, shipScore.Bullets			);
	sprintf_s(uiPlayer.TextDamageDone		.Storage, "Damage dealt    : %llu"	, shipScore.DamageDone		);
	sprintf_s(uiPlayer.TextDamageReceived	.Storage, "Damage received : %llu"	, shipScore.DamageReceived	);
	sprintf_s(uiPlayer.TextHitsSurvived		.Storage, "Hits survived   : %llu"	, shipScore.HitsSurvived	);
	sprintf_s(uiPlayer.TextOrbitersLost		.Storage, "Orbiters lost   : %u"	, shipScore.OrbitersLost	);
	sprintf_s(uiPlayer.TextKilledShips		.Storage, "Killed ships    : %u"	, shipScore.KilledShips		);
	sprintf_s(uiPlayer.TextKilledOrbiters	.Storage, "Killed orbiters : %u"	, shipScore.KilledOrbiters	);
			
	::gpk::SDialog						& playerDialog				= uiPlayer.DialogHome;
	::gpk::SGUI							& playerGUI					= *playerDialog.GUI;
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 +::ghg::UI_PLAYER_Score					, uiPlayer.TextScore			.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 +::ghg::UI_PLAYER_SCORE_Hits			, uiPlayer.TextHits				.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 +::ghg::UI_PLAYER_SCORE_Shots			, uiPlayer.TextShots			.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 +::ghg::UI_PLAYER_SCORE_Bullets			, uiPlayer.TextBullets			.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 +::ghg::UI_PLAYER_SCORE_DamageDone		, uiPlayer.TextDamageDone		.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 +::ghg::UI_PLAYER_SCORE_DamageReceived	, uiPlayer.TextDamageReceived	.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 +::ghg::UI_PLAYER_SCORE_HitsSurvived	, uiPlayer.TextHitsSurvived		.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 +::ghg::UI_PLAYER_SCORE_OrbitersLost	, uiPlayer.TextOrbitersLost		.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 +::ghg::UI_PLAYER_SCORE_KilledShips		, uiPlayer.TextKilledShips		.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 +::ghg::UI_PLAYER_SCORE_KilledOrbiters 	, uiPlayer.TextKilledOrbiters	.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 + ::gpk::get_value_count<::ghg::UI_PLAYER_SCORE>() + ::ghg::UI_PROFILE_Name, playerName));

	{
		::gpk::SControl						& control					= playerGUI.Controls.Controls[1 + ::gpk::get_value_count<::ghg::UI_PLAYER_SCORE>() + ::ghg::UI_PROFILE_Name];
		control.ColorTheme				= int16_t(3 + iPlayer);
		const ::gpk::SControlTheme											& theme													= playerGUI.ControlThemes[(0 == control.ColorTheme) ? playerGUI.ThemeDefault : control.ColorTheme - 1];
		const ::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL];
		playerGUI.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]] = shipColor;
		playerGUI.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]] = shipColor;
	}
	return 0;
}

static	::gpk::error_t			uiPlayerUpdatePlay			(::ghg::SUIPlayer & uiPlayer, uint32_t iPlayer, const ::ghg::SGalaxyHell & game, ::std::mutex & lockGame, ::ghg::SGalaxyHellDrawCache & drawCache) {
	::gpk::SDialog						& playerDialog				= uiPlayer.DialogPlay;
	::gpk::SGUI							& playerGUI					= *playerDialog.GUI;

	sprintf_s(uiPlayer.TextScore.Storage, "Score: %llu"	, game.ShipState.ShipScores[iPlayer].Score);
	const ::ghg::SShipCore				& shipCore					= game.ShipState.ShipCores[iPlayer];
	const ::gpk::SColorFloat			shipColor					= (shipCore.Team ? ::gpk::RED : ::gpk::SColorFloat(game.Pilots[iPlayer].Color));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 + ::ghg::UI_PLAYER_Score, uiPlayer.TextScore.Storage));
	gpk_necs(::gpk::controlTextSet(playerGUI, 1 + ::ghg::UI_PLAYER_Name	, game.Pilots[iPlayer].Name));
	{
		::gpk::SControl						& control					= playerGUI.Controls.Controls[1 + ::ghg::UI_PLAYER_Name];
		control.ColorTheme				= int16_t(3 + iPlayer);
		const ::gpk::SControlTheme											& theme													= playerGUI.ControlThemes[(0 == control.ColorTheme) ? playerGUI.ThemeDefault : control.ColorTheme - 1];
		const ::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL];
		playerGUI.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]] = shipColor;
		playerGUI.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]] = shipColor;
	}
	for(uint32_t iOrbiter = 0, countViewports = game.ShipState.ShipParts[iPlayer].size(); iOrbiter < countViewports; ++iOrbiter) {
		::ghg::SUIPlayShipPartViewport		& viewport			= *uiPlayer.ModuleViewports[iOrbiter];

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

		viewport.GaugeLife.SetValue(healthRatio	);

		{ // Update ship part render and health bar
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
			::gpk::mutex_guard										lock					(lockGame);
			memset(targetPixels.begin(), 0, targetPixels.byte_count());
			memset(depthBuffer.begin(), -1, depthBuffer.byte_count());

			drawCache.PixelCoords.clear();
			uint32_t												pixelsDrawn				= 0;
			pixelsDrawn											+= ::ghg::drawShipOrbiter(game.ShipState, orbiter, {}, (float)game.DecoState.AnimationTime, matrixView, targetPixels, depthBuffer, drawCache);

			if(healthRatio) {
				::ghg::gaugeImageUpdate(viewport.GaugeLife, targetPixels, colorLife, colorLife, colorLife);
			}
		}

		{ // Update weapon load bar
			::ghg::TRenderTarget				& renderTarget		= viewport.RenderTargetWeaponLoad;
			::gpk::view_grid<::gpk::SColorBGRA>	targetPixels		= renderTarget.Color		; 
			::gpk::view_grid<uint32_t>			depthBuffer			= renderTarget.DepthStencil	;
			::gpk::mutex_guard					lock				(lockGame);
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

		{ // Update weapon type bar
			::gpk::mutex_guard					lock				(lockGame);
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
	return 0;
}

static	::gpk::error_t			guiUpdatePlay				(::ghg::SGalaxyHellApp & app) { 
	::ghg::SGalaxyHell					& game						= app.Game;

	::gpk::SDialog						& dialog					= app.DialogPerState[::ghg::APP_STATE_Play];
	::gpk::SGUI							& gui						= *dialog.GUI;

	sprintf_s(app.UIPlay.TextLevel.Storage, "Level: %i", game.PlayState.Stage);
	sprintfTime("Real Time: "		, app.UIPlay.TextTimeReal	.Storage, game.PlayState.TimeReal);
	sprintfTime("Stage Time: "		, app.UIPlay.TextTimeStage	.Storage, game.PlayState.TimeRealStage);
	::gpk::controlTextSet(gui, 1 + ::ghg::UI_PLAY_Level		, ::gpk::vcs{app.UIPlay.TextLevel		.Storage});
	::gpk::controlTextSet(gui, 1 + ::ghg::UI_PLAY_TimeStage , ::gpk::vcs{app.UIPlay.TextTimeStage	.Storage});
	::gpk::controlTextSet(gui, 1 + ::ghg::UI_PLAY_TimeReal	, ::gpk::vcs{app.UIPlay.TextTimeReal	.Storage});

	if(0 == game.ShipState.ShipCores.size())
		return 0;

	for(uint32_t iPlayer = 0; iPlayer < app.TunerPlayerCount->ValueCurrent && iPlayer < app.UIPlay.PlayerUI.size() ; ++iPlayer) { app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI->Controls.States[0].Hidden = false; }
	for(uint32_t iPlayer = (uint32_t)app.TunerPlayerCount->ValueCurrent; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) { app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI->Controls.States[0].Hidden = true; }

	::ghg::SGalaxyHellDrawCache			& drawCache				= app.UIPlay.DrawCache;
	drawCache.LightPointsWorld.clear();
	drawCache.LightColorsWorld.clear();
	::ghg::getLightArraysFromShips(game.ShipState, drawCache.LightPointsWorld, drawCache.LightColorsWorld);
	drawCache.LightPointsModel.reserve(drawCache.LightPointsWorld.size());
	drawCache.LightColorsModel.reserve(drawCache.LightColorsWorld.size());
	if(app.ActiveState == ::ghg::APP_STATE_Play) {
		for(uint32_t iPlayer = 0; iPlayer < game.PlayState.PlayerCount; ++iPlayer) {
			::ghg::SUIPlayer					& uiPlayer				= app.UIPlay.PlayerUI[iPlayer];
			gpk_necall(::uiPlayerUpdatePlay(uiPlayer, iPlayer, app.Game, app.Game.LockUpdate, drawCache), "iPlayer: %i", iPlayer);
			uiPlayer.DialogPlay.GUI->Controls.States[0].Hidden = false; 
		}
		for(uint32_t iPlayer = app.Game.PlayState.PlayerCount; iPlayer < ghg::MAX_PLAYERS; ++iPlayer) { 
			app.UIPlay.PlayerUI[iPlayer].DialogPlay.GUI->Controls.States[0].Hidden = true; 
		}
	}
	return 0;
}

static ::gpk::error_t			guiUpdateHome				(::ghg::SGalaxyHellApp & app) {
	for(uint32_t iPlayer = 0; iPlayer < app.TunerPlayerCount->ValueCurrent; ++iPlayer) {
		::ghg::SUIPlayer					& uiPlayer		= app.UIPlay.PlayerUI[iPlayer];
		if(app.Players.size() <= iPlayer) {
			::gpk::mutex_guard					lock				(app.Game.LockUpdate);
			char text [64] = {};
			sprintf_s(text, "Player %i", app.Players.size() + 1);
			app.Players.push_back({::gpk::label(text)});
		}

		::gpk::SColorFloat					shipColor		= ::ghg::PLAYER_COLORS[iPlayer];
		if((iPlayer < app.Game.ShipState.ShipCores.size())) {
			const ::ghg::SShipCore				& shipCore		=  app.Game.ShipState.ShipCores[iPlayer];
			shipColor						= (shipCore.Team ? ::ghg::PLAYER_COLORS[iPlayer] : app.Game.Pilots[iPlayer].Color);
		}

		gpk_necall(::uiPlayerUpdateHome(uiPlayer, int16_t(iPlayer), app.Players[iPlayer].Name, shipColor
			, (app.Game.ShipState.ShipScores.size() <= iPlayer) ? ::ghg::SShipScore{} : app.Game.ShipState.ShipScores[iPlayer]
			), "iPlayer: %i", iPlayer
		);
		app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI->Controls.States[0].Hidden = false; 
	}
	for(uint32_t iPlayer = (uint32_t)app.TunerPlayerCount->ValueCurrent; iPlayer < ghg::MAX_PLAYERS; ++iPlayer) 
		app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI->Controls.States[0].Hidden = true; 
	return 0;
}

::gpk::error_t					ghg::guiUpdate				(::ghg::SGalaxyHellApp & app, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents) {
	{
		::gpk::SDialog						& dialog					= app.DialogDesktop;
		::gpk::SGUI							& gui						= *dialog.GUI;
		::gpk::SInput						& input						= *dialog.Input;
		::gpk::guiProcessInput(gui, input, sysEvents); 

		dialog.Update();
		::gpk::array_pod<uint32_t>			controlsToProcess			= {};
		::gpk::guiGetProcessableControls(gui, controlsToProcess);

		{
			::gpk::SVirtualKeyboard & vk = app.VirtualKeyboard;
			vk.Events.clear();
			for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
				uint32_t							idControl			= controlsToProcess		[iControl];
				const ::gpk::SControlState			& controlState		= gui.Controls.States	[idControl];
				//bool								handled				= false;
				if(controlState.Execute) {
					::gpk::virtualKeyboardHandleEvent(vk, idControl);
					for(uint32_t iEvent = 0; iEvent < vk.Events.size(); ++iEvent) {
						if(vk.Events[iEvent].Type == ::gpk::VK_EVENT_RELEASE) {
							app.InputboxText.push_back((char)vk.Events[iEvent].ScanCode);
						}
						else if(vk.Events[iEvent].Type == ::gpk::VK_EVENT_EDIT) {
							if(vk.Events[iEvent].ScanCode == gpk::VK_SCANCODE_Backspace)
								app.InputboxText.pop_back(0);
							else if(vk.Events[iEvent].ScanCode == gpk::VK_SCANCODE_Enter) {
								app.Players.push_back({::gpk::label(::gpk::vcc{app.InputboxText})});
								app.Game.Pilots[0] = {app.Players[0].Name, ::ghg::PLAYER_COLORS[0]};
								app.Save(::ghg::SAVE_MODE_AUTO);
							}
							else if(vk.Events[iEvent].ScanCode == gpk::VK_SCANCODE_Escape) {
								app.InputboxText.clear();
							}
						}
					}
				}
			}
			::gpk::controlTextSet(*app.DialogPerState[::ghg::APP_STATE_Welcome].GUI, app.Inputbox->IdClient, ::gpk::vcc{app.InputboxText});
		}
	}

	::ghg::SGalaxyHell					& game						= app.Game;

	::ghg::APP_STATE					appState					= app.ActiveState; 
	for(uint32_t iAppState = 0; iAppState < ::ghg::APP_STATE_COUNT; ++iAppState) {
		::gpk::SDialog						& dialog					= app.DialogPerState[iAppState];
		::gpk::SGUI							& gui						= *dialog.GUI;
		::gpk::SInput						& input						= *dialog.Input;
		::gpk::guiProcessInput(gui, input, sysEvents); 
	}

	for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) {
		::ghg::SUIPlayer					& uiPlayer					= app.UIPlay.PlayerUI[iPlayer];
		::gpk::guiProcessInput(*uiPlayer.DialogPlay.GUI, *uiPlayer.DialogPlay.Input, sysEvents);
		::gpk::guiProcessInput(*uiPlayer.DialogHome.GUI, *uiPlayer.DialogHome.Input, sysEvents);
		{
			::gpk::SDialog				& playerDialog	= uiPlayer.DialogHome;
			::gpk::SGUI					& playerGUI		= *playerDialog.GUI;
			playerGUI.Controls.States[uiPlayer.VirtualKeyboard.IdRoot].Hidden	= false;
		}

	}
	
	if(app.ActiveState == ::ghg::APP_STATE_Play)
		::guiUpdatePlay(app);
	else 
		::guiUpdateHome(app);

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
			case ::ghg::APP_STATE_Load		: appState = (::ghg::APP_STATE)::guiHandleLoad	(app, gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Welcome	: appState = (::ghg::APP_STATE)::guiHandleWelcome	(app, gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Home		: appState = (::ghg::APP_STATE)::guiHandleHome	(app, gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Profile	: appState = (::ghg::APP_STATE)::guiHandleUser	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Shop		: appState = (::ghg::APP_STATE)::guiHandleShop	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Play		: appState = (::ghg::APP_STATE)::guiHandlePlay	(app, gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Settings	: appState = (::ghg::APP_STATE)::guiHandleSetup	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_About		: appState = (::ghg::APP_STATE)::guiHandleAbout	(gui, idControl, game); handled = true; break;
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


