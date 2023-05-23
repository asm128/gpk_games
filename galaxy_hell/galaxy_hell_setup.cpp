#define GPK_INFO_PRINTF_ENABLED

#include "gpk_galaxy_hell.h"
#include "gpk_storage.h"
#include "gpk_json.h"
#include "gpk_noise.h"
#include "gpk_deflate.h"

#include <windows.h>
#include <mmsystem.h>

::gpk::error_t					ghg::solarSystemSave		(const ::ghg::SGalaxyHell & game, ::gpk::vcc fileName) {
	::gpk::au8							serialized;
	gpk_necs(game.Save(serialized));

	::gpk::au8							deflated;
	gpk_necs(::gpk::arrayDeflate(serialized, deflated));

	info_printf("Savegame size in bytes: %u.", serialized.size());
	info_printf("Savegame file size: %u.", deflated.size());

	return ::gpk::fileFromMemory(fileName, deflated);
}

::gpk::error_t					ghg::solarSystemLoad		(::ghg::SGalaxyHell & world,::gpk::vcc filename) {
	::gpk::au8							serialized;
	world.PlayState.TimeLast		= ::gpk::timeCurrent();

	gpk_necall(::gpk::fileToMemory(filename, serialized), "%s", "");
	::gpk::au8							inflated;
	gpk_necs(::gpk::arrayInflate(serialized, inflated));

	info_printf("Savegame file size: %u.", inflated.size());
	info_printf("Savegame size in bytes: %u.", serialized.size());

	::gpk::vcu8							viewSerialized				= inflated;
	if errored(world.Load(viewSerialized)) {
		gpk_necs(::ghg::solarSystemReset(world));
		return -1;
	}
	world.PlayState.Paused			= true;
	return 0;
}

static	int											shipCreate			(::ghg::SShipManager & shipState, int32_t teamId, int32_t iGeometry, int32_t iImage)	{
	::ghg::SShipScene										& scene				= shipState.Scene;
	const uint32_t											countParts			= 6;

	::ghg::SShipCore											ship				= {};
	{	// Create main ship entity
		::ghg::SEntity											entity				= {-1};
		entity												= {-1};
		entity.Geometry										= -1;	//1 + (iGeometry % 5);
		entity.Transform									= scene.Transforms.push_back(shipState.ShipPhysics.MatrixIdentity4);
		entity.Image										= -1;	//iImage % 5;
		entity.Body											= shipState.ShipPhysics.Create();
		ship.Entity											= shipState.EntitySystem.Create(entity, {});
		ship.Team											= teamId;
		//const int32_t											indexBody			= shipState.ShipPhysics.Create(); 
		//(void)indexBody;
	}
	const int32_t											indexShip			= shipState.ShipCores.push_back(ship);
	shipState.ShipParts.push_back({});
	shipState.ShipScores.push_back({});

	//ship.Parts.reserve(countParts);
	::ghg::SEntity											entityOrbit				= {ship.Entity};
	for(uint32_t iPart = 0; iPart < countParts; ++iPart) {	// Create child parts
		::ghg::SEntity											entityPart				= {-1};
		entityOrbit.Parent									= ship.Entity;
		entityOrbit.Geometry								= -1;
		entityOrbit.Transform								= scene.Transforms.push_back(shipState.ShipPhysics.MatrixIdentity4);
		entityOrbit.Image									= -1;
		entityOrbit.Body									= ::gpk::createOrbiter(shipState.ShipPhysics
			, 1		//PLANET_MASSES				[iPlanet]
			, 2.5	//PLANET_DISTANCE			[iPlanet]
			, 0		//PLANET_AXIALTILT			[iPlanet]
			, 1		//PLANET_DAY				[iPlanet]
			, 1		//PLANET_DAY				[PLANET_EARTH]
			, 6		//PLANET_ORBITALPERIOD		[iPlanet]
			, 0		//PLANET_ORBITALINCLINATION	[iPlanet]
			, 1		//1.0 / PLANET_DISTANCE		[PLANET_COUNT - 1] * 2500
			);
		shipState.ShipPhysics.Centers[entityOrbit.Body].Orientation	= {};
		shipState.ShipPhysics.Centers[entityOrbit.Body].Orientation.MakeFromEulerTaitBryan(0, (float)(::gpk::math_2pi / countParts * iPart), 0);

		entityPart.Parent									= shipState.EntitySystem.Create(entityOrbit, {});
		entityPart.Geometry									= iGeometry;
		entityPart.Transform								= scene.Transforms.push_back(shipState.ShipPhysics.MatrixIdentity4);
		entityPart.Image									= iImage % shipState.Scene.Image.size();
		entityPart.Body										= entityOrbit.Body + 1;
		int32_t													indexEntityPart				= shipState.EntitySystem.Create(entityPart, {});
		shipState.EntitySystem.EntityChildren[entityPart.Parent].push_back(indexEntityPart);
		//solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation.Normalize();

		::ghg::SOrbiter											shipPart				= {};
		shipPart.Entity										= entityPart.Parent;
		shipState.ShipParts[indexShip].push_back(shipState.Orbiters.push_back(shipPart));
		shipState.ShipOrbitersDistanceToTargets.push_back({});
		shipState.ShipOrbiterActionQueue.push_back({});

		::gpk::au32								& parentEntityChildren	= shipState.EntitySystem.EntityChildren[ship.Entity];
		parentEntityChildren.push_back(shipPart.Entity);
	}
	return indexShip;
}

//static	int											engineCreate		(::ghg::SGalaxyHell & solarSystem, int32_t iImage)	{
//	return 0;
//}

static	int											shipGeometryBuildEngine	(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildHelix(geometry, 8U, 8U, 0.15f, {0, 1.1f, 0}, {1, 1, 1});
	//::gpk::geometryBuildCylinder(geometry, 1U, 6U, .05f, .05f, {0, 1, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 1U, 8U, .35f, .25f, {0, 1, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 1U, 8U, .35f, .25f, {0, 1, 0}, {-1, 1, 1});
	return 0;
}

static	int											shipGeometryBuildWafer		(::gpk::SGeometryQuads & geometry)			{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildCube(geometry, {1.2f, 0.5f, 1.2f});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, { 0.75f, .75f}, { 1.5, 1, 1.5});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, {-0.75f, .75f}, {-1.5, 1, 1.5});
	return 0;
}

static	int											shipGeometryBuildWaferShotgun	(::gpk::SGeometryQuads & geometry)			{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildCube(geometry, {.3f, 1.5f, .3f}, {-.25f, 0, -.25f});
	::gpk::geometryBuildCube(geometry, {.3f, 1.5f, .3f}, {-.25f, 0, .25f});
	::gpk::geometryBuildCube(geometry, {.3f, 1.5f, .3f}, {.25f, 0, -.25f});
	::gpk::geometryBuildCube(geometry, {.3f, 1.5f, .3f}, {.25f, 0, .25f});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, { 0.75f, .75f}, { 1.5, 1, 1.5});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, {-0.75f, .75f}, {-1.5, 1, 1.5});
	return 0;
}

static	int											shipGeometryBuildGun	(::gpk::SGeometryQuads & geometry)						{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildSphere	(geometry, 8U, 5U, .7f, {0, 0});
	::gpk::geometryBuildFigure0	(geometry, 2U, 8U, 1, {});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .1f, {}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .1f, {}, {-1, 1, 1});
	return 0;
}

static	int											shipGeometryBuildShotgun	(::gpk::SGeometryQuads & geometry)						{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildSphere	(geometry, 8U, 5U, .35f, {0, 0});
	::gpk::geometryBuildFigure0	(geometry, 2U, 8U, 1, {});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, .125f, {0, 0, .25f}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, .125f, {0, 0, .25f}, {-1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, .125f, {0, 0, -.25f}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, .125f, {0, 0, -.25f}, {-1, 1, 1});
	return 0;
}

static	int											shipGeometryBuildCannon	(::gpk::SGeometryQuads & geometry)			{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildSphere	(geometry, 8U, 5U, .8f, {0, 0});
	::gpk::geometryBuildTender	(geometry, 8U, 8U, .75f, {0, -.5, 0}, { .25, .8f, .25});
	::gpk::geometryBuildTender	(geometry, 8U, 8U, .75f, {0, -.5, 0}, {-.25, .8f, .25});
	return 0;
}

static	int											shipGeometryBuildCannonball	(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildSphere		(geometry, 6U, 4U, .5f, {0, 0});
	return 0;
}

static	int											shipGeometryBuildRocket		(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {-1, 1, 1});
	return 0;
}

static	int											shipGeometryBuildMissile	(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {-1, 1, 1});
	return 0;
}


static	int											shipGeometryBuildBullet		(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .001f, {}, {-1, 1, 1});
	return 0;
}

static	int											shipGeometryBuildShred		(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .001f, {}, {-1, 1, 1});
	return 0;
}

static	int											modelsSetup				(::ghg::SShipScene & scene)			{
	scene.Geometry.resize(::gpk::get_value_count<ghg::SHIP_GEOMETRY>() * 1024);

	::shipGeometryBuildEngine		(scene.Geometry[::ghg::SHIP_GEOMETRY_ENGINE			]);
	::shipGeometryBuildGun			(scene.Geometry[::ghg::SHIP_GEOMETRY_GUN			]);
	::shipGeometryBuildWafer		(scene.Geometry[::ghg::SHIP_GEOMETRY_WAFER			]);
	::shipGeometryBuildCannon		(scene.Geometry[::ghg::SHIP_GEOMETRY_CANNON			]);
	::shipGeometryBuildShotgun		(scene.Geometry[::ghg::SHIP_GEOMETRY_SHOTGUN		]);
	::shipGeometryBuildWaferShotgun	(scene.Geometry[::ghg::SHIP_GEOMETRY_WAFER_SHOTGUN	]);
	::shipGeometryBuildCannonball	(scene.Geometry[::ghg::SHIP_GEOMETRY_CANNONBALL		]);
	::shipGeometryBuildRocket		(scene.Geometry[::ghg::SHIP_GEOMETRY_ROCKET			]);
	::shipGeometryBuildMissile		(scene.Geometry[::ghg::SHIP_GEOMETRY_MISSILE		]);
	::shipGeometryBuildBullet		(scene.Geometry[::ghg::SHIP_GEOMETRY_BULLET			]);
	::shipGeometryBuildShred		(scene.Geometry[::ghg::SHIP_GEOMETRY_SHRED			]);

	::gpk::geometryBuildCube		(scene.Geometry[::ghg::SHIP_GEOMETRY_CUBE			], {1, 1, 1});
	::gpk::geometryBuildSphere		(scene.Geometry[::ghg::SHIP_GEOMETRY_SPHERE			], 16U, 16U, .5f, {0, 0});
	::gpk::geometryBuildCylinder	(scene.Geometry[::ghg::SHIP_GEOMETRY_CYLINDER		],  1U, 32U, .5, .5, {0, 0}, {1, 1, 1});
	{

		::gpk::apod<::gpk::SColorFloat>	baseColors;
		for(uint32_t i = 0; i < 4096; ++i)
			baseColors.push_back({(float)::gpk::noiseNormal1D(rand()), (float)::gpk::noiseNormal1D(rand()), (float)::gpk::noiseNormal1D(rand()), 1});

		scene.Image.resize(256);
		for(uint32_t iImage = 0; iImage < scene.Image.size(); ++iImage) {
			::gpk::img<::gpk::bgra>						& image							= scene.Image[iImage];
			if(image.Texels.size())
				continue;
			image.resize(::gpk::n2u32{32, 5});
			for(uint32_t y = 0; y < image.metrics().y; ++y) {// Generate noise color for planet texture
				bool													xAffect						= (y % 2);
				::gpk::SColorFloat										lineColor					= baseColors[(iImage + (rand() % 4)) % (baseColors.size() - 4)];
				for(uint32_t x = 0; x < image.metrics().x; ++x) {
					image.Texels[y * image.metrics().x + x]				= (lineColor * (xAffect ? ::gpk::max(.25, ::gpk::sin(x * (1.0 / image.metrics().x * ::gpk::math_pi))) : 1)).Clamp();
				}
			}
		}
	}
	return 0;
}

int													ghg::solarSystemReset					(::ghg::SGalaxyHell & solarSystem)	{	// Set up enemy ships
	::std::lock_guard										lock(solarSystem.LockUpdate);
	solarSystem.ShipControllers.clear();
	solarSystem.ShipState.Clear();
	solarSystem.DecoState	= {};
	solarSystem.PlayState	= {};
	::gpk::rasterFontDefaults(solarSystem.DecoState.FontManager);
	solarSystem.PilotsReset();
	return 0;
}

int													ghg::stageSetup							(::ghg::SGalaxyHell & solarSystem)	{	// Set up enemy ships
	stacxpr	const uint32_t						partHealth								= 100;

	solarSystem.PlayState.TimeStage						= 0;
	solarSystem.PlayState.TimeRealStage					= 0;
	if(0 == solarSystem.PlayState.Stage) {
		::std::lock_guard										lock(solarSystem.LockUpdate);
		solarSystem.DecoState.Stars.Reset(solarSystem.DrawCache.RenderTargetMetrics);
		solarSystem.PlayState.TimeStart = solarSystem.PlayState.TimeLast = ::gpk::timeCurrent();
		memset(solarSystem.ShipState.ShipScores.begin(), 0, solarSystem.ShipState.ShipScores.byte_count());
		while(solarSystem.Pilots.size() < solarSystem.PlayState.CountPlayers) {
			char text [64] = {};
			sprintf_s(text, "Player %i", solarSystem.Pilots.size() + 1);
			solarSystem.Pilots.push_back({::gpk::label(text), PLAYER_COLORS[solarSystem.Pilots.size() % ::gpk::size(PLAYER_COLORS)]});
		}
		solarSystem.ShipControllers.resize(solarSystem.PlayState.CountPlayers);
	}

#pragma pack(push, 1)
	struct SShipOrbiterSetup {
		::ghg::SHIP_PART_TYPE	Type			;
		uint32_t				MaxHealth		;
		::ghg::WEAPON_TYPE		Weapon			;
		double					MaxDelay		;
		double					Stability		;
		::ghg::WEAPON_LOAD		Munition		;
		float					Speed			;
		int32_t					Damage			;
		uint8_t					ParticleCount	;
		double					Cooldown		;
		double					OverheatPerShot	;
		double					ShotLifetime	;
		::ghg::WEAPON_DAMAGE	DamageType		;

	};
#pragma pack(pop)

	::gpk::SJSONFile										stageFile								= {};
	char													stageFileName			[256]			= "./%s.json";
	stacxpr const SShipOrbiterSetup				weaponDefinitions		[]				=
		{ {::ghg::SHIP_PART_TYPE_Gun			, 128, ::ghg::WEAPON_TYPE_Gun		, .08, 0.975, ::ghg::WEAPON_LOAD_Bullet		,  256,    20, 1,   0,0.00,  1.5, ::ghg::WEAPON_DAMAGE_Pierce	}
		, {::ghg::SHIP_PART_TYPE_Shotgun		, 128, ::ghg::WEAPON_TYPE_Shotgun	, .16, 0.925, ::ghg::WEAPON_LOAD_Bullet		,  160,    10, 6,   0,0.00,  1.5, ::ghg::WEAPON_DAMAGE_Impact	}
		, {::ghg::SHIP_PART_TYPE_Wafer			, 128, ::ghg::WEAPON_TYPE_Gun		, .24, 0.99, ::ghg::WEAPON_LOAD_Ray			,  480,    30, 1,   1,0.50,  1, ::ghg::WEAPON_DAMAGE_Pierce	| ::ghg::WEAPON_DAMAGE_Burn	}
		, {::ghg::SHIP_PART_TYPE_ShotgunWafer	, 128, ::ghg::WEAPON_TYPE_Shotgun	, .32, 0.98, ::ghg::WEAPON_LOAD_Ray			,  640,    15, 6,   1,0.50,  0.6, ::ghg::WEAPON_DAMAGE_Pierce	| ::ghg::WEAPON_DAMAGE_Burn	}
		, {::ghg::SHIP_PART_TYPE_Cannon			, 160, ::ghg::WEAPON_TYPE_Cannon	,   2, 1.00, ::ghg::WEAPON_LOAD_Cannonball	,   48,   156, 1,   0,0.00, 10, ::ghg::WEAPON_DAMAGE_Impact	}
		, {::ghg::SHIP_PART_TYPE_Cannon			, 128, ::ghg::WEAPON_TYPE_Cannon	,   2, 0.90, ::ghg::WEAPON_LOAD_Rocket		,   64,    80, 1,   0,0.00,  5, ::ghg::WEAPON_DAMAGE_Impact	| ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		, {::ghg::SHIP_PART_TYPE_Cannon			, 128, ::ghg::WEAPON_TYPE_Cannon	, 2.5, 0.80, ::ghg::WEAPON_LOAD_Missile		,   72,   112, 1,   0,0.00,  5, ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		, {::ghg::SHIP_PART_TYPE_Shotgun		, 160, ::ghg::WEAPON_TYPE_Shotgun	,   3, 0.95, ::ghg::WEAPON_LOAD_Cannonball	,   72,   112, 7,   0,0.00,  5, ::ghg::WEAPON_DAMAGE_Impact	}
		, {::ghg::SHIP_PART_TYPE_Shotgun		, 128, ::ghg::WEAPON_TYPE_Shotgun	,   3, 0.75, ::ghg::WEAPON_LOAD_Rocket		,   80,    80, 6,   0,0.00,  5, ::ghg::WEAPON_DAMAGE_Pierce	| ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		, {::ghg::SHIP_PART_TYPE_Shotgun		, 128, ::ghg::WEAPON_TYPE_Shotgun	, 3.5, 0.50, ::ghg::WEAPON_LOAD_Missile		,   88,   128, 5,   0,0.00,7.5, ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		, {::ghg::SHIP_PART_TYPE_Gun			, 128, ::ghg::WEAPON_TYPE_Gun		, .12, 0.98, ::ghg::WEAPON_LOAD_Cannonball	,  160,   128, 1,   1,0.25,  5, ::ghg::WEAPON_DAMAGE_Impact	}
		, {::ghg::SHIP_PART_TYPE_Gun			, 128, ::ghg::WEAPON_TYPE_Gun		, .12, 0.75, ::ghg::WEAPON_LOAD_Rocket		,  160,    80, 1,   1,0.25,  5, ::ghg::WEAPON_DAMAGE_Pierce	| ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		, {::ghg::SHIP_PART_TYPE_Gun			, 128, ::ghg::WEAPON_TYPE_Gun		, .12, 0.50, ::ghg::WEAPON_LOAD_Missile		,  160,   112, 1,   1,0.25,  5, ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		};

	sprintf_s(stageFileName, "./levels/%u.json", solarSystem.PlayState.Stage + solarSystem.PlayState.OffsetStage);
	if(0 <= ::gpk::fileToMemory(stageFileName, stageFile.Bytes) && stageFile.Bytes.size()) {
		::std::lock_guard										lock	(solarSystem.LockUpdate);
		gpk_necall(-1 == ::gpk::jsonParse(stageFile.Reader, stageFile.Bytes), "%s", stageFileName);
	} 
	else {
		::std::lock_guard										lock	(solarSystem.LockUpdate);
		// Set up player ships
		if(solarSystem.PlayState.Stage == 0) {
			solarSystem.PlayState.TimeWorld		= 0;
			solarSystem.PlayState.TimeReal		= 0;
			solarSystem.PlayState.TimeStage		= 0;
			solarSystem.PlayState.TimeRealStage	= 0;
			solarSystem.ShipState.Clear();
		}
		solarSystem.ShipState.Weapons.clear();
		solarSystem.ShipState.Shots.clear();
	
		if(0 == solarSystem.ShipState.ShipCores.size()) { // Create player ship
			solarSystem.PlayState.CameraSwitchDelay				= 0;
			solarSystem.ShipState.Scene.Global.CameraReset();

			for(uint32_t iPlayer = 0; iPlayer < solarSystem.PlayState.CountPlayers; ++iPlayer) {
				const int32_t											indexShip						= ::shipCreate(solarSystem.ShipState, 0, 0, iPlayer);
				::ghg::SShipCore										& playerShip					= solarSystem.ShipState.ShipCores[indexShip];
				::gpk::SBodyCenter										& shipPivot						= solarSystem.ShipState.ShipPhysics.Centers[solarSystem.ShipState.EntitySystem.Entities[playerShip.Entity].Body];
				shipPivot.Orientation.MakeFromEulerTaitBryan({0, 0, (float)(-::gpk::math_pi_2)});
				shipPivot.Position									= {-30};
				shipPivot.Position.z = float(solarSystem.PlayState.CountPlayers * 30) / 2 - iPlayer * 30;
			}
		}
		// Set up enemy ships
		while(((int)solarSystem.ShipState.ShipCores.size() - (int)solarSystem.PlayState.CountPlayers - 1) < (int)(solarSystem.PlayState.Stage + solarSystem.PlayState.OffsetStage)) {	// Create enemy ships depending on stage.
			int32_t													indexShip						= ::shipCreate(solarSystem.ShipState, 1, -1, solarSystem.PlayState.Stage + solarSystem.ShipState.ShipCores.size());
			::ghg::SShipCore										& enemyShip						= solarSystem.ShipState.ShipCores[indexShip];
			::gpk::au32								& enemyShipOrbiters				= solarSystem.ShipState.ShipParts[indexShip];
			::gpk::SBodyCenter										& shipTransform					= solarSystem.ShipState.ShipPhysics.Centers[solarSystem.ShipState.EntitySystem.Entities[enemyShip.Entity].Body];
			shipTransform.Orientation.MakeFromEulerTaitBryan({0, 0, (float)(::gpk::math_pi_2)});
			shipTransform.Position								= {5.0f + 5 * solarSystem.ShipState.ShipCores.size()};
			for(uint32_t iPart = 0; iPart < enemyShipOrbiters.size(); ++iPart) {
				const ::ghg::SOrbiter								& shipPart					= solarSystem.ShipState.Orbiters[enemyShipOrbiters[iPart]];
				solarSystem.ShipState.ShipPhysics.Forces[solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity].Body].Rotation.y	*= float(1 + indexShip * .35);
			}
		}


		constexpr int32_t										DEFAULT_NITRO					= 1;
		// set up weapons
		for(uint32_t iShip = 0; iShip < solarSystem.ShipState.ShipCores.size(); ++iShip) {
			::ghg::SShipCore										& ship							= solarSystem.ShipState.ShipCores[iShip];
			::gpk::au32								& shipParts						= solarSystem.ShipState.ShipParts[iShip];
			ship.Health											= 0;
			ship.Nitro											= (float)(ship.MaxNitro = DEFAULT_NITRO);

			for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart) {
				::ghg::SOrbiter									& shipPart						= solarSystem.ShipState.Orbiters[shipParts[iPart]];
				//ship.Team										= iShip ? 1 : 0;
				int32_t												weapon							= 0;
				if(0 == ship.Team) {
					uint32_t											minWeapon						= solarSystem.PlayState.Stage / 4;
					uint32_t											maxWeapon						= ::gpk::min(solarSystem.PlayState.Stage, ::gpk::size(weaponDefinitions) - 1);

					weapon											= (int32_t)::gpk::clamp((uint32_t)minWeapon + iPart, minWeapon, maxWeapon);
					solarSystem.ShipState.ShipOrbiterActionQueue[shipParts[0]].push_back(SHIP_ACTION_spawn);

				}
				else {
					if(iShip < 4 || 0 != ((iShip - 1 - solarSystem.PlayState.CountPlayers - solarSystem.PlayState.OffsetStage) % 3) || 0 != iPart) 
						weapon											= 4;
					else {
						weapon											= (iShip - 5) / 3;
					}
					weapon											%= ::gpk::size(weaponDefinitions) >> 1;
				}

				SShipOrbiterSetup										partCreationData	= weaponDefinitions[weapon];
				::ghg::SWeapon										newWeapon			= {};
				shipPart.Health	= (int32_t)(shipPart.MaxHealth	= partCreationData.MaxHealth);
				shipPart.Type							= partCreationData.Type;
				newWeapon.MaxDelay						= (float)partCreationData.MaxDelay;
				if(0 != ship.Team)
					newWeapon.MaxDelay						*= 1 + (2 * iPart);
				newWeapon.Type							= partCreationData.Weapon;
				newWeapon.Load							= partCreationData.Munition;
				newWeapon.Damage						= partCreationData.Damage;
				newWeapon.Speed							= partCreationData.Speed;
				newWeapon.Delay							= newWeapon.MaxDelay / shipParts.size() * iPart;
				newWeapon.Stability						= (float)partCreationData.Stability;
				newWeapon.ParticleCount					= partCreationData.ParticleCount;
				newWeapon.Cooldown						= (float)partCreationData.Cooldown		;//= 1;
				newWeapon.OverheatPerShot				= (float)partCreationData.OverheatPerShot	;//= 0;
				newWeapon.ShotLifetime					= (float)partCreationData.ShotLifetime	;//= 0;

				shipPart.Weapon							= solarSystem.ShipState.Weapons.push_back(newWeapon);
				solarSystem.ShipState.Shots.push_back({});

					 if(shipPart.Type == ::ghg::SHIP_PART_TYPE_Gun			) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = SHIP_GEOMETRY_GUN;				}
				else if(shipPart.Type == ::ghg::SHIP_PART_TYPE_Wafer		) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = SHIP_GEOMETRY_WAFER;			}
 				else if(shipPart.Type == ::ghg::SHIP_PART_TYPE_Shotgun		) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = SHIP_GEOMETRY_SHOTGUN;			}
 				else if(shipPart.Type == ::ghg::SHIP_PART_TYPE_Cannon		) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = SHIP_GEOMETRY_CANNON;			}
 				else if(shipPart.Type == ::ghg::SHIP_PART_TYPE_ShotgunWafer	) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = SHIP_GEOMETRY_WAFER_SHOTGUN;	}
				ship.Health								+= shipPart.Health;
				const uint32_t								width		= ::gpk::get_value_count<::ghg::WEAPON_LOAD>();
				const uint32_t								height		= ::gpk::get_value_count<::ghg::WEAPON_TYPE>();
				const uint32_t								depth		= ::gpk::get_value_count<::ghg::SHIP_PART_TYPE>();
				solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Image	= iShip * width * height * depth + shipPart.Type * width * height + newWeapon.Type * width + newWeapon.Load;
				solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Image	%= solarSystem.ShipState.Scene.Image.size();
				solarSystem.ShipState.ShipPhysics.Centers[solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Body].Orientation = {0, 0, 0, 1};
			}
		}
	}

	++solarSystem.PlayState.Stage;
	solarSystem.PlayState.Slowing			= true;

#if defined(GPK_WINDOWS)
	//PlaySoundA((LPCSTR)SND_ALIAS_SYSTEMSTART, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
#endif
	return 0;
}

int										ghg::solarSystemSetup			(::ghg::SGalaxyHell & solarSystem, const ::gpk::n2<uint16_t> & windowSize)	{
	::ghg::SShipScene							& scene							= solarSystem.ShipState.Scene;
	gpk_necs(::modelsSetup(scene));
	solarSystem.DecoState.FontManager.Fonts.clear();
	gpk_necs(::gpk::rasterFontDefaults(solarSystem.DecoState.FontManager));

	gpk_necs(::ghg::stageSetup(solarSystem));

	::gpk::SMatrix4<float>						& matrixProjection				= solarSystem.ShipState.Scene.Global.MatrixProjection;
	matrixProjection.FieldOfView(::gpk::math_pi * .25, windowSize.x / (double)windowSize.y, 0.01, 500.0);
	::gpk::SMatrix4<float>						matrixViewport					= {};
	matrixViewport.ViewportLH(windowSize.Cast<uint16_t>());
	matrixProjection						*= matrixViewport;
	return 0;
}
