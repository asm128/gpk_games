#include "ssiege_parse.h"
#include "ssiege_handle.h"
#include "ssiege_event_args.h"

#include "gpk_gui_control_list.h"

#include "gpk_event_screen.h"
#include "gpk_path.h"

static	::gpk::error_t	setupConsole			(::gpk::SGUI & gui, ::ssiege::SSSiegeUI & ssiegeUI) {
	gpk_necs(::gpk::inputBoxCreate(ssiegeUI.UserInput, gui, ssiegeUI.Root));
	ssiegeUI.UserInput.MaxLength	= 64;
	gpk_necs(ssiegeUI.UserInput.Edit(gui, true));

	gui.Controls.Placement[ssiegeUI.UserInput.IdRoot].Align		= ::gpk::ALIGN_CENTER_BOTTOM;
	gui.Controls.Placement[ssiegeUI.UserInput.IdRoot].Area.Size.y	= gui.Controls.Placement[ssiegeUI.UserInput.IdText].Area.Size.y;
	gpk_necs(gui.Controls.SetHidden(ssiegeUI.UserInput.VirtualKeyboard.IdRoot, true));
	return 0;
}

// initialize root gui layout and console input/virtual keyboard
::gpk::error_t			ssiege::setupSSiegeUI		(::gpk::SGUI & gui, ::ssiege::SSSiegeUI & ssiegeUI) {
	gpk_necs(ssiegeUI.Root = ::gpk::createScreenLayout(gui));
	gpk_necs(::setupConsole(gui, ssiegeUI));
	return 0; 
}

::gpk::error_t			ssiege::SSSiegeApp::StateSwitch(APP_STATE newState) {
	::gpk::SGUI					& gui				= *GUI;
	::gpk::SControlTable		& table				= gui.Controls;
	if(newState != ActiveState) {
		info_printf("Switching state: (%i) -> (%i) - '%s' -> '%s'", ActiveState, newState, ::gpk::get_value_namep(ActiveState), ::gpk::get_value_namep(newState));

		if(0 == ActiveState || 0 == gui.Controls.States.size() || ::gpk::CID_INVALID == UI.Root) {
			gpk_necs(::ssiege::setupSSiegeUI(gui, UI));
			//gpk_necs(::ssiege::planetarySystemSetup(World.SolarSystem, World.Engine, "./gpk_solar_system.json"));
		}
		while(newState >= UI.RootPerState.size()) {	// create layouts for each state as they're required
			::gpk::cid_t				rootId;
			gpk_necs(rootId = UI.RootPerState[UI.RootPerState.push_back(::gpk::createScreenLayout(*GUI, UI.Root))]);
			es_if_failed(GUI->Controls.SetHidden(rootId, true));
		}
				
		const ::gpk::cid_t iDialogNew		= UI.RootPerState[newState];
		const ::gpk::cid_t iDialogActive	= UI.RootPerState[ActiveState];
		es_if_failed(table.SetHidden(iDialogNew, false));
		if(ActiveState >= APP_STATE_Welcome && iDialogActive) {
			es_if_failed(table.SetHidden(iDialogActive, true));
		}
		if(newState == APP_STATE_Home && ActiveState > APP_STATE_Home) // Don't autosave first time we enter Home.
			es_if_failed(Save());

		ActiveState				= newState;
	}
	return ActiveState;
}


::gpk::error_t			ssiege::ssiegeDraw		(::ssiege::SSSiegeApp & app, ::gpk::rtbgra8d32 & backBuffer, bool onlyGUI) { 
	if(false == onlyGUI) {
		const ::ssiege::SCamera		& cameraSelected	= app.Camera;
		gpk_necs(::ssiege::worldViewDraw(app.World, backBuffer, cameraSelected.Offset, cameraSelected.Target, {.1f, 10000.f}));
	}
	gpk_necs(::gpk::guiDraw(*app.GUI, backBuffer.Color.View));
	return 0; 
}

static	::gpk::error_t	processScreenEvent		(::ssiege::SSSiegeApp & app, const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Close:
	case ::gpk::EVENT_SCREEN_Deactivate: 
		if(app.ActiveState != ::ssiege::APP_STATE_Welcome) 
			gpk_necs(app.StateSwitch(::ssiege::APP_STATE_Home));
		break;
	case ::gpk::EVENT_SCREEN_Create:
	case ::gpk::EVENT_SCREEN_Resize: {
		::gpk::n2u16				newMetrics				= *(const ::gpk::n2u16*)screenEvent.Data.begin();
		gpk_necs(::gpk::guiUpdateMetrics(*app.GUI, newMetrics, true));

		::gpk::n2u16				renderTargetWorldSize;	
		renderTargetWorldSize	= newMetrics;
		double						currentRatio			= renderTargetWorldSize.y / (double)renderTargetWorldSize.x;
		double						targetRatioY			= 9 / 16.0;
		if(currentRatio >= targetRatioY)
			renderTargetWorldSize.y = (uint16_t)(renderTargetWorldSize.x * targetRatioY + .1f);
		else 
			renderTargetWorldSize.x = (uint16_t)(renderTargetWorldSize.y / targetRatioY + .1f);
		
		gpk_necs(app.RenderTargetWorld->resize(renderTargetWorldSize));
	}
		break;
	}
	return 0;
}

static	::gpk::error_t	processTextEvent		(::ssiege::SSSiegeApp & /*app*/, const ::gpk::SEventView<::gpk::EVENT_TEXT> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_TEXT_Char: break;
	}
	return 0;
}

static	::gpk::error_t	processKeyboardEvent	(::ssiege::SSSiegeApp & app, const ::gpk::SEventView<::gpk::EVENT_KEYBOARD> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_KEYBOARD_Down:
		if(screenEvent.Data[0] == VK_ESCAPE) {
			if(false == app.UI.UserInput.Editing) {
				if(app.ActiveState != ::ssiege::APP_STATE_Home) 
					gpk_necs(app.StateSwitch(::ssiege::APP_STATE_Home));
				else 
					gpk_necs(app.StateSwitch(::ssiege::APP_STATE_Play));
			}
		}
		else if(screenEvent.Data[0] == VK_OEM_5) {
			gpk_necs(app.UI.UserInput.Edit(*app.GUI, true));
		}
		break;
	}
	return 0;
}

static	::gpk::error_t	processSystemEvent		(::ssiege::SSSiegeApp & app, const ::gpk::SSystemEvent & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen		: gpk_necs(::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN		>(sysEvent, [&app](auto ev) { return processScreenEvent		(app, ev); })); break;
	case ::gpk::SYSTEM_EVENT_Text		: gpk_necs(::gpk::eventExtractAndHandle<::gpk::EVENT_TEXT		>(sysEvent, [&app](auto ev) { return processTextEvent		(app, ev); })); break;
	case ::gpk::SYSTEM_EVENT_Keyboard	: gpk_necs(::gpk::eventExtractAndHandle<::gpk::EVENT_KEYBOARD	>(sysEvent, [&app](auto ev) { return processKeyboardEvent	(app, ev); })); break;
	}
	return 0;
}

static	::gpk::error_t	updateInput				(::ssiege::SSSiegeUI & /*ui*/, ::ssiege::SWorldView & world, double actualSecondsElapsed, ::gpk::vcu8 keyStates, const ::gpk::n3i16 /*mouseDeltas*/, ::gpk::vcu8 /*buttonStates*/) { 
	const double				secondsElapsed			= actualSecondsElapsed * world.WorldState.TimeScale;

	if(keyStates[VK_CONTROL]) {
		if(keyStates[VK_ADD]) 
			world.WorldState.TimeScale	+= (float)secondsElapsed;
		else if(keyStates[VK_SUBTRACT]) 
			world.WorldState.TimeScale	= ::gpk::max(0.f, world.WorldState.TimeScale - (float)secondsElapsed);
		else if(keyStates['T']) 
			world.WorldState.TimeScale	= 1;
	}

	return 0;
}

static	::gpk::error_t	parseCommands			(gpk::vpobj<gpk::achar> inputQueue, gpk::apobj<gpk::achar> & inputHistory, ::gpk::apobj<::ssiege::EventSSiege> & outputEvents) { 
	inputQueue.for_each([&outputEvents, &inputHistory](::gpk::pachar & textLine) {
		res_if(!textLine || 0 == textLine->size());	// Shouldn't get here please

		if(textLine->size() > 1 && (*textLine)[0] == '.')
			e_if_failed(::ssiege::parseCommandLine(outputEvents, *textLine), ::gpk::toString(*textLine).begin());

		info_printf("Queued input processed: '%s'", textLine->begin());
		es_if_failed(inputHistory.push_back(textLine));
	});
	return 0;
}

static	::gpk::error_t	handleSSiegeEvent		(::ssiege::SSSiegeApp & app, ::gpk::pobj<::ssiege::EventSSiege> & _eventToProcess, ::gpk::apobj<::ssiege::EventSSiege> & appOutputEvents) { 
	if(!_eventToProcess)
		return false;

	const ssiege::EventSSiege	& eventToProcess			= *_eventToProcess;
	info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

	::gpk::error_t			result			= 0; 
	switch(eventToProcess.Type) {
	case ::ssiege::SSIEGE_EVENT_CHAR_ACTION: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::CHAR_ACTION>(eventToProcess, [&app, &appOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleCHAR_ACTION(app, ev, appOutputEvents); })); break; }
	case ::ssiege::SSIEGE_EVENT_WORLD_ADMIN: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_ADMIN>(eventToProcess, [&app, &appOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_ADMIN(app, ev, appOutputEvents); })); break; }
	case ::ssiege::SSIEGE_EVENT_WORLD_EVENT: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_EVENT>(eventToProcess, [&app, &appOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_EVENT(app, ev, appOutputEvents); })); break; }
	case ::ssiege::SSIEGE_EVENT_CLIENT_ASKS: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::CLIENT_ASKS>(eventToProcess, [&app, &appOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleCLIENT_ASKS(app, ev, appOutputEvents); })); break; }
	case ::ssiege::SSIEGE_EVENT_WORLD_SETUP: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_SETUP>(eventToProcess, [&app, &appOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_SETUP(app, ev, appOutputEvents); })); break; }
	case ::ssiege::SSIEGE_EVENT_WORLD_VALUE: { es_if_failed(result = ::ssiege::eventExtractAndHandle<::ssiege::WORLD_VALUE>(eventToProcess, [&app, &appOutputEvents, &eventToProcess](auto ev){ return ::ssiege::handleWORLD_VALUE(app, ev, appOutputEvents); })); break; }
	default: 
		gpk_warning_unhandled_event(eventToProcess); 
		break;
	}
	return bool(result == 1);
}

::gpk::error_t			ssiege::ssiegeUpdate		(::ssiege::SSSiegeApp & app, double secondsElapsed, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SSystemEvent> systemEvents
	, ::gpk::FBool<::gpk::pobj<::ssiege::EventSSiege> &, ::gpk::apobj<::ssiege::EventSSiege> &> funcHandleEvent
	) {
		gpk_necs(systemEvents.for_each([&app](const ::gpk::pobj<::gpk::SSystemEvent> & sysEvent) { es_if_failed(::processSystemEvent(app, *sysEvent)); 
	}));

	::ssiege::SWorldView			& worldView				= app.World;
	bool						processInput			= false;

	switch(app.ActiveState) {
	case ::ssiege::APP_STATE_Quit	: return 1;
	case ::ssiege::APP_STATE_Welcome	: // APP_STATE_Welcome comes right after APP_STATE_Init.
		gpk_necs(app.StateSwitch(::ssiege::APP_STATE_Home)); 
		break;	
	case ::ssiege::APP_STATE_Init	: 
		{
			::gpk::aachar				fileNames				= {};
			es_if_failed(::gpk::pathList(app.FileStrings.SavegameFolder, fileNames, app.FileStrings.ExtensionSaveAuto));
			if(fileNames.size())
				es_if_failed(app.Load(fileNames[0]));

			::ssiege::SArgsEvent		dummy					= {};
			gpk_necs(::gpk::eventEnqueueChild(app.EventQueue, ::ssiege::SSIEGE_EVENT_WORLD_ADMIN, ::ssiege::WORLD_ADMIN_Initialize, dummy));
		}
		gpk_necs(app.StateSwitch(::ssiege::APP_STATE_Welcome));
		break;
	default: 
		gpk_necs(::updateInput(app.UI, worldView, secondsElapsed, inputState->KeyboardCurrent.KeyState, inputState->MouseCurrent.Deltas.i16(), inputState->MouseCurrent.ButtonState));

		if(0 == funcHandleEvent)
			funcHandleEvent			= [&app](::gpk::pobj<::ssiege::EventSSiege> & _eventToProcess, ::gpk::apobj<::ssiege::EventSSiege> & appOutputEvents) { return handleSSiegeEvent(app, _eventToProcess, appOutputEvents); };

		// Process the input queue with the app handlers before sending it to the world view updates.
		app.EventQueue.for_each([&app, &funcHandleEvent](::gpk::pobj<::ssiege::EventSSiege> & _eventToProcess){ 
			const bool					remove				= funcHandleEvent(_eventToProcess, app.EventQueue); 
			if(remove)
				_eventToProcess.clear();
		});

		{ // Process the current event queue and enqueue messages generated by the world view.
			const uint32_t				offsetToEnqueue		= app.EventQueue.size();
			gpk_necs(::ssiege::worldViewUpdate(worldView, app.EventQueue, app.EventQueue, secondsElapsed * 10));

			if(app.EventQueue.size() > offsetToEnqueue)
				app.EventQueue			= ::gpk::vpobj<::ssiege::EventSSiege>{(::gpk::pobj<::ssiege::EventSSiege>*)&app.EventQueue[offsetToEnqueue], app.EventQueue.size() - offsetToEnqueue};
			else 
				app.EventQueue.clear();
		}
		processInput			= true;
		break;
	} // switch

	if(processInput) { // process console
		::gpk::SGUI					& gui				= *app.GUI;
		::gpk::acid					controlsToProcess	= {};
		gpk_necs(::gpk::guiGetProcessableControls(gui, controlsToProcess));
		if(int32_t result = app.UI.UserInput.Update(gui, app.UI.UserInput.VirtualKeyboard, systemEvents, controlsToProcess)) {
			if(result == INT_MAX) { // enter key pressed
				::gpk::vcc					trimmed				= {};
				gpk_necs(app.UI.UserInput.Text.slice(trimmed, 0, ::gpk::min(app.UI.UserInput.Text.size(), 128U)));
				gpk_necs(::gpk::trim(trimmed));
				if(trimmed.size()) {
					::gpk::pachar				textInput;
					rees_if(0 == textInput.create());
					*textInput				= ::gpk::label(trimmed);
					info_printf("Console input queued: '%s'", textInput->begin());
					gpk_necs(app.UI.InputQueue.push_back(textInput));
				}
				gpk_necs(app.UI.UserInput.SetText(gui, {}));
				gpk_necs(app.UI.UserInput.Edit(gui, false));
			}
		}
		//else { // enable editbox if needed
		//	::gpk::guiProcessControls(gui, controlsToProcess, [&](::gpk::cid_t iControl) {
		//		//uint32_t					offsetControl				= editor.Dialogs[::gpk::EDITOR_APP_DIALOG_Menu];
		//		app.UI.UserInput.SetText(gui, gui.Controls.Text[iControl].Text);
		//		app.UI.UserInput.Edit(gui, true);
		//		return 0;
		//	});
		//}
	}

	if(::ssiege::APP_STATE_Init < app.ActiveState && app.UI.InputQueue.size()) {
		gpk_necs(::parseCommands(app.UI.InputQueue, app.UI.InputHistory, app.EventQueue));
		app.UI.InputQueue.clear();
	}

	//const ::ssiege::APP_STATE			newState			= ::ssiege::uiUpdate(app, systemEvents);
	//return app.StateSwitch(newState);
	return app.ActiveState;
}

