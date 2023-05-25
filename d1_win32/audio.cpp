// Functions for loading WAV audio files. Copyright (c) Microsoft Corporation. Licensed under the MIT License.
// http://go.microsoft.com/fwlink/?LinkId=248929 
// http://go.microsoft.com/fwlink/?LinkID=615561
#include "audio.h"

#include "WAVFileReader.h"

#include <cstdio>

// Must match order of g_PRESET_NAMES
stacxpr	XAUDIO2FX_REVERB_I3DL2_PARAMETERS	g_PRESET_PARAMS[]	=
	{ XAUDIO2FX_I3DL2_PRESET_FOREST
	, XAUDIO2FX_I3DL2_PRESET_DEFAULT
	, XAUDIO2FX_I3DL2_PRESET_GENERIC
	, XAUDIO2FX_I3DL2_PRESET_PADDEDCELL
	, XAUDIO2FX_I3DL2_PRESET_ROOM
	, XAUDIO2FX_I3DL2_PRESET_BATHROOM
	, XAUDIO2FX_I3DL2_PRESET_LIVINGROOM
	, XAUDIO2FX_I3DL2_PRESET_STONEROOM
	, XAUDIO2FX_I3DL2_PRESET_AUDITORIUM
	, XAUDIO2FX_I3DL2_PRESET_CONCERTHALL
	, XAUDIO2FX_I3DL2_PRESET_CAVE
	, XAUDIO2FX_I3DL2_PRESET_ARENA
	, XAUDIO2FX_I3DL2_PRESET_HANGAR
	, XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY
	, XAUDIO2FX_I3DL2_PRESET_HALLWAY
	, XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR
	, XAUDIO2FX_I3DL2_PRESET_ALLEY
	, XAUDIO2FX_I3DL2_PRESET_CITY
	, XAUDIO2FX_I3DL2_PRESET_MOUNTAINS
	, XAUDIO2FX_I3DL2_PRESET_QUARRY
	, XAUDIO2FX_I3DL2_PRESET_PLAIN
	, XAUDIO2FX_I3DL2_PRESET_PARKINGLOT
	, XAUDIO2FX_I3DL2_PRESET_SEWERPIPE
	, XAUDIO2FX_I3DL2_PRESET_UNDERWATER
	, XAUDIO2FX_I3DL2_PRESET_SMALLROOM
	, XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM
	, XAUDIO2FX_I3DL2_PRESET_LARGEROOM
	, XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL
	, XAUDIO2FX_I3DL2_PRESET_LARGEHALL
	, XAUDIO2FX_I3DL2_PRESET_PLATE
	};

::gpk::error_t							audioSetReverb				(IXAudio2SubmixVoice * submixVoice, uint32_t nReverb)	{
	rees_if(nReverb >= ::gpk::size(g_PRESET_PARAMS));

	XAUDIO2FX_REVERB_PARAMETERS					native;
	ReverbConvertI3DL2ToNative( &g_PRESET_PARAMS[nReverb], &native );
	submixVoice->SetEffectParameters( 0, &native, sizeof( native ) );
	return S_OK;
}

// Specify sound cone to add directionality to listener for artistic effect:
// Emitters behind the listener are defined here to be more attenuated, have a lower LPF cutoff frequency, yet have a slightly higher reverb send level.
static	X3DAUDIO_CONE					Listener_DirectionalCone		= { X3DAUDIO_PI*5.0f/6.0f, X3DAUDIO_PI*11.0f/6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };

// Specify LFE level distance curve such that it rolls off much sooner than all non-LFE channels, making use of the subwoofer more dramatic.
static	X3DAUDIO_DISTANCE_CURVE_POINT	Emitter_LFE_CurvePoints[3]		= { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f };
static	X3DAUDIO_DISTANCE_CURVE			Emitter_LFE_Curve				= { &Emitter_LFE_CurvePoints[0], 3 };

// Specify reverb send level distance curve such that reverb send increases slightly with distance before rolling off to silence.
// With the direct channels being increasingly attenuated with distance, this has the effect of increasing the reverb-to-direct sound ratio, reinforcing the perception of distance.
static	X3DAUDIO_DISTANCE_CURVE_POINT	Emitter_Reverb_CurvePoints[3]	= { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f };
static	X3DAUDIO_DISTANCE_CURVE			Emitter_Reverb_Curve			= { &Emitter_Reverb_CurvePoints[0], 3 };

//-----------------------------------------------------------------------------------------
// Initialize the audio by creating the XAudio2 device, mastering voice, etc.
//-----------------------------------------------------------------------------------------
::gpk::error_t				AUDIO_STATE::InitAudio() {
	gpk_hrcall(CoInitializeEx(0, COINIT_MULTITHREADED));

#ifdef USING_XAUDIO2_7_DIRECTX // Workaround for XAudio 2.7 known issue
	mXAudioDLL = LoadLibraryExW(
#ifdef _DEBUG
		L"XAudioD2_7.DLL"
#else
		L"XAudio2_7.DLL"
#endif
		, nullptr, 0x00000800 /* LOAD_LIBRARY_SEARCH_SYSTEM32 */
	);

	if (!mXAudioDLL)
		return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
#endif

	UINT32							flags				= 0;
 #if defined(USING_XAUDIO2_7_DIRECTX) && defined(_DEBUG)
	flags |= XAUDIO2_DEBUG_ENGINE;
 #endif
	::gpk::pcom<IXAudio2>			xAudio2;
	gpk_hrcall(XAudio2Create(&xAudio2, flags));

#if !defined(USING_XAUDIO2_7_DIRECTX) && defined(_DEBUG)
	// To see the trace output, you need to view ETW logs for this application:
	//    Go to Control Panel, Administrative Tools, Event Viewer. View->Show Analytic and Debug Logs.
	//    Applications and Services Logs / Microsoft / Windows / XAudio2. Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK 
	XAUDIO2_DEBUG_CONFIGURATION		debug				= {};
	debug.TraceMask				= XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	debug.BreakMask				= XAUDIO2_LOG_ERRORS;
	xAudio2->SetDebugConfiguration( &debug, 0 );
#endif

	gpk_hrcall(xAudio2->CreateMasteringVoice( &pMasterVoice ));

	// Check device details to make sure it's within our sample supported parameters
	DWORD							dwChannelMask		= 0;
	UINT32							nSampleRate			= 0;

#ifndef USING_XAUDIO2_7_DIRECTX

	XAUDIO2_VOICE_DETAILS			details				= {};
	pMasterVoice->GetVoiceDetails(&details);
	rees_if(details.InputChannels > ::OUTPUTCHANNELS);

	gpk_hrcall(pMasterVoice->GetChannelMask(&dwChannelMask));

	nSampleRate					= details.InputSampleRate;
	ChannelMask					= dwChannelMask;
	FrequencyRatio				= 1.0f; //SampleRateOriginal = SampleRateCurrent = details.InputSampleRate;
	Channels					= details.InputChannels;


#else
	XAUDIO2_DEVICE_DETAILS			details				= {};
	gpk_hrcall(xAudio2->GetDeviceDetails(0, &details));

	rees_if(details.OutputFormat.Format.nChannels > OUTPUTCHANNELS);

	nSampleRate					= details.OutputFormat.Format.nSamplesPerSec;
	SampleRateOriginal			= SampleRateCurrent		= details.OutputFormat.Format.nSamplesPerSec;
	dwChannelMask				= dwChannelMask			= details.OutputFormat.dwChannelMask;
	Channels					= details.OutputFormat.Format.nChannels;
#endif

#ifdef MASTERING_LIMITER
	FXMASTERINGLIMITER_PARAMETERS	params				= {};
	params.Release				= FXMASTERINGLIMITER_DEFAULT_RELEASE;
	params.Loudness				= FXMASTERINGLIMITER_DEFAULT_LOUDNESS;

	gpk_hrcall(CreateFX(__uuidof(FXMasteringLimiter), &VolumeLimiter, &params, sizeof(params)));

	XAUDIO2_EFFECT_DESCRIPTOR		desc				= {};
	desc.InitialState			= TRUE;
	desc.OutputChannels			= nChannels;
	desc.pEffect				= pVolumeLimiter.Get();

	XAUDIO2_EFFECT_CHAIN			chain				= { 1, &desc };
	gpk_hrcall(pMasterVoice->SetEffectChain(&chain));
#endif // MASTERING_LIMITER

	// Create reverb effect
	UINT32							rflags				= 0;
 #if defined(USING_XAUDIO2_7_DIRECTX) && defined(_DEBUG)
	rflags						|= XAUDIO2FX_DEBUG;
 #endif
	gpk_hrcall(XAudio2CreateReverb((IUnknown**)&ReverbEffect, rflags));

	// Create a submix voice. Performance tip: you need not run global FX with the sample number of channels as the final mix. For example, this sample runs the reverb in mono mode, thus reducing CPU overhead.
	XAUDIO2_EFFECT_DESCRIPTOR		effects[]			= { { ReverbEffect, TRUE, 1 } };
	XAUDIO2_EFFECT_CHAIN			effectChain			= { 1, effects };

	gpk_hrcall(xAudio2->CreateSubmixVoice(&pSubmixVoice, 1, nSampleRate, 0, 0, nullptr, &effectChain));

	// Set default FX params
	XAUDIO2FX_REVERB_PARAMETERS		native				= {};
	ReverbConvertI3DL2ToNative(&g_PRESET_PARAMS[0], &native);
	pSubmixVoice->SetEffectParameters(0, &native, sizeof(native));

	// Initialize X3DAudio
	// Speaker geometry configuration on the final mix, specifies assignment of channels to speaker positions, defined as per WAVEFORMATEXTENSIBLE.dwChannelMask
	// SpeedOfSound - speed of sound in user-defined world units/second, used only for doppler calculations, it must be >= FLT_MIN
	stacxpr	float					SPEEDOFSOUND		= X3DAUDIO_SPEED_OF_SOUND;

	X3DAudioInitialize(dwChannelMask, SPEEDOFSOUND, X3DInstance);

	vListenerPos				= {};
	vEmitterPos					= {0, 0, float(::ZMAX)};

	fListenerAngle				= 0;
	DisableListenerCone			= false;
	fUseInnerRadius				= TRUE;
	UseRedirectToLFE			= ((dwChannelMask & SPEAKER_LOW_FREQUENCY) != 0);

	// Setup 3D audio structs
	Listener.Position			= {vListenerPos.x, vListenerPos.y, vListenerPos.z};
	Listener.OrientFront		= {1, 0, 0};
	Listener.OrientTop			= {0, 1, 0};

	Listener.pCone				= (X3DAUDIO_CONE*)&Listener_DirectionalCone;

	EmitterCone					= {};

	// Setting the inner cone angles to X3DAUDIO_2PI and outer cone other than 0 causes the emitter to act like a point emitter using the INNER cone settings only.
	// Setting the outer cone angles to zero causes the emitter to act like a point emitter using the OUTER cone settings only.
	Emitter.pCone				= &EmitterCone;
	Emitter.pCone->OuterVolume	= 1.0f;
	Emitter.pCone->OuterLPF		= 1.0f;
	Emitter.pCone->OuterReverb	= 1.0f;

	Emitter.Position			= {vEmitterPos.x, vEmitterPos.y, vEmitterPos.z};

	Emitter.OrientFront			= {-1, 0, 0};
	Emitter.OrientTop			= {0, 1, 0};

	Emitter.ChannelCount		= ::INPUTCHANNELS;
	Emitter.ChannelRadius		= 1.0f;

	static_assert(::INPUTCHANNELS == 1 || Emitter.pChannelAzimuths != nullptr, "Multi-channel sources require emitter azimuths");
	// For examples of how to configure emitter azimuths for multi-channel sources, see DirectX Tool Kit for Audio helper method AudioEmitter::EnableDefaultMultiChannel http://go.microsoft.com/fwlink/?LinkId=248929
	// Use of Inner radius allows for smoother transitions as a sound travels directly through, above, or below the listener. It also may be used to give elevation cues.
	Emitter.InnerRadius			= 2.0f;
	Emitter.InnerRadiusAngle	= X3DAUDIO_PI / 4.0f;;

	Emitter.pVolumeCurve		= (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
	Emitter.pLFECurve			= (X3DAUDIO_DISTANCE_CURVE*)&Emitter_LFE_Curve;
	Emitter.pLPFDirectCurve		= nullptr; // use default curve
	Emitter.pLPFReverbCurve		= nullptr; // use default curve
	Emitter.pReverbCurve		= (X3DAUDIO_DISTANCE_CURVE*)&Emitter_Reverb_Curve;
	Emitter.CurveDistanceScaler	= 14.0f;
	Emitter.DopplerScaler		= 1.0f;

	DSP.SrcChannelCount			= ::INPUTCHANNELS;
	DSP.DstChannelCount			= Channels;
	DSP.pMatrixCoefficients		= matrixCoefficients;

	// --
	XAudio						= xAudio2;
	Initialized					= true;	// Done
	return S_OK;
}


//-----------------------------------------------------------------------------
// Prepare a looping wave
//-----------------------------------------------------------------------------
::gpk::error_t				AUDIO_STATE::PrepareAudio		(const char* wavname)	{
	rees_if(!Initialized);
	safe_destroySourceVoice(SourceVoice);

	// Search for media
	char							strFilePath[ 4096 ]				= {};
	sprintf_s(strFilePath, "./%s", wavname);

	// Read in the wave file
	const WAVEFORMATEX				* pwfx							= 0;
	::gpk::vcu8						audioView						= {};
	gpk_hrcall(DirectX::LoadWAVAudioFromFile(::gpk::vcs{strFilePath}, WaveData, &pwfx, audioView));

	assert(pwfx->nChannels == ::INPUTCHANNELS);

	// Play the wave using a source voice that sends to both the submix and mastering voices
	XAUDIO2_SEND_DESCRIPTOR			sendDescriptors[2];
	sendDescriptors[0]			= {XAUDIO2_SEND_USEFILTER, pMasterVoice}; // LPF direct-path
	sendDescriptors[1]			= {XAUDIO2_SEND_USEFILTER, pSubmixVoice};// LPF reverb-path -- omit for better performance at the cost of less realistic occlusion
	const XAUDIO2_VOICE_SENDS		sendList						= { 2, sendDescriptors };

	// create the source voice
	gpk_hrcall(XAudio->CreateSourceVoice(&SourceVoice, pwfx, 0, 2.0f, nullptr, &sendList));

	// Submit the wave sample data using an XAUDIO2_BUFFER structure
	XAUDIO2_BUFFER					buffer							= {};
	buffer.pAudioData			= audioView.begin();
	buffer.Flags				= XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes			= audioView.size();
	buffer.LoopCount			= XAUDIO2_LOOP_INFINITE;

	gpk_hrcall(SourceVoice->SubmitSourceBuffer(&buffer));
	gpk_hrcall(SourceVoice->Start());

	Playing						= true;
	FrameToApply3DAudio			= 0;
	return S_OK;
}

//-----------------------------------------------------------------------------
// Perform per-frame update of audio
//-----------------------------------------------------------------------------
::gpk::error_t	AUDIO_STATE::UpdateAudio( double fElapsedTime ) {
	(++FrameToApply3DAudio)			&= 1;

	if(FrameToApply3DAudio)
		return 0; 

	if( vListenerPos.x != Listener.Position.x
	 || vListenerPos.z != Listener.Position.z 
	) { // Calculate listener orientation in x-z plane
		::gpk::n3f						vDelta				= vListenerPos - ::gpk::n3f{Listener.Position.x, Listener.Position.y, Listener.Position.z};
		fListenerAngle				= float( atan2( vDelta.x, vDelta.z ) );
		vDelta.y					= 0.0f;
		::gpk::n3f						delta				= { vDelta.x,vDelta.y,vDelta.z};
		delta.Normalize();
		Listener.OrientFront.x		= delta.x;
		Listener.OrientFront.y		= 0.f;
		Listener.OrientFront.z		= delta.z;
	}

	Listener.pCone				= DisableListenerCone ? 0 : (X3DAUDIO_CONE*)&Listener_DirectionalCone;
	if (fUseInnerRadius) {
		Emitter.InnerRadius			= 2.0f;
		Emitter.InnerRadiusAngle	= X3DAUDIO_PI/4.0f;
	}
	else {
		Emitter.InnerRadius			= 0.0f;
		Emitter.InnerRadiusAngle	= 0.0f;
	}

	if(0 < fElapsedTime) {
		::gpk::n3f						lVelocity		= ( vListenerPos - ::gpk::n3f{Listener.Position.x, Listener.Position.y, Listener.Position.z} ) / fElapsedTime;
		Listener.Position			= {vListenerPos.x, vListenerPos.y, vListenerPos.z};
		Listener.Velocity			= {lVelocity.x, lVelocity.y, lVelocity.z};

		::gpk::n3f						eVelocity		= ( vEmitterPos - ::gpk::n3f{Emitter.Position.x, Emitter.Position.y, Emitter.Position.z} ) / fElapsedTime;
		Emitter.Position			= {vEmitterPos.x, vEmitterPos.y, vEmitterPos.z};
		Emitter.Velocity			= {eVelocity.x, eVelocity.y, eVelocity.z};
	}

	uint32_t						dwCalcFlags		= X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
		| X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
		| X3DAUDIO_CALCULATE_REVERB;

	if (UseRedirectToLFE) {
		dwCalcFlags |= X3DAUDIO_CALCULATE_REDIRECT_TO_LFE;  // On devices with an LFE channel, allow the mono source data to be routed to the LFE destination channel.
	}

	X3DAudioCalculate( X3DInstance, &Listener, &Emitter, dwCalcFlags, &DSP );

	IXAudio2SourceVoice				* voice			= SourceVoice;
	if( voice ) {
		voice->SetFrequencyRatio(FrequencyRatio * DSP.DopplerFactor);
		// Apply X3DAudio generated DSP settings to XAudio2
		voice->SetOutputMatrix(pMasterVoice, ::INPUTCHANNELS, Channels, matrixCoefficients);
		voice->SetOutputMatrix(pSubmixVoice, 1, 1, &DSP.ReverbLevel);

		const	XAUDIO2_FILTER_PARAMETERS	filterDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI/6.0f * DSP.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
		voice->SetOutputFilterParameters(pMasterVoice, &filterDirect);

		const	XAUDIO2_FILTER_PARAMETERS	filterReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI/6.0f * DSP.LPFReverbCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
		voice->SetOutputFilterParameters(pSubmixVoice, &filterReverb);
	}

	for(uint32_t iSound = 0; iSound < Sounds.size(); ++iSound) {
		SWave					* pSound		= Sounds[iSound];
		if(0 == pSound || false == pSound->Playing)
			continue;

		SWave					& sound			= *pSound;
		gpk_necs(sound.UpdateAudio(pMasterVoice, pSubmixVoice, X3DInstance, Listener, fElapsedTime, UseRedirectToLFE));
	}

	return S_OK;
}

::gpk::error_t	AUDIO_STATE::PauseAudio		(bool resume)		{
	if(resume && false == Playing) 
		XAudio->StartEngine();
	else if(Playing && false == resume) 
		XAudio->StopEngine();

	Playing = resume;
	return 0; 
}

::gpk::error_t	AUDIO_STATE::CleanupAudio	()					{
	safe_destroySourceVoice(SourceVoice);	
	safe_destroyVoice(pSubmixVoice);	
	safe_destroyVoice(pMasterVoice);
	if(XAudio) {
		XAudio->StopEngine();
		XAudio			= {};
	}
	CoUninitialize();
	return 0;
}

::gpk::error_t	enumerateAudio	(_In_ IXAudio2* pXaudio2, _Inout_ ::gpk::aobj<AudioDevice> & list) {
	HRESULT					hr				= {};
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
	UNREFERENCED_PARAMETER( pXaudio2 );

#if defined(__cplusplus_winrt )

	// Enumerating with WinRT using C++/CX
	using namespace concurrency;
	using Windows::Devices::Enumeration::DeviceClass;
	using Windows::Devices::Enumeration::DeviceInformation;
	using Windows::Devices::Enumeration::DeviceInformationCollection;
 
	auto					operation		= DeviceInformation::FindAllAsync(DeviceClass::AudioRender);
	auto					task			= create_task( operation );

	task.then([&list]( DeviceInformationCollection^ devices) {
		for(unsigned i=0; i < devices->Size; ++i) {
			using Windows::Devices::Enumeration::DeviceInformation;
 
			DeviceInformation		^d			= devices->GetAt(i);

			AudioDevice				device;
			device.deviceId		= d->Id->Data();
			device.description	= d->Name->Data();
			list.emplace_back(device);
		}
	});

	task.wait();
 
	if ( list.empty() )
		return S_FALSE;

#else
	// Enumerating with WinRT using WRL
	using namespace Microsoft::WRL::Wrappers;
	using namespace ABI::Windows::Foundation;
	using namespace ABI::Windows::Foundation::Collections;
	using namespace ABI::Windows::Devices::Enumeration;

	RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	gpk_hrcall(initialize);

	::gpk::pcom<IDeviceInformationStatics> diFactory;
	gpk_hrcall(ABI::Windows::Foundation::GetActivationFactory( HStringReference(RuntimeClass_Windows_Devices_Enumeration_DeviceInformation).Get(), &diFactory));

	Event findCompleted( CreateEventEx( nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, WRITE_OWNER | EVENT_ALL_ACCESS ) );
	if ( !findCompleted.IsValid() )
		return HRESULT_FROM_WIN32( GetLastError() );

	auto callback = Microsoft::WRL::Callback<IAsyncOperationCompletedHandler<DeviceInformationCollection*>>(
		[&findCompleted,list]( IAsyncOperation<DeviceInformationCollection*>* /*aDevices*/, AsyncStatus /*status*/ ) -> HRESULT
	{
		SetEvent( findCompleted.Get() );
		return S_OK;
	});

	::gpk::pcom<IAsyncOperation<DeviceInformationCollection*>> operation;
	gpk_hrcall(diFactory->FindAllAsyncDeviceClass( DeviceClass_AudioRender, &operation));
	operation->put_Completed( callback.Get() );
	WaitForSingleObject( findCompleted.Get(), INFINITE );

	::gpk::pcom<IVectorView<DeviceInformation*>> devices;
	operation->GetResults( &devices);

	unsigned int count = 0;
	gpk_hrcall(devices->get_Size(&count));

	if ( !count )
		return S_FALSE;

	for( unsigned int j = 0; j < count; ++j ) {
		::gpk::pcom<IDeviceInformation>	deviceInfo;
		ce_if(FAILED(hr = devices->GetAt(j, &deviceInfo)), "0x%X", hr); 

		HString					id;
		HString					name;

		deviceInfo->get_Id  (id  .GetAddressOf());
		deviceInfo->get_Name(name.GetAddressOf());

		AudioDevice				device		= {};
		device.Id			= id.GetRawBuffer( nullptr );
		device.Description	= name.GetRawBuffer( nullptr );
		list.push_back(device);
	}

	return S_OK;

#endif 

#elif defined(USING_XAUDIO2_REDIST)

	// Enumeration for XAudio 2.9 down-level on Windows 7
	::gpk::pcom<IMMDeviceEnumerator> devEnum;
	gpk_hrcall(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(devEnum.GetAddressOf())));

	::gpk::pcom<IMMDeviceCollection> devices;
	gpk_hrcall(devEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices));

	UINT count = 0;
	gpk_hrcall(devices->GetCount(&count));

	if (!count)
		return S_FALSE;

	for (UINT j = 0; j < count; ++j) {
		::gpk::pcom<IMMDevice> endpoint;
		ce_if(FAILED(hr = devices->Item(j, endpoint.GetAddressOf())), "0x%X", hr);

		LPWSTR id = nullptr;
		gpk_hrcall(endpoint->GetId(&id));

		AudioDevice device;
		device.deviceId = id;
		CoTaskMemFree(id);

		::gpk::pcom<IPropertyStore> props;
		gpk_hrcall(endpoint->OpenPropertyStore(STGM_READ, props.GetAddressOf()));

		PROPVARIANT var;
		PropVariantInit(&var);

		gpk_hrcall(hr = props->GetValue(PKEY_Device_FriendlyName, &var));
		if (var.vt != VT_LPWSTR)
			return hr;

		device.description = var.pwszVal;
		PropVariantClear(&var);

		list.emplace_back(device);
	}

#else // USING_XAUDIO2_7_DIRECTX

	// Enumerating with XAudio 2.7
	uint32_t						count			= 0;
	gpk_hrcall(pXaudio2->GetDeviceCount(&count));
	if(!count)
		return S_FALSE;

	list.reserve(count);

	for(UINT32 j = 0; j < count; ++j) {
		XAUDIO2_DEVICE_DETAILS			details			= {};
		ce_if(FAILED(hr = pXaudio2->GetDeviceDetails(j, &details)), "0x%X", hr);
		AudioDevice						device;
		device.deviceId				= details.DeviceID;
		device.description			= details.DisplayName;
		list.emplace_back(device);
	}

#endif

	return S_OK;
}
