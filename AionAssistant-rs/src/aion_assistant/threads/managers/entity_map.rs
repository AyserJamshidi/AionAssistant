use std::sync::{Arc, Mutex};
use std::thread::sleep;
use std::time::Duration;
use debug_print::debug_println;
use crate::aion::structures::entity::c_entity::CEntity;
use crate::aion_assistant::threads::structures::EntityMapVariables;

pub(crate) unsafe fn entity_map(entity_map_variables: Arc<Mutex<EntityMapVariables>>) {
	debug_println!("EntityMapManager started!");

	// Global Variables
	let is_running = (*entity_map_variables.lock().unwrap().global_variables).is_running;
	let current_time = (*entity_map_variables.lock().unwrap().global_variables).current_time;

	// Entity Map Variables
	let centity_hashmap_raw = entity_map_variables.lock().unwrap().centity_hashmap_raw;
	let centity_hashmap_actual = entity_map_variables.lock().unwrap().centity_hashmap_actual;

	while *is_running {
		// * For each pair in the hashmap, if the value is less than 5, retain it, otherwise delete it
		(*centity_hashmap_raw).retain(|key, value| {
			// Check if the current time for this entity is within 500ms
			if (*current_time - *value) < 5 { // 5 indicates 500ms because the current_time is incremented every 100ms
				// Check if this entity's base address is already in hashmap_actual
				if (*centity_hashmap_actual).contains_key(key) {
					return true;
				}

				// // Adds a new CEntity for this entity into hashmap_actual
				if let Some(i) = CEntity::from_raw(*key) {
					(*centity_hashmap_actual).insert(*key, i);
				}

				return true;
			}

			// The entity was invalid, we don't want to keep it!
			if (*centity_hashmap_actual).contains_key(key) {
				(*centity_hashmap_actual).remove(key);
			}
			false
		});

		// (*centity_hashset).retain(|key| {
		// 	(*centity_hashmap).contains_key(&*key.base)
		// });

		sleep(Duration::from_secs(1));
	}

	debug_println!("EntityMapManager ended!");
}



/*

				// * For each valid address in the hashmap, turn it into a CEntity object and store it in the hashset
				if let Some(i) = CEntity::from_raw(*key) {
					if !(*centity_hashset).contains(&i) {
						(*centity_hashset).insert(i);
					}
					return true;
				}
 */