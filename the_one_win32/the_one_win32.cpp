// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "the_one_win32.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

#include <DirectXColors.h>

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "The One");

static				::gpk::error_t										updateSizeDependentResources				(::SApplication& app)											{
	const ::gpk::SCoord2<uint32_t>												newSize										= app.Framework.RootWindow.Size;
	::gpk::updateSizeDependentTarget(app.Framework.BackBuffer->Color, newSize);
	app.DeviceResources->SetLogicalSize(newSize.Cast<float>());
	app.D3DScene.CreateWindowSizeDependentResources(); 
	app.Framework.RootWindow.Resized										= false;
	return 0;
}

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& app)											{
	app.DeviceResources->m_swapChain->SetFullscreenState(FALSE, 0);
	app.D3DScene			= {};	
	app.D3DText				= {};
	app.DeviceResources		= {};

	::gpk::SWindowPlatformDetail												& displayDetail								= app.Framework.RootWindow.PlatformDetail;
	::gpk::mainWindowDestroy(app.Framework.RootWindow);
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	return 0;
}

					::gpk::error_t										setup										(::SApplication& app)											{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::SWindow																& mainWindow								= framework.RootWindow;
	mainWindow.Size															= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");
	app.DeviceResources->RegisterDeviceNotify(&app);
	app.DeviceResources->SetWindow(mainWindow.PlatformDetail.WindowHandle);
	
	gpk_necs(app.D3DScene.Initialize(app.DeviceResources));
	gpk_necs(app.D3DText.Initialize(app.DeviceResources));
	ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	return 0;
}

::gpk::error_t										update										(::SApplication& app, bool systemRequestedExit)					{
	::gpk::SFramework									& framework									= app.Framework;
	::gpk::SFrameInfo									& frameInfo									= framework.FrameInfo;
	::gpk::SWindow										& mainWindow								= app.Framework.RootWindow;
	{
		::gpk::STimer										timer;
		::the1::theOneUpdate(app.TheOne, frameInfo.Seconds.LastFrame, framework.Input, framework.RootWindow.EventQueue);

		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}
	if(mainWindow.Resized) {
		ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	}

	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	::gpk::error_t											frameworkResult								= ::gpk::updateFramework(app.Framework);
	ree_if(errored(frameworkResult), "Unknown error.");
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");

	if(mainWindow.Resized) {
		ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	}

	//-----------------------------
	::gpk::STimer																& timer										= app.Framework.Timer;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);

	app.D3DText.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (uint32_t)frameInfo.FramesPerSecond);
	app.D3DScene.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total);
	
	return 0;
}

					::gpk::error_t										draw										(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= app.Framework;
	if(framework.RootWindow.Resized) {
		ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	}

	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			backBuffer	= framework.BackBuffer;
	//framework.BackBuffer = {};
	backBuffer->resize(framework.BackBuffer->Color.metrics(), 0xFF000030, (uint32_t)-1);

	auto								context				= app.DeviceResources->GetD3DDeviceContext();
	auto								viewport			= app.DeviceResources->GetScreenViewport();	// Reset the viewport to target the whole screen.
	context->RSSetViewports(1, &viewport);

	ID3D11RenderTargetView *			targets	[]			= { app.DeviceResources->GetBackBufferRenderTargetView(), };	// Reset render targets to the screen.
	context->OMSetRenderTargets(1, targets, app.DeviceResources->GetDepthStencilView());

	context->ClearRenderTargetView(app.DeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::DarkBlue);	// Clear the back buffer and depth stencil view.
	context->ClearDepthStencilView(app.DeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	

	app.D3DScene.Render();
	app.D3DText.Render();
	app.DeviceResources->Present();

	//::the1::theOneDraw(app.TheOne, *backBuffer, framework.FrameInfo.Seconds.Total);
	//memcpy(framework.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//::gpk::grid_mirror_y(framework.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.BackBuffer = backBuffer;
	//------------------------------------------------
	return 0;
}

