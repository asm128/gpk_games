#include "gpk_galaxy_hell.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_control.h"
#include "gpk_deflate.h"
#include "gpk_storage.h"

namespace ghg
{
	GDEFINE_ENUM_TYPE(APP_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(APP_STATE, Init		, 0);
	GDEFINE_ENUM_VALUE(APP_STATE, Home		, 1);
	GDEFINE_ENUM_VALUE(APP_STATE, Profile	, 2);
	GDEFINE_ENUM_VALUE(APP_STATE, Shop		, 3);
	GDEFINE_ENUM_VALUE(APP_STATE, Play		, 4);
	GDEFINE_ENUM_VALUE(APP_STATE, Brief		, 5);
	GDEFINE_ENUM_VALUE(APP_STATE, Stage		, 6);
	GDEFINE_ENUM_VALUE(APP_STATE, Stats		, 7);
	GDEFINE_ENUM_VALUE(APP_STATE, Store		, 8);
	GDEFINE_ENUM_VALUE(APP_STATE, Score		, 9);
	GDEFINE_ENUM_VALUE(APP_STATE, About		, 10);
	GDEFINE_ENUM_VALUE(APP_STATE, Settings	, 11);
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

	GDEFINE_ENUM_TYPE (UI_PROFILE, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Back				, 0);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Name				, 1);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Score			, 2);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Hits				, 3);
	GDEFINE_ENUM_VALUE(UI_PROFILE, DamageDone		, 4);
	GDEFINE_ENUM_VALUE(UI_PROFILE, DamageReceived	, 5);
	GDEFINE_ENUM_VALUE(UI_PROFILE, HitsSurvived		, 6);
	GDEFINE_ENUM_VALUE(UI_PROFILE, OrbitersLost		, 7);
	GDEFINE_ENUM_VALUE(UI_PROFILE, KilledShips		, 8);
	GDEFINE_ENUM_VALUE(UI_PROFILE, KilledOrbiters	, 9);
	GDEFINE_ENUM_VALUE(UI_PROFILE, TimeReal			,10);
	GDEFINE_ENUM_VALUE(UI_PROFILE, TimeWorld		,11);

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
	GDEFINE_ENUM_TYPE (UI_DESKTOP, uint8_t);

	GDEFINE_ENUM_TYPE (UI_SETTINGS, uint8_t);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Back		, 0);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Game		, 1);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Graphics	, 2);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Audio		, 3);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Controller	, 4);

	GDEFINE_ENUM_TYPE (UI_SHOP, uint8_t);
	GDEFINE_ENUM_VALUE(UI_SHOP, Back			, 0);

	GDEFINE_ENUM_TYPE (UI_CREDITS, uint8_t);
	GDEFINE_ENUM_VALUE(UI_CREDITS, Back			, 0);
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

	GDEFINE_ENUM_TYPE(UI_BRIEF, uint8_t);
	GDEFINE_ENUM_TYPE(UI_STAGE, uint8_t);
	GDEFINE_ENUM_TYPE(UI_STATS, uint8_t);
	GDEFINE_ENUM_TYPE(UI_STORE, uint8_t);
	GDEFINE_ENUM_TYPE(UI_SCORE, uint8_t);

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
		::gpk::array_pobj<::ghg::SUIPlayModuleViewport>	ModuleViewports			= {};
		::gpk::SDialog									Dialog					= {};
		::gpk::array_static<char, 128>					TextScore				= {};
		::gpk::array_static<char, 128>					TextHits				= {};
		::gpk::array_static<char, 128>					TextDamageDone			= {};
		::gpk::array_static<char, 128>					TextDamageReceived		= {};
		::gpk::array_static<char, 128>					TextHitsSurvived		= {};
		::gpk::array_static<char, 128>					TextOrbitersLost		= {};
		::gpk::array_static<char, 128>					TextKilledShips			= {};
		::gpk::array_static<char, 128>					TextKilledOrbiters		= {};
	};

	struct SUIPlay {
		::gpk::array_pobj<::ghg::SUIPlayModuleViewport>	ModuleViewports			= {};
		::gpk::array_obj<::ghg::SUIPlayer>				PlayerUIs				= {};
		::ghg::SGalaxyHellDrawCache						DrawCache				= {};

		::gpk::array_static<char, 128>					TextLevel				= {};
		::gpk::array_static<char, 128>					TextTimeStage			= {};
		::gpk::array_static<char, 128>					TextTimeWorld			= {};
		::gpk::array_static<char, 128>					TextTimeReal			= {};
	};

	struct SPlayerShip { 
		::ghg::SShipCore								Core;
		::ghg::SShipScore								Score;
		::gpk::array_pod<uint32_t>						Parts;
		::gpk::array_pod<uint32_t>						Weapons;

		::gpk::error_t									Save					(::gpk::array_pod<byte_t> & output) const {
			gpk_necs(::gpk::viewWrite(::gpk::view_array<const ::ghg::SShipCore >{&Core , 1}, output));
			gpk_necs(::gpk::viewWrite(::gpk::view_array<const ::ghg::SShipScore>{&Score, 1}, output));
			gpk_necs(::gpk::viewWrite(Parts		, output));
			gpk_necs(::gpk::viewWrite(Weapons	, output));
			return 0;
		}

		::gpk::error_t									Load					(::gpk::vcc & input) {
			int32_t												bytesRead				= 0;
			::gpk::view_array<const ::ghg::SShipCore>			readCore				= {};
			::gpk::view_array<const ::ghg::SShipScore>			readScore				= {};
			::gpk::view_array<const uint32_t>					readParts				= {};
			::gpk::view_array<const uint32_t>					readWeapons				= {};
			gpk_necs(bytesRead = ::gpk::viewRead(readCore		, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; Core		= readCore	[0];
			gpk_necs(bytesRead = ::gpk::viewRead(readScore		, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; Score		= readScore	[0];
			gpk_necs(bytesRead = ::gpk::viewRead(readParts		, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; Parts		= readParts		;
			gpk_necs(bytesRead = ::gpk::viewRead(readWeapons	, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; Weapons	= readWeapons	;
			return 0;
		}
	};


#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (PLAYER_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, Human	, 0);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, AI		, 1);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, Network	, 2);

	struct SPlayerScore { 
		uint64_t								TimePlayed;
		uint64_t								TimeDead;
		uint64_t								KeyPresses;
	};

	struct SPlayerState {
		PLAYER_TYPE								Type;
		::gpk::SColorBGRA						Color;
		int32_t									ShipSelected;
		::ghg::SPlayerScore						Score;
	};
#pragma pack(pop)

	struct SPlayer {
		::gpk::vcc								Name			= {};
		::ghg::SPlayerState						State			= {};
		::gpk::array_pod<::ghg::SWeapon>		Weapons			= {};
		::gpk::array_pod<::ghg::SOrbiter>		Orbiters		= {};
		::gpk::array_obj<::ghg::SPlayerShip>	Ships			= {};

		::gpk::error_t							Save(const ::gpk::vcc & filename) const {
			::gpk::array_pod<byte_t>					serialized;
			gpk_necs(Save(serialized));
			::gpk::array_pod<byte_t>					deflated;
			gpk_necs(::gpk::arrayDeflate(serialized, deflated));
			info_printf("Player size in bytes: %u.", serialized.size());
			info_printf("Player file size: %u.", deflated.size());
			gpk_necall(::gpk::fileFromMemory(filename, deflated), "fileName: %s", ::gpk::toString(filename).begin());
			return 0;
		}

		::gpk::error_t							Load(const ::gpk::vcc & filename) {
			::gpk::array_pod<byte_t>					serialized;
			gpk_necall(::gpk::fileToMemory(filename, serialized), "fileName: %s", ::gpk::toString(filename).begin());
			::gpk::array_pod<byte_t>					inflated;
			gpk_necs(::gpk::arrayInflate(serialized, inflated));
			info_printf("Player file size: %u.", inflated.size());
			info_printf("Player size in bytes: %u.", serialized.size());
			return Load(inflated);
		}

		::gpk::error_t							Save(::gpk::array_pod<byte_t> & output) const {
			gpk_necs(::gpk::viewWrite(Name		, output));
			gpk_necs(::gpk::viewWrite(::gpk::view_array<const ::ghg::SPlayerState>{&State, 1}, output));
			gpk_necs(::gpk::viewWrite(Weapons	, output));
			gpk_necs(::gpk::viewWrite(Orbiters	, output));
			gpk_necs(output.append(::gpk::vcc{(const byte_t*)&Ships.size(), 4}));
			for(uint32_t iShip = 0; iShip < Ships.size(); ++iShip) {
				gpk_necall(Ships[iShip].Save(output), "iShip: %u", iShip);
			}
			return 0;
		}

		::gpk::error_t							Load(::gpk::vcc & input) {
			int32_t												bytesRead				= 0;
			::gpk::vcc											readName				= {};
			::gpk::view_array<const ::ghg::SPlayerState>		readPlayerState			= {};
			::gpk::view_array<const ::ghg::SWeapon	>			readWeapons				= {};
			::gpk::view_array<const ::ghg::SOrbiter	>			readOrbiters			= {};
			gpk_necs(bytesRead = ::gpk::viewRead(readName			, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; Name		= readName			;
			gpk_necs(bytesRead = ::gpk::viewRead(readPlayerState	, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; State		= readPlayerState	[0];
			gpk_necs(bytesRead = ::gpk::viewRead(readWeapons		, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; Weapons	= readWeapons		;
			gpk_necs(bytesRead = ::gpk::viewRead(readOrbiters		, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; Orbiters	= readOrbiters		;
			gpk_necs(Ships.resize(*(uint32_t*)input.begin()));
			input											= {&input[4], input.size() - 4};
			for(uint32_t iShip = 0; iShip < Ships.size(); ++iShip) {
				gpk_necall(Ships[iShip].Load(input), "iShip: %u", iShip);
			}
			return 0;
		}
	};

	struct SUserCredentials {
		::gpk::vcc								Username;
		::gpk::vcc								Password;
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

		::gpk::SVirtualKeyboard										VirtualKeyboard				= {};
		::gpk::SDialog												DialogDesktop				= {};
		::gpk::array_obj<::ghg::SPlayer>							Players						= {};

		::ghg::SGalaxyHell											Game;

		::gpk::array_pod<::ghg::SUserCredentials>					UserCredentials				= {};

		::gpk::array_obj<::gpk::array_pod<char>>					FileNames					= {};
		::gpk::vcs													SavegameFolder				= ".";
		::gpk::vcs													ExtensionSaveAuto			= ".auto.ghz";
		::gpk::vcs													ExtensionSaveStage			= ".stage.ghz";
		::gpk::vcs													ExtensionSaveUser			= ".user.ghz";
		::gpk::vcs													ExtensionSave				= ".ghz";
		::gpk::vcs													ExtensionImages				= ".png";

		::gpk::vcs													ExtensionProfile			= ".ghp";
		::gpk::vcs													ExtensionCredentials		= ".ghc";

		::ghg::SUIPlay												UIPlay;

		APP_STATE													ActiveState					= APP_STATE_Init;

		::gpk::error_t												Load						(const ::gpk::vcs fileName)				{
			::gpk::mutex_guard guard(Game.LockUpdate);
			return ::ghg::solarSystemLoad(Game, fileName);
		}

		::gpk::error_t												Save						(SAVE_MODE autosaveMode)				{
			char															fileName[4096]				= {};
			for(uint32_t iPlayer = 0; iPlayer < Players.size(); ++iPlayer) {
				const ::ghg::SPlayer & player = Players[iPlayer];
				sprintf_s(fileName, "%s/%s.%llu%s", SavegameFolder.begin(), ::gpk::toString(player.Name).begin(), ::gpk::timeCurrent(), ExtensionProfile.begin());
				gpk_necall(player.Save(fileName), "iPlayer: %i", iPlayer);
			}
			::gpk::vcc														extension;
			switch(autosaveMode) {
			case SAVE_MODE_USER		: extension = ExtensionSaveUser		; break;
			case SAVE_MODE_STAGE	: extension = ExtensionSaveStage	; break;
			case SAVE_MODE_AUTO		: extension = ExtensionSaveAuto		; break;
			}

			const uint64_t													timeCurrent					= (SAVE_MODE_USER != autosaveMode) ? 0 : ::gpk::timeCurrent();
			{
				const uint64_t													timeStart					= (SAVE_MODE_AUTO == autosaveMode) ? 0 : Game.PlayState.TimeStart;
				sprintf_s(fileName, "%s/%llu.%llu%s", SavegameFolder.begin(), timeStart, timeCurrent, extension.begin());

				// Save only if a player is alive
				const int32_t													totalHealth					= Game.ShipState.GetTeamHealth(0);
				if(totalHealth > 0) 
					return ::ghg::solarSystemSave(Game, fileName);
			}
			return 0;
		}
	};

	::gpk::error_t												guiSetup					(::ghg::SGalaxyHellApp & gameui, const ::gpk::ptr_obj<::gpk::SInput> & inputState);
	::gpk::error_t												guiUpdate					(::ghg::SGalaxyHellApp & gameui, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents);
	
	::gpk::error_t												galaxyHellUpdate			(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::ptr_obj<::gpk::SInput> & inputState, const ::gpk::view_array<::gpk::SSysEvent> & systemEvents);
	::gpk::error_t												galaxyHellDraw				(::ghg::SGalaxyHellApp & app, ::gpk::SCoord2<uint16_t> renderTargetSize);
	::gpk::error_t												galaxyHellAppLoad			(::ghg::SGalaxyHellApp & app,::gpk::vcc filename);
	::gpk::error_t												galaxyHellAppSave			(const ::ghg::SGalaxyHellApp & app,::gpk::vcc filename);

	::gpk::error_t												listFilesSavegame			(::ghg::SGalaxyHellApp & app, const ::gpk::vcc & saveGameFolder, ::gpk::array_obj<::gpk::vcc> & savegameFilenames);
	::gpk::error_t												listFilesProfile			(::ghg::SGalaxyHellApp & app, const ::gpk::vcc & saveGameFolder, ::gpk::array_obj<::gpk::vcc> & savegameFilenames);

}
