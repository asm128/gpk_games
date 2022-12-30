//--------------------------------------------------------------------------------------
// File: WAVFileReader.cpp
//
// Functions for loading WAV audio files
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cassert>

#include "WAVFileReader.h"

#include "gpk_windows.h"

using namespace DirectX;

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
				(static_cast<uint32_t>(static_cast<uint8_t>(ch0)) \
				| (static_cast<uint32_t>(static_cast<uint8_t>(ch1)) << 8) \
				| (static_cast<uint32_t>(static_cast<uint8_t>(ch2)) << 16) \
				| (static_cast<uint32_t>(static_cast<uint8_t>(ch3)) << 24))
#endif /* defined(MAKEFOURCC) */

namespace
{
	struct handle_closer { void operator()(HANDLE h) noexcept { if (h) CloseHandle(h); } };

	using ScopedHandle = std::unique_ptr<void, handle_closer>;

	inline HANDLE safe_handle(HANDLE h) noexcept { return (h == INVALID_HANDLE_VALUE) ? nullptr : h; }

	//---------------------------------------------------------------------------------
	// .WAV files
	//---------------------------------------------------------------------------------
	constexpr uint32_t FOURCC_RIFF_TAG        = MAKEFOURCC('R', 'I', 'F', 'F');
	constexpr uint32_t FOURCC_FORMAT_TAG      = MAKEFOURCC('f', 'm', 't', ' ');
	constexpr uint32_t FOURCC_DATA_TAG        = MAKEFOURCC('d', 'a', 't', 'a');
	constexpr uint32_t FOURCC_WAVE_FILE_TAG   = MAKEFOURCC('W', 'A', 'V', 'E');
	constexpr uint32_t FOURCC_XWMA_FILE_TAG   = MAKEFOURCC('X', 'W', 'M', 'A');
	constexpr uint32_t FOURCC_DLS_SAMPLE      = MAKEFOURCC('w', 's', 'm', 'p');
	constexpr uint32_t FOURCC_MIDI_SAMPLE     = MAKEFOURCC('s', 'm', 'p', 'l');
	constexpr uint32_t FOURCC_XWMA_DPDS       = MAKEFOURCC('d', 'p', 'd', 's');
	constexpr uint32_t FOURCC_XMA_SEEK        = MAKEFOURCC('s', 'e', 'e', 'k');

#pragma pack(push,1)
	struct RIFFChunk
	{
		uint32_t tag;
		uint32_t size;
	};

	struct RIFFChunkHeader
	{
		uint32_t tag;
		uint32_t size;
		uint32_t riff;
	};

	struct DLSLoop
	{
		static constexpr uint32_t LOOP_TYPE_FORWARD = 0x00000000;
		static constexpr uint32_t LOOP_TYPE_RELEASE = 0x00000001;

		uint32_t size;
		uint32_t loopType;
		uint32_t loopStart;
		uint32_t loopLength;
	};

	struct RIFFDLSSample
	{
		static constexpr uint32_t OPTIONS_NOTRUNCATION = 0x00000001;
		static constexpr uint32_t OPTIONS_NOCOMPRESSION = 0x00000002;

		uint32_t    size;
		uint16_t    unityNote;
		int16_t     fineTune;
		int32_t     gain;
		uint32_t    options;
		uint32_t    loopCount;
	};

	struct MIDILoop
	{
		static constexpr uint32_t LOOP_TYPE_FORWARD = 0x00000000;
		static constexpr uint32_t LOOP_TYPE_ALTERNATING = 0x00000001;
		static constexpr uint32_t LOOP_TYPE_BACKWARD = 0x00000002;

		uint32_t cuePointId;
		uint32_t type;
		uint32_t start;
		uint32_t end;
		uint32_t fraction;
		uint32_t playCount;
	};

	struct RIFFMIDISample
	{
		uint32_t        manufacturerId;
		uint32_t        productId;
		uint32_t        samplePeriod;
		uint32_t        unityNode;
		uint32_t        pitchFraction;
		uint32_t        SMPTEFormat;
		uint32_t        SMPTEOffset;
		uint32_t        loopCount;
		uint32_t        samplerData;
	};
#pragma pack(pop)

	static_assert(sizeof(RIFFChunk) == 8, "structure size mismatch");
	static_assert(sizeof(RIFFChunkHeader) == 12, "structure size mismatch");
	static_assert(sizeof(DLSLoop) == 16, "structure size mismatch");
	static_assert(sizeof(RIFFDLSSample) == 20, "structure size mismatch");
	static_assert(sizeof(MIDILoop) == 24, "structure size mismatch");
	static_assert(sizeof(RIFFMIDISample) == 36, "structure size mismatch");

	//---------------------------------------------------------------------------------
	const RIFFChunk* FindChunk(::gpk::view_array<const uint8_t> data, _In_ uint32_t tag) noexcept {
		const uint8_t* ptr = data.begin();
		const uint8_t* end = data.end();
		while (end > (ptr + sizeof(RIFFChunk))) {
			auto header = reinterpret_cast<const RIFFChunk*>(ptr);
			if (header->tag == tag)
				return header;

			auto const offset = header->size + sizeof(RIFFChunk);
			ptr += offset;
		}

		return nullptr;
	}


	//---------------------------------------------------------------------------------
	HRESULT WaveFindFormatAndData(::gpk::view_array<const uint8_t> wavData, _Outptr_ const WAVEFORMATEX** pwfx, ::gpk::view_array<const uint8_t>& data, _Out_ bool& dpds, _Out_ bool& seek) noexcept {
		dpds = seek = false;
		retval_gerror_if(E_FAIL, wavData.size() < (sizeof(RIFFChunk) * 2 + sizeof(uint32_t) + sizeof(WAVEFORMAT)), "%s", "");

		const uint8_t* wavEnd = wavData.begin() + wavData.size();

		// Locate RIFF 'WAVE'
		auto riffChunk = FindChunk(wavData, FOURCC_RIFF_TAG);
		retval_gerror_if(E_FAIL, (!riffChunk || riffChunk->size < 4), "%s", "");

		auto riffHeader = reinterpret_cast<const RIFFChunkHeader*>(riffChunk);
		retval_gerror_if(E_FAIL, (riffHeader->riff != FOURCC_WAVE_FILE_TAG && riffHeader->riff != FOURCC_XWMA_FILE_TAG), "%s", "");

		// Locate 'fmt '
		auto ptr = reinterpret_cast<const uint8_t*>(riffHeader) + sizeof(RIFFChunkHeader);
		retval_gerror_if(HRESULT_FROM_WIN32(ERROR_HANDLE_EOF), ((ptr + sizeof(RIFFChunk)) > wavEnd), "%s", "");

		auto fmtChunk = FindChunk({ptr, riffHeader->size}, FOURCC_FORMAT_TAG);
		retval_gerror_if(E_FAIL, (!fmtChunk || fmtChunk->size < sizeof(PCMWAVEFORMAT)), "%s", "");

		ptr = reinterpret_cast<const uint8_t*>(fmtChunk) + sizeof(RIFFChunk);
		retval_gerror_if(HRESULT_FROM_WIN32(ERROR_HANDLE_EOF), (ptr + fmtChunk->size > wavEnd), "%s", "");

		auto wf = reinterpret_cast<const WAVEFORMAT*>(ptr);

		// Validate WAVEFORMAT (focused on chunk size and format tag, not other data that XAUDIO2 will validate)
		switch (wf->wFormatTag)
		{
			case WAVE_FORMAT_PCM:
			case WAVE_FORMAT_IEEE_FLOAT:
				// Can be a PCMWAVEFORMAT (16 bytes) or WAVEFORMATEX (18 bytes)
				// We validiated chunk as at least sizeof(PCMWAVEFORMAT) above
				break;

			default:
			{
				if (fmtChunk->size < sizeof(WAVEFORMATEX))
					return E_FAIL;

				auto wfx = reinterpret_cast<const WAVEFORMATEX*>(ptr);

				if (fmtChunk->size < (sizeof(WAVEFORMATEX) + wfx->cbSize))
					return E_FAIL;

				switch (wfx->wFormatTag) {
					case WAVE_FORMAT_WMAUDIO2:
					case WAVE_FORMAT_WMAUDIO3:
						dpds = true;
						break;

					case  0x166 /*WAVE_FORMAT_XMA2*/: // XMA2 is supported by Xbox One
						retval_gerror_if(E_FAIL, ((fmtChunk->size < 52 /*sizeof(XMA2WAVEFORMATEX)*/) || (wfx->cbSize < 34 /*( sizeof(XMA2WAVEFORMATEX) - sizeof(WAVEFORMATEX) )*/)), "%s", "");
						seek = true;
						break;

					case WAVE_FORMAT_ADPCM:
						retval_gerror_if(E_FAIL, ((fmtChunk->size < (sizeof(WAVEFORMATEX) + 32)) || (wfx->cbSize < 32 /*MSADPCM_FORMAT_EXTRA_BYTES*/)), "%s", "");
						break;

					case WAVE_FORMAT_EXTENSIBLE:
						retval_gerror_if(E_FAIL, ((fmtChunk->size < sizeof(WAVEFORMATEXTENSIBLE)) || (wfx->cbSize < (sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)))), "%s", "")
						else {
							static const GUID s_wfexBase = { 0x00000000, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };

							auto wfex = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(ptr);

							retval_gerror_if(HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED), (memcmp(reinterpret_cast<const BYTE*>(&wfex->SubFormat) + sizeof(DWORD),
								reinterpret_cast<const BYTE*>(&s_wfexBase) + sizeof(DWORD), sizeof(GUID) - sizeof(DWORD)) != 0), "%s", "");

							switch (wfex->SubFormat.Data1) {
								case WAVE_FORMAT_PCM:
								case WAVE_FORMAT_IEEE_FLOAT:
									break;

								// MS-ADPCM and XMA2 are not supported as WAVEFORMATEXTENSIBLE

								case WAVE_FORMAT_WMAUDIO2:
								case WAVE_FORMAT_WMAUDIO3:
									dpds = true;
									break;

								default:
									return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
							}

						}
						break;

					default:
						return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
				}
			}
		}

		// Locate 'data'
		ptr = reinterpret_cast<const uint8_t*>(riffHeader) + sizeof(RIFFChunkHeader);
		retval_gerror_if(HRESULT_FROM_WIN32(ERROR_HANDLE_EOF), ((ptr + sizeof(RIFFChunk)) > wavEnd), "%s", "")
		auto dataChunk = FindChunk({ptr, riffChunk->size}, FOURCC_DATA_TAG);
		retval_gerror_if(HRESULT_FROM_WIN32(ERROR_INVALID_DATA), (!dataChunk || !dataChunk->size), "%s", "")
		ptr = reinterpret_cast<const uint8_t*>(dataChunk) + sizeof(RIFFChunk);
		retval_gerror_if(HRESULT_FROM_WIN32(ERROR_HANDLE_EOF), (ptr + dataChunk->size > wavEnd), "%s", "")
		*pwfx = reinterpret_cast<const WAVEFORMATEX*>(wf);
		data = {ptr, dataChunk->size};
		return S_OK;
	}


	//---------------------------------------------------------------------------------
	HRESULT WaveFindLoopInfo(::gpk::view_array<const uint8_t> wavData, _Out_ uint32_t* pLoopStart, _Out_ uint32_t* pLoopLength) noexcept {
		if (wavData.size() < (sizeof(RIFFChunk) + sizeof(uint32_t)))
			return E_FAIL;

		*pLoopStart = 0;
		*pLoopLength = 0;

		const uint8_t* wavEnd = wavData.end();

		// Locate RIFF 'WAVE'
		auto riffChunk = FindChunk(wavData, FOURCC_RIFF_TAG);
		if (!riffChunk || riffChunk->size < 4)
		{
			return E_FAIL;
		}

		auto riffHeader = reinterpret_cast<const RIFFChunkHeader*>(riffChunk);
		if (riffHeader->riff == FOURCC_XWMA_FILE_TAG)
		{
			// xWMA files do not contain loop information
			return S_OK;
		}

		if (riffHeader->riff != FOURCC_WAVE_FILE_TAG)
		{
			return E_FAIL;
		}

		// Locate 'wsmp' (DLS Chunk)
		auto ptr = reinterpret_cast<const uint8_t*>(riffHeader) + sizeof(RIFFChunkHeader);
		if ((ptr + sizeof(RIFFChunk)) > wavEnd)
		{
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
		}

		auto dlsChunk = FindChunk({ptr, riffChunk->size}, FOURCC_DLS_SAMPLE);
		if (dlsChunk)
		{
			ptr = reinterpret_cast<const uint8_t*>(dlsChunk) + sizeof(RIFFChunk);
			if (ptr + dlsChunk->size > wavEnd)
			{
				return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
			}

			if (dlsChunk->size >= sizeof(RIFFDLSSample))
			{
				auto dlsSample = reinterpret_cast<const RIFFDLSSample*>(ptr);

				if (dlsChunk->size >= (dlsSample->size + dlsSample->loopCount * sizeof(DLSLoop)))
				{
					auto loops = reinterpret_cast<const DLSLoop*>(ptr + dlsSample->size);
					for (uint32_t j = 0; j < dlsSample->loopCount; ++j)
					{
						if ((loops[j].loopType == DLSLoop::LOOP_TYPE_FORWARD || loops[j].loopType == DLSLoop::LOOP_TYPE_RELEASE))
						{
							// Return 'forward' loop
							*pLoopStart = loops[j].loopStart;
							*pLoopLength = loops[j].loopLength;
							return S_OK;
						}
					}
				}
			}
		}

		// Locate 'smpl' (Sample Chunk)
		auto midiChunk = FindChunk({ptr, riffChunk->size}, FOURCC_MIDI_SAMPLE);
		if (midiChunk)
		{
			ptr = reinterpret_cast<const uint8_t*>(midiChunk) + sizeof(RIFFChunk);
			if (ptr + midiChunk->size > wavEnd)
			{
				return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
			}

			if (midiChunk->size >= sizeof(RIFFMIDISample))
			{
				auto midiSample = reinterpret_cast<const RIFFMIDISample*>(ptr);

				if (midiChunk->size >= (sizeof(RIFFMIDISample) + midiSample->loopCount * sizeof(MIDILoop)))
				{
					auto loops = reinterpret_cast<const MIDILoop*>(ptr + sizeof(RIFFMIDISample));
					for (uint32_t j = 0; j < midiSample->loopCount; ++j)
					{
						if (loops[j].type == MIDILoop::LOOP_TYPE_FORWARD)
						{
							// Return 'forward' loop
							*pLoopStart = loops[j].start;
							*pLoopLength = loops[j].end - loops[j].start + 1;
							return S_OK;
						}
					}
				}
			}
		}

		return S_OK;
	}


	//---------------------------------------------------------------------------------
	HRESULT WaveFindTable(::gpk::view_array<const uint8_t> wavData, _In_ uint32_t tag, _Outptr_result_maybenull_ const uint32_t** pData, _Out_ uint32_t* dataCount) noexcept {
		if (wavData.size() < (sizeof(RIFFChunk) + sizeof(uint32_t)))
			return E_FAIL;

		*pData = nullptr;
		*dataCount = 0;

		const uint8_t* wavEnd = wavData.end();

		// Locate RIFF 'WAVE'
		auto riffChunk = FindChunk(wavData, FOURCC_RIFF_TAG);
		if (!riffChunk || riffChunk->size < 4)
			return E_FAIL;

		auto riffHeader = reinterpret_cast<const RIFFChunkHeader*>(riffChunk);
		if (riffHeader->riff != FOURCC_WAVE_FILE_TAG && riffHeader->riff != FOURCC_XWMA_FILE_TAG)
			return E_FAIL;

		// Locate tag
		auto ptr = reinterpret_cast<const uint8_t*>(riffHeader) + sizeof(RIFFChunkHeader);
		if ((ptr + sizeof(RIFFChunk)) > wavEnd)
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

		auto tableChunk = FindChunk({ptr, riffChunk->size}, tag);
		if (tableChunk) {
			ptr = reinterpret_cast<const uint8_t*>(tableChunk) + sizeof(RIFFChunk);
			if (ptr + tableChunk->size > wavEnd)
				return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

			if ((tableChunk->size % sizeof(uint32_t)) != 0)
				return E_FAIL;

			*pData = reinterpret_cast<const uint32_t*>(ptr);
			*dataCount = tableChunk->size / 4;
		}

		return S_OK;
	}


	//---------------------------------------------------------------------------------
	HRESULT LoadAudioFromFile(::gpk::vcc szFileName, _Inout_ ::gpk::apod<uint8_t>& wavData) noexcept { 
		DWORD                               bytesRead           = 0;
		::gpk::apod<wchar_t>           wszFileName         = {};
		::gpk::mbstowcs(wszFileName, szFileName);
		// open the file
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
		ScopedHandle                        hFile               (safe_handle(CreateFile2(wszFileName.begin(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
#else
		ScopedHandle                        hFile               (safe_handle(CreateFileW(wszFileName.begin(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
#endif

		if (!hFile)
			return HRESULT_FROM_WIN32(GetLastError());

		// Get the file size
		FILE_STANDARD_INFO                  fileInfo            = {};
		rve_if(HRESULT_FROM_WIN32(GetLastError()), !GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)), "File not found: '%s'", ::gpk::toString(szFileName).begin());
		ree_if(fileInfo.EndOfFile.HighPart > 0, "%s", "File is too big for 32-bit allocation, so reject read");
		ree_if(fileInfo.EndOfFile.LowPart < (sizeof(RIFFChunk) * 2 + sizeof(DWORD) + sizeof(WAVEFORMAT)), "%s", "Need at least enough data to have a valid minimal WAV file");

		// create enough space for the file data
		wavData.resize(fileInfo.EndOfFile.LowPart);
		// read the data in
		if (!ReadFile(hFile.get(), wavData.begin(), fileInfo.EndOfFile.LowPart, &bytesRead, nullptr))
			return HRESULT_FROM_WIN32(GetLastError());

		return (bytesRead < fileInfo.EndOfFile.LowPart) ? E_FAIL : bytesRead;
	}
}

//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::LoadWAVAudioInMemory(::gpk::view_array<const uint8_t> wavData, _Outptr_ const WAVEFORMATEX** wfx, ::gpk::view_array<const uint8_t> & audioBytes) noexcept {
	// Need at least enough data to have a valid minimal WAV file
	if (wavData.size() < (sizeof(RIFFChunk) * 2 + sizeof(DWORD) + sizeof(WAVEFORMAT)))
		return E_FAIL;

	bool dpds, seek;
	HRESULT hr = WaveFindFormatAndData(wavData, wfx, audioBytes, dpds, seek);
	if (FAILED(hr))
		return hr;

	return (dpds || seek) ? E_FAIL : S_OK;
}


//-------------------------------------------------------------------------------------
HRESULT DirectX::LoadWAVAudioFromFile(::gpk::vcc szFileName, ::gpk::apod<uint8_t>& wavData, const WAVEFORMATEX** wfx, ::gpk::view_array<const uint8_t> & audioBytes) noexcept {
	DWORD bytesRead = LoadAudioFromFile(szFileName, wavData);
	if (FAILED(bytesRead))
		return bytesRead;

	bool dpds, seek;
	gpk_hrcall(WaveFindFormatAndData(wavData, wfx, audioBytes, dpds, seek));

	return (dpds || seek) ? E_FAIL : S_OK;
}


//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::LoadWAVAudioInMemoryEx(::gpk::view_array<const uint8_t> wavData, DirectX::WAVData& result) noexcept {
	// Need at least enough data to have a valid minimal WAV file
	if (wavData.size() < (sizeof(RIFFChunk) * 2 + sizeof(DWORD) + sizeof(WAVEFORMAT))) {
		return E_FAIL;
	}

	bool dpds = false, seek = false;
	gpk_hrcall(WaveFindFormatAndData(wavData, &result.wfx, result.AudioView, dpds, seek));
	gpk_hrcall(WaveFindLoopInfo(wavData, &result.loopStart, &result.loopLength));

	if (dpds)
		gpk_hrcall(WaveFindTable(wavData, FOURCC_XWMA_DPDS, &result.seek, &result.seekCount));
	else if (seek)
		gpk_hrcall(WaveFindTable(wavData, FOURCC_XMA_SEEK, &result.seek, &result.seekCount));

	return S_OK;
}


//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::LoadWAVAudioFromFileEx(::gpk::vcc szFileName, ::gpk::apod<uint8_t>& wavData, DirectX::WAVData& result) noexcept {
	DWORD bytesRead = LoadAudioFromFile(szFileName, wavData);
	if (FAILED(bytesRead))
		return bytesRead;

	bool dpds, seek;
	gpk_hrcall(WaveFindFormatAndData(wavData, &result.wfx, result.AudioView, dpds, seek));
	gpk_hrcall(WaveFindLoopInfo(wavData, &result.loopStart, &result.loopLength));

	if (dpds)
		gpk_hrcall(WaveFindTable(wavData, FOURCC_XWMA_DPDS, &result.seek, &result.seekCount));
	else if (seek)
		gpk_hrcall(WaveFindTable(wavData, FOURCC_XMA_SEEK, &result.seek, &result.seekCount));

	return S_OK;
}

