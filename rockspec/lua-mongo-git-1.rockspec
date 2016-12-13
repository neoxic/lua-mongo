package = 'lua-mongo'
version = 'git-1'
source = {
	url = 'git://github.com/neoxic/lua-mongo.git'
}
description = {
	summary = 'MongoDB Driver for Lua',
	detailed = [[
		lua-mongo is a binding of the MongoDB C Driver (http://mongoc.org) for Lua.
	]],
	license = 'MIT',
	homepage = 'https://github.com/neoxic/lua-mongo',
	maintainer = 'Arseny Vakhrushev <arseny.vakhrushev@gmail.com>'
}
dependencies = {
	'lua >= 5.1'
}
external_dependencies = {
	LIBMONGOC = {
		header = 'libmongoc-1.0/mongoc.h'
	},
	LIBBSON = {
		header = 'libbson-1.0/bson.h'
	}
}
build = {
	type = 'builtin',
	modules = {
		mongo = {
			sources = {
				'src/bson.c',
				'src/bsontype.c',
				'src/client.c',
				'src/collection.c',
				'src/cursor.c',
				'src/database.c',
				'src/flags.c',
				'src/main.c',
				'src/objectid.c',
				'src/util.c',
			},
			libraries = { 'mongoc-1.0', 'bson-1.0' },
			incdirs = { '$(LIBMONGOC_INCDIR)/libmongoc-1.0', '$(LIBBSON_INCDIR)/libbson-1.0' },
			libdirs = { '$(LIBMONGOC_LIBDIR)', '$(LIBBSON_LIBDIR)' }
		}
	}
}
