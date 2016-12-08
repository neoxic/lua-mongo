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

#define MODNAME "lua-mongo"
#define VERSION "0.1.0"

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
#define TYPE_MAXKEY "mongo.MaxKey"
#define TYPE_MINKEY "mongo.MinKey"
#define TYPE_NULL "mongo.Null"
#define TYPE_OBJECTID "mongo.ObjectID"
#define TYPE_REGEX "mongo.Regex"
#define TYPE_TIMESTAMP "mongo.Timestamp"

extern char GLOBAL_MAXKEY;
extern char GLOBAL_MINKEY;
extern char GLOBAL_NULL;

int newBinary(lua_State *L);
int newBSON(lua_State *L);
int newClient(lua_State *L);
int newDateTime(lua_State *L);
int newDouble(lua_State *L);
int newInt32(lua_State *L);
int newInt64(lua_State *L);
int newObjectID(lua_State *L);
int newRegex(lua_State *L);
int newTimestamp(lua_State *L);

void pushBSON(lua_State *L, const bson_t *bson, bool eval);
void pushObjectID(lua_State *L, const bson_oid_t *oid);
void pushMaxKey(lua_State *L);
void pushMinKey(lua_State *L);
void pushNull(lua_State *L);

void pushCollection(lua_State *L, mongoc_collection_t *collection);
void pushCursor(lua_State *L, mongoc_cursor_t *cursor);
void pushDatabase(lua_State *L, mongoc_database_t *database);

bson_t *testBSON(lua_State *L, int idx);
bson_t *checkBSON(lua_State *L, int idx);
bson_t *castBSON(lua_State *L, int idx);
bson_t *toBSON(lua_State *L, int idx);

bson_oid_t *testObjectID(lua_State *L, int idx);
bson_oid_t *checkObjectID(lua_State *L, int idx);

mongoc_client_t *checkClient(lua_State *L, int idx);
mongoc_collection_t *checkCollection(lua_State *L, int idx);
mongoc_cursor_t *checkCursor(lua_State *L, int idx);
mongoc_database_t *checkDatabase(lua_State *L, int idx);

/* Utilities */

bool pushType(lua_State *L, const char *tname, const luaL_Reg *funcs);
void setType(lua_State *L, const char *tname, const luaL_Reg *funcs);
void unsetType(lua_State *L);

void pushHandle(lua_State *L, void *ptr);

void packParams(lua_State *L, int n);
int unpackParams(lua_State *L, int idx);

int checkStatus(lua_State *L, bool cond, const char *msg);

#define isInt32(n) ((n) >= INT32_MIN && (n) <= INT32_MAX)

#if LUA_VERSION_NUM >= 503 && LUA_MAXINTEGER >= INT64_MAX
#define pushInt64 lua_pushinteger
#define toInt64 lua_tointeger
#define checkInt64 luaL_checkinteger
#define optInt64 luaL_optinteger
#else
#define pushInt64 lua_pushnumber
#define toInt64 lua_tonumber
#define checkInt64 luaL_checknumber
#define optInt64 luaL_optnumber
#endif

#if LUA_VERSION_NUM >= 503 && LUA_MAXINTEGER >= INT32_MAX
#define pushInt32 lua_pushinteger
#define toInt32 lua_tointeger
#define checkInt32 luaL_checkinteger
#define optInt32 luaL_optinteger
#else
#define pushInt32 lua_pushnumber
#define toInt32 lua_tonumber
#define checkInt32 luaL_checknumber
#define optInt32 luaL_optnumber
#endif

#if LUA_VERSION_NUM < 502
#define lua_rawlen(L, idx) lua_objlen(L, idx)
#define lua_rawgetp(L, idx, ptr) (lua_pushlightuserdata(L, ptr), lua_rawget(L, idx))
#define lua_rawsetp(L, idx, ptr) (lua_pushlightuserdata(L, ptr), lua_insert(L, -2), lua_rawset(L, idx))
void *luaL_testudata(lua_State* L, int idx, const char *tname);
#endif
