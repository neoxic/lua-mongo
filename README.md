MongoDB Driver for Lua
======================

[lua-mongo] is a binding to [MongoDB C Driver] 1.16 or higher for Lua:
- Unified API for MongoDB commands, CRUD operations and GridFS in [MongoDB C Driver].
- Support for data transformation metamethods/handlers when converting to/from BSON documents.
- Transparent conversion from Lua/JSON to BSON for convenience.
- Automatic conversion of Lua numbers to/from BSON Int32, Int64 and Double types depending on their
  capacity without precision loss (when Lua allows it). Manual conversion is also available.


Dependencies
------------

+ lua >= 5.1 (or luajit)
+ mongo-c-driver >= 1.16


Building and installing with LuaRocks
-------------------------------------

To build and install, run:

    luarocks make

To install the latest release using [luarocks.org], run:

    luarocks install lua-mongo


Building and installing with CMake
----------------------------------

To build and install, run:

    cmake .
    make
    make install

To build for a specific Lua version, set `USE_LUA_VERSION`. For example:

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
local client = mongo.Client('mongodb://127.0.0.1')
local collection = client:getCollection('lua-mongo-test', 'test')
collection:drop() -- Clear collection

-- Common variables
local id = mongo.ObjectID()
local query1 = mongo.BSON('{ "age" : { "$gt" : 25 } }')
local query2 = mongo.BSON{_id = id}
```


Basic features and operations:

```Lua
-- Implicit Lua/JSON to BSON conversion where BSON is required
collection:insert{_id = id, name = 'John Smith', age = 50}
collection:insert('{ "name" : "Bobby", "age" : 3 }')

-- Iterate documents in a for-loop
for person in collection:find({}, {sort = {age = -1}}):iterator() do
    print(person.name, person.age)
end

-- Fetch single document
local person = collection:findOne(query1):value()
print(person.name, person.age)

-- Access to BSON where needed
local bson = collection:findOne(query1)
print(bson) -- BSON is converted to JSON using tostring()

-- Explicit BSON to Lua conversion
local person = bson:value()
print(person.name, person.age)

-- Transparently include BSON documents in other documents
collection:update(query2, {age = 60, old = bson}) -- Update document
collection:remove(query2) -- Remove document
```


Bulk write operations can be used to execute multiple insert, update, replace and remove operations
together. Executing write operations in batches reduces the number of network round trips increasing
write throughput.

```Lua
local bulk = collection:createBulkOperation()

-- Multiple insertions
bulk:insert{a = 1}
bulk:insert{b = 2}
bulk:insert{c = 3}

-- Multiple modifications
bulk:replaceOne({a = 1}, {b = 1})
bulk:updateMany('{}', '{ "$inc" : { "b" : 2 } }')
bulk:removeOne{c = 3}

-- Execute queued operations
bulk:execute()
```


The use of `__toBSON` metamethods and BSON handlers gives full control over how Lua values are
represented in BSON documents and vice versa. In particular, this API facilitates support for
Lua classes (tables with metatables) on their way to and/or from MongoDB.

```Lua
local TestClass = {} -- Class metatable

local function TestObject(id, name) -- Constructor
    local object = {
        id = id,
        name = name,
    }
    return setmetatable(object, TestClass)
end

function TestClass:__tostring() -- Method
    return tostring(self.id) .. ' --> ' .. self.name
end

function TestClass:__toBSON() -- Called when object is serialized into BSON
    return {
        _id = self.id,
        binary = mongo.Binary(self.name), -- Store 'name' as BSON Binary for example
    }
end

-- A root '__toBSON' metamethod may return a table or BSON document.
-- A nested '__toBSON' metamethod may return a value, BSON type or BSON document.

-- BSON handler
local function handler(document)
    local id = document._id
    local name = document.binary:unpack() -- Restore 'name' from BSON Binary
    return TestObject(id, name)
end

-- Note that the same handler is called for each document. Thus, the handler should be able
-- to differentiate documents based on some internal criteria.

local object = TestObject(id, 'abc')
print(object)

-- Explicit BSON <-> Lua conversion
local bson = mongo.BSON(object)
local object = bson:value(handler)
print(object)

-- Store object
collection:insert(object)

-- Restore object
local object = collection:findOne(query2):value(handler)
print(object)

-- Iterate objects in a for-loop
for object in collection:find(query2):iterator(handler) do
    print(object)
end
```


Check out the [API Reference] for more information.

See also the [MongoDB Manual] for detailed information about MongoDB commands and CRUD operations.


[lua-mongo]: https://github.com/neoxic/lua-mongo
[luarocks.org]: https://luarocks.org
[MongoDB C Driver]: http://mongoc.org
[MongoDB Manual]: https://docs.mongodb.com/manual/
[API Reference]: doc/main.md
