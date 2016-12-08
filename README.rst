MongoDB Driver for Lua
======================

`lua-mongo` provides raw access to the MongoDB C Driver (http://mongoc.org) in Lua.

Please note that `lua-mongo` is in beta which means that its API is likely to be backward-incompatible between versions.


Building and installation with CMake
------------------------------------

To build in the source directory, run::

	cmake .
	make
	make test
	make install

To build against a specific Lua version, set USE_LUA_VERSION::

	cmake -D USE_LUA_VERSION=5.1 .

For LuaJIT::

	cmake -D USE_LUA_VERSION=jit .

To build in a separate directory, replace '.' with a path to the source.


Usage example
-------------

Simple usage::

	local mongo = require 'mongo'
	local client = mongo.Client 'mongodb://127.0.0.1'
	local collection = client:getCollection('db_name', 'collection_name')

	local id = mongo.ObjectID()
	local document = { _id = id, name = 'John Smith', age = 50 }
	collection:save(document)

	-- Iterate in a for-loop
	for v in collection:find('{ "age" : { "$gt" : 25 } }'):iterator() do
		print(v.name)
	end

Data manipulation::

	-- Implicit Lua/JSON to BSON conversion where BSON is required
	collection:save { value = 123 }
	collection:save '{ "value" : "123" }'

	-- Low-level access to BSON if needed
	local cursor = collection:find { _id = id }
	local bson = assert(cursor:next()) -- Fetch first BSON document

	-- Explicit BSON to Lua conversion
	local value = bson()
	print(value._id)

	-- Explicit BSON to JSON conversion
	local json = tostring(bson)
	print(json)
	print(bson) -- 'bson' is converted to string, i.e. JSON

	-- Re-use BSON documents
	collection:update({ _id = 123 }, bson)
	collection:update({ _id = 456 }, bson)


Lua API
-------

The Lua API of `lua-mongo` aims to replicate that of the MongoDB C Driver as close as reasonably possible.
The user should be able to refer to the original API documentation provided the following differences are observed:

	- Constructor names begin with a capital letter::

		/* C */
		mongoc_client_t *client = mongoc_client_new(uri)

		-- Lua
		local client = mongo.Cient(uri)

	- 'under_score_notation' is converted to 'camelCaseNotation' in method names::

		/* C */
		mongoc_database_t *database = mongoc_client_get_database(client, name)

		-- Lua
		local database = client:getDatabase(name)

	- Bitwise flags become tables and are generally moved to the end of the argument list::

		/* C */
		mongoc_update_flags_t flags = MONGOC_UPDATE_UPSERT | MONGOC_UPDATE_MULTI_UPDATE;
		mongoc_collection_update(collection, flags, selector, update, ...)

		-- Lua
		local flags = { upsert = true, multiUpdate = true }
		collection:update(selector, update, flags)
