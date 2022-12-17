#include "gpk_d3d.h"
#include "gpk_coord.h"

#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <agile.h>

#include <ppltasks.h>



#ifndef DEVICERESOURCES_H_2387647892
#define DEVICERESOURCES_H_2387647892

namespace DX
{
	// Function that reads from a binary file asynchronously.
	inline Concurrency::task<std::vector<byte>>		ReadDataAsync			(const std::wstring& filename) {
		using namespace										Windows::Storage;
		using namespace										Concurrency;

		Windows::Storage::StorageFolder						^ folder				= Windows::ApplicationModel::Package::Current->InstalledLocation;
		return create_task(folder->GetFileAsync(Platform::StringReference(filename.c_str()))).then([] (StorageFile^ file) { return FileIO::ReadBufferAsync(file); }).then([] 
			(Streams::IBuffer^ fileBuffer) -> std::vector<byte> {
				std::vector<byte>									returnBuffer;
				returnBuffer.resize(fileBuffer->Length);
				Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(Platform::ArrayReference<byte>(returnBuffer.data(), fileBuffer->Length));
				return returnBuffer; 
			}
		);
	}

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
			throw Platform::Exception::CreateException(hr);	// Set a breakpoint on this line to catch Win32 API errors.
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
		Platform::Agile<Windows::UI::Core::CoreWindow>	m_window;	// Cached reference to the Window.

		IDeviceNotify									* m_deviceNotify						= {};	// The IDeviceNotify can be held directly as it owns the DeviceResources.

		// Direct3D objects.
		::gpk::ptr_com<ID3D11Device3>			m_d3dDevice;
		::gpk::ptr_com<ID3D11DeviceContext3>	m_d3dContext;
		::gpk::ptr_com<IDXGISwapChain3>			m_swapChain;

		// Direct3D rendering objects. Required for 3D.
		::gpk::ptr_com<ID3D11RenderTargetView1>	m_d3dRenderTargetView;
		::gpk::ptr_com<ID3D11DepthStencilView>	m_d3dDepthStencilView;
		D3D11_VIEWPORT									m_screenViewport						= {};

		// Direct2D drawing components.
		::gpk::ptr_com<ID2D1Factory3>			m_d2dFactory;
		::gpk::ptr_com<ID2D1Device2>			m_d2dDevice;
		::gpk::ptr_com<ID2D1DeviceContext2>		m_d2dContext;
		::gpk::ptr_com<ID2D1Bitmap1>			m_d2dTargetBitmap;

		// DirectWrite drawing components.
		::gpk::ptr_com<IDWriteFactory3>			m_dwriteFactory;
		::gpk::ptr_com<IWICImagingFactory2>		m_wicFactory;

		// Cached device properties.
		D3D_FEATURE_LEVEL								m_d3dFeatureLevel						= D3D_FEATURE_LEVEL_9_1;
		::gpk::SCoord2<float>							m_d3dRenderTargetSize					= {};
		::gpk::SCoord2<float>							m_outputSize							= {};
		::gpk::SCoord2<float>							m_logicalSize							= {};
		Windows::Graphics::Display::DisplayOrientations	m_nativeOrientation						= {};
		Windows::Graphics::Display::DisplayOrientations	m_currentOrientation					= {};
		float											m_dpi									= -1.0f;
		float											m_effectiveDpi							= -1.0f;	// This is the DPI that will be reported back to the app. It takes into account whether the app supports high resolution screens or not.

		// Transforms used for display orientation.
		D2D1::Matrix3x2F								m_orientationTransform2D				= {};
		DirectX::XMFLOAT4X4								m_orientationTransform3D				= {};

		void											CreateDeviceResources					();
		void											CreateWindowSizeDependentResources		();
		// This method determines the rotation between the display device's native orientation and the current display orientation.
		DXGI_MODE_ROTATION								ComputeDisplayRotation					() {
			DXGI_MODE_ROTATION									rotation								= DXGI_MODE_ROTATION_UNSPECIFIED;	// Note: NativeOrientation can only be Landscape or Portrait even though the DisplayOrientations enum has other values.
			switch (m_nativeOrientation) {
			case Windows::Graphics::Display::DisplayOrientations::Landscape:
				switch (m_currentOrientation) {
				case Windows::Graphics::Display::DisplayOrientations::Landscape			: rotation = DXGI_MODE_ROTATION_IDENTITY; break; 
				case Windows::Graphics::Display::DisplayOrientations::Portrait			: rotation = DXGI_MODE_ROTATION_ROTATE270; break; 
				case Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped	: rotation = DXGI_MODE_ROTATION_ROTATE180; break; 
				case Windows::Graphics::Display::DisplayOrientations::PortraitFlipped	: rotation = DXGI_MODE_ROTATION_ROTATE90; break; 
				}
				break;
			case Windows::Graphics::Display::DisplayOrientations::Portrait:
				switch (m_currentOrientation) {
				case Windows::Graphics::Display::DisplayOrientations::Landscape			: rotation = DXGI_MODE_ROTATION_ROTATE90; break;
				case Windows::Graphics::Display::DisplayOrientations::Portrait			: rotation = DXGI_MODE_ROTATION_IDENTITY; break; 
				case Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped	: rotation = DXGI_MODE_ROTATION_ROTATE270; break; 
				case Windows::Graphics::Display::DisplayOrientations::PortraitFlipped	: rotation = DXGI_MODE_ROTATION_ROTATE180; break;
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
			DX::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &options, (void**)&m_d2dFactory));	// Initialize the Direct2D Factory.
			DX::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), (IUnknown**)&m_dwriteFactory));			// Initialize the DirectWrite Factory.
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

														D3DDeviceResources							()									{
			CreateDeviceIndependentResources();
			CreateDeviceResources();
		}

		::gpk::SCoord2<float>							GetOutputSize							()	const									{ return m_outputSize; }	// The size of the render target, in pixels.
		::gpk::SCoord2<float>							GetLogicalSize							()	const									{ return m_logicalSize; }	// The size of the render target, in dips.
		float											GetDpi									()	const									{ return m_effectiveDpi; }

		// D3D Accessors.
		ID3D11Device3*									GetD3DDevice							()	const									{ return m_d3dDevice.get(); }
		ID3D11DeviceContext3*							GetD3DDeviceContext						()	const									{ return m_d3dContext.get(); }
		IDXGISwapChain3*								GetSwapChain							()	const									{ return m_swapChain.get(); }
		D3D_FEATURE_LEVEL								GetDeviceFeatureLevel					()	const									{ return m_d3dFeatureLevel; }
		ID3D11RenderTargetView1*						GetBackBufferRenderTargetView			()	const									{ return m_d3dRenderTargetView.get(); }
		ID3D11DepthStencilView*							GetDepthStencilView						()	const									{ return m_d3dDepthStencilView.get(); }
		D3D11_VIEWPORT									GetScreenViewport						()	const									{ return m_screenViewport; }
		DirectX::XMFLOAT4X4								GetOrientationTransform3D				()	const									{ return m_orientationTransform3D; }

		// D2D Accessors.
		ID2D1Factory3*									GetD2DFactory							()	const									{ return m_d2dFactory.get(); }
		ID2D1Device2*									GetD2DDevice							()	const									{ return m_d2dDevice.get(); }
		ID2D1DeviceContext2*							GetD2DDeviceContext						()	const									{ return m_d2dContext.get(); }
		ID2D1Bitmap1*									GetD2DTargetBitmap						()	const									{ return m_d2dTargetBitmap.get(); }
		IDWriteFactory3*								GetDWriteFactory						()	const									{ return m_dwriteFactory.get(); }
		IWICImagingFactory2*							GetWicImagingFactory					()	const									{ return m_wicFactory.get(); }
		D2D1::Matrix3x2F								GetOrientationTransform2D				()	const									{ return m_orientationTransform2D; }

		void											RegisterDeviceNotify					(DX::IDeviceNotify* deviceNotify)										{ m_deviceNotify = deviceNotify; }
		void											SetLogicalSize							(const ::gpk::SCoord2<float> & logicalSize)								{ if (m_logicalSize			== logicalSize			) return; m_logicalSize			= logicalSize;			CreateWindowSizeDependentResources(); } 
		void											SetCurrentOrientation					(Windows::Graphics::Display::DisplayOrientations currentOrientation)	{ if (m_currentOrientation	== currentOrientation	) return; m_currentOrientation	= currentOrientation;	CreateWindowSizeDependentResources(); } 
		void											SetDpi									(float dpi)																{
			if (dpi != m_dpi) {
				m_dpi											= dpi;
				m_logicalSize									= {m_window->Bounds.Width, m_window->Bounds.Height};	// When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
				m_d2dContext->SetDpi(m_dpi, m_dpi);
				CreateWindowSizeDependentResources();
			}
		}

		// Call this method when the app suspends. It provides a hint to the driver that the app is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
		void											Trim									() {
			::gpk::ptr_com<IDXGIDevice3>				dxgiDevice;
			m_d3dDevice.as(dxgiDevice);
			dxgiDevice->Trim();
		}

		// Recreate all device resources and set them back to the current state.
		void											ValidateDevice							()											{ if(::gpk::d3dDeviceValidate(m_d3dDevice)) HandleDeviceLost();	}
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

		void											SetWindow								(Windows::UI::Core::CoreWindow^ window)		{
			Windows::Graphics::Display::DisplayInformation		^ currentDisplayInformation				= Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
			m_window										= window;
			m_logicalSize									= {window->Bounds.Width, window->Bounds.Height};
			m_nativeOrientation								= currentDisplayInformation->NativeOrientation;
			m_currentOrientation							= currentDisplayInformation->CurrentOrientation;
			m_dpi											= currentDisplayInformation->LogicalDpi;
			m_d2dContext->SetDpi(m_dpi, m_dpi);
			CreateWindowSizeDependentResources();
		}

		// Present the contents of the swap chain to the screen.
		void											Present									() {
			DXGI_PRESENT_PARAMETERS								parameters								= {};
			HRESULT												hr										= m_swapChain->Present1(1, 0, &parameters);	// The first argument instructs DXGI to block until VSync, putting the application to sleep until the next VSync. This ensures we don't waste any cycles rendering frames that will never be displayed to the screen.
			m_d3dContext->DiscardView1(m_d3dRenderTargetView, nullptr, 0);	// Discard the contents of the render target. This is a valid operation only when the existing contents will be entirely overwritten. If dirty or scroll rects are used, this call should be removed.
			m_d3dContext->DiscardView1(m_d3dDepthStencilView, nullptr, 0);	// Discard the contents of the depth stencil.
			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
				HandleDeviceLost();	// If the device was removed either by a disconnection or a driver upgrade, we  must recreate all device resources.
			else
				DX::ThrowIfFailed(hr);
		}
	};
}

#endif // DEVICERESOURCES_H_2387647892