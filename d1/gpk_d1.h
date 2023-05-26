#include "gpk_frameinfo.h"
#include "gpk_pool_game.h"

#include "gpk_gui_control.h"
#include "gpk_dialog_controls.h"
#include "gpk_deflate.h"
#include "gpk_file.h"


#ifndef GPK_D1_H_098273498237423
#define GPK_D1_H_098273498237423

namespace d1 
{
#pragma pack(push, 1)
	struct SD1Logo {
		::gpk::n3f							A[8]						= {};
		::gpk::n3f							B[12]						= {{-1, 0}, {-2, 0}, {-2, 8}, {-1, 9}, };
		::gpk::n3f							C[12]						= {};

		::gpk::n3f							D[8]						= {};
		::gpk::n3f							E[10]						= {};
		::gpk::n3f							F[12]						= {};
	};
#pragma pack(pop)

	struct SCamera {
		::gpk::n3f							Position					= {-1, 0, 0};
		::gpk::n3f							Target						= {0, 0, 0};
		float								Zoom						= 2.0f;
		uint8_t								BallLockAtPosition			= (uint8_t)-1;
		uint8_t								BallLockAtTarget			= (uint8_t)-1;
	};

	typedef ::gpk::array_static<::d1::SCamera, ::d1p::MAX_BALLS> TCameraStaticArray;
	struct SPlayerCameras {
		::d1::SCamera						Free						= {{-1, 1.75, -2}, {0,}};
		::d1::TCameraStaticArray			Balls						= {{{-1, 1.75, -2}, {}}};
		::d1::TCameraStaticArray			Pockets						= {{{-1, 1.75, -2}, {}}};
		::d1::SCamera						Stick						= {{-1, 1.75, -2}, {}};
		uint32_t							Selected					= ::d1p::MAX_BALLS + 1;
	};

	GDEFINE_ENUM_TYPE(APP_STATE, uint8_t);
	GDEFINE_ENUM_VALUE(APP_STATE, Init		,  0);
	GDEFINE_ENUM_VALUE(APP_STATE, Welcome	,  1);	// Welcome comes right after Init.
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
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (PLAYER_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, Human	, 0);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, AI		, 1);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, Network	, 2);

	struct SD1Player {
		::d1::SPlayerCameras				Cameras						= {};
		::gpk::bgra							Color;
		::gpk::vcc							Name;
		PLAYER_TYPE							Type;

		::gpk::error_t						Save			(::gpk::apod<uint8_t> & output) const {
			gpk_necs(::gpk::savePOD		(output, Cameras));
			gpk_necs(::gpk::savePOD		(output, Color));
			gpk_necs(::gpk::savePOD		(output, Type));
			gpk_necs(::gpk::saveView	(output, Name));
			return 0;
		}

		::gpk::error_t						Load			(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadPOD		(input, Cameras	));
			gpk_necs(::gpk::loadPOD		(input, Color	));
			gpk_necs(::gpk::loadPOD		(input, Type	));
			gpk_necs(::gpk::loadLabel	(input, Name	));
			return 0;
		}
	};

	struct SPlayerUI {
		::gpk::astatic<uint32_t, ::d1::APP_STATE_COUNT>	
											DialogPerState				= {};
	};

	GDEFINE_ENUM_TYPE(CAMERA_INPUT, uint8_t);
	GDEFINE_ENUM_VALUE(CAMERA_INPUT, Default	, 0);
	GDEFINE_ENUM_VALUE(CAMERA_INPUT, Move		, 1);
	GDEFINE_ENUM_VALUE(CAMERA_INPUT, Turn		, 2);
	GDEFINE_ENUM_VALUE(CAMERA_INPUT, Position	, 3);
	GDEFINE_ENUM_VALUE(CAMERA_INPUT, Target		, 4);
	GDEFINE_ENUM_VALUE(CAMERA_INPUT, Instance	, 5);

	typedef ::gpk::SEvent<CAMERA_INPUT>	SEventCamera;

	struct SEventCameraMove		{ gpk::DIRECTION Direction; float Value; };
	struct SEventCameraTurn		{ gpk::DIRECTION Direction; float Value; };
	struct SEventCameraPosition	{ gpk::DIRECTION Direction; float Value; };
	struct SEventCameraTarget	{ gpk::DIRECTION Direction; float Value; };
	struct SEventCameraInstance	{ gpk::DIRECTION Direction; };

	// This structure extends the game setup to include enumerations used by the UI but which are irrelevant to the game mechanics.
	struct SD1StartState : public ::d1p::SMatchState {
		::d1p::TABLE_SIZE					StandardTableSize	= ::d1p::TABLE_SIZE_8_00_FOOT;
		uint8_t								CountPlayers		= 0;
	};
#pragma pack(pop)

	enum SAVE_MODE { SAVE_MODE_AUTO, SAVE_MODE_QUICK, SAVE_MODE_STAGE, SAVE_MODE_USER };

	GDEFINE_ENUM_TYPE(D1_ASSET_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(D1_ASSET_MODE, Procedural, 0);
	GDEFINE_ENUM_VALUE(D1_ASSET_MODE, File		, 1);

	struct SD1Game {
		::d1p::SPoolGame					Pool				= {};
		::d1::SD1StartState					StartState			= {};
		::gpk::SFrameInfo					FrameInfo			= {};
		float								TimeScale			= 1;
		::gpk::apod<::gpk::SContact>		ContactsToDraw		= {};
		::gpk::aobj<::d1p::SEventPlayer>	QueueStick			= {};
		::gpk::aobj<::d1::CAMERA_INPUT>		QueueCamera			= {};
		::gpk::astatic<::d1::SD1Player
			, ::d1p::MAX_PLAYERS>			Players				= {};
		::gpk::n3f							LightPos			= {5, 2, 0};


		::gpk::error_t						Save				(::gpk::apod<uint8_t> & output) const {
			gpk_necs(Pool.Save(output));
			gpk_necs(::gpk::savePOD(output, StartState));
			gpk_necs(::gpk::savePOD(output, TimeScale));
			return 0;
		}

		::gpk::error_t						Load				(::gpk::vcu8 & input) {
			gpk_necs(Pool.Load(input));
			gpk_necs(::gpk::loadPOD(input, StartState));
			gpk_necs(::gpk::loadPOD(input, TimeScale));
			return 0;
		}

		inline	::d1::SPlayerCameras&		ActiveCameras		()			{ return Players[Pool.ActiveTeam()].Cameras; }
		inline	const ::d1::SPlayerCameras&	ActiveCameras		()	const	{ return Players[Pool.ActiveTeam()].Cameras; }

		::d1::SCamera&						CameraSelected		()			{ 
			::d1::SPlayerCameras					& playerCameras		= ActiveCameras();
			return	(playerCameras.Selected == 0			) ? playerCameras.Free 
				:	(playerCameras.Selected > d1p::MAX_BALLS) ? playerCameras.Stick
				:	playerCameras.Balls[playerCameras.Selected - 1] 
				;
		}
		const ::d1::SCamera&				CameraSelected		()	const	{ 
			const ::d1::SPlayerCameras				& playerCameras		= ActiveCameras();
			return	(playerCameras.Selected == 0			) ? playerCameras.Free 
				:	(playerCameras.Selected > d1p::MAX_BALLS) ? playerCameras.Stick
				:	playerCameras.Balls[playerCameras.Selected - 1] 
				;
		}
		::gpk::error_t						ResetStickCamera	()			{
			::gpk::quatf32							stickOrientation	= {};
			::d1::SCamera							& cameraSelected	= CameraSelected();
			Pool.GetBallPosition(0, cameraSelected.Target);
			Pool.GetStickOrientation(stickOrientation);
			cameraSelected.Target.y				+=  Pool.MatchState.Table.BallRadius * 2;
			cameraSelected.Position				= {-Pool.MatchState.Table.BallRadius * 20 * cameraSelected.Zoom, .2f, 0};
			cameraSelected.Position				= stickOrientation.RotateVector(cameraSelected.Position);
			cameraSelected.Position				+= cameraSelected.Target;
			return 0;
		}
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

	GDEFINE_ENUM_TYPE (UI_PLAY, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PLAY, Menu			, 0);
	GDEFINE_ENUM_VALUE(UI_PLAY, Time			, 1);
	GDEFINE_ENUM_VALUE(UI_PLAY, Turn			, 2);
	GDEFINE_ENUM_VALUE(UI_PLAY, Player			, 3);
	GDEFINE_ENUM_VALUE(UI_PLAY, Level			, 4);
	GDEFINE_ENUM_VALUE(UI_PLAY, Shoot			, 5);

	GDEFINE_ENUM_TYPE (UI_PROFILE, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Name			, 0);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Score		, 1);

	struct SD1UI {
		typedef ::gpk::rtbgra8d32			TRenderTarget;

		::gpk::SDialog						Dialog						= {};
		::gpk::pobj<::gpk::SDialogSlider>	ForceSlider					= {};
		TRenderTarget						ForceSliderRenderTarget		= {};
		::gpk::astaticu32<::d1::APP_STATE_COUNT>	
											DialogPerState				= {};
		::gpk::astatic<::d1::SPlayerUI
			, ::d1p::MAX_PLAYERS>			PlayerUI					= {};
		::gpk::SUIInputBox					NameEditBox;

		::gpk::pnco<::gpk::SDialogTuner<uint8_t>>	TunerTeamCount;
		::gpk::pnco<::gpk::SDialogTuner<uint8_t>>	TunerPlayerCount;
		::gpk::pnco<::gpk::SDialogTuner<uint8_t>>	TunerTableSize;


		::d1::SD1Logo						Logo;
		float								ClearColor	[4]				= {.25f, .125f, .35f, 1.0f};

	};

	struct SD1FileStrings {
		::gpk::vcc							SavegameFolder				= {1, "."};
		::gpk::vcc							ExtensionImages				= {4, ".png"};
		::gpk::vcc							ExtensionProfile			= {4, ".d1p"};
		::gpk::vcc							ExtensionCredentials		= {4, ".d1c"};
		::gpk::vcc							ExtensionSave				= {4, ".d1z"};
		::gpk::vcc							ExtensionSaveUser			= {9, ".user.d1z"};
		::gpk::vcc							ExtensionSaveAuto			= {9, ".auto.d1z"};
		::gpk::vcc							ExtensionSaveStage			= {10, ".stage.d1z"};
		::gpk::vcc							ExtensionSaveQuick			= {10, ".quick.d1z"};
	};
	
	struct SD1 {
		::d1::SD1Game						MainGame					= {};
		::d1::SD1UI							AppUI						= {};
		::d1::APP_STATE						ActiveState					= {};

		::d1::SD1FileStrings				FileStrings					= {};

		::gpk::error_t						StateSwitch					(::d1::APP_STATE newState)			{
			if(newState != ActiveState) {
				AppUI.Dialog.GUI->Controls.States[AppUI.DialogPerState[newState]].Hidden = false;
				if(ActiveState >= ::d1::APP_STATE_Home && AppUI.DialogPerState[ActiveState])
					AppUI.Dialog.GUI->Controls.States[AppUI.DialogPerState[ActiveState]].Hidden = true;
				if(newState == ::d1::APP_STATE_Home && ActiveState > ::d1::APP_STATE_Home) // Don't autosave first time we enter Home.
					gpk_necs(Save(::d1::SAVE_MODE_AUTO));

				ActiveState							= newState;
			}
			return ActiveState;
		}

		::gpk::error_t						Save						(SAVE_MODE autosaveMode)	const	{
			::gpk::vcc								extension;
			switch(autosaveMode) {
			case SAVE_MODE_USER		: extension = FileStrings.ExtensionSaveUser		; break;
			case SAVE_MODE_STAGE	: extension = FileStrings.ExtensionSaveStage	; break;
			case SAVE_MODE_QUICK	: extension = FileStrings.ExtensionSaveQuick	; break;
			case SAVE_MODE_AUTO		: extension = FileStrings.ExtensionSaveAuto		; break;
			}

			const uint64_t							timeCurrent					= (SAVE_MODE_USER != autosaveMode) ? 0 : ::gpk::timeCurrent();
			const uint64_t							timeStart					= (SAVE_MODE_AUTO == autosaveMode) ? 0 : MainGame.Pool.MatchState.TimeStart;
			char									fileName[4096]				= {};
			sprintf_s(fileName, "%s/%llu.%llu%s", FileStrings.SavegameFolder.begin(), timeStart, timeCurrent, extension.begin());

			if(false == MainGame.Pool.MatchState.Flags.GameOver) // Save only if a player is alive
				return Save(fileName);
			return 0;
		}

		::gpk::error_t						Save						(::gpk::vcc fileName)		const	{
			::gpk::au8								serialized;
			gpk_necs(MainGame.Save(serialized));
			for(uint32_t iPlayer = 0; iPlayer < MainGame.Players.size(); ++iPlayer) {
				const ::d1::SD1Player					& player					= MainGame.Players[iPlayer];
				gpk_necall(player.Save(serialized), "iPlayer: %i", iPlayer);
			}

			::gpk::au8								deflated;
			gpk_necs(::gpk::arrayDeflate(serialized, deflated));

			info_printf("Savegame size in bytes: %u.", serialized.size());
			info_printf("Savegame file size: %u.", deflated.size());

			return ::gpk::fileFromMemory(fileName, deflated);
		}

		::gpk::error_t						Load						(::gpk::vcc filename)				{
			::gpk::au8								serialized;
			gpk_necs(::gpk::fileToMemory(filename, serialized));

			::gpk::au8								inflated;
			::gpk::arrayInflate(serialized, inflated);

			info_printf("Savegame file size: %u.", inflated.size());
			info_printf("Savegame size in bytes: %u.", serialized.size());

			::gpk::vcu8								viewSerialized				= inflated;
			if errored(MainGame.Load(viewSerialized)) {
				es_if(errored(::d1p::poolGameReset(MainGame.Pool, MainGame.StartState)));
				return -1;
			}
			for(uint32_t iPlayer = 0; iPlayer < MainGame.Players.size(); ++iPlayer) {
				::d1::SD1Player						& player					= MainGame.Players[iPlayer];
				if errored(player.Load(viewSerialized)) {
					error_printf("Failed to load player %i.", iPlayer);
					es_if(errored(::d1p::poolGameReset(MainGame.Pool, MainGame.StartState)));
					return -1;
				}
			}
			gpk_necs(::gpk::sliderSetValue(*AppUI.ForceSlider, int64_t(AppUI.ForceSlider->ValueLimits.Max - MainGame.Pool.ActiveStick().Velocity * (AppUI.ForceSlider->ValueLimits.Max / ::d1p::MAX_SHOOT_VELOCITY))));
			AppUI.TunerPlayerCount->SetValue(MainGame.StartState.CountPlayers);
			AppUI.TunerTableSize->SetValue(MainGame.StartState.StandardTableSize);
			return 0;
		}

	};

	::gpk::error_t							d1Update				(::d1::SD1 & app, double secondsElapsed, const ::gpk::pobj<::gpk::SInput> & inputState, const ::gpk::view<::gpk::SSysEvent> & systemEvents);
	::gpk::error_t							d1Draw					(::d1::SD1UI & appUI, ::d1::SD1Game & poolGame, ::gpk::rt<::gpk::bgra, uint32_t> & backBuffer, double totalSeconds, bool onlyGUI = false);

	::gpk::error_t							guiSetup					(::d1::SD1UI & appUI, ::d1::SD1Game & appGame, const ::gpk::pobj<::gpk::SInput> & input);
	::d1::APP_STATE							guiUpdate					(::d1::SD1 & app, ::gpk::view<::gpk::SSysEvent> sysEvents);
} // namespace

#endif