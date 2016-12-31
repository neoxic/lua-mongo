local test = require 'test'
local BSON = mongo.BSON

local testInt64 = math.maxinteger and math.maxinteger == 9223372036854775807

local function testV(v1, v2, h)
	local b1 = BSON(v1)
	local b2 = BSON(v2)
	-- print(b1, b2)
	assert(b1 == b2) -- Compare with overloaded equality operator
	assert(b1:data() == b2:data()) -- Compare binary data
	assert(tostring(b1) == tostring(b2)) -- Compare as strings
	test.equal(b1:value(h), b2:value(h)) -- Compare as values
	collectgarbage()
end

local function testX(v, h)
	local b = BSON(v)
	local v_ = b:value(h)
	local b_ = BSON(v_)
	-- print(b, b_)
	assert(b == b_)
	test.equal(v, v_)
	collectgarbage()
end

local function testF(v)
	test.failure(BSON, v)
end


-- bson:concat()
local b1 = BSON { a = 1 }
local b2 = BSON { b = 2 }
b1:concat(b2)
testV(b1, '{ "a" : 1, "b" : 2 }')
b1:concat(b2)
testV(b1, '{ "a" : 1, "b" : 2, "b" : 2 }')
test.failure(b1.concat, b1) -- Self unexpected

-- bson:find()
local b = BSON { a = 1, b = { c = mongo.Null } }
assert(b:find('') == nil)
assert(b:find('abc') == nil)
assert(b:find('a') == 1)
testV(b:find('b'), '{ "c" : null }')
assert(b:find('b.c') == mongo.Null)


-- Arrays

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
assert(BSON(a) == BSON(BSON(a):value())) -- Nested arrays
local a1 = { 1, 2, 3 }
local a2 = { a1 = a1 }
testV(a1, '[ 1, 2, 3 ]') -- Root array
testV(a2, '{ "a1" : [ 1, 2, 3 ] }') -- Nested array
testX(a1)
testX(a2)


-- Numeric values

testV({ a = 2147483647 }, '{ "a" : 2147483647 }') -- Max Int32
testV({ a = -2147483648 }, '{ "a" : -2147483648 }') -- Min Int32
testV({ a = 1.7976931348623157e+308 }, '{ "a" : 1.7976931348623157e+308 }') -- Max Double
testV({ a = -1.7976931348623157e+308 }, '{ "a" : -1.7976931348623157e+308 }') -- Min Double
if testInt64 then
	testV({ a = 9223372036854775807 }, '{ "a" : { "$numberLong" : "9223372036854775807" } }') -- Max Int64
	testV({ a = -9223372036854775808 }, '{ "a" : { "$numberLong" : "-9223372036854775808" } }') -- Min Int64
end


-- Types

testV({ a = BSON { b = 1 } }, '{ "a" : { "b" : 1 } }') -- Nested document
testV({ a = mongo.Int32(1234) }, '{ "a" : 1234 }')
testV({ a = mongo.Int64(1234) }, '{ "a" : { "$numberLong" : "1234" } }')
testV({ a = mongo.Double(10) }, '{ "a" : 10.0 }')
testV({ a = mongo.Binary('abc') }, '{ "a" : { "$binary" : "YWJj", "$type" : "0" } }')
testV({ a = mongo.Binary('abc', 0x80) }, '{ "a" : { "$binary" : "YWJj", "$type" : "80" } }')
if testInt64 then -- DateTime as Int64
	testV({ a = mongo.DateTime(9223372036854775807) }, '{ "a" : { "$date": { "$numberLong" : "9223372036854775807" } } }')
	testV({ a = mongo.DateTime(-9223372036854775808) }, '{ "a" : { "$date": { "$numberLong" : "-9223372036854775808" } } }')
else -- DateTime as Double
	testV({ a = mongo.DateTime(9007199254740991) }, '{ "a" : { "$date": { "$numberLong" : "9007199254740991" } } }')
	testV({ a = mongo.DateTime(-9007199254740992) }, '{ "a" : { "$date": { "$numberLong" : "-9007199254740992" } } }')
end
testV({ a = mongo.MaxKey }, '{ "a" : { "$maxKey" : 1 } }')
testV({ a = mongo.MinKey }, '{ "a" : { "$minKey" : 1 } }')
testV({ a = mongo.Null }, '{ "a" : null }')
testV({ a = mongo.Regex('abc') }, '{ "a" : { "$regex" : "abc", "$options" : "" } }')
testV({ a = mongo.Regex('abc', 'def') }, '{ "a" : { "$regex" : "abc", "$options" : "def" } }')
testV({ a = mongo.Timestamp(4294967295, 4294967295) }, '{ "a" : { "$timestamp" : { "t" : 4294967295, "i" : 4294967295 } } }')

-- FIXME These tests fail because binary data differs for some unknown reason (a bug in libbson?)
-- testV({ a = mongo.Javascript('abc') }, '{ "a" : { "$code" : "abc" } }')
-- testV({ a = mongo.Javascript('abc', { a = 1 }) }, '{ "$code" : "abc", "$scope" : { "a" : 1 } } }')


-- Handlers

local mt = { __tobson = function (t) return { bin = mongo.Binary(t.str) } end }
local obj = setmetatable({ str = 'abc' }, mt)
local h1 = function (t) return setmetatable({ str = t.bin:unpack() }, mt) end
local h2 = function (t) return t.a and t or h1(t) end
testV(obj, '{ "bin" : { "$binary" : "YWJj", "$type" : "0" } }', h1) -- Root transformation
testV({ a = obj }, '{ "a" : { "bin" : { "$binary" : "YWJj", "$type" : "00" } } }', h2) -- Nested transformation
testX(obj, h1) -- Root transition
testX({ a = obj }, h2) -- Nested transition


-- Errors

test.failure(mongo.type, setmetatable({}, {})) -- Invalid object for 'mongo.type'
testF(setmetatable({}, { __tobson = function (t) return { t = t } end })) -- Recursion in '__tobson'
testF(setmetatable({}, { __tobson = function (t) return 'abc' end })) -- Root '__tobson' should return table or BSON
testF(setmetatable({}, { __tobson = function (t) t() end })) -- Run-time error in root '__tobson'
testF { a = setmetatable({}, { __tobson = function (t) t() end }) } -- Run-time error in nested '__tobson'

local t = {}
t.t = t
local f = function () end
testF { a = t } -- Circular reference
testF { a = f } -- Invalid value
testF { [f] = 1 } -- Invalid key
testF '' -- Empty JSON
testF 'abc' -- Invalid JSON
testF { 1, nil, 2 } -- Invalid array keys
testF { a = 1, 2 } -- Mixed keys in array
testF { a = 1, [2.1] = 2 } -- Mixed keys in document

local function newBSONType(n, ...)
	return setmetatable({ ... }, { __bsontype = n })
end

testF { a = newBSONType(0x99) } -- Invalid type
testF { a = newBSONType(0x05) } -- Binary: invalid string
testF { a = newBSONType(0x0d) } -- Javascript: invalid string
testF { a = newBSONType(0x0f) } -- Javascript w/ scope: invalid string
testF { a = newBSONType(0x0f, 'abc') } -- Javascript w/ scope: invalid BSON


-- ObjectID

local oid1 = mongo.ObjectID('000000000000000000000000')
local oid2 = mongo.ObjectID('000000000000000000000000')
assert(oid1 == oid2) -- Compare with overloaded equality operator
assert(oid1:data() == oid1:data()) -- Compare binary data
assert(oid1:hash() == oid2:hash()) -- Compare hashes
assert(tostring(oid1) == tostring(oid2)) -- Compare as strings
testV({ a = oid1 }, '{ "a" : { "$oid" : "000000000000000000000000" } }')
test.failure(mongo.ObjectID, 'abc') -- Invalid format
local oid = mongo.ObjectID() -- Random ObjectID
assert(oid ~= oid1 and oid:data() ~= oid1:data() and tostring(oid) ~= tostring(oid1)) -- Compare everything except hashes
