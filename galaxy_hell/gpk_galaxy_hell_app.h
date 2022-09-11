#include "gpk_galaxy_hell.h"
#include "gpk_galaxy_hell_overlay.h"

#include "gpk_dialog_controls.h"

namespace ghg
{
	GDEFINE_ENUM_TYPE(APP_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(APP_STATE, Init		, 0);
	GDEFINE_ENUM_VALUE(APP_STATE, Home		, 1);
	GDEFINE_ENUM_VALUE(APP_STATE, User		, 2);
	GDEFINE_ENUM_VALUE(APP_STATE, Shop		, 3);
	GDEFINE_ENUM_VALUE(APP_STATE, Play		, 4);
	GDEFINE_ENUM_VALUE(APP_STATE, Brief		, 5);
	GDEFINE_ENUM_VALUE(APP_STATE, Stage		, 6);
	GDEFINE_ENUM_VALUE(APP_STATE, Stats		, 7);
	GDEFINE_ENUM_VALUE(APP_STATE, Store		, 8);
	GDEFINE_ENUM_VALUE(APP_STATE, Score		, 9);
	GDEFINE_ENUM_VALUE(APP_STATE, About		, 10);
	GDEFINE_ENUM_VALUE(APP_STATE, Setup		, 11);
	GDEFINE_ENUM_VALUE(APP_STATE, Quit		, 12);
	GDEFINE_ENUM_VALUE(APP_STATE, Load		, 13);
	GDEFINE_ENUM_VALUE(APP_STATE, COUNT		, 14);

	// Start New Game
	// Continue Game
	// Load Game
	// Save Game
	// High Scores
	// Credits
	// Exit
	GDEFINE_ENUM_TYPE (UI_HOME, uint8_t);
	GDEFINE_ENUM_VALUE(UI_HOME, Start			, 0);
	GDEFINE_ENUM_VALUE(UI_HOME, Continue		, 1);
	GDEFINE_ENUM_VALUE(UI_HOME, Shop			, 2);
	GDEFINE_ENUM_VALUE(UI_HOME, Load			, 3);
	GDEFINE_ENUM_VALUE(UI_HOME, Save			, 4);
	GDEFINE_ENUM_VALUE(UI_HOME, Profile			, 5);
	GDEFINE_ENUM_VALUE(UI_HOME, Leaderboards	, 6);
	GDEFINE_ENUM_VALUE(UI_HOME, Credits			, 7);
	GDEFINE_ENUM_VALUE(UI_HOME, Settings		, 8);
	GDEFINE_ENUM_VALUE(UI_HOME, Exit			, 9);

	GDEFINE_ENUM_TYPE (UI_PLAY, uint8_t	);
	GDEFINE_ENUM_VALUE(UI_PLAY, Menu			, 0);

	GDEFINE_ENUM_TYPE (UI_LOAD, uint8_t	);

	GDEFINE_ENUM_TYPE (UI_SETTINGS, uint8_t);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Game		, 0x0);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Graphics	, 0x1);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Audio		, 0x2);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Controller	, 0x3);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Back		, 0x3);

	GDEFINE_ENUM_TYPE (UI_SHOP, uint8_t);
	GDEFINE_ENUM_VALUE(UI_SHOP, Back			, 0x0);

	struct SUIPlayModuleViewport {
		::gpk::SMatrix4	<float>										MatrixProjection			;
		::gpk::SCamera												Camera						;
		::ghg::TRenderTarget										RenderTarget				;
		int32_t														Viewport					;
	};

	struct SUIPlay {
		::gpk::array_pobj<::ghg::SUIPlayModuleViewport>				ModuleViewports					;
		::ghg::SGalaxyHellDrawCache									DrawCache						;
	};

	struct SGalaxyHellApp {
		::gpk::ptr_obj<TRenderTarget>								RenderTarget[16]			= {};
		volatile uint64_t											CurrentRenderTarget			= 0;

		bool														Exit						= false;
		::gpk::array_static<::gpk::SDialog, ::ghg::APP_STATE_COUNT>	DialogPerState				= {};

		::ghg::SGalaxyHell											World;
		::ghg::STextOverlay											Overlay;

		::gpk::array_obj<::gpk::array_pod<char>>					FileNames					= {};
		::gpk::vcs													SavegameFolder				= ".";
		::gpk::vcs													ExtensionSaveAuto			= ".autosave.ghs";
		::gpk::vcs													ExtensionSaveCheckpoint		= ".checkpoint.ghs";
		::gpk::vcs													ExtensionSaveUser			= ".user.ghs";
		::gpk::vcs													ExtensionSave				= ".ghs";
		::gpk::vcs													ExtensionImages				= ".png";
		::ghg::SUIPlay												UIPlay;

		APP_STATE													ActiveState					= APP_STATE_Init;
	};

	::gpk::error_t												guiSetup					(::ghg::SGalaxyHellApp & gameui, const ::gpk::ptr_obj<::gpk::SInput> & inputState);
	::gpk::error_t												guiUpdate					(::ghg::SGalaxyHellApp & gameui, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents);
	
	::gpk::error_t												galaxyHellUpdate			(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::view_array<::gpk::SSysEvent> & systemEvents);
	::gpk::error_t												galaxyHellDraw				(::ghg::SGalaxyHellApp & app, ::gpk::SCoord2<uint16_t> renderTargetSize);

}
