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

static int _getData(lua_State *L) {
	bson_t *bson = checkBSON(L, 1);
	lua_pushlstring(L, (const char *)bson_get_data(bson), bson->len);
	return 1;
}

static int _tostring(lua_State *L) {
	size_t len;
	char *str = bson_as_json(checkBSON(L, 1), &len);
	lua_pushlstring(L, str, len);
	bson_free(str);
	return 1;
}

static int _call(lua_State *L) {
	pushBSON(L, checkBSON(L, 1), true);
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
	{ "_getData", _getData },
	{ "__tostring", _tostring },
	{ "__call", _call },
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
	lua_Integer kval, n = 0;
	for (lua_pushnil(L); lua_next(L, idx); lua_pop(L, 1)) {
		if (lua_type(L, -2) != LUA_TNUMBER || !isInteger(L, -2, &kval) || kval != ++n) {
			lua_pop(L, 2);
			return false;
		}
	}
	return true;
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
			const char *str;
			str = lua_tolstring(L, top + 1, &len);
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
		case BSON_TYPE_MAXKEY:
			bson_append_maxkey(bson, key, klen);
			break;
		case BSON_TYPE_MINKEY:
			bson_append_minkey(bson, key, klen);
			break;
		case BSON_TYPE_NULL:
			bson_append_null(bson, key, klen);
			break;
		default:
			goto error;
	}
	lua_settop(L, top);
	return true;
error:
	lua_settop(L, top);
	return error(L, nerr, "invalid BSON type %d", type);
}

static bool appendTable(lua_State *L, int idx, int ridx, int *nerr, bson_t *bson);

static bool appendValue(lua_State *L, int idx, int ridx, int *nerr, bson_t *bson, const char *key, size_t klen) {
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
			bool ref;
			bson_t child;
			if (luaL_getmetafield(L, idx, "__bsontype")) {
				if (!appendBSONType(L, lua_tointeger(L, -1), idx, nerr, bson, key, klen)) return false;
				lua_pop(L, 1);
				return true;
			}
			lua_pushvalue(L, idx);
			lua_rawget(L, ridx);
			ref = lua_toboolean(L, -1);
			lua_pop(L, 1);
			if (ref) return error(L, nerr, "circular reference detected");
			lua_pushvalue(L, idx);
			lua_pushboolean(L, 1);
			lua_rawset(L, ridx);
			if (isArray(L, idx)) {
				bson_append_array_begin(bson, key, klen, &child);
				if (!appendTable(L, idx, ridx, nerr, &child)) return false;
				bson_append_array_end(bson, &child);
			} else {
				bson_append_document_begin(bson, key, klen, &child);
				if (!appendTable(L, idx, ridx, nerr, &child)) return false;
				bson_append_document_end(bson, &child);
			}
			lua_pushvalue(L, idx);
			lua_pushnil(L);
			lua_rawset(L, ridx);
			break;
		}
		case LUA_TUSERDATA: {
			bson_t *child;
			bson_oid_t *oid;
			if ((child = testBSON(L, idx))) {
				bson_append_document(bson, key, klen, child);
				break;
			}
			if ((oid = testObjectID(L, idx))) {
				bson_append_oid(bson, key, klen, oid);
				break;
			}
			return error(L, nerr, "invalid object");
		}
		default:
			return error(L, nerr, "invalid value type '%s'", luaL_typename(L, idx));
	}
	return true;
}

static bool appendTable(lua_State *L, int idx, int ridx, int *nerr, bson_t *bson) {
	char buf[64];
	const char *key;
	size_t klen;
	lua_Integer kval;
	int kidx = lua_gettop(L) + 1;
	lua_checkstack(L, LUA_MINSTACK);
	for (lua_pushnil(L); lua_next(L, idx); lua_pop(L, 1)) {
		switch (lua_type(L, kidx)) {
			case LUA_TNUMBER:
				if (isInteger(L, kidx, &kval) && isInt32(kval)) {
					klen = bson_uint32_to_string(kval, &key, buf, sizeof buf);
					break;
				}
				key = buf;
#if LUA_VERSION_NUM >= 503
				klen = lua_number2str(buf, sizeof buf, lua_tonumber(L, idx));
#else
				klen = lua_number2str(buf, lua_tonumber(L, idx));
#endif
				break;
			case LUA_TSTRING:
				key = lua_tolstring(L, kidx, &klen);
				break;
			default:
				return error(L, nerr, "invalid key type '%s'", luaL_typename(L, idx));
		}
		if (!appendValue(L, kidx + 1, ridx, nerr, bson, key, klen)) return error(L, nerr, "[\"%s\"] => ", key);
	}
	return true;
}

static void pushTable(lua_State *L, bson_iter_t *iter, bool array);

static void pushValue(lua_State *L, const bson_iter_t *iter) {
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
			bson_iter_t child;
			if (!bson_iter_recurse(iter, &child)) luaL_error(L, "bson_iter_recurse() failed");
			pushTable(L, &child, type == BSON_TYPE_ARRAY);
			break;
		}
		case BSON_TYPE_OID:
			pushObjectID(L, bson_iter_oid(iter));
			break;
		case BSON_TYPE_BINARY: {
			bson_subtype_t subtype;
			uint32_t len;
			const uint8_t *data;
			bson_iter_binary(iter, &subtype, &len, &data);
			lua_rawgetp(L, LUA_REGISTRYINDEX, &NEW_BINARY);
			lua_pushlstring(L, (const char *)data, len);
			lua_pushinteger(L, subtype);
			lua_call(L, 2, 1);
			break;
		}
		case BSON_TYPE_DATE_TIME:
			lua_rawgetp(L, LUA_REGISTRYINDEX, &NEW_DATETIME);
			pushInt64(L, bson_iter_date_time(iter));
			lua_call(L, 1, 1);
			break;
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
		case BSON_TYPE_UNDEFINED:
		case BSON_TYPE_DBPOINTER:
		case BSON_TYPE_CODE:
		case BSON_TYPE_SYMBOL:
		case BSON_TYPE_CODEWSCOPE:
			lua_pushnil(L);
			break;
		default:
			luaL_error(L, "invalid BSON type %d at key '%s'", type, bson_iter_key(iter));
			break;
	}
}

static void pushTable(lua_State *L, bson_iter_t *iter, bool array) {
	lua_Integer n = 0;
	lua_newtable(L);
	lua_checkstack(L, LUA_MINSTACK);
	while (bson_iter_next(iter)) {
		if (array) lua_pushinteger(L, ++n);
		else lua_pushstring(L, bson_iter_key(iter));
		pushValue(L, iter);
		lua_rawset(L, -3);
	}
}

static bson_t *fromString(lua_State *L, int idx) {
	bson_t *bson = lua_newuserdata(L, sizeof *bson);
	size_t len;
	const char *str = lua_tolstring(L, idx, &len);
	bson_error_t error;
	if (!bson_init_from_json(bson, str, len, &error)) {
		luaL_argcheck(L, len, 1, "invalid JSON"); /* FIXME Follow up with bug report: https://jira.mongodb.org/browse/CDRIVER-1936 */
		luaL_argerror(L, idx, error.message);
		return 0;
	}
	setType(L, TYPE_BSON, funcs);
	lua_replace(L, idx);
	return bson;
}

static bson_t *fromTable(lua_State *L, int idx) {
	bson_t *bson = lua_newuserdata(L, sizeof *bson);
	int nerr = 0;
	bson_init(bson);
	lua_newtable(L);
	if (!appendTable(L, idx, lua_gettop(L), &nerr, bson)) {
		bson_destroy(bson); /* 'bson' should be discarded */
		lua_concat(L, nerr);
		luaL_argerror(L, idx, lua_tostring(L, -1));
		return 0;
	}
	lua_pop(L, 1);
	setType(L, TYPE_BSON, funcs);
	lua_replace(L, idx);
	return bson;
}

int newBSON(lua_State *L) {
	switch (lua_type(L, 1)) {
		case LUA_TSTRING:
			fromString(L, 1);
			return 1;
		case LUA_TTABLE:
			fromTable(L, 1);
			return 1;
		default:
			return luaL_argerror(L, 1, "string or table expected");
	}
}

void pushBSON(lua_State *L, const bson_t *bson, bool eval) {
	if (eval) { /* Lua value */
		bson_iter_t iter;
		if (!bson_iter_init(&iter, bson)) luaL_error(L, "bson_iter_init() failed");
		pushTable(L, &iter, false);
	} else { /* Raw copy */
		bson_copy_to(bson, lua_newuserdata(L, sizeof *bson));
		setType(L, TYPE_BSON, funcs);
	}
}

bson_t *testBSON(lua_State *L, int idx) {
	return luaL_testudata(L, idx, TYPE_BSON);
}

bson_t *checkBSON(lua_State *L, int idx) {
	return luaL_checkudata(L, idx, TYPE_BSON);
}

bson_t *castBSON(lua_State *L, int idx) {
	switch (lua_type(L, idx)) {
		case LUA_TSTRING:
			return fromString(L, idx);
		case LUA_TTABLE:
			return fromTable(L, idx);
		case LUA_TUSERDATA:
			return checkBSON(L, idx);
		default:
			luaL_argerror(L, idx, "string, table or " TYPE_BSON " expected");
			return 0;
	}
}

bson_t *toBSON(lua_State *L, int idx) {
	return !lua_isnoneornil(L, idx) ? castBSON(L, idx) : 0;
}
