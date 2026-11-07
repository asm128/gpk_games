#include "gpk_galaxy_hell_app.h"
#include "gpk_file.h"
#include "gpk_raster_lh.h"
#include "gpk_grid_copy.h"
#include "gpk_sphere.h"

using ::gpk::get_value_namep, ::gpk::get_enum_namep, ::gpk::failed;
GPK_USING_TYPEINT();

stxp	::gpk::GUI_COLOR_MODE	GHG_MENU_COLOR_MODE		= ::gpk::GUI_COLOR_MODE_3D;
stxp	bool					BACKGROUND_3D_ONLY		= true;
stxp	::gpk::n2u1_t			MODULE_CAMERA_SIZE		= {48, 48};
stxp	::gpk::n2u1_t			MODULE_VIEWPORT_SIZE	= {128, 64};
stxp	::gpk::n2u1_t			WEAPON_BAR_SIZE			= {96, 16};
stxp	::gpk::n2u1_t			PLAYER_UI_SIZE			= {320, 40};
stxp	::gpk::n2u1_t			HOME_BUTTON_SIZE		= {160, 24};

static	gpk::err_t			guiSetupCommon			(::gpk::SGUI & gui) {
	gui.ColorModeDefault	= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault		= ::gpk::ASCII_COLOR_DARKRED * 16 + 10;
	gui.SelectedFont		= 7;

	::gpk::pobj<::gpk::SGUIColors>	colors;
	colors.create(*gui.Colors);
	gui.Colors				= colors;
	return 0;
}

static	gpk::err_t			dialogCreateCommon		(::gpk::SDialog & dialogLoad, const ::gpk::pobj<::gpk::SInput> & inputState, const ::gpk::n2f2_t & cursorPos) { 
	dialogLoad						= {};
	dialogLoad.Input				= inputState;
	dialogLoad.GUI->CursorPos		= cursorPos;
	dialogLoad.GUI->Controls.Draw[0].NoClient	= true;
	dialogLoad.GUI->Controls.Draw[0].NoBorder	= true;
	::guiSetupCommon(*dialogLoad.GUI);
	dialogLoad.Update();
	return 0;
}

// -------------------------------- Set up player GUI for home screen
static	gpk::err_t			uiPlayerSetupHome		(::ghg::SUIPlayer & uiPlayer, ::gpk::pobj<::gpk::SInput> input, ::gpk::n2f2_t cursorPos, u2_t iPlayer, const ::gpk::vcsc_t playerName) {
	::gpk::ALIGN						playerAlign			= {};
	::gpk::n2s1_t						playerPosition		= {};
	switch(iPlayer) {
	default:
	case 0: playerAlign = ::gpk::ALIGN_BOTTOM_LEFT;		break;
	case 1: playerAlign = ::gpk::ALIGN_TOP_RIGHT;		break;
	case 2: playerAlign = ::gpk::ALIGN_BOTTOM_RIGHT;	break;
	case 3: playerAlign = ::gpk::ALIGN_TOP_LEFT;		break;
	}
	switch(iPlayer) {
	default:
	case 0: playerPosition = {}; break; //{0, -10/2}; break;	// I don't know where this measure comes from
	case 1: playerPosition = {}; break; //{0, s1_t(-20 * (s1_t)::gpk::get_value_count<::ghg::UI_PLAYER_SCORE>())}; break;
	case 2: playerPosition = {}; break; //{120}; break;
	case 3: playerPosition = {}; break; //{-120}; break;
	}
	::gpk::SDialog				& playerDialog		= uiPlayer.DialogHome;
	if_fail_fe(::dialogCreateCommon(playerDialog, input, cursorPos));
	::gpk::SGUI					& playerGUI			= *playerDialog.GUI;
	::gpk::pobj<::gpk::a8bgra>	palette;
	palette.create(*playerGUI.Colors->Palette);
	playerGUI.Colors->Palette = palette;
	playerGUI.ThemeDefault	= ::gpk::ASCII_COLOR_DARKGREY * 16 + 12;
	::gpk::guiCreateControlList<::ghg::UI_PROFILE>(playerGUI, playerDialog.Root, PLAYER_UI_SIZE, playerPosition, playerAlign, ::gpk::ALIGN_CENTER, uiPlayer.ControlsDialogHome);
	for(u2_t iButton = 0, iStop = ::gpk::get_value_count<::ghg::UI_PROFILE>() - 1, iOffset = playerDialog.Root + 1 + ghg::UI_PROFILE_Name; iButton < iStop; ++iButton) {
		playerGUI.Controls.Placement	[iOffset + iButton].Margin				= {}; 
		//playerGUI.Controls.Controls		[iButton].Border				= {2, 2, 2, 2};
		//playerGUI.Controls.Draw			[iOffset + iButton].NoBorder		= true;
		playerGUI.Controls.Draw			[iOffset + iButton].NoClient		= true;
	}
	for(u2_t iButton = 0, iStop = ::gpk::get_value_count<::ghg::UI_PROFILE>() - 1, iOffset = playerDialog.Root + 1 + ghg::UI_PROFILE_Score; iButton < iStop; ++iButton) {
		playerGUI.Controls.Placement[iOffset + iButton].Area.Offset.y		-= s1_t(16 * iButton);
		playerGUI.Controls.Placement[iOffset + iButton].Area.Size.y			= 24; 
	}

	switch(iPlayer) {
	default:
	case 0: playerAlign = ::gpk::ALIGN_BOTTOM_LEFT	; break;
	case 1: playerAlign = ::gpk::ALIGN_TOP_RIGHT	; break;
	case 2: playerAlign = ::gpk::ALIGN_BOTTOM_RIGHT	; break;
	case 3: playerAlign = ::gpk::ALIGN_TOP_LEFT		; break;
	}
	if_fail_fe(::gpk::inputBoxCreate(uiPlayer.InputBox, *playerDialog.GUI, playerDialog.Root));
	playerGUI.Controls.SetHidden(uiPlayer.InputBox.IdRoot, true);
	playerGUI.Controls.Placement[uiPlayer.InputBox.IdRoot].Align			= playerAlign;
	playerGUI.Controls.Placement[uiPlayer.InputBox.IdRoot].Area.Offset.x	= 240;

	if_fail_fe(::gpk::controlFontSet(playerGUI, playerDialog.Root + 1 + ::ghg::UI_PROFILE_Name, 10));
	if_fail_fe(::gpk::controlTextSet(playerGUI, playerDialog.Root + 1 + ::ghg::UI_PROFILE_Name, playerName));
	return 0;
}

// -------------------------------- Set up player GUI for in-stage display
static	gpk::err_t	uiPlayerSetupPlay	(::ghg::SUIPlayer & uiPlayer, ::gpk::pobj<::gpk::SInput> input, ::gpk::n2f2_t cursorPos, u2_t iPlayer, const ::gpk::vcsc_t pilotName, u2_t nShipParts) {
	::gpk::n2s1_t				playerPosition;
	::gpk::ALIGN				playerAlign		;
	switch(iPlayer) {
	default:
	case 0: playerAlign = ::gpk::ALIGN_BOTTOM_LEFT;		break;
	case 1: playerAlign = ::gpk::ALIGN_TOP_RIGHT;		break;
	case 2: playerAlign = ::gpk::ALIGN_BOTTOM_RIGHT;	break;
	case 3: playerAlign = ::gpk::ALIGN_TOP_LEFT;		break;
	}
	switch(iPlayer) {
	default:
	case 0: playerPosition = {}; break;
	case 1: playerPosition = {}; break;
	case 2: playerPosition = {}; break;
	case 3: playerPosition = {}; break;
	}
	::gpk::SDialog				& playerDialog		= uiPlayer.DialogPlay;
	if_fail_fe(::dialogCreateCommon(playerDialog, input, cursorPos));
	::gpk::SGUI					& playerGUI			= *playerDialog.GUI;
	::gpk::pobj<::gpk::a8bgra>	palette;
	palette.create(*playerGUI.Colors->Palette);
	playerGUI.Colors->Palette = palette;
	playerGUI.ThemeDefault	= ::gpk::ASCII_COLOR_DARKGREY * 16 + 12;
	::gpk::guiCreateControlList<::ghg::UI_PROFILE>(playerGUI, playerDialog.Root, PLAYER_UI_SIZE, playerPosition, playerAlign, ::gpk::ALIGN_CENTER, uiPlayer.ControlsDialogPlay);
	for(u2_t iButton = 0, iStop = ::gpk::get_value_count<::ghg::UI_PROFILE>() - 1, iOffset = playerDialog.Root + 1 + ghg::UI_PROFILE_Name; iButton < iStop; ++iButton) {
		playerGUI.Controls.Placement[iOffset + iButton].Margin		= {}; 
		//playerGUI.Controls.Controls	[iOffset + iButton].Border	= {};
		//playerGUI.Controls.Draw		[iOffset + iButton].NoBorder	= true;
		playerGUI.Controls.Draw		[iOffset + iButton].NoClient	= true;
		//playerGUI.Controls.Constraints[iOffset + iButton].AttachSizeToControl.x	= ::gpk::cid_t(iOffset + iButton + 1);
	}
	for(u2_t iButton = 0, iStop = ::gpk::get_value_count<::ghg::UI_PROFILE>() - 1, iOffset = playerDialog.Root + 1 + ghg::UI_PROFILE_Score; iButton < iStop; ++iButton) {
		playerGUI.Controls.Placement[iOffset + iButton].Area.Offset.y	-= s1_t(16 * iButton);
		playerGUI.Controls.Placement[iOffset + iButton].Area.Size.y		= 24; 
		//playerGUI.Controls.Placement[iOffset + iButton].Area.Size.x		= 24; 
		//playerGUI.Controls.Constraints[iOffset + iButton].AttachSizeToText.x	= true;
	}

	if_fail_fe(::gpk::controlFontSet(playerGUI, playerDialog.Root + 1 + ::ghg::UI_PROFILE_Name, 10));
	if_fail_fe(::gpk::controlTextSet(playerGUI, playerDialog.Root + 1 + ::ghg::UI_PROFILE_Name, pilotName));
	if_fail_fe(::gpk::controlTextSet(playerGUI, playerDialog.Root + 1 + ::ghg::UI_PROFILE_Score, ""));

	for(u2_t iOrbiter = 0, countViewports = nShipParts; iOrbiter < countViewports; ++iOrbiter) {
		s2_t						indexVP				= uiPlayer.ModuleViewports.push_back({});
		uiPlayer.ModuleViewports[indexVP]->Viewport = ::gpk::controlCreate(playerGUI); // Orbiter
		::gpk::controlCreate(playerGUI); // Weapon Load
		::gpk::controlCreate(playerGUI); // Weapon Type
		for(u2_t iControl = 0; iControl < 3; ++iControl)
			::gpk::controlSetParent(playerGUI, ::gpk::cid_t(uiPlayer.ModuleViewports[indexVP]->Viewport + iControl), playerDialog.Root); // Weapon Load

		::ghg::SUIPlayShipPartViewport	& viewport		= *uiPlayer.ModuleViewports[indexVP];
		viewport.RenderTargetOrbiter.resize(::MODULE_CAMERA_SIZE);
		viewport.RenderTargetWeaponLoad.resize(::WEAPON_BAR_SIZE);
		viewport.RenderTargetWeaponType.resize(::WEAPON_BAR_SIZE);
		::gpk::SControlPlacement	& controlOrbiter	= playerGUI.Controls.Placement[viewport.Viewport + 0];
		::gpk::SControlPlacement	& controlWeaponType	= playerGUI.Controls.Placement[viewport.Viewport + 2];
		::gpk::SControlPlacement	& controlWeaponLoad	= playerGUI.Controls.Placement[viewport.Viewport + 1];
		{
			::gpk::SControlImage	& imageOrbiter		= playerGUI.Controls.Images[viewport.Viewport + 0];
			::gpk::SControlImage	& imageWeaponType	= playerGUI.Controls.Images[viewport.Viewport + 2];
			::gpk::SControlImage	& imageWeaponLoad	= playerGUI.Controls.Images[viewport.Viewport + 1];
			imageOrbiter	.Image	= viewport.RenderTargetOrbiter.Color.View;
			imageWeaponType	.Image	= viewport.RenderTargetWeaponLoad.Color.View;
			imageWeaponLoad	.Image	= viewport.RenderTargetWeaponType.Color.View;
			imageOrbiter.ImageAlign	= imageWeaponType.ImageAlign = imageWeaponLoad.ImageAlign	= ::gpk::ALIGN_CENTER;
		}

			 if(0 == iPlayer) controlOrbiter.Align = controlWeaponLoad.Align = controlWeaponType.Align = ::gpk::ALIGN_CENTER_LEFT	;
		else if(1 == iPlayer) controlOrbiter.Align = controlWeaponLoad.Align = controlWeaponType.Align = ::gpk::ALIGN_CENTER_RIGHT	;
		else if(2 == iPlayer) controlOrbiter.Align = controlWeaponLoad.Align = controlWeaponType.Align = ::gpk::ALIGN_CENTER_BOTTOM	;
		else if(3 == iPlayer) controlOrbiter.Align = controlWeaponLoad.Align = controlWeaponType.Align = ::gpk::ALIGN_CENTER_TOP	;
		if(1 >= iPlayer)	controlOrbiter.Area.Offset	= controlWeaponLoad.Area.Offset	= controlWeaponType.Area.Offset	= {0, s1_t(-(s1_t(countViewports * (MODULE_CAMERA_SIZE.y + 4)) >> 1) + (MODULE_CAMERA_SIZE.y + 4) * iOrbiter)};
		else	 			controlOrbiter.Area.Offset	= controlWeaponLoad.Area.Offset	= controlWeaponType.Area.Offset	= {s1_t(-(s1_t(countViewports * (WEAPON_BAR_SIZE.x + 4)) >> 1) + (WEAPON_BAR_SIZE.x + 4) * iOrbiter)};
		controlOrbiter.Area.Size	= MODULE_CAMERA_SIZE.s1();
		controlOrbiter.Border		= {};
		controlOrbiter.Margin		= {};

		controlWeaponLoad.Area.Size	= controlWeaponType.Area.Size	= WEAPON_BAR_SIZE.s1();	//MODULE_VIEWPORT_SIZE.s1();
		controlWeaponLoad.Border	= controlWeaponType.Border		= {1, 1, 1, 1};
		controlWeaponLoad.Margin	= controlWeaponType.Margin		= {};

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

		playerGUI.Controls.Text		[viewport.Viewport + 1].Align	= ::gpk::ALIGN_CENTER; //iPlayer ? ::gpk::ALIGN_CENTER_RIGHT : ::gpk::ALIGN_CENTER_LEFT;
		playerGUI.Controls.Text		[viewport.Viewport + 2].Align	= ::gpk::ALIGN_CENTER; //iPlayer ? ::gpk::ALIGN_CENTER_RIGHT : ::gpk::ALIGN_CENTER_LEFT;

		playerGUI.Controls.Draw	[viewport.Viewport + 0].NoClient	= 
		playerGUI.Controls.Draw	[viewport.Viewport + 1].NoClient	= 
		playerGUI.Controls.Draw	[viewport.Viewport + 2].NoClient	= true;
		playerGUI.Controls.Draw	[viewport.Viewport + 0].NoBorder	= true;
		playerGUI.Controls.Draw	[viewport.Viewport + 1].NoBorder	= true;
		//playerGUI.Controls.Draw	[viewport.Viewport + 2].NoBorder	= true;

	::gpk::m4f2_t				& matrixProjection	= viewport.MatrixProjection;
		matrixProjection.FieldOfView(::gpk::math_pi * .25, MODULE_CAMERA_SIZE.x / (double)MODULE_CAMERA_SIZE.y, 0.01, 500.0);
	::gpk::m4f2_t				matrixViewport		= {};
		matrixViewport.ViewportLH(MODULE_CAMERA_SIZE.u1());
		matrixProjection		*= matrixViewport;


		const ::gpk::circlef2_t		circleLife			= {::gpk::min(MODULE_CAMERA_SIZE.x, MODULE_CAMERA_SIZE.y) * .5f - 6 * 0, viewport.RenderTargetOrbiter.Color.View.metrics().f2() * .5};
		const ::gpk::circlef2_t		circleDelay			= {::gpk::min(MODULE_CAMERA_SIZE.x, MODULE_CAMERA_SIZE.y) * .5f - 6 * 1, viewport.RenderTargetOrbiter.Color.View.metrics().f2() * .5};
		const ::gpk::circlef2_t		circleCooldown		= {::gpk::min(MODULE_CAMERA_SIZE.x, MODULE_CAMERA_SIZE.y) * .5f - 6 * 2, viewport.RenderTargetOrbiter.Color.View.metrics().f2() * .5};
		
		::ghg::gaugeBuildRadial(viewport.GaugeLife, circleLife, 32, 12);
	}
	playerDialog.Update();
	return 0;
}

//static gpk::err_t	guiSetupInit		(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_LOAD>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	gpk::err_t	guiSetupLoad		(::gpk::SDialog & /*dialog*/) { 
	//::gpk::ai32					temp;
	//temp.resize();
	//if_fail_fe(::gpk::guiSetupButtonList<::ghg::UI_LOAD>(*dialog.GUI, dialog.Root, 60, 0, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, {})); 
	return 0;
}

static	gpk::err_t	guiSetupHome		(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialog) { 
	if_fail_fe(::gpk::guiCreateControlList<::ghg::UI_HOME>(*dialog.GUI, dialog.Root, HOME_BUTTON_SIZE, {0, s1_t(-HOME_BUTTON_SIZE.y * ::gpk::get_value_count<::ghg::UI_HOME>() / 2)}, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, app.DialogControls[::ghg::APP_STATE_Home])); 
	for(u2_t iButton = 0; iButton < ::gpk::get_value_count<::ghg::UI_HOME>(); ++iButton)
		dialog.GUI->Controls.Placement[iButton + 1].Area.Offset.y += 0;

	for(u2_t iButton = ::ghg::UI_HOME_Start; iButton < ::gpk::get_value_count<::ghg::UI_HOME>(); ++iButton)
		dialog.GUI->Controls.Placement[iButton + 1].Area.Offset.y += 36;

	dialog.GUI->Controls.Text[::ghg::UI_HOME_Start + 1].Text = "Start new game";

	if_fail_fe(::gpk::tunerCreate(dialog, app.TunerPlayerCount));
	app.TunerPlayerCount->ValueLimits.Min	= 1;
	app.TunerPlayerCount->ValueLimits.Max	= ::ghg::MAX_PLAYERS;
	app.TunerPlayerCount->FuncValueFormat	= [](::gpk::vcsc_t & string, u0_t value, const ::gpk::minmax<u0_t> & /*limits*/) { 
		string = (value == 1) ? ::gpk::vcst_t("%lli Player") : ::gpk::vcst_t("%lli Players"); 
		return 0; 
	};
	app.TunerPlayerCount->SetValue(1);
	//tuner->IdGUIControl
	::gpk::SControlPlacement	& control					= dialog.GUI->Controls.Placement[app.TunerPlayerCount->IdGUIControl];
	control.Area.Size.x		= 160;
	control.Area.Size.y		= 24;
	control.Area.Offset.y	= s1_t(dialog.GUI->Controls.Placement[::ghg::UI_HOME_Start + 1].Area.Offset.y - (dialog.GUI->Controls.Placement[::ghg::UI_HOME_Start + 1].Area.Size.y >> 1) - (control.Area.Size.y >> 1));
	control.Align			= ::gpk::ALIGN_CENTER;

	app.UIPlay.PlayerUI.resize(::ghg::MAX_PLAYERS);	
	for(u2_t iPlayer = 0; iPlayer < ::ghg::MAX_PLAYERS; ++iPlayer) {
		::ghg::SUIPlayer			& uiPlayer					= app.UIPlay.PlayerUI[iPlayer];
		if_fail_fef(::uiPlayerSetupHome(uiPlayer, dialog.Input, dialog.GUI->CursorPos, iPlayer, ""/*app.Players[iPlayer].Name*/), "iPlayer: %i", iPlayer);
	}
	return 0;
}
static	gpk::err_t	guiSetupProfile		(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialog) { return ::gpk::guiCreateControlList<::ghg::UI_PLAY>(*dialog.GUI, dialog.Root,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT, ::gpk::ALIGN_CENTER, app.DialogControls[::ghg::APP_STATE_Profile]); }
static	gpk::err_t	guiSetupShop		(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialog) { 
	::gpk::SGUI					& gui				= *dialog.GUI;
	if_fail_fe(::gpk::guiCreateControlList<::ghg::UI_SHOP>(gui, dialog.Root, 60, 0, ::gpk::ALIGN_BOTTOM_RIGHT, ::gpk::ALIGN_CENTER, app.DialogControls[::ghg::APP_STATE_Shop]));

	::gpk::pobj<::gpk::SDialogViewport>	viewport	= {};
	if_fail_fe(::gpk::viewportCreate(dialog, viewport));
	//viewport->Settings.DisplacementLockX	= false;;
	//viewport->Settings.DisplacementLockY	= false;;
	gui.Controls.Placement[viewport->IdGUIControl	].Align			= ::gpk::ALIGN_CENTER;
	gui.Controls.Placement[viewport->IdGUIControl	].Area.Offset	= {};
	gui.Controls.Placement[viewport->IdGUIControl	].Area.Size		= {640, 480};
	gui.Controls.Images		[viewport->IdClient		].ImageInvertY	= true;
	return 0;
}

static	gpk::err_t	guiSetupWelcome		(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialog) { 
	::gpk::SGUI					& gui				= *dialog.GUI;
	if_fail_fe(::gpk::guiCreateControlList<::ghg::UI_WELCOME>(gui, dialog.Root, ::gpk::n2u1_t{128, 32}, ::gpk::n2s1_t{0, 64}, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, app.DialogControls[::ghg::APP_STATE_Welcome]));
	::gpk::viewportCreate(dialog, app.Inputbox);
	::gpk::pobj<::gpk::SDialogViewport>	viewport	= app.Inputbox;
	gui.Controls.Placement[viewport->IdGUIControl	].Align			= ::gpk::ALIGN_CENTER;
	gui.Controls.Placement[viewport->IdGUIControl	].Area.Offset	= {};
	gui.Controls.Placement[viewport->IdClient		].Area.Size		= {256, 20};
	gui.Controls.Text		[viewport->IdTitle		].Text			= "Enter your name:";
	::gpk::viewportAdjustSize(gui.Controls.Placement[viewport->IdGUIControl].Area.Size, gui.Controls.Placement[viewport->IdClient].Area.Size);
	gui.Controls.Images		[viewport->IdClient		].ImageInvertY	= true;
	return 0;
}

static	gpk::err_t	guiSetupPlay		(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialog) { 
	dialog.GUI->SelectedFont	= 6;

	if_fail_fe(::gpk::guiCreateControlList<::ghg::UI_PLAY>(*dialog.GUI, dialog.Root, {220, 18}, {}, ::gpk::ALIGN_TOP_RIGHT, ::gpk::ALIGN_CENTER, app.DialogControls[::ghg::APP_STATE_Play]));
	for(u2_t iButton = 0; iButton < ::ghg::UI_PLAY_Level; ++iButton) {
		//dialog.GUI->Controls.Text	[iButton + 1].FontSelected	= ::gpk::ALIGN_TOP_LEFT;
	}
	for(u2_t iButton = ::ghg::UI_PLAY_Level; iButton < ::gpk::get_value_count<::ghg::UI_PLAY>(); ++iButton) {
		dialog.GUI->Controls.Text		[iButton + 1].FontSelected	= 7;
		dialog.GUI->Controls.Draw		[iButton + 1].ColorTheme	= ::gpk::ASCII_COLOR_GREEN * 16 + 6;
		dialog.GUI->Controls.Placement[iButton + 1].Align			= ::gpk::ALIGN_TOP_LEFT;
		dialog.GUI->Controls.Placement[iButton + 1].Area.Offset.y	-= s1_t(18 - 2 * (iButton - 1));
		dialog.GUI->Controls.Placement[iButton + 1].Area.Size.y	+= 2;
	}

	// Setup new viewports if necessary. This may happen if the ship acquires new modules while playing the stage
	app.UIPlay.PlayerUI.resize(::ghg::MAX_PLAYERS);	
	for(u2_t iPlayer = 0; iPlayer < ::ghg::MAX_PLAYERS; ++iPlayer) {
		::ghg::SUIPlayer			& uiPlayer			= app.UIPlay.PlayerUI[iPlayer];
		if_fail_fef(::uiPlayerSetupPlay(uiPlayer, app.DialogPerState[0].Input, app.DialogPerState[0].GUI->CursorPos, iPlayer, ""/*app.Game.Pilots[iPlayer].Name*/, ::ghg::MAX_ORBITER_COUNT), "iPlayer: %i", iPlayer);
	}
	return 0;
}

static	gpk::err_t	guiSetupAbout		(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialog) { return ::gpk::guiCreateControlList<::ghg::UI_CREDITS>(*dialog.GUI, -1, 160, s1_t(-20 * ::gpk::get_value_count<::ghg::UI_CREDITS>() / 2), ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, app.DialogControls[::ghg::APP_STATE_About]); }
static	gpk::err_t	guiSetupSettings	(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialog) { return ::gpk::guiCreateControlList<::ghg::UI_SETTINGS>(*dialog.GUI, -1, {160,20}, {}, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, app.DialogControls[::ghg::APP_STATE_Settings]); }

gpk::err_t			ghg::guiSetup		(::ghg::SGalaxyHellApp & app, const ::gpk::pobj<::gpk::SInput> & input) {
	if_fail_fe(::dialogCreateCommon(app.DialogDesktop, input, {}));

	for(u2_t iGUI = 0; iGUI < app.DialogPerState.size(); ++iGUI)
		if_fail_fef(::dialogCreateCommon(app.DialogPerState[iGUI], input, {}), "iGUI: %i", iGUI);

	if_fail_fe(::guiSetupWelcome	(app, app.DialogPerState[::ghg::APP_STATE_Welcome	]));
	if_fail_fe(::guiSetupHome	(app, app.DialogPerState[::ghg::APP_STATE_Home		]));
	if_fail_fe(::guiSetupPlay	(app, app.DialogPerState[::ghg::APP_STATE_Play		]));
	if_fail_fe(::guiSetupShop	(app, app.DialogPerState[::ghg::APP_STATE_Shop		]));
	if_fail_fe(::guiSetupProfile	(app, app.DialogPerState[::ghg::APP_STATE_Profile	]));
	if_fail_fe(::guiSetupSettings(app, app.DialogPerState[::ghg::APP_STATE_Settings	]));
	if_fail_fe(::guiSetupAbout	(app, app.DialogPerState[::ghg::APP_STATE_About		]));
	if_fail_fe(::guiSetupLoad	(app.DialogPerState[::ghg::APP_STATE_Load			]));

	if_fail_fe(::gpk::virtualKeyboardSetup437(*app.DialogDesktop.GUI, app.VirtualKeyboard));
	return 0;
}

static	gpk::err_t	guiHandleLoad		(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & gui, u2_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl < (gui.Controls.Text.size() - 2)) {
		try {
			if_fail_fef(::ghg::solarSystemLoad(app.Game, gui.Controls.Text[idControl + 1].Text), "Failed to load file %i: '%.*s'", idControl + 1, GPK_VIEW_ARGS(gui.Controls.Text[idControl + 1].Text));
		}
		catch (const sc_t * ) {
			::ghg::solarSystemReset(app.Game);
			return -1;
		}
		app.TunerPlayerCount->SetValue((u0_t)app.Game.PlayState.Constants.Players);
		for(u2_t iPilot = 0; iPilot < app.Game.PlayState.Constants.Players; ++iPilot) {
			const ::gpk::vcsc_t			namePilot	= app.Game.Pilots[iPilot].Name;
			for(u2_t iPlayer = 0; iPlayer < app.Game.PlayState.Constants.Players; ++iPlayer) {
				if(iPlayer >= app.Players.size())
					app.AddNewPlayer(namePilot);

				const ::gpk::vcsc_t			namePlayer	= app.Players[iPlayer].Name;
				if(namePilot == namePlayer) {
					::std::swap(app.Players[iPlayer], app.Players[iPilot]);
					break;
				}
			}
			for(u2_t iPlayer = app.Game.PlayState.Constants.Players; iPlayer < app.Players.size(); ++iPlayer) {
				const ::gpk::vcsc_t			namePlayer	= app.Players[iPlayer].Name;
				if(namePilot == namePlayer) {
					::std::swap(app.Players[iPlayer], app.Players[iPilot]);
					break;
				}
			}
		}
	}
	return ::ghg::APP_STATE_Home; 
}

static	gpk::err_t	dialogCreateLoad	(::ghg::SGalaxyHellApp & app, ::gpk::SDialog & dialogLoad, ::gpk::view<::gpk::vcst_c> pathFileNames, const ::gpk::pobj<::gpk::SInput> & inputState, const ::gpk::n2f2_t & cursorPos) {
	::dialogCreateCommon(dialogLoad, inputState, cursorPos);
	return ::gpk::guiCreateControlList(*dialogLoad.GUI, pathFileNames, dialogLoad.Root, {256, 20}, {0, s1_t(-20 * pathFileNames.size() / 2)}, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_LEFT, app.DialogControls[::ghg::APP_STATE_Load]);
}

static	gpk::err_t	guiHandleHome		(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & /*gui*/, u2_t idControl, ::ghg::SGalaxyHell & game) { 
	switch((::ghg::UI_HOME)idControl) {
	case ::ghg::UI_HOME_Start: 
		::ghg::solarSystemReset(game);
		game.PlayState.Constants.Players = (u2_t)app.TunerPlayerCount->ValueCurrent;
		for(u2_t iPlayer = app.Players.size(); iPlayer < app.Game.PlayState.Constants.Players; ++iPlayer) {
			sc_t				text [64]			= {};
			sprintf_s(text, "Player %i", app.Players.size() + 1);
			app.AddNewPlayer(text);
		}
		for(u2_t iPlayer = 0; iPlayer < app.Players.size(); ++iPlayer) {
			if(iPlayer >= app.Game.Pilots.size())
				app.Game.PilotCreate({app.Players[iPlayer].Name, ::ghg::PLAYER_COLORS[iPlayer]});
			else
				app.Game.Pilots[iPlayer].Name = app.Players[iPlayer].Name;
		}
		game.PlayState.GlobalState.Paused	= false;
		return ::ghg::APP_STATE_Play;
	case ::ghg::UI_HOME_Continue: 
		game.PlayState.GlobalState.Paused	= false;
		return ::ghg::APP_STATE_Play;
	case ::ghg::UI_HOME_Save:
		if_fail_fe(app.Save(::ghg::SAVE_MODE_USER));
		break;
	case ::ghg::UI_HOME_Load: {
		::gpk::aobj<::gpk::vcst_t>	pathFileNames;
		::ghg::listFilesSavegame(app, app.SavegameFolder, pathFileNames);

		pathFileNames.push_back(::gpk::vcst_t{"Back"});
		::dialogCreateLoad(app, app.DialogPerState[::ghg::APP_STATE_Load], pathFileNames, app.DialogPerState[::ghg::APP_STATE_Home].Input, app.DialogPerState[::ghg::APP_STATE_Home].GUI->CursorPos);
		return ::ghg::APP_STATE_Load;
	}
	case ::ghg::UI_HOME_Settings: return ::ghg::APP_STATE_Settings; 
	case ::ghg::UI_HOME_Credits: return ::ghg::APP_STATE_About; 
	case ::ghg::UI_HOME_Shop: return ::ghg::APP_STATE_Shop; 
	case ::ghg::UI_HOME_Exit: 
		game.PlayState.GlobalState.Paused	= true;
		return ::ghg::APP_STATE_Quit;
	}
	return ::ghg::APP_STATE_Home; 
}

static	gpk::err_t	guiHandleUser		(::gpk::SGUI & /*gui*/, u2_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/) { return ::ghg::APP_STATE_Profile; }
static	gpk::err_t	guiHandleShop		(::gpk::SGUI & /*gui*/, u2_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (u2_t)::ghg::UI_SHOP_Back) {
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Shop; 
}
static	gpk::err_t	guiHandlePlay		(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & /*gui*/, u2_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (u2_t)::ghg::UI_PLAY_Menu) {
		app.Save(::ghg::SAVE_MODE_AUTO);
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Play; 
}

static	gpk::err_t	guiHandleWelcome	(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & /*gui*/, u2_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (u2_t)::ghg::UI_WELCOME_Confirm && app.InputboxText.size()) {
		app.AddNewPlayer(app.InputboxText);
		if(app.Game.Pilots.size())
			app.Game.Pilots[0] = {app.Players[0].Name, ::ghg::PLAYER_COLORS[0]};
		else
			app.Game.PilotCreate({app.Players[0].Name, ::ghg::PLAYER_COLORS[0]});

		app.Save(::ghg::SAVE_MODE_AUTO);
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Welcome; 
}
static	gpk::err_t	guiHandleSetup		(::gpk::SGUI & /*gui*/, u2_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (u2_t)::ghg::UI_SETTINGS_Audio) {
		return ::ghg::APP_STATE_Home;
	}
	else if(idControl == (u2_t)::ghg::UI_SETTINGS_Graphics) {
		return ::ghg::APP_STATE_Home;
	}
	else if(idControl == (u2_t)::ghg::UI_SETTINGS_Back) {
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Settings; 
}
static	gpk::err_t	guiHandleAbout		(::gpk::SGUI & /*gui*/, u2_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (u2_t)::ghg::UI_CREDITS_Back || idControl == (u2_t)::ghg::UI_CREDITS_Home) {
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_About; 
}

tplt<size_t _nStorageSize>
static	gpk::err_t	sprintfTime			(const sc_t *prefix, sc_t (&dest)[_nStorageSize], double seconds) {
	u2_t					timeHours			= (int)seconds / 3600;
	u2_t					timeMinutes			= (int)seconds / 60 % 60;
	u2_t					timeSeconds			= (int)seconds % 60;
	u2_t					timeCents			= int (seconds * 10) % 10;
	sprintf_s(dest, "%s%i:%.2i:%.2i.%.2i", prefix, timeHours, timeMinutes, timeSeconds, timeCents);
	return 0;
}

tplt<size_t _bufSize>
static	gpk::err_t	sprintfScore		(sc_t (&buffer)[_bufSize], const ::gpk::SSpaceshipScore & shipScore, const double nitro) { 
	return sprintf_s(buffer, "%c %llu  %c %llu  %c %llu  %c %llu  %c %.2f", 4, shipScore.Score, 94, shipScore.Shots, 1, (u3_t)shipScore.KilledShips, 2, (u3_t)shipScore.KilledOrbiters, 24, nitro);
}

static	gpk::err_t	uiPlayerUpdateHome	(::ghg::SUIPlayer & uiPlayer, u1_t iPlayer, const ::gpk::vcsc_t playerName, const ::gpk::bgra & shipColor, const ::gpk::SSpaceshipScore & shipScore, const double nitro) { 
	sprintfScore(uiPlayer.TextScore.Storage, shipScore, nitro);
	::gpk::SDialog				& playerDialog		= uiPlayer.DialogHome;
	::gpk::SGUI					& playerGUI			= *playerDialog.GUI;
	if_fail_fe(::gpk::controlTextSet(playerGUI, playerDialog.Root + 1 + ::ghg::UI_PROFILE_Score, uiPlayer.TextScore.Storage));
	if_fail_fe(::gpk::controlTextSet(playerGUI, playerDialog.Root + 1 + ::ghg::UI_PROFILE_Name, playerName));
	{
		::gpk::SControlDraw			& controlDraw		= playerGUI.Controls.Draw[playerDialog.Root + 1 + ::ghg::UI_PROFILE_Name];
		controlDraw.ColorTheme		= s1_t(3 + iPlayer);
		const ::gpk::SControlTheme	& theme				= (*playerGUI.Colors->ControlThemes)[(0 == controlDraw.ColorTheme) ? playerGUI.ThemeDefault : controlDraw.ColorTheme - 1];
		const ::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>	& colorCombo	= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL];
		(*playerGUI.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_TEXT_FACE	]] = shipColor;
		(*playerGUI.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BACKGROUND	]] = shipColor;
		playerGUI.Controls.Draw[uiPlayer.InputBox.IdText].ColorTheme = controlDraw.ColorTheme;
		playerGUI.Controls.Draw[uiPlayer.InputBox.IdText].NoClient = true;
		//playerGUI.Controls.Draw[uiPlayer.InputBox.IdText].NoBorder = true;

	}
	return 0;
}

static	gpk::err_t	uiPlayerUpdatePlay	(::ghg::SUIPlayer & uiPlayer, u2_t iPlayer, const ::ghg::SGalaxyHell & game, const ::gpk::SSpaceshipScore & shipScore, ::std::mutex & lockGame, ::ghg::SGalaxyHellDrawCache & drawCache) {
	::gpk::SDialog				& playerDialog		= uiPlayer.DialogPlay;
	::gpk::SGUI					& playerGUI			= *playerDialog.GUI;

	const ::gpk::SSpaceshipCore		& shipCore			= game.ShipState.SpaceshipManager.ShipCores[iPlayer];
	sprintfScore(uiPlayer.TextScore.Storage, shipScore, shipCore.Nitro.Value);
	const ::gpk::rgbaf			shipColor			= (shipCore.Team ? ::gpk::RED : ::gpk::rgbaf(game.Pilots[iPlayer].Color));
	if_fail_fe(::gpk::controlTextSet(playerGUI, 1 + ::ghg::UI_PILOT_Name	, game.Pilots[iPlayer].Name));
	if_fail_fe(::gpk::controlTextSet(playerGUI, 1 + ::ghg::UI_PILOT_Score, uiPlayer.TextScore.Storage));
	{
		::gpk::SControlDraw			& controlDraw		= playerGUI.Controls.Draw[1 + ::ghg::UI_PILOT_Name];
		controlDraw.ColorTheme	= s1_t(3 + iPlayer);
		const ::gpk::SControlTheme	& theme				= (*playerGUI.Colors->ControlThemes)[(0 == controlDraw.ColorTheme) ? playerGUI.ThemeDefault : controlDraw.ColorTheme - 1];
		const ::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>	& colorCombo			= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL];
		(*playerGUI.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_TEXT_FACE	]] = shipColor;
		(*playerGUI.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BACKGROUND	]] = shipColor;
	}
	for(u2_t iOrbiter = 0, countViewports = game.ShipState.SpaceshipManager.ShipParts[iPlayer].size(); iOrbiter < countViewports; ++iOrbiter) {
		::ghg::SUIPlayShipPartViewport	& viewport		= *uiPlayer.ModuleViewports[iOrbiter];

		const ::gpk::SSpaceshipOrbiter	& orbiter		= game.ShipState.SpaceshipManager.Orbiters[game.ShipState.SpaceshipManager.ShipParts[iPlayer][iOrbiter]];
		const ::gpk::SWeapon			& weapon		= game.ShipState.WeaponManager.Weapons[orbiter.Weapon];

		if_fail_fef(::gpk::controlTextSet(playerGUI, viewport.Viewport + 1, ::gpk::get_value_label(weapon.Shot.Type)), "%s", "");
		if_fail_fef(::gpk::controlTextSet(playerGUI, viewport.Viewport + 2, ::gpk::get_value_label(weapon.Type)), "%s", "");

		const float					healthRatio		= (float)::gpk::clamped(orbiter.Health.Weight(), 0.0, 1.0);
		const float					ratioOverheat	= (orbiter.Health.Value > 0) ? (float)weapon.Trigger.Overheat.WeightClamp() : 0;
		const float					ratioDelay		= (orbiter.Health.Value > 0) ? (float)weapon.Trigger.Delay  .WeightClamp() : 0;

		const ::gpk::rgbaf			colorLife		= ::gpk::interpolate_linear(::gpk::RED, ::gpk::GREEN, healthRatio);
		const ::gpk::rgbaf			colorDelay		= ::gpk::interpolate_linear(::gpk::GRAY * .5, ::gpk::YELLOW, ratioDelay);;
		const ::gpk::rgbaf			colorCooldown	= ::gpk::interpolate_linear(::gpk::LIGHTBLUE * ::gpk::CYAN, ::gpk::ORANGE * .5 + ::gpk::RED * .5, ratioOverheat);

		viewport.GaugeLife.SetValue(healthRatio	);

		{ // Update ship part render and health bar
	::gpk::m4f2_t				matrixView		= {};
			::gpk::SCameraPoints		& camera		= viewport.Camera;
			camera.Target			= game.ShipState.Scene.Transforms[game.ShipState.EntitySystem.Entities[game.ShipState.ShipPartEntity[iOrbiter] + 1].Transform].GetTranslation();
			camera.Position			= camera.Target;
			camera.Position.y		+= 7; //-= 20;
			stxp	::gpk::n3f2_t		cameraUp		= {1, 0, 0};
			matrixView.LookAt(camera.Position, camera.Target, cameraUp);
			matrixView				*= uiPlayer.ModuleViewports[iOrbiter]->MatrixProjection;

			::ghg::TRenderTarget		& renderTarget	= viewport.RenderTargetOrbiter;
			::gpk::g8bgra	targetPixels	= renderTarget.Color		; 
			::gpk::grid<u2_t>		depthBuffer		= renderTarget.DepthStencil	;
			//targetPixels.fill(tone);
			::std::lock_guard			lock			(lockGame);
			memset(targetPixels.begin(), 0, targetPixels.byte_count());
			memset(depthBuffer.begin(), -1, depthBuffer.byte_count());

			drawCache.PixelCoords.clear();
			u2_t					pixelsDrawn		= 0;
			pixelsDrawn				+= ::ghg::drawOrbiter(game.ShipState, game.ShipState.ShipPartEntity[iOrbiter], {}, (float)game.DecoState.AnimationTime, matrixView, targetPixels, depthBuffer, drawCache);

			if(healthRatio) {
				if_fail_fe(::ghg::gaugeImageUpdate(viewport.GaugeLife, targetPixels, ::gpk::RED, ::gpk::YELLOW, ::gpk::LIGHTGREEN));
			}
		}

		{ // Update weapon load bar
			::ghg::TRenderTarget		& renderTarget	= viewport.RenderTargetWeaponLoad;
			::gpk::g8bgra	targetPixels	= renderTarget.Color		; 
			::gpk::grid<u2_t>		depthBuffer		= renderTarget.DepthStencil	;
			::std::lock_guard			lock			(lockGame);
			memset(targetPixels.begin(), 0, targetPixels.byte_count());
			memset(depthBuffer.begin(), -1, depthBuffer.byte_count());

			if(ratioDelay) {
				drawCache.PixelCoords.clear();
				for(u2_t iLine = 0; iLine < targetPixels.metrics().y; ++iLine)
					::gpk::drawLine(targetPixels.metrics().u1(), ::gpk::line2<s1_t>{{0, (s1_t)iLine}, {s1_t((targetPixels.metrics().x - 1) * ratioDelay), (s1_t)iLine}}, drawCache.PixelCoords);
				
				for(u2_t iPixel = 0; iPixel < drawCache.PixelCoords.size(); ++iPixel) {
					const ::gpk::n2s1_t		& pixelCoord = drawCache.PixelCoords[iPixel];
					const ::gpk::n2f2_t		floatCoord			= pixelCoord.f2();
					const double			distanceFromCenter	= ::fabs((floatCoord.y / (targetPixels.metrics().y)) - .5) * 2.0;
					::gpk::setPixel(targetPixels, pixelCoord, ::gpk::interpolate_linear(colorDelay, ::gpk::GRAY * .25, distanceFromCenter));
				}
			}
		}

		{ // Update weapon type bar
			::std::lock_guard			lock			(lockGame);
			::ghg::TRenderTarget		& renderTarget	= viewport.RenderTargetWeaponType;
			::gpk::g8bgra	targetPixels	= renderTarget.Color		; 
			::gpk::grid<u2_t>		depthBuffer		= renderTarget.DepthStencil	;
			memset(targetPixels.begin(), 0, targetPixels.byte_count());
			memset(depthBuffer.begin(), -1, depthBuffer.byte_count());
			if(ratioOverheat) {
				drawCache.PixelCoords.clear();
				for(u2_t iLine = 0; iLine < targetPixels.metrics().y; ++iLine)
					::gpk::drawLine(targetPixels.metrics().u1(), ::gpk::line2<s1_t>{{0, (s1_t)iLine}, {s1_t((targetPixels.metrics().x - 1) * ratioOverheat), (s1_t)iLine}}, drawCache.PixelCoords);
				
				for(u2_t iPixel = 0; iPixel < drawCache.PixelCoords.size(); ++iPixel) {
					const ::gpk::n2s1_t			& pixelCoord		= drawCache.PixelCoords[iPixel];
					const ::gpk::n2f2_t			floatCoord			= pixelCoord.f2();
					const double				distanceFromCenter	= ::fabs((floatCoord.y / (targetPixels.metrics().y)) - .5) * 2.0;
					::gpk::setPixel(targetPixels, drawCache.PixelCoords[iPixel], ::gpk::interpolate_linear(colorCooldown, ::gpk::GRAY * .25, distanceFromCenter));
				}
			}
		}
	}
	return 0;
}

static	gpk::err_t	guiUpdatePlay		(::ghg::SGalaxyHellApp & app) { 
	::ghg::SGalaxyHell			& game				= app.Game;

	::gpk::SDialog				& dialog			= app.DialogPerState[::ghg::APP_STATE_Play];
	::gpk::SGUI					& gui				= *dialog.GUI;

	sprintf_s(app.UIPlay.TextLevel.Storage, "Level: %i", game.PlayState.GlobalState.Stage);
	sprintfTime("Total Time: "	, app.UIPlay.TextTimeReal	.Storage, game.PlayState.GlobalState.UserTime.Played);
	sprintfTime("Stage Time: "	, app.UIPlay.TextTimeStage	.Storage, game.PlayState.TimeRealStage);
	::gpk::controlTextSet(gui, 1 + ::ghg::UI_PLAY_Level		, ::gpk::vcst_t{app.UIPlay.TextLevel		.Storage});
	::gpk::controlTextSet(gui, 1 + ::ghg::UI_PLAY_TimeStage , ::gpk::vcst_t{app.UIPlay.TextTimeStage	.Storage});
	::gpk::controlTextSet(gui, 1 + ::ghg::UI_PLAY_TimeReal	, ::gpk::vcst_t{app.UIPlay.TextTimeReal	.Storage});

	if(0 == game.ShipState.SpaceshipManager.ShipCores.size())
		return 0;

	for(u2_t iPlayer = 0; iPlayer < app.TunerPlayerCount->ValueCurrent &&	iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) { app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI->Controls.SetHidden(0, false); }
	for(u2_t iPlayer = (u2_t)app.TunerPlayerCount->ValueCurrent;		iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) { app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI->Controls.SetHidden(0, true); }

	::ghg::SGalaxyHellDrawCache	& drawCache			= app.UIPlay.DrawCache;
	drawCache.LightPointsWorld.clear();
	drawCache.LightColorsWorld.clear();
	::ghg::getLightArraysFromShips(game.ShipState, drawCache.LightPointsWorld, drawCache.LightColorsWorld);
	drawCache.LightPointsModel.reserve(drawCache.LightPointsWorld.size());
	drawCache.LightColorsModel.reserve(drawCache.LightColorsWorld.size());
	if(app.ActiveState == ::ghg::APP_STATE_Play) {
		for(u2_t iPlayer = 0; iPlayer < game.PlayState.Constants.Players; ++iPlayer) {
			::ghg::SUIPlayer			& uiPlayer			= app.UIPlay.PlayerUI[iPlayer];
			if_fail_fef(::uiPlayerUpdatePlay(uiPlayer, iPlayer, app.Game, app.Game.ShipState.SpaceshipManager.ShipScores[iPlayer], app.Game.LockUpdate, drawCache), "iPlayer: %i", iPlayer);
			uiPlayer.DialogPlay.GUI->Controls.SetHidden(0, false);
		}
		for(u2_t iPlayer = app.Game.PlayState.Constants.Players; iPlayer < ghg::MAX_PLAYERS; ++iPlayer) { 
			app.UIPlay.PlayerUI[iPlayer].DialogPlay.GUI->Controls.SetHidden(0, true); 
		}
	}
	return 0;
}

static	gpk::err_t	guiUpdateHome				(::ghg::SGalaxyHellApp & app, ::gpk::vpobj<::gpk::SEventSystem> frameEvents) {
	for(u2_t iPlayer = 0; iPlayer < app.TunerPlayerCount->ValueCurrent; ++iPlayer) {
		::ghg::SUIPlayer			& uiPlayer					= app.UIPlay.PlayerUI[iPlayer];
		::gpk::SDialog				& dialog					= uiPlayer.DialogHome;
		::gpk::SGUI					& gui						= *dialog.GUI;
		gui.Controls.SetHidden(dialog.Root, false); 

		if(app.Players.size() <= iPlayer) {
			::std::lock_guard			lock						(app.Game.LockUpdate);
			sc_t text [64] = {};
			sprintf_s(text, "Player %i", app.Players.size() + 1);
			app.AddNewPlayer(text);
		}

		::gpk::rgbaf				shipColor					= ::ghg::PLAYER_COLORS[iPlayer];
		double						nitro						= 0;
		if((iPlayer < app.Game.ShipState.SpaceshipManager.ShipCores.size())) {
			const ::gpk::SSpaceshipCore		& shipCore				= app.Game.ShipState.SpaceshipManager.ShipCores[iPlayer];
			shipColor				= (shipCore.Team ? ::ghg::PLAYER_COLORS[iPlayer] : ::gpk::bgra(app.Game.Pilots[iPlayer].Color));
			nitro					= shipCore.Nitro.Value;
		}

		if_fail_fef(::uiPlayerUpdateHome(uiPlayer, s1_t(iPlayer), app.Players[iPlayer].Name, shipColor
			, (app.Game.ShipState.SpaceshipManager.ShipScores.size() <= iPlayer) ? ::gpk::SSpaceshipScore{} : app.Game.ShipState.SpaceshipManager.ShipScores[iPlayer]
			, nitro), "iPlayer: %i", iPlayer
		);

		::gpk::acid					controlsToProcess			= {};
		::gpk::guiGetProcessableControls(gui, controlsToProcess);
		if(s2_t result = uiPlayer.InputBox.Update(gui, uiPlayer.InputBox.VirtualKeyboard, frameEvents, controlsToProcess)) {
			if(result == INT_MAX) {
				::gpk::vcsc_t					trimmed						= {uiPlayer.InputBox.Text.begin(), ::gpk::min(uiPlayer.InputBox.Text.size(), 16U)};
				::gpk::trim(trimmed);
				if(trimmed.size()) {
					app.Players[iPlayer].Name	= ::gpk::label(trimmed);
					if(iPlayer < app.Game.Pilots.size())
						app.Game.Pilots[iPlayer].Name	= app.Players[iPlayer].Name;
				}
				uiPlayer.InputBox.Edit(gui, false);
			}
		}
		else {
			::gpk::guiProcessControls(gui, controlsToProcess, [&](u2_t iControl) {
				u2_t					offsetControl				= dialog.Root + 1;
				switch(iControl - (offsetControl)) {
				case ::ghg::UI_PROFILE_Name:
					uiPlayer.InputBox.SetText(gui, gui.Controls.Text[offsetControl + ::ghg::UI_PROFILE_Name].Text);
					uiPlayer.InputBox.Edit(gui, true);
				}
				return 0;
			});
		}
	}

	for(u2_t iPlayer = (u2_t)app.TunerPlayerCount->ValueCurrent; iPlayer < ghg::MAX_PLAYERS; ++iPlayer) {
		::ghg::SUIPlayer			& uiPlayer		= app.UIPlay.PlayerUI[iPlayer];
		::gpk::SDialog				& dialog		= uiPlayer.DialogHome;
		::gpk::SGUI					& gui			= *dialog.GUI;
		gui.Controls.SetHidden(dialog.Root, true);
	}

	return 0;
}

gpk::err_t			ghg::guiUpdate				(::ghg::SGalaxyHellApp & app, ::gpk::vpobj<::gpk::SEventSystem> sysEvents) {
	{
		::gpk::SDialog				& dialog					= app.DialogDesktop;
		::gpk::SGUI					& gui						= *dialog.GUI;
		::gpk::SInput				& input						= *dialog.Input;
		::gpk::guiProcessInput(gui, input, sysEvents); 
	}

	::ghg::SGalaxyHell			& game						= app.Game;

	::ghg::APP_STATE			appState					= app.ActiveState; 
	for(u2_t iAppState = 0; iAppState < ::ghg::APP_STATE_COUNT; ++iAppState) {
		::gpk::SDialog				& dialog					= app.DialogPerState[iAppState];
		::gpk::SGUI					& gui						= *dialog.GUI;
		::gpk::SInput				& input						= *dialog.Input;
		::gpk::guiProcessInput(gui, input, sysEvents); 
	}

	for(u2_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) {
		::ghg::SUIPlayer			& uiPlayer					= app.UIPlay.PlayerUI[iPlayer];
		::gpk::guiProcessInput(*uiPlayer.DialogPlay.GUI, *uiPlayer.DialogPlay.Input, sysEvents);
		::gpk::guiProcessInput(*uiPlayer.DialogHome.GUI, *uiPlayer.DialogHome.Input, sysEvents);
	}
	
	if(app.ActiveState == ::ghg::APP_STATE_Play)
		::guiUpdatePlay(app);
	else 
		::guiUpdateHome(app, sysEvents);

	::gpk::SDialog				& dialog					= app.DialogPerState[appState];
	::gpk::SGUI					& gui						= *dialog.GUI;
	dialog.Update();
	::gpk::acid					controlsToProcess			= {};
	::gpk::guiProcessControls(gui, [&app, &gui, &game, &appState](::gpk::cid_t idControl) {
			idControl = idControl - 1;
			info_printf("Executed %u.", idControl);
			switch(appState) {
			case ::ghg::APP_STATE_Play		: return appState = (::ghg::APP_STATE)::guiHandlePlay		(app, gui, idControl, game);
			case ::ghg::APP_STATE_Home		: return appState = (::ghg::APP_STATE)::guiHandleHome		(app, gui, idControl, game);
			case ::ghg::APP_STATE_Welcome	: return appState = (::ghg::APP_STATE)::guiHandleWelcome	(app, gui, idControl, game);
			case ::ghg::APP_STATE_Load		: return appState = (::ghg::APP_STATE)::guiHandleLoad		(app, gui, idControl, game);
			case ::ghg::APP_STATE_Profile	: return appState = (::ghg::APP_STATE)::guiHandleUser		(gui, idControl, game);
			case ::ghg::APP_STATE_Shop		: return appState = (::ghg::APP_STATE)::guiHandleShop		(gui, idControl, game);
			case ::ghg::APP_STATE_Settings	: return appState = (::ghg::APP_STATE)::guiHandleSetup		(gui, idControl, game);
			case ::ghg::APP_STATE_About		: return appState = (::ghg::APP_STATE)::guiHandleAbout		(gui, idControl, game);
			}
			return appState;
		});

	return appState;
}

gpk::err_t			ghg::gaugeBuildRadial			(::ghg::SUIRadialGauge & gauge, const ::gpk::circlef2_t & gaugeMetrics, s1_t resolution, s1_t width) {
	const double				stepUnit						= (1.0 / resolution) * ::gpk::math_2pi;
	const ::gpk::spheref2_t		sphereMetrics					= {gaugeMetrics.Radius, {gaugeMetrics.Center.x, gaugeMetrics.Center.y, .5f}};
	const double				finalRadius						= gaugeMetrics.Radius;	//::gpk::interpolate_linear(gaugeMetrics.Radius, gaugeMetrics.Radius * .5, ::gpk::clamped(abs(sinCos.Cos), 0.0, 1.0)); //
	const double				outerRadius						= finalRadius;			//::gpk::interpolate_linear(gaugeMetrics.Radius, gaugeMetrics.Radius * .5, ::gpk::clamped(abs(sinCos.Cos), 0.0, 1.0)); //
	const double				innerRadius						= outerRadius - width;	//::gpk::interpolate_linear(gaugeMetrics.Radius, gaugeMetrics.Radius * .5, ::gpk::clamped(abs(sinCos.Cos), 0.0, 1.0)); //
	for(s1_t iStep = 0, stepCount = resolution; iStep < stepCount; ++iStep) {
		::gpk::SSinCos				sinCos							= {::sin(iStep * stepUnit), -::cos(iStep * stepUnit)};
		const ::gpk::n3f3_t			relativePosSmall				=
			{ sinCos.Sin * innerRadius
			, sinCos.Cos * innerRadius
			};
		const ::gpk::n3f3_t			relativePos						=
			{ sinCos.Sin * outerRadius
	  		, sinCos.Cos * outerRadius
			};
		gauge.Vertices.push_back(sphereMetrics.Center + relativePosSmall.f2());
		gauge.Vertices.push_back(sphereMetrics.Center + relativePos.f2());
	}
	for(s1_t iStep = 0, stepCount = (s1_t)(resolution * 2) - 3; iStep < stepCount; ++iStep) {
		gauge.Indices.push_back({s1_t(0 + iStep), s1_t(1 + iStep), s1_t(2 + iStep)});
		gauge.Indices.push_back({s1_t(1 + iStep), s1_t(3 + iStep), s1_t(2 + iStep)});
	}
	const s1_t				iStep							= s1_t(gauge.Vertices.size() - 2);
	gauge.Indices.push_back({s1_t(0 + iStep), s1_t(1 + iStep), 0});
	gauge.Indices.push_back({s1_t(1 + iStep), 1, 0});
	gauge.MaxValue			= resolution;
	return 0;
}


gpk::err_t			ghg::gaugeImageUpdate			(::ghg::SUIRadialGauge & gauge, ::gpk::g8bgra target, ::gpk::rgbaf colorMin, ::gpk::rgbaf colorMid, ::gpk::rgbaf colorMax, ::gpk::bgra colorEmpty, bool radialColor)  {
	static ::gpk::imgu2_t		dummyDepth;
	const ::gpk::n3f2_t			center3							= (gauge.Vertices[1] - gauge.Vertices[gauge.Vertices.size() / 2 + 1]) / 2 + gauge.Vertices[gauge.Vertices.size() / 2 + 1];
	const ::gpk::n2f2_t			center2							= {center3.x, center3.y};
	const double				radiusLarge						= (center3 - gauge.Vertices[1]).Length();
	const double				radiusSmall						= (center3 - gauge.Vertices[0]).Length();
	const double				radiusHalfWidth					= (radiusLarge - radiusSmall) / 2;
	const double				radiusCenter					= radiusHalfWidth + radiusSmall;
	dummyDepth.resize(target.metrics(), 0xFFFFFFFFU);
	::gpk::apod<::gpk::n2s1_t>		pixelCoords;
	::gpk::apod<::gpk::trif2_t>		triangleWeights;
	for(u2_t iTriangle = 0, triangleCount = gauge.Indices.size(); iTriangle < triangleCount; ++iTriangle) {
		const ::gpk::tri3f2_t			triangleCoords				=
			{ gauge.Vertices[gauge.Indices[iTriangle].A]
			, gauge.Vertices[gauge.Indices[iTriangle].B]
			, gauge.Vertices[gauge.Indices[iTriangle].C]
			};
		const double					colorFactor					= radialColor ? ::gpk::min(1.0, iTriangle / (double)triangleCount) : gauge.CurrentValue / (float)gauge.MaxValue;
		::gpk::rgbaf					finalColor;
		const bool						isEmptyGauge				= ((iTriangle + 2) >> 1) >= (u2_t)gauge.CurrentValue * 2;
		finalColor					= isEmptyGauge
			? ::gpk::rgbaf(colorEmpty)
			: (colorFactor < .5)
				? ::gpk::interpolate_linear(colorMin, colorMid, (::gpk::min(1.0, iTriangle * 2.25 / (double)triangleCount)))
				: ::gpk::interpolate_linear(colorMid, colorMax, (colorFactor - .5) * 2)
				;
		finalColor.Clamp();
		pixelCoords.clear();
		::gpk::drawTriangle(target.metrics(), triangleCoords, pixelCoords, triangleWeights, dummyDepth);
		for(u2_t iPixelCoords = 0; iPixelCoords < pixelCoords.size(); ++iPixelCoords) {
			const ::gpk::n2s1_t		pixelCoord					= pixelCoords[iPixelCoords];
//#define GAUGE_NO_SHADING
#ifndef GAUGE_NO_SHADING
			const ::gpk::n2f2_t				floatCoord					= pixelCoord.f2();
			const double					distanceFromCenter			= (floatCoord - center2).Length();
			const double					distanceFromRadiusCenter	= ::fabs(distanceFromCenter - radiusCenter) / radiusHalfWidth;
			finalColor.a				= (float)(1.f - distanceFromRadiusCenter);
#else
			(void)radiusCenter;
#endif		
			::gpk::bgra						& targetPixel				= target[pixelCoord.y][pixelCoord.x];
			targetPixel					= ::gpk::interpolate_linear(::gpk::rgbaf{targetPixel}, finalColor, finalColor.a);
			targetPixel.a				= u0_t(finalColor.a * 255);
		}
	}
	return 0;
}


