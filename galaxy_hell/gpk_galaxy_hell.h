#include "gpk_galaxy_hell_draw.h"

#include "gpk_galaxy_hell_deco.h"
#include "gpk_galaxy_hell_ships.h"

#include "gpk_input.h"
#include "gpk_sysevent.h"

#include <mutex>

#ifndef GPK_GALAXY_HELL_H_293874239874
#define GPK_GALAXY_HELL_H_293874239874

namespace ghg
{
#pragma pack(push, 1)
	struct SPlayState {
		uint64_t												TimeStart				= 0;
		uint64_t												TimeLast				= 0;
		uint32_t												Seed					= 1;
		uint32_t												OffsetStage				= 2;
		uint32_t												PlayerCount				= 2;

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

#pragma pack(pop)

	struct SGalaxyHell {
		::ghg::SShipState										ShipState				= {};
		::ghg::SDecoState										DecoState				= {};	
		::ghg::SPlayState										PlayState				= {};
		
		::ghg::SGalaxyHellDrawCache								DrawCache;
		::std::mutex											LockUpdate;

		::gpk::error_t											Save					(::gpk::array_pod<byte_t> & output) const {
			::gpk::viewWrite(::gpk::view_array<const ::ghg::SPlayState>{&PlayState, 1}, output);
			ShipState.Save(output);
			return 0;
		}
		::gpk::error_t											Load					(::gpk::view_array<const byte_t> & input) {
			::gpk::view_array<const ::ghg::SPlayState>					readPlayState			= {};
			int32_t bytesRead = ::gpk::viewRead(readPlayState, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; PlayState	= readPlayState[0];
			ShipState.Load(input);
			return 0;
		}

	};

	::gpk::error_t										stageSetup						(::ghg::SGalaxyHell & solarSystem);
	::gpk::error_t										solarSystemSetup				(::ghg::SGalaxyHell & solarSystem, const ::gpk::SCoord2<uint16_t> & windowSize);
	::gpk::error_t										solarSystemReset				(::ghg::SGalaxyHell & solarSystem); 
	::gpk::error_t										solarSystemDraw					(const ::ghg::SGalaxyHell & solarSystem, ::ghg::SGalaxyHellDrawCache & drawCache, ::std::mutex & lockUpdate);
	::gpk::error_t										solarSystemUpdate				(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame, const ::gpk::SInput & input, const ::gpk::view_array<::gpk::SSysEvent> & frameEvents);
	::gpk::error_t										solarSystemLoad					(::ghg::SGalaxyHell & world,::gpk::vcc filename);
	::gpk::error_t										solarSystemSave					(const ::ghg::SGalaxyHell & world,::gpk::vcc filename);
	
	::gpk::error_t										getLightArraysFromDebris
		( const ::ghg::SDecoState								& decoState
		, ::gpk::array_pod<::gpk::SCoord3<float>>				& lightPoints
		, ::gpk::array_pod<::gpk::SColorBGRA>					& lightColors
		, const ::gpk::view_array<const ::gpk::SColorBGRA>		& debrisColors
		);
	::gpk::error_t										getLightArraysFromShips
		( const ::ghg::SShipState								& shipState
		, ::gpk::array_pod<::gpk::SCoord3<float>>				& lightPoints
		, ::gpk::array_pod<::gpk::SColorBGRA>					& lightColors
		);
	::gpk::error_t										getLightArrays
		( const ::ghg::SShipState								& shipState
		, const ::ghg::SDecoState								& decoState
		, ::gpk::array_pod<::gpk::SCoord3<float>>				& lightPoints
		, ::gpk::array_pod<::gpk::SColorBGRA>					& lightColors
		, const ::gpk::view_array<const ::gpk::SColorBGRA>		& debrisColors
		);

	::gpk::error_t										drawShipOrbiter
		( const ::ghg::SShipState							& shipState
		, const ::ghg::SOrbiter								& shipPart
		, const ::gpk::SMatrix4<float>						& matrixVP
		, ::gpk::view_grid<::gpk::SColorBGRA>				& targetPixels
		, ::gpk::view_grid<uint32_t>						depthBuffer
		, ::ghg::SGalaxyHellDrawCache						& drawCache
		);
}

#endif // GPK_GALAXY_HELL_H_293874239874
		  