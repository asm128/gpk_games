#include "DeviceResources.h"
#include "StepTimer.h"
#include <memory>

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
	class Sample3DSceneRenderer {
		// Cached pointer to device resources.
		std::shared_ptr<DX::D3DDeviceResources>		m_deviceResources;

		// Direct3D resources for cube geometry.
		::gpk::ptr_com<ID3D11InputLayout>	m_inputLayout;
		::gpk::ptr_com<ID3D11Buffer>		m_vertexBuffer;
		::gpk::ptr_com<ID3D11Buffer>		m_indexBuffer;
		::gpk::ptr_com<ID3D11VertexShader>	m_vertexShader;
		::gpk::ptr_com<ID3D11PixelShader>	m_pixelShader;
		::gpk::ptr_com<ID3D11Buffer>		m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer			m_constantBufferData					= {};
		uint32										m_indexCount							= {};

		// Variables used with the rendering loop.
		float										m_degreesPerSecond						= 45;
		bool										m_loadingComplete						= false;
		bool										m_tracking								= false;
		
		// Rotate the 3D cube model a set amount of radians.
		void										Rotate									(float radians)		{ XMStoreFloat4x4(&m_constantBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(radians))); }

	public:
													Sample3DSceneRenderer					(const std::shared_ptr<DX::D3DDeviceResources>& deviceResources) : m_deviceResources(deviceResources) {
			CreateDeviceDependentResources();
			CreateWindowSizeDependentResources();
		}

		inline	constexpr	bool					IsTracking								()	const			{ return m_tracking; }
		inline				void					StartTracking							()					{ m_tracking = true; }
		inline				void					StopTracking							()					{ m_tracking = false; }
							void					TrackingUpdate							(float positionX)	{
			if (m_tracking) 
				Rotate(DirectX::XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().x);
		}

		void										ReleaseDeviceDependentResources			() {
			m_loadingComplete	= false;
			m_vertexShader		= {};
			m_inputLayout		= {};
			m_pixelShader		= {};
			m_constantBuffer	= {};
			m_vertexBuffer		= {};
			m_indexBuffer		= {};
		}

		void										CreateDeviceDependentResources			();
		void										CreateWindowSizeDependentResources		();
		void										Update									(DX::StepTimer const & timer) {
			if (!m_tracking)  // Convert degrees to radians, then convert seconds to rotation angle
				Rotate((float)fmod(DirectX::XMConvertToRadians(m_degreesPerSecond) * timer.GetTotalSeconds(), DirectX::XM_2PI));
		}
		void										Render									();

	};
}

#endif // SAMPLE3DSCENERENDERER_H_293468238