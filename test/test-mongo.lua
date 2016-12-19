local test = require 'test'

local function testCollection(collection)
	assert(mongo.type(collection) == 'mongo.Collection')
	collection:drop()

	test.error("A document was corrupt or contained invalid characters . or $", collection:insert({ ['$a'] = 123 })) -- Client-side error
	test.error("Document can't have $ prefixed field names: $a", collection:insert({ ['$a'] = 123 }, { noValidate = true })) -- Server-side error

	assert(collection:insert { _id = 123 })
	assert(not collection:insert { _id = 123 }) -- Duplicate key

	assert(collection:save { _id = 456 })
	assert(collection:save { _id = 789 })

	assert(collection:count() == 3)
	assert(collection:count('{ "_id" : 123 }') == 1)
	assert(collection:count { _id = { ['$gt'] = 123 } } == 2)
	assert(collection:count({}, { skip = 1, limit = 2 }) == 2) -- Options

	-- cursor:next()
	local cursor = collection:find {} -- Find all
	assert(mongo.type(cursor) == 'mongo.Cursor')
	assert(cursor:next()) -- #1
	assert(cursor:next()) -- #2
	assert(cursor:next()) -- #3
	local b, e = cursor:next()
	assert(b == nil and e == nil) -- nil + no error
	b, e = cursor:next()
	assert(b == nil and type(e) == 'string') -- nil + error
	-- cursor:value()
	cursor = collection:find { _id = 123 }
	assert(cursor:value()._id == 123)
	assert(cursor:value() == nil) -- No more items
	test.failure(cursor.value, cursor) -- Cursor exhausted
	cursor = collection:find { _id = 123 }
	assert(cursor:value(function (t) return { id = t._id } end).id == 123) -- With transformation
	assert(cursor:value() == nil) -- No more items
	test.failure(cursor.value, cursor) -- Cursor exhausted
	collectgarbage()

	-- cursor:iterator()
	local f, c = collection:find('{ "_id" : { "$gt" : 123 } }', { sort = { _id = -1 }}):iterator() -- _id > 123, desc order
	local v1 = assert(f(c))
	local v2 = assert(f(c))
	assert(v1._id == 789)
	assert(v2._id == 456)
	assert(f(c) == nil) -- No more items
	test.failure(f, c) -- Cursor exhausted
	f, c = collection:find({ _id = 123 }):iterator(function (t) return { id = t._id } end) -- With transformation
	assert(f(c).id == 123)
	assert(f(c) == nil) -- No more items
	test.failure(f, c) -- Cursor exhausted
	collectgarbage()

	assert(collection:remove({}, { singleRemove = true })) -- Flags
	assert(collection:count() == 2)
	assert(collection:remove { _id = 123 })
	assert(collection:remove { _id = 123 }) -- Remove reports 'true' even if not found
	assert(collection:find({ _id = 123 }):value() == nil) -- Not found

	assert(collection:update({ _id = 123 }, { a = 'abc' }, { noValidate = false, upsert = true })) -- inSERT
	assert(collection:update({ _id = 123 }, { a = 'def' }, { noValidate = false, upsert = true })) -- UPdate
	assert(collection:find({ _id = 123 }):value().a == 'def')

	assert(collection:drop())

	collection = nil
	collectgarbage()
end

local function testDatabase(database)
	assert(mongo.type(database) == 'mongo.Database')

	testCollection(database:getCollection(test.cname))

	database = nil
	collectgarbage()
end

local function testClient(client)
	assert(mongo.type(client) == 'mongo.Client')

	testDatabase(client:getDatabase(test.dbname))
	testCollection(client:getCollection(test.dbname, test.cname))

	client = nil
	collectgarbage()
end

test.failure(mongo.Client, 'abc') -- Invalid URI format
testClient(mongo.Client(test.uri))
