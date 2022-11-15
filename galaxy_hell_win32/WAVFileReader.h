//--------------------------------------------------------------------------------------
// File: WAVFileReader.h
//
// Functions for loading WAV audio files
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------


#include "gpk_ptr.h"
#include "gpk_array.h"

#include <objbase.h>

#include <mmreg.h>

#ifndef WAVFILEREADER_H_02397492830
#define WAVFILEREADER_H_02397492830

namespace DirectX
{
    HRESULT LoadWAVAudioInMemory(::gpk::view_array<const uint8_t> wavData, _Outptr_ const WAVEFORMATEX** wfx, ::gpk::view_array<const uint8_t>& audioView) noexcept;
    HRESULT LoadWAVAudioFromFile(::gpk::view_array<const wchar_t> szFileName, _Inout_ ::gpk::array_pod<uint8_t>& wavData, _Outptr_ const WAVEFORMATEX** wfx, ::gpk::view_array<const uint8_t>& audioView) noexcept;

    struct WAVData {
        const WAVEFORMATEX                  * wfx               = 0;
        ::gpk::view_array<const uint8_t>    AudioView           = {};
        uint32_t                            loopStart           = 0;
        uint32_t                            loopLength          = 0;
        const uint32_t                      * seek              = 0;       // Note: XMA Seek data is Big-Endian
        uint32_t                            seekCount           = 0;
    };

    HRESULT LoadWAVAudioInMemoryEx(::gpk::view_array<const uint8_t> wavData, _Out_ WAVData& result) noexcept;
    HRESULT LoadWAVAudioFromFileEx(::gpk::view_array<const wchar_t> szFileName, _Inout_ ::gpk::array_pod<uint8_t>& wavData, _Out_ WAVData& result) noexcept;
}

#endif // WAVFILEREADER_H_02397492830