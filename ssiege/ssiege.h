#include "ssiege_game.h"

#include "gpk_img_serialize.h"
#include "gpk_engine.h"
#include "gpk_gui.h"
#include "gpk_gui_inputbox.h"
#include "gpk_deflate.h"
#include "gpk_array_ptr.h"
#include "gpk_noise.h"
#include "gpk_string_compose.h"

#include "gpk_file_pack.h"
#include "gpk_path.h"
#include <ctime>


#ifndef SSIEGE_H_23701
#define SSIEGE_H_23701

namespace ssg
{

	enum SAVE_MODE { SAVE_MODE_AUTO, SAVE_MODE_QUICK, SAVE_MODE_STAGE, SAVE_MODE_USER };

	struct SFileStrings {
		::gpk::vcst_t				SavegameFolder				= ".";
		::gpk::vcst_t				ExtensionImages				= ".png";
		::gpk::vcst_t				ExtensionProfile			= ".ssg";
		::gpk::vcst_t				ExtensionCredentials		= ".ssgc";
		::gpk::vcst_t				ExtensionSave				= ".ssgz";
		::gpk::vcst_t				ExtensionSaveUser			= ".user.ssgz";
		::gpk::vcst_t				ExtensionSaveAuto			= ".auto.ssgz";
		::gpk::vcst_t				ExtensionSaveStage			= ".stage.ssgz";
		::gpk::vcst_t				ExtensionSaveQuick			= ".quick.ssgz";

		gpk::err_t			GetSaveGameExtension		(SAVE_MODE saveMode, ::gpk::vcsc_t & extension)	const	{
			switch(saveMode) {
			case SAVE_MODE_USER		: extension = ExtensionSaveUser	; break;
			case SAVE_MODE_STAGE	: extension = ExtensionSaveStage; break;
			case SAVE_MODE_QUICK	: extension = ExtensionSaveQuick; break;
			case SAVE_MODE_AUTO		: extension = ExtensionSaveAuto	; break;
			}
			return 0;
		}

		gpk::err_t			GetPlayerFilePath			(const u3_t time, ::gpk::vcst_t playerName, ::gpk::asc_t & playerFilePath)	const	{
			::gpk::string			b64PlayerName				= {};
			if_fail_fe(::gpk::base64Encode(playerName, b64PlayerName));
			if_fail_fe(::gpk::append_strings(b64PlayerName, time, ExtensionProfile));
			if_fail_fe(::gpk::pathNameCompose(SavegameFolder, b64PlayerName, playerFilePath));
			return 0;
		}
	};

	struct SSiegeUI {
		::gpk::cid_t			Root				= ::gpk::CID_INVALID;
		::gpk::SInputBox		UserInput			= {};
		::gpk::acid				RootPerState		= {};
		::gpk::apobj<string>	InputHistory		= {};
		::gpk::apobj<string>	InputQueue			= {};
	};

	gpk::err_t			setupSSiegeUI			(::gpk::SGUI & gui, ::ssg::SSiegeUI & ssiegeUI);


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
	GDEFINE_ENUM_VALUE(APP_STATE, Quit		, 13);

	struct SPlayer {
		ssiegeid_t					Id					= SSIEGEID_INVALID;
		::gpk::vcst_t				User				= {};
		::gpk::vcst_t				Name				= {};
	};

	struct SPlayerState {
		u2_t				Stage				= 0;
	};

	struct SSiegeApp {
		APP_STATE				ActiveState			= {};

		::ssg::SSiegeGame		Game;

		//SWorldView				World				= {};

		SSiegeUI				UI					= {};
		gpk::apobj<EventSSiege>	EventQueue			= {};

		SFileStrings			FileStrings			= {};


		::gpk::pobj<gpk::SGUI>	GUI;
		::gpk::prtbgra8d32		RenderTargetWorld;
		::gpk::prtbgra8d32		RenderTargetUI;

		gpk::err_t			StateSwitch			(APP_STATE newState);

		gpk::err_t			Save				()	const	{
			if_true_vif(0, Game.Controlled >= Game.Engine.Entities.size(), "No active player character! %i >= %i", Game.Controlled, Game.Engine.Entities.size());
				
			::gpk::string				playerPath;
			if_fail_fe(FileStrings.GetPlayerFilePath(::gpk::timeCurrent(), Game.Players[0].Name, playerPath));
			return Save(playerPath);
		}

		gpk::err_t			Save				(::gpk::vcst_t fileName)	const	{
			::gpk::au0_t					serialized;
			if_fail_fe(Game.World.Save(serialized));
			return ::gpk::deflateFromMemory(fileName, serialized);
		}

		gpk::err_t			Load				(::gpk::vcst_t filename)			{
			::gpk::au0_t					serialized;
			if_fail_fe(::gpk::inflateToMemory(filename, serialized));
			::gpk::vcu0_t					viewSerialized		= serialized;
			return Game.World.Load(viewSerialized);
		}
	};

	gpk::err_t			ssiegeUpdate		(::ssg::SSiegeApp & ssiege, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents
		, ::gpk::FBool<::gpk::pobj<::ssg::EventSSiege> &, ::gpk::apobj<::ssg::EventSSiege> &> funcHandleEvent
		);
	gpk::err_t			ssiegeDraw			(::ssg::SSiegeApp & ssiege, ::gpk::rtbgra8d32 & backBuffer, bool onlyGUI);
} // namespace ssg

#endif // SSIEGE_H_23701
