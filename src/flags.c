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

typedef struct {
	const char *name;
	int val;
} Flag;

static const Flag insertFlags[] = {
	{ "continueOnError", MONGOC_INSERT_CONTINUE_ON_ERROR },
	{ "noValidate", MONGOC_INSERT_NO_VALIDATE },
	{ 0, 0 }
};

static const Flag queryFlags[] = {
	{ "tailableCursor", MONGOC_QUERY_TAILABLE_CURSOR },
	{ "slaveOk", MONGOC_QUERY_SLAVE_OK },
	{ "oplogReplay", MONGOC_QUERY_OPLOG_REPLAY },
	{ "noCursorTimeout", MONGOC_QUERY_NO_CURSOR_TIMEOUT },
	{ "awaitData", MONGOC_QUERY_AWAIT_DATA },
	{ "exhaust", MONGOC_QUERY_EXHAUST },
	{ "partial", MONGOC_QUERY_PARTIAL },
	{ 0, 0 }
};

static const Flag removeFlags[] = {
	{ "singleRemove", MONGOC_REMOVE_SINGLE_REMOVE },
	{ 0, 0 }
};

static const Flag updateFlags[] = {
	{ "upsert", MONGOC_UPDATE_UPSERT },
	{ "multiUpdate", MONGOC_UPDATE_MULTI_UPDATE },
	{ "noValidate", MONGOC_UPDATE_NO_VALIDATE },
	{ 0, 0 }
};

static int checkFlags(lua_State *L, int idx, const Flag flags[]) {
	const Flag *flag;
	const char *key, *name;
	int val = 0;
	if (lua_isnoneornil(L, idx)) return 0;
	luaL_argcheck(L, lua_istable(L, idx), idx, "nil or table expected");
	for (lua_pushnil(L); lua_next(L, idx); lua_pop(L, 1)) {
		luaL_argcheck(L, lua_type(L, -2) == LUA_TSTRING, idx, "keys must be strings");
		key = lua_tostring(L, -2);
		for (flag = flags; (name = flag->name); ++flag) {
			if (!strcmp(key, name)) {
				if (lua_toboolean(L, -1)) val |= flag->val;
				else val &= ~flag->val;
				break;
			}
		}
		if (!name) luaL_error(L, "invalid flag name '%s'", key);
	}
	return val;
}

int checkInsertFlags(lua_State *L, int idx) {
	return checkFlags(L, idx, insertFlags);
}

int checkQueryFlags(lua_State *L, int idx) {
	return checkFlags(L, idx, queryFlags);
}

int checkRemoveFlags(lua_State *L, int idx) {
	return checkFlags(L, idx, removeFlags);
}

int checkUpdateFlags(lua_State *L, int idx) {
	return checkFlags(L, idx, updateFlags);
}
