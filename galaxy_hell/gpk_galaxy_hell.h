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
	
	stacxpr uint16_t	MAX_PLAYERS				= 4;
	stacxpr uint32_t	MAX_ORBITER_COUNT		= 6;

#pragma pack(push, 1)
	stacxpr ::gpk::astatic<::gpk::bgra, 16>	PLAYER_COLORS	= 
		{ 0xFF00FF00U	// GREEN
		, ::gpk::PURPLE
		, 0xFF0000FFU	// BLUE
		, 0xFFFFFF00U	// CYAN
		, 0xFFFF00FFU	// MAGENTA
		, 0xFF00FFFFU	// YELLOW
		, 0xFFFF0000U	// RED
		};

	GDEFINE_ENUM_TYPE(PLAY_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Survival		, 0);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Campaign		, 1);
	GDEFINE_ENUM_VALUE(PLAY_TYPE, Deathmatch	, 2);

	GDEFINE_ENUM_TYPE(PLAY_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(PLAY_MODE, Classic		, 0);
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

		stacxpr	float			Slowing					= -.35f;
		stacxpr	float			Fasting					= .45f;
	};
	 
	struct SShipController {
		uint8_t				Forward					: 1;
		uint8_t				Back					: 1;
		uint8_t				Left					: 1;
		uint8_t				Right					: 1;
		uint8_t				Turbo					: 1;
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

		::gpk::error_t						PilotCreate				(const ::ghg::SShipPilot & shipPilot)			{
			gpk_necs(Pilots.push_back(shipPilot));
			return ShipControllers.push_back({});
		}

		::gpk::error_t						PilotsReset				()			{
			while(Pilots.size() < PlayState.Constants.Players) {
				sc_t									text [64]				= {};
				sprintf_s(text, "Player %i", Pilots.size() + 1);
				gpk_necs(Pilots.push_back({::gpk::label(text), PLAYER_COLORS[Pilots.size() % ::gpk::size(PLAYER_COLORS)]}));
			}
			return ShipControllers.resize(PlayState.Constants.Players);
		}

		::gpk::error_t						Save					(::gpk::au8 & output)		const	{
			gpk_necs(::gpk::savePOD(output, PlayState));
			for(uint32_t iPlayer = 0; iPlayer < PlayState.Constants.Players; ++iPlayer) {
				gpk_necs(::gpk::saveView(output, Pilots[iPlayer].Name));
				gpk_necs(::gpk::savePOD(output, Pilots[iPlayer].Color));
			}
			gpk_necs(::gpk::saveView(output, ShipControllers));
			gpk_necs(ShipState.Save(output));
			return 0;
		}

		::gpk::error_t						Load					(::gpk::vcu8 & input) {
			::std::lock_guard						lock(LockUpdate);
			::gpk::view<const ::ghg::SPlayState>	readPlayState			= {};
			gpk_necs(::gpk::loadPOD(input, PlayState));
			gpk_necs(Pilots.resize(PlayState.Constants.Players));
			for(uint32_t iPlayer = 0; iPlayer < Pilots.size(); ++iPlayer) {
				gpk_necall(::gpk::loadLabel(input, Pilots[iPlayer].Name), "iPlayer %i", iPlayer);
				gpk_necall(::gpk::loadPOD(input, Pilots[iPlayer].Color), "iPlayer %i", iPlayer);
			}
			ShipControllers.clear();
			gpk_necs(::gpk::loadView(input, ShipControllers));
			gpk_necs(ShipState.Load(input));
			return 0;
		}
	};

	::gpk::error_t		stageSetup				(::ghg::SGalaxyHell & solarSystem);
	::gpk::error_t		solarSystemSetup		(::ghg::SGalaxyHell & solarSystem, const ::gpk::n2u16 & windowSize);
	::gpk::error_t		solarSystemReset		(::ghg::SGalaxyHell & solarSystem); 
	::gpk::error_t		solarSystemDraw			(const ::ghg::SGalaxyHell & solarSystem, ::ghg::SGalaxyHellDrawCache & drawCache, ::std::mutex & lockUpdate);
	::gpk::error_t		solarSystemUpdate		(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame, const ::gpk::SInput & input, ::gpk::vpobj<::gpk::SEventSystem> frameEvents);
	::gpk::error_t		solarSystemLoad			(::ghg::SGalaxyHell & world,::gpk::vcc filename);
	::gpk::error_t		solarSystemSave			(const ::ghg::SGalaxyHell & world,::gpk::vcc filename);
	
	::gpk::error_t		getLightArraysFromDebris
		( const ::ghg::SDecoState	& decoState
		, ::gpk::an3f32				& lightPoints
		, ::gpk::a8bgra				& lightColors
		, const ::gpk::vc8bgra		& debrisColors
		);

	::gpk::error_t		getLightArraysFromShips
		( const ::ghg::SShipManager	& shipState
		, ::gpk::an3f32				& lightPoints
		, ::gpk::a8bgra				& lightColors
		);

	::gpk::error_t		getLightArrays
		( const ::ghg::SShipManager	& shipState
		, const ::ghg::SDecoState	& decoState
		, ::gpk::an3f32				& lightPoints
		, ::gpk::a8bgra				& lightColors
		, const ::gpk::vc8bgra		& debrisColors
		);

	::gpk::error_t		drawOrbiter
		( const ::ghg::SShipManager		& shipState
		, const uint32_t				iPartEntity
		, const ::gpk::rgbaf			& shipColor	
		, const float					animationTime
		, const ::gpk::m4f32			& matrixVP
		, ::gpk::g8bgra					& targetPixels
		, ::gpk::gu32					depthBuffer
		, ::ghg::SGalaxyHellDrawCache	& drawCache
		);
}

#endif // GPK_GALAXY_HELL_H_293874239874
		  