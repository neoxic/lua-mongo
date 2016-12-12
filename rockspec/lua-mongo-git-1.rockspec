package = 'lua-mongo'
version = 'git-1'
source = {
	url = 'git://github.com/neoxic/lua-mongo.git'
}
description = {
	summary = 'MongoDB Driver for Lua',
	detailed = [[
		lua-mongo is a binding of the MongoDB C Driver (http://mongoc.org) for Lua
		and aims to replicate the original API as close as reasonably possible.
	]],
	license = 'MIT',
	homepage = 'https://github.com/neoxic/lua-mongo',
	maintainer = 'Arseny Vakhrushev <arseny.vakhrushev@gmail.com>'
}
dependencies = {
	'lua >= 5.1'
}
build = {
	type = 'cmake',
	variables = {
		CMAKE_INSTALL_PREFIX = '$(PREFIX)',
		LUA_MODULE_PATH = '$(LIBDIR)',
	}
}
