#include "gpk_d3d.h"

#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <agile.h>

#include "DirectXHelper.h"

#ifndef DEVICERESOURCES_H_2387647892
#define DEVICERESOURCES_H_2387647892

namespace DX
{
	namespace DisplayMetrics
	{
		static constexpr	bool						SupportHighResolutions					= false;	// High resolution displays can require a lot of GPU and battery power to render. High resolution phones, for example, may suffer from poor battery life if games attempt to render at 60 frames per second at full fidelity. The decision to render at full fidelity across all platforms and form factors should be deliberate.
		static constexpr	float						DpiThreshold							= 192.0f;		// 200% of standard desktop display.
		static constexpr	float						WidthThreshold							= 1920.0f;	// 1080p width.
		static constexpr	float						HeightThreshold							= 1080.0f;	// 1080p height.
		// The default thresholds that define a "high resolution" display. If the thresholds are exceeded and SupportHighResolutions is false, the dimensions will be scaled by 50%.
	};

	// Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
	interface IDeviceNotify {
		virtual void									OnDeviceLost							() = 0;
		virtual void									OnDeviceRestored						() = 0;
	};

	// Controls all the DirectX device resources.
	class DeviceResources {
		IDeviceNotify									* m_deviceNotify						= {};	// The IDeviceNotify can be held directly as it owns the DeviceResources.

		// Direct3D objects.
		Microsoft::WRL::ComPtr<ID3D11Device3>			m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext3>	m_d3dContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>			m_swapChain;

		// Direct3D rendering objects. Required for 3D.
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView1>	m_d3dRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_d3dDepthStencilView;
		D3D11_VIEWPORT									m_screenViewport						= {};

		// Direct2D drawing components.
		Microsoft::WRL::ComPtr<ID2D1Factory3>			m_d2dFactory;
		Microsoft::WRL::ComPtr<ID2D1Device2>			m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext2>		m_d2dContext;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1>			m_d2dTargetBitmap;

		// DirectWrite drawing components.
		Microsoft::WRL::ComPtr<IDWriteFactory3>			m_dwriteFactory;
		Microsoft::WRL::ComPtr<IWICImagingFactory2>		m_wicFactory;

		Platform::Agile<Windows::UI::Core::CoreWindow>	m_window;	// Cached reference to the Window.

		// Cached device properties.
		D3D_FEATURE_LEVEL								m_d3dFeatureLevel						= D3D_FEATURE_LEVEL_9_1;
		Windows::Foundation::Size						m_d3dRenderTargetSize					= {};
		Windows::Foundation::Size						m_outputSize							= {};
		Windows::Foundation::Size						m_logicalSize							= {};
		Windows::Graphics::Display::DisplayOrientations	m_nativeOrientation						= {};
		Windows::Graphics::Display::DisplayOrientations	m_currentOrientation					= {};
		float											m_dpi									= -1.0f;
		float											m_effectiveDpi							= -1.0f;	// This is the DPI that will be reported back to the app. It takes into account whether the app supports high resolution screens or not.

		// Transforms used for display orientation.
		D2D1::Matrix3x2F								m_orientationTransform2D				= {};
		DirectX::XMFLOAT4X4								m_orientationTransform3D				= {};

		void											CreateDeviceIndependentResources		();
		void											CreateDeviceResources					();
		void											CreateWindowSizeDependentResources		();
		DXGI_MODE_ROTATION								ComputeDisplayRotation					();

		// Determine the dimensions of the render target and whether it will be scaled down.
		void											UpdateRenderTargetSize					() {
			m_effectiveDpi									= m_dpi;
			if (!DisplayMetrics::SupportHighResolutions && m_dpi > DisplayMetrics::DpiThreshold) {	// To improve battery life on high resolution devices, render to a smaller render target and allow the GPU to scale the output when it is presented.
				float												width									= DX::ConvertDipsToPixels(m_logicalSize.Width, m_dpi);
				float												height									= DX::ConvertDipsToPixels(m_logicalSize.Height, m_dpi);
				if (std::max(width, height) > DisplayMetrics::WidthThreshold && std::min(width, height) > DisplayMetrics::HeightThreshold)	// When the device is in portrait orientation, height > width. Compare the larger dimension against the width threshold and the smaller dimension against the height threshold.
					m_effectiveDpi									/= 2.0f;	// To scale the app we change the effective DPI. Logical size does not change.
			}
			// Calculate the necessary render target size in pixels.
			m_outputSize.Width								= std::max(1.0f, DX::ConvertDipsToPixels(m_logicalSize.Width, m_effectiveDpi));
			m_outputSize.Height								= std::max(1.0f, DX::ConvertDipsToPixels(m_logicalSize.Height, m_effectiveDpi));
		}

	public:
														DeviceResources							()									{
			CreateDeviceIndependentResources();
			CreateDeviceResources();
		}


		Windows::Foundation::Size						GetOutputSize							()	const									{ return m_outputSize; }	// The size of the render target, in pixels.
		Windows::Foundation::Size						GetLogicalSize							()	const									{ return m_logicalSize; }	// The size of the render target, in dips.
		float											GetDpi									()	const									{ return m_effectiveDpi; }

		// D3D Accessors.
		ID3D11Device3*									GetD3DDevice							()	const									{ return m_d3dDevice.Get(); }
		ID3D11DeviceContext3*							GetD3DDeviceContext						()	const									{ return m_d3dContext.Get(); }
		IDXGISwapChain3*								GetSwapChain							()	const									{ return m_swapChain.Get(); }
		D3D_FEATURE_LEVEL								GetDeviceFeatureLevel					()	const									{ return m_d3dFeatureLevel; }
		ID3D11RenderTargetView1*						GetBackBufferRenderTargetView			()	const									{ return m_d3dRenderTargetView.Get(); }
		ID3D11DepthStencilView*							GetDepthStencilView						()	const									{ return m_d3dDepthStencilView.Get(); }
		D3D11_VIEWPORT									GetScreenViewport						()	const									{ return m_screenViewport; }
		DirectX::XMFLOAT4X4								GetOrientationTransform3D				()	const									{ return m_orientationTransform3D; }

		// D2D Accessors.
		ID2D1Factory3*									GetD2DFactory							()	const									{ return m_d2dFactory.Get(); }
		ID2D1Device2*									GetD2DDevice							()	const									{ return m_d2dDevice.Get(); }
		ID2D1DeviceContext2*							GetD2DDeviceContext						()	const									{ return m_d2dContext.Get(); }
		ID2D1Bitmap1*									GetD2DTargetBitmap						()	const									{ return m_d2dTargetBitmap.Get(); }
		IDWriteFactory3*								GetDWriteFactory						()	const									{ return m_dwriteFactory.Get(); }
		IWICImagingFactory2*							GetWicImagingFactory					()	const									{ return m_wicFactory.Get(); }
		D2D1::Matrix3x2F								GetOrientationTransform2D				()	const									{ return m_orientationTransform2D; }

		void											RegisterDeviceNotify					(DX::IDeviceNotify* deviceNotify)										{ m_deviceNotify = deviceNotify; }
		void											SetLogicalSize							(Windows::Foundation::Size logicalSize)									{ if (m_logicalSize			== logicalSize			) return; m_logicalSize			= logicalSize;			CreateWindowSizeDependentResources(); } 
		void											SetCurrentOrientation					(Windows::Graphics::Display::DisplayOrientations currentOrientation)	{ if (m_currentOrientation	== currentOrientation	) return; m_currentOrientation	= currentOrientation;	CreateWindowSizeDependentResources(); } 
		void											SetDpi									(float dpi)																{
			if (dpi != m_dpi) {
				m_dpi											= dpi;
				m_logicalSize									= Windows::Foundation::Size(m_window->Bounds.Width, m_window->Bounds.Height);	// When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
				m_d2dContext->SetDpi(m_dpi, m_dpi);
				CreateWindowSizeDependentResources();
			}
		}

		// Call this method when the app suspends. It provides a hint to the driver that the app is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
		void											Trim									() {
			Microsoft::WRL::ComPtr<IDXGIDevice3>				dxgiDevice;
			m_d3dDevice.As(&dxgiDevice);
			dxgiDevice->Trim();
		}

		void											SetWindow								(Windows::UI::Core::CoreWindow^ window)						{
			Windows::Graphics::Display::DisplayInformation		^ currentDisplayInformation				= Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
			m_window										= window;
			m_logicalSize									= Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);
			m_nativeOrientation								= currentDisplayInformation->NativeOrientation;
			m_currentOrientation							= currentDisplayInformation->CurrentOrientation;
			m_dpi											= currentDisplayInformation->LogicalDpi;
			m_d2dContext->SetDpi(m_dpi, m_dpi);
			CreateWindowSizeDependentResources();
		}

		void ValidateDevice();
		// Recreate all device resources and set them back to the current state.
		void											HandleDeviceLost						()											{
			m_swapChain										= nullptr;
			if (m_deviceNotify) 
				m_deviceNotify->OnDeviceLost();
			CreateDeviceResources();
			m_d2dContext->SetDpi(m_dpi, m_dpi);
			CreateWindowSizeDependentResources();
			if (m_deviceNotify) 
				m_deviceNotify->OnDeviceRestored();
		}

		// Present the contents of the swap chain to the screen.
		void											Present									() {
			DXGI_PRESENT_PARAMETERS								parameters								= {};
			HRESULT												hr										= m_swapChain->Present1(1, 0, &parameters);	// The first argument instructs DXGI to block until VSync, putting the application to sleep until the next VSync. This ensures we don't waste any cycles rendering frames that will never be displayed to the screen.
			m_d3dContext->DiscardView1(m_d3dRenderTargetView.Get(), nullptr, 0);	// Discard the contents of the render target. This is a valid operation only when the existing contents will be entirely overwritten. If dirty or scroll rects are used, this call should be removed.
			m_d3dContext->DiscardView1(m_d3dDepthStencilView.Get(), nullptr, 0);	// Discard the contents of the depth stencil.
			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
				HandleDeviceLost();	// If the device was removed either by a disconnection or a driver upgrade, we  must recreate all device resources.
			else
				DX::ThrowIfFailed(hr);
		}
	};
}

#endif // DEVICERESOURCES_H_2387647892