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

	GDEFINE_ENUM_TYPE(APP_STATE, u0_t );
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
		u0_t 						BallLockAtPosition	= (u0_t )-1;
		u0_t 						BallLockAtTarget	= (u0_t )-1;
	};

	typedef ::gpk::astatic<::d1::SCamera, ::d1p::MAX_BALLS> TCameraStaticArray;
	struct SPlayerCameras {
		::d1::SCamera				Free				= {{-1, 1.75, -2}, {0,}};
		::d1::TCameraStaticArray	Balls				= {{{-1, 1.75, -2}, {}}};
		::d1::TCameraStaticArray	Pockets				= {{{-1, 1.75, -2}, {}}};
		::d1::SCamera				Stick				= {{-1, 1.75, -2}, {}};
		u2_t 					Selected			= ::d1p::MAX_BALLS + 1;

		const ::d1::SCamera&		GetCamera			(u2_t  cameraId)	const	{
			return	(cameraId == 0				) ? Free 
				:	(cameraId > d1p::MAX_BALLS	) ? Stick
				:	Balls[cameraId - 1]
				;
		}
		::d1::SCamera&				GetCamera			(u2_t  cameraId)	{
			return	(cameraId == 0				) ? Free 
				:	(cameraId > d1p::MAX_BALLS	) ? Stick
				:	Balls[cameraId - 1]
				;
		}
		inline	SCamera&			GetSelected			()			{ return GetCamera(Selected); }
		inline	const SCamera&		GetSelected			()	const	{ return GetCamera(Selected); }
	};

	GDEFINE_ENUM_TYPE (PLAYER_TYPE, u0_t );
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, Human	, 0);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, AI		, 1);
	GDEFINE_ENUM_VALUE(PLAYER_TYPE, Network	, 2);

	struct SD1Player {
		::gpk::vcsc_t					Name				= {};
		::d1::SPlayerCameras		Cameras				= {};
		::gpk::bgra					Color				= {};
		PLAYER_TYPE					Type				= {};

		::gpk::err_t				Save				(::gpk::apod<u0_t > & output) const {
			{ if_fail_fe(::gpk::savePOD		(output, Cameras)); }
			{ if_fail_fe(::gpk::savePOD		(output, Color)); }
			{ if_fail_fe(::gpk::savePOD		(output, Type)); }
			{ if_fail_fe(::gpk::saveView	(output, Name)); }
			return 0;
		}

		::gpk::err_t				Load				(::gpk::vcu0_t & input) {
			{ if_fail_fe(::gpk::loadPOD		(input, Cameras	)); }
			{ if_fail_fe(::gpk::loadPOD		(input, Color	)); }
			{ if_fail_fe(::gpk::loadPOD		(input, Type	)); }
			{ if_fail_fe(::gpk::loadLabel	(input, Name	)); }
			return 0;
		}
	};

	stxp ::gpk::vcsc_t			TEAM_TEXT[]			= {{6, "Team 1"}, {6, "Team 2"}};

	GDEFINE_ENUM_TYPE (UI_TEAM, u0_t );
	GDEFINE_ENUM_VALUE(UI_TEAM, Name		, 0);
	GDEFINE_ENUM_VALUE(UI_TEAM, Score		, 1);
	GDEFINE_ENUM_VALUE(UI_TEAM, Fouls		, 2);
	struct STeamUI {
		::gpk::astcid<::d1::APP_STATE_COUNT>	DialogPerState				= {};	
		::gpk::astcid<::d1::APP_STATE_COUNT>	FirstControl				= {}; 	
		::gpk::astatic<::gpk::acid, APP_STATE_COUNT>	
												DialogControls			= {}; 	
	};

	GDEFINE_ENUM_TYPE(CAMERA_INPUT, u0_t );
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
		u0_t 						CountPlayers		= 0;
	};
#pragma pack(pop)

	enum SAVE_MODE { SAVE_MODE_AUTO, SAVE_MODE_QUICK, SAVE_MODE_STAGE, SAVE_MODE_USER };

	GDEFINE_ENUM_TYPE(D1_ASSET_MODE, u0_t );
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
		::gpk::n3f2_t						LightPos			= {5, 2, 0};


		::gpk::err_t						Save				(::gpk::apod<u0_t > & output) const {
			{ if_fail_fe(Pool.Save(output)); }
			{ if_fail_fe(::gpk::savePOD(output, StartState)); }
			{ if_fail_fe(::gpk::savePOD(output, TimeScale)); }
			return 0;
		}

		::gpk::err_t						Load				(::gpk::vcu0_t & input) {
			{ if_fail_fe(Pool.Load(input)); }
			{ if_fail_fe(::gpk::loadPOD(input, StartState)); }
			{ if_fail_fe(::gpk::loadPOD(input, TimeScale)); }
			return 0;
		}

		inline	::d1::SPlayerCameras&		ActiveCameras		()							{ return Players[Pool.ActivePlayer()].Cameras; }
		inline	::d1::SCamera&				CameraSelected		()							{ return ActiveCameras().GetSelected(); }
		inline	::d1::SCamera&				GetActiveCamera		(u2_t  cameraId)			{ return ActiveCameras().GetCamera(cameraId); }

		inline	const ::d1::SPlayerCameras&	ActiveCameras		()					const	{ return Players[Pool.ActivePlayer()].Cameras; }
		inline	const ::d1::SCamera&		CameraSelected		()					const	{ return ActiveCameras().GetSelected(); }
		inline	const ::d1::SCamera&		GetActiveCamera		(u2_t  cameraId)	const	{ return ActiveCameras().GetCamera(cameraId); }
		::gpk::err_t						ResetStickCamera	(u0_t  iStick)			{
			::d1::SCamera							& stickCamera		= Players[iStick].Cameras.Stick;
			::gpk::quatf32							stickOrientation	= {};
			if_fail_e(Pool.GetBallPosition(0, stickCamera.Target));
			if_fail_ef(Pool.GetStickOrientation(iStick, stickOrientation), "iStick:%i", iStick);
			stickCamera.Target.y				+=  Pool.MatchState.Board.BallRadius * 2;
			stickCamera.Position				= {-Pool.MatchState.Board.BallRadius * 20 * stickCamera.Zoom, .2f, 0};
			stickCamera.Position				= stickOrientation.RotateVector(stickCamera.Position);
			stickCamera.Position				+= stickCamera.Target;
			return 0;
		}
		inline	::gpk::err_t				ResetStickCamera	()			{ return ResetStickCamera(Pool.ActivePlayer()); }
		::gpk::err_t						ResetStickCameras	()			{
			for(u0_t  iPlayer = 0; iPlayer < Players.size(); ++iPlayer) 
				ResetStickCamera(iPlayer);
			return 0;
		}
	};

	GDEFINE_ENUM_TYPE (UI_HOME, u0_t );
	GDEFINE_ENUM_VALUE(UI_HOME, Continue		, 0);
	GDEFINE_ENUM_VALUE(UI_HOME, Shop			, 1);
	GDEFINE_ENUM_VALUE(UI_HOME, Save			, 2);
	GDEFINE_ENUM_VALUE(UI_HOME, Load			, 3);
	GDEFINE_ENUM_VALUE(UI_HOME, Start			, 4);
	GDEFINE_ENUM_VALUE(UI_HOME, Leaderboards	, 5);
	GDEFINE_ENUM_VALUE(UI_HOME, Credits			, 6);
	GDEFINE_ENUM_VALUE(UI_HOME, Settings		, 7);
	GDEFINE_ENUM_VALUE(UI_HOME, Exit			, 8);

	GDEFINE_ENUM_TYPE (UI_PLAY, u0_t );
	GDEFINE_ENUM_VALUE(UI_PLAY, Menu			, 0);
	GDEFINE_ENUM_VALUE(UI_PLAY, Time			, 1);
	GDEFINE_ENUM_VALUE(UI_PLAY, Team			, 2);
	GDEFINE_ENUM_VALUE(UI_PLAY, Player			, 3);
	GDEFINE_ENUM_VALUE(UI_PLAY, Turn			, 4);
	GDEFINE_ENUM_VALUE(UI_PLAY, Shoot			, 5);

	GDEFINE_ENUM_TYPE (UI_HELP, u0_t );
	GDEFINE_ENUM_VALUE(UI_HELP, Yaw				, 0);
	GDEFINE_ENUM_VALUE(UI_HELP, Ball			, 1);
	GDEFINE_ENUM_VALUE(UI_HELP, Aim				, 2);

	GDEFINE_ENUM_TYPE (UI_STICK, u0_t );
	GDEFINE_ENUM_VALUE(UI_STICK, Shift			, 0);
	GDEFINE_ENUM_VALUE(UI_STICK, Angle			, 1);
	GDEFINE_ENUM_VALUE(UI_STICK, Pitch			, 2);

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

		//::gpk::pnco<::gpk::SDialogTuner<u0_t >>	TunerTeamCount;
		//::gpk::pnco<::gpk::SDialogTuner<u0_t >>	TunerPlayerCount;
		::gpk::pnco<::gpk::SDialogTuner<u0_t >>	TunerTableSize;

		float								ClearColor	[4]				= {.25f, .125f, .35f, 1.0f};

		::gpk::err_t						RefreshTeamStrings			(u0_t  teamStripped)			{
			stxp const sc_t*						strteam[]					= {"Solid", "Stripped"};
			sprintf_s(teamsbuffer[0].Storage, "Team 1 - %s", strteam[(teamStripped + 1) & 1]);
			sprintf_s(teamsbuffer[1].Storage, "Team 2 - %s", strteam[teamStripped]);
			return 0;
		}
		::gpk::err_t						RefreshTeamUI				() {
			::gpk::SGUI								& gui						= *Dialog.GUI;
			stxp	::d1::APP_STATE					states[]					= {::d1::APP_STATE_Home, ::d1::APP_STATE_Play};
			for(u2_t  iTeam = 0; iTeam < TeamUI.size(); ++iTeam) {
				const ::d1::STeamUI						& teamUI					= TeamUI[iTeam];
				for(u2_t  iState = 0; iState < ::gpk::size(states); ++iState) {
					::gpk::controlTextSet(gui, teamUI.FirstControl[states[iState]] + ::d1::UI_TEAM_Name, teamsbuffer[iTeam].Storage);
					::gpk::controlTextSet(gui, teamUI.FirstControl[states[iState]] + ::d1::UI_TEAM_Score, scorebuffer[iTeam].Storage);
					::gpk::controlTextSet(gui, teamUI.FirstControl[states[iState]] + ::d1::UI_TEAM_Fouls, foulsbuffer[iTeam].Storage);
				}
			}
			return 0;
		}
		::gpk::err_t						RefreshPlayUI				(u0_t  iPlayer, u0_t  iTeam) {
			::gpk::controlTextSet(*Dialog.GUI, FirstControl[::d1::APP_STATE_Play] + ::d1::UI_PLAY_Player, playrbuffer[iPlayer].Storage);
			::gpk::controlTextSet(*Dialog.GUI, FirstControl[::d1::APP_STATE_Play] + ::d1::UI_PLAY_Team	, teamsbuffer[iTeam].Storage);
			return 0;
		}
	};
	struct SD1FileStrings {
		::gpk::vcsc_t				SavegameFolder				= {1, "."};
		::gpk::vcsc_t				ExtensionImages				= {4, ".png"};
		::gpk::vcsc_t				ExtensionProfile			= {4, ".d1p"};
		::gpk::vcsc_t				ExtensionCredentials		= {4, ".d1c"};
		::gpk::vcsc_t				ExtensionSave				= {4, ".d1z"};
		::gpk::vcsc_t				ExtensionSaveUser			= {9, ".user.d1z"};
		::gpk::vcsc_t				ExtensionSaveAuto			= {9, ".auto.d1z"};
		::gpk::vcsc_t				ExtensionSaveStage			= {10, ".stage.d1z"};
		::gpk::vcsc_t				ExtensionSaveQuick			= {10, ".quick.d1z"};
	};
	
	struct SD1 {
		::d1::SD1Game			MainGame					= {};
		::d1::SD1UI				AppUI						= {};
		::d1::APP_STATE			ActiveState					= {};

		::d1::SD1FileStrings	FileStrings					= {};

		::gpk::err_t			StateSwitch					(::d1::APP_STATE newState)			{
			::gpk::SControlTable		& table						= AppUI.Dialog.GUI->Controls;
			if(newState != ActiveState) {
				const ::gpk::cid_t iDialogNew = AppUI.DialogPerState[newState];
				table.SetHidden(iDialogNew, false);

				const ::gpk::cid_t iDialogActive = AppUI.DialogPerState[ActiveState];
				if(ActiveState >= ::d1::APP_STATE_Home && iDialogActive) {
					table.SetHidden(iDialogActive, true);
				}
				if(newState == ::d1::APP_STATE_Home && ActiveState > ::d1::APP_STATE_Home) // Don't autosave first time we enter Home.
					{ if_fail_fe(Save(::d1::SAVE_MODE_AUTO)); }

				ActiveState				= newState;
			}
			return ActiveState;
		}

		::gpk::err_t			Save			(SAVE_MODE autosaveMode)	const	{
			::gpk::vcsc_t					extension;
			switch(autosaveMode) {
			case SAVE_MODE_USER		: extension = FileStrings.ExtensionSaveUser		; break;
			case SAVE_MODE_STAGE	: extension = FileStrings.ExtensionSaveStage	; break;
			case SAVE_MODE_QUICK	: extension = FileStrings.ExtensionSaveQuick	; break;
			case SAVE_MODE_AUTO		: extension = FileStrings.ExtensionSaveAuto		; break;
			}

			const u3_t 				timeCurrent		= (SAVE_MODE_USER != autosaveMode) ? 0 : ::gpk::timeCurrent();
			const u3_t 				timeStart		= (SAVE_MODE_AUTO == autosaveMode) ? 0 : MainGame.Pool.MatchState.TimeStart;
			sc_t						fileName[4096]	= {};
			sprintf_s(fileName, "%.*s/%llu.%llu%.*s", GPK_VIEW_ARGS(FileStrings.SavegameFolder), timeStart, timeCurrent, GPK_VIEW_ARGS(extension));

			if(false == MainGame.Pool.MatchState.Flags.GameOver) // Save only if a player is alive
				return Save(fileName);
			return 0;
		}

		::gpk::err_t			Save						(::gpk::vcsc_t fileName)		const	{
			::gpk::au0_t					serialized;
			{ if_fail_fe(MainGame.Save(serialized)); }
			for(u2_t  iPlayer = 0; iPlayer < MainGame.Players.size(); ++iPlayer) {
				const ::d1::SD1Player		& player					= MainGame.Players[iPlayer];
				{ if_fail_fef(player.Save(serialized), "iPlayer: %i", iPlayer); }
			}
			return ::gpk::deflateFromMemory(fileName, serialized);
		}

		::gpk::err_t			Load						(::gpk::vcsc_t filename)				{
			::gpk::au0_t					serialized;
			{ if_fail_fe(::gpk::inflateToMemory(filename, serialized)); }
			::gpk::vcu0_t					viewSerialized				= serialized;
			if(::gpk::failed(MainGame.Load(viewSerialized))) {
				if_true_e(::gpk::failed(::d1p::poolGameReset(MainGame.Pool, MainGame.StartState)));
				return -1;
			}
			for(u2_t  iPlayer = 0; iPlayer < MainGame.Players.size(); ++iPlayer) {
				::d1::SD1Player				& player					= MainGame.Players[iPlayer];
				if(::gpk::failed(player.Load(viewSerialized))) {
					error_printf("Failed to load player %i.", iPlayer);
					if_true_e(::gpk::failed(::d1p::poolGameReset(MainGame.Pool, MainGame.StartState)));
					return -1;
				}
			}
			{ if_fail_fe(::gpk::sliderSetValue(*AppUI.ForceSlider, s3_t(AppUI.ForceSlider->ValueLimits.Max - MainGame.Pool.ActiveStick().Velocity * (AppUI.ForceSlider->ValueLimits.Max / ::d1p::MAX_SHOOT_VELOCITY)))); }
			//AppUI.TunerPlayerCount->SetValue(MainGame.StartState.CountPlayers);
			AppUI.TunerTableSize->SetValue(MainGame.StartState.StandardTableSize);
			for(u0_t  iTeam = 0; iTeam < AppUI.TeamUI.size(); ++iTeam)
				sprintf_s(AppUI.scorebuffer[iTeam].Storage, "Score: %u", MainGame.Pool.MatchState.PocketedCount(iTeam));

			return 0;
		}

	};

	::gpk::err_t			d1Update				(::d1::SD1 & app, double secondsElapsed, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEventsNew);
	::gpk::err_t			d1Draw					(::d1::SD1UI & appUI, ::d1::SD1Game & poolGame, ::gpk::rtbgra8d32 & backBuffer, double totalSeconds, bool onlyGUI = false);

	::gpk::err_t			guiSetup				(::d1::SD1UI & appUI, ::d1::SD1Game & appGame, const ::gpk::pobj<::gpk::SInput> & input);
	::d1::APP_STATE			guiUpdate				(::d1::SD1 & app, ::gpk::vpobj<::gpk::SEventSystem> sysEvents);
} // namespace

#endif