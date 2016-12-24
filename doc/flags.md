MongoDB command flags
=====================

Flags are tables with flag names as string keys and flag settings as boolean values:

```Lua
local flags = {
	flag1 = true,
	flag2 = false,
}
```

Insert flags
------------

| Name            | Translation                     |
|-----------------|---------------------------------|
| continueOnError | MONGOC_INSERT_CONTINUE_ON_ERROR |
| noValidate      | MONGOC_INSERT_NO_VALIDATE       |

Refer to http://mongoc.org/libmongoc/current/mongoc_insert_flags_t.html for more information.


Remove flags
------------

| Name   | Translation                 |
|--------|-----------------------------|
| single | MONGOC_REMOVE_SINGLE_REMOVE |

Refer to http://mongoc.org/libmongoc/current/mongoc_remove_flags_t.html for more information.


Update flags
------------

| Name       | Translation                |
|------------|----------------------------|
| upsert     | MONGOC_UPDATE_UPSERT       |
| multi      | MONGOC_UPDATE_MULTI_UPDATE |
| noValidate | MONGOC_UPDATE_NO_VALIDATE  |

Refer to http://mongoc.org/libmongoc/current/mongoc_update_flags_t.html for more information.
