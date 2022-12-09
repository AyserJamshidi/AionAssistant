#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <ShlObj.h>
#include <dxgi1_3.h>

#include <iostream>

static HMODULE s_realVersionModule = nullptr;

void* FindRealAddress(PCSTR name) {
	if (!s_realVersionModule) {
		WCHAR path[MAX_PATH];
		GetSystemDirectory(path, ARRAYSIZE(path));
		wcscat_s(path, L"\\dxgi.dll");
		s_realVersionModule = LoadLibrary(path);
	}

	// TODO ???
	if (s_realVersionModule == 0)
		return 0;

	return GetProcAddress(s_realVersionModule, name);
}

static void* real_CreateDXGIFactory = nullptr;
HRESULT zzCreateDXGIFactory(
	REFIID riid,
	_Out_ void** ppFactory
) {
	if (!real_CreateDXGIFactory) {
		real_CreateDXGIFactory = FindRealAddress("CreateDXGIFactory");
	}

	return ((decltype(*::CreateDXGIFactory)*)real_CreateDXGIFactory)
		(riid, ppFactory);
}

static void* real_CreateDXGIFactory1 = nullptr;
HRESULT zzCreateDXGIFactory1(
	REFIID riid,
	_Out_ void** ppFactory
) {
	if (!real_CreateDXGIFactory1) {
		real_CreateDXGIFactory1 = FindRealAddress("CreateDXGIFactory1");
	}

	return ((decltype(*::CreateDXGIFactory1)*)real_CreateDXGIFactory1)
		(riid, ppFactory);
}

static void* real_CreateDXGIFactory2 = nullptr;
HRESULT zzCreateDXGIFactory2(
	UINT   Flags,
	REFIID riid,
	_Out_ void** ppFactory
) {
	if (!real_CreateDXGIFactory2) {
		real_CreateDXGIFactory2 = FindRealAddress("CreateDXGIFactory2");
	}

	return ((decltype(*::CreateDXGIFactory2)*)real_CreateDXGIFactory2)
		(Flags, riid, ppFactory);
}