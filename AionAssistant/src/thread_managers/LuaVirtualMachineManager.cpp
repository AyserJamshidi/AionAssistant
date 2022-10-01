extern "C" {
#include "../../../Dependencies/Lua542/include/lua.h"
#include "../../../Dependencies/Lua542/include/lauxlib.h"
#include "../../../Dependencies/Lua542/include/lualib.h"
}

#include "../../include/thread_managers/threads.hpp"
#include "../memory/entity/AionEntity.h"

bool CheckLua(lua_State* L, int r) {
	if (r != LUA_OK) {
		printf("%s\n", lua_tostring(L, -1));
		return false;
	}

	return true;
}

void AionAssistantThread::LuaVirtualMachineManager(void* givenStruct) {
	LuaContext ctx;




	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	                                                   
	if (CheckLua(L, luaL_dofile(L, "test.lua"))) {
		lua_getglobal(L, "GetAnimationSpeed");

		if (lua_isfunction(L, -1)) {
			if (CheckLua(L, lua_pcall(L, 0, 1, 0))) {
				AionEntity::GetAnimationSpeed();
			}
		}

		AionEntity::SetAnimationSpeed();
	}
}