#include "gpk_array.h"
#include "gpk_coord.h"

#ifndef GPK_GALAXY_HELL_PHYSICS_H_293874239874
#define GPK_GALAXY_HELL_PHYSICS_H_293874239874

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
	};
} // namespace 

#endif // GPK_GALAXY_HELL_PHYSICS_H_293874239874
