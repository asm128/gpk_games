#include "the_one_uwp.h"

#include "Common\DirectXHelper.h"

#include <DirectXColors.h>

using namespace the_one_uwp;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.

	// TODO: Replace this with your app's content initialization.
	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
//}

// Updates application state when the window size changes (e.g. device orientation change)
void STheOneUWP::CreateWindowSizeDependentResources() 
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	SceneRenderer.CreateWindowSizeDependentResources();
}

// Updates the application state once per frame.
void STheOneUWP::Update() 
{
	// Update scene objects.
	Timer.Tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		SceneRenderer.Update(Timer);
		FpsTextRenderer.Update(Timer);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool STheOneUWP::Render() 
{
	// Don't try to render anything before the first Update.
	if (Timer.GetFrameCount() == 0) {
		return false;
	}

	auto context = DeviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = DeviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { DeviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, DeviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(DeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::DarkBlue);
	context->ClearDepthStencilView(DeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	SceneRenderer	.Render();
	FpsTextRenderer	.Render();

	return true;
}
