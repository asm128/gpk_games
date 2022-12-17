#include "gpk_pool_game.h"
#include "gpk_noise.h"
#include "gpk_gui_text.h"
#include "gpk_pool_shader.h"
#include "gpk_engine_shader.h"

static	::gpk::error_t					poolGameResetTest2Balls		(::the1::SPoolGame & pool) { 
	pool.StateStart.BallCount				= 2;
	::gpk::SEngine								& engine					= pool.Engine;
	engine.SetPosition(pool.StateStart.Ball[0].Entity, {0, pool.StateStart.BallRadius,-.5});
	engine.SetPosition(pool.StateStart.Ball[1].Entity, {0, pool.StateStart.BallRadius, .5});
	for(uint32_t iBall = 0; iBall < pool.StateStart.BallCount; ++iBall) {
		engine.SetDampingLinear(pool.StateStart.Ball[iBall].Entity, pool.StateStart.Physics.DampingClothDisplacement);
		engine.SetDampingAngular(pool.StateStart.Ball[iBall].Entity, pool.StateStart.Physics.DampingClothRotation);
		engine.SetHidden(pool.StateStart.Ball[iBall].Entity, false);
		engine.Integrator.BodyFlags[engine.ManagedEntities.Entities[pool.StateStart.Ball[iBall].Entity].RigidBody].Collides	= true;
	}
	//::gpk::SCoord3<float>						velocity					= {0, 0, -1.f - rand() % 30};
	//velocity.RotateY(::gpk::noiseNormal1D(pool.StartState.Seed + 2) / 20 * ((rand() % 2) ? -1 : 1));
	//engine.SetVelocity(pool.StartState.Balls[1].Entity, velocity);
	return 0; 
}

static	::gpk::error_t					textureBallNumber			(::gpk::view_grid<::gpk::SColorBGRA> view, uint32_t number, const ::gpk::SRasterFont & font) { 
	char										strNumber[4]				= {};
	sprintf_s(strNumber, "%i", number);
	const ::gpk::SRectangle2<int16_t>			targetRect					= 
		{ {int16_t(view.metrics().x / 2 - (font.CharSize.x * strlen(strNumber)) / 2), int16_t(view.metrics().y / 2 - font.CharSize.y / 2)}
		, font.CharSize.Cast<int16_t>()
		};
	::gpk::array_pod<::gpk::SCoord2<uint16_t>>	coords;
	::gpk::textLineRaster(view.metrics().Cast<uint16_t>(), font.CharSize, targetRect, font.Texture, strNumber, coords);
	for(uint32_t iCoord = 0; iCoord < coords.size(); ++iCoord) {
		const ::gpk::SCoord2<uint16_t>				coord						= coords[iCoord];
		view[coord.y][coord.x]					= ::gpk::BLACK;
	}
	return 0; 
}

static	::gpk::error_t					poolGameResetBall10		(::the1::SPoolGame & pool) { (void)pool; return 0; }
static	::gpk::error_t					poolGameResetBall9		(::the1::SPoolGame & pool) { (void)pool; return 0; }
static	::gpk::error_t					poolGameResetBall8		(::the1::SPoolGame & pool) {
	pool.StateStart.BallCount				= 16;
	::gpk::SEngine								& engine					= pool.Engine;
	const ::gpk::SRasterFont					& font					= *engine.Scene->Graphics->Fonts.Fonts[9];
	for(uint32_t iBall = 0; iBall < pool.StateStart.BallCount; ++iBall) {
		//const bool									stripped				= iBall && iBall > 8;
		engine.SetDampingLinear	(pool.StateStart.Ball[iBall].Entity, pool.StateStart.Physics.DampingClothDisplacement);
		engine.SetDampingAngular	(pool.StateStart.Ball[iBall].Entity, pool.StateStart.Physics.DampingClothRotation);
		engine.SetHidden			(pool.StateStart.Ball[iBall].Entity, false);
		engine.SetOrientation		(pool.StateStart.Ball[iBall].Entity, {0, 0, 1, -1});
		engine.Integrator.BodyFlags[engine.ManagedEntities.Entities[pool.StateStart.Ball[iBall].Entity].RigidBody].Collides	= true;

		const ::gpk::SVirtualEntity					& entity				= engine.ManagedEntities.Entities[pool.StateStart.Ball[iBall].Entity];
		const ::gpk::SRenderNode					& renderNode			= engine.Scene->ManagedRenderNodes.RenderNodes[entity.RenderNode];
		::gpk::SSkin								& skin					= *engine.Scene->Graphics->Skins[renderNode.Skin];
		::gpk::SSurface								& surface				= *engine.Scene->Graphics->Surfaces[skin.Textures[0]];
		::gpk::SRenderMaterial						& material				= skin.Material;
		::gpk::SColorFloat							color					= pool.StateStart.BallColors[iBall];
		material.Color.Specular					= ::gpk::WHITE;
		material.Color.Diffuse					= color;
  		material.Color.Ambient					= material.Color.Diffuse * .1f;
		if(iBall) {
			::gpk::view_grid<::gpk::SColorBGRA>			view					= {(::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()};
			textureBallNumber(view, iBall, font);
			//if(0 == iBall)
			//	textureBallCue(view, ::gpk::RED);
			//else if(stripped)
			//	textureBallStripped(view, font, color, iBall);
			//else
			//	textureBallSolid(view, font, color, iBall);
		}
	}

	const uint32_t								ball1					= 1 + ::gpk::noise1DBase(pool.StateStart.Seed + 1) % 7;
	const uint32_t								ball5					= 9 + ::gpk::noise1DBase(pool.StateStart.Seed + 5) % 7;
	pool.StateStart.BallOrder[0]			= 0;
	pool.StateStart.BallOrder[1]			= ball1;
	pool.StateStart.BallOrder[5]			= ball5;
	pool.StateStart.BallOrder[11]			= 8;

	::gpk::array_pod<uint32_t>					ballPool				= {};
	for(uint32_t iBall = 0; ballPool.size() < 12; ++iBall) {
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

	constexpr char								ballsToSet	[12]		= {2, 3, 4, 6, 7, 8, 9, 10, 12, 13, 14, 15};
	for(uint32_t iBallToSet = 0; iBallToSet < 12; ++iBallToSet) { 
		uint32_t index = ::gpk::noise1DBase32((uint32_t)pool.StateStart.Seed + iBallToSet) % ballPool.size(); 
		pool.StateStart.BallOrder[ballsToSet[iBallToSet]] = ballPool[index]; 
		ballPool.remove_unordered(index); 
	}

	const float									distanceFromCenter		= pool.StateStart.Table.Dimensions.Slate.x / 4;

	engine.SetPosition(pool.StateStart.Ball[0].Entity, {-distanceFromCenter, pool.StateStart.BallRadius, 0});
	uint8_t										rowLen					= 5;
	float										ballDiameter			= pool.StateStart.BallRadius * 2;
	::gpk::SCoord3<float>						diagonal				= {1, 0, 1};
	diagonal								= diagonal.Normalize() * 1.25f * ballDiameter; 
	for(uint32_t iRow = 0, iBall = 1; iRow < 5; ++iRow, --rowLen) {
		float										offsetZ					= -(rowLen / 2.0f) * ballDiameter + pool.StateStart.BallRadius;
		for(uint32_t iColumn = 0; iColumn < rowLen; ++iColumn) {
			::gpk::SCoord3<float>						position				= ::gpk::SCoord3<float>{(distanceFromCenter + diagonal.x * 5) - iRow * diagonal.x, pool.StateStart.BallRadius, offsetZ + (float)iColumn * ballDiameter};
			uint32_t									iEntity					= pool.StateStart.Ball[pool.StateStart.BallOrder[iBall++]].Entity;
			engine.SetPosition(iEntity, position);
			engine.SetRotation(iEntity, {0, 0, 0});
		}
	}
	return 0;
}

::gpk::error_t							the1::poolGameReset		(::the1::SPoolGame & pool, POOL_GAME_MODE mode) {
	::gpk::SEngine								& engine					= pool.Engine;
	pool.StateStart.Mode					= mode;
	pool.StateStart.Seed					= ::gpk::timeCurrentInUs() ^ ::gpk::noise1DBase(::gpk::timeCurrentInUs());
	engine.Integrator.ZeroForces();
	for(uint32_t iBall = 0; iBall < ::the1::MAX_BALLS; ++iBall) {
		pool.PositionDeltas[iBall].clear();
		engine.SetHidden	(pool.StateStart.Ball[iBall].Entity, true);
		engine.SetPosition	(pool.StateStart.Ball[iBall].Entity, {});
		engine.Integrator.BodyFlags		[engine.ManagedEntities.Entities[pool.StateStart.Ball[iBall].Entity].RigidBody].Collides	= false;
		engine.Integrator.Masses			[engine.ManagedEntities.Entities[pool.StateStart.Ball[iBall].Entity].RigidBody].InverseMass	= 1.0f / (pool.StateStart.Physics.BallWeight / 1000.0f);
		engine.Integrator.BoundingVolumes	[engine.ManagedEntities.Entities[pool.StateStart.Ball[iBall].Entity].RigidBody].HalfSizes		= {pool.StateStart.BallRadius, pool.StateStart.BallRadius, pool.StateStart.BallRadius};
	}

	engine.SetPosition	(pool.StateStart.Table.Entity, {0, -pool.StateStart.Table.Dimensions.Slate.y * .125f});
	const ::gpk::SCoord2<float>					tableCenter				= {pool.StateStart.Table.Dimensions.Slate.x * .5f, pool.StateStart.Table.Dimensions.Slate.y * .5f};
	for(uint32_t iPocket = 0; iPocket < 6; ++iPocket) {
		const uint32_t								row						= iPocket / 3;
		const uint32_t								column					= iPocket % 3;
		const ::gpk::SCoord3<float>					pocketPosition			= {tableCenter.x * column - tableCenter.x, -pool.StateStart.Table.PocketRadius, pool.StateStart.Table.Dimensions.Slate.y * row - tableCenter.y};
		engine.SetHidden		(pool.StateStart.Table.Pockets[iPocket].Entity, false);
		engine.SetPosition		(pool.StateStart.Table.Pockets[iPocket].Entity, pocketPosition);
	}
	switch(mode) {
	default:
	case POOL_GAME_MODE_8Ball		: gpk_necs(::poolGameResetBall8		(pool)); break;
	case POOL_GAME_MODE_9Ball		: gpk_necs(::poolGameResetBall9		(pool)); break;
	case POOL_GAME_MODE_10Ball		: gpk_necs(::poolGameResetBall10	(pool)); break;
	case POOL_GAME_MODE_Test2Balls	: gpk_necs(::poolGameResetTest2Balls(pool)); break;
	}
	pool.StateCurrent						= pool.StateStart;
	return 0;
}

::gpk::error_t							the1::poolGameSetup			(::the1::SPoolGame & pool, POOL_GAME_MODE mode) {
	::gpk::SEngine								& engine					= pool.Engine;
	gpk_necs(::gpk::rasterFontDefaults(engine.Scene->Graphics->Fonts));

	// balls
	gpk_necs(pool.StateStart.Ball[0].Entity = engine.CreateSphere());
	engine.SetMeshScale(pool.StateStart.Ball[0].Entity, {pool.StateStart.BallRadius * 2, pool.StateStart.BallRadius * 2, pool.StateStart.BallRadius * 2});
	engine.SetShader(pool.StateStart.Ball[0].Entity, ::the1::shaderBall);
	for(uint32_t iBall = 1; iBall < ::the1::MAX_BALLS; ++iBall) {
		gpk_necs(pool.StateStart.Ball[iBall].Entity = engine.Clone(pool.StateStart.Ball[0].Entity, true, true));
	}

	// table
	gpk_necs(pool.StateStart.Table.Entity = engine.CreateBox());
	engine.SetMeshScale(pool.StateStart.Table.Entity, {pool.StateStart.Table.Dimensions.Slate.x, pool.StateStart.Table.Dimensions.Slate.y * .25f, pool.StateStart.Table.Dimensions.Slate.y});
	engine.SetShader((*engine.ManagedEntities.Children[pool.StateStart.Table.Entity])[0], ::the1::shaderCloth);
	for(uint32_t iFace = 1; iFace < 6; ++iFace)
		engine.SetShader((*engine.ManagedEntities.Children[pool.StateStart.Table.Entity])[iFace], ::gpk::shaderHidden);

	// pockets
	uint32_t									iPocketEntity				= engine.CreateCylinder(8, true, 0.5f);
	gpk_necs(pool.StateStart.Table.Pockets[0].Entity = iPocketEntity);
	engine.SetMeshScale(pool.StateStart.Table.Pockets[0].Entity, {pool.StateStart.Table.PocketRadius * 2, pool.StateStart.Table.PocketRadius * 2, pool.StateStart.Table.PocketRadius * 2});
	engine.SetShader(iPocketEntity, ::the1::shaderHole);
	for(uint32_t iPocket = 1; iPocket < 6; ++iPocket) {
		gpk_necs(pool.StateStart.Table.Pockets[iPocket].Entity = engine.Clone(pool.StateStart.Table.Pockets[0].Entity, false, false));
	}

	for(uint32_t iPocket = 0; iPocket < 6; ++iPocket) {
		uint32_t									iEntityHole				= pool.StateStart.Table.Pockets[iPocket].Entity;
		if(iPocket < 3)
			engine.SetOrientation(iEntityHole, ::gpk::SQuaternion<float>{}.CreateFromAxisAngle({0, 1, 0}, (::gpk::math_pi / 3) - ::gpk::math_pi / 3 * iPocket).Normalize());
		else 
			engine.SetOrientation(iEntityHole, ::gpk::SQuaternion<float>{}.CreateFromAxisAngle({0, 1, 0}, (::gpk::math_pi - ::gpk::math_pi / 3) + ::gpk::math_pi / 3 * (iPocket % 3)).Normalize());
	}


	// sticks
	gpk_necs(pool.StateStart.Player[0].Stick.Entity = engine.CreateCylinder(8, false, 1.0f));
	engine.SetShader(pool.StateStart.Player[0].Stick.Entity, ::the1::shaderStick);
	engine.SetMeshScale(pool.StateStart.Player[0].Stick.Entity, {.01f, pool.StateStart.Table.Dimensions.Slate.y, .01f});
	::gpk::SMatrix4<float>				mRotation;
	mRotation.SetOrientation(::gpk::SQuaternion<float>{0, 0, 1, 1}.Normalize());
	engine.Scene->ManagedRenderNodes.BaseTransforms[engine.ManagedEntities.Entities[pool.StateStart.Player[0].Stick.Entity].RenderNode].World *= mRotation;
	engine.SetMeshPosition	(pool.StateStart.Player[0].Stick.Entity, {-pool.StateStart.BallRadius * 2, 0, 0});
	for(uint32_t iPlayer = 1; iPlayer < ::gpk::size(pool.StateStart.Player); ++iPlayer) {
		gpk_necs(pool.StateStart.Player[iPlayer].Stick.Entity = engine.Clone(pool.StateStart.Player[0].Stick.Entity, true, true));
		engine.SetHidden(pool.StateStart.Player[iPlayer].Stick.Entity, true);
	}
	::the1::poolGameReset(pool, mode);
	return 0;
}

//
//static	::gpk::error_t					textureBallStripped		(::gpk::view_grid<::gpk::SColorBGRA> view, const ::gpk::SRasterFont & font, ::gpk::SColorBGRA color, uint32_t number) { 
//	memset(view.begin(), 0xFF, view.byte_count());
//
//	::gpk::SCoord2<uint32_t>					viewCenter				= view.metrics() / 2;
//	::gpk::SSlice<uint16_t>						colorBand				= {uint16_t(view.metrics().y / 3), uint16_t(view.metrics().y / 3 * 2)};
//	for(uint32_t y = colorBand.Begin; y < colorBand.End; ++y)
//	for(uint32_t x = 0; x < view.metrics().x; ++x) {
//		if((viewCenter - ::gpk::SCoord2<uint32_t>{x, y}).Length() < view.metrics().y / 7)
//			view[y][x]								= ::gpk::WHITE;
//		else
//			view[y][x]								= color;
//	}
//
//	::textureBallNumber(view, number, font);
//	return 0; 
//}
//
//static	::gpk::error_t					textureBallSolid		(::gpk::view_grid<::gpk::SColorBGRA> view, const ::gpk::SRasterFont & font, ::gpk::SColorBGRA color, uint32_t number) { 
//	::gpk::SCoord2<uint32_t>					viewCenter				= view.metrics() / 2;
//
//	for(uint32_t y = 0; y < view.metrics().y; ++y)
//	for(uint32_t x = 0; x < view.metrics().x; ++x) {
//		if((viewCenter - ::gpk::SCoord2<uint32_t>{x, y}).Length() < view.metrics().y / 7)
//			view[y][x]								= ::gpk::WHITE;
//		else
//			view[y][x]								= color;
//	}
//
//	::textureBallNumber(view, number, font);
//	return 0; 
//}
//
//static	::gpk::error_t					textureBallCue			(::gpk::view_grid<::gpk::SColorBGRA> view, ::gpk::SColorBGRA color) {
//	::gpk::SCoord2<float>						viewCenter				= view.metrics().Cast<float>() / 2;
//	::gpk::SCoord2<float>						pointCenters[]			= 
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
//				::gpk::SCoord2<float>					pointCenter			= pointCenters[iPoint];
//				if((pointCenter - ::gpk::SCoord2<float>{x + .0f, y + .0f}).Length() <= pointRadius + 1.0f)
//					view[y][x]								= color;
//			}
//		}
//	}
//
//	return 0; 
//}
