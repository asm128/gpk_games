#include "the_one_uwp.h"

#include <DirectXColors.h>

using namespace the_one_uwp;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// Updates the application state once per frame.
void the_one_uwp::STheOneUWP::Update() {
	if(!Input)
		Input.create();

	// Update scene objects.
	Timer.Tick([&]() {
		::gpk::array_obj<::gpk::SSysEvent>			sysEvents;
		::gpk::SSysEvent							newEvent;

		Input->KeyboardPrevious					= Input->KeyboardCurrent;
		Input->MousePrevious					= Input->MouseCurrent;
		Input->KeyboardCurrent					= {};
		Input->MouseCurrent						= {};
		for(uint32_t iVK = 0; iVK < 256; ++iVK) {
			if(iVK == 1 || iVK == 2 || iVK == 4) {
				Input->MouseCurrent.ButtonState[iVK - 1]	= (DeviceResources->m_window->GetAsyncKeyState((Windows::System::VirtualKey)iVK) == Windows::UI::Core::CoreVirtualKeyStates(0))? 0 : 1;
				if(Input->MouseCurrent.ButtonState[iVK - 1] && !Input->MousePrevious.ButtonState[iVK - 1]) {
					newEvent.Type = ::gpk::SYSEVENT_MOUSE_DOWN; 
					newEvent.Data.resize(sizeof(WPARAM)); 
					*(WPARAM*)&newEvent.Data[0] = iVK - 1; 
					sysEvents.push_back(newEvent);
				}
				else if(!Input->MouseCurrent.ButtonState[iVK - 1] && Input->MousePrevious.ButtonState[iVK - 1]) {
					newEvent.Type = ::gpk::SYSEVENT_MOUSE_UP; 
					newEvent.Data.resize(sizeof(WPARAM)); 
					*(WPARAM*)&newEvent.Data[0] = iVK - 1; 
					sysEvents.push_back(newEvent);
				}
			}
			else {
				Input->KeyboardCurrent.KeyState[iVK]	= (DeviceResources->m_window->GetAsyncKeyState((Windows::System::VirtualKey)iVK) == Windows::UI::Core::CoreVirtualKeyStates(0))? 0 : 1;
				if(Input->KeyboardCurrent.KeyState[iVK] && !Input->KeyboardPrevious.KeyState[iVK]) {
					newEvent.Type = ::gpk::SYSEVENT_KEY_DOWN; 
					newEvent.Data.resize(sizeof(WPARAM)); 
					*(WPARAM*)&newEvent.Data[0] = iVK; 
					sysEvents.push_back(newEvent);
				}
				else if(!Input->KeyboardCurrent.KeyState[iVK] && Input->KeyboardPrevious.KeyState[iVK]) {
					newEvent.Type = ::gpk::SYSEVENT_KEY_UP; 
					newEvent.Data.resize(sizeof(WPARAM)); 
					*(WPARAM*)&newEvent.Data[0] = iVK; 
					sysEvents.push_back(newEvent);
				}
			}
		}
		::the1::theOneUpdate(this->TheOneApp, Timer.GetElapsedSeconds(), Input, sysEvents);

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
