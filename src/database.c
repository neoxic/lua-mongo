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

static int _addUser(lua_State *L) {
	mongoc_database_t *database = checkDatabase(L, 1);
	const char *name = luaL_checkstring(L, 2);
	const char *password = luaL_checkstring(L, 3);
	bson_t *roles = toBSON(L, 4);
	bson_t *extra = toBSON(L, 5);
	bson_error_t error;
	return commandStatus(L, mongoc_database_add_user(database, name, password, roles, extra, &error), &error);
}

static int _drop(lua_State *L) {
	mongoc_database_t *database = checkDatabase(L, 1);
	bson_t *options = toBSON(L, 2);
	bson_error_t error;
	return commandStatus(L, mongoc_database_drop_with_opts(database, options, &error), &error);
}

static int _getCollection(lua_State *L) {
	mongoc_database_t *database = checkDatabase(L, 1);
	const char *name = luaL_checkstring(L, 2);
	pushCollection(L, mongoc_database_get_collection(database, name), 1);
	return 1;
}

static int _getCollectionNames(lua_State *L) {
	mongoc_database_t *database = checkDatabase(L, 1);
	bson_error_t error;
	return commandStrVec(L, mongoc_database_get_collection_names(database, &error), &error);
}

static int _getName(lua_State *L) {
	lua_pushstring(L, mongoc_database_get_name(checkDatabase(L, 1)));
	return 1;
}

static int _hasCollection(lua_State *L) {
	mongoc_database_t *database = checkDatabase(L, 1);
	const char *name = luaL_checkstring(L, 2);
	bson_error_t error;
	return commandStatus(L, mongoc_database_has_collection(database, name, &error), &error);
}

static int _removeAllUsers(lua_State *L) {
	mongoc_database_t *database = checkDatabase(L, 1);
	bson_error_t error;
	return commandStatus(L, mongoc_database_remove_all_users(database, &error), &error);
}

static int _removeUser(lua_State *L) {
	mongoc_database_t *database = checkDatabase(L, 1);
	const char *name = luaL_checkstring(L, 2);
	bson_error_t error;
	return commandStatus(L, mongoc_database_remove_user(database, name, &error), &error);
}

static int _gc(lua_State *L) {
	mongoc_database_destroy(checkDatabase(L, 1));
	unsetType(L);
	return 0;
}

static const luaL_Reg funcs[] = {
	{ "addUser", _addUser },
	{ "drop", _drop },
	{ "getCollection", _getCollection },
	{ "getCollectionNames", _getCollectionNames },
	{ "getName", _getName },
	{ "hasCollection", _hasCollection },
	{ "removeAllUsers", _removeAllUsers },
	{ "removeUser", _removeUser },
	{ "__gc", _gc },
	{ 0, 0 }
};

void pushDatabase(lua_State *L, mongoc_database_t *database, int pidx) {
	pushHandle(L, database, pidx);
	setType(L, TYPE_DATABASE, funcs);
}

mongoc_database_t *checkDatabase(lua_State *L, int idx) {
	return *(mongoc_database_t **)luaL_checkudata(L, idx, TYPE_DATABASE);
}
