#define WIN32_LEAN_AND_MEAN
#include "XAudio2Versions.h"

#include "gpk_array.h"
#include "gpk_coord.h"

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

HRESULT EnumerateAudio(_In_ IXAudio2* pXaudio2, _Inout_ ::gpk::array_obj<AudioDevice> & list);

struct WAVE_STATE
{
	::gpk::array_pod<uint8_t>			WaveData;
	IXAudio2SourceVoice					* pSourceVoice          = 0;

	X3DAUDIO_EMITTER					Emitter                 = {};
	X3DAUDIO_CONE						EmitterCone             = {};
	::gpk::SCoord3<float>				EmitterPos              = {};
	FLOAT32								EmitterAzimuths			[INPUTCHANNELS]                     = {};
	FLOAT32								MatrixCoefficients		[INPUTCHANNELS * OUTPUTCHANNELS]    = {};

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
	Microsoft::WRL::ComPtr<IXAudio2>	pXAudio2;
	IXAudio2MasteringVoice				* pMasterVoice			= 0;
	IXAudio2SubmixVoice					* pSubmixVoice			= 0;
	Microsoft::WRL::ComPtr<IUnknown>	pVolumeLimiter;
	Microsoft::WRL::ComPtr<IUnknown>	pReverbEffect;

	::gpk::array_pod<uint8_t>			WaveData;
	IXAudio2SourceVoice					* pSourceVoice			= 0;

	// 3D
	X3DAUDIO_HANDLE						x3DInstance				= {};
	int									nFrameToApply3DAudio	= 0;

	uint32_t							ChannelMask				= 0;
	UINT32								nChannels				= 0;
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


	HRESULT InitAudio();
	HRESULT PrepareAudio( const char* wavname );
	HRESULT UpdateAudio( double fElapsedTime );
	HRESULT SetReverb( int nReverb );
	HRESULT PauseAudio( bool resume );
	HRESULT CleanupAudio();

	int32_t								PlayWave				(const char* wavname);

};


#endif // AUDIO_H_29834928374