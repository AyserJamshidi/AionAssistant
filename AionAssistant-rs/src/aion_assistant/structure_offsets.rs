pub(crate) struct CEntity {
	pub(crate) entity_type: usize,
	pub(crate) is_moving: usize,
	pub(crate) character: usize,
}

impl CEntity {
	pub fn from_address(base: usize) -> CEntity {
		CEntity {
			entity_type: 0x1D0 + base,
			is_moving: 0x170 + base,
			character: 0x368 + base,
		}
	}
}