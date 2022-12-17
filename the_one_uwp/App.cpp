#include "the_one_uwp.h"

#include "DeviceResources.h"

#include <ppltasks.h>

using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Graphics::Display;
using namespace Platform;

namespace the_one_uwp
{
	// Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
	ref class App sealed : public IFrameworkView {
		std::shared_ptr<DX::D3DDeviceResources>			DeviceResources			= {};
		std::unique_ptr<STheOneUWP>						UserUWP					= {};
		bool											WindowClosed			= false;
		bool											WindowVisible			= true;
	//private:
	protected:
		// Application lifecycle event handlers.
		void			OnActivated						(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)	{ CoreWindow::GetForCurrentThread()->Activate(); }
		void			OnResuming						(Object^ sender, Object^ args)										{}	// Initializes scene resources, or loads a previously saved app state. This method is called after the window becomes active.
		void			OnSuspending					(Object^ sender, SuspendingEventArgs^ args)							{
			SuspendingDeferral	^ deferral						= args->SuspendingOperation->GetDeferral();
			::concurrency::create_task([this, deferral]() {
				DeviceResources->Trim();
				// Insert your code here.
				deferral->Complete();
			});
		}

		// Window event handlers.
		void			OnWindowSizeChanged				(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)				{ DeviceResources->SetLogicalSize({sender->Bounds.Width, sender->Bounds.Height}); UserUWP->CreateWindowSizeDependentResources(); }
		void			OnVisibilityChanged				(CoreWindow^ sender, VisibilityChangedEventArgs^ args)				{ WindowVisible = args->Visible; }
		void			OnWindowClosed					(CoreWindow^ sender, CoreWindowEventArgs^ args)						{ WindowClosed = true; }

		// DisplayInformation event handlers.
		void			OnDpiChanged					(DisplayInformation^ sender, Object^ args)							{ DeviceResources->SetDpi(sender->LogicalDpi); UserUWP->CreateWindowSizeDependentResources(); }
		void			OnOrientationChanged			(DisplayInformation^ sender, Object^ args)							{ DeviceResources->SetCurrentOrientation(sender->CurrentOrientation); UserUWP->CreateWindowSizeDependentResources(); }
		void			OnDisplayContentsInvalidated	(DisplayInformation^ sender, Object^ args)							{ DeviceResources->ValidateDevice(); }

	public:
		// IFrameworkView Methods.
		virtual void	Uninitialize					()																	{}
		virtual void	Load							(String^ entryPoint)												{
			if (UserUWP == nullptr) {
				UserUWP			= std::unique_ptr<STheOneUWP>(new STheOneUWP(DeviceResources));
			}
		}

		// The first method called when the IFrameworkView is being created.
		virtual void	Initialize						(CoreApplicationView^ applicationView)								{
			// Register event handlers for app lifecycle. This example includes Activated, so that we can make the CoreWindow active and start rendering on the window.
			applicationView->Activated						+= ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>	(this, &App::OnActivated	);
			CoreApplication::Suspending						+= ref new EventHandler<SuspendingEventArgs^>								(this, &App::OnSuspending	);
			CoreApplication::Resuming						+= ref new EventHandler<Object^>											(this, &App::OnResuming		);
			DeviceResources									= std::make_shared<DX::D3DDeviceResources>();		// At this point we have access to the device. We can create the device-dependent resources.
		}

		virtual void	SetWindow						(CoreWindow^ window)												{
			window->SizeChanged								+= ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>	(this, &App::OnWindowSizeChanged);
			window->VisibilityChanged						+= ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>	(this, &App::OnVisibilityChanged);
			window->Closed									+= ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>			(this, &App::OnWindowClosed);

			DisplayInformation									^ currentDisplayInformation			= DisplayInformation::GetForCurrentView();
			currentDisplayInformation->DpiChanged			+= ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);
			currentDisplayInformation->OrientationChanged	+= ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);
			DisplayInformation::DisplayContentsInvalidated	+= ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

			DeviceResources->SetWindow(window);
		}

		virtual void	Run								() {
			while (!WindowClosed) {
				if (!WindowVisible)
				CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
				else {
					CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
					UserUWP->Update();
					if (UserUWP->Render())
						DeviceResources->Present();
				}
			}
		}
	};
}

ref class Direct3DApplicationSource sealed : IFrameworkViewSource {
public:
	virtual IFrameworkView^	CreateView	()			{ return ref new the_one_uwp::App(); }
};

// The main function is only used to initialize our IFrameworkView class.
[MTAThread]
int									main									(Array<String^>^)	{
	Direct3DApplicationSource				^ direct3DApplicationSource				= ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

