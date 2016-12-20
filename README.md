MongoDB Driver for Lua
======================

_lua-mongo_ is a binding to the MongoDB C Driver (http://mongoc.org) for Lua.

* Transparent conversion from Lua/JSON to BSON.

* Support for custom data transformation handlers when converting to/from BSON documents.

* Automatic conversion of Lua numbers to/from BSON _Int32_, _Int64_ and _Double_ types depending on
  their capacity without precision loss (when Lua allows it). Explicit casts are also available.

* Unified API for MongoDB CRUD operations, wrappers and types as they are introduced in `mongoc`.

* Support for query/insert/update/remove flags.


Please note that _lua-mongo_ is in development beta which means that its API is likely to change
between versions.


Installing with LuaRocks
------------------------

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

To test the build, run:

	make test

Test settings can be configured in `test/test.lua`.


Getting started
---------------

Preparing the playground:

```Lua
local mongo = require 'mongo'
local client = mongo.Client 'mongodb://127.0.0.1'
local collection = client:getCollection('lua-mongo-test', 'test')

-- Common variables
local id = mongo.ObjectID()
local query = mongo.BSON '{ "age" : { "$gt" : 25 } }'
local filter = mongo.BSON { _id = id }
```


Basic features and MongoDB CRUD operations:

```Lua
-- Save document
collection:save { _id = id, name = 'John Smith', age = 50 }

-- Fetch document
local result = collection:find(query):value()
print(result.name)

-- Iterate in a for-loop
for item in collection:find(query):iterator() do
	print(item.name)
end

-- Implicit Lua/JSON to BSON conversion where BSON is required
collection:insert { value = 123 }
collection:insert '{ "value" : "456" }'

-- Use options in queries
print(collection:count({}, { skip = 1, limit = 2 }))

-- Access to BSON where needed
local cursor = collection:find(query)
local bson = cursor:next() -- Fetch BSON document
print(bson) -- BSON is implicitly converted JSON

-- Explicit BSON to Lua conversion
local value = bson()
print(value.name)

-- Explicit BSON to JSON conversion
local json = tostring(bson)
print(json)

-- Transparently include BSON documents in other documents
collection:update(filter, { age = 60, backup = bson }, { upsert = true }) -- Update document
collection:remove(filter) -- Remove document
```


The use of `__tobson` metamethods and BSON handlers gives full control over how Lua values are
represented in BSON documents and vice versa. In particular, this API facilitates support for
classes (tables with metatables) on their way to and/or from MongoDB. For example:

```Lua
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

-- A root '__tobson' metamethod may return a table or a BSON document.
-- A nested '__tobson' metamethod may return a convertible Lua type, a BSON type or a BSON document.

-- BSON handler
local handler = function (doc)
	return setmetatable({ -- The way 'doc' is restored from BSON
		id = doc._id,
		str = doc.bin(),
	}, class)
end

-- Anything callable can serve as a BSON handler. For instance, it can be a table or a userdata
-- with a '__call' metamethod.

-- Please note that the same handler will be called for each root and nested document in the order
-- of appearance. It is thus the sole responsibility of the handler to differentiate documents by
-- their types.

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
```


Dropping the testing collection:

```Lua
collection:drop()
```


Reference
---------

#### mongo.type(value)

Returns a type name of a _lua-mongo_ `value`.

```Lua
print(mongo.type(mongo.Int32(123))) -- mongo.Int32
print(mongo.type(mongo.Null)) -- mongo.Null
```

#### mongo.Binary(data, [subtype])

Creates an instance of a BSON _Binary_ type.

```Lua
local binary = mongo.Binary('abc', 0x80)
print(binary) -- mongo.Binary("abc", 128)
print(binary()) -- abc 128
```

#### mongo.BSON(value): `bson`

Creates an instance of a BSON document from a `value` convertible to BSON, i.e. a BSON document
(in which case this method is no-op), a JSON string, a table, or a table/userdata with a `__tobson`
metamethod that returns a table or a BSON document. This is the general rule for values where a BSON
document is required.

#### mongo.Client(uri): `client`

Creates an instance of a MongoDB _client_ handle.

#### mongo.DateTime(milliseconds)

Creates an instance of a BSON _DateTime_ type.

#### mongo.Double(number)

Creates an instance of a BSON _Double_ type. Overrides automatic number conversion.

#### mongo.Int32(integer)

Creates an instance of a BSON _Int32_ type. Overrides automatic number conversion.

#### mongo.Int64(integer)

Creates an instance of a BSON _Int64_ type. Overrides automatic number conversion.

#### mongo.Javascript(code, [scope])

Creates an instance of a BSON _Javascript_ type.

#### mongo.ObjectID([value])

Creates an instance of a BSON _ObjectID_ type. If a hexadecimal string `value` is provided, it is
used to initialize the instance. Otherwise, a new random value is generated.

#### mongo.Regex(regex, [options])

Creates an instance of a BSON _Regex_ type.

#### mongo.Timestamp(timestamp, increment)

Creates an instance of a BSON _Timestamp_ type.


### `bson`

#### bson:data()


### `client`

#### client:getCollection(dbname, cname)
