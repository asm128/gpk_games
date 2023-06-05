#include "DeviceResources.h"
#include "gpk_engine_scene.h"

#include "gpk_file.h"

#ifndef SAMPLE3DSCENERENDERER_H_293468238
#define SAMPLE3DSCENERENDERER_H_293468238

namespace d1_win32
{

	// This sample renderer instantiates a basic rendering pipeline.
	struct Sample3DSceneRenderer {
		::gpk::pobj<DX::D3DDeviceResources>		DeviceResources;

		// Direct3D resources for cube geometry
		::gpk::acom<ID3D11Buffer		>		VertexBuffer;
		::gpk::acom<ID3D11Buffer		>		IndexBuffer;
		::gpk::acom<ID3D11InputLayout	>		InputLayout;
		::gpk::acom<ID3D11VertexShader	>		VertexShader;
		::gpk::acom<ID3D11PixelShader	>		PixelShader;
		::gpk::acom<ID3D11Buffer		>		ConstantBuffer;
		::gpk::acom<ID3D11SamplerState	>		SamplerStates;
		::gpk::acom<ID3D11ShaderResourceView>	ShaderResourceView;

		// System resources for cube geometry.
		::gpk::SRenderNodeConstants				ConstantBufferModel				= {};
		::gpk::SEngineSceneConstants			ConstantBufferScene				= {};

		// Variables used with the rendering loo
		double									TotalSecondsElapsed				= 0;
		
												~Sample3DSceneRenderer			() { ReleaseDeviceResources(); }

		::gpk::error_t							Initialize						(const ::gpk::pobj<DX::D3DDeviceResources> & deviceResources) {
			DeviceResources							= deviceResources;
			gpk_necs(CreateDeviceResources(DeviceResources->GetD3DDevice()));
			gpk_necs(CreateSizeDependentResources(DeviceResources->GetLogicalSize(), DeviceResources->GetOrientationTransform3D()));
			return 0;
		}

		void									ReleaseDeviceResources			() {
			VertexShader	.clear();
			InputLayout		.clear();
			PixelShader		.clear();
			ConstantBuffer	.clear();
			VertexBuffer	.clear();
			IndexBuffer		.clear();
		}

		void									Render							(uint32_t iMesh, ::gpk::SRange<uint32_t> indexRange, uint32_t iTexture, uint32_t iShader) {
			auto										context							= DeviceResources->GetD3DDeviceContext();
			context->UpdateSubresource1(ConstantBuffer[0], 0, NULL, &ConstantBufferModel, 0, 0, 0);	// Prepare the constant buffer to send it to the graphics device.
			context->UpdateSubresource1(ConstantBuffer[1], 0, NULL, &ConstantBufferScene, 0, 0, 0);	// Prepare the constant buffer to send it to the graphics device.

			// Each vertex is one instance of the VertexPositionColor struct.
			UINT										stride							= sizeof(::gpk::n3f32) * 2 + sizeof(::gpk::n2f32);
			UINT										offset							= 0;

			::gpk::pcom<ID3D11Buffer>					vb								= VertexBuffer[iMesh];
			::gpk::pcom<ID3D11Buffer>					ib								= IndexBuffer [iMesh];
			context->IASetVertexBuffers		(0, 1, &vb, &stride, &offset);
			context->IASetIndexBuffer		(ib, DXGI_FORMAT_R16_UINT, 0);// Each index is one 16-bit unsigned integer (short).
			context->IASetPrimitiveTopology	(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->IASetInputLayout		(InputLayout[0]);
			context->VSSetShader			(VertexShader[0], nullptr, 0);	// Attach our vertex shader.
			context->VSSetConstantBuffers1	(0, 1, &ConstantBuffer[0], nullptr, nullptr);	// Send the constant buffer to the graphics device.
			context->PSSetConstantBuffers1	(0, 1, &ConstantBuffer[0], nullptr, nullptr);	// Send the constant buffer to the graphics device.
			context->PSSetConstantBuffers1	(1, 1, &ConstantBuffer[1], nullptr, nullptr);	// Send the constant buffer to the graphics device.
			context->PSSetShader			(PixelShader[iShader], nullptr, 0);	// Attach our pixel shader.

			D3D11_BUFFER_DESC							desc							= {};
			ib->GetDesc(&desc);

			D3D11_RASTERIZER_DESC						rs								= {};
			rs.FillMode								= D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME; 
			rs.CullMode								= D3D11_CULL_BACK;
			rs.DepthClipEnable						= TRUE;

			::gpk::pcom<ID3D11RasterizerState>			prs;
			DeviceResources->GetD3DDevice()->CreateRasterizerState(&rs, &prs);
			context->RSSetState(prs);
		    context->PSSetSamplers( 0, 1, &SamplerStates[0] );
			context->PSSetShaderResources( 0, 1, &ShaderResourceView[iTexture] );
			context->DrawIndexed(indexRange.Count, indexRange.Offset, 0);	// Draw the objects.
		}

		::gpk::error_t							CreateSizeDependentResources	(const ::gpk::n2f32 & outputSize, const DirectX::XMFLOAT4X4 & orientation) {
			const float									aspectRatio						= outputSize.x / outputSize.y;
			float										fovAngleY						= 70.0f * DirectX::XM_PI / 180.0f;

			// This is a simple example of change that can be made when the app is in portrait or snapped view.
			if (aspectRatio < 1.0f)
				fovAngleY								*= 2.0f;

			// Note that the OrientationTransform3D matrix is post-multiplied here in order to correctly orient the scene to match the display orientation.
			// This post-multiplication step is required for any draw calls that are made to the swap chain render target. For draw calls to other targets, this transform should not be applied.
			// This sample makes use of a right-handed coordinate system using row-major matrices.
			DirectX::XMMATRIX							perspectiveMatrix				= DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 100.0f);
			DirectX::XMMATRIX							orientationMatrix				= DirectX::XMLoadFloat4x4(&orientation);
			DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&ConstantBufferScene.Perspective, DirectX::XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

			// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
			static const DirectX::XMVECTORF32			eye								= { 0.0f, (float)sin(TotalSecondsElapsed) * 5, 5.5f, 0.0f };
			static const DirectX::XMVECTORF32			at								= { 0.0f, -0.1f, 0.0f, 0.0f };
			static const DirectX::XMVECTORF32			up								= { 0.0f, 1.0f, 0.0f, 0.0f };
			DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&ConstantBufferScene.View, DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(eye, at, up)));
			return 0;
		}

		::gpk::error_t							CreateDeviceResources			(ID3D11Device3 * d3dDevice)	{
			SamplerStates		= {};
			InputLayout			= {};
			VertexShader		= {};
			ConstantBuffer		= {};
			//VertexBuffer		= {};
			//IndexBuffer			= {};
			//PixelShader			= {};
			//ShaderResourceView	= {};
			{
				// Create a sampler state
				D3D11_SAMPLER_DESC							samDesc						= {};
				samDesc.Filter							= D3D11_FILTER_MIN_MAG_MIP_POINT;
				samDesc.AddressU						= D3D11_TEXTURE_ADDRESS_CLAMP;
				samDesc.AddressV						= D3D11_TEXTURE_ADDRESS_CLAMP;
				samDesc.AddressW						= D3D11_TEXTURE_ADDRESS_CLAMP;
				samDesc.MaxAnisotropy					= 1;
				samDesc.ComparisonFunc					= D3D11_COMPARISON_ALWAYS;
				samDesc.MaxLOD							= D3D11_FLOAT32_MAX;

				::gpk::pcom<ID3D11SamplerState>				samplerState;
				gpk_hrcall(d3dDevice->CreateSamplerState(&samDesc, &samplerState));
				SamplerStates.push_back(samplerState);
			}
			{
				// Load shaders asynchronously.
				::gpk::pcom<ID3D11InputLayout>				inputLayout;
				::gpk::pcom<ID3D11VertexShader>				vertexShader;

				::gpk::apod<int8_t>							fileVS;
				gpk_necs(::gpk::fileToMemory("SampleVertexShader.cso", fileVS));
				stacxpr	D3D11_INPUT_ELEMENT_DESC			vertexDesc []			=
					{	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 00, D3D11_INPUT_PER_VERTEX_DATA, 0 }
					,	{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
					,	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
					};
				// After the vertex shader file is loaded, create the shader and input layout.
				gpk_hrcall(d3dDevice->CreateVertexShader(&fileVS[0], fileVS.size(), nullptr, &vertexShader));
				gpk_hrcall(d3dDevice->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileVS[0], fileVS.size(), &inputLayout)); 
				InputLayout		.push_back(inputLayout);
				VertexShader	.push_back(vertexShader);
			}
			{
				::gpk::pcom<ID3D11Buffer>					constantBuffer;
				D3D11_BUFFER_DESC							constantBufferDesc		= {sizeof(::gpk::SRenderNodeConstants)};
				constantBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
				gpk_hrcall(d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
				ConstantBuffer.push_back(constantBuffer);
			}
			{
				::gpk::pcom<ID3D11Buffer>					constantBuffer;
				D3D11_BUFFER_DESC							constantBufferDesc		= {sizeof(::gpk::SEngineSceneConstants)};
				constantBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
				gpk_hrcall(d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
				ConstantBuffer.push_back(constantBuffer);
			}
			return 0;
		}
	};
}

#endif // SAMPLE3DSCENERENDERER_H_293468238