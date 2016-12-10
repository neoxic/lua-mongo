local test = {
	uri = 'mongodb://127.0.0.1',
	dbname = 'lua-mongo-test',
	cname = 'test',
}

local function equal(v1, v2)
	if type(v1) ~= 'table' or type(v2) ~= 'table' then
		return v1 == v2
	end
	if v1 == v2  then
		return true
	end
	for k, v in pairs(v1) do
		if not equal(v, v2[k]) then
			return false
		end
	end
	for k, v in pairs(v2) do
		if not equal(v, v1[k]) then
			return false
		end
	end
	return true
end

function test.equal(v1, v2)
	assert(equal(v1, v2), 'values differ!')
end

function test.error(e, ...)
	local s, m = ...
	assert(not s, 'call succeeded!')
	assert(e == m, 'error messages differ!')
end

function test.failure(f, ...)
	assert(type(f) == 'function', 'not a function!')
	assert(not pcall(f, ...), 'call succeeded!')
end

return test
