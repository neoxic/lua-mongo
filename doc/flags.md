Command flags
=============

Flags are tables with flag names as string keys and flag settings as boolean values:

```Lua
local flags = {
    flag1 = true,
    flag2 = false,
}
```

Flags for _insert_
------------------

| Flag            | Translation                     |
|-----------------|---------------------------------|
| continueOnError | MONGOC_INSERT_CONTINUE_ON_ERROR |
| noValidate      | MONGOC_INSERT_NO_VALIDATE       |

Refer to http://mongoc.org/libmongoc/current/mongoc_insert_flags_t.html for more information.


Flags for _remove_
------------------

| Flag   | Translation                 |
|--------|-----------------------------|
| single | MONGOC_REMOVE_SINGLE_REMOVE |

Refer to http://mongoc.org/libmongoc/current/mongoc_remove_flags_t.html for more information.


Flags for _update_
------------------

| Flag       | Translation                |
|------------|----------------------------|
| upsert     | MONGOC_UPDATE_UPSERT       |
| multi      | MONGOC_UPDATE_MULTI_UPDATE |
| noValidate | MONGOC_UPDATE_NO_VALIDATE  |

Refer to http://mongoc.org/libmongoc/current/mongoc_update_flags_t.html for more information.
