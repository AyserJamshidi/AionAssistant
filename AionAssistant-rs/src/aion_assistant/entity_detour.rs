use std::arch::asm;
use std::collections::HashMap;
use debug_print::{debug_eprintln, debug_println};
use detour::GenericDetour;
use crate::aion_assistant::helpers::wait_and_get_module;

// static mut DO_EMOTE: Option<GenericDetour<extern "stdcall" fn()>> = None;
// #[no_mangle]
pub static mut ENTITY_HASHMAP_PTR: *mut HashMap<u64, u32> = std::ptr::null_mut();
pub static mut CURRENT_TIME_PTR: *mut u32 = std::ptr::null_mut();

// #[derive(Default)]
pub(crate) struct EntityDetour {
	// Required vars
	desired_hashmap_ptr: *mut HashMap<u64, u32>,
	desired_time_ptr: *mut u32,
	desired_pattern: String,

	// Internal vars
	detour: Option<GenericDetour<extern "stdcall" fn()>>,
}

impl EntityDetour {
	pub(crate) fn new(
		desired_hashmap_ptr: *mut HashMap<u64, u32>,
		desired_time_ptr: *mut u32,
		desired_pattern: String,
	) -> EntityDetour {
		EntityDetour { desired_hashmap_ptr, desired_time_ptr, desired_pattern, detour: None }
	}

	pub(crate) unsafe fn initialize(&mut self) -> bool {
		// Assign pointers
		ENTITY_HASHMAP_PTR = self.desired_hashmap_ptr;
		CURRENT_TIME_PTR = self.desired_time_ptr;

		// Get CryEntitySystem.dll module handle
		let cry_entity_system_client = wait_and_get_module("CryEntitySystem.dll").unwrap();

		let detour_start_addr = match cry_entity_system_client.find_pattern(self.desired_pattern.as_str()) {
			// let detour_start_address = match cry_entity_system_client.find_pattern("48 8B 43 28 48 83 C4 20") {
			Some(i) => {
				debug_println!("[+] Address: 0x{:x}", i);
				cry_entity_system_client.base_address + i // CryEntitySystem.dll + Pattern offset
				// cry_entity_system_client.base_address + i + 0x8 + 0x4 + 0x1 // Return + skip 12 bytes
			}
			None => {
				debug_eprintln!("Entity Map pattern not found!");
				return false;
			}
		};

		// Create detour_start_addr as a function we can hook later
		let orig_func = std::mem::transmute::<*const usize, extern "stdcall" fn()>(detour_start_addr as *const usize);

		// Assign detour as the original function
		self.detour = Some(
			GenericDetour::<extern "stdcall" fn()>::new(orig_func, entity_detour_function).unwrap()
		);

		return match self.detour {
			Some(ref mut hook) => {
				unsafe { hook.enable().unwrap(); }
				debug_println!("Entity Map Hooked!");
				true
			}
			None => {
				debug_eprintln!("Could not enable hook...");
				false
			}
		};
	}

	pub(crate) unsafe fn terminate(&mut self) {
		match self.detour {
			Some(ref mut hook) => {
				hook.disable().unwrap();
				debug_println!("Entity Map Unhooked!");
			}
			None => {
				debug_eprintln!("Could not disable hook...");
			}
		}
	}
}

/// Stores <b>CEntity address</b> and the <b>current time</b> in a hashmap.
/// The current time will be used to know the last time an entity was updated.
/// This is required to filter out useless entities from the hashmap.
extern "stdcall" fn entity_detour_function() {
	let entity_address: u64;

	// For some reason, we must access the hashmap via this variable, otherwise
	// accessing it directly will cause RAX to be modified
	let entity_hashmap = (unsafe { &mut *ENTITY_HASHMAP_PTR });

	unsafe {
		// Store rax into entity_address
		asm!(
		"mov {}, rax", out(reg) entity_address
		);

		// Store specific registries that get changes when inserting into hashmap
		asm!(
		"push rax",
		"push rcx",
		"push r8",
		"push r9",
		"push r10",
		"push r11",
		);

		// Insert entity_address into hashmap
		// (*ENTITY_HASHMAP_PTR).insert(entity_address, 0);
		entity_hashmap.insert(entity_address, *CURRENT_TIME_PTR);

		// Restore registries
		asm!(
		"pop r11",
		"pop r10",
		"pop r9",
		"pop r8",
		"pop rcx",
		"pop rax",
		);

		// Zero out FPU registries that changed due to hashmap insert
		asm!(
		"pxor xmm0, xmm0",
		"pxor xmm1, xmm1",
		"pxor xmm5, xmm5",
		);
	}
}