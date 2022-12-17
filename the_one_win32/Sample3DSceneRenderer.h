#include "DeviceResources.h"

#include "gpk_ptr.h"

#ifndef SAMPLE3DSCENERENDERER_H_293468238
#define SAMPLE3DSCENERENDERER_H_293468238

namespace the_one_uwp
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
		::gpk::ptr_obj<DX::DeviceResources>			DeviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer			m_constantBufferData					= {};
		uint32_t									m_indexCount							= {};

		// Variables used with the rendering loop.
		float										m_degreesPerSecond						= 45;
		bool										m_loadingComplete						= false;
		bool										m_tracking								= false;
		
		::gpk::error_t								Initialize								(const ::gpk::ptr_obj<DX::DeviceResources> & deviceResources)  {
			DeviceResources								= deviceResources;
			CreateDeviceDependentResources();
			CreateWindowSizeDependentResources();
			return 0;
		}

		// Rotate the 3D cube model a set amount of radians.
		void										Rotate									(float radians)		{ XMStoreFloat4x4(&m_constantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(radians))); }
		inline	constexpr	bool					IsTracking								()	const			{ return m_tracking; }
		inline				void					StartTracking							()					{ m_tracking = true; }
		inline				void					StopTracking							()					{ m_tracking = false; }
							void					TrackingUpdate							(float positionX)	{
			if (m_tracking) 
				Rotate(DirectX::XM_2PI * 2.0f * positionX / DeviceResources->GetOutputSize().x);
		}

		void										ReleaseDeviceDependentResources			() {
			m_loadingComplete = false;
			m_vertexShader	.Reset();
			m_inputLayout	.Reset();
			m_pixelShader	.Reset();
			m_constantBuffer.Reset();
			m_vertexBuffer	.Reset();
			m_indexBuffer	.Reset();
		}

		void										CreateDeviceDependentResources			();
		void										CreateWindowSizeDependentResources		();
		void										Update									(double /*secondsElapsed*/, double totalSecondsElapsed) {
			if (!m_tracking)  // Convert degrees to radians, then convert seconds to rotation angle
				Rotate((float)fmod(DirectX::XMConvertToRadians(m_degreesPerSecond) * totalSecondsElapsed, DirectX::XM_2PI));
		}
		void										Render									();

	};
}

#endif // SAMPLE3DSCENERENDERER_H_293468238