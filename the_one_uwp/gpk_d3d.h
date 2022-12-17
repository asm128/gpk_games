#include "gpk_windows.h"

#include <d3d11_3.h>
#include <wrl.h>

#ifndef GPK_D3D_H_982734
#define GPK_D3D_H_982734

namespace gpk
{
	::gpk::error_t				d3dDeviceValidate				(Microsoft::WRL::ComPtr<::ID3D11Device3> & d3dDevice);
} // namespace

#endif // GPK_D3D_H_982734