GridFS file list
================

Methods
-------

### list:iterator()
Returns an iterator function and `list` itself so that the statement

```Lua
for file in list:iterator() do ... end
```

will iterate over all files in the list.

### list:next()
Iterates `list` and returns the next [GridFS file] from it or `nil` if there are no more files.
On error, returns `nil` and the error message.


[GridFS file]: gridfsfile.md
