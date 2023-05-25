#define WIN32_LEAN_AND_MEAN

#include "XAudio2Versions.h"
#include "WAVFileReader.h"

#include "gpk_array.h"
#include "gpk_n3.h"
#include "gpk_ptr.h"
#include "gpk_complus.h"

#include <wrl/client.h>

#ifndef USING_XAUDIO2_7_DIRECTX
#	pragma comment(lib, "runtimeobject.lib")
#	include <Windows.Devices.Enumeration.h>
#	include <wrl.h>
#	include <ppltasks.h>
#endif

#ifdef USING_XAUDIO2_REDIST
#	include <mmdeviceapi.h>
#	include <Functiondiscoverykeys_devpkey.h>
#endif

#ifndef AUDIO_H_29834928374
#define AUDIO_H_29834928374

stacxpr	uint8_t				INPUTCHANNELS			= 1;	// number of source channels
stacxpr	uint8_t				OUTPUTCHANNELS			= 8;	// maximum number of destination channels supported in this sample
//stacxpr	uint8_t				NUM_PRESETS				= 30;

// Constants to define our world space
stacxpr	int32_t				XMIN					= -10;
stacxpr	int32_t				XMAX					= 10;
stacxpr	int32_t				ZMIN					= -10;
stacxpr	int32_t				ZMAX					= 10;

::gpk::error_t				audioSetReverb			(IXAudio2SubmixVoice * submixVoice, uint32_t nReverb);

//-------------------------------------------------------------------------
// Forward declaration
//-------------------------------------------------------------------------
struct AudioDevice {
	std::wstring				Id;
	std::wstring				Description;
};

// Enumerate audio end-points
::gpk::error_t				enumerateAudio			(_In_ IXAudio2 * pXaudio2, _Inout_ ::gpk::aobj<AudioDevice> & list);


struct SSourceSound {
	IXAudio2SourceVoice			* pVoice				= {};
};

#define safe_destroySourceVoice(pVoice)	do {								\
		if(pVoice) { pVoice->Stop(); pVoice->DestroyVoice(); pVoice = {}; }	\
	} while(0)

struct SWave {
	::gpk::pobj<::gpk::au8>		WaveData				= {};
	IXAudio2SourceVoice			* SourceVoice			= 0;

	X3DAUDIO_EMITTER			Emitter					= {};
	X3DAUDIO_CONE				EmitterCone				= {};
	::gpk::n3f					EmitterPos				= {};
	float						EmitterAzimuths			[::INPUTCHANNELS]						= {};
	float						MatrixCoefficients		[::INPUTCHANNELS * ::OUTPUTCHANNELS]	= {};

	uint32_t					ChannelMask				= 0;
	uint32_t					Channels				= 0;
	X3DAUDIO_DSP_SETTINGS		DSPSettings				= {};
	bool						Playing					= false;
	bool						DisableInnerRadius		= false;
	bool						DisableListenerCone		= false;
	float						FrequencyRatio			= 1.0f;

	::gpk::error_t				SetReverb				(int nReverb);
	::gpk::error_t				PauseAudio				(bool resume);
	::gpk::error_t				PrepareAudio			(const char * wavname, IXAudio2 * pXAudio2, IXAudio2MasteringVoice * pMasterVoice, IXAudio2SubmixVoice * pSubmixVoice) {
		safe_destroySourceVoice(SourceVoice);

		// Search for media
		char							strFilePath	[4096]		= {};
		wchar_t							wstrFilePath[4096]		= {};
		size_t							count					= 0;
		mbstowcs_s(&count, wstrFilePath, strFilePath, ::gpk::min((uint32_t)sprintf_s(strFilePath, "./%s", wavname), ::gpk::size(strFilePath) - 1));

		// Read in the wave file
		const WAVEFORMATEX				* pwfx					= 0;
		::gpk::vcu8						audioView				= {};
		WaveData.create();
		gpk_hrcall(DirectX::LoadWAVAudioFromFile(::gpk::vcs{strFilePath}, *WaveData, &pwfx, audioView));

		// Play the wave using a source voice that sends to both the submix and mastering voices
		XAUDIO2_SEND_DESCRIPTOR			sendDescriptors[2]		= {};
		sendDescriptors[0]			= {XAUDIO2_SEND_USEFILTER, pMasterVoice};	// LPF direct-path
		sendDescriptors[1]			= {XAUDIO2_SEND_USEFILTER, pSubmixVoice};	// LPF reverb-path -- omit for better performance at the cost of less realistic occlusion
		const XAUDIO2_VOICE_SENDS		sendList				= {2, sendDescriptors};

		gpk_hrcall(pXAudio2->CreateSourceVoice(&SourceVoice, pwfx, 0, 2.0f, nullptr, &sendList));

		XAUDIO2_BUFFER					buffer					= {};
		buffer.pAudioData			= audioView.begin();
		buffer.Flags				= XAUDIO2_END_OF_STREAM;
		buffer.AudioBytes			= audioView.size();
		buffer.LoopCount			= XAUDIO2_LOOP_INFINITE;
		gpk_hrcall(SourceVoice->SubmitSourceBuffer(&buffer));
		gpk_hrcall(SourceVoice->Start());
		Playing						= true;
		return S_OK;
	}

	::gpk::error_t				UpdateAudio				(IXAudio2MasteringVoice * pMasterVoice, IXAudio2SubmixVoice * pSubmixVoice, X3DAUDIO_HANDLE & x3DInstance, X3DAUDIO_LISTENER & listener, double fElapsedTime, bool useRedirectToLFE)		{
		if (DisableInnerRadius) {														
			Emitter.InnerRadius			= 0.0f;
			Emitter.InnerRadiusAngle	= 0.0f;
		}
		else {
			Emitter.InnerRadius			= 2.0f;
			Emitter.InnerRadiusAngle	= X3DAUDIO_PI/4.0f;
		}

		if(0 < fElapsedTime) {
			::gpk::n3f						eVelocity				= (EmitterPos - ::gpk::n3f{Emitter.Position.x, Emitter.Position.y, Emitter.Position.z} ) / fElapsedTime;
			Emitter.Position = {EmitterPos.x, EmitterPos.y, EmitterPos.z};
			Emitter.Velocity = {eVelocity.x, eVelocity.y, eVelocity.z};
		}

		uint32_t						dwCalcFlags				= X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
			| X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
			| X3DAUDIO_CALCULATE_REVERB;

		if (useRedirectToLFE) {
			dwCalcFlags					|= X3DAUDIO_CALCULATE_REDIRECT_TO_LFE;  // On devices with an LFE channel, allow the mono source data to be routed to the LFE destination channel.
		}

		X3DAudioCalculate(x3DInstance, &listener, &Emitter, dwCalcFlags, &DSPSettings);

		IXAudio2SourceVoice				* voice					= SourceVoice;
		if( voice ) {
			voice->SetFrequencyRatio(FrequencyRatio * DSPSettings.DopplerFactor);
			// Apply X3DAudio generated DSP settings to XAudio2
			voice->SetOutputMatrix(pMasterVoice, ::INPUTCHANNELS, Channels, MatrixCoefficients);
			voice->SetOutputMatrix(pSubmixVoice, 1, 1, &DSPSettings.ReverbLevel);

			XAUDIO2_FILTER_PARAMETERS		filterDirect			= { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI/6.0f * DSPSettings.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
			voice->SetOutputFilterParameters(pMasterVoice, &filterDirect);

			XAUDIO2_FILTER_PARAMETERS		filterReverb			= { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI/6.0f * DSPSettings.LPFReverbCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
			voice->SetOutputFilterParameters(pSubmixVoice, &filterReverb);
		}
		return S_OK;
	}

};


#define safe_destroyVoice(pVoice) do {						\
		if(pVoice) { pVoice->DestroyVoice(); pVoice = {}; }	\
	} while(0)

//-----------------------------------------------------------------------------
// Struct to hold audio game state
//-----------------------------------------------------------------------------
struct AUDIO_STATE {
	::gpk::apobj<SWave>			Sounds;

	bool						Initialized				= false;

	// XAudio2
#ifdef USING_XAUDIO2_7_DIRECTX
	HMODULE						XAudioDLL				= {};
#endif
	::gpk::pcom<IXAudio2>		XAudio					= {};

	IXAudio2MasteringVoice		* pMasterVoice			= 0;
	IXAudio2SubmixVoice			* pSubmixVoice			= 0;

	::gpk::pcom<IUnknown>		VolumeLimiter;
	::gpk::pcom<IUnknown>		ReverbEffect;
	// 3D
	X3DAUDIO_HANDLE				X3DInstance				= {};
	int							FrameToApply3DAudio		= 0;

	::gpk::au8					WaveData;
	IXAudio2SourceVoice			* SourceVoice			= 0;

	uint32_t					ChannelMask				= 0;
	uint32_t					Channels				= 0;
	X3DAUDIO_DSP_SETTINGS		DSP						= {};

	X3DAUDIO_LISTENER			Listener				= {};
	X3DAUDIO_EMITTER			Emitter					= {};
	X3DAUDIO_CONE				EmitterCone				= {};

	::gpk::n3f					vListenerPos			= {};
	::gpk::n3f					vEmitterPos				= {};
	float						fListenerAngle			= (float)(::gpk::math_pi_2 * .5);
	bool						DisableListenerCone		= false;
	bool						fUseInnerRadius			= true;
	bool						UseRedirectToLFE		= 0;
	bool						Playing					= false;
	float						FrequencyRatio			= 1.0f;
#ifdef USING_XAUDIO2_7_DIRECTX
	uint32_t					SampleRateOriginal		= 0;
	uint32_t					SampleRateCurrent		= 0;
#endif

	float						emitterAzimuths			[::INPUTCHANNELS]						= {};
	float						matrixCoefficients		[::INPUTCHANNELS * ::OUTPUTCHANNELS]	= {};

	inline	::gpk::error_t		SetReverb				(int nReverb)							{ return ::audioSetReverb(pSubmixVoice, nReverb); }

	::gpk::error_t				InitAudio				();
	::gpk::error_t				PrepareAudio			(const char* wavname);
	::gpk::error_t				UpdateAudio				(double fElapsedTime);
	::gpk::error_t				PauseAudio				(bool resume);
	::gpk::error_t				CleanupAudio			();

	int32_t						PlayWave				(const char* wavname);

};

#endif // AUDIO_H_29834928374