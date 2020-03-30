package = 'lua-mongo'
version = 'git-1'
source = {
	url = 'git://github.com/neoxic/lua-mongo.git',
}
description = {
	summary = 'MongoDB Driver for Lua',
	detailed = [[
		lua-mongo is a binding to MongoDB C Driver 1.16 or higher for Lua:
		- Unified API for MongoDB commands, CRUD operations and GridFS in MongoDB C Driver.
		- Support for data transformation metamethods/handlers when converting to/from BSON documents.
		- Transparent conversion from Lua/JSON to BSON for convenience.
		- Automatic conversion of Lua numbers to/from BSON Int32, Int64 and Double types depending on their
		  capacity without precision loss (when Lua allows it). Manual conversion is also available.
	]],
	license = 'MIT',
	homepage = 'https://github.com/neoxic/lua-mongo',
	maintainer = 'Arseny Vakhrushev <arseny.vakhrushev@me.com>',
}
dependencies = {
	'lua >= 5.1',
}
external_dependencies = {
	LIBMONGOC = {
		header = 'libmongoc-1.0/mongoc/mongoc.h',
		library = 'mongoc-1.0',
	},
	LIBBSON = {
		header = 'libbson-1.0/bson/bson.h',
		library = 'bson-1.0',
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
			incdirs = {'$(LIBMONGOC_INCDIR)/libmongoc-1.0', '$(LIBBSON_INCDIR)/libbson-1.0'},
			libdirs = {'$(LIBMONGOC_LIBDIR)', '$(LIBBSON_LIBDIR)'},
			libraries = {'mongoc-1.0', 'bson-1.0'},
		},
	},
}
