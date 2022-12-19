#include "DeviceResources.h"

#include "gpk_ptr.h"

#include "gpk_storage.h"

#ifndef SAMPLE3DSCENERENDERER_H_293468238
#define SAMPLE3DSCENERENDERER_H_293468238

namespace the_one_win32
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer {
		DirectX::XMFLOAT4X4							model;
		DirectX::XMFLOAT4X4							view;
		DirectX::XMFLOAT4X4							projection;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor {
		DirectX::XMFLOAT3							pos;
		DirectX::XMFLOAT3							color;
	};

	// This sample renderer instantiates a basic rendering pipeline.
	struct Sample3DSceneRenderer {
		// Cached pointer to device resources.
		::gpk::ptr_obj<DX::D3DDeviceResources>		DeviceResources;

		// Direct3D resources for cube geometry
		::gpk::array_com<ID3D11InputLayout	>		InputLayout;
		::gpk::array_com<ID3D11Buffer		>		VertexBuffer;
		::gpk::array_com<ID3D11Buffer		>		IndexBuffer;
		::gpk::array_com<ID3D11VertexShader	>		VertexShader;
		::gpk::array_com<ID3D11PixelShader	>		PixelShader;
		::gpk::array_com<ID3D11Buffer		>		ConstantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer			m_constantBufferData					= {};
		uint32_t									m_indexCount							= {};

		// Variables used with the rendering loop.
		float										m_degreesPerSecond						= 45;
		bool										m_loadingComplete						= false;
		bool										m_tracking								= false;
		double										TotalSecondsElapsed						= 0;
		
													~Sample3DSceneRenderer					()		{ ReleaseDeviceDependentResources(); }

		::gpk::error_t								Initialize								(const ::gpk::ptr_obj<DX::D3DDeviceResources> & deviceResources)  {
			DeviceResources								= deviceResources;
			CreateDeviceDependentResources();
			CreateWindowSizeDependentResources();
			return 0;
		}

		// Rotate the 3D cube model a set amount of radians.
		void										Rotate									(float radians)		{ DirectX::XMStoreFloat4x4(&m_constantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(radians))); }
		inline	constexpr	bool					IsTracking								()	const			{ return m_tracking; }
		inline				void					StartTracking							()					{ m_tracking = true; }
		inline				void					StopTracking							()					{ m_tracking = false; }
							void					TrackingUpdate							(float positionX)	{
			if (m_tracking) 
				Rotate(DirectX::XM_2PI * 2.0f * positionX / DeviceResources->GetOutputSize().x);
		}

		void										ReleaseDeviceDependentResources			() {
			m_loadingComplete							= false;
			VertexShader	.clear();
			InputLayout		.clear();
			PixelShader		.clear();
			ConstantBuffer	.clear();
			VertexBuffer	.clear();
			IndexBuffer		.clear();
		}

		void										Update									(double secondsElapsed) {
			TotalSecondsElapsed							+= secondsElapsed;
			if (!m_tracking)  // Convert degrees to radians, then convert seconds to rotation angle
				Rotate((float)fmod(DirectX::XMConvertToRadians(m_degreesPerSecond) * TotalSecondsElapsed, DirectX::XM_2PI));
		}

		void										Render									() {
			if (!m_loadingComplete)	// Loading is asynchronous. Only draw geometry after it's loaded.
				return;

			auto											context					= DeviceResources->GetD3DDeviceContext();
			context->UpdateSubresource1(ConstantBuffer[0], 0, NULL, &m_constantBufferData, 0, 0, 0);	// Prepare the constant buffer to send it to the graphics device.

			// Each vertex is one instance of the VertexPositionColor struct.
			UINT											stride					= sizeof(VertexPositionColor);
			UINT											offset					= 0;

			context->IASetVertexBuffers		(0, 1, &VertexBuffer[0], &stride, &offset);
			context->IASetIndexBuffer		(IndexBuffer[0], DXGI_FORMAT_R16_UINT, 0);// Each index is one 16-bit unsigned integer (short).
			context->IASetPrimitiveTopology	(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->IASetInputLayout		(InputLayout[0]);
			context->VSSetShader			(VertexShader[0], nullptr, 0);	// Attach our vertex shader.
			context->VSSetConstantBuffers1	(0, 1, &ConstantBuffer[0], nullptr, nullptr);	// Send the constant buffer to the graphics device.
			context->PSSetShader			(PixelShader[0], nullptr, 0);	// Attach our pixel shader.
			context->DrawIndexed			(m_indexCount, 0, 0);	// Draw the objects.
		}

		void										CreateWindowSizeDependentResources		() {
			::gpk::SCoord2<float>							outputSize				= DeviceResources->GetOutputSize();
			const float										aspectRatio				= outputSize.x / outputSize.y;
			float											fovAngleY				= 70.0f * DirectX::XM_PI / 180.0f;

			// This is a simple example of change that can be made when the app is in portrait or snapped view.
			if (aspectRatio < 1.0f)
				fovAngleY									*= 2.0f;

			// Note that the OrientationTransform3D matrix is post-multiplied here in order to correctly orient the scene to match the display orientation.
			// This post-multiplication step is required for any draw calls that are made to the swap chain render target. For draw calls to other targets, this transform should not be applied.
			// This sample makes use of a right-handed coordinate system using row-major matrices.
			DirectX::XMMATRIX								perspectiveMatrix		= DirectX::XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, 0.01f, 100.0f);
			DirectX::XMFLOAT4X4								orientation				= DeviceResources->GetOrientationTransform3D();
			DirectX::XMMATRIX								orientationMatrix		= DirectX::XMLoadFloat4x4(&orientation);
			DirectX::XMStoreFloat4x4(&m_constantBufferData.projection, DirectX::XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

			// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
			static const DirectX::XMVECTORF32				eye						= { 0.0f, 0.7f, 1.5f, 0.0f };
			static const DirectX::XMVECTORF32				at						= { 0.0f, -0.1f, 0.0f, 0.0f };
			static const DirectX::XMVECTORF32				up						= { 0.0f, 1.0f, 0.0f, 0.0f };
			DirectX::XMStoreFloat4x4(&m_constantBufferData.view, DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(eye, at, up)));
		}

		void										CreateDeviceDependentResources			()	{
			// Load shaders asynchronously.
			::gpk::ptr_com<ID3D11InputLayout>				inputLayout;
			::gpk::ptr_com<ID3D11Buffer>					vertexBuffer;
			::gpk::ptr_com<ID3D11Buffer>					indexBuffer;
			::gpk::ptr_com<ID3D11VertexShader>				vertexShader;
			::gpk::ptr_com<ID3D11PixelShader>				pixelShader;
			::gpk::ptr_com<ID3D11Buffer>					constantBuffer;

			::gpk::array_pod<byte_t>						fileVS;
			::gpk::fileToMemory("SampleVertexShader.cso", fileVS);
			static constexpr D3D11_INPUT_ELEMENT_DESC		vertexDesc []			=
				{	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 00, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				,	{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};
			// After the vertex shader file is loaded, create the shader and input layout.
			DX::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateVertexShader(&fileVS[0], fileVS.size(), nullptr, &vertexShader));
			DX::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileVS[0], fileVS.size(), &inputLayout)); 

			// After the pixel shader file is loaded, create the shader and constant buffer.
			::gpk::array_pod<byte_t>						filePS;
			::gpk::fileToMemory("SamplePixelShader.cso" , filePS);
			DX::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreatePixelShader(&filePS[0], filePS.size(), nullptr, &pixelShader));

			D3D11_BUFFER_DESC								constantBufferDesc		= {sizeof(ModelViewProjectionConstantBuffer)};
			constantBufferDesc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
			DX::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
		
			static constexpr VertexPositionColor			cubeVertices[]			=	// Once both shaders are loaded, create the mesh. Load mesh vertices. Each vertex has a position and a color.
				{ {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}
				, {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
				, {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}
				, {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 1.0f}}
				, {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}
				, {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 1.0f}}
				, {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}
				, {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
				};

			D3D11_SUBRESOURCE_DATA							vertexBufferData		= {cubeVertices};
			D3D11_BUFFER_DESC								vertexBufferDesc		= {sizeof(cubeVertices)};
			vertexBufferDesc.BindFlags					= D3D11_BIND_VERTEX_BUFFER;
			DX::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer));

			// Load mesh indices. Each trio of indices represents a triangle to be rendered on the screen. For example: 0,2,1 means that the vertices with indexes 0, 2 and 1 from the vertex buffer compose the first triangle of this mesh.
			static constexpr uint16_t						cubeIndices []			= 
				{ 0,2,1, 1,2,3	// -x
				, 4,5,6, 5,7,6	// +x
				, 0,1,5, 0,5,4	// -y
				, 2,6,7, 2,7,3	// +y
				, 0,4,6, 0,6,2	// -z
				, 1,3,7, 1,7,5	// +z
				};

			m_indexCount								= ARRAYSIZE(cubeIndices);
			D3D11_SUBRESOURCE_DATA							indexBufferData			= {cubeIndices};
			D3D11_BUFFER_DESC								indexBufferDesc			= {sizeof(cubeIndices)};
			indexBufferDesc.BindFlags					= D3D11_BIND_INDEX_BUFFER;
			DX::ThrowIfFailed(DeviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer));
	
			InputLayout		.push_back(inputLayout);
			VertexBuffer	.push_back(vertexBuffer);
			IndexBuffer		.push_back(indexBuffer);
			VertexShader	.push_back(vertexShader);
			PixelShader		.push_back(pixelShader);
			ConstantBuffer	.push_back(constantBuffer);

			m_loadingComplete							= true;		// Once the cube is loaded, the object is ready to be rendered.
		}


	};
}

#endif // SAMPLE3DSCENERENDERER_H_293468238