Collection class
================

Methods
-------

### collection:aggregate(pipeline, [options])
Executes an aggregation `pipeline` with `options` on `collection` and returns a [Cursor] object.

### collection:count([query], [options])
Executes a count `query` with `options` on `collection` and returns the result. On error, returns
`nil` and the error message.

### collection:drop([options])
Drops `collection` with `options` and returns `true`. On error, returns `nil` and the error message.

### collection:find(query, [options])
Executes a find `query` with `options` on `collection` and returns a [Cursor] object.

### collection:findAndModify(query, options)
Executes a find-and-modify `query` on `collection` and returns the result as a [BSON document] or
[BSON Null][BSON type] if nothing was found. On error, returns `nil` and the error message.

### collection:getName()
Returns the name of `collection`.

### collection:insert(document, [flags])
Inserts `document` into `collection` and returns `true`. On error, returns `nil` and the error
message. See [Flags for insert] for `flags` description.

### collection:remove([query], [flags])
Removes documents in `collection` that match `query` and returns `true`. On error, returns `nil`
and the error message. See [Flags for remove] for `flags` description.

### collection:save(document)
Saves `document` into `collection` and returns `true`. On error, returns `nil` and the error
message. If the document has an `_id` field it will be updated. Otherwise it will be inserted.

### collection:update(query, update, [flags])
Updates documents in `collection` that match `query` and returns `true`. On error, returns `nil`
and the error message. See [Flags for update] for `flags` description.

### collection:validate([options])
Validates `collection` and returns the result as a [BSON document]. On error, returns `nil` and
the error message.


[BSON document]: bson.md
[BSON type]: bsontype.md
[Cursor]: cursor.md
[Flags for insert]: flags.md#flags-for-insert
[Flags for remove]: flags.md#flags-for-remove
[Flags for update]: flags.md#flags-for-update
