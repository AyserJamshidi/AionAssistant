#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <mmiscapi.h> // Required for MSAcm.h
#include <mmreg.h> // Required for MSAcm.h
#include <MSAcm.h>

static HMODULE s_realVersionModule = nullptr;

void* FindRealAddress(PCSTR name) {
	if (!s_realVersionModule) {
		WCHAR path[MAX_PATH];
		GetSystemDirectory(path, ARRAYSIZE(path));
		wcscat_s(path, L"\\msacm32.dll");
		s_realVersionModule = LoadLibrary(path);
	}

	return GetProcAddress(s_realVersionModule, name);
}

static void* real_acmStreamOpen = nullptr;
MMRESULT ACMAPI zzacmStreamOpen(
	LPHACMSTREAM   phas,
	HACMDRIVER     had,
	LPWAVEFORMATEX pwfxSrc,
	LPWAVEFORMATEX pwfxDst,
	LPWAVEFILTER   pwfltr,
	DWORD_PTR      dwCallback,
	DWORD_PTR      dwInstance,
	DWORD          fdwOpen
) {
	if (!real_acmStreamOpen) {
		real_acmStreamOpen = FindRealAddress("acmStreamOpen");
	}

	return ((decltype(*::acmStreamOpen)*)real_acmStreamOpen)
		(phas, had, pwfxSrc, pwfxDst, pwfltr, dwCallback, dwInstance, fdwOpen);
}

static void* real_acmFormatSuggest = nullptr;
MMRESULT ACMAPI zzacmFormatSuggest(
	HACMDRIVER     had,
	LPWAVEFORMATEX pwfxSrc,
	LPWAVEFORMATEX pwfxDst,
	DWORD          cbwfxDst,
	DWORD          fdwSuggest
) {
	if (!real_acmFormatSuggest) {
		real_acmFormatSuggest = FindRealAddress("acmFormatSuggest");
	}

	return ((decltype(*::acmFormatSuggest)*)real_acmFormatSuggest)
		(had, pwfxSrc, pwfxDst, cbwfxDst, fdwSuggest);
}

static void* real_acmStreamUnprepareHeader = nullptr;
MMRESULT ACMAPI zzacmStreamUnprepareHeader(
	HACMSTREAM        has,
	LPACMSTREAMHEADER pash,
	DWORD             fdwUnprepare
) {
	if (!real_acmStreamUnprepareHeader) {
		real_acmStreamUnprepareHeader = FindRealAddress("acmStreamUnprepareHeader");
	}

	return ((decltype(*::acmStreamUnprepareHeader)*)real_acmStreamUnprepareHeader)
		(has, pash, fdwUnprepare);
}

static void* real_acmStreamConvert = nullptr;
MMRESULT ACMAPI zzacmStreamConvert(
	HACMSTREAM        has,
	LPACMSTREAMHEADER pash,
	DWORD             fdwUnprepare
) {
	if (!real_acmStreamConvert) {
		real_acmStreamConvert = FindRealAddress("acmStreamConvert");
	}

	return ((decltype(*::acmStreamConvert)*)real_acmStreamConvert)
		(has, pash, fdwUnprepare);
}

static void* real_acmStreamPrepareHeader = nullptr;
MMRESULT ACMAPI zzacmStreamPrepareHeader(
	HACMSTREAM        has,
	LPACMSTREAMHEADER pash,
	DWORD             fdwUnprepare
) {
	if (!real_acmStreamPrepareHeader) {
		real_acmStreamPrepareHeader = FindRealAddress("acmStreamPrepareHeader");
	}

	return ((decltype(*::acmStreamPrepareHeader)*)real_acmStreamPrepareHeader)
		(has, pash, fdwUnprepare);
}

static void* real_acmStreamSize = nullptr;
MMRESULT ACMAPI zzacmStreamSize(
	HACMSTREAM has,
	DWORD      cbInput,
	LPDWORD    pdwOutputBytes,
	DWORD      fdwSize
) {
	if (!real_acmStreamSize) {
		real_acmStreamSize = FindRealAddress("acmStreamSize");
	}

	return ((decltype(*::acmStreamSize)*)real_acmStreamSize)
		(has, cbInput, pdwOutputBytes, fdwSize);
}