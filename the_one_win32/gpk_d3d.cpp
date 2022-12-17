#include "gpk_d3d.h"

#include <dxgi1_4.h>

::gpk::error_t				gpk::d3dDeviceValidate				(::gpk::ptr_com<::ID3D11Device3> & d3dDevice)				{
	// The D3D Device is no longer valid if the default adapter changed since the device was created or if the device has been removed. 
	::gpk::ptr_com<IDXGIDevice3>		dxgiDevice;
	::gpk::ptr_com<IDXGIAdapter>		deviceAdapter;
	::gpk::ptr_com<IDXGIFactory4>		deviceFactory;
	::gpk::ptr_com<IDXGIAdapter1>		previousDefaultAdapter;
	::gpk::ptr_com<IDXGIFactory4>		currentFactory;
	::gpk::ptr_com<IDXGIAdapter1>		currentDefaultAdapter;
	DXGI_ADAPTER_DESC1						previousDesc				= {};	// First, get the information for the default adapter from when the device was created.
	DXGI_ADAPTER_DESC1						currentDesc					= {};	// Next, get the information for the current default adapter.

	d3dDevice.as(dxgiDevice);
	gpk_hrcall(dxgiDevice->GetAdapter(&deviceAdapter));
	gpk_hrcall(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));
	gpk_hrcall(deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));
	gpk_hrcall(previousDefaultAdapter->GetDesc1(&previousDesc));

	gpk_hrcall(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));
	gpk_hrcall(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));
	gpk_hrcall(currentDefaultAdapter->GetDesc1(&currentDesc));

	// If the adapter LUIDs don't match, or if the device reports that it has been removed, a new D3D device must be created. Release references to resources related to the old device.
	if (0 == memcmp(&previousDesc.AdapterLuid, &currentDesc.AdapterLuid, sizeof(LUID)) && !FAILED(d3dDevice->GetDeviceRemovedReason())) 
		return 0;

	return 1;
}
