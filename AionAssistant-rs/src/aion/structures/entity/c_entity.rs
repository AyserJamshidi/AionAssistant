use debug_print::{debug_eprintln, debug_println};
use toy_arms::internal::cast;
use crate::aion::structures::entity::character_object::CharacterObject;

// #[derive(Hash, Eq)]
pub(crate) struct CEntity {
	pub(crate) base: *mut u64,
	// Denote the base address of LocalPlayer to use it later in get_health() function.
	assigned_type: u64,
	pub(crate) character: CharacterObject,
}

// pub(crate) trait AionAddress {
// 	fn is_valid(&self) -> bool;
// }

impl CEntity {
	// Constructor-ish function
	pub(crate) unsafe fn from_raw(base: u64) -> Option<Self> {
		let ptr = (base as *mut Self) as *mut u32;

		// ! ----

		// * False if base is null
		if ptr.is_null() {
			return None;
		}

		// * False if base address is out of expected bounds [0x10000000, 0x7FFFFFFF]
		// if ((ptr as u64) < 0x10000000) || ((ptr as u64) > 0x7FFFFFFF) {
		// 	return None;
		// }

		// * Get 8 slices of 4-byte elements then check for 0x1 at index 7
		let byte_slice = std::slice::from_raw_parts(ptr, 8);
		let slice_seven = byte_slice.get(7);

		if slice_seven.is_none() || *slice_seven.unwrap() != 0x1 {
			debug_println!("Invalid Address {:08X}", base);
			return None;
		}

		// * False if offset 0x368 is not a valid adress
		let character_object_addr = *cast!(base + 0x368, usize);
		if (character_object_addr < 0x10000000) || (character_object_addr > 0x7FFFFFFF) {
			return None;
		}

		// ! ----

		// Acquire a new CEntity object
		let char = CharacterObject::from_raw(character_object_addr);

		if char.is_none() {
			debug_eprintln!("Invalid CEntity!");
			return None;
		}

		debug_println!("Created CEntity!");
		Some(CEntity {
			base: base as *mut u64,
			assigned_type: 0x0,
			character: char.unwrap(),
		})
	}

	pub fn equals_base(&self, other_base: u64) -> bool {
		self.base == other_base as _
	}

	pub unsafe fn is_valid(&self) -> bool {
		// False if base is null
		if self.base.is_null() {
			return false;
		}

		// if *cast!(self.assigned_type, char) == '\0' {
		// 	return false;
		// }

		// False if base address is out of expected bounds [0x10000000, 0x7FFFFFFF]
		// if ((self.base as u64) < 0x10000000) || ((self.base as u64) > 0x7FFFFFFF) {
		// 	return false;
		// }

		return true;
	}

	pub fn character_object(&self) -> &CharacterObject {
		&self.character
	}

	// pub(crate) unsafe fn get_character_object(&mut self) -> &mut CharacterObject {
	// 	// *cast!(*(self.base) + 0x2C0, CharacterObject)
	// 	// CharacterObject::from_raw(self.get_value(0x2C0), self)
	// 	&mut self.character_object
	// }
}

// impl PartialEq for CEntity {
// 	fn eq(&self, other: &Self) -> bool {
// 		self.base == other.base
// 	}
// }

// impl UserData for CEntity {
// 	fn add_fields<'lua, F: UserDataFields<'lua, Self>>(fields: &mut F) {
// 		// https://github.com/khvzak/mlua/blob/master/examples/userdata.rs
// 		todo!()
// 	}
//
// 	fn add_methods<'lua, M: UserDataMethods<'lua, Self>>(methods: &mut M) {
//
// 		// methods.add_method("new", |_, this, ()| Ok(this.base));
// 		methods.add_function("new",
// 							 |_, addr: u64| Ok(CEntity::from_raw(addr as _)),
// 		);
// 	}
// }