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

static int _getCollection(lua_State *L) {
	mongoc_database_t *database = checkDatabase(L, 1);
	const char *name = luaL_checkstring(L, 2);
	pushCollection(L, mongoc_database_get_collection(database, name));
	return 1;
}

static int _gc(lua_State *L) {
	mongoc_database_destroy(checkDatabase(L, 1));
	unsetType(L);
	return 0;
}

static const luaL_Reg funcs[] = {
	{ "getCollection", _getCollection },
	{ "__gc", _gc },
	{ 0, 0 }
};

void pushDatabase(lua_State *L, mongoc_database_t *database) {
	pushHandle(L, database);
	setType(L, TYPE_DATABASE, funcs);
}

mongoc_database_t *checkDatabase(lua_State *L, int idx) {
	return *(mongoc_database_t **)luaL_checkudata(L, idx, TYPE_DATABASE);
}
