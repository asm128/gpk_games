#include "gpk_d3d.h"

#include <dxgi1_4.h>

::gpk::error_t				gpk::d3dDeviceValidate				(Microsoft::WRL::ComPtr<::ID3D11Device3> & d3dDevice)				{
	// The D3D Device is no longer valid if the default adapter changed since the device was created or if the device has been removed. 
	Microsoft::WRL::ComPtr<IDXGIDevice3>	dxgiDevice;
	Microsoft::WRL::ComPtr<IDXGIAdapter>	deviceAdapter;
	Microsoft::WRL::ComPtr<IDXGIFactory4>	deviceFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter1>	previousDefaultAdapter;
	Microsoft::WRL::ComPtr<IDXGIFactory4>	currentFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter1>	currentDefaultAdapter;
	DXGI_ADAPTER_DESC1						previousDesc				= {};	// First, get the information for the default adapter from when the device was created.
	DXGI_ADAPTER_DESC1						currentDesc					= {};	// Next, get the information for the current default adapter.

	d3dDevice.As(&dxgiDevice);
	gpk_hrcall(dxgiDevice->GetAdapter(&deviceAdapter));
	gpk_hrcall(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));
	gpk_hrcall(deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));
	gpk_hrcall(previousDefaultAdapter->GetDesc1(&previousDesc));

	gpk_hrcall(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));
	gpk_hrcall(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));
	gpk_hrcall(currentDefaultAdapter->GetDesc1(&currentDesc));

	if (0 == memcmp(&previousDesc.AdapterLuid, &currentDesc.AdapterLuid, sizeof(LUID)) && !FAILED(d3dDevice->GetDeviceRemovedReason())) 
		return 0;

	return -1;
}
