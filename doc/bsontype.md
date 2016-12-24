BSON type
=========

Methods
-------

### bsontype:unpack()
Returns `bsontype`'s parameters.

```Lua
local binary = mongo.Binary('abc', 0x80)
local data, subtype = binary:unpack() -- 'abc', 128
```


Operators
---------

### tostring(bsontype)
Returns string representation of `bsontype`.

```Lua
local regex = mongo.Regex('abc', 'i')
print(regex) -- mongo.Regex("abc", "i")
```
