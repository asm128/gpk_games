#include "gpk_engine_rendermesh.h"
#include "gpk_engine_rendersurface.h"
#include "gpk_engine_renderbuffer.h"
#include "gpk_engine_scene.h"
#include "gpk_complus.h"

#include <d3d11_3.h>

#ifndef GPK_ENGINE_D3D_H_982734
#define GPK_ENGINE_D3D_H_982734

namespace gpk
{
	::gpk::error_t		d3dCreateBuffersFromEngineMeshes		(ID3D11Device* pDevice, const ::gpk::SMeshManager & engineMeshes, const ::gpk::SRenderBufferManager & engineBuffers, ::gpk::array_com<ID3D11Buffer> & indexBuffers, ::gpk::array_com<ID3D11Buffer> & vertexBuffers);
	::gpk::error_t		d3dCreateTexturesFromEngineSurfaces		(ID3D11Device* pDevice, const ::gpk::SSurfaceManager & engineSurfaces, ::gpk::array_com<ID3D11ShaderResourceView> & shaderResourceViews);
	//::gpk::error_t		d3dCreatePixelShadersFromEngineShaders	(ID3D11Device* pDevice, const ::gpk::SShaderManager & engineShaders, ::gpk::array_com<ID3D11PixelShader> & pixelShaders);
} // namespace

#endif // GPK_ENGINE_D3D_H_982734