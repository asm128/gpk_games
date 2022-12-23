#include "gpk_the_one.h"
#include "gpk_engine_d3d.h"
#include "Sample3DSceneRenderer.h"
#include "SampleFpsTextRenderer.h"


#ifndef TEST_GPK_MESH_H_324234234
#define TEST_GPK_MESH_H_324234234

namespace gpk
{
	GDEFINE_ENUM_TYPE(SHAPE_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Custom		, 0);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Rectangle	, 1);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Circle		, 2);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Ring			, 3);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cube			, 4);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Sphere		, 5);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cylinder		, 6);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Torus		, 7);

} // namespace


#include "gpk_framework.h"
#include "gpk_gui.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423



struct SApplication : DX::IDeviceNotify {
	::gpk::SFramework							Framework				;

	::gpk::SGUI									GUI						= {};
	int32_t										IdViewport				= -1;

	int32_t										EntityLightDirectional	= -1;
	int32_t										EntityLightPoint		= -1;
	int32_t										EntityLightSpot			= -1;
	int32_t										EntityBox				= -1;
	int32_t										EntitySphere			= -1;

	::the1::STheOne								TheOne					= {};

	::gpk::pobj<::DX::D3DDeviceResources>	DeviceResources			;
	::the_one_win32::Sample3DSceneRenderer		D3DScene				;
	::the_one_win32::SampleFpsTextRenderer		D3DText					;

	::gpk::array_pod<uint32_t>					D3DBufferMap			;

	// Notifies renderers that device resources need to be released.
	virtual void								OnDeviceLost					() {
		D3DScene.ReleaseDeviceDependentResources();
		D3DText.ReleaseDeviceDependentResources();
	}

	// Notifies renderers that device resources may now be recreated.
	virtual ::gpk::error_t						OnDeviceRestored				() {
		gpk_necs(D3DScene.CreateDeviceDependentResources());
		gpk_necs(D3DText.CreateDeviceDependentResources());
		gpk_necs(D3DScene.CreateWindowSizeDependentResources());

		gpk_necs(CreateDeviceDependentResources());
		return 0;
	}

	::gpk::error_t								CreateDeviceDependentResources	()	{
		::gpk::SEngine									& engine				= TheOne.MainGame.Game.Engine;
		gpk_necs(::gpk::d3dCreateBuffersFromEngineMeshes		(DeviceResources->GetD3DDevice(), engine.Scene->Graphics->Meshes, engine.Scene->Graphics->Buffers, D3DScene.IndexBuffer, D3DScene.VertexBuffer));
		gpk_necs(::gpk::d3dCreateTexturesFromEngineSurfaces		(DeviceResources->GetD3DDevice(), engine.Scene->Graphics->Surfaces, D3DScene.ShaderResourceView));
		gpk_necs(::gpk::d3dCreatePixelShadersFromEngineShaders	(DeviceResources->GetD3DDevice(), engine.Scene->Graphics->Shaders, D3DScene.PixelShader));
		return 0; 
	}

												SApplication					(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423

#endif // GPK_MESH_H_324234234
