﻿#include "gpk_d3d.h"
#include "gpk_coord.h"
#include "gpk_view_grid.h"

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

#if defined(_DEBUG)
	// Check for SDK Layer support.
	static inline bool								SdkLayersAvailable		()							{ return SUCCEEDED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_NULL, 0, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, nullptr, nullptr, nullptr)); }
#endif
	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels. Round to nearest integer.
	static inline float								ConvertDipsToPixels		(float dips, float dpi)		{ static constexpr float dipsPerInch = 96.0f; return floorf(dips * dpi / dipsPerInch + 0.5f); } 

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
		::gpk::ptr_com<ID3D11Device3>					m_d3dDevice;
		::gpk::ptr_com<ID3D11DeviceContext3>			m_d3dContext;
		::gpk::ptr_com<IDXGISwapChain3>					m_swapChain;

		// Direct3D rendering objects. Required for 3D.
		::gpk::ptr_com<ID3D11RenderTargetView1>			m_d3dRenderTargetView;
		::gpk::ptr_com<ID3D11DepthStencilView>			m_d3dDepthStencilView;
		D3D11_VIEWPORT									m_screenViewport						= {};

		// Direct2D drawing components.
		::gpk::ptr_com<ID2D1Factory3>					m_d2dFactory;
		::gpk::ptr_com<ID2D1Device2>					m_d2dDevice;
		::gpk::ptr_com<ID2D1DeviceContext2>				m_d2dContext;
		::gpk::ptr_com<ID2D1Bitmap1>					m_d2dTargetBitmap;

		// DirectWrite drawing components.
		::gpk::ptr_com<IDWriteFactory3>					m_dwriteFactory;
		::gpk::ptr_com<IWICImagingFactory2>				m_wicFactory;

		// Cached device properties.
		D3D_FEATURE_LEVEL								m_d3dFeatureLevel						= D3D_FEATURE_LEVEL_9_1;
		::gpk::SCoord2<float>							m_d3dRenderTargetSize					= {};
		::gpk::SCoord2<float>							m_outputSize							= {};
		::gpk::SCoord2<float>							m_logicalSize							= {};
		::gpk::GRID_ROTATION							m_nativeOrientation						= {};
		::gpk::GRID_ROTATION							m_currentOrientation					= {};
		float											m_dpi									= -1.0f;
		float											m_effectiveDpi							= -1.0f;	// This is the DPI that will be reported back to the app. It takes into account whether the app supports high resolution screens or not.

		// Transforms used for display orientation.
		D2D1::Matrix3x2F								m_orientationTransform2D				= {};
		DirectX::XMFLOAT4X4								m_orientationTransform3D				= {};

		::gpk::error_t									Initialize								()									{
			gpk_necs(CreateDeviceIndependentResources());
			gpk_necs(CreateDeviceResources());
			return 0;
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

		void											RegisterDeviceNotify					(DX::IDeviceNotify* deviceNotify)			{ m_deviceNotify = deviceNotify; }
		::gpk::error_t									SetLogicalSize							(const ::gpk::SCoord2<float> & logicalSize)	{ if (m_logicalSize			!= logicalSize			) { m_logicalSize			= logicalSize;			gpk_necs(CreateWindowSizeDependentResources()); } return 0; } 
		::gpk::error_t									SetCurrentOrientation					(::gpk::GRID_ROTATION currentOrientation)	{ if (m_currentOrientation	!= currentOrientation	) { m_currentOrientation	= currentOrientation;	gpk_necs(CreateWindowSizeDependentResources()); } return 0; } 
		::gpk::error_t									SetDpi									(float dpi)									{
			if (dpi != m_dpi) {
				m_dpi											= dpi;
				m_logicalSize									= {m_window->Bounds.Width, m_window->Bounds.Height};	// When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
				m_d2dContext->SetDpi(m_dpi, m_dpi);
				gpk_necs(CreateWindowSizeDependentResources());
			}
			return 0;
		}

		// Call this method when the app suspends. It provides a hint to the driver that the app is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
		::gpk::error_t									Trim									() {
			::gpk::ptr_com<IDXGIDevice3>						dxgiDevice;
			gpk_necs(m_d3dDevice.as(dxgiDevice));
			dxgiDevice->Trim();
			return 0;
		}

		// Recreate all device resources and set them back to the current state.
		::gpk::error_t									ValidateDevice							()											{ return ::gpk::d3dDeviceValidate(m_d3dDevice) ? HandleDeviceLost() : 0; }
		::gpk::error_t									HandleDeviceLost						()											{
			m_swapChain										= nullptr;
			if (m_deviceNotify) 
				m_deviceNotify->OnDeviceLost();
			gpk_necs(CreateDeviceResources());
			m_d2dContext->SetDpi(m_dpi, m_dpi);
			gpk_necs(CreateWindowSizeDependentResources());
			if (m_deviceNotify) 
				m_deviceNotify->OnDeviceRestored();
			return 0;
		}

		::gpk::error_t									SetWindow								(Windows::UI::Core::CoreWindow^ window)		{
			Windows::Graphics::Display::DisplayInformation		^ currentDisplayInformation				= Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
			m_window										= window;
			m_logicalSize									= {window->Bounds.Width, window->Bounds.Height};
			m_nativeOrientation								= (::gpk::GRID_ROTATION)currentDisplayInformation->NativeOrientation;
			m_currentOrientation							= (::gpk::GRID_ROTATION)currentDisplayInformation->CurrentOrientation;
			m_dpi											= currentDisplayInformation->LogicalDpi;
			m_d2dContext->SetDpi(m_dpi, m_dpi);
			return CreateWindowSizeDependentResources();
		}

		::gpk::error_t									CreateDeviceIndependentResources		() {
			D2D1_FACTORY_OPTIONS								options									= {};	// Initialize Direct2D resources.
		#if defined(_DEBUG)
			options.debugLevel								= D2D1_DEBUG_LEVEL_INFORMATION;		// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
		#endif
			gpk_hrcall(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &options, (void**)&m_d2dFactory));	// Initialize the Direct2D Factory.
			gpk_hrcall(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), (IUnknown**)&m_dwriteFactory));			// Initialize the DirectWrite Factory.
			gpk_hrcall(CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_wicFactory)));	// Initialize the Windows Imaging Component (WIC) Factory.
			return 0;
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

		// Present the contents of the swap chain to the screen.
		::gpk::error_t									Present									() {
			DXGI_PRESENT_PARAMETERS								parameters								= {};
			HRESULT												hr										= m_swapChain->Present1(1, 0, &parameters);	// The first argument instructs DXGI to block until VSync, putting the application to sleep until the next VSync. This ensures we don't waste any cycles rendering frames that will never be displayed to the screen.
			m_d3dContext->DiscardView1(m_d3dRenderTargetView, nullptr, 0);	// Discard the contents of the render target. This is a valid operation only when the existing contents will be entirely overwritten. If dirty or scroll rects are used, this call should be removed.
			m_d3dContext->DiscardView1(m_d3dDepthStencilView, nullptr, 0);	// Discard the contents of the depth stencil.
			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
				gpk_necs(HandleDeviceLost());	// If the device was removed either by a disconnection or a driver upgrade, we  must recreate all device resources.
			else
				gpk_hrcall(hr);
			return 0;
		}

		// Configures the Direct3D device, and stores handles to it and the device context.
		::gpk::error_t									CreateDeviceResources					() {
			// This flag adds support for surfaces with a different color channel ordering than the API default. It is required for compatibility with Direct2D.
			uint32_t											creationFlags							= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		#if defined(_DEBUG)
			if (DX::SdkLayersAvailable()) // If the project is in a debug build, enable debugging via SDK Layers with this flag.
				creationFlags								|= D3D11_CREATE_DEVICE_DEBUG;
		#endif

			// This array defines the set of DirectX hardware feature levels this app will support. Note the ordering should be preserved. Don't forget to declare your application's minimum required feature level in its description. All applications are assumed to support 9.1 unless otherwise stated.
			static constexpr D3D_FEATURE_LEVEL					featureLevels	[]						=
				{ D3D_FEATURE_LEVEL_12_1
				, D3D_FEATURE_LEVEL_12_0
				, D3D_FEATURE_LEVEL_11_1
				, D3D_FEATURE_LEVEL_11_0
				, D3D_FEATURE_LEVEL_10_1
				, D3D_FEATURE_LEVEL_10_0
				, D3D_FEATURE_LEVEL_9_3
				, D3D_FEATURE_LEVEL_9_2
				, D3D_FEATURE_LEVEL_9_1
				};

			// Create the Direct3D 11 API device object and a corresponding context.
			::gpk::ptr_com<ID3D11Device>						device;
			::gpk::ptr_com<ID3D11DeviceContext>					context;

			 // If the initialization fails, fall back to the WARP device. For more information on WARP, see: https://go.microsoft.com/fwlink/?LinkId=286690. 
			if (FAILED(D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, featureLevels, (uint32_t)std::size(featureLevels), D3D11_SDK_VERSION, &device, &m_d3dFeatureLevel, &context)))
				gpk_hrcall(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP, 0, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &m_d3dFeatureLevel, &context));

			// Store pointers to the Direct3D 11.3 API device and immediate context.
			gpk_necs(device.as(m_d3dDevice));
			gpk_necs(context.as(m_d3dContext));

			// Create the Direct2D device object and a corresponding context.
			::gpk::ptr_com<IDXGIDevice3>						dxgiDevice;
			gpk_necs(m_d3dDevice.as(dxgiDevice));
			gpk_hrcall(m_d2dFactory->CreateDevice(dxgiDevice, &m_d2dDevice));
			gpk_hrcall(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dContext));
			return 0;
		}

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

		// These resources need to be recreated every time the window size is changed.
		::gpk::error_t									CreateWindowSizeDependentResources		() {
			// Clear the previous window size specific context.
			ID3D11RenderTargetView								* nullViews	[]							= {{}};
			m_d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
			m_d3dRenderTargetView							= nullptr;
			m_d3dDepthStencilView							= nullptr;

			m_d2dContext->SetTarget(nullptr);
			m_d2dTargetBitmap								= nullptr;

			m_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

			UpdateRenderTargetSize();

			// The width and height of the swap chain must be based on the window's natively-oriented width and height. If the window is not in the native orientation, the dimensions must be reversed.
			DXGI_MODE_ROTATION									displayRotation							= ComputeDisplayRotation();
			bool												swapDimensions							= displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;
			m_d3dRenderTargetSize.x							= swapDimensions ? m_outputSize.y : m_outputSize.x;
			m_d3dRenderTargetSize.y							= swapDimensions ? m_outputSize.x : m_outputSize.y;

			if (m_swapChain) { // If the swap chain already exists, resize it.
				HRESULT												hr									= m_swapChain->ResizeBuffers(2, lround(m_d3dRenderTargetSize.x), lround(m_d3dRenderTargetSize.y), DXGI_FORMAT_B8G8R8A8_UNORM, 0); // Double-buffered swap chain.
				if (hr != DXGI_ERROR_DEVICE_REMOVED && hr != DXGI_ERROR_DEVICE_RESET) 
					gpk_hrcall(hr);
				else {
					gpk_necs(HandleDeviceLost());	// If the device was removed for any reason, a new device and swap chain will need to be created.
					return 0;	// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method and correctly set up the new device.
				}
			} else {
				// Otherwise, create a new one using the same adapter as the existing Direct3D device.
				DXGI_SCALING										scaling								= DisplayMetrics::SupportHighResolutions ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;
				DXGI_SWAP_CHAIN_DESC1								swapChainDesc						= {};
				swapChainDesc.Width								= lround(m_d3dRenderTargetSize.x);		// Match the size of the window.
				swapChainDesc.Height							= lround(m_d3dRenderTargetSize.y);
				swapChainDesc.Format							= DXGI_FORMAT_B8G8R8A8_UNORM;				// This is the most common swap chain format.
				swapChainDesc.SampleDesc.Count					= 1;								// Don't use multi-sampling.
				swapChainDesc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount						= 2;									// Use double-buffering to minimize latency.
				swapChainDesc.SwapEffect						= DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// All Microsoft Store apps must use this SwapEffect.
				swapChainDesc.Scaling							= scaling;
				swapChainDesc.AlphaMode							= DXGI_ALPHA_MODE_IGNORE;

				//DXGI_SWAP_CHAIN_FULLSCREEN_DESC					swapChainDescFS						= {0};
				//MONITORINFO										monitor_info						= {sizeof(monitor_info)};
				//ree_if(FALSE == GetMonitorInfoA(MonitorFromWindow(m_window, MONITOR_DEFAULTTOPRIMARY), &monitor_info), "Cannot get MONITORINFO for hWnd(0x%x)", m_window);

				//swapChainDescFS.Scaling							= DisplayMetrics::SupportHighResolutions ? DXGI_MODE_SCALING_UNSPECIFIED : DXGI_MODE_SCALING_STRETCHED;;
				//swapChainDescFS.Windowed						= true;

				// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
				::gpk::ptr_com<IDXGIDevice3>						dxgiDevice;
				::gpk::ptr_com<IDXGIAdapter>						dxgiAdapter;
				::gpk::ptr_com<IDXGIFactory4>						dxgiFactory;
				::gpk::ptr_com<IDXGISwapChain1>						swapChain;
				gpk_necs(m_d3dDevice.as(&dxgiDevice));
				gpk_hrcall(dxgiDevice->GetAdapter(&dxgiAdapter));
				gpk_hrcall(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));
				gpk_hrcall(dxgiFactory->CreateSwapChainForCoreWindow(m_d3dDevice, (IUnknown*)m_window.Get(), &swapChainDesc, nullptr, &swapChain));
				gpk_necs(swapChain.as(m_swapChain));
				gpk_hrcall(dxgiDevice->SetMaximumFrameLatency(1));	// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and ensures that the application will only render after each VSync, minimizing power consumption.
			}
	
			// Set the proper orientation for the swap chain, and generate 2D and 3D matrix transformations for rendering to the rotated swap chain. Note the rotation angle for the 2D and 3D transforms are different.
			// This is due to the difference in coordinate spaces. Additionally, the 3D matrix is specified explicitly to avoid rounding errors.
			static constexpr DirectX::XMFLOAT4X4				
				ZROTATION0
					( 1.0f, 0.0f, 0.0f, 0.0f
					, 0.0f, 1.0f, 0.0f, 0.0f
					, 0.0f, 0.0f, 1.0f, 0.0f
					, 0.0f, 0.0f, 0.0f, 1.0f
				), ZROTATION90
					( 0.0f, 1.0f, 0.0f, 0.0f
					, -1.0f, 0.0f, 0.0f, 0.0f
					, 0.0f, 0.0f, 1.0f, 0.0f
					, 0.0f, 0.0f, 0.0f, 1.0f
				), ZROTATION180
					( -1.0f, 0.0f, 0.0f, 0.0f
					, 0.0f, -1.0f, 0.0f, 0.0f
					, 0.0f, 0.0f, 1.0f, 0.0f
					, 0.0f, 0.0f, 0.0f, 1.0f
				), ZROTATION270
					( 0.0f, -1.0f, 0.0f, 0.0f
					, 1.0f, 0.0f, 0.0f, 0.0f
					, 0.0f, 0.0f, 1.0f, 0.0f
					, 0.0f, 0.0f, 0.0f, 1.0f
				);

			switch (displayRotation) {
			default:
			case DXGI_MODE_ROTATION_IDENTITY	: m_orientationTransform3D = ZROTATION0		; m_orientationTransform2D = D2D1::Matrix3x2F::Identity(); break;
			case DXGI_MODE_ROTATION_ROTATE270	: m_orientationTransform3D = ZROTATION90	; m_orientationTransform2D = D2D1::Matrix3x2F::Rotation(270.f) * D2D1::Matrix3x2F::Translation(0.0f, m_logicalSize.x); break;
			case DXGI_MODE_ROTATION_ROTATE90	: m_orientationTransform3D = ZROTATION270	; m_orientationTransform2D = D2D1::Matrix3x2F::Rotation(90.0f) * D2D1::Matrix3x2F::Translation(m_logicalSize.y, 0.0f); break;
			case DXGI_MODE_ROTATION_ROTATE180	: m_orientationTransform3D = ZROTATION180	; m_orientationTransform2D = D2D1::Matrix3x2F::Rotation(180.f) * D2D1::Matrix3x2F::Translation(m_logicalSize.x, m_logicalSize.y); break;
			}
			gpk_hrcall(m_swapChain->SetRotation(displayRotation));

			::gpk::ptr_com<ID3D11Texture2D1>						backBuffer;
			gpk_hrcall(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
			gpk_hrcall(m_d3dDevice->CreateRenderTargetView1(backBuffer, nullptr, &m_d3dRenderTargetView));	// Create a render target view of the swap chain back buffer.
	
			CD3D11_TEXTURE2D_DESC1									depthStencilDesc					(DXGI_FORMAT_D24_UNORM_S8_UINT, lround(m_d3dRenderTargetSize.x), lround(m_d3dRenderTargetSize.y), 1, 1, D3D11_BIND_DEPTH_STENCIL);
			::gpk::ptr_com<ID3D11Texture2D1>						depthStencil;
			CD3D11_DEPTH_STENCIL_VIEW_DESC							depthStencilViewDesc				(D3D11_DSV_DIMENSION_TEXTURE2D);
			gpk_hrcall(m_d3dDevice->CreateTexture2D1(&depthStencilDesc, nullptr, &depthStencil));
			gpk_hrcall(m_d3dDevice->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &m_d3dDepthStencilView));		// Create a depth stencil view for use with 3D rendering if needed. This depth stencil view has only one texture. use a single mipmap level
	
			m_screenViewport									= CD3D11_VIEWPORT(0.0f, 0.0f, m_d3dRenderTargetSize.x, m_d3dRenderTargetSize.y);
			m_d3dContext->RSSetViewports(1, &m_screenViewport);	// Set the 3D rendering viewport to target the entire window.

			D2D1_BITMAP_PROPERTIES1									bitmapProperties					= D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), m_dpi, m_dpi);
			::gpk::ptr_com<IDXGISurface2>							dxgiBackBuffer;
			gwarn_if(errored(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))), "%s", "");
			gpk_hrcall(m_d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &bitmapProperties, &m_d2dTargetBitmap));	// Create a Direct2D target bitmap associated with the swap chain back buffer and set it as the current target.

			m_d2dContext->SetTarget(m_d2dTargetBitmap);
			m_d2dContext->SetDpi(m_effectiveDpi, m_effectiveDpi);
			m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);	// Grayscale text anti-aliasing is recommended for all Microsoft Store apps.

			return 0;
		}
	};
} // namespace

#endif // DEVICERESOURCES_H_2387647892