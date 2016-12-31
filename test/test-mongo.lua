local test = require 'test'
local client = mongo.Client(test.uri)


-- Collection

local collection = client:getCollection(test.dbname, test.cname)
assert(collection:getName() == test.cname)
collection:drop()

test.error(collection:insert({ ['$a'] = 123 })) -- Client-side error
test.error(collection:insert({ ['$a'] = 123 }, { noValidate = true })) -- Server-side error

assert(collection:insert { _id = 123 })
test.error(collection:insert { _id = 123 }) -- Duplicate key

assert(collection:save { _id = 456 })
assert(collection:save { _id = 789 })

assert(collection:count() == 3)
assert(collection:count('{ "_id" : 123 }') == 1)
assert(collection:count { _id = { ['$gt'] = 123 } } == 2)
assert(collection:count({}, { skip = 1, limit = 2 }) == 2) -- Options

-- cursor:next()
local cursor = collection:find {} -- Find all
assert(cursor:isAlive())
assert(cursor:next()) -- #1
assert(cursor:next()) -- #2
assert(cursor:next()) -- #3
local b, e = cursor:next()
assert(b == nil and e == nil) -- nil + no error
assert(not cursor:isAlive())
b, e = cursor:next()
assert(b == nil and type(e) == 'string') -- nil + error
collectgarbage()

-- cursor:value()
cursor = collection:find { _id = 123 }
assert(cursor:value()._id == 123)
assert(cursor:value() == nil) -- No more items
test.failure(cursor.value, cursor) -- Cursor exhausted
cursor = collection:find { _id = 123 }
assert(cursor:value(function (t) return { id = t._id } end).id == 123) -- With transformation
collectgarbage()

-- cursor:iterator()
local f, c = collection:find('{ "_id" : { "$gt" : 123 } }', { sort = { _id = -1 } }):iterator() -- _id > 123, desc order
local v1 = assert(f(c))
local v2 = assert(f(c))
assert(v1._id == 789)
assert(v2._id == 456)
assert(f(c) == nil) -- No more items
test.failure(f, c) -- Cursor exhausted
f, c = collection:find({ _id = 123 }):iterator(function (t) return { id = t._id } end) -- With transformation
assert(f(c).id == 123)
collectgarbage()

assert(collection:remove({}, { single = true })) -- Flags
assert(collection:count() == 2)
assert(collection:remove { _id = 123 })
assert(collection:remove { _id = 123 }) -- Remove reports 'true' even if not found
assert(collection:find({ _id = 123 }):value() == nil) -- Not found

assert(collection:update({ _id = 123 }, { a = 'abc' }, { upsert = true })) -- inSERT
assert(collection:update({ _id = 123 }, { a = 'def' }, { upsert = true })) -- UPdate
assert(collection:find({ _id = 123 }):value().a == 'def')

assert(collection:findAndModify({ _id = 123 }, { update = { a = 'abc' } }):find('a') == 'def') -- Old value
assert(collection:findAndModify({ _id = 'abc' }, { remove = true }) == mongo.Null) -- Not found

assert(collection:aggregate('[ { "$group" : { "_id" : "$a", "count" : { "$sum" : 1 } } } ]'):value().count == 1)

assert(collection:validate({ full = true }):find('valid'))

-- Bulk operation
local function bulkInsert(ordered, n)
	collection:drop()
	local bulk = collection:createBulkOperation(ordered)
	for id = 1, n do
		bulk:insert { _id = id }
		bulk:insert { _id = id }
	end
	test.error(bulk:execute()) -- Errors about duplicate keys
	return collection:count()
end
assert(bulkInsert(false, 3) == 3) -- Unordered insert
assert(bulkInsert(true, 3) == 1) -- Ordered insert

collection:drop()
local bulk = collection:createBulkOperation()
for a = 1, 6 do
	bulk:insert { a = a }
end
bulk:removeMany '{ "a" : { "$gt" : 4 } }'
bulk:removeOne {}
bulk:replaceOne({}, { b = 1 })
bulk:updateMany({}, '{ "$inc" : { "b" : 1 } }')
bulk:updateOne({}, '{ "$inc" : { "b" : 1 } }')
assert(bulk:execute())
local cursor = collection:find {}
assert(cursor:value().b == 3)
assert(cursor:value().b == 1)
assert(cursor:value().b == 1)

collection = nil
collectgarbage()


-- Database

local database = client:getDatabase(test.dbname)
assert(database:getName() == test.dbname)

assert(database:removeAllUsers())
assert(database:addUser('test', 'test'))
test.error(database:addUser('test', 'test'))
assert(database:removeUser('test'))
test.error(database:removeUser('test'))

test.value(assert(database:getCollectionNames()), test.cname)
assert(database:hasCollection(test.cname))

database = nil
collectgarbage()


-- Client

test.failure(mongo.Client, 'abc') -- Invalid URI format

-- client:getDefaultDatabase()
local c1 = mongo.Client 'mongodb://aaa'
local c2 = mongo.Client 'mongodb://aaa/bbb'
test.failure(c1.getDefaultDatabase, c1) -- No default database in URI
assert(c2:getDefaultDatabase():getName() == 'bbb')

-- client:getDatabaseNames()
test.value(assert(client:getDatabaseNames()), test.dbname)

-- client:command()
assert(mongo.type(assert(client:command(test.dbname, { find = test.cname }))) == 'mongo.Cursor') -- client:command() returns cursor
assert(mongo.type(assert(client:command(test.dbname, { validate = test.cname }))) == 'mongo.BSON') -- client:command() returns BSON
test.error(client:command('abc', { INVALID_COMMAND = test.cname }))

-- Cleanup
assert(client:getDatabase(test.dbname):drop())
