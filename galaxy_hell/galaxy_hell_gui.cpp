#include "gpk_galaxy_hell_app.h"
#include "gpk_storage.h"
#include "gpk_raster_lh.h"

static constexpr	::gpk::GUI_COLOR_MODE		GHG_MENU_COLOR_MODE		= ::gpk::GUI_COLOR_MODE_3D;
static constexpr	bool						BACKGROUND_3D_ONLY		= true;
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_CAMERA_SIZE		= {80, 64};
static constexpr	::gpk::SCoord2<uint16_t>	MODULE_VIEWPORT_SIZE	= {160, 64};

static	::gpk::error_t			guiSetupCommon				(::gpk::SGUI & gui) {
	gui.ColorModeDefault			= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault				= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 8;
	gui.SelectedFont				= 7;
	gui.Controls.Modes[0].NoBackgroundRect	= true;
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

static	::gpk::error_t			guiSetupLoad				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupHome				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_HOME>(*dialog.GUI, 160, int16_t(-20 * ::gpk::get_value_count<::ghg::UI_HOME>() / 2), ::gpk::ALIGN_CENTER); }
static	::gpk::error_t			guiSetupUser				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT); }
static	::gpk::error_t			guiSetupShop				(::gpk::SDialog & dialog) { 
	::gpk::SGUI							& gui						= *dialog.GUI;
	guiSetupButtonList<::ghg::UI_SHOP>(gui,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT);
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

static	::gpk::error_t			guiSetupAbout				(::gpk::SDialog & dialog) { return guiSetupButtonList<::ghg::UI_PLAY>(*dialog.GUI,  60, 0, ::gpk::ALIGN_BOTTOM_RIGHT	); }

::gpk::error_t					ghg::guiSetup				(::ghg::SGalaxyHellApp & app, const ::gpk::ptr_obj<::gpk::SInput> & input) {
	for(uint32_t iGUI = 0; iGUI < ::gpk::size(app.DialogPerState); ++iGUI) {
		app.DialogPerState[iGUI].Input				= input;
		app.DialogPerState[iGUI].GUI->SelectedFont	= 7;
		guiSetupCommon(*app.DialogPerState[iGUI].GUI);
		app.DialogPerState[iGUI].Update();
	};
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

static	::gpk::error_t			guiHandleLoad				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Load; }
static	::gpk::error_t			guiHandleHome				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & game, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { 
	switch((::ghg::UI_HOME)idControl) {
	case ::ghg::UI_HOME_Start: 
		::ghg::solarSystemReset(game);
		game.PlayState.Paused						= false;
		return ::ghg::APP_STATE_Play;
	case ::ghg::UI_HOME_Continue: 
		game.PlayState.Paused						= false;
		return ::ghg::APP_STATE_Play;
	case ::ghg::UI_HOME_Save: {
		::gpk::array_pod<byte_t>			serialized;
		game.Save(serialized);

		char fileName[4096] = {};
		sprintf_s(fileName, "./%llu.ghs", ::gpk::timeCurrentInMs());
		::gpk::fileFromMemory(fileName, serialized);
		game.PlayState.Paused			= false;
		return ::ghg::APP_STATE_Play;
	}
	case ::ghg::UI_HOME_Load: {
		::gpk::array_pod<byte_t>			serialized;
		//char								fileName[4096]		= {};
		::gpk::array_obj<::gpk::array_pod<char>>	pathFiles;
		::gpk::pathList(::gpk::vcs{"./"}, pathFiles);
		for(uint32_t iFile = 0; iFile < pathFiles.size(); ++iFile)  {
			if(0 == strcmp(&pathFiles[iFile][pathFiles[iFile].size() - 4], ".ghs")) {
				::gpk::fileToMemory(pathFiles[iFile], serialized);
				::gpk::view_array<const byte_t>		viewSerialized		= {(const byte_t*)serialized.begin(), serialized.size()};
				game.Load(viewSerialized);
				break;
			}
		}

		return ::ghg::APP_STATE_Home;
	}
	case ::ghg::UI_HOME_Settings: {
		return ::ghg::APP_STATE_Setup;
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

static	::gpk::error_t			guiHandleUser				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_User; }
static	::gpk::error_t			guiHandleShop				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { 
	if(idControl == (uint32_t)::ghg::UI_SHOP_Back) {
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Shop; 
}
static	::gpk::error_t			guiHandlePlay				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & game, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { 
	if(idControl == (uint32_t)::ghg::UI_PLAY_Menu) {
		::gpk::array_pod<byte_t>			serialized;
		game.Save(serialized);
		char								fileName[4096] = {};
		sprintf_s(fileName, "./%llu.autosave.ghs", 0ULL); //::gpk::timeCurrentInMs());
		::gpk::fileFromMemory(fileName, serialized);
		game.PlayState.Paused								= true;
		return ::ghg::APP_STATE_Home;
	}
	return ::ghg::APP_STATE_Play; 
}

static	::gpk::error_t			guiHandleBrief				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Brief; }
static	::gpk::error_t			guiHandleStage				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Stage; }
static	::gpk::error_t			guiHandleStats				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Stats; }
static	::gpk::error_t			guiHandleStore				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Store; }
static	::gpk::error_t			guiHandleScore				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_Score; }
static	::gpk::error_t			guiHandleSetup				(::gpk::SGUI & /*gui*/, uint32_t idControl, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { 
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
static	::gpk::error_t			guiHandleAbout				(::gpk::SGUI & /*gui*/, uint32_t /*idControl*/, ::ghg::SGalaxyHell & /*game*/, const ::gpk::SCoord2<uint16_t> & /*screenMetrics*/) { return ::ghg::APP_STATE_About; }


static	::gpk::error_t			guiUpdatePlay				(::ghg::SGalaxyHellApp & app) { 
	::ghg::SGalaxyHell					& game						= app.World;
	if(0 == game.ShipState.ShipCores.size())
		return 0;

	::gpk::array_pod<uint32_t>			& shipParts					= game.ShipState.ShipCoresParts[0];
	const uint32_t						moduleCount					= shipParts.size();
	::gpk::SDialog						& dialog					= app.DialogPerState[::ghg::APP_STATE_Play];
	::gpk::SGUI							& gui						= *dialog.GUI;

	// Setup new viewports if necessary. This may happen if the ship acquires new modules while playing the stage
	for(uint32_t iViewport = app.UIPlay.ModuleViewports.size(); iViewport < moduleCount; ++iViewport) {
		::gpk::ptr_nco<::gpk::SDialogViewport>	viewport				= {};
		app.UIPlay.ModuleViewports[app.UIPlay.ModuleViewports.push_back({})]->Viewport = ::gpk::viewportCreate(dialog, viewport);
		app.UIPlay.ModuleViewports[iViewport]->RenderTarget.resize(::MODULE_CAMERA_SIZE);

		::gpk::SCoord2<int16_t>					& viewportSize			= gui.Controls.Controls[viewport->IdGUIControl].Area.Size;
		::gpk::viewportAdjustSize(viewportSize, MODULE_VIEWPORT_SIZE.Cast<int16_t>());
		viewport->Settings.DisplacementLockX	= true;
		viewport->Settings.DisplacementLockY	= true;
		gui.Controls.Modes		[viewport->IdGUIControl	].NoBackgroundRect		= true;
		gui.Controls.Controls	[viewport->IdGUIControl	].Align					= ::gpk::ALIGN_CENTER_BOTTOM;
		gui.Controls.Controls	[viewport->IdGUIControl	].Area.Offset			= {(int16_t)((-(int16_t)(moduleCount * viewportSize.x)>> 1) + viewportSize.x * iViewport), 0};

		gui.Controls.Controls	[viewport->IdClient].Image						= app.UIPlay.ModuleViewports[iViewport]->RenderTarget.Color.View;
		gui.Controls.Controls	[viewport->IdClient].ImageAlign					= ::gpk::ALIGN_RIGHT;
		//gui.Controls.States		[viewport->IdClient		].ImageInvertY			= true;

		::gpk::SMatrix4<float>						& matrixProjection				= app.UIPlay.ModuleViewports[iViewport]->MatrixProjection;
		matrixProjection.FieldOfView(::gpk::math_pi * .25, MODULE_CAMERA_SIZE.x / (double)MODULE_CAMERA_SIZE.y, 0.01, 500);
		::gpk::SMatrix4<float>						matrixViewport					= {};
		matrixViewport.ViewportLH(MODULE_CAMERA_SIZE.Cast<uint32_t>());
		matrixProjection						*= matrixViewport;

		char number[256] = {};
		sprintf_s(number, "#%i", iViewport + 1);
		dialog.Update();
	}
	for(uint32_t iViewport = 0; iViewport < app.UIPlay.ModuleViewports.size(); ++iViewport) {
		const ::ghg::SShipPart									& shipPart			= game.ShipState.ShipParts[shipParts[iViewport]];

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
				const float												healthRatio				= shipPart.Health / float(shipPart.MaxHealth);
				const float												toneWeight				= fabsf(sinf((float)game.PlayState.TimeStage));
				::gpk::SColorFloat										tone					= {::gpk::SColorFloat{::gpk::DARKRED * .5f} * (1.0f - healthRatio) * toneWeight};
				targetPixels.fill(tone);
				memset(depthBuffer.begin(), -1, depthBuffer.byte_count());

				::ghg::SGalaxyHellDrawCache								& drawCache				= app.UIPlay.DrawCache;

				::gpk::SMatrix4<float>									matrixView				= {};
				::gpk::SCamera											& camera				= app.UIPlay.ModuleViewports[iViewport]->Camera;
				camera.Target										= game.ShipState.Scene.Transforms[game.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Transform].GetTranslation();
				camera.Position										= camera.Target;
				camera.Position.y									+= 6; //-= 20;
				camera.Up											= {1, 0, 0};
				matrixView.LookAt(camera.Position, camera.Target, camera.Up);
				matrixView											*= app.UIPlay.ModuleViewports[iViewport]->MatrixProjection;
				drawCache.LightPointsWorld.clear();
				drawCache.LightColorsWorld.clear();
				{
					::std::lock_guard<::std::mutex>							lockUpdate					(game.LockUpdate);
					::ghg::getLightArrays(game.ShipState, game.DecoState, drawCache.LightPointsWorld, drawCache.LightColorsWorld, ::ghg::DEBRIS_COLORS);
				}
				drawCache.LightPointsModel.reserve(drawCache.LightPointsWorld.size());
				drawCache.LightColorsModel.reserve(drawCache.LightColorsWorld.size());

				uint32_t												pixelsDrawn				= 0;
				pixelsDrawn											+= ::ghg::drawShipPart(game.ShipState, shipPart, matrixView, targetPixels, depthBuffer, drawCache);
				for(uint32_t iLine = 0; iLine < 3; ++iLine)
					::gpk::drawLine(targetPixels, ::gpk::SLine2<int16_t>{{0, (int16_t)(targetPixels.metrics().y - 1 * iLine)}, {int16_t((targetPixels.metrics().x - 1) * healthRatio), (int16_t)(targetPixels.metrics().y - 1 * iLine)}}, drawCache.PixelCoords);
				for(uint32_t iPixel = 0; iPixel < drawCache.PixelCoords.size(); ++iPixel) {
					::gpk::setPixel(targetPixels, drawCache.PixelCoords[iPixel], ::gpk::interpolate_linear(::gpk::RED, ::gpk::GREEN, healthRatio));
				}
			}
		}
	}
	return 0;
}

::gpk::error_t					ghg::guiUpdate				(::ghg::SGalaxyHellApp & gameApp, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents, const ::gpk::SCoord2<uint16_t> & screenMetrics) {
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
			case ::ghg::APP_STATE_Load	: appState = (::ghg::APP_STATE)::guiHandleLoad	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Home	: appState = (::ghg::APP_STATE)::guiHandleHome	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_User	: appState = (::ghg::APP_STATE)::guiHandleUser	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Shop	: appState = (::ghg::APP_STATE)::guiHandleShop	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Play	: appState = (::ghg::APP_STATE)::guiHandlePlay	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Brief	: appState = (::ghg::APP_STATE)::guiHandleBrief	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Stage	: appState = (::ghg::APP_STATE)::guiHandleStage	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Stats	: appState = (::ghg::APP_STATE)::guiHandleStats	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Store	: appState = (::ghg::APP_STATE)::guiHandleStore	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Score	: appState = (::ghg::APP_STATE)::guiHandleScore	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_Setup	: appState = (::ghg::APP_STATE)::guiHandleSetup	(gui, idControl, game, screenMetrics); handled = true; break;
			case ::ghg::APP_STATE_About	: appState = (::ghg::APP_STATE)::guiHandleAbout	(gui, idControl, game, screenMetrics); handled = true; break;
			}
			if(handled)
				break;
		}
	}
	return appState;
}

