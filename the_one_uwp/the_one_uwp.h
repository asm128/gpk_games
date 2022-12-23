#include "gpk_the_one.h"

#include "DeviceResources.h"
#include "Sample3DSceneRenderer.h"
#include "SampleFpsTextRenderer.h"
#include "StepTimer.h"

#include <DirectXColors.h>

#ifndef THE_ONE_UWPMAIN_H_98274
#define THE_ONE_UWPMAIN_H_98274

// Renders Direct2D and 3D content on the screen.
namespace the_one_uwp
{
	class STheOneUWP : public DX::IDeviceNotify {
		::the1::STheOne								TheOneApp			;
		::gpk::pobj<::gpk::SInput>					Input				;

		::gpk::pobj<::DX::D3DDeviceResources>		DeviceResources		;
		::the_one_uwp::Sample3DSceneRenderer		SceneRenderer		;
		::the_one_uwp::SampleFpsTextRenderer		FpsTextRenderer		;
		::DX::StepTimer								Timer				;
	public:
													~STheOneUWP								()	{ DeviceResources->RegisterDeviceNotify(nullptr); }

		::gpk::error_t								Initialize			(const ::gpk::pobj<DX::D3DDeviceResources>& deviceResources) {
			DeviceResources								= deviceResources;
			gpk_necs(SceneRenderer		.Initialize(deviceResources));
			gpk_necs(FpsTextRenderer	.Initialize(deviceResources));
			DeviceResources->RegisterDeviceNotify(this);
			return 0;
		}

		virtual void								OnDeviceLost							() {
			SceneRenderer	.ReleaseDeviceDependentResources();
			FpsTextRenderer	.ReleaseDeviceDependentResources();
		}

		virtual ::gpk::error_t						OnDeviceRestored						() {
			gpk_necs(SceneRenderer	.CreateDeviceDependentResources());
			gpk_necs(FpsTextRenderer.CreateDeviceDependentResources());
			CreateWindowSizeDependentResources();		// TODO: Replace this with the size-dependent initialization of your app's content.
			return 0;
		}

		void										CreateWindowSizeDependentResources		() { SceneRenderer.CreateWindowSizeDependentResources(); }

		// Renders the current frame according to the current application state. Returns true if the frame was rendered and is ready to be displayed.
		bool										Render									() {
			if (Timer.GetFrameCount() == 0)		// Don't try to render anything before the first Update.
				return false;

			auto											context									= DeviceResources->GetD3DDeviceContext();
			auto											viewport								= DeviceResources->GetScreenViewport();	// Reset the viewport to target the whole screen.
			context->RSSetViewports(1, &viewport);

			ID3D11RenderTargetView *						targets	[]								= { DeviceResources->GetBackBufferRenderTargetView(), };	// Reset render targets to the screen.
			context->OMSetRenderTargets(1, targets, DeviceResources->GetDepthStencilView());

			context->ClearRenderTargetView(DeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::DarkBlue);	// Clear the back buffer and depth stencil view.
			context->ClearDepthStencilView(DeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	

			// Render the scene objects. Replace this with your app's content rendering functions.
			SceneRenderer	.Render();
			FpsTextRenderer	.Render();

			return true;
		}

		void										Update									() {
			if(!Input)
				Input.create();

			// Update scene objects.
			Timer.Tick([&]() {
				::gpk::array_obj<::gpk::SSysEvent>				sysEvents;
				::gpk::SSysEvent								newEvent;

				Input->KeyboardPrevious						= Input->KeyboardCurrent;
				Input->MousePrevious						= Input->MouseCurrent;
				Input->KeyboardCurrent						= {};
				Input->MouseCurrent							= {};
				for(uint32_t iVK = 0; iVK < 256; ++iVK) {
					if(iVK == 1 || iVK == 2 || iVK == 4) {
						Input->MouseCurrent.ButtonState[iVK - 1]	= (DeviceResources->m_window->GetAsyncKeyState((Windows::System::VirtualKey)iVK) == Windows::UI::Core::CoreVirtualKeyStates(0))? 0 : 1;
						if(Input->MouseCurrent.ButtonState[iVK - 1] && !Input->MousePrevious.ButtonState[iVK - 1]) {
							newEvent.Type								= ::gpk::SYSEVENT_MOUSE_DOWN; 
							newEvent.Data.resize(sizeof(WPARAM)); 
							*(WPARAM*)&newEvent.Data[0]					= iVK - 1; 
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
						Input->KeyboardCurrent.KeyState[iVK]		= (DeviceResources->m_window->GetAsyncKeyState((Windows::System::VirtualKey)iVK) == Windows::UI::Core::CoreVirtualKeyStates(0))? 0 : 1;
						if(Input->KeyboardCurrent.KeyState[iVK] && !Input->KeyboardPrevious.KeyState[iVK]) {
							newEvent.Type								= ::gpk::SYSEVENT_KEY_DOWN; 
							newEvent.Data.resize(sizeof(WPARAM)); 
							*(WPARAM*)&newEvent.Data[0]					= iVK; 
							sysEvents.push_back(newEvent);
						}
						else if(!Input->KeyboardCurrent.KeyState[iVK] && Input->KeyboardPrevious.KeyState[iVK]) {
							newEvent.Type								= ::gpk::SYSEVENT_KEY_UP; 
							newEvent.Data.resize(sizeof(WPARAM)); 
							*(WPARAM*)&newEvent.Data[0]					= iVK; 
							sysEvents.push_back(newEvent);
						}
					}
				}
				::the1::theOneUpdate(this->TheOneApp, Timer.GetElapsedSeconds(), Input, sysEvents);

				SceneRenderer	.Update(Timer.GetElapsedSeconds());	// Replace this with your app's content update functions.
				FpsTextRenderer	.Update(Timer.GetElapsedSeconds(), Timer.GetTotalSeconds(), Timer.GetFramesPerSecond());
			});
		}

	};
}

#endif