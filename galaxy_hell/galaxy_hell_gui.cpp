#include "gpk_galaxy_hell_app.h"
#include "gpk_storage.h"
#include "gpk_raster_lh.h"
#include <gpk_grid_copy.h>

static constexpr	::gpk::GUI_COLOR_MODE		GHG_MENU_COLOR_MODE		= ::gpk::GUI_COLOR_MODE_3D;
static constexpr	bool						BACKGROUND_3D_ONLY		= true;
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_CAMERA_SIZE		= {64, 48};
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_VIEWPORT_SIZE	= {160, 48};

static	::gpk::error_t			guiSetupCommon				(::gpk::SGUI & gui) {
	gui.ColorModeDefault			= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault				= ::gpk::ASCII_COLOR_DARKRED * 16 + 10;
	gui.SelectedFont				= 7;
	gui.Controls.Modes[0].NoBackgroundRect	= true;
	return 0;
}

static	::gpk::error_t			guiSetupButtonList			(::gpk::SGUI & gui, uint16_t buttonWidth, int16_t yOffset, ::gpk::ALIGN controlAlign, ::gpk::view_array<::gpk::vcc> buttonText) {
	for(uint16_t iButton = 0; iButton < buttonText.size(); ++iButton) {
		int32_t								idControl						= ::gpk::controlCreate(gui);
		::gpk::SControl						& control						= gui.Controls.Controls[idControl];
		control.Area					= {{0U, (int16_t)(20*iButton + yOffset)}, {(int16_t)buttonWidth, 20}};
		control.Border					= {1, 1, 1, 1};//{10, 10, 10, 10};
		control.Margin					= {1, 1, 1, 1};
		control.Align					= controlAlign;
		::gpk::SControlText					& controlText					= gui.Controls.Text[idControl];
		controlText.Text				= buttonText[iButton];
		controlText.Align				= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, idControl, -1);
	}
	return 0;
}

template<typename _tUIEnum>
static	::gpk::error_t			guiSetupButtonList			(::gpk::SGUI & gui, uint16_t buttonWidth, int16_t yOffset, ::gpk::ALIGN controlAlign) {
	for(uint16_t iButton = 0; iButton < ::gpk::get_value_count<_tUIEnum>(); ++iButton) {
		int32_t								idControl						= ::gpk::controlCreate(gui);
		::gpk::SControl						& control						= gui.Controls.Controls[idControl];
		control.Area					= {{0U,(int16_t)(20*iButton + yOffset)}, {(int16_t)buttonWidth, 20}};
		control.Border					= {1, 1, 1, 1};//{10, 10, 10, 10};
		control.Margin					= {1, 1, 1, 1};
		control.Align					= controlAlign;
		::gpk::SControlText					& controlText					= gui.Controls.Text[idControl];
		controlText.Text				= ::gpk::get_value_label((_tUIEnum)iButton);
		controlText.Align				= ::gpk::ALIGN_CENTER;
		::gpk::controlSetParent(gui, idControl, 0);
	}
	return 0;
}

static	::gpk::error_t			guiSetupInit				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_LOAD>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupLoad				(::gpk::SDialog & dialog) { 
	gpk_necall(guiSetupButtonList<::ghg::UI_LOAD>(*dialog.GUI,  60, 0, ::gpk::ALIGN_CENTER), "%s", ""); 
	return 0;
}

static	::gpk::error_t			guiSetupHome				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_HOME>(*dialog.GUI, 160, int16_t(-20 * ::gpk::get_value_count<::ghg::UI_HOME>() / 2), ::gpk::ALIGN_CENTER); }
static	::gpk::error_t			guiSetupUser				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
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
	guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); 
	return 0;
}
static	::gpk::error_t			guiSetupBrief				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupStage				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupStats				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupStore				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupScore				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
//static	::gpk::error_t			guiSetupSetup				(::gpk::SGUI & gui) { return guiSetupButtonList<::ghg::UI_PLAY>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); }
static	::gpk::error_t			guiSetupSetup				(::gpk::SDialog & dialog) { 
	dialog.GUI->SelectedFont		= 7;
	::gpk::SGUI							& gui						= *dialog.GUI;
	guiSetupButtonList<::ghg::UI_SETTINGS>(gui,  160, 0, ::gpk::ALIGN_CENTER); 
	return 0;
}

static	::gpk::error_t			guiSetupAbout				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_CREDITS>(*dialog.GUI,  160, int16_t(-20 * ::gpk::get_value_count<::ghg::UI_CREDITS>() / 2), ::gpk::ALIGN_CENTER); }

::gpk::error_t					ghg::guiSetup				(::ghg::SGalaxyHellApp & app, const ::gpk::ptr_obj<::gpk::SInput> & input) {
	for(uint32_t iGUI = 0; iGUI < ::gpk::size(app.DialogPerState); ++iGUI) {
		app.DialogPerState[iGUI].Input				= input;
		guiSetupCommon(*app.DialogPerState[iGUI].GUI);
		app.DialogPerState[iGUI].Update();
	};
	::guiSetupInit	(app.DialogPerState[::ghg::APP_STATE_Init	]);
	::guiSetupLoad	(app.DialogPerState[::ghg::APP_STATE_Load	]);
	::guiSetupHome	(app.DialogPerState[::ghg::APP_STATE_Home	]);
	::guiSetupUser	(app.DialogPerState[::ghg::APP_STATE_User	]);
	::guiSetupShop	(app.DialogPerState[::ghg::APP_STATE_Shop	]);
	::guiSetupPlay	(app, app.DialogPerState[::ghg::APP_STATE_Play	]);
	::guiSetupBrief	(app.DialogPerState[::ghg::APP_STATE_Brief	]);
	::guiSetupStage	(app.DialogPerState[::ghg::APP_STATE_Stage	]);
	::guiSetupStats	(app.DialogPerState[::ghg::APP_STATE_Stats	]);
	::guiSetupStore	(app.DialogPerState[::ghg::APP_STATE_Store	]);
	::guiSetupScore	(app.DialogPerState[::ghg::APP_STATE_Score	]);
	::guiSetupSetup	(app.DialogPerState[::ghg::APP_STATE_Setup	]);
	::guiSetupAbout	(app.DialogPerState[::ghg::APP_STATE_About	]);
	return 0;
}

static	::gpk::error_t			guiHandleLoad				(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & gui, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl < (gui.Controls.Text.size() - 2)) {
		gerror_if(0 > app.Load(gui.Controls.Text[idControl + 1].Text), "%s", gui.Controls.Text[idControl + 1].Text.begin());
	}
	return ::ghg::APP_STATE_Home; 
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
		::gpk::array_obj<::gpk::vcc>				pathFileNames;
		app.FileNames							= {};
		::gpk::pathList(app.SavegameFolder, app.FileNames);
		for(uint32_t iFile = 0; iFile < app.FileNames.size(); ++iFile)  {
			const ::gpk::vcc							fileName			= app.FileNames[iFile];
			if(fileName.size() > app.ExtensionSave.size() && 0 == strcmp(&fileName[fileName.size() - app.ExtensionSave.size()], app.ExtensionSave.begin())) 
				pathFileNames.push_back(fileName);
		}
		pathFileNames.push_back(::gpk::vcs{"Back"});
		app.DialogPerState[::ghg::APP_STATE_Load]					= {};
		app.DialogPerState[::ghg::APP_STATE_Load].Input				= app.DialogPerState[::ghg::APP_STATE_Home].Input;
		app.DialogPerState[::ghg::APP_STATE_Load].GUI->CursorPos	= app.DialogPerState[::ghg::APP_STATE_Home].GUI->CursorPos;
		guiSetupCommon(*app.DialogPerState[::ghg::APP_STATE_Load].GUI);
		app.DialogPerState[::ghg::APP_STATE_Load].Update();
		::guiSetupButtonList(*app.DialogPerState[::ghg::APP_STATE_Load].GUI, 256, 0, ::gpk::ALIGN_CENTER, pathFileNames);
		return ::ghg::APP_STATE_Load;
	}
	case ::ghg::UI_HOME_Settings: {
		return ::ghg::APP_STATE_Setup;
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

static	::gpk::error_t			guiHandleUser				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/) { return ::ghg::APP_STATE_User; }
static	::gpk::error_t			guiHandleShop				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (uint32_t)::ghg::UI_SHOP_Back) {
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Shop; 
}
static	::gpk::error_t			guiHandlePlay				(::ghg::SGalaxyHellApp & app, ::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (uint32_t)::ghg::UI_PLAY_Pause) {
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
	return ::ghg::APP_STATE_Setup; 
}
static	::gpk::error_t			guiHandleAbout				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/) { 
	if(idControl == (uint32_t)::ghg::UI_CREDITS_Back || idControl == (uint32_t)::ghg::UI_CREDITS_Home) {
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_About; 
}


static	::gpk::error_t			guiUpdatePlay				(::ghg::SGalaxyHellApp & app) { 
	::ghg::SGalaxyHell					& game						= app.World;
	if(0 == game.ShipState.ShipCores.size())
		return 0;

	::gpk::array_pod<uint32_t>			& shipParts					= game.ShipState.ShipCoresParts[0];
	const uint32_t						moduleCount					= shipParts.size();
	::gpk::SDialog						& dialog					= app.DialogPerState[::ghg::APP_STATE_Play];
	::gpk::SGUI							& gui						= *dialog.GUI;
	::std::lock_guard<::std::mutex>							lockUpdate			(game.LockUpdate);

	// Setup new viewports if necessary. This may happen if the ship acquires new modules while playing the stage
	for(uint32_t iViewport = app.UIPlay.ModuleViewports.size(); iViewport < moduleCount; ++iViewport) {
		::gpk::ptr_nco<::gpk::SDialogViewport>	viewport				= {};
		app.UIPlay.ModuleViewports[app.UIPlay.ModuleViewports.push_back({})]->Viewport = ::gpk::viewportCreate(dialog, viewport);
		::ghg::SUIPlayModuleViewport			& vp					= *app.UIPlay.ModuleViewports[iViewport];
		vp.RenderTarget.resize(::MODULE_CAMERA_SIZE);

		::gpk::SCoord2<int16_t>					& viewportSize			= gui.Controls.Controls[viewport->IdGUIControl].Area.Size;
		::gpk::viewportAdjustSize(viewportSize, MODULE_VIEWPORT_SIZE.Cast<int16_t>());
		viewport->Settings.DisplacementLockX	= true;
		viewport->Settings.DisplacementLockY	= true;
		gui.Controls.Modes		[viewport->IdGUIControl	].NoBackgroundRect		= true;
		gui.Controls.Controls	[viewport->IdGUIControl	].Align					= ::gpk::ALIGN_CENTER_BOTTOM;
		gui.Controls.Controls	[viewport->IdGUIControl	].Area.Offset			= {(int16_t)((-(int16_t)(moduleCount * viewportSize.x)>> 1) + viewportSize.x * iViewport), 0};

		gui.Controls.Controls	[viewport->IdClient].Image						= vp.RenderTarget.Color.View;
		gui.Controls.Controls	[viewport->IdClient].ImageAlign					= ::gpk::ALIGN_RIGHT;
		//gui.Controls.States		[viewport->IdClient		].ImageInvertY			= true;

		::gpk::SMatrix4<float>						& matrixProjection				= vp.MatrixProjection;
		matrixProjection.FieldOfView(::gpk::math_pi * .25, MODULE_CAMERA_SIZE.x / (double)MODULE_CAMERA_SIZE.y, 0.01, 500);
		::gpk::SMatrix4<float>						matrixViewport					= {};
		matrixViewport.ViewportLH(MODULE_CAMERA_SIZE.Cast<uint32_t>());
		matrixProjection						*= matrixViewport;

		const ::gpk::SCircle<float> circleLife		= {::gpk::min(MODULE_CAMERA_SIZE.x, MODULE_CAMERA_SIZE.y) * .5f, vp.RenderTarget.Color.View.metrics().Cast<float>() * .5};
		const ::gpk::SCircle<float> circleDelay		= {::gpk::min(MODULE_CAMERA_SIZE.x, MODULE_CAMERA_SIZE.y) * .5f - 6 * 1, vp.RenderTarget.Color.View.metrics().Cast<float>() * .5};
		const ::gpk::SCircle<float> circleCooldown	= {::gpk::min(MODULE_CAMERA_SIZE.x, MODULE_CAMERA_SIZE.y) * .5f - 6 * 2, vp.RenderTarget.Color.View.metrics().Cast<float>() * .5};
		
		::ghg::gaugeBuildRadial(vp.GaugeLife	, circleLife		, 64, 6);
		::ghg::gaugeBuildRadial(vp.GaugeDelay	, circleDelay		, 64, 6);
		::ghg::gaugeBuildRadial(vp.GaugeCooldown, circleCooldown	, 64, 6);

		char number[256] = {};
		sprintf_s(number, "#%i", iViewport + 1);
		dialog.Update();
	}
	for(uint32_t iViewport = 0; iViewport < app.UIPlay.ModuleViewports.size(); ++iViewport) {
		const ::ghg::SShipPart									& shipPart			= game.ShipState.ShipParts[shipParts[iViewport]];
		const ::ghg::SWeapon									& weapon			= game.ShipState.Weapons[shipPart.Weapon];

		::gpk::ptr_nco<::gpk::SDialogViewport>	viewport				= {};
		dialog.Get(app.UIPlay.ModuleViewports[iViewport]->Viewport, viewport);
		char temp[1024] = {};
		sprintf_s(temp, "%s - %s", ::gpk::get_value_label(game.ShipState.Weapons[shipPart.Weapon].Type).begin(), ::gpk::get_value_label(game.ShipState.Weapons[shipPart.Weapon].Load).begin());
		::gpk::controlTextSet(gui, viewport->IdTitle, ::gpk::label(temp));
		::ghg::TRenderTarget									& renderTarget			= app.UIPlay.ModuleViewports[iViewport]->RenderTarget;
		::gpk::view_grid<::gpk::SColorBGRA>						targetPixels			= renderTarget.Color		; 
		::gpk::view_grid<uint32_t>								depthBuffer				= renderTarget.DepthStencil	;
		//------------------------------------------- Transform and Draw
		if(0 == targetPixels.size())
			return 1;

		if(false == viewport->Settings.Unfolded) {
			if(0 < shipPart.Health) 
				::gpk::viewportFold(*viewport, false);
		}
		else {
			if(0 >= shipPart.Health) 
				::gpk::viewportFold(*viewport, true);
			else {

				::ghg::SUIPlayModuleViewport							& vp					= *app.UIPlay.ModuleViewports[iViewport];

				const float												healthRatio				= shipPart.Health	/ float(shipPart.MaxHealth);
				const float												ratioOverheat			= weapon.Overheat	/ float(weapon.Cooldown);
				const float												ratioDelay				= weapon.Delay		/ float(weapon.MaxDelay);
				const float												toneWeight				= fabsf(sinf((float)game.PlayState.TimeStage));
				::gpk::SColorFloat										tone					= {::gpk::SColorFloat{::gpk::DARKRED * .5f} * (1.0f - healthRatio) * toneWeight};
				targetPixels.fill(tone);
				memset(depthBuffer.begin(), -1, depthBuffer.byte_count());

				::ghg::SGalaxyHellDrawCache								& drawCache				= app.UIPlay.DrawCache;
				drawCache.PixelCoords.clear();
				::gpk::SMatrix4<float>									matrixView				= {};
				::gpk::SCamera											& camera				= vp.Camera;
				camera.Target										= game.ShipState.Scene.Transforms[game.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Transform].GetTranslation();
				camera.Position										= camera.Target;
				camera.Position.y									+= 6; //-= 20;
				camera.Up											= {1, 0, 0};
				matrixView.LookAt(camera.Position, camera.Target, camera.Up);
				matrixView											*= app.UIPlay.ModuleViewports[iViewport]->MatrixProjection;
				drawCache.LightPointsWorld.clear();
				drawCache.LightColorsWorld.clear();
				::ghg::getLightArrays(game.ShipState, game.DecoState, drawCache.LightPointsWorld, drawCache.LightColorsWorld, ::ghg::DEBRIS_COLORS);
				drawCache.LightPointsModel.reserve(drawCache.LightPointsWorld.size());
				drawCache.LightColorsModel.reserve(drawCache.LightColorsWorld.size());

				uint32_t												pixelsDrawn				= 0;
				pixelsDrawn											+= ::ghg::drawShipPart(game.ShipState, shipPart, matrixView, targetPixels, depthBuffer, drawCache);

				vp.GaugeLife	.SetValue(healthRatio	);
				vp.GaugeDelay	.SetValue(ratioDelay	);
				vp.GaugeCooldown.SetValue(ratioOverheat	);

				const ::gpk::SColorFloat colorLife		= ::gpk::interpolate_linear(::gpk::DARKRED, ::gpk::GREEN, healthRatio);
				const ::gpk::SColorFloat colorDelay		= ::gpk::YELLOW;
				const ::gpk::SColorFloat colorCooldown	= ::gpk::interpolate_linear(::gpk::BLUE, ::gpk::ORANGE * .5 + ::gpk::RED * .5, ratioOverheat);

				::ghg::gaugeImageUpdate(vp.GaugeLife	, targetPixels, colorLife		, colorLife		, colorLife		);
				::ghg::gaugeImageUpdate(vp.GaugeDelay	, targetPixels, colorDelay		, colorDelay	, colorDelay		);
				::ghg::gaugeImageUpdate(vp.GaugeCooldown, targetPixels, colorCooldown	, colorCooldown	, colorCooldown	);
				
				drawCache.PixelCoords.clear();
				for(uint32_t iLine = 0; iLine < 3; ++iLine)
					::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{{0, (int16_t)(targetPixels.metrics().y - 1 * iLine)}, {int16_t((targetPixels.metrics().x - 1) * healthRatio), (int16_t)(targetPixels.metrics().y - 1 * iLine)}}, drawCache.PixelCoords);
				
				for(uint32_t iPixel = 0; iPixel < drawCache.PixelCoords.size(); ++iPixel) {
					::gpk::setPixel(targetPixels, drawCache.PixelCoords[iPixel], colorLife);
				}
				
				drawCache.PixelCoords.clear();
				for(uint32_t iLine = 3; iLine < 6; ++iLine)
					::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{{0, (int16_t)(targetPixels.metrics().y - 1 * iLine)}, {int16_t((targetPixels.metrics().x - 1) * ratioOverheat), (int16_t)(targetPixels.metrics().y - 1 * iLine)}}, drawCache.PixelCoords);
				
				for(uint32_t iPixel = 0; iPixel < drawCache.PixelCoords.size(); ++iPixel) {
					::gpk::setPixel(targetPixels, drawCache.PixelCoords[iPixel], colorCooldown);
				}
				
				drawCache.PixelCoords.clear();
				for(uint32_t iLine = 6; iLine < 9; ++iLine)
					::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{{0, (int16_t)(targetPixels.metrics().y - 1 * iLine)}, {int16_t((targetPixels.metrics().x - 1) * ratioDelay), (int16_t)(targetPixels.metrics().y - 1 * iLine)}}, drawCache.PixelCoords);
				
				for(uint32_t iPixel = 0; iPixel < drawCache.PixelCoords.size(); ++iPixel) {
					::gpk::setPixel(targetPixels, drawCache.PixelCoords[iPixel], colorDelay);
				}
			}
		}
	}
	return 0;
}

::gpk::error_t					ghg::guiUpdate				(::ghg::SGalaxyHellApp & gameApp, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents) {
	::ghg::SGalaxyHell					& game						= gameApp.World;
	::ghg::APP_STATE					appState					= gameApp.ActiveState; 
	for(uint32_t iAppState = 0; iAppState < ::ghg::APP_STATE_COUNT; ++iAppState) {
		::gpk::SDialog						& dialog					= gameApp.DialogPerState[iAppState];
		::gpk::SGUI							& gui						= *dialog.GUI;
		::gpk::SInput						& input						= *dialog.Input;
		::gpk::guiProcessInput(gui, input, sysEvents); 
	}
	::gpk::SDialog						& dialog					= gameApp.DialogPerState[appState];
	dialog.Update();
	::gpk::SGUI							& gui						= *dialog.GUI;
	::gpk::array_pod<uint32_t>			controlsToProcess			= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	::guiUpdatePlay(gameApp);

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
			case ::ghg::APP_STATE_User	: appState = (::ghg::APP_STATE)::guiHandleUser	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Shop	: appState = (::ghg::APP_STATE)::guiHandleShop	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Play	: appState = (::ghg::APP_STATE)::guiHandlePlay	(gameApp, gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Brief	: appState = (::ghg::APP_STATE)::guiHandleBrief	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Stage	: appState = (::ghg::APP_STATE)::guiHandleStage	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Stats	: appState = (::ghg::APP_STATE)::guiHandleStats	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Store	: appState = (::ghg::APP_STATE)::guiHandleStore	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Score	: appState = (::ghg::APP_STATE)::guiHandleScore	(gui, idControl, game); handled = true; break;
			case ::ghg::APP_STATE_Setup	: appState = (::ghg::APP_STATE)::guiHandleSetup	(gui, idControl, game); handled = true; break;
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
#endif		
			::gpk::SColorBGRA					& targetPixel					= target[pixelCoord.y][pixelCoord.x];
			finalColor.Clamp();
			targetPixel						= ::gpk::interpolate_linear(::gpk::SColorFloat{targetPixel}, finalColor, finalColor.a);
		}
	}
	return 0;
}
