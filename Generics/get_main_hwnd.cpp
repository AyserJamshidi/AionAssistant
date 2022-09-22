#include "helpers.hpp"

// Source = https://guidedhacking.com/threads/how-to-get-a-hwnd-by-process-name.12104/
struct handle_data {
	unsigned long process_id;
	HWND window_handle;
};

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lparam) {
	auto& data = *reinterpret_cast<handle_data*>(lparam);

	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);

	if (data.process_id != process_id) {
		return TRUE;
	}
	//Log::get()->msg("data.process_id: {} | process_id: {}", data.process_id, process_id);
	data.window_handle = handle;
	return FALSE;
}

HWND find_main_window() {
	handle_data data{};

	data.process_id = GetCurrentProcessId();
	data.window_handle = nullptr;
	EnumWindows(enum_windows_callback, reinterpret_cast<LPARAM>(&data));

	return data.window_handle;
}

HWND Helpers::GetMainHWND() {
	return find_main_window();
}