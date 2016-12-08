local test = require 'test'

local BSON = mongo.BSON
local hasInt64 = math.maxinteger and math.maxinteger == 9223372036854775807

local function check(v1, v2)
	local b1 = BSON(v1)
	local b2 = BSON(v2)
	-- print(b1, b2)
	assert(b1 == b2) -- Compare as binary
	assert(tostring(b1) == tostring(b2)) -- Compare as strings
	test.equal(b1(), b2()) -- Compare as values
	collectgarbage()
end

local function a(n)
	local t = {}
	for i = 1, n do
		t[#t + 1] = i
	end
	if n < 100 then
		t[#t + 1] = a(n + 1)
	end
	return t
end

local a = { a = a(1) }
assert(BSON(a) == BSON(BSON(a)())) -- Nested arrays

-- Numeric values

check({ a = 2147483647 }, '{ "a" : 2147483647 }') -- Max Int32
check({ a = -2147483648 }, '{ "a" : -2147483648 }') -- Min Int32
check({ a = 1.7976931348623157e+308 }, '{ "a" : 1.7976931348623157e+308 }') -- Max Double
check({ a = -1.7976931348623157e+308 }, '{ "a" : -1.7976931348623157e+308 }') -- Min Double
if hasInt64 then
	check({ a = 9223372036854775807 }, '{ "a" : { "$numberLong" : "9223372036854775807" } }') -- Max Int64
	check({ a = -9223372036854775808 }, '{ "a" : { "$numberLong" : "-9223372036854775808" } }') -- Min Int64
end

-- Types

check({ a = BSON { b = 1 } }, '{ "a" : { "b" : 1 } }') -- Nested document
check({ a = mongo.Int32(1234) }, '{ "a" : 1234 }')
check({ a = mongo.Int64(1234) }, '{ "a" : { "$numberLong" : "1234" } }')
check({ a = mongo.Double(10) }, '{ "a" : 10.0 }')
check({ a = mongo.Binary('abc', 0x81) }, '{ "a" : { "$binary" : "YWJj", "$type" : "81" } }')
if hasInt64 then -- DateTime as Int64
	check({ a = mongo.DateTime(9223372036854775807) }, '{ "a" : { "$date": { "$numberLong" : "9223372036854775807" } } }')
	check({ a = mongo.DateTime(-9223372036854775808) }, '{ "a" : { "$date": { "$numberLong" : "-9223372036854775808" } } }')
else -- DateTime as Double
	check({ a = mongo.DateTime(9007199254740991) }, '{ "a" : { "$date": { "$numberLong" : "9007199254740991" } } }')
	check({ a = mongo.DateTime(-9007199254740992) }, '{ "a" : { "$date": { "$numberLong" : "-9007199254740992" } } }')
end
check({ a = mongo.MaxKey }, '{ "a" : { "$maxKey" : 1 } }')
check({ a = mongo.MinKey }, '{ "a" : { "$minKey" : 1 } }')
check({ a = mongo.Null }, '{ "a" : null }')
check({ a = mongo.Regex('abc') }, '{ "a" : { "$regex" : "abc", "$options" : "" } }')
check({ a = mongo.Regex('abc', 'def') }, '{ "a" : { "$regex" : "abc", "$options" : "def" } }')
check({ a = mongo.Timestamp(4294967295, 4294967295) }, '{ "a" : { "$timestamp" : { "t" : 4294967295, "i" : 4294967295 } } }')

-- Errors

local function failedBSON(v)
	test.failure(BSON, v)
end

local t = {}
t.t = t
local f = function () end
failedBSON { a = t } -- Circular reference
failedBSON { a = f } -- Invalid value type
failedBSON { [f] = 1 } -- Invalid key type
failedBSON '' -- Empty JSON
failedBSON 'abc' -- Invalid JSON

local function newBSONType(n, ...)
	return setmetatable({ ... }, { __bsontype = n })
end

failedBSON { a = newBSONType(0x99) } -- Invalid type code
failedBSON { a = newBSONType(0x05) } -- Invalid binary data

-- ObjectID

local oid1 = mongo.ObjectID('000000000000000000000000')
local oid2 = mongo.ObjectID('000000000000000000000000')
assert(oid1 == oid2) -- Compare as binary
assert(tostring(oid1) == tostring(oid2)) -- Compare as strings
assert(oid1:hash() == oid2:hash()) -- Compare hashes
check({ a = oid1 }, '{ "a" : { "$oid" : "000000000000000000000000" } }')
test.failure(mongo.ObjectID, 'abc') -- Invalid format
assert(oid1 ~= mongo.ObjectID()) -- Random ObjectID
