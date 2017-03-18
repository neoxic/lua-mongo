BSON type
=========

Methods
-------

### bsontype:unpack()
Returns `bsontype`'s parameters.

```Lua
local binary = mongo.Binary('abc', 0x80)
print(binary:unpack())
```
Output:
```
abc 128
```


Operators
---------

### tostring(bsontype)
Returns string representation of `bsontype`.

```Lua
print(mongo.Regex('abc', 'i'))
```
Output:
```
mongo.Regex("abc", "i")
```
