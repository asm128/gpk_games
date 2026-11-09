#include "ssiege_parse.h"
#include "ssiege_handle.h"
#include "ssiege_event_args.h"

#include "gpk_gui_control_list.h"

#include "gpk_event_screen.h"
#include "gpk_path.h"

using ::gpk::get_value_namep, ::gpk::get_enum_namep, ::gpk::failed;
GPK_USING_TYPEINT();

static	gpk::err_t	setupConsole			(::gpk::SGUI & gui, ::ssg::SSiegeUI & ssiegeUI) {
	if_fail_fe(::gpk::inputBoxCreate(ssiegeUI.UserInput, gui, ssiegeUI.Root));
	ssiegeUI.UserInput.MaxLength	= 64;
	if_fail_fe(ssiegeUI.UserInput.Edit(gui, true));

	::gpk::SControlPlacement & userInputRoot = gui.Controls.Placement[ssiegeUI.UserInput.IdRoot];
	::gpk::SControlPlacement & userInputText = gui.Controls.Placement[ssiegeUI.UserInput.IdText];
	userInputRoot.Align			= ::gpk::ALIGN_CENTER_BOTTOM;
	userInputRoot.Area.Size.y	= userInputText.Area.Size.y;
	gui.Controls.SetHidden(ssiegeUI.UserInput.VirtualKeyboard.IdRoot, true);
	return 0;
}

// initialize root gui layout and console input/virtual keyboard
gpk::err_t			ssg::setupSSiegeUI		(::gpk::SGUI & gui, ::ssg::SSiegeUI & ssiegeUI) {
	if_fail_fe(ssiegeUI.Root = ::gpk::createScreenLayout(gui));
	if_fail_fe(::setupConsole(gui, ssiegeUI));
	return 0; 
}

gpk::err_t			ssg::SSiegeApp::StateSwitch(APP_STATE newState) {
	::gpk::SGUI					& gui				= *GUI;
	::gpk::SControlTable		& table				= gui.Controls;
	if(newState != ActiveState) {
		info_printf("Switching state: (%i) -> (%i) - '%s' -> '%s'", ActiveState, newState, ::gpk::get_value_namep(ActiveState), ::gpk::get_value_namep(newState));

		if(0 == ActiveState || 0 == gui.Controls.States.size() || ::gpk::CID_INVALID == UI.Root) {
			if_fail_fe(::ssg::setupSSiegeUI(gui, UI));
		}
		while(newState >= UI.RootPerState.size()) {	// create layouts for each state as they're required
			::gpk::cid_t				rootId;
			if_fail_fe(rootId = UI.RootPerState[UI.RootPerState.push_back(::gpk::createScreenLayout(*GUI, UI.Root))]);
			if_zero_e(GUI->Controls.SetHidden(rootId, true));
		}
				
		const ::gpk::cid_t iDialogNew		= UI.RootPerState[newState];
		const ::gpk::cid_t iDialogActive	= UI.RootPerState[ActiveState];
		if_true_e(table.SetHidden(iDialogNew, false));
		if(ActiveState >= APP_STATE_Welcome && iDialogActive) {
			if_zero_e(table.SetHidden(iDialogActive, true));
		}
		if(newState == APP_STATE_Home && ActiveState > APP_STATE_Home) // Don't autosave first time we enter Home.
			if_fail_e(Save());

		ActiveState				= newState;
	}
	return ActiveState;
}


gpk::err_t			ssg::ssiegeDraw		(::ssg::SSiegeApp & app, ::gpk::rtbgra8d32 & backBuffer, bool onlyGUI) { 
	if(false == onlyGUI) {
		const ::gpk::SCamera		& cameraSelected		= app.Game.Camera;
		if_fail_fe(::ssg::ssiegeGameDraw(app.Game, backBuffer, cameraSelected.Position, cameraSelected.Target, cameraSelected.Up, cameraSelected.NearFar));
	}
	if_fail_fe(::gpk::guiDraw(*app.GUI, backBuffer.Color.View));
	return 0; 
}

static	gpk::err_t	processScreenEvent		(::ssg::SSiegeApp & app, const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Close:
	case ::gpk::EVENT_SCREEN_Deactivate: 
		if(app.ActiveState != ::ssg::APP_STATE_Welcome) 
			if_fail_fe(app.StateSwitch(::ssg::APP_STATE_Home));
		break;
	case ::gpk::EVENT_SCREEN_Create:
	case ::gpk::EVENT_SCREEN_Resize: {
		::gpk::n2u1_t				newMetrics				= *(const ::gpk::n2u1_t*)screenEvent.Data.begin();
		if_fail_fe(::gpk::guiUpdateMetrics(*app.GUI, newMetrics, true));

		::gpk::n2u1_t				renderTargetWorldSize;	
		renderTargetWorldSize	= newMetrics;
		double						currentRatio			= renderTargetWorldSize.y / (double)renderTargetWorldSize.x;
		double						targetRatioY			= 9 / 16.0;
		if(currentRatio >= targetRatioY)
			renderTargetWorldSize.y = (u1_t)(renderTargetWorldSize.x * targetRatioY + .1f);
		else 
			renderTargetWorldSize.x = (u1_t)(renderTargetWorldSize.y / targetRatioY + .1f);
		
		if_fail_fe(app.RenderTargetWorld->resize(renderTargetWorldSize));
	}
		break;
	}
	return 0;
}

static	gpk::err_t	processTextEvent		(::ssg::SSiegeApp & /*app*/, const ::gpk::SEventView<::gpk::EVENT_TEXT> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_TEXT_Char: break;
	}
	return 0;
}

static	gpk::err_t	processKeyboardEvent	(::ssg::SSiegeApp & app, const ::gpk::SEventView<::gpk::EVENT_KEYBOARD> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_KEYBOARD_Down:
		if(screenEvent.Data[0] == ::gpk::SYSTEM_KEY_Escape) {
			if(false == app.UI.UserInput.Editing) {
				if(app.ActiveState != ::ssg::APP_STATE_Home) {
					if_fail_fe(app.StateSwitch(::ssg::APP_STATE_Home));
				}
				else 
					if_fail_fe(app.StateSwitch(::ssg::APP_STATE_Play));
			}
		}
		else if(screenEvent.Data[0] == ::gpk::SYSTEM_KEY_OEM_5) {
			if_fail_fe(app.UI.UserInput.Edit(*app.GUI, true));
		}
		break;
	}
	return 0;
}

static	gpk::err_t	processSystemEvent		(::ssg::SSiegeApp & app, const ::gpk::SEventSystem & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen		: if_fail_fe(::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN		>(sysEvent, [&app](auto ev) { return processScreenEvent		(app, ev); })); break;
	case ::gpk::SYSTEM_EVENT_Text		: if_fail_fe(::gpk::eventExtractAndHandle<::gpk::EVENT_TEXT		>(sysEvent, [&app](auto ev) { return processTextEvent		(app, ev); })); break;
	case ::gpk::SYSTEM_EVENT_Keyboard	: if_fail_fe(::gpk::eventExtractAndHandle<::gpk::EVENT_KEYBOARD	>(sysEvent, [&app](auto ev) { return processKeyboardEvent	(app, ev); })); break;
	}
	return 0;
}

static	gpk::err_t	updateInput				(::ssg::SSiegeUI & /*ui*/, ::ssg::SWorldView & world, double actualSecondsElapsed, ::gpk::vcu0_t keyStates, const ::gpk::n3s1_t /*mouseDeltas*/, ::gpk::vcu0_t /*buttonStates*/) { 
	const double				secondsElapsed			= actualSecondsElapsed * world.WorldState.TimeScale;

	if(keyStates[::gpk::SYSTEM_KEY_Control]) {
		if(keyStates[::gpk::SYSTEM_KEY_Add]) 
			world.WorldState.TimeScale	+= (float)secondsElapsed;
		else if(keyStates[::gpk::SYSTEM_KEY_Subtract]) 
			world.WorldState.TimeScale	= ::gpk::max(0.f, world.WorldState.TimeScale - (float)secondsElapsed);
		else if(keyStates['T']) 
			world.WorldState.TimeScale	= 1;
	}

	return 0;
}

static	gpk::err_t	parseCommands			(gpk::vpobj<::gpk::asc_t> inputQueue, gpk::apobj<::gpk::asc_t> & inputHistory, ::gpk::apobj<::ssg::EventSSiege> & outputEvents) { 
	inputQueue.for_each([&outputEvents, &inputHistory](::gpk::pasc_t & textLine) {
		if_null_fe(textLine.get_ref());	
		if_zero_fe(textLine->size());	

		if(textLine->size() > 1 && (*textLine)[0] == '.')
			if_fail_fef(::ssg::parseCommandLine(outputEvents, *textLine), "%.*s", GPK_VIEW_ARGS((*textLine)));

		info_printf("Queued input processed: '%.*s'", GPK_VIEW_ARGS((*textLine)));
		if_fail_fe(inputHistory.push_back(textLine));
		return 0;
	});
	return 0;
}

static	gpk::err_t	handleSSiegeEvent		(::ssg::SSiegeApp & app, ::gpk::pobj<::ssg::EventSSiege> & _eventToProcess, ::gpk::apobj<::ssg::EventSSiege> & appOutputEvents) { 
	if(!_eventToProcess)
		return false;

	const ssg::EventSSiege	& eventToProcess			= *_eventToProcess;
	info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

	gpk::err_t			result			= 0; 
	switch(eventToProcess.Type) {
	CASE_SSG_EVENT(result, app, WORLD_EVENT, appOutputEvents); 
	CASE_SSG_EVENT(result, app, WORLD_ADMIN, appOutputEvents); 
	CASE_SSG_EVENT(result, app, ACT_SAILING, appOutputEvents); 
	CASE_SSG_EVENT(result, app, ACTION_CHAR, appOutputEvents); 
	CASE_SSG_EVENT(result, app, ACT_ENGINES, appOutputEvents); 
	CASE_SSG_EVENT(result, app, ACT_AIRSHIP, appOutputEvents); 
	CASE_SSG_EVENT(result, app, ACT_WHEELED, appOutputEvents); 
	CASE_SSG_EVENT(result, app, CLIENT_ASKS, appOutputEvents); 
	CASE_SSG_EVENT(result, app, WORLD_SETUP, appOutputEvents); 
	CASE_SSG_EVENT(result, app, WORLD_VALUE, appOutputEvents); 
	default: 
		gpk_warning_unhandled_event(eventToProcess); 
		break;
	}
	return bool(result == 1);
}

gpk::err_t			ssg::ssiegeUpdate		(::ssg::SSiegeApp & app, double secondsElapsed, const ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents
	, ::gpk::FBool<::gpk::pobj<::ssg::EventSSiege> &, ::gpk::apobj<::ssg::EventSSiege> &> funcHandleEvent
	) {
	if_fail_fe(systemEvents.for_each([&app](const ::gpk::pobj<::gpk::SEventSystem> & sysEvent) { return ::processSystemEvent(app, *sysEvent); }));

	SWorldView		& worldView				= app.Game.World;
	bool						processInput			= false;

	switch(app.ActiveState) {
	case ::ssg::APP_STATE_Quit	: return 1;
	case ::ssg::APP_STATE_Welcome	: // APP_STATE_Welcome comes right after APP_STATE_Init.
		if_fail_fe(app.StateSwitch(::ssg::APP_STATE_Home)); 
		break;	
	case ::ssg::APP_STATE_Init	: 
		{
			::gpk::aobj<string>		fileNames				= {};
			if_fail_e(::gpk::pathList(app.FileStrings.SavegameFolder, fileNames, app.FileStrings.ExtensionSaveAuto));
			if(fileNames.size())
				if_fail_e(app.Load(fileNames[0]));

			::ssg::SArgsEvent		dummy					= {};
			if_fail_fe(::gpk::eventEnqueueChild(app.EventQueue, ::ssg::SSG_EVENT_WORLD_ADMIN, ::ssg::WORLD_ADMIN_Initialize, dummy));
		}
		if_fail_fe(app.StateSwitch(::ssg::APP_STATE_Welcome));
		break;
	default: 
		if_fail_fe(::updateInput(app.UI, worldView, secondsElapsed, inputState->KeyboardCurrent.KeyState, inputState->MouseCurrent.Deltas.s1(), inputState->MouseCurrent.ButtonState));

		if(0 == funcHandleEvent)
			funcHandleEvent			= [&app](::gpk::pobj<::ssg::EventSSiege> & _eventToProcess, ::gpk::apobj<::ssg::EventSSiege> & appOutputEvents) { return handleSSiegeEvent(app, _eventToProcess, appOutputEvents); };

		// Process the input queue with the app handlers before sending it to the world view updates.
		//::gpk::eventEnqueueChild(app.EventQueue, ::ssg::SSG_EVENT_WORLD_EVENT, ::ssg::WORLD_EVENT_Tick, secondsElapsed);
		if_fail_fe(app.EventQueue.for_each([&app, &funcHandleEvent](::gpk::pobj<::ssg::EventSSiege> & _eventToProcess){ 
			bool					remove				= funcHandleEvent(_eventToProcess, app.EventQueue); 
			if(remove)
				_eventToProcess.clear();
			return 0;
		}));

		{ // Process the current event queue and enqueue messages generated by the world view.
			const u2_t				offsetToEnqueue		= app.EventQueue.size();
			if_fail_fe(::ssg::ssiegeGameUpdate(app.Game, app.EventQueue, app.EventQueue, secondsElapsed));

			if(app.EventQueue.size() > offsetToEnqueue)
				app.EventQueue			= ::gpk::vpobj<::ssg::EventSSiege>{(::gpk::pobj<::ssg::EventSSiege>*)&app.EventQueue[offsetToEnqueue], app.EventQueue.size() - offsetToEnqueue};
			else 
				app.EventQueue.clear();
		}
		processInput			= true;
		break;
	} // switch

	app.Game.Global.State.UserTime.Played += secondsElapsed;
	if(app.Game.Global.Stages.size()) {
		app.Game.Global.Stages[app.Game.Global.Stages.size() - 1].Time.Played += secondsElapsed;
		stxp	float			Fasting					= .45f;
		app.Game.Global.Stages[app.Game.Global.Stages.size() - 1].SimulatedTime.Update(secondsElapsed, Fasting);
	}
	if(processInput) { // process console
		::gpk::SGUI					& gui				= *app.GUI;
		::gpk::acid					controlsToProcess	= {};
		if_fail_fe(::gpk::guiGetProcessableControls(gui, controlsToProcess));
		if(s2_t result = app.UI.UserInput.Update(gui, app.UI.UserInput.VirtualKeyboard, systemEvents, controlsToProcess)) {
			if(result == INT_MAX) { // enter key pressed
				::gpk::vcsc_t					trimmed				= {};
				if_fail_fe(app.UI.UserInput.Text.slice(trimmed, 0, ::gpk::min(app.UI.UserInput.Text.size(), 128U)));
				if_fail_fe(::gpk::trim(trimmed));
				if(trimmed.size()) {
					::gpk::pasc_t				textInput;
					if_true_fe(0 == textInput.create());
					*textInput				= ::gpk::label(trimmed);
					info_printf("Console input queued: '%s'", textInput->begin());
					if_fail_fe(app.UI.InputQueue.push_back(textInput));
				}
				if_fail_fe(app.UI.UserInput.SetText(gui, {}));
				if_fail_fe(app.UI.UserInput.Edit(gui, false));
			}
		}
		//app.Game.Camera.Position.y = (float)sin(app.Game.Global.State.UserTime.Played) * .05f;
		//app.Game.Camera.Position.RotateY(-secondsElapsed * .05f);
		//else { // enable editbox if needed
		//	::gpk::guiProcessControls(gui, controlsToProcess, [&](::gpk::cid_t iControl) {
		//		//u2_t					offsetControl				= editor.Dialogs[::gpk::EDITOR_APP_DIALOG_Menu];
		//		app.UI.UserInput.SetText(gui, gui.Controls.Text[iControl].Text);
		//		app.UI.UserInput.Edit(gui, true);
		//		return 0;
		//	});
		//}
	}

	if(::ssg::APP_STATE_Init < app.ActiveState && app.UI.InputQueue.size()) {
		if_fail_fe(::parseCommands(app.UI.InputQueue, app.UI.InputHistory, app.EventQueue));
		app.UI.InputQueue.clear();
	}

	//const ::ssg::APP_STATE			newState			= ::ssg::uiUpdate(app, systemEvents);
	//return app.StateSwitch(newState);
	return app.ActiveState;
}

