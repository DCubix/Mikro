#ifndef MIK_LUA_H
#define MIK_LUA_H

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include <string>
#include "mik.h"

namespace mik {
	class MikLua {
	public:
		static void init();
		static void deinit();

		static std::vector<u8> compile(std::string const& scriptFile);

		static void run(std::string const& cartFile);
		static void run(std::vector<u8> const& bytecode);
		static void runScript(std::string const& scriptFile);

		static void call(std::string const& function);
		static void call(std::string const& function, float arg);

		static Mik* mik;
		static lua_State* L;
	};
}

#endif // MIK_LUA_H