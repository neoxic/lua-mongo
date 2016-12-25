/*
 * Copyright (C) 2016 Arseny Vakhrushev <arseny.vakhrushev@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include "common.h"

static int _tostring(lua_State *L) {
	if (!luaL_getmetafield(L, 1, "__name")) return 0;
	lua_pushfstring(L, "%s: %p", lua_tostring(L, -1), lua_topointer(L, 1));
	return 1;
}

bool pushType(lua_State *L, const char *tname, const luaL_Reg *funcs) {
	if (!luaL_newmetatable(L, tname)) return false;
	lua_newtable(L);
	lua_setfield(L, -2, "__metatable"); /* Make metatable private */
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index"); /* mt.__index = mt */
	lua_pushstring(L, tname);
	lua_setfield(L, -2, "__name");
	lua_pushcfunction(L, _tostring);
	lua_setfield(L, -2, "__tostring");
#if LUA_VERSION_NUM >= 502
	luaL_setfuncs(L, funcs, 0);
#else
	luaL_register(L, 0, funcs);
#endif
	return true;
}

void setType(lua_State *L, const char *tname, const luaL_Reg *funcs) {
	pushType(L, tname, funcs);
	lua_setmetatable(L, -2);
}

void unsetType(lua_State *L) {
	lua_pushnil(L);
	lua_setmetatable(L, -2);
}

void pushHandle(lua_State *L, void *ptr) {
	assert(ptr);
	*(void **)lua_newuserdata(L, sizeof ptr) = ptr;
	if (!lua_touserdata(L, 1)) return; /* No parent handle */
	/* Save parent handle to make sure it doesn't get garbage collected */
#if LUA_VERSION_NUM >= 503
	lua_pushvalue(L, 1);
	lua_setuservalue(L, -2); /* Set directly as user value */
#else
	lua_createtable(L, 1, 0);
	lua_pushvalue(L, 1);
	lua_rawseti(L, -2, 1); /* Store as t[1] ... */
#if LUA_VERSION_NUM >= 502
	lua_setuservalue(L, -2); /* ... and set as user table */
#else
	lua_setfenv(L, -2); /* ... and set as environment table */
#endif
#endif
}

void packParams(lua_State *L, int n) {
	lua_settop(L, n);
	lua_createtable(L, n, 0);
	lua_insert(L, 1);
	while (n) lua_rawseti(L, 1, n--);
}

int unpackParams(lua_State *L, int idx) {
	int i = 0, n = lua_rawlen(L, idx);
	luaL_checkstack(L, n, "too many parameters");
	while (i < n) lua_rawgeti(L, idx, ++i);
	return n;
}

int commandError(lua_State *L, const bson_error_t *error) {
	lua_pushnil(L);
	lua_pushstring(L, error->message);
	return 2;
}

int commandStatus(lua_State *L, bool status, const bson_error_t *error) {
	if (!status) return commandError(L, error);
	lua_pushboolean(L, 1);
	return 1;
}

int commandReply(lua_State *L, bool status, bson_t *reply, const char *field, const bson_error_t *error) {
	if (!status) {
		bson_destroy(reply);
		return commandError(L, error);
	}
	if (field) {
		pushBSONField(L, reply, field);
		bson_destroy(reply);
		return 1;
	}
	pushBSONSteal(L, reply);
	return 1;
}

#if LUA_VERSION_NUM < 502
void *luaL_testudata(lua_State* L, int idx, const char *tname) {
	void *ptr = lua_touserdata(L, idx);
	if (!ptr || !lua_getmetatable(L, idx)) return 0;
	luaL_getmetatable(L, tname);
	if (!lua_rawequal(L, -1, -2)) ptr = 0;
	lua_pop(L, 2);
	return ptr;
}
#endif
