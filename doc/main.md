Main module table
=================

```Lua
local mongo = require 'mongo'
```


Functions
---------

### mongo.type(value)
Returns the type of `value` as a string.

```Lua
print(mongo.type(mongo.Int32(123)))
print(mongo.type(mongo.Null))
print(mongo.type('abc'))
```
Output:
```
mongo.Int32
mongo.Null
string
```


Constructors
------------

### mongo.Binary(data, [subtype])
Returns an instance of [BSON Binary][BSON type].

### mongo.BSON(value)
Returns an instance of [BSON document] constructed from `value` that can be one of the following:
- a [BSON document] in which case this method does nothing;
- a string in BSON or JSON format;
- a table (without a metatable);
- a table or userdata with a `__toBSON` metamethod that returns a value, [BSON type] or [BSON document];

Note that non-numeric keys are __unordered__ in Lua tables. As a result, it is impossible to ensure
a specific order for fields in a [BSON document] constructed from a table. When strict order is
required, the following workaround can be used:

```Lua
print(mongo.BSON{a = 1, b = 2, c = 3}) -- From table (order is unspecified)
print(mongo.BSON('{ "a" : 1, "b" : 2, "c" : 3 }')) -- From JSON (order is preserved)

-- Add fields incrementally
local bson = mongo.BSON('{}')
bson:append('a', 1)
bson:append('b', 2)
bson:append('c', 3)
print(bson)
```
Output:
```
{ "a" : 1, "c" : 3, "b" : 2 }
{ "a" : 1, "b" : 2, "c" : 3 }
{ "a" : 1, "b" : 2, "c" : 3 }
```

A table (root or nested) is converted to an _array_ if it has a field `__array` whose value is
_true_. The length of the resulting array can be adjusted by storing an integer value in that field.
Otherwise, it is assumed to be equal to the raw length of the table.

```Lua
print(mongo.BSON{a = {__array = true, 1, 2, 3}})
print(mongo.BSON{a = {__array = true, nil, 1, nil, 2, nil}})
print(mongo.BSON{a = {__array = 3,    nil, 1, nil, 2, nil}})
```
Output:
```
{ "a" : [ 1, 2, 3 ] }
{ "a" : [ null, 1 ] }
{ "a" : [ null, 1, null ] }
```

### mongo.Client(uri)
Returns a new [Client] handle. See also [MongoDB Connection String URI Format] for information on `uri`.

### mongo.DateTime(milliseconds)
Returns an instance of [BSON DateTime][BSON type].

### mongo.Decimal128(value)
Returns an instance of [BSON Decimal128][BSON type].

### mongo.Double(number)
Returns an instance of [BSON Double][BSON type]. This type can be used to override automatic number
conversion where needed.

### mongo.Int32(integer)
Returns an instance of [BSON Int32][BSON type]. This type can be used to override automatic number
conversion where needed.

### mongo.Int64(integer)
Returns an instance of [BSON Int64][BSON type]. This type can be used to override automatic number
conversion where needed.

### mongo.Javascript(code, [scope])
Returns an instance of [BSON Javascript][BSON type] with `scope` (converted to a [BSON document]).

### mongo.ObjectID([value])
Returns an instance of [BSON ObjectID]. Optional hexadecimal string `value` is used to initialize
the instance. Otherwise, a new unique value is generated.

### mongo.ReadPrefs(mode, [tags], [maxStalenessSeconds])
Returns an instance of read preferences with `mode` (a string) that can be one of the following:
- `primary`
- `primaryPreferred`
- `secondary`
- `secondaryPreferred`
- `nearest`

Refer to http://mongoc.org/libmongoc/current/mongoc_read_mode_t.html for more information.

### mongo.Regex(regex, [options])
Returns an instance of [BSON Regex][BSON type].

### mongo.Timestamp(timestamp, increment)
Returns an instance of [BSON Timestamp][BSON type].


Singletons
----------

### mongo.MaxKey
The [BSON MaxKey][BSON type] singleton object.

### mongo.MinKey
The [BSON MinKey][BSON type] singleton object.

### mongo.Null
The [BSON Null][BSON type] singleton object.


[BSON document]: bson.md
[BSON ObjectID]: objectid.md
[BSON type]: bsontype.md
[Client]: client.md
[MongoDB Connection String URI Format]: https://docs.mongodb.com/manual/reference/connection-string/
