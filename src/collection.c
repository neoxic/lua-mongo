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

static int _aggregate(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *pipeline = castBSON(L, 2);
	bson_t *options = toBSON(L, 3);
	pushCursor(L, mongoc_collection_aggregate(collection, MONGOC_QUERY_NONE, pipeline, options, 0));
	return 1;
}

static int _count(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = toBSON(L, 2);
	bson_t *options = toBSON(L, 3);
	bson_error_t error;
	int64_t n = mongoc_collection_count_with_opts(collection, MONGOC_QUERY_NONE, query, 0, 0, options, 0, &error);
	if (n == -1) return commandError(L, &error);
	pushInt64(L, n);
	return 1;
}

static int _drop(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *options = toBSON(L, 2);
	bson_error_t error;
	return commandStatus(L, mongoc_collection_drop_with_opts(collection, options, &error), &error);
}

static int _find(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = castBSON(L, 2);
	bson_t *options = toBSON(L, 3);
	pushCursor(L, mongoc_collection_find_with_opts(collection, query, options, 0));
	return 1;
}

static int _findAndModify(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = castBSON(L, 2);
	bson_t *options = castBSON(L, 3);
	bool status;
	bson_t reply;
	bson_error_t error;
	mongoc_find_and_modify_opts_t *opts = mongoc_find_and_modify_opts_new();
	mongoc_find_and_modify_opts_append(opts, options);
	status = mongoc_collection_find_and_modify_with_opts(collection, query, opts, &reply, &error);
	mongoc_find_and_modify_opts_destroy(opts);
	return commandReply(L, status, &reply, "value", &error);
}

static int _insert(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *document = castBSON(L, 2);
	int flags = toInsertFlags(L, 3);
	bson_error_t error;
	return commandStatus(L, mongoc_collection_insert(collection, flags, document, 0, &error), &error);
}

static int _remove(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = castBSON(L, 2);
	int flags = toRemoveFlags(L, 3);
	bson_error_t error;
	return commandStatus(L, mongoc_collection_remove(collection, flags, query, 0, &error), &error);
}

static int _save(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *document = castBSON(L, 2);
	bson_error_t error;
	return commandStatus(L, mongoc_collection_save(collection, document, 0, &error), &error);
}

static int _update(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = castBSON(L, 2);
	bson_t *update = castBSON(L, 3);
	int flags = toUpdateFlags(L, 4);
	bson_error_t error;
	return commandStatus(L, mongoc_collection_update(collection, flags, query, update, 0, &error), &error);
}

static int _validate(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *options = toBSON(L, 2);
	bson_t reply;
	bson_error_t error;
	return commandReply(L, mongoc_collection_validate(collection, options, &reply, &error), &reply, 0, &error);
}

static int _gc(lua_State *L) {
	mongoc_collection_destroy(checkCollection(L, 1));
	unsetType(L);
	return 0;
}

static const luaL_Reg funcs[] = {
	{ "aggregate", _aggregate },
	{ "count", _count },
	{ "drop", _drop },
	{ "find", _find },
	{ "findAndModify", _findAndModify },
	{ "insert", _insert },
	{ "remove", _remove },
	{ "save", _save },
	{ "update", _update },
	{ "validate", _validate },
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
