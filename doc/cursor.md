Cursor
======

Methods
-------

### cursor:iterator([handler])
Returns an iterator function and `cursor` itself so that the statement

```Lua
for value in cursor:iterator() do ... end
```

will iterate over all values from the cursor.

This method is semantically equivalent to:

```Lua
function cursor:iterator(handler)
    return function (cursor)
        return cursor:value(handler)
    end,
    cursor
end
```

### cursor:more()
Checks if `cursor` allows for more documents to be acquired. Useful for tailable cursors.

### cursor:next()
Iterates `cursor` and returns the next [BSON document] from it or `nil` if there are no more
documents to read. On error, returns `nil` and the error message.

### cursor:value([handler])
Iterates `cursor` and returns the next value from it or `nil` if there are no more documents to read.
On error, exception is thrown.

This method is semantically equivalent to:

```Lua
function cursor:value(handler)
    local bson, err = cursor:next()
    if bson then
        return bson:value(handler)
    end
    if err then
        error(err)
    end
    return nil
end
```

except that it avoids creating a temporary [BSON document].


[BSON document]: bson.md
