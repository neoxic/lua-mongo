BSON document
=============

Methods
-------

### bson:append(key, value)
Appends a pair of `key` => `value` to `bson`.

```Lua
local bson = mongo.BSON{}
bson:append('a', 1)
print(bson)
bson:append('b', bson)
print(bson)
```
Output:
```
{ "a" : 1 }
{ "a" : 1, "b" : { "a" : 1 } }
```

### bson:concat(value)
Appends the contents of `value` (converted to a BSON document) to `bson`.

```Lua
local bson1 = mongo.BSON{a = 1}
local bson2 = mongo.BSON{b = 2}
bson1:concat(bson2)
print(bson1)
bson1:concat(bson2)
print(bson1)
```
Output:
```
{ "a" : 1, "b" : 2 }
{ "a" : 1, "b" : 2, "b" : 2 }
```

### bson:data()
Returns the contents of `bson`.

### bson:find(key)
Returns the value matching `key` in `bson` or `nil` if nothing was found. A field name may contain
dots to recurse into subdocuments.

```Lua
local bson = mongo.BSON{a = {b = 1}}
print(bson:find('a'))
print(bson:find('a.b'))
print(bson:find('a.b.c'))
```
Output:
```
{ "b" : 1 }
1
nil
```

### bson:value([handler])
Converts `bson` into a table and returns it. Optional `handler` is called for each new table (root
or nested), and its return value is used instead of the original table.

When an _array_ is restored, its length is stored in a field `__array` of the resulting table.

```Lua
local bson = mongo.BSON{a = 1, b = {__array = true, 2, 3}}
local function sum(t)
    local r = 0
    local n = t.__array
    if n then
        for i = 1, n do
            r = r + t[i]
        end
    else
        for k, v in pairs(t) do
            r = r + v
        end
    end
    return r
end
print(type(bson:value()))
print(bson:value(sum))
```
Output:
```
table
6
```


Operators
---------

### tostring(bson)
Returns JSON representation of `bson`.

### #bson
Returns the size of `bson`.

### bson1 == bson2
Compares the contents of `bson1` and `bson2`.
