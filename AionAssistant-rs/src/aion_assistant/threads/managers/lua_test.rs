use std::error::Error;
use debug_print::debug_println;
use mlua::Lua;
use mlua::prelude::LuaResult;
use crate::aion::structures::entity::c_entity::CEntity;

fn hello(_lua: &Lua, _: ()) -> LuaResult<()> {
	debug_println!("Hello!");
	Ok(())
}

pub(crate) fn lua_test_func(lua: &Lua) -> Result<(), Box<dyn Error>> {

	let function_container = lua.create_function(hello)?;
	lua.globals().set("hello", function_container)?;
	// lua.globals().set("CEntity", lua.create_proxy::<CEntity>().unwrap())?;



	Ok(())
}