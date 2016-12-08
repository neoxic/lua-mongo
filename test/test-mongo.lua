local test = require 'test'

local function testCollection(collection)
	assert(collection)
	collection:drop()

	assert(collection:save { _id = 123 })
	assert(collection:save { _id = 456 })
	assert(collection:save { _id = 789 })

	assert(collection:count() == 3)
	assert(collection:count('{ "_id" : 123 }') == 1)
	assert(collection:count { _id = { ['$gt'] = 123 } } == 2)

	-- cursor:next()
	local cursor = collection:find {} -- Find all
	assert(cursor:next()) -- #1
	assert(cursor:next()) -- #2
	assert(cursor:next()) -- #3
	local b, e = cursor:next()
	assert(b == nil and e == nil) -- nil + no error
	b, e = cursor:next()
	assert(b == nil and e ~= nil) -- nil + error is set
	assert(collection:find({ _id = 123 }):next(true)._id == 123) -- Evaluate first value
	collectgarbage()

	-- cursor:iterator()
	local f, c = collection:find('{ "_id" : { "$gt" : 123 } }', { sort = { _id = -1 }}):iterator() -- _id > 123, desc order
	local v1 = assert(f(c))
	local v2 = assert(f(c))
	assert(v1._id == 789)
	assert(v2._id == 456)
	assert(not f(c)) -- No more items
	test.failure(f, c) -- Exception is thrown
	collectgarbage()

	assert(collection:update({ _id = 123 }, { a = 'a' }))
	assert(tostring(collection:find({}):next()) == '{ "_id" : 123, "a" : "a" }')

	assert(collection:remove { _id = 123 }) -- #1
	assert(collection:remove { _id = 123 }) -- #2
	assert(collection:count() == 2)

	assert(collection:drop())

	collection = nil
	collectgarbage()
end

local function testDatabase(database)
	assert(database)

	database = nil
	collectgarbage()
end

local function testClient(client)
	assert(client)

	testDatabase(client:getDatabase(test.dbname))
	testCollection(client:getCollection(test.dbname, test.cname))

	client = nil
	collectgarbage()
end

test.failure(mongo.Client, 'abc') -- Invalid URI format
testClient(mongo.Client(test.uri))
