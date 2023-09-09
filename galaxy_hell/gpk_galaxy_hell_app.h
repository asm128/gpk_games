#include "gpk_galaxy_hell.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_control_list.h"
#include "gpk_deflate.h"
#include "gpk_file.h"
#include "gpk_circle.h"
#include "gpk_gui_inputbox.h"

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
		::gpk::apod<::gpk::n3f32>		Vertices				= {};
		::gpk::apod<::gpk::trii16>		Indices					= {};
		int16_t							CurrentValue			= 0;
		int16_t							MaxValue				= 64;
		::gpk::img<::gpk::bgra>			Image					= {};

		stacxpr	::gpk::rgbaf			COLOR_MIN_DEFAULT		= ::gpk::GREEN;
		stacxpr	::gpk::rgbaf			COLOR_MID_DEFAULT		= ::gpk::ORANGE;
		stacxpr	::gpk::rgbaf			COLOR_MAX_DEFAULT		= ::gpk::RED;
		stacxpr	::gpk::rgbaf			COLOR_BKG_DEFAULT		= ::gpk::BROWN * .5 + ::gpk::DARKBLUE * .5;
		
		inline	int16_t					SetValue				(float weight)							noexcept	{ return CurrentValue = int16_t(weight * MaxValue); }
				int16_t					SetValue				(float currentValue, uint32_t maxValue)	noexcept	{ return SetValue(currentValue / maxValue); }
	};

			::gpk::error_t			gaugeBuildRadial		(::ghg::SUIRadialGauge & gauge, const ::gpk::circlef32 & gaugeMetrics, int16_t resolution, int16_t width);
			::gpk::error_t			gaugeImageUpdate		(::ghg::SUIRadialGauge & gauge, ::gpk::g8bgra target, ::gpk::rgbaf colorMin = ::ghg::SUIRadialGauge::COLOR_MIN_DEFAULT, ::gpk::rgbaf colorMid = ::ghg::SUIRadialGauge::COLOR_MID_DEFAULT, ::gpk::rgbaf colorMax = ::ghg::SUIRadialGauge::COLOR_MAX_DEFAULT, ::gpk::bgra colorEmpty = ::ghg::SUIRadialGauge::COLOR_BKG_DEFAULT, bool radialColor = false);
	inline	::gpk::error_t			gaugeImageUpdate		(::ghg::SUIRadialGauge & gauge, ::gpk::rgbaf colorMin = ::ghg::SUIRadialGauge::COLOR_MIN_DEFAULT, ::gpk::rgbaf colorMid = ::ghg::SUIRadialGauge::COLOR_MID_DEFAULT, ::gpk::rgbaf colorMax = ::ghg::SUIRadialGauge::COLOR_MAX_DEFAULT, ::gpk::bgra colorEmpty = ::ghg::SUIRadialGauge::COLOR_BKG_DEFAULT, bool radialColor = false) {
		//gauge.Image.Texels.fill({0, 0, 0, 0});
		return ::ghg::gaugeImageUpdate(gauge, gauge.Image, colorMin, colorMid, colorMax, colorEmpty, radialColor);
	}

	struct SUIPlayShipPartViewport {
		::gpk::cid_t					Viewport		;
		::gpk::SCameraPoints			Camera			;
		SUIRadialGauge					GaugeLife		;
		SUIRadialGauge					GaugeCooldown	;
		SUIRadialGauge					GaugeDelay		;
		::gpk::m4f32					MatrixProjection;
		::ghg::TRenderTarget			RenderTargetOrbiter	;
		::ghg::TRenderTarget			RenderTargetWeaponLoad	;
		::ghg::TRenderTarget			RenderTargetWeaponType	;
	};


	GDEFINE_ENUM_TYPE (SHIP_CONTROLLER, uint16_t);
	GDEFINE_ENUM_VALUE(SHIP_CONTROLLER, Forward	, 0x01);
	GDEFINE_ENUM_VALUE(SHIP_CONTROLLER, Back	, 0x02);
	GDEFINE_ENUM_VALUE(SHIP_CONTROLLER, Left	, 0x03);
	GDEFINE_ENUM_VALUE(SHIP_CONTROLLER, Right	, 0x04);
	GDEFINE_ENUM_VALUE(SHIP_CONTROLLER, Turbo	, 0x05);

	struct SUIPlayer {
		::gpk::apobj<::ghg::SUIPlayShipPartViewport>	
								ModuleViewports			= {};
		::gpk::SDialog			DialogPlay				= {};
		::gpk::SDialog			DialogHome				= {};

		::gpk::acid				ControlsDialogPlay		= {};
		::gpk::acid				ControlsDialogHome		= {};

		::gpk::SInputBox		InputBox				= {};
	
		::gpk::astc<128>		TextScore				= {};
		::gpk::astc<128>		TextHits				= {};
		::gpk::astc<128>		TextShots				= {};
		::gpk::astc<128>		TextBullets				= {};
		::gpk::astc<128>		TextDamageDone			= {};
		::gpk::astc<128>		TextDamageReceived		= {};
		::gpk::astc<128>		TextHitsSurvived		= {};
		::gpk::astc<128>		TextOrbitersLost		= {};
		::gpk::astc<128>		TextKilledShips			= {};
		::gpk::astc<128>		TextKilledOrbiters		= {};
	};

	struct SUIPlay {
		::gpk::aobj<::ghg::SUIPlayer>	PlayerUI				= {};
		::ghg::SGalaxyHellDrawCache		DrawCache				= {};

		::gpk::astatic<char, 128>		TextLevel				= {};
		::gpk::astatic<char, 128>		TextTimeStage			= {};
		::gpk::astatic<char, 128>		TextTimeWorld			= {};
		::gpk::astatic<char, 128>		TextTimeReal			= {};
	};


	struct SPlayerShip { 
		::gpk::SSpaceshipCore			Core;
		::gpk::SSpaceshipScore			Score;
		::gpk::au32						Parts;
		::gpk::au32						Weapons;

		::gpk::error_t					Save					(::gpk::au8 & output) const {
			gpk_necs(::gpk::savePOD (output, Core	));
			gpk_necs(::gpk::savePOD (output, Score	));
			gpk_necs(::gpk::saveView(output, Parts	));
			gpk_necs(::gpk::saveView(output, Weapons));
			return 0;
		}

		::gpk::error_t					Load					(::gpk::vcu8 & input) {
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
		uint64_t			TimePlayed;
		uint64_t			TimeDead;
		uint64_t			KeyPresses;
	};

	struct SPlayerState {
		PLAYER_TYPE			Type;
		::gpk::bgra			Color;
		int32_t				ShipSelected;
		::ghg::SPlayerScore	Score;
	};
#pragma pack(pop)

	struct SPlayer {
		::gpk::vcc						Name			= {};
		::ghg::SPlayerState				State			= {};
		::gpk::apod<::gpk::SWeapon>		Weapons			= {};
		::gpk::apod<::gpk::SSpaceshipOrbiter>	Orbiters		= {};
		::gpk::aobj<::ghg::SPlayerShip>	Ships			= {};

		::gpk::error_t					Save			(const ::gpk::vcc & filename) const {
			::gpk::au8							serialized;
			gpk_necs(Save(serialized));
			::gpk::au8							deflated;
			gpk_necs(::gpk::arrayDeflate(serialized, deflated));
			info_printf("Player size in bytes: %u.", serialized.size());
			info_printf("Player file size: %u.", deflated.size());
			gpk_necall(::gpk::fileFromMemory({filename}, deflated), "fileName: %s", ::gpk::toString(filename).begin());
			return 0;
		}

		::gpk::error_t					Load			(const ::gpk::vcc & filename) {
			::gpk::au8							serialized;
			gpk_necall(::gpk::fileToMemory({filename}, serialized), "fileName: %s", ::gpk::toString(filename).begin());
			::gpk::au8							inflated;
			gpk_necs(::gpk::arrayInflate(serialized, inflated));
			info_printf("Player file size: %u.", inflated.size());
			info_printf("Player size in bytes: %u.", serialized.size());
			::gpk::vcu8							input		= inflated;
			return Load(input);
		}

		::gpk::error_t					Save			(::gpk::au8 & output) const {
			gpk_necs(::gpk::saveView(output, Name));
			gpk_necs(::gpk::savePOD(output, State));
			gpk_necs(::gpk::saveView(output, Weapons));
			gpk_necs(::gpk::saveView(output, Orbiters));
			gpk_necs(output.append(::gpk::vcu8{(const uint8_t*)&Ships.size(), 4}));
			for(uint32_t iShip = 0; iShip < Ships.size(); ++iShip) {
				gpk_necall(Ships[iShip].Save(output), "iShip: %u", iShip);
			}
			return 0;
		}

		::gpk::error_t					Load			(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadLabel	(input, Name	));
			gpk_necs(::gpk::loadPOD		(input, State	));
			gpk_necs(::gpk::loadView	(input, Weapons	));
			gpk_necs(::gpk::loadView	(input, Orbiters));
			gpk_necs(Ships.resize(*(uint32_t*)input.begin()));
			input							= {input.begin() + sizeof(uint32_t), input.size() - 4};
			for(uint32_t iShip = 0; iShip < Ships.size(); ++iShip) {
				gpk_necall(Ships[iShip].Load(input), "iShip: %u", iShip);
			}
			return 0;
		}
	};

	struct SPlayerManager {

	};

	struct SUserCredentials {
		::gpk::vcc		Username;
		::gpk::vcc		Password;
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
	GDEFINE_ENUM_VALUE(APP_STATE, Load		, 13);
	GDEFINE_ENUM_VALUE(APP_STATE, Quit		, 14);
	GDEFINE_ENUM_VALUE(APP_STATE, COUNT		, 15);

	struct SGalaxyHellApp {
		::gpk::apobj<TRenderTarget>			RenderTargetPool			= {};
		::gpk::apobj<TRenderTarget>			RenderTargetQueue			= {};
		::std::mutex						RenderTargetLockPool		= {};
		::std::mutex						RenderTargetLockQueue		= {};
		volatile uint64_t					CurrentRenderTarget			= 0;
		int32_t								PlayerSelected				= 0;

		bool								Exit						= false;
		::gpk::astatic<::gpk::SDialog, ::ghg::APP_STATE_COUNT>	
											DialogPerState				= {};

		::gpk::astatic<::gpk::acid, APP_STATE_COUNT>	
											DialogControls				= {}; 	

		::gpk::SVirtualKeyboard				VirtualKeyboard				= {};
		::ghg::SUIPlay						UIPlay;

		::gpk::SDialog						DialogDesktop				= {};
		::gpk::aobj<::ghg::SPlayer>			Players						= {};
		::gpk::pnco<::gpk::SDialogTuner<uint8_t>>	TunerPlayerCount;
		::gpk::acid							DesktopControls			= {}; 	

		::ghg::SGalaxyHell						Game;
		::gpk::aobj<::ghg::SUserCredentials>	UserCredentials				= {};

		::gpk::pobj<::gpk::SDialogViewport>	Inputbox;
		::gpk::apod<char>					InputboxText;

		::gpk::aobj<::gpk::apod<char>>	FileNames					= {};
		::gpk::vcs						SavegameFolder				= ".";
		::gpk::vcs						ExtensionSaveAuto			= ".auto.ghz";
		::gpk::vcs						ExtensionSaveStage			= ".stage.ghz";
		::gpk::vcs						ExtensionSaveQuick			= ".quick.ghz";
		::gpk::vcs						ExtensionSaveUser			= ".user.ghz";
		::gpk::vcs						ExtensionSave				= ".ghz";
		::gpk::vcs						ExtensionImages				= ".png";

		::gpk::vcs						ExtensionProfile			= ".ghp";
		::gpk::vcs						ExtensionCredentials		= ".ghc";

		::ghg::APP_STATE				ActiveState					= APP_STATE_Init;

		::gpk::error_t					AddNewPlayer				(::gpk::vcc playerName)			{
			::gpk::trim(playerName);
			return Players.push_back({::gpk::label(::gpk::vcs{playerName})});
		}

		::gpk::error_t					Save						(SAVE_MODE autosaveMode)				{
			char								fileName[4096]				= {};
			::gpk::au8							b64PlayerName				= {};
			for(uint32_t iPlayer = 0; iPlayer < Players.size(); ++iPlayer) {
				b64PlayerName.clear();
				const ::ghg::SPlayer				& player					= Players[iPlayer];
				::gpk::base64Encode(player.Name, b64PlayerName);
				sprintf_s(fileName, "%s/%s.%llu%s", SavegameFolder.begin(), b64PlayerName.begin(), 0ULL, ExtensionProfile.begin());
				gpk_necall(player.Save(fileName), "iPlayer: %i", iPlayer);
			}
			::gpk::vcc							extension;
			switch(autosaveMode) {
			case SAVE_MODE_USER		: extension = ExtensionSaveUser		; break;
			case SAVE_MODE_STAGE	: extension = ExtensionSaveStage	; break;
			case SAVE_MODE_QUICK	: extension = ExtensionSaveQuick	; break;
			case SAVE_MODE_AUTO		: extension = ExtensionSaveAuto		; break;
			}

			const uint64_t						timeCurrent					= (SAVE_MODE_USER != autosaveMode) ? 0 : ::gpk::timeCurrent();
			const uint64_t						timeStart					= (SAVE_MODE_AUTO == autosaveMode) ? 0 : Game.PlayState.GlobalState.UserTime.Started;
			sprintf_s(fileName, "%s/%llu.%llu%s", SavegameFolder.begin(), timeStart, timeCurrent, extension.begin());

			const int32_t						totalHealth					= Game.ShipState.SpaceshipManager.GetTeamHealth(0);
			if(totalHealth > 0) // Save only if a player is alive
				return ::ghg::solarSystemSave(Game, fileName);
			return 0;
		}
	};

	::gpk::error_t	guiSetup			(::ghg::SGalaxyHellApp & gameui, const ::gpk::pobj<::gpk::SInput> & inputState);
	::gpk::error_t	guiUpdate			(::ghg::SGalaxyHellApp & gameui, ::gpk::vpobj<::gpk::SEventSystem> sysEvents);
	
	::gpk::error_t	galaxyHellUpdate	(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEventsNew);
	::gpk::error_t	galaxyHellDraw		(::ghg::SGalaxyHellApp & app, ::gpk::n2u16 renderTargetSize);

	::gpk::error_t	listFilesSavegame	(::ghg::SGalaxyHellApp & app, const ::gpk::vcc & saveGameFolder, ::gpk::aobj<::gpk::vcc> & savegameFilenames);
	::gpk::error_t	listFilesProfile	(::ghg::SGalaxyHellApp & app, const ::gpk::vcc & saveGameFolder, ::gpk::aobj<::gpk::vcc> & savegameFilenames);

}
