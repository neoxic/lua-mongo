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

static int _next(lua_State *L) {
	mongoc_cursor_t *cursor = checkCursor(L, 1);
	bool iter = lua_toboolean(L, lua_upvalueindex(1)); /* Iterator mode */
	int cb = iter ? lua_upvalueindex(2) : lua_toboolean(L, 2) ? 3 : 0; /* Callback index */
	const bson_t *bson;
	bson_error_t error;
	lua_settop(L, 3); /* Ensure callback index */
	if (mongoc_cursor_next(cursor, &bson)) {
		pushBSON(L, bson, cb);
		return 1;
	}
	lua_pushnil(L);
	if (!mongoc_cursor_error(cursor, &error)) return 1;
	if (iter) return luaL_error(L, "mongoc_cursor_next() failed: %s", error.message);
	lua_pushstring(L, error.message);
	return 2;
}

static int _gc(lua_State *L) {
	mongoc_cursor_destroy(checkCursor(L, 1));
	unsetType(L);
	return 0;
}

static const luaL_Reg funcs[] = {
	{ "next", _next },
	{ "__gc", _gc },
	{ 0, 0 }
};

static int _iterator(lua_State *L) {
	checkCursor(L, 1);
	if (lua_isnoneornil(L, 2)) lua_pushvalue(L, lua_upvalueindex(1)); /* Cached default iterator */
	else {
		lua_pushboolean(L, 1); /* Iterator mode on */
		lua_pushvalue(L, 2); /* Callback object */
		lua_pushcclosure(L, _next, 2); /* Callback iterator */
	}
	lua_pushvalue(L, 1);
	return 2; /* OUT: iterator, cursor */
}

void pushCursor(lua_State *L, mongoc_cursor_t *cursor) {
	pushHandle(L, cursor);
	if (pushType(L, TYPE_CURSOR, funcs)) {
		lua_pushboolean(L, 1); /* Iterator mode on */
		lua_pushcclosure(L, _next, 1); /* Default iterator */
		lua_pushcclosure(L, _iterator, 1); /* Cached as upvalue 1 */
		lua_setfield(L, -2, "iterator");
	}
	lua_setmetatable(L, -2);
}

mongoc_cursor_t *checkCursor(lua_State *L, int idx) {
	return *(mongoc_cursor_t **)luaL_checkudata(L, idx, TYPE_CURSOR);
}
