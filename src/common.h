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

#pragma once

#include <lua.h>
#include <lauxlib.h>
#include <mongoc.h>
#include <bson.h>

#define MODNAME "lua-mongo"
#define VERSION "0.2.0-dev"

#define TYPE_BINARY "mongo.Binary"
#define TYPE_BSON "mongo.BSON"
#define TYPE_CLIENT "mongo.Client"
#define TYPE_CURSOR "mongo.Cursor"
#define TYPE_COLLECTION "mongo.Collection"
#define TYPE_DATABASE "mongo.Database"
#define TYPE_DATETIME "mongo.DateTime"
#define TYPE_DOUBLE "mongo.Double"
#define TYPE_INT32 "mongo.Int32"
#define TYPE_INT64 "mongo.Int64"
#define TYPE_JAVASCRIPT "mongo.Javascript"
#define TYPE_MAXKEY "mongo.MaxKey"
#define TYPE_MINKEY "mongo.MinKey"
#define TYPE_NULL "mongo.Null"
#define TYPE_OBJECTID "mongo.ObjectID"
#define TYPE_REGEX "mongo.Regex"
#define TYPE_TIMESTAMP "mongo.Timestamp"

extern char NEW_BINARY, NEW_DATETIME, NEW_JAVASCRIPT, NEW_REGEX, NEW_TIMESTAMP;
extern char GLOBAL_MAXKEY, GLOBAL_MINKEY, GLOBAL_NULL;

int newBinary(lua_State *L);
int newBSON(lua_State *L);
int newClient(lua_State *L);
int newDateTime(lua_State *L);
int newDouble(lua_State *L);
int newInt32(lua_State *L);
int newInt64(lua_State *L);
int newJavascript(lua_State *L);
int newObjectID(lua_State *L);
int newRegex(lua_State *L);
int newTimestamp(lua_State *L);

void pushBSON(lua_State *L, const bson_t *bson, int hidx);
void pushBSONField(lua_State *L, const bson_t *bson, const char *name);
void pushBSONSteal(lua_State *L, bson_t *bson);
void pushCollection(lua_State *L, mongoc_collection_t *collection, int pidx);
void pushCursor(lua_State *L, mongoc_cursor_t *cursor, int pidx);
void pushDatabase(lua_State *L, mongoc_database_t *database, int pidx);
void pushMaxKey(lua_State *L);
void pushMinKey(lua_State *L);
void pushNull(lua_State *L);
void pushObjectID(lua_State *L, const bson_oid_t *oid);

bson_t *checkBSON(lua_State *L, int idx);
bson_t *testBSON(lua_State *L, int idx);
bson_t *castBSON(lua_State *L, int idx);
bson_t *toBSON(lua_State *L, int idx);

bson_oid_t *checkObjectID(lua_State *L, int idx);
bson_oid_t *testObjectID(lua_State *L, int idx);

mongoc_client_t *checkClient(lua_State *L, int idx);
mongoc_collection_t *checkCollection(lua_State *L, int idx);
mongoc_cursor_t *checkCursor(lua_State *L, int idx);
mongoc_database_t *checkDatabase(lua_State *L, int idx);

int toInsertFlags(lua_State *L, int idx);
int toRemoveFlags(lua_State *L, int idx);
int toUpdateFlags(lua_State *L, int idx);

/* Helpers */

bool pushType(lua_State *L, const char *tname, const luaL_Reg *funcs);
void setType(lua_State *L, const char *tname, const luaL_Reg *funcs);
void unsetType(lua_State *L);

void pushHandle(lua_State *L, void *ptr, int pidx);

void packParams(lua_State *L, int n);
int unpackParams(lua_State *L, int idx);

int commandError(lua_State *L, const bson_error_t *error);
int commandStatus(lua_State *L, bool status, const bson_error_t *error);
int commandReply(lua_State *L, bool status, bson_t *reply, const char *field, const bson_error_t *error);
int commandStrVec(lua_State *L, char **strv, const bson_error_t *error);

#define check(L, cond) (void)((cond) || luaL_error(L, "precondition failed: %s at %s:%d", #cond, __FILE__, __LINE__))

#if LUA_VERSION_NUM >= 503 && LUA_MAXINTEGER >= INT64_MAX
#define pushInt64 lua_pushinteger
#define toInt64 lua_tointeger
#define checkInt64 luaL_checkinteger
#else
#define pushInt64 lua_pushnumber
#define toInt64 lua_tonumber
#define checkInt64 luaL_checknumber
#endif

#if LUA_VERSION_NUM >= 503 && LUA_MAXINTEGER >= INT32_MAX
#define pushInt32 lua_pushinteger
#define toInt32 lua_tointeger
#define checkInt32 luaL_checkinteger
#else
#define pushInt32 lua_pushnumber
#define toInt32 lua_tonumber
#define checkInt32 luaL_checknumber
#endif

#if LUA_VERSION_NUM < 502
#define lua_rawlen lua_objlen
#define lua_getuservalue lua_getfenv
#define lua_setuservalue lua_setfenv
#define lua_rawgetp(L, idx, ptr) (lua_pushlightuserdata(L, ptr), lua_rawget(L, idx))
#define lua_rawsetp(L, idx, ptr) (lua_pushlightuserdata(L, ptr), lua_insert(L, -2), lua_rawset(L, idx))
void *luaL_testudata(lua_State* L, int idx, const char *tname);
#endif
