#include "yce_ide_event.h"
#include "yce_ide_id.h"
#include "gpk_engine_planetary_system.h"

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

#ifndef SSIEGE_WORLD_H_23701
#define SSIEGE_WORLD_H_23701

namespace ssg
{
	GDEFINE_ENUM_TYPE (TERRAIN_TYPE, u0_t );
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Soil		, 0);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Grass		, 1);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Stone		, 2);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Sand		, 3);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Ice		, 4);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Snow		, 5);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Brickstone	, 6);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Concrete	, 7);
	GDEFINE_ENUM_VALUE(TERRAIN_TYPE, Asphalt	, 8);

	GDEFINE_ENUM_TYPE (GROUND_TYPE, u0_t );
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Bridge	, 0);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Road	, 1);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Highway	, 2);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Field	, 3);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Forest	, 4);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Beach	, 5);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Building, 6);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, River	, 7);
	GDEFINE_ENUM_VALUE(GROUND_TYPE, Sea		, 8);

	GDEFINE_ENUM_TYPE (DEMOGRAPHIC_TYPE, u0_t );
	GDEFINE_ENUM_VALUE(DEMOGRAPHIC_TYPE, Wild	, 0);
	GDEFINE_ENUM_VALUE(DEMOGRAPHIC_TYPE, Town	, 1);
	GDEFINE_ENUM_VALUE(DEMOGRAPHIC_TYPE, City	, 2);
	GDEFINE_ENUM_VALUE(DEMOGRAPHIC_TYPE, Capital, 3);

	GDEFINE_ENUM_TYPE (OBJECT_TYPE, u0_t );
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

	GDEFINE_ENUM_TYPE (SPREADABLE, u0_t );
	GDEFINE_ENUM_VALUE(SPREADABLE, Light, 0);
	GDEFINE_ENUM_VALUE(SPREADABLE, Fire	, 1);
	GDEFINE_ENUM_VALUE(SPREADABLE, Water, 2);
	GDEFINE_ENUM_VALUE(SPREADABLE, Cold	, 3);
	GDEFINE_ENUM_VALUE(SPREADABLE, Warm	, 4);

	GDEFINE_ENUM_TYPE (CLOTHING, u0_t );
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

	GDEFINE_ENUM_TYPE (FURNITURE, u0_t );
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

	GDEFINE_ENUM_TYPE (WEAR_LOCATION, u0_t );
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
 
	struct SWearable	{ u2_t  Color; };
	struct SDrink		{ u2_t  Color; };
	struct SFood		{ u2_t  Color; };
	struct SPet			{ u2_t  Color; };
	struct STree		{ u2_t  Color; };
	struct STool		{ u2_t  Color; };
	struct SRiver		{ u2_t  Color; };
	struct SBridge		{ u2_t  Color; };
	struct SVehicle		{ u2_t  Color; };
	struct SWeapon		{ u2_t  Color; };
	struct SChest		{ u2_t  Color; };
	struct SBox			{ u2_t  Color; };

	struct STown {
		::gpk::vcc				Name;
		u2_t 				Country;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			{ if_fail_fe(::gpk::saveView(output, Name)); } 
			{ if_fail_fe(::gpk::savePOD (output, Country)); } 
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			{ if_fail_fe(::gpk::loadLabel(input, Name)); } 
			{ if_fail_fe(::gpk::loadPOD  (input, Country)); } 
			return 0;
		}
	};

	struct SMiniMe {
		::gpk::vcc				Name;
		u2_t 				Hangar;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			{ if_fail_fe(::gpk::saveView(output, Name)); } 
			{ if_fail_fe(::gpk::savePOD (output, Hangar)); } 
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			{ if_fail_fe(::gpk::loadLabel(input, Name)); } 
			{ if_fail_fe(::gpk::loadPOD  (input, Hangar)); } 
			return 0;
		}
	};

	struct SHangar {
		::gpk::n2u16			Size;
		assiegeid				Furniture;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			{ if_fail_fe(::gpk::savePOD (output, Size)); } 
			{ if_fail_fe(::gpk::saveView(output, Furniture)); } 
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			{ if_fail_fe(::gpk::loadPOD (input, Size)); } 
			{ if_fail_fe(::gpk::loadView(input, Furniture)); } 
			return 0;
		}
	};

	struct SWorldArea {
		::gpk::n2u32			Offset;
		::gpk::au32				Hangars;
		::gpk::au32				Characters;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			{ if_fail_fe(::gpk::savePOD (output, Offset)); } 
			{ if_fail_fe(::gpk::saveView(output, Hangars)); } 
			{ if_fail_fe(::gpk::saveView(output, Characters)); } 
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			{ if_fail_fe(::gpk::loadPOD (input, Offset)); } 
			{ if_fail_fe(::gpk::loadView(input, Hangars)); } 
			{ if_fail_fe(::gpk::loadView(input, Characters)); } 
			return 0;
		}
	};

	struct SWorldTiles {
		gpk::apimg8bgra			Values;
		gpk::apobj<SWorldArea>	Blocks;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			{ if_fail_fe(::gpk::savePOD(output, Values.size())); } 
			for(u2_t  iValueMap = 0; iValueMap < Values.size(); ++iValueMap)
				{ if_fail_fef(::gpk::saveImage<::gpk::bgra>(output, Values[iValueMap]->View), "Failed to load tile map %i", iValueMap); }

			{ if_fail_fe(::gpk::savePOD(output, Blocks.size())); } 
			for(u2_t  iBlock = 0; iBlock < Blocks.size(); ++iBlock)
				{ if_fail_fef(Blocks[iBlock]->Save(output), "Failed to load world area %i", iBlock); }

			return 0; 
		}
		::gpk::error_t			Load				(::gpk::vcu8 & input) { 
			u2_t 					valuesSize;
			{ if_fail_fe(::gpk::loadPOD(input, valuesSize)); } 
			{ if_fail_fe(Values.reserve(valuesSize)); }
			for(u2_t  iValueMap = 0; iValueMap < valuesSize; ++iValueMap) { 
				::gpk::pimg8bgra			tileMap;
				tileMap.create();
				{ if_fail_fef(::gpk::loadImage(input, *tileMap), "Failed to load tile map %i", iValueMap); } 
				{ if_fail_fe(Values.push_back(tileMap)); }
			}

			u2_t 					blocksSize;
			{ if_fail_fe(::gpk::loadPOD(input, blocksSize)); }
			for(u2_t  iBlock = 0; iBlock < blocksSize; ++iBlock) { 
				::gpk::pobj<SWorldArea>		block;
				{ if_fail_fef(block->Load(input), "Failed to load world area block %i", iBlock); }
				{ if_fail_fe(Blocks.push_back(block)); }
			}
			return 0; 
		}
	};

#pragma pack(push, 1)
	struct SWorldConfig {
		u3_t 				Seed					= ::gpk::noise1DBase(::gpk::timeCurrentInUs());
		::gpk::n2u8				BlockSize				= {255, 255};
		::gpk::n2u32			WorldSizeInBlocks		= {};	// in blocks
	};

	struct SWorldState {
		u3_t 				TimeCreated				= (u3_t )::gpk::timeCurrentInMs();
		u3_t 				TimeOffset				= {};
		u2_t 				DaylightOffsetMinutes	= 90;
		float					DaylightRatioExtra		= .5;
		u3_t 				TimeLastSaved			= {};
		float			 		TimeScale				= 1;
	};
#pragma pack(pop)

	struct SWorldView {
		SWorldState				WorldState;
		SWorldConfig			WorldConfig;
		::gpk::apod<SHangar>	Hangars;
		::gpk::aobj<SMiniMe>	Characters;
		SWorldTiles				Tiles;

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			{ if_fail_fe(::gpk::savePOD(output, WorldConfig)); } 
			{ if_fail_fe(::gpk::savePOD(output, WorldState)); } 
			{ if_fail_fe(::gpk::saveView(output, Hangars)); }
			{ if_fail_fe(::gpk::savePOD(output, Characters.size())); } 
			for(u2_t  iCharacter = 0; iCharacter < Characters.size(); ++iCharacter)
				{ if_fail_fef(Characters[iCharacter].Save(output), "Failed to save character %i", iCharacter); }

			{ if_fail_fe(Tiles.Save(output)); }
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			{ if_fail_fe(::gpk::loadPOD(input, WorldConfig)); } 
			{ if_fail_fe(::gpk::loadPOD(input, WorldState)); } 
			{ if_fail_fe(::gpk::loadView(input, Hangars)); }
			u2_t 					characterCount;
			{ if_fail_fe(::gpk::loadPOD(input, characterCount)); } 
			for(u2_t  iMiniMes = 0; iMiniMes < characterCount; ++iMiniMes) { 
				SMiniMe						miniMes;
				{ if_fail_fef(miniMes.Load(input), "Failed to load character %i", iMiniMes); }
				{ if_fail_fe(Characters.push_back(miniMes)); }
			}

			{ if_fail_fe(Tiles.Load(input)); }
			return 0;
		}
	};
} // namespace ssg

#endif // SSIEGE_WORLD_H_23701
