#include "gpk_array.h"
#include "gpk_coord.h"
#include "gpk_label.h"

#ifndef GPK_GALAXY_HELL_PHYSICS_H_293874239874
#define GPK_GALAXY_HELL_PHYSICS_H_293874239874

namespace gpk
{
	template<typename _tPOD> ::gpk::error_t		loadPOD			(::gpk::view_array<const byte_t> & input, _tPOD & output) { 
		::gpk::view_array<const _tPOD>					readView		= {}; 
		uint32_t										bytesRead		= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(readView, input)); 
		input										= {input.begin() + bytesRead, input.size() - bytesRead}; 
		output										= readView[0]; 
		return 0;
	}

	template<typename _tPOD> ::gpk::error_t		loadView		(::gpk::view_array<const byte_t> & input, ::gpk::array_pod<_tPOD> & output) { 
		::gpk::view_array<const _tPOD>					readView		= {}; 
		uint32_t										bytesRead		= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(readView, input)); 
		input										= {input.begin() + bytesRead, input.size() - bytesRead}; 
		output										= readView; 
		return 0;
	}

	static inline ::gpk::error_t				loadLabel		(::gpk::view_array<const byte_t> & input, ::gpk::vcc & output) { 
		uint32_t										bytesRead		= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(output, input)); 
		input										= {input.begin() + bytesRead, input.size() - bytesRead}; 
		output										= ::gpk::label(output);
		return 0;
	}
}

namespace ghg 
{
	// simple particle force integrator
	struct SParticles3 {
		::gpk::array_pod<::gpk::SCoord3<float>>		Position			= {};
		::gpk::array_pod<::gpk::SCoord3<float>>		Direction			= {};
		::gpk::array_pod<float>						Speed				= {};

		int											IntegrateSpeed		(double secondsLastFrame)	{
			for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
				::gpk::SCoord3<float>							& direction			= Direction	[iShot];
				::gpk::SCoord3<float>							& position			= Position	[iShot];
				float											& speed				= Speed		[iShot];
				position									+= direction * speed * secondsLastFrame;
			}
			return 0;
		}

		int											Remove				(int32_t iParticle)	{
			Position	.remove_unordered(iParticle);
			Direction	.remove_unordered(iParticle);
			return Speed.remove_unordered(iParticle);
		}

		int											Create				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed)	{
			Position	.push_back(position);
			Direction	.push_back(direction);
			return Speed.push_back(speed);
		}
		::gpk::error_t										Save(::gpk::array_pod<byte_t> & output) const { 
			gpk_necs(::gpk::viewWrite(::gpk::view_array<const ::gpk::SCoord3<float>	>{Position	}, output)); 
			gpk_necs(::gpk::viewWrite(::gpk::view_array<const ::gpk::SCoord3<float>	>{Direction	}, output)); 
			gpk_necs(::gpk::viewWrite(::gpk::view_array<const float					>{Speed		}, output)); 
			info_printf("Saved %s, %i", "Position	", Position		.size()); 
			info_printf("Saved %s, %i", "Direction	", Direction	.size()); 
			info_printf("Saved %s, %i", "Speed		", Speed		.size()); 
			return 0; 
		}
		::gpk::error_t										Load(::gpk::view_array<const byte_t> & input) { 
			gpk_necs(::gpk::loadView(input, Position	));
			gpk_necs(::gpk::loadView(input, Direction	));
			gpk_necs(::gpk::loadView(input, Speed		));
			return 0;
		}
	};
} // namespace 

#endif // GPK_GALAXY_HELL_PHYSICS_H_293874239874
