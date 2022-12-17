#include "gpk_d3d.h"
#include "gpk_view_grid.h"

#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>

#include <ppltasks.h>

#ifndef DEVICERESOURCES_H_2387647892
#define DEVICERESOURCES_H_2387647892

namespace DX 
{
	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
	static inline float								ConvertDipsToPixels		(float dips, float dpi) {
		static constexpr float								dipsPerInch				= 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
	}
#if defined(_DEBUG)
	static inline bool								SdkLayersAvailable		() {		// Check for SDK Layer support.
		return SUCCEEDED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_NULL, 0, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, nullptr, nullptr, nullptr));
	}
#endif
	static inline void								ThrowIfFailed			(HRESULT hr) {
		if (FAILED(hr))
			throw("");	// Set a breakpoint on this line to catch Win32 API errors.
	}

	namespace DisplayMetrics
	{
		static constexpr	bool						SupportHighResolutions					= false;	// High resolution displays can require a lot of GPU and battery power to render. High resolution phones, for example, may suffer from poor battery life if games attempt to render at 60 frames per second at full fidelity. The decision to render at full fidelity across all platforms and form factors should be deliberate.
		static constexpr	float						DpiThreshold							= 192.0f;	// 200% of standard desktop display.
		static constexpr	float						WidthThreshold							= 1920.f;	// 1080p width.
		static constexpr	float						HeightThreshold							= 1080.f;	// 1080p height.
		// The default thresholds that define a "high resolution" display. If the thresholds are exceeded and SupportHighResolutions is false, the dimensions will be scaled by 50%.
	};

	// Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
	struct IDeviceNotify {
		virtual void									OnDeviceLost							() = 0;
		virtual void									OnDeviceRestored						() = 0;
	};

	// Controls all the DirectX device resources.
	struct D3DDeviceResources {
		HWND											m_window								= 0;	// Cached reference to the Window.
		DEVMODE											DeviceMode								= {sizeof(DEVMODE)};

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

		// Cached device properties.
		D3D_FEATURE_LEVEL								m_d3dFeatureLevel						= D3D_FEATURE_LEVEL_9_1;
		::gpk::SCoord2<float>							m_d3dRenderTargetSize					= {};
		::gpk::SCoord2<float>							m_outputSize							= {};
		::gpk::SCoord2<float>							m_logicalSize							= {};
		::gpk::GRID_ROTATION							m_nativeOrientation						= ::gpk::GRID_ROTATION_0;
		::gpk::GRID_ROTATION							m_currentOrientation					= ::gpk::GRID_ROTATION_0;
		float											m_dpi									= 96.0f;
		float											m_effectiveDpi							= 96.0f;	// This is the DPI that will be reported back to the app. It takes into account whether the app supports high resolution screens or not.

		// Transforms used for display orientation.
		D2D1::Matrix3x2F								m_orientationTransform2D				= {};
		DirectX::XMFLOAT4X4								m_orientationTransform3D				= {};

														D3DDeviceResources							()									{
			CreateDeviceIndependentResources();
			CreateDeviceResources();
		}

		::gpk::error_t									CreateDeviceResources					();
		::gpk::error_t 									CreateWindowSizeDependentResources		();
		// This method determines the rotation between the display device's native orientation and the current display orientation.
		DXGI_MODE_ROTATION								ComputeDisplayRotation					() {
			DXGI_MODE_ROTATION									rotation								= DXGI_MODE_ROTATION_UNSPECIFIED;	// Note: NativeOrientation can only be Landscape or Portrait even though the DisplayOrientations enum has other values.
			switch (m_nativeOrientation) {
			case ::gpk::GRID_ROTATION_0:
				switch (m_currentOrientation) {
				case ::gpk::GRID_ROTATION_0		: rotation = DXGI_MODE_ROTATION_IDENTITY; break; 
				case ::gpk::GRID_ROTATION_90	: rotation = DXGI_MODE_ROTATION_ROTATE270; break; 
				case ::gpk::GRID_ROTATION_180	: rotation = DXGI_MODE_ROTATION_ROTATE180; break; 
				case ::gpk::GRID_ROTATION_270	: rotation = DXGI_MODE_ROTATION_ROTATE90; break; 
				}
				break;
			case ::gpk::GRID_ROTATION_90:
				switch (m_currentOrientation) {
				case ::gpk::GRID_ROTATION_0		: rotation = DXGI_MODE_ROTATION_ROTATE90; break;
				case ::gpk::GRID_ROTATION_90	: rotation = DXGI_MODE_ROTATION_IDENTITY; break; 
				case ::gpk::GRID_ROTATION_180	: rotation = DXGI_MODE_ROTATION_ROTATE270; break; 
				case ::gpk::GRID_ROTATION_270	: rotation = DXGI_MODE_ROTATION_ROTATE180; break;
				}
				break;
			}
			return rotation;
		}

		void											CreateDeviceIndependentResources		() {
			D2D1_FACTORY_OPTIONS								options									= {};	// Initialize Direct2D resources.
		#if defined(_DEBUG)
			options.debugLevel								= D2D1_DEBUG_LEVEL_INFORMATION;		// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
		#endif
			DX::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &options, &m_d2dFactory));	// Initialize the Direct2D Factory.
			DX::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), &m_dwriteFactory));			// Initialize the DirectWrite Factory.
			DX::ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_wicFactory)));	// Initialize the Windows Imaging Component (WIC) Factory.
		}

		// Determine the dimensions of the render target and whether it will be scaled down.
		void											UpdateRenderTargetSize					() {
			m_effectiveDpi									= m_dpi;
			if (!DisplayMetrics::SupportHighResolutions && m_dpi > DisplayMetrics::DpiThreshold) {	// To improve battery life on high resolution devices, render to a smaller render target and allow the GPU to scale the output when it is presented.
				float												width									= DX::ConvertDipsToPixels(m_logicalSize.x, m_dpi);
				float												height									= DX::ConvertDipsToPixels(m_logicalSize.y, m_dpi);
				if (std::max(width, height) > DisplayMetrics::WidthThreshold && std::min(width, height) > DisplayMetrics::HeightThreshold)	// When the device is in portrait orientation, height > width. Compare the larger dimension against the width threshold and the smaller dimension against the height threshold.
					m_effectiveDpi									/= 2.0f;	// To scale the app we change the effective DPI. Logical size does not change.
			}
			// Calculate the necessary render target size in pixels.
			m_outputSize.x									= std::max(1.0f, DX::ConvertDipsToPixels(m_logicalSize.x, m_effectiveDpi));
			m_outputSize.y									= std::max(1.0f, DX::ConvertDipsToPixels(m_logicalSize.y, m_effectiveDpi));
		}

		::gpk::SCoord2<float>							GetOutputSize							()	const									{ return m_outputSize; }	// The size of the render target, in pixels.
		::gpk::SCoord2<float>							GetLogicalSize							()	const									{ return m_logicalSize; }	// The size of the render target, in dips.
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

		void											RegisterDeviceNotify					(DX::IDeviceNotify* deviceNotify)			{ m_deviceNotify = deviceNotify; }
		void											SetLogicalSize							(const ::gpk::SCoord2<float> & logicalSize)	{ if (m_logicalSize			== logicalSize			) return; m_logicalSize			= logicalSize;			CreateWindowSizeDependentResources(); } 
		void											SetCurrentOrientation					(::gpk::GRID_ROTATION currentOrientation)	{ if (m_currentOrientation	== currentOrientation	) return; m_currentOrientation	= currentOrientation;	CreateWindowSizeDependentResources(); } 
		void											SetDpi									(float dpi)									{
			if (dpi != m_dpi) {
				m_dpi											= dpi;
				RECT												rect									= {};
				GetClientRect(m_window, &rect);
				m_logicalSize									= {float(rect.right - rect.left), float(rect.bottom - rect.top)};	// When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
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

		// Recreate all device resources and set them back to the current state.
		void											ValidateDevice							()											{ if(::gpk::d3dDeviceValidate(m_d3dDevice)) HandleDeviceLost();	}
		void											HandleDeviceLost						()											{
			m_swapChain->SetFullscreenState(FALSE, 0);
			m_swapChain										= nullptr;
			if (m_deviceNotify) 
				m_deviceNotify->OnDeviceLost();
			CreateDeviceResources();
			m_d2dContext->SetDpi(m_dpi, m_dpi);
			CreateWindowSizeDependentResources();
			if (m_deviceNotify) 
				m_deviceNotify->OnDeviceRestored();
		}

		void											SetWindow								(HWND window)		{
			m_window										= window;

			RECT												rect									= {};
			GetClientRect(m_window, &rect);
			m_logicalSize									= {float(rect.right - rect.left), float(rect.bottom - rect.top)};	// When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
			m_nativeOrientation								= ScreenRotation();
			m_currentOrientation							= ScreenRotation();
			uint32_t											dpi										= GetDpiForWindow(m_window);
			m_dpi											= float(dpi);
			m_d2dContext->SetDpi(m_dpi, m_dpi);
			CreateWindowSizeDependentResources();
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
		::gpk::GRID_ROTATION							ScreenRotation							()	{
			DeviceMode											= {sizeof(DEVMODE)};
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DeviceMode);	// returned rotation is relative to the natural (default) rotation for this display

			switch(DeviceMode.dmDisplayOrientation) {
			default:
			case DMDO_DEFAULT	: return ::gpk::GRID_ROTATION_0			;
			case DMDO_90		: return ::gpk::GRID_ROTATION_90		;
			case DMDO_180		: return ::gpk::GRID_ROTATION_180		;
			case DMDO_270		: return ::gpk::GRID_ROTATION_270		;
			}
		}
	};
}

#endif // DEVICERESOURCES_H_2387647892