#include "DeviceResources.h"

using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Platform;


// Set the proper orientation for the swap chain, and generate 2D and 3D matrix transformations for rendering to the rotated swap chain. Note the rotation angle for the 2D and 3D transforms are different.
// This is due to the difference in coordinate spaces. Additionally, the 3D matrix is specified explicitly to avoid rounding errors.
static constexpr XMFLOAT4X4				
	ZRotation0
		( 1.0f, 0.0f, 0.0f, 0.0f
		, 0.0f, 1.0f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.0f, 0.0f, 0.0f, 1.0f
	), ZRotation90
		( 0.0f, 1.0f, 0.0f, 0.0f
		, -1.0f, 0.0f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.0f, 0.0f, 0.0f, 1.0f
	), ZRotation180
		( -1.0f, 0.0f, 0.0f, 0.0f
		, 0.0f, -1.0f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.0f, 0.0f, 0.0f, 1.0f
	), ZRotation270
		( 0.0f, -1.0f, 0.0f, 0.0f
		, 1.0f, 0.0f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.0f, 0.0f, 0.0f, 1.0f
	);

// Configures the Direct3D device, and stores handles to it and the device context.
void DX::D3DDeviceResources::CreateDeviceResources() {
	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D.
	UINT								creationFlags					= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	if (DX::SdkLayersAvailable()) // If the project is in a debug build, enable debugging via SDK Layers with this flag.
		creationFlags					|= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// This array defines the set of DirectX hardware feature levels this app will support. Note the ordering should be preserved. Don't forget to declare your application's minimum required feature level in its description. All applications are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL					featureLevels	[]				=
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
	ComPtr<ID3D11Device>				device;
	ComPtr<ID3D11DeviceContext>			context;

	 // If the initialization fails, fall back to the WARP device. For more information on WARP, see: https://go.microsoft.com/fwlink/?LinkId=286690. 
	if (FAILED(D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, featureLevels, (uint32_t)std::size(featureLevels), D3D11_SDK_VERSION, &device, &m_d3dFeatureLevel, &context)))
		DX::ThrowIfFailed(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP, 0, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &m_d3dFeatureLevel, &context));

	// Store pointers to the Direct3D 11.3 API device and immediate context.
	DX::ThrowIfFailed(device.As(&m_d3dDevice));
	DX::ThrowIfFailed(context.As(&m_d3dContext));

	// Create the Direct2D device object and a corresponding context.
	ComPtr<IDXGIDevice3>				dxgiDevice;
	DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));
	DX::ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));
	DX::ThrowIfFailed(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dContext));
}

// These resources need to be recreated every time the window size is changed.
void DX::D3DDeviceResources::CreateWindowSizeDependentResources() {
	// Clear the previous window size specific context.
	ID3D11RenderTargetView				* nullViews[]					= {{}};
	m_d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	m_d3dRenderTargetView			= nullptr;
	m_d3dDepthStencilView			= nullptr;

	m_d2dContext->SetTarget(nullptr);
	m_d2dTargetBitmap				= nullptr;

	m_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	UpdateRenderTargetSize();

	// The width and height of the swap chain must be based on the window's natively-oriented width and height. If the window is not in the native orientation, the dimensions must be reversed.
	DXGI_MODE_ROTATION					displayRotation					= ComputeDisplayRotation();
	bool								swapDimensions					= displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;
	m_d3dRenderTargetSize.x			= swapDimensions ? m_outputSize.y : m_outputSize.x;
	m_d3dRenderTargetSize.y			= swapDimensions ? m_outputSize.x : m_outputSize.y;

	if (m_swapChain) { // If the swap chain already exists, resize it.
		HRESULT								hr								= m_swapChain->ResizeBuffers(2, lround(m_d3dRenderTargetSize.x), lround(m_d3dRenderTargetSize.y), DXGI_FORMAT_B8G8R8A8_UNORM, 0); // Double-buffered swap chain.
		if (hr != DXGI_ERROR_DEVICE_REMOVED && hr != DXGI_ERROR_DEVICE_RESET) 
			DX::ThrowIfFailed(hr);
		else {
			HandleDeviceLost();	// If the device was removed for any reason, a new device and swap chain will need to be created.
			return;	// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method and correctly set up the new device.
		}
	} else {
		// Otherwise, create a new one using the same adapter as the existing Direct3D device.
		DXGI_SCALING							scaling						= DisplayMetrics::SupportHighResolutions ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;
		DXGI_SWAP_CHAIN_DESC1					swapChainDesc				= {0};
		swapChainDesc.Width					= lround(m_d3dRenderTargetSize.x);		// Match the size of the window.
		swapChainDesc.Height				= lround(m_d3dRenderTargetSize.y);
		swapChainDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;				// This is the most common swap chain format.
		swapChainDesc.Stereo				= false;
		swapChainDesc.SampleDesc.Count		= 1;								// Don't use multi-sampling.
		swapChainDesc.SampleDesc.Quality	= 0;
		swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount			= 2;									// Use double-buffering to minimize latency.
		swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// All Microsoft Store apps must use this SwapEffect.
		swapChainDesc.Flags					= 0;
		swapChainDesc.Scaling				= scaling;
		swapChainDesc.AlphaMode				= DXGI_ALPHA_MODE_IGNORE;

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		ComPtr<IDXGIDevice3>					dxgiDevice;
		ComPtr<IDXGIAdapter>					dxgiAdapter;
		ComPtr<IDXGIFactory4>					dxgiFactory;
		ComPtr<IDXGISwapChain1>					swapChain;
		DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));
		DX::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));
		DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));
		DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForCoreWindow(m_d3dDevice.Get(), (IUnknown*)m_window.Get(), &swapChainDesc, nullptr, &swapChain));
		DX::ThrowIfFailed(swapChain.As(&m_swapChain));
		DX::ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));	// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and ensures that the application will only render after each VSync, minimizing power consumption.
	}
	switch (displayRotation) {
	case DXGI_MODE_ROTATION_IDENTITY	: m_orientationTransform3D = ZRotation0		; m_orientationTransform2D = Matrix3x2F::Identity(); break;
	case DXGI_MODE_ROTATION_ROTATE90	: m_orientationTransform3D = ZRotation270	; m_orientationTransform2D = Matrix3x2F::Rotation(90.0f) * Matrix3x2F::Translation(m_logicalSize.y, 0.0f); break;
	case DXGI_MODE_ROTATION_ROTATE180	: m_orientationTransform3D = ZRotation180	; m_orientationTransform2D = Matrix3x2F::Rotation(180.f) * Matrix3x2F::Translation(m_logicalSize.x, m_logicalSize.y); break;
	case DXGI_MODE_ROTATION_ROTATE270	: m_orientationTransform3D = ZRotation90	; m_orientationTransform2D = Matrix3x2F::Rotation(270.f) * Matrix3x2F::Translation(0.0f, m_logicalSize.x); break;
	default:
		throw ref new FailureException();
	}
	DX::ThrowIfFailed(m_swapChain->SetRotation(displayRotation));

	ComPtr<ID3D11Texture2D1>				backBuffer;
	DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
	DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView1(backBuffer.Get(), nullptr, &m_d3dRenderTargetView));	// Create a render target view of the swap chain back buffer.
	
	CD3D11_TEXTURE2D_DESC1					depthStencilDesc			(DXGI_FORMAT_D24_UNORM_S8_UINT, lround(m_d3dRenderTargetSize.x), lround(m_d3dRenderTargetSize.y), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	ComPtr<ID3D11Texture2D1>				depthStencil;
	CD3D11_DEPTH_STENCIL_VIEW_DESC			depthStencilViewDesc		(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D1(&depthStencilDesc, nullptr, &depthStencil));
	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &m_d3dDepthStencilView));		// Create a depth stencil view for use with 3D rendering if needed. This depth stencil view has only one texture. use a single mipmap level
	
	m_screenViewport					= CD3D11_VIEWPORT(0.0f, 0.0f, m_d3dRenderTargetSize.x, m_d3dRenderTargetSize.y);
	m_d3dContext->RSSetViewports(1, &m_screenViewport);	// Set the 3D rendering viewport to target the entire window.

	D2D1_BITMAP_PROPERTIES1					bitmapProperties			= D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), m_dpi, m_dpi);
	ComPtr<IDXGISurface2>					dxgiBackBuffer;
	DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)));
	DX::ThrowIfFailed(m_d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &bitmapProperties, &m_d2dTargetBitmap));	// Create a Direct2D target bitmap associated with the swap chain back buffer and set it as the current target.

	m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
	m_d2dContext->SetDpi(m_effectiveDpi, m_effectiveDpi);
	m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);	// Grayscale text anti-aliasing is recommended for all Microsoft Store apps.
}
