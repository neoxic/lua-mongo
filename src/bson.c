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

#define MAXSTACK 1000 /* Arbitrary stack size limit to check for recursion */
#define isInt32(n) ((n) >= INT32_MIN && (n) <= INT32_MAX)

static int _data(lua_State *L) {
	bson_t *bson = checkBSON(L, 1);
	lua_pushlstring(L, (const char *)bson_get_data(bson), bson->len);
	return 1;
}

static int _value(lua_State *L) {
	pushBSON(L, checkBSON(L, 1), 2);
	return 1;
}

static int _tostring(lua_State *L) {
	size_t len;
	char *str = bson_as_json(checkBSON(L, 1), &len);
	lua_pushlstring(L, str, len);
	bson_free(str);
	return 1;
}

static int _len(lua_State *L) {
	pushInt32(L, checkBSON(L, 1)->len);
	return 1;
}

static int _eq(lua_State *L) {
	lua_pushboolean(L, bson_equal(checkBSON(L, 1), checkBSON(L, 2)));
	return 1;
}

static int _gc(lua_State *L) {
	bson_destroy(checkBSON(L, 1));
	unsetType(L);
	return 0;
}

static const luaL_Reg funcs[] = {
	{ "data", _data },
	{ "value", _value },
	{ "__tostring", _tostring },
	{ "__len", _len },
	{ "__eq", _eq },
	{ "__gc", _gc },
	{ 0, 0 }
};

static bool isInteger(lua_State *L, int idx, lua_Integer *val) {
#if LUA_VERSION_NUM >= 503
	int res;
	lua_Integer n = lua_tointegerx(L, idx, &res);
#else
	lua_Number d = lua_tonumber(L, idx);
	lua_Integer n = (lua_Integer)d;
	bool res = d == (lua_Number)n;
#endif
	if (!res) return false;
	*val = n;
	return true;
}

static bool isArray(lua_State *L, int idx) {
	lua_Integer i;
	lua_pushnil(L);
	if (!lua_next(L, idx)) return true; /* Empty table */
	bool res = lua_type(L, -2) == LUA_TNUMBER && isInteger(L, -2, &i) && i == 1;
	lua_pop(L, 2);
	return res;
}

static bool error(lua_State *L, int *nerr, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	lua_pushvfstring(L, fmt, ap);
	va_end(ap);
	lua_insert(L, -(++(*nerr)));
	return false;
}

static bool appendBSONType(lua_State *L, bson_type_t type, int idx, int *nerr, bson_t *bson, const char *key, size_t klen) {
	int top = lua_gettop(L);
	unpackParams(L, idx);
	switch (type) {
		case BSON_TYPE_INT32:
			bson_append_int32(bson, key, klen, toInt32(L, top + 1));
			break;
		case BSON_TYPE_INT64:
			bson_append_int64(bson, key, klen, toInt64(L, top + 1));
			break;
		case BSON_TYPE_DOUBLE:
			bson_append_double(bson, key, klen, lua_tonumber(L, top + 1));
			break;
		case BSON_TYPE_BINARY: {
			size_t len;
			const char *str = lua_tolstring(L, top + 1, &len);
			if (!str) goto error;
			bson_append_binary(bson, key, klen, lua_tointeger(L, top + 2), (const uint8_t *)str, len);
			break;
		}
		case BSON_TYPE_DATE_TIME:
			bson_append_date_time(bson, key, klen, toInt64(L, top + 1));
			break;
		case BSON_TYPE_REGEX:
			bson_append_regex(bson, key, klen, lua_tostring(L, top + 1), lua_tostring(L, top + 2));
			break;
		case BSON_TYPE_TIMESTAMP:
			bson_append_timestamp(bson, key, klen, toInt32(L, top + 1), toInt32(L, top + 2));
			break;
		case BSON_TYPE_CODE: {
			const char *code = lua_tostring(L, top + 1);
			if (!code) goto error;
			bson_append_code(bson, key, klen, code);
			break;
		}
		case BSON_TYPE_CODEWSCOPE: {
			const char *code = lua_tostring(L, top + 1);
			bson_t *scope = testBSON(L, top + 2);
			if (!code || !scope) goto error;
			bson_append_code_with_scope(bson, key, klen, code, scope);
			break;
		}
		case BSON_TYPE_MAXKEY:
			bson_append_maxkey(bson, key, klen);
			break;
		case BSON_TYPE_MINKEY:
			bson_append_minkey(bson, key, klen);
			break;
		case BSON_TYPE_NULL:
			bson_append_null(bson, key, klen);
			break;
		default: error:
			return error(L, nerr, "invalid parameters for BSON type %d", type);
	}
	lua_settop(L, top);
	return true;
}

static bool appendTable(lua_State *L, int idx, int ridx, int *nerr, bson_t *bson, bool array);

static bool appendValue(lua_State *L, int idx, int ridx, int *nerr, bson_t *bson, const char *key, size_t klen) {
	if (luaL_getmetafield(L, idx, "__tobson")) { /* Transform value with '__tobson' metamethod */
		lua_pushvalue(L, idx);
		if (lua_pcall(L, 1, 1, 0)) return error(L, nerr, "%s", lua_tostring(L, -1));
		lua_replace(L, idx);
	}
	switch (lua_type(L, idx)) {
		case LUA_TNIL:
			bson_append_null(bson, key, klen);
			break;
		case LUA_TBOOLEAN:
			bson_append_bool(bson, key, klen, lua_toboolean(L, idx));
			break;
		case LUA_TNUMBER: {
			lua_Integer val;
			if (isInteger(L, idx, &val)) {
				if (isInt32(val)) bson_append_int32(bson, key, klen, val);
				else bson_append_int64(bson, key, klen, val);
				break;
			}
			bson_append_double(bson, key, klen, lua_tonumber(L, idx));
			break;
		}
		case LUA_TSTRING: {
			size_t len;
			const char *str = lua_tolstring(L, idx, &len);
			bson_append_utf8(bson, key, klen, str, len);
			break;
		}
		case LUA_TTABLE: {
			bool array;
			bson_t val;
			if (luaL_getmetafield(L, idx, "__bsontype")) {
				if (!appendBSONType(L, lua_tointeger(L, -1), idx, nerr, bson, key, klen)) return false;
				lua_pop(L, 1);
				return true;
			}
			lua_pushvalue(L, idx);
			lua_rawget(L, ridx);
			if (lua_toboolean(L, -1)) return error(L, nerr, "circular reference detected");
			lua_pop(L, 1);
			lua_pushvalue(L, idx);
			lua_pushboolean(L, 1);
			lua_rawset(L, ridx);
			if ((array = isArray(L, idx))) {
				bson_append_array_begin(bson, key, klen, &val);
				if (!appendTable(L, idx, ridx, nerr, &val, array)) return false;
				bson_append_array_end(bson, &val);
			} else {
				bson_append_document_begin(bson, key, klen, &val);
				if (!appendTable(L, idx, ridx, nerr, &val, array)) return false;
				bson_append_document_end(bson, &val);
			}
			lua_pushvalue(L, idx);
			lua_pushnil(L);
			lua_rawset(L, ridx);
			break;
		}
		case LUA_TUSERDATA: {
			bson_t *val;
			bson_oid_t *oid;
			if ((val = testBSON(L, idx))) {
				bson_append_document(bson, key, klen, val);
				break;
			}
			if ((oid = testObjectID(L, idx))) {
				bson_append_oid(bson, key, klen, oid);
				break;
			}
			/* Fall through */
		}
		default:
			return error(L, nerr, "%s unexpected", luaL_typename(L, idx));
	}
	return true;
}

static bool appendTable(lua_State *L, int idx, int ridx, int *nerr, bson_t *bson, bool array) {
	char buf[64];
	const char *key;
	size_t klen;
	lua_Integer i, n = 0;
	int top = lua_gettop(L);
	if (top >= MAXSTACK) return error(L, nerr, "recursion detected");
	lua_checkstack(L, LUA_MINSTACK);
	for (lua_pushnil(L); lua_next(L, idx); lua_pop(L, 1)) {
		switch (lua_type(L, top + 1)) {
			case LUA_TNUMBER:
				if (!array) goto error;
				if (!isInteger(L, top + 1, &i) || --i != n++ || !isInt32(i)) return error(L, nerr, "array keys must be sequential");
				klen = bson_uint32_to_string(i, &key, buf, sizeof buf);
				break;
			case LUA_TSTRING:
				if (array) goto error;
				key = lua_tolstring(L, top + 1, &klen);
				break;
			default: error:
				return error(L, nerr, "%s key unexpected in %s", luaL_typename(L, top + 1), array ? "array" : "document");
		}
		if (!appendValue(L, top + 2, ridx, nerr, bson, key, klen)) return error(L, nerr, "[\"%s\"] => ", key);
	}
	return true;
}

static void pushTable(lua_State *L, bson_iter_t *iter, int hidx, bool array);

static void pushValue(lua_State *L, bson_iter_t *iter, int hidx) {
	bson_type_t type = bson_iter_type(iter);
	switch (type) {
		case BSON_TYPE_BOOL:
			lua_pushboolean(L, bson_iter_bool(iter));
			break;
		case BSON_TYPE_INT32:
			pushInt32(L, bson_iter_int32(iter));
			break;
		case BSON_TYPE_INT64:
			pushInt64(L, bson_iter_int64(iter));
			break;
		case BSON_TYPE_DOUBLE:
			lua_pushnumber(L, bson_iter_double(iter));
			break;
		case BSON_TYPE_UTF8: {
			uint32_t len;
			const char *str = bson_iter_utf8(iter, &len);
			lua_pushlstring(L, str, len);
			break;
		}
		case BSON_TYPE_DOCUMENT:
		case BSON_TYPE_ARRAY: {
			bson_iter_t val;
			check(L, bson_iter_recurse(iter, &val));
			pushTable(L, &val, hidx, type == BSON_TYPE_ARRAY);
			break;
		}
		case BSON_TYPE_OID:
			pushObjectID(L, bson_iter_oid(iter));
			break;
		case BSON_TYPE_BINARY: {
			bson_subtype_t subtype;
			uint32_t len;
			const uint8_t *buf;
			bson_iter_binary(iter, &subtype, &len, &buf);
			lua_rawgetp(L, LUA_REGISTRYINDEX, &NEW_BINARY);
			lua_pushlstring(L, (const char *)buf, len);
			lua_pushinteger(L, subtype);
			lua_call(L, 2, 1);
			break;
		}
		case BSON_TYPE_DATE_TIME:
			lua_rawgetp(L, LUA_REGISTRYINDEX, &NEW_DATETIME);
			pushInt64(L, bson_iter_date_time(iter));
			lua_call(L, 1, 1);
			break;
		case BSON_TYPE_CODE: {
			uint32_t len;
			const char *code = bson_iter_code(iter, &len);
			lua_rawgetp(L, LUA_REGISTRYINDEX, &NEW_JAVASCRIPT);
			lua_pushlstring(L, code, len);
			lua_call(L, 1, 1);
			break;
		}
		case BSON_TYPE_CODEWSCOPE: {
			bson_t sval;
			uint32_t len, slen;
			const uint8_t *sbuf;
			const char *code = bson_iter_codewscope(iter, &len, &slen, &sbuf);
			check(L, bson_init_static(&sval, sbuf, slen));
			lua_rawgetp(L, LUA_REGISTRYINDEX, &NEW_JAVASCRIPT);
			lua_pushlstring(L, code, len);
			pushBSON(L, &sval, 0);
			lua_call(L, 2, 1);
			break;
		}
		case BSON_TYPE_REGEX: {
			const char *options;
			lua_rawgetp(L, LUA_REGISTRYINDEX, &NEW_REGEX);
			lua_pushstring(L, bson_iter_regex(iter, &options));
			lua_pushstring(L, options);
			lua_call(L, 2, 1);
			break;
		}
		case BSON_TYPE_TIMESTAMP: {
			uint32_t t, i;
			bson_iter_timestamp(iter, &t, &i);
			lua_rawgetp(L, LUA_REGISTRYINDEX, &NEW_TIMESTAMP);
			pushInt32(L, t);
			pushInt32(L, i);
			lua_call(L, 2, 1);
			break;
		}
		case BSON_TYPE_MAXKEY:
			lua_rawgetp(L, LUA_REGISTRYINDEX, &GLOBAL_MAXKEY);
			break;
		case BSON_TYPE_MINKEY:
			lua_rawgetp(L, LUA_REGISTRYINDEX, &GLOBAL_MINKEY);
			break;
		case BSON_TYPE_NULL:
			lua_rawgetp(L, LUA_REGISTRYINDEX, &GLOBAL_NULL);
			break;
		/* Deprecated types */
		case BSON_TYPE_UNDEFINED:
		case BSON_TYPE_DBPOINTER:
		case BSON_TYPE_SYMBOL:
			lua_pushnil(L);
			break;
		default:
			luaL_error(L, "invalid BSON type %d at key '%s'", type, bson_iter_key(iter));
			break;
	}
}

static void pushTable(lua_State *L, bson_iter_t *iter, int hidx, bool array) {
	lua_Integer i = 0;
	lua_newtable(L);
	luaL_checkstack(L, LUA_MINSTACK, "too many nested documents");
	while (bson_iter_next(iter)) {
		if (array) lua_pushinteger(L, ++i);
		else lua_pushstring(L, bson_iter_key(iter));
		pushValue(L, iter, hidx);
		lua_rawset(L, -3);
	}
	if (lua_isnoneornil(L, hidx)) return; /* No handler */
	lua_pushvalue(L, hidx);
	lua_insert(L, -2);
	lua_call(L, 1, 1); /* Transform value */
}

static bool bsonIsArray(const bson_t *bson) {
	bson_iter_t iter;
	return bson_iter_init(&iter, bson) && bson_iter_next(&iter) && !strcmp(bson_iter_key(&iter), "0");
}

int newBSON(lua_State *L) {
	castBSON(L, 1);
	return 1;
}

void pushBSON(lua_State *L, const bson_t *bson, int hidx) {
	if (hidx) { /* Evaluate */
		bson_iter_t iter;
		check(L, bson_iter_init(&iter, bson));
		lua_pushvalue(L, hidx); /* Ensure handler index is valid */
		pushTable(L, &iter, lua_gettop(L), bsonIsArray(bson));
		lua_replace(L, -2);
	} else { /* Copy-by-value */
		bson_copy_to(bson, lua_newuserdata(L, sizeof *bson));
		setType(L, TYPE_BSON, funcs);
	}
}

void pushBSON_steal(lua_State *L, bson_t *bson) {
	bson_steal(lua_newuserdata(L, sizeof *bson), bson);
	setType(L, TYPE_BSON, funcs);
}

bson_t *checkBSON(lua_State *L, int idx) {
	return luaL_checkudata(L, idx, TYPE_BSON);
}

bson_t *testBSON(lua_State *L, int idx) {
	return luaL_testudata(L, idx, TYPE_BSON);
}

bson_t *castBSON(lua_State *L, int idx) {
	bson_t *bson;
	size_t len;
	const char *str = lua_tolstring(L, idx, &len);
	if (str) { /* From string */
		bson_error_t error;
		bson = lua_newuserdata(L, sizeof *bson);
		if (!bson_init_from_json(bson, str, len, &error)) {
			luaL_argcheck(L, len, idx, "JSON expected"); /* FIXME Follow up with bug report: https://jira.mongodb.org/browse/CDRIVER-1936 */
			luaL_argerror(L, idx, error.message);
		}
	} else { /* From value */
		int nerr = 0;
		if (luaL_callmeta(L, idx, "__tobson")) lua_replace(L, idx); /* Transform value with '__tobson' metamethod */
		if ((bson = testBSON(L, idx))) return bson; /* Nothing to do */
		luaL_argcheck(L, lua_istable(L, idx), idx, "string, table or " TYPE_BSON " expected");
		bson = lua_newuserdata(L, sizeof *bson);
		bson_init(bson);
		lua_newtable(L);
		if (!appendTable(L, idx, lua_gettop(L), &nerr, bson, isArray(L, idx))) {
			bson_destroy(bson);
			lua_concat(L, nerr);
			luaL_argerror(L, idx, lua_tostring(L, -1));
		}
		lua_pop(L, 1);
	}
	setType(L, TYPE_BSON, funcs);
	lua_replace(L, idx);
	return bson;
}

bson_t *toBSON(lua_State *L, int idx) {
	return lua_isnoneornil(L, idx) ? 0 : castBSON(L, idx);
}
