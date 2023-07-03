#include "ssiege_event.h"
#include "ssiege_id.h"
#include "ssiege_solar_system.h"

#include "gpk_img_serialize.h"
#include "gpk_engine.h"
#include "gpk_gui.h"
#include "gpk_gui_inputbox.h"
#include "gpk_deflate.h"
#include "gpk_array_ptr.h"
#include "gpk_noise.h"
#include <ctime>

// Mercury - 20° E of crater Hun Kal
// Venus - central peak of crater Ariadne
// Earth - no longer defined by a single point, but by hundreds of points around the world, taking into account tectonic motion
// Mars - 47.95137° E of Viking 1 lander
// The gas giants - multiple systems are used, based either on planetary magnetic fields or observed atmospheric features.
// Moon - the mean Earth point: the point on average facing the Earth
// Io - the mean Jupiter point
// Europa - 178° W of crater Cilix
// Ganymede - 128° E of crater Anat
// Callisto - 34° W of crater Saga
// Mimas - 162° E of crater Palomides
// Enceladus - 5° E of crater Salih
// Tethys - 61° W of crater Arete
// Dione - 63° E of crater Palinurus
// Rhea - 20° W of crater Tore
// Iapetus - 84° W of crater Almeric
// The IAU also defines meridians for some dwarf planets, asteroids, and comets.

#ifndef CAMPP_WORLD_H_23701
#define CAMPP_WORLD_H_23701

namespace ssiege
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE (TERRAIN_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Soil		, 0);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Grass		, 1);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Stone		, 2);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Sand		, 3);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Ice		, 4);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Snow		, 5);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Brickstone	, 6);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Concrete	, 7);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Asphalt	, 8);

	GDEFINE_ENUM_TYPE (GROUND_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Bridge	, 0);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Road	, 1);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Highway	, 2);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Field	, 3);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Forest	, 4);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Beach	, 5);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Building, 6);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, River	, 7);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Sea		, 8);

	GDEFINE_ENUM_TYPE (DEMOGRAPHIC_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(DEMOGRAPHIC_TYPE, Wild	, 0);
	GDEFINE_ENUM_VALUE(DEMOGRAPHIC_TYPE, Town	, 1);
	GDEFINE_ENUM_VALUE(DEMOGRAPHIC_TYPE, City	, 2);
	GDEFINE_ENUM_VALUE(DEMOGRAPHIC_TYPE, Capital, 3);

	GDEFINE_ENUM_TYPE (OBJECT_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Campfire	, 0);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, MiniMe		, 1);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Tree		, 2);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Rope		, 3);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Tool		, 4);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Fence		, 5);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Wall		, 6);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Stair		, 7);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Wearable	, 8);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Drink		, 9);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Food		,10);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Pet			,12);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Vehicle		,13);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Weapon		,14);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Crate		,15);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Dispenser	,16);
	GDEFINE_ENUM_VALUE(OBJECT_TYPE, Lamp		,17);

	GDEFINE_ENUM_TYPE (SPREADABLE, uint8_t);
	GDEFINE_ENUM_VALUE(SPREADABLE, Light, 0);
	GDEFINE_ENUM_VALUE(SPREADABLE, Fire	, 1);
	GDEFINE_ENUM_VALUE(SPREADABLE, Water, 2);
	GDEFINE_ENUM_VALUE(SPREADABLE, Cold	, 3);
	GDEFINE_ENUM_VALUE(SPREADABLE, Warm	, 4);

	GDEFINE_ENUM_TYPE (CLOTHING, uint8_t);
	GDEFINE_ENUM_VALUE(CLOTHING, Shirt		, 0);
	GDEFINE_ENUM_VALUE(CLOTHING, Pants		, 1);
	GDEFINE_ENUM_VALUE(CLOTHING, Shorts		, 2);
	GDEFINE_ENUM_VALUE(CLOTHING, Skirt		, 3);
	GDEFINE_ENUM_VALUE(CLOTHING, Shoes		, 4);
	GDEFINE_ENUM_VALUE(CLOTHING, Sandals	, 5);
	GDEFINE_ENUM_VALUE(CLOTHING, Boots		, 6);
	GDEFINE_ENUM_VALUE(CLOTHING, Hat		, 7);
	GDEFINE_ENUM_VALUE(CLOTHING, Belt		, 8);
	GDEFINE_ENUM_VALUE(CLOTHING, Ring		, 9);
	GDEFINE_ENUM_VALUE(CLOTHING, Glasses	,10);
	GDEFINE_ENUM_VALUE(CLOTHING, Bracelet	,12);
	GDEFINE_ENUM_VALUE(CLOTHING, Necklace	,13);
	GDEFINE_ENUM_VALUE(CLOTHING, Glove		,14);
	GDEFINE_ENUM_VALUE(CLOTHING, Swimsuit 	,15);
	GDEFINE_ENUM_VALUE(CLOTHING, Scuba		,16);
	GDEFINE_ENUM_VALUE(CLOTHING, Hazmat		,17);
	GDEFINE_ENUM_VALUE(CLOTHING, Lamp		,18);

	GDEFINE_ENUM_TYPE (FURNITURE, uint8_t);
	GDEFINE_ENUM_VALUE(FURNITURE, Table		, 0);
	GDEFINE_ENUM_VALUE(FURNITURE, Chair		, 1);
	GDEFINE_ENUM_VALUE(FURNITURE, Pot		, 2);
	GDEFINE_ENUM_VALUE(FURNITURE, Fishbowl	, 3);
	GDEFINE_ENUM_VALUE(FURNITURE, Cage		, 4);
	GDEFINE_ENUM_VALUE(FURNITURE, Stand		, 5);
	GDEFINE_ENUM_VALUE(FURNITURE, Player	, 6);
	GDEFINE_ENUM_VALUE(FURNITURE, Console	, 7);
	GDEFINE_ENUM_VALUE(FURNITURE, Jar		, 8);
	GDEFINE_ENUM_VALUE(FURNITURE, Bust		, 9);
	GDEFINE_ENUM_VALUE(FURNITURE, Painting	,10);
	GDEFINE_ENUM_VALUE(FURNITURE, Drawing	,11);
	GDEFINE_ENUM_VALUE(FURNITURE, Poster	,12);
	GDEFINE_ENUM_VALUE(FURNITURE, Window	,13);
	GDEFINE_ENUM_VALUE(FURNITURE, Door		,14);
	GDEFINE_ENUM_VALUE(FURNITURE, Sofa		,15);
	GDEFINE_ENUM_VALUE(FURNITURE, Bed		,16);
	GDEFINE_ENUM_VALUE(FURNITURE, Light		,17);

	GDEFINE_ENUM_TYPE (WEAR_LOCATION, uint8_t);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Head		, 0);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Eye		, 1);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Glasses	, 2);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Neck		, 3);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Leg		, 4);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Foot		, 5);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Chest		, 6);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Finger	, 7);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Ear		, 8);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Hand		, 9);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Nose		,10);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Mouth		,11);
	GDEFINE_ENUM_VALUE(WEAR_LOCATION, Lips		,12);
 
	struct SWearable	{ uint32_t Color; };
	struct SDrink		{ uint32_t Color; };
	struct SFood		{ uint32_t Color; };
	struct SPet			{ uint32_t Color; };
	struct STree		{ uint32_t Color; };
	struct STool		{ uint32_t Color; };
	struct SRiver		{ uint32_t Color; };
	struct SBridge		{ uint32_t Color; };
	struct SVehicle		{ uint32_t Color; };
	struct SWeapon		{ uint32_t Color; };
	struct SChest		{ uint32_t Color; };
	struct SBox			{ uint32_t Color; };

	struct STown {
		::gpk::vcc				Name;
		uint32_t				Country;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(::gpk::saveView(output, Name)); 
			gpk_necs(::gpk::savePOD (output, Country)); 
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			gpk_necs(::gpk::loadLabel(input, Name)); 
			gpk_necs(::gpk::loadPOD  (input, Country)); 
			return 0;
		}
	};

	struct SMiniMe {
		::gpk::vcc				Name;
		uint32_t				Camp;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(::gpk::saveView(output, Name)); 
			gpk_necs(::gpk::savePOD (output, Camp)); 
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			gpk_necs(::gpk::loadLabel(input, Name)); 
			gpk_necs(::gpk::loadPOD  (input, Camp)); 
			return 0;
		}
	};

	struct SCamp {
		::gpk::n2u16			Size;
		acampid					Furniture;
	};

	struct SWorldArea {
		::gpk::n2u32			Offset;
		::gpk::au32				Camps;
		::gpk::au32				Characters;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(::gpk::savePOD (output, Offset)); 
			gpk_necs(::gpk::saveView(output, Camps)); 
			gpk_necs(::gpk::saveView(output, Characters)); 
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			gpk_necs(::gpk::loadPOD (input, Offset)); 
			gpk_necs(::gpk::loadView(input, Camps)); 
			gpk_necs(::gpk::loadView(input, Characters)); 
			return 0;
		}
	};

	struct SWorldTiles {
		gpk::apimg8bgra			Values;
		gpk::apobj<SWorldArea>	Blocks;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(::gpk::savePOD(output, Values.size())); 
			for(uint32_t iValueMap = 0; iValueMap < Values.size(); ++iValueMap)
				gpk_necall(::gpk::saveImage<::gpk::bgra>(output, Values[iValueMap]->View), "Failed to load tile map %i", iValueMap);

			gpk_necs(::gpk::savePOD(output, Blocks.size())); 
			for(uint32_t iBlock = 0; iBlock < Blocks.size(); ++iBlock)
				gpk_necall(Blocks[iBlock]->Save(output), "Failed to load world area %i", iBlock);

			return 0; 
		}
		::gpk::error_t			Load				(::gpk::vcu8 & input) { 
			uint32_t					valuesSize;
			gpk_necs(::gpk::loadPOD(input, valuesSize)); 
			gpk_necs(Values.reserve(valuesSize));
			for(uint32_t iValueMap = 0; iValueMap < valuesSize; ++iValueMap) { 
				::gpk::pimg8bgra			tileMap;
				tileMap.create();
				gpk_necall(::gpk::loadImage(input, *tileMap), "Failed to load tile map %i", iValueMap); 
				gpk_necs(Values.push_back(tileMap));
			}

			uint32_t					blocksSize;
			gpk_necs(::gpk::loadPOD(input, blocksSize));
			for(uint32_t iBlock = 0; iBlock < blocksSize; ++iBlock) { 
				::gpk::pobj<SWorldArea>		block;
				gpk_necall(block->Load(input), "Failed to load world area block %i", iBlock);
				gpk_necs(Blocks.push_back(block));
			}
			return 0; 
		}
	};

	struct SWorldConfig {
		uint64_t				Seed					= ::gpk::noise1DBase(::gpk::timeCurrentInUs());
		uint64_t				MaxPlayers				= {};
		::gpk::n2u8				BlockSize				= {255, 255};
	};

	struct SWorldState {
		uint64_t				TimeCreated				= (uint64_t)::gpk::timeCurrentInMs();
		uint64_t				TimeOffset				= {};
		uint32_t				DaylightOffsetMinutes	= 90;
		float					DaylightRatioExtra		= .5;
		uint64_t				TimeLastSaved			= {};
		::gpk::n2u32			WorldSize				= {};
		float			 		TimeScale				= 1;
	};
#pragma pack(pop)

	struct SWorldView {
		SWorldState				WorldState;
		SWorldConfig			WorldConfig;
		::gpk::apod<SCamp>		Camps;
		::gpk::aobj<SMiniMe>	Characters;
		SWorldTiles				Tiles;
		::ssiege::SSolarSystem	SolarSystem;

		::gpk::SEngine			Engine;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(::gpk::savePOD(output, WorldConfig)); 
			gpk_necs(::gpk::savePOD(output, WorldState)); 
			gpk_necs(::gpk::saveView(output, Camps));
			gpk_necs(::gpk::savePOD(output, Characters.size())); 
			for(uint32_t iCharacter = 0; iCharacter < Characters.size(); ++iCharacter)
				gpk_necall(Characters[iCharacter].Save(output), "Failed to save character %i", iCharacter);

			gpk_necs(Tiles.Save(output));
			gpk_necs(Engine.Save(output));
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			gpk_necs(::gpk::loadPOD(input, WorldConfig)); 
			gpk_necs(::gpk::loadPOD(input, WorldState)); 
			gpk_necs(::gpk::loadView(input, Camps));
			uint32_t					characterCount;
			gpk_necs(::gpk::loadPOD(input, characterCount)); 
			for(uint32_t iMiniMes = 0; iMiniMes < characterCount; ++iMiniMes) { 
				SMiniMe					miniMes;
				gpk_necall(miniMes.Load(input), "Failed to load character %i", iMiniMes);
				gpk_necs(Characters.push_back(miniMes));
			}

			gpk_necs(Tiles.Load(input));
			gpk_necs(Engine.Load(input));
			return 0;
		}
	};

	::gpk::error_t			worldViewUpdate		(::ssiege::SWorldView & world, ::gpk::vpobj<::ssiege::EventCampp> inputEvents, ::gpk::apobj<::ssiege::EventCampp> & outputEvents, double secondsElapsed);
	::gpk::error_t			worldViewDraw		(::ssiege::SWorldView & pool
		, ::gpk::rtbgra8d32			& backBuffer
		, const ::gpk::n3f32		& cameraPosition
		, const ::gpk::n3f32		& cameraTarget
		, const ::gpk::n3f32		& cameraUp = {0, 1, 0}
		, const ::gpk::minmaxf32	& nearFar = {.1f, 10000.f}
		);
} // namespace ssiege

#endif // CAMPP_WORLD_H_23701
