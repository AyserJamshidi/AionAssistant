use debug_print::debug_println;
use toy_arms::internal::module::Module;

pub(crate) fn wait_and_get_module(module_name: &str) -> Option<Module> {
	debug_println!("Inside wait_and_get_module!!");
	for _ in 0..60 {
		match Module::from_name(module_name) {
			Some(i) => {
				return Some(i);
			}
			None => {
				debug_println!("Waiting for {}...", module_name);
				std::thread::sleep(std::time::Duration::from_secs(1));
			}
		}
	}

	return None;
}