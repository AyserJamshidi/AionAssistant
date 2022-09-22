//
//
//extern "C" {
//#include "../../../Dependencies/Lua542/include/lua.h"
//#include "../../../Dependencies/Lua542/include/lauxlib.h"
//#include "../../../Dependencies/Lua542/include/lualib.h"
//}
//
//bool CheckLua(lua_State* L, int r) {
//	if (r != LUA_OK) {
//		printf("%s\n", lua_tostring(L, -1));
//		return false;
//	}
//
//	return true;
//}
//
//void LuaVirtualMachineManager(void* givenStruct) {
////	lua_State* L = luaL_newstate();
////	luaL_openlibs(L);
//}