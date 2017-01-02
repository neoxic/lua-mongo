MongoDB Driver for Lua
======================

[lua-mongo] is a binding to the [MongoDB C Driver] for Lua.

* Unified API for MongoDB commands and CRUD operations in the [MongoDB C Driver].

* Support for custom data transformation handlers when converting to/from BSON documents.

* Transparent conversion from Lua/JSON to BSON for convenience.

* Automatic conversion of Lua numbers to/from BSON Int32, Int64 and Double types depending on their
  capacity without precision loss (when Lua allows it). Explicit casts are also available.


Installing with LuaRocks
------------------------

To install the latest release, run:

	luarocks install lua-mongo

To install the current development version, run:

	luarocks install --server=http://luarocks.org/dev lua-mongo


Building and installing with CMake
----------------------------------

To build in the source directory, run:

	cmake .
	make
	make install

To build against a specific Lua version, set the `USE_LUA_VERSION` variable. For example:

	cmake -D USE_LUA_VERSION=5.1 .

or for LuaJIT:

	cmake -D USE_LUA_VERSION=jit .

To build in a separate directory, replace `.` with a path to the source.

To check your build, run:

	make test

A local MongoDB server at `mongodb://127.0.0.1` will be used for testing by default. Test settings
can be configured in `test/test.lua`.


Getting started
---------------

Preparing the playground:

```Lua
local mongo = require 'mongo'
local client = mongo.Client 'mongodb://127.0.0.1'
local collection = client:getCollection('lua-mongo-test', 'test')
collection:drop()

-- Common variables
local id = mongo.ObjectID()
local query1 = mongo.BSON '{ "age" : { "$gt" : 25 } }'
local query2 = mongo.BSON { _id = id }
```


Basic features and MongoDB CRUD operations:

```Lua
-- Store document
collection:save { _id = id, name = 'John Smith', age = 50 }

-- Fetch document
local document = collection:find(query1):value()
print(document.name)

-- Iterate in a for-loop
for document in collection:find(query1):iterator() do
	print(document.name)
end

-- Implicit Lua/JSON to BSON conversion where BSON is required
collection:insert { integer = 123 }
collection:insert '{ "string" : "abc" }'

-- Use options in queries
print(collection:count({}, { skip = 1, limit = 2 }))

-- Access to BSON where needed
local cursor = collection:find(query1)
local bson = cursor:next() -- Fetch BSON document
print(bson) -- BSON is implicitly converted to JSON

-- Explicit BSON to Lua conversion
local value = bson:value()
print(value.name)

-- Transparently include BSON documents in other documents
collection:update(query2, { age = 60, backup = bson }, { upsert = true }) -- Update document
collection:remove(query2) -- Remove document
```


Bulk write operations can be used to execute multiple insert, update, replace and remove operations
together. Executing write operations in batches reduces the number of network round trips increasing
write throughput:

```Lua
local bulk = collection:createBulkOperation()

-- Multiple insertions
bulk:insert { a = 1 }
bulk:insert { b = 2 }
bulk:insert { c = 3 }

-- Multiple modifications
bulk:replaceOne({ a = 1 }, { b = 1 })
bulk:updateMany('{}', '{ "$inc" : { "b" : 2 } }')
bulk:removeOne { c = 3 }

-- Execute all the queued operations
assert(bulk:execute())
```


The use of `__tobson` metamethods and BSON handlers gives full control over how Lua values are
represented in BSON documents and vice versa. In particular, this API facilitates support for
classes (tables with metatables) on their way to and/or from MongoDB:

```Lua
local SimpleClass = {} -- Class metatable

local function SimpleObject(id, name) -- Constructor
	return setmetatable({
		id = id,
		name = name,
	}, SimpleClass)
end

function SimpleClass:__tostring() -- Method
	return ('%s -> %s'):format(self.id, self.name)
end

function SimpleClass:__tobson() -- Called when object is packed into BSON
	return {
		_id = self.id,
		binary = mongo.Binary(self.name), -- Store 'name' as BSON Binary for example
	}
end

-- A root '__tobson' metamethod may return a table or a BSON document.
-- A nested '__tobson' metamethod may return a Lua value, a BSON type or a BSON document.

-- BSON handler
local function handler(document)
	return SimpleObject(document._id, (document.binary:unpack()))
end

-- Anything callable can serve as a BSON handler. For instance, it can be a table or a userdata
-- with a '__call' metamethod.

-- Note that the same handler will be called for each nested document. Thus, the handler should
-- be able to differentiate documents based on some internal criteria.

local object = SimpleObject(id, 'John Smith')
print(object)

-- Explicit BSON <-> Lua conversion
local bson = mongo.BSON(object)
local object = bson:value(handler)
print(object)

-- Store object
collection:save(object)

-- Restore object
local object = collection:find(query2):value(handler)
print(object)

-- Restore while iterating in a for-loop
for object in collection:find(query2):iterator(handler) do
	print(object)
end
```


Check out the [API Reference] for more information.

See also the [MongoDB Manual] for detailed information about MongoDB commands and CRUD operations.


[lua-mongo]: https://github.com/neoxic/lua-mongo
[MongoDB C Driver]: http://mongoc.org
[MongoDB Manual]: https://docs.mongodb.com/manual/
[API Reference]: doc/main.md
