#include "miklua.h"

#include "log.h"

#define MIK_LUA(name) static int mik_##name(lua_State* L)

namespace mik {
	static const void *luaL_checklightuserdata(lua_State *L, int narg) {
		luaL_checktype(L, narg, LUA_TLIGHTUSERDATA);
		return lua_topointer( L, narg );
	}

	static bool luaL_checkboolean(lua_State *L, int narg) {
		luaL_checktype(L, narg, LUA_TBOOLEAN);
		return lua_toboolean(L, narg) == 1 ? true : false;
	}

	class MikLuaGame : public MikGame {
	public:
		void onInit(Mik& mik) {
			MikLua::call("_init");
		}

		void onUpdate(Mik& mik, float timeStep) {
			MikLua::call("_update", timeStep);
		}

		void onDraw(Mik& mik) {
			MikLua::call("_draw");
		}

		void onExit(Mik& mik) {
			MikLua::call("_exit");
		}
	};

	Mik* MikLua::mik;
	lua_State* MikLua::L;

	#define MIK MikLua::mik

	MIK_LUA(quit) {
		MIK->quit();
		return 0;
	}

	MIK_LUA(btnp) {
		const char* btn = luaL_checkstring(L, 1);
		lua_pushboolean(L, MIK->buttonPressed(std::string(btn)) ? 1 : 0);
		return 1;
	}

	MIK_LUA(btnr) {
		const char* btn = luaL_checkstring(L, 1);
		lua_pushboolean(L, MIK->buttonReleased(std::string(btn)) ? 1 : 0);
		return 1;
	}

	MIK_LUA(btnh) {
		const char* btn = luaL_checkstring(L, 1);
		lua_pushboolean(L, MIK->buttonHeld(std::string(btn)) ? 1 : 0);
		return 1;
	}

	MIK_LUA(warp) {
		i32 x = i32(luaL_checknumber(L, 1));
		i32 y = i32(luaL_checknumber(L, 2));
		MIK->warpMouse(x, y);
		return 0;
	}

	MIK_LUA(mousex) {
		lua_pushinteger(L, MIK->mouseX());
		return 1;
	}

	MIK_LUA(mousey) {
		lua_pushinteger(L, MIK->mouseY());
		return 1;
	}

	MIK_LUA(title) {
		if (lua_gettop(L) == 0) {
			const char* title = luaL_checkstring(L, 1);
			MIK->setTitle(std::string(title));
			return 0;
		} else {
			lua_pushstring(L, MIK->title().c_str());
			return 1;
		}
	}

	MIK_LUA(key) {
		u8 r = u8(i32(luaL_checknumber(L, 1)) & 0xFF);
		u8 g = u8(i32(luaL_checknumber(L, 2)) & 0xFF);
		u8 b = u8(i32(luaL_checknumber(L, 3)) & 0xFF);
		MIK->key(r, g, b);
		return 0;
	}

	MIK_LUA(clip) {
		if (lua_gettop(L) < 4) {
			MIK->unclip();
		} else {
			i32 x = i32(luaL_checknumber(L, 1));
			i32 y = i32(luaL_checknumber(L, 2));
			i32 w = i32(luaL_checknumber(L, 3));
			i32 h = i32(luaL_checknumber(L, 4));
			MIK->clip(x, y, w, h);
		}
		return 0;
	}

	MIK_LUA(clear) {
		if (lua_gettop(L) == 0) {
			MIK->clear(0, 0, 0);
		} else {
			u8 r = u8(i32(luaL_checknumber(L, 1)) & 0xFF);
			u8 g = u8(i32(luaL_checknumber(L, 2)) & 0xFF);
			u8 b = u8(i32(luaL_checknumber(L, 3)) & 0xFF);
			MIK->clear(r, g, b);
		}
		return 0;
	}

	MIK_LUA(dot) {
		i32 x = i32(luaL_checknumber(L, 1));
		i32 y = i32(luaL_checknumber(L, 2));
		u8 r = u8(i32(luaL_checknumber(L, 3)) & 0xFF);
		u8 g = u8(i32(luaL_checknumber(L, 4)) & 0xFF);
		u8 b = u8(i32(luaL_checknumber(L, 5)) & 0xFF);
		MIK->dot(x, y, r, g, b);
		return 0;
	}

	MIK_LUA(line) {
		i32 x1 = i32(luaL_checknumber(L, 1));
		i32 y1 = i32(luaL_checknumber(L, 2));
		i32 x2 = i32(luaL_checknumber(L, 3));
		i32 y2 = i32(luaL_checknumber(L, 4));
		u8 r = u8(i32(luaL_checknumber(L, 5)) & 0xFF);
		u8 g = u8(i32(luaL_checknumber(L, 6)) & 0xFF);
		u8 b = u8(i32(luaL_checknumber(L, 7)) & 0xFF);
		MIK->line(x1, y1, x2, y2, r, g, b);
		return 0;
	}

	MIK_LUA(rect) {
		i32 x = i32(luaL_checknumber(L, 1));
		i32 y = i32(luaL_checknumber(L, 2));
		i32 w = i32(luaL_checknumber(L, 3));
		i32 h = i32(luaL_checknumber(L, 4));
		u8 r = u8(i32(luaL_checknumber(L, 5)) & 0xFF);
		u8 g = u8(i32(luaL_checknumber(L, 6)) & 0xFF);
		u8 b = u8(i32(luaL_checknumber(L, 7)) & 0xFF);
		MIK->rect(x, y, w, h, r, g, b);
		return 0;
	}

	MIK_LUA(rectf) {
		i32 x = i32(luaL_checknumber(L, 1));
		i32 y = i32(luaL_checknumber(L, 2));
		i32 w = i32(luaL_checknumber(L, 3));
		i32 h = i32(luaL_checknumber(L, 4));
		u8 r = u8(i32(luaL_checknumber(L, 5)) & 0xFF);
		u8 g = u8(i32(luaL_checknumber(L, 6)) & 0xFF);
		u8 b = u8(i32(luaL_checknumber(L, 7)) & 0xFF);
		MIK->rectf(x, y, w, h, r, g, b);
		return 0;
	}

	MIK_LUA(circ) {
		i32 x = i32(luaL_checknumber(L, 1));
		i32 y = i32(luaL_checknumber(L, 2));
		i32 rad = i32(luaL_checknumber(L, 3));
		u8 r = u8(i32(luaL_checknumber(L, 4)) & 0xFF);
		u8 g = u8(i32(luaL_checknumber(L, 5)) & 0xFF);
		u8 b = u8(i32(luaL_checknumber(L, 6)) & 0xFF);
		MIK->circ(x, y, rad, r, g, b);
		return 0;
	}

	MIK_LUA(circf) {
		i32 x = i32(luaL_checknumber(L, 1));
		i32 y = i32(luaL_checknumber(L, 2));
		i32 rad = i32(luaL_checknumber(L, 3));
		u8 r = u8(i32(luaL_checknumber(L, 4)) & 0xFF);
		u8 g = u8(i32(luaL_checknumber(L, 5)) & 0xFF);
		u8 b = u8(i32(luaL_checknumber(L, 6)) & 0xFF);
		MIK->circf(x, y, rad, r, g, b);
		return 0;
	}

	MIK_LUA(spr) {
		Sprite* bmp = (Sprite*) luaL_checklightuserdata(L, 1);
		i32 x = i32(luaL_checknumber(L, 2));
		i32 y = i32(luaL_checknumber(L, 3));
		i32 sx = 0, sy = 0, sw = 0, sh = 0;
		bool flipx = false, flipy = false;
		if (lua_gettop(L) == 7) {
			sx = i32(luaL_checknumber(L, 4));
			sy = i32(luaL_checknumber(L, 5));
			sw = i32(luaL_checknumber(L, 6));
			sh = i32(luaL_checknumber(L, 7));
		}
		if (lua_gettop(L) >= 8) {
			flipx = luaL_checkboolean(L, 8);
		} else if (lua_gettop(L) == 9) {
			flipy = luaL_checkboolean(L, 9);
		}
		MIK->spr(bmp, x, y, sx, sy, sw, sh, flipx, flipy);
		return 0;
	}

	MIK_LUA(aspr) {
		Sprite* bmp = (Sprite*) luaL_checklightuserdata(L, 1);
		i32 x = i32(luaL_checknumber(L, 2));
		i32 y = i32(luaL_checknumber(L, 3));
		bool flipx = false, flipy = false;
		if (lua_gettop(L) >= 4) {
			flipx = luaL_checkboolean(L, 4);
		} else if (lua_gettop(L) == 5) {
			flipy = luaL_checkboolean(L, 5);
		}
		MIK->aspr(bmp, x, y, flipx, flipy);
		return 0;
	}

	MIK_LUA(tile) {
		Sprite* bmp = (Sprite*) luaL_checklightuserdata(L, 1);
		i32 index = i32(luaL_checknumber(L, 2));
		i32 x = i32(luaL_checknumber(L, 3));
		i32 y = i32(luaL_checknumber(L, 4));
		bool flipx = false, flipy = false;
		if (lua_gettop(L) >= 5) {
			flipx = luaL_checkboolean(L, 5);
		}
		if (lua_gettop(L) == 6) {
			flipy = luaL_checkboolean(L, 6);
		}
		MIK->tile(bmp, index, x, y, flipx, flipy);
		return 0;
	}

	MIK_LUA(chr) {
		Sprite* fnt = (Sprite*) luaL_checklightuserdata(L, 1);
		const char* map = luaL_checkstring(L, 2);
		const char* chrs = luaL_checkstring(L, 3);
		if (strlen(chrs) == 0) {
			LogE("[lua] invalid char.");
			return 0;
		}
		i32 x = i32(luaL_checknumber(L, 4));
		i32 y = i32(luaL_checknumber(L, 5));
		lua_pushinteger(L, MIK->chr(fnt, std::string(map), chrs[0], x, y));
		return 1;
	}

	MIK_LUA(text) {
		Sprite* fnt = (Sprite*) luaL_checklightuserdata(L, 1);
		const char* map = luaL_checkstring(L, 2);
		const char* txt = luaL_checkstring(L, 3);
		i32 x = i32(luaL_checknumber(L, 4));
		i32 y = i32(luaL_checknumber(L, 5));
		MIK->text(fnt, std::string(map), std::string(txt), x, y);
		return 0;
	}

	MIK_LUA(load_sprite) {
		const char* fname = luaL_checkstring(L, 1);
		i32 rows = 1, cols = 1;
		if (lua_gettop(L) == 3) {
			rows = luaL_checkinteger(L, 2);
			cols = luaL_checkinteger(L, 3);
		}
		Sprite* bmp = MIK->loadSprite(std::string(fname), rows, cols);
		lua_pushlightuserdata(L, bmp);
		return 1;
	}

	MIK_LUA(create_sprite) {
		i32 w = i32(luaL_checknumber(L, 1));
		i32 h = i32(luaL_checknumber(L, 2));
		Sprite* bmp = MIK->createSprite(w, h);
		lua_pushlightuserdata(L, bmp);
		return 1;
	}

	// Helper functions
	MIK_LUA(width) {
		if (lua_gettop(L) == 0) {
			lua_pushinteger(L, MikScreenWidth);
		} else {
			Sprite* bmp = (Sprite*) luaL_checklightuserdata(L, 1);
			lua_pushinteger(L, bmp->width());
		}
		return 1;
	}

	MIK_LUA(height) {
		if (lua_gettop(L) == 0) {
			lua_pushinteger(L, MikScreenHeight);
		} else {
			Sprite* bmp = (Sprite*) luaL_checklightuserdata(L, 1);
			lua_pushinteger(L, bmp->height());
		}
		return 1;
	}

	MIK_LUA(pget) {
		Sprite* bmp = (Sprite*) luaL_checklightuserdata(L, 1);
		i32 x = i32(luaL_checknumber(L, 2));
		i32 y = i32(luaL_checknumber(L, 3));
		Color col = bmp->get(x, y);
		lua_pushinteger(L, col.r);
		lua_pushinteger(L, col.g);
		lua_pushinteger(L, col.b);
		return 3;
	}

	MIK_LUA(pset) {
		Sprite* bmp = (Sprite*) luaL_checklightuserdata(L, 1);
		i32 x = i32(luaL_checknumber(L, 2));
		i32 y = i32(luaL_checknumber(L, 3));
		u8 r = u8(i32(luaL_checknumber(L, 4)) & 0xFF);
		u8 g = u8(i32(luaL_checknumber(L, 5)) & 0xFF);
		u8 b = u8(i32(luaL_checknumber(L, 6)) & 0xFF);
		bool ghost = false;
		if (lua_gettop(L) == 7) {
			ghost = luaL_checkboolean(L, 7);
		}
		bmp->dot(x, y, r, g, b, ghost);
		return 0;
	}

	MIK_LUA(frame) {
		Sprite* spr = (Sprite*) luaL_checklightuserdata(L, 1);
		lua_pushinteger(L, spr->animator().frame());
		return 1;
	}

	MIK_LUA(animation) {
		Sprite* spr = (Sprite*) luaL_checklightuserdata(L, 1);
		lua_pushstring(L,spr->animator().animation().c_str());
		return 1;
	}

	MIK_LUA(play) {
		Sprite* spr = (Sprite*) luaL_checklightuserdata(L, 1);
		const char* name = luaL_checkstring(L, 2);
		float speed = luaL_checknumber(L, 3);
		bool loop = luaL_checkboolean(L, 4);
		spr->animator().play(std::string(name), speed, loop);
		return 0;
	}

	MIK_LUA(add_animation) {
		Sprite* spr = (Sprite*) luaL_checklightuserdata(L, 1);
		const char* name = luaL_checkstring(L, 2);
		if (lua_gettop(L) == 3) {
			// Get frames table
			std::vector<u32> frames;

			luaL_checktype(L, 3, LUA_TTABLE);
			const int len = lua_rawlen(L, 3);
			for (int i = 1; i <= len; ++i) {
				lua_pushinteger(L, i);
				lua_gettable(L, -2);
				frames.push_back(u32(lua_tonumber( L, -1 )));
				lua_pop(L, 1);
			}
			spr->animator().add(std::string(name), frames);
		} else {
			spr->animator().add(std::string(name));
		}
		return 0;
	}

	MIK_LUA(load_sound) {
		const char* fname = luaL_checkstring(L, 1);
		Sound* snd = MIK->loadSound(std::string(fname));
		lua_pushlightuserdata(L, snd);
		return 1;
	}

	MIK_LUA(play_sound) {
		Sound* snd = (Sound*) luaL_checklightuserdata(L, 1);
		int args = lua_gettop(L);
		f32 gain = 1.0f;
		f32 pitch = 1.0f;
		f32 pan = 0.0f;
		if (args >= 2) {
			gain = luaL_checknumber(L, 2);
		}
		if (args >= 3) {
			pitch = luaL_checknumber(L, 3);
		}
		if (args >= 4) {
			pan = luaL_checknumber(L, 4);
		}
		Voice* voc = MIK->audio()->play(snd, gain, pitch, pan);
		lua_pushlightuserdata(L, voc);
		return 1;
	}

	MIK_LUA(stop_sound) {
		Sound* snd = (Sound*) luaL_checklightuserdata(L, 1);
		MIK->audio()->stop(snd);
		return 0;
	}

	MIK_LUA(play_music) {
		Sound* snd = (Sound*) luaL_checklightuserdata(L, 1);
		int args = lua_gettop(L);
		f32 gain = 1.0f;
		f32 pan = 1.0f;
		f32 fade = 0.0f;
		if (args >= 2) {
			gain = luaL_checknumber(L, 2);
		}
		if (args >= 3) {
			pan = luaL_checknumber(L, 3);
		}
		if (args >= 4) {
			fade = luaL_checknumber(L, 4);
		}
		MIK->audio()->playMusic(snd, gain, pan, fade);
		return 0;
	}

	MIK_LUA(stop_music) {
		int args = lua_gettop(L);
		f32 fade = 0.0f;
		if (args == 1) {
			fade = luaL_checknumber(L, 1);
		}
		MIK->audio()->stopMusic(fade);
		return 0;
	}

	MIK_LUA(volume) {
		Voice* voc = (Voice*) luaL_checklightuserdata(L, 1);
		if (lua_gettop(L) == 2) {
			voc->gain = luaL_checknumber(L, 2);
			return 0;
		} else {
			lua_pushnumber(L, voc->gain);
			return 1;
		}
	}

	MIK_LUA(master_volume) {
		Voice* voc = (Voice*) luaL_checklightuserdata(L, 1);
		if (lua_gettop(L) == 1) {
			MIK->audio()->gain(luaL_checknumber(L, 2));
			return 0;
		} else {
			lua_pushnumber(L, MIK->audio()->gain());
			return 1;
		}
	}

	MIK_LUA(pitch) {
		Voice* voc = (Voice*) luaL_checklightuserdata(L, 1);
		if (lua_gettop(L) == 2) {
			voc->pitch = luaL_checknumber(L, 2);
			return 0;
		} else {
			lua_pushnumber(L, voc->pitch);
			return 1;
		}
	}

	MIK_LUA(pan) {
		Voice* voc = (Voice*) luaL_checklightuserdata(L, 1);
		if (lua_gettop(L) == 2) {
			voc->pan = luaL_checknumber(L, 2);
			return 0;
		} else {
			lua_pushnumber(L, voc->pan);
			return 1;
		}
	}

	MIK_LUA(lerp) {
		f32 a = luaL_checknumber(L, 1);
		f32 b = luaL_checknumber(L, 2);
		f32 t = luaL_checknumber(L, 3);
		lua_pushnumber(L, math::lerp(a, b, t));
		return 1;
	}

	MIK_LUA(clamp) {
		f32 v = luaL_checknumber(L, 1);
		f32 mi = luaL_checknumber(L, 2);
		f32 ma = luaL_checknumber(L, 3);
		lua_pushnumber(L, math::clamp(v, mi, ma));
		return 1;
	}

	#define MIK_ENTRY(name) { #name, mik_##name }
	static const luaL_Reg miklib[] = {
		MIK_ENTRY(quit),
		MIK_ENTRY(btnp),
		MIK_ENTRY(btnr),
		MIK_ENTRY(btnh),
		MIK_ENTRY(warp),
		MIK_ENTRY(mousex),
		MIK_ENTRY(mousey),
		MIK_ENTRY(title),
		MIK_ENTRY(key),
		MIK_ENTRY(clip),
		MIK_ENTRY(clear),
		MIK_ENTRY(dot),
		MIK_ENTRY(line),
		MIK_ENTRY(rect),
		MIK_ENTRY(rectf),
		MIK_ENTRY(circ),
		MIK_ENTRY(circf),
		MIK_ENTRY(spr),
		MIK_ENTRY(aspr),
		MIK_ENTRY(tile),
		MIK_ENTRY(chr),
		MIK_ENTRY(text),
		MIK_ENTRY(load_sprite),
		MIK_ENTRY(create_sprite),
		MIK_ENTRY(width),
		MIK_ENTRY(height),
		MIK_ENTRY(pget),
		MIK_ENTRY(pset),
		MIK_ENTRY(frame),
		MIK_ENTRY(animation),
		MIK_ENTRY(play),
		MIK_ENTRY(add_animation),
		MIK_ENTRY(load_sound),
		MIK_ENTRY(play_sound),
		MIK_ENTRY(stop_sound),
		MIK_ENTRY(volume),
		MIK_ENTRY(master_volume),
		MIK_ENTRY(pitch),
		MIK_ENTRY(pan),
		MIK_ENTRY(play_music),
		MIK_ENTRY(stop_music),
		{ nullptr, nullptr }
	};

	static const luaL_Reg mathext[] = {
		MIK_ENTRY(lerp),
		MIK_ENTRY(clamp),
		{ nullptr, nullptr }
	};

	void MikLua::run(std::string const& scriptFile) {
		mik = new Mik(new MikLuaGame());

		L = luaL_newstate();
		luaL_openlibs(L);

		lua_newtable(L);
		luaL_setfuncs(L, miklib, 0);
		lua_setglobal(L, "mik");

		// Extend math :P
		for (auto&& reg : mathext) {
			if (reg.name == nullptr) break;
			lua_getglobal(L, "math");
			lua_pushstring(L, reg.name);
			lua_pushcfunction(L, reg.func);
			lua_settable(L, -3);
			lua_pop(L, 1);
		}

		if (luaL_dofile(L, scriptFile.c_str()) != 0) {
			LogE("[script] ", lua_tostring(L, -1));
			lua_close(L);
			delete mik;
			return;
		}

		mik->run();
		lua_close(L);
		delete mik;
	}

	void MikLua::call(std::string const& function) {
		lua_settop(L, 0);
		lua_getglobal(L, function.c_str());
		if (lua_isfunction(L, -1) == 0) return;
		if (lua_pcall(L, 0, 0, 0) != 0) {
			LogE("[lua: ", function, "] ", lua_tostring(L, -1));
		}
	}

	void MikLua::call(std::string const& function, float arg) {
		lua_settop(L, 0);
		lua_getglobal(L, function.c_str());
		if (lua_isfunction(L, -1) == 0) return;
		lua_pushnumber(L, arg);
		if (lua_pcall(L, 1, 0, 0) != 0) {
			LogE("[lua: ", function, "] ", lua_tostring(L, -1));
		}
	}
}