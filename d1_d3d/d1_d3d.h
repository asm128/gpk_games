#include "Sample3DSceneRenderer.h"
#include "SampleFpsTextRenderer.h"
#include "DeviceResources.h"

#include "gpk_engine_d3d.h"

#include "gpk_image.h"
#include "gpk_storage.h"

#ifndef THE_ONE_D3D_H_28903749823
#define THE_ONE_D3D_H_28903749823

namespace d1_win32
{
	template<typename _tColor>
	struct SD3DGUIStuff {
		typedef	::gpk::img<_tColor>				TRenderTarget;

		TRenderTarget							RenderTarget					= {};

		// We need to render the GUI separately to compose DirectX target from a dynamic texture.
		// Device-dependent
		::gpk::pcom<ID3D11Buffer>				VertexBuffer		;
		::gpk::pcom<ID3D11Buffer>				IndexBuffer			;
		::gpk::pcom<ID3D11SamplerState	>		SamplerStates		;
		// -- 
		::gpk::pcom<ID3D11ShaderResourceView>	SRV					;
		::gpk::pcom<ID3D11Texture2D>			Texture2D			;
		::gpk::pcom<ID3D11InputLayout	>		InputLayout			;
		::gpk::pcom<ID3D11VertexShader	>		VertexShader		;
		::gpk::pcom<ID3D11PixelShader	>		PixelShader			;
		::gpk::pcom<ID3D11Buffer		>		ConstantBuffer		;
		::gpk::pcom<ID3D11ShaderResourceView>	ShaderResourceView	;
		::gpk::pcom<ID3D11Buffer		>		ConstantBufferNode, ConstantBufferScene;

		void									ReleaseDeviceResources	() {
			SRV										= {};
			Texture2D								= {};
		}

		::gpk::error_t							CreateSizeDependentResources	(ID3D11Device3 * d3dDevice, ::gpk::n2<uint16_t> windowSize)	{
			gpk_necs(RenderTarget.resize(windowSize.Cast<uint32_t>(), {0, 0, 0, 0}));
			gpk_necs(::gpk::d3dCreateTextureDynamic(d3dDevice, Texture2D, SRV, RenderTarget));
			return 0;
		}

		::gpk::error_t							CreateDeviceResources		(ID3D11Device3 * d3dDevice)	{
			{		
				::gpk::ptr_com<ID3D11Buffer>				constantBuffer;
				D3D11_BUFFER_DESC							constantBufferDesc					= {sizeof(::gpk::SRenderNodeConstants)};
				constantBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
				gpk_hrcall(d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
				ConstantBufferNode						= constantBuffer;
			}
			{
				::gpk::ptr_com<ID3D11Buffer>				constantBuffer;
				D3D11_BUFFER_DESC							constantBufferDesc					= {sizeof(::gpk::SEngineSceneConstants)};
				constantBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
				gpk_hrcall(d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
				ConstantBufferScene						= constantBuffer;
			}
			{
				// Load shaders asynchronously.
				::gpk::pcom<ID3D11InputLayout>				inputLayout	;
				::gpk::pcom<ID3D11VertexShader>				vertexShader;

				::gpk::vcs									shaderName							= "vsGUI";
				char										shaderFileName	[1024]				= {};
				constexpr	D3D11_INPUT_ELEMENT_DESC		vertexDesc []						=
					{	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 00, D3D11_INPUT_PER_VERTEX_DATA, 0 }
					,	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
					};
				sprintf_s(shaderFileName, "%s.cso", shaderName.begin()) ;
				::gpk::apod<int8_t>							fileVS;
				gpk_necs(::gpk::fileToMemory(::gpk::vcs{shaderFileName}, fileVS));
				// After the vertex shader file is loaded, create the shader and input layout.
				gpk_hrcall(d3dDevice->CreateVertexShader(&fileVS[0], fileVS.size(), nullptr, &vertexShader));
				gpk_hrcall(d3dDevice->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileVS[0], fileVS.size(), &inputLayout)); 
				InputLayout								= inputLayout;
				VertexShader							= vertexShader;
			}
			{
				::gpk::vcs									shaderName							= "psGUI";
				char										shaderFileName	[1024]				= {};
				::gpk::pcom<ID3D11PixelShader>				pixelShader;
				sprintf_s(shaderFileName, "%s.cso", shaderName.begin());
				::gpk::apod<int8_t>							filePS;
				gpk_necs(::gpk::fileToMemory(::gpk::vcs{shaderFileName}, filePS));

				gpk_hrcall(d3dDevice->CreatePixelShader(&filePS[0], filePS.size(), nullptr, &pixelShader));
				PixelShader								= pixelShader;
			}

			{
	#pragma pack(push, 1)
				struct SPosUV  {
					::gpk::n3<float> Position;
					::gpk::n2<float> UV;
				};
	#pragma pack(pop)
				constexpr SPosUV							vertices[4]						= {{{-1, 1}, {0, 0}},{{1, 1}, {1, 0}},{{-1, -1}, {0, 1}}, {{1, -1}, {1, 1}}};

				D3D11_SUBRESOURCE_DATA						vertexBufferData				= {vertices};
				D3D11_BUFFER_DESC							vertexBufferDesc				= {sizeof(SPosUV) * 4};
				vertexBufferDesc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
				::gpk::pcom<ID3D11Buffer>					d3dBuffer;
				gpk_hrcall(d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d3dBuffer));
				VertexBuffer							= d3dBuffer;
			}
			{
				constexpr uint16_t							indices[]						= {0, 1, 2, 1, 3, 2};
				D3D11_SUBRESOURCE_DATA						vertexBufferData				= {indices};
				D3D11_BUFFER_DESC							vertexBufferDesc				= {sizeof(uint16_t) * 6};
				vertexBufferDesc.BindFlags				= D3D11_BIND_INDEX_BUFFER;
				::gpk::pcom<ID3D11Buffer>					d3dBuffer;
				gpk_hrcall(d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d3dBuffer));
				IndexBuffer								= d3dBuffer;
			}
			// 3515504948
			{
				// Create a sampler state
				D3D11_SAMPLER_DESC							samDesc							= {};
				samDesc.Filter							= D3D11_FILTER_MIN_MAG_MIP_POINT;
				samDesc.AddressU						= D3D11_TEXTURE_ADDRESS_CLAMP;
				samDesc.AddressV						= D3D11_TEXTURE_ADDRESS_CLAMP;
				samDesc.AddressW						= D3D11_TEXTURE_ADDRESS_CLAMP;
				samDesc.MaxAnisotropy					= 1;
				samDesc.ComparisonFunc					= D3D11_COMPARISON_ALWAYS;
				samDesc.MaxLOD							= D3D11_FLOAT32_MAX;

				::gpk::pcom<ID3D11SamplerState>				samplerState;
				gpk_hrcall(d3dDevice->CreateSamplerState(&samDesc, &samplerState));
				SamplerStates							= samplerState;
			}

			return 0;
		}

	};

	::gpk::error_t							d3dGUIDraw							(::DX::D3DDeviceResources & d3dResources, ::d1_win32::SD3DGUIStuff<::gpk::bgra> & guiStuff);

	struct SD3DApplication : DX::IDeviceNotify {
		::gpk::pobj<::gpk::SEngineGraphics>		EngineGraphics;

		::gpk::pobj<::DX::D3DDeviceResources>	DeviceResources			;
		::d1_win32::Sample3DSceneRenderer		Scene					;
		::d1_win32::SampleFpsTextRenderer		Text					;
		::d1_win32::SD3DGUIStuff<::gpk::bgra>	GUIStuff				;
	
		virtual void							OnDeviceLost			() { return ReleaseDeviceResources(); }
		virtual ::gpk::error_t					OnDeviceRestored		() { return CreateDeviceResources(*EngineGraphics); }

		::gpk::error_t							Shutdown								()	{
			DeviceResources->m_swapChain->SetFullscreenState(FALSE, 0);
			ReleaseDeviceResources();
			Scene			= {};
			Text			= {};
			GUIStuff		= {};
			DeviceResources	= {};
			return 0;
		}
		::gpk::error_t							Initialize								(HWND hWnd, const ::gpk::pobj<::gpk::SEngineGraphics> & engineGraphics)	{
			EngineGraphics							= engineGraphics;

			gpk_necs(DeviceResources->Initialize());
			gpk_necs(DeviceResources->SetWindow(hWnd));
			DeviceResources->RegisterDeviceNotify(this);
			gpk_necs(Scene.Initialize(DeviceResources));
			gpk_necs(Text .Initialize(DeviceResources));
			return 0;
		}

		// Notifies renderers that device resources need to be released.
		void									ReleaseDeviceResources					()	{
			Scene		.ReleaseDeviceResources();
			Text		.ReleaseDeviceResources();
			GUIStuff	.ReleaseDeviceResources();
		}
		// Notifies renderers that device resources may now be recreated.

		::gpk::error_t							CreateDeviceDependentEngineResources	(ID3D11Device3 * d3dDevice,  const ::gpk::SEngineGraphics & engineGraphics)	{
			gpk_necs(::gpk::d3dCreateBuffersFromEngineMeshes		(d3dDevice, engineGraphics.Meshes	, engineGraphics.Buffers, Scene.IndexBuffer, Scene.VertexBuffer));
			gpk_necs(::gpk::d3dCreateTexturesFromEngineSurfaces		(d3dDevice, engineGraphics.Surfaces	, Scene.ShaderResourceView));
			gpk_necs(::gpk::d3dCreatePixelShadersFromEngineShaders	(d3dDevice, engineGraphics.Shaders	, Scene.PixelShader));
			return 0;
		}
		::gpk::error_t							CreateDeviceResources					(const ::gpk::SEngineGraphics & engineGraphics)	{
			gpk_necs(Scene		.CreateDeviceResources(DeviceResources->GetD3DDevice()));
			gpk_necs(Text		.CreateDeviceResources(DeviceResources->GetD2DDeviceContext()));
			gpk_necs(GUIStuff	.CreateDeviceResources(DeviceResources->GetD3DDevice()));
			gpk_necs(CreateDeviceDependentEngineResources(DeviceResources->GetD3DDevice(), engineGraphics));
			return 0; 
		}

		::gpk::error_t							CreateSizeDependentResources			(::gpk::n2u16 windowSize)	{
			gpk_necs(Scene		.CreateSizeDependentResources(DeviceResources->GetOutputSize(), DeviceResources->GetOrientationTransform3D()));
			gpk_necs(GUIStuff	.CreateSizeDependentResources(DeviceResources->GetD3DDevice(), windowSize));
			return 0;
		}

		::gpk::error_t							SetWindowSize							(::gpk::n2u16 windowSize)	{
			gpk_necs(DeviceResources->SetLogicalSize(windowSize.Cast<float>()));
			gpk_necs(CreateSizeDependentResources(windowSize));
			return 0;
		}
	};

	::gpk::error_t							d3dDrawEngineScene		(::d1_win32::Sample3DSceneRenderer & d3dScene, const ::gpk::SEngineScene & engineScene, const ::gpk::n2u16 & targetMetrics, const ::gpk::n3f & lightPos, const ::gpk::n3f & cameraPosition, const gpk::n3f & cameraTarget);
	::gpk::error_t							d3dAppDraw				(::d1_win32::SD3DApplication & d3dApp, const ::gpk::SEngineScene & engineScene, const ::gpk::rgbaf & clearColor, const ::gpk::n3f & lightPos, const ::gpk::n3f & cameraPosition, const gpk::n3f & cameraTarget);
};

#endif