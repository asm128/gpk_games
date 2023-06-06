#include "gpk_particle.h"
#include "gpk_array_static.h"
#include "gpk_font.h"

#ifndef GPK_GALAXY_HELL_DECO_H_293874239874
#define GPK_GALAXY_HELL_DECO_H_293874239874

namespace ghg 
{
	struct SStars	{
		::gpk::apod<::gpk::n2f32>	Position			= {};
		::gpk::af32					Speed				= {};
		::gpk::af32					Brightness			= {};

		int					Reset				(::gpk::n2u16 targetSize, uint16_t starCount = 128)	{
			if(0 == targetSize.y || 0 == targetSize.x) 
				return 0;
			Speed		.resize(starCount);
			Brightness	.resize(starCount);
			Position	.resize(starCount);
			for(uint32_t iStar = 0; iStar < Brightness.size(); ++iStar) {
				Speed		[iStar]		= float(16 + (rand() % 64));
				Brightness	[iStar]		= float(1.0 / RAND_MAX * rand());
				Position	[iStar].y	= float(rand() % targetSize.y);
				Position	[iStar].x	= float(rand() % targetSize.x);
			}
			return 0;
		}

		int					Update				(::gpk::n2u16 targetSize, float secondsLastFrame)	{
			stacxpr	double			randUnit			= 1.0 / RAND_MAX;
			for(uint32_t iStar = 0; iStar < Brightness.size(); ++iStar) {
				::gpk::n2f32			& starPos			= Position[iStar];
				float					& starSpeed			= Speed[iStar];
				starPos.y			+= starSpeed * secondsLastFrame;
				Brightness[iStar]	= float(randUnit * rand());
				if(starPos.y >= targetSize.y) {
					starSpeed			= float(16 + (rand() % 64)) * .1f;
					starPos.y			= 0;
				}
				if(starPos.x >= targetSize.x) {
					starSpeed			= float(16 + (rand() % 64)) * .1f;
					starPos.y			= 0;
				}
			}
			return 0;
		}
	};

	struct SDebris	{
		::gpk::af32			Brightness			= {};
		::gpk::SParticles3	Particles			= {};
		
		int					Remove				(int32_t iParticle)	{
			Brightness.remove_unordered(iParticle);
			return Particles.Remove(iParticle);
		}

		int					Create				(const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speed, float brightness)	{
			Particles.Create(position, direction, speed);
			return Brightness.push_back(brightness);
		}

		int					SpawnSpherical		(uint32_t countDebris, const ::gpk::n3f32 & position, float speedDebris, float brightness)	{
			stacxpr	double			randUnit			= ::gpk::math_2pi / RAND_MAX;
			for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
				::gpk::n3f32									direction			= {0, 1, 0};
				direction.RotateX(rand() * randUnit);
				direction.RotateY(rand() * randUnit);
				direction.Normalize();
				Create(position, direction, speedDebris, brightness);
			}
			return 0;
		}

		int					SpawnDirected		(uint32_t countDebris, double noiseFactor, const ::gpk::n3f32 & direction, const ::gpk::n3f32 & position, float speedDebris, float brightness)	{
			stacxpr	double			randUnit			= ::gpk::math_2pi / RAND_MAX;
			for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
				::gpk::n3f32									finalDirection		= {0, 1, 0};
				finalDirection.RotateX(rand() * randUnit);
				finalDirection.RotateY(rand() * randUnit);
				finalDirection.Normalize();
				Create(position, ::gpk::interpolate_linear(direction, finalDirection, noiseFactor), speedDebris, brightness);
			}
			return 0;
		}

		int					Update				(float secondsLastFrame)	{
			Particles.IntegrateSpeed(secondsLastFrame);
			for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
				float					& speed				= Particles.Speed		[iShot];
				float					& brightness 		= Brightness			[iShot];
				brightness			-= secondsLastFrame;
				if(0 > brightness)
					Remove(iShot);
				else 
					speed				-= secondsLastFrame * ((0 > speed) ? (rand() % 16) * 5 : (rand() % 16));
			}
			return 0;
		}
	};


#pragma pack(push, 1)
	struct STriColor {
		uint32_t			Min;
		uint32_t			Mid;
		uint32_t			Max;
	};
	struct SScoreParticle {
		int32_t				Score;
		float				Brightness;
		uint8_t				IndexColor;
	};
#pragma pack(pop)

	struct SScoreParticles {
		::gpk::apod<::ghg::SScoreParticle>			Scores		= {};
		::gpk::SParticles3							Particles	= {};
		::gpk::array_static<::ghg::STriColor, 256>	Palette		= {{0xFF00FF00, 0xFFFFEE11, 0xFFFF0000}, };

		int								Remove				(int32_t iParticle)	{
			Scores.remove_unordered(iParticle);
			return Particles.Remove(iParticle);
		}

		int								Create				(const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speed, const ::ghg::SScoreParticle & score)	{
			Particles.Create(position, direction + (direction * (score.Score * .0001)), speed);
			return Scores.push_back(score);
		}

		int								Update				(float secondsLastFrame)	{
			Particles.IntegrateSpeed(secondsLastFrame);
			for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
				::ghg::SScoreParticle		& particle			= Scores[iShot];
				float						& brightness 		= particle.Brightness;
				brightness				-= secondsLastFrame;
				if(0 >= brightness)
					Remove(iShot);
				else {
					float						& speed				= Particles.Speed[iShot];
					if(speed > 0)
						speed					-= secondsLastFrame;// * ((0 > speed) ? (rand() % 16) * 5 : (rand() % 16));
					else
						Remove(iShot);
				}
			}
			return 0;
		}
	};

	struct SExplosion {
		int32_t							IndexMesh;
		int32_t							IndexImage;
		::gpk::apod<::gpk::rangeu16>	Slices;
		::gpk::SParticles3				Particles;

		int								Remove				(uint32_t iSlice)			{
			Particles.Remove(iSlice);
			return Slices.remove_unordered(iSlice);
		}
		int								Update				(float secondsLastFrame)	{
			Particles.IntegrateSpeed(secondsLastFrame);
			for(uint32_t iSlice = 0; iSlice < Particles.Speed.size(); ++iSlice) {
				float											& speed				= Particles.Speed[iSlice];
				//speed										-= secondsLastFrame * (rand() % 16);
				speed										-= secondsLastFrame * ((0 > speed) ? (rand() % 16) * 5 : (rand() % 16));
				if (speed < -30)
					Remove(iSlice--);
			}
			return 0;
		}
	};

		// Deco elements don't affect any relevant game variables. ie. they only use the game state as constants
	struct SDecoState {
		::ghg::SStars					Stars							= {};
		::ghg::SDebris					Debris							= {};
		::ghg::SScoreParticles			ScoreParticles					= {};
		::gpk::aobj<::ghg::SExplosion>	Explosions						= {};

		double							AnimationTime					= 0;

		::gpk::SRasterFontManager		FontManager						= {};
	};

	::gpk::error_t					decoExplosionAdd	(::gpk::aobj<::ghg::SExplosion> & explosions, int32_t indexMesh, int32_t indexImage, uint32_t triangleCount, const ::gpk::n3f32 &collisionPoint, double debrisSpeed);
	::gpk::error_t					decoUpdate			(::ghg::SDecoState & decoState, double secondsLastFrame, double relativeSpeed, const ::gpk::n2u16 & screenMetrics);
}

#endif // GPK_GALAXY_HELL_DECO_H_293874239874
