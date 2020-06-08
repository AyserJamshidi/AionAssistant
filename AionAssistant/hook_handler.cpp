#include "hook_handler.h"
#include <iostream>
#include "AttachHelper.h"
#include "memoryLoop.h"

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

HWND winHandle = FindWindowA(NULL, "DirectX 9 Test Environment");

DWORD processId;

DWORD* pTable = NULL;
IDirect3D9* pD3D = NULL;
IDirect3DDevice9* d3dDevice = NULL;
HMODULE dllModule;

BOOL showD3D = TRUE;

HRESULT __stdcall hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	std::cout << "Reset Hooked!" << std::endl;

	//HRESULT returnValue = oReset(pDevice, pPresentationParameters);

	return oReset(pDevice, pPresentationParameters); // returnValue;
}

void Eject() {
	showD3D = FALSE;

	if (MH_RemoveHook(MH_ALL_HOOKS) != MH_OK) { std::cout << "Couldn't remove hook 1."; }
	if (MH_Uninitialize() != MH_OK) { std::cout << "Couldn't remove hook 2."; }

	// Clear vars
	oEndScene = NULL;
	oReset = NULL;
	d3dDevice = NULL;
	pD3D->Release();
	pD3D = NULL;

	SetWindowLongPtrA(winHandle, GWLP_WNDPROC, LONG_PTR(oWndProc));
	// We acquire the console's HANDLE before freeing as we will receive NULL if we attempt it afterwards.
	HWND console = GetConsoleWindow();
	FreeConsole(); // Free the console.
	Sleep(300); // Without this sleep the close message sometimes closes the main window...
	SendMessage(console, WM_CLOSE, NULL, NULL); // Send console window handle the close message.
	FreeLibraryAndExitThread(dllModule, 0);

}

HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
	//LPDIRECT3DSTATEBLOCK9 oStateBlock; // Keep so Aion doesn't look weird when D3D Transparency is up
	//pDevice->CreateStateBlock(D3DSBT_ALL, &oStateBlock); // Keep so Aion doesn't look weird when D3D Transparency is up
	
	static bool init = true;
	if (init) {
		init = false;
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(winHandle);
		ImGui_ImplDX9_Init(pDevice);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Text("Hello user!");

	if (ImGui::Button("Unload")) {
		oWndProc = NULL;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Eject, dllModule, NULL, NULL);
	}

	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	//pDevice->EndScene(); //Call the EndScene function with our Device

	//oStateBlock->Apply(); // Keep so Aion doesn't look weird when D3D Transparency is up
	//oStateBlock->Release(); // Keep so Aion doesn't look weird when D3D Transparency is up

	return oEndScene(pDevice);
}

LRESULT CALLBACK WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) && showD3D) {
		return true;
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI DirectXInit(__in  LPVOID lpParameter) {
	dllModule = (HMODULE)lpParameter;

	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D == NULL) {
		std::cout << "Failed to create D3D interface." << std::endl;
		return -1;
	}

	D3DPRESENT_PARAMETERS d3dpp{ 0 };
	d3dpp.hDeviceWindow = winHandle, d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD, d3dpp.Windowed = TRUE;

	if (FAILED(pD3D->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice))) {
		std::cout << "Failed to create D3D device." << std::endl;
		pD3D->Release();
		return -1;
	}

#if defined _M_X64
	DWORD64* pTable = (DWORD64*)d3dDevice;
	pTable = (DWORD64*)pTable[0];
#else
	pTable = (DWORD*)d3dDevice;
	pTable = (DWORD*)pTable[0];
#endif

	if (d3dDevice)
		d3dDevice->Release(), d3dDevice = NULL;

	oReset = (Reset)pTable[16]; // Set oReset to the original Reset.
	oEndScene = (EndScene)pTable[42]; // Set oEndScene to the original EndScene.

	if (MH_Initialize() != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pTable[16], &hkReset, reinterpret_cast<void**>(&oReset)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pTable[16]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pTable[42], &hkEndScene, reinterpret_cast<void**>(&oEndScene)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pTable[42]) != MH_OK) { return 1; }
	//82 == DrawIndexedPrimitive

	// GWL_WNDPROC == x86, GWLP_WNDPROC == x64, both define to -4...
	oWndProc = (WNDPROC)SetWindowLongA(winHandle, -4, (LONG_PTR)WndProc);

	std::cout << "Successfully hooked." << std::endl;

	return 1;
}