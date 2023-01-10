use debug_print::debug_println;
use toy_arms::internal::{cast, GameObject};
use toy_arms::derive::GameObject;

#[derive(GameObject)]
pub struct CEntitySystem {
	pub base: *const u64 // Denotes the base address of this GameObject
}

impl CEntitySystem {
	pub fn is_valid(&self) -> bool {
		// False if base is null
		if self.base.is_null() {
			return false;
		}

		// False if base address is out of expected bounds [0x10000000, 0x7FFFFFFF]
		if ((self.base as u64) < 0x10000000) || ((self.base as u64) > 0x7FFFFFFF) {
			return false;
		}

		return true;
	}
}