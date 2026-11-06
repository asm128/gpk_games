#include "gpk_galaxy_hell.h"
#include "gpk_dialog_controls.h"
#include "gpk_gui_control_list.h"
#include "gpk_deflate.h"
#include "gpk_file.h"
#include "gpk_circle.h"
#include "gpk_gui_inputbox.h"
#include "gpk_chrono.h"

namespace ghg
{

	GDEFINE_ENUM_TYPE (UI_HOME, u0_t);
	GDEFINE_ENUM_VALUE(UI_HOME, Continue		, 0);
	GDEFINE_ENUM_VALUE(UI_HOME, Shop			, 1);
	GDEFINE_ENUM_VALUE(UI_HOME, Save			, 2);
	GDEFINE_ENUM_VALUE(UI_HOME, Load			, 3);
	GDEFINE_ENUM_VALUE(UI_HOME, Start			, 4);
	GDEFINE_ENUM_VALUE(UI_HOME, Leaderboards	, 5);
	GDEFINE_ENUM_VALUE(UI_HOME, Credits			, 6);
	GDEFINE_ENUM_VALUE(UI_HOME, Settings		, 7);
	GDEFINE_ENUM_VALUE(UI_HOME, Exit			, 8);
	
	GDEFINE_ENUM_TYPE (UI_PROFILE, u0_t);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Name				, 0);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Score			, 1);

	GDEFINE_ENUM_TYPE (UI_PLAY, u0_t);
	GDEFINE_ENUM_VALUE(UI_PLAY, Menu			, 0);
	GDEFINE_ENUM_VALUE(UI_PLAY, Level			, 1);
	GDEFINE_ENUM_VALUE(UI_PLAY, TimeReal		, 2);
	GDEFINE_ENUM_VALUE(UI_PLAY, TimeStage		, 3);
	
	GDEFINE_ENUM_TYPE (UI_PILOT, u0_t);
	GDEFINE_ENUM_VALUE(UI_PILOT, Name				, 0);
	GDEFINE_ENUM_VALUE(UI_PILOT, Score				, 1);
	
	GDEFINE_ENUM_TYPE (UI_PLAYER_SCORE, u0_t);
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

	GDEFINE_ENUM_TYPE (UI_LOAD, u0_t	);
	GDEFINE_ENUM_TYPE (UI_DESKTOP, u0_t);

	GDEFINE_ENUM_TYPE (UI_SETTINGS, u0_t);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Back		, 0);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Game		, 1);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Graphics	, 2);
	GDEFINE_ENUM_VALUE(UI_SETTINGS, Audio		, 3);

	GDEFINE_ENUM_TYPE (UI_SHOP, u0_t);
	GDEFINE_ENUM_VALUE(UI_SHOP, Back			, 0);

	GDEFINE_ENUM_TYPE (UI_WELCOME, u0_t);
	GDEFINE_ENUM_VALUE(UI_WELCOME, Confirm		, 0);

	GDEFINE_ENUM_TYPE (UI_CREDITS, u0_t);
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

	GDEFINE_ENUM_TYPE(UI_BRIEF, u0_t);
	GDEFINE_ENUM_TYPE(UI_STAGE, u0_t);
	GDEFINE_ENUM_TYPE(UI_STATS, u0_t);
	GDEFINE_ENUM_TYPE(UI_STORE, u0_t);
	GDEFINE_ENUM_TYPE(UI_SCORE, u0_t);

	struct SUIRadialGauge {
		::gpk::apod<::gpk::n3f2_t>		Vertices				= {};
		::gpk::apod<::gpk::tris1_t>		Indices					= {};
		s1_t							CurrentValue			= 0;
		s1_t							MaxValue				= 64;
		::gpk::img<::gpk::bgra>			Image					= {};

		stxp	::gpk::rgbaf			COLOR_MIN_DEFAULT		= ::gpk::GREEN;
		stxp	::gpk::rgbaf			COLOR_MID_DEFAULT		= ::gpk::ORANGE;
		stxp	::gpk::rgbaf			COLOR_MAX_DEFAULT		= ::gpk::RED;
		stxp	::gpk::rgbaf			COLOR_BKG_DEFAULT		= ::gpk::BROWN * .5 + ::gpk::DARKBLUE * .5;
		
		inline	s1_t					SetValue				(float weight)							noexcept	{ return CurrentValue = s1_t(weight * MaxValue); }
				s1_t					SetValue				(float currentValue, u2_t maxValue)	noexcept	{ return SetValue(currentValue / maxValue); }
	};

			gpk::err_t			gaugeBuildRadial		(::ghg::SUIRadialGauge & gauge, const ::gpk::circlef2_t & gaugeMetrics, s1_t resolution, s1_t width);
			gpk::err_t			gaugeImageUpdate		(::ghg::SUIRadialGauge & gauge, ::gpk::g8bgra target, ::gpk::rgbaf colorMin = ::ghg::SUIRadialGauge::COLOR_MIN_DEFAULT, ::gpk::rgbaf colorMid = ::ghg::SUIRadialGauge::COLOR_MID_DEFAULT, ::gpk::rgbaf colorMax = ::ghg::SUIRadialGauge::COLOR_MAX_DEFAULT, ::gpk::bgra colorEmpty = ::ghg::SUIRadialGauge::COLOR_BKG_DEFAULT, bool radialColor = false);
	inline	gpk::err_t			gaugeImageUpdate		(::ghg::SUIRadialGauge & gauge, ::gpk::rgbaf colorMin = ::ghg::SUIRadialGauge::COLOR_MIN_DEFAULT, ::gpk::rgbaf colorMid = ::ghg::SUIRadialGauge::COLOR_MID_DEFAULT, ::gpk::rgbaf colorMax = ::ghg::SUIRadialGauge::COLOR_MAX_DEFAULT, ::gpk::bgra colorEmpty = ::ghg::SUIRadialGauge::COLOR_BKG_DEFAULT, bool radialColor = false) {
		//gauge.Image.Texels.fill({0, 0, 0, 0});
		return ::ghg::gaugeImageUpdate(gauge, gauge.Image, colorMin, colorMid, colorMax, colorEmpty, radialColor);
	}

	struct SUIPlayShipPartViewport {
		::gpk::cid_t					Viewport		;
		::gpk::SCameraPoints			Camera			;
		SUIRadialGauge					GaugeLife		;
		SUIRadialGauge					GaugeCooldown	;
		SUIRadialGauge					GaugeDelay		;
		::gpk::m4f2_t					MatrixProjection;
		::ghg::TRenderTarget			RenderTargetOrbiter	;
		::ghg::TRenderTarget			RenderTargetWeaponLoad	;
		::ghg::TRenderTarget			RenderTargetWeaponType	;
	};


	GDEFINE_ENUM_TYPE (SHIP_CONTROLLER, u1_t);
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
	
		::gpk::astsc_t<128>	TextScore				= {};
		::gpk::astsc_t<128>	TextHits				= {};
		::gpk::astsc_t<128>	TextShots				= {};
		::gpk::astsc_t<128>	TextBullets				= {};
		::gpk::astsc_t<128>	TextDamageDone			= {};
		::gpk::astsc_t<128>	TextDamageReceived		= {};
		::gpk::astsc_t<128>	TextHitsSurvived		= {};
		::gpk::astsc_t<128>	TextOrbitersLost		= {};
		::gpk::astsc_t<128>	TextKilledShips			= {};
		::gpk::astsc_t<128>	TextKilledOrbiters		= {};
	};

	struct SUIPlay {
		::gpk::aobj<::ghg::SUIPlayer>	PlayerUI				= {};
		::ghg::SGalaxyHellDrawCache		DrawCache				= {};

		::gpk::astatic<sc_t, 128>		TextLevel				= {};
		::gpk::astatic<sc_t, 128>		TextTimeStage			= {};
		::gpk::astatic<sc_t, 128>		TextTimeWorld			= {};
		::gpk::astatic<sc_t, 128>		TextTimeReal			= {};
	};


	struct SPlayerShip { 
		::gpk::SSpaceshipCore			Core;
		::gpk::SSpaceshipScore			Score;
		::gpk::au2_t						Parts;
		::gpk::au2_t						Weapons;

		gpk::err_t					Save					(::gpk::au0_t & output) const {
			if_fail_fe(::gpk::savePOD (output, Core	));
			if_fail_fe(::gpk::savePOD (output, Score	));
			if_fail_fe(::gpk::saveView(output, Parts	));
			if_fail_fe(::gpk::saveView(output, Weapons));
			return 0;
		}

		gpk::err_t					Load					(::gpk::vcu0_t & input) {
			if_fail_fe(::gpk::loadPOD	(input, Core	));
			if_fail_fe(::gpk::loadPOD	(input, Score	));
			if_fail_fe(::gpk::loadView(input, Parts	));
			if_fail_fe(::gpk::loadView(input, Weapons	));
			return 0;
		}
	};


#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (PLAYER_TYPE, u0_t);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, Human	, 0);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, AI		, 1);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, Network	, 2);

	struct SPlayerScore { 
		u3_t			TimePlayed;
		u3_t			TimeDead;
		u3_t			KeyPresses;
	};

	struct SPlayerState {
		PLAYER_TYPE			Type;
		::gpk::bgra			Color;
		s2_t				ShipSelected;
		::ghg::SPlayerScore	Score;
	};
#pragma pack(pop)

	struct SPlayer {
		::gpk::vcst_t							Name			= {};
		::ghg::SPlayerState						State			= {};
		::gpk::apod<::gpk::SWeapon>				Weapons			= {};
		::gpk::apod<::gpk::SSpaceshipOrbiter>	Orbiters		= {};
		::gpk::aobj<::ghg::SPlayerShip>			Ships			= {};

		gpk::err_t					Save			(const ::gpk::vcsc_t & filename) const {
			::gpk::au0_t					serialized;
			if_fail_fe(Save(serialized));
			::gpk::au0_t							deflated;
			if_fail_fe(::gpk::arrayDeflate(serialized, deflated));
			info_printf("Player size in bytes: %u.", serialized.size());
			info_printf("Player file size: %u.", deflated.size());
			if_fail_fef(::gpk::fileFromMemory({filename}, deflated), "fileName: %.*s", GPK_VIEW_ARGS(filename));
			return 0;
		}

		gpk::err_t					Load			(const ::gpk::vcsc_t & filename) {
			::gpk::au0_t							serialized;
			if_fail_fef(::gpk::fileToMemory({filename}, serialized), "fileName: %.*s", GPK_VIEW_ARGS(filename));
			::gpk::au0_t							inflated;
			if_fail_fe(::gpk::arrayInflate(serialized, inflated));
			info_printf("Player file size: %u.", inflated.size());
			info_printf("Player size in bytes: %u.", serialized.size());
			::gpk::vcu0_t							input		= inflated;
			return Load(input);
		}

		gpk::err_t					Save			(::gpk::au0_t & output) const {
			if_fail_fe(::gpk::saveView(output, Name));
			if_fail_fe(::gpk::savePOD(output, State));
			if_fail_fe(::gpk::saveView(output, Weapons));
			if_fail_fe(::gpk::saveView(output, Orbiters));
			if_fail_fe(output.append(::gpk::vcu0_t{(const u0_t*)&Ships.size(), 4}));
			for(u2_t iShip = 0; iShip < Ships.size(); ++iShip) {
				if_fail_fef(Ships[iShip].Save(output), "iShip: %u", iShip);
			}
			return 0;
		}

		gpk::err_t					Load			(::gpk::vcu0_t & input) {
			if_fail_fe(::gpk::loadLabel	(input, Name	));
			if_fail_fe(::gpk::loadPOD		(input, State	));
			if_fail_fe(::gpk::loadView	(input, Weapons	));
			if_fail_fe(::gpk::loadView	(input, Orbiters));
			if_fail_fe(Ships.resize(*(u2_t*)input.begin()));
			input							= {input.begin() + sizeof(u2_t), input.size() - 4};
			for(u2_t iShip = 0; iShip < Ships.size(); ++iShip) {
				if_fail_fef(Ships[iShip].Load(input), "iShip: %u", iShip);
			}
			return 0;
		}
	};

	struct SPlayerManager {

	};

	struct SUserCredentials {
		::gpk::vcsc_t		Username;
		::gpk::vcsc_t		Password;
	};

	enum SAVE_MODE { SAVE_MODE_AUTO, SAVE_MODE_QUICK, SAVE_MODE_STAGE, SAVE_MODE_USER };

	GDEFINE_ENUM_TYPE(APP_STATE, u0_t);
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
		vltl	u3_t					CurrentRenderTarget			= 0;
		s2_t								PlayerSelected				= 0;

		bool								Exit						= false;
		::gpk::astatic<::gpk::SDialog, ::ghg::APP_STATE_COUNT>	
											DialogPerState				= {};

		::gpk::astatic<::gpk::acid, APP_STATE_COUNT>	
											DialogControls				= {}; 	

		::gpk::SVirtualKeyboard				VirtualKeyboard				= {};
		::ghg::SUIPlay						UIPlay;

		::gpk::SDialog						DialogDesktop				= {};
		::gpk::aobj<::ghg::SPlayer>			Players						= {};
		::gpk::pnco<::gpk::SDialogTuner<u0_t>>	TunerPlayerCount;
		::gpk::acid							DesktopControls			= {}; 	

		::ghg::SGalaxyHell						Game;
		::gpk::aobj<::ghg::SUserCredentials>	UserCredentials				= {};

		::gpk::pobj<::gpk::SDialogViewport>	Inputbox;
		::gpk::apod<sc_t>					InputboxText;

		::gpk::aobj<::gpk::apod<sc_t>>	FileNames					= {};
		::gpk::vcst_t						SavegameFolder				= ".";
		::gpk::vcst_t						ExtensionSaveAuto			= ".auto.ghz";
		::gpk::vcst_t						ExtensionSaveStage			= ".stage.ghz";
		::gpk::vcst_t						ExtensionSaveQuick			= ".quick.ghz";
		::gpk::vcst_t						ExtensionSaveUser			= ".user.ghz";
		::gpk::vcst_t						ExtensionSave				= ".ghz";
		::gpk::vcst_t						ExtensionImages				= ".png";

		::gpk::vcst_t						ExtensionProfile			= ".ghp";
		::gpk::vcst_t						ExtensionCredentials		= ".ghc";

		::ghg::APP_STATE				ActiveState					= APP_STATE_Init;

		gpk::err_t					AddNewPlayer				(::gpk::vcsc_t playerName)			{
			::gpk::trim(playerName);
			return Players.push_back({::gpk::label(::gpk::vcst_t{playerName})});
		}

		gpk::err_t					Save						(SAVE_MODE autosaveMode)				{
			sc_t								fileName[4096]				= {};
			::gpk::au0_t							b64PlayerName				= {};
			for(u2_t iPlayer = 0; iPlayer < Players.size(); ++iPlayer) {
				b64PlayerName.clear();
				const ::ghg::SPlayer				& player					= Players[iPlayer];
				::gpk::base64Encode(player.Name, b64PlayerName);
				::sprintf_s(fileName, "%s/%s.%llu%s", SavegameFolder.begin(), b64PlayerName.begin(), 0ULL, ExtensionProfile.begin());
				if_fail_fef(player.Save(fileName), "iPlayer: %i", iPlayer);
			}
			::gpk::vcsc_t							extension;
			switch(autosaveMode) {
			case SAVE_MODE_USER		: extension = ExtensionSaveUser		; break;
			case SAVE_MODE_STAGE	: extension = ExtensionSaveStage	; break;
			case SAVE_MODE_QUICK	: extension = ExtensionSaveQuick	; break;
			case SAVE_MODE_AUTO		: extension = ExtensionSaveAuto		; break;
			}

			const u3_t						timeCurrent					= (SAVE_MODE_USER != autosaveMode) ? 0 : ::gpk::timeCurrent();
			const u3_t						timeStart					= (SAVE_MODE_AUTO == autosaveMode) ? 0 : Game.PlayState.GlobalState.UserTime.Started;
			::sprintf_s(fileName, "%s/%llu.%llu%s", SavegameFolder.begin(), timeStart, timeCurrent, extension.begin());

			const s2_t						totalHealth					= Game.ShipState.SpaceshipManager.GetTeamHealth(0);
			if(totalHealth > 0) // Save only if a player is alive
				return ::ghg::solarSystemSave(Game, fileName);
			return 0;
		}
	};

	gpk::err_t	guiSetup			(::ghg::SGalaxyHellApp & gameui, const ::gpk::pobj<::gpk::SInput> & inputState);
	gpk::err_t	guiUpdate			(::ghg::SGalaxyHellApp & gameui, ::gpk::vpobj<::gpk::SEventSystem> sysEvents);
	
	gpk::err_t	galaxyHellUpdate	(::ghg::SGalaxyHellApp & app, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEventsNew);
	gpk::err_t	galaxyHellDraw		(::ghg::SGalaxyHellApp & app, ::gpk::n2u1_t renderTargetSize);

	gpk::err_t	listFilesSavegame	(::ghg::SGalaxyHellApp & app, const ::gpk::vcsc_t & saveGameFolder, ::gpk::aobj<::gpk::vcst_t> & savegameFilenames);
	gpk::err_t	listFilesProfile	(::ghg::SGalaxyHellApp & app, const ::gpk::vcsc_t & saveGameFolder, ::gpk::aobj<::gpk::vcst_t> & savegameFilenames);

}
