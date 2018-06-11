/*
 * Copyright (C) 2016-2018 Arseny Vakhrushev <arseny.vakhrushev@gmail.com>
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

static int m_aggregate(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *pipeline = castBSON(L, 2);
	bson_t *options = toBSON(L, 3);
	pushCursor(L, mongoc_collection_aggregate(collection, MONGOC_QUERY_NONE, pipeline, options, 0), 1);
	return 1;
}

static int m_count(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = toBSON(L, 2);
	bson_t *options = toBSON(L, 3);
	bson_error_t error;
	int64_t n = mongoc_collection_count_with_opts(collection, MONGOC_QUERY_NONE, query, 0, 0, options, 0, &error);
	if (n == -1) return commandError(L, &error);
	pushInt64(L, n);
	return 1;
}

static int m_createBulkOperation(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *options;
	if (lua_isboolean(L, 2)) { /* TODO remove in version 2 */
		lua_pushfstring(L, "{\"ordered\":%s}", lua_toboolean(L, 2) ? "true" : "false");
		lua_replace(L, 2);
	}
	options = toBSON(L, 2);
	pushBulkOperation(L, mongoc_collection_create_bulk_operation_with_opts(collection, options), 1);
	return 1;
}

static int m_drop(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *options = toBSON(L, 2);
	bson_error_t error;
	return commandStatus(L, mongoc_collection_drop_with_opts(collection, options, &error), &error);
}

static int m_find(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = castBSON(L, 2);
	bson_t *options = toBSON(L, 3);
	pushCursor(L, mongoc_collection_find_with_opts(collection, query, options, 0), 1);
	return 1;
}

static int m_findAndModify(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = castBSON(L, 2);
	bson_t *options = castBSON(L, 3);
	bson_t reply;
	bson_error_t error;
	int nres = 1;
	mongoc_find_and_modify_opts_t *opts = mongoc_find_and_modify_opts_new();
	mongoc_find_and_modify_opts_append(opts, options);
	if (!mongoc_collection_find_and_modify_with_opts(collection, query, opts, &reply, &error)) nres = commandError(L, &error);
	else pushBSONField(L, &reply, "value", false);
	mongoc_find_and_modify_opts_destroy(opts);
	bson_destroy(&reply);
	return nres;
}

static int m_findOne(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = castBSON(L, 2);
	bson_t *options = toBSON(L, 3);
	bson_t opts;
	mongoc_cursor_t *cursor;
	int nres;
	bson_init(&opts);
	if (options) bson_copy_to_excluding_noinit(options, &opts, "limit", "singleBatch", (char *)0);
	BSON_APPEND_INT32(&opts, "limit", 1);
	BSON_APPEND_BOOL(&opts, "singleBatch", true);
	cursor = mongoc_collection_find_with_opts(collection, query, &opts, 0);
	nres = iterateCursor(L, cursor, 0);
	mongoc_cursor_destroy(cursor);
	bson_destroy(&opts);
	return nres;
}

static int m_getName(lua_State *L) {
	lua_pushstring(L, mongoc_collection_get_name(checkCollection(L, 1)));
	return 1;
}

static int m_insert(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *document = castBSON(L, 2);
	int flags = toInsertFlags(L, 3);
	bson_error_t error;
	return commandStatus(L, mongoc_collection_insert(collection, flags, document, 0, &error), &error);
}

static int m_remove(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = castBSON(L, 2);
	int flags = toRemoveFlags(L, 3);
	bson_error_t error;
	return commandStatus(L, mongoc_collection_remove(collection, flags, query, 0, &error), &error);
}

static int m_rename(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	const char *dbname = luaL_checkstring(L, 2);
	const char *collname = luaL_checkstring(L, 3);
	bool force = lua_toboolean(L, 4);
	bson_t *options = toBSON(L, 5);
	bson_error_t error;
	return commandStatus(L, mongoc_collection_rename_with_opts(collection, dbname, collname, force, options, &error), &error);
}

static int m_update(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *query = castBSON(L, 2);
	bson_t *update = castBSON(L, 3);
	int flags = toUpdateFlags(L, 4);
	bson_error_t error;
	return commandStatus(L, mongoc_collection_update(collection, flags, query, update, 0, &error), &error);
}

static int m__gc(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	if (getHandleMode(L, 1)) return 0; /* Reference handle */
	mongoc_collection_destroy(collection);
	unsetType(L);
	return 0;
}

/* TODO remove in version 2 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
static int m_stats(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *options = toBSON(L, 2);
	bson_t reply;
	bson_error_t error;
	return commandReply(L, mongoc_collection_stats(collection, options, &reply, &error), &reply, &error);
}

static int m_validate(lua_State *L) {
	mongoc_collection_t *collection = checkCollection(L, 1);
	bson_t *options = toBSON(L, 2);
	bson_t reply;
	bson_error_t error;
	return commandReply(L, mongoc_collection_validate(collection, options, &reply, &error), &reply, &error);
}
#pragma GCC diagnostic pop

static const luaL_Reg funcs[] = {
	{ "aggregate", m_aggregate },
	{ "count", m_count },
	{ "createBulkOperation", m_createBulkOperation },
	{ "drop", m_drop },
	{ "find", m_find },
	{ "findAndModify", m_findAndModify },
	{ "findOne", m_findOne },
	{ "getName", m_getName },
	{ "insert", m_insert },
	{ "remove", m_remove },
	{ "rename", m_rename },
	{ "update", m_update },
	{ "__gc", m__gc },
	/* TODO remove in version 2 */
	{ "stats", m_stats },
	{ "validate", m_validate },
	{ 0, 0 }
};

void pushCollection(lua_State *L, mongoc_collection_t *collection, bool ref, int pidx) {
	pushHandle(L, collection, ref ? 1 : -1, pidx);
	setType(L, TYPE_COLLECTION, funcs);
}

mongoc_collection_t *checkCollection(lua_State *L, int idx) {
	return *(mongoc_collection_t **)luaL_checkudata(L, idx, TYPE_COLLECTION);
}
