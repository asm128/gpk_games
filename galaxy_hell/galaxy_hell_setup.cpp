//#define GPK_INFO_PRINTF_ENABLED

#include "gpk_galaxy_hell.h"
#include "gpk_file.h"
#include "gpk_json.h"
#include "gpk_noise.h"
#include "gpk_deflate.h"
#include "gpk_engine_orbit.h"
#include "gpk_chrono.h"

#include <windows.h>
#include <mmsystem.h>

using ::gpk::get_value_namep, ::gpk::get_enum_namep, ::gpk::failed;
GPK_USING_TYPEINT();

::gpk::error_t			ghg::solarSystemSave	(const ::ghg::SGalaxyHell & game, ::gpk::vcsc_t fileName) {
	::gpk::au0_t					serialized;
	gpk_necs(game.Save(serialized));
	gpk_necs(::gpk::deflateFromMemory(fileName, serialized));
	return 0;
}

::gpk::error_t			ghg::solarSystemLoad	(::ghg::SGalaxyHell & world,::gpk::vcsc_t filename) {
	::gpk::au0_t					serialized;
	gpk_necs(::gpk::inflateToMemory(filename, serialized));

	::gpk::vcu0_t					viewSerialized			= serialized;
	if (failed(world.Load(viewSerialized))) {
		gpk_necs(::ghg::solarSystemReset(world));
		return -1;
	}

	world.PlayState.GlobalState.UserTime.Loaded	= ::gpk::timeCurrent();
	world.PlayState.GlobalState.Paused		= true;
	return 0;
}

static	::gpk::error_t	shipCreate				(::ghg::SShipManager & shipState, int32_t teamId, int32_t iGeometry, int32_t iImage)	{
	::ghg::SShipScene			& scene					= shipState.Scene;
	const uint32_t				countParts				= 6;

	::gpk::SSpaceshipCore		ship					= {};
	//shipState.Engine.CreateSphere();

	int32_t						iEntityMain;
	{	// Create main ship entity
		::ghg::SGHEntity			entity					= {-1};
		entity					= {-1};
		entity.Geometry			= -1;	//1 + (iGeometry % 5);
		entity.Transform		= scene.Transforms.push_back(shipState.Engine.Integrator.MatrixIdentity4);
		entity.Image			= -1;	//iImage % 5;
		entity.Body				= shipState.Engine.Integrator.Create();
		gpk_necs(shipState.ShipCoreEntity.push_back(iEntityMain = shipState.EntitySystem.Create(entity, {})));
		ship.Team				= teamId;
		//const int32_t				indexBody				= shipState.ShipPhysics.Create(); 
		//(void)indexBody;
	}
	const int32_t				indexShip				= shipState.SpaceshipManager.ShipCores.push_back(ship);
	shipState.SpaceshipManager.ShipParts.push_back({});
	shipState.SpaceshipManager.ShipScores.push_back({});

	//ship.Parts.reserve(countParts);
	::ghg::SGHEntity			entityOrbit				= {};
	for(uint32_t iPart = 0; iPart < countParts; ++iPart) {	// Create child parts
		::ghg::SGHEntity			entityPart				= {-1};
		entityOrbit.Parent		= iEntityMain;
		entityOrbit.Geometry	= -1;
		entityOrbit.Transform	= scene.Transforms.push_back(shipState.Engine.Integrator.MatrixIdentity4);
		entityOrbit.Image		= -1;
		entityOrbit.Body		= ::gpk::createOrbiter(shipState.Engine.Integrator
			, ::gpk::AXIS_X_POSITIVE
			, ::gpk::AXIS_Y_NEGATIVE
			, 0		//PLANET_ORBITALINCLINATION	[iPlanet]
			, 6		//PLANET_ORBITALPERIOD		[iPlanet]
			, 1		//PLANET_ORBITALUNIT		[iPlanet]
			, 0		//PLANET_AXIALTILT			[iPlanet]
			, 2.5	//PLANET_DISTANCE			[iPlanet]
			, 1		//1.0 / PLANET_DISTANCE		[PLANET_COUNT - 1] * 2500
			, 1		//PLANET_DAY				[iPlanet]
			, 2		//PLANET_DAY				[PLANET_EARTH]
			);
		shipState.Engine.Integrator.Centers[entityOrbit.Body].Orientation.MakeFromEuler(0, (float)(::gpk::math_2pi / countParts * iPart), 0);
		shipState.Engine.Integrator.SetActive(entityOrbit.Body, true);
		entityPart.Parent		= shipState.EntitySystem.Create(entityOrbit, {});
		entityPart.Geometry		= iGeometry;
		entityPart.Transform	= scene.Transforms.push_back(shipState.Engine.Integrator.MatrixIdentity4);
		entityPart.Image		= iImage % shipState.Scene.Image.size();
		entityPart.Body			= entityOrbit.Body + 1;
		int32_t						indexEntityPart			= shipState.EntitySystem.Create(entityPart, {});
		shipState.EntitySystem.EntityChildren[entityPart.Parent].push_back(indexEntityPart);
		//solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation.Normalize();
		shipState.Engine.Integrator.SetActive(entityPart.Body, true);

		::gpk::SSpaceshipOrbiter				shipPart				= {};
		shipState.ShipPartEntity.push_back(entityPart.Parent);
		shipState.SpaceshipManager.ShipParts[indexShip].push_back((uint16_t)shipState.SpaceshipManager.Orbiters.push_back(shipPart));
		shipState.SpaceshipManager.ShipOrbiterActionQueue.push_back({});

		::gpk::au2_t					& parentEntityChildren	= shipState.EntitySystem.EntityChildren[iEntityMain];
		parentEntityChildren.push_back(entityPart.Parent);
	}
	return indexShip;
}

//static	int			engineCreate		(::ghg::SGalaxyHell & solarSystem, int32_t iImage)	{
//	return 0;
//}

static	::gpk::error_t	shipGeometryBuildEngine			(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildHelix(geometry, 8U, 8U, 0.15f, {0, 1.1f, 0}, {1, 1, 1});
	//::gpk::geometryBuildCylinder(geometry, 1U, 6U, .05f, .05f, {0, 1, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 1U, 8U, .35f, .25f, {0, 1, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 1U, 8U, .35f, .25f, {0, 1, 0}, {-1, 1, 1});
	return 0;
}

static	::gpk::error_t	shipGeometryBuildWafer			(::gpk::SGeometryQuads & geometry)			{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildCube(geometry, {1.2f, 0.5f, 1.2f});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, { 0.75f, .75f}, { 1.5, 1, 1.5});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, {-0.75f, .75f}, {-1.5, 1, 1.5});
	return 0;
}

static	::gpk::error_t	shipGeometryBuildWaferShotgun	(::gpk::SGeometryQuads & geometry)			{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildCube(geometry, {.3f, 1.5f, .3f}, {-.25f, 0, -.25f});
	::gpk::geometryBuildCube(geometry, {.3f, 1.5f, .3f}, {-.25f, 0, .25f});
	::gpk::geometryBuildCube(geometry, {.3f, 1.5f, .3f}, {.25f, 0, -.25f});
	::gpk::geometryBuildCube(geometry, {.3f, 1.5f, .3f}, {.25f, 0, .25f});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, { 0.75f, .75f}, { 1.5, 1, 1.5});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, {-0.75f, .75f}, {-1.5, 1, 1.5});
	return 0;
}

static	::gpk::error_t	shipGeometryBuildGun		(::gpk::SGeometryQuads & geometry)						{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildSphere	(geometry, 8U, 5U, .7f, {0, 0});
	::gpk::geometryBuildFigure0	(geometry, 2U, 8U, 1, {});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .1f, {}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .1f, {}, {-1, 1, 1});
	return 0;
}

static	::gpk::error_t	shipGeometryBuildShotgun	(::gpk::SGeometryQuads & geometry)						{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildSphere	(geometry, 8U, 5U, .35f, {0, 0});
	::gpk::geometryBuildFigure0	(geometry, 2U, 8U, 1, {});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, .125f, {0, 0, .25f}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, .125f, {0, 0, .25f}, {-1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, .125f, {0, 0, -.25f}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, .125f, {0, 0, -.25f}, {-1, 1, 1});
	return 0;
}

static	::gpk::error_t	shipGeometryBuildCannon		(::gpk::SGeometryQuads & geometry)			{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildSphere	(geometry, 8U, 5U, .8f, {0, 0});
	::gpk::geometryBuildTender	(geometry, 8U, 8U, .75f, {0, -.5, 0}, { .25, .8f, .25});
	::gpk::geometryBuildTender	(geometry, 8U, 8U, .75f, {0, -.5, 0}, {-.25, .8f, .25});
	return 0;
}

static	::gpk::error_t	shipGeometryBuildCannonball	(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildSphere		(geometry, 6U, 4U, .5f, {0, 0});
	return 0;
}

static	::gpk::error_t	shipGeometryBuildRocket		(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {-1, 1, 1});
	return 0;
}

static	::gpk::error_t	shipGeometryBuildMissile	(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {-1, 1, 1});
	return 0;
}

static	::gpk::error_t	shipGeometryBuildBullet		(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .001f, {}, {-1, 1, 1});
	return 0;
}

static	::gpk::error_t	shipGeometryBuildShred		(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .125f, {}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .125f, .001f, {}, {-1, 1, 1});
	return 0;
}

static	::gpk::error_t	modelsSetupOld			(::ghg::SShipScene & scene)			{
	scene.Geometry.resize(::gpk::get_value_count<gpk::SHIP_GEOMETRY>() * 1024);

	::shipGeometryBuildEngine		(scene.Geometry[::gpk::SHIP_GEOMETRY_Engine			]);
	::shipGeometryBuildGun			(scene.Geometry[::gpk::SHIP_GEOMETRY_Gun			]);
	::shipGeometryBuildWafer		(scene.Geometry[::gpk::SHIP_GEOMETRY_Wafer			]);
	::shipGeometryBuildCannon		(scene.Geometry[::gpk::SHIP_GEOMETRY_Cannon			]);
	::shipGeometryBuildShotgun		(scene.Geometry[::gpk::SHIP_GEOMETRY_Shotgun		]);
	::shipGeometryBuildWaferShotgun	(scene.Geometry[::gpk::SHIP_GEOMETRY_WaferShotgun	]);
	::shipGeometryBuildCannonball	(scene.Geometry[::gpk::SHIP_GEOMETRY_Cannonball		]);
	::shipGeometryBuildRocket		(scene.Geometry[::gpk::SHIP_GEOMETRY_Rocket			]);
	::shipGeometryBuildMissile		(scene.Geometry[::gpk::SHIP_GEOMETRY_Missile		]);
	::shipGeometryBuildBullet		(scene.Geometry[::gpk::SHIP_GEOMETRY_Bullet			]);
	::shipGeometryBuildShred		(scene.Geometry[::gpk::SHIP_GEOMETRY_Shred			]);

	::gpk::geometryBuildCube		(scene.Geometry[::gpk::SHIP_GEOMETRY_Cube			], {1, 1, 1});
	::gpk::geometryBuildSphere		(scene.Geometry[::gpk::SHIP_GEOMETRY_Sphere			], 16U, 16U, .5f, {0, 0});
	::gpk::geometryBuildCylinder	(scene.Geometry[::gpk::SHIP_GEOMETRY_Cylinder		],  1U, 32U, .5, .5, {0, 0}, {1, 1, 1});
	{

		::gpk::apod<::gpk::rgbaf>	baseColors;
		for(uint32_t i = 0; i < 4096; ++i)
			baseColors.push_back({(float)::gpk::noiseNormal1D(rand()), (float)::gpk::noiseNormal1D(rand()), (float)::gpk::noiseNormal1D(rand()), 1});

		scene.Image.resize(256);
		for(uint32_t iImage = 0; iImage < scene.Image.size(); ++iImage) {
			::gpk::img<::gpk::bgra>		& image					= scene.Image[iImage];
			if(image.Texels.size())
				continue;
			image.resize(::gpk::n2u2_t{32, 5});
			for(uint32_t y = 0; y < image.metrics().y; ++y) {// Generate noise color for planet texture
				bool						xAffect					= (y % 2);
				::gpk::rgbaf				lineColor				= baseColors[(iImage + (rand() % 4)) % (baseColors.size() - 4)];
				for(uint32_t x = 0; x < image.metrics().x; ++x) {
					image.Texels[y * image.metrics().x + x]				= (lineColor * (xAffect ? ::gpk::max(.25, ::gpk::sin(x * (1.0 / image.metrics().x * ::gpk::math_pi))) : 1)).Clamp();
				}
			}
		}
	}
	return 0;
}

static	::gpk::error_t	shipNodeCreateRoot	(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) {
	gpk::SMeshManager			& meshes			= graphics.Meshes;
	const uint32_t				rootNode			= (uint32_t)nodes.Create();
	nodes[rootNode].Mesh	= (uint32_t)meshes.Create();
	meshes[nodes[rootNode].Mesh]->ConstantBuffers.clear();
	meshes[nodes[rootNode].Mesh]->Desc.Type			= ::gpk::GEOMETRY_TYPE_Triangle;
	meshes[nodes[rootNode].Mesh]->Desc.Mode			= ::gpk::MESH_MODE_List;
	return rootNode;
}

static	::gpk::error_t	shipNodeBuildEngine			(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }
static	::gpk::error_t	shipNodeBuildWafer			(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }
static	::gpk::error_t	shipNodeBuildWaferShotgun	(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }
static	::gpk::error_t	shipNodeBuildGun			(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }
static	::gpk::error_t	shipNodeBuildShotgun		(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }
static	::gpk::error_t	shipNodeBuildCannon			(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }
static	::gpk::error_t	shipNodeBuildCannonball		(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }
static	::gpk::error_t	shipNodeBuildRocket			(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }
static	::gpk::error_t	shipNodeBuildMissile		(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }
static	::gpk::error_t	shipNodeBuildBullet			(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }
static	::gpk::error_t	shipNodeBuildShred			(::gpk::SRenderNodeManager & nodes, ::gpk::SEngineGraphics & graphics) { const uint32_t rootNode = (uint32_t)shipNodeCreateRoot(nodes, graphics); return rootNode; }

static	::gpk::error_t	modelsSetup			(::gpk::SEngine & engine)			{
	gpk::pobj<gpk::SEngineScene>	scene			= engine.Scene;

	gpk::SRenderNodeManager		& nodes				= scene->RenderNodes;
	gpk::pobj<gpk::SEngineGraphics>	& graphics		= scene->Graphics;
	if(!graphics) 
		graphics.create();

	::shipNodeBuildEngine		(nodes, *graphics);
	::shipNodeBuildGun			(nodes, *graphics);
	::shipNodeBuildWafer		(nodes, *graphics);
	::shipNodeBuildCannon		(nodes, *graphics);
	::shipNodeBuildShotgun		(nodes, *graphics);
	::shipNodeBuildWaferShotgun	(nodes, *graphics);
	::shipNodeBuildCannonball	(nodes, *graphics);
	::shipNodeBuildRocket		(nodes, *graphics);
	::shipNodeBuildMissile		(nodes, *graphics);
	::shipNodeBuildBullet		(nodes, *graphics);
	::shipNodeBuildShred		(nodes, *graphics);

	//::gpk::geometryBuildCube		(scene.Geometry[::gpk::SHIP_GEOMETRY_CUBE			], {1, 1, 1});
	//::gpk::geometryBuildSphere	(scene.Geometry[::gpk::SHIP_GEOMETRY_SPHERE			], 16U, 16U, .5f, {0, 0});
	//::gpk::geometryBuildCylinder	(scene.Geometry[::gpk::SHIP_GEOMETRY_CYLINDER		],  1U, 32U, .5, .5, {0, 0}, {1, 1, 1});

	return 0;
}

::gpk::error_t			ghg::solarSystemReset	(::ghg::SGalaxyHell & solarSystem)	{	// Set up enemy ships
	::std::lock_guard			lock					(solarSystem.LockUpdate);
	solarSystem.ShipControllers.clear();
	solarSystem.ShipState.Clear();
	solarSystem.DecoState	= {};
	solarSystem.PlayState	= {};
	::gpk::rasterFontDefaults(solarSystem.DecoState.FontManager);
	solarSystem.PilotsReset();
	return 0;
}


::gpk::error_t			ghg::stageSetup			(::ghg::SGalaxyHell & solarSystem)	{	// Set up enemy ships
	stacxpr	uint32_t			partHealth				= 100;

	solarSystem.PlayState.TimeStage		= 0;
	solarSystem.PlayState.TimeRealStage	= 0;
	if(0 == solarSystem.PlayState.GlobalState.Stage) {
		::std::lock_guard			lock(solarSystem.LockUpdate);
		solarSystem.DecoState.Stars.Reset(solarSystem.DrawCache.RenderTargetMetrics);
		solarSystem.PlayState.GlobalState.UserTime	= {};
		solarSystem.PlayState.GlobalState.UserTime.Started = solarSystem.PlayState.GlobalState.UserTime.Loaded = ::gpk::timeCurrent();
		memset(solarSystem.ShipState.SpaceshipManager.ShipScores.begin(), 0, solarSystem.ShipState.SpaceshipManager.ShipScores.byte_count());
		while(solarSystem.Pilots.size() < solarSystem.PlayState.Constants.Players) {
			sc_t						text [64]				= {};
			sprintf_s(text, "Player %i", solarSystem.Pilots.size() + 1);
			solarSystem.Pilots.push_back({::gpk::label(text), PLAYER_COLORS[solarSystem.Pilots.size() % ::gpk::size(PLAYER_COLORS)]});
		}
		solarSystem.ShipControllers.resize(solarSystem.PlayState.Constants.Players);
	}

#pragma pack(push, 1)
	struct SShipOrbiterSetup {
		::gpk::SHIP_PART_TYPE	Type			;
		uint32_t				MaxHealth		;
		::gpk::WEAPON_TYPE		Weapon			;
		double					MaxDelay		;
		double					Stability		;
		::gpk::WEAPON_LOAD		Munition		;
		float					Speed			;
		int32_t					Damage			;
		uint8_t					ParticleCount	;
		double					Cooldown		;
		double					OverheatPerShot	;
		double					ShotLifetime	;
		::gpk::DAMAGE_TYPE		DamageType		;

	};
#pragma pack(pop)

	::gpk::SJSONFile			stageFile					= {};
	sc_t						stageFileName		[256]	= "./%s.json";
	stacxpr SShipOrbiterSetup	weaponDefinitions	[]		=
		{ {::gpk::SHIP_PART_TYPE_Gun			, 128, ::gpk::WEAPON_TYPE_Gun		, .08, 0.975, ::gpk::WEAPON_LOAD_Bullet		,  256,    20, 1,   0,0.00,  1.5, ::gpk::DAMAGE_TYPE_Pierce	}
		, {::gpk::SHIP_PART_TYPE_Shotgun		, 128, ::gpk::WEAPON_TYPE_Shotgun	, .16, 0.925, ::gpk::WEAPON_LOAD_Bullet		,  160,    10, 6,   0,0.00,  1.5, ::gpk::DAMAGE_TYPE_Impact	}
		, {::gpk::SHIP_PART_TYPE_Wafer			, 128, ::gpk::WEAPON_TYPE_Gun		, .24, 0.99, ::gpk::WEAPON_LOAD_Ray			,  480,    30, 1,   1,0.50,  1, ::gpk::DAMAGE_TYPE_Pierce	| ::gpk::DAMAGE_TYPE_Burn	}
		, {::gpk::SHIP_PART_TYPE_ShotgunWafer	, 128, ::gpk::WEAPON_TYPE_Shotgun	, .32, 0.98, ::gpk::WEAPON_LOAD_Ray			,  640,    15, 6,   1,0.50,  0.6, ::gpk::DAMAGE_TYPE_Pierce	| ::gpk::DAMAGE_TYPE_Burn	}
		, {::gpk::SHIP_PART_TYPE_Cannon			, 160, ::gpk::WEAPON_TYPE_Cannon	,   2, 1.00, ::gpk::WEAPON_LOAD_Cannonball	,   48,   156, 1,   0,0.00, 10, ::gpk::DAMAGE_TYPE_Impact	}
		, {::gpk::SHIP_PART_TYPE_Cannon			, 128, ::gpk::WEAPON_TYPE_Cannon	,   2, 0.90, ::gpk::WEAPON_LOAD_Rocket		,   64,    80, 1,   0,0.00,  5, ::gpk::DAMAGE_TYPE_Impact	| ::gpk::DAMAGE_TYPE_Burn | ::gpk::DAMAGE_TYPE_Wave }
		, {::gpk::SHIP_PART_TYPE_Cannon			, 128, ::gpk::WEAPON_TYPE_Cannon	, 2.5, 0.80, ::gpk::WEAPON_LOAD_Missile		,   72,   112, 1,   0,0.00,  5, ::gpk::DAMAGE_TYPE_Burn | ::gpk::DAMAGE_TYPE_Wave }
		, {::gpk::SHIP_PART_TYPE_Shotgun		, 160, ::gpk::WEAPON_TYPE_Shotgun	,   3, 0.95, ::gpk::WEAPON_LOAD_Cannonball	,   72,   112, 7,   0,0.00,  5, ::gpk::DAMAGE_TYPE_Impact	}
		, {::gpk::SHIP_PART_TYPE_Shotgun		, 128, ::gpk::WEAPON_TYPE_Shotgun	,   3, 0.75, ::gpk::WEAPON_LOAD_Rocket		,   80,    80, 6,   0,0.00,  5, ::gpk::DAMAGE_TYPE_Pierce	| ::gpk::DAMAGE_TYPE_Burn | ::gpk::DAMAGE_TYPE_Wave }
		, {::gpk::SHIP_PART_TYPE_Shotgun		, 128, ::gpk::WEAPON_TYPE_Shotgun	, 3.5, 0.50, ::gpk::WEAPON_LOAD_Missile		,   88,   128, 5,   0,0.00,7.5, ::gpk::DAMAGE_TYPE_Burn | ::gpk::DAMAGE_TYPE_Wave }
		, {::gpk::SHIP_PART_TYPE_Gun			, 128, ::gpk::WEAPON_TYPE_Gun		, .12, 0.98, ::gpk::WEAPON_LOAD_Cannonball	,  160,   128, 1,   1,0.25,  5, ::gpk::DAMAGE_TYPE_Impact	}
		, {::gpk::SHIP_PART_TYPE_Gun			, 128, ::gpk::WEAPON_TYPE_Gun		, .12, 0.75, ::gpk::WEAPON_LOAD_Rocket		,  160,    80, 1,   1,0.25,  5, ::gpk::DAMAGE_TYPE_Pierce	| ::gpk::DAMAGE_TYPE_Burn | ::gpk::DAMAGE_TYPE_Wave }
		, {::gpk::SHIP_PART_TYPE_Gun			, 128, ::gpk::WEAPON_TYPE_Gun		, .12, 0.50, ::gpk::WEAPON_LOAD_Missile		,  160,   112, 1,   1,0.25,  5, ::gpk::DAMAGE_TYPE_Burn | ::gpk::DAMAGE_TYPE_Wave }
		};

	sprintf_s(stageFileName, "./levels/%u.json", solarSystem.PlayState.GlobalState.Stage + solarSystem.PlayState.PlaySetup.OffsetStage);
	if(0 <= ::gpk::fileToMemory(stageFileName, stageFile.Bytes) && stageFile.Bytes.size()) {
		::std::lock_guard			lock	(solarSystem.LockUpdate);
		gpk_necall(-1 == ::gpk::jsonParse(stageFile.Reader, stageFile.Bytes), "%s", stageFileName);
	} 
	else {
		::std::lock_guard			lock	(solarSystem.LockUpdate);
		// Set up player ships
		if(solarSystem.PlayState.GlobalState.Stage == 0) {
			solarSystem.ShipState.ShipCoreEntity.clear();
			solarSystem.ShipState.ShipPartEntity.clear();
			solarSystem.PlayState.SimulatedTime			= {};
			solarSystem.PlayState.TimeStage				= 0;
			solarSystem.PlayState.TimeRealStage			= 0;
			solarSystem.ShipState.Clear();
		}
		solarSystem.ShipState.WeaponManager.Weapons.clear();
		solarSystem.ShipState.WeaponManager.Shots.clear();
	

		if(0 == solarSystem.ShipState.SpaceshipManager.ShipCores.size()) { // Create player ship
			solarSystem.PlayState.GlobalState.CameraSwitchDelay	= 0;
			solarSystem.ShipState.Scene.Global.CameraReset();

			for(uint32_t iPlayer = 0; iPlayer < solarSystem.PlayState.Constants.Players; ++iPlayer) {
				const int32_t				indexShip			= ::shipCreate(solarSystem.ShipState, 0, 0, iPlayer);
				::gpk::SBodyCenter			& shipPivot			= solarSystem.ShipState.GetShipPivot(indexShip);
				shipPivot.Orientation.MakeFromEuler({0, 0, (float)(-::gpk::math_pi_2)});
				shipPivot.Position		= {-30};
				shipPivot.Position.z = float(solarSystem.PlayState.Constants.Players * 30) / 2 - iPlayer * 30;
			}
		}
		// Set up enemy ships
		while(((int)solarSystem.ShipState.SpaceshipManager.ShipCores.size() - (int)solarSystem.PlayState.Constants.Players - 1) < (int)(solarSystem.PlayState.GlobalState.Stage + solarSystem.PlayState.PlaySetup.OffsetStage)) {	// Create enemy ships depending on stage.
			int32_t						indexShip				= ::shipCreate(solarSystem.ShipState, 1, -1, solarSystem.PlayState.GlobalState.Stage + solarSystem.ShipState.SpaceshipManager.ShipCores.size());
			::gpk::SBodyCenter			& shipPivot				= solarSystem.ShipState.GetShipPivot(indexShip);
			shipPivot.Orientation.MakeFromEuler({0, 0, (float)(::gpk::math_pi_2)});
			shipPivot.Position		= {5.0f + 5 * solarSystem.ShipState.SpaceshipManager.ShipCores.size()};

			::gpk::au1_t					& enemyShipOrbiters		= solarSystem.ShipState.SpaceshipManager.ShipParts[indexShip];
			for(uint32_t iPart = 0; iPart < enemyShipOrbiters.size(); ++iPart) {
				solarSystem.ShipState.Engine.Integrator.Forces[solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[enemyShipOrbiters[iPart]]].Body].Rotation.y *= float(1 + indexShip * .35);
			}
		}


		cnstxpr int32_t				DEFAULT_NITRO					= 1;
		// set up weapons
		for(uint32_t iShip = 0; iShip < solarSystem.ShipState.SpaceshipManager.ShipCores.size(); ++iShip) {
			::gpk::SSpaceshipCore		& ship							= solarSystem.ShipState.SpaceshipManager.ShipCores[iShip];
			::gpk::au1_t					& shipParts						= solarSystem.ShipState.SpaceshipManager.ShipParts[iShip];
			ship.Health				= 0;
			ship.Nitro				= {DEFAULT_NITRO, DEFAULT_NITRO};

			for(uint32_t iPart = 0; iPart < shipParts.size(); ++iPart) {
				::gpk::SSpaceshipOrbiter			& shipPart						= solarSystem.ShipState.SpaceshipManager.Orbiters[shipParts[iPart]];
				//ship.Team			= iShip ? 1 : 0;
				int32_t					weapon							= 0;
				if(0 == ship.Team) {
					uint32_t				minWeapon						= solarSystem.PlayState.GlobalState.Stage / 4;
					uint32_t				maxWeapon						= ::gpk::min(solarSystem.PlayState.GlobalState.Stage, ::gpk::size(weaponDefinitions) - 1);

					weapon				= (int32_t)::gpk::clamped(minWeapon + iPart, minWeapon, maxWeapon);
					solarSystem.ShipState.SpaceshipManager.ShipOrbiterActionQueue[shipParts[0]]->push_back(::gpk::SHIP_ACTION_Spawn);

				}
				else {
					if(iShip < 4 || 0 != ((iShip - 1 - solarSystem.PlayState.Constants.Players - solarSystem.PlayState.PlaySetup.OffsetStage) % 3) || 0 != iPart) 
						weapon				= 4;
					else {
						weapon				= (iShip - 5) / 3;
					}
					weapon				%= ::gpk::size(weaponDefinitions) >> 1;
				}

				SShipOrbiterSetup				partCreationData	= weaponDefinitions[weapon];
				::gpk::SWeapon					newWeapon			= {};
				shipPart.Health.Value		= (int32_t)(shipPart.Health.Limit = partCreationData.MaxHealth);
				shipPart.Type				= partCreationData.Type;
				newWeapon.Trigger.Delay.Limit		= (float)partCreationData.MaxDelay;
				if(0 != ship.Team)
					newWeapon.Trigger.Delay.Limit		*= 1 + (2 * iPart);
				newWeapon.Type						= partCreationData.Weapon;
				newWeapon.Trigger.Delay.Value		= newWeapon.Trigger.Delay.Limit / shipParts.size() * iPart;
				newWeapon.Trigger.Overheat.Limit	= (float)partCreationData.Cooldown		;//= 1;
				newWeapon.Trigger.OverheatPerShot	= (float)partCreationData.OverheatPerShot	;//= 0;
				newWeapon.Shot.Type					= partCreationData.Munition;
				newWeapon.Shot.Damage				= partCreationData.Damage;
				newWeapon.Shot.Speed				= partCreationData.Speed;
				newWeapon.Shot.Stability			= (float)partCreationData.Stability;
				newWeapon.Shot.ParticleCount		= partCreationData.ParticleCount;
				newWeapon.Shot.Lifetime				= (float)partCreationData.ShotLifetime	;//= 0;

				shipPart.Weapon				= solarSystem.ShipState.WeaponManager.Weapons.push_back(newWeapon);
				solarSystem.ShipState.WeaponManager.Shots.push_back({});
				solarSystem.ShipState.WeaponManager.WeaponTargets.push_back({});

					 if(shipPart.Type == ::gpk::SHIP_PART_TYPE_Gun			) { solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[shipParts[iPart]] + 1].Geometry = ::gpk::SHIP_GEOMETRY_Gun;			}
				else if(shipPart.Type == ::gpk::SHIP_PART_TYPE_Wafer		) { solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[shipParts[iPart]] + 1].Geometry = ::gpk::SHIP_GEOMETRY_Wafer;		}
 				else if(shipPart.Type == ::gpk::SHIP_PART_TYPE_Shotgun		) { solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[shipParts[iPart]] + 1].Geometry = ::gpk::SHIP_GEOMETRY_Shotgun;		}
 				else if(shipPart.Type == ::gpk::SHIP_PART_TYPE_Cannon		) { solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[shipParts[iPart]] + 1].Geometry = ::gpk::SHIP_GEOMETRY_Cannon;		}
 				else if(shipPart.Type == ::gpk::SHIP_PART_TYPE_ShotgunWafer	) { solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[shipParts[iPart]] + 1].Geometry = ::gpk::SHIP_GEOMETRY_WaferShotgun;	}
				ship.Health				+= shipPart.Health.Value;
				const uint32_t				width		= ::gpk::get_value_count<::gpk::WEAPON_LOAD>();
				const uint32_t				height		= ::gpk::get_value_count<::gpk::WEAPON_TYPE>();
				const uint32_t				depth		= ::gpk::get_value_count<::gpk::SHIP_PART_TYPE>();
				solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[shipParts[iPart]] + 1].Image	= iShip * width * height * depth + shipPart.Type * width * height + newWeapon.Type * width + newWeapon.Shot.Type;
				solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[shipParts[iPart]] + 1].Image	%= solarSystem.ShipState.Scene.Image.size();
				solarSystem.ShipState.Engine.Integrator.Centers[solarSystem.ShipState.EntitySystem.Entities[solarSystem.ShipState.ShipPartEntity[shipParts[iPart]] + 1].Body].Orientation = {0, 0, 0, 1};
			}
		}
	}

	++solarSystem.PlayState.GlobalState.Stage;
	solarSystem.PlayState.SimulatedTime.Step	= solarSystem.PlayState.Slowing;

#if defined(GPK_WINDOWS)
	//PlaySoundA((LPCSTR)SND_ALIAS_SYSTEMSTART, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
#endif
	return 0;
}

::gpk::error_t			ghg::solarSystemSetup	(::ghg::SGalaxyHell & solarSystem, const ::gpk::n2u1_t & windowSize)	{
	gpk_necs(::modelsSetup(solarSystem.ShipState.Engine));
	gpk_necs(::modelsSetupOld(solarSystem.ShipState.Scene));

	solarSystem.DecoState.FontManager.Fonts.clear();
	gpk_necs(::gpk::rasterFontDefaults(solarSystem.DecoState.FontManager));

	gpk_necs(::ghg::stageSetup(solarSystem));

	::gpk::m4f32				& matrixProjection		= solarSystem.ShipState.Scene.Global.MatrixProjection;
	matrixProjection.FieldOfView(::gpk::math_pi * .25, windowSize.x / (double)windowSize.y, 0.01, 500.0);

	::gpk::m4f32				matrixViewport			= {};
	matrixViewport.ViewportLH(windowSize.u1_t());
	matrixProjection		*= matrixViewport;
	return 0;
}
