#include "Sample3DSceneRenderer.h"

using namespace the_one_uwp;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources() {
	Size									outputSize				= m_deviceResources->GetOutputSize();
	float									aspectRatio				= outputSize.Width / outputSize.Height;
	float									fovAngleY				= 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in portrait or snapped view.
	if (aspectRatio < 1.0f)
		fovAngleY							*= 2.0f;

	// Note that the OrientationTransform3D matrix is post-multiplied here in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are made to the swap chain render target. For draw calls to other targets, this transform should not be applied.
	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX								perspectiveMatrix		= XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, 0.01f, 100.0f);
	XMFLOAT4X4								orientation				= m_deviceResources->GetOrientationTransform3D();
	XMMATRIX								orientationMatrix		= XMLoadFloat4x4(&orientation);
	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32				eye						= { 0.0f, 0.7f, 1.5f, 0.0f };
	static const XMVECTORF32				at						= { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32				up						= { 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

void Sample3DSceneRenderer::CreateDeviceDependentResources() {
	// Load shaders asynchronously.
	Concurrency::task<std::vector<byte>>	loadVSTask				= DX::ReadDataAsync(L"SampleVertexShader.cso");
	Concurrency::task<std::vector<byte>>	loadPSTask				= DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	Concurrency::task<void>					createVSTask			= loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_vertexShader));
		static const D3D11_INPUT_ELEMENT_DESC	vertexDesc []			=
			{	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 00, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			,	{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_inputLayout));
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	Concurrency::task<void>					createPSTask			= loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_pixelShader));
		CD3D11_BUFFER_DESC						constantBufferDesc		(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer));
	});

	// Once both shaders are loaded, create the mesh.
	Concurrency::task<void>					createCubeTask			= (createPSTask && createVSTask).then([this] () {
		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor		cubeVertices[]			=
			{ {XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)}
			, {XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)}
			, {XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)}
			, {XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)}
			, {XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)}
			, {XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)}
			, {XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)}
			, {XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)}
			};

		D3D11_SUBRESOURCE_DATA					vertexBufferData		= {};
		vertexBufferData.pSysMem			= cubeVertices;
		vertexBufferData.SysMemPitch		= 0;
		vertexBufferData.SysMemSlicePitch	= 0;
		CD3D11_BUFFER_DESC						vertexBufferDesc		(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));

		// Load mesh indices. Each trio of indices represents a triangle to be rendered on the screen. For example: 0,2,1 means that the vertices with indexes 0, 2 and 1 from the vertex buffer compose the first triangle of this mesh.
		static const uint16_t					cubeIndices []			= 
			{ 0,2,1, 1,2,3	// -x
			, 4,5,6, 5,7,6	// +x
			, 0,1,5, 0,5,4	// -y
			, 2,6,7, 2,7,3	// +y
			, 0,4,6, 0,6,2	// -z
			, 1,3,7, 1,7,5	// +z
			};

		m_indexCount						= ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA					indexBufferData			= {};
		indexBufferData.pSysMem				= cubeIndices;
		indexBufferData.SysMemPitch			= 0;
		indexBufferData.SysMemSlicePitch	= 0;
		CD3D11_BUFFER_DESC						indexBufferDesc			(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));
	});

	createCubeTask.then([this] () { m_loadingComplete = true; });		// Once the cube is loaded, the object is ready to be rendered.
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width. Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render() {
	if (!m_loadingComplete)	// Loading is asynchronous. Only draw geometry after it's loaded.
		return;

	auto									context					= m_deviceResources->GetD3DDeviceContext();
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);	// Prepare the constant buffer to send it to the graphics device.

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT									stride					= sizeof(VertexPositionColor);
	UINT									offset					= 0;
	context->IASetVertexBuffers		(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer		(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);// Each index is one 16-bit unsigned integer (short).
	context->IASetPrimitiveTopology	(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout		(m_inputLayout.Get());
	context->VSSetShader			(m_vertexShader.Get(), nullptr, 0);	// Attach our vertex shader.
	context->VSSetConstantBuffers1	(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);	// Send the constant buffer to the graphics device.
	context->PSSetShader			(m_pixelShader.Get(), nullptr, 0);	// Attach our pixel shader.
	context->DrawIndexed			(m_indexCount, 0, 0);	// Draw the objects.
}
