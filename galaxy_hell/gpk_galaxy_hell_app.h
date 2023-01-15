#include "gpk_galaxy_hell.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_control.h"
#include "gpk_deflate.h"
#include "gpk_storage.h"

namespace ghg
{
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
	
	GDEFINE_ENUM_TYPE (UI_PROFILE, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Name				, 0);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Score			, 1);

	GDEFINE_ENUM_TYPE (UI_PLAY, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PLAY, Menu			, 0);
	GDEFINE_ENUM_VALUE(UI_PLAY, Level			, 1);
	GDEFINE_ENUM_VALUE(UI_PLAY, TimeReal		, 2);
	GDEFINE_ENUM_VALUE(UI_PLAY, TimeStage		, 3);
	
	GDEFINE_ENUM_TYPE (UI_PILOT, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PILOT, Name				, 0);
	GDEFINE_ENUM_VALUE(UI_PILOT, Score				, 1);
	
	GDEFINE_ENUM_TYPE (UI_PLAYER_SCORE, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, Score				, 0);
	GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, Hits				, 1);
	GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, Shots				, 2);
	GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, Bullets				, 3);
	GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, DamageDone			, 4);
	GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, DamageReceived		, 5);
	GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, HitsSurvived		, 6);
	GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, OrbitersLost		, 7);
	GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, KilledShips			, 8);
	GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, KilledOrbiters		, 9);
	//GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, TimeReal			,10);
	//GDEFINE_ENUM_VALUE(UI_PLAYER_SCORE, TimeSim			,11);

	GDEFINE_ENUM_TYPE (UI_LOAD, uint8_t	);
	GDEFINE_ENUM_TYPE (UI_DESKTOP, uint8_t);

	GDEFINE_ENUM_TYPE (UI_SETTINGS, uint8_t);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Back		, 0);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Game		, 1);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Graphics	, 2);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Audio		, 3);

	GDEFINE_ENUM_TYPE (UI_SHOP, uint8_t);
	GDEFINE_ENUM_VALUE(UI_SHOP, Back			, 0);

	GDEFINE_ENUM_TYPE (UI_WELCOME, uint8_t);
	GDEFINE_ENUM_VALUE(UI_WELCOME, Confirm		, 0);

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

	struct SUIRadialGauge {
		::gpk::apod<::gpk::SCoord3<float>>				Vertices				= {};
		::gpk::apod<::gpk::STriangle<int16_t>>			Indices					= {};
		int16_t											CurrentValue			= 0;
		int16_t											MaxValue				= 64;
		::gpk::img<::gpk::SColorBGRA>					Image					= {};

		static constexpr const ::gpk::SColorFloat		COLOR_MIN_DEFAULT		= ::gpk::GREEN;
		static constexpr const ::gpk::SColorFloat		COLOR_MID_DEFAULT		= ::gpk::ORANGE;
		static constexpr const ::gpk::SColorFloat		COLOR_MAX_DEFAULT		= ::gpk::RED;
		static constexpr const ::gpk::SColorFloat		COLOR_BKG_DEFAULT		= ::gpk::BROWN * .5 + ::gpk::DARKBLUE * .5;
		
		inline	int16_t									SetValue				(float weight)							noexcept	{ return CurrentValue = int16_t(weight * MaxValue); }
				int16_t									SetValue				(float currentValue, uint32_t maxValue)	noexcept	{ return SetValue(currentValue / maxValue); }
	};

			::gpk::error_t			gaugeBuildRadial		(::ghg::SUIRadialGauge & gauge, const ::gpk::SCircle<float> & gaugeMetrics, int16_t resolution, int16_t width);
			::gpk::error_t			gaugeImageUpdate		(::ghg::SUIRadialGauge & gauge, ::gpk::view2d<::gpk::SColorBGRA> target	, ::gpk::SColorFloat colorMin = ::ghg::SUIRadialGauge::COLOR_MIN_DEFAULT, ::gpk::SColorFloat colorMid = ::ghg::SUIRadialGauge::COLOR_MID_DEFAULT, ::gpk::SColorFloat colorMax = ::ghg::SUIRadialGauge::COLOR_MAX_DEFAULT, ::gpk::SColorBGRA colorEmpty = ::ghg::SUIRadialGauge::COLOR_BKG_DEFAULT, bool radialColor = false);
	inline	::gpk::error_t			gaugeImageUpdate		(::ghg::SUIRadialGauge & gauge, ::gpk::SColorFloat colorMin = ::ghg::SUIRadialGauge::COLOR_MIN_DEFAULT, ::gpk::SColorFloat colorMid = ::ghg::SUIRadialGauge::COLOR_MID_DEFAULT, ::gpk::SColorFloat colorMax = ::ghg::SUIRadialGauge::COLOR_MAX_DEFAULT, ::gpk::SColorBGRA colorEmpty = ::ghg::SUIRadialGauge::COLOR_BKG_DEFAULT, bool radialColor = false) {
		//gauge.Image.Texels.fill({0, 0, 0, 0});
		return ::ghg::gaugeImageUpdate(gauge, gauge.Image, colorMin, colorMid, colorMax, colorEmpty, radialColor);
	}

	struct SUIPlayShipPartViewport {
		int32_t											Viewport		;
		::gpk::SCamera									Camera			;
		SUIRadialGauge									GaugeLife		;
		SUIRadialGauge									GaugeCooldown	;
		SUIRadialGauge									GaugeDelay		;
		::gpk::SMatrix4	<float>							MatrixProjection;
		::ghg::TRenderTarget							RenderTargetOrbiter	;
		::ghg::TRenderTarget							RenderTargetWeaponLoad	;
		::ghg::TRenderTarget							RenderTargetWeaponType	;
	};


	GDEFINE_ENUM_TYPE (SHIP_CONTROLLER, uint16_t);
	GDEFINE_ENUM_VALUE(SHIP_CONTROLLER, Forward	, 0x01);
	GDEFINE_ENUM_VALUE(SHIP_CONTROLLER, Back	, 0x02);
	GDEFINE_ENUM_VALUE(SHIP_CONTROLLER, Left	, 0x03);
	GDEFINE_ENUM_VALUE(SHIP_CONTROLLER, Right	, 0x04);
	GDEFINE_ENUM_VALUE(SHIP_CONTROLLER, Turbo	, 0x05);

	struct SUIPlayer {
		::gpk::apobj<::ghg::SUIPlayShipPartViewport>	ModuleViewports			= {};
		::gpk::SDialog									DialogPlay				= {};
		::gpk::SDialog									DialogHome				= {};

		::gpk::SUIInputBox								InputBox				= {};
	
		::gpk::array_static<char, 128>					TextScore				= {};
		::gpk::array_static<char, 128>					TextHits				= {};
		::gpk::array_static<char, 128>					TextShots				= {};
		::gpk::array_static<char, 128>					TextBullets				= {};
		::gpk::array_static<char, 128>					TextDamageDone			= {};
		::gpk::array_static<char, 128>					TextDamageReceived		= {};
		::gpk::array_static<char, 128>					TextHitsSurvived		= {};
		::gpk::array_static<char, 128>					TextOrbitersLost		= {};
		::gpk::array_static<char, 128>					TextKilledShips			= {};
		::gpk::array_static<char, 128>					TextKilledOrbiters		= {};
	};

	struct SUIPlay {
		::gpk::aobj<::ghg::SUIPlayer>			PlayerUI				= {};
		::ghg::SGalaxyHellDrawCache				DrawCache				= {};

		::gpk::array_static<char, 128>			TextLevel				= {};
		::gpk::array_static<char, 128>			TextTimeStage			= {};
		::gpk::array_static<char, 128>			TextTimeWorld			= {};
		::gpk::array_static<char, 128>			TextTimeReal			= {};
	};


	struct SPlayerShip { 
		::ghg::SShipCore						Core;
		::ghg::SShipScore						Score;
		::gpk::apod<uint32_t>					Parts;
		::gpk::apod<uint32_t>					Weapons;

		::gpk::error_t							Save			(::gpk::apod<byte_t> & output) const {
			gpk_necs(::gpk::viewWrite(::gpk::view_array<const ::ghg::SShipCore >{&Core , 1}, output));
			gpk_necs(::gpk::viewWrite(::gpk::view_array<const ::ghg::SShipScore>{&Score, 1}, output));
			gpk_necs(::gpk::viewWrite(Parts		, output));
			gpk_necs(::gpk::viewWrite(Weapons	, output));
			return 0;
		}

		::gpk::error_t							Load			(::gpk::vcc & input) {
			gpk_necs(::gpk::loadPOD	(input, Core	));
			gpk_necs(::gpk::loadPOD	(input, Score	));
			gpk_necs(::gpk::loadView(input, Parts	));
			gpk_necs(::gpk::loadView(input, Weapons	));
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
		::gpk::apod<::ghg::SWeapon>				Weapons			= {};
		::gpk::apod<::ghg::SOrbiter>			Orbiters		= {};
		::gpk::aobj<::ghg::SPlayerShip>			Ships			= {};

		::gpk::error_t							Save			(const ::gpk::vcc & filename) const {
			::gpk::apod<byte_t>							serialized;
			gpk_necs(Save(serialized));
			::gpk::apod<byte_t>							deflated;
			gpk_necs(::gpk::arrayDeflate(serialized, deflated));
			info_printf("Player size in bytes: %u.", serialized.size());
			info_printf("Player file size: %u.", deflated.size());
			gpk_necall(::gpk::fileFromMemory(filename, deflated), "fileName: %s", ::gpk::toString(filename).begin());
			return 0;
		}

		::gpk::error_t							Load			(const ::gpk::vcc & filename) {
			::gpk::apod<byte_t>							serialized;
			gpk_necall(::gpk::fileToMemory(filename, serialized), "fileName: %s", ::gpk::toString(filename).begin());
			::gpk::apod<byte_t>							inflated;
			gpk_necs(::gpk::arrayInflate(serialized, inflated));
			info_printf("Player file size: %u.", inflated.size());
			info_printf("Player size in bytes: %u.", serialized.size());
			::gpk::vcc									input		= inflated;
			return Load(input);
		}

		::gpk::error_t							Save			(::gpk::apod<byte_t> & output) const {
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

		::gpk::error_t							Load			(::gpk::vcc & input) {
			gpk_necs(::gpk::loadLabel	(input, Name	));
			gpk_necs(::gpk::loadPOD		(input, State	));
			gpk_necs(::gpk::loadView	(input, Weapons	));
			gpk_necs(::gpk::loadView	(input, Orbiters));
			gpk_necs(Ships.resize(*(uint32_t*)input.begin()));
			input											= {input.begin() + sizeof(uint32_t), input.size() - 4};
			for(uint32_t iShip = 0; iShip < Ships.size(); ++iShip) {
				gpk_necall(Ships[iShip].Load(input), "iShip: %u", iShip);
			}
			return 0;
		}
	};

	struct SPlayerManager {

	};

	struct SUserCredentials {
		::gpk::vcc								Username;
		::gpk::vcc								Password;
	};

	enum SAVE_MODE { SAVE_MODE_AUTO, SAVE_MODE_QUICK, SAVE_MODE_STAGE, SAVE_MODE_USER };

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

	struct SGalaxyHellApp {
		::gpk::array_pobj<TRenderTarget>						RenderTargetPool			= {};
		::gpk::array_pobj<TRenderTarget>						RenderTargetQueue			= {};
		::std::mutex											RenderTargetLockPool		= {};
		::std::mutex											RenderTargetLockQueue		= {};
		volatile uint64_t										CurrentRenderTarget			= 0;
		int32_t													PlayerSelected				= 0;

		bool													Exit						= false;
		::gpk::astatic<::gpk::SDialog, ::ghg::APP_STATE_COUNT>	DialogPerState				= {};

		::gpk::SVirtualKeyboard									VirtualKeyboard				= {};
		::ghg::SUIPlay											UIPlay;

		::gpk::SDialog											DialogDesktop				= {};
		::gpk::aobj<::ghg::SPlayer>								Players						= {};
		::gpk::pnco<::gpk::SDialogTuner>						TunerPlayerCount;

		::ghg::SGalaxyHell										Game;
		::gpk::aobj<::ghg::SUserCredentials>					UserCredentials				= {};

		::gpk::pobj<::gpk::SDialogViewport>						Inputbox;
		::gpk::apod<char>										InputboxText;

		::gpk::aobj<::gpk::apod<char>>							FileNames					= {};
		::gpk::vcs												SavegameFolder				= ".";
		::gpk::vcs												ExtensionSaveAuto			= ".auto.ghz";
		::gpk::vcs												ExtensionSaveStage			= ".stage.ghz";
		::gpk::vcs												ExtensionSaveQuick			= ".quick.ghz";
		::gpk::vcs												ExtensionSaveUser			= ".user.ghz";
		::gpk::vcs												ExtensionSave				= ".ghz";
		::gpk::vcs												ExtensionImages				= ".png";

		::gpk::vcs												ExtensionProfile			= ".ghp";
		::gpk::vcs												ExtensionCredentials		= ".ghc";

		::ghg::APP_STATE										ActiveState					= APP_STATE_Init;

		::gpk::error_t											AddNewPlayer				(::gpk::vcc playerName)			{
			::gpk::trim(playerName);
			return Players.push_back({::gpk::label(::gpk::vcs{playerName})});
		}

		::gpk::error_t											Save						(SAVE_MODE autosaveMode)				{
			char														fileName[4096]				= {};
			::gpk::apod<char>											b64PlayerName				= {};
			for(uint32_t iPlayer = 0; iPlayer < Players.size(); ++iPlayer) {
				b64PlayerName.clear();
				const ::ghg::SPlayer										& player					= Players[iPlayer];
				::gpk::base64Encode(player.Name, b64PlayerName);
				sprintf_s(fileName, "%s/%s.%llu%s", SavegameFolder.begin(), b64PlayerName.begin(), 0ULL, ExtensionProfile.begin());
				gpk_necall(player.Save(fileName), "iPlayer: %i", iPlayer);
			}
			::gpk::vcc													extension;
			switch(autosaveMode) {
			case SAVE_MODE_USER		: extension = ExtensionSaveUser		; break;
			case SAVE_MODE_STAGE	: extension = ExtensionSaveStage	; break;
			case SAVE_MODE_QUICK	: extension = ExtensionSaveQuick	; break;
			case SAVE_MODE_AUTO		: extension = ExtensionSaveAuto		; break;
			}

			const uint64_t												timeCurrent					= (SAVE_MODE_USER != autosaveMode) ? 0 : ::gpk::timeCurrent();
			const uint64_t												timeStart					= (SAVE_MODE_AUTO == autosaveMode) ? 0 : Game.PlayState.TimeStart;
			sprintf_s(fileName, "%s/%llu.%llu%s", SavegameFolder.begin(), timeStart, timeCurrent, extension.begin());

			const int32_t												totalHealth					= Game.ShipState.GetTeamHealth(0);
			if(totalHealth > 0) // Save only if a player is alive
				return ::ghg::solarSystemSave(Game, fileName);
			return 0;
		}
	};

	::gpk::error_t											guiSetup					(::ghg::SGalaxyHellApp & gameui, const ::gpk::pobj<::gpk::SInput> & inputState);
	::gpk::error_t											guiUpdate					(::ghg::SGalaxyHellApp & gameui, const ::gpk::view_array<::gpk::SSysEvent> & sysEvents);
	
	::gpk::error_t											galaxyHellUpdate			(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, const ::gpk::view_array<::gpk::SSysEvent> & systemEvents);
	::gpk::error_t											galaxyHellDraw				(::ghg::SGalaxyHellApp & app, ::gpk::SCoord2<uint16_t> renderTargetSize);

	::gpk::error_t											listFilesSavegame			(::ghg::SGalaxyHellApp & app, const ::gpk::vcc & saveGameFolder, ::gpk::aobj<::gpk::vcc> & savegameFilenames);
	::gpk::error_t											listFilesProfile			(::ghg::SGalaxyHellApp & app, const ::gpk::vcc & saveGameFolder, ::gpk::aobj<::gpk::vcc> & savegameFilenames);

}
