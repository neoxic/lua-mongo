BSON document
=============

Methods
-------

### bson:data()
Returns binary contents of `bson`.

### bson:value([handler])
Converts `bson` into a Lua value and returns it. If `handler` is provided, it is called for
every nested document. Its returned value is then used to replace the original value.

```Lua
local bson = mongo.BSON { a = { 1, 2 }, b = 3 }
local function sum(t)
	local r = 0
	for k, v in pairs(t) do
		r = r + v
	end
	return r
end
print(bson:value(sum)) -- 6
```


Operators
---------

### tostring(bson)
Returns JSON representation of `bson`.

### #bson
Returns the size of `bson`.

### bson1 == bson2
Compares binary contents of `bson1` and `bson2`.
