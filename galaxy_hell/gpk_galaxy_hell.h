#include "gpk_galaxy_hell_deco.h"
#include "gpk_galaxy_hell_ships.h"

#include "gpk_sysevent.h"

#include <mutex>

#ifndef GPK_GALAXY_HELL_H_293874239874
#define GPK_GALAXY_HELL_H_293874239874

namespace ghg
{

	struct SPlayState {
		uint32_t												OffsetStage				= 2;
		uint32_t												Stage					= 0;
		double													TimeStage				= 0;
		double													TimePlayed				= 0;

		double													CameraSwitchDelay		= 0;

		static constexpr const double							RelativeSpeedTarget		= 20;
		double													RelativeSpeedCurrent	= -50;
		int														AccelerationControl		= 0;

		double													TimeScale				= 1;
		bool													Slowing					= true;
	};

	struct SGalaxyHellDrawCache {
		// Used when rastering triangles
		::gpk::array_pod<::gpk::SCoord2<int16_t>>				PixelCoords				;
		::gpk::array_pod<::gpk::STriangleWeights<float>>		PixelVertexWeights		;
		
		// Used for pixel shading
		::gpk::array_pod<::gpk::SCoord3<float>>					LightPointsWorld		;
		::gpk::array_pod<::gpk::SColorBGRA>						LightColorsWorld		;
		::gpk::array_pod<::gpk::SCoord3<float>>					LightPointsModel		;
		::gpk::array_pod<::gpk::SColorBGRA>						LightColorsModel		;

		// Gameplay render target. I decided the world will have its own rendertarget to facilitate reuse of the code.
		::gpk::SCoord2<uint16_t>								RenderTargetMetrics		= {1280, 720};
		::gpk::array_obj<::gpk::ptr_obj<::ghg::TRenderTarget>>	RenderTargetPool		= {};
		::gpk::ptr_obj<::ghg::TRenderTarget>					RenderTargetQueue		= {};
		::std::mutex											RenderTargetQueueMutex	;

		::gpk::error_t											Clear					() {
			::gpk::clear
				( PixelCoords
				, PixelVertexWeights		
				, LightPointsWorld		
				, LightColorsWorld		
				, LightPointsModel		
				, LightColorsModel				
				);
			return 0;
		}
	};

	struct SGalaxyHell {
		::ghg::SShipState										ShipState				= {};
		::ghg::SDecoState										DecoState				= {};	

		uint32_t												Score					= 0;
		uint32_t												Seed					= 0;

		::ghg::SPlayState										PlayState				= {};
		
		::ghg::SGalaxyHellDrawCache								DrawCache;
		::std::mutex											LockUpdate;

		bool													Paused					= false;
		
		static	::gpk::error_t									Save					(const ::ghg::SGalaxyHell & game, ::gpk::array_pod<byte_t> & output) {
			::gpk::viewWrite(::gpk::view_array<const ::ghg::SEntity>{game.ShipState.EntitySystem.Entities.begin(), game.ShipState.EntitySystem.Entities.size()}, output);
			for(uint32_t iEntity = 0; iEntity < game.ShipState.EntitySystem.EntityChildren.size(); ++iEntity) {
				::gpk::viewWrite(::gpk::view_array<const uint32_t>{game.ShipState.EntitySystem.EntityChildren[iEntity].begin(), game.ShipState.EntitySystem.EntityChildren[iEntity].size()}, output);
			}
			return 0;
		}

	};

	int													stageSetup						(::ghg::SGalaxyHell & solarSystem);
	int													solarSystemSetup				(::ghg::SGalaxyHell & solarSystem, const ::gpk::SCoord2<uint16_t> & windowSize);
	int													solarSystemReset				(::ghg::SGalaxyHell & solarSystem); 
	int													solarSystemDraw					(const ::ghg::SGalaxyHell & solarSystem, ::ghg::SGalaxyHellDrawCache & drawCache, ::std::mutex & lockUpdate);
	int													solarSystemUpdate				(::ghg::SGalaxyHell & solarSystem, double secondsLastFrame, const ::gpk::view_array<::gpk::SSysEvent> & frameEvents);
}

#endif // GPK_GALAXY_HELL_H_293874239874
		  