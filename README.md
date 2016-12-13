# MongoDB Driver for Lua
------------------------

_lua-mongo_ is a binding of the MongoDB C Driver (http://mongoc.org) for Lua.

Please note that _lua-mongo_ is in beta which means that its API is likely to change between versions.


## Building and installation with CMake

To build in the source directory, run:

	cmake .
	make
	make test
	make install

To build against a specific Lua version, set the `USE_LUA_VERSION` variable:

	cmake -D USE_LUA_VERSION=5.1 .

For LuaJIT:

	cmake -D USE_LUA_VERSION=jit .

To build in a separate directory, replace `.` with a path to the source.


## Usage example

```Lua
local mongo = require 'mongo'
local client = mongo.Client 'mongodb://127.0.0.1'
local collection = client:getCollection('lua-mongo-test', 'test')

local id = mongo.ObjectID()
local document = { _id = id, name = 'John Smith', age = 50 }
collection:save(document)

-- Iterate in a for-loop
for item in collection:find('{ "age" : { "$gt" : 25 } }'):iterator() do
	print(item.name)
end

-- Implicit Lua/JSON to BSON conversion where BSON is required
collection:insert { value = 123 }
collection:insert '{ "value" : "456" }'

-- Use options in queries
print(collection:count({}, { skip = 1, limit = 2 }))

-- Low-level access to BSON if needed
local cursor = collection:find { _id = id }
local bson = assert(cursor:next()) -- Fetch first BSON document
print(bson) -- BSON is implicitly converted to a string, i.e. JSON

-- Explicit BSON to Lua conversion
local item = bson()
print(item._id)

-- Explicit BSON to JSON conversion
local json = tostring(bson)
print(json)

-- Re-use BSON documents
local flags = { upsert = true }
collection:update({ _id = 123 }, bson, flags)
collection:update({ _id = 456 }, bson, flags)

-- Cleanup
collection:drop()
```
