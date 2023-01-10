use debug_print::{debug_eprintln, debug_println};
use mlua::UserData;
use toy_arms::internal::cast;
// use toy_arms::internal::GameObject;
// use toy_arms::derive::GameObject;

// #[derive(GameObject)]
#[derive(Default)]
pub(crate) struct CharacterObject {
	base: usize,
	has_animation: usize,
	level: usize,
	hide_status: usize,
	invul_status: usize,
	interaction_disabled: usize, // TODO: This is probably improperly named
	health: usize,
	health_max: usize,
	mana: usize,
	mana_max: usize,
	name: usize,
	owner: usize,
	legion: usize,
	note: usize,
	target_id: usize,
	animation_speed: usize,
	left_weapon: usize,
	right_weapon: usize,
	gravity_status: usize,
	animation_type: usize,
	eye_status: usize,
}

// impl Default for CharacterObject {
// 	fn default() -> Self {
// 		Self {
// 			base: 0x0,
// 			has_animation: 0x0,
// 			level: 0x0,
// 			hide_status: 0x0,
// 			invul_status: 0x0,
// 			interaction_disabled: 0x0,
// 			health: 0x0,
// 			health_max: 0x0,
// 			mana: 0x0,
// 			mana_max: 0x0,
// 			name: 0x0,
// 			owner: 0x0,
// 			legion: 0x0,
// 			note: 0x0,
// 			target_id: 0x0,
// 			animation_speed: 0x0,
// 			left_weapon: 0x0,
// 			right_weapon: 0x0,
// 			gravity_status: 0x0,
// 			animation_type: 0x0,
// 			eye_status: 0x0,
// 		}
// 	}
// }

impl CharacterObject {
	pub(crate) unsafe fn from_raw(base: usize/*, HashMap<>*/) -> Option<CharacterObject> {
		Some(CharacterObject {
			base,
			..Default::default()
		})
	}

	pub(crate) unsafe fn get_animation_speed(&self) -> u16 {
		debug_println!("Base: {:?}", self.base);
		// *cast!(self.base + 0x4FA, u16)
		0
	}
}

impl UserData for CharacterObject {

}

// pub(crate) struct Character {
// 	pub(crate) has_animation: usize,
// 	pub(crate) level: usize,
// 	pub(crate) hide_status: usize,
// 	pub(crate) invul_status: usize,
// 	pub(crate) interaction_status: usize,
// 	pub(crate) health: usize,
// 	pub(crate) health_max: usize,
// 	pub(crate) mana: usize,
// 	pub(crate) mana_max: usize,
// 	pub(crate) name: usize,
// 	pub(crate) owner: usize,
// 	pub(crate) legion: usize,
// 	pub(crate) note: usize,
// 	pub(crate) target_id: usize,
// 	pub(crate) animation_speed: usize,
// 	pub(crate) left_weapon: usize,
// 	pub(crate) right_weapon: usize,
// 	pub(crate) gravity_status: usize,
// 	pub(crate) animation_type: usize,
// 	pub(crate) eye_status: usize,
// }