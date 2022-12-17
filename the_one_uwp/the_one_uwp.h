#include "gpk_the_one.h"

#include "StepTimer.h"
#include "DeviceResources.h"
#include "Sample3DSceneRenderer.h"
#include "SampleFpsTextRenderer.h"

#ifndef THE_ONE_UWPMAIN_H_98274
#define THE_ONE_UWPMAIN_H_98274

// Renders Direct2D and 3D content on the screen.
namespace the_one_uwp
{
	class STheOneUWP : public DX::IDeviceNotify {
		::the1::STheOne								TheOneApp			;
		::gpk::ptr_obj<::gpk::SInput>				Input				;

		::std::shared_ptr<::DX::D3DDeviceResources>	DeviceResources		;
		::the_one_uwp::Sample3DSceneRenderer		SceneRenderer		;
		::the_one_uwp::SampleFpsTextRenderer		FpsTextRenderer		;
		::DX::StepTimer								Timer				;
	public:
													~STheOneUWP								()	{ DeviceResources->RegisterDeviceNotify(nullptr); }

													STheOneUWP								(const std::shared_ptr<DX::D3DDeviceResources>& deviceResources)	
			: DeviceResources	(deviceResources)
			, SceneRenderer		(deviceResources)
			, FpsTextRenderer	(deviceResources) {
			DeviceResources->RegisterDeviceNotify(this);
		}

		void										Update									();
		bool										Render									();

		void										CreateWindowSizeDependentResources		() {
			SceneRenderer	.CreateWindowSizeDependentResources();		// TODO: Replace this with the size-dependent initialization of your app's content.
		}
		virtual void								OnDeviceLost							() {
			SceneRenderer	.ReleaseDeviceDependentResources();
			FpsTextRenderer	.ReleaseDeviceDependentResources();
		}

		virtual void								OnDeviceRestored						() {
			SceneRenderer	.CreateDeviceDependentResources();
			FpsTextRenderer	.CreateDeviceDependentResources();
			CreateWindowSizeDependentResources();		// TODO: Replace this with the size-dependent initialization of your app's content.
		}
	};
}

#endif