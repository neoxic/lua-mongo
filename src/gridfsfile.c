/*
 * Copyright (C) 2016-2017 Arseny Vakhrushev <arseny.vakhrushev@gmail.com>
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


static int fileError(lua_State *L, mongoc_gridfs_file_t *file) {
	bson_error_t error;
	lua_pushnil(L);
	if (!mongoc_gridfs_file_error(file, &error)) return 1; /* No actual error */
	lua_pushstring(L, error.message);
	return 2;
}

static int _getAliases(lua_State *L) {
	pushBSON(L, mongoc_gridfs_file_get_aliases(checkGridFSFile(L, 1)), 0);
	return 1;
}

static int _getChunkSize(lua_State *L) {
	pushInt32(L, mongoc_gridfs_file_get_chunk_size(checkGridFSFile(L, 1)));
	return 1;
}

static int _getContentType(lua_State *L) {
	lua_pushstring(L, mongoc_gridfs_file_get_content_type(checkGridFSFile(L, 1)));
	return 1;
}

static int _getFilename(lua_State *L) {
	lua_pushstring(L, mongoc_gridfs_file_get_filename(checkGridFSFile(L, 1)));
	return 1;
}

static int _getId(lua_State *L) {
	pushBSONValue(L, mongoc_gridfs_file_get_id(checkGridFSFile(L, 1)));
	return 1;
}

static int _getLength(lua_State *L) {
	pushInt64(L, mongoc_gridfs_file_get_length(checkGridFSFile(L, 1)));
	return 1;
}

static int _getMD5(lua_State *L) {
	lua_pushstring(L, mongoc_gridfs_file_get_md5(checkGridFSFile(L, 1)));
	return 1;
}

static int _getMetadata(lua_State *L) {
	pushBSON(L, mongoc_gridfs_file_get_metadata(checkGridFSFile(L, 1)), 0);
	return 1;
}

static int _getUploadDate(lua_State *L) {
	pushInt64(L, mongoc_gridfs_file_get_upload_date(checkGridFSFile(L, 1)));
	return 1;
}

static int _read(lua_State *L) {
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	lua_Integer maxlen = luaL_checkinteger(L, 2);
	lua_Integer buflen = LUAL_BUFFERSIZE;
	luaL_Buffer b;
	mongoc_iovec_t iov;
	ssize_t n;
	luaL_argcheck(L, maxlen > 0, 2, "must be positive");
	luaL_buffinit(L, &b);
	for ( ;; ) {
		if (buflen > maxlen) buflen = maxlen; /* Read no more than needed */
		iov.iov_base = luaL_prepbuffer(&b);
		iov.iov_len = buflen;
		n = mongoc_gridfs_file_readv(file, &iov, 1, buflen, 0);
		if (n <= 0) break;
		luaL_addsize(&b, n);
		maxlen -= n;
	}
	luaL_pushresult(&b);
	if (n == -1) return fileError(L, file);
	if (!n && !lua_rawlen(L, -1)) lua_pushnil(L); /* EOF */
	return 1;
}

static int _remove(lua_State *L) {
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	bson_error_t error;
	return commandStatus(L, mongoc_gridfs_file_remove(file, &error), &error);
}

static int _save(lua_State *L) {
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	if (!mongoc_gridfs_file_save(file)) return fileError(L, file);
	lua_pushboolean(L, 1);
	return 1;
}

static int _seek(lua_State *L) {
	static const char *const whence[] = { "set", "cur", "end", 0 };
	static const int mode[] = { SEEK_SET, SEEK_CUR, SEEK_END };
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	int64_t offset = checkInt64(L, 2);
	int n = luaL_checkoption(L, 3, whence[0], whence);
	lua_pushboolean(L, !mongoc_gridfs_file_seek(file, offset, mode[n]));
	return 1;
}

static int _setAliases(lua_State *L) {
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	const bson_t *aliases = castBSON(L, 2);
	mongoc_gridfs_file_set_aliases(file, aliases);
	return 0;
}

static int _setContentType(lua_State *L) {
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	const char *contentType = luaL_checkstring(L, 2);
	mongoc_gridfs_file_set_content_type(file, contentType);
	return 0;
}

static int _setFilename(lua_State *L) {
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	const char *filename = luaL_checkstring(L, 2);
	mongoc_gridfs_file_set_filename(file, filename);
	return 0;
}

static int _setId(lua_State *L) {
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	bson_value_t value;
	bool status;
	bson_error_t error;
	initBSONValue(L, 2, &value);
	status = mongoc_gridfs_file_set_id(file, &value, &error);
	bson_value_destroy(&value);
	checkStatus(L, status, &error);
	return 0;
}

static int _setMD5(lua_State *L) {
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	const char *md5 = luaL_checkstring(L, 2);
	mongoc_gridfs_file_set_md5(file, md5);
	return 0;
}

static int _setMetadata(lua_State *L) {
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	const bson_t *metadata = castBSON(L, 2);
	mongoc_gridfs_file_set_metadata(file, metadata);
	return 0;
}

static int _tell(lua_State *L) {
	pushInt64(L, mongoc_gridfs_file_tell(checkGridFSFile(L, 1)));
	return 1;
}

static int _write(lua_State *L) {
	mongoc_gridfs_file_t *file = checkGridFSFile(L, 1);
	size_t len;
	const char *str = luaL_checklstring(L, 2, &len);
	mongoc_iovec_t iov = { .iov_len = len, .iov_base = (char *)str };
	lua_Integer n = mongoc_gridfs_file_writev(file, &iov, 1, 0);
	if (n == -1) return fileError(L, file);
	lua_pushinteger(L, n);
	return 1;
}

static int _gc(lua_State *L) {
	mongoc_gridfs_file_destroy(checkGridFSFile(L, 1));
	unsetType(L);
	return 0;
}

static const luaL_Reg funcs[] = {
	{ "getAliases", _getAliases },
	{ "getChunkSize", _getChunkSize },
	{ "getContentType", _getContentType },
	{ "getFilename", _getFilename },
	{ "getId", _getId },
	{ "getLength", _getLength },
	{ "getMD5", _getMD5 },
	{ "getMetadata", _getMetadata },
	{ "getUploadDate", _getUploadDate },
	{ "read", _read },
	{ "remove", _remove },
	{ "save", _save },
	{ "seek", _seek },
	{ "setAliases", _setAliases },
	{ "setContentType", _setContentType },
	{ "setFilename", _setFilename },
	{ "setId", _setId },
	{ "setMD5", _setMD5 },
	{ "setMetadata", _setMetadata },
	{ "tell", _tell },
	{ "write", _write },
	{ "__len", _getLength },
	{ "__gc", _gc },
	{ 0, 0 }
};

void pushGridFSFile(lua_State *L, mongoc_gridfs_file_t *file, int pidx) {
	pushHandle(L, file, -1, pidx);
	setType(L, TYPE_GRIDFSFILE, funcs);
}

mongoc_gridfs_file_t *checkGridFSFile(lua_State *L, int idx) {
	return *(mongoc_gridfs_file_t **)luaL_checkudata(L, idx, TYPE_GRIDFSFILE);
}
