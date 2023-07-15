#include "ssiege_game.h"

#include "gpk_img_serialize.h"
#include "gpk_engine.h"
#include "gpk_gui.h"
#include "gpk_gui_inputbox.h"
#include "gpk_deflate.h"
#include "gpk_array_ptr.h"
#include "gpk_noise.h"

#include <ctime>

#ifndef SSIEGE_H_23701
#define SSIEGE_H_23701

namespace ssiege
{

	enum SAVE_MODE { SAVE_MODE_AUTO, SAVE_MODE_QUICK, SAVE_MODE_STAGE, SAVE_MODE_USER };

	struct SFileStrings {
		::gpk::vcc				SavegameFolder				= {1, "."};
		::gpk::vcc				ExtensionImages				= {4, ".png"};
		::gpk::vcc				ExtensionProfile			= {6, ".ssg"};
		::gpk::vcc				ExtensionCredentials		= {6, ".ssgc"};
		::gpk::vcc				ExtensionSave				= {6, ".ssgz"};
		::gpk::vcc				ExtensionSaveUser			= {11, ".user.ssgz"};
		::gpk::vcc				ExtensionSaveAuto			= {11, ".auto.ssgz"};
		::gpk::vcc				ExtensionSaveStage			= {12, ".stage.ssgz"};
		::gpk::vcc				ExtensionSaveQuick			= {12, ".quick.ssgz"};

		::gpk::error_t			GetSaveGameExtension		(SAVE_MODE saveMode, ::gpk::vcc & extension)	const	{
			switch(saveMode) {
			case SAVE_MODE_USER		: extension = ExtensionSaveUser		; break;
			case SAVE_MODE_STAGE	: extension = ExtensionSaveStage	; break;
			case SAVE_MODE_QUICK	: extension = ExtensionSaveQuick	; break;
			case SAVE_MODE_AUTO		: extension = ExtensionSaveAuto		; break;
			}
			return 0;
		}

		::gpk::error_t			GetPlayerFilePath			(const uint64_t time, const ::gpk::vcc & playerName, ::gpk::achar & playerFilePath)	const	{
			::gpk::au8					b64PlayerName				= {};
			gpk_necs(::gpk::base64Encode(playerName, b64PlayerName));
			gpk_necs(playerFilePath.append(SavegameFolder));
			gpk_necs(playerFilePath.append_string("/"));
			gpk_necs(playerFilePath.append(b64PlayerName.cc()));

			char						postfix[64]					= {};
			sprintf_s(postfix, "%llu%s", time, ExtensionProfile.begin());
			return playerFilePath.append_string(postfix);
		}
	};

	struct SCamera {
		::gpk::n3f32			Offset				= {-0.5f, .05f, -.125f};
		::gpk::n3f32			Target				= {0, 0, 0};
		float					Zoom				= 2.0f;
	};

	struct SSSiegeUI {
		::gpk::cid_t			Root				= ::gpk::CID_INVALID;
		::gpk::SInputBox		UserInput			= {};
		::gpk::acid				RootPerState		= {};
		::gpk::apachar			InputHistory		= {};
		::gpk::apachar			InputQueue			= {};
	};

	::gpk::error_t			setupSSiegeUI			(::gpk::SGUI & gui, ::ssiege::SSSiegeUI & ssiegeUI);


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
		ssiegeid_t				Id					= SSIEGEID_INVALID;
		::gpk::vcc				User				= {};
		::gpk::vcc				Name				= {};
	};

	struct SPlayerState {
		uint32_t				Stage				= 0;
	};

	struct SSSiegeApp {
		APP_STATE				ActiveState			= {};

		::ssg::SSiegeGame		Game;

		//SWorldView				World				= {};

		SSSiegeUI				UI					= {};
		gpk::apobj<EventSSiege>	EventQueue			= {};

		SFileStrings			FileStrings			= {};

		SCamera					Camera				= {};

		ssiegeid_t				CharacterControlled	= SSIEGEID_INVALID;
		::gpk::aobj<::ssiege::SPlayer>	Players;

		::gpk::pobj<gpk::SGUI>	GUI;
		::gpk::prtbgra8d32		RenderTargetWorld;
		::gpk::prtbgra8d32		RenderTargetUI;

		::gpk::error_t			StateSwitch			(APP_STATE newState);

		::gpk::error_t			Save				()	const	{
			rni_if(CharacterControlled >= Game.World.Characters.size(), "No active player character! %i >= %i", CharacterControlled, Game.World.Characters.size());
				
			::gpk::achar				playerPath;
			gpk_necs(FileStrings.GetPlayerFilePath(::gpk::timeCurrent(), Players[0].Name, playerPath));
			return Save(playerPath);
		}

		::gpk::error_t			Save				(::gpk::vcc fileName)	const	{
			::gpk::au8					serialized;
			gpk_necs(Game.World.Save(serialized));
			return ::gpk::deflateFromMemory(fileName, serialized);
		}

		::gpk::error_t			Load				(::gpk::vcc filename)			{
			::gpk::au8					serialized;
			gpk_necs(::gpk::inflateToMemory(filename, serialized));
			::gpk::vcu8					viewSerialized		= serialized;
			return Game.World.Load(viewSerialized);
		}
	};

	::gpk::error_t			ssiegeUpdate		(::ssiege::SSSiegeApp & ssiege, double lastTimeSeconds, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents
		, ::gpk::FBool<::gpk::pobj<::ssiege::EventSSiege> &, ::gpk::apobj<::ssiege::EventSSiege> &> funcHandleEvent
		);
	::gpk::error_t			ssiegeDraw			(::ssiege::SSSiegeApp & ssiege, ::gpk::rtbgra8d32 & backBuffer, bool onlyGUI);
} // namespace ssiege

#endif // SSIEGE_H_23701
