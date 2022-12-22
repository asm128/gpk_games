#include "gpk_engine_d3d.h"
#include "gpk_storage.h"

::gpk::error_t								gpk::d3dCreateBuffersFromEngineMeshes		(ID3D11Device* pDevice, const ::gpk::SMeshManager & engineMeshes, const ::gpk::SRenderBufferManager & engineBuffers, ::gpk::array_com<ID3D11Buffer> & indexBuffers, ::gpk::array_com<ID3D11Buffer> & vertexBuffers)	{
	for(uint32_t iMesh = 0; iMesh < engineMeshes.size(); ++iMesh) {
		const ::gpk::ptr_obj<::gpk::SGeometryMesh>		& mesh									= engineMeshes[iMesh];
		if(!mesh)
			continue;

		::gpk::ptr_obj<::gpk::SRenderBuffer>			engineBufferIndices;
		::gpk::array_pobj<::gpk::SRenderBuffer>			engineBufferVertices;
		::gpk::array_pod<uint32_t>						layoutOffsets;

		uint32_t										vertexSize					= 0;
		for(uint32_t iBuffer = 0; iBuffer < mesh->GeometryBuffers.size(); ++iBuffer) {
			const ::gpk::ptr_obj<::gpk::SRenderBuffer>		& buffer								= engineBuffers[mesh->GeometryBuffers[iBuffer]];
			if(buffer->Desc.Usage == ::gpk::BUFFER_USAGE_Index) {
				engineBufferIndices							= buffer;
				continue;
			}
			gpk_necs(engineBufferVertices.push_back(buffer));
			gpk_necs(layoutOffsets.push_back(vertexSize));
			vertexSize									+= buffer->Desc.Format.TotalBytes();

		}
		::gpk::array_pod<ubyte_t>						packed;
		for(uint32_t iValue = 0, valueCount = engineBufferVertices[0]->Data.size() / engineBufferVertices[0]->Desc.Format.TotalBytes(); iValue < valueCount; ++iValue) {
			for(uint32_t iBuffer = 0; iBuffer < engineBufferVertices.size(); ++iBuffer) {
				const ::gpk::SRenderBuffer						& buffer					= *engineBufferVertices[iBuffer];
				::gpk::view_const_ubyte							value						= {&buffer.Data[iValue * buffer.Desc.Format.TotalBytes()], buffer.Desc.Format.TotalBytes()};
				packed.append(value);
			}
		}
		{
			D3D11_SUBRESOURCE_DATA							vertexBufferData			= {packed.begin()};
			D3D11_BUFFER_DESC								vertexBufferDesc			= {packed.size()};
			vertexBufferDesc.BindFlags					= D3D11_BIND_VERTEX_BUFFER;
			::gpk::ptr_com<ID3D11Buffer>					d3dBuffer;
			gpk_hrcall(pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d3dBuffer));
			vertexBuffers.push_back(d3dBuffer);
		}
		{
			D3D11_SUBRESOURCE_DATA							indexBufferData			= {engineBufferIndices->Data.begin()};
			D3D11_BUFFER_DESC								indexBufferDesc			= {engineBufferIndices->Data.size()};
			indexBufferDesc.BindFlags					= D3D11_BIND_INDEX_BUFFER;
			::gpk::ptr_com<ID3D11Buffer>					d3dBuffer;
			gpk_hrcall(pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &d3dBuffer));
			indexBuffers.push_back(d3dBuffer);
		}
	}

	return 0;
}

::gpk::error_t								gpk::d3dCreateTexturesFromEngineSurfaces	(ID3D11Device* pDevice, const ::gpk::SSurfaceManager & engineSurfaces, ::gpk::array_com<ID3D11ShaderResourceView> & shaderResourceViews) {
	for(uint32_t iSurface = 0; iSurface < engineSurfaces.size(); ++iSurface) {
		const ::gpk::SSurface						& surface			= *engineSurfaces[iSurface];
		D3D11_TEXTURE2D_DESC						desc				= {surface.Desc.Dimensions.x, surface.Desc.Dimensions.y, 1, 1, DXGI_FORMAT_B8G8R8A8_UNORM, {1, 0}, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE};
		D3D11_SUBRESOURCE_DATA						initData			= {};
		initData.pSysMem						= surface.Data.begin();
		initData.SysMemPitch					= surface.Desc.Dimensions.x * 4;

		::gpk::ptr_com<ID3D11Texture2D>				pTex2D				= {};
		gpk_hrcall(pDevice->CreateTexture2D(&desc, &initData, &pTex2D));
		if(!pTex2D) 
			continue;

		D3D11_SHADER_RESOURCE_VIEW_DESC				srvDesc				= {};
		srvDesc.Format							= desc.Format;
		srvDesc.ViewDimension					= D3D_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels				= desc.MipLevels;

		::gpk::ptr_com<ID3D11ShaderResourceView>	pSRV;
		gpk_hrcall(pDevice->CreateShaderResourceView(pTex2D, &srvDesc, &pSRV));
		shaderResourceViews.push_back(pSRV);
	}
	return 0;
}

//
::gpk::error_t								gpk::d3dCreatePixelShadersFromEngineShaders	(ID3D11Device* pDevice, const ::gpk::SShaderManager & engineShaders, ::gpk::array_com<ID3D11PixelShader> & pixelShaders) {
	for(uint32_t iShader = 0; iShader < engineShaders.size(); ++iShader) {
		const ::gpk::vcc								shaderName				= engineShaders.Names[iShader];

		// After the pixel shader file is loaded, create the shader.
		char											shaderFileName	[1024]	= {};
		sprintf_s(shaderFileName, "%s.cso", shaderName.begin());
		::gpk::array_pod<byte_t>						filePS;
		gpk_necs(::gpk::fileToMemory(::gpk::vcs{shaderFileName}, filePS));

		::gpk::ptr_com<ID3D11PixelShader>				pixelShader;
		gpk_hrcall(pDevice->CreatePixelShader(&filePS[0], filePS.size(), nullptr, &pixelShader));
		pixelShaders.push_back(pixelShader);
	}
	return 0;
}