#include "gpk_galaxy_hell_deco.h"

static constexpr int								MAX_SLICE_TRIANGLES	= 32;

int							ghg::decoExplosionAdd	(::gpk::array_obj<::ghg::SExplosion> & explosions, int32_t indexMesh, uint32_t triangleCount, const ::gpk::SCoord3<float> &collisionPoint, double debrisSpeed) {
	::ghg::SExplosion				newExplosion			= {};
	newExplosion.IndexMesh		= indexMesh;
	for(uint32_t iTriangle = 0; iTriangle < triangleCount; ) {
		const	uint32_t				sliceTriangleCount		= ::gpk::min((uint32_t)(rand() % MAX_SLICE_TRIANGLES), (uint32_t)(triangleCount - iTriangle));
		newExplosion.Slices.push_back({(uint16_t)iTriangle, (uint16_t)sliceTriangleCount});
		iTriangle					+= sliceTriangleCount;

		::gpk::SCoord3<float>			direction				= {0, 1, 0};
		direction.RotateX(rand() * (::gpk::math_2pi / RAND_MAX));
		direction.RotateY(rand() * (::gpk::math_2pi / RAND_MAX));
		direction.RotateZ(rand() * (::gpk::math_2pi / RAND_MAX));
		direction.Normalize();
		newExplosion.Particles.Create(collisionPoint, direction, (float)debrisSpeed);
	}
	for(uint32_t iExplosion = 0; iExplosion < explosions.size(); ++iExplosion) {
		::ghg::SExplosion				& explosion				= explosions[iExplosion];
		if(0 == explosion.Slices.size()) {
			explosion					= newExplosion;
			return iExplosion;
		}
	}
	return explosions.push_back(newExplosion);
}

::gpk::error_t				ghg::decoUpdate			(::ghg::SDecoState & decoState, double secondsLastFrame, double relativeSpeed, const ::gpk::SCoord2<uint16_t> & screenMetrics)		{
	// Update background stars
	decoState.Stars.Update(screenMetrics, (float)secondsLastFrame);

	// Create and update random debris traveling in the wind
	const int32_t											randDebris			= rand();
	decoState.Debris.Create({200.0f, ((randDebris % 2) ? -1.0f : 1.0f) * (randDebris % 400), ((randDebris % 3) ? -1.0f : 1.0f) * (randDebris % 400)}, {-1, 0, 0}, 400, 2);
	decoState.Debris.Update((float)secondsLastFrame);
	for(uint32_t iParticle = 0; iParticle < decoState.Debris.Particles.Position.size(); ++iParticle)
		decoState.Debris.Particles.Position[iParticle].x	-= (float)(relativeSpeed * secondsLastFrame);

	// Update explosions 
	for(uint32_t iExplosion = 0; iExplosion < decoState.Explosions.size(); ++iExplosion) {
		::ghg::SExplosion			& explosion			= decoState.Explosions[iExplosion];
		explosion.Update((float)secondsLastFrame);
		for(uint32_t iParticle = 0; iParticle < explosion.Particles.Position.size(); ++iParticle)
			explosion.Particles.Position[iParticle].x	-= (float)(relativeSpeed * secondsLastFrame);
	}

	return 0;
}
