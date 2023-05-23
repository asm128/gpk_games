#include "gpk_galaxy_hell_ships.h"

#include "gpk_galaxy_hell_deco.h"

#include "gpk_galaxy_hell_draw.h"


#include "gpk_input.h"
#include "gpk_sysevent.h"
#include "gpk_label.h"

#include <mutex>

#ifndef GPK_GALAXY_HELL_H_293874239874
#define GPK_GALAXY_HELL_H_293874239874

namespace ghg
{
	static constexpr uint16_t								MAX_PLAYERS			= 4;
	static constexpr uint32_t								MAX_ORBITER_COUNT	= 6;

#pragma pack(push, 1)
	static constexpr ::gpk::SColorBGRA						PLAYER_COLORS[]		= 
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
		uint64_t												TimeStart				= 0;
		uint64_t												TimeLast				= 0;
		uint32_t												Seed					= 1;
		uint16_t												OffsetStage				= 2;
		uint8_t													CountPlayers			= 1;
		PLAY_TYPE												PlayType				= (PLAY_TYPE)0;
		PLAY_MODE												PlayMode				= (PLAY_MODE)PLAY_MODE_VR;

		uint32_t												Stage					= 0;
		double													TimeStage				= 0;
		double													TimeWorld				= 0;
		double													TimeRealStage			= 0;
		double													TimeReal				= 0;
		bool													Paused					= false;

		double													CameraSwitchDelay		= 0;

		double													RelativeSpeedTarget		= 20;
		double													RelativeSpeedCurrent	= -50;
		int														AccelerationControl		= 0;

		double													TimeScale				= 1.0f;
		bool													Slowing					= true;

	};
	 
	struct SShipController {
		uint8_t					Forward				: 1;
		uint8_t					Back				: 1;
		uint8_t					Left				: 1;
		uint8_t					Right				: 1;
		uint8_t					Turbo				: 1;
		::gpk::n3<int16_t>	PointerDeltas		= {};
		::gpk::n3<int16_t>	PointerPosition		= {};
	};
#pragma pack(pop)

	struct SShipPilot {
		::gpk::vcc												Name					= "Evil Dead";
		::gpk::SColorBGRA										Color					= ::gpk::MAGENTA;
	};

	struct SGalaxyHell {
		::ghg::SShipManager										ShipState				= {};
		::ghg::SDecoState										DecoState				= {};	
		::ghg::SPlayState										PlayState				= {};
		::gpk::aobj<::ghg::SShipPilot>							Pilots					= {};
		::gpk::apod<::ghg::SShipController>						ShipControllers			= {};
		
		::ghg::SGalaxyHellDrawCache								DrawCache;
		::std::mutex											LockUpdate;

		::gpk::error_t											PilotCreate				(const ::ghg::SShipPilot & shipPilot)			{
			Pilots.push_back(shipPilot);
			return ShipControllers.push_back({});
		}

		::gpk::error_t											PilotsReset				()			{
			while(Pilots.size() < PlayState.CountPlayers) {
				char														text [64]				= {};
				sprintf_s(text, "Player %i", Pilots.size() + 1);
				Pilots.push_back({::gpk::label(text), PLAYER_COLORS[Pilots.size() % ::gpk::size(PLAYER_COLORS)]});
			}
			return ShipControllers.resize(PlayState.CountPlayers);
		}

		::gpk::error_t											Save					(::gpk::au8 & output)		const	{
			gpk_necs(::gpk::savePOD(output, PlayState));
			for(uint32_t iPlayer = 0; iPlayer < PlayState.CountPlayers; ++iPlayer) {
				::gpk::saveView(output, Pilots[iPlayer].Name);
				::gpk::savePOD(output, Pilots[iPlayer].Color);
			}

			::gpk::saveView(output, ShipControllers);
			ShipState.Save(output);
			return 0;
		}
		::gpk::error_t											Load					(::gpk::vcu8 & input) {
			::std::lock_guard											lock(LockUpdate);
			::gpk::view_array<const ::ghg::SPlayState>					readPlayState			= {};
			gpk_necs(::gpk::loadPOD(input, PlayState));
			Pilots.resize(PlayState.CountPlayers);
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

	::gpk::error_t										stageSetup						(::ghg::SGalaxyHell & solarSystem);
	::gpk::error_t										solarSystemSetup				(::ghg::SGalaxyHell & solarSystem, const ::gpk::n2<uint16_t> & windowSize);
	::gpk::error_t										solarSystemReset				(::ghg::SGalaxyHell & solarSystem); 
	::gpk::error_t										solarSystemDraw					(const ::ghg::SGalaxyHell & solarSystem, ::ghg::SGalaxyHellDrawCache & drawCache, ::std::mutex & lockUpdate);
	::gpk::error_t										solarSystemUpdate				(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame, const ::gpk::SInput & input, const ::gpk::view_array<::gpk::SSysEvent> & frameEvents);
	::gpk::error_t										solarSystemLoad					(::ghg::SGalaxyHell & world,::gpk::vcc filename);
	::gpk::error_t										solarSystemSave					(const ::ghg::SGalaxyHell & world,::gpk::vcc filename);
	
	::gpk::error_t										getLightArraysFromDebris
		( const ::ghg::SDecoState								& decoState
		, ::gpk::apod<::gpk::n3<float>>				& lightPoints
		, ::gpk::apod<::gpk::bgra>					& lightColors
		, const ::gpk::view_array<const ::gpk::SColorBGRA>		& debrisColors
		);
	::gpk::error_t										getLightArraysFromShips
		( const ::ghg::SShipManager								& shipState
		, ::gpk::apod<::gpk::n3<float>>				& lightPoints
		, ::gpk::apod<::gpk::bgra>					& lightColors
		);
	::gpk::error_t										getLightArrays
		( const ::ghg::SShipManager								& shipState
		, const ::ghg::SDecoState								& decoState
		, ::gpk::apod<::gpk::n3<float>>				& lightPoints
		, ::gpk::apod<::gpk::bgra>					& lightColors
		, const ::gpk::view_array<const ::gpk::SColorBGRA>		& debrisColors
		);

	::gpk::error_t										drawOrbiter
		( const ::ghg::SShipManager							& shipState
		, const ::ghg::SOrbiter								& shipPart
		, const ::gpk::SColorFloat							& shipColor	
		, float												animationTime
		, const ::gpk::SMatrix4<float>						& matrixVP
		, ::gpk::view2d<::gpk::bgra>					& targetPixels
		, ::gpk::view2d<uint32_t>							depthBuffer
		, ::ghg::SGalaxyHellDrawCache						& drawCache
		);
}

#endif // GPK_GALAXY_HELL_H_293874239874
		  