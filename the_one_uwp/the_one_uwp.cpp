#include "the_one_uwp.h"

#include <DirectXColors.h>

using namespace the_one_uwp;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;


// Updates application state when the window size changes (e.g. device orientation change)
void the_one_uwp::STheOneUWP::CreateWindowSizeDependentResources() {
	SceneRenderer.CreateWindowSizeDependentResources();		// TODO: Replace this with the size-dependent initialization of your app's content.
}

// Updates the application state once per frame.
void the_one_uwp::STheOneUWP::Update() {
	// Update scene objects.
	Timer.Tick([&]() {
		SceneRenderer	.Update(Timer);	// Replace this with your app's content update functions.
		FpsTextRenderer	.Update(Timer);
	});
}

// Renders the current frame according to the current application state. Returns true if the frame was rendered and is ready to be displayed.
bool the_one_uwp::STheOneUWP::Render() {
	if (Timer.GetFrameCount() == 0)		// Don't try to render anything before the first Update.
		return false;

	auto								context				= DeviceResources->GetD3DDeviceContext();
	auto								viewport			= DeviceResources->GetScreenViewport();	// Reset the viewport to target the whole screen.
	context->RSSetViewports(1, &viewport);

	ID3D11RenderTargetView *			targets	[]			= { DeviceResources->GetBackBufferRenderTargetView(), };	// Reset render targets to the screen.
	context->OMSetRenderTargets(1, targets, DeviceResources->GetDepthStencilView());

	context->ClearRenderTargetView(DeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::DarkBlue);	// Clear the back buffer and depth stencil view.
	context->ClearDepthStencilView(DeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	

	// Render the scene objects. Replace this with your app's content rendering functions.
	SceneRenderer	.Render();
	FpsTextRenderer	.Render();

	return true;
}
