#include "gpk_engine_rendermesh.h"
#include "gpk_engine_rendersurface.h"
#include "gpk_engine_renderbuffer.h"
#include "gpk_complus.h"

#include <d3d11_3.h>

#ifndef GPK_ENGINE_D3D_H_982734
#define GPK_ENGINE_D3D_H_982734

namespace gpk
{
	::gpk::error_t		d3dCreateBuffersFromEngineMeshes		(const ::gpk::SMeshManager & engineMeshes, const ::gpk::SRenderBufferManager & engineBuffers, ID3D11Device* pDevice, ::gpk::array_com<ID3D11Buffer> & indexBuffers, ::gpk::array_com<ID3D11Buffer> & vertexBuffers);
	::gpk::error_t		d3dCreateTexturesFromEngineSurfaces		(const ::gpk::SSurfaceManager & engineSurfaces, ID3D11Device* pDevice, ::gpk::array_com<ID3D11ShaderResourceView> & shaderResourceViews);
} // namespace

#endif // GPK_ENGINE_D3D_H_982734