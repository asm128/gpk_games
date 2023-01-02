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
	::gpk::SFramework						Framework				;

	::gpk::SGUI								GUI						= {};
	int32_t									IdViewport				= -1;
	::the1::STheOne							TheOne					= {};

	::gpk::pobj<::DX::D3DDeviceResources>	DeviceResources			;
	::the1_win32::Sample3DSceneRenderer		D3DScene				;
	::the1_win32::SampleFpsTextRenderer		D3DText					;

	// We need to render the GUI separately to compose DirectX target from a dynamic texture.
	::gpk::pcom<ID3D11ShaderResourceView>	GUISRV					;
	::gpk::pcom<ID3D11Texture2D>			GUITexture2D			;
	::gpk::pcom<ID3D11Buffer>				GUIVertexBuffer			;
	::gpk::pcom<ID3D11Buffer>				GUIIndexBuffer			;
	::gpk::pcom<ID3D11InputLayout	>		GUIInputLayout;
	::gpk::pcom<ID3D11VertexShader	>		GUIVertexShader;
	::gpk::pcom<ID3D11PixelShader	>		GUIPixelShader;
	::gpk::pcom<ID3D11Buffer		>		GUIConstantBuffer;
	::gpk::pcom<ID3D11SamplerState	>		GUISamplerStates;
	::gpk::pcom<ID3D11ShaderResourceView>	GUIShaderResourceView;
	::gpk::pcom<ID3D11Buffer		>		GUIConstantBufferNode, GUIConstantBufferScene;


	::gpk::apod<uint32_t>					D3DBufferMap			;

	// Notifies renderers that device resources need to be released.
	virtual void								OnDeviceLost					() {
		D3DScene.ReleaseDeviceDependentResources();
		D3DText.ReleaseDeviceDependentResources();

		GUISRV										= {};
		GUITexture2D								= {};
	}

	// Notifies renderers that device resources may now be recreated.
	virtual ::gpk::error_t						OnDeviceRestored				() {
		gpk_necs(D3DScene.CreateDeviceDependentResources());
		gpk_necs(D3DText.CreateDeviceDependentResources());
		gpk_necs(D3DScene.CreateWindowSizeDependentResources());

		gpk_necs(CreateDeviceDependentResources());
		return 0;
	}

	::gpk::error_t								CreateWindowSizeDependentResources	()	{
		D3DScene.CreateWindowSizeDependentResources();
		TheOne.UIRenderTarget.resize(Framework.RootWindow.Size, {0, 0, 0, 0}, 0xFFFFFFFFU);
		gpk_necs(::gpk::d3dCreateTextureDynamic(DeviceResources->GetD3DDevice(), GUITexture2D, GUISRV, TheOne.UIRenderTarget.Color));
		// 3515504948
		{
			// Create a sampler state
			D3D11_SAMPLER_DESC								samDesc				= {};
			samDesc.Filter								= D3D11_FILTER_MIN_MAG_MIP_POINT;
			samDesc.AddressU							= D3D11_TEXTURE_ADDRESS_CLAMP;
			samDesc.AddressV							= D3D11_TEXTURE_ADDRESS_CLAMP;
			samDesc.AddressW							= D3D11_TEXTURE_ADDRESS_CLAMP;
			samDesc.MaxAnisotropy						= 1;
			samDesc.ComparisonFunc						= D3D11_COMPARISON_ALWAYS;
			samDesc.MaxLOD								= D3D11_FLOAT32_MAX;

			::gpk::pcom<ID3D11SamplerState>					samplerState;
			gpk_hrcall(DeviceResources->GetD3DDevice()->CreateSamplerState(&samDesc, &samplerState));
			GUISamplerStates							= samplerState;
		}

		{
#pragma pack(push, 1)
			struct SPosUV  {
				::gpk::n3d<float> Position;
				::gpk::n2d<float> UV;
			};
#pragma pack(pop)
			constexpr SPosUV								vertices[4]					= {{{-1, 1}, {0, 0}},{{1, 1}, {1, 0}},{{-1, -1}, {0, 1}}, {{1, -1}, {1, 1}}};

			D3D11_SUBRESOURCE_DATA							vertexBufferData			= {vertices};
			D3D11_BUFFER_DESC								vertexBufferDesc			= {sizeof(SPosUV) * 4};
			vertexBufferDesc.BindFlags					= D3D11_BIND_VERTEX_BUFFER;
			::gpk::ptr_com<ID3D11Buffer>					d3dBuffer;
			gpk_hrcall(DeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d3dBuffer));
			GUIVertexBuffer								= d3dBuffer;
		}
		{
			constexpr uint16_t								indices[]					= {0, 1, 2, 1, 3, 2};
			D3D11_SUBRESOURCE_DATA							vertexBufferData			= {indices};
			D3D11_BUFFER_DESC								vertexBufferDesc			= {sizeof(uint16_t) * 6};
			vertexBufferDesc.BindFlags					= D3D11_BIND_INDEX_BUFFER;
			::gpk::ptr_com<ID3D11Buffer>					d3dBuffer;
			gpk_hrcall(DeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d3dBuffer));
			GUIIndexBuffer								= d3dBuffer;
		}
		return 0;
	}
	::gpk::error_t								CreateDeviceDependentResources		()	{
		::gpk::SEngineGraphics									& engineGraphics				= *TheOne.MainGame.Game.Engine.Scene->Graphics;
		gpk_necs(::gpk::d3dCreateBuffersFromEngineMeshes		(DeviceResources->GetD3DDevice(), engineGraphics.Meshes, engineGraphics.Buffers, D3DScene.IndexBuffer, D3DScene.VertexBuffer));
		gpk_necs(::gpk::d3dCreateTexturesFromEngineSurfaces		(DeviceResources->GetD3DDevice(), engineGraphics.Surfaces, D3DScene.ShaderResourceView));
		gpk_necs(::gpk::d3dCreatePixelShadersFromEngineShaders	(DeviceResources->GetD3DDevice(), engineGraphics.Shaders, D3DScene.PixelShader));
		{		
			::gpk::ptr_com<ID3D11Buffer>					constantBuffer;
			D3D11_BUFFER_DESC								constantBufferDesc		= {sizeof(::gpk::SRenderNodeConstants)};
			constantBufferDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
			gpk_hrcall(DeviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
			GUIConstantBufferNode = constantBuffer;
		}
		{
			::gpk::ptr_com<ID3D11Buffer>					constantBuffer;
			D3D11_BUFFER_DESC								constantBufferDesc		= {sizeof(::gpk::SEngineSceneConstants)};
			constantBufferDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
			gpk_hrcall(DeviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
			GUIConstantBufferScene = constantBuffer;
		}
		{
			// Load shaders asynchronously.
			::gpk::pcom<ID3D11InputLayout>					inputLayout	;
			::gpk::pcom<ID3D11VertexShader>					vertexShader;

			::gpk::vcs										shaderName					= "vsGUI";
			char											shaderFileName	[1024]		= {};
			static constexpr D3D11_INPUT_ELEMENT_DESC		vertexDesc []				=
				{	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 00, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				,	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};
			sprintf_s(shaderFileName, "%s.cso", shaderName.begin()) ;
			::gpk::apod<byte_t>								fileVS;
			gpk_necs(::gpk::fileToMemory(::gpk::vcs{shaderFileName}, fileVS));
			// After the vertex shader file is loaded, create the shader and input layout.
			gpk_hrcall(DeviceResources->GetD3DDevice()->CreateVertexShader(&fileVS[0], fileVS.size(), nullptr, &vertexShader));
			gpk_hrcall(DeviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileVS[0], fileVS.size(), &inputLayout)); 
			GUIInputLayout								= inputLayout;
			GUIVertexShader								= vertexShader;
		}
		{
			::gpk::vcs										shaderName					= "psGUI";
			char											shaderFileName	[1024]		= {};
			::gpk::pcom<ID3D11PixelShader>					pixelShader;
			sprintf_s(shaderFileName, "%s.cso", shaderName.begin());
			::gpk::apod<byte_t>								filePS;
			gpk_necs(::gpk::fileToMemory(::gpk::vcs{shaderFileName}, filePS));

			gpk_hrcall(DeviceResources->GetD3DDevice()->CreatePixelShader(&filePS[0], filePS.size(), nullptr, &pixelShader));
			GUIPixelShader								= pixelShader;
		}

		return 0; 
	}

												SApplication					(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423

#endif // GPK_MESH_H_324234234
