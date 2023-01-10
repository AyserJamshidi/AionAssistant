use debug_print::debug_println;
use toy_arms::internal::{cast, GameObject};
use toy_arms::derive::GameObject;

#[derive(GameObject)]
pub struct DialogObject {
	pub base: *const u64 // Denote the base address of LocalPlayer to use it later in get_health() function.
}

impl DialogObject {
	pub fn is_valid(&self) -> bool {
		// False if base is null
		if self.base.is_null() {
			return false;
		}

		// False if base address is out of expected bounds [0x10000000, 0x7FFFFFFF]
		if ((self.base as u64) < 0x10000000) || ((self.base as u64) > 0x7FFFFFFF) {
			return false;
		}

		// Ensure this dialog_object is not null by checking for V in "Vera" and M in MYRIADPR
		unsafe {
			let character = *cast!(self.base as u64 + 0xEC, u8) as char;
			if character != 'V' && character != 'M' {
				return false;
			}
		}

		return true;
	}

	pub fn name(&self) {
		debug_println!("Name: ", );
	}
}

// impl Default for DialogObject {
// 	fn default() -> Self {
// 		Self {
// 			base: std::ptr::null_mut(),
// 		}
// 	}
// }

// https://stackoverflow.com/questions/19650265/is-there-a-faster-shorter-way-to-initialize-variables-in-a-rust-struct