Collection
==========

Methods
-------

### collection:aggregate(pipeline, [options], [prefs])
Executes an aggregation `pipeline` on `collection` and returns a [Cursor] handle.

### collection:count(query, [options], [prefs])
Executes a count `query` on `collection` and returns the result. On error, returns `nil` and the
error message.

### collection:createBulkOperation([options])
Returns a new [Bulk operation]. By default, the operation is _ordered_ (see below). To denote the
type of a new bulk operation, set `ordered` in `options` to either `true` or `false`.

_Ordered_ bulk operations are batched and sent to the server in the order suitable for serial
execution. The processing aborts when the first error is encountered.

_Unordered_ bulk operations are batched and sent to the server in arbitrary order where they may
be executed in parallel with any errors reported after all operations are attempted.

### collection:drop([options])
Drops `collection` and returns `true`. On error, returns `nil` and the error message.

### collection:find(query, [options], [prefs])
Executes a find `query` on `collection` and returns a [Cursor] handle.

### collection:findAndModify(query, options)
Executes a find-and-modify `query` on `collection` and returns a [BSON document] or `nil` if nothing
was found. On error, returns `nil` and the error message.

### collection:findOne(query, [options], [prefs])
Returns the first [BSON document] in `collection` that matches `query` or `nil` if nothing was found.
On error, returns `nil` and the error message.

This method is semantically equivalent to:

```Lua
function collection:findOne(query, options)
    options = options or {}
    options.limit = 1
    options.singleBatch = true
    local cursor = collection:find(query, options)
    return cursor:next()
end
```

except that it avoids creating a temporary [Cursor] handle.

### collection:getName()
Returns the name of `collection`.

### collection:getReadPrefs()
Returns the default read preferences.

### collection:insert(document, [flags])
Inserts `document` into `collection` and returns `true`. On error, returns `nil` and the error
message. See also [Flags for insert] for information on `flags`.

If no `_id` element is found in the document, a new unique [BSON ObjectID][BSON type] will be
generated locally and added to the document. If you must know the inserted document's `_id`,
generate it in your code and add to the document before calling this method.

### collection:insertMany(document1, document2, ...)
Inserts `document1`, `document2`, etc. into `collection` and returns `true`. On error, returns `nil`
and the error message.

### collection:insertOne(document, [options])
Inserts `document` into `collection` and returns `true`. On error, returns `nil` and the error
message.

### collection:remove(query, [flags])
Removes documents in `collection` that match `query` and returns `true`. On error, returns `nil`
and the error message. See also [Flags for remove] for information on `flags`.

### collection:removeMany(query, [options])
Removes all documents in `collection` that match `query` and returns `true`. On error, returns `nil`
and the error message.

### collection:removeOne(query, [options])
Removes at most one document in `collection` that matches `query` and returns `true`. On error,
returns `nil` and the error message.

### collection:rename(dbname, collname, [force], [options])
Renames `collection` on the server using new database name `dbname` and collection name `collname`.
If `force` is `true`, an existing collection with the same name will be dropped first.

### collection:replaceOne(query, document, [options])
Replaces at most one document in `collection` that matches `query` with `document` and returns `true`.
On error, returns `nil` and the error message.

### collection:setReadPrefs(prefs)
Sets the default read preferences.

### collection:update(query, document, [flags])
Updates documents in `collection` that match `query` with `document` and returns `true`. On error,
returns `nil` and the error message. See also [Flags for update] for information on `flags`.

### collection:updateMany(query, document, [options])
Updates all documents in `collection` that match `query` with `document` and returns `true`.
On error, returns `nil` and the error message.

### collection:updateOne(query, document, [options])
Updates at most one document in `collection` that matches `query` with `document` and returns `true`.
On error, returns `nil` and the error message.


[BSON document]: bson.md
[BSON type]: bsontype.md
[Bulk operation]: bulkoperation.md
[Cursor]: cursor.md
[Flags for insert]: flags.md#flags-for-insert
[Flags for remove]: flags.md#flags-for-remove
[Flags for update]: flags.md#flags-for-update
