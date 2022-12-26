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

//#define DISABLE_D3D11

static				::gpk::error_t					updateSizeDependentResources				(::SApplication& app)											{
	const ::gpk::SCoord2<uint32_t>							newSize										= app.Framework.RootWindow.Size;
	if(app.Framework.RootWindow.BackBuffer) {
		::gpk::updateSizeDependentTarget(app.Framework.RootWindow.BackBuffer->Color, newSize);
		::gpk::updateSizeDependentTarget(app.Framework.RootWindow.BackBuffer->DepthStencil, newSize);
	}
#if !defined(DISABLE_D3D11)
	app.DeviceResources->SetLogicalSize(newSize.Cast<float>());
	app.D3DScene.CreateWindowSizeDependentResources();
#endif
	app.Framework.RootWindow.Resized					= false;
	return 0;
}

// --- Cleanup application resources.
					::gpk::error_t					cleanup										(::SApplication& app)											{
#if !defined(DISABLE_D3D11)
	app.DeviceResources->m_swapChain->SetFullscreenState(FALSE, 0);
	app.D3DScene										= {};	
	app.D3DText											= {};
	app.DeviceResources									= {};
#endif

	::gpk::mainWindowDestroy(app.Framework.RootWindow);
	return 0;
}

::gpk::error_t										setup										(::SApplication& app)											{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	::gpk::SFramework										& framework									= app.Framework;
	::gpk::SWindow											& mainWindow								= framework.RootWindow;
	mainWindow.Size										= {1280, 720};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");

#if !defined(DISABLE_D3D11)
	gpk_necs(app.DeviceResources->Initialize());
	app.DeviceResources->RegisterDeviceNotify(&app);
	gpk_necs(app.DeviceResources->SetWindow(mainWindow.PlatformDetail.WindowHandle));
	
	gpk_necs(app.D3DScene.Initialize(app.DeviceResources));
	gpk_necs(app.D3DText.Initialize(app.DeviceResources));
	ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");
#endif
	return 0;
}

::gpk::error_t										update										(::SApplication& app, bool systemRequestedExit)					{
	::gpk::SFramework										& framework									= app.Framework;
	::gpk::SFrameInfo										& frameInfo									= framework.FrameInfo;
	::gpk::SWindow											& mainWindow								= app.Framework.RootWindow;
	{
		::gpk::STimer										timer;
		::the1::theOneUpdate(app.TheOne, frameInfo.Seconds.LastFrame, framework.Input, framework.RootWindow.EventQueue);

		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}
	if(mainWindow.Resized) {
		ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	}
#if !defined(DISABLE_D3D11)
	if(app.TheOne.ActiveState != ::the1::APP_STATE_Init && app.D3DScene.IndexBuffer.size() < app.TheOne.MainGame.Game.Engine.Scene->Graphics->Meshes.size()) {
		gpk_necs(app.CreateDeviceDependentResources());

	}
#endif

	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");

	::gpk::error_t											frameworkResult								= ::gpk::updateFramework(app.Framework);
	gpk_necs(frameworkResult);
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");

	if(mainWindow.Resized) {
		ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	}

	//-----------------------------
	::gpk::STimer											& timer										= app.Framework.Timer;
	char													buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND													windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
#if !defined(DISABLE_D3D11)
	app.D3DText.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (uint32_t)frameInfo.FramesPerSecond);
#endif
	
	return 0;
}

::gpk::error_t										draw										(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	::gpk::SFramework										& framework									= app.Framework;
	if(framework.RootWindow.Resized) {
		ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	}

#if !defined(DISABLE_D3D11)
	auto													context										= app.DeviceResources->GetD3DDeviceContext();
	auto													viewport									= app.DeviceResources->GetScreenViewport();	// Reset the viewport to target the whole screen.
	context->RSSetViewports(1, &viewport);

	ID3D11RenderTargetView *								targets	[]									= { app.DeviceResources->GetBackBufferRenderTargetView(), };	// Reset render targets to the screen.
	context->OMSetRenderTargets(1, targets, app.DeviceResources->GetDepthStencilView());

	context->ClearRenderTargetView(app.DeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::DarkBlue);	// Clear the back buffer and depth stencil view.
	context->ClearDepthStencilView(app.DeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	

	
	const ::the1::SPlayerUI									& playerUI									= app.TheOne.MainGame.PlayerUI[app.TheOne.MainGame.CurrentPlayer];
	const ::the1::SCamera									& cameraSelected		
		= (playerUI.Cameras.Selected == 0				) ? playerUI.Cameras.Free 
		: (playerUI.Cameras.Selected > the1::MAX_BALLS	) ? playerUI.Cameras.Stick
		: playerUI.Cameras.Balls[playerUI.Cameras.Selected - 1] 
		;

	::gpk::SCoord3<float>							cameraFront						= (cameraSelected.Target - cameraSelected.Position).Normalize();
	::gpk::SNearFar									nearFar 						= {.0001f, 10.0f}; 

	::gpk::SEngineSceneConstants					& constants						= app.D3DScene.ConstantBufferScene;
	constants.CameraPosition					= cameraSelected.Position;
	constants.CameraFront						= cameraFront;
	constants.LightPosition						= app.TheOne.MainGame.LightPos;
	constants.LightDirection					= {0, -1, 0};

	const ::gpk::SCoord2<uint16_t>					offscreenMetrics				= framework.RootWindow.Size.Cast<uint16_t>();
	constants.View.LookAt(cameraSelected.Position, cameraSelected.Target, {0, 1, 0});
	constants.Perspective.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far);
	constants.Screen.ViewportLH(offscreenMetrics);
	constants.VP								= constants.View * constants.Perspective;
	constants.VPS								= constants.VP * constants.Screen;

	const ::gpk::SEngine							& engine		= app.TheOne.MainGame.Game.Engine;
	const ::gpk::SEngineScene						& scene			= *engine.Scene;
	for(uint32_t iNode = 0; iNode < scene.ManagedRenderNodes.RenderNodes.size(); ++iNode) {
		const ::gpk::SRenderNodeFlags					& flags						= scene.ManagedRenderNodes.Flags[iNode];
		const ::gpk::SRenderNode						& node						= scene.ManagedRenderNodes.RenderNodes[iNode];
		if(flags.NoDraw)
			continue;

		if(node.Mesh >= scene.Graphics->Meshes.size())
			continue;

		const ::gpk::SRenderNodeTransforms							& transforms				= scene.ManagedRenderNodes.Transforms[iNode];

		::gpk::SRenderNodeConstants									& nodeConstants				= app.D3DScene.ConstantBufferModel;
		nodeConstants.MVP										= (transforms.World * constants.View * constants.Perspective).GetTranspose();
		nodeConstants.Model										= transforms.World.GetTranspose();
		nodeConstants.ModelInverseTranspose						= transforms.WorldInverseTranspose.GetTranspose();

		const ::gpk::SGeometryMesh									& mesh						= *scene.Graphics->Meshes[node.Mesh];
		verbose_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iNode, node.Mesh, node.Slice, scene.Graphics->Meshes.Names[node.Mesh].begin());

		const ::gpk::view_array<const uint16_t>						indices						
			= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>{(const uint16_t*)scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)} 
			: ::gpk::view_array<const uint16_t>{}
			;

		const ::gpk::SSkin											& skin						= *scene.Graphics->Skins.Elements[node.Skin];
		const ::gpk::SGeometrySlice									slice						= (node.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[node.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};
		nodeConstants.Material									= skin.Material;

		app.D3DScene.Render(node.Mesh, slice.Slice, skin.Textures[0], node.Shader);
	}
	app.D3DText.Render();
	app.DeviceResources->Present();
	framework.RootWindow.BackBuffer								= {};
#else 
	::gpk::pobj<::gpk::SWindow::TOffscreen>			backBuffer									= framework.RootWindow.BackBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), 0xFF000030, (uint32_t)-1);
	::the1::theOneDraw(app.TheOne, *backBuffer, framework.FrameInfo.Seconds.Total);
	memcpy(framework.RootWindow.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
#endif

	//::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.RootWindow.BackBuffer = backBuffer;
	return 0;
}
