MongoDB Driver for Lua
======================

_lua-mongo_ is a binding of the MongoDB C Driver (http://mongoc.org) for Lua.

Please note that _lua-mongo_ is in development beta which means that its API is likely to change between versions.


Building and installation with CMake
------------------------------------

To build in the source directory, type:

	cmake .
	make
	make install

To build against a specific Lua version, set the `USE_LUA_VERSION` variable. For example:

	cmake -D USE_LUA_VERSION=5.1 .

or for LuaJIT:

	cmake -D USE_LUA_VERSION=jit .

To build in a separate directory, replace `.` with a path to the source.

To run tests, type:

	make test

Test settings can be configured in `test/test.lua`.


Usage example
-------------

Basic features and CRUD operations:

```Lua
local mongo = require 'mongo'
local client = mongo.Client 'mongodb://127.0.0.1'
local collection = client:getCollection('lua-mongo-test', 'test')

-- Create and save document
local id = mongo.ObjectID()
local document = { _id = id, name = 'John Smith', age = 50 }
collection:save(document)

-- Fetch document
local result = collection:find({ _id = id }):value()
print(result.name)

-- Iterate in a for-loop
for item in collection:find('{ "age" : { "$gt" : 25 } }'):iterator() do
	print(item.name)
end

-- Implicit Lua/JSON to BSON conversion where BSON is required
collection:insert { value = 123 }
collection:insert '{ "value" : "456" }'

-- Use options in queries
print(collection:count({}, { skip = 1, limit = 2 }))

-- Access to BSON if needed
local cursor = collection:find { _id = id }
local bson = cursor:next() -- Fetch document
print(bson) -- BSON is implicitly converted to a string, i.e. JSON

-- Explicit BSON to Lua conversion
local value = bson()
print(value.name)

-- Explicit BSON to JSON conversion
local json = tostring(bson)
print(json)

-- Create/cache/re-use BSON objects
local selector = mongo.BSON { _id = id }
local update = mongo.BSON { age = 60, oldinfo = bson }
local flags = { upsert = true }
collection:update(selector, update, flags) -- Update document
collection:remove(selector) -- Remove document

-- Cleanup
collection:drop()
```

The use of `__tobson` metamethods and BSON handlers gives full control over how
Lua values are represented in BSON objects and vice versa. In particular, this API
facilitates transparent support for Lua classes (tables with metatables) on their
way to and from MongoDB. For example:

```Lua
local mongo = require 'mongo'
local client = mongo.Client 'mongodb://127.0.0.1'
local collection = client:getCollection('lua-mongo-test', 'test')

-- Class metatable
local class = {
	__tostring = function (obj)
		return obj.str
	end,
	__tobson = function (obj)
		return { -- The way 'obj' is stored in BSON
			_id = obj.id,
			bin = mongo.Binary(obj.str),
		}
	end
}

-- BSON evaluation handler (something callable, i.e. a function or a table/userdata with a '__call' metamethod)
local handler = function (doc)
	return setmetatable({ -- The way 'doc' is restored from BSON
		id = doc._id,
		str = doc.bin(),
	}, class)
end

local id = 123
local filter = mongo.BSON { _id = id }

-- Simple object with a "class" metatable
local object = setmetatable({
	id = id,
	str = 'Hello from object!',
}, class)
print(object)

-- Explicit BSON <-> Lua conversion
local bson = mongo.BSON(object)
local object = bson(handler)
print(object)

-- Store object
collection:save(object) -- 'object' is implicitly converted to BSON

-- Restore object
local object = collection:find(filter):value(handler) -- Fetch document and evaluate with handler
print(object)

-- Restore while iterating in a for-loop
for object in collection:find(filter):iterator(handler) do
	print(object)
end

-- Cleanup
collection:drop()
```
