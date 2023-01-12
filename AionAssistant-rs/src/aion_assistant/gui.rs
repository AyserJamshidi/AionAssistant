use hudhook::hooks::{ImguiRenderLoop, ImguiRenderLoopFlags};
use hudhook::renderers::imgui_dx12::imgui;
use hudhook::renderers::imgui_dx12::imgui::Condition;

pub(crate) struct Dx9HookExample {}

impl Dx9HookExample {
    pub(crate) fn new() -> Self {
        Dx9HookExample {}
    }

    pub(crate) fn player_tab(&mut self, ui: &imgui::Ui) {
        if let Some(_) = ui.tab_item("Player") {
            ui.text("Hello Player!");
            if ui.is_item_hovered() {
                ui.tooltip_text("text");
            }
        }
    }

	pub(crate) fn target_tab(&mut self, ui: &imgui::Ui) {
		if let Some(_) = ui.tab_item("Target") {
			ui.text("Hello Target!");
		}
	}
}

// Menu Customization: https://www.youtube.com/watch?v=iOQ7ZrNQLuI
impl ImguiRenderLoop for Dx9HookExample {
    fn render(&mut self, ui: &mut imgui::Ui, _flags: &ImguiRenderLoopFlags) {
        ui.window("##main")
            .size([320., 200.], Condition::Always)
            .build(|| {
                if let Some(_i) = ui.tab_bar("##tab_bar") {
					self.player_tab(ui);
					self.target_tab(ui);
                }
            });
    }
}
