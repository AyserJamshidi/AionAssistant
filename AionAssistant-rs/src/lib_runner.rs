use std::collections::HashMap;
use std::ffi::c_void;
use std::sync::{Arc, Mutex};
use active_win_pos_rs::get_active_window;
use debug_print::{debug_eprintln, debug_println};

use toy_arms::utils::keyboard::{detect_keypress, VirtualKeyCode};
use crate::aion::structures::entity::c_entity::CEntity;
use crate::aion_assistant::entity_detour::EntityDetour;
use crate::aion_assistant::threads::managers::entity_map::entity_map;
use crate::aion_assistant::threads::managers::time_updater::time_updater;
use crate::aion_assistant::threads::structures::{EntityMapVariables, GlobalVariables, TimeUpdaterVariables, VariablesContainer};

pub(crate) fn main_loop(_hmodule_dll: *mut c_void) {
	// ! ---------- Passed vars ----------
	// Globals
	let mut is_running: bool = false;
	let mut current_time: u32 = 0;

	// Entity containers
	let mut centity_hashmap_raw: HashMap<u64, u32> = HashMap::new();
	let mut centity_hashmap_actual: HashMap<u64, CEntity> = HashMap::new();
	let mut centity_player: u64 = 0;

	// let cry_entity_system_dll = wait_and_get_module("CryEntitySystem.dll");
	// let game_dll = wait_and_get_module("Game.dll");

	// ! ------ Variable containers ------
	let global_variables = GlobalVariables {
		is_running: &mut is_running,
		current_time: &mut current_time,
	};

	let time_updater_variables = TimeUpdaterVariables {
		global_variables: &global_variables,
	};

	let entity_map_variables = EntityMapVariables {
		global_variables: &global_variables,
		centity_hashmap_raw: &mut centity_hashmap_raw,
		centity_hashmap_actual: &mut centity_hashmap_actual,
		centity_player: &mut centity_player,
	};

	let variables_container = VariablesContainer {
		global_variables: Arc::new(Mutex::new(global_variables)),
		time_updater_variables: Arc::new(Mutex::new(time_updater_variables)),
		entity_map_variables: Arc::new(Mutex::new(entity_map_variables)),
	};

	let mut entity_detour = EntityDetour::new(
		&mut centity_hashmap_raw,
		&mut current_time,
		"C3 CC CC CC CC CC CC 48 8D ? ? ? ? ? 48 89 ? 33 C0".to_string(),
		// desired_pattern: Some(String::from("48 8B 40 20 48 83 C4 20 5B C3 CC CC")),
		// desired_pattern: Some(String::from("48 89 53 18 48 8B 40 20")),
	);

	// ! Done with variables

	debug_println!("Starting main loop!");
	// debug_println!("Location: {:?}", std::env::current_dir());

	loop {
		unsafe {
			debug_println!("Looping! HM: {} -- HM_Actual: {} -- Time: {:?}", centity_hashmap_raw.len(), centity_hashmap_actual.len(), *(variables_container.global_variables.lock().unwrap().current_time));

			// Sometimes get_active_window returns an error, so we must check before calling unwrap
			let active_window = get_active_window();

			// If this Aion window is the currently active window
			if active_window.is_ok() && active_window.unwrap().process_id == std::process::id() as u64 {

				if detect_keypress(VirtualKeyCode::VK_LEFT) {
					if let Some(i) = CEntity::from_raw(0x40548800) {
						debug_println!("test_me: {:?}", i.character.get_animation_speed());
					} else {
						debug_eprintln!("CEntity address given was invalid!");
					}

					// let lua = Lua::new();
					//
					// lua_test_func(&lua);
					//
					// lua.load("hello()").exec().unwrap();
				}

				if detect_keypress(VirtualKeyCode::VK_RIGHT) {
					if !is_running {
						debug_println!("is_initialized == false, rejecting is_initialized = false request");
						continue;
					}

					terminate(&mut is_running, &mut entity_detour);
				}

				if detect_keypress(VirtualKeyCode::VK_UP) {
					if is_running {
						debug_println!("is_initialized == true, rejecting init request!");
						continue;
					}

					initialize(&mut is_running, &mut entity_detour, &variables_container);
				}

				if detect_keypress(VirtualKeyCode::VK_DOWN) {
					if is_running {
						debug_println!("is_initialized == true, rejecting quit request!");
						continue;
					}

					break;
				}
			}
		}

		std::thread::sleep(std::time::Duration::from_millis(300));
	}

	debug_println!("Main loop ended!");

	debug_println!("Threads ended!");
}

unsafe fn initialize(is_running: &mut bool, detour: &mut EntityDetour, variables_container: &VariablesContainer) {
	*is_running = true;

	debug_println!("Initializing thread...");

	(*detour).initialize();

	debug_println!("Initializing threads...");

	initialize_threads(&variables_container);

	// let hooks: Box<dyn hooks::Hooks> = Dx9HookExample::new().into_hook::<ImguiDx9Hooks>();
	//
	// unsafe {
	// 	hooks.hook();
	// }
	//
	// hudhook::lifecycle::global_state::set_hooks(hooks);


	// let game_hmodule_wrapped = get_module_handle("Game.dll");
	//
	// if game_hmodule_wrapped.is_none() {
	// 	debug_println!("Could not find game.dll base address!");
	// 	return;
	// }

	// let game_module = Module::from_handle(game_hmodule_wrapped.unwrap()).unwrap();

	// Entity List
	// let entity_list_offset = 0xA23668;
	//
	// unsafe {
	// 	let ptr_one = CEntitySystem::from_raw(game_module.read(entity_list_offset as usize)).unwrap();
	// 	debug_println!("ptr_one: {:?}", (*ptr_one).base);
	//
	// 	if (*ptr_one).is_valid() {
	// 		let ptr_two = CEntity::from_raw(((*ptr_one).base as u64 + 0x58 as u64) as *const usize).unwrap();
	// 		debug_println!("ptr_two: {:?}", (*ptr_two).base);
	//
	// 		if (*ptr_two).is_valid() {
	// 			let ptr_three = CEntity::from_raw(((*ptr_two).base as u64 + 0x10 as u64) as *const usize).unwrap();
	// 			debug_println!("ptr_thr: {:?}", (*ptr_three).base);
	//
	// 			let mut changing_ptr = CEntity::from_raw(((*ptr_three).base as u64 + 0x10 as u64) as *const usize).unwrap();
	// 			let mut index = 0;
	//
	// 			while (*changing_ptr).is_valid() && index < 300 {
	// 				debug_println!("changing_ptr[{:?}]: {:?}", index, (*changing_ptr).base);
	// 				changing_ptr = CEntity::from_raw(((*changing_ptr).base as u64 + 0x10 as u64) as *const usize).unwrap();
	// 				index += 1;
	// 			}
	// 		}
	// 	}
	// }


	// let dialog_array_offset = 0x1292B80;
	//
	// let mut found_count = 0;
	// for cur_dialog_num in 0x0..0xF08 {
	// 	unsafe {
	// 		let asdf = DialogObject::from_raw(game_module.read((dialog_array_offset + (0x8 * cur_dialog_num)) as usize)).unwrap();
	//
	// 		if (*asdf).is_valid() {
	// 			found_count += 1;
	// 		}
	//
	// 		(*asdf).name();
	// 		// debug_println!("Found {:?}", asdf);
	// 	}
	// }
	//
	// debug_println!("found_count: {:?}", found_count);
}

unsafe fn initialize_threads(variables_container: &VariablesContainer) {
	let time_updater_variables = Arc::clone(&variables_container.time_updater_variables);
	let entity_map_variables = Arc::clone(&variables_container.entity_map_variables);

	// ! Create threads
	// Time Updater
	std::thread::spawn(move || { time_updater(time_updater_variables); });

	// Entity Map Manager
	std::thread::spawn(move || { entity_map(entity_map_variables); });
}

unsafe fn terminate(is_running: &mut bool, detour: &mut EntityDetour) {
	debug_println!("Terminating...");

	(*detour).terminate();
	*is_running = false;

	debug_println!("Terminated.");
}

// fn wait_and_get_module(module_name: &str) -> Option<Module> {
// 	debug_println!("Waiting for {}...", module_name);
//
// 	let returned_module: Option<Module>;
//
// 	for _ in 0..60 {
// 		returned_module = Module::from_name(module_name);
//
// 		if returned_module.is_some() {
// 			debug_println!("'{}' handle = {:?}", module_name, returned_module.unwrap().handle as usize);
// 			return returned_module;
// 		}
//
// 		std::thread::sleep(std::time::Duration::from_secs(1));
// 	}
//
// 	return None;
// }