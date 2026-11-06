#include "gpk_galaxy_hell_ships.h"

#include "gpk_galaxy_hell_deco.h"

#include "gpk_galaxy_hell_draw.h"

#include "gpk_game.h"

#include "gpk_view_color.h"

#include "gpk_input.h"
#include "gpk_label.h"

#include "gpk_system_event.h"

#include <mutex>

#ifndef GPK_GALAXY_HELL_H_293874239874
#define GPK_GALAXY_HELL_H_293874239874

namespace ghg
{
	GPK_USING_TYPEINT();
	
	stxp u1_t	MAX_PLAYERS				= 4;
	stxp u2_t	MAX_ORBITER_COUNT		= 6;

#pragma pack(push, 1)
	stxp ::gpk::astatic<::gpk::bgra, 16>	PLAYER_COLORS	= 
		{ 0xFF00FF00U	// GREEN
		, ::gpk::PURPLE
		, 0xFF0000FFU	// BLUE
		, 0xFFFFFF00U	// CYAN
		, 0xFFFF00FFU	// MAGENTA
		, 0xFF00FFFFU	// YELLOW
		, 0xFFFF0000U	// RED
		};

	GDEFINE_ENUM_TYPE(PLAY_TYPE, u0_t);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, FreeRoam		, 0);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Campaign		, 1);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Deathmatch	, 2);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, CTF			, 3);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Speedrun		, 4);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Ambush		, 5);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Circuit		, 6);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Defense		, 7);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Capture		, 8);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Assassination	, 9);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Worldscape	, 0xA);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Skyscape		, 0xB);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Seascape		, 0xC);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Orbitscape	, 0xD);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Systemscape	, 0xE);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Tactical		, 0xF);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Interception	, 0x10);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Excavation	, 0x11);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Rescue		, 0x12);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Sabotage		, 0x13);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Spy			, 0x14);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Extermination	, 0x15);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Assault		, 0x16);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Duel			, 0x17);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Hive			, 0x18);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Disruption	, 0x19);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Survival		, 0x1A);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Escort		, 0x1B);
	//GDEFINE_ENUM_VALUE(PLAY_TYPE, , 0x1C);
	//GDEFINE_ENUM_VALUE(PLAY_TYPE, , 0x1D);
	//GDEFINE_ENUM_VALUE(PLAY_TYPE, , 0x1E);
	//GDEFINE_ENUM_VALUE(PLAY_TYPE, , 0x1F);

	GDEFINE_ENUM_TYPE(PLAY_MODE, u0_t);
	GDEFINE_ENUM_VALUE(PLAY_MODE, Arcade		, 0);
	GDEFINE_ENUM_VALUE(PLAY_MODE, VR			, 1);

	struct SPlayState {
		::gpk::SGameSetup		PlaySetup				= {2};
		PLAY_MODE				PlayMode				= (PLAY_MODE)PLAY_MODE_VR;

		double					TimeStage				= 0;
		double					TimeRealStage			= 0;

		::gpk::SGameState		GlobalState;

		::gpk::SStageSetup		Constants				= {1, 1};
		::gpk::SRelativeSpeed	BackgroundSpeed;
		::gpk::SRelativeTime	SimulatedTime;

		stxp	float			Slowing					= -.35f;
		stxp	float			Fasting					= .45f;
	};
	 
	struct SShipController {
		u0_t				Forward					: 1;
		u0_t				Back					: 1;
		u0_t				Left					: 1;
		u0_t				Right					: 1;
		u0_t				Turbo					: 1;
	};
#pragma pack(pop)

	struct SShipPilot : ::gpk::SPlayer {};

	struct SGalaxyHell {
		::ghg::SShipManager					ShipState				= {};
		::ghg::SDecoState					DecoState				= {};	
		::ghg::SPlayState					PlayState				= {};
		::gpk::aobj<::ghg::SShipPilot>		Pilots					= {};
		::gpk::apod<::ghg::SShipController>	ShipControllers			= {};

		::ghg::SGalaxyHellDrawCache			DrawCache;
		::std::mutex						LockUpdate;

		gpk::err_t						PilotCreate				(const ::ghg::SShipPilot & shipPilot)			{
			if_fail_fe(Pilots.push_back(shipPilot));
			return ShipControllers.push_back({});
		}

		gpk::err_t						PilotsReset				()			{
			while(Pilots.size() < PlayState.Constants.Players) {
				sc_t									text [64]				= {};
				::sprintf_s(text, "Player %i", Pilots.size() + 1);
				if_fail_fe(Pilots.push_back({::gpk::label(text), PLAYER_COLORS[Pilots.size() % ::gpk::size(PLAYER_COLORS)]}));
			}
			return ShipControllers.resize(PlayState.Constants.Players);
		}

		gpk::err_t						Save					(::gpk::au0_t & output)		const	{
			if_fail_fe(::gpk::savePOD(output, PlayState));
			for(u2_t iPlayer = 0; iPlayer < PlayState.Constants.Players; ++iPlayer) {
				if_fail_fe(::gpk::saveView(output, Pilots[iPlayer].Name));
				if_fail_fe(::gpk::savePOD(output, Pilots[iPlayer].Color));
			}
			if_fail_fe(::gpk::saveView(output, ShipControllers));
			if_fail_fe(ShipState.Save(output));
			return 0;
		}

		gpk::err_t						Load					(::gpk::vcu0_t & input) {
			::std::lock_guard						lock(LockUpdate);
			::gpk::view<const ::ghg::SPlayState>	readPlayState			= {};
			if_fail_fe(::gpk::loadPOD(input, PlayState));
			if_fail_fe(Pilots.resize(PlayState.Constants.Players));
			for(u2_t iPlayer = 0; iPlayer < Pilots.size(); ++iPlayer) {
				if_fail_fef(::gpk::loadLabel(input, Pilots[iPlayer].Name), "iPlayer %i", iPlayer);
				if_fail_fef(::gpk::loadPOD(input, Pilots[iPlayer].Color), "iPlayer %i", iPlayer);
			}
			ShipControllers.clear();
			if_fail_fe(::gpk::loadView(input, ShipControllers));
			if_fail_fe(ShipState.Load(input));
			return 0;
		}
	};

	gpk::err_t		stageSetup				(::ghg::SGalaxyHell & solarSystem);
	gpk::err_t		solarSystemSetup		(::ghg::SGalaxyHell & solarSystem, const ::gpk::n2u1_t & windowSize);
	gpk::err_t		solarSystemReset		(::ghg::SGalaxyHell & solarSystem); 
	gpk::err_t		solarSystemDraw			(const ::ghg::SGalaxyHell & solarSystem, ::ghg::SGalaxyHellDrawCache & drawCache, ::std::mutex & lockUpdate);
	gpk::err_t		solarSystemUpdate		(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame, const ::gpk::SInput & input, ::gpk::vpobj<::gpk::SEventSystem> frameEvents);
	gpk::err_t		solarSystemLoad			(::ghg::SGalaxyHell & world,::gpk::vcsc_t filename);
	gpk::err_t		solarSystemSave			(const ::ghg::SGalaxyHell & world,::gpk::vcsc_t filename);
	
	gpk::err_t		getLightArraysFromDebris
		( const ::ghg::SDecoState	& decoState
		, ::gpk::a3f2_t				& lightPoints
		, ::gpk::a8bgra				& lightColors
		, const ::gpk::vc8bgra		& debrisColors
		);

	gpk::err_t		getLightArraysFromShips
		( const ::ghg::SShipManager	& shipState
		, ::gpk::a3f2_t				& lightPoints
		, ::gpk::a8bgra				& lightColors
		);

	gpk::err_t		getLightArrays
		( const ::ghg::SShipManager	& shipState
		, const ::ghg::SDecoState	& decoState
		, ::gpk::a3f2_t				& lightPoints
		, ::gpk::a8bgra				& lightColors
		, const ::gpk::vc8bgra		& debrisColors
		);

	gpk::err_t		drawOrbiter
		( const ::ghg::SShipManager		& shipState
		, const u2_t				iPartEntity
		, const ::gpk::rgbaf			& shipColor	
		, const float					animationTime
		, const ::gpk::m4f2_t			& matrixVP
		, ::gpk::g8bgra					& targetPixels
		, ::gpk::gu2_t					depthBuffer
		, ::ghg::SGalaxyHellDrawCache	& drawCache
		);
}

#endif // GPK_GALAXY_HELL_H_293874239874

