use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use crate::aion::structures::entity::c_entity::CEntity;

pub(crate) struct VariablesContainer {
	pub(crate) global_variables: Arc<Mutex<GlobalVariables>>,
	pub(crate) time_updater_variables: Arc<Mutex<TimeUpdaterVariables>>,
	pub(crate) entity_map_variables: Arc<Mutex<EntityMapVariables>>,
}

pub(crate) struct GlobalVariables {
	pub(crate) is_running: *mut bool,
	pub(crate) current_time: *mut u32,
}
unsafe impl Send for GlobalVariables {}

pub(crate) struct TimeUpdaterVariables {
	pub(crate) global_variables: *const GlobalVariables,
}
unsafe impl Send for TimeUpdaterVariables {}

pub(crate) struct EntityMapVariables {
	pub(crate) global_variables: *const GlobalVariables,
	pub(crate) centity_hashmap_raw: *mut HashMap<u64, u32>,
	pub(crate) centity_hashmap_actual: *mut HashMap<u64, CEntity>,
	pub(crate) centity_player: *mut u64,
}
unsafe impl Send for EntityMapVariables {}