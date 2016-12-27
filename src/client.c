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
	mongoc_client_t *client = checkClient(L, 1);
	const char *dbname = luaL_checkstring(L, 2);
	const char *cname = luaL_checkstring(L, 3);
	pushCollection(L, mongoc_client_get_collection(client, dbname, cname), 1);
	return 1;
}

static int _getDatabase(lua_State *L) {
	mongoc_client_t *client = checkClient(L, 1);
	const char *name = luaL_checkstring(L, 2);
	pushDatabase(L, mongoc_client_get_database(client, name), 1);
	return 1;
}

static int _getDatabaseNames(lua_State *L) {
	mongoc_client_t *client = checkClient(L, 1);
	bson_error_t error;
	return commandStrVec(L, mongoc_client_get_database_names(client, &error), &error);
}

static int _getDefaultDatabase(lua_State *L) {
	mongoc_database_t *database = mongoc_client_get_default_database(checkClient(L, 1));
	if (!database) luaL_error(L, "default database is not configured");
	pushDatabase(L, database, 1);
	return 1;
}

static int _gc(lua_State *L) {
	mongoc_client_destroy(checkClient(L, 1));
	unsetType(L);
	return 0;
}

static const luaL_Reg funcs[] = {
	{ "getCollection", _getCollection },
	{ "getDatabase", _getDatabase },
	{ "getDatabaseNames", _getDatabaseNames },
	{ "getDefaultDatabase", _getDefaultDatabase },
	{ "__gc", _gc },
	{ 0, 0 }
};

int newClient(lua_State *L) {
	mongoc_client_t *client = mongoc_client_new(luaL_checkstring(L, 1));
	luaL_argcheck(L, client, 1, "invalid format");
	pushHandle(L, client, 0);
	setType(L, TYPE_CLIENT, funcs);
	return 1;
}

mongoc_client_t *checkClient(lua_State *L, int idx) {
	return *(mongoc_client_t **)luaL_checkudata(L, idx, TYPE_CLIENT);
}
