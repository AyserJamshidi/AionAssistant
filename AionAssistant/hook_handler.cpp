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

HRESULT APIENTRY hkReset(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
HRESULT APIENTRY hkEndScene(IDirect3DDevice9*);

Reset oReset;
EndScene oEndScene;
WNDPROC oWndProc;

DWORD_PTR* pTable;

IDirect3D9* pD3D;
IDirect3DDevice9* d3dDevice;
HMODULE dllModule;

HWND winHandle = FindWindowA(NULL, "Test Environment - Engine: DirectX 9 - Made by TheAifam5");

DWORD processId;

BOOL showD3D = TRUE;

HRESULT __stdcall hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	std::cout << "Reset Hooked!" << std::endl;

	//HRESULT returnValue = oReset(pDevice, pPresentationParameters);

	return oReset(pDevice, pPresentationParameters); // returnValue;
}

void Eject() {
	showD3D = FALSE;

	/*if (MH_DisableHook((DWORD_PTR*)pTable[16])) { std::cout << "Couldn't remove hook 0."; }
	if (MH_RemoveHook((DWORD_PTR*)pTable[16]) != MH_OK) { std::cout << "Couldn't remove hook 1."; }
	if (MH_RemoveHook((DWORD_PTR*)pTable[42]) != MH_OK) { std::cout << "Couldn't remove hook 2."; }*/
	if (MH_Uninitialize() != MH_OK) { std::cout << "MinHook failed to uninitialize."; }

	// Clear vars
	oEndScene = NULL;
	oReset = NULL;
	d3dDevice = NULL;
	pD3D->Release();
	pD3D = NULL;
	pTable = NULL;

	SetWindowLongPtrW(winHandle, -4, LONG_PTR(oWndProc)); // Restore the window pointer.0

	HWND console = GetConsoleWindow();// We acquire the console's HANDLE before freeing as we will receive NULL if we attempt it afterwards.
	FreeConsole(); // Free the console.
	Sleep(200); // Without this sleep the close message sometimes closes the main window...
	SendMessage(console, WM_CLOSE, NULL, NULL); // Send console window handle the close message.
	FreeLibraryAndExitThread(dllModule, 0);
}

bool firstEndSceneLoop = true;
HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
	//LPDIRECT3DSTATEBLOCK9 oStateBlock; // Keep so Aion doesn't look weird when D3D Transparency is up
	//pDevice->CreateStateBlock(D3DSBT_ALL, &oStateBlock); // Keep so Aion doesn't look weird when D3D Transparency is up

	if (firstEndSceneLoop) {
		firstEndSceneLoop = false;
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

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
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return true;
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI DirectXInit(__in  LPVOID lpParameter) {
	dllModule = (HMODULE)lpParameter;

	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D == NULL) {
		std::cout << "Failed to create D3D interface." << std::endl;
		Eject();
		return -1;
	}

	D3DPRESENT_PARAMETERS d3dpp{ 0 };
	d3dpp.hDeviceWindow = winHandle, d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD, d3dpp.Windowed = TRUE;

	if (FAILED(pD3D->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice))) {
		std::cout << "Failed to create D3D device." << std::endl;
		Eject();
		return -1;
	}

pTable = (DWORD_PTR*)d3dDevice;
pTable = (DWORD_PTR*)pTable[0];

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
	oWndProc = (WNDPROC)SetWindowLongPtrA(winHandle, -4, (LONG_PTR)WndProc);

	std::cout << "Successfully hooked." << std::endl;

	return 1;
}