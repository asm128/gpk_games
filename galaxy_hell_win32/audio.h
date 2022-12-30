#define WIN32_LEAN_AND_MEAN
#include "XAudio2Versions.h"
#include "WAVFileReader.h"

#include "gpk_array.h"
#include "gpk_coord.h"
#include "gpk_ptr.h"
#include "gpk_complus.h"

#include <wrl/client.h>

#ifndef USING_XAUDIO2_7_DIRECTX
#pragma comment(lib,"runtimeobject.lib")
#include <Windows.Devices.Enumeration.h>
#include <wrl.h>
#include <ppltasks.h>
#endif

#ifdef USING_XAUDIO2_REDIST
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#endif

#ifndef AUDIO_H_29834928374
#define AUDIO_H_29834928374

static constexpr uint8_t            INPUTCHANNELS       = 1;// number of source channels
static constexpr uint8_t            OUTPUTCHANNELS      = 8;// maximum number of destination channels supported in this sample

static constexpr uint8_t            NUM_PRESETS         = 30;

// Constants to define our world space
static constexpr int32_t           XMIN                 = -10;
static constexpr int32_t           XMAX                 = 10;
static constexpr int32_t           ZMIN                 = -10;
static constexpr int32_t           ZMAX                 = 10;

//--------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------
struct AudioDevice
{
	std::wstring deviceId;
	std::wstring description;
};

HRESULT EnumerateAudio(_In_ IXAudio2* pXaudio2, _Inout_ ::gpk::aobj<AudioDevice> & list);

struct WAVE_STATE
{
	::gpk::pobj<::gpk::apod<uint8_t>>	WaveData;
	IXAudio2SourceVoice							* pSourceVoice			= 0;

	X3DAUDIO_EMITTER							Emitter					= {};
	X3DAUDIO_CONE								EmitterCone				= {};
	::gpk::SCoord3<float>						EmitterPos				= {};
	float										EmitterAzimuths			[INPUTCHANNELS]                     = {};
	float										MatrixCoefficients		[INPUTCHANNELS * OUTPUTCHANNELS]    = {};

	uint32_t									ChannelMask				= 0;
	uint32_t									nChannels				= 0;
	X3DAUDIO_DSP_SETTINGS						dspSettings				= {};
	bool										Playing					= false;

	HRESULT										PrepareAudio			(const char* wavname, IXAudio2*	pXAudio2, IXAudio2MasteringVoice * pMasterVoice, IXAudio2SubmixVoice * pSubmixVoice) {
		if( pSourceVoice ) {
			pSourceVoice->Stop( 0 );
			pSourceVoice->DestroyVoice();
			pSourceVoice = 0;
		}

		// Search for media
		char											strFilePath[ 4096 ]		= {};
		sprintf_s(strFilePath, "./%s", wavname);
		wchar_t											wstrFilePath[ 4096 ]	= {};
		size_t											count					= 0;
		mbstowcs_s(&count, wstrFilePath, strFilePath, 4095);

		// Read in the wave file
		const WAVEFORMATEX								* pwfx					= 0;
		::gpk::view_array<const uint8_t>				audioView				= {};
		WaveData.create();
		gpk_hrcall(DirectX::LoadWAVAudioFromFile(::gpk::vcs{strFilePath}, *WaveData, &pwfx, audioView));

		// Play the wave using a source voice that sends to both the submix and mastering voices
		XAUDIO2_SEND_DESCRIPTOR							sendDescriptors[2]		= {};
		sendDescriptors[0]							= {XAUDIO2_SEND_USEFILTER, pMasterVoice}; // LPF direct-path
		sendDescriptors[1]							= {XAUDIO2_SEND_USEFILTER, pSubmixVoice};// LPF reverb-path -- omit for better performance at the cost of less realistic occlusion
		const XAUDIO2_VOICE_SENDS						sendList				= { 2, sendDescriptors };

		// create the source voice
		gpk_hrcall(pXAudio2->CreateSourceVoice(&pSourceVoice, pwfx, 0, 2.0f, nullptr, &sendList));

		// Submit the wave sample data using an XAUDIO2_BUFFER structure
		XAUDIO2_BUFFER									buffer					= {};
		buffer.pAudioData							= audioView.begin();
		buffer.Flags								= XAUDIO2_END_OF_STREAM;
		buffer.AudioBytes							= audioView.size();
		buffer.LoopCount							= XAUDIO2_LOOP_INFINITE;

		gpk_hrcall(pSourceVoice->SubmitSourceBuffer(&buffer));
		gpk_hrcall(pSourceVoice->Start(0));
		Playing										= true;
		return S_OK;
	}
};

//-----------------------------------------------------------------------------
// Struct to hold audio game state
//-----------------------------------------------------------------------------
struct AUDIO_STATE
{
	bool                                bInitialized           = false;

	// XAudio2
#ifdef USING_XAUDIO2_7_DIRECTX
	HMODULE								mXAudioDLL;
#endif
	::gpk::ptr_com<IXAudio2>			pXAudio2				= {};

	IXAudio2MasteringVoice				* pMasterVoice			= 0;
	IXAudio2SubmixVoice					* pSubmixVoice			= 0;

	::gpk::ptr_com<IUnknown>			pVolumeLimiter;
	::gpk::ptr_com<IUnknown>			pReverbEffect;
	// 3D
	X3DAUDIO_HANDLE						x3DInstance				= {};
	int									nFrameToApply3DAudio	= 0;

	::gpk::apod<uint8_t>			WaveData;
	IXAudio2SourceVoice					* pSourceVoice			= 0;


	uint32_t							ChannelMask				= 0;
	uint32_t							nChannels				= 0;
	X3DAUDIO_DSP_SETTINGS				dspSettings				= {};

	X3DAUDIO_LISTENER					listener				= {};
	X3DAUDIO_EMITTER					emitter					= {};
	X3DAUDIO_CONE						emitterCone				= {};

	::gpk::SCoord3<float>				vListenerPos			= {};
	::gpk::SCoord3<float>				vEmitterPos				= {};
	float								fListenerAngle			= (float)(::gpk::math_pi_2 * .5);
	bool								fUseListenerCone		= true;
	bool								fUseInnerRadius			= true;
	bool								fUseRedirectToLFE		= 0;
	bool								Playing					= false;
	float								FrequencyRatio			= 1.0f;
#ifdef USING_XAUDIO2_7_DIRECTX
	uint32_t							SampleRateOriginal		= 0;
	uint32_t							SampleRateCurrent		= 0;
#endif

	FLOAT32								emitterAzimuths			[INPUTCHANNELS]                     = {};
	FLOAT32								matrixCoefficients		[INPUTCHANNELS * OUTPUTCHANNELS]    = {};


	HRESULT								InitAudio				();
	HRESULT								PrepareAudio			( const char* wavname );
	HRESULT								UpdateAudio				( double fElapsedTime );
	HRESULT								SetReverb				( int nReverb );
	HRESULT								PauseAudio				( bool resume );
	HRESULT								CleanupAudio			();

	int32_t								PlayWave				(const char* wavname);

};


#endif // AUDIO_H_29834928374