#include "gpk_ascii_color.h"
#include "gpk_pool_game.h"
#include "gpk_gui_text.h"
#include "gpk_pool_shader.h"
#include "gpk_engine_shader.h"

#include "gpk_storage.h"
#include "gpk_deflate.h"
#include "gpk_voxel.h"


::gpk::error_t		d1::poolGameSave		(const ::d1::SPoolGame & game, ::gpk::vcc fileName) {
	::gpk::au8							serialized;
	game.Save(serialized);

	::gpk::au8							deflated;
	::gpk::arrayDeflate(serialized, deflated);

	info_printf("Savegame size in bytes: %u.", serialized.size());
	info_printf("Savegame file size: %u.", deflated.size());

	return ::gpk::fileFromMemory(fileName, deflated);
}

::gpk::error_t		d1::poolGameLoad		(::d1::SPoolGame & world,::gpk::vcc filename) {
	::gpk::au8							serialized;
	gpk_necs(::gpk::fileToMemory(filename, serialized));
	::gpk::au8							inflated;
	::gpk::arrayInflate(serialized, inflated);

	info_printf("Savegame file size: %u.", inflated.size());
	info_printf("Savegame size in bytes: %u.", serialized.size());

	::gpk::vcu8							viewSerialized				= {(const uint8_t*)inflated.begin(), inflated.size()};
	if errored(world.Load(viewSerialized)) {
		::d1::poolGameReset(world);
		return -1;
	}
	return 0;
}


static	::gpk::error_t	poolGameResetTest2Balls		(::d1::SPoolGame & pool, ::d1::SPoolMatchState & startState) { 
	startState.CountBalls					= 2;
	::gpk::SEngine								& engine					= pool.Engine;
	engine.SetPosition(pool.Entities.Balls[0], {0, startState.Table.BallRadius,-.5});
	engine.SetPosition(pool.Entities.Balls[1], {0, startState.Table.BallRadius, .5});
	for(uint32_t iBall = 0; iBall < startState.CountBalls; ++iBall) {
		const uint32_t								iEntity						= pool.Entities.Balls[iBall];
		engine.SetDampingLinear	(iEntity, startState.Physics.Damping.ClothDisplacement);
		engine.SetDampingAngular(iEntity, startState.Physics.Damping.ClothRotation);
		engine.SetHidden		(iEntity, false);
		engine.Integrator.Flags[engine.Entities[iEntity].RigidBody].Collides	= true;
	}
	//::gpk::n3f								velocity					= {0, 0, -1.f - rand() % 30};
	//velocity.RotateY(::gpk::noiseNormal1D(pool.StartState.Seed + 2) / 20 * ((rand() % 2) ? -1 : 1));
	//engine.SetVelocity(pool.StartState.Balls[1].Entity, velocity);
	return 0;
}

static	::gpk::error_t	textureBallNumber			(::gpk::view2d<::gpk::bgra> view, uint32_t number, const ::gpk::SRasterFont & font) { 
	char										strNumber[4]				= {};
	sprintf_s(strNumber, "%i", number);
	const ::gpk::rect2<int16_t>			targetRect					= 
		{ {int16_t(view.metrics().x / 2 - (font.CharSize.x * strlen(strNumber)) / 2), int16_t(view.metrics().y / 2 - font.CharSize.y / 2)}
		, font.CharSize.Cast<int16_t>()
		};
	::gpk::apod<::gpk::n2<uint16_t>>		coords;
	::gpk::textLineRaster(view.metrics().Cast<uint16_t>(), font.CharSize, targetRect, font.Texture, strNumber, coords);
	for(uint32_t iCoord = 0; iCoord < coords.size(); ++iCoord) {
		const ::gpk::n2<uint16_t>				coord						= coords[iCoord];
		view[coord.y][coord.x]					= ::gpk::BLACK;
	}
	return 0; 
}

static	::gpk::error_t	poolGameResetBall10		(::d1::SPoolGame & pool, ::d1::SPoolMatchState & /*startState*/) { (void)pool; return 0; }
static	::gpk::error_t	poolGameResetBall9		(::d1::SPoolGame & pool, ::d1::SPoolMatchState & /*startState*/) { (void)pool; return 0; }
static	::gpk::error_t	poolGameResetBall8		(::d1::SPoolGame & pool, ::d1::SPoolMatchState & startState) {
	startState.CountBalls					= 16;
	::gpk::SEngine								& engine				= pool.Engine;
	const ::gpk::SRasterFont					& font					= *engine.Scene->Graphics->Fonts.Fonts[8];
	for(uint32_t iBall = 0; iBall < startState.CountBalls; ++iBall) {
		::gpk::TFuncPixelShader						& ps					= (0 == iBall) ? ::d1::psBallCue			: (8 >= iBall) ? ::d1::psBallSolid		: ::d1::psBallStripped;
		const ::gpk::vcc							psName					= (0 == iBall) ? ::gpk::vcs{"psBallCue"}	: (8 >= iBall) ? ::gpk::vcs{"psBallSolid"}	: ::gpk::vcs{"psBallStripped"};

		const uint32_t								iEntity					= pool.Entities.Balls[iBall];
		engine.Integrator.Flags[engine.Entities[iEntity].RigidBody].Collides	= true;

		//const bool									stripped				= iBall && iBall > 8;
		engine.SetDampingLinear		(iEntity, startState.Physics.Damping.ClothDisplacement);
		engine.SetDampingAngular	(iEntity, startState.Physics.Damping.ClothRotation);
		engine.SetHidden			(iEntity, false);
		engine.SetOrientation		(iEntity, {0, 0, 1, -1});
		engine.SetShader			(iEntity, ps, psName);

		const ::gpk::SVirtualEntity					& entity				= engine.Entities[iEntity];
		const ::gpk::SRenderNode					& renderNode			= engine.Scene->RenderNodes[entity.RenderNode];
		::gpk::SSkin								& skin					= *engine.Scene->Graphics->Skins[renderNode.Skin];
		::gpk::SSurface								& surface				= *engine.Scene->Graphics->Surfaces[skin.Textures[0]];
		::gpk::SRenderMaterial						& material				= skin.Material;
		::gpk::SColorFloat							color					= pool.BallColors[iBall];
		material.Color.Specular					= ::gpk::WHITE;
		material.Color.Diffuse					= color;
  		material.Color.Ambient					= material.Color.Diffuse * .1f;
		if(iBall) {
			::gpk::view2d<::gpk::bgra>					view					= {(::gpk::bgra*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()};
			textureBallNumber(view, iBall, font);
			//if(0 == iBall)
			//	textureBallCue(view, ::gpk::RED);
			//else if(stripped)
			//	textureBallStripped(view, font, color, iBall);
			//else
			//	textureBallSolid(view, font, color, iBall);
		}
	}

	const uint8_t								ball1					= uint8_t(1 + ::gpk::noise1DBase(startState.Seed + 1) % 7);
	const uint8_t								ball5					= uint8_t(9 + ::gpk::noise1DBase(startState.Seed + 5) % 7);
	::gpk::astaticu8<::d1::MAX_BALLS>			ballOrder				= {};

	ballOrder[0]							= 0;
	ballOrder[1]							= ball1;
	ballOrder[5]							= ball5;
	ballOrder[11]							= 8;

	constexpr char								ballsToSet	[12]		= {2, 3, 4, 6, 7, 8, 9, 10, 12, 13, 14, 15};
	::gpk::au8									ballPool				= {};
	for(uint8_t iBall = 0; ballPool.size() < ::gpk::size(ballsToSet); ++iBall) {
		if(iBall == 8)
			continue;
		if(iBall == 0)
			continue;
		if(iBall == ball1)
			continue;
		if(iBall == ball5)
			continue;
		ballPool.push_back(iBall);
	}

	for(uint32_t iBallToSet = 0; iBallToSet < ::gpk::size(ballsToSet); ++iBallToSet) { 
		uint8_t index = uint8_t(::gpk::noise1DBase32((uint32_t)startState.Seed + iBallToSet) % ballPool.size()); 
		ballOrder[ballsToSet[iBallToSet]]		= ballPool[index];
		gpk_necs(ballPool.remove_unordered(index)); 
	}

	const float									distanceFromCenter		= startState.Table.Dimensions.Slate.x / 4;

	gpk_necs(engine.SetPosition(pool.Entities.Balls[0], {-distanceFromCenter, startState.Table.BallRadius, 0}));
	uint8_t										rowLen					= 5;
	float										ballDiameter			= startState.Table.BallRadius * 2;
	::gpk::n3f									diagonal				= {1, 0, 1};
	diagonal								= diagonal.Normalize() * 1.25f * ballDiameter; 
	for(uint32_t iRow = 0, iBall = 1; iRow < 5; ++iRow, --rowLen) {
		float										offsetZ					= -(rowLen / 2.0f) * ballDiameter + startState.Table.BallRadius;
		for(uint32_t iColumn = 0; iColumn < rowLen; ++iColumn) {
			::gpk::n3f									position				= ::gpk::n3f{(distanceFromCenter + diagonal.x * 5) - iRow * diagonal.x, startState.Table.BallRadius, offsetZ + (float)iColumn * ballDiameter};
			uint32_t									iEntity					= pool.Entities.Balls[ballOrder[iBall++]];
			gpk_necs(engine.SetPosition(iEntity, position));
		}
	}

	cnstxpr double								piPerPocket				= (::gpk::math_pi / 3);
	const ::gpk::n2f32							tableCenter				= {startState.Table.Dimensions.Slate.x * .5f, startState.Table.Dimensions.Slate.y * .5f};
	for(uint32_t iPocket = 0; iPocket < 6; ++iPocket) {
		const uint32_t								iEntity					= pool.Entities.Pockets[iPocket];

		const uint32_t								row						= iPocket / 3;
		const uint32_t								column					= iPocket % 3;
		const ::gpk::n3f32							pocketPosition			= {tableCenter.x * column - tableCenter.x, -startState.Table.Dimensions.PocketRadius, startState.Table.Dimensions.Slate.y * row - tableCenter.y};
		const double								orientationAngle		= (iPocket < 3) ? piPerPocket - piPerPocket * iPocket : (::gpk::math_pi - piPerPocket) + piPerPocket * (iPocket % 3);
		const ::gpk::quatf							pocketOrientation		= ::gpk::quatf{}.CreateFromAxisAngle({0, 1, 0}, orientationAngle).Normalize();
		{ // Set up rigid body
			const uint32_t								iRigidBody				= engine.GetRigidBody(iEntity);
			engine.Integrator.Flags				[iRigidBody]				= {::gpk::BOUNDING_TYPE_Cylinder, true};
			engine.Integrator.Centers			[iRigidBody]				= {pocketPosition, pocketOrientation};
			engine.Integrator.BoundingVolumes	[iRigidBody].HalfSizes		= {startState.Table.Dimensions.PocketRadius, startState.Table.Dimensions.PocketRadius, startState.Table.Dimensions.PocketRadius};
			engine.Integrator.Masses			[iRigidBody].InverseMass	= 1.0f / 9999;
		}
		::gpk::n3f32								pocketScale				= {};
		pocketScale.y = (pocketScale.x = pocketScale.z = startState.Table.Dimensions.PocketRadius * 2);
		gpk_necs(engine.SetMeshScale(iEntity, pocketScale));
		gpk_necs(engine.SetHidden	(iEntity, false));
	}

	//for(uint32_t iCushion = 0; iCushion < 6; ++iCushion) {
	//	const uint32_t								iEntity						= pool.Entities.Cushions[iCushion];
	//	::gpk::n3f32								cushionScale				= {};
	//	cushionScale.x = startState.Table.Dimensions.PlayingSurface.x / 2 - startState.Table.Dimensions.PocketRadius * 4;
	//	cushionScale.z = startState.Table.Dimensions.PlayingSurface.y / 2 - startState.Table.Dimensions.PocketRadius * 2;
	//	::gpk::n3f32								cushionPosition				= {};
	//	gpk_necs(engine.SetMeshScale(iEntity, cushionScale));
	//	if(0 == (iCushion % 3)) {
	//		//cushionPosition.x	= startState.Table.Dimensions.PlayingSurface.x * .5 * (iCushion ? 1 : -1);
	//		//cushionScale		= {startState.Table.Dimensions.PlayingSurface.x, 0, startState.Table.Dimensions.PlayingSurface.y}
	//	}
	//	else {
	//		engine.SetOrientation(iEntity, {0, 1, 0, 1});
	//	}
	//}

	return 0;
}

::gpk::error_t		d1::poolGameReset		(::d1::SPoolGame & pool) {
	pool.MatchState.Reset();
	return ::d1::poolGameReset(pool, pool.MatchState);
}

::gpk::error_t		d1::poolGameReset		(::d1::SPoolGame & pool, ::d1::SPoolMatchState & startState) {
	pool.Engine.Integrator.Reset();

	::gpk::clear(pool.LastFrameContactsBall, pool.LastFrameContactsCushion, pool.TurnHistory, pool.BallEventHistory);
	
	::gpk::SEngine								& engine				= pool.Engine;
	for(uint32_t iBody = 0; iBody < engine.Integrator.Flags.size(); ++iBody)
		engine.Integrator.Flags[iBody].UpdatedTransform		= false;

	for(uint32_t iBall = 0; iBall < ::d1::MAX_BALLS; ++iBall) {
		pool.PositionDeltas[iBall].clear();
		const uint32_t								iEntity						= pool.Entities.Balls[iBall];
		gpk_necs(engine.SetMeshScale(iEntity, {startState.Table.BallRadius * 2, startState.Table.BallRadius * 2, startState.Table.BallRadius * 2}));
		gpk_necs(engine.SetPosition	(iEntity, {}));
		gpk_necs(engine.SetMass		(iEntity, startState.Table.BallWeight / 1000.0f));
		gpk_necs(engine.SetHidden	(iEntity, true));

		engine.Integrator.Flags				[engine.Entities[iEntity].RigidBody].Collides	= false;
		engine.Integrator.BoundingVolumes	[engine.Entities[iEntity].RigidBody].HalfSizes	= {startState.Table.BallRadius, startState.Table.BallRadius, startState.Table.BallRadius};
	}

	gpk_necs(engine.SetMeshScale	(pool.Entities.Table, {startState.Table.Dimensions.Slate.x, startState.Table.Dimensions.Slate.y * .25f, startState.Table.Dimensions.Slate.y}));
	gpk_necs(engine.SetPosition		(pool.Entities.Table, {0, -startState.Table.Dimensions.Slate.y * .125f}));
	//constexpr double							piPerPocket					= (::gpk::math_pi / 3);
	for(uint32_t iPocket = 0; iPocket < pool.Entities.Pockets.size(); ++iPocket) {
		const uint32_t								iEntity						= pool.Entities.Pockets[iPocket];
		gpk_necs(engine.SetMeshScale(iEntity, {startState.Table.Dimensions.PocketRadius * 2, startState.Table.Dimensions.PocketRadius * 2, startState.Table.Dimensions.PocketRadius * 2}));
		gpk_necs(engine.SetHidden	(iEntity, true));
		engine.Integrator.BoundingVolumes[engine.Entities[iEntity].RigidBody].HalfSizes	= {startState.Table.Dimensions.PocketRadius, startState.Table.Dimensions.PocketRadius, startState.Table.Dimensions.PocketRadius};
	}

	for(uint32_t iCushion = 0; iCushion < pool.Entities.Cushions.size(); ++iCushion) {
		const uint16_t								cushionEntity			= pool.Entities.Cushions[iCushion];
		gpk_necs(engine.SetHidden(cushionEntity, true));
		for(uint32_t iFace = 0; iFace < 6; ++iFace) {
			if(engine.Entities.Children[cushionEntity] && engine.Entities.Children[cushionEntity]->size() > iFace)
				gpk_necs(engine.SetHidden((*engine.Entities.Children[cushionEntity])[iFace], true));
		}
	}

	::gpk::m4<float>							mRotation;
	mRotation.SetOrientation(::gpk::quatf{0, 0, 1, 1}.Normalize());
	for(uint8_t iStick = 0; iStick < pool.Entities.Sticks.size(); ++iStick) {
		const uint16_t								stickEntity			= pool.Entities.Sticks[iStick];
		gpk_necs(engine.SetMeshScale	(stickEntity, {.01f, startState.Table.Dimensions.Slate.y, .01f}));
		engine.Scene->RenderNodes.BaseTransforms[engine.Entities[stickEntity].RenderNode].World *= mRotation;
		gpk_necs(engine.SetMeshPosition	(stickEntity, {-startState.Table.BallRadius * 2, 0, 0}));
		gpk_necs(engine.SetHidden		(stickEntity, iStick));
	}

	pool.Teams[0] = pool.Teams[1]			= {2};

	for(uint8_t iPlayer = 0; iPlayer < pool.Players.size(); ++iPlayer) {
		pool.Players[iPlayer].StickControl				= {};
		pool.Teams[iPlayer % 2].Players[iPlayer / 2]	= iPlayer;
	}

	switch(startState.Mode) {
	default:
	case ::d1::POOL_GAME_MODE_9Ball			: gpk_necs(::poolGameResetBall9		(pool, startState)); break;
	case ::d1::POOL_GAME_MODE_8Ball			: gpk_necs(::poolGameResetBall8		(pool, startState)); break;
	case ::d1::POOL_GAME_MODE_10Ball		: gpk_necs(::poolGameResetBall10	(pool, startState)); break;
	case ::d1::POOL_GAME_MODE_Test2Balls	: gpk_necs(::poolGameResetTest2Balls(pool, startState)); break;
	}
	pool.MatchState							= startState;
	gpk_necs(pool.Engine.Update(0));

	return 0;
}

::gpk::error_t		d1::poolGameSetup			(::d1::SPoolGame & pool) {
	::gpk::SEngine								& engine					= pool.Engine;
	gpk_necs(::gpk::rasterFontDefaults(engine.Scene->Graphics->Fonts));

	// balls
	gpk_necs(pool.Entities.Balls[0]	= (uint16_t)engine.CreateSphere());
	gpk_necs(engine.SetShader(pool.Entities.Balls[0], ::d1::psBallSolid, "psBallSolid"));
	for(uint32_t iBall = 1; iBall < ::d1::MAX_BALLS; ++iBall) 
		gpk_necs(pool.Entities.Balls[iBall] = (uint16_t)engine.Clone(pool.Entities.Balls[0], true, true, true));

	// table
	gpk_necs(pool.Entities.Table = (uint16_t)engine.CreateBox());
	for(uint32_t iFace = 0; iFace < 6; ++iFace)
		gpk_necs(engine.SetShader((*engine.Entities.Children[pool.Entities.Table])[iFace], ::d1::psTableCloth, "psHidden"));

	gpk_necs(engine.SetShader((*engine.Entities.Children[pool.Entities.Table])[::gpk::VOXEL_FACE_Top], ::d1::psTableCloth, "psTableCloth"));
	gpk_necs(engine.SetColorDiffuse((*engine.Entities.Children[pool.Entities.Table])[::gpk::VOXEL_FACE_Top], ::gpk::RED * .5f));

	// pockets
	uint16_t									iPocketEntity				= (uint16_t)engine.CreateCylinder(16, true, 0.5f);
	gpk_necs(pool.Entities.Pockets[0] = iPocketEntity);
	gpk_necs(engine.SetColorDiffuse(iPocketEntity, ::gpk::DARKGRAY * .5f));
	gpk_necs(engine.SetShader(iPocketEntity, ::d1::psPocket, "psPocket"));
	for(uint32_t iPocket = 1; iPocket < ::d1::MAX_POCKETS; ++iPocket) 
		gpk_necs(pool.Entities.Pockets[iPocket] = (uint16_t)engine.Clone(pool.Entities.Pockets[0], false, false, false));

	// sticks
	gpk_necs(pool.Entities.Sticks[0] = (uint16_t)engine.CreateCylinder(8, false, 1.0f));
	gpk_necs(engine.SetShader(pool.Entities.Sticks[0], ::d1::psStick, "psStick"));
	for(uint32_t iPlayer = 1; iPlayer < pool.Entities.Sticks.size(); ++iPlayer)
		gpk_necs(pool.Entities.Sticks[iPlayer]	= (uint16_t)engine.Clone(pool.Entities.Sticks[0], true, true, false));

	//gpk_necs(pool.Entities.Cushions[0] = (uint16_t)engine.Clone(pool.Entities.Table, true, false, true));	// reuse box geometry
	//for(uint32_t iFace = 1; iFace < 6; ++iFace)
	//	gpk_necs(engine.SetShader((*engine.Entities.Children[pool.Entities.Cushions[0]])[iFace], ::d1::psTableCloth, "psTableCloth"));
	//
	//for(uint32_t iCushion = 1; iCushion < pool.Entities.Cushions.size(); ++iCushion) 
	//	gpk_necs(pool.Entities.Cushions[iCushion] = (uint16_t)engine.Clone(pool.Entities.Cushions[0], false, false, false));	

	gpk_necs(::d1::poolGameReset(pool));
	return 0;
}

//
//static	::gpk::error_t	textureBallStripped		(::gpk::view2d<::gpk::bgra> view, const ::gpk::SRasterFont & font, ::gpk::bgra color, uint32_t number) { 
//	memset(view.begin(), 0xFF, view.byte_count());
//
//	::gpk::n2<uint32_t>					viewCenter				= view.metrics() / 2;
//	::gpk::SSlice<uint16_t>						colorBand				= {uint16_t(view.metrics().y / 3), uint16_t(view.metrics().y / 3 * 2)};
//	for(uint32_t y = colorBand.Begin; y < colorBand.End; ++y)
//	for(uint32_t x = 0; x < view.metrics().x; ++x) {
//		if((viewCenter - ::gpk::n2<uint32_t>{x, y}).Length() < view.metrics().y / 7)
//			view[y][x]								= ::gpk::WHITE;
//		else
//			view[y][x]								= color;
//	}
//
//	::textureBallNumber(view, number, font);
//	return 0; 
//}
//
//static	::gpk::error_t	textureBallSolid		(::gpk::view2d<::gpk::bgra> view, const ::gpk::SRasterFont & font, ::gpk::bgra color, uint32_t number) { 
//	::gpk::n2<uint32_t>					viewCenter				= view.metrics() / 2;
//
//	for(uint32_t y = 0; y < view.metrics().y; ++y)
//	for(uint32_t x = 0; x < view.metrics().x; ++x) {
//		if((viewCenter - ::gpk::n2<uint32_t>{x, y}).Length() < view.metrics().y / 7)
//			view[y][x]								= ::gpk::WHITE;
//		else
//			view[y][x]								= color;
//	}
//
//	::textureBallNumber(view, number, font);
//	return 0; 
//}
//
//static	::gpk::error_t	textureBallCue			(::gpk::view2d<::gpk::bgra> view, ::gpk::bgra color) {
//	::gpk::n2f						viewCenter				= view.metrics().Cast<float>() / 2;
//	::gpk::n2f						pointCenters[]			= 
//		{ {0, viewCenter.y}
//		, {view.metrics().x / 4.0f * 1, viewCenter.y}
//		, {view.metrics().x / 4.0f * 2, viewCenter.y}
//		, {view.metrics().x / 4.0f * 3, viewCenter.y}
//		, {view.metrics().x * 1.0f, viewCenter.y}
//		};
//	float										pointRadius				= view.metrics().y / 16.0f;
//	if(0 == pointRadius)
//		pointRadius = 3;
//
//	memset(view.begin(), 0xFF, view.byte_count());
//	for(uint32_t y = 0; y < view.metrics().y; ++y)
//	for(uint32_t x = 0; x < view.metrics().x; ++x) {
//		if(y <= pointRadius)
//			view[y][x]								= color;
//		else if(y >= (view.metrics().y - pointRadius - 1.0f))
//			view[y][x]								= color;
//		else {
//			for(uint32_t iPoint = 0; iPoint < ::gpk::size(pointCenters); ++iPoint) {
//				::gpk::n2f					pointCenter			= pointCenters[iPoint];
//				if((pointCenter - ::gpk::n2f{x + .0f, y + .0f}).Length() <= pointRadius + 1.0f)
//					view[y][x]								= color;
//			}
//		}
//	}
//
//	return 0; 
//}

static	::gpk::error_t	geometryBuildTableCushion			(::gpk::SGeometryIndexedTriangles & output) {
	stacxpr	::gpk::n3f						TABLE_CUSHION_POSITIONS	[8]		= 
		{ {0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}	// top face
		, {0, 0, 0}, {1, 0, 0}, {0, 1, 1}, {1, 1, 1}	// diagonal face
		};

	stacxpr	::gpk::n2f						TABLE_CUSHION_UV		[8]		= 
		{ {0, 0}, {1, 0}, {0, 1}, {1, 1}
		, {0, 0}, {1, 0}, {0, 1}, {1, 1}
		};

	stacxpr	::gpk::n3f						TABLE_CUSHION_NORMALS	[8]		= 
		{ {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}
		, {0, -1, -1}, {0, -1, -1}, {0, -1, -1}, {0, -1, -1}
		};

	stacxpr	uint32_t						TABLE_CUSHION_INDICES	[12]	= 
		{ 0 + 0, 0 + 1, 0 + 2, 0 + 1, 0 + 3, 0 + 2
		, 4 + 0, 4 + 2, 4 + 1, 4 + 1, 4 + 2, 4 + 3
		};

	output.PositionIndices				= TABLE_CUSHION_INDICES;
	output.Positions					= TABLE_CUSHION_POSITIONS;
	output.Normals						= TABLE_CUSHION_NORMALS;
	output.TextureCoords				= TABLE_CUSHION_UV;

	for(uint32_t iNormal = 0; iNormal < output.Normals.size(); ++iNormal) 
		output.Normals[iNormal]				= output.Normals[iNormal].Normalize();

	return 0;
}

::gpk::error_t		d1::SPoolEngine::CreateTableCushion	()	{
	::gpk::SGeometryIndexedTriangles		geometry;
	::geometryBuildTableCushion(geometry);

	int32_t									iEntity								= this->Entities.Create();
	Entities.Names[iEntity]		= ::gpk::vcs{"TableCushion"};
	::gpk::SVirtualEntity					& entity							= Entities[iEntity];
	entity.RenderNode					= Scene->RenderNodes.Create();;
	Integrator.BoundingVolumes[entity.RigidBody = this->Integrator.Create()].HalfSizes = {0.5f, 0.5f, 0.5f};

	::gpk::pobj<::gpk::SRenderBuffer>		pIndicesVertex;
	::gpk::pobj<::gpk::SRenderBuffer>		pVertices;
	::gpk::pobj<::gpk::SRenderBuffer>		pNormals;
	::gpk::pobj<::gpk::SRenderBuffer>		pUV;

	pIndicesVertex	->Desc.Format		= ::gpk::DATA_TYPE_UINT16;
	pIndicesVertex	->Desc.Usage		= ::gpk::BUFFER_USAGE_Index;

	pVertices		->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_3;
	pVertices		->Desc.Usage		= ::gpk::BUFFER_USAGE_Position;

	pNormals		->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_3;
	pNormals		->Desc.Usage		= ::gpk::BUFFER_USAGE_Normal;

	pUV				->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_2;
	pUV				->Desc.Usage		= ::gpk::BUFFER_USAGE_UV;

	pIndicesVertex	->Data.resize(geometry.PositionIndices	.byte_count() / 2);
	::gpk::view<uint16_t>					viewIndices							= {(uint16_t*)pIndicesVertex->Data.begin(), geometry.PositionIndices.size()};
	for(uint32_t index = 0; index < geometry.PositionIndices.size(); ++index) {
		viewIndices[index] = (uint16_t)geometry.PositionIndices[index];
	}
	//memcpy(&pIndicesVertex	->Data[0], geometry.PositionIndices	.begin(), pIndicesVertex	->Data.size());

	pVertices	->Data.resize(geometry.Positions		.byte_count());
	pNormals	->Data.resize(geometry.Normals			.byte_count());
	pUV			->Data.resize(geometry.TextureCoords	.byte_count());
	memcpy(&pVertices	->Data[0], geometry.Positions		.begin(), pVertices	->Data.size());
	memcpy(&pNormals	->Data[0], geometry.Normals			.begin(), pNormals	->Data.size());
	memcpy(&pUV			->Data[0], geometry.TextureCoords	.begin(), pUV		->Data.size());

	uint32_t								iVertices				= (uint32_t)Scene->Graphics->Buffers.push_back(pVertices);
	uint32_t								iNormals				= (uint32_t)Scene->Graphics->Buffers.push_back(pNormals);
	uint32_t								iUV						= (uint32_t)Scene->Graphics->Buffers.push_back(pUV);
	uint32_t								iIndicesVertex			= (uint32_t)Scene->Graphics->Buffers.push_back(pIndicesVertex);

	uint32_t								iMesh					= (uint32_t)Scene->Graphics->Meshes.Create();
	::gpk::pobj<::gpk::SGeometryMesh>		& mesh					= Scene->Graphics->Meshes[iMesh];
	Scene->Graphics->Meshes.Names[iMesh]	= ::gpk::vcs{"Sphere"};
	mesh->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});

	mesh->Desc.Mode						= ::gpk::MESH_MODE_List;
	mesh->Desc.Type						= ::gpk::GEOMETRY_TYPE_Triangle;
	mesh->Desc.NormalMode				= ::gpk::NORMAL_MODE_Point;

	uint32_t								iSkin					= (uint32_t)Scene->Graphics->Skins.Create();
	::gpk::pobj<::gpk::SSkin>				& skin					= Scene->Graphics->Skins.Elements[iSkin];
	skin->Material.Color.Ambient		= ::gpk::bgra(::gpk::ASCII_PALETTE[3]) * .1f;
	skin->Material.Color.Diffuse		= ::gpk::bgra(::gpk::ASCII_PALETTE[3]);
	skin->Material.Color.Specular		= ::gpk::WHITE;
	skin->Material.SpecularPower		= 0.5f;

	uint32_t								iSurface				= (uint32_t)Scene->Graphics->Surfaces.Create();
	skin->Textures.push_back(iSurface);

	skin->Material.Color.Ambient		*= .1f;

	::gpk::pobj<::gpk::SSurface>			& surface				= Scene->Graphics->Surfaces[iSurface];
	surface->Desc.ColorType				= ::gpk::COLOR_TYPE_BGRA;
	surface->Desc.MethodCompression		= 0;
	surface->Desc.MethodFilter			= 0;
	surface->Desc.MethodInterlace		= 0;
	surface->Desc.Dimensions			= {32, 32};
	surface->Data.resize(surface->Desc.Dimensions.Area() * sizeof(::gpk::bgra));
	memset(surface->Data.begin(), 0xFF, surface->Data.size());
	::gpk::view2d<::gpk::bgra>				view					= {(::gpk::bgra*)surface->Data.begin(), surface->Desc.Dimensions.Cast<uint32_t>()};
	::gpk::SColorRGBA						color					= {::gpk::ASCII_PALETTE[rand() % 16]};
	for(uint32_t y = surface->Desc.Dimensions.y / 3; y < surface->Desc.Dimensions.y / 3U * 2U; ++y)
	for(uint32_t x = 0; x < surface->Desc.Dimensions.x; ++x)
		view[y][x]	= color;

	mesh->GeometrySlices.resize(1);	// one per face
	::gpk::SGeometrySlice					& slice					= mesh->GeometrySlices[0];
	slice.Slice							= {0, geometry.PositionIndices.size()};

	::gpk::SRenderNode						& renderNode						= Scene->RenderNodes.RenderNodes[entity.RenderNode];
	renderNode.Skin						= iSkin;
	renderNode.Mesh						= iMesh;
	renderNode.Slice					= 0;
	Scene->Graphics->Shaders[renderNode.Shader = Scene->Graphics->Shaders.push_back({})].create(::gpk::psSolid);
	return iEntity;
}
