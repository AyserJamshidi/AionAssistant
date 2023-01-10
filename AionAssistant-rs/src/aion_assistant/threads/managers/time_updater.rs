use std::sync::{Arc, Mutex};
use std::thread::sleep;
use std::time::Duration;
use debug_print::debug_println;
use crate::aion_assistant::threads::structures::TimeUpdaterVariables;

pub(crate) unsafe fn time_updater(time_updater_variables: Arc<Mutex<TimeUpdaterVariables>>) {
	debug_println!("TimeManager started!");

	let is_running = (*time_updater_variables.lock().unwrap().global_variables).is_running;
	let current_time = (*time_updater_variables.lock().unwrap().global_variables).current_time;

	while *is_running {
		*current_time += 1;
		sleep(Duration::from_millis(100));
	}

	debug_println!("TimeManager ended!");
}