package = 'lua-mongo'
version = 'git-1'
source = {
	url = 'git://github.com/neoxic/lua-mongo.git',
}
description = {
	summary = 'MongoDB Driver for Lua',
	detailed = 'lua-mongo is a binding to MongoDB C Driver 1.13 or higher.',
	license = 'MIT',
	homepage = 'https://github.com/neoxic/lua-mongo',
	maintainer = 'Arseny Vakhrushev <arseny.vakhrushev@gmail.com>',
}
dependencies = {
	'lua >= 5.1',
}
external_dependencies = {
	LIBMONGOC = {
		header = 'libmongoc-1.0/mongoc/mongoc.h'
	},
	LIBBSON = {
		header = 'libbson-1.0/bson/bson.h'
	},
}
build = {
	type = 'builtin',
	modules = {
		mongo = {
			sources = {
				'src/bson.c',
				'src/bsontype.c',
				'src/bulkoperation.c',
				'src/client.c',
				'src/collection.c',
				'src/cursor.c',
				'src/database.c',
				'src/flags.c',
				'src/gridfs.c',
				'src/gridfsfile.c',
				'src/gridfsfilelist.c',
				'src/main.c',
				'src/objectid.c',
				'src/readprefs.c',
				'src/util.c',
			},
			libraries = {'mongoc-1.0', 'bson-1.0'},
			incdirs = {'$(LIBMONGOC_INCDIR)/libmongoc-1.0', '$(LIBBSON_INCDIR)/libbson-1.0'},
			libdirs = {'$(LIBMONGOC_LIBDIR)', '$(LIBBSON_LIBDIR)'},
		},
	},
}
