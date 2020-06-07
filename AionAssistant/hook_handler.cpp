#include "hook_handler.h"
#include <iostream>
#include "AttachHelper.h"

#include "MinHook/include/MinHook.h"
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msh, WPARAM wParam, LPARAM lParam);

typedef HRESULT(APIENTRY* Reset) (IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT(APIENTRY* EndScene) (IDirect3DDevice9*);
typedef LRESULT(CALLBACK* WNDPROC) (HWND, UINT, WPARAM, LPARAM);

WNDPROC oWndProc;

HRESULT APIENTRY hkReset(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
HRESULT APIENTRY hkEndScene(IDirect3DDevice9*);

Reset oReset = FALSE;
EndScene oEndScene = FALSE;	

HWND window = FindWindowA(NULL, "DirectX 9 Test Environment");

DWORD processId;

HRESULT __stdcall hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	std::cout << "Reset Hooked!" << std::endl;

	HRESULT returnValue = oReset(pDevice, pPresentationParameters);

	return returnValue;
}


HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
	//LPDIRECT3DSTATEBLOCK9 oStateBlock; // Keep so Aion doesn't look weird when D3D Transparency is up
	//pDevice->CreateStateBlock(D3DSBT_ALL, &oStateBlock); // Keep so Aion doesn't look weird when D3D Transparency is up

	static bool init = true;
	if (init) {
		init = false;
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX9_Init(pDevice);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Text("Hello user!");
	ImGui::EndFrame();

	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	//pDevice->EndScene(); //Call the EndScene function with our Device

	//oStateBlock->Apply(); // Keep so Aion doesn't look weird when D3D Transparency is up
	//oStateBlock->Release(); // Keep so Aion doesn't look weird when D3D Transparency is up

	return oEndScene(pDevice);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI DirectXInit(__in  LPVOID lpParameter) {
	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D == NULL) {
		std::cout << "Failed to create D3D interface." << std::endl;
		return -1;
	}

	D3DPRESENT_PARAMETERS d3dpp{ 0 };
	d3dpp.hDeviceWindow = window, d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD, d3dpp.Windowed = TRUE;

	IDirect3DDevice9* d3dDevice = nullptr;
	if (FAILED(pD3D->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice))) {
		std::cout << "Failed to create D3D device." << std::endl;
		pD3D->Release();
		return -1;
	}

#if defined _M_X64
	DWORD64* pTable = (DWORD64*)d3dDevice;
	pTable = (DWORD64*)pTable[0];
#else
	DWORD* pTable = (DWORD*)d3dDevice;
	pTable = (DWORD*)pTable[0];
#endif

	if (d3dDevice)
		d3dDevice->Release(), d3dDevice = nullptr;

	oReset = (Reset)pTable[16]; // Set oReset to the original Reset.
	oEndScene = (EndScene)pTable[42]; // Set oEndScene to the original EndScene.

	if (MH_Initialize() != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pTable[16], &hkReset, reinterpret_cast<void**>(&oReset)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pTable[16]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pTable[42], &hkEndScene, reinterpret_cast<void**>(&oEndScene)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pTable[42]) != MH_OK) { return 1; }
	//82 == DrawIndexedPrimitive

	// GWL_WNDPROC == x86, GWLP_WNDPROC == x64
	oWndProc = (WNDPROC)SetWindowLongPtr(window, -4, (LONG_PTR)WndProc);

	std::cout << "Successfully hooked." << std::endl;

	return 1;
}