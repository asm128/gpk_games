#include "gpk_galaxy_hell.h"
#include "gpk_galaxy_hell_overlay.h"

#include "gpk_ptr.h"

namespace ghg
{
	GDEFINE_ENUM_TYPE(APP_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(APP_STATE, Load		, 0);
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
	GDEFINE_ENUM_VALUE(APP_STATE, Quit		, 11);
	GDEFINE_ENUM_VALUE(APP_STATE, COUNT		, 12);

	// Start New Game
	// Continue Game
	// Load Game
	// Save Game
	// High Scores
	// Credits
	// Exit
	GDEFINE_ENUM_TYPE (UI_HOME, uint8_t	);
	GDEFINE_ENUM_VALUE(UI_HOME, Start			, 0);
	GDEFINE_ENUM_VALUE(UI_HOME, Continue		, 1);
	GDEFINE_ENUM_VALUE(UI_HOME, Load			, 2);
	GDEFINE_ENUM_VALUE(UI_HOME, Save			, 3);
	GDEFINE_ENUM_VALUE(UI_HOME, Profile			, 4);
	GDEFINE_ENUM_VALUE(UI_HOME, Leaderboards	, 5);
	GDEFINE_ENUM_VALUE(UI_HOME, Credits			, 6);
	GDEFINE_ENUM_VALUE(UI_HOME, Settings		, 7);
	GDEFINE_ENUM_VALUE(UI_HOME, Exit			, 8);

	GDEFINE_ENUM_TYPE (UI_PLAY, uint8_t	);
	GDEFINE_ENUM_VALUE(UI_PLAY, Menu			, 0);

	struct SShipUI {
		int32_t														IdControl;	// Root control of the ship UI

		::gpk::array_pod<int32_t>									IdShipCameras; // These belong to the root control, which in turn may contain child controls not stored in this array. Indices are 0 for the center of gravity and the rest for each module of the ship.
		::gpk::array_pod<int32_t>									IdShipWeapons; // These belong to the root control, which in turn may contain child controls not stored in this array. Indices are 0 for the center of gravity and the rest for each module of the ship.
		::gpk::array_pod<int32_t>									IdShipHealth ; // These belong to the root control, which in turn may contain child controls not stored in this array. Indices are 0 for the center of gravity and the rest for each module of the ship.
	};

	struct SGameUI {
		::gpk::array_obj<::ghg::SShipUI>							Ships;

		bool														Exit						= false;
		::gpk::array_static<::gpk::SGUI, ::ghg::APP_STATE_COUNT>	GUIPerState;

	};

	::gpk::error_t												guiSetup				(::ghg::SGameUI & gameui);
	::gpk::error_t												guiUpdate				(::ghg::SGameUI & gameui, ::ghg::SGalaxyHell & game, ::ghg::APP_STATE appState, const ::gpk::SInput & input, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents, const ::gpk::SCoord2<uint16_t> & screenMetrics);
	
	struct SGalaxyHellApp {
		::gpk::ptr_obj<TRenderTarget>								RenderTarget[16]			= {};
		volatile uint64_t											CurrentRenderTarget			= 0;

		::ghg::SGalaxyHell											World;
		::ghg::SGameUI												UI;
		::ghg::STextOverlay											Overlay;

		APP_STATE													ActiveState					= APP_STATE_Load;
	};

	::gpk::error_t												galaxyHellUpdate			(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::SInput & inputState, const ::gpk::view_array<::gpk::SSysEvent> & systemEvents, const ::gpk::SCoord2<uint16_t> & windowSize);
	::gpk::error_t												galaxyHellDraw				(::ghg::SGalaxyHellApp & app, ::gpk::SCoord2<uint16_t> renderTargetSize);
}