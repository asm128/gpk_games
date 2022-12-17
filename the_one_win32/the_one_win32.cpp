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
	const ::gpk::SCoord2<uint32_t>												newSize										= app.Framework.MainDisplay.Size;
	::gpk::updateSizeDependentTarget(app.Framework.MainDisplayOffscreen->Color, newSize);
	return 0;
}

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& app)											{
	app.DeviceResources->m_swapChain->SetFullscreenState(FALSE, 0);
	app.D3DScene			= {};	
	app.D3DText				= {};
	app.DeviceResources		= {};

	::gpk::SWindowPlatformDetail												& displayDetail								= app.Framework.MainDisplay.PlatformDetail;
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	return 0;
}

					::gpk::error_t										setup										(::SApplication& app)											{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::SWindow																& mainWindow								= framework.MainDisplay;
	mainWindow.Size															= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");
	app.DeviceResources->RegisterDeviceNotify(&app);
	app.DeviceResources->SetWindow(mainWindow.PlatformDetail.WindowHandle);
	
	gpk_necs(app.D3DScene.Initialize(app.DeviceResources));
	gpk_necs(app.D3DText.Initialize(app.DeviceResources));
	app.D3DScene.CreateWindowSizeDependentResources(); 
	ree_if	(errored(::updateSizeDependentResources	(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
	return 0;
}

::gpk::error_t										update										(::SApplication& app, bool systemRequestedExit)					{
	::gpk::SFramework									& framework									= app.Framework;
	::gpk::SFrameInfo									& frameInfo									= framework.FrameInfo;
	::gpk::SWindow										& mainWindow								= app.Framework.MainDisplay;
	if(mainWindow.Resized) {
		app.DeviceResources->SetLogicalSize(mainWindow.Size.Cast<float>());
		app.D3DScene.CreateWindowSizeDependentResources();
	}

	{
		::gpk::STimer										timer;
		::the1::theOneUpdate(app.TheOne, frameInfo.Seconds.LastFrame, framework.Input, framework.MainDisplay.EventQueue);

		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}


	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	::gpk::error_t											frameworkResult								= ::gpk::updateFramework(app.Framework);
	ree_if(errored(frameworkResult), "Unknown error.");
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	//-----------------------------
	::gpk::STimer																& timer										= app.Framework.Timer;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);


	app.D3DScene.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total);
	
	return 0;
}

					::gpk::error_t										draw										(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			backBuffer	= framework.MainDisplayOffscreen;
	//framework.MainDisplayOffscreen = {};
	backBuffer->resize(framework.MainDisplayOffscreen->Color.metrics(), 0xFF000030, (uint32_t)-1);

	auto								context				= app.DeviceResources->GetD3DDeviceContext();
	auto								viewport			= app.DeviceResources->GetScreenViewport();	// Reset the viewport to target the whole screen.
	context->RSSetViewports(1, &viewport);

	ID3D11RenderTargetView *			targets	[]			= { app.DeviceResources->GetBackBufferRenderTargetView(), };	// Reset render targets to the screen.
	context->OMSetRenderTargets(1, targets, app.DeviceResources->GetDepthStencilView());

	context->ClearRenderTargetView(app.DeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::DarkBlue);	// Clear the back buffer and depth stencil view.
	context->ClearDepthStencilView(app.DeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	

	app.D3DScene.Render();
	app.DeviceResources->Present();

	//::the1::theOneDraw(app.TheOne, *backBuffer, framework.FrameInfo.Seconds.Total);
	//memcpy(framework.MainDisplayOffscreen->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//::gpk::grid_mirror_y(framework.MainDisplayOffscreen->Color.View, backBuffer->Color.View);
	//framework.MainDisplayOffscreen = backBuffer;
	//------------------------------------------------
	return 0;
}

