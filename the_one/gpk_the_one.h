#include "gpk_pool_game.h"

#include "gpk_dialog.h"
#include "gpk_gui_control.h"

#ifndef GPK_THE_ONE_H_098273498237423
#define GPK_THE_ONE_H_098273498237423

namespace the1 
{
	struct SCamera {
		::gpk::SCoord3<float>							Position					= {};
		::gpk::SCoord3<float>							Target						= {1, 0, 0};
		uint8_t											BallLockAtPosition			= (uint8_t)-1;
		uint8_t											BallLockAtTarget			= (uint8_t)-1;
	};

	typedef ::gpk::array_static<::the1::SCamera, ::the1::MAX_BALLS> TCameraStaticArray;
	struct SPlayerCameras {
		::the1::SCamera									Free						= {{-1, 1.75, -1}, {0,}};
		::the1::TCameraStaticArray						Balls						= {{{-1, 1.75, -1}, {}}};
		::the1::TCameraStaticArray						Pockets						= {{{-1, 1.75, -1}, {}}};
		::the1::SCamera									Stick						= {{-1, 1.75, -1}, {}};
		uint32_t										Selected					= 0;
	};

	struct SPlayerUI {
		::gpk::SDialog									DialogPlay					= {};
		::gpk::SDialog									DialogHome					= {};
		::the1::SPlayerCameras							Cameras						= {};
		::gpk::SUIInputBox								Name;
	};

	typedef ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> TRenderTarget;
	struct STheOneGame {
		::the1::SPoolGame								Game						= {};
		::the1::TRenderTarget							GameRenderTarget			= {};
		::gpk::array_pod<::the1::SContactBall>			ContactsToDraw				= {};
		uint32_t										CurrentPlayer				= 0;
		float											TimeScale					= 1;

		::gpk::SDialog									Dialog						= {};
		::the1::SPlayerUI								PlayerUI[::the1::MAX_BALLS]	= {};

	};

	GDEFINE_ENUM_TYPE (UI_HOME, uint8_t);
	GDEFINE_ENUM_VALUE(UI_HOME, Continue		, 0);
	GDEFINE_ENUM_VALUE(UI_HOME, Shop			, 1);
	GDEFINE_ENUM_VALUE(UI_HOME, Save			, 2);
	GDEFINE_ENUM_VALUE(UI_HOME, Load			, 3);
	GDEFINE_ENUM_VALUE(UI_HOME, Start			, 4);
	GDEFINE_ENUM_VALUE(UI_HOME, Leaderboards	, 5);
	GDEFINE_ENUM_VALUE(UI_HOME, Credits			, 6);
	GDEFINE_ENUM_VALUE(UI_HOME, Settings		, 7);
	GDEFINE_ENUM_VALUE(UI_HOME, Exit			, 8);

	GDEFINE_ENUM_TYPE(APP_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(APP_STATE, Init		,  0);
	GDEFINE_ENUM_VALUE(APP_STATE, Welcome	,  1);
	GDEFINE_ENUM_VALUE(APP_STATE, Home		,  2);
	GDEFINE_ENUM_VALUE(APP_STATE, Profile	,  3);
	GDEFINE_ENUM_VALUE(APP_STATE, Shop		,  4);
	GDEFINE_ENUM_VALUE(APP_STATE, Play		,  5);
	GDEFINE_ENUM_VALUE(APP_STATE, Brief		,  6);
	GDEFINE_ENUM_VALUE(APP_STATE, Stage		,  7);
	GDEFINE_ENUM_VALUE(APP_STATE, Stats		,  8);
	GDEFINE_ENUM_VALUE(APP_STATE, Store		,  9);
	GDEFINE_ENUM_VALUE(APP_STATE, Score		, 10);
	GDEFINE_ENUM_VALUE(APP_STATE, About		, 11);
	GDEFINE_ENUM_VALUE(APP_STATE, Settings	, 12);
	GDEFINE_ENUM_VALUE(APP_STATE, Quit		, 13);
	GDEFINE_ENUM_VALUE(APP_STATE, Load		, 14);
	GDEFINE_ENUM_VALUE(APP_STATE, COUNT		, 15);

	struct STheOne {
		::the1::STheOneGame								MainGame					= {};
		::gpk::array_static<::the1::STheOneGame, 16>	TestGames					= {};
		::gpk::SDialog									Dialog						= {};
		::gpk::array_static<::gpk::SDialog
			, ::the1::APP_STATE_COUNT>					DialogPerState				= {};

		::gpk::SVirtualKeyboard							VirtualKeyboard				= {};

		APP_STATE										ActiveState					= {};
	};

	::gpk::error_t									theOneUpdate		(::the1::STheOne & app, double secondsElapsed, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::view_array<::gpk::SSysEvent> & systemEvents);
	::gpk::error_t									theOneDraw			(::the1::STheOne & app, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t> & backBuffer, double totalSeconds);

	::gpk::error_t									guiSetup			(::the1::STheOne & app, const ::gpk::ptr_obj<::gpk::SInput> & input);
	::gpk::error_t									guiUpdate			(::the1::STheOne & app, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents);
} // namespace

#endif