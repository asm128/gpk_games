#include "lls_game.h"

#include "gpk_img_serialize.h"
#include "gpk_engine.h"
#include "gpk_gui.h"
#include "gpk_gui_inputbox.h"
#include "gpk_dialog.h"
#include "gpk_deflate.h"
#include "gpk_array_ptr.h"
#include "gpk_noise.h"

#include <ctime>

#ifndef SSIEGE_H_23701
#define SSIEGE_H_23701

namespace lls
{
	GDEFINE_ENUM_TYPE (UI_PROFILE, uint8_t);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Name , 0);
	GDEFINE_ENUM_VALUE(UI_PROFILE, Score, 1);

	struct SUIPlayer {
		::gpk::SDialog			DialogHome			= {};
		::gpk::acid				ControlsDialogHome	= {};
		::gpk::SInputBox		InputBox			= {};
		::gpk::astc<128>		TextScore			= {};
	};

	struct SUIPlay {
		::gpk::aobj<::lls::SUIPlayer>	PlayerUI		= {};
	};

	enum SAVE_MODE { SAVE_MODE_AUTO, SAVE_MODE_QUICK, SAVE_MODE_STAGE, SAVE_MODE_USER };

	struct SFileStrings {
		::gpk::vcsc_t				SavegameFolder				= GPK_CXS(".");
		::gpk::vcsc_t				ExtensionImages				= GPK_CXS(".png");
		::gpk::vcsc_t				ExtensionProfile			= GPK_CXS(".lls");
		::gpk::vcsc_t				ExtensionCredentials		= GPK_CXS(".llsc");
		::gpk::vcsc_t				ExtensionSave				= GPK_CXS(".llsz");
		::gpk::vcsc_t				ExtensionSaveUser			= GPK_CXS(".user.llsz");
		::gpk::vcsc_t				ExtensionSaveAuto			= GPK_CXS(".auto.llsz");
		::gpk::vcsc_t				ExtensionSaveStage			= GPK_CXS(".stage.llsz");
		::gpk::vcsc_t				ExtensionSaveQuick			= GPK_CXS(".quick.llsz");

		::gpk::error_t			GetSaveGameExtension		(SAVE_MODE saveMode, ::gpk::vcsc_t & extension)	const	{
			switch(saveMode) {
			case SAVE_MODE_USER		: extension = ExtensionSaveUser	; break;
			case SAVE_MODE_STAGE	: extension = ExtensionSaveStage; break;
			case SAVE_MODE_QUICK	: extension = ExtensionSaveQuick; break;
			case SAVE_MODE_AUTO		: extension = ExtensionSaveAuto	; break;
			}
			return 0;
		}

		::gpk::error_t			GetPlayerFilePath			(const uint64_t time, const ::gpk::vcsc_t & playerName, ::gpk::asc_t & playerFilePath)	const	{
			::gpk::au0_t					b64PlayerName				= {};
			gpk_necs(::gpk::base64Encode(playerName, b64PlayerName));
			gpk_necs(playerFilePath.append(SavegameFolder));
			gpk_necs(playerFilePath.append_string("/"));
			gpk_necs(playerFilePath.append(b64PlayerName.cc()));

			sc_t						postfix[64]					= {};
			sprintf_s(postfix, "%llu%s", time, ExtensionProfile.begin());
			return playerFilePath.append_string(postfix);
		}
	};

	struct LLSUI {
		::gpk::cid_t			Root				= ::gpk::CID_INVALID;
		::gpk::SInputBox		UserInput			= {};
		::gpk::acid				RootPerState		= {};
		::gpk::apachar			InputHistory		= {};
		::gpk::apachar			InputQueue			= {};
	};

	::gpk::error_t			setupLLSUI			(::gpk::SGUI & gui, ::lls::LLSUI & llsUI);


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

	struct SPlayer {
		llsid_t					Id					= SSIEGEID_INVALID;
		::gpk::vcsc_t				User				= {};
		::gpk::vcsc_t				Name				= {};
	};

	struct SPlayerState {
		uint32_t				Stage				= 0;
	};

	struct LLSApp {
		APP_STATE				ActiveState			= {};

		::lls::LLSGame		Game;

		//SWorldView				World				= {};

		LLSUI				UI					= {};
		gpk::apobj<EventLLS>	EventQueue			= {};

		SFileStrings			FileStrings			= {};


		::gpk::pobj<gpk::SGUI>	GUI;
		::gpk::prtbgra8d32		RenderTargetWorld;
		::gpk::prtbgra8d32		RenderTargetUI;

		::gpk::error_t			StateSwitch			(APP_STATE newState);

		::gpk::error_t			Save				()	const	{
			rni_if(Game.Controlled >= Game.Engine.Entities.size(), "No active player character! %i >= %i", Game.Controlled, Game.Engine.Entities.size());
				
			::gpk::asc_t				playerPath;
			gpk_necs(FileStrings.GetPlayerFilePath(::gpk::timeCurrent(), Game.Players[0].Name, playerPath));
			return Save(playerPath);
		}

		::gpk::error_t			Save				(::gpk::vcsc_t fileName)	const	{
			::gpk::au0_t					serialized;
			gpk_necs(Game.World.Save(serialized));
			return ::gpk::deflateFromMemory(fileName, serialized);
		}

		::gpk::error_t			Load				(::gpk::vcsc_t filename)			{
			::gpk::au0_t					serialized;
			gpk_necs(::gpk::inflateToMemory(filename, serialized));
			::gpk::vcu0_t					viewSerialized		= serialized;
			return Game.World.Load(viewSerialized);
		}
	};

	::gpk::error_t			llsUpdate		(::lls::LLSApp & lls, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents
		, ::gpk::FBool<::gpk::pobj<::lls::EventLLS> &, ::gpk::apobj<::lls::EventLLS> &> funcHandleEvent
		);
	::gpk::error_t			llsDraw			(::lls::LLSApp & lls, ::gpk::rtbgra8d32 & backBuffer, bool onlyGUI);
} // namespace lls

#endif // SSIEGE_H_23701
