#include "gpk_galaxy_hell.h"
#include "gpk_storage.h"
#include "gpk_json.h"
#include "gpk_noise.h"
#include "gpk_deflate.h"

#include <windows.h>
#include <mmsystem.h>

::gpk::error_t					ghg::solarSystemSave		(const ::ghg::SGalaxyHell & game, ::gpk::vcc fileName) {
	::gpk::array_pod<byte_t>			serialized;
	game.Save(serialized);
	::gpk::array_pod<byte_t>			deflated;
	::gpk::arrayDeflate(serialized, deflated);
	return ::gpk::fileFromMemory(fileName, deflated);
}

::gpk::error_t					ghg::solarSystemLoad		(::ghg::SGalaxyHell & world,::gpk::vcc filename) {
	::gpk::array_pod<byte_t>			serialized;
	world.PlayState.TimeLast		= ::gpk::timeCurrent();
	gpk_necall(::gpk::fileToMemory(filename, serialized), "%s", "");
	::gpk::array_pod<byte_t>			inflated;
	::gpk::arrayInflate(serialized, inflated);
	::gpk::view_array<const byte_t>		viewSerialized			= {(const byte_t*)inflated.begin(), inflated.size()};
	world.Load(viewSerialized);
	world.PlayState.Paused			= true;
	return 0;
}

static	int											shipCreate			(::ghg::SShipState & shipState, int32_t teamId, int32_t iGeometry, int32_t iImage)	{
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
		shipState.ShipPhysics.Transforms[entityOrbit.Body].Orientation	= {};
		shipState.ShipPhysics.Transforms[entityOrbit.Body].Orientation.MakeFromEulerTaitBryan(0, (float)(::gpk::math_2pi / countParts * iPart), 0);

		entityPart.Parent									= shipState.EntitySystem.Create(entityOrbit, {});
		entityPart.Geometry									= 1 + ((iGeometry + (iPart % 2)) % 5);
		entityPart.Transform								= scene.Transforms.push_back(shipState.ShipPhysics.MatrixIdentity4);
		entityPart.Image									= iImage % shipState.Scene.Image.size();
		entityPart.Body										= entityOrbit.Body + 1;
		int32_t													indexEntityPart				= shipState.EntitySystem.Create(entityPart, {});
		shipState.EntitySystem.EntityChildren[entityPart.Parent].push_back(indexEntityPart);
		//solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation.Normalize();

		::ghg::SOrbiter										shipPart				= {};
		shipPart.Entity										= entityPart.Parent;
		shipState.ShipParts[indexShip].push_back(shipState.Orbiters.push_back(shipPart));
		shipState.ShipOrbitersDistanceToTargets.push_back({});
		shipState.ShipOrbiterActionQueue.push_back({});

		::gpk::array_pod<uint32_t>								& parentEntityChildren	= shipState.EntitySystem.EntityChildren[ship.Entity];
		parentEntityChildren.push_back(shipPart.Entity);
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

		::gpk::array_pod<::gpk::SColorFloat>	baseColors;
		for(uint32_t i = 0; i < 4096; ++i)
			baseColors.push_back({(float)::gpk::noiseNormal1D(rand()), (float)::gpk::noiseNormal1D(rand()), (float)::gpk::noiseNormal1D(rand()), 1});

		scene.Image.resize(256);
		for(uint32_t iImage = 0; iImage < scene.Image.size(); ++iImage) {
			::gpk::SImage<::gpk::SColorBGRA>						& image							= scene.Image[iImage];
			if(image.Texels.size())
				continue;
			image.resize(32, 5);
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
	::gpk::clear(solarSystem.ShipState.Scene.Transforms, solarSystem.ShipState.Orbiters, solarSystem.ShipState.Weapons, solarSystem.ShipState.ShipCores);
	solarSystem.ShipState.Shots							= {};
	solarSystem.ShipState.ShipParts				= {};
	solarSystem.ShipState.ShipPhysics					= {};
	solarSystem.ShipState.ShipOrbiterActionQueue		= {};
	solarSystem.ShipState.EntitySystem.Entities			= {};
	solarSystem.ShipState.EntitySystem.EntityChildren	= {};
	solarSystem.PlayState								= {};
	return 0;
}

int													ghg::stageSetup							(::ghg::SGalaxyHell & solarSystem)	{	// Set up enemy ships
	static constexpr	const uint32_t						partHealth								= 100;

	solarSystem.PlayState.TimeStage									= 0;
	solarSystem.PlayState.TimeRealStage								= 0;
	if(0 == solarSystem.PlayState.Stage) {
		::gpk::mutex_guard										rtGuard	(solarSystem.DrawCache.RenderTargetQueueMutex);
		solarSystem.DecoState.Stars.Reset(solarSystem.DrawCache.RenderTargetMetrics);
		solarSystem.PlayState.TimeStart = solarSystem.PlayState.TimeLast = ::gpk::timeCurrent();
		memset(solarSystem.ShipState.ShipScores.begin(), 0, solarSystem.ShipState.ShipScores.byte_count());
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
		double					Cooldown			;//= 1;
		double					OverheatPerShot		;//= 0;
		double					ShotLifetime		;//= 0;
		::ghg::WEAPON_DAMAGE	DamageType		;

	};
#pragma pack(pop)

	::gpk::SJSONFile										stageFile;
	char													stageFileName[256]							= "./%s.json";
	static constexpr const SShipOrbiterSetup					weaponDefinitions		[]				=
		{ {::ghg::SHIP_PART_TYPE_Gun	, 128, ::ghg::WEAPON_TYPE_Gun		, .08, 0.975, ::ghg::WEAPON_LOAD_Bullet		,  256,    20, 1,   0,0.00,  5, ::ghg::WEAPON_DAMAGE_Pierce	}
		, {::ghg::SHIP_PART_TYPE_Gun	, 128, ::ghg::WEAPON_TYPE_Shotgun	, .16, 0.925, ::ghg::WEAPON_LOAD_Shell		,  160,    10, 6,   0,0.00,  5, ::ghg::WEAPON_DAMAGE_Impact	}
		, {::ghg::SHIP_PART_TYPE_Wafer	, 128, ::ghg::WEAPON_TYPE_Gun		, .24, 0.99, ::ghg::WEAPON_LOAD_Ray			,  480,    30, 1,   1,0.50,  5, ::ghg::WEAPON_DAMAGE_Pierce	| ::ghg::WEAPON_DAMAGE_Burn	}
		, {::ghg::SHIP_PART_TYPE_Wafer	, 128, ::ghg::WEAPON_TYPE_Shotgun	, .32, 0.98, ::ghg::WEAPON_LOAD_Ray			,  320,    15, 6,   1,0.50,  5, ::ghg::WEAPON_DAMAGE_Pierce	| ::ghg::WEAPON_DAMAGE_Burn	}
		, {::ghg::SHIP_PART_TYPE_Cannon	, 160, ::ghg::WEAPON_TYPE_Cannon	,   2, 1.00, ::ghg::WEAPON_LOAD_Cannonball	,   32,   156, 1,   0,0.00, 10, ::ghg::WEAPON_DAMAGE_Impact	}
		, {::ghg::SHIP_PART_TYPE_Cannon	, 128, ::ghg::WEAPON_TYPE_Cannon	,   2, 0.90, ::ghg::WEAPON_LOAD_Rocket		,   64,    80, 1,   0,0.00,  5, ::ghg::WEAPON_DAMAGE_Impact	| ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		, {::ghg::SHIP_PART_TYPE_Cannon	, 128, ::ghg::WEAPON_TYPE_Cannon	, 2.5, 0.80, ::ghg::WEAPON_LOAD_Missile		,   72,   112, 1,   0,0.00,  5, ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		, {::ghg::SHIP_PART_TYPE_Gun	, 160, ::ghg::WEAPON_TYPE_Shotgun	,   3, 0.95, ::ghg::WEAPON_LOAD_Cannonball	,   72,   112, 7,   0,0.00,  5, ::ghg::WEAPON_DAMAGE_Impact	}
		, {::ghg::SHIP_PART_TYPE_Gun	, 128, ::ghg::WEAPON_TYPE_Shotgun	,   3, 0.75, ::ghg::WEAPON_LOAD_Rocket		,   80,    80, 6,   0,0.00,  5, ::ghg::WEAPON_DAMAGE_Pierce	| ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		, {::ghg::SHIP_PART_TYPE_Gun	, 128, ::ghg::WEAPON_TYPE_Shotgun	, 3.5, 0.50, ::ghg::WEAPON_LOAD_Missile		,   88,   128, 5,   0,0.00,7.5, ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		, {::ghg::SHIP_PART_TYPE_Gun	, 128, ::ghg::WEAPON_TYPE_Gun		, .12, 0.98, ::ghg::WEAPON_LOAD_Cannonball	,  160,   128, 1,   1,0.25,  5, ::ghg::WEAPON_DAMAGE_Impact	}
		, {::ghg::SHIP_PART_TYPE_Gun	, 128, ::ghg::WEAPON_TYPE_Gun		, .12, 0.75, ::ghg::WEAPON_LOAD_Rocket		,  160,    80, 1,   1,0.25,  5, ::ghg::WEAPON_DAMAGE_Pierce	| ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		, {::ghg::SHIP_PART_TYPE_Gun	, 128, ::ghg::WEAPON_TYPE_Gun		, .12, 0.50, ::ghg::WEAPON_LOAD_Missile		,  160,   112, 1,   1,0.25,  5, ::ghg::WEAPON_DAMAGE_Burn | ::ghg::WEAPON_DAMAGE_Wave }
		};

	sprintf_s(stageFileName, "./levels/%u.json", solarSystem.PlayState.Stage + solarSystem.PlayState.OffsetStage);
	if(0 <= ::gpk::fileToMemory(stageFileName, stageFile.Bytes) && stageFile.Bytes.size()) {
		gpk_necall(-1 == ::gpk::jsonParse(stageFile.Reader, stageFile.Bytes), "%s", stageFileName);
	} 
	else {
		// Set up player ships
		if(solarSystem.PlayState.Stage == 0) {
			solarSystem.ShipState.ShipPhysics = {};
			solarSystem.ShipState.EntitySystem = {};
			::gpk::clear
				( solarSystem.ShipState.ShipScores						
				, solarSystem.ShipState.ShipCores						
				, solarSystem.ShipState.ShipParts					
				, solarSystem.ShipState.Orbiters					
				, solarSystem.ShipState.ShipOrbitersDistanceToTargets	
				, solarSystem.ShipState.ShipOrbiterActionQueue			
			);
		}
		solarSystem.ShipState.Weapons.clear();
		solarSystem.ShipState.Shots.clear();
	
		if(0 == solarSystem.ShipState.ShipCores.size()) { // Create player ship
			solarSystem.PlayState.CameraSwitchDelay							= 0;
			solarSystem.ShipState.Scene.Global.Camera[CAMERA_MODE_SKY].Target				= {};
			solarSystem.ShipState.Scene.Global.Camera[CAMERA_MODE_SKY].Position				= {-0.000001f, 250, 0};
			solarSystem.ShipState.Scene.Global.Camera[CAMERA_MODE_SKY].Up					= {0, 1, 0};

			solarSystem.ShipState.Scene.Global.Camera[CAMERA_MODE_PERSPECTIVE].Target		= {};
			solarSystem.ShipState.Scene.Global.Camera[CAMERA_MODE_PERSPECTIVE].Position		= {-0.000001f, 200, 0};
			solarSystem.ShipState.Scene.Global.Camera[CAMERA_MODE_PERSPECTIVE].Up			= {0, 1, 0};
			solarSystem.ShipState.Scene.Global.Camera[CAMERA_MODE_PERSPECTIVE].Position.RotateZ(::gpk::math_pi * .30);

			for(uint32_t iPlayer = 0; iPlayer < solarSystem.PlayState.PlayerCount; ++iPlayer) {
				const int32_t											indexShip						= ::shipCreate(solarSystem.ShipState, 0, 0, 0);
				::ghg::SShipCore										& playerShip					= solarSystem.ShipState.ShipCores[indexShip];
				::gpk::STransform3										& shipPivot						= solarSystem.ShipState.ShipPhysics.Transforms[solarSystem.ShipState.EntitySystem.Entities[playerShip.Entity].Body];
				shipPivot.Orientation.MakeFromEulerTaitBryan({0, 0, (float)(-::gpk::math_pi_2)});
				shipPivot.Position									= {-30};
				shipPivot.Position.z = -float(solarSystem.PlayState.PlayerCount * 30) / 2 + iPlayer * 30;
			}
		}
		// Set up enemy ships
		while(((int)solarSystem.ShipState.ShipCores.size() - (int)solarSystem.PlayState.PlayerCount - 1) < (int)(solarSystem.PlayState.Stage + solarSystem.PlayState.OffsetStage)) {	// Create enemy ships depending on stage.
			int32_t													indexShip						= ::shipCreate(solarSystem.ShipState, 1, solarSystem.PlayState.Stage + solarSystem.ShipState.ShipCores.size(), solarSystem.PlayState.Stage + solarSystem.ShipState.ShipCores.size());
			::ghg::SShipCore										& enemyShip						= solarSystem.ShipState.ShipCores[indexShip];
			::gpk::array_pod<uint32_t>								& enemyShipOrbiters				= solarSystem.ShipState.ShipParts[indexShip];
			::gpk::STransform3										& shipTransform					= solarSystem.ShipState.ShipPhysics.Transforms[solarSystem.ShipState.EntitySystem.Entities[enemyShip.Entity].Body];
			shipTransform.Orientation.MakeFromEulerTaitBryan({0, 0, (float)(::gpk::math_pi_2)});
			shipTransform.Position								= {5.0f + 5 * solarSystem.ShipState.ShipCores.size()};
			for(uint32_t iPart = 0; iPart < enemyShipOrbiters.size(); ++iPart) {
				const ::ghg::SOrbiter								& shipPart					= solarSystem.ShipState.Orbiters[enemyShipOrbiters[iPart]];
				solarSystem.ShipState.ShipPhysics.Forces[solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity].Body].Rotation.y	*= float(1 + indexShip * .35);
			}
		}


		// set up weapons
		for(uint32_t iShip = 0; iShip < solarSystem.ShipState.ShipCores.size(); ++iShip) {
			::ghg::SShipCore										& ship							= solarSystem.ShipState.ShipCores[iShip];
			::gpk::array_pod<uint32_t>								& shipParts						= solarSystem.ShipState.ShipParts[iShip];
			ship.Health										= 0;
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
					if(iShip < 4 || 0 != ((iShip - 1 - solarSystem.PlayState.PlayerCount - solarSystem.PlayState.OffsetStage) % 3) || 0 != iPart) 
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

					 if(shipPart.Type == ::ghg::SHIP_PART_TYPE_Gun		) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = 1; }
				else if(shipPart.Type == ::ghg::SHIP_PART_TYPE_Wafer	) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = 2; }
 				else if(shipPart.Type == ::ghg::SHIP_PART_TYPE_Cannon	) { solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Geometry = 3; }
				ship.Health										+= shipPart.Health;
				const uint32_t										width		= ::gpk::get_value_count<::ghg::WEAPON_LOAD>();
				const uint32_t										height		= ::gpk::get_value_count<::ghg::WEAPON_TYPE>();
				const uint32_t										depth		= ::gpk::get_value_count<::ghg::SHIP_PART_TYPE>();
				solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Image	= iShip * width * height * depth + shipPart.Type * width * height + newWeapon.Type * width + newWeapon.Load;
				solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Image	%= solarSystem.ShipState.Scene.Image.size();
				solarSystem.ShipState.ShipPhysics.Transforms[solarSystem.ShipState.EntitySystem.Entities[shipPart.Entity + 1].Body].Orientation = {0, 0, 0, 1};
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

int										ghg::solarSystemSetup			(::ghg::SGalaxyHell & solarSystem, const ::gpk::SCoord2<uint16_t> & windowSize)	{
	::ghg::SShipScene							& scene							= solarSystem.ShipState.Scene;
	::modelsSetup(scene);

	::ghg::stageSetup(solarSystem);

	::gpk::SMatrix4<float>						& matrixProjection				= solarSystem.ShipState.Scene.Global.MatrixProjection;
	matrixProjection.FieldOfView(::gpk::math_pi * .25, windowSize.x / (double)windowSize.y, 0.01, 500);
	::gpk::SMatrix4<float>						matrixViewport					= {};
	matrixViewport.ViewportLH(windowSize.Cast<uint32_t>());
	matrixProjection						*= matrixViewport;
	return 0;
}
