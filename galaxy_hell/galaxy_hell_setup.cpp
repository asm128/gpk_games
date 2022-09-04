#include "gpk_galaxy_hell.h"
#include "gpk_storage.h"
#include "gpk_json.h"

#include <windows.h>
#include <mmsystem.h>



static	int											shipCreate			(::ghg::SShipState & shipState, int32_t teamId, int32_t iGeometry, int32_t iImage)	{
	::ghg::SShipScene										& scene				= shipState.Scene;
	const uint32_t											countParts			= 6;

	::ghg::SShip											ship				= {};
	{	// Create main ship entity
		::ghg::SEntity											entity				= {-1};
		entity												= {-1};
		entity.Geometry										= -1;	//1 + (iGeometry % 5);
		entity.Transform									= scene.Transforms.push_back(shipState.ShipPhysics.MatrixIdentity4);
		entity.Image										= -1;	//iImage % 5;
		entity.Body											= shipState.ShipPhysics.Create();
		ship.Entity											= shipState.EntitySystem.Create(entity, {});
		ship.Team											= teamId;
		const int32_t											indexBody			= shipState.ShipPhysics.Create(); (void)indexBody;
	}
	const int32_t											indexShip			= shipState.Ships.push_back(ship);
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
		shipState.ShipPhysics.Transforms[entityOrbit.Body].Orientation	= {};
		shipState.ShipPhysics.Transforms[entityOrbit.Body].Orientation.MakeFromEulerTaitBryan(0, (float)(::gpk::math_2pi / countParts * iPart), 0);

		entityPart.Parent									= shipState.EntitySystem.Create(entityOrbit, {});
		entityPart.Geometry									= 1 + ((iGeometry + (iPart % 2)) % 5);
		entityPart.Transform								= scene.Transforms.push_back(shipState.ShipPhysics.MatrixIdentity4);
		entityPart.Image									= (4 + iImage - (iPart % 2)) % 32;
		entityPart.Body										= entityOrbit.Body + 1;
		int32_t													indexEntityPart				= shipState.EntitySystem.Create(entityPart, {});
		shipState.EntitySystem.EntityChildren[entityPart.Parent].push_back(indexEntityPart);
		//solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation.Normalize();

		::ghg::SShipPart										shipPart				= {};
		shipPart.Entity										= entityPart.Parent;
		::gpk::array_pod<uint32_t>								& parentEntityChildren	= shipState.EntitySystem.EntityChildren[ship.Entity];
 		parentEntityChildren.push_back(shipPart.Entity);

		::ghg::SShip											& parentShip			= shipState.Ships[indexShip];
		parentShip.Parts.push_back(shipPart);
	}
	return indexShip;
}

//static	int											engineCreate		(::ghg::SGalaxyHell & solarSystem, int32_t iImage)	{
//	return 0;
//}

static	int											shipGeometryBuildEngine	(::gpk::SGeometryQuads & geometry)			{
	::gpk::geometryBuildHelix(geometry, 8U, 8U, 0.15f, {0, 1.1f, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 1U, 6U, .05f, {0, 1, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 1U, 8U, .35f, {0, 1, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 1U, 8U, .35f, {0, 1, 0}, {-1, 1, 1});
	return 0;
}

static	int											shipGeometryBuildWafer	(::gpk::SGeometryQuads & geometry)			{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildCube(geometry, {1.2f, 0.5f, 1.2f});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, { 0.75f, .75f}, { 1.5, 1, 1.5});
	::gpk::geometryBuildGrid(geometry, {1U, 1U}, {-0.75f, .75f}, {-1.5, 1, 1.5});
	return 0;
}
static	int											shipGeometryBuildGun	(::gpk::SGeometryQuads & geometry)						{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildSphere	(geometry, 8U, 5U, .7f, {0, 0});
	::gpk::geometryBuildFigure0	(geometry, 2U, 8U, 1, {});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, {0, 0}, {1, 1, 1});
	::gpk::geometryBuildCylinder(geometry, 2U, 8U, .25f, {0, 0}, {-1, 1, 1});
	return 0;
}
static	int											shipGeometryBuildCannon	(::gpk::SGeometryQuads & geometry)			{
	::shipGeometryBuildEngine(geometry);
	::gpk::geometryBuildSphere	(geometry, 8U, 5U, .8f, {0, 0});
	::gpk::geometryBuildTender	(geometry, 8U, 8U, .75f, {0, -.5, 0}, { .25, .8f, .25});
	::gpk::geometryBuildTender	(geometry, 8U, 8U, .75f, {0, -.5, 0}, {-.25, .8f, .25});
	return 0;
}

//static	int											shipGeometryBuildSilo	(::gpk::SGeometryQuads & geometry)			{ return 0; }
//static	int											shipGeometryBuildShield	(::gpk::SGeometryQuads & geometry)			{ return 0; }

GDEFINE_ENUM_TYPE (SHIP_GEOMETRY, uint8_t);
GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, ENGINE	, 0);
GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, GUN		, 1);
GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, WAFER		, 2);
GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, CANNON	, 3);
GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, CUBE		, 4);

static	int											modelsSetup				(::ghg::SShipScene & scene)			{
	scene.Geometry.resize(6);

	::shipGeometryBuildEngine	(scene.Geometry[0]);
	::shipGeometryBuildGun		(scene.Geometry[1]);
	::shipGeometryBuildWafer	(scene.Geometry[2]);
	::shipGeometryBuildCannon	(scene.Geometry[3]);

	::gpk::geometryBuildCube	(scene.Geometry[4], {1, 1, 1});
	::gpk::geometryBuildSphere	(scene.Geometry[4], 4U, 2U, 1, {0, 0});
	::gpk::geometryBuildSphere	(scene.Geometry[5], 6U, 2U, 1, {0, 0});

	{
		::gpk::SColorFloat										baseColor	[]			=
			{ ::gpk::LIGHTGREEN
			, ::gpk::LIGHTBLUE
			, ::gpk::LIGHTRED
			, ::gpk::LIGHTCYAN
			, ::gpk::LIGHTORANGE
			, ::gpk::LIGHTYELLOW
			, ::gpk::YELLOW
			, ::gpk::RED
			, ::gpk::BLUE
			, ::gpk::BROWN
			, ::gpk::GRAY
			, ::gpk::PANOCHE
			, ::gpk::TURQUOISE
			};

		scene.Image.resize(32);
		for(uint32_t iImage = 0; iImage < scene.Image.size(); ++iImage) {
			::gpk::SImage<::gpk::SColorBGRA>						& image							= scene.Image[iImage];
			if(image.Texels.size())
				continue;
			image.resize(32, 5);
			for(uint32_t y = 0; y < image.metrics().y; ++y) {// Generate noise color for planet texture
				bool													xAffect						= (y % 2);
				::gpk::SColorFloat										lineColor					= baseColor[(iImage + (rand() % 4)) % (::gpk::size(baseColor) - 4)];
				for(uint32_t x = 0; x < image.metrics().x; ++x) {
					image.Texels[y * image.metrics().x + x]				= (lineColor * (xAffect ? ::gpk::max(.25, ::gpk::sin(x * (1.0 / image.metrics().x * ::gpk::math_pi))) : 1)).Clamp();
				}
			}
		}
	}
	return 0;
}

int													ghg::solarSystemReset					(::ghg::SGalaxyHell & solarSystem)	{	// Set up enemy ships
	solarSystem.ShipState.Scene.Transforms.clear();
	solarSystem.ShipState.Ships							= {};
	solarSystem.ShipState.ShipPhysics					= {};
	solarSystem.ShipState.ShipScores					= {};
	solarSystem.ShipState.ShipActionQueue				= {};
	solarSystem.ShipState.EntitySystem.Entities			= {};
	solarSystem.ShipState.EntitySystem.EntityChildren	= {};
	solarSystem.PlayState								= {};
	solarSystem.Score									= 0;
	solarSystem.Seed									= 0;
	return 0;
}

int													ghg::stageSetup							(::ghg::SGalaxyHell & solarSystem)	{	// Set up enemy ships
	static constexpr	const uint32_t						partHealth								= 10;

	solarSystem.PlayState.TimeStage									= 0;
	solarSystem.PlayState.CameraSwitchDelay							= 0;
	solarSystem.ShipState.Scene.Camera[CAMERA_MODE_SKY].Target				= {};
	solarSystem.ShipState.Scene.Camera[CAMERA_MODE_SKY].Position				= {-0.000001f, 250, 0};
	solarSystem.ShipState.Scene.Camera[CAMERA_MODE_SKY].Up					= {0, 1, 0};

	solarSystem.ShipState.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Target		= {};
	solarSystem.ShipState.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Position		= {-0.000001f, 135, 0};
	solarSystem.ShipState.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Up			= {0, 1, 0};
	solarSystem.ShipState.Scene.Camera[CAMERA_MODE_PERSPECTIVE].Position.RotateZ(::gpk::math_pi * .25);

	if(0 == solarSystem.PlayState.Stage) {
		::gpk::mutex_guard										rtGuard	(solarSystem.DrawCache.RenderTargetQueueMutex);
		solarSystem.DecoState.Stars.Reset(solarSystem.DrawCache.RenderTargetMetrics);
	}

	struct SShipPartSetup {
		::ghg::SHIP_PART_TYPE	Type	;
		::ghg::WEAPON_TYPE		Weapon	;
		::ghg::WEAPON_LOAD		Munition;
		double					MaxDelay;
		int32_t					Damage	;
	};

	::gpk::SJSONFile										stageFile;
	char													stageFileName[256]							= "./%s.json";
	static constexpr const SShipPartSetup					weaponDefinitions		[]				=
		{ {::ghg::SHIP_PART_TYPE_GUN	, ::ghg::WEAPON_TYPE_GUN		, ::ghg::WEAPON_LOAD_BULLET		, .1,  1}
		, {::ghg::SHIP_PART_TYPE_GUN	, ::ghg::WEAPON_TYPE_SHOTGUN	, ::ghg::WEAPON_LOAD_BULLET		, .3,  1}
		, {::ghg::SHIP_PART_TYPE_WAFER	, ::ghg::WEAPON_TYPE_GUN		, ::ghg::WEAPON_LOAD_RAY		, .3,  3}
		, {::ghg::SHIP_PART_TYPE_WAFER	, ::ghg::WEAPON_TYPE_SHOTGUN	, ::ghg::WEAPON_LOAD_RAY		, .6,  3}
		, {::ghg::SHIP_PART_TYPE_CANNON	, ::ghg::WEAPON_TYPE_CANNON		, ::ghg::WEAPON_LOAD_SHELL		,  2, 10}
		, {::ghg::SHIP_PART_TYPE_CANNON	, ::ghg::WEAPON_TYPE_CANNON		, ::ghg::WEAPON_LOAD_WAVE		,  2,  1}
		, {::ghg::SHIP_PART_TYPE_CANNON	, ::ghg::WEAPON_TYPE_CANNON		, ::ghg::WEAPON_LOAD_FLARE		,  2,  1}
		, {::ghg::SHIP_PART_TYPE_SILO	, ::ghg::WEAPON_TYPE_ROCKET		, ::ghg::WEAPON_LOAD_FLARE		,  2,  3}
		, {::ghg::SHIP_PART_TYPE_SHIELD	, ::ghg::WEAPON_TYPE_SHIELD		, ::ghg::WEAPON_LOAD_FLARE		,  2,  3}
		, {::ghg::SHIP_PART_TYPE_SILO	, ::ghg::WEAPON_TYPE_CANNON		, ::ghg::WEAPON_LOAD_RAY		,  2,  3}
		};

	sprintf_s(stageFileName, "./levels/%u.json", solarSystem.PlayState.Stage + solarSystem.PlayState.OffsetStage);
	if(0 <= ::gpk::fileToMemory(stageFileName, stageFile.Bytes) && stageFile.Bytes.size()) {
		gpk_necall(-1 == ::gpk::jsonParse(stageFile.Reader, stageFile.Bytes), "%s", stageFileName);
	} 
	else {
		if(0 == solarSystem.ShipState.Ships.size()) { // Create player ship
			const int32_t											indexShip						= ::shipCreate(solarSystem.ShipState, 0, 0, 0);
			::ghg::SShip											& playerShip					= solarSystem.ShipState.Ships[indexShip];
			::gpk::STransform3										& shipPivot						= solarSystem.ShipState.ShipPhysics.Transforms[solarSystem.ShipState.EntitySystem.Entities[playerShip.Entity].Body];
			shipPivot.Orientation.MakeFromEulerTaitBryan({0, 0, (float)(-::gpk::math_pi_2)});
			shipPivot.Position									= {-30};
		}
		while(((int)solarSystem.ShipState.Ships.size() - 2) < (int)(solarSystem.PlayState.Stage + solarSystem.PlayState.OffsetStage)) {	// Create enemy ships depending on stage.
			int32_t													indexShip						= ::shipCreate(solarSystem.ShipState, 1, solarSystem.PlayState.Stage + solarSystem.ShipState.Ships.size(), solarSystem.PlayState.Stage + solarSystem.ShipState.Ships.size());
			::ghg::SShip											& enemyShip						= solarSystem.ShipState.Ships[indexShip];
			::gpk::STransform3										& shipTransform					= solarSystem.ShipState.ShipPhysics.Transforms[solarSystem.ShipState.EntitySystem.Entities[enemyShip.Entity].Body];
			shipTransform.Orientation.MakeFromEulerTaitBryan({0, 0, (float)(::gpk::math_pi_2)});
			shipTransform.Position								= {5.0f + 5 * solarSystem.ShipState.Ships.size()};
			for(uint32_t iPart = 0; iPart < enemyShip.Parts.size(); ++iPart)
				solarSystem.ShipState.ShipPhysics.Forces[solarSystem.ShipState.EntitySystem.Entities[enemyShip.Parts[iPart].Entity].Body].Rotation.y	*= float(1 + indexShip * .35);
		}

		for(uint32_t iShip = 0; iShip < solarSystem.ShipState.Ships.size(); ++iShip) {
			::ghg::SShip										& ship							= solarSystem.ShipState.Ships[iShip];
			ship.Health										= 0;
			for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
				::ghg::SShipPart									& shipPart						= ship.Parts[iPart];
				shipPart.Health									= partHealth;
				//ship.Team										= iShip ? 1 : 0;
				int32_t												weapon							= 0;
				if(0 == ship.Team)
					weapon											= (iPart % 5) % (solarSystem.ShipState.Ships.size() / 2);
				else {
					if(solarSystem.ShipState.Ships.size() < 5 || 0 != ((iShip - 2 - solarSystem.PlayState.OffsetStage) % 4) || 0 != iPart) 
						weapon											= 4;
					else {
						weapon											= (iShip / 5) % 5;
					}
				}
				shipPart.Type									= weaponDefinitions[weapon].Type;
				shipPart.Weapon.MaxDelay						= weaponDefinitions[weapon].MaxDelay;
				if(0 != ship.Team)
					shipPart.Weapon.MaxDelay							*= 1 + (2 * iPart);
				shipPart.Weapon.Type							= weaponDefinitions[weapon].Weapon;
				shipPart.Weapon.Load							= weaponDefinitions[weapon].Munition;
				shipPart.Weapon.Damage							= weaponDefinitions[weapon].Damage;
				shipPart.Weapon.Delay							= shipPart.Weapon.MaxDelay / ship.Parts.size() * iPart;

					 if(shipPart.Type == ::ghg::SHIP_PART_TYPE_GUN		) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = 1; }
				else if(shipPart.Type == ::ghg::SHIP_PART_TYPE_WAFER	) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = 2; }
 				else if(shipPart.Type == ::ghg::SHIP_PART_TYPE_CANNON	) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = 3; }
				ship.Health										+= shipPart.Health;
			}
		}
	}

	++solarSystem.PlayState.Stage;
	solarSystem.PlayState.Slowing			= true;
#if defined(GPK_WINDOWS)
	PlaySoundA((LPCSTR)SND_ALIAS_SYSTEMSTART, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
#endif
	return 0;
}

int													ghg::solarSystemSetup	(::ghg::SGalaxyHell & solarSystem, const ::gpk::SCoord2<uint16_t> & windowSize)	{
	::ghg::SShipScene										& scene				= solarSystem.ShipState.Scene;
	::modelsSetup(scene);
	::ghg::stageSetup(solarSystem);

	::gpk::SMatrix4<float>						& matrixProjection				= solarSystem.ShipState.Scene.MatrixProjection;
	matrixProjection.FieldOfView(::gpk::math_pi * .25, windowSize.x / (double)windowSize.y, 0.01, 500);
	::gpk::SMatrix4<float>						matrixViewport					= {};
	matrixViewport.ViewportLH(windowSize.Cast<uint32_t>());
	matrixProjection						*= matrixViewport;

	return 0;
}
