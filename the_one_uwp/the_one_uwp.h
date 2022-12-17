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

		::std::shared_ptr<::DX::DeviceResources>	DeviceResources		;
		::the_one_uwp::Sample3DSceneRenderer		SceneRenderer		;
		::the_one_uwp::SampleFpsTextRenderer		FpsTextRenderer		;
		::DX::StepTimer								Timer				;
	public:
													~STheOneUWP								()	{ DeviceResources->RegisterDeviceNotify(nullptr); }

													STheOneUWP								(const std::shared_ptr<DX::DeviceResources>& deviceResources)	
			: DeviceResources	(deviceResources)
			, SceneRenderer		(deviceResources)
			, FpsTextRenderer	(deviceResources) {
			DeviceResources->RegisterDeviceNotify(this);
		}

		void										CreateWindowSizeDependentResources		();
		void										Update									();
		bool										Render									();

		virtual void								OnDeviceLost							() {
			SceneRenderer	.ReleaseDeviceDependentResources();
			FpsTextRenderer	.ReleaseDeviceDependentResources();
		}

		virtual void								OnDeviceRestored						() {
			SceneRenderer	.CreateDeviceDependentResources();
			FpsTextRenderer	.CreateDeviceDependentResources();
			CreateWindowSizeDependentResources();
		}
	};
}

#endif