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
		uint32_t												Seed					= 0;
		uint32_t												OffsetStage				= 2;
		uint32_t												PlayerCount				= 1;

		uint32_t												Stage					= 0;
		double													TimeStage				= 0;
		double													TimeWorld				= 0;
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
			ShipState.Save(output);
			::gpk::viewWrite(::gpk::view_array<const ::ghg::SPlayState>{&PlayState, 1}, output);
			return 0;
		}
		::gpk::error_t											Load					(::gpk::view_array<const byte_t> & input) {
			ShipState.Load(input);
			::gpk::view_array<const ::ghg::SPlayState>					readPlayState			= {};
			int32_t bytesRead = ::gpk::viewRead(readPlayState, input); input = {input.begin() + bytesRead, input.size() - bytesRead}; PlayState	= readPlayState[0];
			return 0;
		}

	};

	int													stageSetup						(::ghg::SGalaxyHell & solarSystem);
	int													solarSystemSetup				(::ghg::SGalaxyHell & solarSystem, const ::gpk::SCoord2<uint16_t> & windowSize);
	int													solarSystemReset				(::ghg::SGalaxyHell & solarSystem); 
	int													solarSystemDraw					(const ::ghg::SGalaxyHell & solarSystem, ::ghg::SGalaxyHellDrawCache & drawCache, ::std::mutex & lockUpdate);
	int													solarSystemUpdate				(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame, const ::gpk::SInput & input, const ::gpk::view_array<::gpk::SSysEvent> & frameEvents);

	int													getLightArrays
		( const ::ghg::SShipState								& shipState
		, const ::ghg::SDecoState								& decoState
		, ::gpk::array_pod<::gpk::SCoord3<float>>				& lightPoints
		, ::gpk::array_pod<::gpk::SColorBGRA>					& lightColors
		, const ::gpk::view_array<const ::gpk::SColorBGRA>		& debrisColors
		);

	int													drawShipPart
		( const ::ghg::SShipState							& shipState
		, const ::ghg::SShipPart							& shipPart
		, const ::gpk::SMatrix4<float>						& matrixVP
		, ::gpk::view_grid<::gpk::SColorBGRA>				& targetPixels
		, ::gpk::view_grid<uint32_t>						depthBuffer
		, ::ghg::SGalaxyHellDrawCache						& drawCache
		);
}

#endif // GPK_GALAXY_HELL_H_293874239874
		  