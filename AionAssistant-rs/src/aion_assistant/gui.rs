use debug_print::debug_println;
use hudhook::hooks::{ImguiRenderLoop, ImguiRenderLoopFlags};
use hudhook::renderers::imgui_dx12::imgui;
use hudhook::renderers::imgui_dx12::imgui::{Condition, Window};
use windows::Win32::UI::Input::KeyboardAndMouse::{GetAsyncKeyState, VK_RSHIFT};

enum UiState {
	MenuOpen,
	Closed,
	Hidden,
}

pub(crate) struct Dx9HookExample {
	ui_state: UiState,
}

impl Dx9HookExample {

	pub(crate) fn new() -> Self {
		debug_println!("Initializing imGui");
		Dx9HookExample {
			ui_state: UiState::Closed,
		}
	}

	fn render_visible(&mut self, ui: &mut imgui::Ui, flags: &ImguiRenderLoopFlags) {
		Window::new("Hello world").size([300.0, 110.0], Condition::FirstUseEver).build(ui, || {
			ui.text("Hello world!");
			ui.text("こんにちは世界！");
			ui.text("This...is...imgui-rs!");
			ui.separator();
			let mouse_pos = ui.io().mouse_pos;
			ui.text(format!("Mouse Position: ({:.1},{:.1})", mouse_pos[0], mouse_pos[1]));
		});
	}
}

impl ImguiRenderLoop for Dx9HookExample {
	fn render(&mut self, ui: &mut imgui::Ui, flags: &ImguiRenderLoopFlags) {
		let rshift = unsafe { GetAsyncKeyState(VK_RSHIFT.0 as _) < 0 };

		if flags.focused {
			// TODO: Set Smooth Background to 1 (No Interaction)
			self.ui_state = match (&self.ui_state, rshift) {
				(UiState::Hidden, _) => UiState::Closed,
				(_, true) => UiState::Hidden,
				(UiState::MenuOpen, _) => UiState::Closed,
				(UiState::Closed, _) => UiState::MenuOpen,
			};
		}
	}
}