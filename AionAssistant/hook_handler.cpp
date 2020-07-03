#include "hook_handler.h"
#include <iostream>
//#include "AttachHelper.h"
//#include "memory_loop.h"

#include "MinHook/include/MinHook.h"
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
//#pragma comment(lib, "d3dx9.lib")

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

//#include <string>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msh, WPARAM wParam, LPARAM lParam);

typedef HRESULT(APIENTRY* EndScene) (IDirect3DDevice9*);
HRESULT APIENTRY hkEndScene(IDirect3DDevice9*);
EndScene oEndScene = nullptr;

typedef HRESULT(APIENTRY* Reset) (IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
HRESULT APIENTRY hkReset(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
Reset oReset = nullptr;

typedef LRESULT(CALLBACK* WNDPROC) (HWND, UINT, WPARAM, LPARAM);
WNDPROC oWndProc = nullptr;

uintptr_t* pVTable = nullptr;

IDirect3D9* pD3D = nullptr;
IDirect3DDevice9* d3dDevice = nullptr;
HINSTANCE dllModule = 0;

HWND pHwnd = FindWindowA(nullptr, "AION");
//HWND winHandle = FindWindowA(NULL, "AION Client");
//HWND pHwnd = FindWindowA(NULL, "Test Environment - Engine: DirectX 9 - Made by TheAifam5");

uint64_t processId = 0; // Can probably just make it uLong or uInt later.

bool showD3D = TRUE;

/*HRESULT __stdcall hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	std::cout << "Reset Hooked!" << std::endl;

	//HRESULT returnValue = oReset(pDevice, pPresentationParameters);

	return oReset(pDevice, pPresentationParameters); // returnValue;
}*/

void __stdcall Eject(const char* msg, bool isInitialized) {
	printf("%s\n", msg);
	showD3D = FALSE;

	fclose((FILE*)stdout);

	HWND console = GetConsoleWindow();// We acquire the console's HANDLE before freeing as we will receive NULL if we attempt it afterwards.
	FreeConsole(); // Free the console.
	Sleep(200); // Without this sleep the close message sometimes closes the main window...
	SendMessage(console, WM_CLOSE, NULL, NULL); // Send console window handle the close message.*/

	if (isInitialized) {
		SetWindowLongPtr(pHwnd, GWLP_WNDPROC, (LONG_PTR)oWndProc); // Restore the window pointer.

		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		// TODO: Figure out why disable/remove hook fails.
		/*if (MH_DisableHook((DWORD_PTR*)pTable[16])) { std::cout << "Couldn't remove hook 0."; }
		if (MH_RemoveHook((DWORD_PTR*)pTable[16]) != MH_OK) { std::cout << "Couldn't remove hook 1."; }
		if (MH_RemoveHook((DWORD_PTR*)pTable[42]) != MH_OK) { std::cout << "Couldn't remove hook 2."; }*/
		if (MH_Uninitialize() != MH_OK) { std::cout << "MinHook failed to uninitialize."; }
	}

	// Clear vars
	oEndScene = NULL;
	oReset = NULL;
	d3dDevice = NULL;
	pD3D->Release();
	pD3D = NULL;
	pVTable = NULL;

	FreeLibraryAndExitThread(dllModule, 0);
}

HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
	if (showD3D) {
		
		static bool initializeImGui = true;
		if (initializeImGui) {
			initializeImGui = false;

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			//io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

			ImGui_ImplWin32_Init(pHwnd);
			ImGui_ImplDX9_Init(pDevice);
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//if (MemLoop::IsInitialized()) {
		//}

		if (ImGui::Button("Unload")) {
			/*DWORD procId;
			GetWindowThreadProcessId(pHwnd, &procId);
			//pHandle = OpenProcess(PROCESS_VM_READ, 0, procId);
			MemLoop::Initialize(OpenProcess(PROCESS_VM_READ, 0, procId));
			//oWndProc = NULL;*/
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Eject, 0, 0, 0);
		}

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	return oEndScene(pDevice);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (wParam) {
	case VK_UP:
		if (uMsg == WM_KEYDOWN) {
			std::cout << "UP" << std::endl;
			oWndProc = nullptr;
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Eject, dllModule, 0, 0);
		}
		break;
	case VK_DOWN:
		if (uMsg == WM_KEYDOWN) {
			showD3D = !showD3D;
			std::cout << "showD3D is now " << showD3D << std::endl;
		}
		break;
	case VK_LEFT:
		if (uMsg == WM_KEYDOWN) {
			//GetName();
		}
	case WM_LBUTTONDOWN:
		break;
	default:
		break;
	}

	/*switch (uMsg) {
				 case WM_KEYDOWN:
				 case WM_KEYUP:
				 case WM_CAPTURECHANGED:
				 case WM_LBUTTONDBLCLK:
				 case WM_LBUTTONDOWN:
				 case WM_MBUTTONDBLCLK:
				 case WM_MBUTTONDOWN:
				 case WM_MBUTTONUP:
				 case WM_MOUSEACTIVATE:
				 case WM_MOUSEHOVER:
				 case WM_MOUSEHWHEEL:
				 case WM_MOUSELEAVE:
				 case WM_MOUSEMOVE:
				 case WM_MOUSEWHEEL:
				 case WM_NCHITTEST:
				 case WM_NCLBUTTONDBLCLK:
				 case WM_NCLBUTTONDOWN:
				 case WM_NCLBUTTONUP:
				 case WM_NCMBUTTONDBLCLK:
				 case WM_NCMBUTTONDOWN:
				 case WM_NCMBUTTONUP:
				 case WM_NCMOUSEHOVER:
				 case WM_NCMOUSELEAVE:
				 case WM_NCMOUSEMOVE:
				 case WM_NCRBUTTONDBLCLK:
				 case WM_NCRBUTTONDOWN:
				 case WM_NCRBUTTONUP:
				 case WM_NCXBUTTONDBLCLK:
				 case WM_NCXBUTTONDOWN:
				 case WM_NCXBUTTONUP:
				 case WM_RBUTTONDBLCLK:
				 case WM_RBUTTONDOWN:
				 case WM_RBUTTONUP:
				 case WM_XBUTTONDBLCLK:
				 case WM_XBUTTONDOWN:
				 case WM_XBUTTONUP:
				 case WM_LBUTTONUP:
					 return 1L;
	}*/

	/*if (showD3D && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;*/

	return (showD3D && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) ? true : CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

DWORD __stdcall DirectXInit(HMODULE lpParameter) {
	// TODO: Is there a better way to pass this variable along to eject?
	dllModule = lpParameter;
	
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (pD3D == nullptr) { Eject("Failed to create D3D interface.", false); return true; }

	D3DPRESENT_PARAMETERS d3dpp { 0 };
	d3dpp.hDeviceWindow = pHwnd, d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD, d3dpp.Windowed = TRUE;

	if (FAILED(pD3D->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice)))
		Eject("Failed to create D3D device.", false); return true;

	pVTable = (uintptr_t*)d3dDevice;
	pVTable = (uintptr_t*)pVTable[0];

	if (d3dDevice) { d3dDevice->Release(), d3dDevice = nullptr; }

	if (MH_Initialize() != MH_OK) { Eject("Failed to initialize MinHook.", false); return true; }

	/*oReset = (Reset)pVTable[16]; // Set oReset to the original Reset.
	if (MH_CreateHook((uintptr_t*)pVTable[16], &hkReset, reinterpret_cast<void**>(&oReset)) != MH_OK) { return true; }
	if (MH_EnableHook((uintptr_t*)pVTable[16]) != MH_OK) { return true; }*/

	oEndScene = (EndScene)pVTable[42]; // Set oEndScene to the original EndScene.
	if (MH_CreateHook((uintptr_t*)pVTable[42], &hkEndScene, reinterpret_cast<void**>(&oEndScene)) != MH_OK) { return true; }
	if (MH_EnableHook((uintptr_t*)pVTable[42]) != MH_OK) { return true; }

	//82 == DrawIndexedPrimitive

	oWndProc = WNDPROC(SetWindowLongPtr(pHwnd, -4 /* GWL_WNDPROC(x86), GWLP_WNDPROC(x86_64) */, (LONG_PTR)WndProc));

	/*DWORD procId;
	GetWindowThreadProcessId(pHwnd, &procId);
	//pHandle = OpenProcess(PROCESS_VM_READ, 0, procId);
	MemLoop::Initialize(OpenProcess(PROCESS_VM_READ, 0, procId));*/

	printf("Successful hook.\n");
	return true;
}