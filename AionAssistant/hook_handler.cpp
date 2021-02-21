#include <iostream>
#include <d3d9.h>
#include <thread>

#include "Handlers/entity_handler.h"
#include "hook_handler.h"
#include "MinHook/include/MinHook.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

#pragma comment(lib, "d3d9.lib") // Required for Direct3DCreate9

// DirectX9 EndScene() hook.  Needed to display GUI.
typedef HRESULT(__stdcall* EndScene) (IDirect3DDevice9*);
HRESULT __stdcall EndSceneHook(IDirect3DDevice9*);
EndScene originalEndScene = nullptr;

// DirectX9 Reset() hook.  Needed to avoid crashing during Alt+Tab/Game Resizing.
typedef HRESULT(__stdcall* Reset) (IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
HRESULT __stdcall ResetHook(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
Reset originalReset = nullptr;

// ImGui mouse event handling.
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msh, WPARAM wParam, LPARAM lParam);
typedef LRESULT(__stdcall* WNDPROC) (HWND, UINT, WPARAM, LPARAM);
WNDPROC oWndProc = nullptr;

// DirectX9 variables, created in order of use.
IDirect3D9* pD3D = nullptr;
IDirect3DDevice9* d3dDevice = nullptr;
uintptr_t* pVTable = nullptr;

// If this gets assigned at runtime, maybe we can move this to be assigned during AAGUI_Initialization if it causes problems later?
HWND pHwnd = nullptr;
HINSTANCE dllModule = nullptr;

// GUI variables
bool showD3D = true;
BOOL imguiInitialized = false;
BOOL isInitialized = false;
ImGuiIO io;

//HWND pHwnd = FindWindowA(NULL, "Test Environment - Engine: DirectX 9 - Made by TheAifam5");

void AAGUI::ThisThing() {
	printf("Hi! == %f", (float)31.2);
}

void AAGUI::Initialize(HMODULE lpParameter) {
	/// TODO: Is there a better way to pass this variable along to eject?
	dllModule = lpParameter; // assign dll module
	pHwnd = FindWindowEx(0, 0, L"AIONClientWndClass1.0", 0); // get window of aion via window class
	std::cout << "Target: " << pHwnd << std::endl;

	pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (pD3D == nullptr) { Eject("Failed to create D3D interface."); return; }

	D3DPRESENT_PARAMETERS d3dpp{ 0 };
	d3dpp.hDeviceWindow = pHwnd, d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD, d3dpp.Windowed = true;

	/*D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp)); // Fills d3dpp with necessary amount of 00 bytes
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = pHwnd;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;*/

	if (FAILED(pD3D->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice))) {
		Eject("Failed to create D3D device.");
		return;
	}

	pVTable = (uintptr_t*)d3dDevice;
	pVTable = (uintptr_t*)pVTable[0];

	if (d3dDevice) { d3dDevice->Release(), d3dDevice = nullptr; }

	if (MH_Initialize() != MH_OK) { Eject("Failed to initialize MinHook."); return; }

	originalReset = (Reset)pVTable[16];
	if (MH_CreateHook((uintptr_t*)pVTable[16], &ResetHook, reinterpret_cast<void**>(&originalReset)) != MH_OK) { return; }
	if (MH_EnableHook((uintptr_t*)pVTable[16]) != MH_OK) { return; }

	originalEndScene = (EndScene)pVTable[42];
	if (MH_CreateHook((uintptr_t*)pVTable[42], &EndSceneHook, reinterpret_cast<void**>(&originalEndScene)) != MH_OK) { return; }
	if (MH_EnableHook((uintptr_t*)pVTable[42]) != MH_OK) { return; }

	//82 == DrawIndexedPrimitive

	oWndProc = WNDPROC(SetWindowLongPtr(pHwnd, -4, (intptr_t)WndProc));
	isInitialized = true;

	EntityHandler::Initialize(0x5A107);

	printf("Successful hook.\n");
}

/* This MUST be called via CreateThread.  std::thread does not handle interrupts
 * and this will cause a DLL hang during unload.  It'll still be in use even
 * if we can re-inject the same DLL.  DLL file can't be modified.
 */
void __stdcall Eject(const char* msg) {
	std::cout << "\nInside Eject" << std::endl;
	showD3D = false;
	EntityHandler::Uninitialize();

	//HWND console = GetConsoleWindow();// We acquire the console's HANDLE before freeing as we will receive NULL if we attempt it afterwards.

	//Sleep(500); // Without this sleep the close message sometimes closes the main window...

	SetWindowLongPtr(pHwnd, GWLP_WNDPROC, (LONG_PTR)oWndProc); // Restore the window pointer.

	if (isInitialized) {
		if (imguiInitialized) {
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		// TODO: Figure out why disable/remove hook fails.
		if (MH_DisableHook((uintptr_t*)pVTable[16])) { std::cout << "Couldn't remove hook 0."; }
		if (MH_RemoveHook((uintptr_t*)pVTable[16]) != MH_OK) { std::cout << "Couldn't remove hook 1."; }
		if (MH_RemoveHook((uintptr_t*)pVTable[42]) != MH_OK) { std::cout << "Couldn't remove hook 2."; }
		if (MH_Uninitialize() != MH_OK) { std::cout << "MinHook failed to uninitialize."; }

		pD3D->Release();
	}

	// Clear memory
	originalEndScene = nullptr;
	originalReset = nullptr;
	d3dDevice = nullptr;
	pD3D = nullptr;
	pVTable = nullptr;

	fclose((FILE*)stdout);
	FreeConsole(); // Free the console.
	FreeLibraryAndExitThread(dllModule, 0);
}

HRESULT __stdcall ResetHook(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	std::cout << "Reset Hooked!" << std::endl;

	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT returnValue = originalReset(pDevice, pPresentationParameters);

	if (returnValue == D3D_OK) { // Reset stuff!
		ImGui_ImplDX9_CreateDeviceObjects();
	}

	return returnValue; // returnValue;
}

/*#include "Handlers/entity_handler.h"
#include <codecvt>
#include <string>

// convert UTF-8 string to wstring
std::wstring utf8_to_wstring (const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.from_bytes(str);
}

// convert wstring to UTF-8 string
std::string wstring_to_utf8 (const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(str);
}*/


HRESULT __stdcall EndSceneHook(LPDIRECT3DDEVICE9 pDevice) {
	if (showD3D) {
		if (!imguiInitialized) {
			imguiInitialized = true;

			ImGui::CreateContext();
			ImGui_ImplWin32_Init(pHwnd);
			ImGui_ImplDX9_Init(pDevice);
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		SetMousePosition();

		//ImGui::ShowDemoWindow();

		if (ImGui::Begin("Aion Assistant!")) {
			if (ImGui::Button("Unload")) {
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Eject, "Unload clicked.", 0, 0);
			}

			if (ImGui::Checkbox("Hidden Moobs", false)) {

			}

			if (ImGui::BeginTabBar("##TabBar")) {
				if (ImGui::BeginTabItem("Main")) {

					ImGui::EndTabItem();
				}

				// Entity List
				if (ImGui::BeginTabItem("Entity List")) {
					static int selected = 0;

					// Left
					ImGui::BeginChild("left pane", ImVec2(150, 0), true);
					/*for (int i = 0; i < entityVect.size(); i++) {
						char label[128];

						sprintf_s(label, "%s", wstring_to_utf8(entityVect.at(i)->CEntity->name));
						if (ImGui::Selectable(label, selected == i))
							selected = i;

					}*/
					ImGui::EndChild();

					ImGui::SameLine();

					// Right
					ImGui::BeginGroup();
					ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
					ImGui::Text("MyObject: %d", selected);
					ImGui::Separator();
					if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
						if (ImGui::BeginTabItem("Description")) {
							ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Details")) {
							ImGui::Text("ID: 0123456789");
							ImGui::EndTabItem();
						}
						ImGui::EndTabBar();
					}
					ImGui::EndChild();
					if (ImGui::Button("Revert")) {}
					ImGui::SameLine();
					if (ImGui::Button("Save")) {}
					ImGui::EndGroup();

					/*	static int selected = 0;
						ImGui::BeginChild("left pane", ImVec2(150, 0), true);

						// For when we implement entity list/radar
						for (int i = 0; i < 100; i++) {
							char label[128];
							printf(label, "Ent - %s", i);
							if (ImGui::Selectable(label, selected == i))
								selected = i;
						}

						ImGui::EndChild();
						ImGui::SameLine();


						// right
						ImGui::BeginGroup();
						ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
						ImGui::Text("MyObject: %d", selected);
						ImGui::Separator();
						ImGui::EndChild();
						ImGui::EndGroup();*/
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::End();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	return originalEndScene(pDevice);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (wParam) {
	case VK_UP:
		if (uMsg == WM_KEYDOWN) {
			std::cout << "UP" << std::endl;
			//oWndProc = nullptr;
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Eject, "Ejecting.", 0, 0);
			return true;
		}
		break;
	case VK_DOWN:
		if (uMsg == WM_KEYDOWN) {
			showD3D = !showD3D;
			std::cout << "showD3D is now " << showD3D << std::endl;
			return true; // Block input
		}
		break;
	case VK_LEFT:
		if (uMsg == WM_KEYDOWN) {
			//GetName();
			return true; // Block input
		}
	case WM_LBUTTONDOWN:
		return true;
		break;
	default:
		break;
	}

	if (showD3D && io.WantCaptureMouse) {
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam); // Pass mouse to ImGui
		return true;
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

		// If we should show d3d and imgui handled the mouse properly, true, otherwise do standard window call.

		//return (showD3D && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) ? true : CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam); // Standard mouse handling
}

void SetMousePosition() {
	POINT mousePosition;
	GetCursorPos(&mousePosition);

	io = ImGui::GetIO();
	io.MousePos.x = mousePosition.x;
	io.MousePos.y = mousePosition.y;
}