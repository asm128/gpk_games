#include "the_one_uwp.h"

#include "Common\DeviceResources.h"

#include <ppltasks.h>

#ifndef APP_H_BLABLA_UWP_SUCKS_MICROSOFT_CANT_CODE
#define APP_H_BLABLA_UWP_SUCKS_MICROSOFT_CANT_CODE

namespace the_one_uwp
{
	// Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
	ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView {
		std::shared_ptr<DX::DeviceResources>			DeviceResources			= {};
		std::unique_ptr<STheOneUWP>						UserUWP					= {};
		bool											WindowClosed			= false;
		bool											WindowVisible			= true;
	//private:
	protected:
		// Application lifecycle event handlers.
		void			OnResuming						(Platform::Object^ sender, Platform::Object^ args)					{}
		void			OnActivated						(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args)	{ Windows::UI::Core::CoreWindow::GetForCurrentThread()->Activate(); }
		void			OnSuspending					(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args) {
			Windows::ApplicationModel::SuspendingDeferral	^ deferral						= args->SuspendingOperation->GetDeferral();
			::concurrency::create_task([this, deferral]() {
				DeviceResources->Trim();
				// Insert your code here.
				deferral->Complete();
			});
		}

		// Window event handlers.
		void			OnWindowSizeChanged				(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args)	{ DeviceResources->SetLogicalSize(Windows::Foundation::Size(sender->Bounds.Width, sender->Bounds.Height)); UserUWP->CreateWindowSizeDependentResources(); }
		void			OnVisibilityChanged				(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args)	{ WindowVisible = args->Visible; }
		void			OnWindowClosed					(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args)			{ WindowClosed = true; }

		// DisplayInformation event handlers.
		void			OnDpiChanged					(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args)				{ DeviceResources->SetDpi(sender->LogicalDpi); UserUWP->CreateWindowSizeDependentResources(); }
		void			OnOrientationChanged			(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args)				{ DeviceResources->SetCurrentOrientation(sender->CurrentOrientation); UserUWP->CreateWindowSizeDependentResources(); }
		void			OnDisplayContentsInvalidated	(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args)				{ DeviceResources->ValidateDevice(); }

		
		// Application lifecycle event handlers.

	public:
		// IFrameworkView Methods.
		virtual void	Uninitialize					()																	{}
		virtual void	Load							(Platform::String^ entryPoint)										{
			if (UserUWP == nullptr) {
				UserUWP			= std::unique_ptr<STheOneUWP>(new STheOneUWP(DeviceResources));
			}
		}

		// The first method called when the IFrameworkView is being created.
		virtual void	Initialize						(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView) {
			// Register event handlers for app lifecycle. This example includes Activated, so that we can make the CoreWindow active and start rendering on the window.
			applicationView->Activated						+= ref new Windows::Foundation::TypedEventHandler<Windows::ApplicationModel::Core::CoreApplicationView^, Windows::ApplicationModel::Activation::IActivatedEventArgs^>	(this, &App::OnActivated	);
			Windows::ApplicationModel::Core::CoreApplication::Suspending	+= ref new Windows::Foundation::EventHandler<Windows::ApplicationModel::SuspendingEventArgs^>	(this, &App::OnSuspending	);
			Windows::ApplicationModel::Core::CoreApplication::Resuming		+= ref new Windows::Foundation::EventHandler<Platform::Object^>		(this, &App::OnResuming		);
			DeviceResources								= std::make_shared<DX::DeviceResources>();		// At this point we have access to the device. We can create the device-dependent resources.
		}

		virtual void	SetWindow						(Windows::UI::Core::CoreWindow^ window) {
			window->SizeChanged								+= ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);
			window->VisibilityChanged						+= ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);
			window->Closed									+= ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::CoreWindowEventArgs^>		(this, &App::OnWindowClosed);

			Windows::Graphics::Display::DisplayInformation		^ currentDisplayInformation			= Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

			currentDisplayInformation->DpiChanged			+= ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation^, Platform::Object^>(this, &App::OnDpiChanged);
			currentDisplayInformation->OrientationChanged	+= ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation^, Platform::Object^>(this, &App::OnOrientationChanged);
			Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated	+= ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation^, Platform::Object^>(this, &App::OnDisplayContentsInvalidated);

			DeviceResources->SetWindow(window);
		}

		virtual void	Run								() { // Initializes scene resources, or loads a previously saved app state. This method is called after the window becomes active.
			while (!WindowClosed) {
				if (!WindowVisible)
				Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessOneAndAllPending);
				else {
					Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);
					UserUWP->Update();
					if (UserUWP->Render())
						DeviceResources->Present();
				}
			}
		}
	};
}
#endif // APP_H_BLABLA_UWP_SUCKS_MICROSOFT_CANT_CODE