#include "gpk_galaxy_hell.h"

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

	GDEFINE_ENUM_TYPE (UI_HOME, uint8_t);
	GDEFINE_ENUM_VALUE(UI_HOME, Continue		, 0);
	GDEFINE_ENUM_VALUE(UI_HOME, Start			, 1);
	GDEFINE_ENUM_VALUE(UI_HOME, Shop			, 2);
	GDEFINE_ENUM_VALUE(UI_HOME, Load			, 3);
	GDEFINE_ENUM_VALUE(UI_HOME, Save			, 4);
	GDEFINE_ENUM_VALUE(UI_HOME, Profile			, 5);
	GDEFINE_ENUM_VALUE(UI_HOME, Leaderboards	, 6);
	GDEFINE_ENUM_VALUE(UI_HOME, Credits			, 7);
	GDEFINE_ENUM_VALUE(UI_HOME, Settings		, 8);
	GDEFINE_ENUM_VALUE(UI_HOME, Exit			, 9);

	GDEFINE_ENUM_TYPE (UI_PLAY, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PLAY, Menu			, 0);
	GDEFINE_ENUM_VALUE(UI_PLAY, Level			, 1);
	GDEFINE_ENUM_VALUE(UI_PLAY, TimeReal		, 2);
	GDEFINE_ENUM_VALUE(UI_PLAY, TimeWorld		, 3);
	GDEFINE_ENUM_VALUE(UI_PLAY, TimeStage		, 4);
	
	GDEFINE_ENUM_TYPE (UI_PLAYER, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PLAYER, Score				, 0);
	GDEFINE_ENUM_VALUE(UI_PLAYER, Hits				, 1);
	GDEFINE_ENUM_VALUE(UI_PLAYER, DamageDone		, 2);
	GDEFINE_ENUM_VALUE(UI_PLAYER, DamageReceived	, 3);
	GDEFINE_ENUM_VALUE(UI_PLAYER, HitsSurvived		, 4);
	GDEFINE_ENUM_VALUE(UI_PLAYER, OrbitersLost		, 5);
	GDEFINE_ENUM_VALUE(UI_PLAYER, KilledShips		, 6);
	GDEFINE_ENUM_VALUE(UI_PLAYER, KilledOrbiters	, 7);

	GDEFINE_ENUM_TYPE (UI_LOAD, uint8_t	);

	GDEFINE_ENUM_TYPE (UI_SETTINGS, uint8_t);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Game		, 0x0);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Graphics	, 0x1);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Audio		, 0x2);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Controller	, 0x3);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Back		, 0x3);

	GDEFINE_ENUM_TYPE (UI_SHOP, uint8_t);
	GDEFINE_ENUM_VALUE(UI_SHOP, Back			, 0x0);

	GDEFINE_ENUM_TYPE (UI_CREDITS, uint8_t);
	GDEFINE_ENUM_VALUE(UI_CREDITS, Back			, 0x0);
	GDEFINE_ENUM_VALUE(UI_CREDITS, __________	, 1);
	GDEFINE_ENUM_VALUE(UI_CREDITS, Idea			, 2);
	GDEFINE_ENUM_VALUE(UI_CREDITS, Gontrill		, 3);
	GDEFINE_ENUM_VALUE(UI_CREDITS, _________	, 4);
	GDEFINE_ENUM_VALUE(UI_CREDITS, Programming	, 5);
	GDEFINE_ENUM_VALUE(UI_CREDITS, asm128		, 6);
	GDEFINE_ENUM_VALUE(UI_CREDITS, ________		, 7);
	GDEFINE_ENUM_VALUE(UI_CREDITS, Sounds		, 8);
	GDEFINE_ENUM_VALUE(UI_CREDITS, Microph		, 9);
	GDEFINE_ENUM_VALUE(UI_CREDITS, _______		, 10);
	GDEFINE_ENUM_VALUE(UI_CREDITS, Graphics		, 11);
	GDEFINE_ENUM_VALUE(UI_CREDITS, PabloAriel	, 12);
	GDEFINE_ENUM_VALUE(UI_CREDITS, ______		, 13);
	GDEFINE_ENUM_VALUE(UI_CREDITS, Home			, 14);

	struct SUIControlGauge {
		::gpk::array_pod<::gpk::SCoord3<float>>				Vertices				= {};
		::gpk::array_pod<::gpk::STriangleWeights<int16_t>>	Indices					= {};
		int16_t												CurrentValue			= 0;
		int16_t												MaxValue				= 64;
		::gpk::SImage<::gpk::SColorBGRA>					Image					= {};

		static constexpr const ::gpk::SColorFloat			COLOR_MIN_DEFAULT		= ::gpk::GREEN;
		static constexpr const ::gpk::SColorFloat			COLOR_MID_DEFAULT		= ::gpk::ORANGE;
		static constexpr const ::gpk::SColorFloat			COLOR_MAX_DEFAULT		= ::gpk::RED;
		static constexpr const ::gpk::SColorFloat			COLOR_BKG_DEFAULT		= ::gpk::BROWN * .5 + ::gpk::DARKBLUE * .5;
		
		inline	int16_t										SetValue				(float weight)							noexcept	{ return CurrentValue = int16_t(weight * MaxValue); }
				int16_t										SetValue				(float currentValue, uint32_t maxValue)	noexcept	{ return SetValue(currentValue / maxValue); }
	};

			::gpk::error_t	gaugeBuildRadial		(::ghg::SUIControlGauge & gauge, const ::gpk::SCircle<float> & gaugeMetrics, int16_t resolution, int16_t width);
			::gpk::error_t	gaugeImageUpdate		(::ghg::SUIControlGauge & gauge, ::gpk::view_grid<::gpk::SColorBGRA> target	, ::gpk::SColorFloat colorMin = ::ghg::SUIControlGauge::COLOR_MIN_DEFAULT, ::gpk::SColorFloat colorMid = ::ghg::SUIControlGauge::COLOR_MID_DEFAULT, ::gpk::SColorFloat colorMax = ::ghg::SUIControlGauge::COLOR_MAX_DEFAULT, ::gpk::SColorBGRA colorEmpty = ::ghg::SUIControlGauge::COLOR_BKG_DEFAULT);
	inline	::gpk::error_t	gaugeImageUpdate		(::ghg::SUIControlGauge & gauge												, ::gpk::SColorFloat colorMin = ::ghg::SUIControlGauge::COLOR_MIN_DEFAULT, ::gpk::SColorFloat colorMid = ::ghg::SUIControlGauge::COLOR_MID_DEFAULT, ::gpk::SColorFloat colorMax = ::ghg::SUIControlGauge::COLOR_MAX_DEFAULT, ::gpk::SColorBGRA colorEmpty = ::ghg::SUIControlGauge::COLOR_BKG_DEFAULT) {
		//gauge.Image.Texels.fill({0, 0, 0, 0});
		return ::ghg::gaugeImageUpdate(gauge, gauge.Image, colorMin, colorMid, colorMax, colorEmpty);
	}

	struct SUIPlayModuleViewport {
		int32_t											Viewport		;
		::gpk::SCamera									Camera			;
		SUIControlGauge									GaugeLife		;
		SUIControlGauge									GaugeCooldown	;
		SUIControlGauge									GaugeDelay		;
		::gpk::SMatrix4	<float>							MatrixProjection;
		::ghg::TRenderTarget							RenderTargetOrbiter	;
		::ghg::TRenderTarget							RenderTargetWeaponLoad	;
		::ghg::TRenderTarget							RenderTargetWeaponType	;
	};

	struct SUIPlayer {
		::gpk::array_pobj<::ghg::SUIPlayModuleViewport>	ModuleViewports		= {};
		::gpk::SDialog									Dialog				= {};
		::gpk::array_static<char, 128>					TextScore			= {};
		::gpk::array_static<char, 128>					TextHits			= {};
		::gpk::array_static<char, 128>					TextDamageDone		= {};
		::gpk::array_static<char, 128>					TextDamageReceived	= {};
		::gpk::array_static<char, 128>					TextHitsSurvived	= {};
		::gpk::array_static<char, 128>					TextOrbitersLost	= {};
		::gpk::array_static<char, 128>					TextKilledShips		= {};
		::gpk::array_static<char, 128>					TextKilledOrbiters	= {};
	};

	struct SUIPlay {
		::gpk::array_pobj<::ghg::SUIPlayModuleViewport>	ModuleViewports			= {};
		::gpk::array_obj<::ghg::SUIPlayer>				PlayerUIs				= {};
		::ghg::SGalaxyHellDrawCache						DrawCache				= {};

		::gpk::array_static<char, 128>					TextLevel		= {};
		::gpk::array_static<char, 128>					TextTimeStage	= {};
		::gpk::array_static<char, 128>					TextTimeWorld	= {};
		::gpk::array_static<char, 128>					TextTimeReal	= {};
	};

	enum SAVE_MODE { SAVE_MODE_AUTO, SAVE_MODE_STAGE, SAVE_MODE_USER };

	struct SGalaxyHellApp {
		::gpk::array_pobj<TRenderTarget>							RenderTargetPool			= {};
		::gpk::array_pobj<TRenderTarget>							RenderTargetQueue			= {};
		::std::mutex												RenderTargetLockPool		= {};
		::std::mutex												RenderTargetLockQueue		= {};
		volatile uint64_t											CurrentRenderTarget			= 0;

		bool														Exit						= false;
		::gpk::array_static<::gpk::SDialog, ::ghg::APP_STATE_COUNT>	DialogPerState				= {};

		::ghg::SGalaxyHell											World;

		::gpk::array_obj<::gpk::array_pod<char>>					FileNames					= {};
		::gpk::array_pobj<::ghg::SGalaxyHell>						FileGames					= {};
		::gpk::vcs													SavegameFolder				= ".";
		::gpk::vcs													ExtensionSaveAuto			= ".auto.ghz";
		::gpk::vcs													ExtensionSaveCheckpoint		= ".stage.ghz";
		::gpk::vcs													ExtensionSaveUser			= ".user.ghz";
		::gpk::vcs													ExtensionSave				= ".ghz";
		::gpk::vcs													ExtensionImages				= ".png";
		::ghg::SUIPlay												UIPlay;

		APP_STATE													ActiveState					= APP_STATE_Init;

		::gpk::error_t												Load						(const ::gpk::vcs fileName)				{
			::gpk::mutex_guard guard(World.LockUpdate);
			return ::ghg::solarSystemLoad(World, fileName);
		}

		::gpk::error_t												Save						(SAVE_MODE autosaveMode)				{
			::gpk::mutex_guard guard(World.LockUpdate);
			::gpk::vcc														extension;
			const uint64_t													timeStart					= (SAVE_MODE_AUTO == autosaveMode) ? 0 : World.PlayState.TimeStart;
			const uint64_t													timeCurrent					= (SAVE_MODE_USER != autosaveMode) ? 0 : ::gpk::timeCurrent();
			switch(autosaveMode) {
			case SAVE_MODE_USER		: extension = ExtensionSaveUser			; break;
			case SAVE_MODE_STAGE	: extension = ExtensionSaveCheckpoint	; break;
			case SAVE_MODE_AUTO		: extension = ExtensionSaveAuto			; break;
			}
			char															fileName[4096]				= {};
			sprintf_s(fileName, "%s/%llu.%llu%s", SavegameFolder.begin(), timeStart, timeCurrent, extension.begin());

			// Save only if a player is alive
			int32_t								totalHealth			= World.ShipState.GetTeamHealth(0);
			if(totalHealth > 0) 
				return ::ghg::solarSystemSave(World, fileName);

			return 0;
		}
	};

	::gpk::error_t												guiSetup					(::ghg::SGalaxyHellApp & gameui, const ::gpk::ptr_obj<::gpk::SInput> & inputState);
	::gpk::error_t												guiUpdate					(::ghg::SGalaxyHellApp & gameui, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents);
	
	::gpk::error_t												galaxyHellUpdate			(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::view_array<::gpk::SSysEvent> & systemEvents);
	::gpk::error_t												galaxyHellDraw				(::ghg::SGalaxyHellApp & app, ::gpk::SCoord2<uint16_t> renderTargetSize);

}
