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

static int _count(lua_State *L) {
	bson_error_t error;
	int64_t n = mongoc_collection_count_with_opts(
		checkCollection(L, 1), /* collection */
		MONGOC_QUERY_NONE, /* flags */
		toBSON(L, 2), /* query */
		optInt64(L, 3, 0), /* skip */
		optInt64(L, 4, 0), /* limit */
		toBSON(L, 5), /* opts */
		0, /* read_prefs */
		&error
	);
	if (n != -1) {
		pushInt64(L, n);
		return 1;
	}
	lua_pushnil(L);
	lua_pushstring(L, error.message);
	return 2;
}

static int _drop(lua_State *L) {
	bson_error_t error;
	return checkStatus(L, mongoc_collection_drop_with_opts(
		checkCollection(L, 1), /* collection */
		toBSON(L, 2), /* opts */
		&error
	), error.message);
}

static int _find(lua_State *L) {
	pushCursor(L, mongoc_collection_find_with_opts(
		checkCollection(L, 1), /* collection */
		castBSON(L, 2), /* filter */
		toBSON(L, 3), /* opts */
		0 /* read_prefs */
	));
	return 1;
}

static int _remove(lua_State *L) {
	bson_error_t error;
	return checkStatus(L, mongoc_collection_remove(
		checkCollection(L, 1), /* collection */
		MONGOC_REMOVE_NONE, /* flags */
		castBSON(L, 2), /* selector */
		0, /* write_concern */
		&error
	), error.message);
}

static int _save(lua_State *L) {
	bson_error_t error;
	return checkStatus(L, mongoc_collection_save(
		checkCollection(L, 1), /* collection */
		castBSON(L, 2), /* document */
		0, /* write_concern */
		&error
	), error.message);
}

static int _update(lua_State *L) {
	bson_error_t error;
	return checkStatus(L, mongoc_collection_update(
		checkCollection(L, 1), /* collection */
		MONGOC_UPDATE_NONE, /* flags */
		castBSON(L, 2), /* selector */
		castBSON(L, 3), /* update */
		0, /* write_concern */
		&error
	), error.message);
}

static int _gc(lua_State *L) {
	mongoc_collection_destroy(checkCollection(L, 1));
	unsetType(L);
	return 0;
}

static const struct luaL_Reg funcs[] = {
	{ "count", _count },
	{ "drop", _drop },
	{ "find", _find },
	{ "remove", _remove },
	{ "save", _save },
	{ "update", _update },
	{ "__gc", _gc },
	{ 0, 0 }
};

void pushCollection(lua_State *L, mongoc_collection_t *collection) {
	pushHandle(L, collection);
	setType(L, TYPE_COLLECTION, funcs);
}

mongoc_collection_t *checkCollection(lua_State *L, int idx) {
	return *(mongoc_collection_t **)luaL_checkudata(L, idx, TYPE_COLLECTION);
}
