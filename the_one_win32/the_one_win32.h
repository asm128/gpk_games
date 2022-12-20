#include "gpk_the_one.h"
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

struct SGeometryMeshToD3DMap {
	uint32_t		Indices;
	uint32_t		Vertices;
};

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

	::gpk::ptr_obj<::DX::D3DDeviceResources>	DeviceResources			;
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
		for(uint32_t iMesh = 0; iMesh < engine.Scene->Graphics->Meshes.size(); ++iMesh) {
			const ::gpk::ptr_obj<::gpk::SGeometryMesh>		& mesh					= engine.Scene->Graphics->Meshes[iMesh];
			if(!mesh)
				continue;

			::gpk::ptr_obj<::gpk::SRenderBuffer>			engineBufferIndices;
			::gpk::array_pobj<::gpk::SRenderBuffer>			engineBufferVertices;
			::gpk::array_pod<uint32_t>						layoutOffsets;

			uint32_t										vertexSize					= 0;
			for(uint32_t iBuffer = 0; iBuffer < mesh->GeometryBuffers.size(); ++iBuffer) {
				const ::gpk::ptr_obj<::gpk::SRenderBuffer>		& buffer					= engine.Scene->Graphics->Buffers[mesh->GeometryBuffers[iBuffer]];
				if(buffer->Desc.Usage == ::gpk::BUFFER_USAGE_Index) {
					engineBufferIndices							= buffer;
					continue;
				}
				gpk_necs(engineBufferVertices.push_back(buffer));
				gpk_necs(layoutOffsets.push_back(vertexSize));
				vertexSize									+= buffer->Desc.Format.TotalBytes();

			}
			::gpk::array_pod<ubyte_t>						packed;
			for(uint32_t iValue = 0, valueCount = engineBufferVertices[0]->Data.size() / engineBufferVertices[0]->Desc.Format.TotalBytes(); iValue < valueCount; ++iValue) {
				for(uint32_t iBuffer = 0; iBuffer < engineBufferVertices.size(); ++iBuffer) {
					 const ::gpk::SRenderBuffer						& buffer					= *engineBufferVertices[iBuffer];
					::gpk::view_const_ubyte							value						= {&buffer.Data[iValue * buffer.Desc.Format.TotalBytes()], buffer.Desc.Format.TotalBytes()};
					packed.append(value);
				}
			}
			{
				D3D11_SUBRESOURCE_DATA							vertexBufferData			= {packed.begin()};
				D3D11_BUFFER_DESC								vertexBufferDesc			= {packed.size()};
				vertexBufferDesc.BindFlags					= D3D11_BIND_VERTEX_BUFFER;
				{
					::gpk::ptr_com<ID3D11Buffer>					d3dBuffer;
					gpk_hrcall(DeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d3dBuffer));
					D3DScene.VertexBuffer.push_back(d3dBuffer);
				}
			}
			{
				D3D11_SUBRESOURCE_DATA							indexBufferData			= {engineBufferIndices->Data.begin()};
				D3D11_BUFFER_DESC								indexBufferDesc			= {engineBufferIndices->Data.size()};
				indexBufferDesc.BindFlags					= D3D11_BIND_INDEX_BUFFER;
				{
					::gpk::ptr_com<ID3D11Buffer>					d3dBuffer;
					gpk_hrcall(DeviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &d3dBuffer));
					D3DScene.IndexBuffer.push_back(d3dBuffer);
				}
			}
		}

		return 0; 
	}

												SApplication					(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423

#endif // GPK_MESH_H_324234234
