#include "gpk_frameinfo.h"
#include "gpk_pool_game.h"
#include "gpk_camera.h"

#include "gpk_gui_control_list.h"
#include "gpk_gui_inputbox.h"
#include "gpk_dialog_controls.h"
#include "gpk_deflate.h"
#include "gpk_file.h"


#ifndef GPK_D1_H_098273498237423
#define GPK_D1_H_098273498237423

namespace d1 
{
	GPK_USING_TYPEINT();

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
	struct SCamera : ::gpk::SCameraPoints {
		float						Zoom				= 2.0f;
		uint8_t						BallLockAtPosition	= (uint8_t)-1;
		uint8_t						BallLockAtTarget	= (uint8_t)-1;
	};

	typedef ::gpk::astatic<::d1::SCamera, ::d1p::MAX_BALLS> TCameraStaticArray;
	struct SPlayerCameras {
		::d1::SCamera				Free				= {{-1, 1.75, -2}, {0,}};
		::d1::TCameraStaticArray	Balls				= {{{-1, 1.75, -2}, {}}};
		::d1::TCameraStaticArray	Pockets				= {{{-1, 1.75, -2}, {}}};
		::d1::SCamera				Stick				= {{-1, 1.75, -2}, {}};
		uint32_t					Selected			= ::d1p::MAX_BALLS + 1;

		const ::d1::SCamera&		GetCamera			(uint32_t cameraId)	const	{
			return	(cameraId == 0				) ? Free 
				:	(cameraId > d1p::MAX_BALLS	) ? Stick
				:	Balls[cameraId - 1]
				;
		}
		::d1::SCamera&				GetCamera			(uint32_t cameraId)	{
			return	(cameraId == 0				) ? Free 
				:	(cameraId > d1p::MAX_BALLS	) ? Stick
				:	Balls[cameraId - 1]
				;
		}
		inline	SCamera&			GetSelected			()			{ return GetCamera(Selected); }
		inline	const SCamera&		GetSelected			()	const	{ return GetCamera(Selected); }
	};

	GDEFINE_ENUM_TYPE (PLAYER_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, Human	, 0);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, AI		, 1);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, Network	, 2);

	struct SD1Player {
		::gpk::vcc					Name				= {};
		::d1::SPlayerCameras		Cameras				= {};
		::gpk::bgra					Color				= {};
		PLAYER_TYPE					Type				= {};

		::gpk::error_t				Save				(::gpk::apod<uint8_t> & output) const {
			gpk_necs(::gpk::savePOD		(output, Cameras));
			gpk_necs(::gpk::savePOD		(output, Color));
			gpk_necs(::gpk::savePOD		(output, Type));
			gpk_necs(::gpk::saveView	(output, Name));
			return 0;
		}

		::gpk::error_t				Load				(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadPOD		(input, Cameras	));
			gpk_necs(::gpk::loadPOD		(input, Color	));
			gpk_necs(::gpk::loadPOD		(input, Type	));
			gpk_necs(::gpk::loadLabel	(input, Name	));
			return 0;
		}
	};

	stacxpr ::gpk::vcc			TEAM_TEXT[]			= {{6, "Team 1"}, {6, "Team 2"}};

	GDEFINE_ENUM_TYPE (UI_TEAM, uint8_t);
	GDEFINE_ENUM_VALUE(UI_TEAM, Name		, 0);
	GDEFINE_ENUM_VALUE(UI_TEAM, Score		, 1);
	GDEFINE_ENUM_VALUE(UI_TEAM, Fouls		, 2);
	struct STeamUI {
		::gpk::astcid<::d1::APP_STATE_COUNT>	DialogPerState				= {};	
		::gpk::astcid<::d1::APP_STATE_COUNT>	FirstControl				= {}; 	
		::gpk::astatic<::gpk::acid, APP_STATE_COUNT>	
												DialogControls			= {}; 	
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
		::d1p::TABLE_SIZE			StandardTableSize	= ::d1p::TABLE_SIZE_8_00_FOOT;
		uint8_t						CountPlayers		= 0;
	};
#pragma pack(pop)

	enum SAVE_MODE { SAVE_MODE_AUTO, SAVE_MODE_QUICK, SAVE_MODE_STAGE, SAVE_MODE_USER };

	GDEFINE_ENUM_TYPE(D1_ASSET_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(D1_ASSET_MODE, Procedural, 0);
	GDEFINE_ENUM_VALUE(D1_ASSET_MODE, File		, 1);

	struct SD1Game {
		::d1p::SPoolGame					Pool				= {};
		::d1::SD1StartState					StartState			= {};
		float								TimeScale			= 1;
		::gpk::apod<::gpk::SContact>		ContactsToDraw		= {};
		::gpk::aobj<::d1p::SEventPlayer>	QueueStick			= {};
		::gpk::aobj<::d1::CAMERA_INPUT>		QueueCamera			= {};
		::gpk::astatic<::d1::SD1Player
			, ::d1p::MAX_PLAYERS>			Players				= {};
		::gpk::n3f32							LightPos			= {5, 2, 0};


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

		inline	::d1::SPlayerCameras&		ActiveCameras		()							{ return Players[Pool.ActivePlayer()].Cameras; }
		inline	::d1::SCamera&				CameraSelected		()							{ return ActiveCameras().GetSelected(); }
		inline	::d1::SCamera&				GetActiveCamera		(uint32_t cameraId)			{ return ActiveCameras().GetCamera(cameraId); }

		inline	const ::d1::SPlayerCameras&	ActiveCameras		()					const	{ return Players[Pool.ActivePlayer()].Cameras; }
		inline	const ::d1::SCamera&		CameraSelected		()					const	{ return ActiveCameras().GetSelected(); }
		inline	const ::d1::SCamera&		GetActiveCamera		(uint32_t cameraId)	const	{ return ActiveCameras().GetCamera(cameraId); }
		::gpk::error_t						ResetStickCamera	(uint8_t iStick)			{
			::d1::SCamera							& stickCamera		= Players[iStick].Cameras.Stick;
			::gpk::quatf32							stickOrientation	= {};
			Pool.GetBallPosition(0, stickCamera.Target);
			Pool.GetStickOrientation(iStick, stickOrientation);
			stickCamera.Target.y				+=  Pool.MatchState.Board.BallRadius * 2;
			stickCamera.Position				= {-Pool.MatchState.Board.BallRadius * 20 * stickCamera.Zoom, .2f, 0};
			stickCamera.Position				= stickOrientation.RotateVector(stickCamera.Position);
			stickCamera.Position				+= stickCamera.Target;
			return 0;
		}
		inline	::gpk::error_t				ResetStickCamera	()			{ return ResetStickCamera(Pool.ActivePlayer()); }
		::gpk::error_t						ResetStickCameras	()			{
			for(uint8_t iPlayer = 0; iPlayer < Players.size(); ++iPlayer) 
				ResetStickCamera(iPlayer);
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
	GDEFINE_ENUM_VALUE(UI_PLAY, Team			, 2);
	GDEFINE_ENUM_VALUE(UI_PLAY, Player			, 3);
	GDEFINE_ENUM_VALUE(UI_PLAY, Turn			, 4);
	GDEFINE_ENUM_VALUE(UI_PLAY, Shoot			, 5);

	GDEFINE_ENUM_TYPE (UI_HELP, uint8_t);
	GDEFINE_ENUM_VALUE(UI_HELP, Yaw				, 0);
	GDEFINE_ENUM_VALUE(UI_HELP, Ball			, 1);
	GDEFINE_ENUM_VALUE(UI_HELP, Aim				, 2);

	struct SD1UI {
		typedef ::gpk::rtbgra8d32			TRenderTarget;

		::gpk::SDialog						Dialog					= {};
		::gpk::pobj<::gpk::SDialogSlider>	ForceSlider				= {};
		TRenderTarget						ForceSliderRenderTarget	= {};
		::gpk::astcid<APP_STATE_COUNT>		DialogPerState			= {};
		::gpk::astcid<APP_STATE_COUNT>		FirstControl			= {}; 	
		::gpk::astatic<::gpk::acid, APP_STATE_COUNT>	
											DialogControls			= {}; 	
		::gpk::astatic<::d1::STeamUI, 2>	TeamUI					= {};
		::gpk::SInputBox					NameEditBox;
		
		::gpk::astatic<sc_t, 64>			turnsbuffer;
		::gpk::astatic<sc_t, 64>			secdsbuffer;
		::gpk::astatic<sc_t, 64>			playrbuffer[6]			=  {{"Player 1"}, {"Player 2"}, {"Player 3"}, {"Player 4"}, {"Player 5"}, {"Player 6"}};
		::gpk::astatic<sc_t, 64>			teamsbuffer[2]			=  {{"Team 1"}, {"Team 2"}};
		::gpk::astatic<sc_t, 64>			scorebuffer[2]			=  {{"Score: 0"}, {"Score: 0"}};
		::gpk::astatic<sc_t, 64>			foulsbuffer[2]			=  {{"Fouls: 0"}, {"Fouls: 0"}};

		//::gpk::pnco<::gpk::SDialogTuner<uint8_t>>	TunerTeamCount;
		//::gpk::pnco<::gpk::SDialogTuner<uint8_t>>	TunerPlayerCount;
		::gpk::pnco<::gpk::SDialogTuner<uint8_t>>	TunerTableSize;

		float								ClearColor	[4]				= {.25f, .125f, .35f, 1.0f};

		::gpk::error_t						RefreshTeamStrings			(uint8_t teamStripped)			{
			stacxpr const sc_t*						strteam[]					= {"Solid", "Stripped"};
			sprintf_s(teamsbuffer[0].Storage, "Team 1 - %s", strteam[(teamStripped + 1) & 1]);
			sprintf_s(teamsbuffer[1].Storage, "Team 2 - %s", strteam[teamStripped]);
			return 0;
		}
		::gpk::error_t						RefreshTeamUI				() {
			::gpk::SGUI								& gui						= *Dialog.GUI;
			stacxpr	::d1::APP_STATE					states[]					= {::d1::APP_STATE_Home, ::d1::APP_STATE_Play};
			for(uint32_t iTeam = 0; iTeam < TeamUI.size(); ++iTeam) {
				const ::d1::STeamUI						& teamUI					= TeamUI[iTeam];
				for(uint32_t iState = 0; iState < ::gpk::size(states); ++iState) {
					::gpk::controlTextSet(gui, teamUI.FirstControl[states[iState]] + ::d1::UI_TEAM_Name, teamsbuffer[iTeam].Storage);
					::gpk::controlTextSet(gui, teamUI.FirstControl[states[iState]] + ::d1::UI_TEAM_Score, scorebuffer[iTeam].Storage);
					::gpk::controlTextSet(gui, teamUI.FirstControl[states[iState]] + ::d1::UI_TEAM_Fouls, foulsbuffer[iTeam].Storage);
				}
			}
			return 0;
		}
		::gpk::error_t						RefreshPlayUI				(uint8_t iPlayer, uint8_t iTeam) {
			::gpk::controlTextSet(*Dialog.GUI, FirstControl[::d1::APP_STATE_Play] + ::d1::UI_PLAY_Player, playrbuffer[iPlayer].Storage);
			::gpk::controlTextSet(*Dialog.GUI, FirstControl[::d1::APP_STATE_Play] + ::d1::UI_PLAY_Team	, teamsbuffer[iTeam].Storage);
			return 0;
		}
	};

	struct SD1FileStrings {
		::gpk::vcc				SavegameFolder				= {1, "."};
		::gpk::vcc				ExtensionImages				= {4, ".png"};
		::gpk::vcc				ExtensionProfile			= {4, ".d1p"};
		::gpk::vcc				ExtensionCredentials		= {4, ".d1c"};
		::gpk::vcc				ExtensionSave				= {4, ".d1z"};
		::gpk::vcc				ExtensionSaveUser			= {9, ".user.d1z"};
		::gpk::vcc				ExtensionSaveAuto			= {9, ".auto.d1z"};
		::gpk::vcc				ExtensionSaveStage			= {10, ".stage.d1z"};
		::gpk::vcc				ExtensionSaveQuick			= {10, ".quick.d1z"};
	};
	
	struct SD1 {
		::d1::SD1Game			MainGame					= {};
		::d1::SD1UI				AppUI						= {};
		::d1::APP_STATE			ActiveState					= {};

		::d1::SD1FileStrings	FileStrings					= {};

		::gpk::error_t			StateSwitch					(::d1::APP_STATE newState)			{
			::gpk::SControlTable		& table						= AppUI.Dialog.GUI->Controls;
			if(newState != ActiveState) {
				const ::gpk::cid_t iDialogNew = AppUI.DialogPerState[newState];
				table.SetHidden(iDialogNew, false);

				const ::gpk::cid_t iDialogActive = AppUI.DialogPerState[ActiveState];
				if(ActiveState >= ::d1::APP_STATE_Home && iDialogActive) {
					table.SetHidden(iDialogActive, true);
				}
				if(newState == ::d1::APP_STATE_Home && ActiveState > ::d1::APP_STATE_Home) // Don't autosave first time we enter Home.
					gpk_necs(Save(::d1::SAVE_MODE_AUTO));

				ActiveState				= newState;
			}
			return ActiveState;
		}

		::gpk::error_t			Save			(SAVE_MODE autosaveMode)	const	{
			::gpk::vcc					extension;
			switch(autosaveMode) {
			case SAVE_MODE_USER		: extension = FileStrings.ExtensionSaveUser		; break;
			case SAVE_MODE_STAGE	: extension = FileStrings.ExtensionSaveStage	; break;
			case SAVE_MODE_QUICK	: extension = FileStrings.ExtensionSaveQuick	; break;
			case SAVE_MODE_AUTO		: extension = FileStrings.ExtensionSaveAuto		; break;
			}

			const uint64_t				timeCurrent		= (SAVE_MODE_USER != autosaveMode) ? 0 : ::gpk::timeCurrent();
			const uint64_t				timeStart		= (SAVE_MODE_AUTO == autosaveMode) ? 0 : MainGame.Pool.MatchState.TimeStart;
			sc_t						fileName[4096]	= {};
			sprintf_s(fileName, "%s/%llu.%llu%s", FileStrings.SavegameFolder.begin(), timeStart, timeCurrent, extension.begin());

			if(false == MainGame.Pool.MatchState.Flags.GameOver) // Save only if a player is alive
				return Save(fileName);
			return 0;
		}

		::gpk::error_t			Save						(::gpk::vcc fileName)		const	{
			::gpk::au8					serialized;
			gpk_necs(MainGame.Save(serialized));
			for(uint32_t iPlayer = 0; iPlayer < MainGame.Players.size(); ++iPlayer) {
				const ::d1::SD1Player		& player					= MainGame.Players[iPlayer];
				gpk_necall(player.Save(serialized), "iPlayer: %i", iPlayer);
			}
			return ::gpk::deflateFromMemory(fileName, serialized);
		}

		::gpk::error_t			Load						(::gpk::vcc filename)				{
			::gpk::au8					serialized;
			gpk_necs(::gpk::inflateToMemory(filename, serialized));
			::gpk::vcu8					viewSerialized				= serialized;
			if(errored(MainGame.Load(viewSerialized))) {
				es_if(errored(::d1p::poolGameReset(MainGame.Pool, MainGame.StartState)));
				return -1;
			}
			for(uint32_t iPlayer = 0; iPlayer < MainGame.Players.size(); ++iPlayer) {
				::d1::SD1Player				& player					= MainGame.Players[iPlayer];
				if(errored(player.Load(viewSerialized))) {
					error_printf("Failed to load player %i.", iPlayer);
					es_if(errored(::d1p::poolGameReset(MainGame.Pool, MainGame.StartState)));
					return -1;
				}
			}
			gpk_necs(::gpk::sliderSetValue(*AppUI.ForceSlider, int64_t(AppUI.ForceSlider->ValueLimits.Max - MainGame.Pool.ActiveStick().Velocity * (AppUI.ForceSlider->ValueLimits.Max / ::d1p::MAX_SHOOT_VELOCITY))));
			//AppUI.TunerPlayerCount->SetValue(MainGame.StartState.CountPlayers);
			AppUI.TunerTableSize->SetValue(MainGame.StartState.StandardTableSize);
			for(uint8_t iTeam = 0; iTeam < AppUI.TeamUI.size(); ++iTeam)
				sprintf_s(AppUI.scorebuffer[iTeam].Storage, "Score: %u", MainGame.Pool.MatchState.PocketedCount(iTeam));

			return 0;
		}

	};

	::gpk::error_t			d1Update				(::d1::SD1 & app, double secondsElapsed, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEventsNew);
	::gpk::error_t			d1Draw					(::d1::SD1UI & appUI, ::d1::SD1Game & poolGame, ::gpk::rtbgra8d32 & backBuffer, double totalSeconds, bool onlyGUI = false);

	::gpk::error_t			guiSetup				(::d1::SD1UI & appUI, ::d1::SD1Game & appGame, const ::gpk::pobj<::gpk::SInput> & input);
	::d1::APP_STATE			guiUpdate				(::d1::SD1 & app, ::gpk::vpobj<::gpk::SEventSystem> sysEvents);
} // namespace

#endif