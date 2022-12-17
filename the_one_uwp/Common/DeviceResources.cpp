#include "DeviceResources.h"

using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Platform;

// Constructor for DeviceResources. Configures resources that don't depend on the Direct3D device.
void DX::DeviceResources::CreateDeviceIndependentResources() {
	D2D1_FACTORY_OPTIONS				options							= {};	// Initialize Direct2D resources.
#if defined(_DEBUG)
	options.debugLevel				= D2D1_DEBUG_LEVEL_INFORMATION;		// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
#endif
	DX::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &options, &m_d2dFactory));	// Initialize the Direct2D Factory.
	DX::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), &m_dwriteFactory));			// Initialize the DirectWrite Factory.
	DX::ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_wicFactory)));	// Initialize the Windows Imaging Component (WIC) Factory.
}

// Configures the Direct3D device, and stores handles to it and the device context.
void DX::DeviceResources::CreateDeviceResources() {
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

	HRESULT								hr								= D3D11CreateDevice
		( nullptr								// Specify nullptr to use the default adapter.
		, D3D_DRIVER_TYPE_HARDWARE				// Create a device using the hardware graphics driver.
		, 0										// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		, creationFlags							// Set debug and Direct2D compatibility flags.
		, featureLevels							// List of feature levels this app can support.
		, (uint32_t)std::size(featureLevels)	// Size of the list above.
		, D3D11_SDK_VERSION						// Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
		, &device								// Returns the Direct3D device created.
		, &m_d3dFeatureLevel					// Returns feature level of device created.
		, &context								// Returns the device immediate context.
		);

	if (FAILED(hr)) // If the initialization fails, fall back to the WARP device. For more information on WARP, see: https://go.microsoft.com/fwlink/?LinkId=286690. 
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
void DX::DeviceResources::CreateWindowSizeDependentResources() {
	// Clear the previous window size specific context.
	ID3D11RenderTargetView				* nullViews[]					= {nullptr};
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
	m_d3dRenderTargetSize.Width		= swapDimensions ? m_outputSize.Height : m_outputSize.Width;
	m_d3dRenderTargetSize.Height	= swapDimensions ? m_outputSize.Width : m_outputSize.Height;

	if (m_swapChain) { // If the swap chain already exists, resize it.
		HRESULT								hr								= m_swapChain->ResizeBuffers(2, lround(m_d3dRenderTargetSize.Width), lround(m_d3dRenderTargetSize.Height), DXGI_FORMAT_B8G8R8A8_UNORM, 0); // Double-buffered swap chain.
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

		swapChainDesc.Width					= lround(m_d3dRenderTargetSize.Width);		// Match the size of the window.
		swapChainDesc.Height				= lround(m_d3dRenderTargetSize.Height);
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
	// Set the proper orientation for the swap chain, and generate 2D and 3D matrix transformations for rendering to the rotated swap chain. Note the rotation angle for the 2D and 3D transforms are different.
	// This is due to the difference in coordinate spaces. Additionally, the 3D matrix is specified explicitly to avoid rounding errors.
	static constexpr XMFLOAT4X4 ZRotation0
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
	switch (displayRotation) {
	case DXGI_MODE_ROTATION_IDENTITY	: m_orientationTransform3D = ZRotation0		; m_orientationTransform2D = Matrix3x2F::Identity(); break;
	case DXGI_MODE_ROTATION_ROTATE90	: m_orientationTransform3D = ZRotation270	; m_orientationTransform2D = Matrix3x2F::Rotation(90.0f) * Matrix3x2F::Translation(m_logicalSize.Height, 0.0f); break;
	case DXGI_MODE_ROTATION_ROTATE180	: m_orientationTransform3D = ZRotation180	; m_orientationTransform2D = Matrix3x2F::Rotation(180.f) * Matrix3x2F::Translation(m_logicalSize.Width, m_logicalSize.Height); break;
	case DXGI_MODE_ROTATION_ROTATE270	: m_orientationTransform3D = ZRotation90	; m_orientationTransform2D = Matrix3x2F::Rotation(270.f) * Matrix3x2F::Translation(0.0f, m_logicalSize.Width); break;
	default:
		throw ref new FailureException();
	}
	DX::ThrowIfFailed(m_swapChain->SetRotation(displayRotation));

	ComPtr<ID3D11Texture2D1>				backBuffer;
	DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
	DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView1(backBuffer.Get(), nullptr, &m_d3dRenderTargetView));	// Create a render target view of the swap chain back buffer.
	
	CD3D11_TEXTURE2D_DESC1					depthStencilDesc			(DXGI_FORMAT_D24_UNORM_S8_UINT, lround(m_d3dRenderTargetSize.Width), lround(m_d3dRenderTargetSize.Height), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	ComPtr<ID3D11Texture2D1>				depthStencil;
	CD3D11_DEPTH_STENCIL_VIEW_DESC			depthStencilViewDesc		(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D1(&depthStencilDesc, nullptr, &depthStencil));
	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &m_d3dDepthStencilView));		// Create a depth stencil view for use with 3D rendering if needed. This depth stencil view has only one texture. use a single mipmap level
	
	m_screenViewport					= CD3D11_VIEWPORT(0.0f, 0.0f, m_d3dRenderTargetSize.Width, m_d3dRenderTargetSize.Height);
	m_d3dContext->RSSetViewports(1, &m_screenViewport);	// Set the 3D rendering viewport to target the entire window.

	D2D1_BITMAP_PROPERTIES1					bitmapProperties			= D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), m_dpi, m_dpi);
	ComPtr<IDXGISurface2>					dxgiBackBuffer;
	DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)));
	DX::ThrowIfFailed(m_d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &bitmapProperties, &m_d2dTargetBitmap));	// Create a Direct2D target bitmap associated with the swap chain back buffer and set it as the current target.

	m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
	m_d2dContext->SetDpi(m_effectiveDpi, m_effectiveDpi);
	m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);	// Grayscale text anti-aliasing is recommended for all Microsoft Store apps.
}

static int32_t validationFailed(Microsoft::WRL::ComPtr<ID3D11Device3> d3dDevice) {
	// The D3D Device is no longer valid if the default adapter changed since the device was created or if the device has been removed. 
	Microsoft::WRL::ComPtr<IDXGIDevice3>	dxgiDevice;
	Microsoft::WRL::ComPtr<IDXGIAdapter>	deviceAdapter;
	Microsoft::WRL::ComPtr<IDXGIFactory4>	deviceFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter1>	previousDefaultAdapter;
	Microsoft::WRL::ComPtr<IDXGIFactory4>	currentFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter1>	currentDefaultAdapter;
	DXGI_ADAPTER_DESC1						previousDesc				= {};	// First, get the information for the default adapter from when the device was created.
	DXGI_ADAPTER_DESC1						currentDesc					= {};	// Next, get the information for the current default adapter.

	DX::ThrowIfFailed(d3dDevice.As(&dxgiDevice));
	DX::ThrowIfFailed(dxgiDevice->GetAdapter(&deviceAdapter));
	DX::ThrowIfFailed(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));
	DX::ThrowIfFailed(deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));
	DX::ThrowIfFailed(previousDefaultAdapter->GetDesc1(&previousDesc));

	DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));
	DX::ThrowIfFailed(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));
	DX::ThrowIfFailed(currentDefaultAdapter->GetDesc1(&currentDesc));

	if (0 == memcmp(&previousDesc.AdapterLuid, &currentDesc.AdapterLuid, sizeof(LUID)) && !FAILED(d3dDevice->GetDeviceRemovedReason())) 
		return 0;

	return -1;
}

// This method is called in the event handler for the DisplayContentsInvalidated event.
void DX::DeviceResources::ValidateDevice() { 
	// If the adapter LUIDs don't match, or if the device reports that it has been removed, a new D3D device must be created. Release references to resources related to the old device.
	if(validationFailed(m_d3dDevice))
		HandleDeviceLost();	// Create a new device and swap chain.
}

// This method determines the rotation between the display device's native orientation and the
// current display orientation.
DXGI_MODE_ROTATION DX::DeviceResources::ComputeDisplayRotation() {
	DXGI_MODE_ROTATION						rotation					= DXGI_MODE_ROTATION_UNSPECIFIED;	// Note: NativeOrientation can only be Landscape or Portrait even though the DisplayOrientations enum has other values.
	switch (m_nativeOrientation) {
	case DisplayOrientations::Landscape:
		switch (m_currentOrientation) {
		case DisplayOrientations::Landscape			: rotation = DXGI_MODE_ROTATION_IDENTITY; break; 
		case DisplayOrientations::Portrait			: rotation = DXGI_MODE_ROTATION_ROTATE270; break; 
		case DisplayOrientations::LandscapeFlipped	: rotation = DXGI_MODE_ROTATION_ROTATE180; break; 
		case DisplayOrientations::PortraitFlipped	: rotation = DXGI_MODE_ROTATION_ROTATE90; break; 
		}
		break;
	case DisplayOrientations::Portrait:
		switch (m_currentOrientation) {
		case DisplayOrientations::Landscape			: rotation = DXGI_MODE_ROTATION_ROTATE90; break;
		case DisplayOrientations::Portrait			: rotation = DXGI_MODE_ROTATION_IDENTITY; break; 
		case DisplayOrientations::LandscapeFlipped	: rotation = DXGI_MODE_ROTATION_ROTATE270; break; 
		case DisplayOrientations::PortraitFlipped	: rotation = DXGI_MODE_ROTATION_ROTATE180; break;
		}
		break;
	}
	return rotation;
}