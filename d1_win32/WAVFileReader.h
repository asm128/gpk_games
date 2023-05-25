// Functions for loading WAV audio files. Copyright (c) Microsoft Corporation. Licensed under the MIT License.
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
#include "gpk_ptr.h"
#include "gpk_array.h"

#include <objbase.h>

#include <mmreg.h>

#ifndef WAVFILEREADER_H_02397492830
#define WAVFILEREADER_H_02397492830

namespace DirectX
{
	HRESULT					LoadWAVAudioInMemory	(::gpk::vcu8 wavData, _Outptr_ const WAVEFORMATEX* * wfx, ::gpk::vcu8 & audioView) noexcept;
	HRESULT					LoadWAVAudioFromFile	(::gpk::vcc szFileName, ::gpk::au8 & wavData, const WAVEFORMATEX* * wfx, ::gpk::vcu8 & audioView) noexcept;
							
	struct WAVData {		
		const WAVEFORMATEX		* WaveFormat			= 0;
		::gpk::vcu8				AudioView				= {};
		uint32_t				LoopStart				= 0;
		uint32_t				LoopLength				= 0;
		const uint32_t			* Seek					= 0;	// Note: XMA Seek data is Big-Endian
		uint32_t				SeekCount				= 0;
	};						
							
	HRESULT					LoadWAVAudioInMemoryEx	(::gpk::vcu8 wavData, WAVData & result) noexcept;
	HRESULT					LoadWAVAudioFromFileEx	(::gpk::vcc szFileName, ::gpk::au8 & wavData, WAVData & result) noexcept;
}

#endif // WAVFILEREADER_H_02397492830